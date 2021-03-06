#! /usr/bin/env python
# @author Sebastian Marichal
# 		   sebastian_marichal@hotmail.com
# 		   sebarocker@gmail.com
#
# Universidad de la Republica, Montevideo , Uruguay
#
import pygame, sys, os, time,gobject
sys.path.append("../../python/")
import PuiModule
from pygame import *
from pychecker.msgs import CATCH_BAD_EXCEPTION

#Configuracion del pipeline de Gstreamer
vconf = "v4l2src device=/dev/video0 ! videorate ! video/x-raw-yuv,width=320,height=240,framerate=5/1 ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
#Archivo de configuracion Multi Marker de Artoolkit. Define la posicion relativa de los marcadores
config_multi_markers = "letras.dat"
#Archivo de calibracion de la camara, siempre usamos el mismo y no ha presentado problemas
cparam_name    = "../../bin/Data/camera_para.dat"
#Archivo de configuracion para la deteccion del apuntador. Este archivo fue generado mediante la utilidad CalibradorColor
calibracion_apuntador = "../../bin/Data/CalibradorColor/calibracionApuntadorCompletaPy.con"

cant_historia = 4
cantElementosCargados = -1
usarAlgoritmoRapido = 1
mostrarDebugAR = 0
idObjetoPUISeleccionado = -1
idObjetoPUIActual = -1
correspondenciaCorrecta =False

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
#(0,0)------(320,0)------------------------------(640,0)#
# Salida Video	|					#
#		|	Imagenes Animales		#
#-----------(320,240)					#
#		|					#
#	Radar	|					#
#		|					#
#(480,0)---------------------------------------(640,480)
def actualizarPantalla():
	global idObjetoPUIActual,surface,pui,correspondenciaCorrecta
	print '**********actualizarPantalla!!**********\n'
	if not idObjetoPUIActual==-1:
			try:
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
				if correspondenciaCorrecta:
					sound = pui.getSonidoObjetoPUI(idObjetoPUIActual)
					print "****************************************Reproduccion del sonido " + sound
					pygame.mixer.music.load(pui.getSonidoObjetoPUI(idObjetoPUIActual))
					pygame.mixer.music.play()
			except Exception:
				print "Error actualizando pantalla"

def mostrarStatus():
	global screen,cantElementosCargados,pui,display
	cant_visibles = 0
	for i in range(cantElementosCargados):
		infoMarcador = pui.getInfoMultiMarcador(i)
		if infoMarcador.visible:
			cant_visibles=cant_visibles + 1
	pygame.draw.rect(display.get_surface(), (0,0,0), (40,400,80,60),0)
	
	status_surface = pygame.font.Font(None,80).render(str(cant_visibles),1,(255,0,0))
	screen.blit(status_surface,(50,400))
	pygame.display.flip()

def proximoElemento():
	global cantElementosCargados,idObjetoPUIActual
	if idObjetoPUIActual == cantElementosCargados -1:
		idObjetoPUIActual = 0
	else:
		idObjetoPUIActual = idObjetoPUIActual +1 


def desplegarDataImage(imageData):
	global running,screen,frameSize,mostrarDebugAR,mostrarVideo
	print '14'
	pg_img = pygame.image.frombuffer(imageData,frameSize, "RGB")
	print '15'	
	screen.blit(pg_img, (0,0))
	pygame.display.flip()
	print '16'
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
				print '*** %s (frame/sec)\n' % (pui.getFPS())
				print 'Threshold Actual = %s' % ( pui.getARThreshold() )
				user_input = input('*************** Ingrese nuevo Threshold ******************')
				print 'Threshold ingresado = %s' % (user_input)
				pui.setARThreshold(user_input)
			elif event.key == K_d: #set debug image
				mostrarDebugAR = 1 - mostrarDebugAR
				#pui.desplegarImagenAR(mostrarDebugAR)
			elif event.key == K_PLUS:
				pui.setHminApuntador(pui.getHminApuntador()+5)
				pui.setHmaxApuntador(pui.getHmaxApuntador()+5)
				print '*** Aumento hue: ['%pui.getHminApuntador()%','%pui.getHmaxApuntador()%']\n'
			elif event.key == K_MINUS: 
				pui.setHminApuntador(pui.getHminApuntador()-5)
				pui.setHmaxApuntador(pui.getHmaxApuntador()-5)
				print '*** Disminuyo hue: ['%pui.getHminApuntador()%','%pui.getHmaxApuntador()%']\n'
		else:
			print 'Evento desconocido!'

def main():
	global screen,pui,running,cantElementosCargados,cant_historia,usarAlgoritmoRapido,calibracion_apuntador,config_multi_markers,cparam_name,vconf,err_deteccion,idObjetoPUISeleccionado,idObjetoPUIActual,correspondenciaCorrecta

	#try:
	pui = PuiModule.Pui()
	#pui.setDebugMode(1)
	print '1\n'
	pui.initPuiConCapturaDeVideo(PuiModule.MODO_MULTI_MARKERS,calibracion_apuntador,config_multi_markers,cparam_name,vconf,usarAlgoritmoRapido,pui.NORMAL_SPEED_LOOP,1.0);
	
	print '2\n'	
	#Si queremos usar la funcionalidad de PUI para desplegar el video tenemos que abrir la ventana grafica
	#pui.abrirVentanaGrafica()

	pui.capStart()
	cantElementosCargados = pui.leerConfiguracionObjetosPUI("../../bin/Data/tocarAnimalesPUI/objetosPUI.config",10);
	print 'Se cargaron %s elementos\n' % cantElementosCargados

	pui.imprimirPropiedadesObjetosPUI();
	print '3\n'	
	
	idObjetoPUIActual=0
	actualizarPantalla()
	status_surface = pygame.font.Font(None,40).render('-Marcadores Visibles-',1,(255,0,0))
	screen.blit(status_surface,(20,350))
	pygame.display.flip()
	
	while running:
		dataImagen = pui.capturarPyImageAR()	#capturo imagen
		err_deteccion = pui.detectarYCalcularPosicionTodosMultiMarkers() #Detecto los marcadores y calculo las posiciones
		if err_deteccion==PuiModule.DETECCION_CORRECTA:	#No hubo error al detectar los marcadores
			idObjetoPUISeleccionado = pui.detectarSeleccion()
			if idObjetoPUISeleccionado >= 0:
				print '&&&&&&&&&&& COLISION CON MARCADOR %d &&&&&&&&&&&&&&&'  %idObjetoPUISeleccionado
				#En este caso se corrobora que el objeto PUI seleccinoado corresponda con el objeto PUI representado en 
				#pantalla. En caso de que se utilicen objetos relacionados se puede preguntar por la relacion entre ellos
				#de la siguiente manera pui.esRelacionCorrecta(idObjetoPUIActual,idObjetoPUISeleccionado)
				correspondenciaCorrecta = idObjetoPUISeleccionado==idObjetoPUIActual
				actualizarPantalla()
		#A continuacion se muestran los diferentes codigos de error que pueden ser devueltos por PUI
		#elif err_deteccion == PuiModule.ERROR_NINGUN_MARCADOR_DETECTADO:
			#no se detecto ningun marcador, eventualmente se podria alerta al usuario
			# si se reitar esta situacion varias iteraciones seguidas
			#print 'no se detectan marcadores...\n'
		#elif err_deteccion == PuiModule.ERROR_DETECTANDO_MARCADOERS:
			#error en el algoritmo de Artoolkit al detectar marcador....
		#elif err_deteccion == PuiModule.ERROR_CALCULANDO_MATRIZ_TRANSFORMACION:
			#error en el algoritmo de Artoolkit al calcular la matriz de transformacion
			#que nos da la posicion de cada marcador. Este codigo de error es muy comun
		#elif err_deteccion == PuiModule.ERROR_IMAGEN_NULL:
			#Por algun motivo la imagen es NULL, no deberia pasar nunca...
		#else:
			#error desconocido
		if not dataImagen == None:	#si se pudo obtener el frame entonces continuo
			desplegarDataImage(dataImagen.data)
			print '15'
			mostrarStatus()
			pui.capNext()
	#except Exception:
	#	print "Excepcion recibida"
					
			
if __name__ == "__main__":
	pygame.init()
	gobject.threads_init()
	main()
	pygame.quit()
	pui.finish()
	print 'Fin del programa!!'
