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

#include "General_Fkt.h"
#include "ch_ig_key.h"
#include <_uart_terminal.h>

/** local definitions **/
static bool gResetRequest = FALSE;

/** default settings **/

/** external data **/

extern uword g_uwWaitTime;

void BeepOn(void)
{
	Buzzer_ON;			// io5 = 1
}

void BeepOff(void)
{
	Buzzer_OFF;		// io5 = 0
}

void Wait(uword time)
{
	g_uwWaitTime = time;						// Zeitraster 10ms
	while(g_uwWaitTime) WDO();
	//HAL_Delay(time*10);
}

ubyte CheckInt(int min,int max,int ist)
{
	if((ist < min) || (ist > max)) return FALSE;
	return TRUE;
}

/*
void DrvSetHoldCurrent(float teiler, const struct SIN_COS *sin_ptr, ubyte *adr_sp1, ubyte *adr_sp2){
	float spule1,spule2;
	spule1 = (ubyte)(sin_ptr->sin)-128;
	spule2 = (ubyte)(sin_ptr->cos)-128;
	spule1 = spule1*teiler+128;
	spule2 = spule2*teiler+128;
	*adr_sp1 = (ubyte)spule1;			// -> zum ADC
	*adr_sp2 = (ubyte)spule2;
}
*/

/*--------------------------------------------------------------
 ubyte ReadAbortConditions(void)
 ---------------------------------------------------------------
 FT:	Lesen der Abbruchbedingungen
 EP:	---
 RV: 	True / False */
 ubyte ReadAbortConditions(void)
 {
   if(ExceptionCtrl()||(RequestSKey())) return TRUE;
   return FALSE;
 }

void CopyRevPara(struct REV_PARA *dst,const struct REV_PARA *scr)
{
  dst->Start = scr->Start;
  dst->End = scr->End;
  dst->cnt = scr->cnt;
  dst->EndReduce = scr->EndReduce;
}

/* FUNCTION: 	ftoi
   PARAMETERS:	float
   DESCRIPTION:	Float-Zahl runden und als Integer bereitstellen
   RETURNS: 	Integer */
int ftoi(float f)
{
  if(f > 0) return (int)(f+0.5); else return (int)(f-0.5);
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief                  Sets the software reset request flag
/////////////////////////////////////////////////////////////////////////////////
void SetResetRequest(void)
{
	gResetRequest = TRUE;
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief                  Checks whether a software reset has been requested
/// \return                 true if a reset request is active, false otherwise
/////////////////////////////////////////////////////////////////////////////////
bool IsResetRequest(void)
{
	return gResetRequest;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief                  Perform a software reset
/////////////////////////////////////////////////////////////////////////////////
void PerformSoftwareReset(void)
{
	Wait(10);	//100ms
	HAL_NVIC_SystemReset();
}


