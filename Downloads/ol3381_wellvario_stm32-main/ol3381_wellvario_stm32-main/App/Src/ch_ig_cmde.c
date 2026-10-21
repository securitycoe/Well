/*******************************************************************
 *
 *	DESCRIPTION:	Kommandoerweiterungen
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/
#include "stdbool.h"
#include <stdio.h>

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

#include "ch_head_fram.h"
#include "head_piston_tab.h"
#include <ig_define.h>
// ---------- Inc:
#include <_i2c_fram.h>

// ---------- Master
#include "ch_ig_cmd.h"
#include <ch_ig_config.h>
#include "ch_ig_eeprom.h"
#include "ch_ig_grp.h"
#include "ch_ig_head.h"
#include "ch_ig_hrz.h"
#include "ch_ig_key.h"
#include "ch_ig_peri.h"
#include "ch_ig_piston.h"
#include "ch_ig_ptme.h"
#include "ch_ig_spw.h"
#include "ch_ig_test.h"
#include "ch_ig_uniadapter.h"
#include "ch_ig_utility.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"
// ---------- Pipettor_Allgemein/VolumenKorrektur
// ---------- Pipettor_Allgemein
#include "Well_Cmd.h"
#include "Well_Error.h"
#include "string_extension.h"
#include "math_extension.h"

/** local definitions **/
typedef enum
{
	SRC_HEAD_FRAM = 0
	,SRC_DEV_EEPROM
}EEPROMSource_t	;

#define IDENTIFIER_FRAM_VERSION				"-v"
#define IDENTIFIER_FRAM_DELETE				"-d"
#define IDENTIFIER_FRAM_WRITE				"-w"
#define IDENTIFIER_FRAM_READ				"-r"
#define IDENTIFIER_FRAM_USED_SIZE			"-s"
#define IDENTIFIER_FRAM_HEX					"-hex"
#define SEP_WHITESPACE						" "

#define END_CHAR							'\n'

#define EEPROM_READ_SIZE 				8
#define EEPROM_MAX_BLOCK_SIZE 			32

/** default settings **/

/** external data **/

/** internal functions **/
ubyte CmdExtForHead(void);
ubyte CmdExtForDev(void);
ubyte CmdHeadReadNo(void);
ubyte CmdHeadSetNo(void);
ubyte CmdHeadReadStructVersions(void);
ubyte CmdHeadReadPara(void);
ubyte CmdHeadInitFRAM(void);
ubyte CmdHeadReadMemHex(void);
ubyte CmdHeadReadDevExchange(void);
ubyte CmdDevSetNo(void);
ubyte CmdDevReadNo(void);
ubyte CmdDevInitEEPROM(void);
ubyte CmdDevReadStructVersions(void);
ubyte DevTestSensors(void);
ubyte CmdDevTestPeri(void);
ubyte CmdHeadPeriTest(void);
ubyte CmdReadHeadExchange(void);
ubyte CmdSetHeadExchange(void);
ubyte CmdHeadSetDevExchange(void);
ubyte CmdHeadReadTotalVolume(void);
ubyte CmdHeadReadPstCnt(void);
ubyte CmdSetTipsParaFromASPW(void);
ubyte CmdDevTestHeadLift(void);
ubyte CmdDevCheckHeadContact(void);
ubyte CmdHeadSetPstCnt(void);
ubyte CmdHeadSetTotalVolume(void);
ubyte CmdDevSetCarrType(void);
ubyte CmdDevReadCarrType(void);
ubyte CmdDevSetHeadSafety(void);
ubyte CmdDevReadLS(void);
ubyte CmdReadWay(void);
ubyte CmdReadSensor(void);
ubyte CmdDevSetShieldWatch(void);
ubyte CmdDevReadShieldWatch(void);
ubyte CmdDevSetXYCnt(void);
ubyte CmdDevReadXYCnt(void);
ubyte CmdDevSetUniServoPara(void);
ubyte CmdDevReadUniServoPara(void);
ubyte CmdGoDrivePos(void);
ubyte CmdReadDrivePos(void);
ubyte CmdDevReadPMFlag(void);
ubyte CmdDevSetPMFlag(void);
ubyte CmdDevSetMagazineMode(void);
ubyte CmdDevReadMagazineMode(void);
ubyte CmdDevReadXYSwitchPos(void);
ubyte CmdDevSetXYSwitchPos(void);
ubyte CmdDevSetKeyboardLayout(void);
ubyte CmdDevReadKeyboardLayout(void);
ubyte CmdSetVolumeCorr(void);
ubyte CmdGetVolumeCorr(void);
ubyte CmdSetOffset(void);
ubyte CmdGetOffset(void);
ubyte CmdSetTipNo(void);
ubyte CmdGetTipNo(void);
static ErrorState_t CmdSetSoftwareReset (void);
static ErrorState_t CmdManageHeadFRAMContent(void);
static ErrorState_t CmdManageDevEEPROMContent(void);
static ErrorState_t CmdEEPROMCheckReadPara(const EEPROMSource_t source, stre_split_vars * const pTokenizer, uint16_t * const pAddress, uint16_t *pLength);
static ErrorState_t CmdEEPROMReadDataToBinaryString(const EEPROMSource_t source, const uint32_t eeprom_address, const uint16_t length, char* pAnswer);
static ErrorState_t CmdEEPROMCheckWritePara(const EEPROMSource_t source, stre_split_vars * const pTokenizer, uint16_t * const pAddress, uint16_t * const pLength, char ** const pData);
static ErrorState_t CmdEEPROMWriteDataFromBinaryString(const EEPROMSource_t source,const uint32_t eeprom_address, const uint16_t length, const char* const pDataHex);
static uint32_t GetEEPROMSize (const EEPROMSource_t source);
static ErrorState_t WriteToEEPROM (const EEPROMSource_t source, const uint8_t address, const uint8_t * const pData, const uint16_t data_length);
static ErrorState_t ReadFromEEPROM (const EEPROMSource_t source,const uint8_t address,  uint8_t * const pData, const uint16_t data_length);

/** public data **/

/* FUNCTION: 	CmdExtension
   PARAMETERS:	---
   DESCRIPTION:	Kommando zur Befehlerweiterung
   RETURNS: 	CB_READY / ERROR */
ubyte CmdExtension(void)
{
  if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA; /* Befehl */
  switch(*g_pchCmdBuf)
  {
	case C_GRIPPER:			return CmdGripperCtrl();
	case C_READ_TIPS_CNT:	sprintf(g_acAnswerBuf,"I%u\r",g_pudtHeadDevPara->PstCnt); return INFO_FLAG;
	case C_CMD_EXT_H: 		return CmdExtForHead();
	case C_CMD_EXT_D: 		return CmdExtForDev();
	case C_SET_TIPS_PAR:	return CmdSetTipsParaFromASPW();
	case C_READ_WAY:		return CmdReadWay();
	case C_READ_SENSOR:		return CmdReadSensor();
	case C_DISP_CURSOR:		return CmdDispCurCtrl();
	case C_GO_DRIVE_POS:  	return CmdGoDrivePos();
	case C_READ_DRIVE_POS: 	return CmdReadDrivePos();

	default:   return IERR_CMD_NOT_IMPLEMENTED;
  }
}

/* FUNCTION: 	CmdReadSensor
   PARAMETERS:	---
   DESCRIPTION:	Kommando zum Lesen eines ausgewählten Sensors
   RETURNS: 	INFO_FLAG / ERROR */
ubyte CmdReadSensor(void)
{float fhelp;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  switch(atoi(g_pchCmdBuf))
  {
	case 1:		fhelp = ReadVertDrvCurrentPos();
				if(fhelp >=  g_fVertXYSwitchPos) fhelp -= g_fVertHeightCorr;	// oberhalb von g_fVertXYSwitchPos wird mit Korrektur gerechnet
				sprintf(g_acAnswerBuf,"I%7.2f\r",(double)fhelp); return INFO_FLAG;

	default:   return IERR_PARA;
  }
}

/* FUNCTION: 	CmdExtForHead
   PARAMETERS:	---
   DESCRIPTION:	Kommando zur Befehlerweiterung für den Kopf
   RETURNS: 	CB_READY / ERROR */
ubyte CmdExtForHead(void)
{
  NextPara();if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  switch(*g_pchCmdBuf)
  {
	case C_HEAD_READ_NO:		return CmdHeadReadNo();
	case C_HEAD_SET_NO:			return CmdHeadSetNo();
	case C_HEAD_READ_VER:		return CmdHeadReadStructVersions();
	case C_HEAD_INT_FRAM:		return CmdHeadInitFRAM();
	case C_HEAD_READ_DEV_EX:	return CmdHeadReadDevExchange();
	case C_HEAD_SET_DEV_EX:		return CmdHeadSetDevExchange();
	case C_HEAD_READ_MEM_HEX:	return CmdHeadReadMemHex();
	case C_HEAD_READ_PARA:		return CmdHeadReadPara();
	case C_HEAD_TEST_FKT:		return CmdHeadPeriTest();
	case C_HEAD_SET_VOL:		return CmdHeadSetTotalVolume();
	case C_HEAD_READ_VOL:		return CmdHeadReadTotalVolume();
	case C_HEAD_SET_PST_CNT:	return CmdHeadSetPstCnt();
	case C_HEAD_READ_PST_CNT:	return CmdHeadReadPstCnt();

	case C_HEAD_SET_VOL_CORR: 	return CmdSetVolumeCorr();	//vorerst keine Impementierung 17.07.2015 Bau
	case C_HEAD_GET_VOL_CORR: 	return CmdGetVolumeCorr();
	case C_HEAD_SET_OFFSET:		return CmdSetOffset();
	case C_HEAD_GET_OFFSET:		return CmdGetOffset();
	//case C_HEAD_SET_TIP_NO:		return CmdSetTipNo();
	//case C_HEAD_GET_TIP_NO:		return CmdGetTipNo();
	case C_HEAD_EEPROM_ACCESS:	return CmdManageHeadFRAMContent();
	default:   return IERR_CMD_NOT_IMPLEMENTED;
  }
}


/* FUNCTION: 	CmdExtForDev
   PARAMETERS:	---
   DESCRIPTION:	Kommando zur Befehlerweiterung für das Grundgerät
   RETURNS: 	CB_READY / ERROR */
ubyte CmdExtForDev(void)
{
  NextPara();if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  switch(*g_pchCmdBuf)
  {
	case C_DEV_INT_EEPROM:		return CmdDevInitEEPROM();
	case C_DEV_READ_VER:		return CmdDevReadStructVersions();
	case C_DEV_READ_NO:			return CmdDevReadNo();
	case C_DEV_SET_NO:			return CmdDevSetNo();
	case C_DEV_READ_ERR:		return CmdDevReadErr();
	case C_DEV_READ_MEM_HEX:	return EEPROMReadAndDisplay();
	case C_DEV_SET_HEAD_EX:		return CmdSetHeadExchange();
	case C_DEV_READ_HEAD_EX:	return CmdReadHeadExchange();
	case C_DEV_TEST_FKT:		return CmdDevTestPeri();
	case C_DEV_TEST_HEAD_LIFT:	return CmdDevTestHeadLift();
	case C_DEV_HEAD_CONTACT:	return CmdDevCheckHeadContact();
	case C_DEV_SET_CARR_TYPE:   return CmdDevSetCarrType();
	case C_DEV_READ_CARR_TYPE:	return CmdDevReadCarrType();
	case C_DEV_SET_HEAD_SAFETY:	return CmdDevSetHeadSafety();
	case C_DEV_READ_LS:			return CmdDevReadLS();
	case C_DEV_SET_SHIELD_WATCH:  return CmdDevSetShieldWatch();
	case C_DEV_READ_SHIELD_WATCH: return CmdDevReadShieldWatch();
	case C_DEV_SET_XY:			return CmdDevSetXYCnt();
	case C_DEV_READ_XY:			return CmdDevReadXYCnt();
	case C_DEV_SET_UNI_SERVO_PARA:  return CmdDevSetUniServoPara();
	case C_DEV_READ_UNI_SERVO_PARA: return CmdDevReadUniServoPara();
	case C_DEV_READ_PM_FLAG:		return CmdDevReadPMFlag();
	case C_DEV_SET_PM_FLAG:		 return CmdDevSetPMFlag();
	case C_DEV_SET_MAGAZINE_MODE:	return CmdDevSetMagazineMode();
	case C_DEV_READ_MAGAZINE_MODE: 	return CmdDevReadMagazineMode();
	case C_DEV_READ_XY_SWITCH_POS:	return CmdDevReadXYSwitchPos();
	case C_DEV_SET_XY_SWITCH_POS:	return CmdDevSetXYSwitchPos();
	case C_DEV_SET_KEYBOARD_LAYOUT:		return CmdDevSetKeyboardLayout();
	case C_DEV_READ_KEYBOARD_LAYOUT:	return CmdDevReadKeyboardLayout();
	case C_DEV_SOFTWARE_RESET:		return CmdSetSoftwareReset();
	case C_DEV_SET_MEM_HEX:			return CmdManageDevEEPROMContent();

	default:					return IERR_CMD_NOT_IMPLEMENTED;
  }
}

ubyte CmdDevSetCarrType(void)
{ubyte help,state;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = atoi(g_pchCmdBuf);
  if(IsCarrType(help))
  {state = EEPROMSetCarrTypeAndCmp(help);
	if(state == CB_READY)
  	{
  		state = SetAndSaveHrzDrvParaToDefault();
  	}
  }
  else state = IERR_PARA;
  return state;
}

ubyte CmdDevReadCarrType(void)
{ubyte state;
  state = EEPROMReadCarrType(); if(state != CB_READY) return state;
  sprintf(g_acAnswerBuf,"I%u\r",(int)g_pudtHrzPosTabPtr->cartype);
  return INFO_FLAG;
}


/* FUNCTION: 	CmdSetTipsParaFromASPW
   PARAMETERS:	---
   DESCRIPTION:	Kommando zum Setzen der Spitzenparameter
   RETURNS: 	CB_READY / ERROR */
ubyte CmdSetTipsParaFromASPW(void)
{ubyte state; float max_vol,min_vol,over_vol,fkt_asc,fkt_zero;
  NextPara();
  max_vol = atof(g_pchCmdBuf); NextPara();
  min_vol = atof(g_pchCmdBuf); NextPara();
  over_vol = atof(g_pchCmdBuf); NextPara();
  fkt_asc = atof(g_pchCmdBuf); NextPara();
  fkt_zero = atof(g_pchCmdBuf); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	//Prüfung auf Plausibilität der Werte
  if(/*(max_vol > g_pudtHeadType->dev->VolMax)
  ||*/(max_vol < g_pudtHeadType->dev->VolMin)
  ||(min_vol > max_vol)
  ||(min_vol < g_pudtHeadType->dev->VolMin)
  ||(over_vol > g_pudtHeadType->dev->VolOver)
  ||(over_vol < g_pudtHeadType->dev->VolMin)
  ||(fkt_asc > 	g_pudtHeadType->dev->AscMax)||(fkt_asc < g_pudtHeadType->dev->AscMin)
  ||(fkt_zero > g_pudtHeadType->dev->ZeroMax)||(fkt_zero < g_pudtHeadType->dev->ZeroMin)
  ) return IERR_PARA;
  g_pudtTipsPara = &g_udtTipsParaFromASPW;
  /* Beim Betrieb der 40 µl-Köpfe mit dem ASPW tritt folgender Sachverhalt auf:
	 Es gibt nur eine Spitzenkennung für 25µl-Spitzen,diese können jedoch max. 40 µl fassen.
	 Um keine neuen Labels für die selbe Spitze zu erstellen wurde zusammen mit PM folgende Lµsung
	 abgestimmt: die 25µl werden beim 40µl-Kopf auf 40µl gesetzt.
  */
  if((g_pudtHeadType->type == OL3383_5)&&(max_vol>= 25))
	 g_udtTipsParaFromASPW.MaxVol = HeadDev_OL3383_5.VolMax;
  else if((g_pudtHeadType->type == OL3383_2)&&(max_vol>= 25))
	g_udtTipsParaFromASPW.MaxVol = HeadDev_OL3383_2.VolMax;
  else
  {
	if(max_vol > g_pudtHeadType->dev->VolMax) return IERR_PARA;
	g_udtTipsParaFromASPW.MaxVol = max_vol;
  }
  g_udtTipsParaFromASPW.MinVol = min_vol; g_udtTipsParaFromASPW.MoreDisp = over_vol;
  g_udtTipsParaFromASPW.asc = fkt_asc; g_udtTipsParaFromASPW.zero = fkt_zero;	g_udtTipsParaFromASPW.MoreSuck = 1;
  state = EEPROMSetFlagAndSaveTipsPara(&g_udtTipsParaFromASPW);
  if(state == CB_READY) g_ubytASPW = TRUE;
  return state;
}

/* FUNCTION: 	CmdReadHeadExchange
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Kopfewechsel am Gerät
   RETURNS: 	Fehlerkode / INFO_FLAG */
ubyte CmdReadHeadExchange(void)
{ulong help; ubyte state;
  state = EEPROMReadHeadExchangeAtDev(&help);
  if(state == CB_READY) {sprintf(g_acAnswerBuf,"I%lu\r",help); return INFO_FLAG;}
  return state;
}

/* FUNCTION: 	CmdSetHeadExchange
   PARAMETERS:	---
   DESCRIPTION:	Setzen der Kopfewechsel am Gerät
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdSetHeadExchange(void)
{ulong help;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = atol(g_pchCmdBuf); if(help < 0) return IERR_PARA;
  return EEPROMSetHeadExchangeAtDev(&help);
}

/* FUNCTION: 	CmdDevTestPeri
   PARAMETERS:	---
   DESCRIPTION:	Test diverser Peripheriekomponenten
   RETURNS: 	CB_READY */
ubyte CmdDevTestPeri(void)
{ubyte help;
  NextPara();
  help = atoi(g_pchCmdBuf);
  if(help == 0)	DevTestSensors();
  if(help == 1){DevTestIn0(); DevTestIn2(); DevTestIn3();DevTestOut7();}
  if(help == 2) {return PtmeTimeTest();}
  return CB_READY;
}

/* FUNCTION: 	DevTestSensors
   PARAMETERS:	---
   DESCRIPTION:	Test diverser Sensoren des Basisgerätes
   RETURNS: 	CB_READY */
ubyte DevTestSensors(void){
	if(g_uiDeviceType != 3381){
		DevTestSwitchCover();
		DevTestSwitchShield();
		DevTestHrzZeroPos();
		DevTestVertZeroPos();
		DevTestLiftPreWarnPos();
		DevTestLiftLowerPos();
		DevTestLiftUpperPos();
		DevTestSwitchHeadInsert();
		DevTestBeep();
	}else{				// manueller Kopfwechsel  oder kein Wechselkopf
		// für Gerät OL3714 und OL3381
		DevTestHeadContactX20();
		DevTestHrzZeroPos();
		DevTestVertZeroPos();
		DevTestBeep();
	}
	return CB_READY;
}

/* FUNCTION: 	CmdDevReadStructVersions
   PARAMETERS:	---
   DESCRIPTION:	Kommando zum Lesen der internen EEPROM Struktur
   RETURNS: 	INFO / Fehlerkode */
ubyte CmdDevReadStructVersions(void)
{ubyte state,vers;
  if((state = EEPROMReadStructVersion(&vers)) == CB_READY)
  {sprintf(g_acAnswerBuf,"I%d\r",(int)vers); state = INFO_FLAG;}
  return state;
}

/* FUNCTION: 	CmdDevInitEEPROM
   PARAMETERS:	---
   DESCRIPTION:	Löschen und Setzen der Versionsnummer des EEPROM's
   RETURNS: 	INFO / Fehlerkode */
ubyte CmdDevInitEEPROM(void)
{
	ubyte state;

	if((state = EEPROMClearAllAndCmp()) != CB_READY) return state;
	if((state = EEPROMASPWFlagResetAndCmp()) != CB_READY) return state;
	if((state = EEPROMHostModeFlagResetAndCmp()) != CB_READY) return state;
	g_bHostMode = TRUE;	// Da der Befehl ja nur über Schnittstelle ausgeführt werden kann!
	if((state = EEPROMSetStructVersionAndCmp()) != CB_READY) return state;
	if((state = EEPROMResetLSAndCmp()) != CB_READY) return state;
	if((state = EEPROMSetShieldWatchFlagAndCmp()) != CB_READY) return state;
	if((state = EEPROMSetHrzMotPara(&m_udtHrzRevDef)) != CB_READY) return state;
	//if((state = EEPROMWriteLifeParaPosition(&pos)) != CB_READY) return state; //write back
	//if((state = EEPROMWriteLifePara(&para)) != CB_READY) return state; //write back
	return EEPROMSetCarrTypeAndCmp(CARR_4);
	}

/* FUNCTION: 	CmdDevSetNo
   PARAMETERS:	---
   DESCRIPTION:	Einschreiben der Gerätenummer
   RETURNS: 	INFO / Fehlerkode */
ubyte CmdDevSetNo(void)
{ubyte state;
  state = CB_READY;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  state = EEPROMSetDevNumber(g_pchCmdBuf);
  if(state != CB_READY) return state;
  state = EEPROMReadDeviceType();	//globalen Gerätepointer aktualisieren
  if(state != CB_READY) return state;
  if((g_uiDeviceType == 3383)||(g_uiDeviceType == 3381))
  	state = EEPROMWriteVerticalSwitchPos(GetDefXYSwitchHeightOL3383());
  else
 	state = EEPROMWriteVerticalSwitchPos(GetDefXYSwitchHeightOL3714());
  return state;
}

/* FUNCTION: 	CmdDevReadNo
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Gerätenummer
   RETURNS: 	INFO / Fehlerkode */
ubyte CmdDevReadNo(void)
{ubyte state; struct EEPROM * ptr;
  char *cPtr = (char*)malloc(sizeof(ptr->DeviceNr));
  if((state = EEPROMReadDevNumber(cPtr)) == CB_READY){sprintf(g_acAnswerBuf,"I%s\r",cPtr); state = INFO_FLAG;}
  free(cPtr);
  return state;
}

/* FUNCTION: 	CmdHeadReadDevExchange
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Gerätewechselanzahl des Kopfes
   RETURNS: 	INFO / Fehlerkode */
ubyte CmdHeadReadDevExchange(void)
{ubyte state; ulong ulngDevExAtHead;
  if((state = FRAMReadDevExchangeAtHead(&ulngDevExAtHead)) == CB_READY)
  {	sprintf(g_acAnswerBuf,"I%lu\r",ulngDevExAtHead); state = INFO_FLAG;}
  return state;
}

/* FUNCTION: 	CmdHeadSetDevExchange
   PARAMETERS:	---
   DESCRIPTION:	Setzen der Gerätewechselanzahl des Kopfes
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdHeadSetDevExchange(void)
{long lhelp; ulong ulngDevExAtHead;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  lhelp = atol(g_pchCmdBuf);
  if(lhelp < 0) return IERR_PARA; else ulngDevExAtHead = lhelp;
  return FRAMWriteDevExchangeAtHead(&ulngDevExAtHead);
}

/* FUNCTION: 	CmdHeadInitFRAM	- Nur für Erstinbetriebnahme verwenden
   PARAMETERS:	---
   DESCRIPTION:	Löschen und Setzen der Versionsnummer des Kopfspeichers
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdHeadInitFRAM(void)
{ubyte state;
  if((state = FRAMClearAllAndCmp()) != CB_READY) return state;
  if((state = FRAMSetStructVersionP0AndCmp()) != CB_READY)return state;
  return FRAMSetStructVersionP1AndCmp();
}

/* FUNCTION: 	CmdHeadReadMemHex
   PARAMETERS:	---
   DESCRIPTION:	Auslesen des Kopfspeichers (hexadezimal)
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdHeadReadMemHex(void)
{ubyte state;
  if((state = FRAMReadPageAndDisplay(0x52)) != CB_READY) return state;
  return FRAMReadPageAndDisplay(0x53);
}

/* FUNCTION: 	CmdHeadReadNo
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Seriennummer des Kopfes
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdHeadReadNo(void)
{ubyte state; struct FRAM1 * ptr;
  char *cPtr = (char*)malloc(sizeof(ptr->HeadNo));
  state = FRAMReadHeadNumber(cPtr);
  if(state == CB_READY) {sprintf(g_acAnswerBuf,"I%s\r",cPtr); state = INFO_FLAG;}
  free(cPtr); return state;
}

/* FUNCTION: 	CmdHeadSetNo
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der Seriennummer in den Kopfspeicher
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdHeadSetNo(void)
{
  NextPara();if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  return FRAMSetHeadNumber(g_pchCmdBuf);
}

/* FUNCTION: 	CmdHeadReadStructVersions
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Strukturversionen des Kopfspeicher von Page 0 und 1
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdHeadReadStructVersions(void)
{ubyte state,ver0,ver1;	//struct FRAM1 * ptr;
	//ubyte *ubytPtr = (ubyte*)malloc(sizeof(ptr->StructVersNr));
  state = FRAMReadStructVersionP0(&ver0);
  if(state == CB_READY)
  {state = FRAMReadStructVersionP1(&ver1);
	if(state == CB_READY) {sprintf(g_acAnswerBuf,"I%d %d\r",(int) ver0,(int)ver1); state = INFO_FLAG;}
  }
	//free(ubytPtr);
  return state;
}

/* FUNCTION: 	CmdHeadReadPara
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Geräteparameter des  "Kopfes"
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdHeadReadPara(void)
{
  sprintf(g_acAnswerBuf,"I%f %f %f %f %f %u\r",g_pudtHeadDevPara->PstDrvHoldCurrent,g_pudtHeadDevPara->PstMaxWay,
			g_pudtHeadDevPara->PstUpperPlay,g_pudtHeadDevPara->PstGearRatio,g_pudtHeadDevPara->PstDrvMicroGearRatio,
			g_pudtHeadDevPara->PstCnt);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdHeadPeriTest
   PARAMETERS:	---
   DESCRIPTION:	Test diverser Sensoren des Kopfes
   RETURNS: 	CB_READY */
ubyte CmdHeadPeriTest(void)
{ubyte state,help;
  NextPara();
  help = atoi(g_pchCmdBuf);
  if(help == 0)
  {
	state = ReadHeadNoAndSetDevPara();
	if(state != CB_READY) return state;	// Fehler beim Lesen der Kopfnummer
	if(g_pudtHeadType->type == OL3387_8)// Hummingwell-Kopf
	{
	   if((state = Test_OL3387_ReadMagazinePara()) != CB_READY) return state;
	   if((state = TestReadEccentricSensorInFront()) != CB_READY) return state;
	   if((state = TestReadEccentricBackSensor()) != CB_READY) return state;
	   if((state = TestPstReadMagazineInsert()) != CB_READY) return state;
	}
	else
	{
	   if((state = TestPstReadMagazinePara()) != CB_READY) return state;
	   if((state = TestPstReadLowerPos()) != CB_READY) return state;
	   if((state = TestPstReadUpperPos()) != CB_READY) return state;
	   if((state = TestPstReadFineZeroPos()) != CB_READY) return state;
	   if((state = TestPstReadCoarseZeroPos()) != CB_READY) return state;
	   if((state = TestReadEccentricSensorInFront()) != CB_READY) return state;
	   if((state = TestReadEccentricBackSensor()) != CB_READY) return state;
	   if((state = TestPstReadMagazineInsert()) != CB_READY) return state;
	}
  }
  if(help == 1) return TestDrvEccentric();
  if(help == 2) return AdjTipsTightenMechanism();
  if(help == 3)	return TestTipsTightenMechanism();
  if(help == 4) return AdjPstCoarseZeroPos();
  if(help == 5) return AdjPstFineZero();
  if(help == 6) return TestHeadSwitchS1();
  if(help == 7) return TestOL3387_Valve();
  return CB_READY;
}

/* FUNCTION: 	CmdHeadSetTotalVolume
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Gesamtvolumenzählers des Kopfes
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdHeadSetTotalVolume(void)
{
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  return HeadSetTotalVolume(atof(g_pchCmdBuf));
}

/* FUNCTION: 	HeadSetTotalVolume
   PARAMETERS:	Gesamtvolumen
   DESCRIPTION:	Setzen des Gesamtvolumenzählers des Kopfes
   RETURNS: 	CB_READY / Fehlerkode */
ubyte HeadSetTotalVolume(float vol)
{ubyte state;
  if(vol < 0) return	IERR_PARA;
  g_udtLifePara.PstTotalVolume = vol;
  state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
  state = FRAMWritePstVolume(); if(state != CB_READY) return state;
  state = FRAMReadPstVolume(); if(state != CB_READY) return state;
  if(vol != g_udtLifePara.PstTotalVolume) return IERR_I2C_MEMORY;
  return CB_READY;
}

/* FUNCTION: 	CmdHeadReadTotalVolume
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Gesamtvolumenzählers des Kopfes
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdHeadReadTotalVolume(void)
{
	//Aktueller Werte steht nur im RAM
  sprintf(g_acAnswerBuf,"I%4.2f\r",g_udtLifePara.PstTotalVolume);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdHeadSetPstCnt
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Kolbenzykluszählers des Kopfes
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdHeadSetPstCnt(void)
{
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  return HeadSetPstCnt(atol(g_pchCmdBuf));
}

/* FUNCTION: 	HeadSetPstCnt
   PARAMETERS:	Zyklenanzahl
   DESCRIPTION:	Setzen des Kolbenzykluszählers des Kopfes
   RETURNS: 	CB_READY / Fehlerkode */
ubyte HeadSetPstCnt(long cycle)
{ubyte state;
  if(cycle < 0) return	IERR_PARA;
  g_udtLifePara.ulngPistonCycles = (ulong)cycle;
  state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
  state = FRAMWritePstCycles(); if(state != CB_READY) return state;
  state = FRAMReadPstCycles(); if(state != CB_READY) return state;
  if(cycle != g_udtLifePara.ulngPistonCycles) return IERR_I2C_MEMORY;
  return CB_READY;
}

/* FUNCTION: 	CmdHeadReadPstCnt
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Kolbenzykluszählers des Kopfes
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdHeadReadPstCnt(void)
{
	//Aktueller Wert steht nur im RAM
  sprintf(g_acAnswerBuf,"I%lu\r",g_udtLifePara.ulngPistonCycles);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdDevTestHeadLift
   PARAMETERS:	---
   DESCRIPTION:	Dauertest des Kopfaushebers
   RETURNS: 	Fehlerkode */
ubyte CmdDevTestHeadLift(void){
	ubyte state;
	state = EEPROMReadDeviceType();
	if(state != CB_READY) return state;
	if(g_uiDeviceType == 3383){	// CyBi-Well vario mit elektrischen Kopfwechsel
		NextPara();
		if(!*g_pchCmdBuf) return DevTestHeadLift();
		if(atoi(g_pchCmdBuf) == 1) return DevAdjustHeadLift();
		return IERR_PARA;
	}else{
		return IERR_COM_NOT_PRACTICABLE;
	}
}

/* FUNCTION: 	CmdDevCheckHeadContact
   PARAMETERS:	---
   DESCRIPTION:	Kontrolle des Abstandes elektr. Kontakt des Kopfes
				zur unteren Position (Sicherstellung des elektr. Kontaktes)
   RETURNS: 	Fehlerkode */
ubyte CmdDevCheckHeadContact(void){	// evtl. noch überarbeiten bei JQV-Erstellung
	ubyte state;
	state = EEPROMReadDeviceType();
	if(state != CB_READY) return state;
	if(g_uiDeviceType == 3383){	// CyBi-Well vario mit elektrischen Kopfwechsel
		while(1){
			WDO();
			{
				if(IsHeadPreWarnPos() || IsHeadLowerPos() || (HeadConnectionPin == 0)){
					HeadDCMotorUpwards;	//Ausfahren des Kopfes
				}else{
					Wait(100);
					HeadDCMotorStopp;
					break;
				}
				//Sollte nie erreicht werden,aber sicher ist sicher
				if(IsHeadUpperPos()){
					HeadDCMotorStopp;
					return IERR_SYSTEM;
				}
			}
			if(ReadAbortConditions()){
				HeadDCMotorStopp;
				return IERR_PROG_ABORT;
			}
		}
		// Anfahren des elektrischen Kontaktes (mit Kopf)
		Wait(30);	// Abbremszeit
		while(HeadConnectionPin == 1){	// solange bis Kontakt geschlossen
			WDO();
			HeadDCMotorDown;
			if(IsHeadLowerPos()){	// Untere Endlage erreicht,aber kein elektrischer Kontakt!
				HeadDCMotorStopp;
				printf("kein elektrischer Kontakt zum Kopf vorhanden!\r"); fflush(stdout);
				return IERR_HEAD_NOT_CONNECTED;
			}
			if(ReadAbortConditions()){
				HeadDCMotorStopp;
				return IERR_PROG_ABORT;
			}
		}
		HeadDCMotorStopp;
		printf("elektrischer Kontakt zum Kopf vorhanden!\r"); fflush(stdout);
		return CB_READY;
	}else{
		 return IERR_COM_NOT_PRACTICABLE;
	}
}

/* FUNCTION: 	CmdDevSetHeadSafety
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Flags Lichtvorhang
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdDevSetHeadSafety(void)
{ubyte help;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = (ubyte)atoi(g_pchCmdBuf);
  if((help == WORKING_WITH_LS)||(help == WORKING_WITH_BLIND)||(help == WORKING_WITH_TC))
  g_ubytSafetyHead = help;
  else return IERR_PARA;
  return EEPROMSetHeadSafety(g_ubytSafetyHead);
}

/* FUNCTION: 	CmdDevReadLS
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Flags Lichtvorhang
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdDevReadLS(void)
{ubyte state;
  state = EEPROMReadHeadSafety(); if(state != CB_READY) return state;
  sprintf(g_acAnswerBuf,"I%u\r",(unsigned int)g_ubytSafetyHead);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdReadWay
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Wagenposition in 1/10 mm
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdReadWay(void)
{
  if(g_pudtHrzPosTabPtr->cartype == DISK_10)
	sprintf(g_acAnswerBuf,"I%d\r",(int)CarReadWayPos());
  else
	sprintf(g_acAnswerBuf,"I%d\r",(int)(CarReadWayPos() * (-10)));
  return INFO_FLAG;
}

/* FUNCTION: 	CmdDevReadShieldWatch
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Flags Jaloisieüberwachung
   RETURNS: 	INFO_FLAG / Fehlerkode */
ubyte CmdDevReadShieldWatch(void)
{ubyte state;
  state = EEPROMReadShieldWatchFlag(); if(state != CB_READY) return state;
  sprintf(g_acAnswerBuf,"I%u\r",(unsigned int)g_ubytShieldWatch);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdDevSetShieldWatch
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Flags zur Jaloisieüberwachung
   RETURNS: 	CB_READY / Fehlerkode */
ubyte CmdDevSetShieldWatch(void)
{ubyte help;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = (ubyte)atoi(g_pchCmdBuf);
  if((help == WORKING_WITH_SHIELD)||(help == WORKING_WITHOUT_SHIELD))  g_ubytShieldWatch = help;
  else return IERR_PARA;
  return EEPROMSetShieldWatchFlag(g_ubytShieldWatch);
}

/* FUNCTION: 	CmdDevSetXYCnt
   PARAMETERS:	---
   DESCRIPTION:	Setzen des XY-Zyklenzählers
   RETURNS: 	CB_READY  */
ubyte CmdDevSetXYCnt(void)
{
  ubyte state;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  g_udtLifePara.ulngXYCycles = atol(g_pchCmdBuf);
  state = EEPROMWriteLifePara(&g_udtLifePara); if(state != CB_READY) return state;
  return CB_READY;
}

/* FUNCTION: 	CmdDevReadXYCnt
   PARAMETERS:	---
   DESCRIPTION:	Lesen des XY-Zyklenzählers
   RETURNS: 	CB_READY  */
ubyte CmdDevReadXYCnt(void)
{
  sprintf(g_acAnswerBuf,"I%lu\r",g_udtLifePara.ulngXYCycles);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdDevSetUniServoPara
   PARAMETERS:	---
   DESCRIPTION:	Setzen der Uniadapter-Servo Korrekturparameter
   RETURNS: 	CB_READY,IERR_NOT_ENOUGH_PARA,IERR_PARA,IERR_EEPROM_WRITE  */
ubyte CmdDevSetUniServoPara(void)
{int gain,offset;
  NextPara(); gain = atoi(g_pchCmdBuf);
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  offset = atoi(g_pchCmdBuf);
  if(!UNIServoCheckPara(gain,offset)) return IERR_PARA;
  g_udtUniServoRawAdjPara.intUniGain = gain; g_udtUniServoRawAdjPara.intUniOffset = offset;
  return EEPROMSetUniAdapterPara(&g_udtUniServoRawAdjPara);
}

/* FUNCTION: 	CmdDevReadUniServoPara
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Uniadapter-Servo Korrekturparameter
   RETURNS: 	INFO_FLAG  */
ubyte CmdDevReadUniServoPara(void)
{
  sprintf(g_acAnswerBuf,"I%d %d\r",g_udtUniServoRawAdjPara.intUniGain,g_udtUniServoRawAdjPara.intUniOffset);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdGoDrivePos
   PARAMETERS:	---
   DESCRIPTION:	Setzen der Uniadapter-Servo Position (+-50Grad)
   RETURNS: 	CB_READY / IERR_PARA,IERR_NOT_ENOUGH_PARA */
ubyte CmdGoDrivePos(void)
{ubyte state; float way;
  NextPara(); state = IERR_NOT_ENOUGH_PARA;
  while(*g_pchCmdBuf)
  {
	switch(atoi(g_pchCmdBuf))
	{case DRV_IDENT_SERVO: NextPara(); if(!*g_pchCmdBuf)state = IERR_NOT_ENOUGH_PARA; else state = UNIServoAngle(atof(g_pchCmdBuf)); break;
	  case DRV_IDENT_X:		NextPara();
	  						if(!*g_pchCmdBuf) state = IERR_NOT_ENOUGH_PARA;
	  						else
	  						{way = -atof(g_pchCmdBuf); if(g_pudtHrzPosTabPtr->cartype == DISK_10) way *= -1;	//Kompatibilität zum Disk
	  						  if(CarWayCheck(way) ==  FALSE) state = IERR_PARA; else state = HrzDrvWay(way);}
	  						break;
	  case DRV_IDENT_P:	 NextPara();
		  					if(!*g_pchCmdBuf) state = IERR_NOT_ENOUGH_PARA;
		  					else
		  					{
		  						if(g_pudtHeadType->type == OL3387_8) state = IERR_COM_NOT_PRACTICABLE; 	// Hummingwell-Kopf
		  						else state = PstDrvWay((float)atof(g_pchCmdBuf));
		  					}
		  					break;

	  default:				state = IERR_PARA;
	}
	if(state != CB_READY) break;
	NextPara();
  }
  return state;
}

/* FUNCTION: 	CmdReadDrivePos
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Uniadapter-Servo Position
   RETURNS: 	INFO_FLAG / IERR_PARA,IERR_TO_MANY_PARA,IERR_NOT_ENOUGH_PARA */
ubyte CmdReadDrivePos(void)
{ubyte state; char strHelp[20];
  sprintf(g_acAnswerBuf,"I"); strHelp[0] = 0;
  NextPara(); state = IERR_NOT_ENOUGH_PARA;
  while(*g_pchCmdBuf)
  {
	switch(atoi(g_pchCmdBuf))
	{case DRV_IDENT_SERVO: sprintf(strHelp,"%u ",(unsigned int)DRV_IDENT_SERVO); strcat(g_acAnswerBuf,strHelp);
	 						sprintf(strHelp,"%2.2f ",g_fUNIServoActAngle); strcat(g_acAnswerBuf,strHelp); state = CB_READY; break;

	  case DRV_IDENT_X:		sprintf(strHelp,"%u ",(unsigned int)DRV_IDENT_X); strcat(g_acAnswerBuf,strHelp);
							sprintf(strHelp,"%3.2f ",XReadPos()); strcat(g_acAnswerBuf,strHelp); state = CB_READY; break;

	  case DRV_IDENT_P:		sprintf(strHelp,"%u ",(unsigned int)DRV_IDENT_P); strcat(g_acAnswerBuf,strHelp);
							sprintf(strHelp,"%3.4f ",(double)PstReadWay()); strcat(g_acAnswerBuf,strHelp);
							state = CB_READY; break;

	  default:				state = IERR_PARA;
	}
	if(strlen(g_acAnswerBuf) > ANSWER_BUF_LEN-12) state = IERR_TO_MANY_PARA;	// Verhinderung des Bufferüberlaufes	mit 12 bit Reserve
	if(state != CB_READY) break;
	NextPara();
  }
  g_acAnswerBuf[strlen(g_acAnswerBuf)-1] = 0;	// Letztes Leerzeichen ausblenden
  strcat(g_acAnswerBuf,"\r");
  if(state != CB_READY) return state;
  return INFO_FLAG;
}

/* FUNCTION: 	CmdReadPMFlag
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Präsentiermechanik-Flags
   RETURNS: 	INFO_FLAG / IERR_EEPROM_READ */
ubyte CmdDevReadPMFlag(void)
{ubyte state;
  state = EEPROMReadPMFlag(); if(state == IERR_EEPROM_READ) return state;
  sprintf(g_acAnswerBuf,"I%u\r",(unsigned int)usIsPresMechConfig);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdDevSetPMFlag
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Präsentiermechanik-Flags
   RETURNS: 	CB_READY / Fehler */
ubyte CmdDevSetPMFlag(void)
{ubyte help,state;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = (ubyte)atoi(g_pchCmdBuf);
  if((help == 0) || (help == 1))
  {state = EEPROMWritePMFlag(help);
	usIsPresMechConfig = help;
  }
  else state = IERR_PARA;
  return state;
}

/* FUNCTION: 	CmdDevSetMagazineMode
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Magazin-Kopfwechsel-Flag
				1 - am Ende des Kopfwechselzyklus wird kein Magazin eingesetzt
				0 - am Ende des Kopfwechselzyklus wird ein Magazin eingesetzt(Normalmode)
   RETURNS: 	CB_READY / Fehler */
ubyte CmdDevSetMagazineMode(void)
{ubyte help,state;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = (ubyte)atoi(g_pchCmdBuf);
  if((help == 0) || (help == 1))
  {state = EEPROMWriteMagazineMode(help);
	g_ubytMagNotInsertAfterHeadExchange = help;
  }
  else state = IERR_PARA;
  return state;
}

/* FUNCTION: 	CmdDevReadMagazineMode
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Magazin-Kopfwechsel-Flag
   RETURNS: 	INFO_FLAG / IERR_EEPROM_READ */
ubyte CmdDevReadMagazineMode(void)
{ubyte state;
  state = EEPROMReadMagazineMode(); if(state == IERR_EEPROM_READ) return state;
  sprintf(g_acAnswerBuf,"I%u\r",(unsigned int)g_ubytMagNotInsertAfterHeadExchange);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdDevReadXYSwitchPos
   PARAMETERS:	---
   DESCRIPTION:	Lesen der min. Aushebers-Position zur XY-Steuerung
   RETURNS: 	INFO_FLAG / IERR_EEPROM_READ */
ubyte CmdDevReadXYSwitchPos(void)
{ubyte state;
  state = EEPROMReadAndSetVerticalSwitchPos(); if(state == IERR_EEPROM_READ) return state;
  sprintf(g_acAnswerBuf,"I%4.2f\r",g_fVertXYSwitchPos);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdDevSetXYSwitchPos
   PARAMETERS:	---
   DESCRIPTION:	Setzen der min. Aushebers-Position zur XY-Steuerung
   RETURNS: 	INFO_FLAG / IERR_EEPROM_READ */
ubyte CmdDevSetXYSwitchPos(void)
{float help;
  ubyte state;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = atof(g_pchCmdBuf);
  if((help > DZ_MAX) && (help < g_fVertMaxWay))
  {state = EEPROMWriteVerticalSwitchPos(help);
	if(state == CB_READY) g_fVertXYSwitchPos = help;
  }
  else state = IERR_PARA;
  return state;
}

/* FUNCTION: 	CmdDevSetKeyboardLayout
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Tatstaturlayouts
   RETURNS: 	CB_READY / Fehler */
ubyte CmdDevSetKeyboardLayout(void)
{ubyte help;
  ubyte state;
  NextPara(); if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
  help = atoi(g_pchCmdBuf);
  if((help == KEYBOARD_LAYOUT_DEFAULT) || (help == KEYBOARD_LAYOUT_BEHIND_DEV))
  {state = EEPROMWriteKeyboardLayout(help);
	if(state == CB_READY) g_ubytKeyboardLayout = help;
  }
  else state = IERR_PARA;
  return state;
}

/* FUNCTION: 	CmdDevReadKeybordProp
   PARAMETERS:	---
   DESCRIPTION:	Modifikation des Tasten-Layouts
   RETURNS: 	INFO_FLAG / IERR_EEPROM_READ */
ubyte CmdDevReadKeyboardLayout(void)
{ubyte state;
  state = EEPROMReadKeyboardLayout(); if(state == IERR_EEPROM_READ) return state;
  sprintf(g_acAnswerBuf,"I%d\r",g_ubytKeyboardLayout);
  return INFO_FLAG;
}

/* FUNCTION: 	CmdSetVolumeCorr
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Zustandes der Volumenkorrektur (intern/extern)
   RETURNS:		CB_READY / IERR_PARA */
ubyte CmdSetVolumeCorr(void)
{
	enum VOLUME_CORRECTION vc;
	ubyte state;

	NextPara();
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	state = IERR_PARA;
	vc = atoi(g_pchCmdBuf);
	if((vc == VC_INTERN)||(vc == VC_EXTERN))
	{
		g_VolumeCorrection = vc;
		state = CB_READY;
	}
	return state;
}

/* FUNCTION: 	CmdGetVolumeCorr
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Zustandes der Volumenkorrektur (intern/extern)
   RETURNS:		INFO_FLAG */
ubyte CmdGetVolumeCorr(void)
{
	sprintf(g_acAnswerBuf,"I%u\r",(unsigned int)g_VolumeCorrection);
	return INFO_FLAG;
}

/* FUNCTION: 	CmdSetOffset
   PARAMETERS:	Id,Offset
   DESCRIPTION:	Speicher des Zusatzvolumens (eine Art Offset)
   RETURNS:		CB_READY */
ubyte CmdSetOffset(void)
{
	float offset;
	unsigned char Id,state;
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	state = CB_READY;
	NextPara();
	Id = *g_pchCmdBuf;
	NextPara();
	if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
	offset = atof(g_pchCmdBuf);
	switch(Id)
	{
			case 'v':	FRAMSetAddVolume(offset);
						g_AddVolume = offset;
						break;
			default:	state = IERR_PARA;
	}
	return state;
}

/* FUNCTION: 	CmdGetOffset
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Zusatzvolumens
   RETURNS:		Info / Error */
ubyte CmdGetOffset(void)
{
	float offset;
	unsigned char Id,state;
	state = INFO_FLAG;
	NextPara();
	Id = *g_pchCmdBuf;
	switch(Id)
	{
		case 'v':	offset = g_AddVolume;	// tatsächlichen Wert lesen!
					break;
		default:	state = IERR_PARA;
	}
	if(state == INFO_FLAG)
		sprintf(g_acAnswerBuf,"I%4.2f\r",offset);
	return state;
}

/* FUNCTION: 	CmdSetTipNo
   PARAMETERS:	---
   DESCRIPTION:	Setzen eines Spitzentypes
   RETURNS:		CB_READY
ubyte CmdSetTipNo(void)
{
	ubyte state;
	unsigned int tip_id;
	state = IERR_PARA;
	if(PstReadEccentricState() == PST_ECCENTRIC_TIGHTEN)
	{
		if(!*g_pchCmdBuf) return IERR_NOT_ENOUGH_PARA;
		tip_id = atoi(g_pchCmdBuf);
		state = SetTipNo(tip_id);
		if(state == CB_READY)
			FRAMSetTipId(tip_id);
	}
	else
		state = IERR_COM_NOT_PRACTICABLE;
	return state;
} */

/* FUNCTION: 	CmdGetTipNo
   PARAMETERS:	---
   DESCRIPTION:	Lesen eines Spitzentypes
   RETURNS:		CB_READY
ubyte CmdGetTipNo(void)
{
	sprintf(g_acAnswerBuf,"I%u\r",GetTipNo());
	return INFO_FLAG;
}
*/

/////////////////////////////////////////////////////////////////////////////////
/// \brief                  Triggers a software reset request
/// \return                 CB_READY to indicate the command was accepted
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t CmdSetSoftwareReset (void)
{
	SetResetRequest();
	return CB_READY;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Manages various operations on the head FRAM content based on
///                 command parameters. Supports reading structure version, used
///                 size, deleting content, reading and writing data in hex format.
/// \return         CB_READY if successful,
///                 INFO_FLAG if informational response is generated,
///                 IERR_PARA or IERR_NOT_ENOUGH_PARA for parameter errors,
///                 or other error codes from underlying operations
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t CmdManageHeadFRAMContent(void)
{
	ErrorState_t state= CB_READY;
	char* token= NULL;
	stre_split_vars tokenizer= {0};


	//Remove line feed '\n' n command buffer
	token = strrchr(g_pchCmdBuf, END_CHAR);
	if(token != NULL)
		*token = 0;
	NextPara();
	//Check parameter
	token= stre_split(g_pchCmdBuf, SEP_WHITESPACE, &tokenizer);
	if(token != NULL)
	{
		//Get FRAM structure version
		if(STRE_EQUAL(token,IDENTIFIER_FRAM_VERSION))
		{
			uint8_t value;

			free(token);
			state= EEPROMReadStructVersion(&value);
			if(state == CB_READY)
			{
				sprintf(g_acAnswerBuf,"I%u\r",value);
				state= INFO_FLAG;
			}
		}
		//Get FRAM used size
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_USED_SIZE))
		{
			free(token);
			sprintf(g_acAnswerBuf,"I%lu\r", FRAMGetSizeInByte());
			state= INFO_FLAG;
		}
		//Delete FRAM content
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_DELETE))
		{
			free(token);
			state=FRAMClearAllAndCmp();
		}
		//Read FRAM content
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_READ))
		{
			free(token);
			token= stre_split(NULL, SEP_WHITESPACE, &tokenizer);
			if(token != NULL)
			{
				uint16_t address;
				uint16_t length;

				if(STRE_EQUAL(token,IDENTIFIER_FRAM_HEX))
				{
					free(token);
					state= CmdEEPROMCheckReadPara(SRC_HEAD_FRAM, &tokenizer,&address,&length);
					if(state == CB_READY)
					{
						state= CmdEEPROMReadDataToBinaryString(SRC_HEAD_FRAM, address, length, g_acAnswerBuf);
					}
				}
				else
				{
					free(token);
					state= IERR_PARA;
				}
			}
			else
				state= IERR_NOT_ENOUGH_PARA;
		}
		//Write FRAM content
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_WRITE))
		{
			free(token);
			token= stre_split(NULL, SEP_WHITESPACE, &tokenizer);
			if(token != NULL)
			{
				uint16_t address;
				uint16_t length;

				if(STRE_EQUAL(token,IDENTIFIER_FRAM_HEX))
				{
					char* pData = NULL;

					free(token);
					state= CmdEEPROMCheckWritePara(SRC_HEAD_FRAM, &tokenizer,&address,&length,&pData);
					if(state == CB_READY)
					{
						state= CmdEEPROMWriteDataFromBinaryString(SRC_HEAD_FRAM, address, length, pData);
					}
					if(pData != NULL)
						free(pData);
				}
				else
				{
					free(token);
					state= IERR_PARA;
				}
			}
			else
				state= IERR_NOT_ENOUGH_PARA;
		}
		else
		{
			free(token);
			state= IERR_PARA;
		}
	}
	else
		state= IERR_NOT_ENOUGH_PARA;
	return state;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Manages various operations on the device EEPROM content based
///                 on command parameters. Supports reading structure version,
///                 used size, deleting content, reading and writing data in hex format.
/// \return         CB_READY if successful,
///                 INFO_FLAG if informational response is generated,
///                 IERR_PARA or IERR_NOT_ENOUGH_PARA for parameter errors,
///                 or other error codes from underlying operations
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t CmdManageDevEEPROMContent(void)
{
	ErrorState_t state= CB_READY;
	char* token= NULL;
	stre_split_vars tokenizer= {0};


	//Remove line feed '\n' n command buffer
	token = strrchr(g_pchCmdBuf, END_CHAR);
	if(token != NULL)
		*token = 0;
	NextPara();
	//Check parameter
	token= stre_split(g_pchCmdBuf, SEP_WHITESPACE, &tokenizer);
	if(token != NULL)
	{
		//Get FRAM structure version
		if(STRE_EQUAL(token,IDENTIFIER_FRAM_VERSION))
		{
			uint8_t value;

			free(token);
			state= EEPROMReadStructVersion(&value);
			if(state == CB_READY)
			{
				sprintf(g_acAnswerBuf,"I%u\r",value);
				state= INFO_FLAG;
			}
		}
		//Get FRAM used size
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_USED_SIZE))
		{
			free(token);
			sprintf(g_acAnswerBuf,"I%lu %lu\r",EEPROMGetStructureSizeInByte(), EEPROMGetSizeInByte());
			state= INFO_FLAG;
		}
		//Delete FRAM content
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_DELETE))
		{
			free(token);
			state=EEPROMClearAllAndCmp();
		}
		//Read FRAM content
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_READ))
		{
			free(token);
			token= stre_split(NULL, SEP_WHITESPACE, &tokenizer);
			if(token != NULL)
			{
				uint16_t address;
				uint16_t length;

				if(STRE_EQUAL(token,IDENTIFIER_FRAM_HEX))
				{
					free(token);
					state= CmdEEPROMCheckReadPara(SRC_DEV_EEPROM, &tokenizer,&address,&length);
					if(state == CB_READY)
					{
						state= CmdEEPROMReadDataToBinaryString(SRC_DEV_EEPROM, address, length, g_acAnswerBuf);
					}
				}
				else
				{
					free(token);
					state= IERR_PARA;
				}
			}
			else
				state= IERR_NOT_ENOUGH_PARA;
		}
		//Write FRAM content
		else if	(STRE_EQUAL(token,IDENTIFIER_FRAM_WRITE))
		{
			free(token);
			token= stre_split(NULL, SEP_WHITESPACE, &tokenizer);
			if(token != NULL)
			{
				uint16_t address;
				uint16_t length;

				if(STRE_EQUAL(token,IDENTIFIER_FRAM_HEX))
				{
					char* pData = NULL;

					free(token);
					state= CmdEEPROMCheckWritePara(SRC_DEV_EEPROM, &tokenizer,&address,&length,&pData);
					if(state == CB_READY)
					{
						state= CmdEEPROMWriteDataFromBinaryString(SRC_DEV_EEPROM, address, length, pData);
					}
					if(pData != NULL)
						free(pData);
				}
				else
				{
					free(token);
					state= IERR_PARA;
				}
			}
			else
				state= IERR_NOT_ENOUGH_PARA;
		}
		else
		{
			free(token);
			state= IERR_PARA;
		}
	}
	else
		state= IERR_NOT_ENOUGH_PARA;
	return state;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief 					Function checks read parameter for EEPROM access
/// \param[in] *pTokenizer  Pointer to command parameter string
/// \param[out] *pAddress  	Pointer to EEPROM address
/// \param[out] *pLength  	Pointer to data length
/// \param[in] eeprom_size  Specifies the the size of the eeprom in bytes
///	\return 				CB_READY - OK / ERRORCODE
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t CmdEEPROMCheckReadPara(const EEPROMSource_t source, stre_split_vars * const pTokenizer, uint16_t * const pAddress, uint16_t *pLength)
{
	ErrorState_t state= CB_READY;
	char* token= NULL;
	bool success;

	token= stre_split(NULL, SEP_WHITESPACE, pTokenizer);
	if(token != NULL)
	{
		success= math_convert_hex_cstring_to_uint16(token,pAddress);
		free(token);
		if(success)
		{
			if(*pAddress < GetEEPROMSize(source))
			{
				token= stre_split(NULL, SEP_WHITESPACE, pTokenizer);
				if(token != NULL)
				{
					success= math_convert_hex_cstring_to_uint16(token,pLength);
					free(token);
					if(success)
					{
						if(*pLength > EEPROM_MAX_BLOCK_SIZE)
						{
							state= IERR_PARA;
						}
					}
				}
				else
					state= IERR_NOT_ENOUGH_PARA;
			}
			else
				state= IERR_PARA;
		}
		else
			state= IERR_PARA;
	}
	else
		state= IERR_NOT_ENOUGH_PARA;
	return state;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Parses and validates EEPROM write parameters from a command
///                 tokenizer. Extracts address, length, and data string.
/// \param[in]      source      EEPROM source identifier
/// \param[in]      pTokenizer  Pointer to tokenizer structure containing command tokens
/// \param[out]     pAddress    Pointer to variable where parsed EEPROM address will be stored
/// \param[out]     pLength     Pointer to variable where parsed data length will be stored
/// \param[out]     pData       Pointer to string containing the binary data to write
/// \return         CB_READY if parameters are valid,
///                 IERR_PARA for invalid parameters,
///                 IERR_NOT_ENOUGH_PARA if parameters are missing
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t CmdEEPROMCheckWritePara(const EEPROMSource_t source, stre_split_vars * const pTokenizer, uint16_t * const pAddress, uint16_t * const pLength, char ** const pData)
{
	ErrorState_t state = CB_READY;
	char* token = NULL;
	bool success;

	// Adresse lesen
	token = stre_split(NULL, SEP_WHITESPACE, pTokenizer);
	if (token != NULL)
	{
		success = math_convert_hex_cstring_to_uint16(token, pAddress);
		free(token);
		if (success && *pAddress < GetEEPROMSize(source))
		{
			// Länge lesen
			token = stre_split(NULL, SEP_WHITESPACE, pTokenizer);
			if (token != NULL)
			{
				success = math_convert_hex_cstring_to_uint16(token, pLength);
				free(token);
				if (success && *pLength <= EEPROM_MAX_BLOCK_SIZE)
				{
					// Datenstring lesen
					token = stre_split(NULL, SEP_WHITESPACE, pTokenizer);
					if (token != NULL)
					{
						// Validierung: Länge des Strings muss exakt 2 * Länge sein
						size_t expected_len = 2 * (*pLength);
						if (strlen(token) >= expected_len)
						{
							token[expected_len] = '\0';  // Abschneiden auf exakte Länge
							*pData = token;
						}
						else
						{
							free(token);
							state = IERR_PARA;
						}
					}
					else
					state = IERR_NOT_ENOUGH_PARA;
				}
				else
				state = IERR_PARA;
			}
			else
			state = IERR_NOT_ENOUGH_PARA;
		}
		else
		state = IERR_PARA;
	}
	else
		state = IERR_NOT_ENOUGH_PARA;

	return state;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Reads a block of data from EEPROM and converts it into a
///                 binary string format with a prefix and hexadecimal representation.
/// \param[in]      source          EEPROM source identifier
/// \param[in]      eeprom_address  Start address in EEPROM to read from
/// \param[in]      length          Number of bytes to read
/// \param[out]     pAnswer         Pointer to output string buffer where the
///                                 binary string will be written
/// \return         CB_READY if successful,
///                 INFO_FLAG if data was read and formatted
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t CmdEEPROMReadDataToBinaryString(const EEPROMSource_t source, const uint32_t eeprom_address, const uint16_t length, char* pAnswer)
{
	ErrorState_t state = CB_READY;
	uint8_t array[EEPROM_READ_SIZE] = {0};
	uint32_t address = eeprom_address;
	uint16_t remaining = length;
	char* pWrite = pAnswer;

	// Schreibe Prefix "I" + Länge in Hex + Leerzeichen
	pWrite += sprintf(pWrite, "I%03x ", length);

	while (remaining > 0 && state == CB_READY)
	{
		uint16_t chunk_size = (remaining > EEPROM_READ_SIZE) ? EEPROM_READ_SIZE : remaining;
		state = ReadFromEEPROM(source, address, array, chunk_size);
		if (state == CB_READY)
		{
			for (uint16_t i = 0; i < chunk_size; i++)
			{
				pWrite += sprintf(pWrite, "%02x", array[i]);
			}
				address += chunk_size;
				remaining -= chunk_size;
			}
	}

	if (state == CB_READY)
	{
		*pWrite++ = '\r';  // Zeilenende
		*pWrite = '\0';    // Nullterminator
		state = INFO_FLAG;
	}

	return state;
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief 					Function writes binary string (hex) to adaptor EEPROM
/// \param[in] fram_address Specifies the EEPROM address
/// \param[in] length  		Specifies the data length
/// \param[in] *pDataHex  	Pointer to data string (hex)
///	\return 				CB_READY - OK / ERRORCODE
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t CmdEEPROMWriteDataFromBinaryString(const EEPROMSource_t source, const uint32_t eeprom_address, const uint16_t length, const char* const pDataHex)
{
	ErrorState_t state = CB_READY;

	//Check size (fram_adress + length)
	if((eeprom_address + length) <= GetEEPROMSize(source))
	{
		//Check string length
		size_t temp= strlen(pDataHex);
		if(temp != (2*length))
			state= IERR_PARA;
	}
	else
		state= IERR_PARA;

	if(state == CB_READY)
	{
		const char* pHexString= pDataHex;
		uint8_t array[EEPROM_MAX_BLOCK_SIZE]={0};
		char str_array[3]= {0};
		int pos;

		for(pos=0; pos< length; pos++)
		{
			memcpy(str_array,pHexString,2);
			if(!math_convert_hex_cstring_to_uint8(str_array,&(array[pos])))
			{
				state= IERR_PARA;
				break;
			}
			pHexString+=2;
		}
		if(state == CB_READY)
		{
			state= WriteToEEPROM(source, eeprom_address, array, length);
		}
	}
	return state;
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief 				Get EEPROM size in bytes
/// \param[in] source	Specifies the EEPROM source
///	\retval size		EEPROM size in bytes
/////////////////////////////////////////////////////////////////////////////////
static uint32_t GetEEPROMSize (const EEPROMSource_t source)
{
	uint32_t size = 0;

	if(source == SRC_DEV_EEPROM)
		size = EEPROMGetSizeInByte();
	else if(source == SRC_HEAD_FRAM)
		size = FRAMGetSizeInByte();
	return size;
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief 					Write to EEPROM
/// \param[in] source		Specifies the EEPROM source
/// \param[in] address 		Specifies the EEPROM address
/// \param[in] *pData	  	Pointer to data to be written
/// \param[in] data_length 	Specifies the data length
///	\retval state			CB_READY - OK / ERRORCODE
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t WriteToEEPROM (const EEPROMSource_t source, const uint8_t address, const uint8_t * const pData, const uint16_t data_length)
{
	ErrorState_t state = IERR_SYSTEM;

	if(source == SRC_DEV_EEPROM)
	{
		state= EEPROMWrite(address, pData, data_length);

	}
	else if(source == SRC_HEAD_FRAM)
	{
		state= FRAMWrite(address,pData,data_length);
	}
	return state;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief 					Read from EEPROM
/// \param[in] source		Specifies the EEPROM source
/// \param[in] address 		Specifies the EEPROM address
/// \param[in] *pData	  	Pointer to data to be read
/// \param[in] data_length 	Specifies the data length
///	\retval state			CB_READY - OK / ERRORCODE
/////////////////////////////////////////////////////////////////////////////////
static ErrorState_t ReadFromEEPROM (const EEPROMSource_t source,const uint8_t address,  uint8_t * const pData, const uint16_t data_length)
{
	ErrorState_t state = IERR_SYSTEM;

	if(source == SRC_DEV_EEPROM)
	{
		state= EEPROMRead(address, pData, data_length);
	}
	else if(source == SRC_HEAD_FRAM)
	{
		state= FRAMRead(address,pData,data_length);
	}
	return state;
}

