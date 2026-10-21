/*******************************************************************
 *
 *	DESCRIPTION: Grippersteuerung
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions
#include "ch_ig_grp.h"
#include "ch_ig_cmd.h"
#include "Well_Error.h"


// Interne Funktionen
ubyte GripperCtrl(ubyte dir);
ubyte GripperReqEndPos(void);

#define GRIPPER_DIR_CLOSE		1
#define GRIPPER_TIMEOUT			400	//4s
#define GRIPPER_END_POS_CNT		20	// Anzahl der Abfrage der Endlage

#define IsGripperNotConnected		!nGripper_connected_RD
#define IsGripperEndPos 			(!nGripper_Endlage_RD)
#define GripperStart 				Gripper_Start
#define GripperStop 				Gripper_Stop
#define GripperOpen 				Gripper_Open
#define GripperClose 				Gripper_Close

extern uword g_uwWaitTime;

extern bit g_bBreakFlag;

/* FUNCTION: 	CmdGripperCtrl
   PARAMETERS:	---
   DESCRIPTION:	Kommando zur Steuerung des Grippers
   RETURNS: 	CB_READY / ERROR */
ubyte CmdGripperCtrl(void)
{ubyte dir;
  if(IsGripperNotConnected) return IERR_COM_NOT_PRACTICABLE;
  NextPara();
  if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  dir = (ubyte)atoi(g_pchCmdBuf);
  return GripperCtrl(dir);
}

/* FUNCTION: 	GripperCtrl
   PARAMETERS:	Richtung ZU --> 1,AUF --> 0
   DESCRIPTION:	Steuerung des Grippers,Endlage erreicht -->0
   RETURNS: 	CB_READY / ERROR */
ubyte GripperCtrl(ubyte dir)
{
  if(dir == GRIPPER_DIR_CLOSE) GripperClose; else	GripperOpen;
  GripperStart;
  g_uwWaitTime = 2;					//Warten auf den aktuellen Status der Endlage 2,da Timer asynchron läuft
  while(g_uwWaitTime) WDO();		// Einschwingzeit zum Umschalten der Endlage
  g_uwWaitTime = GRIPPER_TIMEOUT;	//in 10ms
  while(g_uwWaitTime)
  {WDO();
	if(ReadAbortConditions()) {GripperStop; g_bBreakFlag = FALSE; return IERR_PROG_ABORT;}
	if(GripperReqEndPos()) {GripperStop; return CB_READY;}
  }
  GripperStop;
  return IERR_EXT_DEVICE;
}

/* FUNCTION: 	GripperReqEndPos
   PARAMETERS:	---
   DESCRIPTION:	Abfrage der aktuellen Endlage
   RETURNS: 	TRUE / FALSE */
ubyte GripperReqEndPos(void)
{ubyte i,j;
  j = 0;
  for(i = 0; i<GRIPPER_END_POS_CNT; i++) if(IsGripperEndPos) j++;
  if(j > GRIPPER_END_POS_CNT/2) return TRUE;
  return FALSE;
}

/* FUNCTION: 	GripperSelfTest
   PARAMETERS:	---
   DESCRIPTION:	Steuert den Gripper zu und auf (1x)
				Verwendung im Geräteselbsttest
   RETURNS: 	CB_READY / ERROR */
ubyte GripperSelfTest(void)
{ubyte state;
  if(IsGripperNotConnected) return CB_READY;
  state = GripperCtrl(GRIPPER_DIR_CLOSE);
  if(state != CB_READY) return state;
  Wait(100);	// 1s
  state = GripperCtrl(!GRIPPER_DIR_CLOSE);
  Wait(100);
  return state;
}
