"""Utility functions for cairo-specific operations

USE_BASE_ARRAY -- if False (default), uses numpy arrays,
    currently this is the only version that works on 32-bit
    machines.
"""
import pygame, struct, logging
big_endian = struct.pack( '=i', 1 ) == struct.pack( '>i', 1 )

log = logging.getLogger( 'olpcgames._cairoimage' )
##log.setLevel( logging.DEBUG )

USE_BASE_ARRAY = False

def newContext( width, height ):
    """Create a new render-to-image context
    
    width, height -- pixel dimensions to be rendered
    
    Produces an ARGB format Cairo ImageSurface for 
    rendering your data into using rsvg, Cairo or Pango.
    
    returns (ImageSurface, CairoContext) for rendering
    """
    import cairo
    csrf = cairo.ImageSurface(cairo.FORMAT_ARGB32, width, height)
    context = cairo.Context (csrf)
    #log.info( 'Format (expect: %s): %s', cairo.FORMAT_ARGB32, csrf.get_format())
    return csrf, context

def mangle_color(color):
    """Mange a colour depending on endian-ness, and swap-necessity
    
    Converts a 3 or 4 int (or float) value in the range 0-255 into a 
    4-float value in the range 0.0-1.0
    """
    r,g,b = color[:3]
    if len(color) > 3:
        a = color[3]
    else:
        a = 255.0
    return map(_fixColorBase, (r,g,b,a) )

def _fixColorBase( v ):
    """Return a properly clamped colour in floating-point space"""
    return max((0,min((v,255.0))))/255.0

def asImage( csrf ):
    """Get the pixels in csrf as a Pygame image
    
    Note that Pygame 1.7.1 on (Gentoo Linux) AMD64 is incorrectly 
    calculating the required size ARGB images, so this code will *not* work 
    on that  platform with that version of the library.  Pygame-ctypes 
    does work correctly there.
    
    Note also that Pygame 1.7.1 is showing a strange colour rotation 
    bug on 32-bit platforms, such that ARGB mode cannot be used for 
    images there.  Instead we have to do an expensive bit-shift operation
    to produce an RGBA image from the ARGB native Cairo format.
    
    Will raise a ValueError if passed a Null image (i.e. dimension of 0)
    
    returns Pygame.Surface (image) with convert_alpha() called for it.
    """
    # Create and return a new Pygame Image derived from the Cairo Surface
    format = 'ARGB'
    if hasattr(csrf,'get_data'):
        # more recent API, native-format, but have to (potentially) convert the format...
        log.debug( 'Native-mode api (get_data)' )
        data = csrf.get_data()
        if not big_endian:
            # we use array here because it's considerably lighter-weight
            # to import than the numpy module
            log.debug( 'Not big-endian, byte-swapping array' )
            if USE_BASE_ARRAY:
                import array
                a = array.array( 'I' )
                a.fromstring( data )
                a.byteswap()
                data = a.tostring()
            else:
                import numpy 
                n = numpy.fromstring( data, dtype='I' )
                n =  ((n & 0xff000000) >> 24  ) | ((n & 0x00ffffff) << 8 )
                n = n.byteswap()
                data = n.tostring()
                format = 'RGBA'
        else:
            log.debug( 'Big-endian, array unchanged' )
            data = str(data) # there's one copy
    else:
        # older api, not native, but we know what it is...
        log.debug( 'Non-native mode api, explicitly RGBA' )
        data = csrf.get_data_as_rgba()
        data = str(data) # there's one copy
        format = 'RGBA'
    width, height = csrf.get_width(),csrf.get_height()

    try:
        log.info( 'Format = %s', format )
        return pygame.image.fromstring(
            data, 
            (width,height), 
            format
        ) # there's the next
    except ValueError, err:
        err.args += (len(data), (width,height), width*height*4,format )
        raise

if __name__ == "__main__":
    import unittest
    logging.basicConfig()
    class Tests( unittest.TestCase ):
        def test_colours( self ):
            """Test that colours are correctly translated
            
            If we draw a given colour in cairo, we want the same
            colour to show up in Pygame, let's test that...
            """
            for sourceColour in [
                (255,0,0, 255),
                (0,255,0, 255),
                (0,0,255, 255),
                (255,255,0, 255),
                (0,255,255,255),
                (255,0,255,255),
            ]:
                csrf,cctx = newContext( 1,1 )
                background = mangle_color( sourceColour )
                cctx.set_source_rgba(*background)
                cctx.paint()
                img = asImage( csrf )
                colour = img.get_at( (0,0))
                assert colour == sourceColour, (sourceColour,mangle_color(sourceColour),colour)
    unittest.main()
