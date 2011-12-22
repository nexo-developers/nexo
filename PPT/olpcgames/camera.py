"""Accesses OLPC Camera functionality via gstreamer

Depends upon:
    pygame 
    gstreamer (particularly gst-launch)
    
Activity demonstrating usage:

    http://dev.laptop.org/git?p=projects/games-misc;a=tree;f=cameratest.activity;hb=HEAD


"""
import threading, subprocess
import logging
import olpcgames
import time
import os
import pygame
from olpcgames.util import get_activity_root

log = logging.getLogger( 'olpcgames.camera' )
#log.setLevel( logging.DEBUG )

CAMERA_LOAD, CAMERA_LOAD_FAIL = olpcgames.CAMERA_LOAD, olpcgames.CAMERA_LOAD

class Camera(object):
    """A class representing a still-picture camera
    
    Produces a simple gstreamer bus that terminates in a filesink, that is, 
    it stores the results in a file.  When a picture is "snapped" the gstreamer
    stream is iterated until it finishes processing and then the file can be 
    read.
    
    There are two APIs available, a synchronous API which can potentially 
    stall your activity's GUI (and is NOT recommended) and an 
    asynchronous API which returns immediately and delivers the captured 
    camera image via a Pygame event.  To be clear, it is recommended 
    that you use the snap_async method, *not* the snap method.
    
    Note:
    
        The Camera class is simply a convenience wrapper around a fairly 
        straightforward gst-launch bus.  If you have more involved 
        requirements for your camera manipulations you will probably 
        find it easier to write your own camera implementation than to 
        use this one.  Basically we provide here the "normal" use case of 
        snapping a picture into a pygame image.
    
    Note:
    
        With the current camera implementation taking a single photograph 
        requires about 6 seconds!  Obviously we'll need to figure out what's 
        taking gstreamer so long to process the pipe and fix that.

    """
    _aliases = {
        'camera': 'v4l2src',
        'test': 'videotestsrc',
        'testing': 'videotestsrc',
        'png': 'pngenc',
        'jpeg': 'jpegenc',
        'jpg': 'jpegenc',
    }
    def __init__(self, source='camera', format='png', filename=None, directory = None):
        """Initialises the Camera's internal description
        
        source -- the gstreamer source for the video to capture, useful values:
            'v4l2src','camera' -- the camera
            'videotestsrc','test' -- test pattern generator source
        format -- the gstreamer encoder to use for the capture, useful values:
            'pngenc','png' -- PNG format graphic
            'jpegenc','jpg','jpeg' -- JPEG format graphic
        filename -- the filename to use for the capture, if not specified defaults 
            to a random UUID + '.' + format
        directory -- the directory in which to create the temporary file, defaults 
            to get_activity_root() + 'tmp'
        """
        log.info( 'Creating camera' )
        if not filename:
            import uuid
            filename = '%s.%s'%( uuid.uuid4(), format )
        self.source = self._aliases.get( source, source )
        self.format = self._aliases.get( format, format )
        self.filename = filename 
        self.directory = directory
    SNAP_PIPELINE = 'gst-launch','%(source)s','!','ffmpegcolorspace','!','%(format)s','!','filesink','location="%(filename)s"'
    def _create_subprocess( self ):
        """Method to create the gstreamer subprocess from our settings"""
        if not self.directory:
            path = os.path.join( get_activity_root(), 'tmp' )
            try:
                os.makedirs( path )
                log.info( 'Created temporary directory: %s', path )
            except (OSError,IOError), err:
                pass
        else:
            path = self.directory
        filename = os.path.join( path, self.filename )
        format = self.format 
        source = self.source 
        pipeline = [s%locals() for s in self.SNAP_PIPELINE ]
        return filename, subprocess.Popen(
            pipeline,stderr = subprocess.PIPE
        )

    def snap(self):
        """Snap a picture via the camera by iterating gstreamer until finished
        
        Note: this is an unsafe implementation, it will cause the whole 
        activity to hang until the capture finishes.  Time to finish is often
        measured in whole seconds (3-6s).
       
        It is *strongly* recommended that you use snap_async instead of snap!
        """
        log.debug( 'Starting snap' )
        filename, pipe = self._create_subprocess()
        if not pipe.wait():
            log.debug( 'Ending snap, loading: %s', filename )
            return self._load_and_clean( filename )
        else:
            raise IOError( """Unable to complete snapshot: %s""", pipe.stderr.read() )
    def _load_and_clean( self, filename ):
        """Use pygame to load given filename, delete after loading/attempt"""
        try:
            log.info( 'Loading snapshot file: %s', filename )
            return pygame.image.load(filename)
        finally:
            try:
                os.remove( filename )
            except (IOError,OSError), err:
                pass
    def snap_async( self, token=None ):
        """Snap a picture asynchronously generating event on success/failure
        
        token -- passed back as attribute of the event which signals that capture
            is finished
        
        We return events of type CAMERA_LOAD with an attribute "succeed"
        depending on whether we succeed or not.  Attributes of the events which 
        are returned:
        
            success -- whether the loading process succeeded
            token -- as passed to this method 
            image -- pygame image.load result if successful, None otherwise
            filename -- the filename in our temporary directory we used to store 
                the file temporarily (this file will be deleted before the event 
                is sent, the name is for informational purposes only).
            err -- Exception instance if failed, None otherwise
        
        Basically identical to the snap method, save that it posts a message 
        to the event bus in pygame.event instead of blocking and returning...
        
        Example:
            if event == pygame.MOUSEBUTTONDOWN:
                camera = Camera( source='test', filename = 'picture32' )
                camera.snap_async( myIdentifier )
            ...
            elif event.type == olpcgames.CAMERA_LOAD:
                if event.token == myIdentifier:
                    doSomething( event.image )
        """
        log.debug( 'beginning async snap')
        t = threading.Thread(target=self._background_snap, args=[token])
        t.start()
        return token

    def _background_snap( 
        self,
        token = None,
    ):
        """Process gst messages until pipe is finished
        
        pipe -- gstreamer pipe definition for parse_launch, normally it will 
            produce a file into which the camera should store an image
        
        We consider pipe to be finished when we have had two "state changed"
        gstreamer events where the pending state is VOID, the first for when 
        we begin playing, the second for when we finish.
        """
        log.debug( 'Background thread kicking off gstreamer capture begun' )
        from pygame.event import Event, post
        filename, pipe = self._create_subprocess()
        if not pipe.wait():
            success = True
            log.debug( 'Ending capture, loading: %s', filename )
            try:
                image = self._load_and_clean( filename )
            except Exception, err:
                image = None 
                success = False
            else:
                err = None
        else:
            success = False
            err = pipe.stderr.read()
            image = None
        evt = Event(
            CAMERA_LOAD, 
            dict(
                filename=filename, 
                success = success,
                token = token, 
                image=image, 
                err=err
            )
        )
        post( evt )

def snap():
    """Dump a snapshot from the camera to a pygame surface in background thread
    
    See Camera.snap
    """
    return Camera().snap()

def snap_async( token=None, **named ):
    """Dump snapshot from camera return asynchronously as event in Pygame
    
    See Camera.snap_async
    """
    return Camera(**named).snap_async( token )
