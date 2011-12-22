"""Implements bridge connection between Sugar/GTK and Pygame"""
import os
import sys
import logging
log = logging.getLogger( 'olpcgames.canvas' )
##log.setLevel( logging.DEBUG )
import threading
from pprint import pprint

import pygtk
pygtk.require('2.0')
import gtk
import gobject
import pygame

from olpcgames import gtkEvent, util

__all__ = ['PygameCanvas']

class PygameCanvas(gtk.Layout):
    """Canvas providing bridge methods to run Pygame in GTK
    
    The PygameCanvas creates a secondary thread in which the Pygame instance will 
    live, providing synthetic Pygame events to that thread via a Queue.  The GUI 
    connection is  done by having the Pygame canvas use a GTK Port object as it's 
    window pointer, it draws to that X-level window in order to produce output.
    """
    mod_name = None
    def __init__(self, width, height):
        """Initializes the Canvas Object
        
        width,height -- passed to the inner EventBox in order to request a given size,
            the Socket is the only child of this EventBox, and the Pygame commands
            will be writing to the Window ID of the socket.  The internal EventBox is 
            centered via an Alignment instance within the PygameCanvas instance.
            
        XXX Should refactor so that the internal setup can be controlled by the 
        sub-class, e.g. to get size from the host window, or something similar.
        """
        # Build the main widget
        log.info( 'Creating the pygame canvas' )
        super(PygameCanvas,self).__init__()
        self.set_flags(gtk.CAN_FOCUS)
        
        # Build the sub-widgets
        self._align = gtk.Alignment(0.5, 0.5)
        self._inner_evb = gtk.EventBox()
        self._socket = gtk.Socket()

        
        # Add internal widgets
        self._inner_evb.set_size_request(width, height)
        self._inner_evb.add(self._socket)
        
        self._socket.show()
        
        self._align.add(self._inner_evb)
        self._inner_evb.show()
        
        self._align.show()
        
        self.put(self._align, 0,0)
        
        # Construct a gtkEvent.Translator
        self._translator = gtkEvent.Translator(self, self._inner_evb)
        # <Cue Thus Spract Zarathustra>
        self.show()
    def connect_game(self, app):
        """Imports the given main-loop and starts processing in secondary thread 
        
        app -- fully-qualified Python path-name for the game's main-loop, with 
            name within module as :functionname, if no : character is present then 
            :main will be assumed.
        
        Side effects:
        
            Sets the SDL_WINDOWID variable to our socket's window ID 
            Calls Pygame init
            Causes the gtkEvent.Translator to "hook" Pygame
            Creates and starts secondary thread for Game/Pygame event processing.
        """
        log.info( 'Connecting the pygame canvas' )
        # Setup the embedding
        os.environ['SDL_WINDOWID'] = str(self._socket.get_id())
        #print 'Socket ID=%s'%os.environ['SDL_WINDOWID']
        pygame.init()

        self._translator.hook_pygame()
        
        # Load the modules
        # NOTE: This is delayed because pygame.init() must come after the embedding is up
        if ':' not in app:
            app += ':main'
        mod_name, fn_name = app.split(':')
        self.mod_name = mod_name
        mod = __import__(mod_name, globals(), locals(), [])
        fn = getattr(mod, fn_name)
        
        # Start Pygame
        self.__thread = threading.Thread(target=self._start, args=[fn])
        self.__thread.start()

    def _start(self, fn):
        """The method that actually runs in the background thread"""
        log.info( 'Staring the mainloop' )
        import olpcgames
        olpcgames.widget = olpcgames.WIDGET = self
        try:
            import sugar.activity.activity,os
        except ImportError, err:
            log.info( """Running outside Sugar""" )
        else:
            try:
                os.chdir(sugar.activity.activity.get_bundle_path())
            except KeyError, err:
                pass
        
        try:
            try:
                try:
                    log.info( '''Running mainloop: %s''', fn )
                    fn()
                except Exception, err:
                    log.error(
                        """Uncaught top-level exception: %s""",
                        util.get_traceback( err ),
                    )
                    raise
                else:
                    log.info( "Mainloop exited" )
            finally:
                log.debug( "Clearing any pending events" )
                from olpcgames import eventwrap
                eventwrap.clear()
        finally:
            log.info( 'Main function finished, calling main_quit' )
            gtk.main_quit()

    source_object_id = None
    def view_source(self):
        """Implement the 'view source' key by saving 
        datastore, and then telling the Journal to view it."""
        if self.source_object_id is None:
            from sugar import profile
            from sugar.datastore import datastore
            from sugar.activity.activity import get_bundle_name, get_bundle_path
            from gettext import gettext as _
            import os.path
            jobject = datastore.create()
            metadata = {
                'title': _('%s Source') % get_bundle_name(),
                'title_set_by_user': '1',
                'suggested_filename': 'pippy_app.py',
                'icon-color': profile.get_color().to_string(),
                'mime_type': 'text/x-python',
                }
            for k,v in metadata.items():
                jobject.metadata[k] = v # dict.update method is missing =(
            jobject.file_path = os.path.join(get_bundle_path(), 'pippy_app.py')
            datastore.write(jobject)
            self.__source_object_id = jobject.object_id
            jobject.destroy()
        self.journal_show_object(self.__source_object_id)
    def journal_show_object(self, object_id):
        """Invoke journal_show_object from sugar.activity.activity if it
        exists."""
        try:
            from sugar.activity.activity import show_object_in_journal
            show_object_in_journal(object_id)
        except ImportError:
            pass # no love from sugar.
