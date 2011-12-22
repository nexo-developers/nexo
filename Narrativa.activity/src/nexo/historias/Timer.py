import time
import threading
import pygame



from pysrt import SubRipFile


class Timer(threading.Thread):            #subclass Thread
    def __init__(self, seconds):            #make it possible to pass the time in seconds that we want the timer to run
        self.runTime = seconds            #set our runTime in seconds
        threading.Thread.__init__(self)    #call the Thread's constructor
        self._surf_display= None
        self._stop = threading.Event()

        
        
     
    def run(self):                    #define what we want our Timer thread to do
        while 1:
            time.sleep(self.runTime)        #have it sleep for runTime seconds
            print "Buzzzz!! Time's up!"        #print a message when done
     
     
     
     
    def mostrarSubtitulos(self, ruta):
        # t = Timer(1)
        # t.start()
        subs = SubRipFile.open(ruta, encoding='iso-8859-1')
        
        print("Hay" ,subs.__len__()," subtitulos")



        cant_segs= pygame.time.get_ticks()/1000
        print "SEGUNDOS=", cant_segs
        linea= subs[0]
        
        if ((linea.start.seconds<=cant_segs) and (cant_segs<=linea.end.seconds)): 
            self.printTexto(linea.text)
        else:
            self.printTexto("")
     
     
     
    def setearDatos(self,_surf_display):
        self._surf_display= _surf_display
     
     
     
    def stop(self):
        self._stop.set()
  
     
    def printTexto(self, texto):
            font = pygame.font.Font(None,37)
            text = font.render(texto, True, (255,255, 255))
            # Create a rectangle
            textRect = text.get_rect()
            
            # Center the rectangle
            textRect.centerx = self._surf_display.get_rect().centerx
            textRect.centery = 480 - 30 
            #self._surf_display.get_rect().centery
            
            # Blit the text
            self._surf_display.blit(text, textRect)
            
            pygame.display.update()


        
