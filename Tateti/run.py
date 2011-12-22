#! /usr/bin/env python
"""Skeleton project file mainloop for new OLPCGames users"""
import olpcgames, pygame, logging 
#from olpcgames import pausescreen
import olpcgames.mesh as mesh
import time
from tateti import Tateti

log = logging.getLogger('TatetiMesh run')
log.setLevel(logging.DEBUG)

def main():
    protocolo = Protocolo()
    protocolo.run()

class Protocolo():
    ganados = 0
    perdidos = 0
    
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
            
            # Procesamos los eventos pendientes
            # Estos eventos son los que se disparan mientras el oponente esta haciendo su jugada
            events = pygame.event.get() 
            if events:
                for event in events:
                    log.debug("Event: %s", event)
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
                            #self.miJugador = 1 # Cruz
                            self.miJugador = self.juego.X
                        else:
                            #self.miJugador = -1 # Circulo
                            self.miJugador = self.juego.O
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
                            #juego.printText("Empate")
                            #Fin chequeo de ganador

                pygame.display.flip()
                
            # Tomamos acciones
                
            if self.jugando and self.miTurno:
                jugada = self.juego.playMe(self.miJugador)
                self.miTurno = False
                self.jugadas += 1
                mesh.send_to(self.handleAmigo, "registroJugada:" + str(jugada))
                self.evaluarJuego()
          
            
    def iniciarJuego(self, iniciador, miTurno):
        self.juego.printText("Iniciando nuevo juego...")
        log.debug("Iniciando juego")
        # inicio juego
        self.juego = Tateti(self.screen)       
        self.mostrarResultadoGlobal()
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
    
    def mostrarResultadoGlobal(self):
        if self.miJugador == self.juego.O:
            self.juego.printInfoPanel(self.ganados, self.perdidos)
        else:
            self.juego.printInfoPanel(self.perdidos, self.ganados)
    
    def evaluarJuego(self):
        result = self.juego.testBoard(self.miJugador)
        if result == self.miJugador:
            self.juego.printText("Jugador Gana")
            self.ganados += 1
            self.mostrarResultadoGlobal()
            self.fin = True
            self.jugando = False
        elif result == self.miJugador * -1:
            self.juego.printText("Oponente Gana")
            self.perdidos += 1
            self.mostrarResultadoGlobal()
            self.fin = True
            self.jugando = False
        elif self.jugadas >= self.juego.cellCount:
            self.jugando = False
            self.juego.printText("Empate")
        #else:
        #    pygame.display.update()
  


if __name__ == "__main__":
    logging.basicConfig()
    main()
    

