import olpcgames, pygame, logging
import time

log = logging.getLogger('TatetiMesh run')
log.setLevel(logging.DEBUG)

class PPT (object):
    # Colores
    GRAY = 0x888888
    RED  = 0xAA0000
    WHITE = 0xFFFFFF
    BLUE = 0x0000AA
    CELL_COLOR = WHITE
    BACK_COLOR = GRAY
    SELECTION_COLOR = GRAY
    
    # Color de los simbolos X y O
    CROSS_COLOR = BLUE
    CIRCLE_COLOR = RED
    
    # Identificadores de jugador
    Yo = 0
    Oponente = 1 

    Nada = 0
    Roca = 1
    Papel = 2
    Tijera = 3
    
    gane = 1
    perdi = -1
    empate = 0
    noresuelto = 2

    def __init__(self, screen):  
        # Tamanio de la actividad
        self.window = screen.get_size()
        log.debug("TAMANIO: "+str(self.window[0])+", "+str(self.window[1]))
        # Tamanio del tablero. Cuadrado que ocupa la totalidad del lado mas pequenio del espacio disponible
        bWidth = min(self.window[0], self.window[1])
        # Para calcular la altura es necesario descontar el tamanio de la TOOLBAR
        bHeight = min(self.window[0], self.window[1])
        self.board = (bWidth, bHeight) 
        
        # INICIALIZACIONES
        pygame.init()
        self.clock = pygame.time.Clock()
        pygame.font.init()
        self.fontMgr = pygame.font.Font(None, self.board[0]/10)
        
        self.imgX = pygame.image.load("img/X.png").convert_alpha()
        self.imgX.set_colorkey((255,255,255))
        self.imgO = pygame.image.load("img/O.png").convert_alpha()
        self.imgO.set_colorkey((255,255,255))
        
        self.imgRoca = pygame.image.load("img/piedra.png").convert_alpha()
        self.imgRoca.set_colorkey((255,255,255))
        self.imgPapel = pygame.image.load("img/papel.png").convert_alpha()
        self.imgPapel.set_colorkey((255,255,255))
        self.imgTijera = pygame.image.load("img/tijera.png").convert_alpha()
        self.imgTijera.set_colorkey((255,255,255))
        
        self.screen = screen
        self.screen.fill(self.BACK_COLOR)
        
        self.values = [self.Nada,self.Nada]
        
        # Pintamos el tablero
        self.printBoard(self.values)
        # Pintamos el panel de la derecha (logo)
        self.printLogo()
        # Changui para que estabilice el dibujo
        time.sleep(1)  
        pygame.display.flip()
    
    def printLogo(self):
        img = pygame.image.load("img/Tateti.png").convert_alpha()
        img.set_colorkey((255,255,255))
        panel = (self.board[0]+1, 1, self.window[0]-self.board[0]-2, self.window[1]-2)
        self.screen.fill(self.CELL_COLOR, panel)
        IMG_RATIO = 2.05
        imgHeight = int(panel[3]/2 - 20)
        imgWidth = int(imgHeight/IMG_RATIO)
        padding = (panel[2]-imgWidth)/2
        
        self.screen.blit(pygame.transform.smoothscale(img, (imgWidth, imgHeight)), (self.board[0]+padding, 20))
    
    def printInfoPanel(self, oWins, xWins):
        panel = (self.board[0]+1, 1, self.window[0]-self.board[0]-2, self.window[1]-2)
        infoPanel = pygame.Rect(panel[0], (self.window[1]-2)/2, panel[2], (self.window[1]-2)/2)
        self.screen.fill(self.CELL_COLOR, infoPanel) 
        
        oRect = infoPanel.inflate(-panel[2]/2, -infoPanel[3]*4/5)
        oRect.midbottom = infoPanel.center
        
        xRect = infoPanel.inflate(-panel[2]/2, -infoPanel[3]*4/5)
        xRect.midtop = infoPanel.center
        xRect.top += 10
        
#        imgO = pygame.transform.smoothscale(self.imgO, (xRect.height, xRect.height))
#        self.screen.blit(imgO, oRect.topleft)
#        
#        imgX = pygame.transform.smoothscale(self.imgX, (xRect.height, xRect.height))
#        self.screen.blit(imgX, xRect.topleft)
        
        self.printWins(oRect, xRect, oWins, xWins)
        pygame.display.update()
        
    def printWins(self, oRect, xRect, oWins, xWins):    
        oTxt = self.fontMgr.render("   Yo: "+str(oWins), 1, (0,0,0), (255, 255, 255))
        xTxt = self.fontMgr.render("Amigo: "+str(xWins), 1, (0,0,0), (255, 255, 255))
        rect = oTxt.get_rect()
        rect.midright = oRect.midright
        rect.bottom += 5
        self.screen.blit(oTxt, rect)
        rect = xTxt.get_rect()
        rect.midright = xRect.midright 
        rect.bottom += 5
        self.screen.blit(xTxt, rect)
    
    def registroJugada(self,jugada, jugador):
        # Invertimos el jugador para dibujar al otro
        self.values[jugador] = jugada
        self.printBoard(self.values)
    
    def printBoard(self, values):
        # TODO imprimir tarjetas
        log.debug("Mi jugada es " + str(values[self.Yo]))
        log.debug("La jugada del op es " + str(values[self.Oponente]))
        return
        
    def testBoard(self, player):
        if self.values[self.Yo] == self.Nada or self.values[self.Oponente] == self.Nada:
            return self.noresuelto
        if self.values[self.Yo] <> self.Nada and self.values[self.Yo] == self.values[self.Oponente]:
            return self.empate
        if self.values[self.Yo] == self.Roca and self.values[self.Oponente] == self.Tijera:
            return self.gane
        elif self.values[self.Yo] == self.Tijera and self.values[self.Oponente] == self.Papel:
            return self.gane
        elif self.values[self.Yo] == self.Papel and self.values[self.Oponente] == self.Roca:
            return self.gane
        else:
            return self.perdi
    
    def printText(self, text, yOffset=0):
        txt = self.fontMgr.render(text + "!!!", 1, (0,0,0), (240,240,255))
        rect = txt.get_rect()
        rect.center = (self.board[0]/2, self.board[1]/2 + yOffset)
        self.screen.blit(txt,rect)
        pygame.display.update()
        
    def printJugadas(self):        
        if self.values[self.Yo]<>self.Nada:        
            posYo = (50,150)
            if self.values[self.Yo] == self.Roca:
                self.screen.blit(self.imgRoca, posYo)
            elif self.values[self.Yo] == self.Papel:
                self.screen.blit(self.imgPapel, posYo)
            elif self.values[self.Yo] == self.Tijera:
                self.screen.blit(self.imgTijera, posYo)
        
            posOp = (300,150)
            if self.values[self.Oponente] == self.Roca:                
                self.screen.blit(pygame.transform.flip(self.imgRoca, True, False), posOp)
            elif self.values[self.Oponente] == self.Papel:
                self.screen.blit(pygame.transform.flip(self.imgPapel, True, False), posOp)
            elif self.values[self.Oponente] == self.Tijera:
                self.screen.blit(pygame.transform.flip(self.imgTijera, True, False), posOp)
            