
/*******************************************************************
 *
 *	DESCRIPTION:	Ansteuerung und Abgleich des XY-Tisches
 *
 *	AUTHOR:		Baumann
 *
 *	HISTORY:
 *
 *******************************************************************/
/** include files **/
#include <string.h>

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

#include "ch_ig_cmd.h"			//
#include "head_piston_tab.h"	//

#include "ch_ig_config.h"
#include "ch_ig_eeprom.h"
#include "ch_ig_err.h"
#include "ch_ig_head.h"
#include "ch_ig_hrz.h"
#include "ch_ig_key.h"
#include "ch_ig_piston.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"

#include "Well_Defines.h"
#include "Well_Error.h"

/** local definitions **/
#define ZERO_START 1520+1		/* {+1,da PPx+1}(1436 alt)in us (Mittelstellung)*/
#define ACC_START  0.086		/* (70)in Grad/us 	*/					// 220930 mku exactly: 0.1 (S3115: 60°/600µs) )
#define ZERO_SCOPE 2			/* Nullpunktänderungskoeffizient 2 */
#define ACC_SCOPE 0.002			/* !!! Wegänderungskoeffizient 0.004 */

#define TIME_GRID	3.2005		/* in us		*/						// 220930 mku exactly: 3,2000 (1/pwm_clk_167 = 1/312500 Hz)
#define RADIUS_FIX_POS 7.5		/* in mm		*/						// 220930 mku,übliche "Servo Horn" sind im Inch Raster,könnte also 3 inch = 7,62 mm sein
#define PI 3.1415927

#define X_SERVO_MAX_WAY 4.5		// in mm
#define X_SERVO_MIN_WAY -4.5	// in mm
#define Y_SERVO_MAX_WAY 4.5		// in mm
#define Y_SERVO_MIN_WAY -4.5	// in mm

#define EXECUTION_TIME 30		// in 10ms

/** default settings **/

/** external data **/

/** internal functions **/
ubyte XYProgModify(const struct ADJ_XY *ptr,int8_t len,int8_t beg,int8_t cls);
void GetXYAdjustVar(struct XYAdj *ptr);
void SaveXYAdjustVar(struct XYAdj *ptr);
void XProgOffset(int8_t i);
void YProgOffset(int8_t i);
void XProgSlope(int8_t i);
void YProgSlope(int8_t i);
void XYAdjustDisplay(const struct ADJ_XY *ptr,unsigned char len,unsigned char cls);
void XYShowAdjPara(const struct ADJ_XY *xy,char exe_flag);
void return_xy(void);
void XYServoSetZeroAndSlope(void);
int XYCheckAndLimit(int ist,int max,int min);
uword XYServoCalcWayToTimeGrid(float way,float acc,float zero);
ubyte XYPlateTypeCmp(struct DEV_TYPE *pScr,struct DEV_TYPE *pDest);
void ShowXYPosition(char *str);

static const int m_iServoXYSlopeMax = 16;			//Grenzwerte für den Tischabgleich	Verstärkung
static const int m_iServoXYSlopeMin = -16;
static const int m_iServoXYOffsetMax = 16;		//Grenzwerte für den Tischabgleich Nullpunkt
static const int m_iServoXYOffsetMin = -16;

	// Faktoren für die XY-Steuerung
static struct XYPara m_udtServoFuncPara;		// NullPunkt- und Verstärkungkoeffizienten für Servos
static float m_fXServoActWay,m_fYServoActWay; 	// Aktulle Wegposition des XY-Tisches
static int8_t m_iXYActPosNo; 						// Aktuelle Positionnummer des XY-Tisches

//unsigned int uintStageType = 1536;		96/384/1536 z.Z. nur 1536 vorhanden
struct XYAdj g_udtServoAdjustPara;				// NullPunkt- und Verstärkungeingabewerte

	//Dieser Offset wird nur durch den Befehl "CmdHrzDrvNo" und "CmdHrzDrvWay" vorgegeben
float g_fServoOffsetX = 0;
bool g_bAdjustMode1536 = FALSE;

//#pragma noclear
//unsigned long ulngXYCycles;		// siehe  struct LIFE_PARA g_udtLifePara; Zähler der XY-Spiele über die Lebenszeit,nur über PC setzbar
//#pragma clear

DispString_t str_spot[]		 = 	{{1,31,"Platz:"}
								,{1,32,"Pos.:"}};
DispString_t str_spot_cls[]	 = 	{{1,31,"          "}
								,{1,32,"         "}};

const struct ZW_POS *m_pudtActManPos;

	// Positionen für die XY-Justierun
const struct ZW_POS m_udtPos_0			 = {0,"  "};
const struct ZW_POS m_udtPos_1			 = {1,"A1"};
const struct ZW_POS m_udtPos_4			 = {4,"B2"};
const struct ZW_POS m_udtPos_x			 = {70,"X "};
const struct ZW_POS m_udtPos_y			 = {71,"Y "};
const struct ZW_POS m_udtPos_120		 = {120,"A1"};
const struct ZW_POS m_udtPos_135		 = {135,"D4"};
const struct ZW_POS m_udtPos_x_1536		 = {1,"A1"};
//const struct ZW_POS m_udtPos_y_1536	 = {2,"A1"};

const struct ZW_POS str_zw_pos_384[]		 = 	{{0,"  "},{1,"A1"},{2,"A2"},{3,"B1"},{4,"B2"}};
const struct ZW_POS str_zw_pos_384_OL3714[]	 = 	{{0,"  "},{1,"A1"},{2,"A2"},{3,"B1"},{4,"B2"}};

const struct ZW_POS str_zw_pos_1536[]		 = 	{{0,"  "},{120,"A1"},{121,"A2"},{122,"A3"},{123,"A4"},{124,"B1"},
												{125,"B2"},{126,"B3"},{127,"B4"},{128,"C1"},{129,"C2"},
												{130,"C3"},{131,"C4"},{132,"D1"},{133,"D2"},{134,"D3"},
												{135,"D4"}};

const struct ZW_POS str_zw_pos1536_1536[]	 = 	{{0,"  "},{1,"A1"},{2,"A3"},{3,"C1"},{4,"C3"}};	//nur für den Justiermode!

DispString_t str_A1A2[]			 = 	{{1,1,"A1-A2"}
									,{1,1,"A1-A2"}};
DispString_t str_A1B1[]			 = 	{{2,1,"A1-B1"}
									,{2,1,"A1-B1"}};

//DispString_t str_B2B3[]		 = 	{{1,1,"B2-B3"},{1,1,"B2-B3"}};
//DispString_t str_B2A2[]		 = 	{{2,1,"B2-A2"},{2,1,"B2-A2"}};

DispString_t str_xy_go_esc[]	 = 	{{2,29,"Abbr:\004\005 OK:\002"}
									,{2,30,"Esc:\004\005 OK:\002"}};
DispString_t str_zero_x[]		 = 	{{1,7,"Nullpkt:"}
									,{1,7,"Zero:"}};
DispString_t str_way_x[]		 = 	{{1,19,"Weg:"}
									,{1,18,"Way:"}};
DispString_t str_zero_y[]		 = 	{{2,7,"Nullpkt:"}
									,{2,7,"Zero:"}};
DispString_t str_way_y[]		 = 	{{2,19,"Weg:"}
									,{2,18,"Way:"}};
DispString_t str_JM1536[]		 = 	{{1,1,"Justierhilfe XY(1536) eingesetzt?"}
									,{1,1,"Tool to adjust XY(1536) inserted?"}};

	// Einstellung der Parameter für den XY -Tisch (mit 96er Kopf)
const struct ADJ_XY g_audtXYConfig384[]	=
{
										{str_zero_x,XProgOffset,&g_udtServoAdjustPara.intXOffset,&m_udtPos_0}
										,{str_way_x,XProgSlope,&g_udtServoAdjustPara.intXSlope,&m_udtPos_1}
										,{str_zero_y,YProgOffset,&g_udtServoAdjustPara.intYOffset,&m_udtPos_0}
										,{str_way_y,YProgSlope,&g_udtServoAdjustPara.intYSlope,&m_udtPos_4}
};

const struct ADJ_XY g_audtXYConfig384_OL3714[]	 =
{
										{str_zero_x,XProgOffset,&g_udtServoAdjustPara.intXOffset,&m_udtPos_0}
										,{str_way_x,XProgSlope,&g_udtServoAdjustPara.intXSlope,&m_udtPos_x}
										,{str_zero_y,YProgOffset,&g_udtServoAdjustPara.intYOffset,&m_udtPos_0}
										,{str_way_y,YProgSlope,&g_udtServoAdjustPara.intYSlope,&m_udtPos_y}
};

	// Einstellung der Parameter für den XY -Tisch	(mit 96er Kopf)
const struct ADJ_XY m_audtXYConfig1536[]		 =
	{{str_zero_x,XProgOffset,&g_udtServoAdjustPara.intXOffset,&m_udtPos_0}
	,{str_way_x,XProgSlope,&g_udtServoAdjustPara.intXSlope,&m_udtPos_120}
	,{str_zero_y,YProgOffset,&g_udtServoAdjustPara.intYOffset,&m_udtPos_0}
	,{str_way_y,YProgSlope,&g_udtServoAdjustPara.intYSlope,&m_udtPos_135}};

	// Einstellung der Parameter für den XY -Tisch	mit 1536er Kopf,an sich ist nur der Nullpunkt für 1536 von Interesse,
	// da aber Wechselkopf-Gerät wird bei einem anderen Kopf auch die Auslenkung benötigt
const struct ADJ_XY g_audtXYConfig1536_1536[]	 =
	{{str_zero_x,XProgOffset,&g_udtServoAdjustPara.intXOffset,&m_udtPos_0}
	,{str_way_x,XProgSlope,&g_udtServoAdjustPara.intXSlope,&m_udtPos_x_1536}
	,{str_zero_y,YProgOffset,&g_udtServoAdjustPara.intYOffset,&m_udtPos_0}
	,{str_way_y,YProgSlope,&g_udtServoAdjustPara.intYSlope,&m_udtPos_x_1536}};

 /* Platzpositionen für den 384er-CyBi-Well */
const struct POS_CTRL m_audtXYPosTab384[]	 =
	{{0,0,		0}			/* 0-Position fuer die 384 Wells	*/
	,{1,1.125,-1.125}		/* A1-Position fuer 1536-Wells		*/
	,{2,-1.125,-1.125}		/* A2 ...							*/
	,{3,1.125,1.12}		/* B1 ...							*/
	,{4,-1.125,1.125}		/* B2 ...							*/
	,{5,0,		0}			/* AB1								*/
	,{6,-2.25,-2.25}		/* AB2 (vorher 0)ab Version 3.40	*/

	,{11,0.50,-0.50}		/* tip touch Pos 1 - 384er Wells	*/
	,{12,-0.50,-0.50}		/* tip touch Pos 2 - 384er Wells	*/
	,{13,-0.50,0.50}		/* tip touch Pos 3 - 384er Wells	*/
	,{14,0.50,0.50}		/* tip touch Pos 4 - 384er Wells	*/

	,{125,1.125,-1.125}	/* Position B2 für 1536er PLATE		*/
	,{126,-1.125,-1.125}	/* Position B3 für 1536er PLATE		*/
	,{129,1.125,1.125}		/* Position C2 für 1536er PLATE		*/
	,{130,-1.125,1.125}};	/* Position C3 für 1536er PLATE		*/

 /* Platzpositionen für den 384er-LHA-Pipettor OL3714 */
const struct POS_CTRL m_audtXYPosTab_384_OL3714[]	 =
	{{0,0,		0}			/* 0-Position fuer die 384 Wells	*/
	,{1,1.125,-1.125}		/* A1-Position fuer 1536-Wells		*/
	,{2,-1.125,-1.125}		/* A2 ...							*/
	,{3,1.125,1.125}		/* B1 ...							*/
	,{4,-1.125,1.125}		/* B2 ...							*/
	,{5,0,		0}			/* AB1								*/
	,{6,-2.25,	0}			/* AB2 Unterschiedlich zum CyBi-Well vario */

	,{11,0.50,-0.50}		/* tip touch Pos 1 - 384er Wells	*/
	,{12,-0.50,-0.50}		/* tip touch Pos 2 - 384er Wells	*/
	,{13,-0.50,0.50}		/* tip touch Pos 3 - 384er Wells	*/
	,{14,0.50,0.50}		/* tip touch Pos 4 - 384er Wells	*/

	,{70,-2.25,	0,}			/* Justierposition X		*/
	,{71,0,  -2.25}		/* Justierposition Y		*/

	,{125,1.125,-1.125}	/* Position B2 für 1536er PLATE		*/
	,{126,-1.125,-1.125}	/* Position B3 für 1536er PLATE		*/
	,{129,1.125,1.125}	/* Position C2 für 1536er PLATE		*/
	,{130,-1.125,1.125}};	/* Position C3 für 1536er PLATE		*/

 /* Platzpositionen für den 96er-CyBi-Well */
const struct POS_CTRL m_audtXYPosTab96[]	 =
	{{0,0,	   0}			/* 0-Position fuer die 96er Wells	*/		//pos_tab_small
	,{1,2.25,-2.25}		/* A1-Position fuer 384-Wells		*/
	,{2,-2.25,-2.25}		/* A2 ...							*/
	,{3,2.25,2.25}		/* B1 ...							*/
	,{4,-2.25,2.25}		/* B2 ...							*/
	,{5,2.25,0}			/* AB1 für eine 192er Wells			*/
	,{6,-2.25,-4.0}			/* AB2 für eine 192er Wells (0) ab Version 3.40	*/
	,{7,0,	  -2.25}		/* A12 für US-Wanne					*/
	,{8,0,   2.25}		/* B12 für US-Wanne					*/

	/* tip touch Positionen für 96er Well-Platte */
	,{11,1.30,-1.30}	/* tip touch Pos 1 für 96er Wells   */
	,{12,-1.30,-1.30}	/* tip touch Pos 2 für 96er Wells   */
	,{13,-1.30,1.30}	/* tip touch Pos 3 für 96er Wells   */
	,{14,1.30,1.30}		/* tip touch Pos 4 für 96er Wells   */

	/* tip touch Positionen für 384er Well-Platte */
	,{21,2.75,-2.75}		/* tip touch Pos 1 für A1 Well		*/
	,{22,1.75,-2.75}		/* tip touch Pos 2 für A1 Well		*/
	,{23,1.75,-1.75}		/* tip touch Pos 3 für A1 Well		*/
	,{24,2.75,-1.75}		/* tip touch Pos 4 für A1 Well		*/

	,{31,-1.75,-2.75}		/* tip touch Pos 1 für A2 Well		*/
	,{32,-2.75,-2.75}		/* tip touch Pos 2 für A2 Well		*/
	,{33,-2.75,-1.75}		/* tip touch Pos 3 für A2 Well		*/
	,{34,-1.75,-1.75}		/* tip touch Pos 4 für A2 Well		*/

	,{41, 2.75,1.75}		/* tip touch Pos 1 für B1 Well		*/
	,{42, 1.75,1.75}		/* tip touch Pos 2 für B1 Well		*/
	,{43, 1.75,2.75}		/* tip touch Pos 3 für B1 Well		*/
	,{44, 2.75,2.75}		/* tip touch Pos 4 für B1 Well		*/

	,{51,-1.75,1.75}		/* tip touch Pos 1 für B2 Well		*/
	,{52,-2.75,1.75}		/* tip touch Pos 2 für B2 Well		*/
	,{53,-2.75,2.75}		/* tip touch Pos 3 für B2 Well		*/
	,{54,-1.75,2.75}		/* tip touch Pos 4 für B2 Well		*/

	/* 24-well Platte,Spezialmagazin mit 24 Spitzen */
	,{61, 4.5,-4.5}		/* Pos. für Transkript u. 24 well  A1*/
	,{62,	 0,-4.5}		/*				"					*/
	,{63,-4.5,-4.5}		/*				"					A2*/
	,{64, 4.5,	 0}			/*				"					*/
	,{65,-4.5,	 0}			/*				"					*/
	,{66, 4.5, 4.5}		/*				"					B1*/
	,{67,	 0, 4.5}		/*				"					*/
	,{68,-4.5, 4.5}		/*				"					B2*/

	/* tip touch Positionen für 864er Well-Platte */
	,{101, 3.0,-3.0}		/* Position A1 für 864er PLATE		*/
	,{102, 0.0,-3.0}		/* Position A2 für 864er PLATE		*/
	,{103,-3.0,-3.0}		/* Position A3 für 864er PLATE		*/
	,{104, 3.0, 0.0}		/* Position B1 für 864er PLATE		*/
	,{105, 0.0, 0.0}		/* Position B2 für 864er PLATE		*/
	,{106,-3.0, 0.0}		/* Position B3 für 864er PLATE		*/
	,{107, 3.0, 3.0}		/* Position C1 für 864er PLATE		*/
	,{108, 0.0, 3.0}		/* Position C2 für 864er PLATE		*/
	,{109,-3.0, 3.0}		/* Position C3 für 864er PLATE		*/

	/* tip touch Positionen für 1536er Well-Platte */
	,{120,3.375,-3.375}	/* Position A1 für 1536er PLATE 		*/
	,{121,1.125,-3.375}	/* Position A2 für 1536er PLATE		*/
	,{122,-1.125,-3.375}	/* Position A3 für 1536er PLATE		*/
	,{123,-3.375,-3.375}	/* Position A4 für 1536er PLATE		*/
	,{124,3.375,-1.125}	/* Position B1 für 1536er PLATE		*/
	,{125,1.125,-1.125}	/* Position B2 für 1536er PLATE		*/
	,{126,-1.125,-1.125}	/* Position B3 für 1536er PLATE		*/
	,{127,-3.375,-1.125}	/* Position B4 für 1536er PLATE		*/
	,{128,3.375,1.125}	/* Position C1 für 1536er PLATE		*/
	,{129,1.125,1.125}	/* Position C2 für 1536er PLATE		*/
	,{130,-1.125,1.125}	/* Position C3 für 1536er PLATE		*/
	,{131,-3.375,1.125}	/* Position C4 für 1536er PLATE		*/
	,{132,3.375,3.375}	/* Position D1 für 1536er PLATE		*/
	,{133,1.125,3.375}	/* Position D2 für 1536er PLATE		*/
	,{134,-1.125,3.375}	/* Position D3 für 1536er PLATE		*/
	,{135,-3.375,3.375}};	/* Position D4 für 1536er PLATE		*/

 /* Platzpositionen für den 1536er-CyBi-Well mit folgender Anordnung bezogen auf Prüfmittel OL...
	wobei Y die Grundposition representiert
	A1	A3
	x  Y  x
	C1	C3
*/
const struct POS_CTRL m_audtXYPosTab1536[]	 =
	{{0,  0,  0}		/* Grund-Position fuer die 1536 Wells	*/
	,{1,2.25,-2.25}	/* A1-Position der 1536-Well-Platte für Prüfmittel OL5021-10-119 */
	,{2,-2.25,-2.25}	/* A3-Position der 1536-Well-Platte für Prüfmittel OL5021-10-119 */
	,{3,2.25,2.25}	/* C1-Position der 1536-Well-Platte für Prüfmittel OL5021-10-119 */
	,{4,-2.25,2.25}};	/* C3-Position der 1536-Well-Platte für Prüfmittel OL5021-10-119 */

const struct POS_CTRL *ptrXYActTablePos;	// Aktueller Tabellenplatz

	// Denkbar auch DeviceType96_96,DeviceType384_384,je ohne XY-Tisch
struct DEV_TYPE g_udtDeviceType96_384	 =
	{str_zw_pos_384
	,sizeof(str_zw_pos_384)/sizeof(struct ZW_POS)
	,g_audtXYConfig384
	,sizeof(g_audtXYConfig384)/sizeof(struct ADJ_XY)
	,m_audtXYPosTab96
	,sizeof(m_audtXYPosTab96)/sizeof(struct POS_CTRL)};

struct DEV_TYPE g_udtDeviceType96_1536	 =
	{str_zw_pos_1536
	,sizeof(str_zw_pos_1536)/sizeof(struct ZW_POS)
	,m_audtXYConfig1536
	,sizeof(m_audtXYConfig1536)/sizeof(struct ADJ_XY)
	,m_audtXYPosTab96
	,sizeof(m_audtXYPosTab96)/sizeof(struct POS_CTRL)};

struct DEV_TYPE DeviceType384_1536	 =
	{str_zw_pos_384
	,sizeof(str_zw_pos_384)/sizeof(struct ZW_POS)
	,g_audtXYConfig384
	,sizeof(g_audtXYConfig384)/sizeof(struct ADJ_XY)
	,m_audtXYPosTab384
	,sizeof(m_audtXYPosTab384)/sizeof(struct POS_CTRL)};

struct DEV_TYPE DeviceType384_1536_OL3714	 =
	{str_zw_pos_384_OL3714
	,sizeof(str_zw_pos_384_OL3714)/sizeof(struct ZW_POS)
	,g_audtXYConfig384_OL3714
	,sizeof(g_audtXYConfig384_OL3714)/sizeof(struct ADJ_XY)
	,m_audtXYPosTab_384_OL3714
	,sizeof(m_audtXYPosTab_384_OL3714)/sizeof(struct POS_CTRL)};

struct DEV_TYPE DeviceType1536_1536	 =
	{str_zw_pos1536_1536
	,sizeof(str_zw_pos1536_1536)/sizeof(struct ZW_POS)
	,g_audtXYConfig1536_1536
	,sizeof(g_audtXYConfig1536_1536)/sizeof(struct ADJ_XY)
	,m_audtXYPosTab1536
	,sizeof(m_audtXYPosTab1536)/sizeof(struct POS_CTRL)};

//#pragma noclear
struct DEV_TYPE *g_pudtActDeviceType;	// Copy im EEPROM
//#pragma clear

/** private data **/

/* FUNCTION: 	XYCheckAndLimit
   PARAMETERS:	Ist-,Max- und Min-Wert	der Servo-Abgleichkoeffizienten
   DESCRIPTION:	Prüfen und Begrenzen des Istwertes innerhalb seines Wertebereiches
   RETURNS: 	Istwert */
int XYCheckAndLimit(int ist,int max,int min)
{
  if(ist > max) ist = max;
  if(ist < min) ist = min;
  return ist;
}

void GetXYAdjustVar(struct XYAdj *ptr)
{
  g_udtServoAdjustPara.intXOffset = ptr->intXOffset; g_udtServoAdjustPara.intYOffset = ptr->intYOffset;
  g_udtServoAdjustPara.intXSlope = ptr->intXSlope; g_udtServoAdjustPara.intYSlope = ptr->intYSlope;
}

void SaveXYAdjustVar(struct XYAdj *ptr)
{
  ptr->intXOffset = g_udtServoAdjustPara.intXOffset; ptr->intYOffset = g_udtServoAdjustPara.intYOffset;
  ptr->intXSlope = g_udtServoAdjustPara.intXSlope; ptr->intYSlope = g_udtServoAdjustPara.intYSlope;
}

/* FUNCTION: 	XProgOffset
   PARAMETERS:	Inkrement
   DESCRIPTION:	Setzen des globalen X-Offsetwertes
   RETURNS: 	--- */
void XProgOffset(int8_t i)
{
  g_udtServoAdjustPara.intXOffset = XYCheckAndLimit(g_udtServoAdjustPara.intXOffset+i,m_iServoXYOffsetMax,m_iServoXYOffsetMin);
}

/* FUNCTION: 	XProgSlope
   PARAMETERS:	Inkrement
   DESCRIPTION:	Setzen des globalen X-Anstiegswertes
   RETURNS: 	--- */
void XProgSlope(int8_t i)
{
  g_udtServoAdjustPara.intXSlope = XYCheckAndLimit(g_udtServoAdjustPara.intXSlope+i,m_iServoXYOffsetMax,m_iServoXYOffsetMin);
}

/* FUNCTION: 	YProgOffset
   PARAMETERS:	Inkrement
   DESCRIPTION:	Setzen des globalen Y-Offsetwertes
   RETURNS: 	--- */
void YProgOffset(int8_t i)
{
  g_udtServoAdjustPara.intYOffset = XYCheckAndLimit(g_udtServoAdjustPara.intYOffset+i,m_iServoXYOffsetMax,m_iServoXYOffsetMin);
}

/* FUNCTION: 	YProgSlope
   PARAMETERS:	Inkrement
   DESCRIPTION:	Setzen des globalen Y-Anstiegswertes
   RETURNS: 	--- */
void YProgSlope(int8_t i)
{
  g_udtServoAdjustPara.intYSlope = XYCheckAndLimit(g_udtServoAdjustPara.intYSlope+i,m_iServoXYSlopeMax,m_iServoXYSlopeMin);
}

void XYInitStage(const struct ZW_POS *ptr)
{
  //m_iXYActPosNo = g_pudtActDeviceType->mpos[show_pos].no;
  m_iXYActPosNo = ptr->no;
  if(g_fVertDrvCurrentPos < g_fVertXYSwitchPos) VertDrvWay(g_fVertXYSwitchPos);
  XYServoInit(m_iXYActPosNo);
  ShowLocation(FALSE);
  ShowXYPosition(ptr->show);
}

/* FUNCTION: 	XYProgConfig1536
   PARAMETERS:	---
   DESCRIPTION:	Konfigurationsprogramm für die XY-Parameter	zusammen mit dem Justiermagazin OL5021-10-119
   RETURNS: 	--- */
void XYProgConfig1536(void)
{
  // Abfrage PM JM 1536 eingesetzt
  LcdClear(ALL_LINE);
  Display(str_JM1536); Display(str_xy_go_esc);
  if(WaitForAbortOrEnter() == S_KEY) return;
  ShowInit();
  g_bAdjustMode1536 = TRUE;
  XYServoInit(0);	/* Initialisierung des Tisches,um Bewegung zu ermöglichen */
  VertDrvWay(g_fVertXYSwitchPos);
  LcdClear(ALL_LINE);
  Display(str_A1A2); Display(str_A1B1); Display(str_xy_go_esc); g_bShowSpots384 = TRUE;

  XYProgModify(g_audtXYConfig1536_1536,sizeof(g_audtXYConfig1536_1536)/sizeof(struct ADJ_XY),0,NO_LINE);
  g_bShowSpots384 = FALSE;
  ShowInit();
  ErrorDisplay(EEPROMSetServoAdjustPara());
  XYServoInit(0); Wait(20); m_iXYActPosNo = 0; VertDrvWay(0);
  g_bAdjustMode1536 = FALSE;
}

/* FUNCTION: 	XYProgConfig
   PARAMETERS:	---
   DESCRIPTION:	Konfigurationsprogramm für die XY-Parameter
   RETURNS: 	--- */
void XYProgConfig(void)
{
  ShowInit();
  XYServoInit(0);	/* Initialisierung des Tisches,um Bewegung zu ermöglichen */
  VertDrvWay(g_fVertXYSwitchPos);
  LcdClear(ALL_LINE);
  Display(str_A1A2); Display(str_A1B1); Display(str_xy_go_esc); g_bShowSpots384 = TRUE;

  XYProgModify(g_pudtActDeviceType->adj,g_pudtActDeviceType->CntAdjXY,0,NO_LINE);
  g_bShowSpots384 = FALSE;
  ShowInit();
  ErrorDisplay(EEPROMSetServoAdjustPara());
  XYServoInit(0); Wait(20); m_iXYActPosNo = 0; VertDrvWay(0);
}

/* FUNCTION: 	XYProgModify
   PARAMETERS:	Zeiger auf Menü- und Programmstruktur,Anz. der Einträge,Startpos.,Flag Disp. löschen
   DESCRIPTION:	Einstellen der Offset- und Verstärkungswerte am Servo (beide Kanäle)
   RETURNS: 	Index des zuletzt bearbeiteten Menüpunktes */
ubyte XYProgModify(const struct ADJ_XY *fptr,int8_t len, int8_t beg,int8_t cls)
{
	int8_t id,i;
	struct ADJ_XY *hptr;
	struct ADJ_XY *pmod_xy;
	struct XYAdj h_xy;
	struct ADJ_XY adjHelp[sizeof(g_audtXYConfig384)/sizeof(struct ADJ_XY)],*ptr;

	memcpy(&adjHelp,fptr,len * sizeof(struct ADJ_XY));	//copy far to near
	ptr = adjHelp;
	pmod_xy = ptr;id = beg; hptr = ptr;
	SaveXYAdjustVar(&h_xy);
	XYAdjustDisplay(ptr,len,cls);
	//for(i = 0;i<COUNT_XY_F;i++) {XYShowAdjPara(hptr,FALSE);hptr++;}	//OL3714
	for(i = 0;i<len;i++)
	{
		XYShowAdjPara(hptr,FALSE);
		hptr++;
	}
	pmod_xy = ptr+beg; XYShowAdjPara(pmod_xy,TRUE);
	while(1)
	{
		XYShowAdjPara(pmod_xy,FALSE);	/* nur den Cursor setzen */
		switch (KeyWaitForAny())
		{
			case ARR_LEFT_KEY:	BeepCtrl(BEEP_TIME);
								if(--pmod_xy < ptr)
								{
									pmod_xy = ptr+len-1;
								}
								if(--id < 0)
								{
									id = len-1;
								}
								XYInitStage(pmod_xy->adj_pos);
								break;

			case ARR_RIGHT_KEY: BeepCtrl(BEEP_TIME);
								if(++pmod_xy == ptr+len)
								{
									pmod_xy = ptr;
								}
								if(++id == len)
								{
									id = 0;
								}
								XYInitStage(pmod_xy->adj_pos);
								break;

			case ARR_UP_KEY:	pmod_xy->fp(1);
								XYShowAdjPara(pmod_xy,TRUE);
								break;

			case ARR_DOWN_KEY:	pmod_xy->fp(-1);
								XYShowAdjPara(pmod_xy,TRUE);
								break;

			case ENTER_KEY:		return_xy();
								return id;

			case S_KEY:			GetXYAdjustVar(&h_xy);
								return_xy();
								return id;
		}
	}
}

void return_xy(void){
  BeepCtrl(BEEP_TIME);SetCursorOff();
  //man_z_para();
}

/* FUNCTION: 	XYAdjustDisplay
   PARAMETERS:	Zeiger auf Menü- und Programmstruktur,Anz. der Einträge,Flag Disp. löschen
   DESCRIPTION:	Aufbau des Justiermenüs
   RETURNS: 	--- */
void XYAdjustDisplay(const struct ADJ_XY *ptr,unsigned char len,unsigned char cls){
  int i;
  DispString_t *s;
  LcdClear(cls);
  for(i = 0;i<len;i++) {s = ptr->msg; Display(s);ptr++;WDO();} /*Anzeige aufbauen */
}

/* FUNCTION: 	XYShowAdjPara
   PARAMETERS:	Zeiger auf Menü- und Programmstruktur,Flag für Servoinitialisierung
   DESCRIPTION:	Anzeige des Justierparameterwertes
   RETURNS: 	--- */
void XYShowAdjPara(const struct ADJ_XY *xy,char exe_flag){
  DispString_t *s;
  char h_disp[LCD_DIGITS+2];
  char line,column;
  s = xy->msg; s+= g_ubytLang; line = s->line; column = (s->column)+(char)strlen(s->message);
  SetCursorOff();
  sprintf(h_disp,"%3d",(int)*(xy->way)); LcdShow(line,column,h_disp);
  if(exe_flag) XYInitStage(xy->adj_pos);
  SetCursorPos(line,column+2);
}

/* FUNCTION: 	XYCheckAdjPara
   PARAMETERS:	---
   DESCRIPTION:	Prüfen der durch Batterie gestützten Justierparameter
   RETURNS: 	TRUE / FALSE */
ubyte XYCheckAdjPara(void){
  if(CheckInt(m_iServoXYOffsetMin,m_iServoXYOffsetMax,g_udtServoAdjustPara.intXOffset)
  && CheckInt(m_iServoXYOffsetMin,m_iServoXYOffsetMax,g_udtServoAdjustPara.intYOffset)
  && CheckInt(m_iServoXYSlopeMin,m_iServoXYSlopeMax,g_udtServoAdjustPara.intXSlope)
  && CheckInt(m_iServoXYSlopeMin,m_iServoXYSlopeMax,g_udtServoAdjustPara.intYSlope))
	  return TRUE;
  return FALSE;
}

/* FUNCTION: 	XYSetAdjParaToDefault
   PARAMETERS:	---
   DESCRIPTION:	Defaulparameter den Variablen zuweisen
   RETURNS: 	--- */
void XYSetAdjParaToDefault(void){
  g_udtServoAdjustPara.intXOffset = 0; g_udtServoAdjustPara.intYOffset = 0;
  g_udtServoAdjustPara.intXSlope = 0; g_udtServoAdjustPara.intYSlope = 0;
}

/* FUNCTION: 	XYServoInit
   PARAMETERS:	Platznummer
   DESCRIPTION:	Initialisierung der Servos mit akt. Parametern und Einstellen der Platznummer
   RETURNS: 	--- */
void XYServoInit(ubyte no){
  ptrXYActTablePos = XYServoCheckAndGetPos(no);  /* Einstellen des Zeigers auf no */
  XYServoSetZeroAndSlope();
  XYServoWay(ptrXYActTablePos->pos_x,ptrXYActTablePos->pos_y);
  m_iXYActPosNo = ptrXYActTablePos->no;
  g_ubytDrvInitState &= 0xFB; g_fServoOffsetX = 0;
}

/* FUNCTION: 	XYServoSetZeroAndSlope
   PARAMETERS:	---
   DESCRIPTION:	Berechnen der Funktionskoeffizienten für Anstieg und Nullpunkt
   RETURNS: 	--- */
void XYServoSetZeroAndSlope(void){
  m_udtServoFuncPara.XSlope	 = (-g_udtServoAdjustPara.intXSlope)	* (float)ACC_SCOPE 						+ (float)ACC_START;
  m_udtServoFuncPara.XOffset = g_udtServoAdjustPara.intXOffset	* (float)ZERO_SCOPE * (float)TIME_GRID 	+ (float)ZERO_START;
  m_udtServoFuncPara.YSlope	 = (-g_udtServoAdjustPara.intYSlope)	* (float)ACC_SCOPE 						+ (float)ACC_START;
  m_udtServoFuncPara.YOffset = g_udtServoAdjustPara.intYOffset	* (float)ZERO_SCOPE * (float)TIME_GRID 	+ (float)ZERO_START;
}

/* FUNCTION: 	XYServoCalcWayToTimeGrid
   PARAMETERS:	Weg,Anstiegskoeffizient,Nullpunktkoeffizient
   DESCRIPTION:	Berechnen der Funktionskoeffizienten für Anstieg und Nullpunkt
   RETURNS: 	Zeitraster für die PWM-Steuerung */
uword XYServoCalcWayToTimeGrid(float way,float acc,float zero){
	uword help;
	float h1;

// 2022.09.30 mku
// ---------- old ----------
//	help = (uword)((asin(way / RADIUS_FIX_POS) * (180 / PI) /acc 			+ zero) / (float) TIME_GRID);
// ---------- analyse  ----------
//	help = 						RADIUS_FIX_POS				acc_start			zero_start		(1/pwm_clk)
//								7,5mm 						0,086 [°/µs]		1520 [µs]		3,2 [µs]
//														 = 	51,6 / 600 [°/µs]
// S3115 (X/Y Servo) hat aber				 			    60,0 / 600 [°/µs]
//					 acc_start müsste also				->	0,100 [°/µs] sein
// A C H T U N G ! Berechnung trotz der fehlerhaften Umrechnungskonstante NICHT korrigieren!
// Anderfalls müssten sämliche zugehörige bisher einmessene und abgespeicherte Korrekturwerte ebenfalls korrigiert werden!
//  ---------- new ----------
// Umrechnungskonstante TIME_GRID entfällt,da der neue Timer bereits genau 1 µs je  umsetzt
	help = (uword)(asin(way / RADIUS_FIX_POS) * 180 / PI /acc + zero);

	if(g_bTestFlag){
		h1 = asin(way/RADIUS_FIX_POS)*(180/PI);
		printf(" way:%5.2f alpha:%5.2f time grid:%d\r",
			 (double)way									// Strecke in mm
			,(double)h1										// Winkel in Grad
			,(int)help);									// Impulslänge in Digit (old time[µs]/3,2; new: 1µs/Digit)
		fflush(stdout);
	}
	return help;
}

/* FUNCTION: 	XYServoCheckAndGetPos
   PARAMETERS:	Platznummer
   DESCRIPTION:	Prüfen auf gültige Platznummer
   RETURNS: 	Zeiger auf die Tabellenpos. des Platzes / Nullzeiger,falls Platz nicht exsitiert */
const struct POS_CTRL *XYServoCheckAndGetPos(ubyte no){
  ubyte i;
  const struct POS_CTRL *ptrHelp;
  ptrHelp = g_pudtActDeviceType->pos;
  for(i = 0; i < g_pudtActDeviceType->CntPos; i++){
	  if(no == ptrHelp->no) return ptrHelp;
	  else ptrHelp++;
  }
  return NULL;
}

/* FUNCTION: 	ShowLocation
   PARAMETERS:	Flag für Anzeigestatus (flasch --> nur Wagenposition wird angezeigt)
   DESCRIPTION:	Anzeigen der Platznummer und der XY-Position
   RETURNS: 	--- */
void ShowLocation(ubyte complete){
	int8_t i;
  char hdisp[15]; ubyte Len;
  const ubyte *pPos,*phPos;
  struct CurPos hCurPos;
  const struct ZW_POS *udtPtr;
  if(g_bShowSpots384)  			// Anzeige freigegeben
	{GetCursorPos(&hCurPos); 	// Retten der aktuellen Cursorposition
	ClearStringOnDisplay(str_spot_cls);
	/* Umkodieren der Platznummer wegen den 4er Tisch */
	phPos = g_pudtHrzPosTabPtr->CarPara->ManLoc->Location; Len = g_pudtHrzPosTabPtr->CarPara->ManLoc->Len;
	for(pPos = phPos; pPos <= phPos+Len; pPos++) if(*pPos == g_ubytHrzActNo) break;
	if(pPos == phPos+Len+1) pPos = phPos;	//falls durch die Vorgeschichte kein vorhandener manueller Platz eingestellt war
	i = pPos-phPos+1;
	sprintf(hdisp,"%s%1d",str_spot[g_ubytLang].message,(int)i);
 	//printf("Show:XY-No.:%d\r",m_iXYActPosNo);fflush(stdout);
	udtPtr = g_pudtActDeviceType->mpos;	// Suchen der Platzposition von m_iXYActPosNo
	for(i = 0; i< g_pudtActDeviceType->CntZwPos; i++) if(udtPtr->no == m_iXYActPosNo) break; else udtPtr++;
	if((complete)&&(i < g_pudtActDeviceType->CntZwPos)) strcat(hdisp,udtPtr->show);
	LcdShow(1,str_spot[g_ubytLang].column,hdisp);
	if(g_bCursorState == TRUE) SetCursorPos(hCurPos.Line,hCurPos.Row);
  }
}

/* FUNCTION: 	ShowXYPosition
   PARAMETERS:	Zeichenkette
   DESCRIPTION:	Anzeigen der XY-Position
   RETURNS: 	--- */
void ShowXYPosition(char *str){
  struct CurPos hCurPos;
  GetCursorPos(&hCurPos);
  LcdShow(1,str_spot[g_ubytLang].column+strlen(str_spot[g_ubytLang].message)+1,str);
  SetCursorPos(hCurPos.Line,hCurPos.Row);
}

/* FUNCTION: 	XYServoWay
   PARAMETERS:	Weg X- und Y-Koordinate
   DESCRIPTION: Weg in Zeitraster für die PWM umsetzen und ausgeben
   RETURNS: 	CB_READY / IERR_PARA */
ubyte XYServoWay(float way_x,float way_y){
	uword act_time_x,act_time_y;
	static uword old_time_x,old_time_y;
	if((way_x > X_SERVO_MAX_WAY) || (way_x < X_SERVO_MIN_WAY)) return IERR_PARA;
	if((way_y > Y_SERVO_MAX_WAY) || (way_y < Y_SERVO_MIN_WAY)) return IERR_PARA;
	m_fXServoActWay = way_x;
	m_fYServoActWay = way_y;

	act_time_x = XYServoCalcWayToTimeGrid(way_x,m_udtServoFuncPara.XSlope,m_udtServoFuncPara.XOffset); /* X-Koordinate */
	act_time_y = XYServoCalcWayToTimeGrid(way_y,m_udtServoFuncPara.YSlope,m_udtServoFuncPara.YOffset); /* Y-Koordinate */

	XYServoSetPWM(act_time_x,act_time_y);																									// Pausenzeit setzen und starten

	if((old_time_x != act_time_x) || (old_time_y != act_time_y)){
		old_time_x = act_time_x;
		old_time_y = act_time_y;
		uint32_t lifePos;
		EEPROMReadLifeParaPosition(&lifePos);
		g_udtLifePara.ulngXYCycles++;
		ErrorDisplay(EEPROMWriteLifePara(&g_udtLifePara));
	}

	if((way_x == 0) && (way_y == 0)) m_iXYActPosNo = 0;

	Wait(EXECUTION_TIME);
	return CB_READY;
}

/* FUNCTION: 	XYServoCheckZeroPos
   PARAMETERS:	---
   DESCRIPTION: Prüfen der Servostg auf Null-Position
   RETURNS: 	TRUE / FALSE */
ubyte XYServoCheckZeroPos(void){
  if((m_fXServoActWay == 0)&&(m_fYServoActWay == 0)) return TRUE;
  return FALSE;
}

/* FUNCTION: 	XYNextManPos
   PARAMETERS:	Inkrement
   DESCRIPTION: Einstellen der nächsten manuellen Tischposition
   RETURNS: 	--- */
void XYNextManPos(int8_t dir){
//  if(!flag_xy) return;
//  if(g_bShowSpots384 && !IsHeadType1536())
  if(g_bShowSpots384){

  	if(!IsHeadType1536() || g_bAdjustMode1536){	//normalerweise gibt es keine XY-Positionen beim 1536-Kopf,sondern nur als Justierhilfe
		if(m_iXYActPosNo == 0) m_pudtActManPos = g_pudtActDeviceType->mpos;	// Zeiger synchronisieren
		m_pudtActManPos+= dir;
		if(m_pudtActManPos > (g_pudtActDeviceType->mpos+g_pudtActDeviceType->CntZwPos-1))	m_pudtActManPos = g_pudtActDeviceType->mpos;
		if(m_pudtActManPos <  g_pudtActDeviceType->mpos)	m_pudtActManPos = g_pudtActDeviceType->mpos + g_pudtActDeviceType->CntZwPos -1;
		ErrorDisplay(XYServoPos(m_pudtActManPos->no));
//		printf("XY-No.:%d\r",m_iXYActPosNo);fflush(stdout);
		ShowLocation(TRUE); Wait(40);
	}
  }
}

/* FUNCTION: 	XYServoPos
   PARAMETERS:	Tischposition
   DESCRIPTION: Einstellen der Tischposition
   RETURNS: 	CB_READY / IERR_PARA */
ubyte XYServoPos(ubyte no){
  const struct POS_CTRL *ptr;
  ptr = XYServoCheckAndGetPos(no);  /* Einstellen des Zeigers auf no */
  if(ptr == NULL) return IERR_PARA;
  ptrXYActTablePos = ptr;
  XYServoWay(ptrXYActTablePos->pos_x,ptrXYActTablePos->pos_y);
  m_iXYActPosNo = ptrXYActTablePos->no;
  return CB_READY;
}

/* FUNCTION: 	XYPlateTypeCmp
   PARAMETERS:	Ziel- und Destinationpointer der Struktur "DEV_TYPE"
   DESCRIPTION: Vergleich der Inhalte
   RETURNS: 	TRUE / FALSE */
ubyte XYPlateTypeCmp(struct DEV_TYPE *pScr,struct DEV_TYPE *pDest){
  if((pScr->mpos == pDest->mpos)&&(pScr->CntZwPos == pDest->CntZwPos)
	&&(pScr->adj == pDest->adj)&&(pScr->pos == pDest->pos)
	&&(pScr->CntPos == pDest->CntPos))
	return TRUE;
  return FALSE;
}

/* FUNCTION: 	PlateTypeModeCheck
   PARAMETERS:	---
   DESCRIPTION: Plausibilitätskontrolle der Variable(n)
   RETURNS: 	TRUE / FALSE */
ubyte PlateTypeModeCheck(void){
  if((XYPlateTypeCmp(g_pudtActDeviceType,&g_udtDeviceType96_1536) == TRUE)
  || (XYPlateTypeCmp(g_pudtActDeviceType,&g_udtDeviceType96_384) == TRUE)
  || (XYPlateTypeCmp(g_pudtActDeviceType,&DeviceType384_1536) == TRUE))
	return TRUE;
  return FALSE;
}

/* FUNCTION: 	SetPlateTypeMode
   PARAMETERS:	Zeiger auf Kopftyp
   DESCRIPTION: Plattenauswahltabelle einstellen
   RETURNS: 	--- */
void SetPlateTypeMode(const struct HEAD_TYPE *ptr){
  if(IsHeadType1536()){
	g_pudtActDeviceType = &DeviceType1536_1536;
  }else{
	if(ptr->type == OL3714_6){
		if(g_bTestFlag)	{printf("SetDevTyp 384_1536_OL3714\r"); fflush(stdout);}
		g_pudtActDeviceType = &DeviceType384_1536_OL3714;
	}else{
		if(ptr->dev->PstCnt == 384){
			if(g_bTestFlag)	{printf("SetDevTyp 384_1536\r"); fflush(stdout);}
			g_pudtActDeviceType = &DeviceType384_1536;
		}else{
			if(g_bTestFlag)	{printf("SetDevTyp 96_384\r"); fflush(stdout);}
			g_pudtActDeviceType = &g_udtDeviceType96_384;
		}
	}
  }
}
