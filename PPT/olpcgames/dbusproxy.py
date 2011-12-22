"""Spike test for a safer networking system for DBUS-based objects"""
from olpcgames import eventwrap, util
from dbus import proxies
import logging 
log = logging.getLogger( 'dbus' )
log.setLevel( logging.DEBUG )

def wrap( value, tube=None,path=None ):
    """Wrap object with any required pygame-side proxies"""
    if isinstance( value,proxies._ProxyMethod ):
        return DBUSMethod( value, tube=tube, path=path )
    elif isinstance( value, proxies._DeferredMethod ):
        value._proxy_method = DBUSMethod( value._proxy_method, tube=tube, path=path )
        return value 
    elif isinstance( value, proxies.ProxyObject ):
        return DBUSProxy( value, tube=tube, path=path )
    else:
        return value 

class DBUSProxy( object ):
    """Proxy for the DBUS Proxy object"""
    def __init__( self, proxy, tube=None, path=None ):
        log.info( 'Creating Pygame-side proxy for %s (%s)', proxy,path )
        self.__proxy = proxy
        self.__tube = tube
        self.__path = path
    def __getattr__( self, key ):
        """Retrieve attribute of given key"""
        from dbus import proxies
        return wrap( getattr( self.__proxy, key ) )
    def add_signal_receiver( self, callback, eventName, interface, path=None, sender_keyword='sender'):
        """Add a new signal handler (which will be called many times) for given signal 
        """
        log.info( """Setting signal receiver %s for event %s on interface %s (object path %s) with sender_keyword = %r""",
            callback, eventName, interface, path, sender_keyword,
        )
        log.debug( """proxy: %s proxy.tube: %s""", self.__proxy, self.__proxy.tube )
        self.__tube.add_signal_receiver(
            Callback( callback ),
            eventName,
            interface,
            path = path or self.__path, 
            sender_keyword = sender_keyword,
        )

class DBUSMethod( object ):
    """DBUS method which does callbacks in the Pygame (eventwrapper) thread"""
    def __init__( self, proxy, tube,path ):
        log.info( 'Creating Pygame-side method proxy for %s', proxy )
        self.__proxy = proxy
        self.__tube = tube 
        self.__path = path
    def __call__( self, *args, **named ):
        """Perform the asynchronous call"""
        log.info( 'Calling proxy for %s with *%s, **%s', self.__proxy, args, named )
        callback, errback = named.get( 'reply_handler'), named.get( 'error_handler' )
        if not callback:
            raise TypeError( """Require a reply_handler named argument to do any asynchronous call""" )
        else:
            callback = Callback( callback )
        if not errback:
            errback = defaultErrback
        else:
            errback = Callback( errback )
        named['reply_handler'] = callback
        named['error_handler'] = errback
        return self.__proxy( *args, **named )

def defaultErrback( error ):
    """Log the error to stderr/log"""
    log.error( """Failure in DBUS call: %s""", error )

class Callback( object ):
    """PyGTK-side callback which generates a CallbackResult to process on the Pygame side"""
    def __init__( self, callable, callContext = None):
        """Initialize the callback to process results"""
        self.callable = callable
        if callContext is None:
            callContext = util.get_traceback( None )
        self.callContext = callContext
    def __call__( self, *args, **named ):
        """PyGTK-side callback operation"""
        log.info( 'Callback %s return value *%s, **%s', self.callable, args, named )
        from olpcgames import eventwrap
        args = [wrap(a) for a in args]
        named = dict([
            (k,wrap(v)) for k,v in named.items()
        ])
        eventwrap.post( 
            eventwrap.CallbackResult( 
                self.callable, args, named, callContext = self.callContext 
            )
        )
