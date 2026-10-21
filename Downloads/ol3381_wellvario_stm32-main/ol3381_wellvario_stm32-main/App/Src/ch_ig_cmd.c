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

// ---------- Inc
#include <_uart_terminal.h>

#include "ch_ig_cmd.h"		//

#include "lcd.h"
#include "ch_ig_ptme.h"
#include "ch_ig_head.h"
#include "ch_ig_eeprom.h"
#include "ch_ig_key.h"
#include "ch_ig_hrz.h"
#include "ch_ig_piston.h"
#include "ch_ig_spw.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_ig_peri.h"
#include "ch_igel.h"
#include "ch_ig_uniadapter.h"
#include "ch_ig_err.h"

#include <Well_Cmd.h>
#include "Well_Error.h"
#include "Well_Defines.h"


/** local definitions **/
#define PRAESENTIER_MECHANIK 1
#define MIN_VALVE_OPEN 1	  /* in 10 ms */
#define MAX_VALVE_OPEN 20000 /* in 10 ms */

/** default settings **/

/** external data **/
extern bit g_bCR;
extern bit g_bBreakFlag;

extern DispString_t str_config[];

extern volatile int16_t HrzEncoder;	// replacements for old (C167CS) hardware registers

extern uword g_uwWaitTime;

/** internal functions **/

/** public data **/
char * g_pchCmdBuf,g_acAnswerBuf[ANSWER_BUF_LEN];
struct ERR_MEM g_udtErrorState;	//Speicherung der internen,externen Fehlernummer

/** private data **/


const uint16_t	gVersion_Major		= 	5;
const uint16_t	gVersion_Minor  	=	2;
const uint16_t	gVersion_BugFix 	=	0;
const uint16_t	gVersion_Testbuild  =	0;

const char *Version1 = 			"CyBi-Well";
const char *VersionDate 		= __DATE__;
const char *VersionTime 		= __TIME__;
const char *Version2 			= "Copyright by Analytik Jena last modification:";
const char *str_host_err[] 		= {"Hostfehler","Host error"};
const char *str_internal_err[] 	= {"Interner Fehler","Internal error"};

const char *strSafetyConfig[] 	= {"Konfiguration Sicherheitskreis","Configuration safety circuit"};
const char *strCarType[]	 	= {"Wagentyp","Carriage type"};
const char *strBlindConfig[] 	= {"Konfiguration Jalousie","Configuration blind"};


/** private functions **/
ubyte AspWaitTime(unsigned int time);	// in 10 ms

static char m_acCmdBuf[STDIN_BUFLEN];

void CtrlExecute(void){
	WDO();
	if(IsPermitHostMode() && IsCmdInit()){
		g_bHostMode = TRUE;
		while(1){
			WDO();
			if(g_bHostMode == FALSE) break;
			if(IsCmdComplete()|| isException()){
				CmdCopy();
				CmdExec();
			}
			if(IsResetRequest())
				PerformSoftwareReset();
		}
	}else{
		if(IsCmdComplete()){
			CmdCopy();
			if((m_acCmdBuf[0] == C_LAST_ANSWER)|| (m_acCmdBuf[0] == C_QUIT))
				CmdExec();
			else{
				g_bCR = FALSE;
				Done(IERR_COM_NOT_PRACTICABLE);
				printf("%c",ACK);
				printf("%s",g_acAnswerBuf);
				fflush(stdout);
			}
		}
		if(IsResetRequest())
			PerformSoftwareReset();
	}

}

void CmdExec(void){
	Asc0DtrCtrl(OFF);
	if(ExceptionCtrl()) m_acCmdBuf[0] = C_BREAK;	//else CmdCopy();
	printf("%c",ACK);
	fflush(stdout);
	NextPara();	// 1. Parameter eingestellt
	switch (m_acCmdBuf[0]){		/*Befehlsauswertung*/
	case C_INIT:				Done(CmdInit()); break;/*Auswertung beim 1. Aufrufin wart_inp*/
	case C_QUIT:		  		Done(CmdQuit()); break;
	case C_BREAK:				Done(IERR_PROG_ABORT); break;
 	// Piston handling
	case C_PST_SUCK:			Done(CmdSuck()); break;
	case C_PST_SUCK_OVER:		Done(CmdSuckOverstroke()); break;
	case C_PST_DISPENSE:		Done(CmdDispense()); break;
	case C_PST_CALIBRATE:		Done(PstDrvCalibrate()); break;
	case C_PST_CALC_REV:		Done(CB_READY); break;	//ALT: Done(CmdPstCalcRev()) - nicht mehr notwendig, Berechnung im Motor-Treiber
	case C_PST_LIMIT_REV:		Done(PstSpeedLimit(atof(g_pchCmdBuf))); break;
	case C_PST_READ_LIM:		PstDrvGetLimits(g_acAnswerBuf); break;
	case C_PST_READ_PARA:		PstDrvGetPara(g_acAnswerBuf); break;
	case C_PST_RES_VOL:		 	Done(CmdPstSetTotalVolume()); break;
	case C_PST_READ_VOL_PAR:	sprintf(g_acAnswerBuf,"I%4.3f %4.3f %4.3f %u\r",(double)g_pudtTipsPara->MaxVol,(double)g_pudtTipsPara->MinVol,
								(double)g_pudtTipsPara->MoreDisp,(g_udtLifePara.PstCycleVolume == 0)? TRUE : FALSE);
								break;
	case C_TIPS_TIGHTEN:		Done(CmdTipsTighten()); break;
	case C_TIPS_LOOSE:			Done(CmdTipsLoose()); break;

	case C_HRZ_ABS_WAY:			Done(CmdHrzDrvWay()); break;
	case C_HRZ_NO:				Done(CmdHrzDrvNo()); break;
	case C_HRZ_DRV_FUNC:		Done(CB_READY); break;	//ALT: Done(CmdHrzDrvFunc())- nicht mehr notwendig, Berechnung im Motor-Treiber
	case C_HRZ_LIMIT_REV:		Done(HrzDrvSpeedLimit(atof(g_pchCmdBuf))); break;
	case C_HRZ_READ_LIM:		CarDrvGetLimits(g_acAnswerBuf);	break;
	case C_HRZ_READ_PAR:		CarDrvGetPara(g_acAnswerBuf); break;

	case C_VERT_DRV_FUNC:		Done(CB_READY); break;	//ALT: Done(CmdVertDrvFunc())- nicht mehr notwendig, Berechnung im Motor-Treiber
 	case C_VERT_LIMIT_REV:  	Done(CmdVertDrvSpeedLimit()); break;
 	case C_VERT_READ_LIM:		VertDrvGetLimits(g_acAnswerBuf) ;break;
 	case C_VERT_READ_PAR:		VertDrvGetPara(g_acAnswerBuf); break;
	case C_VERT_WAY:			Done(CmdVertDrvWay()); break;

	case C_DIV_READ:			sprintf(g_acAnswerBuf,"I%u %u %u\r",(g_pudtHrzPosTabPtr->cartype == CARR_0)?(int)0:(int)g_pudtHrzPosTabPtr->CarPara->ManLoc->Len,
								(int)g_fVertMaxWay*10,(g_pudtHeadDevPara->PstCnt == 384)? 4384:1536);break;	// in 1/10mm

	case C_XY_POS:				Done(CmdXYServoPos()); break;
	case C_MULTI_COM:			Done(CmdXYServoExtension()); break;
	case C_DISP_CLEAR:	  		Done(CmdDispCls());break;
	case C_DISP_PRINT:	  		Done(CmdDispPrint());break;
	case C_WAIT_KEY:	  	  	CmdWaitForKey();break;
	case C_TEST_FLAG:			Done(CmdTestFlag());break;
	case C_VERSION:		 		sprintf(g_acAnswerBuf,"I%s V%02u.%02u.%02u.%02u HW:%s %s %s %s\r",Version1,gVersion_Major, gVersion_Minor, gVersion_BugFix,gVersion_Testbuild,Version2,HW_Revision,VersionDate,VersionTime);break;
	case C_LAST_ANSWER:		 	break;   /* Lesen der letzten Antwort zum Host */
	case C_DRV_ADJ:			 	Done(ComInitDrv()); break;
	case C_DRV_FREE:			Done(CmdDrvCurrentToZero()); break;
	case C_READ_STATE:			Done(CmdReadState()); break;
	case C_LIFE_READ:			sprintf(g_acAnswerBuf,"I%lu %lu %lu\r",(long)g_udtLifePara.PstTotalVolume,g_udtLifePara.ulngXYCycles,g_udtLifePara.ulngPistonCycles); break;
	case C_RES_SET_PAR:		 	Done( CmdSetLifePara()); break;
	case C_CMD_EXT:				Done(CmdExtension()); break;
	// Testfkt
	case C_READ_H_CNT:			sprintf(g_acAnswerBuf,"I%d\r",HrzEncoder); break;
	case C_REST_H_CNT:			HrzEncoder = 0;
								Done(HrzExtEncoderReset()); break;
	case C_SLIDE_CTRL:			Done(CmdSlider()); break;
	case C_SWITCH:				Done(CmdSwitchCtrl()); break;
	case C_READ_PST_CUR_VOL:	sprintf(g_acAnswerBuf,"I%0.2f\r",g_udtLifePara.PstCycleVolume); break;
	default:					Done(IERR_CMD_NOT_IMPLEMENTED); break;
	}
	printf("%s",g_acAnswerBuf);
	fflush(stdout); 	/* Senden der Antwort */
	// g_bCR = FALSE;
	Asc0DtrCtrl(ON);
}

ubyte CmdReadState(void){
	ubyte mag_state,pst_state;
	int vert_way;
	float fhelp;
	//Zustand der Magazinposition
	mag_state = PstReadMagazineInsert();
	if(mag_state == IERR_I2C_HEAD_MAG) return mag_state;
	if(mag_state == PST_MAG_AVAILABLE)
		mag_state = 1;
	else
		mag_state = 0;
	//Kolbenposition
	pst_state = PstReadUpperPos();
	if(pst_state == IERR_I2C_PST) return pst_state;
	if(pst_state == PST_POS)
		pst_state = 2;	// obere Endlage
	else{
		pst_state = PstReadLowerPos();
		if(pst_state == IERR_I2C_PST) return pst_state;
		if(pst_state == PST_POS)
			pst_state = 1;
		else
			pst_state = 0;	// untere Endlage,im Arbeitsbereich
	}
	//Ausheberposition
	if((g_ubytDrvInitState&0x02) == 0x02){	// noch keine Initialisierung erfolgt
		if(VertCheckZeroPos())
			vert_way = 0;
		else
			vert_way = -1; }
	else{
		//fhelp = (g_fVertDrvCurrentPos/*-g_fVertHeightCorr*/)*10;	//Kombatibilität zu Vorgängerversionen
		// ab Version 1.40 wird die aktuelle Höhe gelesen
		fhelp = ReadVertDrvCurrentPos();
		if(fhelp >=  g_fVertXYSwitchPos) fhelp -= g_fVertHeightCorr;	// oberhalb von g_fVertXYSwitchPos wird mit Korrektur gerechnet
		fhelp *= 10;	// Ausgabe in 1/10mm
		vert_way = fhelp;
		if((fhelp-vert_way) >0.5) vert_way += 1;
	}
	sprintf(g_acAnswerBuf,"I%u %u %u %d %u\r",(int)mag_state,(int)pst_state,(int)g_ubytHrzActNo,vert_way,(int)g_ubytDrvInitState);
	return INFO_FLAG;
}

/* FUNCTION: 	CmdDrvCurrentToZero
   PARAMETERS:	---
   DESCRIPTION:	Stromlosschalten eines ausgewählten Antriebes
   RETURNS: 	CB_READY / Fehlermeldung */
ubyte CmdDrvCurrentToZero(void){
	ubyte state;
	state = CB_READY;
	if(!*g_pchCmdBuf) CarDrvSetCurrentToZero();	// Kombatibilitätsmode ...
	else{
		switch(atoi(g_pchCmdBuf)){
		case 1: CarDrvSetCurrentToZero(); break;
		case 2: VertDrvSetCurrentToZero(); break;
		case 3: XYServoStopp(); break;
		case 4: PstDrvSetCurrentToZero(); break;
		default: state = IERR_PARA; break;
		}
	}
	return state;
}

/* FUNCTION: 	CmdXYServoExtension
   PARAMETERS:	---
   DESCRIPTION:	Kommandoerweiterung für die XY-Steuerung (kompatibel zum CyBi-Well 2000)
   RETURNS: 	CB_READY / Fehlermeldung */
ubyte CmdXYServoExtension(void){
	if(*g_pchCmdBuf == SET_WAY_XY) return CmdXYServoWay();
	return IERR_CMD_NOT_IMPLEMENTED;
}

/* FUNCTION: 	CmdXYServoWay
   PARAMETERS:	---
   DESCRIPTION:	Freie XY_Positionierung in mm
   RETURNS: 	CB_READY / Fehlermeldung */
ubyte CmdXYServoWay(void){
	float x,y;
	if(g_fVertDrvCurrentPos < g_fVertXYSwitchPos) return IERR_COM_NOT_PRACTICABLE;
	NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA; x = atof(g_pchCmdBuf);
	NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA; y = atof(g_pchCmdBuf);
	return XYServoWay(x,y);
}

/* FUNCTION: 	CmdXYServoPos
   PARAMETERS:	---
   DESCRIPTION:	XY_Positionierung Festpositionen
   RETURNS: 	CB_READY / Fehlermeldung */
ubyte CmdXYServoPos(void){
	int para;
	if(g_fVertDrvCurrentPos < g_fVertXYSwitchPos) return IERR_COM_NOT_PRACTICABLE;
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	para = atoi(g_pchCmdBuf);
	if(IsHeadType1536() && (para != 0))	// beim 1536er Kopf gibt es z.Z. keine Platzpositionen
	return IERR_PARA;
	return XYServoPos(atoi(g_pchCmdBuf));
}

/* FUNCTION: 	CmdVertDrvSpeedLimit
   PARAMETERS:	Enddrehzahl
   DESCRIPTION:	Reduzierung der Enddrehzahl
   RETURNS: 	CB_READY bzw. Fehlermeldung */
ubyte CmdVertDrvSpeedLimit(void){
	float speed;struct DRV_ZK *ptr;
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	speed	 = atof(g_pchCmdBuf); ptr = VertDrvSpeedLimit(speed);
	if(ptr == NULL) return IERR_PARA;
	g_pudtVertEndZk = ptr;
	return CB_READY;
}

/* FUNCTION: 	CmdVertDrvWay
   PARAMETERS:	Weg in 1/10 mm
   DESCRIPTION:	Verfahren des Weges unter Beachtung der XY-Position
   RETURNS: 	CB_READY bzw. Fehlermeldung */
ubyte CmdVertDrvWay(void){
	float way;
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	way	 = atof(g_pchCmdBuf)/10; /* Umrechnung in mm */
	return VertDrvWayXYCtrl(way);
}

/* FUNCTION: 	CmdVertDrvFunc
   PARAMETERS:	Start-,Enddrehzahl,Anz. der Schritte
   DESCRIPTION:	Rechnen der Kennlinie des Verikalantriebes
   RETURNS: 	CB_READY bzw. Fehlermeldung */
ubyte CmdVertDrvFunc(void){
	struct REV_PARA rev;
	rev.Start = atof(g_pchCmdBuf);
	NextPara();
	rev.End = atof(g_pchCmdBuf);
	NextPara();
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	rev.cnt = atoi(g_pchCmdBuf);
 	return VertCalcDrvPara(&rev);
}

/* FUNCTION: 	ComInitDrv
   PARAMETERS:	---
   DESCRIPTION:	Kalibrierung eines ausgewählten Antriebes
   RETURNS: 	CB_READY / Fehlermeldung */
ubyte ComInitDrv(void){
	if(!*g_pchCmdBuf) return CB_READY;	// ohne Parameter keine Aktion (kompatibel mit Vorgänger)
	switch(atoi(g_pchCmdBuf)){
	case 1: return CarDrvCalibrate();
	case 2: return VertDrvCalibrate();
	case 3: XYServoInit(0); return CB_READY;
	case 4: return PstDrvCalibrate();
	}
	return IERR_PARA;
}

ubyte CmdSetLifePara(void){
	ubyte index;
	if(!g_bTestFlag) return IERR_CMD_NOT_IMPLEMENTED;
	index = atoi(g_pchCmdBuf);
	NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	if(index == 1)
	{
		g_udtLifePara.ulngXYCycles = atol(g_pchCmdBuf);
		ErrorDisplay(EEPROMWriteLifePara(&g_udtLifePara));
		return CB_READY;

	}
	if(index == 2) return HeadSetPstCnt(atol(g_pchCmdBuf));
	return IERR_PARA;
}

ubyte CmdInit(void){
	ubyte state;
	char h_str[40];

	if(g_bTestFlag){
		printf("\r%s\r",str_config[g_ubytLang].message);

		ShowVersionNr(h_str);
		printf("%s\r",h_str);

		ShowDeviceNo(h_str);
		printf("%s\r",h_str);
		WDO();

		printf("%s: %d\r",strCarType[g_ubytLang],(int)g_ubytCarType);
		fflush(stdout);

		printf("%s: %d\r",strSafetyConfig[g_ubytLang],(int)g_ubytSafetyHead);
		WDO();
		printf("%s: %d\r",strBlindConfig[g_ubytLang],(int)g_ubytShieldWatch);
		fflush(stdout);

		ShowXYCycles(h_str);
		printf("%s\r",h_str);
		WDO();

		ShowDeviceChangingAtHead(h_str);
		printf("%s\r",h_str);
		printf("\r");
		fflush(stdout);
	}
	g_bHostMode = TRUE;
	/* gleiche Startbedingungen erzwingen,Kennlinie wird bei QUIT neu erstellt
		und im manuellen Mode kann nur die Enddrehzahl geändert werden */
/*	if(g_pudtHeadType->type == OL5021_1)
		state = PstSpeedLimit(35.0);
	else
		state = PstSpeedLimit(g_udtPstRevMan.End);	*/
	if (!PstCmpRevParaToDefault()){
		state = PstSetDrvParaToHost();
		if(state != CB_READY) return state;
	}
	UNIServoAngleDrvToZero();
	// siehe vorherigen Kommentar
	return VertDrvSetupHostSpeed();
}

ubyte CmdQuit(void){
	ubyte state;
	state = CB_READY;
	SetCursorOff();

	g_bHostMode = FALSE;
	state = VertSetDrvFktToMan();	 		// Erzwingen der manuell eingestellten Antriebswerte
	//Kennlinie neu rechnen
	if(state != CB_READY) return state;
	state = HrzCheckAndSetRevToDefault();	// Erzwingen der standardmäßig eingestellten Antriebswerte
	if(state != CB_READY) return state;
	return PstCheckAndSetRevToMan();		// Erzwingen der manuell eingestellten Antriebswerte
}

/* FUNCTION: 	CmdSuck
   PARAMETERS:	---
   DESCRIPTION:	Ansaugen eines Volumens
   RETURNS: 	Zustand Fehler bzw. CB_READY */
ubyte CmdSuck(void){
	unsigned int time;
	if(g_pudtHeadType->type == OL3387_8){
		time = atof(g_pchCmdBuf) * 100;	// in 10 ms
		return AspWaitTime(time);
	}
	return Suck(atof(g_pchCmdBuf));
}

/* FUNCTION: 	CmdSuckOverstroke
   PARAMETERS:	---
   DESCRIPTION:	Ansaugen eines Volumens mit überhub,anschliesend Ausstoß des überhubes
   RETURNS: 	Zustand Fehler bzw. CB_READY */
ubyte CmdSuckOverstroke(void){
	unsigned int time;
	if(g_pudtHeadType->type == OL3387_8){
		time = atof(g_pchCmdBuf) * 100;	// in 10 ms
		return AspWaitTime(time);
	}
	return SuckWithOverstroke(atof(g_pchCmdBuf));
}

/* FUNCTION: 	CmdDispense
   PARAMETERS:	---
   DESCRIPTION:	Ausstoßen eines Volumens
   RETURNS: 	Zustand Fehler bzw. CB_READY */
ubyte CmdDispense(void){
	return Dispense(atof(g_pchCmdBuf),TRUE);
//  return FrankDispense(atof(g_pchCmdBuf),TRUE);
}

/* FUNCTION: 	CmdPstCalcRev
   PARAMETERS:	---
   DESCRIPTION:	Neurechnen der Schrittmotorkennlinie für den Pumpantrieb
   RETURNS: 	Zustand Fehler bzw. CB_READY */
ubyte CmdPstCalcRev(void){
	struct REV_PARA rev;
	rev.Start = atof(g_pchCmdBuf);
	NextPara();
	rev.End = atof(g_pchCmdBuf);
	NextPara();
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	rev.cnt = atoi(g_pchCmdBuf);
	rev.EndReduce = rev.End;
	return PstCalcDrvPara(&rev);
}

 /* FUNCTION: 	CmdPstSetTotalVolume
   PARAMETERS:	keine
   DESCRIPTION: Setzen des bisherigen Gesamtvolumen,das von den Kolben abgegeben wurde
   RETURNS: 	Fehler / CB_READY	*/
ubyte CmdPstSetTotalVolume(void){
	if(!*g_pchCmdBuf) return IERR_PARA;
	if(g_bTestFlag) return HeadSetTotalVolume(atof(g_pchCmdBuf));
	return IERR_CMD_NOT_IMPLEMENTED;
}

/* FUNCTION: 	CmdHrzDrvWay
   PARAMETERS:	---
   DESCRIPTION: Anfahren einer Position
   RETURNS: 	CB_READY / ERROR */
ubyte CmdHrzDrvWay(void){
	ubyte state; float way;
	if(g_pudtHrzPosTabPtr->cartype == CARR_0) return CB_READY;
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	way	 = -atof(g_pchCmdBuf); /* Wegen Standardkoordinatenkreuz */
	if(g_pudtHrzPosTabPtr->cartype == DISK_10) way *= -1;	//Kompatibilität zum Disk
	if(CarWayCheck(way) ==  FALSE) return IERR_PARA;
	NextPara();
	if(!*g_pchCmdBuf){	// keine weiteren Parameter (X-ServoOffset)
		if(g_fServoOffsetX == 0)
			return HrzDrvWay(way);
		else
			g_fServoOffsetX = 0;}
	else	g_fServoOffsetX = atof(g_pchCmdBuf);
	state = XYServoWay(g_fServoOffsetX,0);
	if(state != CB_READY) return state;
	return HrzDrvWay(way);
}

/* FUNCTION: 	CmdHrzDrvNo
   PARAMETERS:	---
   DESCRIPTION: Anfahren einer Platzposition
   RETURNS: 	CB_READY / ERROR */
ubyte CmdHrzDrvNo(void){
	ubyte no,state;
	if(g_pudtHrzPosTabPtr->cartype == CARR_0) return CB_READY;
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	no = atoi(g_pchCmdBuf);
	NextPara();
	if(!*g_pchCmdBuf){
		if(g_fServoOffsetX == 0)
			return CarDrvNo(no);
		else
			g_fServoOffsetX = 0;}
	else g_fServoOffsetX = atof(g_pchCmdBuf);
	state = XYServoWay(g_fServoOffsetX,0);
	if(state != CB_READY) return state;
	return CarDrvNo(no);
}

/* FUNCTION: 	CmdHrzDrvFunc
   PARAMETERS:	keine
   DESCRIPTION: Rechnen der Schrittmotorkennline
   RETURNS: 	Kommandostatus	*/
ubyte CmdHrzDrvFunc(void){
	struct REV_PARA rev;
	ubyte state,flag;
	state = CB_READY;
	rev.Start = atof(g_pchCmdBuf); NextPara();
	rev.End = atof(g_pchCmdBuf); NextPara();
	rev.EndReduce = rev.End;
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	rev.cnt = atoi(g_pchCmdBuf);
	NextPara();
	flag = atoi(g_pchCmdBuf);
	if((!*g_pchCmdBuf)||(flag == 0)){	// ohne zusätzlichen Parameter bzw. 0 wird nichts in den FRAM gespeichert
		return HrzCalcDrvPara(&rev);
	}
	if(flag == 1){
		state = EEPROMSetHrzMotPara(&rev);
		if(state == CB_READY) return HrzCalcDrvPara(&rev);
	}else if(flag == 2){
		return SetAndSaveHrzDrvParaToDefault();
	}
	else state = IERR_PARA;
	return state;
}

/* FUNCTION: 	CmdTestFlag
   PARAMETERS:	keine
   DESCRIPTION:	Setzen bzw. Rücksetzen des Testflag
   RETURNS:		Kommandostatus */
ubyte CmdTestFlag(void){
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	if(atoi(g_pchCmdBuf)) g_bTestFlag = TRUE; else g_bTestFlag = FALSE;
	return CB_READY;
}

/* FUNCTION: 	CmdWaitForKey
   PARAMETERS:	keine
   DESCRIPTION:	Tastaturabfrage
   RETURNS: 	Information: Tastenkode o. Fehlerkode: Programmabbruch */
void CmdWaitForKey(void){
	ubyte key;
	key = KeyRequestFromHost();
	BeepCtrl(BEEP_TIME);
	if(key)
		sprintf(g_acAnswerBuf,"I%u\r",(int)key);
	else
		sprintf(g_acAnswerBuf,"E%u\r",(int) IERR_PROG_ABORT);
}

/* FUNCTION: 	CmdDispCls
   PARAMETERS:	keine
   DESCRIPTION:	Löscht ausgewählte Zeile(n) am Display
				kein Parameter gesamtes Display,1,2 jeweilige Zeile
   RETURNS: 	Kommandostatus */
ubyte CmdDispCls(void){
	ubyte line;
	if(!*g_pchCmdBuf)
		LcdClear(ALL_LINE);
	else{
		line = atoi(g_pchCmdBuf);
		if((line > SECOND_LINE)||(line < FIRST_LINE)) return IERR_PARA;
		LcdClear(line);
	}
	SetCursorOff(); return CB_READY;
}

/* FUNCTION: 	CmdDispPrint
   PARAMETERS:	keine
   DESCRIPTION:	Anzeige eines Textes beginnend an Position line,column
   RETURNS: 	Kommandostatus */
ubyte CmdDispPrint(void){
	ubyte line,column;
	SetCursorOff();
	line = atoi(g_pchCmdBuf);
	NextPara();
	column = atoi(g_pchCmdBuf)+1;	// Neues Display basiert auf 1 ...40
	NextPara();
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	if(LcdCheckPara(line,column,strlen(g_pchCmdBuf)) ==  FALSE) return IERR_PARA;
	LcdShow(line,column,g_pchCmdBuf);
	return CB_READY;
}

/* FUNCTION: 	Done
   PARAMETERS:	Ferigmeldung als Zustandskode
   DESCRIPTION:	Vorbereitung der Antwort zum Host
   RETURNS:		void */
void Done(ubyte state){
	//if(prg_break_flag) state = IERR_PROG_ABORT;
	g_bBreakFlag = FALSE;
	if(state == CB_READY){
		g_acAnswerBuf[0] = RDY;
		g_acAnswerBuf[1] = 0;
		return;
	}
	if(state == INFO_FLAG) return;					// ZK ist bereits in g_acAnswerBuf
	g_udtErrorState.IntErr = state;
	g_udtErrorState.HostErr = ErrDesc[state].h_err;
	sprintf(g_acAnswerBuf,"E%u\r",(int)ErrDesc[state].h_err);
	return;	// Es bleibt nur noch die Fehlermeldung
}

/* FUNCTION: 	CmdDevReadErr
   PARAMETERS:	---
   DESCRIPTION:	Servicebefehl für die erweiterten Fehlermeldungen
   RETURNS:		INFO_FLAG */
ubyte CmdDevReadErr(void){
	sprintf(g_acAnswerBuf,"I%s:%u %s:%u %s\r",*(str_host_err+g_ubytLang),(int)g_udtErrorState.HostErr,
  		  *(str_internal_err+g_ubytLang),(int)g_udtErrorState.IntErr,
		  (ErrDesc[g_udtErrorState.IntErr].msg+g_ubytLang)->message);
	return INFO_FLAG;
}

/* FUNCTION: 	CmdCopy
   PARAMETERS:	keine
   DESCRIPTION:	Kopiert einen Kommandostring vom Eingabepuffer in den Kommandopuffer
   RETURNS:		TRUE --> Operation erfolgreich,ansonsten FALSE	*/
ubyte CmdCopy(void){
	if(!IsCmdComplete()) return FALSE;	// Kommando noch nicht vollständig
	gets(m_acCmdBuf);
	g_pchCmdBuf = m_acCmdBuf;
	g_bCR = FALSE;
	return TRUE;
}

/* FUNCTION: 	NextPara
   PARAMETERS:	keine
   DESCRIPTION:	Positioniert den Parameterzeiger zum nächsten Parameter
   RETURNS: 	void */
void NextPara(void){
	while(*g_pchCmdBuf) if(*g_pchCmdBuf == ' ') break; else g_pchCmdBuf++;
	while(*g_pchCmdBuf) if(*g_pchCmdBuf != ' ') break; else g_pchCmdBuf++;
}

ubyte CmdDispCurCtrl(void){ubyte cmd,line,column;
	NextPara(); cmd = atoi(g_pchCmdBuf);
	if((!*g_pchCmdBuf)||(cmd == 0)){
		SetCursorOff();
		return CB_READY;
	} /* Löschen des Cursors */
	NextPara(); line = atoi(g_pchCmdBuf);
	NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	column = atoi(g_pchCmdBuf)+1;	// Neues Display basiert auf 1 ...40
	/* Prüfen der Parameter */
	if(LcdCheckPara(line,column,1) ==  FALSE) return IERR_PARA;
	//if((line > SECOND_LINE)||(line < FIRST_LINE)||(column>LCD_MAX_COLUMN)||(column<LCD_MIN_COLUMN)) return PARA_WRONG;
	SetCursorPos(line,column);
	return CB_READY;
}

unsigned short CmdSlider(void){
	unsigned short usState;
	short sSlider;

	sSlider = atoi(g_pchCmdBuf);
	NextPara();
	switch(sSlider){
	case PRAESENTIER_MECHANIK:
		if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
		usState = MovePresMech(atoi(g_pchCmdBuf));
		break;
	default:
		usState = IERR_PARA;
	}
	return usState;
}

ubyte AspWaitTime(unsigned int time){	// in 10 ms
	ubyte state;
	if((time < MIN_VALVE_OPEN)||(time > MAX_VALVE_OPEN)) return IERR_PARA;
	g_uwWaitTime = time;	/* Generieren der Zeitüberwachung */
	state = CB_READY;
	while(g_uwWaitTime){
		WDO();
		if(ReadAbortConditions()) break;
	}
	if(g_uwWaitTime != 0) state = IERR_PROG_ABORT;
	return state;
}

unsigned char CmdSwitchCtrl(void){
	ubyte sw_no,sw_state;
	ubyte state;
	state = CB_READY;
	sw_no = atoi(g_pchCmdBuf);	//z.Z. nur als Platzhalter verwendet
	NextPara();
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	sw_state = atoi(g_pchCmdBuf);
	if((sw_no == 1)&&((sw_state == VALVE_ON)||(sw_state == !VALVE_ON))){
		state = OL3387_ValveOnOff(sw_state);
	}else
		state = IERR_PARA;
	return state;
}
