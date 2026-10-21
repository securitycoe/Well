/****************************************************************************/
// @Module
// @Filename	  ch_ig_xy.h
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

#ifndef _XY_H_
#define _XY_H_

#include "head_piston_tab.h"
// #define COUNT_XY_F 4

 extern float g_fServoOffsetX;
 extern const struct ADJ_XY g_audtXYConfig384[];

 extern struct DEV_TYPE *g_pudtActDeviceType;
 extern struct DEV_TYPE g_udtDeviceType96_384;
 extern struct DEV_TYPE g_udtDeviceType96_1536;
 extern struct XYAdj g_udtServoAdjustPara;

	// Initialisierung der Servos in der vorgegebenen Position
 void XYServoInit(ubyte no);	// mag_ctrl_ini(unsigned char bef,unsigned char no);
 void ShowLocation(ubyte complete);
 void XYProgConfig(void);
 void XYInitStage(const struct ZW_POS *ptr);
 const struct POS_CTRL *XYServoCheckAndGetPos(ubyte no);
 ubyte XYServoWay(float way_x,float way_y);
 ubyte XYServoCheckZeroPos(void);
 void XYNextManPos(int8_t dir);
 ubyte XYServoPos(ubyte no);
 ubyte XYCheckAdjPara(void);
 ubyte PlateTypeModeCheck(void);
 void XYSetAdjParaToDefault(void);
 void SetPlateTypeMode(const struct HEAD_TYPE *ptr);
 void XYProgConfig1536(void);

#endif
