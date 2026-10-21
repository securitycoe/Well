/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/


#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

#include <_i2c_fram.h>

#include "ch_ig_head.h"
// ---------- Master/C167CS

#include "ch_ig_peri.h"
#include "ch_ig_err.h"
#include "ch_ig_cmd.h"
#include "Well_Error.h"
#include "ch_igel.h"

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/
extern uword g_uwWaitTime;
extern uword g_uwKeyRepeatTime;
extern uword g_uwBeepTime;
extern uword g_uwTimeout;
extern uword g_uwHeadSafetyTime;

extern unsigned char	io2;
extern unsigned char	io6;

/** internal functions **/

/** public data **/
unsigned char	io0;
unsigned char	io2;
unsigned char	io3;
unsigned char	io6;
unsigned char	io7;
/** private data **/

/** public functions **/

/* FUNCTION: 	IsHeadLowerPos
   PARAMETERS:	---
   DESCRIPTION:	Abfrage der unteren Kopfposition
   RETURNS: 	TRUE Kopf in unterer Position / FALSE */
ubyte IsHeadLowerPos(void)
{	// ca. 0.5ms Abfragezeit bei 20MHz,2 Waitstates
	return TRUE; //does not exist anymore
}

/* FUNCTION: 	IsHeadUpperPos
   PARAMETERS:	---
   DESCRIPTION:	Abfrage der oberen Kopfposition
   RETURNS: 	TRUE Kopf in oberer Position / FALSE */
ubyte IsHeadUpperPos(void)
{	// ca. 0.5ms Abfragezeit bei 20MHz,2 Waitstates
	return TRUE; //does not exist anymore
}

/* FUNCTION: 	IsHeadInsertPos
   PARAMETERS:	---
   DESCRIPTION:	Abfrage Kopf eingeschoben
   RETURNS: 	TRUE Kopf ist eingeschoben / FALSE */
ubyte IsHeadInsertPos(void)
{	// ca. 0.5ms Abfragezeit bei 20MHz,2 Waitstates
	return TRUE; //does not exist anymore
}

/* FUNCTION: 	IsHeadPreWarnPos
   PARAMETERS:	---
   DESCRIPTION:	Kopf hat nVorwarnposition erreicht
   RETURNS: 	TRUE Kopf in nVorwarnposition / FALSE */
ubyte IsHeadPreWarnPos(void)
{
	return FALSE; //does not exist anymore
}

/* FUNCTION: 	HeadReadChangeTool
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Kennung Kopfwechseltool,Magnet schaltet auf 0V
   RETURNS: 	Kopfwechseltool - vorhanden/ nicht vorhanden
				bzw. Fehler in der Kommunikation durch I2C */
ubyte HeadReadChangeTool(void)
{
	ubyte state;
	if(I2CRead(0x22,1,&state) == FALSE)
		return IERR_I2C_HEAD_MAG;
	if((state&0xC0) == 0x00)
		return PST_HEAD_TOOL_AVAILABLE;
	return PST_MAG_NOT_AVAILABLE;
}

/* FUNCTION: 	IsHeadCoverOpen
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Sicherheitsschalter Haube
   RETURNS: 	TRUE / FALSE */
ubyte IsHeadCoverOpen(void)
{
  return FALSE; //does not exist anymore
}

/* FUNCTION: 	IsHeadShieldOpen
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Sicherheitsschalter Kopfschieber bzw. der Lichtschranke (gleiche Anschlusspins)
				Beide Eingänge sind über pull-down-Widerstände auf Gnd gezogen,
				entsprechende Stecker sind beim OL3381 (manueller Kopfwechsel) nicht angeschlossen
   RETURNS: 	TRUE / FALSE */
ubyte IsHeadShieldOpen(void)
{
	return FALSE; //does not exist
}

/* FUNCTION: 	IsLSInterrupted
   PARAMETERS:	---
   DESCRIPTION:	Abfrage Lichtschranke (gleiche Anschlusspins wie Kopfschieber)
				Beide Eingänge sind über pull-down-Widerstände auf Gnd gezogen,entsprechende Stecker
				sind beim OL3381 (manueller Kopfwechsel) nicht angeschlossen
   RETURNS: 	TRUE / FALSE */
ubyte IsLSInterrupted(void)
{
	return FALSE; //does not exist
}

/* FUNCTION: 	BlindOpenOrLSInterrupted
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Sicherheitsschalter Kopfschieber
				bzw. der Lichtschranke (gleiche Anschlusspins)
   RETURNS: 	IERR_NO_ERR (keine Bedingung erfüllt) / IERR_JALOUSIE_OPEN / IERR_LS_INTERRUPTED*/
/*ubyte BlindOpenOrLSInterrupted(void)
{ubyte state;
  state = IERR_NO_ERR;
  if(g_ubytSafetyHead == WORKING_WITH_BLIND)
  {if(g_ubytShieldWatch == WORKING_WITH_SHIELD)
	{if(IsHeadShieldOpen()) state = IERR_JALOUSIE_OPEN;}
  }
  else
  {if(IsLSInterrupted()) state = IERR_LS_INTERRUPTED;}
  return state;
} */

ubyte BlindOpenOrLSInterrupted(void)
{
	ubyte state;
	state = IERR_NO_ERR;
	switch(g_ubytSafetyHead){
	case  WORKING_WITH_BLIND:
		if(g_ubytShieldWatch == WORKING_WITH_SHIELD){
			if(IsHeadShieldOpen()) state = IERR_JALOUSIE_OPEN;
		}
		break;
	case  WORKING_WITH_LS:
		if(IsLSInterrupted()) state = IERR_LS_INTERRUPTED;
		break;
	case  WORKING_WITH_TC:
		if(g_uwHeadSafetyTime == 0) state = IERR_TC_INTERRUPTED;
		break;
	}
	return state;
}

/* FUNCTION: 	SecurityCheckIsShieldOpen
   PARAMETERS:	---
   DESCRIPTION:	Sicherheitscheck Kopfschieberüberwachungsschalter offen
   RETURNS: 	CB_READY / Fehler */
ubyte SecurityCheckIsShieldOpen(void)
{
	ubyte cnt_shield_open,i;
	cnt_shield_open = 0;
	for(i = 0; i<10; i++) if(IsHeadShieldOpen()) cnt_shield_open++;
//  if(cnt_shield_open != 10) return IERR_SHIELD_MONITORING;	// obsolete
	return CB_READY;
}

/* FUNCTION: 	IsChangeToolInsert
   PARAMETERS:	---
   DESCRIPTION:	Prüfung auf eingesetztes Kopfwechseltool
				Programm nicht für die Hostkommunikation verwenden!
   RETURNS: 	TRUE / FALSE */
ubyte IsChangeToolInsert(void)
{	// nicht im Hostmode verwenden
	ubyte state;
	state = HeadReadChangeTool();
	if(state == IERR_I2C_HEAD_MAG)
		ErrorDisplay(state);
	if(state == PST_HEAD_TOOL_AVAILABLE)
		return TRUE;
	return FALSE;
}

ubyte HeadReadSwitchS1(ubyte *para)
{
	if(I2CRead(0x21,1,para) == FALSE)
		return IERR_I2C_S1;
	return CB_READY;
}

/* FUNCTION: 	DrvEnable
   PARAMETERS:	---
   DESCRIPTION:	Stromreferenzspannung für die SM-Endstufen zuschalten
   RETURNS: 	--- */
void DrvEnable(void)
{
	io6 = 1;	// Stromreferenzspannung für die SM-Endstufen zuschalten
}

/* FUNCTION: 	DrvDisable
   PARAMETERS:	---
   DESCRIPTION:	Stromreferenzspannung für die SM-Endstufen ausschalten
   RETURNS: 	--- */
void DrvDisable(void)
{
	io6 = 0; 	// Stromreferenzspannung für die SM-Endstufen ausschalten
	g_ubytDrvInitState|= 0x0B;	// SM-Antriebe unkalibriert
}

/** private functions **/
