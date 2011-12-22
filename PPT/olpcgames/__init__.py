"""Wrapper/adaptation system for writing/porting Pygame games to OLPC/Sugar

The wrapper system attempts to substitute various pieces of the Pygame 
implementation in order to make code written without knowledge of the
OLPC/Sugar environment run "naturally" under the GTK environment of 
Sugar.  It also provides some convenience mechanisms for dealing with 
e.g. the Camera and Mesh Network system.

Considerations for Developers:

Pygame programs running under OLPCGames will generally not have
"hardware" surfaces, and will not be able to have a reduced-resolution 
full-screen view to optimise rendering.  The Pygame code will run in 
a secondary thread, with the main GTK UI running in the primary thread.
A third "mainloop" thread will occasionally be created to handle the 
GStreamer interface to the camera.

Attributes of Note:

    ACTIVITY -- if not None, then the activity instance which represents
        this activity at the Sugar shell level.
    WIDGET -- PygameCanvas instance, a GTK widget with an embedded
        socket object which is a proxy for the SDL window Pygame to which
        pygame renders.
    
    Constants: All event constants used by the package are defined at this 
        level.  Note that eventually we will need to switch to using UserEvent
        and making these values sub-types rather than top-level types.


Pygame events at the Activity Level:

    pygame.USEREVENT
        code == olpcgames.FILE_READ_REQUEST
            filename (unicode/string) -- filename from which to read
            metadata (dictionary-like) -- mapping from key to string values
            
            Note: due to a limitation in the Sugar API, the GTK event loop
            will be *frozen* during this operation, as a result you cannot 
            make any DBUS or GTK calls, nor can you use GUI during the 
            call to provide input.  That is, you have to process this event 
            synchronously.
            
        code == olpcgames.FILE_WRITE_REQUEST
            filename (unicode/string) -- file name to which to write
            metadata (dictionary-like) -- mapping from key: value where all 
                values must (currently) be strings

            Note: due to a limitation in the Sugar API, the GTK event loop
            will be *frozen* during this operation, as a result you cannot 
            make any DBUS or GTK calls, nor can you use GUI during the 
            call to provide input.  That is, you have to process this event 
            synchronously.

see also the mesh and camera modules for more events.

Deprecated:

	This module includes the activity.PyGameActivity class currently,
	this is a deprecated mechanism for accessing the activity class,
	and uses the deprecated spelling (case) of the name.  Use:

		from olpcgames import activity

		class MyActivity( activity.PygameActivity ):
			...
	
	to define your PygameActivity subclass (note the case of the 
	spelling, which now matches Pygame's own spelling).
"""
from olpcgames._version import __version__
ACTIVITY = None
widget = WIDGET = None

# XXX problem here, we're filling up the entirety of the Pygame 
# event-set with just this small bit of functionality, obviously 
# Pygame is not intending for this kind of usage!
(
    CAMERA_LOAD, CAMERA_LOAD_FAIL,
    
    CONNECT,PARTICIPANT_ADD,
    PARTICIPANT_REMOVE,
    MESSAGE_UNI,MESSAGE_MULTI,
) = range( 25, 32 )

# These events use UserEvent.code, eventually *all* events should be 
# delivered as UserEvent with code set to the values defined here...

(
	#NET_CONNECT, NET_PARTICIPANT_ADD,NET_PARTICIPANT_REMOVE,
	#NET_MESSAGE_UNICAST, NET_MESSAGE_MULTICAST,
	#CAMERA_LOAD, CAMERA_LOAD_FAIL,
	FILE_READ_REQUEST, FILE_WRITE_REQUEST,
) = range(
	2**16, 2**16+2,
)

try:
	from olpcgames.activity import PygameActivity as PyGameActivity
except ImportError, err:
	PyGameActivity = None

