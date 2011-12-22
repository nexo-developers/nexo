#! /usr/bin/env python
"""Skeleton project file mainloop for new OLPCGames users"""
import olpcgames, pygame, logging 
#from olpcgames import pausescreen
import olpcgames.mesh as mesh
import time
from ppt import PPT

log = logging.getLogger('PPT run')
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
        self.fin = False
        self.handleAmigo = None
        # Jugadas realizadas
        self.jugadas = 0
    
        self.size = (800,600)
        if olpcgames.ACTIVITY:
            self.size = olpcgames.ACTIVITY.game_size
        self.screen = pygame.display.set_mode(self.size)
        
        # inicio el juego para que se vea algo
        self.juego = PPT(self.screen)
        
        self.clock = pygame.time.Clock()
        
        self.timereventid = pygame.USEREVENT + 1

    
    def run(self):
        running = True
        while running:
            milliseconds = self.clock.tick(25) # maximum number of frames per second
            
            # Procesamos los eventos pendientes
            # Estos eventos son los que se disparan mientras el oponente esta haciendo su jugada
            events = pygame.event.get() 
            if events:
                for event in events:
#                    log.debug("Event: %s", event)
                    if event.type == pygame.QUIT:
                        running = False
                    elif event.type == pygame.KEYDOWN:
                        if event.key == pygame.K_ESCAPE:
                            running = False
                        elif self.jugando:
                            if event.key == pygame.K_r:                   
                                log.debug("PRESIONO Roca")            
                                self.juego.registroJugada(self.juego.Roca, self.juego.Yo)
                                mesh.send_to(self.handleAmigo, "registroJugada:" + str(self.juego.Roca))
                                self.evaluarJuego()
                            elif event.key == pygame.K_p:                   
                                log.debug("PRESIONO Papel")            
                                self.juego.registroJugada(self.juego.Papel, self.juego.Yo)
                                mesh.send_to(self.handleAmigo, "registroJugada:" + str(self.juego.Papel))
                                self.evaluarJuego()
                            elif event.key == pygame.K_t:                   
                                log.debug("PRESIONO tijera")            
                                self.juego.registroJugada(self.juego.Tijera, self.juego.Yo)
                                mesh.send_to(self.handleAmigo, "registroJugada:" + str(self.juego.Tijera))
                                self.evaluarJuego()
                    elif event.type == pygame.MOUSEBUTTONDOWN:
                        if event.button == 1:
                            # Si no estaba jugando, conecto y comienzo
                            if self.estoyConectado and self.hayAmigo and not self.jugando:
                                 # inicio juego
                                self.iniciarJuego(True)
                    elif event.type==mesh.CONNECT :
                        log.debug("CONNECT")
                        self.estoyConectado = True
                    elif event.type==mesh.PARTICIPANT_ADD:
                        log.debug("PARTICIPANT_ADD")
                        # Si no soy yo
                        if event.handle <> mesh.my_handle():
                            self.hayAmigo = True
                            self.handleAmigo = event.handle
                            
                            # Si soy el servidor comienzo el primer juego
                            if mesh.is_initiating():
                                self.iniciarJuego(True)
                    elif event.type==mesh.PARTICIPANT_REMOVE:
                        log.debug("PARTICIPANT_REMOVE")
                    elif event.type==mesh.MESSAGE_MULTI:
                        log.debug("MESSAGE_MULTI")
                    elif event.type==mesh.MESSAGE_UNI:
                        log.debug("MESSAGE_UNI: " + event.content)
                        toks = event.content.split(':')
                        if toks[0] == 'comenzarJuego':
                             # inicio juego
                            self.iniciarJuego(False)
                        elif toks[0] == 'registroJugada':
                            jugada = int(toks[1])
                            self.juego.registroJugada(jugada, self.juego.Oponente)
                            self.evaluarJuego()
                            #juego.printText("Empate")
                            #Fin chequeo de ganador
                    elif event.type==self.timereventid:
                        pygame.time.set_timer(self.timereventid, 0)
                        self.evaluarJuegoFin()
                        

                pygame.display.flip()
          
            
    def iniciarJuego(self, iniciador):
        self.juego.printText("Iniciando nuevo juego...")
        log.debug("Iniciando juego")
        # inicio juego
        self.juego = PPT(self.screen)       
        self.mostrarResultadoGlobal()
        self.jugando = True
        
        if iniciador:
            # Envio mensaje
            mesh.send_to(self.handleAmigo, "comenzarJuego")
    
    def mostrarResultadoGlobal(self):
         self.juego.printInfoPanel(self.ganados, self.perdidos)
    
    def evaluarJuego(self):
        # Mostrar opciones jugadas (si yo ya jugue)
        self.juego.printJugadas()
        
        result = self.juego.testBoard(self.juego.Yo)
        if result<>self.juego.noresuelto:
            # Timer para verlas y finalizar jugada
            pygame.time.set_timer(self.timereventid, 1000)
        
    def evaluarJuegoFin(self):
        result = self.juego.testBoard(self.juego.Yo)
        if result == self.juego.gane:
            self.juego.printText("Jugador Gana", 50)
            self.ganados += 1
            self.mostrarResultadoGlobal()
            self.fin = True
            self.jugando = False
        elif result == self.juego.perdi:
            self.juego.printText("Oponente Gana", 50)
            self.perdidos += 1
            self.mostrarResultadoGlobal()
            self.fin = True
            self.jugando = False
        elif result == self.juego.empate:
            self.jugando = False
            self.juego.printText("Empate", 50)
        #else:
        #    pygame.display.update()
  


if __name__ == "__main__":
    logging.basicConfig()
    main()
    

