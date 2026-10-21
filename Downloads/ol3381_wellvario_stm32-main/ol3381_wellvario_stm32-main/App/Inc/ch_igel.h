//****************************************************************************
// @Module
// @Filename	  ch_ig.h
// @Project	   Ch_ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	  TASKING C166
//
// @Codegenerator DAvE 2-0-0
//
// @Description
//
//----------------------------------------------------------------------------
// @Date		  03.09.01
//
//****************************************************************************

//****************************************************************************
// @Defines
//****************************************************************************
#include "typedefs.h"

#ifndef _IG_H_
 #define _IG_H_

 #define COUNT_EX_F 4
 #define COUNT_OL3387_EX_F 3

 extern const struct Modus execute_function[];
 extern const struct Modus OL3387_ExecuteFunctions[];

 extern ubyte g_ubytDrvInitState;
 extern ubyte g_ubytLang;			// Sprachauswahl
 extern bool g_bTestFlag;			// Wahr: Hilfsausgaben,die nicht dem Protokoll entsprechen,werden erlaubt
 extern bool g_bAllowHostMode;		// erlaube Hostmode
 extern bool g_bHostMode;			// host_flag - Kennung für Hostmode
 extern bool g_bXYPresent;			// flag_xy - nur bei 384er Ausführung wahr,bei 96er falsch
 extern bool g_bEnableDrive;		// gilt für HDrv und VertDrv
 extern bool g_bBeepState;			// Piepser an / aus
 extern bool g_bShowSpots384; 		// erlaubt Anzeige der Platznummern

 extern unsigned short g_uiDeviceType;


 void UserMain(void);
 int8_t ProgModification(const struct Modus *ptr,int8_t len,int8_t beg,ubyte cls);
 void ShowInit(void);
 void ProgExit(void);
 void PermitHostMode(ubyte flg);
 ubyte IsPermitHostMode(void);

#endif
