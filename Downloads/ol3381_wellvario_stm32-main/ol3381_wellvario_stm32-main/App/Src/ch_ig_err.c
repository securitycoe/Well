/*******************************************************************
 *
 *	DESCRIPTION:	Fehlerhandling
 *
 *	AUTHOR:		Baumann
 *
 *	HISTORY:
 *
 *******************************************************************/
/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

#include <_uart_terminal.h>
#include <lcd.h>
#include "ch_ig_err.h"
#include "ch_igel.h"
#include "ch_ig_cmd.h"
#include "ch_ig_utility.h"
#include "ch_ig_peri.h"
#include "ch_ig_head.h"
#include "ig_define.h"
#include "Well_Defines.h"
#include "Well_Error.h"
#include "ch_ig_piston.h"
#include "ch_ig_z.h"
#include "ch_ig_hrz.h"

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/
extern DispString_t str_err[]; 				// Variable aus dem Modul Well_Error.c
extern DispString_t str_err_unknown[];

extern bit g_bBreakFlag;

/** internal functions **/
 void ErrorSignalAndWaitEndless(void);

/** public data **/

/** private data **/

/** public functions **/

/** private functions **/


//Programm nicht im computergesteuerten Mode verwenden
ubyte ErrorDisplay(ubyte err_no){ /*Fehlerausgabe*/
	char help[4];
	DispString_t *sPtr;
	//if((error_no == CB_READY)||(g_bHostMode)) return error_no;
	if(err_no == CB_READY) return err_no;
	if(HeadConnectionPin == 0){	//Kopf eingesetzt
		CarDrvAbort();			//im Fehlerfall Abbruch aller Fahrten
		VertDrvAbort();
		PstDrvAbort();
		ClearAllAndDisplay(str_err);
		sprintf(help,"%u",err_no); LcdShow(1,(char)strlen(str_err[g_ubytLang].message)+1,help);
		if((err_no == 0)||(err_no > (GetErrDescSize()/sizeof(struct ERR_HANDLING)-1))) sPtr = str_err_unknown;
		else sPtr = ErrDesc[err_no].msg;
		Display(sPtr);
		g_udtErrorState.IntErr = err_no; 					//Speicherung der internenFehlernummer
		g_udtErrorState.HostErr = ErrDesc[err_no].h_err;
		ErrorSignalAndWaitEndless();
		return err_no;	// nur dummy,da diese Stelle nie erreicht wird
	}else{
		printf("UnexpectedHeadRemove");
		UnexpectedHeadRemove();
		return err_no;	// nur dummy,da diese Stelle nie erreicht wird
	}
}

void ErrorSignalAndWaitEndless(void){
	unsigned char i;
	Asc0DtrCtrl(ON);
	for(i = 0;i<12;i++) {BeepCtrl(25); Wait(18);}
	while(1){					// Dient evtl. zur Ermittlung des Fehlers für den Service
		WDO();
		if(IsCmdComplete()||g_bBreakFlag) {CmdCopy(); CmdExec();
		}
	}
}

ubyte ErrorCheck(ubyte err)	//drv_error_check(unsigned char error)
{
  if(g_bHostMode) return err; /*Fehlerweitergabe zum Host*/
  if((err == CB_READY)||(err == IERR_PROG_ABORT)) return err;
  return ErrorDisplay(err);			/*mit Endlosschleife*/
}


