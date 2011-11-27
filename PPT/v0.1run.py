#! /usr/bin/env python
"""Skeleton project file mainloop for new OLPCGames users"""
import olpcgames, pygame, logging 
from olpcgames import pausescreen
import olpcgames.mesh as mesh
import time

log = logging.getLogger( 'TatetiMesh run' )
log.setLevel( logging.DEBUG )

def main():
    protocolo = Protocolo()
    protocolo.run()

class Protocolo():
    
    def __init__(self):

        """The mainloop which is specified in the activity.py file
        
        "main" is the assumed function name
        """
        # Variables de estado del juego
        self.estoyConectado = False
        self.hayAmigo = False
        self.jugando = False
        self.miTurno = False
        self.fin = False
        self.handleAmigo = None
        # Cruz o circulo
        self.miJugador = 0
        # Jugadas realizadas
        self.jugadas = 0
    
        self.size = (800,600)
        if olpcgames.ACTIVITY:
            self.size = olpcgames.ACTIVITY.game_size
        self.screen = pygame.display.set_mode(self.size)
        
        # inicio el juego para que se vea algo
        self.juego = Tateti(self.screen)
        
        self.clock = pygame.time.Clock()

    
    def run(self):
        running = True
        while running:
            milliseconds = self.clock.tick(25) # maximum number of frames per second
            
            # Event-management loop with support for pausing after X seconds (20 here)
            events = pausescreen.get_events()
            # Now the main event-processing loop
            if events:
                for event in events:
                    log.debug( "Event: %s", event )
                    if event.type == pygame.QUIT:
                        running = False
                    elif event.type == pygame.KEYDOWN:
                        if event.key == pygame.K_ESCAPE:
                            running = False
                    elif event.type == pygame.MOUSEBUTTONDOWN:
                        if event.button == 1:
                            # Si no estaba jugando, conecto y comienzo
                            if self.estoyConectado and self.hayAmigo and not self.jugando:
                                 # inicio juego
                                self.iniciarJuego(True, None)
                    elif event.type==mesh.CONNECT :
                        log.debug("CONNECT")
                        self.estoyConectado = True
                        if mesh.is_initiating():
                            self.miJugador = 1 # Cruz
                        else:
                            self.miJugador = -1 # Circulo
                    elif event.type==mesh.PARTICIPANT_ADD:
                        log.debug("PARTICIPANT_ADD")
                        # Si no soy yo
                        if event.handle <> mesh.my_handle():
                            self.hayAmigo = True
                            self.handleAmigo = event.handle
                            
                            # Si soy el servidor comienzo el primer juego
                            if mesh.is_initiating():
                                self.iniciarJuego(True, None)
                    elif event.type==mesh.PARTICIPANT_REMOVE:
                        log.debug("PARTICIPANT_REMOVE")
                    elif event.type==mesh.MESSAGE_MULTI:
                        log.debug("MESSAGE_MULTI")
                    elif event.type==mesh.MESSAGE_UNI:
                        log.debug("MESSAGE_UNI: " + event.content)
                        toks = event.content.split(':')
                        if toks[0] == 'configJugadores':
                             # inicio juego
                            self.iniciarJuego(False, toks[1] == 'True')
                        elif toks[0] == 'registroJugada':
                            jugada = int(toks[1])
                            self.juego.registroJugada(jugada, self.miJugador)
                            self.miTurno = True
                            self.jugadas += 1
                            self.evaluarJuego()
                			#juego.printResult("Empate")
                			#Fin chequeo de ganador
    
                pygame.display.flip()
                
            # Tomamos acciones
            
            if self.jugando and self.miTurno:
                jugada = self.juego.playMe(self.miJugador)
                self.miTurno = False
                self.jugadas += 1
                mesh.send_to(self.handleAmigo, "registroJugada:" + str(jugada))
                self.evaluarJuego()
    	    #Chequeo de ganador
           
            
            # If condicion
            # jugar
            # elif condicion
            # establecer juego
            
    def iniciarJuego(self, iniciador, miTurno):
        self.juego.printResult("Iniciando nuevo juego...")
        log.debug("Iniciando juego")
        # inicio juego
        self.juego = Tateti(self.screen)       
        self.jugando = True
        self.jugadas = 0
        
        if iniciador:
            # TODO: Sorteo
            self.miTurno = True
            # El turno del otro
            if self.miTurno:
                tuTStr = 'False'
            else:
                tuTStr = 'True'
            # Envio mensaje
            mesh.send_to(self.handleAmigo, "configJugadores:" + tuTStr)
        else:
            self.miTurno = miTurno
                
    def evaluarJuego(self):
        result = self.juego.testBoard(self.miJugador)
        if result == self.miJugador:
            self.juego.printResult("Jugador Gana")
            self.fin = True
            self.jugando = False
        elif result == self.miJugador * -1:
            self.juego.printResult("Oponente Gana")
            self.fin = True
            self.jugando = False
        elif self.jugadas >= self.juego.cellCount:
            self.jugando = False
            self.juego.printResult("Empate")
        else:
            pygame.display.update()
############################################ EL JUEGO ###############################################   
  
class Tateti (object): 
    # Velocidad de Barrido
    SPEED = 250
    
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
    
    CROSS_COLOR = BLUE
    CIRCLE_COLOR = RED
    
    CROSS_VALUE = -1
    CIRCLE_VALUE = 1 
            
    def __init__(self, screen):  
        # Tamanio de la actividad
        window = screen.get_size()
        # Tamanio del tablero. Cuadrado que ocupa la totalidad del lado mas pequenio del espacio disponible
        bWidth = min(window[0], window[1])
        bHeight = min(window[1], window[0])
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
        
        self.screen = screen
        self.screen.fill(self.BACK_COLOR)
        self.cells = [self.screen.fill(self.CELL_COLOR, (x+1, y+1, self.CELL_WIDTH-2, self.CELL_HEIGHT-2))
                       for y in range(0,self.board[1],self.CELL_HEIGHT) for x in range(0,self.board[0],self.CELL_WIDTH)]
        self.values = [0]*9    
        self.printBoard(self.values)
        # Changui para que estabilice el dibujo
        time.sleep(1)  
        pygame.display.flip()
    
    def registroJugada(self,jugada, miJugador):
    	# Invertimos el jugador para dibujar al otro
    	self.values[jugada] = miJugador * -1
    	self.printBoard(self.values)
    
    def setPointer(self,x):
        pygame.mouse.set_pos((x % (self.CELL_WIDTH * self.COL_COUNT), (self.CELL_HEIGHT / 2) + self.CELL_HEIGHT * int(x / (self.COL_COUNT * self.CELL_WIDTH))))
    
    def getScanningIndex(self):
        return int(pygame.mouse.get_pos()[0]/self.CELL_WIDTH) + self.COL_COUNT * int(pygame.mouse.get_pos()[1]/self.CELL_HEIGHT)
    
    def fillCross(self,index, stroke):
        cell = self.cells[index].inflate(-self.CELL_WIDTH/5, -self.CELL_WIDTH/5)
        padding = 5
        # Dibujamos la cruz
        tr = (cell.topright[0] - padding, cell.topright[1])
        bl = (cell.bottomleft[0] + padding, cell.bottomleft[1])
        pygame.draw.line(self.screen, self.CROSS_COLOR, tr, bl, stroke)
        tl = (cell.topleft[0] + padding, cell.topleft[1])
        br = (cell.bottomright[0] - padding, cell.bottomright[1])
        pygame.draw.line(self.screen, self.CROSS_COLOR, tl, br, stroke)
    
    def fillCircle(self,index):
        pygame.draw.ellipse(self.screen, self.CIRCLE_COLOR, self.cells[index].inflate(-self.BORDER, -self.BORDER), 14)
    
    def resetCell(self,index):
        self.screen.fill(self.CELL_COLOR, self.cells[index])
    
    def printBoard(self,values):
        for i in range(self.cellCount):
            self.resetCell(i)
            if values[i] == self.CROSS_VALUE:
                self.fillCross(i, 20)
            elif values[i] == self.CIRCLE_VALUE:
                self.fillCircle(i)
        
    def testBoard(self,player):
        lines = (self.values[0:3], self.values[3:6], self.values[6:9], self.values[0:7:3], self.values[1:8:3], self.values[2:9:3], self.values[0:9:4], self.values[2:7:2])
        #print lines
        
        if [player]*3 in lines:
            #Gano
            return player
        elif [-player]*3 in lines:
            #Pierdo
            return -player
        else:
            return 0
    
    def printResult(self,text):
        log.debug("Fin del Juego - " + text)
        txt = self.fontMgr.render(text + "!!!", 1, (0,0,0), (240,240,255))
        rect = txt.get_rect()
        rect.center = self.board[0]/2, self.board[1]/2
        self.screen.blit(txt,rect)
        pygame.display.update()
    
    def playMe(self,me):
        # Arranca seleccionada la celda de arriba a la izquierda
        scanningIndex = -1
        for i in range(self.cellCount * self.CELL_WIDTH):
            self.clock.tick(self.SPEED)
            # (Re)posicionamos el cursor en la ventana, dando sensacion de barrido
            self.setPointer(i) 
            tmpIndex = self.getScanningIndex()
            if not (scanningIndex == tmpIndex):
                # Hubo cambio en la opcion actualmente seleccionable. Hay que actualizar el estado de la escena
                scanningIndex = tmpIndex
            # Capturamos un clic
            for ev in pygame.event.get():
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    # Clic derecho termina
                    if ev.button == 3:
                        exit(0)
                    
                    # Clic izquierdo, tenemos que determinar la celda elegida en funcion 
                    # de la posicion del puntero al momento de clickear
                    selection = self.getScanningIndex()
                    # Registramos la opcion elegida y termina el ciclo
                    if self.values[selection] == 0:
                        self.values[selection] = me
                        self.printBoard(self.values)
                        return selection
                elif ev.type==pygame.KEYDOWN:
                    if ev.key==pygame.K_UP:
                        self.juego.SPEED=self.juego.SPEED+40
                    else:
                        self.juego.SPEED=self.juego.SPEED-40
        return self.playMe(me)
    
    #pygame.display.init()
    #log.debug("Iniciando Partida")
    #
    #
    #me  = CROSS_VALUE
    #for round in range(cellCount):
    #    #print "Jugando round", (round+1)
    #    if (round % 2) == 0:
    #        playMe(me)    
    #    else:
    #        playOpponent(me)
    #    printBoard(values)
    #    
    #    result = testBoard(me)
    #    if result == me:
    #        printResult("Jugador Gana")
    #        return
    #    elif result == -me:
    #        printResult("Oponente Gana")
    #        return
    #    else:
    #        pygame.display.update()
    #printResult("Empate")


if __name__ == "__main__":
    logging.basicConfig()
    main()
    

