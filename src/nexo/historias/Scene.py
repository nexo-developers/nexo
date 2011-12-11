import pygame
from nexo.historias.Character import Character


class Scene(object):
    
    def __init__(self, screen, background_file, escena_opcion1, escena_opcion2, escena_opcion3 , audio, relato, optionsList):
        self.back_image = pygame.image.load(background_file)
        self.screen = screen
        self.escena_opcion1 = escena_opcion1
        self.escena_opcion2 = escena_opcion2
        self.escena_opcion3 = escena_opcion3
        self.audio = audio
        self.relato = relato
        self.relatoIsPlaying = False
        self.audioIsPlaying = False
        self.options = optionsList
        self.character_list = {}
        self._surf_display= None
        self.nroEscena= None
        self.timer= None
        # Lista con los id de los "personajes" (opciones a sacar)
        # La idea es que al momento de elegir una opcion se cargue en esta variable el personaje a sacar
        # Y que al momento de dibujar la escena se filtr los personajes que se encuentren aqui
        # 11-12-2011 No es necesario, lo manejamos en el xml
        #self.sacarPersonajes = []; 

    def setearDatos(self,_surf_display, nroEscena):
        self._surf_display= _surf_display
        self.nroEscena= nroEscena
        


    def addCharacter(self, character):
        print "agregando personaje " + character.id
        self.character_list[character.id] = character
        
        
    def draw(self):
        rect = self.back_image.get_rect()
        rect.topleft = (0,0)
        self.screen.blit(self.back_image, rect)
        #print "dibujando escena con " + str(len(self.character_list)) + " personajes"
        #conjDePersonajes = set(self.sacarPersonajes)
        for i in self.character_list:
            #if i not in conjDePersonajes:
                self.screen.blit(self.character_list[i].image, self.character_list[i].rect)
        pygame.display.flip()
        
        
    def playAudioFondo(self):        
        # CARGAMOS SONIDO DE FONDO
        if not self.audioIsPlaying:            
            self.audio = pygame.mixer.Sound(self.audio)
            self.audio.stop()
            self.audio.set_volume(0.5)
            self.audioIsPlaying = True
            self.audio.play()


    def playRelato(self):
        """
        Reproduce el audio de fondo de la escena, si la escena ya esta reproduciendo, no hace nada
        """
      
        print "Reproducir relato de escena"
        # CARGAMOS SONIDO DEL RELATO
        if not self.relatoIsPlaying:            
            pygame.mixer.music.load(self.relato)
            self.relatoIsPlaying = True
            pygame.mixer.music.play()
            
                        
            # Evento de fin de relato disparado, ahi muestra OPCIONES
            pygame.mixer.music.set_endevent(pygame.USEREVENT)
          

    def drawOptions(self):
        #self.draw()
        toAdd = Character("sub", "../../../resources/generales/opciones.png", 0, 485)
        self.addCharacter(toAdd)
        self.draw()          # reimprime la escena
        print "Dibujando opciones " + str(len(self.options))
        for opt in self.options:
            self.screen.blit(opt.image, opt.rect)
            print "dibujando opcion"
        pygame.display.flip()
        

    def executeOption(self, optionId):
        pass
        