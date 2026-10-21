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
#include "stdbool.h"

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- BASE
#include "ch_head_fram.h"
#include "head_piston_tab.h"
#include "ig_define.h"
// ---------- Inc
#include <_i2c_fram.h>
#include <_tmc5130a.h>
// ---------- Master
#include "ch_ig_eeprom.h"
#include "ch_ig_err.h"
#include <ch_ig_hrz.h>
#include "ch_ig_piston.h"
#include "ch_ig_spw.h"
#include "ch_ig_xy.h"
#include "ch_igel.h"
#include "zk_calc.h"
// ---------- Pipettor_Allgemein
#include "Well_Error.h"

/** local definitions **/


#define PstDrvDisableInt ///T0IC_T0IE = 0	deaktivieren Schritt-Timer Interrupt
#define PstDrvEnableInt	 //T0IC_T0IE = 1	aktivieren Schritt-Timer Interrupt

#define HUMMINGWELL_MAG_96		0x05	// Kodierung ist hardwareabhängig
#define HUMMINGWELL_MAG_384		0x06	// Kodierung ist hardwareabhängig

/** default settings **/

/** external data **/
extern uword g_uwWaitTime;

/** internal functions **/
/*
void PstDrvStart(struct DRV_ZK *ptr);
void PstDrvAbort(void);
void PstDrvSetHoldCurrent(void);
ubyte PstDrv(ubyte dir,ubyte watch_magazine,ubyte abort,long steps);
ubyte PstDrvPreZeroPos();	// Suchen der Grobnull
*/
ubyte PstReadMagState(void);
ubyte IncrementAndSaveCyclesToHead(void);
ubyte ReadAndSetCartridgeTyp(void);

//ubyte PstWatchHead(ubyte dir,ubyte flg_watch_magazine);

/** public data **/
struct DRV_ZK g_audtPstZk[PST_ZK_DIM+1];
ubyte g_ubytPstDrvState;
bool g_bPstDrvDir;
bool g_bPstFirstSuck = TRUE;
bool g_bFirstDisp = TRUE;
long g_lPstDrvSteps;
short g_sPstDrvTabWay;

struct DRV_ZK *g_pudtPstActZk,*g_pudtPstActEndZk;
// Motorkennline manueller Betrieb,Copy im EEPROM
struct REV_PARA g_udtPstRevMan;
const struct REV_PARA g_udtPstRevDef		 = 	{PST_DEF_START_RPM,PST_DEF_END_RPM,PST_DEF_COUNTS,PST_DEF_END_RPM};
const struct REV_PARA g_udtPstRevDef_1536	 = 	{PST_DEF_START_RPM,PST_DEF_END_RPM,PST_DEF_COUNTS,PST_DEF_END_RPM_1536};

enum VOLUME_CORRECTION g_VolumeCorrection = VC_INTERN;


//#pragma noclear	// Werte dieser Sektion werden von der Batterie gestützt
struct LIFE_PARA g_udtLifePara; //_at(0x40002);

//#pragma clear


struct TIPS_PARA g_udtTipsParaFromASPW;	// nur gültig bei g_ubytASPW == TRUE !!!
const struct TIPS_PARA *g_pudtTipsPara;
//const struct HEAD_TYPE *g_pudtHeadType;
struct HEAD_TYPE *g_pudtHeadType;

 /* ab V1.44 wird in der Initialisierungsphase aktualisiert,es sei denn es wird ein
	Spitzenwechseltool nach dem Einschalten erkannt,dann wird mit dieser Initialisierung
	vorerst gearbeitet! */
const struct HEAD_DEV *g_pudtHeadDevPara = &HeadDev_OL3383_8;

/** private data **/
static uword PstAccSteps;
static struct TIPS_PARA m_pudtTipsParaZero[]	 = 	{{0,0,0,0,0,0}};
static struct REV_PARA m_udtPstRevAct;
static float m_fPstWaySearchFinePos = 2;	// 2mm --> 1 Umdr
static struct DRV_ZK *m_pudtPstEndZk;
float m_fPstDrvWay = 0;	//wird beim Fahren in mm benötigt
float g_AddVolume;	// in µl

/** private functions **/

/* FUNCTION: 	ReadHeadNoAndSetDevPara
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Kopfnummer und Setzen der Geräteparameter
   RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R / IERR_HEAD_NOT_DEF */
ubyte ReadHeadNoAndSetDevPara(void)
{
	ubyte state,tmp; struct FRAM1 * ptr;
	char *cPtr = (char*)malloc(sizeof(ptr->HeadNo));
	state = FRAMReadHeadNumber(cPtr);
	if(state != CB_READY){free(cPtr); return state;}
	//Suchen des Kopftypes

	state = IERR_HEAD_NOT_DEF;
	tmp = strlen(cPtr)-5;	//Kopfnummer ohne Leerzeichen und vierstellige Seriennummer
	for(g_pudtHeadType = HeadTypeTab; g_pudtHeadType < HeadTypeTab+(GetSize_HeadTypeTab()/sizeof(struct HEAD_TYPE)); g_pudtHeadType++)
	{
		//tmp = _fstrlen(g_pudtHeadType->type);
		// Kopftyp gefunden
		if(strncmp(g_pudtHeadType->type,cPtr,tmp) == 0) 	// Kopftyp gefunden
		{
			if(g_bTestFlag){printf("Head no: %s\r",g_pudtHeadType->type); fflush(stdout);}
			SetPlateTypeMode(g_pudtHeadType);	// für Hummingwell-Kopf wird der Anfangswert übernommen,der später noch aktualisiert wird
			g_pudtHeadDevPara = g_pudtHeadType->dev;
/*			state = EEPROMReadPstManMotPara();
			if(state == CB_READY)
			{
				if(g_pudtHeadType->type == OL5021_1)
			 		g_udtPstRevMan.EndReduce = 35.0;
				else
					g_udtPstRevMan.EndReduce = g_udtPstRevMan.End;
				state = PstSpeedLimit(g_udtPstRevMan.EndReduce);
				printf("Enddrz.: %f state: %d\r",g_udtPstRevMan.EndReduce,(int) state ); fflush(stdout);
		 	 }*/
			state = CB_READY;
			break;
		}
	}
	free(cPtr);	//kein bekannten Kopftyp gefunden
	return state;
}

ubyte ReadMagazineTypeAndSetTipsPara(const struct HEAD_TYPE *ptr)
{ubyte state,para;
  g_pudtTipsPara = g_pudtHeadType->tips;
  state = PstReadMagazinePara(&para); if(state != CB_READY) return state;
  if(ptr->type == OL3387_8) para = 0;	// Hummingwellkopf,Spitzenparameter dienen nur der Kompatibilität
  g_pudtTipsPara+= para;
	//Anfangswerte der Voluminas setzen
  if(g_bTestFlag)
  {printf("TipsPara:\r %f %f %f %f %f %f\r",
			 g_pudtTipsPara->asc,g_pudtTipsPara->zero,g_pudtTipsPara->MaxVol,
			 g_pudtTipsPara->MinVol,g_pudtTipsPara->MoreDisp,g_pudtTipsPara->MoreSuck); fflush(stdout);
  }

  if((g_pudtHeadType->type == OL5021_1)||(g_pudtHeadType->type == OL5021_12))
  {
	if(para == 5)	//Kodierung Metallspitzenmagazin mit Magnet
		g_pudtHeadType->pudtVolCorr = &tVolumeCorr_OL5021_14_130__1536_1;
	 else
	 	g_pudtHeadType->pudtVolCorr = &tVolumeCorr_OL5021_14_130__1536_8;
  }
  return CB_READY;
}

void SetTipsParaToZero(void)
{
  g_pudtTipsPara = m_pudtTipsParaZero;
}

/*  Das Programm stellt die Pumpendgeschwindigkeit der Kolben ein.
	Falls dieser Wert ausserhalb des zulässigen Grenzen liegt,wird ein Fehler
	generiert. Die ursprünglichen Werte bleiben erhalten.  */
ubyte PstSpeedLimit(float speed)
{
	float help;
  if((speed > m_udtPstRevAct.End)||(speed < m_udtPstRevAct.Start)) return IERR_PARA;
  help = (speed - m_udtPstRevAct.Start)/(m_udtPstRevAct.End-m_udtPstRevAct.Start)*(float)m_udtPstRevAct.cnt;
  m_pudtPstEndZk = g_audtPstZk+(int)help;
  m_udtPstRevAct.EndReduce = speed;
  if(g_bTestFlag) printf("Anz in Pst.:%u %x\r",(int)help,(unsigned int)m_pudtPstEndZk);
  tmc5130a_VELOCITY(P_DEVICE, M1_SpU *uSpS*speed/60);		// Umrechnung RPM auf µStep/s
  return CB_READY;
}

/* FUNCTION: 	PstGetActEndReduce
   PARAMETERS:	---
   DESCRIPTION:	Speichern der aktuellen Endgeschwindigkeitdes Kolbenantriebes
   RETURNS: 	akt. Endgeschwindigkeit */
float PstGetActEndReduce(void)
{
   return m_udtPstRevAct.EndReduce;
}


/* FUNCTION: 	PstDrvGetLimits
   PARAMETERS:	Pointer auf Zeichenkette,ZK muss groß genug sein,um alle Zeichen zu speichern!
   DESCRIPTION:	Bereitstellung der Grenzwerte und Standardwerte der  Geschwindigkeitsparameter
				des Kolbenantriebes
   RETURNS: 	void*/
/*void PstDrvGetLimits(char *zk)
{//nicht kompatibel zum CyBi Ctrl
  sprintf(zk,"I%4.1f %4.1f %u %u %4.1f %4.1f %d\r",(double)PST_RPM_MIN,(double)PST_RPM_MAX,(uword)PST_ACC_MIN,(uword)PST_ACC_MAX,
	 								(double)g_udtPstRevDef.Start,(double)g_udtPstRevDef.End,(uword)g_udtPstRevDef.cnt);
}*/

void PstDrvGetLimits(char *zk)
{
  if((g_pudtHeadType->type == OL5021_1) || (g_pudtHeadType->type == OL5021_12) ||(g_pudtHeadType->type == OL3384_15))
  	sprintf(zk,"I%d %d %u %u %d %d %d\r",(int)PST_RPM_MIN,(int)PST_RPM_MAX,(uword)PST_ACC_MIN,(uword)PST_ACC_MAX,
	 								ftoi(g_udtPstRevDef_1536.Start),ftoi(g_udtPstRevDef_1536.End),(uword)g_udtPstRevDef_1536.cnt);
 else
  	sprintf(zk,"I%d %d %u %u %d %d %d\r",(int)PST_RPM_MIN,(int)PST_RPM_MAX,(uword)PST_ACC_MIN,(uword)PST_ACC_MAX,
	 								ftoi(g_udtPstRevDef.Start),ftoi(g_udtPstRevDef.End),(uword)g_udtPstRevDef.cnt);
}

/* FUNCTION: 	PstDrvGetPara
   PARAMETERS:	Pointer auf Zeichenkette,ZK muss groß genug sein,um alle Zeichen zu speichern!
   DESCRIPTION:	Bereitstellung der aktuellen Geschwindigkeitsparameter des Kolbenantriebes
   RETURNS: 	void*/
/*void PstDrvGetPara(char *zk)
{//nicht kompatibel zum CyBi Ctrl
  sprintf(zk,"I%4.1f %4.1f %u %4.1f\r",(double)m_udtPstRevAct.Start,(double)m_udtPstRevAct.End,
									   (uword)m_udtPstRevAct.cnt,(double)m_udtPstRevAct.EndReduce);
}*/

void PstDrvGetPara(char *zk)
{
  sprintf(zk,"I%d %d %u %d\r",ftoi(m_udtPstRevAct.Start),ftoi(m_udtPstRevAct.End),
									   (uword)m_udtPstRevAct.cnt,ftoi(m_udtPstRevAct.EndReduce));
}

/* FUNCTION: 	PstDrvWay
   PARAMETERS:	Weg in mm
   DESCRIPTION:	Verfahren der Kolben in mm,wird bei der Spindelmessung benötigt
   RETURNS: 	CB_READY / Fehlerkode */
ubyte PstDrvWay(float way)
{float fhelp;
   if((way > g_pudtHeadDevPara->PstMaxWay) || (way < 0)) return IERR_PARA;
   fhelp = way-m_fPstDrvWay;
   m_fPstDrvWay = way;
   if( fhelp == 0) return CB_READY;
   if( fhelp > 0)
   {g_bPstDrvDir = SUCK; }
   else
   {g_bPstDrvDir	 = ! SUCK; fhelp *= -1; }
   g_lPstDrvSteps = fhelp * g_pudtHeadDevPara->PstDrvMicroGearRatio;
   return PstDrv(g_bPstDrvDir,FALSE,FALSE,g_lPstDrvSteps);
}

/* FUNCTION: 	PstReadWay
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Kolbenposition in mm
   RETURNS: 	aktuelle Position in mm */
float PstReadWay(void)
{
   return m_fPstDrvWay;
}

/* FUNCTION: 	PstDrv
   PARAMETERS:	dir			--> Richtung (SUCK / !SUCK)
				watch_mag 	--> Magazinüberwachung (TRUE / FALSE)
				abort		--> Abbruch enable (TRUE / FALSE)
				steps		--> Anzahl der Microsteps
   DESCRIPTION:	Kolbenantrieb bewegen
   RETURNS: 	Zustand des Kolbenantriebes CB_READY --> i.O. / Fehlermeldung */
 ubyte PstDrv(ubyte dir,ubyte watch_magazine,ubyte abort,long steps){
	static ubyte pst_cnt = 0; 		// Hilfszähler für die Kolbenzyklen
	static ubyte old_dir = SUCK; 	// Kennung für Richtungswechsel
	ubyte err;
	if(old_dir != dir){
		pst_cnt++;
		old_dir = dir;
	}
	// if(pst_cnt == 2) {pst_cnt = 0; g_udtLifePara.ulngPistonCycles++;}
	if(pst_cnt == 2){
		pst_cnt = 0; err = IncrementAndSaveCyclesToHead();
		if(err != CB_READY) return err;
	}
	if((err = PstWatchHead(dir,watch_magazine)) != CB_READY){
		return err;
	}
	g_bPstDrvDir = dir;
	g_lPstDrvSteps = steps;
	PstDrvStart(m_pudtPstEndZk);
	while(g_ubytPstDrvState != CB_READY){
		if (tmc5130a_is_stop(P_DEVICE) == TRUE){
			g_ubytPstDrvState = CB_READY;
		}
		WDO();
		if((err = PstWatchHead(dir,watch_magazine)) != CB_READY){
			PstDrvAbort();
			return err;
		}
		if(abort){
			if(ReadAbortConditions()) {PstDrvAbort();
			return IERR_PROG_ABORT;
			}
		}
	 }
	 PstDrvSetHoldCurrent();
	 return CB_READY;
}

ubyte PstCheckAndSetRevToMan(void){
	ubyte state;
	state = CB_READY;
	if((g_udtPstRevMan.Start != m_udtPstRevAct.Start)||(g_udtPstRevMan.End != m_udtPstRevAct.End)||(g_udtPstRevMan.cnt != m_udtPstRevAct.cnt))
		state = PstInitZkMan();
	if(state != CB_READY) return state;
	if(m_udtPstRevAct.EndReduce != g_udtPstRevMan.EndReduce) state = PstSpeedLimit(g_udtPstRevMan.EndReduce);
	return state;
}

/* FUNCTION: 	PstInitZkMan
   PARAMETERS:	---
   DESCRIPTION:	Einstellen der manuellen Kennlinie des Kolbenantriebes
   RETURNS: 	CB_READY / Fehlermeldung */
ubyte PstInitZkMan(void){
	ubyte state;
	state = PstCalcDrvPara(&g_udtPstRevMan);
	return state;
}

/* FUNCTION: 	PstCheckRevManPara
   PARAMETERS:	---
   DESCRIPTION:	Testet die Plausibilität der durch die Batterie gepufferten manuell
				eingestellten Drehzahlparameter
   RETURNS: 	TRUE / FALSE */
ubyte PstCheckRevManPara(void){
	return PstCheckRevPara(&g_udtPstRevMan);
}

/* FUNCTION: 	PstCheckRevPara
   PARAMETERS:	---
   DESCRIPTION:	Testet die Gültigkeit der Drehzahlparameter des Kolbenantriebes
   RETURNS: 	TRUE / FALSE */
ubyte PstCheckRevPara(const struct REV_PARA *rev){
	if((rev->Start < (float)PST_RPM_MIN)||(rev->End > (float)PST_RPM_MAX)||(rev->Start >=  rev->End)
		||(rev->cnt < (float)PST_ACC_MIN)||(rev->cnt > (float)PST_ACC_MAX))
		return FALSE;
	return TRUE;
}


/* FUNCTION: 	PstCalcDrvPara
   PARAMETERS:	---
   DESCRIPTION:	Errechnen der Drehzahlkennlinie des Kolbenantriebes
   RETURNS: 	CB_READY / Feher: IERR_DRV_ZK_PST,IERR_PARA */
ubyte PstCalcDrvPara(const struct REV_PARA *rev){
	if(PstCheckRevPara(rev) == FALSE) return IERR_PARA;
	if((PstAccSteps = CalcTimeTable(g_audtPstZk,rev->Start,rev->End,rev->cnt)) == 0) return IERR_DRV_ZK_PST;
	m_pudtPstEndZk = g_audtPstZk+rev->cnt; g_pudtPstActEndZk = m_pudtPstEndZk;
	CopyRevPara(&m_udtPstRevAct,rev);
	PstSpeedLimit(rev->EndReduce);
//  m_udtPstRevAct.EndReduce = m_udtPstRevAct.End;
	return CB_READY;
}

/* FUNCTION: 	PstDrvSetCurrentToZero
   PARAMETERS:	keine
   DESCRIPTION:	Stromlosschalten des Kolbenantriebes
   RETURNS: 	void*/
void PstDrvSetCurrentToZero(void){
	PstDrvAbort();
	tmc5130a_HOLD_CURR_MAN(P_DEVICE, 0);
	g_ubytDrvInitState |= 0x08;
}

/*------------------------------------------------------------------
  Interne Funktionen
 ------------------------------------------------------------------*/

/* FUNCTION: 	PstDrvStart
   PARAMETERS:	Pointer auf die Endgeschwindigkeit
   DESCRIPTION:	Start des Kolbenantriebes
   RETURNS: 	void*/
void PstDrvStart(struct DRV_ZK *ptr){			// struct {Schrittweite, Schrittdauer} zielwert
	g_pudtPstActEndZk = ptr;
	g_pudtPstActZk = g_audtPstZk; 				// struct {Schrittweite, Schrittdauer} startwert
	g_ubytPstDrvState = RESET; 					// "RESET" entspricht "läuft"

	if (g_bPstDrvDir == SUCK){
		g_lPstDrvSteps *= -1;
	}


	tmc5130a_POSITIONING_MODE(P_DEVICE);
	tmc5130a_HOMING(P_DEVICE);							// Position = 0 (equivalent für Schrittzähler = 0)
	tmc5130a_start_dest(P_DEVICE,g_lPstDrvSteps*8);
	tmc5130a_PIN_RUN(P_DEVICE);							//Freigabe Motor-Treiber
}

/* FUNCTION: 	PstDrvSetHoldCurrent
   PARAMETERS:	---
   DESCRIPTION:	Stetzen des Haltestroms für den Kolbenantrieb
   RETURNS: 	void*/
void PstDrvSetHoldCurrent(void){
	PstDrvAbort();
	tmc5130a_HOLD_CURR_MAN(P_DEVICE, M1_HOLD);
//	tmc5130a_HOLD_CURR_MAN(P_DEVICE, g_pudtHeadDevPara->PstDrvHoldCurrent * 1000); //Ströme aus Tabelle sind anders als die gemessenen
}

/* FUNCTION: 	PstDrvAbort
   PARAMETERS:	---
   DESCRIPTION:	Abbruch der Fahrroutine für den Kolbenantrieb (Nothalt),Motor stromfrei schalten
   RETURNS: 	void*/
void PstDrvAbort(void){
	PstDrvDisableInt;

	if(g_lPstDrvSteps > (long)g_sPstDrvTabWay) g_lPstDrvSteps = (long)g_sPstDrvTabWay;

	PstDrvEnableInt;
	tmc5130a_STOP(P_DEVICE); //new Stoppe Fahrt
	g_ubytPstDrvState = CB_READY;
	tmc5130a_PIN_STOP(P_DEVICE); 	//Sperren Motor-Treiber
	//PstDrvSetCurrentToZero();
}

/* FUNCTION: 	PstWatchHead
   PARAMETERS:	dir: Pumprichtung für die entsprechende Endlagenüberwachung
				flg_watch_magazine: True --> mit Magazinüberwachung (fest,lose,nicht vorhanden)
   DESCRIPTION:	überwachung des Kopfzustandes
   RETURNS: 	Zustand des Kopfes
				bzw. Fehler in der Kommunikation durch I2C
				IERR_I2C	 		--> Fehler durch I2C-Bus
				IERR_ECC_OPEN		--> Spannmechanismus offen
				IERR_MAG_NOT_INSERT --> Magazin nicht verfügbar
				IERR_PST_UPPER_END_POS 	--> Kolben haben die obere Endlage erreicht
				ERR_PST_LOWER_END_POS	--> Kolben haben die untere Endlage erreicht
				CB_READY			--> Zustand i.O.*/
 ubyte PstWatchHead(ubyte dir,ubyte flg_watch_magazine)
 {ubyte state;
   /* state:P7-> Spitzen fest,P6-> Spitzen lose
			P5-> obere Endl. Kolben
			P4-> Null-Lage Kolben fein
			P3-> Null-Lage Kolben grob
			P2-> untere Endl. Kolben */
   if(I2CRead(0x20,1,&state) == FALSE) return IERR_I2C_HEAD_MAG;
   if(dir == SUCK)
   {
	if(state&0x20)
	{
		return IERR_PST_UPPER_END_POS;
	}
   }
   else
   {if(state&0x04) return ERR_PST_LOWER_END_POS;}
   if(flg_watch_magazine)
   {if((state&0xc0) != 0xc0) return IERR_HEAD_ECC_OPEN;
	 state = PstReadMagazineInsert();
	 if(state == PST_MAG_NOT_AVAILABLE) return IERR_MAG_NOT_INSERT;
	 if(state == IERR_I2C_HEAD_MAG) return state;
   }
   return CB_READY;
 }


/* FUNCTION: 	PstDrvPreZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Suchen der Grob-Null
   RETURNS: 	Fehlermeldung bzw. CB_READY */
ubyte PstDrvPreZeroPos(void){
	ubyte state;
	state = PstReadCoarseZeroPos();
	if(state == PST_POS) return CB_READY;
	if(state == IERR_I2C_PST) return state;
	g_bPstDrvDir = !SUCK; g_lPstDrvSteps = g_pudtHeadDevPara->PstMaxWay * g_pudtHeadDevPara->PstDrvMicroGearRatio;
	PstDrvStart(m_pudtPstEndZk);
	while(1){
		WDO();
		state = PstReadCoarseZeroPos();
		if(state == PST_POS) break;
		if(state == IERR_I2C_PST){
			PstDrvAbort();
			return state;
		}
		if((state = PstWatchHead(g_bPstDrvDir,TRUE)) != CB_READY){
			PstDrvAbort();
			return state;
		}
		if(ReadAbortConditions()){
			PstDrvAbort();
			return IERR_PROG_ABORT;
		}

		if (tmc5130a_is_stop(P_DEVICE) == TRUE){
			g_ubytPstDrvState = CB_READY;
		}
		if(g_ubytPstDrvState == CB_READY){
			PstDrvSetCurrentToZero();
			return IERR_PST_ZERO_POS;
		}
	}
	PstDrvAbort();
	PstDrvSetHoldCurrent();
	return CB_READY;
}

/* FUNCTION: 	PstDrvToZero
   PARAMETERS:	---
   DESCRIPTION:	Suchen der Fein-Null,Voraussetzung: Grob-Null wurde schon angefahren
   RETURNS: 	Fehlermeldung bzw. CB_READY */
ubyte PstDrvToZero(void){
	float save_speed = m_udtPstRevAct.EndReduce;
	ubyte state;

	if(g_pudtHeadType->type == OL3387_8) return CB_READY;
	tmc5130a_VELOCITY(P_DEVICE, M1_SpU *uSpS*100/60);
	g_bPstDrvDir = SUCK; g_lPstDrvSteps = g_pudtHeadDevPara->PstMaxWay/2 * g_pudtHeadDevPara->PstDrvMicroGearRatio;
	// Eine Abfrage des Kopfzustandes zum Start sollte reichen
	if((state = PstWatchHead(g_bPstDrvDir,TRUE))!= CB_READY) return state;
	PstDrvStart(m_pudtPstEndZk);
	// Verlassen der Grob-Null
	while(1){
		WDO();
		state = PstReadCoarseZeroPos();
		if(state != PST_POS) break;
		if(state == IERR_I2C_PST){
			PstDrvAbort();
			return state;
		}
		if(ReadAbortConditions()){
			PstDrvAbort();
			return IERR_PROG_ABORT;
		}
	}
	PstDrvAbort(); //Hat gefehlt?
	if (!(g_pudtHeadType->type == OL5021_1) || (g_pudtHeadType->type == OL5021_12)){
		// Weg bis zum Suchen der Fein-Null einstellen
		PstDrvDisableInt;
		// der Einlaufweg im Start/Stopp sollte etwa 7...8mm betragen
		if((g_pudtHeadType->type == OL3384_2)||(g_pudtHeadType->type == OL3384_1)||(g_pudtHeadType->type == OL3384_15)){	// 2,5 µl-Kopf,1536 bzw. 384/8µl-Kopf
			g_lPstDrvSteps = (long)1400;	// ist von der Justage abhängig sollte in den Kopfparameter abgelegt werden
		}else{
			g_lPstDrvSteps = (long)2800;	// ist von der Justage abhängig
		}
		PstDrvEnableInt;
		while(g_ubytPstDrvState == RESET){	// Motor läuft noch
			if (tmc5130a_is_stop(P_DEVICE) == TRUE){
				g_ubytPstDrvState = CB_READY;
			}
			WDO();
			if(ReadAbortConditions()){
				PstDrvAbort();
				return IERR_PROG_ABORT;
			}
		}
	}
	tmc5130a_VELOCITY(P_DEVICE, M1_SpU *uSpS*6/60);
	// Suchen der Fein-Null im Start/Stop-Mode
	g_lPstDrvSteps = m_fPstWaySearchFinePos * g_pudtHeadDevPara->PstDrvMicroGearRatio;
	PstDrvStart(g_audtPstZk);
	while(1){
		WDO();
		state = PstReadFineZeroPos();
		if(state == PST_POS) break;
		if(state == IERR_I2C_PST){
			PstDrvAbort();
			return state;
		}
		if(ReadAbortConditions()){
			PstDrvAbort();
			return IERR_PROG_ABORT;
		}
	}
	PstDrvAbort();
	tmc5130a_VELOCITY(P_DEVICE, M1_SpU *uSpS*save_speed/60);
	g_bPstFirstSuck = TRUE;
	g_bFirstDisp = TRUE;
	g_udtLifePara.PstCycleVolume = 0;
	state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
	PstDrvSetHoldCurrent();
	g_ubytDrvInitState &= 0xf7;
	m_fPstDrvWay = 0;
	VolumeCorrection(VC_MODE_RESET,NULL,NULL);
	return CB_READY;
}

/* FUNCTION: 	Suck
   PARAMETERS:	volume --> Ansaugvolumen
   DESCRIPTION:	Ansaugen des Volumens
   RETURNS: 	Fehlermeldung bzw. CB_READY */
ubyte Suck(float volume){
	ubyte err, state;
	long l_steps; float f_steps,help;
	if(g_pudtHeadType->type == OL3387_8) return CB_READY;
	if(volume < g_pudtTipsPara->MinVol)  return IERR_PARA;
	g_udtLifePara.PstCycleVolume += volume;
	state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
	help = g_pudtTipsPara->MaxVol + g_pudtTipsPara->MoreSuck - g_udtLifePara.PstCycleVolume;
	if((help < -ROUNDING_ERROR)||(volume <= 0)) {
		g_udtLifePara.PstCycleVolume -= volume;
		state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
		return IERR_PARA;}
	if(g_pudtHeadType->pudtVolCorr == NULL){
		if(g_bPstFirstSuck == TRUE){	//wird beim Anfahren des Nullpunktes auf wahr gesetzt
			f_steps	 = (g_pudtTipsPara->asc * volume + g_pudtTipsPara->zero) * g_pudtHeadDevPara->PstGearRatio; g_bPstFirstSuck = FALSE;
		}else{
			f_steps = g_pudtTipsPara->asc * volume * g_pudtHeadDevPara->PstGearRatio+g_pudtTipsPara->zero;
		}
	}else{
		if(g_VolumeCorrection == VC_INTERN){
			if(!VolumeCorrection(VC_MODE_DOSIEREN,&volume,g_pudtHeadType->pudtVolCorr)){
				return IERR_VOLUME_CORRECTION;
			}
		}
		f_steps = volume * g_pudtHeadDevPara->PstGearRatio;
	}
	l_steps = (long)f_steps;
	if(g_bTestFlag){
		printf("Volume:%f,Steps:%ld\r",volume,l_steps);
		fflush(stdout);
	}
	if((f_steps-l_steps) > 0.5) l_steps += 1;
	err = PstDrv(SUCK,TRUE,TRUE,l_steps);
	ErrorCheck(err);
	return err;
}

/*
ubyte FrankSuck(float volume){
	ubyte err,fehler;
	long l_steps; float f_steps,help;
	g_udtLifePara.PstCycleVolume += volume;
	help = g_pudtTipsPara->MaxVol + g_pudtTipsPara->MoreSuck - g_udtLifePara.PstCycleVolume;
	if((help < -ROUNDING_ERROR)||(volume <= 0)){
		g_udtLifePara.PstCycleVolume -= volume;
		return IERR_PARA;
	}
	fehler = FALSE;
	printf("   Volume:%f,\r",volume);
	fflush(stdout);
	if(!VolumeCorrection(VC_MODE_DOSIEREN,&volume,g_pudtHeadType->pudtVolCorr)){
		fehler = TRUE;
	}
	printf("VC Volume:%f,\r",volume);
	fflush(stdout);
	if(fehler) return ERR_FP;
	return CB_READY;
}
*/

/* FUNCTION: 	SuckWithOverstroke
   PARAMETERS:	volume --> Ansaugvolumen
   DESCRIPTION:	Ansaugen des Volumens plus überhubvolumen
				anschließend Ausstossen des überhubvolumens (Lose der Kolben beseitigen)
   RETURNS: 	Fehlermeldung bzw. CB_READY */
ubyte SuckWithOverstroke(float volume){
	ubyte err, state; long l_steps;
	float pst_play,help,current_volume;
	if(g_pudtHeadType->type == OL3387_8) return CB_READY;
	if(g_bTestFlag) {printf("SuckOver: %f\r",volume);
	fflush(stdout);}
	if(volume < g_pudtTipsPara->MinVol) return IERR_PARA;

 	g_udtLifePara.PstCycleVolume = g_udtLifePara.PstCycleVolume + volume;
 	state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
 	help = g_pudtTipsPara->MaxVol + g_pudtTipsPara->MoreSuck - g_udtLifePara.PstCycleVolume;
 	if((help < -ROUNDING_ERROR)||(volume <= 0)){
 		g_udtLifePara.PstCycleVolume -= volume;
 		state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
 		return IERR_PARA;
 	}
	pst_play = g_pudtHeadDevPara->PstUpperPlay * g_pudtHeadDevPara->PstDrvMicroGearRatio;

	if(g_pudtHeadType->pudtVolCorr == NULL){
		//l_steps = ((g_pudtTipsPara->asc*volume+g_pudtTipsPara->zero) * g_pudtHeadDevPara->PstGearRatio) + pst_play;
		current_volume = g_pudtTipsPara->asc*volume+g_pudtTipsPara->zero;
	}else{
		if(g_VolumeCorrection == VC_INTERN){
			if(!VolumeCorrection(VC_MODE_PIPETTIEREN,&volume,g_pudtHeadType->pudtVolCorr)){
				return IERR_VOLUME_CORRECTION;
			}
		}
		//l_steps = (volume * g_pudtHeadDevPara->PstGearRatio) + pst_play;
		current_volume = volume;
	}

	current_volume += g_AddVolume;
	l_steps = (current_volume * g_pudtHeadDevPara->PstGearRatio) + pst_play;

	err = PstDrv(SUCK,TRUE,TRUE,l_steps);
	if(err != CB_READY){
		ErrorCheck(err);
		return err;
	}
	err = PstDrv(!SUCK,TRUE,TRUE,pst_play);
	ErrorCheck(err);
	return err;
}
/*
 ubyte FrankSuckWithOverstroke(float volume)
 {ubyte err,fehler; long l_steps;
   float pst_play,help;
   if(g_bTestFlag) {printf("SuckOver: %f\r",volume); fflush(stdout);}
   g_udtLifePara.PstCycleVolume += volume;
   help = g_pudtTipsPara->MaxVol + g_pudtTipsPara->MoreSuck - g_udtLifePara.PstCycleVolume;
   if((help < -ROUNDING_ERROR)||(volume <= 0)){g_udtLifePara.PstCycleVolume -= volume; return IERR_PARA;}
fehler = FALSE;
	printf("   Volume:%f,\r",volume); fflush(stdout);
	if(!VolumeCorrection(VC_MODE_PIPETTIEREN,&volume,g_pudtHeadType->pudtVolCorr))
	{
		fehler = TRUE;
	}
	printf("VC Volume:%f,\r",volume); fflush(stdout);
	if(fehler) return ERR_FP;
   return CB_READY;
 }
*/

/* FUNCTION: 	Dispense
   PARAMETERS:	volume --> Ausstossvolumen
				enable_abort --> Flag zur Abbrucherlaubnis
   DESCRIPTION:	Dispensieren eines Volumens in µl
   RETURNS: 	Fehlermeldung bzw. CB_READY */
ubyte Dispense(float volume,ubyte enable_abort){
	ubyte err, state; long l_steps;
	float f_steps,help,h_vol;
	float fVC_Volume;
	unsigned int tmp;
	/* Hier ist die falsche Stelle,da bei einem Ausschalten während des Ausstossen das Volumen
	  der Wirklichkeit voreilt,eine evtl. vorhandene Flüssigkeit in den Spitzen wird so nicht
	  erkannt */
	//PstCycleVolume -= volume;
	if(g_pudtHeadType->type == OL3387_8){
		if(volume == 0)
			tmp = VALVE_ON_TIME;
		else
			tmp = volume*1000/10;
		return OL3387_ValveCtrl(tmp);
	}
	h_vol = g_udtLifePara.PstCycleVolume -volume;
	state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
	if(g_bTestFlag){
		printf("Dispense: %f\r",volume);
		fflush(stdout);
	}
	//help = PstCycleVolume+g_pudtTipsPara->MoreDisp;
	help = h_vol+g_pudtTipsPara->MoreDisp;
	// Rundungsgenauigkeit unterdrücken,Test: Disp 3*0.1,Ansaugen 252
	//if((help < -ROUNDING_ERROR)||(volume <= 0)){g_udtLifePara.PstCycleVolume += volume; return IERR_PARA;}
	if((help < -ROUNDING_ERROR)||(volume <= 0)){
		h_vol += volume;
		return IERR_PARA;
	}
	g_udtLifePara.PstTotalVolume += (volume /1000);	//Gesamtvolumen über die Lebensdauer in ml
	state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
	/* Achtung!,alles in Gleitkomma rechnen !!! */
	if(g_pudtHeadType->pudtVolCorr == NULL){
		if (g_bFirstDisp){				// wird beim Anfahren des Nullpunktes auf wahr gesetzt
			f_steps = (g_pudtTipsPara->asc * volume + g_pudtTipsPara->zero) * g_pudtHeadDevPara->PstGearRatio; g_bFirstDisp = FALSE;
		}else{
			f_steps = (g_pudtTipsPara->asc * volume + g_pudtTipsPara->zero) * g_pudtHeadDevPara->PstGearRatio;
		}
	}else{
		fVC_Volume = volume;
		if(g_VolumeCorrection == VC_INTERN){
			if(!VolumeCorrection(VC_MODE_DISPENSIEREN,&fVC_Volume,g_pudtHeadType->pudtVolCorr)){
				return IERR_VOLUME_CORRECTION;
			}
		}
		f_steps = fVC_Volume * g_pudtHeadDevPara->PstGearRatio;
	}
	l_steps = (long)f_steps;
	if((f_steps-l_steps) > 0.5) l_steps += 1;
	err = PstDrv(!SUCK,TRUE,enable_abort,l_steps);
	if(err == CB_READY)
	{
		g_udtLifePara.PstCycleVolume -= volume;
		state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
	}
	ErrorCheck(err);
	return err;
}

/*
 ubyte FrankDispense(float volume,ubyte enable_abort)
 {ubyte err,fehler; long l_steps; float f_steps,help,h_vol;
	//PstCycleVolume -= volume;
   h_vol = g_udtLifePara.PstCycleVolume -volume;
   if(g_bTestFlag) {printf("Dispense: %f\r",volume); fflush(stdout);}
	//help = PstCycleVolume+g_pudtTipsPara->MoreDisp;
   help = h_vol+g_pudtTipsPara->MoreDisp;
	// Rundungsgenauigkeit unterdrücken,Test: Disp 3*0.1,Ansaugen 252
	//if((help < -ROUNDING_ERROR)||(volume <= 0)){g_udtLifePara.PstCycleVolume += volume; return IERR_PARA;}
   if((help < -ROUNDING_ERROR)||(volume <= 0)){h_vol += volume; return IERR_PARA;}
   g_udtLifePara.PstTotalVolume += (volume /1000);	//Gesamtvolumen über die Lebensdauer in ml

	// Achtung!,alles in Gleitkomma rechnen !!!
fehler = FALSE;
	printf("   Volume:%f,\r",volume); fflush(stdout);
	if(!VolumeCorrection(VC_MODE_DISPENSIEREN,&volume,g_pudtHeadType->pudtVolCorr))
	{
		fehler = TRUE;
	}
	printf("VC Volume:%f,\r",volume); fflush(stdout);
	if(fehler) return ERR_FP;
   return CB_READY;
 }
*/

/* FUNCTION: 	SuckWithoutTips
   PARAMETERS:	---
   DESCRIPTION:	Anfahren der oberen Endlage bzw. des erforderlichen Weges ohne
	 			Unterbrechungsmöglichkeit
   RETURNS: 	Fehlermeldung bzw. CB_READY */
ubyte SuckWithoutTips(void){
	ubyte state;
	float pst_rev;
	if(PstReadUpperPos() ==  PST_POS) return CB_READY;
	if(g_ubytFlgTCMAXWay){	// ab Version 1.44
		// Geschwindugkeit reduzieren,da der überlaufweg in der oberen Endlage zu kurz ist
		// Fahren bis zur oberen Endlage
		pst_rev = m_udtPstRevAct.EndReduce;	// retten der akt. Geschwindigkeit
		PstSpeedLimit(PST_TC_RPM);
		state = PstDrv(SUCK,FALSE,FALSE,g_pudtHeadDevPara->PstMaxWay * g_pudtHeadDevPara->PstDrvMicroGearRatio);
		PstSpeedLimit(pst_rev);
		if(state == IERR_PST_UPPER_END_POS){
			g_ubytFlgTCMAXWay = FALSE;
			/*PstDrvSetHoldCurrent();*/
			state = CB_READY;
		}else
			/* Obere Endlage muß erreicht werden!,alles andere sind Fehler (auch CB_READY) */
			state = IERR_PST_NOT_UPPER_POS;
	}else
		state = PstDrv(SUCK,FALSE,FALSE,g_pudtHeadDevPara->TCWay * g_pudtHeadDevPara->PstDrvMicroGearRatio);
	PstDrvSetHoldCurrent();
	return CB_READY;
}

/* FUNCTION: 	PstDrvCalibrate
   PARAMETERS:	---
   DESCRIPTION:	Kalibrierung des Kolbenantriebes
   RETURNS: 	Fehlermeldung bzw. CB_READY */
ubyte PstDrvCalibrate(void){
	ubyte state,err;
	err = CB_READY;
	if(g_pudtHeadType->type == OL3387_8) return CB_READY;
	state = PstReadCoarseZeroPos();
	if(state == IERR_I2C_PST)	return state;
	if(state == PST_NOT_POS) err = PstDrvPreZeroPos();
	if(err != CB_READY) return err;
	return PstDrvToZero();
}

/*
ubyte FrankPstDrvCalibrate(void)
{ubyte state,err;
  err = CB_READY;
   g_bPstFirstSuck = TRUE; g_bFirstDisp = TRUE; g_udtLifePara.PstCycleVolume = 0;
   g_ubytDrvInitState &= 0xf7;

	VolumeCorrection(VC_MODE_RESET,NULL,NULL);
	return err;
}
*/

/* FUNCTION: 	PstReadLowerPos
   PARAMETERS:	---
   DESCRIPTION:	Lesen des unteren Endlagesensors des Kolbenantriebes
   RETURNS: 	Status bzw.Fehlermeldung */
ubyte PstReadLowerPos(void){
	return PstReadEndPos(!SUCK);
}

/* FUNCTION: 	PstReadUpperPos
   PARAMETERS:	---
   DESCRIPTION:	Lesen des oberen Endlagesensors des Kolbenantriebes
   RETURNS: 	Status bzw.Fehlermeldung */
ubyte PstReadUpperPos(void){
	return PstReadEndPos(SUCK);
}

/* FUNCTION: 	TestShowHeadPara - Serviceroutine
   PARAMETERS:	Pointer auf die auszugebende Geräteparameterstruktur
   DESCRIPTION:	Ausgabe der	Geräteparameterstruktur
   RETURNS: 	--- */
void TestShowHeadPara(struct HEAD_DEV *ptr){
	printf("Head device parameter\r");
	printf("PstDrvHoldCurrent:	  %f\r",(double)ptr->PstDrvHoldCurrent);
	printf("PstMaxWay:		   %f\r",(double)ptr->PstMaxWay);
	printf("PstUpperPlay:		%f\r",(double)ptr->PstUpperPlay);
	printf("PstGearRatio:		%f\r",(double)ptr->PstGearRatio);
	printf("PstDrvMicroGearRatio:%f\r",(double)ptr->PstDrvMicroGearRatio);
	printf("Volumeneingabeformat:%d\r", (int)ptr->VolFrmt);
	fflush(stdout);
}

/* FUNCTION: 	PstDrvLeavePreZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Testprogramm - Verlassen der Grobnull
   RETURNS: 	CB_READY / ERROR */
ubyte PstDrvLeavePreZeroPos(void){
	ubyte err;
	if((err = PstWatchHead(SUCK,FALSE)) != CB_READY) return err;
	g_bPstDrvDir = SUCK;
	g_lPstDrvSteps = 20 * g_pudtHeadDevPara->PstDrvMicroGearRatio;
	PstDrvStart(g_audtPstZk);	// Start/Stopp-Modus	//m_pudtPstEndZk
	while(PstReadCoarseZeroPos() != PST_NOT_POS){
		WDO();
		if (tmc5130a_is_stop(P_DEVICE) == TRUE){
			g_ubytPstDrvState = CB_READY;
		}
		if(g_ubytPstDrvState == CB_READY) {PstDrvAbort();
		return IERR_PST_ZERO_POS;}
		if((err = PstWatchHead(SUCK,FALSE)) != CB_READY) {PstDrvAbort(); return err;}
		if(PstReadCoarseZeroPos() == PST_NOT_POS) break;	// Grob-Null verlassen
		if(ReadAbortConditions()) {PstDrvAbort(); return IERR_PROG_ABORT;}
	}
	tmc5130a_STOP(P_DEVICE);
	PstDrvSetHoldCurrent(); return CB_READY;
}

/* FUNCTION: 	PstReadEccentricState
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Zustandes Spitzenanzug
				Bit 6 und 7: Lose (Gabelkoppler abgedeckt):0 0
							 Fest (Gabelkoppler offen): 1 1
   RETURNS: 	Zustand des Spitzenanzugsmechanismus - fest/lose
				bzw. Fehler in der Kommunikation durch I2C */
ubyte PstReadEccentricState(void){
	ubyte state;
	if(I2CRead(0x20,1,&state) == FALSE) return IERR_I2C_ECC;
	state = state&0xc0;
	if(state == 0xc0) return PST_ECCENTRIC_TIGHTEN;
	if(state == 0x00) return PST_ECCENTRIC_LOOSE;
	return PST_ECCENTRIC_NOT_TIGHTEN;	// Exzenter nicht gespannt
}

ubyte IsPstEccentricTighten(void){
	if(PstReadEccentricState()  == 	PST_ECCENTRIC_TIGHTEN) return TRUE;
	return FALSE;
}

/* FUNCTION: 	PstReadEndPos
   PARAMETERS:	obere (SUCK) / untere (!SUCK)
   DESCRIPTION:	Lesen der Kolbenendlage
   RETURNS: 	Zustand der Endlage bzw. Fehler in der Kommunikation durch I2C */
ubyte PstReadEndPos(ubyte dir){
	ubyte state;
	if(I2CRead(0x20,1,&state) == FALSE) return IERR_I2C_PST;
	if(dir == SUCK){if(state&0x20) return PST_POS;}
	else {if(state&0x04) return PST_POS;}
	return PST_NOT_POS;
}

/* FUNCTION: 	PstReadCoarseZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Grob-Null
   RETURNS: 	Zustand der Grob-Null bzw. Fehler in der Kommunikation durch I2C */
 ubyte PstReadCoarseZeroPos(void){
	 ubyte state;
	 if(I2CRead(0x20,1,&state) == FALSE) return IERR_I2C_PST;
	 if(state&0x08) return PST_POS;
	 return PST_NOT_POS;
}

/* FUNCTION: 	PstReadFineZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Fein-Null
   RETURNS: 	Zustand der Fein-Null bzw. Fehler in der Kommunikation durch I2C */
ubyte PstReadFineZeroPos(void){
	ubyte state;
	if(I2CRead(0x20,1,&state) == FALSE) return IERR_I2C_PST;
	if(state&0x10) return PST_NOT_POS;	// Low --> Schlitz im Koppler
	return PST_POS;
}

/* FUNCTION: 	PstReadMagazineInsert
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Magazinposition
   RETURNS: 	Zustand des Magazins - vorhanden/ nicht vorhanden
				bzw. Fehler in der Kommunikation durch I2C */
ubyte PstReadMagazineInsert(void){
	ubyte state;
	if(I2CRead(0x22,1,&state) == FALSE) return IERR_I2C_HEAD_MAG;
	if(state&0x02) return PST_MAG_NOT_AVAILABLE;
	return PST_MAG_AVAILABLE;	// Magazin vorhanden
}

/* FUNCTION: 	PstReadMagazinePara
   PARAMETERS:	Zeiger auf die zu speichernde Magazinkodierung
   DESCRIPTION:	Lesen der Magazinparameter
   RETURNS: 	CB_READY / IERR_MAG_NOT_INSERT,IERR_I2C_MAG_PARA */
ubyte PstReadMagazinePara(ubyte *para){
	ubyte state,ret_state;
	ret_state = CB_READY;
	if(I2CRead(0x22,1,&state) == FALSE) return IERR_I2C_HEAD_MAG;
/*
	if(state&0x02) return IERR_MAG_NOT_INSERT;			//PST_MAG_NOT_AVAILABLE;
	*para = (state>>2)&0x07;  */
	if(state&0x02) ret_state = IERR_MAG_NOT_INSERT;
	*para = (state>>2)&0x07;
	return ret_state;  										//PST_MAG_AVAILABLE;  Magazin vorhanden
}


/* FUNCTION: 	IncrementAndSaveCyclesToHead
   PARAMETERS:	---
   DESCRIPTION:	Abspeichern der Lebenszyklusdaten Volumen und Kolbenzyklen im Kopf
				(erfolgt zyklisch an Hand der Kolbenzyklen in PstDrv())
   RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W */
ubyte IncrementAndSaveCyclesToHead(void){
	static int CycleCnt = 0;
	ubyte state;
	state = CB_READY;

	g_udtLifePara.ulngPistonCycles++; CycleCnt++;
	state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
	if(CycleCnt == CYCLE_CNT){
		// Abspeichern der aktuellen Lebensdaten im Kopf
		state = ErrorDisplay(FRAMWritePstVolume());
		if(state == CB_READY){
			state = ErrorDisplay(FRAMWritePstCycles());
			if(state == CB_READY) CycleCnt = 0;
		}
	}
	return state;
}

/* FUNCTION: 	ReadAndSetCartridgeTyp
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Magazinparameter,die beim Hummingwell zur Kodierung 96 bzw. 384 verwendet werden
   RETURNS: 	CB_READY / IERR_MAG_NOT_INSERT,IERR_I2C_MAG_PARA */
ubyte ReadAndSetCartridgeTyp(void){
	ubyte state,magazine_code;
	state = PstReadMagazinePara(&magazine_code);
	if(state == CB_READY){
		if(magazine_code == HUMMINGWELL_MAG_384){
			// zu diesem Zeitpunkt ist bereits bekannt,das es der Hummingwell-Kopf ist,deshalb direkter Zugriff möglich
			HeadDev_OL3387_8.PstCnt = 384;
		}else if(magazine_code == HUMMINGWELL_MAG_96){
			HeadDev_OL3387_8.PstCnt = 96;
		}else{	//Unbekannte Cartridge
			state = IERR_MAG_UNKNOWN;
		}
	}
	return state;
}

/* FUNCTION: 	OL3387_ValveCtrl
   PARAMETERS:	Zeit in 10ms
   DESCRIPTION:	Steuerung des Magnetventils vom Hummingwell-Kopf
   RETURNS: 	CB_READY / IERR_I2C_PST / IERR_PROG_ABORT */
ubyte OL3387_ValveCtrl(unsigned int time){
	ubyte state;
	state = OL3387_ValveOnOff(VALVE_ON);
	if(state == CB_READY){
		g_uwWaitTime = time;	//VALVE_ON_TIME;
		while(g_uwWaitTime){
			WDO();
			if(ReadAbortConditions()) break;
		}
		state = OL3387_ValveOnOff(!VALVE_ON);
		if(g_uwWaitTime != 0) state = IERR_PROG_ABORT;
	}
	return state;
}

/* FUNCTION: 	OL3387_ValveOnOff
   PARAMETERS:	state (on/off)
   DESCRIPTION:	Steuerung des Magnetventils vom Hummingwell-Kopf
  				Achtung,Bit 0 und 1 steuern den Spitzenanzugsmotor,Portpin P4 (XS4)
				dient der Ventilsteuerung,der Rest sind Eingänge,die high bleiben müssen
				ein Low-Signal an P4 schaltet das Ventiel ein,in dieser Phase wird der
				Spitzenanzugsmotor jedoch nicht verwendet
   RETURNS: 	CB_READY / IERR_I2C_PST */
ubyte OL3387_ValveOnOff(ubyte state){
	ubyte ctrl,status;
	status = I2CRead(0x20,1,&ctrl);
	if(status){		// Einschalten des Ventiles
		if(state == VALVE_ON){
			ctrl &= (ubyte)~0x10;	//P4 = 1,Einschalten
		}else{
			ctrl |= 0x10;	  	//P4 = 1,Ausschalten
		}
		status = I2CWrite(0x20,1,&ctrl); // ValveOnOff
	}
	if(status) status = CB_READY; else status = IERR_I2C_PST;
	return status;
}

ubyte PstSetDrvParaToHost(void){
	ubyte state;
	if((g_pudtHeadType->type == OL5021_1) || (g_pudtHeadType->type == OL5021_12) ||(g_pudtHeadType->type == OL3384_15)){	//1536-Kopf und 384/8µl-Kopf fährt mit reduzierter Geschwindigkeit!
		state = PstCalcDrvPara(&g_udtPstRevDef_1536);
		if(state == CB_READY)
			state = PstSpeedLimit(g_udtPstRevDef_1536.EndReduce);
	}else{
		state = PstCalcDrvPara(&g_udtPstRevDef);
		if(state == CB_READY)
			state = PstSpeedLimit(g_udtPstRevDef.EndReduce);
	}
	return state;
}

ubyte PstSetDrvParaToDefault(void){
	if((g_pudtHeadType->type == OL5021_1)||(g_pudtHeadType->type == OL5021_12) ||(g_pudtHeadType->type == OL3384_15)){	//1536-Kopf und 384/8µl-Kopf fährt mit reduzierter Geschwindigkeit!
		PstSetRevParaToDefault(&g_udtPstRevDef_1536);
	}else{
		PstSetRevParaToDefault(&g_udtPstRevDef);
	}
	return PstInitZkMan();
}

ubyte PstCmpRevParaToDefault(void){
	const struct REV_PARA *ptr;
	if((g_pudtHeadType->type == OL5021_1) || (g_pudtHeadType->type == OL5021_12) ||(g_pudtHeadType->type == OL3384_15))
		ptr = &g_udtPstRevDef_1536;
	else
		ptr = &g_udtPstRevDef;

	if((m_udtPstRevAct.Start == ptr ->Start )&&(m_udtPstRevAct.End == ptr->End )&&(m_udtPstRevAct.cnt == ptr->cnt)
		&&(m_udtPstRevAct.EndReduce == ptr->EndReduce)) return TRUE;
	return FALSE;
}

