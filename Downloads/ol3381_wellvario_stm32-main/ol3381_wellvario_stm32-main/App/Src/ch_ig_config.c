//****************************************************************************
// @Module		CH-Igel
// @Filename	  CH-ig_config.c
// @Project
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	  Tasking C166
//
// @Description   Hauptmenu
//
//----------------------------------------------------------------------------
// @Date		  11.07.00 15:34:01
//
//****************************************************************************

//****************************************************************************
// @Project Includes
//****************************************************************************
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Inc
#include <_uart_terminal.h>

#include "ch_ig_config.h"

#include "lcd.h"
#include "ch_ig_err.h"
#include "ch_igel.h"
#include "ch_ig_key.h"
#include "ch_ig_utility.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_ig_piston.h"
#include "ch_ig_hrz.h"
#include "ch_ig_peri.h"
#include "ch_ig_head.h"
#include "ch_head_fram.h"
#include "ch_ig_spw.h"
#include "ch_ig_man.h"
#include "ch_ig_cmd.h"
#include "ch_ig_test.h"
#include "ch_ig_eeprom.h"
#include "ch_ig_uniadapter.h"
#include "Well_Defines.h"
#include "Well_Error.h"

extern bool HeadInterruptEnabled;
extern bit g_bBreakFlag;

extern DispString_t str_empty_rinse[];
extern DispString_t str_go_on[];

#define DISP_WAIT_TIME_MIN 0	/* in Sekunden */
#define DISP_WAIT_TIME_MAX 10   /* in Sekunden */
#define DISP_WAIT_TIME_DEF 1	 /* in Sekunden */

 /* nach längerem Stillstand haften die Kolben an den Dichtungen,deshalb mit geringerer Drehzahl starten
	Die Reduzierung wirkt nur nach dem Einschalten */
#define START_END_REF	100.0

 /** public data **/
float g_fDispWaitTime;	// Wartezeit beim manuellen Liquidhandling in Sekunden,Copie im EEPROM


	// Interne Programme
void SetMode384(void);
void SetMode1536(void);
void DeviceQueryAndSettingMode(void);
void HostModeCtrl(void);

DispString_t str_mag_not_insert[]	 = 	{{1,1,"Magazin nicht korrekt eingesetzt"}
										,{1,1,"Magazine not correctly inserted"}};
DispString_t str_yes[]				 = 	{{2,1,"Ja"}
										,{2,1,"Yes"}};
DispString_t str_no[]				 = 	{{2,4,"Nein"}
										,{2,5,"No"}};
DispString_t str_wait_time_colon[]	 = 	{{2,1,"Wartezeit:"}
										,{2,1,"Delay time:"}};
DispString_t str_sec_unit[]			 = 	{{2,13,"s"}		//Spaltenpos spielt keine Rolle
					  					,{2,13,"s"}};
DispString_t str_speaker_big[]		 = 	{{1,1,"KLICK"}
										,{1,1,"BEEPTONE"}};
DispString_t str_lang[]				 = 	{{1,1,"Language:"}
										,{1,1,"Language:"}};
DispString_t str_german[]			 = 	{{2,1,"Deutsch"}
										,{2,1,"Deutsch"}};
DispString_t str_english[]			 = 	{{2,9,"English"}
										,{2,9,"English"}};
DispString_t StrWells384[]			 = 	{{2,8,"384"}
										,{2,7,"384"}};
DispString_t StrWells1536[]			 = 	{{2,13,"1536"}
										,{2,12,"1536"}};
DispString_t StrPlateMode[]			 = 	{{1,1,"PLATTENTYPAUSWAHL"}
										,{1,1,"CHOICE OF PLATE TYPE"}};
DispString_t StrPlateWells[]		 = 	{{2,1,"Platte:"}
										,{2,1,"Plate:"}};
DispString_t str_dz_big[]			 = 	{{1,1,"HOEHENKORREKTUR"}
										,{1,1,"HEIGHT CORRECTION"}};
DispString_t str_dz_diff[]			 = 	{{2,1,"Korrekturhoehe:"}
										,{2,1,"Corr. height:"}};
DispString_t str_unit_height[]		 = 	{{2,1,"mm"}
										,{2,1,"mm"}};
DispString_t str_res_para[]			 = 	{{1,1,"Reset aller Parameter!"}
										,{1,1,"Reset all parameters!"}};
DispString_t str_drv_to_def[]		 = 	{{1,1,"Reset der Antriebsparameter!"}
										,{1,1,"Reset all driving parameters!"}};
DispString_t str_stand_by[]			 = 	{{1,14,"BEREITSCHAFT"}
										,{1,16,"STAND BY"}};
DispString_t str_space_1[]			 = 	{{2,25," "}
										,{2,25," "}};
DispString_t str_ins_rinse[]		 = 	{{2,1,"Wanne unter Spitzen bewegen"}
										,{2,1,"Position reservoir under tips"}};
DispString_t str_query_mode[]		 = 	{{1,1,"SERVICE MODE"}
										,{1,1,"SERVICE MODE"}};
DispString_t str_pump_rev_big[]		 = 	{{1,1,"DREHZAHL KOLBENANTRIEB"}
										,{1,1,"PIPETTING DRIVE SPEED"}};
DispString_t str_rev_unit[]			 = 	{{2,20,"Umdr/min"}
										,{2,22,"rpm"}};
DispString_t str_rev[]				 = 	{{2,1,"Drehzahl:"}
										,{2,1,"Revolutions:"}};
DispString_t str_wait_time_big[]	 = 	{{1,1,"WARTEZEIT"}
										,{1,1,"DELAY TIME"}};

DispString_t str_ok_abort[]	 		 = 	{{2,29,"Abbr:\004\005 OK:\002"}
									    ,{2,28,"Abort:\004\005 OK:\002"}};

DispString_t str_reset_ongoing[]	 = 	{{1,1,"Werden gel\357scht"}
										,{1,1,"Reset ongoing"}};

DispString_t str_back_app[]			 = 	{{1,1,"Zur\365ck zum Hauptprogramm"}
										,{1,1,"Back to main application"}};


static const struct Modus m_udtSpeakerOnOff[]		 = 	{{str_yes,ProgYes}
														,{str_no,ProgNo}};
static const struct Modus m_udtLanguageChoice[]		 = 	{{str_german,ProgGerman}
														,{str_english,ProgEnglish}};
static const struct Modus m_udtChoicePlate[]  		 = 	{{StrWells384,SetMode384}
														,{StrWells1536,SetMode1536}};
static const struct Modus m_udtChoicePlate1536[]	 = 	{{StrWells1536,SetMode1536}};

/* FUNCTION: 	ProgSetDispWait
   PARAMETERS:	---
   DESCRIPTION:	Wartezeit für den Restausstoß im manuellen Betrieb einstellen
   RETURNS: 	--- */
void ProgSetDispWait(void){
	ClearAllAndDisplay(str_wait_time_big);
	g_fDispWaitTime = ed_float(g_fDispWaitTime,DISP_WAIT_TIME_MIN,DISP_WAIT_TIME_MAX
								,str_wait_time_colon,str_sec_unit,0.1,1.0,"%4.1f",NO_LINE);
	 ErrorDisplay(EEPROMSetDispWaitTime());
}

/* FUNCTION: 	DispWaitTimeCheck
   PARAMETERS:	---
   DESCRIPTION:	überprüfung des manuell konfigurierbaren Parameter Wartezeit
   RETURNS: 	TRUE / FALSE */
ubyte DispWaitTimeCheck(void){
	if((g_fDispWaitTime > DISP_WAIT_TIME_MAX )||(g_fDispWaitTime < DISP_WAIT_TIME_MIN))
		return FALSE;
	return TRUE;
}

/* FUNCTION: 	ProgSetSpeaker
   PARAMETERS:	---
   DESCRIPTION:	Ein- bzw. Ausschalten des Piepsers
   RETURNS: 	--- */
void ProgSetSpeaker(void){
	ubyte id;
	ClearAllAndDisplay(str_speaker_big);
	id = ProgModification(m_udtSpeakerOnOff,sizeof(m_udtSpeakerOnOff)/sizeof(struct Modus),0,SECOND_LINE);
	m_udtSpeakerOnOff[id].fp();
	ErrorDisplay(EEPROMSetBeepState());
}

void ProgYes(void){
	g_bBeepState = TRUE;
}
void ProgNo(void){
	g_bBeepState = FALSE;
}

/* FUNCTION: 	ProgSetLanguage
   PARAMETERS:	---
   DESCRIPTION:	Sprachauswahl
   RETURNS: 	--- */
void ProgSetLanguage(void){
	ubyte id;
	//blnLangState = TRUE;
	ClearAllAndDisplay(str_lang);
	id = ProgModification(m_udtLanguageChoice,sizeof(m_udtLanguageChoice)/sizeof(struct Modus),0,SECOND_LINE);
	m_udtLanguageChoice[id].fp();
	ErrorDisplay(EEPROMSetLanguage());
}

/* FUNCTION: 	ProgGerman
   PARAMETERS:	---
   DESCRIPTION:	Sprache auf Deutsch setzen
   RETURNS: 	--- */
void ProgGerman(void){
	g_ubytLang = GERMAN;
}

/* FUNCTION: 	ProgEnglish
   PARAMETERS:	---
   DESCRIPTION:	Sprache auf Englisch setzen
   RETURNS: 	--- */
 void ProgEnglish(void){
	 g_ubytLang = ENGLISH;
 }

/* FUNCTION: 	LanguageCheck
   PARAMETERS:	---
   DESCRIPTION:	überprüfung der manuell konfigurierbaren Sprachauswahl
   RETURNS: 	TRUE / FALSE */
ubyte LanguageCheck(void){
	if((g_ubytLang == ENGLISH)||(g_ubytLang == GERMAN))
		return TRUE;
	return FALSE;
}

/* FUNCTION: 	ChoicePlateTypeMode
   PARAMETERS:	---
   DESCRIPTION:	Einstellen des XY-Modus auf 384er o. 1536er Platten beim 96er Kopf
   RETURNS: 	--- */
void ChoicePlateTypeMode(){
	unsigned char id,idx;
	ClearAllAndDisplay(StrPlateMode);
	Display(StrPlateWells);
	Display(str_go_on);
	if(g_pudtHeadType->dev->PstCnt != 384){
		if(g_pudtActDeviceType->adj == g_audtXYConfig384)
			id = 0;
		else
			id = 1;
		idx = ProgModification(m_udtChoicePlate,sizeof(m_udtChoicePlate)/sizeof(struct Modus),id,NO_LINE);
		m_udtChoicePlate[idx].fp();
		ErrorDisplay(EEPROMSetServoPlateTypeMode());
	}else{	/* Ab Version 4.00
		Hier kann keine Auswahl des XY-Modus erfolgen,da mit 384 Kolben immer eine 1536 Platte manuell befüllt werden kann.
		Diese Sektion wird nur zur Anzeige verwendet */
		idx = ProgModification(m_udtChoicePlate1536,sizeof(m_udtChoicePlate1536)/sizeof(struct Modus),0,NO_LINE);
	}
}

 void SetMode384(void){
	 g_pudtActDeviceType = &g_udtDeviceType96_384;
 }

 void SetMode1536(void){
	 g_pudtActDeviceType = &g_udtDeviceType96_1536;
 }

/* FUNCTION: 	ProgSetDz
   PARAMETERS:	---
   DESCRIPTION:	Einstellen der Höhenkorrektur zwecks Ausgleich der Tischtoleranzen
   RETURNS: 	--- */
void ProgSetDz(void){
	LcdClear(ALL_LINE);
	Display(str_dz_big);
	g_fVertHeightCorr = ed_float(g_fVertHeightCorr,DZ_MIN,DZ_MAX,str_dz_diff,str_unit_height,0.1,0.1,"%+3.1f",NO_LINE);
	EEPROMSetVertHeightCorr();
}

/* FUNCTION: 	DzCheck
   PARAMETERS:	---
   DESCRIPTION:	überprüfung des manuell konfigurierbaren Parameter
   RETURNS: 	TRUE / FALSE */
ubyte DzCheck(void){
	if((g_fVertHeightCorr > DZ_MAX )||(g_fVertHeightCorr < DZ_MIN))
		return FALSE;
	return TRUE;
}

/* FUNCTION: 	SytemInitialization
   PARAMETERS:	---
   DESCRIPTION:	Initialisierung und Kalibrierung aller Antriebe unter Beachtung
				der konstruktiven Gegebenheiten
   RETURNS: 	--- */
void SytemInitialization(void)
{
	bool blnSystemVarCheck,blnHrzVertCalFlag; ubyte key;
	ubyte state,VersNo;
	char temp_zk[20];

	g_bEnableDrive = FALSE;
	blnSystemVarCheck = TRUE; 	//Voreinstellung
	g_bShowSpots384 = FALSE; 	//keine Anzeige der Platznummern
	blnHrzVertCalFlag = FALSE;	// Antriebe noch nicht kalibriert
	DCMotorStop();	// Abschalten des Spitzenanzuges,der kann nach einem Softreset noch laufen
	DrvEnable();	// ab Version 3.10

	//Auswertung spezieller Tasten während des Einschaltvorganges
	key = KeysCheckForSetup();
	if(key == TRI_ARR_UP_KEYS) DeviceQueryAndSettingMode();
	VertDrvSetParaDefaults();
	// Versionierung
	ErrorDisplay(EEPROMReadStructVersion(&VersNo));
	if(VersNo < 3)
	{
		ErrorDisplay(EEPROMSetHrzMotPara(&m_udtHrzRevDef));
		ErrorDisplay(EEPROMSetStructVersionAndCmp());
	}
	if(VersNo < 2)
	{
		ErrorDisplay(EEPROMWriteVerticalSwitchPos(GetDefXYSwitchHeightOL3383()));	// Initialisierung der neuen Strukturkomponente
		ErrorDisplay(EEPROMSetStructVersionAndCmp()); 					// Strukturversion aktualisieren
	}

#ifndef DoNotReadEEPROM
	//Lesen der Setup-Werte aus dem EEPROM
	uint32_t temp1, temp2;
	EEPROMReadLifeParaPosition(&temp1);
	for(int i = 0; i < 3; i++)
	{
		EEPROMReadLifeParaPosition(&temp2);
		if(temp1 != temp2) //Read position must be the same 3 times in a row
			i = 0;			//There will be a Reset through Watchdog if it cant read eeprom succesfully for 8s
	}
	g_lifePos = temp1;
	if(g_lifePos + sizeof(g_udtLifePara) >= EEPROM_PHYSICAL_SIZE - sizeof(g_udtLifePara)) //Struct is completely saveable in EEPROM?
	{
		ErrorDisplay(IERR_EEPROM_MEMORY); //not enough space. If (64000byte/(5*4byte)*1000000 Cyclen erreicht)
	}
	ErrorDisplay(EEPROMReadLifePara(&g_udtLifePara)); 		//Read Life Parameter from EEPROM
	ErrorDisplay(EEPROMReadAndCheckLanguage());
	ErrorDisplay(EEPROMReadHeadSafety());
	ErrorDisplay(EEPROMReadShieldWatchFlag());
	ErrorDisplay(EEPROMReadDispWaitTime());
	ErrorDisplay(EEPROMReadVertHeightCorr());
	ErrorDisplay(EEPROMReadVertManWayLimit());
	ErrorDisplay(EEPROMReadPstManMotPara());
	ErrorDisplay(EEPROMReadVertManMotPara());
	ErrorDisplay(EEPROMReadBeepState());
	ErrorDisplay(EEPROMReadServoAdjustPara());
	ErrorDisplay(EEPROMReadHostModeFlag());
	ErrorDisplay(EEPROMReadCarrType());
	ErrorDisplay(EEPROMReadHrzMotPara(&m_udtHrzRevDef));		// Aufruf nach Fkt. EEPROMReadCarrType()
	ErrorDisplay(UNIServoInit());
	ErrorDisplay(EEPROMReadDeviceType());
	ErrorDisplay(EEPROMReadPMFlag());
	ErrorDisplay(EEPROMReadMagazineMode());
	ErrorDisplay(EEPROMReadAndSetVerticalSwitchPos());
	ErrorDisplay(EEPROMReadKeyboardLayout());
#endif


	//VertDrvSetPara();
	//Auswertung spezieller Tasten während des Einschaltvorganges
	//key = KeysCheckForSetup();
	//printf("Key:%d\r",(int)key);fflush(stdout);//Test
	//if(key == TRI_ARR_UP_KEYS) DeviceQueryAndSettingMode();
	if(key == ENTER_KEY) ProgSetLanguage();

	if(key == TRI_UP_KEY)		//Reset all device parameters
	{
		ClearAllAndDisplay(str_res_para);
		Display(str_ok_abort);
		key= WaitForAbortOrEnter();
		if(key == ENTER_KEY)
		{
			ClearAllAndDisplay(str_reset_ongoing);
			blnSystemVarCheck = FALSE;	//Reset aller Parameter
		}
		else
			ClearAllAndDisplay(str_back_app);
		Wait(150);
		LcdClear(ALL_LINE);
		key = NO_KEYS;
	}

	if(key == S_KEY)		//Reset all drive parameter
	{
		ClearAllAndDisplay(str_drv_to_def);
		Wait(100);
		VertSetRevDefToRevMan();
		//für Horizontalantrieb nicht erforderlich,da in HrzInitZk() immer die Defaultwerte benutzt werden
		PstSetRevParaToDefault(&g_udtPstRevDef);	//zu diesem Zeitpunkt ist der Kopftyp noch unbekannt
	}

	if(key == TRI_DOWN_KEY) HostModeCtrl();	// nur Flags setzen

	ShowInit();
	VertSetRevManToRevAct();
	if(SystemVarCheck() ==  FALSE) blnSystemVarCheck = FALSE;
	if(blnSystemVarCheck == FALSE)
	{
		SystemVarSetToDefault();
		ProgSetLanguage();
	}

	g_bXYPresent = TRUE;	// z.Z. gibt es keine andere Ausführung
	//printf("Init ZK Z-Drv\r");fflush(stdout);
	ShowInit();
	ErrorDisplay(VertInitZkMan());
	ErrorDisplay(HrzInitZk());	// es werden immer die Defaultwerte benutzt

	if(key == ARR_RIGHT_KEY)
	{
		// DrvEnable();  ab Version 3.10
		if(g_pudtHrzPosTabPtr->cartype == DISK_10)
			AdjDiskZero();
		else
			AdjStackPos();
	}

   /* Bedienereingabe nach Maschinenrichtlinie erforderlich */
	if(!g_bHostMode) WarningDisplay(str_stand_by,str_space_1);
	if(!g_bHostMode) UNIServoAngleDrvToZero();
	// Kopf initialisieren
	//printf("Kopfwechsel?\r");fflush(stdout);//Test
	ErrorDisplay(PstInitZkMan());
	//Kopfnummer aus EEPROM lesen zwecks späteren Vergleich zum aktuellen Kopf
	ErrorDisplay(EEPROMReadHeadTypeNumber(temp_zk));

	// DrvEnable(); ab Version 3.10
	if(HeadConnectionPin == 1)	// kein Kopf eingesetzt,bei OL3714 fest verdrahtet --> Kopf immer eingesetzt
	{
		HeadInsert();	// Kopfparamenter werden hier eingelesen und Antriebe initialisiert
		blnHrzVertCalFlag = TRUE;
	}
	else
	{
		ErrorDisplay(ReadHeadNoAndSetDevPara());
		ErrorDisplay(EEPROMSetHeadTypeNumber(g_pudtHeadType->type));	// ist notwendig,da Kopf auch im ausgeschalteten Zustand gewechselt werden kann!
		if(strcmp(temp_zk,g_pudtHeadType->type))
		{
			ErrorDisplay(PstSetDrvParaToDefault());
		}
	}
	if(key == S_KEY)
		ErrorDisplay(PstSetDrvParaToDefault());	// vorher war der Kopftyp unbekannt

//	CC8IR = 0; 				//Reset eines evtl. anhängigen Interrupts vom Kopfwechsel (Prellen bei Kontaktgabe möglich)


	if(g_uiDeviceType == 3381)
		HeadInterruptEnabled = ENABLE;

	if(g_uiDeviceType != 3714)// Kopf für LHA --> kein Wechselkopf,keine Magneterkennung
	{
		/* eventuell ist noch der Magazinadapter eingesetzt bzw. beim maschinellen Kopfwechsel ist der Kopf noch nicht komplett in der unteren Endlage */
		if(!IsHeadLowerPos()|| IsChangeToolInsert())
		{
			if(g_bHostMode) WarningDisplay(str_stand_by,str_space_1);	//kein Bewegungsvorgang ohne Bestätigung
			if(g_uiDeviceType == 3381)
			{
				RemoveHeadChangeTool();
			}
			else
			{
				HeadInsertAndDrvDown();
				blnHrzVertCalFlag = TRUE;
			}
			ShowInit();
		}
	}
	// ErrorDisplay(ReadHeadNoAndSetDevPara());	//OL3381

/* Die Kolbenzyklen sowie das Kolbenvolumen werden beim Kopfwechsel gelesen bzw. in den Kopf geschrieben.
   Bedingt durch den manuellen Kopfwechsel und den daraus resultierenten Änderungen müssen diese Parameter
   auch hier gelesen werden,sie werden ab der Version 3.0 zyklisch im Kopf gespeichert. Das Handeln der Kopfwechsel
   am Gerät sowie die Gerätewechsel am Kopf werden weiterhin beim Kopfwechsel aktualisiert. */
	ErrorDisplay(FRAMCheckAndSetStruktureVersionP1());
	ErrorDisplay(FRAMReadPstCycles());
	ErrorDisplay(FRAMReadPstVolume());
	ErrorDisplay(FRAMReadAddVolume(&g_AddVolume));

	if(!g_bHostMode)
	{
		if(!IsMagazineTighten())
		{
			//g_ubytFlgTCMAXWay = TRUE;	// Zustand der Kolben undefiniert,daher Kolben ganz nach oben fahren
			ProgManTipsChange();
		}
		else
		{
			state = ReadMagazineTypeAndSetTipsPara(g_pudtHeadType);
			if(state == IERR_MAG_NOT_INSERT)
			{
				WarningDisplay(str_mag_not_insert,str_space_1);
				g_ubytFlgTCMAXWay = TRUE;
				ProgManTipsChange();
			}
			else
				ErrorDisplay(state);
		}
	}
	else
	{
		SetTipsParaToZero(); 					//Nullsetzen der akt. Spitzenparameter
		state = EEPROMReadTipsPara(&g_udtTipsParaFromASPW /*g_pudtTipsPara*/ );	// Spitzenparameter evtl. überschreiben,falls vorhanden
		g_pudtTipsPara = &g_udtTipsParaFromASPW;
		if(state == IERR_NO_TIPS_PARA)
		{ 		//keine Spitzenparameter vorhanden
			if(IsMagazineTighten())
				state = ReadMagazineTypeAndSetTipsPara(g_pudtHeadType);
		}
	}
	if(g_bTestFlag)
	{
		printf("TipsPara:\r %f %f %f %f %f %f\r",
				g_pudtTipsPara->asc,g_pudtTipsPara->zero,g_pudtTipsPara->MaxVol,
				g_pudtTipsPara->MinVol,g_pudtTipsPara->MoreDisp,g_pudtTipsPara->MoreSuck);
		fflush(stdout);
	}
	if(g_pudtHeadType->type == OL3387_8)
	{	//Hummingwell-Kopf
		ErrorDisplay(ReadAndSetCartridgeTyp());	// Auswahl 96 bzw. 384
		SetPlateTypeMode(g_pudtHeadType);
		g_udtLifePara.PstCycleVolume = 0;	//hat bei diesem Kopf keine Bedeutung,anderenfalls kommt immer die Aufforderung das Restvolumen auszustoßen
		ErrorDisplay(EEPROMWriteLifePara(&g_udtLifePara));
	}

	// Antriebe initialisieren
	g_bEnableDrive = TRUE;
	XYServoInit(0);	//gleich welcher Kopf zum Einsatz kommt,da 0-Position für alle gleich ist
	if(!g_bHostMode)
		if(blnHrzVertCalFlag == FALSE) HrzVertDrvCalibrate();

	if(blnSystemVarCheck == FALSE) SystemConfiguration();

	/* Hummingwell-Kopf */
	if(strncmp(g_pudtHeadType->type,OL3387_8,8) == 0)	/* Hummingwell-Kopf */
	{
		g_ubytDrvInitState &= 0xf7;	// Kolben,da nicht vorhanden --> immer kalibriert
	}
	else
	{
		if(!g_bHostMode)
			UserRequestAndPstCalibrate();
	}
	if(key == ARR_UP_KEY) PrgSelfTest();
}

void UserRequestAndPstCalibrate(void)
{
	float save;
	save = PstGetActEndReduce();  ;	  /* ab Version 3.23 */
	PstSpeedLimit(START_END_REF);
	if(g_udtLifePara.PstCycleVolume != 0)
		UserRequestRinse();
	else
		ErrorDisplay(PstDrvCalibrate());
	PstSpeedLimit(save);
}

void UserRequestRinse(void)
{
	WarningDisplay(str_empty_rinse,str_ins_rinse);
	ErrorDisplay(PstDrvCalibrate()); dispense_end_cycle();
}

/* FUNCTION: 	SystemConfiguration
   PARAMETERS:	---
   DESCRIPTION:	Neusetzen der Variablen,die durch die Batterie gestützt werden
   RETURNS: 	--- */
void SystemConfiguration(void)
{
	//printf("sytem configuration\r");fflush(stdout);//Test
	ProgSetDispWait(); VertConfigWayLimit(); ProgSetDz();	// XY-Mode einstellen
	ChoicePlateTypeMode();
	XYServoInit(0); XYProgConfig();
	//ConfigPumpManEndRev();
}

/* FUNCTION: 	SystemVarCheck
   PARAMETERS:	---
   DESCRIPTION:	Prüfen der Integrität der gespeicherten Variablen
   RETURNS: 	TRUE / FALSE */
ubyte SystemVarCheck(void)
{
	if(LanguageCheck()&& DzCheck()&& DispWaitTimeCheck()&& XYCheckAdjPara()
			/* && PlateTypeModeCheck()*/
			&& VertDrvManParaCheck()&&(PstCheckRevManPara()))
		return TRUE;
	return FALSE;
}

void SystemVarSetToDefault(void)
{
	g_ubytLang = ENGLISH;
	g_bBeepState = 1;
	g_bHostMode = FALSE;
	g_fVertHeightCorr = DZ_DEF;
	g_fVertManLimitWay = g_fVertMaxWay;
	g_fDispWaitTime = DISP_WAIT_TIME_DEF;
	XYSetAdjParaToDefault();	//PlateTypeModeSetToDefault();
	//VertSetRevParaToDefault();
	VertSetRevDefToRevMan();
	PstSetRevParaToDefault(&g_udtPstRevDef);	//zu diesem Zeitpunkt ist der Kopftyp noch unbekannt
}

void DeviceQueryAndSettingMode(void)
{
	Asc0DtrCtrl(ON); g_bAllowHostMode = TRUE;
	ClearAllAndDisplay(str_query_mode);
	while(1)
	{
		WDO();
		if(IsCmdComplete()||g_bBreakFlag)
		{
			CmdCopy();
			CmdExec();
		}
		if(IsResetRequest())
			PerformSoftwareReset();
	}
}

/* FUNCTION: 	HrzVertDrvCalibrate
   PARAMETERS:	---
   DESCRIPTION:	Kalibrierung des Vertikal- und des Horizontalantriebes
   RETURNS: 	--- */
void HrzVertDrvCalibrate()
{
	if(VertCheckZeroPos())
	{
		ErrorDisplay(CarDrvCalibrate());
		ErrorDisplay(VertDrvCalibrate());
	}
	else
	{
		ErrorDisplay(VertDrvCalibrate());
		ErrorDisplay(CarDrvCalibrate());
	}
}

/* FUNCTION: 	HostModeCtrl
   PARAMETERS:	---
   DESCRIPTION:	Steuerung des Gerätearbeitsmodus
   RETURNS: 	--- */
void HostModeCtrl(void){
	//ändern des momentanen Zustandes (Manueller Betrieb - Hostmode) und speichern im EEPROM
	if(g_bHostMode == TRUE)
		g_bHostMode = FALSE;
	else
		g_bHostMode = TRUE;
	ErrorDisplay(EEPROMSetHostModeFlag());
}

/* FUNCTION: 	ConfigPumpManEndRev
   PARAMETERS:	---
   DESCRIPTION:	Einstellung der manuellen Pumpenddrehzahl
   RETURNS: 	--- */
void ConfigPumpManEndRev(void)
{
	ClearAllAndDisplay(str_pump_rev_big);
	/* g_udtPstRevMan.EndReduce = ed_float(g_udtPstRevMan.EndReduce,g_udtPstRevMan.Start,g_udtPstRevMan.End,str_rev,
  						  str_rev_unit,1.0,10.0,"%3.0f",SECOND_LINE);
	 ErrorDisplay(PstSpeedLimit(g_udtPstRevMan.EndReduce)); */

	// ab Version 2.20: damit auch manuell bis 500 Umdr. / min erreichbar sind
	g_udtPstRevMan.End = ed_float(g_udtPstRevMan.EndReduce,g_udtPstRevMan.Start,(float)PST_RPM_MAX,str_rev,
  						  str_rev_unit,1.0,10.0,"%3.0f",SECOND_LINE);
	g_udtPstRevMan.EndReduce = g_udtPstRevMan.End;
	ErrorDisplay(PstCalcDrvPara(&g_udtPstRevMan));
	//ErrorDisplay(PstSpeedLimit(g_udtPstRevMan.EndReduce));
	ErrorDisplay(EEPROMSetPstManMotPara());	// evtl. Rücksetzen durch Drücken der Start/Stopp-Taste beim Einschalten
}

/* FUNCTION: 	PstRevSetToDef
   PARAMETERS:	---
   DESCRIPTION:	Einstellen der Standardwerte des Kolbenantriebes
   RETURNS: 	--- */
void PstSetRevParaToDefault(const struct REV_PARA *para)
{
	CopyRevPara(&g_udtPstRevMan,para);
	ErrorDisplay(EEPROMSetPstManMotPara());
}

/* FUNCTION: 	EEPROMReadDeviceType
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Gerätenummer und wandeln in Integer
   RETURNS: 	CB_READY,IERR_EEPROM_READ */
ubyte EEPROMReadDeviceType(void)
{
	ubyte state;
	char tmpDevType[14];
	state = EEPROMReadDevNumber(tmpDevType);
	if(state != CB_READY)
		return state;
	g_uiDeviceType = atoi(&tmpDevType[2]);
	return CB_READY;
}
