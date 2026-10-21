/****************************************************************************/
// @Module		CH-Igel
// @Filename	  ch_ig_zg.h
// @Project
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	  TASKING C166
//
//----------------------------------------------------------------------------
// @Date		  23.08.01 07:57:54
//
//****************************************************************************

//****************************************************************************
// @Defines
//****************************************************************************

#ifndef _IG_Z_H_
#define _IG_Z_H_

#define VertDrvDisableInt
#define VertDrvEnableInt

 extern bool g_bVertDrvDir;
 extern long g_lVertDrvSteps;
 extern short g_sVertDrvTabWay;
 extern ubyte g_ubytVertDrvState;
 extern const struct SIN_COS *g_pudtVertDrvSinus;
 extern float g_fVertHeightCorr;		// Copy im EEPROM
 extern float g_fVertManLimitWay;		// Copy im EEPROM
 extern float g_fVertMaxWay;
 extern float g_fVertXYSwitchPos;
 extern float g_fVertDrvCurrentPos;
 extern float g_fVertDrvMeasureToEncoderImpulse;	// Pysikalische Maßänderung/Anzahl der Encoderimpulse
 extern float g_fVertDrvPrecision; 		  	// geforderte Positioniergenauigkeit CW vario
 extern uword g_uwVertAccSteps;
 extern struct REV_PARA g_udtVertRevMan;
 extern struct DRV_ZK *g_pudtVertEndManZk;
 extern struct DRV_ZK g_audtVertZk[],*g_pudtVertEndZk,*g_pudtVertActZk,*g_pudtVertActEndZk;

 ubyte VertDrvWay(float way);

 void VertDrvSetCurrentToZero(void);
 void VertDrvGetPara(char *zk);
 void VertDrvGetLimits(char *zk);
 struct DRV_ZK* VertDrvSpeedLimit(float speed);
 float VertGetVelocity();
 ubyte VertCalcDrvPara(struct REV_PARA *rev);
 ubyte VertCheckDrivePara(struct REV_PARA *rev);
 ubyte VertInitZkMan(void);
 void VertConfManRev(void);
 ubyte VertDrvCalibrate(void);
 ubyte VertDrvWayXYCtrl(float way);
 void VertDrvBreak(void);
 void VertDrvCtrlStart(float way);
 void VertConfigWayLimit(void);
 ubyte VertSetrpmanSpeed(void);
 ubyte VertDrvSetupHostSpeed(void);
 ubyte VertCheckZeroPos(void);
 void VertSetRevParaToDefault(void);
 ubyte VertDrvManParaCheck(void);
 ubyte VertInitZkMan(void);
 void VertSetRevDefToRevMan(void);
 void VertSetRevManToRevAct(void);
 ubyte VertSetDrvFktToMan(void);
 ubyte VertManAbortHandling(void);
 float ReadVertDrvCurrentPos(void);
 void VertDrvSetParaDefaults(void);
 float GetDefXYSwitchHeightOL3383(void);
 float GetDefXYSwitchHeightOL3714(void);
 void VertDrvAbort(void);

#endif
