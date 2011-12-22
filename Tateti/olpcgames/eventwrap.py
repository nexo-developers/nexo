"""Provides substitute for Pygame's "event" module using gtkEvent

Provides methods which will be substituted into Pygame in order to 
provide the synthetic events that we will feed into the Pygame queue.
These methods are registered by the "install" method.

This event queue does not support getting events only of a certain type. 
You need to get all pending events at a time, or filter them yourself. You 
can, however, block and unblock events of certain types, so that may be 
useful to you. 

Set_grab doesn't do anything (you are not allowed to grab events). Sorry.

Extensions:

    wait( timeout=None ) -- allows you to wait for only a specified period 
        before you return to the application.  Can be used to e.g. wait for a 
        short period, then release some resources, then wait a bit more, then
        release a few more resources, then a bit more...
"""
import pygame
import gtk
import Queue
import thread, threading
import logging
from olpcgames import util

log = logging.getLogger( 'olpcgames.eventwrap' )

from pygame.event import Event, event_name, pump as pygame_pump, get as pygame_get

class Event(object):
    """Mock pygame events"""
    def __init__(self, type, dict=None,**named):
        """Initialise the new event variables from dictionary and named become attributes"""
        self.type = type
        if dict:
            self.__dict__.update( dict )
        self.__dict__.update( named )
    def _get_dict( self ):
        return self.__dict__
    dict = property( _get_dict )
    def __repr__( self ):
        result = []
        for key,value in self.__dict__.items():
            if not key.startswith( '_' ):
                result.append( '%s = %r'%( key, value ))
        return '%s( %s, %s )'%(
            self.__class__.__name__,
            self.type,
            ",".join( result ),
        )
    def block( self ):
        """Block until this event is finished processing
        
        Event process is only finalized on the *next* call to retrieve an event
        after the processing operation in which the event is processed.  In some 
        extremely rare cases we might actually see that happen, were the 
        file-saving event (for example) causes the Pygame event loop to exit.
        In that case, the GTK event loop *could* hang.
        """
        log.info( '''Blocking GTK thread on event: %s''', self )
        self.__lock = threading.Event()
        self.__lock.wait()
    def retire( self ):
        """Block the GTK event loop until this event is processed"""
        try:
            self.__lock.set()
            log.info( '''Released GTK thread on event: %s''', self )
        except AttributeError, err:
            pass 

class CallbackResult( object ):
    def __init__( self, callable, args, named, callContext=None ):
        """Perform callback in Pygame loop with args and named
        
        callContext is used to provide more information when there is 
        a failure in the callback (for debugging purposes)
        """
        self.callable = callable
        self.args = args 
        self.named = named 
        if callContext is None:
            callContext = util.get_traceback( None )
        self.callContext = callContext
    def __call__( self ):
        """Perform the actual callback in the Pygame event loop"""
        try:
            self.callable( *self.args, **self.named )
        except Exception, err:
            log.error(
                """Failure in callback %s( *%s, **%s ): %s\n%s""",
                getattr(self.callable, '__name__',self.callable), 
                self.args, self.named,
                util.get_traceback( err ),
                self.callContext
            )


_EVENTS_TO_RETIRE = []

def _releaseEvents( ):
    """Release/retire previously-processed events"""
    if _EVENTS_TO_RETIRE:
        for event in _EVENTS_TO_RETIRE:
            try:
                event.retire()
            except AttributeError, err:
                pass

def _processCallbacks( events ):
    """Process any callbacks in events and remove from the stream"""
    result = []
    for event in events:
        if isinstance( event, CallbackResult ):
            event()
        else:
            result.append( event )
    if events and not result:
        result.append( 
            Event( type=pygame.NOEVENT )
        )
    return result

def _recordEvents( events ):
    """Record the set of events to retire on the next iteration"""
    global _EVENTS_TO_RETIRE
    events = _processCallbacks( events )
    _EVENTS_TO_RETIRE = events 
    return events

def install():
    """Installs this module (eventwrap) as an in-place replacement for the pygame.event module.
   
    Use install() when you need to interact with Pygame code written
    without reference to the olpcgames wrapper mechanisms to have the 
    code use this module's event queue.
    
    XXX Really, use it everywhere you want to use olpcgames, as olpcgames
    registers the handler itself, so you will always wind up with it registered when 
    you use olpcgames (the gtkEvent.Translator.hook_pygame method calls it).
    """
    log.info( 'Installing OLPCGames event wrapper' )
    from olpcgames import eventwrap
    import pygame
    pygame.event = eventwrap
    import sys
    sys.modules["pygame.event"] = eventwrap

# Event queue:
class _FilterQueue( Queue.Queue ):
    """Simple Queue sub-class with a put_left method"""
    def get_type( self, filterFunction, block=True, timeout=None ):
        """Get events of a given type
        
        Note: can raise Empty *even* when blocking if someone else 
        pops the event off the queue before we get around to it.
        """
        self.not_empty.acquire()
        try:
            if not block:
                if self._empty_type( filterFunction ):
                    raise Queue.Empty
            elif timeout is None:
                while self._empty_type( filterFunction ):
                    self.not_empty.wait()
            else:
                if timeout < 0:
                    raise ValueError("'timeout' must be a positive number")
                endtime = _time() + timeout
                while self._empty_type( filterFunction ):
                    remaining = endtime - _time()
                    if remaining <= 0.0:
                        raise Queue.Empty
                    self.not_empty.wait(remaining)
            item = self._get_type( filterFunction )
            self.not_full.notify()
            return item
        finally:
            self.not_empty.release()
    def _empty_type( self, filterFunction ):
        """Are we empty with respect to filterFunction?"""
        for element in self.queue:
            if filterFunction( element ):
                return False 
        return True
    def _get_type( self, filterFunction ):
        """Get the first instance which matches filterFunction"""
        for element in self.queue:
            if filterFunction( element ):
                self.queue.remove( element )
                return element 
        # someone popped the event off the queue before we got to it!
        raise Queue.Empty
    def peek_type( self, filterFunction= lambda x: True ):
        """Peek to see if we have filterFunction-matching element
        
        Note: obviously this is *not* thread safe, it's just informative...
        """
        try:
            for element in self.queue:
                if filterFunction( element ):
                    return element
            return None
        except RuntimeError, err:
            return None # none yet, at least
    
g_events = _FilterQueue()

# Set of blocked events as set by set
g_blocked = set()
g_blockedlock = thread.allocate_lock() # should use threading instead
g_blockAll = False

def _typeChecker( types ):
    """Create check whether an event is in types"""
    try:
        if 1 in types:
            pass
        def check( element ):
            return element.type in types
        return check
    except TypeError, err:
        def check( element ):
            return element.type == types
        return check

def pump():
    """Handle any window manager and other external events that aren't passed to the user
    
    Call this periodically (once a frame) if you don't call get(), poll() or wait()
    """
    pygame_pump()
    _releaseEvents()

def get( types=None):
    """Get a list of all pending events
    
    types -- either an integer event-type or a sequence of integer event types 
        which restrict the set of event-types returned from the queue.  Keep in mind 
        that if you do not remove events you may wind up with an eternally growing
        queue or a full queue.  Normally you will want to remove all events in your 
        top-level event-loop and propagate them yourself.
    
        Note: if you use types you lose all event ordering guarantees, events
        may show up after events which were originally produced before them due to 
        the re-ordering of the queue on filtering!
    """
    pump()
    eventlist = []
    try:
        if types:
            check = _typeChecker( types )
            while True:
                eventlist.append(g_events.get_type( check, block=False))
        else:
            while True:
                eventlist.append(g_events.get(block=False))
    except Queue.Empty:
        pass
    
    pygameEvents = pygame_get()
    if pygameEvents:
        log.info( 'Raw Pygame events: %s', pygameEvents)
        eventlist.extend( pygameEvents )
    return _recordEvents( eventlist )

def poll():
    """Get the next pending event if exists. Otherwise, return pygame.NOEVENT."""
    pump()
    try:
        result = g_events.get(block=False)
        return _recordEvents( [result] )[0]
    except Queue.Empty:
        return Event(pygame.NOEVENT)


def wait( timeout = None):
    """Get the next pending event, wait up to timeout if none
    
    timeout -- if present, only wait up to timeout seconds, if we 
        do not find an event before then, return None.  timeout 
        is an OLPCGames-specific extension.
    """
    pump()
    try:
        result = None 
        result = g_events.get(block=True, timeout=timeout)
        try:
            return _recordEvents( [result] )[0]
        except IndexError, err:
            return Event( type=pygame.NOEVENT )
    except Queue.Empty, err:
        return None

def peek(types=None):
    """True if there is any pending event
    
    types -- optional set of event-types used to check whether 
        an event is of interest.  If specified must be either a sequence 
        of integers/longs or an integer/long.
    """
    if types:
        check = _typeChecker( types )
        return g_events.peek_type( check ) is not None
    return not g_events.empty()
    
def clear():
    """Clears the entire pending queue of events 
    
    Rarely used
    """
    try:
        discarded = []
        while True:
            discarded.append( g_events.get(block=False) )
        discarded = _recordEvents( discarded )
        _releaseEvents()
        return discarded
    except Queue.Empty:
        pass

def set_blocked(item):
    """Block item/items from being added to the event queue"""
    g_blockedlock.acquire()
    try:
        # FIXME: we do not currently know how to block all event types when
        # you set_blocked(none).
        [g_blocked.add(x) for x in makeseq(item)]
    finally:
        g_blockedlock.release()
    
def set_allowed(item):
    """Allow item/items to be added to the event queue"""
    g_blockedlock.acquire()
    try:
        if item is None:
            # Allow all events when you set_allowed(none). Strange, eh?
            # Pygame is a wonderful API.
            g_blocked.clear()
        else:
            [g_blocked.remove(x) for x in makeseq(item)]
    finally:
        g_blockedlock.release()

def get_blocked(*args, **kwargs):
    g_blockedlock.acquire()
    try:
        blocked = frozenset(g_blocked)
        return blocked
    finally:
        g_blockedlock.release()

def set_grab(grabbing):
    """This method will not be implemented"""

def get_grab():
    """This method will not be implemented"""

def post(event):
    """Post a new event to the Queue of events"""
    g_blockedlock.acquire()
    try:
        if getattr(event,'type',None) not in g_blocked:
            g_events.put(event, block=False)
    finally:
        g_blockedlock.release()

def makeseq(obj):
    """Accept either a scalar object or a sequence, and return a sequence
    over which we can iterate. If we were passed a sequence, return it
    unchanged. If we were passed a scalar, return a tuple containing only
    that scalar. This allows the caller to easily support one-or-many.
    """
    # Strings are the exception because you can iterate over their chars
    # -- yet, for all the purposes I've ever cared about, I want to treat
    # a string as a scalar.
    if isinstance(obj, basestring):
        return (obj,)
    try:
        # Except as noted above, if you can get an iter() from an object,
        # it's a collection.
        iter(obj)
        return obj
    except TypeError:
        # obj is a scalar. Wrap it in a tuple so we can iterate over the
        # one item.
        return (obj,)
