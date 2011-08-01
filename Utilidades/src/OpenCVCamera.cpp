/*
 * OpenCVCamera.cpp
 *
 *  Created on: 22/02/2011
 *      Author: seba
 */

#include "OpenCVCamera.h"


	//Constructor
	OpenCVCamera::OpenCVCamera(){
		printf("OpenCVCamera construido\n");
	}

	//Destructor
	OpenCVCamera::~OpenCVCamera(){
		if(capture!=NULL){
			cvReleaseCapture(&capture);
		}
		if(frame!=NULL){
			cvReleaseImage(&frame);
		}
	}

	void OpenCVCamera::getInfo(){
		printf("OpenCVCamera es una utilidad para leer el video desde la camara web o desde un archivo\n");
	}
	/**
	 * tipo = 0 lee de la camara web
	 * tipo = 1 lee del archivo file
	 */
	int OpenCVCamera::initCamera(int tipo, const char * file,int width,int height){
		if(tipo==READ_FROM_CAM){
			capture = cvCreateCameraCapture(0);
			if(capture==NULL)
				return 0;
			cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,320);
			cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,240);
			/*	NO FUNCIONA SETEAR EL FPS
			 * cvSetCaptureProperty(capture, CV_CAP_PROP_FPS,5);
			 * */
		}else if(tipo==READ_FROM_FILE){
			printf("1\n");
			capture = cvCreateFileCapture(file);
			printf("2\n");
			if(capture==NULL){
				printf("3\n");
				return 0;
			}
			printf("4\n");
			cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,320);
			cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,240);
		}

		return 1;
	}

	IplImage* OpenCVCamera::queryFrame(){
		frame = cvQueryFrame(capture);
		return frame;
	}

	IplImage* OpenCVCamera::getLastFrame(){
		return frame;
	}

