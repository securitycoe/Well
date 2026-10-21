/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
#ifndef _UTIL_H_
 #define _UTIL_H_

#include "typedefs.h"
#include "Well_Defines.h"

extern ubyte g_ubytSKeyFlag;

int ed_int(int ist,int min,int max,DispString_t *str,DispString_t *unit,int inc,char *frt,
unsigned char cls,unsigned char point_pos);
float ed_float(float ist,float min,float max,DispString_t *str,DispString_t *unit,
float start_inc,float end_inc,const char *frt,unsigned char cls);

ubyte KeyMoveCtrl(ubyte new_key);
void WarnBeep(void);
void DisplayAndEnter(DispString_t *ptr);
extern void WarningDisplay(DispString_t *ptr1,DispString_t *ptr2);
void FloatFormatter(float x,const char *frmt,char *dst);
void ProgStatisticsMode(void);

void ShowXYCycles(char *str);
void ShowPistonCycles(char *str);
void ShowTotalVolume(char *str);
void ShowHeadChangingAtDevice(char *str);
void ShowDeviceChangingAtHead(char *str);
void ShowVersionNr(char *str);
void ShowTimeCode(char *str);
void ShowHardwareRevision(char *str);
void ShowDeviceNo(char *str);
void ShowHeadNo(char *str);
void ShowPistonNumber(char *str);

void BeepCtrl(uword time);

#endif // _UTIL_H_
