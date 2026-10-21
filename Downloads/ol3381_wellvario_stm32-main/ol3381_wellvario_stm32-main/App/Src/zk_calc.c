/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions
 #include "zk_calc.h"
 #include "ch_igel.h"

/** local definitions **/

/** default settings **/
 #define MAX_FREQUENCY		10000	/*Max. Interruptfrequenz für die Schrittausgabe*/
 #define BASIC_FREQUENCY	1250000	/* Eingangsfrequenz für die Zähler 20MHz mit Vorteiler 16*/
 #define MICRO_STEP_REV		6400	/* Anzahl der Impulse pro Umdrehung	*/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

/** private functions **/


/* FUNCTION: 	CalcTimeTable
   PARAMETERS:	Zeiger auf Tabelle,Startdrehzahl,Enddrehzahl,Anzahl der Schritte
   DESCRIPTION:	Berechnung der Schrittmotorkurve
   RETURNS:		Error: 0,ansonsten Gesamtschrittzahl der Anlaufkurve
*/

int CalcTimeTable( struct DRV_ZK *zk_ptr,float start_rev,float end_rev,int count){		//zk_array_gen
	float diff_rev,act_rev,frequency;
	float zk_temp,anlauf_zeit;
	int i,step_sum;
	ubyte step_width;
	anlauf_zeit = 0;
	start_rev /= 60;
	end_rev /= 60; /* Umrechnung von  rpmin in  rps*/
	step_width = 1;
	step_sum = 0;
	diff_rev = (end_rev-start_rev)/count;
	zk_temp = MICRO_STEP_REV/step_width*start_rev;
	if(zk_temp > (float)MAX_FREQUENCY) return 0;  /*Fehler,da Schrittweite mit 1 beginnen muß */

	//vtzk0 = low_time_calc(step_width,start_rev);
	for (i = 0;i<count+1;i++){
		act_rev = diff_rev*i+start_rev;
		WDO();
		while((frequency = MICRO_STEP_REV/step_width*act_rev) > MAX_FREQUENCY){
			WDO();
			step_width++;
		}
//		if(step_width > 32) return 0;
		if(step_width > 254) return 0;

/*		while((zk_temp = BASIC_FREQUENCY/frequency/vtzk0+0.5) > 65535) return 0;
		{
			WDO();
			if(++vtzk0 > 256) return 0;
		} 																		// Fehler,ZK kann nicht berechnet werden!
*/

		if((zk_temp = BASIC_FREQUENCY/frequency /*+ 0.5*/) > 65535) return 0; 	// Fehler,ZK kann nicht berechnet werden!
		step_sum += step_width;
		zk_ptr->step_width	 = step_width;
		zk_ptr->zk = (uword)(65536-zk_temp);
		anlauf_zeit += 1/frequency; /* Test */
		if(g_bTestFlag){
			printf("rpm:%4.1f ",act_rev*60);
			printf("freq:%4.1f ",frequency );
			printf("stepw:%u ",(uword)step_width);
			printf("zk:%u\r",(uword)zk_temp);
			fflush(stdout);
		}
		zk_ptr++;
	}
	if(g_bTestFlag)	printf("Anlaufzeit [ms]:%f\r",anlauf_zeit);
	return step_sum;

/*
	if(g_bTestFlag){
		fprintf(HOST,"accelaration: steps %u way %d [1/10 mm]\r",step_sum,(int)((float)step_sum/gear_ratio_z_act));
		printf("Anlaufzeit:%d\r",(int)(anlauf_zeit*1000));
	}
*/
}
