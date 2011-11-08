"""RSVG/Cairo-based rendering of SVG into Pygame Images"""
from pygame import sprite, Rect
from olpcgames import _cairoimage

class SVGSprite( sprite.Sprite ):
    """Sprite class which renders SVG source-code as a Pygame image
    
    Note:
    
        Currently this sprite class is a bit over-engineered, it gets in the way 
        if you want to, e.g. animate among a number of SVG drawings, as it 
        assumes that setSVG will always set a single SVG file for rendering.
    """
    rect = image = None
    resolution = None
    def __init__( 
        self, svg=None, size=None, *args
    ):
        """Initialise the svg sprite
        
        svg -- svg source text (i.e. content of an svg file)
        size -- optional, to constrain size, (width,height), leaving one 
            as None or 0 causes proportional scaling, leaving both 
            as None or 0 causes natural scaling (screen resolution)
        args -- if present, groups to which to automatically add
        """
        self.size = size
        super( SVGSprite, self ).__init__( *args )
        if svg:
            self.setSVG( svg )
    def setSVG( self, svg ):
        """Set our SVG source"""
        self.svg = svg
        # XXX could delay this until actually asked to display...
        if self.size:
            width,height = self.size
        else:
            width,height = None,None
        self.image = self._render( width,height ).convert_alpha()
        rect = self.image.get_rect()
        if self.rect:
            rect.move( self.rect ) # should let something higher-level do that...
        self.rect = rect 

    def _render( self, width, height ):
        """Render our SVG to a Pygame image"""
        import rsvg
        handle = rsvg.Handle( data = self.svg )
        originalSize = (width,height)
        scale = 1.0
        hw,hh = handle.get_dimension_data()[:2]
        if hw and hh:
            if not width:
                if not height:
                    width,height = hw,hh 
                else:
                    scale = float(height)/hh
                    width = hh/float(hw) * height
            elif not height:
                scale = float(width)/hw
                height = hw/float(hh) * width
            else:
                # scale only, only rendering as large as it is...
                if width/height > hw/hh:
                    # want it taller than it is...
                    width = hh/float(hw) * height
                else:
                    height = hw/float(hh) * width
                scale = float(height)/hh
            
            csrf, ctx = _cairoimage.newContext( int(width), int(height) )
            ctx.scale( scale, scale )
            handle.render_cairo( ctx )
            return _cairoimage.asImage( csrf )
        return None
    def copy( self ):
        """Create a copy of this sprite without reloading the svg image"""
        result = self.__class__(
            size = self.size
        )
        result.image = self.image 
        result.rect = Rect(self.rect)
        result.resolution = self.resolution
        return result
