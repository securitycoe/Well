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

#include <_tmc5130a.h>

#include "head_piston_tab.h"
#include "ch_ig_eeprom.h"
#include "ch_ig_err.h"
#include "ch_ig_hrz.h"
#include "ch_ig_hrz_pos.h"
#include "ch_ig_key.h"
#include "ch_ig_peri.h"
#include "ch_ig_utility.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"
#include "zk_calc.h"
#include "Well_Defines.h"
#include "Well_Error.h"

/** local definitions **/
#define HrzDrvDisableInt			T0IE = 0		//T0IC_T0IE = 0
#define HrzDrvEnableInt				T0IE = 1		//T0IC_T0IE = 1

#define HRZ_DEF_START_RPM 			6					// 10 Standardstartdrehzahl (Umdr/min)
#define HRZ_DEF_END_RPM				200					// 200 Standardenddrehzahl (Umdr/min)

#define HRZ_DEF_COUNTS 				500					// 500 Anlauflänge der SM-Kennlinie,muss kleiner als  HRZ_ZK_DIM sein
#define HRZ_ZK_DIM 					1200				// 1200 Feldlänge für Anlaufkurve
#define HRZ_DEF_COUNTS_45 			1200				// ab Vers. 3.26 von 700 auf 1200 Anlauflänge 4,5-Platzwagen der SM-Kennlinie,muss kleiner als  HRZ_ZK_DIM sein
#define HRZ_DEF_COUNTS_5 			1200				// ab Vers. 3.26 von 700 auf 1200 Anlauflänge 5-Platzwagen der SM-Kennlinie,muss kleiner als  HRZ_ZK_DIM sein

#define HRZ_RPM_MIN 				1					// 1 min. mögliche Drehzahl (Umdr/min)
#define HRZ_RPM_MAX 				250					// 250 max. mögliche Drehzahl (Umdr/min)

#define HRZ_ACC_MIN 				100					// 100 min. mögliche Anlauflänge
#define HRZ_ACC_MAX 				HRZ_ZK_DIM			// 1200 max. mögliche Anlauflänge

#define HRZ_DEF_HOLD_CURRENT		0.35				// Haltestrom des Schrittmotors
#define HRZ_DRV_PRECISION			0.1					// geforderte Positioniergenauigkeit des SM-Antriebes

														// 1600 [Imp/U] Encoderimpulse pro Umdrehung
														// 43.982 [mm/U]
														// 32 [µStep/Step] 										>>>>>>> New: 256 [µStep/Step]
//#define HRZ_MICROSTEPS_PER_REV	51200				// 51200 [µStep/U] Mikroschritte pro Umdrehung 			>>>>>>> New
#define HRZ_STEPS_TO_MEASURE		(float)51200/43.982	// 1164,11 [µStep/mm] Mikroschritte pro Maßeinheit		>>>>>>> New

//#define HRZ_MICROSTEPS_PER_REV	6400				// 6400 [µStep/U] Mikroschritte pro Umdrehung 			>>>>>>> Old
//#define HRZ_STEPS_TO_MEASURE		(float)6400/43.982	// 145.514 [µStep/mm] Mikroschritte pro Maßeinheit 		>>>>>>> Old
//#define HRZ_GEAR_RATIO			145.5				// 145.514 [µStep/mm] Mikroschritte pro Maßeinheit

#define HRZ_MEASURE_TO_ENCODER		43.982/1600			// 0,02748 [mm/Imp] Maßeinheit pro Encoderimp.

#define HRZ_MAX_WAY					605					// 560 max. möglicher Weg in mm,Vergrößerung durch Einsatz im FlexVario
#define HRZ_MAX_WAY_5				727.5				// 5-Platzwagen,max. möglicher Weg in mm
#define HRZ_MAX_WAY_45				525					// 4,5-Platzwagen
#define HRZ_MAX_WAY_3				386					// 3-Platzwagen
#define HRZ_MAX_WAY_35				410					// 3-Platzwagen LHA OL3714-14-940
#define HRZ_MIN_WAY					-60					// min.	möglicher Weg in mm	ab V4.60
#define HRZ_MIN_WAY_5				-27.5				// min.	möglicher Weg in mm
#define HRZ_MIN_WAY_45				-340				// min.	möglicher Weg in mm

#define HRZ_ZERO_SEARCH_WAY			250
#define HRZ_ZERO_SEARCH_WAY_45		480
#define HRZ_ZERO_LEAVE_WAY			640
#define HRZ_ZERO_LIMIT_WAY			20

#define HRZ_DRV_REPEAT_CNT			4					// Anzahl der Wiederholungen bei der Positionierung

/** default settings **/

 //#define MAX_SPOT_4_384D 50
#define HRZ_ZERO_DIFF_STD			-132

 // Defaultwerte mit einer Masse von 3200g (auf Plätzen verteilt) getestet
#define DISK_DEF_START_RPM			1					// 10 Standardstartdrehzahl (Umdr/min)
#define DISK_DEF_END_RPM			130					// Standardenddrehzahl (Umdr/min)
#define DISK_DEF_COUNTS 			1000				// Anlauflänge der SM-Kennlinie,muss kleiner als  HRZ_ZK_DIM sein

#define DISK_DEF_REV_INIT			45					// Initialisierungsgeschwindigkeit in der Initialisierungsphase
#define DISK_RPM_MAX 				250					// max. mögliche Drehzahl (Umdr/min)
#define DISK_RPM_MIN 				1					// min. mögliche Drehzahl (Umdr/min)
#define DISK_ACC_MAX 				HRZ_ZK_DIM			// max. mögliche Anlauflänge
#define DISK_ACC_MIN 				100					// min. mögliche Anlauflänge
#define DISK_WAY_MAX				7750				// der Rest zu 8000 ist verbotener Bereich
#define DISK_WAY_MIN				0

#define DISK_WAY_INIT				8000				// 360 Grad
#define DISK_WAY_ZERO				400 				// in Inkremente

//#define DISK_GEAR_RATIO 			15.385 				// Mikrosteps/Maßeinheit (Ink) 31.023551 30.07 15.3958
#define DISK_GEAR_RATIO 			15.385 *8			// old 32, new 256  [µStep/Step]

#define DISK_DRV_PRECISION			2					// Inkremente
#define DISK_MEASURE_TO_ENCODER		1					// übersetzungsverhältnis physikal. Maßeinheit(INK) zu Encoderimp.(INK)
#define DISK_DEF_HOLD_CURRENT		0.5					// Haltestrom des Schrittmotors
#define DISK_STEAHLTH				0
//#define H_WAY_4_384D_MIN			-39					/* Weggrenzen für die Ansteuerung ion mm */

/** external data **/
extern DispString_t str_go_on[];

/** internal functions **/
const struct HRZ_POS_CTRL* CarCheckPos(ubyte no);
ubyte HRZDrvWayCtrl(struct DRV_ZK *speed);
void CarDrvAbort(void);
void HrzDrvStart(struct DRV_ZK *ptr);
void CarDrvSetHoldCurrent(void);
void CopyHrzRevDefPara(const struct HRZ_POS_TABLE *ptr);

/** private data **/
volatile int16_t HrzEncoder;						// replacement for old hardware register

static const ubyte m_aubytManLocationTab3[] = {1,2,3};		 				// 3-Platzwagen
static const ubyte m_aubytManLocationTab4[] = {1,2,3,8};	 				// 4-Platzwagen
static const ubyte m_aubytManLocationTab5[] = {1,2,3,4,5};	 				// 5-Platzwagen
static const ubyte m_aubytManLocationTab45[] = {1,2,3,8,61,62,63,68};		// 4,5-Platzwagen
static const ubyte m_aubytDisk10ManLocationTab[] = {1,2,3,4,5,6,7,8,9,10};

static uword m_uwHrzAccSteps;
	// Motorkennline Ausheber für den manuellen Betrieb,Kopie im EEPROM und aktuell
static struct REV_PARA m_udtHrzRevAct;
struct REV_PARA m_udtHrzRevDef = {HRZ_DEF_START_RPM,HRZ_DEF_END_RPM,HRZ_DEF_COUNTS,HRZ_DEF_END_RPM};

//static float HrzDrvGearMotorToEncoder;
static float m_fCarDrvPrecision = HRZ_DRV_PRECISION; 						// wird angepasst an Wagentyp
//static unsigned int m_uiHrzDrvMicroStepsPerRev = HRZ_MICROSTEPS_PER_REV;	// Mikroschritte pro Umdrehung
static float m_fCarDrvMeasureToEncoderImpulse = HRZ_MEASURE_TO_ENCODER;		// wird angepasst Pysikalische Maßänderung/Anzahl der Encoderimpulse
static float m_fCarDrvStepsToMeasure = HRZ_STEPS_TO_MEASURE;				// wird angepasst an Wagentyp
static float m_fHrzDrvCurrentPos;

static const struct HRZ_POS_CTRL m_audtHrzPosTab0[]	 = 	{{0,0}};										// kein Wagen vorhanden

static const struct HRZ_POS_CTRL m_audtHrzPosTab3[]	 =
	{{0,0},{1,POS3_1},{2,POS3_2},{3,POS3_3}									// 3-Platzwagen
	,{4,POS3_4},{5,POS3_5},{6,POS3_6},{7,POS3_7}
	,{43,POS3_43},{44,POS3_44},{45,POS3_45},{47,POS3_47}					// 43-49 Pos. für Spülwanne
	,{48,POS3_48},{49,POS3_49}};

static const struct HRZ_POS_CTRL m_audtHrzPosTab4[]	 = 	 					// 4-Platzwagen
	{{0,0},{1,POS4_1},{2,POS4_2},{3,POS4_3},{8,POS4_8}						// Grundpos. für sym. Platten
	,{4,POS4_4},{5,POS4_5},{10,POS4_10},{14,POS4_14},{20,POS4_20}			// Stacker,linke Seite
	,{6,POS4_6},{7,POS4_7},{11,POS4_11},{17,POS4_17},{21,POS4_21}			// Stacker,rechte Seite
	,{43,POS4_43},{44,POS4_44},{45,POS4_45},{46,POS4_46}					// Grundpos. + pos. Offset
	,{47,POS4_47},{48,POS4_48},{49,POS4_49},{50,POS4_50}					// Grundpos. + neg. Offset
	,{22,POS4_22},{23,POS4_23},{25,POS4_25},{26,POS4_26},{27,POS4_27}		// Inkubatorpos.
	,{28,POS4_28},{24,POS4_24},{34,POS4_34}													// ...
	,{30,POS4_30},{31,POS4_31},{32,POS4_32},{33,POS4_33},{38,POS4_38}		// Grundpos. für asym. Gefäße
	,{39,POS4_39},{40,POS4_40},{41,POS4_41},{42,POS4_42}					// spez. Gefäße
	,{120,POS4_120},{121,POS4_121}};

static const struct HRZ_POS_CTRL m_audtHrzPosTab35[]	 = 	 											// 3-Platzwagen für LHA OL3714
	{{0,0},{1,POS35_1},{2,POS35_2},{3,POS35_3},{4,POS35_4},{5,POS35_5}						// Grundpos. für sym. Platten
	,{43,POS35_43},{44,POS35_44},{45,POS35_45},{47,POS35_47},{48,POS35_48},{49,POS35_49}};	// 43-49 Pos. für Spülwanne

static const struct HRZ_POS_CTRL m_audtHrzPosTab45[]	 = 	 					// 4,5-Platzwagen
	{{0,0},{1,CAR45_1},{2,CAR45_2},{3,CAR45_3},{8,CAR45_8}						// Grundpos. für sym. Platten
	,{4,CAR45_4},{5,CAR45_5}	//,{10,CAR45_10},{14,CAR45_14},{20,CAR45_20}	// Stacker,linke Seite
	,{6,CAR45_6},{7,CAR45_7},{11,CAR45_11},{17,CAR45_17},{21,CAR45_21}			// Stacker,rechte Seite
	,{43,CAR45_43},{44,CAR45_44},{45,CAR45_45},{46,CAR45_46}					// Grundpos. + pos. Offset
	,{47,CAR45_47},{48,CAR45_48},{49,CAR45_49},{50,CAR45_50}					// Grundpos. + neg. Offset
	,{31,CAR45_31},{32,CAR45_32},{33,CAR45_33},{38,CAR45_38}					// Grundpos. für asym. Gefäße
	,{39,CAR45_39},{40,CAR45_40},{41,CAR45_41},{42,CAR45_42}					// spez. Gefäße
	,{61,CAR45_61},{62,CAR45_62},{63,CAR45_63},{68,CAR45_68}					// Platz1-4 unterm DILUS
	,{120,CAR45_120},{121,CAR45_121}};

// die Platznummern werden auch im File ch_ig_test.c verwendet (Stackerjustierung)
static const struct HRZ_POS_CTRL m_audtHrzPosTab5[]	 = 	 										// 5-Platzwagen
	{{0,0},{1,CAR5_1},{2,CAR5_2},{3,CAR5_3},{4,CAR5_4},{5,CAR5_5}								// Grundposition
	,{60,CAR5_60},{61,CAR5_61},{62,CAR5_62},{65,CAR5_65},{66,CAR5_66},{83,CAR5_83}				// Stackerpositionen
	,{84,CAR5_84},{85,CAR5_85},{86,CAR5_86},{87,CAR5_87},{67,CAR5_67},{80,CAR5_80}				// Stackerpositionen
	,{81,CAR5_81},{82,CAR5_82},{63,CAR5_63},{64,CAR5_64}										// Stackerpositionen
	,{11,CAR5_11},{12,CAR5_12},{13,CAR5_13},{14,CAR5_14},{15,CAR5_15}							// Grundpos. mit 2,25mm Offset
	,{21,CAR5_21},{22,CAR5_22},{23,CAR5_23},{24,CAR5_24},{25,CAR5_25}							// Grundpos. mit -2,25mm Offset
	,{100,CAR5_100},{101,CAR5_101},{102,CAR5_102},{103,CAR5_103},{104,CAR5_104},{105,CAR5_105}	// Position mit Adapter Inkubator auf Platz1
	,{110,CAR5_110},{111,CAR5_111},{112,CAR5_112},{113,CAR5_113},{114,CAR5_114},{115,CAR5_115}	// Position mit Adapter Inkubator auf Platz5
	,{120,CAR5_120},{121,CAR5_121},{122,CAR5_122},{123,CAR5_123}								// Platz 1 links außen,Platz 5 rechts außen
	,{31,CAR5_31},{32,CAR5_32},{33,CAR5_33},{34,CAR5_34},{35,CAR5_35}							// Grundpos. mit 4,5mm Offset
 	,{41,CAR5_41},{42,CAR5_42},{43,CAR5_43},{44,CAR5_44},{45,CAR5_45}};							// Grundpos. mit -4,5mm Offset

static const struct HRZ_POS_CTRL m_audtHrzPosTabDisk10[]	 =
	{{0,0},{1,DISK10_1},{2,DISK10_2},{3,DISK10_3},{4,DISK10_4}
	,{5,DISK10_5},{6,DISK10_6},{7,DISK10_7},{8,DISK10_8},{9,DISK10_9}
	,{10,DISK10_10}};

static const struct MAN_LOC HrzManLoc3	 = 	{m_aubytManLocationTab3,sizeof(m_aubytManLocationTab3)/sizeof(char)};
static const struct MAN_LOC HrzManLoc4	 = 	{m_aubytManLocationTab4,sizeof(m_aubytManLocationTab4)/sizeof(char)};
static const struct MAN_LOC HrzManLoc5	 = 	{m_aubytManLocationTab5,sizeof(m_aubytManLocationTab5)/sizeof(char)};
static const struct MAN_LOC HrzManLoc35	 = 	{m_aubytManLocationTab3,sizeof(m_aubytManLocationTab3)/sizeof(char)};
static const struct MAN_LOC HrzManLoc45	 = 	{m_aubytManLocationTab45,sizeof(m_aubytManLocationTab45)/sizeof(char)};
static const struct MAN_LOC Disk10ManLoc = 	{m_aubytDisk10ManLocationTab,sizeof(m_aubytDisk10ManLocationTab)/sizeof(char)};

static const struct HRZ_CAR_PARA m_audtHrzCar3Para[]	 =
	{{HRZ_RPM_MIN,HRZ_RPM_MAX,HRZ_ACC_MIN,HRZ_ACC_MAX
	,{HRZ_DEF_START_RPM,HRZ_DEF_END_RPM,HRZ_DEF_COUNTS,HRZ_DEF_END_RPM}
	,HRZ_MIN_WAY,HRZ_MAX_WAY_3,HRZ_DEF_HOLD_CURRENT,HRZ_STEPS_TO_MEASURE
	,HRZ_DRV_PRECISION,HRZ_MEASURE_TO_ENCODER,&HrzManLoc3}};

static const struct HRZ_CAR_PARA m_audtHrzCar35Para[]	 =
	{{HRZ_RPM_MIN,HRZ_RPM_MAX,HRZ_ACC_MIN,HRZ_ACC_MAX
	,{HRZ_DEF_START_RPM,HRZ_DEF_END_RPM,HRZ_DEF_COUNTS,HRZ_DEF_END_RPM}
	,HRZ_MIN_WAY,HRZ_MAX_WAY_35,HRZ_DEF_HOLD_CURRENT,HRZ_STEPS_TO_MEASURE
	,HRZ_DRV_PRECISION,HRZ_MEASURE_TO_ENCODER,&HrzManLoc35}};

static const struct HRZ_CAR_PARA m_audtHrzCar4Para[]	 =
	{{HRZ_RPM_MIN,HRZ_RPM_MAX,HRZ_ACC_MIN,HRZ_ACC_MAX
	,{HRZ_DEF_START_RPM,HRZ_DEF_END_RPM,HRZ_DEF_COUNTS,HRZ_DEF_END_RPM}
	,HRZ_MIN_WAY,HRZ_MAX_WAY,HRZ_DEF_HOLD_CURRENT,HRZ_STEPS_TO_MEASURE
	,HRZ_DRV_PRECISION,HRZ_MEASURE_TO_ENCODER,&HrzManLoc4}};

static const struct HRZ_CAR_PARA m_audtHrzCar45Para[]	 =
	{{HRZ_RPM_MIN,HRZ_RPM_MAX,HRZ_ACC_MIN,HRZ_ACC_MAX
	,{HRZ_DEF_START_RPM,HRZ_DEF_END_RPM,HRZ_DEF_COUNTS_45,HRZ_DEF_END_RPM}
	,HRZ_MIN_WAY_45,HRZ_MAX_WAY_45,HRZ_DEF_HOLD_CURRENT,HRZ_STEPS_TO_MEASURE
	,HRZ_DRV_PRECISION,HRZ_MEASURE_TO_ENCODER,&HrzManLoc45}};

static const struct HRZ_CAR_PARA m_audtHrzCar5Para[]	 =
	{{HRZ_RPM_MIN,HRZ_RPM_MAX,HRZ_ACC_MIN,HRZ_ACC_MAX
	,{HRZ_DEF_START_RPM,HRZ_DEF_END_RPM,HRZ_DEF_COUNTS_5,HRZ_DEF_END_RPM}
	,HRZ_MIN_WAY_5,HRZ_MAX_WAY_5,HRZ_DEF_HOLD_CURRENT,HRZ_STEPS_TO_MEASURE
	,HRZ_DRV_PRECISION,HRZ_MEASURE_TO_ENCODER,&HrzManLoc5}};

static const struct HRZ_CAR_PARA m_audtHrzDisk10Para[]	 =
	{{DISK_RPM_MIN,DISK_RPM_MAX,DISK_ACC_MIN,DISK_ACC_MAX
	,{DISK_DEF_START_RPM,DISK_DEF_END_RPM,DISK_DEF_COUNTS,DISK_DEF_END_RPM}
	,DISK_WAY_MIN,DISK_WAY_MAX,DISK_DEF_HOLD_CURRENT,DISK_GEAR_RATIO
	,DISK_DRV_PRECISION,DISK_MEASURE_TO_ENCODER,&Disk10ManLoc}};

static const struct HRZ_POS_TABLE m_audtHrzPosTable[]	 =
	{{CARR_0,m_audtHrzPosTab0,0,m_audtHrzCar4Para}
	,{CARR_4,m_audtHrzPosTab4,sizeof(m_audtHrzPosTab4)/sizeof(struct HRZ_POS_CTRL),m_audtHrzCar4Para}
	,{CARR_45,m_audtHrzPosTab45,sizeof(m_audtHrzPosTab45)/sizeof(struct HRZ_POS_CTRL),m_audtHrzCar45Para}
	,{CARR_3,m_audtHrzPosTab3,sizeof(m_audtHrzPosTab3)/sizeof(struct HRZ_POS_CTRL),m_audtHrzCar3Para}
	,{CARR_35,m_audtHrzPosTab35,sizeof(m_audtHrzPosTab35)/sizeof(struct HRZ_POS_CTRL),m_audtHrzCar35Para}
	,{CARR_5,m_audtHrzPosTab5,sizeof(m_audtHrzPosTab5)/sizeof(struct HRZ_POS_CTRL),m_audtHrzCar5Para}
	,{DISK_10,m_audtHrzPosTabDisk10,sizeof(m_audtHrzPosTabDisk10)/sizeof(struct HRZ_POS_CTRL),m_audtHrzDisk10Para}};

const struct HRZ_POS_TABLE *g_pudtHrzPosTabPtr = &m_audtHrzPosTable[1];	// wird in Abhängigkeit des Wagentypes noch modifiziert

DispString_t str_carriage_init[]	 = 	{{1,1,"Neuinitialisierung des Wagens"}
 										,{1,1,"New initialization of carriage"}};
DispString_t str_disk_adj[]		 	 = 	{{1,10,"Karusselljustierung"}
										,{1,10,"carrousel adjustment"}};
DispString_t str_look_for_zero[]	 = 	{{2,1,"IGR-Null suchen"}
										,{2,1,"looking for encoder zero"}};
DispString_t str_look_raw_zero[]	 = 	{{2,1,"Grob-Null suchen"}
										,{2,1,"looking for raw zero"}};
DispString_t str_found_zero[]	 	 = 	{{2,1,"IGR-Null gefunden"}
										,{2,1,"encoder zero found"}};

/** public data **/
ubyte g_ubytHrzActNo;
ubyte g_ubytCarType;

struct DRV_ZK g_audtHrzZk[HRZ_ZK_DIM+1];
ubyte g_ubytHrzDrvState;
bool g_bHrzDrvDir;
long g_lHrzDrvSteps;
short g_sHrzDrvTabWay;
struct DRV_ZK *g_pudtHrzActZk,*pudtHrzActEndZk;
static struct DRV_ZK *m_pudtHrzEndZk;

const char *str_way_set[]	 = 	{"Weg(Inkremente):","way(increments):"};
int ActManLocationTabSize = sizeof(m_aubytManLocationTab4)/sizeof(char);

/** public functions **/

/* Eigentlich sollten alle Funktionen die für alle Wagentypen gleich sind
   mit "Car" beginnen. Das konnte wegen Zeitmangel noch nicht vollständig
   durchgeführt werden,auch der Typ CAR_0 ist noch pflegebedürftig!!! */

bool HrzZeroRequest(void)// Abfrage Null-Lagenschalter
{
  if(g_uiDeviceType == 3714)
	return  HrzZeroRequest3714;			// Null_Lage OL3714
  else
	return HrzZeroRequestStd;			// nWagen_Null_Lage_RD
}

ubyte HrzSetCarType(ubyte type){
	ubyte state; int i;
	state = IERR_HRZ_TYPE;
	for(i = 0; i < sizeof(m_audtHrzPosTable)/sizeof(struct HRZ_POS_TABLE);i++)
		if(m_audtHrzPosTable[i].cartype == type){
			g_pudtHrzPosTabPtr = &m_audtHrzPosTable[i];
			/* CopyHrzRevDefPara(g_pudtHrzPosTabPtr);
	  	  	  bis V4.40,jetzt werden die Daten standardmäßig vom FRAM gelesen und nur über die Schnittstelle bzw. über den Wagentyp
	  	  	  geändert! */
			m_fCarDrvStepsToMeasure = g_pudtHrzPosTabPtr->CarPara->GearRatio;
			m_fCarDrvPrecision = g_pudtHrzPosTabPtr->CarPara->Precision;
			m_fCarDrvMeasureToEncoderImpulse = g_pudtHrzPosTabPtr->CarPara->UnitToEnc;
			state = CB_READY;
			g_ubytCarType = type;
			break;
		}
	return state;
}

void CopyHrzRevDefPara(const struct HRZ_POS_TABLE *ptr)
{
/*	m_udtHrzRevDef.Start = ptr->CarPara->DefStartRev;
	m_udtHrzRevDef.End = ptr->CarPara->DefEndRev;
	m_udtHrzRevDef.cnt = ptr->CarPara->DefAcc;
	m_udtHrzRevDef.EndReduce = ptr->CarPara->DefEndReduce;*/
	CopyRevPara(&m_udtHrzRevDef,&(ptr->CarPara->Rev));
}

/* FUNCTION: 	CarDrvCalibrate
   PARAMETERS:	---
   DESCRIPTION:	Kalibrierung des Wagens bzw des Rundtisches
   RETURNS: 	Fehler / CB_READY */
ubyte CarDrvCalibrate(void){
	if(g_pudtHrzPosTabPtr->cartype == DISK_10) return DiskInit();		// Drehtisch
	return HrzDrvCalibrate();											// Tisch
}

/* FUNCTION: 	HrzDrvCalibrate
   PARAMETERS:	---
   DESCRIPTION:	Anfahren des Null-Punktes
   RETURNS: 	Fehler / CB_READY */
ubyte HrzDrvCalibrate(void){
	ubyte state;
	float save_speed = m_udtHrzRevAct.EndReduce;
	if(g_pudtHrzPosTabPtr->cartype == CARR_0){
		g_ubytHrzActNo = 0;
		HrzEncoder = 0;					// Encoder zurücksetzen
		g_ubytDrvInitState |= 0x01;
		return CB_READY;
	}
	g_ubytDrvInitState |= 0x01; 		/* Bit 0 = 1,Kennung Antrieb unkalibriert */
	if(!HrzZeroRequest()){				// Kodelineal ausserhalb des Sensors{
		if(g_pudtHrzPosTabPtr->cartype == CARR_45){
			g_lHrzDrvSteps = (long)((HRZ_ZERO_SEARCH_WAY_45) * m_fCarDrvStepsToMeasure);
		}else{
			g_lHrzDrvSteps = (long)((HRZ_ZERO_SEARCH_WAY) * m_fCarDrvStepsToMeasure);
		}
		g_bHrzDrvDir = !CLOCKWISE;
		HrzDrvStart(m_pudtHrzEndZk);
		while(g_ubytHrzDrvState != CB_READY){
			WDO();
			if(ReadAbortConditions()){
				CarDrvAbort();
				return IERR_PROG_ABORT;
			}
			if(HrzZeroRequest()){
				tmc5130a_CFG_SOFT_STOP(HRZ_DEVICE); //SOFT STOP
				CarDrvAbort();
				break;
			}
			if (tmc5130a_is_stop(HRZ_DEVICE)) g_ubytHrzDrvState = CB_READY;
		}
	}
	tmc5130a_CFG_HARD_STOP(HRZ_DEVICE); //ab hier wieder HARD_STOP
	if(!HrzZeroRequest()){
		CarDrvSetCurrentToZero();
		return IERR_HRZ_ZERO;					// kein Kodelineal gefunden
	}
	//  Verlassen des Kodelineals (Wegbegrenzung,max. Weg)
	g_lHrzDrvSteps = (long)((HRZ_ZERO_LEAVE_WAY) * m_fCarDrvStepsToMeasure);
	g_bHrzDrvDir = CLOCKWISE;

	HrzDrvStart(m_pudtHrzEndZk);

	while(g_ubytHrzDrvState != CB_READY){
		if (tmc5130a_is_stop(HRZ_DEVICE)){
			g_ubytHrzDrvState = CB_READY;
		}
		WDO();
		if(ReadAbortConditions()){
			CarDrvAbort();
			return IERR_PROG_ABORT;
		}
		if(!HrzZeroRequest()) break;
	}
	if(g_ubytHrzDrvState == CB_READY){
		CarDrvSetCurrentToZero();
		return IERR_HRZ_ZERO;					// Kodelineal konnte nicht verlassen werden
	}
	tmc5130a_CFG_SOFT_STOP(HRZ_DEVICE); //SOFT STOP
	CarDrvAbort();								// Stop
	tmc5130a_CFG_HARD_STOP(HRZ_DEVICE); //ab hier wieder HARD_STOP
	tmc5130a_VELOCITY(HRZ_DEVICE, M3_SpU *uSpS*6/60); //6 RPM in Feinanfahrt
	// Anfahren Kodelineal im Start/Stop
	g_lHrzDrvSteps = (long)((HRZ_ZERO_LIMIT_WAY) * m_fCarDrvStepsToMeasure);
	g_bHrzDrvDir = !CLOCKWISE;
	HrzDrvStart(g_audtHrzZk);

	while(g_ubytHrzDrvState != CB_READY){
		if (tmc5130a_is_stop(HRZ_DEVICE)){
			g_ubytHrzDrvState = CB_READY;
		}
		WDO();
		if(ReadAbortConditions()){
			CarDrvAbort();
			return IERR_PROG_ABORT;
		}
		if(HrzZeroRequest()){
			CarDrvAbort(); 				// Schritt-Timer abschalten
			Wait(10); 					// wegen mechanischer Stabilisierung
			HrzEncoder = 0;
			tmc5130a_VELOCITY(HRZ_DEVICE, M3_SpU *uSpS*save_speed/60); //Geschwindigkeit wiederherstellen
			if(g_uiDeviceType != 3714){
				if((state = HrzDrvWay(HRZ_ZERO_DIFF_STD)) != CB_READY) return state;	// Fahrt starten
			}
			g_ubytHrzActNo = 1;
			HrzEncoder = 0; 									// Encoderzähler auf 0
			g_ubytDrvInitState &= 0xfe; 	/* Bit 0 = 0 */		// Hrz ist nun kalibriert
			HrzExtEncoderReset();								// externer Encoderzähler ebenfalls auf 0
			return CB_READY;
		}
	}
	CarDrvSetCurrentToZero();									// Abschalten Motorstrom (Fehlerfall)
	return IERR_DRV_H;
}

/* FUNCTION: 	HrzExtEncoderReset
   PARAMETERS:	---
   DESCRIPTION:	Rücksetzen externer Encoder	z.B Drop
   RETURNS: 	CB_READY */
ubyte HrzExtEncoderReset(void){
	ubyte i;
	SendRes1_L;
	for(i = 0; i<100 ; i++) WDO();
	SendRes1_H;

	return CB_READY;
}

/* FUNCTION: 	CarDrvNo
   PARAMETERS:	Platznummer
   DESCRIPTION:	Anfahren des Platzes
   RETURNS: 	CB_READY / Fehlermeldung  */
ubyte CarDrvNo(ubyte no){
	ubyte state;
	const struct HRZ_POS_CTRL  *ptr;
	if(g_pudtHrzPosTabPtr->cartype == CARR_0) return CB_READY; /* Kompatibilität zur PC-Software */
	ptr = CarCheckPos(no);
	if(ptr == NULL) return IERR_PARA;
	state = HrzDrvWay(ptr->way);
	if(state!= IERR_PROG_ABORT) g_ubytHrzActNo = no;
	return state;
}

/* FUNCTION: 	HrzDrvWay
   PARAMETERS:	Wegposition
   DESCRIPTION:	Anfahren der Wegposition
   RETURNS: 	CB_READY bzw. IERR_PROG_ABORT,IERR_DRV_H */
ubyte HrzDrvWay(float way){
	ubyte state,i;
	struct DRV_ZK *speed;						// {Schrittweite, Schrittdauer}
	speed = m_pudtHrzEndZk;						// zielwert?
	float save_rev;
	save_rev = m_udtHrzRevAct.EndReduce;		// {start_rpm, end_rpm, def-cnt, endredu_rpm}
	if(g_uiDeviceType == 3714){			// ab V3.22 nachfolgende überwachung nur für LHA,da filigrane Fingerkonstruktion des Aushebers
		if(fabs(ReadVertDrvCurrentPos()) > (4*g_fVertDrvPrecision)){
			if(fabs(CarReadWayPos()-way) > (2*HRZ_DRV_PRECISION)) return IERR_COM_NOT_PRACTICABLE;
		}
	}

	for(i = 0; i<HRZ_DRV_REPEAT_CNT; i++){
		m_fHrzDrvCurrentPos = CarReadWayPos();	// Position aus Encoderwert berechnen
		if(g_bTestFlag){
			printf("cyc:%d,curr pos:%f,dest way:%f\r",(int)i,m_fHrzDrvCurrentPos,way);fflush(stdout);
		}
		if(fabs(way - m_fHrzDrvCurrentPos) < m_fCarDrvPrecision){
			CarDrvSetHoldCurrent();
			HrzDrvSpeedLimit(save_rev);
			return CB_READY;
		}

		g_lHrzDrvSteps = (long)((way - m_fHrzDrvCurrentPos) * m_fCarDrvStepsToMeasure);
		state = HRZDrvWayCtrl(speed);
		if(state == IERR_PROG_ABORT){
			CarDrvSetCurrentToZero();
			return state;
		}
		//speed = g_audtHrzZk;		// falls Weg nicht korrekt angefahren wurde,weiter im Start/Stop
		HrzDrvSpeedLimit(m_udtHrzRevAct.EndReduce/2);
		speed = m_pudtHrzEndZk;
	}
	CarDrvSetCurrentToZero();
	return IERR_HRZ_WAY;
}

/* FUNCTION: 	HrzInitZk
   PARAMETERS:	---
   DESCRIPTION:	Berechnung des Kennlinienfeldes des H-Antriebes mit Defaultwerten
   RETURNS: 	--- */
ubyte HrzInitZk(void){
	return HrzCalcDrvPara(&m_udtHrzRevDef);
}

/* FUNCTION: 	HrzCheckAndSetRevToDefault
   PARAMETERS:	---
   DESCRIPTION:	Prüfen und evtl. Neurechnen der Antriebskennlinie für den Wagen
   RETURNS: 	CB_READY / Fehlercode */
ubyte HrzCheckAndSetRevToDefault(void)
{ubyte state;
  state = CB_READY;
  if((m_udtHrzRevAct.Start != m_udtHrzRevDef.Start)||(m_udtHrzRevAct.End != m_udtHrzRevDef.End)||(m_udtHrzRevAct.cnt != m_udtHrzRevDef.cnt))
	  state = HrzInitZk();
  if(state != CB_READY) return state;
  if(m_udtHrzRevAct.EndReduce != m_udtHrzRevDef.EndReduce) state = HrzDrvSpeedLimit(m_udtHrzRevDef.EndReduce);
  return state;
}

/* FUNCTION: 	HrzCheckDrvPara
   PARAMETERS:	Startdrehzahl,Enddrehzahl,Anzahl der Schritte
   DESCRIPTION:	Prüfen der Antriebsparameter für die SM-Kennlinie
   RETURNS: 	TRUE / FALSE */
ubyte HrzCheckDrvPara(const struct REV_PARA *rev)
{
/*  if( (rev->Start < (float)HRZ_RPM_MIN)
 	 ||	(rev->End > (float)HRZ_RPM_MAX)
 	 ||	(rev->Start >=  rev->End)
	 ||	(rev->cnt < (float)HRZ_ACC_MIN)
	 ||	(rev->cnt > (float)HRZ_ACC_MAX)) return FALSE;
	return TRUE; */
	if( (rev->Start < g_pudtHrzPosTabPtr->CarPara->MinRev)
	||	(rev->End > g_pudtHrzPosTabPtr->CarPara->MaxRev)
	||	(rev->Start >=  rev->End)
	||	(rev->cnt < g_pudtHrzPosTabPtr->CarPara->MinAcc)
	||	(rev->cnt > g_pudtHrzPosTabPtr->CarPara->MaxAcc)) return FALSE;
	return TRUE;
}

/* FUNCTION: 	HrzCalcDrvPara
   PARAMETERS:	Startdrehzahl,Enddrehzahl,Anzahl der Schritte zum Erreichen der Enddrehzahl
   DESCRIPTION:	Kennlinienfeld des SM-Antriebes errechnen
   RETURNS: 	CB_READY / IERR_PARA */
ubyte HrzCalcDrvPara(const struct REV_PARA *rev){
	if(HrzCheckDrvPara(rev) == FALSE) return IERR_PARA;
	if((m_uwHrzAccSteps = CalcTimeTable(g_audtHrzZk,rev->Start,rev->End,rev->cnt)) == 0) return IERR_DRV_ZK_HRZ;
	m_pudtHrzEndZk = g_audtHrzZk + rev->cnt;
	CopyRevPara(&m_udtHrzRevAct,rev);
	m_udtHrzRevAct.EndReduce = m_udtHrzRevAct.End;
	HrzDrvSpeedLimit(m_udtHrzRevAct.EndReduce);

	//Disk; Car type set in ch_ig_config.c:314 this functions  is called through ch_ig_config.c:363
	if(g_pudtHrzPosTabPtr->cartype == DISK_10)
	{
//		uint8_t spread[5];
//		spread[0] = 5;//Off
//		spread[1] = 4;//hstart
//		spread[2] = 1;//hend
//		spread[3] = 0;//rdntf
//		spread[4] = 1;//tbl
		tmc5130a_ACCELERATION_MAN(HRZ_DEVICE, 100000, 100000);
//		tmc5130a_CHOPCONF_MAN(HRZ_DEVICE, spread, 0); //manipulate chopper conf for Stillstand
//		tmc5130a_CHOPCONF_MAN(HRZ_DEVICE, spread, 20); //manipulate chopper conf for Stillstand
//		tmc5130a_CHOPCONF_MAN(HRZ_DEVICE, spread, 40); //manipulate chopper conf for Stillstand
//		tmc5130a_CHOPCONF_MAN(HRZ_DEVICE, spread, 60); //manipulate chopper conf for Stillstand
//		tmc5130a_CHOPCONF_MAN(HRZ_DEVICE, spread, 80); //manipulate chopper conf for Stillstand
//		tmc5130a_STEALTHCHOP_CONF_MAN(HRZ_DEVICE, 0x01, 0x00, 0x01, 0x00, 1, 255);
//		tmc5130a_STEALTHCHOP_ACTIVATE(HRZ_DEVICE, DISK_STEAHLTH);
	}
	else //leave standard acceleration
	{
	}
	return CB_READY;
}

/* FUNCTION: 	HrzDrvSpeedLimit
   PARAMETERS:	Enddrehzahl
   DESCRIPTION:	Reduzierung der Endgeschwindigkeit
   RETURNS: 	Ready oder Fehler */
ubyte HrzDrvSpeedLimit(float speed){
	float help;
	if((speed > m_udtHrzRevAct.End) || (speed < m_udtHrzRevAct.Start)) return IERR_PARA;								// außerhalb Kenndaten?
	help = (speed - m_udtHrzRevAct.Start) / (m_udtHrzRevAct.End - m_udtHrzRevAct.Start) *(float)m_udtHrzRevAct.cnt;		// Schritt berechnen
	m_pudtHrzEndZk = g_audtHrzZk + (int)help;
	m_udtHrzRevAct.EndReduce = speed;
	tmc5130a_VELOCITY(HRZ_DEVICE, M3_SpU *uSpS*speed/60);		// Umrechnung RPM auf µStep/s
	if(g_pudtHrzPosTabPtr->cartype == DISK_10)
	{
		tmc5130a_COOLCONF_MAN(HRZ_DEVICE, M3_VCOOL, 0, 0, 1, 1, 1, 1, 0); //Deactivate Coolstep for Disk
		tmc5130a_CURR_MAN(HRZ_DEVICE, motor_3.hold_delay, M3_CURR, motor_3.hold_current);
	}
	return CB_READY;
}

/* FUNCTION: 	CarDrvGetLimits
   PARAMETERS:	Pointer auf Zeichenkette,ZK muss groß genug sein,um alle Zeichen zu speichern!
   DESCRIPTION:	Bereitstellung der Grenzwerte der Horizontal-Antriebsparameter
   RETURNS: 	--- */
/*void HrzDrvGetLimits(char *zk)
{//nicht kompatibel zum CyBio Ctrl
  sprintf(zk,"I%-4.1f %-4.1f %u %u %-4.1f %-4.1f %d\r",(double)HRZ_RPM_MIN,(double)HRZ_RPM_MAX,(uword)HRZ_ACC_MIN,(uword)HRZ_ACC_MAX,
									(double)m_udtHrzRevDef.Start,(double)m_udtHrzRevDef.End,(uword)m_udtHrzRevDef.cnt);
}  */

/*void HrzDrvGetLimits(char *zk)
{
  sprintf(zk,"I%d %d %u %u %d %d %d\r",(int)HRZ_RPM_MIN,(int)HRZ_RPM_MAX,(uword)HRZ_ACC_MIN,(uword)HRZ_ACC_MAX,
									ftoi(m_udtHrzRevDef.Start),ftoi(m_udtHrzRevDef.End),(uword)m_udtHrzRevDef.cnt);
} */

void CarDrvGetLimits(char *zk){
	sprintf(zk,"I%d %d %u %u %d %d %d\r",
		 (int)g_pudtHrzPosTabPtr->CarPara->MinRev
		,(int)g_pudtHrzPosTabPtr->CarPara->MaxRev
		,(uword)g_pudtHrzPosTabPtr->CarPara->MinAcc
		,(uword)g_pudtHrzPosTabPtr->CarPara->MaxAcc
		,ftoi(g_pudtHrzPosTabPtr->CarPara->Rev.Start)
		,ftoi(g_pudtHrzPosTabPtr->CarPara->Rev.End)
		,(uword)g_pudtHrzPosTabPtr->CarPara->Rev.cnt);
}

/* FUNCTION: 	CarDrvGetPara
   PARAMETERS:	Pointer auf Zeichenkette,ZK muss groß genug sein,um alle Zeichen zu speichern!
   DESCRIPTION:	Bereitstellung der aktuellen Geschwindigkeitsparameter des Horizontalantriebes
   RETURNS: 	--- */
/* void HrzDrvGetPara(char *zk)
{//nicht kompatibel zum CyBio Ctrl
  sprintf(zk,"I%-4.1f %-4.1f %u %-4.1f\r",(double)m_udtHrzRevAct.Start,(double)m_udtHrzRevAct.End,
  									   (uword)m_udtHrzRevAct.cnt,(double)m_udtHrzRevAct.EndReduce);
} */

void CarDrvGetPara(char *zk){
	sprintf(zk,"I%d %d %u %d\r",
		 ftoi(m_udtHrzRevAct.Start)
		,ftoi(m_udtHrzRevAct.End)
		,(uword)m_udtHrzRevAct.cnt
		,ftoi(m_udtHrzRevAct.EndReduce));
}


/* FUNCTION: 	CarDrvSetHoldCurrent
   PARAMETERS:	Teilungsfaktor für Haltestrom
   DESCRIPTION:	Einstellen des Haltestromes Horizontalantrieb
   RETURNS: 	--- */
void CarDrvSetHoldCurrent(void){
	CarDrvAbort();
	tmc5130a_HOLD_CURR_MAN(HRZ_DEVICE,M3_HOLD);
//	tmc5130a_HOLD_CURR_MAN(HRZ_DEVICE, round(g_pudtHrzPosTabPtr->CarPara->HoldCurrent*1000)); //Ströme aus Tabelle sind anders als die gemessenen
//	if(g_pudtHrzPosTabPtr->cartype == DISK_10)
//	{
//		tmc5130a_STEALTHCHOP_ACTIVATE(HRZ_DEVICE, DISK_STEAHLTH); //always activate SteahlthChop on Diskmotor
//	}
}


/*------------------------------------------------------------------
  Interne Funktionen
 ------------------------------------------------------------------*/
/* FUNCTION: 	CarDrvSetCurrentToZero
   PARAMETERS:	---
   DESCRIPTION:	Stromlosschalten des Horizontalantriebes
   RETURNS: 	--- */
void CarDrvSetCurrentToZero(void){
	CarDrvAbort();
	tmc5130a_HOLD_CURR_MAN(HRZ_DEVICE, 0);
//	tmc5130a_STEALTHCHOP_ACTIVATE(HRZ_DEVICE, DISK_STEAHLTH);

}

/* FUNCTION: 	HrzDrvStart
   PARAMETERS:	Pointer auf die Endgeschwindigkeit
   DESCRIPTION:	Start des Horizontalantriebes
   RETURNS: 	--- */
void HrzDrvStart(struct DRV_ZK *ptr) // struct {Schrittweite, Schrittdauer} zielwert
{
	pudtHrzActEndZk = ptr;
	g_pudtHrzActZk = g_audtHrzZk; 					// struct {Schrittweite, Schrittdauer} startwert
	g_ubytHrzDrvState = RESET; 						// "RESET" entspricht "läuft"

	if(g_bHrzDrvDir != CLOCKWISE){
		g_lHrzDrvSteps *= -1;
	}
	tmc5130a_POSITIONING_MODE(HRZ_DEVICE);
	tmc5130a_HOMING(HRZ_DEVICE);							// Position = 0 (equivalent für Schrittzähler = 0)
	tmc5130a_start_dest(HRZ_DEVICE,g_lHrzDrvSteps);
	tmc5130a_PIN_RUN(HRZ_DEVICE);
}

/* FUNCTION: 	HRZDrvWayCtrl
   PARAMETERS:	Zeiger auf Endgeschwindigkeit
   DESCRIPTION:	Verfahren des Weges,der über  g_lHrzDrvSteps voreingestellt ist
   RETURNS: 	CB_READY bzw. IERR_PROG_ABORT */
ubyte HRZDrvWayCtrl(struct DRV_ZK *speed)
{
	if(g_lHrzDrvSteps != 0){
		if(g_lHrzDrvSteps > 0) g_bHrzDrvDir = !CLOCKWISE; else	{g_lHrzDrvSteps *= -1; g_bHrzDrvDir = CLOCKWISE;}

		HrzDrvStart(speed);								// struct {Schrittweite, Schrittdauer} zielwert

		while(g_ubytHrzDrvState != CB_READY){
			if (tmc5130a_is_stop(HRZ_DEVICE)){
				g_ubytHrzDrvState = CB_READY;
			}
			WDO();
			if(ReadAbortConditions()){
				CarDrvAbort();
				return IERR_PROG_ABORT;
			}
		}
	}
	return CB_READY;
}

/* FUNCTION: 	CarDrvAbort
   PARAMETERS:	---
   DESCRIPTION:	Abbruch der Fahrroutine des Wagens
   RETURNS: 	--- */
void CarDrvAbort(void){
	tmc5130a_STOP(HRZ_DEVICE);
	g_ubytHrzDrvState = CB_READY;
	tmc5130a_PIN_STOP(HRZ_DEVICE); //sperren Motor-Treiber
}

/* FUNCTION: 	CarCheckPos
   PARAMETERS:	Platznummer
   DESCRIPTION:	Prüfen auf Vorhandensein der Platznummer
   RETURNS: 	TRUE / FALSE */
const struct HRZ_POS_CTRL* CarCheckPos(ubyte no)
{const struct HRZ_POS_CTRL *ptr;
  for(ptr = g_pudtHrzPosTabPtr->tab; ptr < (g_pudtHrzPosTabPtr->tab + g_pudtHrzPosTabPtr->size); ptr++)
	{if(ptr->no == no) return ptr;}
  return NULL;
}

/* FUNCTION: 	CarWayCheck
   PARAMETERS:	Weg
   DESCRIPTION:	Prüfung des Weges in den Grenzen min/max
   RETURNS: 	TRUE / FALSE */
ubyte CarWayCheck(float way)
{
  if((way > g_pudtHrzPosTabPtr->CarPara->WayMax)||(way <	g_pudtHrzPosTabPtr->CarPara->WayMin)) return FALSE;
  return TRUE;
}

/* FUNCTION: 	CarNextPos
   PARAMETERS:	Inkrement
   DESCRIPTION:	nächste Platzposition anfahren
   RETURNS: 	--- */
void CarNextPos(int8_t offset){
	const ubyte *pPos,*phPos;
	ubyte state,Len;
	if(g_pudtHrzPosTabPtr->cartype == CARR_0) return;
	if(g_pudtHrzPosTabPtr->cartype == DISK_10) offset *= -1;
	/* Da Platz 4 aus Kompatibilitätsgründen die Nummer 8 in der Wegtabellenposition
	 	 einnimmt,muß hier eine Konvertierung erfolgen */
	phPos = g_pudtHrzPosTabPtr->CarPara->ManLoc->Location; Len = g_pudtHrzPosTabPtr->CarPara->ManLoc->Len;
	for(pPos = phPos; pPos <= phPos+Len; pPos++) if(*pPos == g_ubytHrzActNo) break;
	pPos = pPos+offset;
	if(pPos < g_pudtHrzPosTabPtr->CarPara->ManLoc->Location) pPos = g_pudtHrzPosTabPtr->CarPara->ManLoc->Location;
	if(pPos > g_pudtHrzPosTabPtr->CarPara->ManLoc->Location+Len-1) pPos = g_pudtHrzPosTabPtr->CarPara->ManLoc->Location+Len-1;
	if(!XYServoCheckZeroPos()){
		ErrorDisplay(XYServoWay(0,0));
		g_fServoOffsetX = 0;
	}
	state = CarDrvNo(*pPos);
	if(state == IERR_PROG_ABORT){
		while(1){
			if( CarManAbortHandling() == CB_READY) break;
		}
	}
	else  ErrorDisplay(state);
	ShowLocation(FALSE);
}

/* FUNCTION: 	CarManAbortHandling
   PARAMETERS:	---
   DESCRIPTION:	Abbruchhandling Horizontalantrieb
   RETURNS: 	CB_READY / ERROR */
ubyte CarManAbortHandling(void){
	ubyte state;
	struct LCD_DISP dHelp;
	SaveDisplay(&dHelp);
	ClearAllAndDisplay(str_carriage_init);
	DisplayAndEnter(str_go_on);
	state = CarDrvCalibrate();
	RestoreDisplay(&dHelp);
	return state;
}

/* FUNCTION: 	IsCarrType
   PARAMETERS:	Carriage type
   DESCRIPTION:	Prüft die Gültigkeit des Wagentypes
   RETURNS: 	TRUE / FALSE */
ubyte IsCarrType(ubyte type)
{ubyte state,i;
  state = FALSE;
  for(i = 0; i < sizeof(m_audtHrzPosTable)/sizeof(struct HRZ_POS_TABLE);i++)
  if(m_audtHrzPosTable[i].cartype == type){state = TRUE; break;}
  return state;
}


float CarReadWayPos(void)
{
  return (int)HrzEncoder * m_fCarDrvMeasureToEncoderImpulse;	// Lesen der Istposition
}


/* FUNCTION: 	DiskInit
   PARAMETERS:	---
   DESCRIPTION:	Initialisierung (Nullung) der Drehscheibe
   RETURNS: 	CB_READY / IERR_PROG_ABORT,IERR_HRZ_ZERO  */
ubyte DiskInit(void){
	g_ubytDrvInitState |= 0x01;		// Bit0 = 1,Kennung Antrieb unkalibriert
	tmc5130a_CFG_SOFT_STOP(HRZ_DEVICE); //SOFT STOP
	if(!DiskRawZeroRequest){		/* Fahren zur Grobnull-Lage */
		g_bHrzDrvDir = CLOCKWISE;
		g_lHrzDrvSteps = (long)(DISK_WAY_INIT*DISK_GEAR_RATIO);  	//MAX_INK_H*gear_ratio_h;
		HrzDrvSpeedLimit(DISK_DEF_REV_INIT);
		HrzDrvStart(m_pudtHrzEndZk);

		while(!DiskRawZeroRequest){
			WDO();
			if(ReadAbortConditions()){
				CarDrvAbort();
				return IERR_PROG_ABORT;
			}
			if(DiskRawZeroRequest){
				CarDrvAbort();
				break;
			}
		}
		if(!DiskRawZeroRequest){
			CarDrvSetCurrentToZero();
			return IERR_HRZ_ZERO;
		}	//keine Grobnull-Lage gefunden
	}
	HrzDrvSpeedLimit(1); //s. Video: sehr langsames anfahren der Fein-Null
	/* Verlassen der Grobnull */
	g_bHrzDrvDir = !CLOCKWISE;
	g_lHrzDrvSteps = (long)(DISK_WAY_ZERO*DISK_GEAR_RATIO);
	HrzDrvStart(g_audtHrzZk);						// ab V1.4 		HrzDrvStart(m_pudtHrzEndZk);
	while(g_ubytHrzDrvState != CB_READY){
		if (tmc5130a_is_stop(HRZ_DEVICE)){
			g_ubytHrzDrvState = CB_READY;
		}
		WDO();
		if(ReadAbortConditions()){
			CarDrvAbort();
			return IERR_PROG_ABORT;
		}
		if(!DiskRawZeroRequest){
			break; 									/* Grobnull wurde verlassen */
		}
	}

	tmc5130a_CFG_HARD_STOP(HRZ_DEVICE); //HARD STOP
	/* Suchen der Fein-Null (IGR-Null) */

	while(DiskFineZero == 0){
		if (tmc5130a_is_stop(HRZ_DEVICE)){
			g_ubytHrzDrvState = CB_READY;
		}
		WDO();
		if(g_ubytHrzDrvState == CB_READY){
			CarDrvSetCurrentToZero();
			return IERR_HRZ_ZERO;
		}	// innerhalb der Wegvorgabe keine IGR-Null gefunden
		if(ReadAbortConditions()){
			CarDrvAbort();
			return IERR_PROG_ABORT;
		}
	}
	CarDrvAbort();//HrzDrvRunBit(DRV_DISABLE);						// Schritt-Timer deaktivieren
	Wait(10);			// 100ms,wegen mechanischer Stabilisierung des Antriebssystems
	HrzEncoder = 0;		// Nullung des Wegmeßsystems
	g_ubytHrzActNo = 1; g_ubytDrvInitState &= 0xfe;	//Bit0 = 0		// ist nun Kalibriert
	CarDrvSetHoldCurrent();
	HrzDrvSpeedLimit(DISK_DEF_END_RPM);
	return CB_READY;
}

void AdjDiskZero(void){
	char * ptr = (char *) calloc(42,sizeof(char));
	ClearAllAndDisplay(str_disk_adj); SetCursorOff();
	if(DiskRawZeroRequest) g_bHrzDrvDir = !CLOCKWISE; else g_bHrzDrvDir = CLOCKWISE;
	while(1){
		ClearLineAndDisplay(str_look_for_zero); HrzDrvSpeedLimit(DISK_ADJ_REV);
		g_lHrzDrvSteps = (long)((DISK_WAY_INIT+100)*DISK_GEAR_RATIO);
		HrzDrvStart(m_pudtHrzEndZk);
		while(DiskFineZero == 0){	// Anfahren der Encodernull
			WDO();
			if(g_ubytHrzDrvState == CB_READY){
				CarDrvSetCurrentToZero();
				ErrorDisplay(IERR_DRV_H);
			}
			if(ReadAbortConditions()){
				CarDrvAbort();
				break;
			}	// Nachfolgendes Ergebnis Unsinnig,keine bessere Lsg.
			if(DiskRawZeroRequest){
				CarDrvAbort();
				g_lHrzDrvSteps = (long)((DISK_WAY_INIT+100)*DISK_GEAR_RATIO);
				if(DiskRawZeroRequest) g_bHrzDrvDir = !CLOCKWISE; else g_bHrzDrvDir = CLOCKWISE;
				HrzDrvStart(m_pudtHrzEndZk);
				while(DiskRawZeroRequest){
					WDO();
					if(g_ubytHrzDrvState == CB_READY){
						if (tmc5130a_is_stop(HRZ_DEVICE)){
							g_ubytHrzDrvState = CB_READY;
						}
						CarDrvSetCurrentToZero();
						ErrorDisplay(IERR_DRV_H);
					}
					if(ReadAbortConditions()){
						CarDrvAbort(); break;
					}	// Nachfolgendes Ergebnis Unsinnig,keine bessere Lsg.
				}
			}
		}
		CarDrvAbort();
		CarDrvSetHoldCurrent();
		HrzEncoder = 0;	// Nullung des Wegmeßsystems
		ClearLineAndDisplay(str_found_zero);
		Display(str_go_on);
		KeyWaitForAny();
		ClearStringOnDisplay(str_go_on);
		// Anfahren der Grobnull und Zählen der Impulse
		HrzDrvSpeedLimit(DISK_ADJ_REV); ClearLineAndDisplay(str_look_raw_zero);
		g_bHrzDrvDir = CLOCKWISE;
		g_lHrzDrvSteps = (long)((DISK_WAY_INIT+100)*DISK_GEAR_RATIO);
		HrzDrvStart(m_pudtHrzEndZk);
		while(!DiskRawZeroRequest){
			WDO();
			if(g_ubytHrzDrvState == CB_READY){
				if (tmc5130a_is_stop(HRZ_DEVICE)){
					g_ubytHrzDrvState = CB_READY;
				}
				CarDrvSetCurrentToZero();
				ErrorDisplay(IERR_DRV_H);
			}
			if(ReadAbortConditions()){
				CarDrvAbort();
				break;
			}
		}
		g_bHrzDrvDir = !g_bHrzDrvDir;
		CarDrvAbort();
		CarDrvSetHoldCurrent();
		sprintf(ptr,"%s%d",*(str_way_set+g_ubytLang),(int)HrzEncoder);
		LcdClear(SECOND_LINE); LcdShow(2,1,ptr);
		Display(str_go_on); KeyWaitForEnter(); ClearStringOnDisplay(str_go_on);
	}
}

float XReadPos(void)
{if(g_pudtHrzPosTabPtr->cartype == DISK_10) return CarReadWayPos();
  else return CarReadWayPos()*(-1);
}

/* FUNCTION: 	SetAndSaveHrzDrvParaToDefault
   PARAMETERS:	---
   DESCRIPTION:	Kennlinienparameter auf Default-Werte des Wagentyps setzen
				und Kennlinie aktualisieren
   RETURNS: 	CB_READY / IERROR */
ubyte SetAndSaveHrzDrvParaToDefault(void)
{ubyte state;
   state = EEPROMSetHrzMotPara(&(g_pudtHrzPosTabPtr->CarPara->Rev));	// Kennlinienparameter auf Default-Werte des Wagentyps setzen
   if(state == CB_READY) HrzCalcDrvPara(&(g_pudtHrzPosTabPtr->CarPara->Rev));
   return state;
}
