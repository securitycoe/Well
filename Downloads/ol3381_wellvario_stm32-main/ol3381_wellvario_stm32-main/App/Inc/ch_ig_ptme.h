//****************************************************************************
// @Module		%qEnter Module_name:$
// @Filename	  %qEnter File_name:$
// @Project	   common
//----------------------------------------------------------------------------
// @Controller	Infineon C167CS-L
//
// @Compiler	  Tasking
//
//
// @Description   This file contains functions and variables
//				for .
//
//----------------------------------------------------------------------------
// @Date		  %date %time
//
//****************************************************************************
#ifndef _CH_IG_PTME_H_
#define _CH_IG_PTME_H_
//****************************************************************************
// @Project Includes
//****************************************************************************

//****************************************************************************
// @Macros
//****************************************************************************
//#define IsPresMechCyBiWellPos()	P2_7					// Zu -> H am SchmitttriggerOut,L am Schalter
//#define IsPresMechPresentPos()	P2_6  					// Zu -> H am SchmitttriggerOut,L am Schalter
//#define MagVentilCyBiWellPos		P2_4 = 0				// L = Magnetventil nicht bestromt,Transisor gesperrt
//#define MagVentilPresentPos		P2_4 = 1				// H = Magnetventil bestromt,Transisor durchgeschaltet

#define IsPresMechCyBiWellPos()		nShield_connected_RD  	// Zu -> H am SchmitttriggerOut,L am Schalter
#define IsPresMechPresentPos() 		nShield_Endlage_RD  	// Zu -> H am SchmitttriggerOut,L am Schalter
#define MagVentilCyBiWellPos		Shield_Start			// L = Magnetventil nicht bestromt,TransisTor gesperrt
#define MagVentilPresentPos			Shield_Stop				// H = Magnetventil bestromt,TransisTor durchgeschaltet

//****************************************************************************
// @Defines
//****************************************************************************
#define PTME_CyBiWellPos 0	// Präsentiermechanik,Position am CyBiWell
#define PTME_PresentPos 1	// Präsentiermechanik,Präsentierposition
//****************************************************************************
// @Typedefs
//****************************************************************************

//****************************************************************************
// @Imported External Variables
//****************************************************************************

//****************************************************************************
// @Public Variables
//****************************************************************************
extern unsigned short usIsPresMechConfig;	// Flag,Präsentiermechanik Konfigurieren
											// TRUE = vorhanden
											// FALSE = nicht vorhanden

//****************************************************************************
// @External functions
//****************************************************************************

//****************************************************************************
// @Prototypes Of Private Functions
//****************************************************************************

//****************************************************************************
// @Prototypes Of Public Functions
//****************************************************************************
//****************************************************************************
//Funktion	IsPresMechConfig
//
//----------------------------------------------------------------------------
//Description   Flag,Präsentiermechanik Konfiguration abfragen
//
//----------------------------------------------------------------------------
//Returnvalue   unsigned short Value von usIsPresMechConfig
//
//----------------------------------------------------------------------------
//Parameters	none
//
//----------------------------------------------------------------------------
//Date		  24.04.2008 10:16:01
//
//****************************************************************************
unsigned short IsPresMechConfig(void);
//****************************************************************************
//Funktion	SetPresMechConfig
//
//----------------------------------------------------------------------------
//Description   Flag,Präsentiermechanik Konfiguration setzen
//
//----------------------------------------------------------------------------
//Returnvalue   None
//
//----------------------------------------------------------------------------
//Parameters	unsigned short
//					Value fuer usIsPresMechConfig
//					0 = Präsentiermechanik ist nicht konfiguriert
//----------------------------------------------------------------------------
//Date		  24.04.2008 10:17:34
//
//****************************************************************************
void SetPresMechConfig(unsigned short usValue);
//****************************************************************************
//Funktion	MovePresMech
//
//----------------------------------------------------------------------------
//Description   Händling Präsentiermechanik bewegen
//
//----------------------------------------------------------------------------
//Returnvalue   unsigned short
//					CB_READY
//					IERR_PROG_ABORT
//					IERR_PMTE_PRESENT_POS
//					IERR_PMTE_CYBIWELL_POS
//					IERR_PMTE_NOT_CONFIG
//----------------------------------------------------------------------------
//Parameters	unsigned short
//					PTME_CyBiWellPos,CyBiWell
//					PTME_PresentPos,Präsentieren
//----------------------------------------------------------------------------
//Date		  24.04.2008 11:55:46
//
//****************************************************************************
unsigned short MovePresMech(unsigned short usValue);
//void SetPresMechMagVentilPowerOn(void);
unsigned short MovePresMechCyBiWell(void);
unsigned short MovePresMechPresentPos(void);

#endif
