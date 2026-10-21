/****************************************************************************/
// @Module		
// @Filename	  ch_ig_head.h
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
// @Date		  22.08.02 16:00
//
//****************************************************************************

//****************************************************************************
// @Defines
//****************************************************************************

#ifndef _HEAD_H_
#define _HEAD_H_

#include "Well_Defines.h"

 #define TC_SAFETY_COUNT 50	// in 10 ms --> 0,50s da die langsamste Antwort vom TC ca. 450 ms dauert

 extern ubyte g_ubytSafetyHead;	// Flag für die Art der Sicherheitsüberwachung beim Kopfwechsel
 extern ubyte g_ubytShieldWatch;

 void ProgHeadChange(void);
 void HeadCheckInsertAndWarning(void);
 void HeadInsertAndDrvDown(void);
 ubyte HeadDrvToLowerPos(void);
 ubyte HeadDrvToUpperPos(void);
 void WarningDisplayHeadInsert(void);
 ubyte HeadMoveCtrl(ubytFct fp);
 ubyte RemoveTipsMagAndInsertHeadChangeTool(void);
 ubyte IsHeadInsert(void);
 ubyte LifterDrvToUpperPos(void);
 void PrgLifterDrvToUpperPos(void);
 ubyte HeadRemove(ubyte FirstCall);
 void RemoveHeadChangeTool(void);
 ubyte HeadInsert(void);
 void UnexpectedHeadRemove(void);
 void UserMovePresMechToPresentPos(DispString_t* str);
 void UserMovePresMechToCyBiWell(DispString_t* str);
 unsigned char IsHeadType1536(void);

#endif
