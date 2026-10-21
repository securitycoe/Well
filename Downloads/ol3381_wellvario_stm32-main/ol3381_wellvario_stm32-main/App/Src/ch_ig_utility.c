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
#include <ctype.h>

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions
#include <_tmc5130a.h>
#include "ch_ig_utility.h"
#include "ch_head_fram.h"						//
#include "ch_ig_cmd.h"
#include "ch_ig_eeprom.h"
#include "ch_ig_err.h"
#include "ch_ig_hrz.h"
#include "ch_ig_key.h"
#include "ch_ig_piston.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"
#include "Well_Error.h"

/** local definitions **/
#define RPM_TO_SPEPS SpU * uSpS
#define MAN_SPEED_Z_REDUCED 40

/** default settings **/

/** external data **/
extern DispString_t str_xy_go_esc[];

extern volatile int16_t VertEncoder;

extern uword g_uwWaitTime;
extern uword g_uwBeepTime;

/** internal functions **/
static int16_t ConvertMonth(const char* const sptr);
static void ConvertDate(const char *const date, char* const dest_date);
/** public data **/

ubyte g_ubytSKeyFlag;

DispString_t str_go_on[]			=	{{2,33,"weiter:\002"}
										,{2,34,"cont.:\002"}};
DispString_t str_big_statistic[]	=	{{1,1,"STATISTIK"}
										,{1,1,"STATISTICS"}};

const char *str_cycles_xy[]			= {"Zyklen des XY-Tisches:","XY stage cycles:"} ;
const char *str_cycles_piston[]		= {"Kolbenzyklen:","Piston cycles:"};
const char *str_total_volume[]		= {"Gesamtvolumen [ml]:","Total volume [ml]:"};
const char *str_head_changing[]		= {"Kopfwechsel am Ger\204t:","Head changes at device:"};
const char *str_device_changing[]	= {"Ger\204tewechsel am Kopf:","Device changes at head:"};
const char *str_version[]			= {"Vers:","Vers:"};
const char *str_timecode[]			= {"Erstellt:","Build:"};
const char *str_device_no[]			= {"Ger\204tenr.:","Device no:"};
const char *str_head_no[]			= {"Kopfnr.:","Head no:"};
const char *str_unknown[]			= {"unbekannt","unknown"};
const char *str_piston_no[]			= {"Kanalanzahl:","channels:"};
const char *str_hwrev[]				= {"HW Rev:","HW Rev:"};

pFct LifeFkt[] =
	{ShowDeviceNo
	,ShowHeadNo
	,ShowXYCycles
	,ShowPistonCycles
	,ShowTotalVolume
	,ShowHeadChangingAtDevice
	,ShowDeviceChangingAtHead
	,ShowPistonNumber
	,ShowVersionNr
	,ShowTimeCode
	,ShowHardwareRevision
	};

/** private data **/

/** private functions **/


 int ed_int(int ist,int min,int max,DispString_t *str,DispString_t *unit,int inc,char *frt, unsigned char cls,unsigned char point_pos)
 {
	 char help[10],col,key;
	 int old,act_inc;
	 DispString_t *h_str;
	 unsigned char tast_cnt,tast_rep_limit;

	 h_str = str+g_ubytLang;
	 unit += g_ubytLang;
	 old = ist;
	 LcdClear(cls);
	 g_ubytSKeyFlag = FALSE;
	 act_inc = 1;
	 tast_cnt = 0;
	 tast_rep_limit = 10;
	 if(ist > max) ist = max;
	 if(ist < min) ist = min;
	 Display(str_xy_go_esc); Display(str);
	 //SetCursorOff(); Test
	 col = strlen(h_str->message)+h_str->column+atoi(frt+1)+((point_pos == 0)? 0:1);
	 LcdShow(unit->line,col+1,unit->message);
	 while(1)
	 {
		 sprintf(help,frt,ist);// PointDisp(help,point_pos);
		 LcdShow(str->line,col-1,help);WDO();
		 key = KeyWaitForAny();
		 if(g_ubytKeyRepeateFlag)
		 {
			 Wait(4);
			 tast_cnt++;
		 }
		 else
		 {
			 tast_cnt = 0;
			 act_inc = 1;
		 }
		 if(tast_cnt > inc)
		 {
			 act_inc = inc;
			 tast_cnt = tast_rep_limit;
		 }
		 switch (key)
		 {
			 case ARR_UP_KEY:
				 if((ist += act_inc) > max) ist = min;
				 break;
			 case ARR_DOWN_KEY:
				 if((ist -= act_inc) < min) ist = max;
				 break;
			 case ENTER_KEY:
				 return ist;
			 case S_KEY:
				 g_ubytSKeyFlag = TRUE;
				 return old;
		 }
	 }
 }

float ed_float(float ist,float min,float max,DispString_t *str,DispString_t *unit, float start_inc,float end_inc,const char *frt,unsigned char cls)
{
	char help[40],col,key;
	float act_inc,old;
	DispString_t *h_str;
	char tast_cnt,tast_rep_limit,tast_rep_limit_2;

	h_str = str+g_ubytLang;
	unit += g_ubytLang;
	old = ist;
	LcdClear(cls);
	g_ubytSKeyFlag = FALSE;
	act_inc = start_inc;
	tast_cnt = 0;
	tast_rep_limit = 10;
	tast_rep_limit_2 = 30;
	if(ist > max) ist = max;
	if(ist < min) ist = min;
	Display(str_xy_go_esc); Display(str);
	col = strlen(h_str->message)+h_str->column+1;
	while(1)
	{
		// printf("ist:%f,%3.1f,min:%f,max:%f,\rstart_inc:%f,end_inc:%f\r",
		// (double)ist,(double)ist,(double)min,(double)max,(double)start_inc,(double)end_inc); fflush(stdout);
		//sprintf(help,strFrmt,(double)ist);
		FloatFormatter(ist,frt,help);
		strcat(help,unit->message);
		LcdShow(str->line,col,help);WDO();
		key = KeyWaitForAny();
		if(g_ubytKeyRepeateFlag)
		{
			Wait(3);
			tast_cnt++;
		}
		else
		{
			tast_cnt = 0;
			act_inc = start_inc;
		}
		if(tast_cnt == tast_rep_limit)
		{
			act_inc = end_inc;
			/* tast_cnt = tast_rep_limit;*/
		}
		if(tast_cnt == tast_rep_limit_2)
			act_inc += end_inc;
		if(tast_cnt > tast_rep_limit_2)
			tast_cnt = tast_rep_limit_2;
		switch (key)
		{
			case ARR_UP_KEY:
				if((ist += act_inc) > max) ist = min;
				break;
			case ARR_DOWN_KEY:
				if((ist -= act_inc) < min) ist = max;
				break;
			case ENTER_KEY:
				return ist;
			case S_KEY:
				g_ubytSKeyFlag = TRUE;
				return old;
		}
	}
}

/* FUNCTION: 	FloatFormatter
   PARAMETERS:	Zahl,Zeiger auf Formatierungsstring,Zeiger auf Speicher für Zahl
   DESCRIPTION:	Erweitert das Format um eine Stelle wegen eines undef. Rundungsfehler
				Zusatzstelle wird anschließend wieder abgeschnitten
				Achtung: kein voller Funktionsumfang für alle Formate!!!
   RETURNS: 	--- */
void FloatFormatter(float x,const char *frmt,char *dst)
{
	char *cPtr,strHelp[10],strFrmt[20];
	unsigned int i;
	float HelpFrmt;

	//Formatierung modifizieren
	strcpy(strFrmt,frmt);
	cPtr = strstr(frmt,".0");
	if(cPtr == NULL)
	{
		HelpFrmt = atof(frmt+1);
		HelpFrmt+= 1.1;
		//Suchen der ersten Zifferposition
		for(i = 0; i<strlen(strFrmt); i++)
		{
			if(isdigit((unsigned char)strFrmt[i])!= 0) break;
		}
		sprintf(strHelp,"%3.1ff",HelpFrmt);
		strcpy(&strFrmt[i],strHelp);
		sprintf(dst,strFrmt,(double)x);
		cPtr = dst+strlen(dst)-1;	//letzte Stelle wird ausgeblendet,da Format erweitert wurde
		*cPtr = 0;
	}
	else
		sprintf(dst,strFrmt,(double)x);
}

static ubyte ubytOldKey = NO_KEYS;

ubyte KeyMoveCtrl(ubyte new_key)
{
	ubyte state;
	WDO();
	switch (new_key)
	{
		case BOTH_KEYS:
							VertDrvBreak();
							state = VertDrvWayXYCtrl(0);
							if(state == IERR_PROG_ABORT)
								state = VertManAbortHandling();
							else
							{
								if((state != CB_READY)&&(state != IERR_PROG_ABORT)) ErrorDisplay(state);
							}
							ShowLocation(FALSE); ubytOldKey = NO_KEYS; return 0;
		case TRI_UP_KEY:
							g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
							if(g_fVertDrvCurrentPos >= 	g_fVertManLimitWay) return 0;
							if(g_fVertDrvCurrentPos < g_fVertXYSwitchPos-0.5)
							{
								//Velocity to High and acceleration to high
								float speed = VertGetVelocity();
								tmc5130a_VELOCITY(1, RPM_TO_SPEPS*MAN_SPEED_Z_REDUCED/60); //40 RPM
								tmc5130a_ACCELERATION_MAN(1, FAST_ACCELERATION, FAST_ACCELERATION); //linear beschleunigen
								state = VertDrvWayXYCtrl(g_fVertXYSwitchPos);
								tmc5130a_VELOCITY(1, RPM_TO_SPEPS*speed/60); //140 RPM
								//langsam beschleunigen, Bremsen quasi sofort
								//0,6 und 0,3 wurde aus alter Leiterplatte ermittelt
								tmc5130a_ACCELERATION_MAN(1, tmc5130a_SPEED_ACCELERATION_CURVE(0.6, 0.3, speed/60)*RPM_TO_SPEPS, FAST_ACCELERATION);
								if((state != CB_READY)&&(state != IERR_PROG_ABORT)) ErrorDisplay(state);
								ShowLocation(FALSE); ubytOldKey = NO_KEYS;
								return 0;
							}
							if(ubytOldKey == TRI_UP_KEY)
							{	//weiterfahren
									if(g_uwWaitTime == 0)
									{
									}
									return 0;
							}
							if(ubytOldKey == NO_KEYS)
							{
								g_uwWaitTime = 10;					// 100ms warten,da der Mensch träge ist
								while(g_uwWaitTime != 0) WDO();
								VertDrvCtrlStart(g_fVertManLimitWay);
								g_uwWaitTime = 20; 					// 200ms
								ubytOldKey = TRI_UP_KEY;
								return 0;
							}
							return 0;	// erstmal zum Stillstand kommen
		case TRI_DOWN_KEY:
							g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse;	// Lesen der Istposition
							if(g_fVertDrvCurrentPos <= g_fVertDrvPrecision) return 0;
							if(g_fVertDrvCurrentPos <= g_fVertXYSwitchPos+0.5)
							{
								VertDrvBreak();//	erst zum Stillstand kommen,dann neu starten
								float speed = VertGetVelocity();
								tmc5130a_VELOCITY(1, RPM_TO_SPEPS*MAN_SPEED_Z_REDUCED/60); //40 RPM
								tmc5130a_ACCELERATION_MAN(1, FAST_ACCELERATION, FAST_ACCELERATION); //linear beschleunigen
								state = VertDrvWayXYCtrl(0);
								tmc5130a_VELOCITY(1, RPM_TO_SPEPS*speed/60); //140 RPM
								//langsam beschleunigen, Bremsen quasi sofort
								//0,6 und 0,3 wurde aus alter Leiterplatte ermittelt
								tmc5130a_ACCELERATION_MAN(1, tmc5130a_SPEED_ACCELERATION_CURVE(0.6, 0.3, speed/60)*RPM_TO_SPEPS, FAST_ACCELERATION);
								if((state != CB_READY)&&(state != IERR_PROG_ABORT)) ErrorDisplay(state);
								ShowLocation(FALSE);
								return 0;
							}
							if(ubytOldKey == TRI_DOWN_KEY)
							{ //weiterfahren
								// beschleunigen falls möglich
								if(g_uwWaitTime == 0)
								{
								}
								return 0;
							}
							if(ubytOldKey == NO_KEYS)
							{
								g_uwWaitTime = 10;				// 100ms warten,da der Mensch träge ist
								while(g_uwWaitTime != 0) WDO();
								VertDrvCtrlStart(g_fVertXYSwitchPos);
								g_uwWaitTime = 20; 				// 200ms
								ubytOldKey = TRI_DOWN_KEY;
								return 0;
							}
							return 0;
		case TRI_LEFT_KEY:
							g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse; //new dha
							if(g_fVertDrvCurrentPos <= g_fVertDrvPrecision)
							{	// Genauigkeit mit der der Nullpunkt angefahren wird
								if(g_ubytKeyboardLayout == KEYBOARD_LAYOUT_DEFAULT)
									CarNextPos(+1);
								else
									CarNextPos(-1);
							}
							else XYNextManPos(1); //Tisch ausgefahren? -> dann servos
							return 0;
		case TRI_RIGHT_KEY:
							g_fVertDrvCurrentPos = (int)VertEncoder * g_fVertDrvMeasureToEncoderImpulse; //new dha
							if(g_fVertDrvCurrentPos <= g_fVertDrvPrecision)
							{				//Tisch ausgefahren? -> dann servos
								if(g_ubytKeyboardLayout == KEYBOARD_LAYOUT_DEFAULT)
									CarNextPos(-1);
								else
									CarNextPos(+1);
							}
							else
								XYNextManPos(-1);
							return 0;
		case NO_KEYS:
						if(ubytOldKey != NO_KEYS)
						{
							ubytOldKey = NO_KEYS;
							VertDrvBreak();
						}
						return 0;
	}
	return new_key;
}

void WarnBeep(void)
{
	ubyte i;
	for(i = 0;i<5;i++)
	{
		BeepCtrl(5);
		Wait(5);
	}
}

void DisplayAndEnter(DispString_t *ptr)
{			//disp_enter
	Display(ptr);
	KeyWaitForEnter();
	ClearStringOnDisplay(ptr);
}

void WarningDisplay(DispString_t *ptr1,DispString_t *ptr2)
{
	struct LCD_DISP dHelp;
	SaveDisplay(&dHelp);
	ClearAllAndDisplay(ptr1); Display(ptr2); SetCursorOff();
	Display(str_go_on); WarnBeep();
	KeyWaitForEnter();Wait(50);
	RestoreDisplay(&dHelp);
}

/* FUNCTION: 	ProgStatisticsMode
   PARAMETERS:	---
   DESCRIPTION:	Anzeige verschiedener Lifetimeparameter
   RETURNS: 	--- */
void ProgStatisticsMode(void
		){
	pFct *udtPtr;
	char help[LCD_DIGITS];
	PermitHostMode(FALSE);
	ClearAllAndDisplay(str_big_statistic);
	udtPtr = LifeFkt;
	while(1)
	{
		LcdClear(SECOND_LINE);
		(*udtPtr)(help);
		LcdShow(2,1,help); Display(str_go_on);
		switch (KeyWaitForAny())
		{
			case ARR_DOWN_KEY:
								if(udtPtr == LifeFkt+sizeof(LifeFkt)/sizeof(pFct)-1)
									udtPtr = LifeFkt;
								else
									udtPtr++;
								break;
		  case ARR_UP_KEY:
								if(udtPtr == LifeFkt)
									udtPtr = LifeFkt+sizeof(LifeFkt)/sizeof(pFct)-1;
								else
									udtPtr--;
								break;
		  case ENTER_KEY:
							  if(udtPtr == LifeFkt+sizeof(LifeFkt)/sizeof(pFct)-1)
							  {
								  PermitHostMode(TRUE);
								  return;
							  }else
								  udtPtr++;
							  break;
		  case S_KEY:
							  PermitHostMode(TRUE);
							  return;
		}
	}
}

/* FUNCTION: 	ShowPistonNumber
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Kolbenanzahl des Kopfes
   RETURNS: 	--- */
void ShowPistonNumber(char *str)
{
	sprintf(str,"%s %u",str_piston_no[g_ubytLang],g_pudtHeadDevPara->PstCnt);
}

/* FUNCTION: 	ShowXYCycles
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Tischbewegungen des Gerätes über die Lebensdauer
   RETURNS: 	--- */
void ShowXYCycles(char *str)
{
	sprintf(str,"%s %ld",str_cycles_xy[g_ubytLang],g_udtLifePara.ulngXYCycles);
}

/* FUNCTION: 	ShowPistonCycles
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Kolbenzyklen des Kopfes über die Lebensdauer
   RETURNS: 	--- */
void ShowPistonCycles(char *str)
{
	sprintf(str,"%s %ld",str_cycles_piston[g_ubytLang],g_udtLifePara.ulngPistonCycles);
}

/* FUNCTION: 	ShowHeadChangingAtDevice
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Kopfwechselwechselanzahl am Gerät
   RETURNS: 	--- */
void ShowHeadChangingAtDevice(char *str)
{
	ulong help;
	ErrorDisplay(EEPROMReadHeadExchangeAtDev(&help));
	sprintf(str,"%s %ld",str_head_changing[g_ubytLang],help);
}

/* FUNCTION: 	ShowDeviceChangingAtHead
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Gerätewechselanzahl am Kopf
   RETURNS: 	--- */
void ShowDeviceChangingAtHead(char *str)
{
	ulong ulngDevExAtHead;
	ErrorDisplay(FRAMReadDevExchangeAtHead(&ulngDevExAtHead));
	sprintf(str,"%s %ld",str_device_changing[g_ubytLang],ulngDevExAtHead);
}

/* FUNCTION: 	ShowTotalVolume
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung des bisher verarbeiteten Gesamtvolumens
   RETURNS: 	--- */
void ShowTotalVolume(char *str)
{
	sprintf(str,"%s %6.2f",str_total_volume[g_ubytLang],(double)g_udtLifePara.PstTotalVolume);
}

/* FUNCTION: 	ShowVersionNr
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Versionsnummer
   RETURNS: 	--- */
void ShowVersionNr(char *str)
{
	sprintf(str,"%s%02u.%02u.%02u.%02u",str_version[g_ubytLang],gVersion_Major, gVersion_Minor, gVersion_BugFix,gVersion_Testbuild);
}

/* FUNCTION: 	ShowTimeCode
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung des Firmware build time codes
   RETURNS: 	--- */
void ShowTimeCode(char *str)
{
	char date_string[12];

	ConvertDate(VersionDate, date_string);
	sprintf(str,"%s%sT%s",str_timecode[g_ubytLang],date_string,VersionTime);
}



/* FUNCTION: 	ShowVersionNr
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Hardware Revision
   RETURNS: 	--- */
void ShowHardwareRevision(char *str)
{
	sprintf(str,"%s %s",str_hwrev[g_ubytLang], HW_Revision);
}

/* FUNCTION: 	ShowDeviceNo
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Gerätenummer
   RETURNS: 	--- */
void ShowDeviceNo(char *str)
{
	ubyte state; struct EEPROM * ptr;
	char *cPtr = (char*)malloc(sizeof(ptr->DeviceNr));
	if((state = EEPROMReadDevNumber(cPtr)) == CB_READY)
		sprintf(str,"%s %s",str_device_no[g_ubytLang],cPtr );
	else
		sprintf(str,"%s %s",str_device_no[g_ubytLang],str_unknown[g_ubytLang]);
	free(cPtr);
}

/* FUNCTION: 	ShowHeadNo
   PARAMETERS:	Pointer auf ZK
   DESCRIPTION:	Bereitstellung der Kopfnummer
   RETURNS: 	--- */
void ShowHeadNo(char *str)
{
	ubyte state; struct FRAM1 * ptr;
	char *cPtr = (char*)malloc(sizeof(ptr->HeadNo));
	if((state = FRAMReadHeadNumber(cPtr)) == CB_READY)
		sprintf(str,"%s %s",str_head_no[g_ubytLang],cPtr );
	else
		sprintf(str,"%s %s",str_head_no[g_ubytLang],str_unknown[g_ubytLang]);
	free(cPtr);
}

void BeepCtrl(uword time)
{
	if(!g_bBeepState) return;	// Töne sind unerwünscht
	BeepOn();
	g_uwBeepTime = time;
	while(g_uwBeepTime) WDO();
	BeepOff();
}

/// \brief	Konvertierung Monatstring in Short
/// \param	Pointer auf Monatsstring
/// \return	short
static int16_t ConvertMonth(const char* const sptr)
{
	int16_t i;
	const char* const MonthTab[]= {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	for(i=0; i< 12; i++)
	{
		if(strncmp(sptr, MonthTab[i], 3) == 0)
			break;
	}
	return i+1;
}

/// \brief	Vorbereitung und Kommunikation mit Slave
/// \param	date 	- Pointer auf zu konvertierendes Datum (z.B. "Sep 29 2015")
/// \param	dest_date - Pointer auf ZK-Buffer des konvertierten Datums (z.B. "2015-09-29")
static void ConvertDate(const char *const date, char* const dest_date)
{
	int16_t year, month, day;
	char *cptr;

	month= ConvertMonth(date);
	cptr= strchr(date, ' '); cptr++;
	day= atoi(cptr); cptr++;
	cptr= strchr(cptr, ' '); cptr++;
	year= atoi(cptr);
	sprintf(dest_date, "%04d-%02d-%02d", year, month, day);
}

