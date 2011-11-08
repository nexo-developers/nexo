"""Simple Sprite sub-class that renders via a PangoFont"""
from pygame import sprite 
from olpcgames import pangofont

class TextSprite( sprite.Sprite ):
    """Sprite with a simple text renderer"""
    image = rect = text = color = background = None
    def __init__( self, text=None, family=None, size=None, bold=False, italic=False, color=None, background=None ):
        super( TextSprite, self ).__init__( )
        self.font = pangofont.PangoFont( family=family, size=size, bold=bold, italic=italic )
        self.set_color( color )
        self.set_background( background )
        self.set_text( text )
    def set_text( self, text ):
        """Set our text string and render to a graphic"""
        self.text = text 
        self.render( )
    def set_color( self, color =None):
        """Set our rendering colour (default white)"""
        self.color = color or (255,255,255)
        self.render()
    def set_background( self, color=None ):
        """Set our background color, default transparent"""
        self.background = color 
        self.render()
    def render( self ):
        """Render our image and rect (or None,None)
        
        After a render you will need to move the rect member to the 
        correct location on the screen.
        """
        if self.text:
            self.image = self.font.render( self.text, color = self.color, background = self.background )
            currentRect = self.rect
            self.rect = self.image.get_rect()
            if currentRect:
                self.rect.center = currentRect.center 
        else:
            self.rect = None 
            self.image = None
