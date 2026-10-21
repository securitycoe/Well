//****************************************************************************
//Module		ch_ig_ptme.c
//Filename	  ch_ig_ptme.c
//Project	   common
//----------------------------------------------------------------------------
//Controller	Infineon C167CS-L
//
//Compiler	  Tasking
//
//Description   This file contains functions and variables
//				for .
//
//----------------------------------------------------------------------------
//Date		  24.04.2008 09:45:57
//
//****************************************************************************

//****************************************************************************
//Project Includes
//****************************************************************************
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions
#include "ch_ig_ptme.h"
#include "general_fkt.h"

#include "Well_Error.h"

//****************************************************************************
//Macros
//****************************************************************************

//****************************************************************************
//local definitions
//****************************************************************************
#define PM_END_POS_CYCLES 	30	// Tiefpass Sensorabfrage
#define PM_CYLINDER_TIMEOUT 150	// Basis 10ms

//#define SAFETY_CNT 23	   /* Anzahl der Zeichen der Testzeichenkette */
//****************************************************************************
//Imported External Variables
//****************************************************************************

//****************************************************************************
//Private Variables
//****************************************************************************

/*
const char szSafetyConst[SAFETY_CNT] = "ABCxyzabcXYZ0123456789";
#pragma noclear	// Werte dieser Sektion werden von der Batterie gestützt
char szSafetyMem[SAFETY_CNT] _at(0x40012);
unsigned short usIsPresMechConfig _at(0x4002A);
unsigned short usMagVentilState _at(0x4002C);
#pragma clear
*/

//****************************************************************************
//Public Variables
//****************************************************************************
unsigned short usIsPresMechConfig;	// Flag,Präsentiermechanik Konfigurieren
									// TRUE = vorhanden
									// FALSE = nicht vorhanden

//****************************************************************************
//External functions
//****************************************************************************

//****************************************************************************
//Prototypes Of Private Functions
//****************************************************************************
unsigned short CylinderMoveAndWait(unsigned short usSensor,unsigned short usWaitTime);
unsigned short IsPresMechPosition(unsigned short usSensor);

//short BatteryRAMCheckFailure(void);
//void ResetPresMechDataAfterLowBatteryCondition(void);
//****************************************************************************
//Prototypes Of Public Functions
//****************************************************************************

//****************************************************************************
//Default Settings
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
unsigned short IsPresMechConfig(void){
	return usIsPresMechConfig;
}
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
void SetPresMechConfig(unsigned short usValue)
{
	usIsPresMechConfig = usValue;
}

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
unsigned short MovePresMech(unsigned short usValue){
	if(!IsPresMechConfig()){
		return IERR_PMTE_NOT_CONFIG;
	}
	switch(usValue){
		case PTME_CyBiWellPos:	return MovePresMechCyBiWell();
		case PTME_PresentPos:	return MovePresMechPresentPos();
		default:				return IERR_PARA;
	}
}

//****************************************************************************
//Funktion	MovePresMechCyBiWell
//
//----------------------------------------------------------------------------
//Description   Händling Präsentiermechanik zum CyBiWell bewegen
//
//----------------------------------------------------------------------------
//Returnvalue   unsigned short
//					CB_READY
//					IERR_PROG_ABORT
//					IERR_PMTE_PRESENT_POS
//					IERR_PMTE_CYBIWELL_POS
//----------------------------------------------------------------------------
//Parameters	none
//
//----------------------------------------------------------------------------
//Date		  24.04.2008 11:55:46
//
//****************************************************************************
unsigned short MovePresMechCyBiWell(void){
	unsigned short usState;
	//MagVentilCyBiWellPos;	//Magnetventil schalten
	if((usState = CylinderMoveAndWait(PTME_CyBiWellPos,PM_CYLINDER_TIMEOUT)) != CB_READY){
		MagVentilPresentPos;//Magnetventil zurückschalten
	}
	return usState;
}
//****************************************************************************
//Funktion	MovePresMechPresentPos
//
//----------------------------------------------------------------------------
//Description   Händling Präsentiermechanik zur Präsentierposition bewegen
//
//----------------------------------------------------------------------------
//Returnvalue   unsigned short
//					CB_READY
//					IERR_PROG_ABORT
//					IERR_PMTE_PRESENT_POS
//					IERR_PMTE_CYBIWELL_POS
//----------------------------------------------------------------------------
//Parameters	none
//
//----------------------------------------------------------------------------
//Date		  24.04.2008 11:55:46
//
//****************************************************************************
/*unsigned short MovePresMechPresentPos(void)
{unsigned short usState;
	short s = 0;
	for(; s < 20; s = s + 1)
	{
		MagVentilPresentPos;	//Magnetventil schalten
		if((usState = WaitCylinderMove(PTME_PresentPos)) != CB_READY)
		{
			// Da klemmt vielleicht das Magazin,also rütteln wir mal...
			short sInTime = 50;
			short sOutTime = 13;
			MagVentilCyBiWellPos;	// Nochmal rein
			Wait(sInTime);
			MagVentilPresentPos;	// Raus
			Wait(sOutTime);
			MagVentilCyBiWellPos;	// Nochmal rein
			Wait(sInTime);
			MagVentilPresentPos;	// Raus
			Wait(sOutTime);
			MagVentilCyBiWellPos;	// Nochmal rein
			Wait(sInTime);
			MagVentilPresentPos;	// Raus
			Wait(sOutTime);
			MagVentilCyBiWellPos;	// Nochmal rein
			Wait(sInTime);
			MagVentilPresentPos;	// Raus
			Wait(sOutTime);
			MagVentilCyBiWellPos;	// Nochmal rein
			Wait(sInTime);
		}
	}

	MagVentilPresentPos;	// Raus
	if((usState = WaitCylinderMove(PTME_PresentPos)) != CB_READY)
	{
		MagVentilCyBiWellPos;//Magnetventil zurückschalten
	}

	return usState;
} */

// ab version 4.22
unsigned short MovePresMechPresentPos(void){
	unsigned short usState;
	short s = 0;
	short sInTime = 50;
	short sOutTime = 25;

	for(; s < 20; s = s + 1){
		// Start mit kurzem Vorab-Rütteln,da die Spitzen immer kleben
		CylinderMoveAndWait(PTME_PresentPos,sOutTime);	// Raus
		CylinderMoveAndWait(PTME_CyBiWellPos,sInTime);	// Nochmal rein
		Wait(sInTime);	// noch etwas warten,damit das Magazin auch mechanisch am Rahmen anschlägt
		CylinderMoveAndWait(PTME_PresentPos,sOutTime);	// Raus
		CylinderMoveAndWait(PTME_CyBiWellPos,sInTime);	// Nochmal rein
		Wait(sInTime);
		CylinderMoveAndWait(PTME_PresentPos,sOutTime);	// Raus
		CylinderMoveAndWait(PTME_CyBiWellPos,sInTime);	// Nochmal rein
		Wait(sInTime);
		if((usState = CylinderMoveAndWait(PTME_PresentPos,PM_CYLINDER_TIMEOUT)) == CB_READY)  break;
	}

	if((usState = CylinderMoveAndWait(PTME_PresentPos,PM_CYLINDER_TIMEOUT)) != CB_READY){
		MagVentilCyBiWellPos;//Magnetventil zurückschalten
	}
	return usState;
}

//****************************************************************************
//Funktion	CylinderMoveAndWait
//
//----------------------------------------------------------------------------
//Description   Steuerung Magnetventil Zylinder und warten bis Pneumatikzylinder
//			  Position erreicht.
//
//
//----------------------------------------------------------------------------
//Returnvalue   unsigned short
//					CB_READY
//					IERR_PROG_ABORT
//					IERR_PMTE_PRESENT_POS
//					IERR_PMTE_CYBIWELL_POS
//----------------------------------------------------------------------------
//Parameters	unsigned short
//					Position PTME_CyBiWellPos
//					Position PTME_PresentPos
//----------------------------------------------------------------------------
//Date		  24.04.2008 11:41:39
//
//****************************************************************************
unsigned short CylinderMoveAndWait(unsigned short usSensor,unsigned short usWaitTime){
	// Magnet-Ventilsteuerung
	if(usSensor == PTME_PresentPos)
		MagVentilPresentPos;	// Raus schieben zur Uebergabeposition
	else
		MagVentilCyBiWellPos;

	SetWaitTime(usWaitTime);
	while(IsWaitTime()){
		WDO();
		if(IsPresMechPosition(usSensor)){
			ResetWaitTime();
			return CB_READY;
		}
		if(ReadAbortConditions()){
			return IERR_PROG_ABORT;
		}
	}
	switch(usSensor){
	case PTME_CyBiWellPos:
		return IERR_PMTE_CYBIWELL_POS;
	case PTME_PresentPos:
		return IERR_PMTE_PRESENT_POS;
	default:
		return IERR_PARA;
	}
}


//****************************************************************************
//Funktion	IsPresMechPosition
//
//----------------------------------------------------------------------------
//Description   Positionssensor Abfragen
//
//----------------------------------------------------------------------------
//Returnvalue   unsigned short
//					TRUE = Position erreicht
//					FALSE = Position nicht erreicht
//----------------------------------------------------------------------------
//Parameters	unsigned short
//					Position
//----------------------------------------------------------------------------
//Date		  24.04.2008 11:36:25
//
//****************************************************************************
unsigned short IsPresMechPosition(unsigned short usSensor){
	unsigned short usCounter;
	unsigned short usHitCyBiWellPos = 0,usHitPresentPos = 0;

	for(usCounter = 0; usCounter < PM_END_POS_CYCLES; usCounter++){
		WDO();
		switch(usSensor){
		case PTME_CyBiWellPos:
			if(IsPresMechCyBiWellPos()){
				usHitCyBiWellPos++;
			}
			break;
		case PTME_PresentPos:
			if(IsPresMechPresentPos()){
				usHitPresentPos++;
			}
			break;
		default:
			break;
		}
	}
	if((usSensor == PTME_CyBiWellPos) && (usHitCyBiWellPos > PM_END_POS_CYCLES / 2)){
		return TRUE;
	}else if((usSensor == PTME_PresentPos) && (usHitPresentPos > PM_END_POS_CYCLES / 2)){
		return TRUE;
	}
	return FALSE;
}
