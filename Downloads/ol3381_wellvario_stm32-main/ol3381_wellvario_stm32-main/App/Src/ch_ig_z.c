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
#include <stdbool.h>

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Base
#include "ch_ig_cmd.h"
// ---------- Inc
#include <_tmc5130a.h>
#include <_encoder.h>
// ---------- Master
#include "ch_ig_eeprom.h"
#include "ch_ig_err.h"
#include "ch_ig_key.h"
#include "ch_ig_peri.h"
#include "ch_ig_utility.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"
#include "zk_calc.h"
// ---------- Pipettor_Allgemein/VolumenKorrektur
// ---------- Pipettor_Allgemein
#include "Well_Defines.h"
#include "Well_Error.h"

extern ubyte g_ubytHrzDrvState;

/** local definitions **/
#define UPWARDS 			FALSE
#define VERT_DRV_REPEAT_CNT	4			// Anzahl der Wiederholungen bei der Positionierung //TODO: maybe make this higher

#define VERT_ZK_DIM 		600			// Feldlänge für Anlaufkurve
#define VERT_DEF_START_RPM	1			// 10 Standardstartdrehzahl (Umdr/min)
#define VERT_DEF_END_RPM	140			// Standardenddrehzahl (Umdr/min)
#define VERT_DEF_COUNTS 	300			// Anlauflänge der SM-Kennlinie,muss kleiner als  HRZ_ZK_DIM sein
#define VERT_RPM_MAX 		140			// max. mögliche Drehzahl (Umdr/min)
#define VERT_RPM_MIN 		1			// min. mögliche Drehzahl (Umdr/min)
#define VERT_ACC_MAX 		VERT_ZK_DIM	// max. mögliche Anlauflänge
#define VERT_ACC_MIN 		100			// min. mögliche Anlauflänge
#define VERT_DEF_HOLD_CURRENT 0.5		// 0.3	Haltestrom des Schrittmotors
#define VERT_DEF_MAN_REV 30	  			// bis Version 2.10: 60 Umdr/min
#define VERT_ADJ_REV 20					//20  rpmin
#define VERT_ZERO_SEARCH_WAY 115		// in mm
#define VERT_ZERO_SERCH_FINE_POS 8		// in mm
#define VERT_MIN_WAY 0					// in mm
#define VERT_OPTO_SWITCH_DIST 5			// in mm
#define VERT_DRV_PRECISION 	0.05		// geforderte Positioniergenauigkeit des SM-Antriebes
//#define VERT_MICROSTEPS_PER_REV 6400	// mku replaced: Mikroschritte pro Umdrehung
#define VERT_MICROSTEPS_PER_REV 51200	// Mikroschritte pro Umdrehung
// #define VERT_GEAR_RATIO 170.6667		// 1 Umdr = 37.5mm
#define VERT_MAX_HOLD_CURRENT 	840			//Haltestrom in mA (wird mit 0.7 bzw. 0.5 skaliert)
 	 // Z-Ausheber für CyBi-Well vario
#define VERT_MEASURE_TO_ENCODER_3383 (float)(37.5/1600)	// /1600 bei 400Imp übersetzungsverhältnis physikal. Maßeinheit zu Encoderimp.
#define VERT_STEPS_TO_MEASURE_3383 (float)(VERT_MICROSTEPS_PER_REV/37.5)// Mikrosteps pro Maßeinheit  6400/37.5mm
#define VERT_DEF_HOLD_CURRENT_3383 0.7			// 0.3Haltestrom des Schrittmotors Z-Antrieb
#define VERT_DEF_HOLD_CURRENT_POS0_3383 0.5	// Haltestrom in der Null-Lage des Z-Antriebes
#define VERT_XY_SWITCH_POS_3383 16				// in mm
#define VERT_MAX_WAY_3383 160					// in mm

	// Z-Ausheber vom CyBi-Well2000 im LHA
#define VERT_MEASURE_TO_ENCODER_3391 (float)(37.699/1600)	// /1600 bei 400Imp übersetzungsverhältnis physikal. Maßeinheit zu Encoderimp.
#define VERT_STEPS_TO_MEASURE_3391 (float)(VERT_MICROSTEPS_PER_REV/37.699)// Mikrosteps pro Maßeinheit  6400/37.5mm
#define VERT_DEF_HOLD_CURRENT_3391 0.5			// 0.3Haltestrom des Schrittmotors in %
#define VERT_DEF_HOLD_CURRENT_POS0_3391  0.3	// Haltestrom in der Null-Lage des Z-Antriebes
#define VERT_XY_SWITCH_POS_3391 48			// in mm
#define VERT_MAX_WAY_3391 87				// in mm
#define VERT_DRV_PRECISION_3391 0.05		// in mm

/** default settings **/

/** internal functions **/
void VertDrvStart(struct DRV_ZK *ptr);


/** external data **/
extern DispString_t str_go_on[];
extern DispString_t str_xy_go_esc[];
extern DispString_t str_rev_unit[];
extern DispString_t str_rev[];

/** public data **/
volatile int16_t VertEncoder;

	//Neu:
DispString_t str_stage_rev_big[]		 = 	{{1,1,"DREHZAHL TISCH"}
											,{1,1,"STAGE SPEED"}};
DispString_t str_upper_stage_pos[]		 = 	{{1,1,"Obere Tischh\357he festlegen"}
											,{1,1,"Adjust upper stage position"}};
DispString_t str_upper_stage_stored[]	 = 	{{1,1,"Manuell begrenzte Tischh\357he"}
											,{1,1,"Manual set max stage height"}};
DispString_t str_act_stage_pos[]		 = 	{{2,1,"Tischh\357he:"}
											,{2,1,"Stage height:"}};
DispString_t str_stage_init[]			 = 	{{1,1,"Neuinitialisierung des Aushebers"}
 											,{1,1,"New initialization of stage"}};

const char str_unit_mm[] = "mm";

struct DRV_ZK g_audtVertZk[VERT_ZK_DIM+1],*g_pudtVertEndZk,*g_pudtVertActZk,*g_pudtVertActEndZk;
struct DRV_ZK *g_pudtVertEndManZk;

	// Motorkennline Ausheber für den manuellen Betrieb,Kopie im EEPROM und aktuell
struct REV_PARA g_udtVertRevMan;

static const struct REV_PARA VertRevDef = {VERT_DEF_START_RPM,VERT_DEF_END_RPM,VERT_DEF_COUNTS,VERT_DEF_MAN_REV};
uword g_uwVertAccSteps;
float g_fVertDrvPrecision = VERT_DRV_PRECISION; 				 	// geforderte Positioniergenauigkeit CW vario
float g_fVertDrvMeasureToEncoderImpulse;	// = VERT_MEASURE_TO_ENCODER_3383; 		// Pysikalische Maßänderung/Anzahl der Encoderimpulse
float g_fVertDrvCurrentPos;
float g_fVertXYSwitchPos;	// = VERT_XY_SWITCH_POS_3383;
float g_fVertMaxWay = VERT_MAX_WAY_3383;	// wird im Nachhinein überschrieben,Wert ist aber für die Erstinbetriebnahme wichtig!

float g_fVertHeightCorr;		// Copy im EEPROM
float g_fVertManLimitWay;		// Copy im EEPROM

ubyte g_ubytVertDrvState;
short g_sVertDrvTabWay;
long g_lVertDrvSteps;
bool g_bVertDrvDir;

/** private data **/
static float m_fVertDrvHoldCurrentPos0;
static float m_fVertDrvHoldCurrent;	// = VERT_DEF_HOLD_CURRENT_3383;
static float m_fVertDrvStepsToMeasure;	// = VERT_STEPS_TO_MEASURE_3383;			// Mikroschritte pro Umdrehung/ Maßänderung
static struct REV_PARA m_udtVertRevAct;

/** private functions **/
void DisplayWay(DispString_t *str,float z);
void VertDrvSetHoldCurrent(float way);
ubyte VertDrvWayCtrl(struct DRV_ZK *speed);
float VertDrvConfigWay(void);

/* FUNCTION: 	GetDefXYSwitchHeight
   PARAMETERS:	---
   DESCRIPTION:	Rückgabe der Defaultumschalthöhe für XY-Tisch
   RETURNS: 	--- */
float GetDefXYSwitchHeightOL3383(void){
	return  VERT_XY_SWITCH_POS_3383;
}
float GetDefXYSwitchHeightOL3714(void){
	return  VERT_XY_SWITCH_POS_3391;
}

/* FUNCTION: 	VertDrvSetParaDefaults
   PARAMETERS:	---
   DESCRIPTION:	Setzen der notwendigen Antriebsparameter
   RETURNS: 	--- */
void VertDrvSetParaDefaults(void){
	if(g_uiDeviceType == 3714){	// in LHA wurde der CyBi-Well2000 Ausheber verwendet (Diversität der Ausheber sichern)
		g_fVertDrvMeasureToEncoderImpulse = VERT_MEASURE_TO_ENCODER_3391; 		// Pysikalische Maßänderung/Anzahl der Encoderimpulse
		m_fVertDrvStepsToMeasure = VERT_STEPS_TO_MEASURE_3391;			// Mikroschritte pro Umdrehung/ Maßänderung
		m_fVertDrvHoldCurrent = VERT_DEF_HOLD_CURRENT_3391;
		m_fVertDrvHoldCurrentPos0 = VERT_DEF_HOLD_CURRENT_POS0_3391;
		//g_fVertXYSwitchPos = VERT_XY_SWITCH_POS_3391;	wird im EEPROM gespeichert
		g_fVertMaxWay = VERT_MAX_WAY_3391;
		g_fVertDrvPrecision = VERT_DRV_PRECISION_3391;
	}else{
		g_fVertDrvMeasureToEncoderImpulse = VERT_MEASURE_TO_ENCODER_3383; 		// Pysikalische Maßänderung/Anzahl der Encoderimpulse
		m_fVertDrvStepsToMeasure = VERT_STEPS_TO_MEASURE_3383;			// Mikroschritte pro Umdrehung/ Maßänderung
		m_fVertDrvHoldCurrent = VERT_DEF_HOLD_CURRENT_3383;
		m_fVertDrvHoldCurrentPos0 = VERT_DEF_HOLD_CURRENT_POS0_3383;
		//g_fVertXYSwitchPos = VERT_XY_SWITCH_POS_3383; wird im EEPOM gespeichert
		g_fVertMaxWay = VERT_MAX_WAY_3383;
	}
}

/* FUNCTION: 	VertDrvWayXYCtrl
   PARAMETERS:	Weg in mm
   DESCRIPTION:	Verfahren des Weges unter Beachtung der XY-Position
   RETURNS: 	CB_READY bzw. Fehlermeldung */
ubyte VertDrvWayXYCtrl(float way){
	ubyte state;
	//if((way < g_fVertXYSwitchPos)&&(XYServoCheckZeroPos() != TRUE))
	if((way < g_fVertXYSwitchPos)&&((g_fServoOffsetX != 0)||(XYServoCheckZeroPos() != TRUE))){
		state = VertDrvWay(g_fVertXYSwitchPos);
		if(state != CB_READY) return state;
		XYServoWay(g_fServoOffsetX,0);
	}
	return VertDrvWay(way);
}

/* FUNCTION: 	VertDrvWayCtrl
   PARAMETERS:	Zeiger auf Endgeschwindigkeit
   DESCRIPTION:	Verfahren des Weges,der über  g_lVertDrvSteps voreingestellt ist
   RETURNS: 	CB_READY,IERR_PROG_ABORT,IERR_DRV_VERT */
ubyte VertDrvWayCtrl(struct DRV_ZK *speed)
{			// ab Version 1.45

	long lngActSteps;
	float StartWay,CurrentWay,diff_way;
	if(g_lVertDrvSteps != 0)
	{
		if(g_lVertDrvSteps > 0)
			g_bVertDrvDir = UPWARDS;
		else
		{
			g_lVertDrvSteps *= -1; //Betrag (wird in Richtungsfahrt entsprechend gesetzt
			g_bVertDrvDir = !UPWARDS;
		}
		StartWay = ReadVertDrvCurrentPos();
		VertDrvStart(speed);
		global_flag_counter_10ms = 2;
		while(g_ubytVertDrvState != CB_READY)
		{
			if(!global_flag_counter_10ms) //alle 10 ms abfragen
			{
				global_flag_counter_10ms = 2; //reaktivieren
				if (tmc5130a_is_stop(VERT_DEVICE))
				{
						g_ubytVertDrvState = CB_READY;
						return CB_READY;
				}
				lngActSteps = abs(tmc5130a_read_position(VERT_DEVICE));
				CurrentWay = ReadVertDrvCurrentPos();
				diff_way = lngActSteps/m_fVertDrvStepsToMeasure-fabs(StartWay-CurrentWay);
				if(diff_way > 0.4)   //Schleppfehler während des Fahren
				{
					VertDrvAbort(); //VertDrvRunBit(DRV_DISABLE);
					if(g_bTestFlag)
					{
						printf("diff:%4.2f\r",diff_way);
						fflush(stdout);
					}//
					return IERR_DRV_VERT;
				}
			}
			WDO();
			if(ReadAbortConditions())
			{
				VertDrvAbort();
				return IERR_PROG_ABORT;
			}
	 	}
	}
	return CB_READY;
}

/* FUNCTION: 	VertDrvWay
   PARAMETERS:	Wegposition
   DESCRIPTION:	Anfahren der Wegposition
				Die Höhenkorrektur "g_fVertHeightCorr" hat auf die manuelle Bedienung keinen Einfluss
   RETURNS: 	CB_READY bzw. IERR_PROG_ABORT,IERR_DRV_H,IERR_PARA */
ubyte VertDrvWay(float way){
	struct DRV_ZK *speed;
	ubyte state,i;
	if((way > g_fVertMaxWay )||(way < VERT_MIN_WAY)) return IERR_PARA;
	way += g_fVertHeightCorr;
	if(way < g_fVertXYSwitchPos) way -= g_fVertHeightCorr;	//trifft den Bereich der Null-Lage
	speed = g_pudtVertEndZk;
	for(i = 0; i<VERT_DRV_REPEAT_CNT; i++){
		ENCODER_update(); //Manuelles update der Encoder
		g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
		if(fabs(way - g_fVertDrvCurrentPos) < g_fVertDrvPrecision) //hier findet eine Überprüfung statt, ob richtig angefahren wurde
		{
			Wait(5);
			if(g_bTestFlag) printf("Way:%f\r",(double)g_fVertDrvCurrentPos);
			fflush(stdout);
			if(way == 0){
				VertDrvSetHoldCurrent(m_fVertDrvHoldCurrentPos0);
			}else{
				VertDrvSetHoldCurrent(m_fVertDrvHoldCurrent);
			}
			Wait(5);		// Zeit wird für die mechanische Stabilisierung benötigt
			if(g_bTestFlag){
				printf("Way after hold:%f\r",(double)((int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse));
				fflush(stdout);
			}
			return CB_READY;
			// geändert ab Version 3.51
			//g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// nochmaliges Lesen der Istposition
			//if(fabs(way - g_fVertDrvCurrentPos) <= g_fVertDrvPrecision) return CB_READY;
			//if(repcnt == 1) return IERR_DRV_VERT;
			//repcnt++;
		}
		// Im Folgenden erfolgt bei der Konvertierung ein abschneiden und kein Runden
		// da aber 1/10mm 17 Motorsteps sind kann man es verschmerzen
		g_lVertDrvSteps = (long)round((way - g_fVertDrvCurrentPos) * m_fVertDrvStepsToMeasure);
		if(g_bTestFlag){
			printf("cyc:%d,curr pos:%f,dest way:%f steps:%ld\r",(int)i,(double)g_fVertDrvCurrentPos,way,g_lVertDrvSteps);fflush(stdout);
		}
		state = VertDrvWayCtrl(speed);
		if(state == IERR_PROG_ABORT) {VertDrvSetCurrentToZero();
		return state;
		}
		if(state == IERR_DRV_VERT)	i++;	//Zyklenzahl wird verkürzt
		speed = g_audtVertZk;		// falls Weg nicht korrekt angefahren wurde,weiter im Start/Stop
	}
	VertDrvSetCurrentToZero();
	return IERR_DRV_VERT;
}

 /* FUNCTION: 	VertDrvCalibrate
   PARAMETERS:	---
   DESCRIPTION:	Kalibrierung des Vertikalantriebes
   RETURNS: 	CB_READY bzw. IERR_PROG_ABORT,ERR_VERT_ZERO,ERR_VERT_PRE_ZERO */
ubyte VertDrvCalibrate(void){
	float save_speed = m_udtVertRevAct.EndReduce;
	long VertImpOptoSwitchDist;
	VertImpOptoSwitchDist = VERT_OPTO_SWITCH_DIST * m_fVertDrvStepsToMeasure;
	g_ubytDrvInitState |= 0x02;	//Kennung unkalibriert
 	// Prüfe untere Endlage erreicht,wenn ja dann verlassen
	tmc5130a_VELOCITY(VERT_DEVICE, M2_SpU *uSpS*140/60); //140 RPM grob
	if(VertReadEndPos)
	{																// _Unten
		if(g_bTestFlag){
			printf("leave home position\r");
			fflush(stdout);
		}
		g_lVertDrvSteps = (long)(VERT_ZERO_SERCH_FINE_POS * m_fVertDrvStepsToMeasure);
		g_bVertDrvDir = UPWARDS;
		VertDrvStart(g_audtVertZk);
		while(g_ubytVertDrvState == RESET){
			if (tmc5130a_is_stop(VERT_DEVICE)){
				g_ubytVertDrvState = CB_READY;
			}
			WDO();
			if(ReadAbortConditions()){
				VertDrvAbort();
				return IERR_PROG_ABORT;
			}
			if(!(VertReadEndPos)){
			VertDrvRunBit(DRV_DISABLE);
				break;
			}
		}
		if(g_ubytVertDrvState == CB_READY){
			if (tmc5130a_is_stop(VERT_DEVICE)){
				g_ubytHrzDrvState = CB_READY;
			}
			VertDrvSetCurrentToZero();
			return IERR_VERT_ZERO;
		}
		if(g_bTestFlag){
			printf("home position was left\r");
			fflush(stdout);
		}
	}
	// Prüfe nVorwarnschalter schon erreicht,dann im Start / Stopp weiter
	if(!VertReadAdvancePos){
		// mit volle Geschwindigkeit und max. Weg nVorwarnschalter anfahren
		if(g_bTestFlag){
			printf("driving to pre-warning position with speed\r");
			fflush(stdout);
		}
		g_lVertDrvSteps = (long)((VERT_ZERO_SEARCH_WAY) * m_fVertDrvStepsToMeasure);
		g_bVertDrvDir = !UPWARDS;
		VertDrvStart(g_pudtVertEndZk);
		while(g_ubytVertDrvState == RESET){
			if (tmc5130a_is_stop(VERT_DEVICE)){
				g_ubytVertDrvState = CB_READY;
			}
			WDO();
			if(ReadAbortConditions()){
				VertDrvAbort();
				return IERR_PROG_ABORT;
			}
			if(VertReadAdvancePos){			// nVorwarnschalter erreicht
				break;
			}
		}
		//Stoppe nach erreichen der Grob null
		if(g_ubytVertDrvState == CB_READY) //DrvAbort hat stattgefunden
		{
			if (tmc5130a_is_stop(VERT_DEVICE)){
				g_ubytHrzDrvState = CB_READY;
			}
			VertDrvSetCurrentToZero();
			return IERR_VERT_PRE_ZERO;
		}
		VertDrvAbort(); //erst hier bleibt er stehen old: VertDrvDisableInt;
		// der Abstand der Optpkoppler ist ca. 6 mm
		g_lVertDrvSteps = VertImpOptoSwitchDist;
		VertDrvEnableInt;
		while(g_ubytVertDrvState == RESET){
			if (tmc5130a_is_stop(VERT_DEVICE)){
				g_ubytVertDrvState = CB_READY;
			}
			WDO();
			if(ReadAbortConditions()){
				VertDrvAbort();
				return IERR_PROG_ABORT;
			}
		}
		if(VertReadEndPos) return IERR_VERT_ZERO;	//irgendwie wurde die Null-Lage schon erreicht
	}
	//Null-Lage im Start / Stop -Mode Anfahren
	if(g_bTestFlag){
		printf(" driving to zero position with start/stop frequency\r");
		fflush(stdout);
	}
	g_lVertDrvSteps = (long)((VERT_ZERO_SERCH_FINE_POS) * m_fVertDrvStepsToMeasure);
	g_bVertDrvDir = !UPWARDS;
	tmc5130a_VELOCITY(VERT_DEVICE, M2_SpU *uSpS*1/60); //1 RPM fein
	VertDrvStart(g_audtVertZk);
	while(g_ubytVertDrvState == RESET){
		if (tmc5130a_is_stop(VERT_DEVICE)){
			g_ubytVertDrvState = CB_READY;
		}
		WDO();
		if(ReadAbortConditions()){
			VertDrvSetCurrentToZero();
			return IERR_PROG_ABORT;
		}
		if(VertReadEndPos){
			VertDrvAbort();//VertDrvRunBit(DRV_DISABLE);
			g_ubytDrvInitState &= 0xFD; /* Bit 1 = 0 */
			g_fVertDrvCurrentPos = 0;
			VertEncoder = 0;
			Wait(10);	// wegen mechanischer Stabilisierung
			VertDrvSetHoldCurrent(m_fVertDrvHoldCurrentPos0);
			tmc5130a_VELOCITY(VERT_DEVICE, M2_SpU *uSpS*save_speed/60); //Geschwindigkeit wiederherstellen
			return CB_READY;
		}
	}
	VertDrvSetCurrentToZero();
	return IERR_VERT_ZERO;
}

/* FUNCTION: 	VertDrvSetCurrentToZero
   PARAMETERS:	---
   DESCRIPTION:	Motorsrtom des Z-Antriebes auf 0 setzen
   RETURNS: 	--- */
void VertDrvSetCurrentToZero(void){
	VertDrvAbort();
	tmc5130a_HOLD_CURR_MAN(VERT_DEVICE, 0);
}

/* FUNCTION: 	VertDrvSetHoldCurrent
   PARAMETERS:	prozentualer Haltestrom
   DESCRIPTION:	Einstellen des Haltestromes Vertikalantrieb
   RETURNS: 	--- */
void VertDrvSetHoldCurrent(float curr){
	VertDrvAbort();
	tmc5130a_HOLD_CURR_MAN(VERT_DEVICE, curr*VERT_MAX_HOLD_CURRENT);
//	tmc5130a_HOLD_CURR_MAN(VERT_DEVICE, curr * 1000); //Ströme aus Parameter sind anders als die gemessenen
}

/* FUNCTION: 	VertDrvStart
   PARAMETERS:	Pointer auf die Endgeschwindigkeit
   DESCRIPTION:	Start des Vertikalantriebes
   RETURNS: 	void*/
void VertDrvStart(struct DRV_ZK *ptr){

	g_pudtVertActEndZk = ptr;						// struct {Schrittweite, Schrittdauer} zielwert
	g_pudtVertActZk = g_audtVertZk;					// struct {Schrittweite, Schrittdauer} startwert
	g_ubytVertDrvState = RESET; 					// "RESET" entspricht "läuft"
	if(g_bVertDrvDir == UPWARDS){
		g_lVertDrvSteps *= -1;
	}

	tmc5130a_POSITIONING_MODE(VERT_DEVICE);
	tmc5130a_HOMING(VERT_DEVICE);					// Position = 0 (equivalent für Schrittzähler = 0)
	tmc5130a_start_dest(VERT_DEVICE,g_lVertDrvSteps);
	tmc5130a_PIN_RUN(VERT_DEVICE); //freigabe Motor-Treiber
}
/* FUNCTION: 	VertDrvAbort
   PARAMETERS:	---
   DESCRIPTION:	Abbruch der Fahrroutine des Vertikalantriebes
   RETURNS: 	void*/
void VertDrvAbort(void){
	VertDrvDisableInt;
	if(g_lVertDrvSteps > (long)g_sVertDrvTabWay) g_lVertDrvSteps = (long)g_sVertDrvTabWay;
	VertDrvEnableInt;
	tmc5130a_STOP(VERT_DEVICE);
	g_ubytVertDrvState = CB_READY;
	g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
	//VertDrvSetCurrentToZero();
	tmc5130a_PIN_STOP(VERT_DEVICE); //Motor-Treiber sperren
}

/* FUNCTION: 	VertDrvBreak
   PARAMETERS:	---
   DESCRIPTION:	Abbremsen der Fahrroutine des Vertikalantriebes
   RETURNS: 	--- */
void VertDrvBreak(void){
	VertDrvDisableInt;
	if(g_lVertDrvSteps > (long)g_sVertDrvTabWay) g_lVertDrvSteps = (long)g_sVertDrvTabWay;
	VertDrvEnableInt;
	tmc5130a_STOP(VERT_DEVICE);
	g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
	VertDrvSetHoldCurrent(m_fVertDrvHoldCurrentPos0);
	tmc5130a_PIN_STOP(VERT_DEVICE); //Motor-Treiber sperren
}

/* FUNCTION: 	VertDrvCtrlStart
   PARAMETERS:	---
   DESCRIPTION:	Starten der Fahrroutine,ohne Warten auf das Ende
   RETURNS: 	--- */
void VertDrvCtrlStart(float way){	// z_drv_stg(int abs_koord)
	if(g_fVertDrvCurrentPos == way) return;
	g_lVertDrvSteps = (long)((way - g_fVertDrvCurrentPos) * m_fVertDrvStepsToMeasure);

	if(g_lVertDrvSteps > 0)
		g_bVertDrvDir = UPWARDS;
	else{
		g_bVertDrvDir = !UPWARDS;
		g_lVertDrvSteps = labs(g_lVertDrvSteps);
	}

	VertDrvStart(g_audtVertZk);
}

/* FUNCTION: 	VertCheckZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Prüfen auf Null-Lage des Aushebers
   RETURNS: 	TRUE / FALSE */
ubyte VertCheckZeroPos(void){
	return VertReadEndPos;
}

/* FUNCTION: 	VertDrvGetLimits
   PARAMETERS:	Pointer auf Zeichenkette,ZK muss groß genug sein,um alle Zeichen zu speichern!
   DESCRIPTION:	Bereitstellung der Grenzwerte der Horizontal-Antriebsparameter
   RETURNS: 	--- */
/*
void VertDrvGetLimits(char *zk){		//nicht kompatibel zu CyBi-Ctrl
	sprintf(zk,"I%4.1f %4.1f %u %u %4.1f %4.1f %d\r",
		(double)VERT_RPM_MIN,(double)VERT_RPM_MAX,(uword)VERT_ACC_MIN,(uword)VERT_ACC_MAX,
		(double)VertRevDef.Start,(double)VertRevDef.End,VertRevDef.cnt);
} */

void VertDrvGetLimits(char *zk){
	sprintf(zk,"I%d %d %u %u %d %d %d\r",
		(int)VERT_RPM_MIN,(int)VERT_RPM_MAX,(uword)VERT_ACC_MIN,(uword)VERT_ACC_MAX,
	  	ftoi(VertRevDef.Start),ftoi(VertRevDef.End),VertRevDef.cnt);
}

/* FUNCTION: 	VertDrvGetPara
   PARAMETERS:	Pointer auf Zeichenkette,ZK muss groß genug sein,um alle Zeichen zu speichern!
   DESCRIPTION:	Bereitstellung der aktuellen Geschwindigkeitsparameter des Vertikalantriebes
   RETURNS: 	--- */
/*
void VertDrvGetPara(char *zk){		//nicht kompatibel zu CyBi-Ctrl
	sprintf(zk,"I%4.1f %4.1f %u %4.1f\r",
	(double)m_udtVertRevAct.Start,(double)m_udtVertRevAct.End,m_udtVertRevAct.cnt,
	(double)m_udtVertRevAct.EndReduce);
} */

void VertDrvGetPara(char *zk){
	sprintf(zk,"I%d %d %u %d\r",
		ftoi(m_udtVertRevAct.Start),ftoi(m_udtVertRevAct.End) ,m_udtVertRevAct.cnt,
		ftoi(m_udtVertRevAct.EndReduce));
}

/* FUNCTION: 	VertConfigWayLimit
   PARAMETERS:	---
   DESCRIPTION:	Einstellen des max. Fahrweges im manuellen Betrieb
   RETURNS: 	--- */
void VertConfigWayLimit(void){
	float fhelp;
	float save;
	ubyte key;
	g_pudtVertEndManZk = VertDrvSpeedLimit(VERT_ADJ_REV);
	if(g_pudtVertEndManZk == NULL) ErrorDisplay(IERR_DRV_ZK_VERT);

	ClearAllAndDisplay(str_upper_stage_stored);		// ab Version 3.10
	Display(str_act_stage_pos); Display(str_xy_go_esc);

	DisplayWay(&str_act_stage_pos[g_ubytLang],g_fVertManLimitWay);
	key = KeyWaitForAny();
	if(key == S_KEY){
		BeepCtrl(BEEP_TIME);
		return;
	}

	save = g_fVertManLimitWay;
	ClearAllAndDisplay(str_upper_stage_pos);
	g_fVertManLimitWay = g_fVertMaxWay;
	if((fhelp = VertDrvConfigWay()) == 0)
		g_fVertManLimitWay = save;	// keine Änderung
	else
		g_fVertManLimitWay = fhelp;
	g_pudtVertEndManZk = VertDrvSpeedLimit(g_udtVertRevMan.EndReduce);
	ErrorDisplay(EEPROMSetVertManWayLimit());
}

/* FUNCTION: 	VertDrvConfigWay
   PARAMETERS:	---
   DESCRIPTION:	Festlegen des max. Fahrweges im manuellen Betrieb
   RETURNS: 	--- */
float VertDrvConfigWay(void){
	float old_way; ubyte key;
	old_way = 0;
	g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
	Display(str_act_stage_pos); Display(str_xy_go_esc);
	DisplayWay(&str_act_stage_pos[g_ubytLang],g_fVertDrvCurrentPos);
	while(tast_read()!= NO_KEYS) WDO();
	while(1){
		WDO();
		g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
		if ((old_way!= g_fVertDrvCurrentPos)){
			old_way = g_fVertDrvCurrentPos;
			DisplayWay(&str_act_stage_pos[g_ubytLang],g_fVertDrvCurrentPos);
		}
		key = tast_read();
		if(key == ENTER_KEY){
			BeepCtrl(BEEP_TIME);
			return g_fVertDrvCurrentPos;
		}
		if(key == S_KEY){
			BeepCtrl(BEEP_TIME);
			return 0;
		}
	}
}

/* FUNCTION: 	DisplayWay
   PARAMETERS:	---
   DESCRIPTION:	Anzeigen der aktuellen Höhe
   RETURNS: 	--- */
void DisplayWay(DispString_t *str,float way){
	ubyte col;
	char h_disp[20];
	col = strlen(str->message)+str->column+1;
	sprintf(h_disp,"%5.1f",way);
	strcat(h_disp,str_unit_mm); LcdShow(str->line,col,h_disp);
}

/* FUNCTION: 	VertConfManRev
   PARAMETERS:	---
   DESCRIPTION:	Einstellen der manuellen Aushebergeschwindigkeit
   RETURNS: 	--- */
void VertConfManRev(void){
	ClearAllAndDisplay(str_stage_rev_big);
	g_udtVertRevMan.EndReduce =
			ed_float(g_udtVertRevMan.EndReduce
			,g_udtVertRevMan.Start
			,g_udtVertRevMan.End
			,str_rev
			,str_rev_unit
			,1.0
			,10.0
			,"%3.0f"
			,SECOND_LINE);
	ErrorDisplay(VertSetrpmanSpeed());
	ErrorDisplay(EEPROMSetVertManMotPara());
}

/* FUNCTION: 	VertCalcDrvPara
   PARAMETERS:	Zeiger auf struct REV_PARA
   DESCRIPTION:	Kennlinienfeld des SM-Antriebes errechnen
   RETURNS: 	CB_READY / IERR_PARA */
ubyte VertCalcDrvPara(struct REV_PARA *rev){
	if(VertCheckDrivePara(rev) == FALSE) return IERR_PARA;
	if((g_uwVertAccSteps = CalcTimeTable(g_audtVertZk,rev->Start,rev->End,rev->cnt)) == 0) return IERR_DRV_ZK_VERT;
	g_pudtVertEndZk = g_audtVertZk+rev->cnt;
	CopyRevPara(&m_udtVertRevAct,rev);
	m_udtVertRevAct.EndReduce = m_udtVertRevAct.End;
	return CB_READY;
}

/* FUNCTION: 	VertDrvSpeedLimit
   PARAMETERS:	Enddrehzahl
   DESCRIPTION:	Reduzierung der Endgeschwindigkeit
   RETURNS: 	aktueller Zeiger,Null-Zeiger ist Fehler */
struct DRV_ZK* VertDrvSpeedLimit(float speed){
	float help;
	struct DRV_ZK *ptr;
	if((speed > m_udtVertRevAct.End)||(speed < m_udtVertRevAct.Start)) return NULL;
	help = (speed - m_udtVertRevAct.Start)/(m_udtVertRevAct.End-m_udtVertRevAct.Start)*(float)m_udtVertRevAct.cnt;
	ptr = g_audtVertZk+(int)help;
	m_udtVertRevAct.EndReduce = speed;
	tmc5130a_VELOCITY(VERT_DEVICE, M2_SpU *uSpS*speed/60);		//Umrechnung RPM auf µStep/s
	return ptr;
}

/* FUNCTION: 	VertDrvSpeedLimit
   PARAMETERS:
   DESCRIPTION:	Aktuelle GEschwindigkeit
   RETURNS: 	 Aktuelle GEschwindigkeit*/
float VertGetVelocity()
{
	return m_udtVertRevAct.EndReduce;
}

/* FUNCTION: 	VertCheckDrivePara
   PARAMETERS:	Startdrehzahl,Enddrehzahl,Anzahl der Schritte
   DESCRIPTION:	Prüfen der Antriebsparameter für die SM-Kennlinie
   RETURNS: 	TRUE / FALSE */
ubyte VertCheckDrivePara(struct REV_PARA *rev){
	if( (rev->Start < (float)VERT_RPM_MIN) || (rev->End > (float)VERT_RPM_MAX) || (rev->Start >=  rev->End)
		||(rev->cnt < (float)VERT_ACC_MIN) || (rev->cnt > (float)VERT_ACC_MAX))
		return FALSE;
	return TRUE;
}

/* FUNCTION: 	VertSetrpmanSpeed
   PARAMETERS:	---
   DESCRIPTION:	Einstellen der manuellen Parameter für den Vertikalantrieb
				(nach Verlassen des Hostmodes)
   RETURNS: 	CB_READY / IERR_PARA */
ubyte VertSetrpmanSpeed(void){
	g_pudtVertEndManZk = VertDrvSpeedLimit(g_udtVertRevMan.EndReduce);
	g_pudtVertEndZk = g_pudtVertEndManZk;
	if(g_pudtVertEndZk == NULL) return IERR_DRV_ZK_VERT;
	m_udtVertRevAct.EndReduce = g_udtVertRevMan.EndReduce;
	return CB_READY;
}

/* FUNCTION: 	VertDrvSetupHostSpeed
   PARAMETERS:	---
   DESCRIPTION:	Einstellen der Hostparameter für den Vertikalantrieb
   RETURNS: 	CB_READY / IERR_PARA */
ubyte VertDrvSetupHostSpeed(void){
	g_pudtVertEndZk = VertDrvSpeedLimit(g_udtVertRevMan.End);
	if(g_pudtVertEndZk == NULL) return IERR_PARA;
	return CB_READY;
}

/* FUNCTION: 	VertDrvManParaCheck
   PARAMETERS:	---
   DESCRIPTION:	überprüfung der manuell konfigurierbaren Parameter
   RETURNS: 	TRUE / FALSE */
 ubyte VertDrvManParaCheck(void){
	//if((VertManEndRpm > VertActEndRpm)||(VertManEndRpm < VertActStartRpm)) return FALSE;
	if(!VertCheckDrivePara(&g_udtVertRevMan)) return FALSE;
	if((g_fVertManLimitWay > g_fVertMaxWay )||(g_fVertManLimitWay < VERT_MIN_WAY))return FALSE;
	return TRUE;
}

/* FUNCTION: 	VertSetRevManToRevAct
   PARAMETERS:	---
   DESCRIPTION:	Manuelle Drehzahlparam. werden den aktuellen zugewiesen
   RETURNS: 	---	*/
void VertSetRevManToRevAct(void){
	CopyRevPara(&m_udtVertRevAct,&g_udtVertRevMan);
}

/* FUNCTION: 	VertSetRevDefToRevMan
   PARAMETERS:	---
   DESCRIPTION:	Default-Drehzahlparameter werden den Manuellen zugewiesen
   RETURNS: 	---	*/
void VertSetRevDefToRevMan(void){
	CopyRevPara(&g_udtVertRevMan,&VertRevDef);
	ErrorDisplay(EEPROMSetVertManMotPara());
}

/* FUNCTION: 	VertInitZkMan
   PARAMETERS:	---
   DESCRIPTION:	Berechnung des Kennlinienfeldes des Z-Antriebes mit Defaultwerten
   RETURNS: 	CB_READY / IERR_PARA */
ubyte VertInitZkMan(void){
	ubyte state;
	state = VertCalcDrvPara(&g_udtVertRevMan);
	if(state != CB_READY) return state;
	return VertSetrpmanSpeed();
}

/* FUNCTION: 	VertSetDrvFktToMan
   PARAMETERS:	---
   DESCRIPTION:	Rücksetzen der Vertikalantriebskennlinie zur Manuellen
   RETURNS: 	CB_READY / ERROR */
ubyte VertSetDrvFktToMan(void){
	ubyte state;
	if((g_udtVertRevMan.Start != m_udtVertRevAct.Start) || (g_udtVertRevMan.End != m_udtVertRevAct.End)
	|| (g_udtVertRevMan.cnt != m_udtVertRevAct.cnt)){
		state = VertCalcDrvPara(&g_udtVertRevMan);
		if(state != CB_READY) return state;
	}
	return VertSetrpmanSpeed();
}

/* FUNCTION: 	VertManAbortHandling
   PARAMETERS:	---
   DESCRIPTION:	Abbruchhandling Vertikalantrieb
   RETURNS: 	CB_READY / ERROR */
ubyte VertManAbortHandling(void){
	ubyte state;
	struct LCD_DISP dHelp;
	SaveDisplay(&dHelp);
	ClearAllAndDisplay(str_stage_init);
	DisplayAndEnter(str_go_on);
	g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
	if(g_fVertDrvCurrentPos > g_fVertXYSwitchPos){
		state = VertDrvWayXYCtrl(g_fVertXYSwitchPos-0.5);	// XY wird gleichzeitig auf 0-Position gesetzt
		if((state != CB_READY)&&(state != IERR_PROG_ABORT)) ErrorDisplay(state);
	}
	state = VertDrvCalibrate();
	RestoreDisplay(&dHelp);
	return state;
}

/* FUNCTION: 	ReadVertDrvCurrentPos
   PARAMETERS:	---
   DESCRIPTION:	Lesen des aktuellen Ausheberweges in mm
   RETURNS: 	Weg in mm */
float ReadVertDrvCurrentPos(void){
	ENCODER_update();
	return (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;
}
