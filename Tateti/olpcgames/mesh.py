'''Utilities for wrapping the telepathy network for Pygame

The 'mesh' module allows your Pygame game to be Shared 
across the OLPC networking infrastructure (D-bus and Tubes).
It offers a simplified view of the Telepathy system.

All Sugar activities have a 'Share' menu (toolbar) which is 
intended to allow other people to join the activity instance 
and collaborate with you. When you select Share, the activity's 
icon appears on the Neighborhood view of other laptops.

If you do nothing else with networking, this is all that will 
happen: if anyone selects your shared activity icon, they will 
just spawn a new instance of the activity, and they will get to 
play your game alone.

The mesh module automatically sets up a connection from each 
participant to every other participant.  It provides (string based)
communications channels that let you either broadcast messages 
to other users or communicate point-to-point to one other user.

You can use the "handles" which uniquely idenify users to send 
messages to an individual user (send_to( handle, message )) or 
broadcast( message ) to send a message to all participants.

More advanced (structured) networking can be handled by using 
the get_object( handle, path ) function, which looks up an object 
(by DBUS path) shared by the user "handle" and returns a 
DBUS/Telepathy proxy for that object.  The object you get back is 
actually an olpcgames.dbusproxy.DBUSProxy instance, which 
enforces asynchronous operations and runs your 
reply_handler/error_handler in the Pygame event loop.

NOTE:
    You *cannot* make synchronous calls on these objects!  
    You must use the named arguments:

        reply_handler, error_handler

    for every call which you perform on a shared object (normally 
    these are ExportedGObject instances).

If you want to run your callbacks in the GTK event loop (for instance 
because they need to handle GTK-side objects), you can use the 
dbus_get_object function.  This is *not* recommended for normal 
usage, as any call to Pygame operations within the GTK event loop 
can cause a segfault/core of your entire Activity.

Note:

    mesh sets up N**2 connections for each shared activity, obviously 
    that will not scale to very large shared activities. 

Note: 

    The intention is that mesh will be refactored, possibly as a 
    new module called "olpcgames.network", which would break out 
    the various components so that there is no longer an assumed 
    networking layout.  We will attempt to retain the mesh module's 
    API as we do so.

Events produced:

    olpcgames.CONNECT -- The tube connection was started. (i.e., the 
        user clicked Share or started the activity from the Neighborhood 
        screen).
        
        Event properties:
        
            id -- a unique identifier for this connection. (shouldn't be needed 
                for anything)

    olpcgames.PARTICIPANT_ADD -- A participant joined the activity. 
        This will trigger for the local user as well as any arriving remote 
        users.  Note that this *only* occurs after the activity is shared, 
        that is, the local user does not appear until after they have 
        shared a locally-started activity.

        Event properties:

            handle --  the arriving user's handle (a uniquely identifying string 
                assigned to the user by the Telepathy system, not human 
                readable), see lookup_buddy to retrieve human-readable 
                descriptions of the user.

    olpcgames.PARTICIPANT_REMOVE --  A participant quit the activity.
    
        Event properties:

            handle -- the departing user's handle.

    olpcgames.MESSAGE_UNI -- A message was sent to you.
    
        Event properties:
        
           content --  the content of the message (a string)
           handle -- the handle of the sending user.

    olpcgames.MESSAGE_MULTI -- A message was sent to everyone.

        Event properties:
        
           content -- the content of the message (a string)
           handle -- the handle of the sending user.

Note:

    Eventually we will stop using top-level Pygame event types for the 
    various networking message types (currently four of them).  We will 
    likely use UserEvent with a sub-type specifier for the various events 
    that OLPCGames produces.

See Also:

    http://blog.vrplumber.com/2016 -- Discussion of how Productive uses 
        the mesh module and raw Telepathy (ExportedGObject instances)
'''
import logging
log = logging.getLogger( 'olpcgames.mesh' )
##log.setLevel( logging.DEBUG )
import olpcgames
from olpcgames.util import get_traceback
try:
    from sugar.presence.tubeconn import TubeConnection
except ImportError, err:
    TubeConnection = object
try:
    from dbus.gobject_service import ExportedGObject
except ImportError, err:
    ExportedGObject = object
from dbus.service import method, signal

try:
    import telepathy
except ImportError, err:
    telepathy = None

try:
    import sugar.presence.presenceservice
except Exception, err:
    pass
import pygame.event as PEvent

class OfflineError( Exception ):
    """Raised when we cannot complete an operation due to being offline"""

DBUS_IFACE="org.laptop.games.pygame"
DBUS_PATH="/org/laptop/games/pygame"
DBUS_SERVICE = None


### NEW PYGAME EVENTS ###

CONNECT            = olpcgames.CONNECT
PARTICIPANT_ADD    = olpcgames.PARTICIPANT_ADD
PARTICIPANT_REMOVE = olpcgames.PARTICIPANT_REMOVE
MESSAGE_UNI        = olpcgames.MESSAGE_UNI
MESSAGE_MULTI      = olpcgames.MESSAGE_MULTI


# Private objects for useful purposes!
pygametubes = []
text_chan, tubes_chan = (None, None)
conn = None
initiating = False
joining = False

connect_callback = None

def is_initiating():
    '''A version of is_initiator that's a bit less goofy, and can be used
    before the Tube comes up.'''
    global initiating
    return initiating

def is_joining():
    '''Returns True if the activity was started up by means of the
    Neighbourhood mesh view.'''
    global joining
    return joining

def set_connect_callback(cb):
    '''Just the same as the Pygame event loop can listen for CONNECT,
    this is just an ugly callback that the glib side can use to be aware
    of when the Tube is ready.'''
    global connect_callback
    connect_callback = cb

def activity_shared(activity):
    '''Called when the user clicks Share.'''

    global initiating
    initiating = True

    _setup(activity)


    log.debug('This is my activity: making a tube...')
    channel = tubes_chan[telepathy.CHANNEL_TYPE_TUBES]
    if hasattr( channel, 'OfferDBusTube' ):
        id = channel.OfferDBusTube(
            DBUS_SERVICE, {})
    else:
        id = channel.OfferTube(
            telepathy.TUBE_TYPE_DBUS, DBUS_SERVICE, {})

    global connect_callback
    if connect_callback is not None:
        connect_callback()

def activity_joined(activity):
    '''Called at the startup of our Activity, when the user started it via Neighborhood intending to join an existing activity.'''

    # Find out who's already in the shared activity:
    log.debug('Joined an existing shared activity')

    for buddy in activity._shared_activity.get_joined_buddies():
        log.debug('Buddy %s is already in the activity' % buddy.props.nick)


    global initiating
    global joining
    initiating = False
    joining = True


    _setup(activity)

    tubes_chan[telepathy.CHANNEL_TYPE_TUBES].ListTubes(
        reply_handler=_list_tubes_reply_cb,
        error_handler=_list_tubes_error_cb)

    global connect_callback
    if connect_callback is not None:
        connect_callback()

def _getConn( activity ):
    log.debug( '_getConn' )
    global conn
    if conn:
        return conn
    else:
        if hasattr( activity._shared_activity, 'telepathy_conn' ):
            log.debug( '''new-style api for retrieving telepathy connection present''' )
            conn = activity._shared_activity.telepathy_conn
        else:
            pservice = _get_presence_service()
            log.debug( '_get_presence_service -> %s', pservice )
            name, path = pservice.get_preferred_connection()
            log.debug( '_get_presence_service -> %s, %s', name, path)
            conn = telepathy.client.Connection(name, path)
        log.debug( 'Telepathy Client Connection: %s', conn )
        return conn



def _setup(activity):
    '''Determines text and tube channels for the current Activity. If no tube
channel present, creates one. Updates text_chan and tubes_chan.

setup(sugar.activity.Activity, telepathy.client.Connection)'''
    global text_chan, tubes_chan, DBUS_SERVICE
    log.info( 'Setup for %s', activity )
    if not DBUS_SERVICE:
        DBUS_SERVICE = activity.get_bundle_id()
    if not activity.get_shared():
        log.error('Failed to share or join activity')
        raise "Failure"

    if hasattr( activity._shared_activity, 'telepathy_tubes_chan' ):
        log.debug( '''Improved channel setup API available''' )
        _getConn( activity )
        conn = activity._shared_activity.telepathy_conn
        tubes_chan = activity._shared_activity.telepathy_tubes_chan
        text_chan = activity._shared_activity.telepathy_text_chan
    else:
        log.debug( '''Old-style setup API''' )
        bus_name, conn_path, channel_paths = activity._shared_activity.get_channels()
        _getConn( activity )

        # Work out what our room is called and whether we have Tubes already
        room = None
        tubes_chan = None
        text_chan = None
        for channel_path in channel_paths:
            log.debug( 'Testing channel path: %s', channel_path)
            channel = telepathy.client.Channel(bus_name, channel_path)
            htype, handle = channel.GetHandle()
            log.debug( '  Handle Type: %s  Handle: %s', htype, handle)
            if htype == telepathy.HANDLE_TYPE_ROOM:
                log.debug('Found our room: it has handle#%d "%s"',
                    handle, conn.InspectHandles(htype, [handle])[0])
                room = handle
                ctype = channel.GetChannelType()
                if ctype == telepathy.CHANNEL_TYPE_TUBES:
                    log.debug('Found our Tubes channel at %s', channel_path)
                    tubes_chan = channel
                elif ctype == telepathy.CHANNEL_TYPE_TEXT:
                    log.debug('Found our Text channel at %s', channel_path)
                    text_chan = channel

        if room is None:
            log.error("Presence service didn't create a room")
            raise "Failure"
    if text_chan is None:
        log.error("Presence service didn't create a text channel")
        raise "Failure"

    # Make sure we have a Tubes channel - PS doesn't yet provide one
    if tubes_chan is None:
        log.debug("Didn't find our Tubes channel, requesting one...")
        tubes_chan = conn.request_channel(telepathy.CHANNEL_TYPE_TUBES,
            telepathy.HANDLE_TYPE_ROOM, room, True)

    tubes_chan[telepathy.CHANNEL_TYPE_TUBES].connect_to_signal('NewTube',
        new_tube_cb)

    log.info( 'Setup for %s complete', activity )
    return (text_chan, tubes_chan)

def new_tube_cb(id, initiator, type, service, params, state):
    log.debug("New_tube_cb called: %s %s %s" % (id, initiator, type))
    if (type == telepathy.TUBE_TYPE_DBUS and service == DBUS_SERVICE):
        if state == telepathy.TUBE_STATE_LOCAL_PENDING:
            channel = tubes_chan[telepathy.CHANNEL_TYPE_TUBES]
            if hasattr( channel, 'AcceptDBusTube' ):
                channel.AcceptDBusTube( id )
            else:
                channel.AcceptTube(id)

        tube_conn = TubeConnection(conn,
            tubes_chan[telepathy.CHANNEL_TYPE_TUBES],
            id, group_iface=text_chan[telepathy.CHANNEL_INTERFACE_GROUP])

        global pygametubes, initiating
        pygametubes.append(PygameTube(tube_conn, initiating, len(pygametubes)))


def _list_tubes_reply_cb(tubes):
    for tube_info in tubes:
        new_tube_cb(*tube_info)

def _list_tubes_error_cb(e):
    log.error('ListTubes() failed: %s', e)

def lookup_buddy( dbus_handle, callback, errback=None ):
    """Do a lookup on the buddy information, callback with the information
    
    Calls callback( buddy ) with the result of the lookup, or errback( error ) with 
    a dbus description of the error in the lookup process.
    
    returns None
    """
    log.debug('Trying to find owner of handle %s...', dbus_handle)
    cs_handle = instance().tube.bus_name_to_handle[dbus_handle]
    log.debug('Trying to find my handle in %s...', cs_handle)
    group = text_chan[telepathy.CHANNEL_INTERFACE_GROUP]
    log.debug( 'Calling GetSelfHandle' )
    if not errback:
        def errback( error ):
            log.error( """Failure retrieving handle for buddy lookup: %s""", error )
    def with_my_csh( my_csh ):
        log.debug('My handle in that group is %s', my_csh)
        def _withHandle( handle ):
            """process the results of the handle values"""
            # XXX: we're assuming that we have Buddy objects for all contacts -
            # this might break when the server becomes scalable.
            pservice = _get_presence_service()
            name, path = pservice.get_preferred_connection()
            callback( pservice.get_buddy_by_telepathy_handle(name, path, handle) )
        if my_csh == cs_handle:
            conn.GetSelfHandle(reply_handler = _withHandle, error_handler=errback)
            log.debug('CS handle %s belongs to me, looking up with GetSelfHandle', cs_handle)
        elif group.GetGroupFlags() & telepathy.CHANNEL_GROUP_FLAG_CHANNEL_SPECIFIC_HANDLES:
            handle = group.GetHandleOwners([cs_handle])[0]
            log.debug('CS handle %s belongs to %s', cs_handle, handle)
            _withHandle( handle )
        else:
            handle = cs_handle
            log.debug('non-CS handle %s belongs to itself', handle)
            _withHandle( handle )
    group.GetSelfHandle( reply_handler = with_my_csh, error_handler = errback)

    

def get_buddy(dbus_handle):
    """DEPRECATED: Get a Buddy from a handle
    
    THIS API WAS NOT THREAD SAFE!  It has been removed to avoid
    extremely hard-to-debug failures in activities.  Use lookup_buddy 
    instead!
    
    Code that read:
    
        get_buddy( handle )
        doSomething( handle, buddy )
        doSomethingElse( buddy )
        
    Translates to:
        
        def withBuddy( buddy ):
            doSomething( handle, buddy )
            doSomethingElse( buddy )
        lookup_buddy( handle, callback=withBuddy )
    """
    raise RuntimeError(
        """get_buddy is not thread safe and will crash your activity (hard).  Use lookup_buddy."""
    )

def _get_presence_service( ):
    """Attempt to retrieve the presence service (check for offline condition)

    The presence service, when offline, has no preferred connection type,
    so we check that before returning the object...
    """
    log.debug( """About to import sugar.presence.presenceservice""" )
    try:
        log.debug( 'About to retrieve presence service instance' )
        pservice = sugar.presence.presenceservice.get_instance()
        try:
            log.debug( '  Retrieved presence service instance: %s', pservice )
            name, path = pservice.get_preferred_connection()
            log.debug( '  Name = %s  Path = %s', name, path )
        except (TypeError,ValueError), err:
            log.warn('Working in offline mode, cannot retrieve buddy information for %s: %s', handle, err )
            raise OfflineError( """Unable to retrieve buddy information, currently offline""" )
        else:
            return pservice
    except Exception, err:
        log.error( """Failure in _get_presence_service: %s""", get_traceback( err ))

def instance(idx=0):
    return pygametubes[idx]


class PygameTube(ExportedGObject):
    '''The object whose instance is shared across D-bus

    Call instance() to get the instance of this object for your activity service.
    Its 'tube' property contains the underlying D-bus Connection.
    '''
    def __init__(self, tube, is_initiator, tube_id):
        super(PygameTube, self).__init__(tube, DBUS_PATH)
        log.info( 'PygameTube init' )
        self.tube = tube
        self.is_initiator = is_initiator
        self.entered = False
        self.ordered_bus_names = []
        PEvent.post(PEvent.Event(CONNECT, id=tube_id))

        if not self.is_initiator:
            self.tube.add_signal_receiver(self.new_participant_cb, 'NewParticipants', DBUS_IFACE, path=DBUS_PATH)
        self.tube.watch_participants(self.participant_change_cb)
        self.tube.add_signal_receiver(self.broadcast_cb, 'Broadcast', DBUS_IFACE, path=DBUS_PATH, sender_keyword='sender')


    def participant_change_cb(self, added, removed):
        log.debug( 'participant_change_cb: %s %s', added, removed )
        for handle, bus_name in added:
            dbus_handle = self.tube.participants[handle]
            self.ordered_bus_names.append(dbus_handle)
            PEvent.post(PEvent.Event(PARTICIPANT_ADD, handle=dbus_handle))

        for handle in removed:
            dbus_handle = self.tube.participants[handle]
            self.ordered_bus_names.remove(dbus_handle)
            PEvent.post(PEvent.Event(PARTICIPANT_REMOVE, handle=dbus_handle))

        if self.is_initiator:
            if not self.entered:
                # Initiator will broadcast a new ordered_bus_names each time
                # a participant joins.
                self.ordered_bus_names = [self.tube.get_unique_name()]
            self.NewParticipants(self.ordered_bus_names)

        self.entered = True

    @signal(dbus_interface=DBUS_IFACE, signature='as')
    def NewParticipants(self, ordered_bus_names):
        '''This is the NewParticipants signal, sent when the authoritative list of ordered_bus_names changes.'''
        log.debug("sending NewParticipants: %s" % ordered_bus_names)
        pass

    @signal(dbus_interface=DBUS_IFACE, signature='s')
    def Broadcast(self, content):
        '''This is the Broadcast signal; it sends a message to all other activity participants.'''
        pass

    @method(dbus_interface=DBUS_IFACE, in_signature='s', out_signature='', sender_keyword='sender')
    def Tell(self, content, sender=None):
        '''This is the targeted-message interface; called when a message is received that was sent directly to me.'''
        PEvent.post(PEvent.Event(MESSAGE_UNI, handle=sender, content=content))

    def broadcast_cb(self, content, sender=None):
        '''This is the Broadcast callback, fired when someone sends a Broadcast signal along the bus.'''
        PEvent.post(PEvent.Event(MESSAGE_MULTI, handle=sender, content=content))

    def new_participant_cb(self, new_bus_names):
        '''This is the NewParticipants callback, fired when someone joins or leaves.'''
        log.debug("new participant. new bus names %s, old %s" % (new_bus_names, self.ordered_bus_names))
        if self.ordered_bus_names != new_bus_names:
            log.warn("ordered bus names out of sync with server, resyncing")
            self.ordered_bus_names = new_bus_names

def send_to(handle, content=""):
    '''Sends the given message to the given buddy identified by handle.'''
    log.debug( 'send_to: %s %s', handle, content )
    remote_proxy = dbus_get_object(handle, DBUS_PATH)
    remote_proxy.Tell(content, reply_handler=dbus_msg, error_handler=dbus_err)

def dbus_msg():
    log.debug("async reply to send_to")
def dbus_err(e):
    log.error("async error: %s" % e)

def broadcast(content=""):
    '''Sends the given message to all participants.'''
    log.debug( 'Broadcast: %s', content )
    instance().Broadcast(content)

def my_handle():
    '''Returns the handle of this user
    
    Note, you can get a DBusException from this if you have 
    not yet got a unique ID assigned by the bus.  You may need 
    to delay calling until you are sure you are connected.
    '''
    log.debug( 'my handle' )
    return instance().tube.get_unique_name()

def is_initiator():
    '''Returns the handle of this user.'''
    log.debug( 'is initiator' )
    return instance().is_initiator

def get_participants():
    '''Returns the list of active participants, in order of arrival.
    List is maintained by the activity creator; if that person leaves it may not stay in sync.'''
    log.debug( 'get_participants' )
    try:
        return instance().ordered_bus_names[:]
    except IndexError, err:
        return [] # no participants yet, as we don't yet have a connection

def dbus_get_object(handle, path, warning=True):
    '''Get a D-bus object from another participant

    Note: this *must* be called *only* from the GTK mainloop, calling 
    it from Pygame will cause crashes!  If you are *sure* you only ever 
    want to call methods on this proxy from GTK, you can use 
    warning=False to silence the warning log message.
    '''
    if warning:
        log.warn( 'Use of dbus_get_object is only safe from the GTK mainloop, use dbus_get_object_proxy instead: %s %s', handle, path )
    return instance().tube.get_object(handle, path)

def get_object(handle, path):
    '''Get a D-BUS proxy object from another participant for use in Pygame

    This is how you can communicate with other participants using
    arbitrary D-bus objects without having to manage the participants
    yourself.  You can use the returned proxy's methods from Pygame,
    with your callbacks occuring in the Pygame thread, rather than 
    in the DBUS/GTK event loop.

    Simply define a D-bus class with an interface and path that you
    choose; when you want a reference to the corresponding remote
    object on a participant, call this method.
    
    returns an olpcgames.dbusproxy.DBUSProxy( ) object wrapping 
    the DBUSProxy object.
    
    The dbus_get_object_proxy name is deprecated
    '''
    log.debug( 'DBUS get_object( %r %r )', handle, path )
    from olpcgames import dbusproxy
    return dbusproxy.DBUSProxy( 
        instance().tube.get_object( handle, path), 
        tube=instance().tube, 
        path=path 
    )

dbus_get_object_proxy = get_object
