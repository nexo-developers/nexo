

from pysrt import SubRipFile
import pygame
import re
from nexo.historias.Character import Character



class imprimir(object):
    tmp= 0
    imprimir= 1     
    escena= 0
    entrar=0
    text=[]
    tiempoActual=0
    tiempoCambio=0
    ok=0 # por defecto no permite impresiones
    offset=0 # uso esta variable para determinar el tiempo que hay de delay entre el pygame.init y el compenzo del relato
    
    
    def dejarImprimir(self):
        self.ok= 1
    
    
     
    def mostrarSubtitulos(self, escena, ruta):
            if (self.ok==1):
                
                self.escena= escena
                
                #subs = SubRipFile.open(ruta, encoding='iso-8859-1')
                subs = SubRipFile.open(ruta, encoding='UTF-8') # Con esta codificacion logramos ver los tildes
                
                #print("Hay" ,subs.__len__()," subtitulos")
                
         
##                if (self.tiempoCambio < pygame.time.get_ticks()/250):  # paso cuarto segundo   
##                    self.tiempoCambio = pygame.time.get_ticks()/250
#                if (self.tiempoCambio < pygame.time.get_ticks()):  # paso cuarto segundo   
#                    self.tiempoCambio = pygame.time.get_ticks()
#                    self.tiempoActual= self.tiempoActual+1 
                #print "SEGUNDOS=", cant_segs
                if (self.tmp== subs.__len__()): # cuando llega al final de los subtitulos
                    #self.tmp= subs.__len__()-1                
                    self.tmp= 0
                    self.ok= 0
                    #print("entro en tiempo " ,self.tiempoActual)
                    self.tiempoActual= 0
                   
               
                linea= subs[self.tmp]
                
#                inicio_Msg_min = linea.start.minutes
#                fin_Msg_min = linea.end.minutes
#                inicio_Msg_seg = linea.start.seconds
#                fin_Msg_seg = linea.end.seconds
#                inicio_Msg_mili = linea.start.milliseconds
#                fin_Msg_mili = linea.end.milliseconds
                #tiempo_ini = time(self.hours, self.minutes, self.seconds,self.milliseconds * 1000)
                #tiempo_ini = time(0, inicio_Msg_min, inicio_Msg_seg,inicio_Msg_mili)
                #tiempo_fin = time(0, fin_Msg_min, fin_Msg_seg,fin_Msg_mili)
                #print("tiempo_ini=", tiempo_ini)
                #print("tiempo_fin", tiempo_fin)
                
#                tics_ini = (inicio_Msg_min*60*1000)+(inicio_Msg_seg*1000)+inicio_Msg_mili
#                tics_fin = (fin_Msg_min*60*1000)+(fin_Msg_seg*1000)+fin_Msg_mili
                tics_ini = (linea.start.minutes*60*1000)+(linea.start.seconds*1000)+linea.start.milliseconds
                tics_fin = (linea.end.minutes*60*1000)+(linea.end.seconds*1000)+linea.end.milliseconds
                
                if ((tics_ini<=(pygame.time.get_ticks()-self.offset)) and ((pygame.time.get_ticks()-self.offset)<=tics_fin)):
                #if ((inicio_Msg_seg<=self.tiempoActual/4) and (self.tiempoActual/4<=fin_Msg_seg)): 
                    
                    if (self.imprimir==1):
                        self.escena.draw()          # reimprime la escena
                        self.printTexto(linea.text) # imprime mensaje
                        self.imprimir= 0
                        self.tmp= self.tmp+1
                        self.entrar= 1
                        
                else:
                    if (self.entrar==1):   
                        self.printTexto("")                                   
                        self.imprimir= 1                
                        self.entrar=0


     
    def setearDatos(self,_surf_display):
        self._surf_display= _surf_display
     
     
     
    def stop(self):
        self._stop.set()
  
     
    def printTexto(self, texto):
            
            # Seteo la fuente #
            font = pygame.font.Font(None,37)
            
            # Parto el texto en varios renglones#
            lista = re.split("\n",texto)
            # Obtengo la cantidad de renglones, en nuestro caso como mucho habran 2, el Split
            # genera un ultimo "renglon" vacio porque el texto cargado trae un fin de linea al 
            # final de subtitulo #
            cant= len(lista)
            
            if (lista[0]=="Guzman:"):                
                toAdd = Character("sub", "../../../resources/generales/guz.png", 0, 485)
                self.escena.addCharacter(toAdd)
                self.escena.draw()          # reimprime la escena
            
            if (lista[0]=="Charly:"):                
                toAdd = Character("sub", "../../../resources/generales/char.png", 0, 485)
                self.escena.addCharacter(toAdd)
                self.escena.draw()          # reimprime la escena
            
            # El loop lo hago hasta cant-1 porque el indice empieza en 0 y tengo que ir hasta el 
            # penultimo renglon. El rango no incluye el extremo final #
            
            if (cant>=2):
                
                for i in range(1,cant-1):
                    self.text.append(lista[i])
                    self.text[i-1] = font.render(lista[i],True,(0,0,255))
                    rectangulo = self.text[i-1].get_rect()
                    rectangulo.centerx = self._surf_display.get_rect().centerx+10
                    rectangulo.centery = 600-(40-((i-1)*25))-20
                    self._surf_display.blit(self.text[i-1],rectangulo)
                
            pygame.display.update()  
          
            
            