/*******************************************************************
 *
 *	DESCRIPTION:	Ansteuerung und Abgleich des Servos zur Steuerung des Uni-Adapters
 *
 *	AUTHOR:		Baumann
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions
 #include "ch_ig_uniadapter.h"
 #include "ch_igel.h"
 #include "ch_ig_eeprom.h"
 #include "Well_Error.h"

 #define UNI_SERVO_MAX_ANGLE 50		// 20 in Grad
 #define UNI_SERVO_MIN_ANGLE -50	// in Grad
 #define UNI_SERVO_INIT_ANGLE 0		// in Grad
 #define UNI_SERVO_EXE_TIME 30		// in 10 ms

 #define UNI_SERVO_MAX_GAIN 20		// Grenzwerte für die Verstärkungskorrektur (Anstieg)
 #define UNI_SERVO_MIN_GAIN -20
 #define UNI_SERVO_MAX_OFFSET 20	// Grenzwerte für die Nullpunktkorrektur
 #define UNI_SERVO_MIN_OFFSET -20

 #define UNI_SERVO_NORM_GAIN (float)0.012 		// Anstieg [ms/Grad]
 #define UNI_SERVO_ZERO (float)1.520 	 		// Null-Punkt [ms]
// #define UNI_SERVO_TIME_GRID (float)3.2E-03	// [ms]	(221004 mku old pwm_clk:  312500 Hz)
 #define UNI_SERVO_TIME_GRID_NEW (float)1E-03	// [ms] (221004 mku new pwm_clk: 1000000 Hz)

/** internal functions **/

 struct UNIAdj g_udtUniServoRawAdjPara;	// NullPunkt- und Verstärkungeingabewerte
 float g_fUNIServoActAngle = 0;
 unsigned long ulngUniServoCycles;

/* FUNCTION: 	UNIServoAngle
   PARAMETERS:	Drehwinkel,Mittelstellung des Servos ist Null Grad
   DESCRIPTION: Weg in Zeitraster für die PWM umsetzen und ausgeben
   RETURNS: 	CB_READY / IERR_PARA */
ubyte UNIServoAngle(float angle){
	uword act_time_angle;
	if((angle > UNI_SERVO_MAX_ANGLE) || (angle < UNI_SERVO_MIN_ANGLE)) return IERR_PARA;

	act_time_angle = UNIServoCalcAngleToTimeGrid(angle,&g_udtUniServoRawAdjPara);
	UNIServoSetPWM(act_time_angle);

	if(g_fUNIServoActAngle != angle){
		ulngUniServoCycles++;
		Wait(UNI_SERVO_EXE_TIME);
	}
	g_fUNIServoActAngle = angle;

	return CB_READY;
}

/* FUNCTION: 	UNIServoCalcAngleToTimeGrid
   PARAMETERS:	Winkel,Anstiegskoeffizient,Nullpunktkoeffizient
   DESCRIPTION:	Berechnen der Winkel-Zeitfunktion für den Servo zur Stg. des Universaladapters
   RETURNS: 	Zeitraster für die PWM-Steuerung */
uword UNIServoCalcAngleToTimeGrid(float angle,struct UNIAdj *ptr){
	uword help;
	float f_gain,f_offset;
	// 201004 mku
	// ---------- Analyse ---------
	f_gain  = 	UNI_SERVO_NORM_GAIN +	(ptr->intUniGain	*0.0001);
	//			0.012 [ms/°]		+	Korrektur
	//			0.600/50
	// Der Uni-adapter Servo S3101 hat laut Händlerangaben aber 60°! UNI_SERVO_NORM_GAIN müsste daher
	//		->	0.600/60
	//		->	0.01 [°/ms] haben!
	// A C H T U N G ! Berechnung trotz der fehlerhaften Umrechnungskonstante NICHT korrigieren!
	// Anderfalls müssten sämliche zugehörige bisher einmessene und abgespeicherte Korrekturwerte ebenfalls korrigiert werden!
	f_offset  = 	UNI_SERVO_ZERO		+	(ptr->intUniOffset	*0.00075);	// Ausgleichsmöglichkeit +-10%
	//			1.520 [ms]			+	Korrektur

	// ---------- old ----------
// 	help  = 	(f_gain * angle +	f_offset) /	UNI_SERVO_TIME_GRID;
	//		(ms/degree)						(1 / pwm_clk)
	//		0.012 [ms/°]					0.0032 [ms]

	//  ---------- new ----------
	// Umrechnungskonstante nun UNI_SERVO_TIME_GRID_NEW (0.001),da der neue Timer mit 1000 kHz (alt: 312,5 kHz) Takt arbeitet
	help  = 	(f_gain	* angle	+ 	f_offset) / UNI_SERVO_TIME_GRID_NEW;

	if(g_bTestFlag){
		printf(" angle:%3.2f raw gain:%d gain:%2.5f raw offset:%d offset:%2.3f time cnt:%d\r",
			angle				// Sollwinkel
			,ptr->intUniGain	// Adapter spezifischer Korrekturwert Anstieg
			,f_gain				// Anstiegskoeffizient
			,ptr->intUniOffset	// Adapter spezifischer Korrekturwert Nullpunkt
			,f_offset			// Nullpunktkoeffizient
			,help);				// Stellwert Servo (Impulslänge)
		fflush(stdout);
	}
	return help;
}

/* FUNCTION: 	UNIServoCheckPara
   PARAMETERS:	Anstiegskoeffizient,Nullpunktkoeffizient
   DESCRIPTION:	Prüfung des Wertebereiches
   RETURNS: 	TRUE / FALSE */
uword UNIServoCheckPara(int gain,int offset){
  if( (gain	< UNI_SERVO_MIN_GAIN)
	||(gain	> UNI_SERVO_MAX_GAIN)
	||(offset < UNI_SERVO_MIN_OFFSET)
	||(offset > UNI_SERVO_MAX_OFFSET))
	return FALSE;
  return TRUE;
}

/* FUNCTION: 	UNIServoInit
   PARAMETERS:	---
   DESCRIPTION:	Setzen der Parameter für die Korrektur der Kennlinie des Servos
   RETURNS: 	CB_READY / IERR_EEPROM_READ */
ubyte UNIServoInit(void){
  return EEPROMReadUniAdapterPara(&g_udtUniServoRawAdjPara);
}

/* FUNCTION: 	UNIServoAngleDrvToZero
   PARAMETERS:	---
   DESCRIPTION:	Bewegen des Servos auf Null-Position
   RETURNS: 	CB_READY / IERR_PARA */
ubyte UNIServoAngleDrvToZero(void){
  return UNIServoAngle(UNI_SERVO_INIT_ANGLE);
}
