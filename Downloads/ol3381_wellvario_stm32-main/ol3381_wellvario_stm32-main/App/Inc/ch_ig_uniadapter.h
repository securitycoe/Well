/*******************************************************************
 *
 *	DESCRIPTION: Headerdatei
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
#ifndef _UNI_H_
#define _UNI_H_

 extern struct UNIAdj g_udtUniServoRawAdjPara;	// NullPunkt- und Verstärkungeingabewerte
 extern float g_fUNIServoActAngle;

 ubyte UNIServoAngle(float angle);
 uword UNIServoCalcAngleToTimeGrid(float angle,struct UNIAdj *ptr);
 uword UNIServoCheckPara(int gain,int offset);
 ubyte UNIServoInit(void);
 ubyte UNIServoAngleDrvToZero(void);
#endif
