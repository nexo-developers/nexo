/*
 * TimeUtils.h
 *
 *  Created on: 22/02/2011
 *      Author: seba
 */

#ifndef TIMEUTILS_H_
#define TIMEUTILS_H_

#include <sys/time.h>
#include <stdio.h>
class TimeUtils {

private:
	int      ss, sms;
	int      ss_prev, sms_prev;
	int                s1, s2;
	struct timeval     time;

public:
	TimeUtils();
	virtual ~TimeUtils();

	/*Retorna el tiempo en segundos trasncurrido desde la
	 * ultima vez que se llamo a utilTimerReset*/
	double utilTimer(void);

	/**
	 * Retorna el tiempo en segundos trasncurrido desde la
	 * ultima vez que se invoco a esta misma
	 * operacion
	 */
	double utilTimerGetElapsedTimeFromLast(void);

	/**
	 * Vuelve la cuenta de tiempo a 0
	 */
	void utilTimerReset(void);

};

#endif /* TIMEUTILS_H_ */
