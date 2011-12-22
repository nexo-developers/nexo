"""Embeds the Canvas widget into a Sugar-specific Activity environment

The olpcgames.activity module encapsulates creation of a Pygame activity.
Your Activity should inherit from this class. Simply setting some class 
attributes is all you need to do in a class inheriting from 
olpcgames.activity.PygameActivity in order to get Pygame to work.  

(The skeleton builder script creates this file automatically for you).

Note: 
    You should not import pygame into your activity file, as the olpcgames 
    wrapper needs to be initialized before pygame is imported the first time.

Example usage:

    class PygameActivity(activity.Activity):
        game_name = None
        game_title = 'Pygame Game'
        game_size = (units.grid_to_pixels(16),
                     units.grid_to_pixels(11))
        pygame_mode = 'SDL'
"""
import logging
logging.root.setLevel( logging.WARN )
log = logging.getLogger( 'olpcgames.activity' )
##log.setLevel( logging.DEBUG )

import pygtk
pygtk.require('2.0')
import gtk
import gtk.gdk
import os

from sugar.activity import activity
from sugar.graphics import style
from olpcgames.canvas import PygameCanvas
from olpcgames import mesh, util

__all__ = ['PygameActivity']

class PygameActivity(activity.Activity):
    """Pygame-specific activity type, provides boilerplate toolbar, creates canvas

    Subclass Overrides:

        game_name -- specifies a fully-qualified name for the game's main-loop
            format like so:
                'package.module:main'
            if no function name is provided, "main" is assumed.
            
        game_handler -- DEPRECATED. alternate specification via direct 
            reference to a main-loop function. 

        game_size -- two-value tuple specifying the size of the display in pixels,
            this is currently static, so once the window is created it cannot be
            changed.

            If None, use the bulk of the screen for the Pygame surface based on
            the values reported by the gtk.gdk functions.  Note that None is
            *not* the default value.

        game_title -- title to be displayed in the Sugar Shell UI

        pygame_mode -- chooses the rendering engine used for handling the
            Pygame drawing mode, 'SDL' chooses the standard Pygame renderer,
            'Cairo' chooses the experimental pygamecairo renderer.
            
            Note: You likely do *not* want to use Cairo, it is no longer maintained.

        PYGAME_CANVAS_CLASS -- normally PygameCanvas, but can be overridden
            if you want to provide a different canvas class, e.g. to provide a different
            internal layout.  Note: only used where pygame_mode == 'SDL'

    The Activity, once created, will be made available as olpcgames.ACTIVITY,
    and that access mechanism should allow code to test for the presence of the
    activity before accessing Sugar-specific functionality.

    XXX Note that currently the toolbar and window layout are hard-coded into
    this super-class, with no easy way of overriding without completely rewriting
    the __init__ method.  We should allow for customising both the UI layout and
    the toolbar contents/layout/connection.
    
    XXX Note that if you change the title of your activity in the toolbar you may 
    see the same focus issues as we have patched around in the build_toolbar 
    method.  If so, please report them to Mike Fletcher.
    """
    game_name = None
    game_title = 'Pygame Game'
    game_handler = None
    game_size = (16 * style.GRID_CELL_SIZE,
                 11 * style.GRID_CELL_SIZE)
    pygame_mode = 'SDL'

    def __init__(self, handle):
        """Initialise the Activity with the activity-description handle"""
        super(PygameActivity, self).__init__(handle)
        self.make_global()
        if self.game_size is None:
            width,height = gtk.gdk.screen_width(), gtk.gdk.screen_height()
            log.info( 'Total screen size: %s %s', width,height)
            # for now just fudge the toolbar size...
            self.game_size = width, height - (1*style.GRID_CELL_SIZE)
        self.set_title(self.game_title)
        toolbar = self.build_toolbar()
        log.debug( 'Toolbar size: %s', toolbar.get_size_request())
        canvas = self.build_canvas()
        self.connect( 'configure-event', canvas._translator.do_resize_event )

    def make_global( self ):
        """Hack to make olpcgames.ACTIVITY point to us
        """
        import weakref, olpcgames
        assert not olpcgames.ACTIVITY, """Activity.make_global called twice, have you created two Activity instances in a single process?"""
        olpcgames.ACTIVITY = weakref.proxy( self )

    def build_toolbar( self ):
        """Build our Activity toolbar for the Sugar system

        This is a customisation point for those games which want to
        provide custom toolbars when running under Sugar.
        """
        toolbar = activity.ActivityToolbar(self)
        toolbar.show()
        self.set_toolbox(toolbar)
        def shared_cb(*args, **kwargs):
            log.info( 'shared: %s, %s', args, kwargs )
            try:
                mesh.activity_shared(self)
            except Exception, err:
                log.error( """Failure signaling activity sharing to mesh module: %s""", util.get_traceback(err) )
            else:
                log.info( 'mesh activity shared message sent, trying to grab focus' )
            try:
                self._pgc.grab_focus()
            except Exception, err:
                log.warn( 'Focus failed: %s', err )
            else:
                log.info( 'asserting focus' )
                assert self._pgc.is_focus(), """Did not successfully set pygame canvas focus"""
            log.info( 'callback finished' )
            
        def joined_cb(*args, **kwargs):
            log.info( 'joined: %s, %s', args, kwargs )
            mesh.activity_joined(self)
            self._pgc.grab_focus()
        self.connect("shared", shared_cb)
        self.connect("joined", joined_cb)

        if self.get_shared():
            # if set at this point, it means we've already joined (i.e.,
            # launched from Neighborhood)
            joined_cb()

        toolbar.title.unset_flags(gtk.CAN_FOCUS)
        return toolbar

    PYGAME_CANVAS_CLASS = PygameCanvas
    def build_canvas( self ):
        """Construct the Pygame or PygameCairo canvas for drawing"""
        assert self.game_handler or self.game_name, 'You must specify a game_handler or game_name on your Activity (%r)'%(
            self.game_handler or self.game_name
        )
        if self.pygame_mode != 'Cairo':
            self._pgc = self.PYGAME_CANVAS_CLASS(*self.game_size)
            self.set_canvas(self._pgc)
            self._pgc.grab_focus()
            self._pgc.connect_game(self.game_handler or self.game_name)
            # XXX Bad coder, do not hide in a widely subclassed operation
            # map signal does not appear to show up on socket instances
            gtk.gdk.threads_init()
            return self._pgc
        else:
            import hippo
            self._drawarea = gtk.DrawingArea()
            canvas = hippo.Canvas()
            canvas.grab_focus()
            self.set_canvas(canvas)
            self.show_all()

            import pygamecairo
            pygamecairo.install()

            pygamecairo.display.init(canvas)
            app = self.game_handler or self.game_name
            if ':' not in app:
                app += ':main'
            mod_name, fn_name = app.split(':')
            mod = __import__(mod_name, globals(), locals(), [])
            fn = getattr(mod, fn_name)
            fn()
    def read_file(self, file_path):
        """Handle request to read the given file on the Pygame side
        
        This is complicated rather noticeably by the silly semantics of the Journal
        where it unlinks the file as soon as this method returns.  We either have to 
        handle the file-opening in PyGTK (not acceptable), block this thread until 
        the Pygame thread handles the event (which it may never do) or we have 
        to make the silly thing use a non-standard file-opening interface.
        """
        log.info( 'read_file: %s %s', file_path, self.metadata )
        import olpcgames, pygame
        from olpcgames import eventwrap
        event = eventwrap.Event(
            type = pygame.USEREVENT,
            code = olpcgames.FILE_READ_REQUEST,
            filename = file_path,
            metadata = self.metadata,
        )
        eventwrap.post( event )
        event.block()
    def write_file( self, file_path ):
        """Handle request to write to the given file on the Pygame side 
        
        This is rather complicated by the need to have the file complete by the 
        time the function returns.  Very poor API, after all, if I have to write a 
        multi-hundred-megabyte file it might take many minutes to complete 
        writing.
        """
        log.info( 'write_file: %s %s', file_path, self.metadata )
        if os.path.exists( file_path ):
            self.read_file( file_path )
        import olpcgames, pygame
        from olpcgames import eventwrap
        event = eventwrap.Event(
            type = pygame.USEREVENT,
            code = olpcgames.FILE_WRITE_REQUEST,
            filename = file_path,
            metadata = self.metadata,
        )
        eventwrap.post( event )
        event.block()
        if not os.path.exists( file_path ):
            log.warn( '''No file created in %r''', file_path )
            raise NotImplementedError( """Pygame Activity code did not produce a file for %s"""%( file_path, ))
        else:
            log.info( '''Stored file in %r''', file_path )
        

import olpcgames
olpcgames.PyGameActivity = PygameActivity
PyGameActivity = PygameActivity
