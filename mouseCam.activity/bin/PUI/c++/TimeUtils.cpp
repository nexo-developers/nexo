/*
 * TimeUtils.cpp
 *
 *  Created on: 22/02/2011
 * @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */

#include "TimeUtils.h"

TimeUtils::TimeUtils() {
	this->ss_prev = 0;
	this->sms_prev = 0;

}

TimeUtils::~TimeUtils() {
	// TODO Auto-generated destructor stub
}


double TimeUtils::utilTimer(void)
{
	gettimeofday( &time, NULL );
	s1 = time.tv_sec  - ss;
	s2 = time.tv_usec/1000 - sms;
	return (double)s1 + (double)s2 / 1000.0;
}

double TimeUtils::utilTimerGetElapsedTimeFromLast(void){
	gettimeofday( &time, NULL );
	s1 = time.tv_sec  - ss_prev;
	s2 = time.tv_usec/1000 - sms_prev;

	ss_prev = time.tv_sec;
	sms_prev = time.tv_usec/1000 ;

	/*printf("s1 = %f \n ",(double)s1);
	printf("s2 = %f \n ",(double)s2);
	printf("s2/1000.0 = %f \n ",(double)s2/ 1000.0);
	*/
	return (double)s1 + (double)s2 / 1000.0;
}

void  TimeUtils::utilTimerReset(void)
{
	gettimeofday( &time, NULL );
	ss  = time.tv_sec;
	sms = time.tv_usec / 1000;

	ss_prev = ss;
	sms_prev = sms;
}





