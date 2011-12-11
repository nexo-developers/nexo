'''
Created on Sep 22, 2011

@author: losangeles
'''
import  sys
import pygame
from pygame.locals import USEREVENT, KEYUP
from nexo.historias.Character import Character
from Scene import Scene
import xml.dom.minidom
from nexo.historias.Action import Action
from nexo.historias.Option import Option
from imprimir import imprimir



class Main(object):
    scenes = {}
    imprimir= imprimir() 
    anchoPantalla= 800
    altoPantalla= 600
        
    def __init__(self):
        self._running = True
        self._surf_display = None
        self.size = self.width, self.height = self.anchoPantalla, self.altoPantalla
        self.current_scene = "1"
        #self.clock = pygame.time.Clock()
        self.start = pygame.time.get_ticks()
        self.showOptions = False
        self.escenaDibujada = False
        self.optionsVisible = False
        self.character_sprites = {} #se llena al parsear los campos character del archivo
  



    def onInit(self):
        pygame.init() # inicia el 0 de los ticks de pygame.time
        pygame.mixer.init(frequency=22050, size=-16, channels=2, buffer=4096)
        self._surf_display = pygame.display.set_mode(self.size, pygame.HWSURFACE | pygame.DOUBLEBUF)
        self.parseCharacters("../../../resources/Scenes.xml")
        self.parseScenes    ("../../../resources/Scenes.xml")
        self._running = True
        
    # Ahora no se usa
#    def persId(self, opciones, idOpcion):
#        res=None
#        for opcion in opciones:
#            if opcion.id == idOpcion:
#                res = opcion.personajeId 
#            
#        return res

    def onEvent(self, event):
        if event.type == pygame.QUIT:
            self._running = False
        if event.type == USEREVENT:
            print "Evento de fin de relato disparado"
            self.showOptions = True

        
        if (event.type == KEYUP) and self.optionsVisible:
            if pygame.key.name(event.key) == "1":
                print "Se selecciono la opcion 1"
                #pers = self.scenes[self.current_scene].options[0].personajeId
                #pers = self.persId(self.scenes[self.current_scene].options,1)
                self.current_scene = self.scenes[self.current_scene].escena_opcion1
                #self.scenes[self.current_scene].sacarPersonajes.append(pers)
                self.scenes[self.current_scene].draw()
               

          
            elif pygame.key.name(event.key) == "2":
                print "Se selecciono la opcion 2"
                #pers = self.persId(self.scenes[self.current_scene].options,2)
                self.current_scene = self.scenes[self.current_scene].escena_opcion2
                #self.scenes[self.current_scene].sacarPersonajes.append(pers)
                self.scenes[self.current_scene].draw()
               
                
              
            elif pygame.key.name(event.key) == "3":
                print "Se selecciono la opcion 3"
                #pers = self.persId(self.scenes[self.current_scene].options,3)
                self.current_scene = self.scenes[self.current_scene].escena_opcion3
                #self.scenes[self.current_scene].sacarPersonajes.append(pers)
                self.scenes[self.current_scene].draw()
                #self.scenes[self.current_scene].playRelato()
               

        if (self.showOptions != True): 
            self.scenes[self.current_scene].setearDatos(self._surf_display, self.current_scene)
   
    
    def onLoop(self):
        pass
    
    
    def onRender(self):
        if not self.escenaDibujada:
            self.escenaDibujada = True
            self.scenes[self.current_scene].draw()
        if not self.scenes[self.current_scene].relatoIsPlaying:
            self.scenes[self.current_scene].playRelato() 
            self.imprimir.offset = pygame.time.get_ticks() # Obtengo el delay entre el inicio de los ticks (pygame.init) y el inicio del relato
            self.imprimir.dejarImprimir()     
        #if not self.scenes[self.current_scene].audioIsPlaying:
        #    self.scenes[self.current_scene].playAudioFondo()  
        if self.showOptions:
            self.showOptions = False
            self.optionsVisible = True
            self.scenes[self.current_scene].drawOptions()
           
        
        
        num = self.current_scene
        self.imprimir.mostrarSubtitulos(self.scenes[num],'../../../resources/escena '+num+'/Relato_Escena_'+num+'.srt')

                
    
    def onCleanUp(self):
        pygame.quit()
        sys.exit()
        
    def onExecute(self):
        self.onInit()
        pygame.display.set_caption('La Aventura de Charly Angel')
        while self._running:
            for event in pygame.event.get():
                self.onEvent(event)
            self.onLoop()
            self.onRender()
        self.onCleanUp()


    #Parsea la lista de personajes, se debe llamar antes que el parser de escenas porque sino este ultimo se cae
    def parseCharacters(self,filepath):
        dom = xml.dom.minidom.parse(filepath)
        charnodes = dom.getElementsByTagName("characters")[0]
        childs = charnodes.getElementsByTagName("character")
        for child in childs:
            charid = child.getAttribute("id")
            charsprite = child.getElementsByTagName("sprite")[0].getAttribute("value")
            self.character_sprites[charid] = charsprite

                        
    #Deberia validar que los personajes nombrados en las escenas esten declarados en la def de characters
    def parseScenes(self, storyPath):
        dom = xml.dom.minidom.parse(storyPath)
        scenes = dom.getElementsByTagName("scene")
        for x in scenes:
            idscene = x.getAttribute("id")
            fondo = x.getElementsByTagName("background")[0]
            background = fondo.getAttribute("value")
            if len(x.getElementsByTagName("escena_opcion1")) != 0:
                escena_opcion1 = x.getElementsByTagName("escena_opcion1")[0].getAttribute("value")
            if len(x.getElementsByTagName("escena_opcion2")) != 0:
                escena_opcion2 = x.getElementsByTagName("escena_opcion2")[0].getAttribute("value")
            if len(x.getElementsByTagName("escena_opcion3")) != 0:
                escena_opcion3 = x.getElementsByTagName("escena_opcion3")[0].getAttribute("value")
                 
            audio = x.getElementsByTagName("audio")[0].getAttribute("value")
            relato = x.getElementsByTagName("relato")[0].getAttribute("value")
            opciones = x.getElementsByTagName("option")
            parsedOptions = []
            for opcion in opciones:
                print "parsing option"
                optionId = opcion.getAttribute("id")
                optionImage = opcion.getElementsByTagName("image")[0].getAttribute("value")
                optionPosX = opcion.getElementsByTagName("position")[0].getAttribute("x")
                optionPosY = opcion.getElementsByTagName("position")[0].getAttribute("y")
                action_verb = opcion.getElementsByTagName("action")[0].getAttribute("verb")
                action_resource = opcion.getElementsByTagName("action")[0].getAttribute("resource")
                action_x = opcion.getElementsByTagName("action")[0].getAttribute("x")
                action_y = opcion.getElementsByTagName("action")[0].getAttribute("y")
                action_speed = opcion.getElementsByTagName("action")[0].getAttribute("x")
                personajeId = opcion.getElementsByTagName("character")[0].getAttribute("charid")
                act = Action(action_verb,action_resource,action_x,action_y,action_speed)

                newOpt = Option(optionId,optionImage,optionPosX, optionPosY,act,personajeId)
                parsedOptions.append(newOpt)
            scene = Scene(self._surf_display, background, escena_opcion1 , escena_opcion2, escena_opcion3, audio, relato, parsedOptions)
            characters_xml = x.getElementsByTagName("scene_character")
            print "personajes " + str(len(characters_xml))
            for charxml in characters_xml:
                charid = charxml.getAttribute("id")
                print "IdEscena= " + idscene + ". Personaje " + charid
                charx = charxml.getElementsByTagName("position")[0].getAttribute("x")
                chary = charxml.getElementsByTagName("position")[0].getAttribute("y")
                if charid != "guzman":
                    toAdd = Character(charid, self.character_sprites[charid], charx, chary)
                    scene.addCharacter(toAdd)
                    print "[Main] se agrego un personaje a la escena. Personaje " + charid

            print "se creo escena con" + str(len(parsedOptions)) + " opciones"
            self.scenes[idscene] = scene
            self.scenes[self.current_scene].setearDatos(self._surf_display, self.current_scene)
            self.imprimir.setearDatos(self._surf_display)
            
            
    
if __name__ == '__main__':
    main = Main()
    main.onExecute()
