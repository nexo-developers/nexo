"""gtkEvent.py: translate GTK events into Pygame events."""
import pygtk
pygtk.require('2.0')
import gtk
import gobject
import pygame
from olpcgames import eventwrap
import logging 
log = logging.getLogger( 'olpcgames.gtkevent' )
##log.setLevel( logging.DEBUG )

class _MockEvent(object):
    """Used to inject key-repeat events on the gtk side."""
    def __init__(self, keyval):
        self.keyval = keyval

class Translator(object):
    """Utility class to translate GTK events into Pygame events 
    
    The Translator object interprets incoming GTK events and generates
    Pygame events in the eventwrap module's queue as a result.
    It also handles generating Pygame style key-repeat events 
    by synthesizing them via a GTK timer.
    """
    key_trans = {
        'Alt_L': pygame.K_LALT,
        'Alt_R': pygame.K_RALT,
        'Control_L': pygame.K_LCTRL,
        'Control_R': pygame.K_RCTRL,
        'Shift_L': pygame.K_LSHIFT,
        'Shift_R': pygame.K_RSHIFT,
        'Super_L': pygame.K_LSUPER,
        'Super_R': pygame.K_RSUPER,
        'KP_Page_Up' : pygame.K_KP9, 
        'KP_Page_Down' : pygame.K_KP3,
        'KP_End' : pygame.K_KP1, 
        'KP_Home' : pygame.K_KP7,
        'KP_Up' : pygame.K_KP8,
        'KP_Down' : pygame.K_KP2,
        'KP_Left' : pygame.K_KP4,
        'KP_Right' : pygame.K_KP6,

    }
    
    mod_map = {
        pygame.K_LALT: pygame.KMOD_LALT,
        pygame.K_RALT: pygame.KMOD_RALT,
        pygame.K_LCTRL: pygame.KMOD_LCTRL,
        pygame.K_RCTRL: pygame.KMOD_RCTRL,
        pygame.K_LSHIFT: pygame.KMOD_LSHIFT,
        pygame.K_RSHIFT: pygame.KMOD_RSHIFT,
    }
    
    def __init__(self, mainwindow, mouselistener=None):
        """Initialise the Translator with the windows to which to listen"""
        # _inner_evb is Mouselistener
        self._mainwindow = mainwindow
        if mouselistener is None:
            mouselistener = mainwindow

        self._inner_evb = mouselistener

        # Need to set our X event masks so we see mouse motion and stuff --
        mainwindow.set_events(
            gtk.gdk.KEY_PRESS_MASK | \
            gtk.gdk.KEY_RELEASE_MASK \
        )
        
        self._inner_evb.set_events(
            gtk.gdk.POINTER_MOTION_MASK | \
            gtk.gdk.POINTER_MOTION_HINT_MASK | \
            gtk.gdk.BUTTON_MOTION_MASK | \
            gtk.gdk.BUTTON_PRESS_MASK | \
            gtk.gdk.BUTTON_RELEASE_MASK 
        )
          
        # Callback functions to link the event systems
        mainwindow.connect('unrealize', self._quit)
        mainwindow.connect('key_press_event', self._keydown)
        mainwindow.connect('key_release_event', self._keyup)
        self._inner_evb.connect('button_press_event', self._mousedown)
        self._inner_evb.connect('button_release_event', self._mouseup)
        self._inner_evb.connect('motion-notify-event', self._mousemove)

        # You might need to do this
        mainwindow.set_flags(gtk.CAN_FOCUS)
        self._inner_evb.set_flags(gtk.CAN_FOCUS)
        
        # Internal data
        self.__stopped = False
        self.__keystate = [0] * 323
        self.__button_state = [0,0,0]
        self.__mouse_pos = (0,0)
        self.__repeat = (None, None)
        self.__held = set()
        self.__held_time_left = {}
        self.__held_last_time = {}
        self.__tick_id = None

        #print "translator  initialized"
        self._inner_evb.connect( 'expose-event', self.do_expose_event )
#        screen = gtk.gdk.screen_get_default()
#        screen.connect( 'size-changed', self.do_resize_event )
        self._inner_evb.connect( 'configure-event', self.do_resize_event )
    def do_expose_event(self, event, widget):
        """Handle exposure event (trigger redraw by gst)"""
        log.info( 'Expose event: %s', event )
        from olpcgames import eventwrap
        eventwrap.post( eventwrap.Event( eventwrap.pygame.VIDEOEXPOSE ))
        return True
    def do_resize_event( self, activity, event ):
        """Our screen (actually, the default screen) has resized"""
        log.info( 'Resize event: %s %s', activity, event )
        log.info( 'Event values: %s', (event.width,event.height) )
#        from olpcgames import eventwrap
#        # shouldn't the activity's window have this information too?
#        eventwrap.post( 
#            eventwrap.Event( 
#                eventwrap.pygame.VIDEORESIZE, 
#                dict(size=(event.width,event.height), width=event.width, height=event.height) 
#            )
#        )
        return False # continue processing
    def hook_pygame(self):
        """Hook the various Pygame features so that we implement the event APIs"""
        # Pygame should be initialized. Hijack their key and mouse methods
        pygame.key.get_pressed = self._get_pressed
        pygame.key.set_repeat = self._set_repeat
        pygame.mouse.get_pressed = self._get_mouse_pressed
        pygame.mouse.get_pos = self._get_mouse_pos
        import eventwrap
        eventwrap.install()
        
    def _quit(self, data=None):
        self.__stopped = True
        eventwrap.post(eventwrap.Event(pygame.QUIT))

    def _keydown(self, widget, event):
        key = event.keyval
        log.debug( 'key down: %s', key )
        if key in self.__held:
            return True
        else:
            if self.__repeat[0] is not None:
                self.__held_last_time[key] = pygame.time.get_ticks()
                self.__held_time_left[key] = self.__repeat[0]
            self.__held.add(key)
            
        return self._keyevent(widget, event, pygame.KEYDOWN)
        
    def _keyup(self, widget, event):
        key = event.keyval
        if self.__repeat[0] is not None:
            if key in self.__held:
                # This is possibly false if set_repeat() is called with a key held
                del self.__held_time_left[key]
                del self.__held_last_time[key]
        self.__held.discard(key)

        return self._keyevent(widget, event, pygame.KEYUP)
        
    def _keymods(self):
        """Extract the keymods as they stand currently."""
        mod = 0
        for key_val, mod_val in self.mod_map.iteritems():
            mod |= self.__keystate[key_val] and mod_val
        return mod
        
        
    def _keyevent(self, widget, event, type):
        key = gtk.gdk.keyval_name(event.keyval)
        if key is None:
            # No idea what this key is.
            return False 
        
        keycode = None
        if key in self.key_trans:
            keycode = self.key_trans[key]
        elif hasattr(pygame, 'K_'+key.upper()):
            keycode = getattr(pygame, 'K_'+key.upper())
        elif hasattr(pygame, 'K_'+key.lower()):
            keycode = getattr(pygame, 'K_'+key.lower())
        elif key == 'XF86Start':
            # view source request, specially handled...
            self._mainwindow.view_source()
        else:
            print 'Key %s unrecognized'%key
            
        if keycode is not None:
            if type == pygame.KEYDOWN:
                mod = self._keymods()
            self.__keystate[keycode] = type == pygame.KEYDOWN
            if type == pygame.KEYUP:
                mod = self._keymods()
            ukey = unichr(gtk.gdk.keyval_to_unicode(event.keyval))
            if ukey == '\000':
                ukey = ''
            evt = eventwrap.Event(type, key=keycode, unicode=ukey, mod=mod)
            assert evt.key, evt
            self._post(evt)
        return True

    def _get_pressed(self):
        """Retrieve map/array of which keys are currently depressed (held down)"""
        return self.__keystate

    def _get_mouse_pressed(self):
        """Return three-element array of which mouse-buttons are currently depressed (held down)"""
        return self.__button_state

    def _mousedown(self, widget, event):
        self.__button_state[event.button-1] = 1
        return self._mouseevent(widget, event, pygame.MOUSEBUTTONDOWN)

    def _mouseup(self, widget, event):
        self.__button_state[event.button-1] = 0
        return self._mouseevent(widget, event, pygame.MOUSEBUTTONUP)
        
    def _mouseevent(self, widget, event, type):

        evt = eventwrap.Event(type, 
                                             button=event.button, 
                                             pos=(event.x, event.y))
        self._post(evt)
        return True
        
    def _mousemove(self, widget, event):
        # From http://www.learningpython.com/2006/07/25/writing-a-custom-widget-using-pygtk/
        # if this is a hint, then let's get all the necessary 
        # information, if not it's all we need.
        if event.is_hint:
            x, y, state = event.window.get_pointer()
        else:
            x = event.x
            y = event.y
            state = event.state

        rel = (x - self.__mouse_pos[0],
                y - self.__mouse_pos[1])
        self.__mouse_pos = (x, y)
        
        self.__button_state = [
            state & gtk.gdk.BUTTON1_MASK and 1 or 0,
            state & gtk.gdk.BUTTON2_MASK and 1 or 0,
            state & gtk.gdk.BUTTON3_MASK and 1 or 0,
        ]
        
        evt = eventwrap.Event(pygame.MOUSEMOTION,
                                             pos=self.__mouse_pos,
                                             rel=rel,
                                             buttons=self.__button_state)
        self._post(evt)
        return True
        
    def _tick(self):
        """Generate synthetic events for held-down keys"""
        cur_time = pygame.time.get_ticks()
        for key in self.__held:
            delta = cur_time - self.__held_last_time[key] 
            self.__held_last_time[key] = cur_time
            
            self.__held_time_left[key] -= delta
            if self.__held_time_left[key] <= 0:
                self.__held_time_left[key] = self.__repeat[1]
                self._keyevent(None, _MockEvent(key), pygame.KEYDOWN)
                
        return True
        
    def _set_repeat(self, delay=None, interval=None):
        """Set the key-repetition frequency for held-down keys"""
        if delay is not None and self.__repeat[0] is None:
            self.__tick_id = gobject.timeout_add(10, self._tick)
        elif delay is None and self.__repeat[0] is not None:
            gobject.source_remove(self.__tick_id)
        self.__repeat = (delay, interval)
        
    def _get_mouse_pos(self):
        """Retrieve the current mouse position as a two-tuple of integers"""
        return self.__mouse_pos
            
    def _post(self, evt):
        try:
            eventwrap.post(evt)
        except pygame.error, e:
            if str(e) == 'Event queue full':
                print "Event queue full!"
                pass
            else:
                raise e
