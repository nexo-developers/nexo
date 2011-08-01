#! /usr/bin/env python
import pygame, sys, os, time, gobject
import PuiModule
from pygame import *

vconf = "v4l2src device=/dev/video0 ! videorate ! video/x-raw-yuv,width=320,height=240,framerate=5/1 ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
count = 0
config_nameCalibrador = "Data/CalibradorColor/MarkersCalibradorColor.dat"
rangosEscalaColor =  PuiModule.RangosColorApuntador()
usarAlgoritmoRapido = 1
mostrarDebugAR = 0
aplicarMascara = 0
frameSize = (320,240)

h_bins = 30	#El histograma de HUE lo dividimos en 30 partes iguales
s_bins = 20	#El histograma de SATURATION lo dividimos en 20 partes iguales
v_bins = 20	#El histograma de BRIGHTNESS lo dividimos en 20 partes iguales

id_marcador =0
tipoMascara = PuiModule.TIPO_MASCARA_BINARIA
err_deteccion = 0

cantidad_muestras= 6
res_aplicar_mascara = 0
muestra = 0

aplicar_mascara=1
aplicar_nueva_mascara=1
tomar_muestra=0
thresh = 100
factorProporcionMaximoH = 10
porcentajeMinimoRuido = 7

pui = None
dataImagen = None

running = True
esperar = True

#Variables de pantalla
size = (640,480)
screen = pygame.display.set_mode(size)
surface = pygame.display.get_surface()    

def initRangosColor():
	global rangosEscalaColor
	rangosEscalaColor.hmin = 1	#Teoricamente seria correcto que hmin fuera 0. Lo ponemos en 1
	rangosEscalaColor.hmax = 360	#Porque los negros totales y los blancos totales son convertidos
	rangosEscalaColor.smin = 0	#con H=0 y eso lleva a que se introduzca ruido en el histograma
	rangosEscalaColor.smax = 100
	rangosEscalaColor.vmin = 0
	rangosEscalaColor.vmax = 100

def desplegarRangosColor(rangos):
	print '--------Rangos de Color--------\n'
	print 'Hue: 		['+ str(rangos.hmin)+','+str(rangos.hmax)+']\n'
	print 'Saturation: ['+str(rangos.smin)+','+str(rangos.smax)+']\n'
	print 'Brightness: ['+str(rangos.vmin)+','+str(rangos.vmax)+']\n'
	print '--------------------------------\n'

def desplegarDataImage(imageData):
	global esperar,running,screen,frameSize,aplicar_mascara,aplicar_nueva_mascara,tipoMascara,pui,mostrarDebugAR,tomar_muestra
	print '14'
	pg_img = pygame.image.frombuffer(imageData,frameSize, "RGB")
	print '15'	
	screen.blit(pg_img, (0,0))
	pygame.display.flip()
    
	print '16'
	for event in pygame.event.get():
		if event.type == pygame.QUIT:
			running = False
			esperar = False
		elif event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				running = False
				esperar=False
			elif event.key == K_n: #nueva mascara
				aplicar_nueva_mascara = 1 - aplicar_nueva_mascara
				aplicar_mascara = 1
			elif event.key == K_a: #aplicar mascara
				aplicar_mascara = 1 - aplicar_mascara
			elif event.key == K_u: #tipo de mascara
				if tipoMascara==PuiModule.TIPO_MASCARA_BINARIA :
					tipoMascara=PuiModule.TIPO_MASCARA_ATENUANTE
					print 'Usando mascara ATENUANTE!\n'
				else:
					tipoMascara=PuiModule.TIPO_MASCARA_BINARIA
					print 'Usando mascara BINARIA!\n'
			elif event.key == K_t: #set threshold			
				print '*** %s (frame/sec)\n' % (pui.getFPS())
				print 'Threshold Actual = %s' % (pui.getARThreshold())
				user_input = input('*************** Ingrese nuevo Threshold ******************')
				print 'Threshold ingresado = %s' % (user_input)
				pui.setARThreshold(user_input)
			elif event.key == K_d: #set debug image
				mostrarDebugAR = 1 - mostrarDebugAR
				pui.desplegarImagenAR(mostrarDebugAR)
			elif event.key == K_1:
				pui.setAchicarRadioMascaraCircular( pui.getAchicarRadioMascaraCircular()+3 )
				aplicar_nueva_mascara=1
				aplicar_mascara=1
			elif event.key == K_2:
				pui.setAchicarRadioMascaraCircular( pui.getAchicarRadioMascaraCircular()-3 )
				aplicar_nueva_mascara=1
				aplicar_mascara=1
			elif event.key == K_RETURN:
				tomar_muestra = 1
				aplicar_nueva_mascara = 1 - aplicar_nueva_mascara
				aplicar_mascara = 1
		else:
			print 'Evento desconocido!'



def calibrar():
	global tomar_muestra,pui, id_marcador, muestra, aplicar_mascara, aplicar_nueva_mascara, tipoMascara, mostrarDebugAR
	
	print '7\n'
	if tomar_muestra:
		print 'Tomo la muestra!!!!!\n'
		err_tomarMuestra = pui.tomarMuestraApuntador()
		if err_tomarMuestra==PuiModule.ERROR_TOMAR_MUESTRA_FORMA_NO_IMPLEMENTADA:
			sys.stderr.write("Error, no se pudo tomar muestra. Forma no implementada\n")
		elif err_tomarMuestra==PuiModule.ERROR_TOMAR_MUESTRA_OK:
			print 'Muestra tomada OK\n'
		
		tomar_muestra=0
		id_marcador+=1 
		muestra+=1
	else:
		print '8\n'
		dataImagen = pui.capturarPyImageAR()
		print '9\n'		
		if aplicar_mascara:
			if aplicar_nueva_mascara:	#Calculo la nueva mascara y la aplico
				err_deteccion = pui.detectarYCalcularPosicionTodosMultiMarkers()
				if err_deteccion==PuiModule.DETECCION_CORRECTA:
					res_aplicar_mascara = pui.aplicarNuevaMascaraCalibrador(id_marcador,tipoMascara)
					print pui.getFPSAndResetCount()
					if res_aplicar_mascara==PuiModule.MASCARA_APLICADA:
						print 'Se aplico la mascara correctamente\n'
						aplicar_nueva_mascara = 0				
			else:			#No calculo nueva mascara pero la aplico
				pui.aplicarMascaraCalibrador(tipoMascara);
			
#		pui.desplegarImagenAR(mostrarDebugAR)
		if not dataImagen == None:
			desplegarDataImage(dataImagen.data)
		pui.capNext()
	


def main():
	global pui, aplicar_mascara,aplicar_nueva_mascara,tomar_muestra,muestra,cantidad_muestras,jugar,factorProporcionMaximoH,porcentajeMinimoRuido,running,esperar

	aplicar_mascara=1
	aplicar_nueva_mascara=1
	tomar_muestra = 0
	pui = PuiModule.Pui()
	initRangosColor()
	pui.getModuleInfo()
	print '1\n'

	pui.initPuiConCapturaDeVideo(PuiModule.MODO_MULTI_MARKERS_CALIBRADOR,"",config_nameCalibrador,"Data/camera_para.dat",vconf,usarAlgoritmoRapido,pui.NORMAL_SPEED_LOOP,1.0);
	#pui.initARConVideo(config_nameCalibrador,"Data/camera_para.dat",vconf,usarAlgoritmoRapido,pui.NORMAL_SPEED_LOOP,1.0)	
	print '2\n'	
	pui.abrirVentanaGrafica()

	pui.capStart()
	pui.initPropiedadesCalibracionSoloApuntador(PuiModule.FORMA_APUNTADOR_CIRCULO,1,360,0,100,0,100,h_bins,s_bins,v_bins,1)
	print '3\n'	
	pui.setDebugCalibracion(False)
	print '4\n'
	pui.capStart()
	print '5\n'

	while running:
		if muestra<cantidad_muestras :
			print '6\n'
			calibrar()
		else :
			running = False

	if esperar:
		print 'se tomaron todas las muestras\n se calculan los resultados....\n'
		pui.calcularResultadosCalibracionApuntador(factorProporcionMaximoH,porcentajeMinimoRuido)
		rangosResultado = PuiModule.RangosColorApuntador()
		rangosResultado = pui.getResultadosRangosColor()

		desplegarRangosColor(rangosResultado)
	    
		print 'Guardo resultados en el archivo Data/CalibradorColor/calibracionColorPy.con\n'
		pui.guardarSoloRangosColorApuntador("Data/CalibradorColor/calibracionColorPy.con")
		#Parametros de la funcion guardarCalibracionApuntador:  nombre archivo,area minima, factor proporcion unico para todos los marcadores,erode,dilate,smooth,usar enclosing circle
		pui.guardarCalibracionApuntador("Data/CalibradorColor/calibracionApuntadorCompletaPy.con",300,1.0,1,1,1,1);
		print 'fin de la calibracion'
		surfaceMsgFinal1 = pygame.font.Font(None,25).render('Fin de la calibracion',0,(255,0,0))
		surfaceMsgFinal2 = pygame.font.Font(None,25).render('Los resultados se han guardado en los archivos:',0,(255,0,0))
		surfaceMsgFinal3 = pygame.font.Font(None,25).render('Data/CalibradorColor/calibracionColorPy.con',0,(255,0,0))
		surfaceMsgFinal4 = pygame.font.Font(None,25).render('Data/CalibradorColor/calibracionApuntadorCompletaPy.con',0,(255,0,0))
		surfaceMsgFinal5 = pygame.font.Font(None,40).render('Presione ESC para salir',0,(255,0,0))
		screen.blit(surfaceMsgFinal1,(40,320))
		screen.blit(surfaceMsgFinal2,(40,340))
		screen.blit(surfaceMsgFinal3,(40,360))
		screen.blit(surfaceMsgFinal4,(40,380))
		screen.blit(surfaceMsgFinal5,(40,410))
		pygame.display.flip()
		
	while esperar:
		event =pygame.event.wait()
		if event.type == pygame.QUIT:
			esperar=False
		elif event.type == pygame.KEYDOWN:
			if event.key == pygame.K_ESCAPE:
				esperar = False
			
if __name__ == "__main__":
    pygame.init()
    gobject.threads_init()
    main()
    pygame.quit()
    pui.finish()
    print 'Fin del programa!!'
