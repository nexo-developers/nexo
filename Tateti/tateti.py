import olpcgames, pygame, logging
import time

log = logging.getLogger('TatetiMesh run')
log.setLevel(logging.DEBUG)

class Tateti (object):
    # Velocidad de Barrido del tablero (msec per pixel)
    SPEED = 250
    # Constante de variacion de la velociadad de barrido
    SPEED_STEP = 50
    SPEED_MIN = SPEED_STEP
    SPEED_MAX = SPEED_STEP*20
    
    # Cantidad de Casillas (Celdas)
    ROW_COUNT = 3
    COL_COUNT = 3 

    # Colores
    GRAY = 0x888888
    RED  = 0xAA0000
    WHITE = 0xFFFFFF
    BLUE = 0x0000AA
    CELL_COLOR = WHITE
    BACK_COLOR = GRAY
    SELECTION_COLOR = GRAY
    # Color de los simboles X y O
    CROSS_COLOR = BLUE
    CIRCLE_COLOR = RED
    
    # Identificadores de jugador
    X = -1
    O = 1 


    def __init__(self, screen):  
        # Tamanio de la actividad
        self.window = screen.get_size()
        log.debug("TAMANIO: "+str(self.window[0])+", "+str(self.window[1]))
        # Tamanio del tablero. Cuadrado que ocupa la totalidad del lado mas pequenio del espacio disponible
        bWidth = min(self.window[0], self.window[1])
        # Para calcular la altura es necesario descontar el tamanio de la TOOLBAR
        bHeight = min(self.window[0], self.window[1])
        self.board = (bWidth, bHeight) 
         # Tamanio de las celdas
        self.CELL_HEIGHT = int(self.board[1]/self.ROW_COUNT)
        self.CELL_WIDTH = int(self.board[0]/self.COL_COUNT)
        self.BORDER = self.CELL_HEIGHT/20
        self.cellCount = self.ROW_COUNT * self.COL_COUNT
        
        # INICIALIZACIONES
        pygame.init()
        self.clock = pygame.time.Clock()
        pygame.font.init()
        self.fontMgr = pygame.font.Font(None, self.board[0]/10)
        
        self.imgX = pygame.image.load("img/X.png").convert_alpha()
        self.imgX.set_colorkey((255,255,255))
        self.imgO = pygame.image.load("img/O.png").convert_alpha()
        self.imgO.set_colorkey((255,255,255))
        
        self.screen = screen
        self.screen.fill(self.BACK_COLOR)
        self.cells = [self.screen.fill(self.CELL_COLOR, (x+1, y+1, self.CELL_WIDTH-2, self.CELL_HEIGHT-2))
                       for y in range(0,self.board[1],self.CELL_HEIGHT) for x in range(0,self.board[0],self.CELL_WIDTH)]
        self.values = [0]*9    
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
        
        imgO = pygame.transform.smoothscale(self.imgO, (xRect.height, xRect.height))
        self.screen.blit(imgO, oRect.topleft)
        
        imgX = pygame.transform.smoothscale(self.imgX, (xRect.height, xRect.height))
        self.screen.blit(imgX, xRect.topleft)
        
        self.printWins(oRect, xRect, oWins, xWins)
        pygame.display.update()
        
    def printWins(self, oRect, xRect, oWins, xWins):    
        oTxt = self.fontMgr.render("x "+str(oWins), 1, (0,0,0), (255, 255, 255))
        xTxt = self.fontMgr.render("x "+str(xWins), 1, (0,0,0), (255, 255, 255))
        rect = oTxt.get_rect()
        rect.midright = oRect.midright
        rect.bottom += 5
        self.screen.blit(oTxt, rect)
        rect = xTxt.get_rect()
        rect.midright = xRect.midright 
        rect.bottom += 5
        self.screen.blit(xTxt, rect)
    
    def registroJugada(self,jugada, miJugador):
        # Invertimos el jugador para dibujar al otro
        self.values[jugada] = miJugador * -1
        self.printBoard(self.values)
    
    def setPointer(self,x):
        pygame.mouse.set_pos((x % (self.CELL_WIDTH * self.COL_COUNT), (self.CELL_HEIGHT / 2) + self.CELL_HEIGHT * int(x / (self.COL_COUNT * self.CELL_WIDTH))))
    
    def getScanningIndex(self, pos):
        #idx = int(pygame.mouse.get_pos()[0]/self.CELL_WIDTH) + self.COL_COUNT * int(pygame.mouse.get_pos()[1]/self.CELL_HEIGHT)
        #if idx < self.cellCount:
        #    return idx
        #else:
        #    return 0 
        return int(pos / self.CELL_WIDTH)
    
    def nextAvailableIndex(self, currentIndex):
        while not (self.values[currentIndex] == 0):
            currentIndex = (currentIndex + 1) % self.cellCount
        return currentIndex
       
    def fillCross(self, index, stroke):
        img = pygame.transform.smoothscale(self.imgX, (self.CELL_WIDTH*3/4, self.CELL_HEIGHT*3/4))
        self.screen.blit(img, self.cells[index].inflate(-self.CELL_WIDTH/4, -self.CELL_HEIGHT/4))
      
    def fillCircle(self, index):
        img = pygame.transform.smoothscale(self.imgO, (self.CELL_WIDTH*3/4, self.CELL_HEIGHT*3/4))
        self.screen.blit(img, self.cells[index].inflate(-self.CELL_WIDTH/4, -self.CELL_HEIGHT/4))
    
    def printMiniSymbol(self, index, me):
        if me == self.X:
            myImg = self.imgX
        else:
            myImg = self.imgO
        img = pygame.transform.smoothscale(myImg, (self.CELL_WIDTH/4, self.CELL_HEIGHT/4))           
        self.screen.blit(img, self.cells[index].inflate(-self.CELL_WIDTH*3/4, -self.CELL_HEIGHT*3/4))
        pygame.display.update()    
    
    def resetCell(self, index):
        self.screen.fill(self.CELL_COLOR, self.cells[index])

    def speedUp(self):
        if self.SPEED < self.SPEED_MAX:
            self.SPEED += self.SPEED_STEP
            log.debug("VELOCIDAD+: " + str(self.SPEED))
   
    def speedDown(self):
        if self.SPEED > self.SPEED_MIN:
            self.SPEED -= self.SPEED_STEP
            log.debug("VELOCIDAD-: " + str(self.SPEED))
    
    def printBoard(self, values):
        for i in range(self.cellCount):
            self.resetCell(i)
            if values[i] == self.X:
                self.fillCross(i, 20)
            elif values[i] == self.O:
                self.fillCircle(i)
        
    def testBoard(self, player):
        lines = (self.values[0:3], self.values[3:6], self.values[6:9], self.values[0:7:3], self.values[1:8:3], self.values[2:9:3], self.values[0:9:4], self.values[2:7:2])
        
        if [player]*3 in lines:
            #Gano
            return player
        elif [-player]*3 in lines:
            #Pierdo
            return -player
        else:
            return 0
    
    def printText(self, text):
        txt = self.fontMgr.render(text + "!!!", 1, (0,0,0), (240,240,255))
        rect = txt.get_rect()
        rect.center = (self.board[0]/2, self.board[1]/2)
        self.screen.blit(txt,rect)
        pygame.display.update()
        
    def playMe(self, me):
        # Arranca seleccionada la celda de arriba a la izquierda
        scanningIndex = -1
        i = 0
        #for i in range(self.cellCount * self.CELL_WIDTH):
        while i < (self.cellCount * self.CELL_WIDTH):
            self.clock.tick(self.SPEED)
            # (Re)posicionamos el cursor en la ventana, dando sensacion de barrido
            self.setPointer(i)
            tmpIndex = self.getScanningIndex(i)
            if not (scanningIndex == tmpIndex):
                # Hubo cambio en la opcion actualmente seleccionable. Hay que actualizar el estado de la escena
                #scanningIndex = tmpIndex
                scanningIndex = self.nextAvailableIndex(tmpIndex)
                #PROBANDO...
                offset = i % self.CELL_WIDTH
                i = (scanningIndex * self.CELL_WIDTH) + offset
                #render mini symbol
                self.printBoard(self.values)
                self.printMiniSymbol(scanningIndex,me)
                
            # Capturamos un clic
            for ev in pygame.event.get():
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    # Clic derecho termina
                    if ev.button == 3:
                        log.debug("BOTON DERECHO")
                        exit(0)
                    
                    # Clic izquierdo, tenemos que determinar la celda elegida en funcion 
                    # de la posicion del puntero al momento de clickear
                    selection = self.getScanningIndex(i)
                    # Registramos la opcion elegida y termina el ciclo
                    if self.values[selection] == 0:
                        self.values[selection] = me
                        self.printBoard(self.values)
                        return selection
                elif ev.type == pygame.KEYDOWN:
                    log.debug("PRESIONO TECLA")
                    #if ev.key == pygame.K_PRINT:
                    if ev.key == pygame.K_UP:			        #ACA TENEMOS UN POSIBLE PROBLEMA DE MAPEO DE TECLAS#
                        log.debug("PRESIONO ARRIBA")			#CON EL EMULADOR, ME TOMA FLECHA ARRIBA COMO PRINT SCREEN#
                        self.speedUp()				#Y FLECHA ABAJO COMO RIGHT SUPER#
                    #elif ev.key == pygame.K_RSUPER:
                    elif ev.key == pygame.K_DOWN:
                        log.debug("PRESIONO ABAJO")
                        self.speedDown()
                    else:
                        log.debug("Tecla INESPERADA: " + pygame.key.name(ev.key))
            i += 1
        return self.playMe(me)
