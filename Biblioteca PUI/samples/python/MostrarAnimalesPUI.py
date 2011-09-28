#! /usr/bin/env python
# @author Sebastian Marichal
# 		   sebastian_marichal@hotmail.com
# 		   sebarocker@gmail.com
#
# Universidad de la Republica, Montevideo , Uruguay
#
import pygame, sys, os, time,gobject
from time import sleep
sys.path.append("../../python/")
import PuiModule
from pygame import *

#Configuracion del pipeline de Gstreamer
vconf = "v4l2src device=/dev/video0 ! videorate ! video/x-raw-yuv,width=320,height=240,framerate=5/1 ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
#Archivo de configuracion Multi Marker de Artoolkit. Define la posicion relativa de los marcadores
config_name = "Data/MostrarAnimalesPUI/mostrarAnimales_ModoSimple.patterns"
#Archivo de calibracion de la camara, siempre usamos el mismo y no ha presentado problemas
cparam_name    = "Data/camera_para.dat"

cant_historia = 4
usarAlgoritmoRapido = 1
mostrarDebugAR = 0

idObjetoPUIActual = -1

frameSize = (320,240)
mostrarVideo=1

err_deteccion = 0

thresh = 100

pui = None
dataImagen = None

running = True

#Variables de pantalla
size = (640,480)
screen = pygame.display.set_mode(size)
surface = pygame.display.get_surface()    

#
#(0,0)------(320,0)------------------------------(640,0)
#				|									#
# Salida Video	|									#
#				|			Imagenes Animales		#
#-----------(320,240)								#
#				|									#
#	    		|									#
#				|									#
#(480,0)---------------------------------------(640,480)
def actualizarPantalla():
	global idObjetoPUIActual,surface,pui
	print '**********actualizarPantalla!!**********\n'
	if not idObjetoPUIActual==-1:
			print '%%%%%%%%%%%%%DIBUJO!%%%%%%%%%'
			img_surface =pygame.image.load(pui.getImagenObjetoPUI(idObjetoPUIActual))
			if img_surface.get_width > 320 or img_surface.get_height > 320: 
				img_surface = pygame.transform.scale(img_surface,(320,320))
			screen.blit(img_surface, (320,0))
			#idem pero con el nombre
			pygame.draw.rect(display.get_surface(), (0,255,0), (320,320,320,160),0)
			nombre_surface = pygame.font.Font(None,80).render(pui.getNombreObjetoPUI(idObjetoPUIActual),1,(255,0,0))
			screen.blit(nombre_surface,(410,380))
			pygame.display.flip()
			
			sound = pui.getSonidoObjetoPUI(idObjetoPUIActual)
			print "****************************************Reproduccion del sonido " + sound
			pygame.mixer.music.load(pui.getSonidoObjetoPUI(idObjetoPUIActual))
			pygame.mixer.music.play()
			sleep(1.0)


def desplegarDataImage(imageData):
	global running,screen,frameSize,mostrarDebugAR,mostrarVideo
	pg_img = pygame.image.frombuffer(imageData,frameSize, "RGB")
	screen.blit(pg_img, (0,0))
	pygame.display.flip()
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
		    running = False
		elif event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				running = False
			elif event.key == K_v: #mostrar video
				mostrarVideo = 1 - mostrarVideo
			elif event.key == K_RETURN: #Proximo elemento
				proximoElemento()
				actualizarPantalla()
			elif event.key == K_t: #set threshold 
				print 'Threshold Actual = %s' % ( pui.getARThreshold() )
				user_input = input('*************** Ingrese nuevo Threshold ******************')
				print 'Threshold ingresado = %s' % (user_input)
				pui.setARThreshold(user_input)
			elif event.key == K_d: #set debug image
				mostrarDebugAR = 1 - mostrarDebugAR

def main():
	global screen,pui,running,cant_historia,usarAlgoritmoRapido,config_name,cparam_name,vconf,err_deteccion,idObjetoDetectado,idObjetoPUIActual

	pui = PuiModule.Pui()
	pui.setDebugMode(1)
	print '1\n'
	pui.initPuiConCapturaDeVideo(PuiModule.MODO_SIMPLE,None,config_name,cparam_name,vconf,usarAlgoritmoRapido,pui.NORMAL_SPEED_LOOP,1.0);
	print '2\n'	
	#Si queremos usar la funcionalidad de PUI para desplegar el video tenemos que abrir la ventana grafica
	#pui.abrirVentanaGrafica()

	pui.capStart()
	cantElementosCargados = pui.leerConfiguracionObjetosPUI("Data/MostrarAnimalesPUI/mostrarAnimales_ModoSimple.objetosPUI",10);
	print 'Se cargaron %s elementos\n' % cantElementosCargados

	pui.imprimirPropiedadesObjetosPUI();
	print '3\n'	
	
	#Habilito funcion de historico. Al detectar 5 selecciones seguidas recien se cuenta como una seleccion real
	pui.habilitarHistorico(True,5,-1)
	
	while running:
		dataImagen = pui.capturarPyImageAR()	#capturo imagen
		err_deteccion = pui.detectarMarcadoresSimple() #Detecto marcador
		if err_deteccion==PuiModule.DETECCION_CORRECTA:	#No hubo error al detectar los marcadores
			idObjetoDetectado = pui.getIdMarcadorSimpleDetectado()
			if idObjetoDetectado >= 0:
				print '&&&&&&&&&&& MARCADOR %d DETECTADO &&&&&&&&&&&&&&&'  %idObjetoDetectado
				idObjetoPUIActual = idObjetoDetectado;
				actualizarPantalla()
		if mostrarDebugAR:
			debugImg = pui.obtenerPyImagenDebugAR()
			if not debugImg== None:
				desplegarDataImage(debugImg.data)
		elif not dataImagen == None:	#si se pudo obtener el frame entonces continuo:
			desplegarDataImage(dataImagen.data)
		pui.capNext()
					
			
if __name__ == "__main__":
	pygame.init()
	gobject.threads_init()
	main()
	pygame.quit()
	pui.finish()
	print 'Fin del programa!!'
