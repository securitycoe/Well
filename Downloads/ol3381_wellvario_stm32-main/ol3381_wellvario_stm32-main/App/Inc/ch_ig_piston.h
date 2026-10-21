/****************************************************************************/
// @Module
// @Filename	  ch_ig_piston.h
// @Project	   Ch_ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	  TASKING C166
//
// @Codegenerator DAvE 2-0-0
//
// @Description   This is the include header file for all other modules.
//
//----------------------------------------------------------------------------
// @Date		  23.08.01 07:57:54
//
//****************************************************************************

//****************************************************************************
// @Defines
//****************************************************************************

#ifndef _PISTON_H_
#define _PISTON_H_

 #include "head_piston_tab.h"	//

 #define SUCK 0		// Kennung für Saugen
 #define VALVE_ON 1	// Ventil für Hummingwell-Kopf


 #define PST_ZK_DIM 			600			// 600 Feldlänge für Anlaufkurve
 #define PST_DEF_START_RPM 		6			// Standardstartdrehzahl (Umdr/min)
 #define PST_DEF_END_RPM		300			// 300 Standardenddrehzahl (Umdr/min)
 #define PST_DEF_END_RPM_1536	35			// 300 Standardenddrehzahl (Umdr/min)
 #define PST_DEF_COUNTS 		600			// 600 Anlauflänge der SM-Kennlinie,muss kleiner als  HRZ_ZK_DIM sein
 #define PST_RPM_MAX 			500			// max. mögliche Drehzahl (Umdr/min)
 #define PST_RPM_MIN 			1			// min. mögliche Drehzahl (Umdr/min)
 #define PST_ACC_MAX 			PST_ZK_DIM	// max. mögliche Anlauflänge
 #define PST_ACC_MIN 			150			// min. mögliche Anlauflänge
 #define PST_TC_RPM				(float)PST_DEF_END_RPM * 0.6


	// interne Zustandsmeldungen im Kopf,nicht für die Schnittstelle zugelassen
 /* Berechnung des übersetztungsverhältnisses zum Volumen
	V[µl] = A[mm2] * h[mm];  mit h[mm] = 3200 Imp/mm   1Umdr == 6400Imp == 2mm
	h[mm] = V[mm3]/A[mm2];  mit 1mm = 3200Imp
	daraus folgt: h[Imp] = V[mm3] / A[mm2] *3200[Imp/mm]
 */

	// Anwendung im CyBi-Well und LID
 struct LIFE_PARA					// Parameter,die über die Batterie gestützt werden
 {
	 float PstCycleVolume;
	 float PstTotalVolume;  			// Volumen in ml über die gesamte Lebensdauer,wird beim Kopfwechsel im Selbigen gespeichert
	 unsigned long ulngPistonCycles; 	// Anzahl der Kolbenzyklen über die gesamte Lebensdauer,wird beim Kopfwechsel im Selbigen gespeichert
	 unsigned long ulngXYCycles;		// Anzahl der XY-Zyklen über die gesamte Lebensdauer
	 unsigned long writeCycles;			//Anzahl der Schreibzyklen in einen EEPROM
 };

extern const struct TIPS_PARA *g_pudtTipsPara;
extern const struct HEAD_DEV *g_pudtHeadDevPara;
extern struct HEAD_TYPE *g_pudtHeadType;
extern struct TIPS_PARA g_udtTipsParaFromASPW;
extern struct LIFE_PARA g_udtLifePara;
extern struct REV_PARA g_udtPstRevMan;
extern const struct REV_PARA g_udtPstRevDef;
extern const struct REV_PARA g_udtPstRevDef_1536;
extern bool g_bPstFirstSuck;
extern bool g_bFirstDisp;
extern ubyte g_ubytPstDrvState;
extern bool g_bPstDrvDir;
extern long g_lPstDrvSteps;
extern short g_sPstDrvTabWay;
extern struct DRV_ZK *g_pudtPstActZk,*g_pudtPstActEndZk;
extern struct DRV_ZK g_audtPstZk[];
extern float g_AddVolume;	// in µl
extern enum VOLUME_CORRECTION g_VolumeCorrection;


 void calc_zk_p(float start,float end,int anz);	/* Einstellung der Pumpe */
 ubyte Suck(float volume);		/* Ansaugen in µl */
 ubyte SuckWithOverstroke(float volume);
 ubyte Dispense(float volume,ubyte enable_abort);
/*
 ubyte FrankSuck(float volume);
 ubyte FrankSuckWithOverstroke(float volume);
 ubyte FrankDispense(float volume,ubyte enable_abort);
 ubyte FrankPstDrvCalibrate(void);
*/
 ubyte PstSpeedLimit(float speed);
 void PstDrvGetLimits(char *zk);
 void PstDrvGetPara(char *zk);
 void PstDrvSetCurrentToZero(void);
 ubyte PstInitZkMan(void);
 ubyte PstCheckRevManPara(void);
 ubyte PstCheckRevPara(const struct REV_PARA *rev);
 ubyte PstCalcDrvPara(const struct REV_PARA *rev);
 ubyte PstDrvToZero(void);
 ubyte PstDrvPreZeroPos(void);	// Suchen der Grobnull
 ubyte SuckWithoutTips(void);
	//ubyte ReadHeadNoAndTipsPara(void);
 ubyte ReadHeadNoAndSetDevPara(void);
 ubyte ReadMagazineTypeAndSetTipsPara(const struct HEAD_TYPE *ptr);
 void SetTipsParaToZero(void);
	//ubyte PstDrvCalForInternApply(void);

	// nur für Test,da interne Funktionen
	//ubyte PstReadMagState(void);
	//ubyte PstReadEndPos(ubyte dir);
 ubyte PstWatchHead(ubyte dir,ubyte flg_watch_magazine);
	//ubyte PstReadFineZeroPos(void);

 void PstDrvStart(struct DRV_ZK *ptr);
 void PstDrvAbort(void);
 void PstDrvSetHoldCurrent(void);
 ubyte PstDrv(ubyte dir,ubyte watch_magazine,ubyte abort,long steps);
 ubyte PstDrvCalibrate(void);
 ubyte PstReadUpperPos(void);
 ubyte PstReadLowerPos(void);
 ubyte PstCheckAndSetRevToMan(void);
 void TestShowHeadPara(struct HEAD_DEV *ptr);
 ubyte PstDrvLeavePreZeroPos(void);

 ubyte PstReadEccentricState(void);
 ubyte IsPstEccentricTighten(void);
 ubyte PstReadEndPos(ubyte dir);
 ubyte PstReadCoarseZeroPos(void);
 ubyte PstReadFineZeroPos(void);
 ubyte PstReadMagazineInsert(void);
 ubyte PstReadMagazinePara(ubyte *para);

 float PstGetActEndReduce(void);
 ubyte ReadAndSetCartridgeTyp(void);
 ubyte OL3387_ValveCtrl(unsigned int time);
 ubyte OL3387_ValveOnOff(ubyte state);
 ubyte PstDrvWay(float way);
 float PstReadWay(void);
 ubyte PstSetDrvParaToHost(void);
 ubyte PstSetDrvParaToDefault(void);
 ubyte PstCmpRevParaToDefault(void);
#endif
