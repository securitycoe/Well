//****************************************************************************
// @Module		CH-Igel
// @Filename	CH-Igel.C
// @Project		CH-ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	Tasking C166
//
// @Description	Hauptmenu
//
//----------------------------------------------------------------------------
// @Date		  11.07.00 15:34:01
//
//****************************************************************************


//****************************************************************************
// @Project Includes
//****************************************************************************
#include "stdbool.h"

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- BASE
#include "ch_ig_cmd.h"
#include "ch_head_fram.h"
// ---------- Inc:
#include <_i2c_fram.h>
#include <_uart_terminal.h>
// ---------- Master
#include "ch_ig_config.h"
#include "ch_ig_eeprom.h"
#include "ch_ig_head.h"
#include "ch_ig_hrz.h"
#include "ch_ig_key.h"
#include "ch_ig_man.h"
#include "ch_ig_rinse.h"
#include "ch_ig_peri.h"
#include "ch_ig_piston.h"
#include "ch_ig_spw.h"
#include "ch_ig_test.h"
#include "ch_ig_uniadapter.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"
// ---------- Pipettor_Allgemein
#include "Well_Defines.h"

extern bit g_bBreakFlag;

	// Lokale Funktionen
void ProgMainMode(void);
void ProgManMode(void);
void ProgHostMode(void);
void ProgConfigMode(void);
void ShowChoiceDisplay(const struct Modus *ptr,unsigned char len,unsigned char cls);

/* --- HAUPTMENÜ --- */
extern struct String str_cover_open1[],str_cover_shut[]; 	// Variable aus dem Modul ch_ig_head.c

DispString_t str_man_ctrl[]			 = 	{{1,1,"Manuell"}
										,{1,1,"Manually"}};
DispString_t str_config[]			 = 	{{1,10,"Konfiguration"}
										,{1,11,"Configuration"}};
DispString_t str_statistics[]		 = 	{{1,25,"Statistik"}
										,{1,26,"Statistics"}};
DispString_t str_head_change[]		 = 	{{2,1,"Kopfwechsel"}
										,{2,1,"Changing head"}};
DispString_t str_host_ctrl[]		 = 	{{1,15,"HOST - MODE"}
										,{1,15,"HOST - MODE"}};

 /* --- MENÜ HANDBETRIEB --- */
DispString_t str_dos[]				 = 	{{1,1,"Dosieren"}
										,{1,1,"Dosage"}};
DispString_t str_pip[]				 = 	{{1,10,"Pipettieren"}
										,{1,9,"Pipetting"}};
DispString_t str_disp[]				 = 	{{1,22,"Dispensieren"}
										,{1,20,"Dispensing"}};
DispString_t str_rinsing[]			 = 	{{1,35,"Sp\365len"}
										,{1,32,"Rinse"}};
DispString_t str_dil[]				 = 	{{2,1,"Dilutieren"}
										,{2,1,"Dilution"}};
DispString_t str_ex_tips[]			 = 	{{2,12,"Spitzenwechsel"}
										,{2,11,"Changing tips"}};
DispString_t str_exit[]				 = 	{{2,32,"Verlassen"}
										,{2,37,"Exit"}};

/* --- Menü HANDBETRIEB OL3387 Hummingwell-Kopf ---*/
DispString_t str_OL3387_disp[]		 = 	{{1,1,"Dispensieren"}
										,{1,1,"Dispensing"}};
DispString_t str_OL3387_ex_tips[]	 = 	{{1,14,"Spitzenwechsel"}
										,{1,12,"Changing tips"}};

/* --- MENÜ HOSTMODE --- */
DispString_t str_host_ex_tips[]					 = 	{{2,13,"Spitzenwechsel"}
													,{2,15,"Changing tips"}};
DispString_t str_host_ex_tips_no_head_change[]	 = 	{{2,1,"Spitzenwechsel"}
													,{2,1,"Changing tips"}};

 /* --- MENÜ KONFIGURATION --- */
DispString_t str_wait_gen[]			 = 	{{1,1,"Wartezeit"}
										,{1,1,"Delay"}};
DispString_t str_upper_way_gen[]	= 	{{1,11,"Tischweg"}
										,{1,8,"Stage height"}};
DispString_t str_conf_dz[]			 = 	{{1,21,"dZ"}
										,{1,22,"dZ"}};
DispString_t str_conf_wells[]		 = 	{{1,25,"Wells"}
										,{1,26,"Wells"}};
DispString_t str_conf_xy[]			 = 	{{1,32,"XY"}
										,{1,33,"XY"}};
DispString_t str_pump_rev[]			 = 	{{2,1,"Kolben/"}
										,{2,1,"Piston speed"}};
DispString_t str_stage_rev[]		 = 	{{2,8,"Tisch Geschw."}
										,{2,15,"Stage speed"}};
DispString_t str_speaker[]			 = 	{{2,23,"Klick"}
										,{2,28,"Beep"}};
DispString_t str_exit_con[]			 = 	{{2,32,"Verlassen"}
										,{2,37,"Exit"}};

/* --- Menü KONFIGURATION OL3387 Hummingwell-Kopf ---*/
DispString_t str_OL3387_upper_way_gen[]	 = 	{{1,1,"Tischweg"}
											,{1,1,"Stage height"}};
DispString_t str_OL3387_conf_dz[]		 = 	{{1,10,"dZ"}
											,{1,14,"dZ"}};
DispString_t str_OL3387_conf_wells[]	 = 	{{1,13,"Wells"}
											,{1,17,"Wells"}};
DispString_t str_OL3387_conf_xy[]		 = 	{{1,19,"XY"}
											,{1,23,"XY"}};
DispString_t str_OL3387_stage_rev[]		 = 	{{2,1,"Tisch Geschw."}
											,{2,1,"Stage speed"}};
DispString_t str_OL3387_speaker[]		 = 	{{2,15,"Klick"}
											,{2,13,"Beep"}};

 /* --- UNTERMENÜ HANDBETRIEB --- */
DispString_t str_start_ex[]		 = 	{{2,1,"Start"}
									,{2,1,"Start"}};
DispString_t str_exit_ex[]		 = 	{{2,7,"Verlassen"}
									,{2,7,"Exit"}};
DispString_t str_ex_tips_ex[]	 = 	{{2,18,"Spitzenwechsel"}
									,{2,13,"Changing tips"}};
DispString_t str_rinsing_ex[]	 = 	{{2,34,"Sp\365len"}
									,{2,28,"Rinse"}};
DispString_t str_tipping_big[]	 = 	{{1,1,"ABTUPFPAUSE"}
									,{1,1,"TIP BLOTTING"}};
DispString_t str_stop[]			 = 	{{2,1,"Stopp:\004\005"}
									,{2,1,"Stop:\004\005"}};
DispString_t str_prg_end[]		 = 	{{1,1,"Programm beendet"}
									,{1,1,"Program stopped"}};
DispString_t str_wait[]			 = 	{{2,1,"Bitte warten!"}
									,{2,1,"Please wait!"}};
DispString_t str_empty_rinse[]	 = 	{{1,1,"Spitzen leeren"}
									,{1,1,"Empty tips"}};
DispString_t str_head_adjust[]	 = 	{{1,1,"Kolbenjustierung"}
									,{1,1,"Piston adjustment"}};
DispString_t str_rev_unit_c[]	 = 	{{2,14,"Umdr/min"}
									,{2,17,"rpm"}};
DispString_t str_par_err[]		 = 	{{1,1,"Parameterfehler"}
									,{1,1,"Wrong parameter"}};
DispString_t str_ini[]			 = 	{{1,12,"Initialisierung"}
									,{1,13,"Initialization"}};
DispString_t str_ini_p[]		 = 	{{1,9,"Initialisierung Kolben"}
									,{1,10,"Initialization piston"}};
DispString_t str_ini_z[]		 = 	{{1,9,"Initialisierung Tisch"}
									,{1,9,"Initialization stage"}};

const char *str_rev_pz[]					 = 	{"Drehzahl [Umdr/min]:","Revolutions [rpm]:"};
const char *str_steps[]						 = 	{"Anz.:","Steps:"};

DispString_t key_break[]			 = 	{{1,1,"Abbruch durch Taste!"}
									 	,{1,1,"Stop with key"}};
DispString_t str_abort[]			 = 	{{2,30,"Abbruch:\004\005"}
										,{2,33,"Stop:\004\005"}};

 /* Achtung,bei Auswahl einer anderen Sprache müssen die 3 Felder dynamisch
	initialisiert werden! */
const struct Modus host_mode[]									 = 	{{str_head_change,ProgHeadChange}
																	,{str_host_ex_tips,ProgManTipsChange}};
const struct Modus host_mode_no_head_change[]					 = 	{{str_host_ex_tips_no_head_change,ProgManTipsChange}};
const struct Modus mode_of_operation[]							 = 	{{str_man_ctrl,ProgManMode}
																	,{str_config,	ProgConfigMode}
																	,{str_statistics,ProgStatisticsMode}
																	,{str_head_change,ProgHeadChange}};
const struct Modus mode_of_operation_no_head_change[]			 = 	{{str_man_ctrl,ProgManMode}
		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	,{str_config,	ProgConfigMode}
																	,{str_statistics,ProgStatisticsMode}};
const struct Modus basic_function[]								 = 	{{str_dos,prg_man_dos}
																	,{str_pip,prg_man_pip}
																	,{str_disp,prg_man_disp}
																	,{str_rinsing,prg_rinsing}
																	,{str_dil,prg_man_dil}
																	,{str_ex_tips,ProgManTipsChange}
																	,{str_exit,ProgExit}};
const struct Modus OL3387_BasicFunktions[]						 = 	{{str_OL3387_disp,prg_OL3387_man_disp}
																	,{str_OL3387_ex_tips,ProgManTipsChange}
																	,{str_exit,ProgExit}};
const struct Modus execute_function[COUNT_EX_F]					 = 	{{str_start_ex,prg_man_dos}			/* Dummy adr,nicht ändern */
																	,{str_exit_ex,ProgExit}
																	,{str_ex_tips_ex,ProgManTipsChange}
																	,{str_rinsing_ex,prg_rinsing}};
const struct Modus OL3387_ExecuteFunctions[COUNT_OL3387_EX_F]	 = 	{{str_start_ex,prg_man_dos}			/* Dummy adr,nicht ändern */
																	,{str_exit_ex,ProgExit}
																	,{str_ex_tips_ex,ProgManTipsChange}};
const struct Modus config_function_384[]						 = 	{{str_wait_gen,ProgSetDispWait}
																	,{str_upper_way_gen,VertConfigWayLimit}
																	,{str_conf_dz,ProgSetDz}
																	,{str_conf_wells,ChoicePlateTypeMode}
																	,{str_conf_xy,XYProgConfig}
																	,{str_pump_rev,ConfigPumpManEndRev}
																	,{str_stage_rev,VertConfManRev}
																	,{str_speaker,ProgSetSpeaker}
																	,{str_exit_con,ProgExit}};
const struct Modus OL3387_ConfigFunctions_384[]				 = 	{{str_OL3387_upper_way_gen,VertConfigWayLimit}
																	,{str_OL3387_conf_dz,ProgSetDz}
																	,{str_OL3387_conf_wells,ChoicePlateTypeMode}
																	,{str_OL3387_conf_xy,XYProgConfig}
																	,{str_OL3387_stage_rev,VertConfManRev}
																	,{str_OL3387_speaker,ProgSetSpeaker}
																	,{str_exit_con,ProgExit}};
const struct Modus config_function_96[]							 = 	{{str_wait_gen,ProgSetDispWait}
																	,{str_upper_way_gen,VertConfigWayLimit}
																	,{str_conf_dz,ProgSetDz}
																	,{str_pump_rev,ConfigPumpManEndRev}
																	,{str_stage_rev,VertConfManRev}
																	,{str_speaker,ProgSetSpeaker}
																	,{str_exit_con,ProgExit}};
const struct Modus OL3387_ConfigFunctions_96[]					 = 	{{str_upper_way_gen,VertConfigWayLimit}
																	,{str_conf_dz,ProgSetDz}
																	,{str_stage_rev,VertConfManRev}
																	,{str_speaker,ProgSetSpeaker}
																	,{str_exit_con,ProgExit}};
const struct Modus config_function_1536[]						 = 	{{str_wait_gen,ProgSetDispWait}
																	,{str_upper_way_gen,VertConfigWayLimit}
																	,{str_conf_dz,ProgSetDz}
//																	,{str_conf_wells,ChoicePlateTypeMode}	// z.Z. werden keine höherauflösenden Platten unterstützt
																	,{str_conf_xy,XYProgConfig1536}
																	,{str_pump_rev,ConfigPumpManEndRev}
																	,{str_stage_rev,VertConfManRev}
																	,{str_speaker,ProgSetSpeaker}
																	,{str_exit_con,ProgExit}};

ubyte g_ubytLang;		// Index für Sprachauswahl,Copy im EEPROM
ubyte g_ubytDrvInitState = 0x0F;	// alle Antriebe unkalibriert

bool g_bAllowHostMode;	// erlaube Hostmode
bool g_bXYPresent;		// flag_xy - nur bei 384er Ausführung wahr,bei 96er falsch
bool g_bHostMode;		// host_flag - Kennung für Hostmode
bool g_bTestFlag;		// Wahr: Hilfsausgaben,die nicht dem Protokoll entsprechen,werden erlaubt
bool g_bEnableDrive;	// gilt für HDrv und VertDrv
bool g_bBeepState;		// Piepser an / aus
bool g_bShowSpots384;	// erlaubt Anzeige der Platznummern

unsigned short g_uiDeviceType;

static ubyte m_bytCurPosIndex;	// Position des Kursors im Menü beim Aufruf

enum {BASIC_MODE,MAN_MODE,PROG_MODE,MAIN_MODE }enProgMode;

void UserMain(void){
	// prg_break_flag = FALSE;
	g_bBeepState = TRUE;
	g_bTestFlag = FALSE; // for Programmer
	g_bHostMode = FALSE;
	g_bXYPresent = FALSE;
	g_bEnableDrive = FALSE;
	g_bAllowHostMode = FALSE;
	g_bPstFirstSuck = TRUE;
	g_bFirstDisp = TRUE;
	g_bCursorState = FALSE;
	g_bShowSpots384 = FALSE;	//keine Anzeige der Platznummern
	g_bBreakFlag = FALSE;

	SytemInitialization();
	if(g_bTestFlag){
		printf("Initstate nach Init:%x\r",g_ubytDrvInitState);
		fflush(stdout);
	}
//	EEPROMReadAndDisplay();
//	while(1){}
	// für EMV-Test
	//PrgSelfTest();

   /* Hauptprogramm */
	ResetInpBuffHost();
	Asc0DtrCtrl(ON); PermitHostMode(TRUE);
	if(g_bHostMode){					//HostMode()
		ClearAllAndDisplay(str_host_ctrl);
		while(1){
			WDO();
			ProgHostMode();
			if(IsCmdComplete()||g_bBreakFlag)
			{
				CmdCopy();
				CmdExec();
			}
			if(IsResetRequest())
				PerformSoftwareReset();
		}
	}

	if(!((g_uiDeviceType == 3381)||(g_uiDeviceType == 3714))){	//OL3381 --> MKF bzw. OL3714--> LHA
		while(IsHeadCoverOpen()) WarningDisplay(str_cover_open1,str_cover_shut);
	}

	while(1){
		if(enProgMode == MAN_MODE) ProgManMode();
		m_bytCurPosIndex = 0; ProgMainMode();			//es gibt nur main mode?
	}
}

	// Anzeige und Auswahl im Hostmenu	   sizeof(host_mode)/sizeof(struct Modus)  COUNT_HM_NO_HEAD_CHANGE
void ProgHostMode(void){
	if(g_uiDeviceType == 3714){	// kein Wechselkopfgerät
		m_bytCurPosIndex = ProgModification(host_mode_no_head_change,(char)sizeof(host_mode_no_head_change)/sizeof(struct Modus),0,SECOND_LINE);
		PermitHostMode(FALSE);
		host_mode_no_head_change[m_bytCurPosIndex].fp();//mod_ptr->fp();
	}else{
		m_bytCurPosIndex = ProgModification(host_mode,(char)sizeof(host_mode)/sizeof(struct Modus),0,SECOND_LINE);
		PermitHostMode(FALSE);
		host_mode[m_bytCurPosIndex].fp();//mod_ptr->fp();
	}
	PermitHostMode(TRUE);
}

	// Anzeige und Auswahl im Hauptmenu
void ProgMainMode(void){
	if(g_uiDeviceType == 3714){	// kein Wechselkopfgerät
		m_bytCurPosIndex = ProgModification(mode_of_operation_no_head_change,(char)sizeof(mode_of_operation_no_head_change)/sizeof(struct Modus),0,ALL_LINE);
		mode_of_operation_no_head_change[m_bytCurPosIndex].fp();//mod_ptr->fp();
		enProgMode = MAIN_MODE;
	}else{
		m_bytCurPosIndex = ProgModification(mode_of_operation,(char)sizeof(mode_of_operation)/sizeof(struct Modus),0,ALL_LINE);
		mode_of_operation[m_bytCurPosIndex].fp();//mod_ptr->fp();
		enProgMode = MAIN_MODE;
	}
}

	// Anzeige und Auswahl im Menu "Manueller Mode"
void ProgManMode(void){
	void (*fp_help)();
	const struct Modus *pMod;
	fp_help = ProgExit; enProgMode = MAN_MODE;
	if(g_pudtHeadType->type == OL3387_8){
		while(1){
			m_bytCurPosIndex = ProgModification(OL3387_BasicFunktions,(char)sizeof(OL3387_BasicFunktions)/sizeof(struct Modus),m_bytCurPosIndex,ALL_LINE);
			pMod = &OL3387_BasicFunktions[m_bytCurPosIndex]; if(pMod->fp == fp_help) return;
			// nachfolgende Befehle werden über Init und Quit gesteuert
			g_bShowSpots384 = TRUE; PermitHostMode(FALSE);
			pMod->fp(); PermitHostMode(TRUE);
			g_bShowSpots384 = FALSE;
		}
	}else{
		while(1){
			m_bytCurPosIndex = ProgModification(basic_function,(char)sizeof(basic_function)/sizeof(struct Modus),m_bytCurPosIndex,ALL_LINE);
			//if(mod_ptr->fp == fp_help) return;
			pMod = &basic_function[m_bytCurPosIndex]; if(pMod->fp == fp_help) return;
			// nachfolgende Befehle werden über Init und Quit gesteuert
			g_bShowSpots384 = TRUE; PermitHostMode(FALSE);
			if(g_udtLifePara.PstCycleVolume != 0){
				ClearAllAndDisplay(str_empty_rinse);
				UserRequestRinse();
			}
			pMod->fp(); PermitHostMode(TRUE);
			g_bShowSpots384 = FALSE;
		}
	}
}

// Anzeige und Auswahl im Menu "Konfiguration"
void ProgConfigMode(void){
	void (*fp_help)();
	const struct Modus *pMod;
	fp_help = ProgExit;
	if(IsHeadType1536()){
		while(1){
			if(g_bXYPresent)
				m_bytCurPosIndex = ProgModification(config_function_1536,(char)sizeof(config_function_1536)/sizeof(struct Modus),0,ALL_LINE);
			else
				m_bytCurPosIndex = ProgModification(config_function_96,(char)sizeof(config_function_96)/sizeof(struct Modus),0,ALL_LINE);
			pMod = &config_function_1536[m_bytCurPosIndex]; if(pMod->fp == fp_help) break;
			PermitHostMode(FALSE); pMod->fp(); PermitHostMode(TRUE);
		}
	}else{
		if(g_pudtHeadType->type == OL3387_8){
			while(1){
				if(g_bXYPresent)
					m_bytCurPosIndex = ProgModification(OL3387_ConfigFunctions_384,(char)sizeof(OL3387_ConfigFunctions_384)/sizeof(struct Modus),0,ALL_LINE);
				else
					m_bytCurPosIndex = ProgModification(OL3387_ConfigFunctions_96,(char)sizeof(OL3387_ConfigFunctions_96)/sizeof(struct Modus),0,ALL_LINE);
				pMod = &OL3387_ConfigFunctions_384[m_bytCurPosIndex]; if(pMod->fp == fp_help) break;
				PermitHostMode(FALSE); pMod->fp(); PermitHostMode(TRUE);
			}
		}else{
			while(1){
				if(g_bXYPresent)
					m_bytCurPosIndex = ProgModification(config_function_384,(char)sizeof(config_function_384)/sizeof(struct Modus),0,ALL_LINE);
				else
					m_bytCurPosIndex = ProgModification(config_function_96,(char)sizeof(config_function_96)/sizeof(struct Modus),0,ALL_LINE);
				pMod = &config_function_384[m_bytCurPosIndex]; if(pMod->fp == fp_help) break;
				PermitHostMode(FALSE); pMod->fp(); PermitHostMode(TRUE);
			}
		}
	}
}

void ProgExit(void){			// Dient nur als Adressspezifizierung
	;
}

/* FUNCTION:	ProgModification
   PARAMETERS:  Zeiger auf Menü,Anz. der Einträge,Kursorposition,Flag Löschen des Displays
   DESCRIPTION:	Menüaufbau und Steuerung des Kursors durch das Menü
   RETURNS:	Kursorposition */
int8_t ProgModification(const struct Modus *ptr,int8_t len,int8_t beg,ubyte cls){
	int8_t id;
	const struct Modus *pMod;
	pMod = (struct Modus*)ptr+beg; id = beg;
	ShowChoiceDisplay(ptr,len,cls);
	while(1){
		SetCursorOn((pMod->msg)+g_ubytLang);
		while(KeyScan() != NO_KEYS) WDO();	// Warten auf Freigabe der Tasten
		switch (KeyWaitForAny()){
		case ARR_LEFT_KEY:
			if(--pMod < ptr) pMod = ptr+len-1;
			if(--id < 0) id = len-1;
			break;
		case ARR_RIGHT_KEY:
			if(++pMod == ptr+len) pMod = ptr;
			if(++id == len) id = 0;
			break;
		case ENTER_KEY:
			SetCursorOff();
			return id;
		// Display neu generieren
		case CMD_COMPLETE:
			ShowChoiceDisplay(ptr,len,cls);
		}
	}
}

void ShowChoiceDisplay(const struct Modus *ptr,unsigned char len,unsigned char cls){
	int i;
	LcdClear(cls);
	for(i = 0;i<len;i++) {	//Anzeige aufbauen
		Display((ptr->msg)	/*+lang*/);
		ptr++;
		WDO();
	}
}

void ShowInit(void){
	ClearAllAndDisplay(str_ini);
	Display(str_wait);
}

void PermitHostMode(ubyte flg){
	if(flg == TRUE){
		g_bAllowHostMode = TRUE;
		ResetInpBuffHost();
	}else
		g_bAllowHostMode = FALSE;
	return;
}

ubyte IsPermitHostMode(void){
	if(g_bAllowHostMode) return TRUE;
	return FALSE;
}
