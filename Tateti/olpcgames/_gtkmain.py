"""Support for GObject mainloop-requiring libraries when not inside GTK

INITIALIZED -- whether we have a running gobject loop yet...
LOOP_TRACKER -- if present, the manual gtk event loop used to 
    support gobject-based code running in a non-Gobject event loop

Holder -- objects which can be held as attributes to keep the mainloop running
"""
import threading, logging
log = logging.getLogger( 'olpcgames._gtkmain' )
##log.setLevel( logging.DEBUG )

INITIALIZED = False
LOOP_TRACKER = None

class _TrackLoop( object ):
    """Tracks the number of open loops and stops when finished"""
    count = 0
    _mainloop = None
    def increment( self ):
        log.info( 'Increment from %s', self.count )
        self.count += 1 # XXX race condition here?
        if self.count == 1:
            log.info( 'Creating GObject mainloop')
            self.t_loop = threading.Thread(target=self.loop)
            self.t_loop.setDaemon( True )
            self.t_loop.start()
    def decrement( self ):
        log.info( 'Decrement from %s', self.count )
        self.count -= 1
    def loop( self ):
        """Little thread loop that replicates the gtk mainloop"""
        import gtk
        while self.count >= 1:
            log.debug( 'GTK loop restarting' )
            while gtk.events_pending():
                gtk.main_iteration()
        log.debug( 'GTK loop exiting' )
        try:
            del self.t_loop
        except AttributeError, err:
            pass

class Holder():
    """Object which, while held, keeps the gtk mainloop running"""
    def __init__( self ):
        log.info( 'Beginning hold on GTK mainloop with Holder object' )
        startGTK()
    def __del__( self ):
        log.info( 'Releasing hold on GTK mainloop with Holder object' )
        stopGTK()

def startGTK( ):
    """GTK support is required here, process..."""
    if not INITIALIZED:
        init()
    if LOOP_TRACKER:
        LOOP_TRACKER.increment()
def stopGTK( ):
    """GTK support is no longer required, release"""
    if LOOP_TRACKER:
        LOOP_TRACKER.decrement()
def init( ):
    """Create a gobject mainloop in a sub-thread (you don't need to call this normally)"""
    global INITIALIZED, LOOP_TRACKER
    if not INITIALIZED:
        if not LOOP_TRACKER:
            LOOP_TRACKER = _TrackLoop()
        INITIALIZED = True
    return LOOP_TRACKER
