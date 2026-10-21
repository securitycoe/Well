/****************************************************************************/
// @Module		
// @Filename	  ch_ig_spw.h
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

#ifndef _SPW_H_
#define _SPW_H_

 #define LS_WATCH_CYCLES 400	// Anzahl der LS-Unterbrechungszyklen während des Magazinwechsels

 extern ubyte g_ubytASPW;
 extern ubyte g_ubytFlgTCMAXWay;
 extern ubyte g_ubytMagNotInsertAfterHeadExchange;

 ubyte CmdTipsTighten(void);
 ubyte CmdTipsLoose(void);
 void ProgManTipsChange(void); /* manueller Spitzenwechsel */
 ubyte TipsTighten(void);
 ubyte TipsLoose(void);
 void SpwManTipsChange(DispString_t *head_line,DispString_t *remove,ubyte magazine_not_insert);
 ubyte IsSpwMagazineInsert(void);
 ubyte IsMagazineTighten(void);

 void DCMotorOnClockwise(void);
 void DCMotorOnCounterwise(void);
 void DCMotorStop(void);
 ubyte ASPWCtrlReset(void);
#endif

