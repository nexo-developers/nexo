"""Display a "paused" version of the currently-displayed screen

This code is largely cribbed from the Pippy activity's display code,
but we try to be a little more generally usable than they are, as
we have more involved activities using the code.

We use svgsprite to render a graphic which is stored in the 
olpcgames data directory over a dimmed version of the current
screen contents.

_LAST_EVENT_TIME -- tracks the last time that we saw an event 
    come across the wire.
"""
import logging
log = logging.getLogger( 'olpcgames.pausescreen' )
import pygame
from pygame import sprite

_LAST_EVENT_TIME = 0

def _set_last_event_time( time=None ):
    """Set time as the last event time
    
    time -- if None, pygame.time.get_ticks() is used
    
    returns time set
    """
    global _LAST_EVENT_TIME
    if time is None:
        time = pygame.time.get_ticks()
    _LAST_EVENT_TIME = time 
    return time 

def last_event_time( ):
    """Return the duration since last event for pausing operations
   
    returns time in seconds
    """
    global _LAST_EVENT_TIME
    return (pygame.time.get_ticks() - _LAST_EVENT_TIME)/1000.


def get_events( sleep_timeout = 10, pause=None, **args ):
    """Retrieve the set of pending events or sleep
    
    sleep_timeout -- dormant period before we invoke pause_screen
    pause -- callable to produce visual notification of pausing, normally
        by taking the current screen and modifying it in some way.  Defaults
        to pauseScreen in this module.  If you return nothing from this 
        function then no restoration or display-flipping will occur
    *args -- if present, passed to 'pause' to configuration operation (e.g.
        to specify a different overlaySVG file)
    
    returns set of pending events (potentially empty)
    """
    if not pause:
        pause = pauseScreen
    events = pygame.event.get( )
    if not events:
        log.info( 'No events in queue' )
        old_screen = None
        if last_event_time() > sleep_timeout:
            # we've been waiting long enough, go to sleep visually
            log.warn( 'Pausing activity after %s with function %s', sleep_timeout, pause )
            old_screen = pause( )
            if old_screen:
                pygame.display.flip()
            # now we wait until there *are* some events (efficiently)
            # and retrieve any extra events that are waiting...
            events = [ pygame.event.wait() ] + pygame.event.get()
            log.warn( 'Activity restarted')
            if old_screen:
                restoreScreen( old_screen )
    if events:
        _set_last_event_time()
    return events

def pauseScreen( overlaySVG=None ):
    """Display a "Paused" screen and suspend
    
    This default implementation will not do anything to shut down your 
    simulation or other code running in other threads.  It will merely block 
    this thread (the pygame thread) until an event shows up in the 
    eventwrap queue.
    
    Returns a surface to pass to restoreScreen to continue...
    """
    from olpcgames import svgsprite
    if not overlaySVG:
        from olpcgames.data import sleeping_svg
        overlaySVG = sleeping_svg.data
    screen = pygame.display.get_surface()
    old_screen = screen.copy()  # save this for later.
    pause_sprite = svgsprite.SVGSprite(
        overlaySVG,
    )
    pause_sprite.rect.center = screen.get_rect().center
    group = sprite.RenderUpdates( )
    group.add( pause_sprite )

    # dim the screen and display the 'paused' message in the center.
    BLACK = (0,0,0)
    WHITE = (255,255,255)
    dimmed = screen.copy()
    dimmed.set_alpha(128)
    screen.fill(BLACK)
    screen.blit(dimmed, (0,0))
    
    group.draw( screen )
    return old_screen

def restoreScreen( old_screen ):
    """Restore the original screen and return"""
    screen = pygame.display.get_surface()
    screen.blit(old_screen, (0,0))
    return old_screen
