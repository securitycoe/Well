/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

#ifndef __EEPROM__
#define __EEPROM__

#include "main.h"

#include "head_piston_tab.h"	//

#include "ch_ig_xy.h"
#include "ch_ig_piston.h"
#include "ch_ig_utility.h"
#include "ch_ig_config.h"

#define EEPROM_SIZE 512
#define STRUCT_VERSION_EEPROM 1	//Änderung nur durch den Entwickler möglich!!!
#define EEPROM_TRUE 0 			// Hartmuts neue Definition der Richtigkeit

struct EEPROM
{
  char StructVersNo;
  char DeviceNr[14];				// Gerätenummer
  ubyte bytLang;					// Index für Sprachauswahl
  float DispWaitTime; 				// Wartezeit beim manuellen Liquidhandling in Sekunden
  float VertHeightCorr;				// Höhenkorrekturwert des Aushebers
  float VertManLimitWay;			// Wegbegrenzung des Aushebers im manuellen Mode
  struct REV_PARA PstManMotPara;	// Motorkennlinie des Kolbenantriebes im manuellen Mode
  struct REV_PARA VertManMotPara;	// Motorkennlinie des Z-Antriebes im manuellen Mode
  struct DEV_TYPE *udtActDeviceType;// XY-Modeauswahl
  ubyte bytBeepState;				// Klick
  struct XYAdj udtServoAdjustPara;	// XY-Justierparameter
  ulong ulngHeadExchange;			// Kopfwechsel am Gerät
  ubyte ubytASPWFlag;				// Kennung nachfolgende Parameter sind aktuell
  struct TIPS_PARA udtTipsPara;		// über den ASPW definierte Spitzenparameter
  ubyte bytHostModeState;			// Arbeitsmode (Manuell oder Hostmode)
  ubyte bytCarrType;				// 0--> kein Wagen,4 --> 4 Plätze,Erweiterungen denkbar!
  ubyte bytSafetyHead;				// geändert: bytSafetyLight Betrieb mit Lichtvorhang
  ubyte bytShieldWatch;				// Steuerung der Jaloisieüberwachung
};

//****************************************************************************
// @Prototypes of global functions
//****************************************************************************
ubyte EEPROMClearAll(void);
ubyte EEPROMSetToZero(void);
ubyte EEPROMClearAllAndCmp(void);
ubyte EEPROMSetStructVersion(void);
ubyte EEPROMReadStructVersion(ubyte *vers);
ubyte EEPROMSetStructVersionAndCmp(void);
ubyte EEPROMReadDevNumber(char *sptr);
ubyte EEPROMSetDevNumber(char *sptr);
ubyte EEPROMReadAndDisplay(void);
ubyte EEPROMSetFlagAndSaveTipsPara(struct TIPS_PARA *ptr);
ubyte EEPROMReadTipsPara(struct TIPS_PARA *udtPtr);
ubyte EEPROMReadASPWFlag(ubyte *flg);
ubyte EEPROMWriteASPWFlag(ubyte flg);
ubyte EEPROMASPWFlagResetAndCmp(void);
ubyte EEPROMASPWFlagCtrl(ubyte flg);
ubyte EEPROMSetLanguage(void);
ubyte EEPROMReadAndCheckLanguage(void);
ubyte EEPROMSetDispWaitTime(void);
ubyte EEPROMReadDispWaitTime(void);
ubyte EEPROMSetVertHeightCorr(void);
ubyte EEPROMReadVertHeightCorr(void);
ubyte EEPROMSetVertManWayLimit(void);
ubyte EEPROMReadVertManWayLimit(void);
ubyte EEPROMSetPstManMotPara(void);
ubyte EEPROMReadPstManMotPara(void);
ubyte EEPROMSetVertManMotPara(void);
ubyte EEPROMReadVertManMotPara(void);
ubyte EEPROMSetBeepState(void);
ubyte EEPROMReadBeepState(void);
ubyte EEPROMSetServoAdjustPara(void);
ubyte EEPROMReadServoAdjustPara(void);
ubyte EEPROMSetServoPlateTypeMode(void);
ubyte EEPROMReadServoPlateTypeMode(struct DEV_TYPE **udtPtr);
ubyte EEPROMSetHostModeFlag(void);
ubyte EEPROMReadHostModeFlag(void);
ubyte EEPROMHostModeFlagResetAndCmp(void);
ubyte EEPROMReadHeadExchangeAtDev(ulong *cnt);
ubyte EEPROMSetHeadExchangeAtDev(ulong *cnt);
ubyte EEPROMSetCarrType(ubyte car);
ubyte EEPROMReadCarrType(void);
ubyte EEPROMSetCarrTypeAndCmp(ubyte car);
ubyte EEPROMSetHeadSafety(ubyte flg);
ubyte EEPROMReadHeadSafety(void);
ubyte EEPROMResetLSAndCmp(void);
ubyte EEPROMSetShieldWatchFlag(ubyte flg);
ubyte EEPROMReadShieldWatchFlag(void);
ubyte EEPROMSetShieldWatchFlagAndCmp(void);
#endif


