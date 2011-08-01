/*
 * OpenCVCamera.h
 *
 *  Created on: 22/02/2011
 *      Author: seba
 */

#ifndef OPENCVCAMERA_H_
#define OPENCVCAMERA_H_

#include <stdio.h>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

class OpenCVCamera {

private:
	CvCapture* capture;
	IplImage* frame;

public:

	const static int READ_FROM_CAM = 0;
	const static int READ_FROM_FILE = 1;

	//Constructor
	OpenCVCamera();
	//Destructor
	~OpenCVCamera();

	void getInfo();
	/**
	 * tipo = 0 lee de la camara web
	 * tipo = 1 lee del archivo file
	 */
	int initCamera(int tipo, const char * file,int width,int height);

	IplImage* queryFrame();

	IplImage* getLastFrame();
};

#endif /* OPENCVCAMERA_H_ */
