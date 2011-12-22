"""Video widget for displaying a gstreamer pipe

Note: currently this module is not all that elegant or useful,
we need a better recipe for using and working with Video 
under OLPCGames.
"""
import logging
log = logging.getLogger( 'olpcgames.video' )
#log.setLevel( logging.INFO )
import os
import signal
import pygame
import weakref
import olpcgames
from olpcgames import _gtkmain

import pygtk
pygtk.require('2.0')
import gtk
import gst

class VideoWidget(gtk.DrawingArea):
    """Widget to render GStreamer video over our Pygame Canvas
    
    The VideoWidget is a simple GTK window which is 
    held by the PygameCanvas, just as is the Pygame 
    window we normally use.  As such this approach 
    *cannot* work without the GTK wrapper.
    
    It *should* be possible to use raw X11 operations 
    to create a child window of the Pygame/SDL window 
    and use that for the same purpose, but that would 
    require some pretty low-level ctypes hacking.
    
    Attributes of Note:
    
        rect -- Pygame rectangle which tells us where to 
            display ourselves, setting the rect changes the 
            position and size of the window.
    """
    _imagesink = None
    _renderedRect = None
    def __init__(self, rect=None, force_aspect_ratio=True):
        super(VideoWidget, self).__init__()
        self.unset_flags(gtk.DOUBLE_BUFFERED)
        if rect is None:
            rect = pygame.Rect( (0,0), (160,120))
        self.rect = rect
        self.force_aspect_ratio = force_aspect_ratio
        self.set_size_request(rect.width,rect.height)
        olpcgames.WIDGET.put( self, rect.left,rect.top)
        self._renderedRect = rect
        self.show()
    
    def set_rect( self, rect ):
        """Set our rectangle (area of the screen)"""
        log.debug( 'Set rectangle: %s', rect )
        self.set_size_request(rect.width,rect.height)
        olpcgames.WIDGET.move( self, rect.left,rect.top)
        self.rect = rect

    def do_expose_event(self, event):
        """Handle exposure event (trigger redraw by gst)"""
        if self._imagesink:
            self._imagesink.expose()
            return False
        else:
            return True

    def set_sink(self, sink):
        """Set our window-sink for output"""
        assert self.window.xid
        self._imagesink = sink
        self._imagesink.set_xwindow_id(self.window.xid)
        self._imagesink.set_property('force-aspect-ratio', self.force_aspect_ratio)

class PygameWidget( object ):
    """Render "full-screen" video to the entire Pygame screen 
    
    Not particularly useful unless this happens to be exactly what you need.
    """
    def __init__( self ):
        try:
            window_id = pygame.display.get_wm_info()['window']
        except KeyError, err: # pygame-ctypes...
            window_id = int(os.environ['SDL_WINDOWID'])
        self.window_id = window_id
        self._imagesink = None 
        #self._holder = _gtkmain.Holder()
    def set_sink( self, sink ):
        """Set up our gst sink"""
        log.info( 'Setting sink: %s', sink )
        self._imagesink = sink 
        sink.set_xwindow_id( self.window_id )
        
#pipe_desc = 'v4l2src ! video/x-raw-yuv,width=160,height=120 ! ffmpegcolorspace ! xvimagesink'
class Player(object):
    pipe_desc = 'v4l2src ! ffmpegcolorspace ! video/x-raw-yuv ! xvimagesink'
    test_pipe_desc = 'videotestsrc ! ffmpegcolorspace ! video/x-raw-yuv ! xvimagesink'
    _synchronized = False
    def __init__(self, videowidget, pipe_desc=pipe_desc):
        self._playing = False
        self._videowidget = videowidget

        self._pipeline = gst.parse_launch(pipe_desc)

        bus = self._pipeline.get_bus()
        bus.enable_sync_message_emission()
        bus.add_signal_watch()
        bus.connect('sync-message::element', self.on_sync_message)
        bus.connect('message', self.on_message)

    def play(self):
        log.info( 'Play' )
        if self._playing == False:
            self._pipeline.set_state(gst.STATE_PLAYING)
            self._playing = True

    def pause(self):
        log.info( 'Pause' )
        if self._playing == True:
            if self._synchronized:
                log.debug( '  pause already sync\'d' )
                self._pipeline.set_state(gst.STATE_PAUSED)
            self._playing = False
    def stop( self ):
        """Stop all playback"""
        self._pipeline.set_state( gst.STATE_NULL )

    def on_sync_message(self, bus, message):
        log.info( 'Sync: %s', message )
        if message.structure is None:
            return
        if message.structure.get_name() == 'prepare-xwindow-id':
            self._synchronized = True
            self._videowidget.set_sink(message.src)

    def on_message(self, bus, message):
        log.info( 'Message: %s', message )
        t = message.type
        if t == gst.MESSAGE_ERROR:
            err, debug = message.parse_error()
            log.warn("Video error: (%s) %s" ,err, debug)
            self._playing = False
    
if __name__ == "__main__":
    # Simple testing code...
    logging.basicConfig()
    log.setLevel( logging.DEBUG )
    from pygame import image,display, event
    import pygame
    def main():
        display.init()
        maxX,maxY = display.list_modes()[0] 
        screen = display.set_mode( (maxX/3, maxY/3 ) )
        
        display.flip()
        
        pgw = PygameWidget( )
        p = Player( pgw, pipe_desc=Player.test_pipe_desc )
        p.play()
        
        clock = pygame.time.Clock()
        
        running = True
        while running:
            clock.tick( 60 )
            for evt in [pygame.event.wait()] + pygame.event.get():
                if evt.type == pygame.KEYDOWN:
                    if p._playing:
                        p.pause()
                    else:
                        p.play()
                elif evt.type == pygame.QUIT:
                    p.stop()
                    running = False
            #display.flip()
    main()
