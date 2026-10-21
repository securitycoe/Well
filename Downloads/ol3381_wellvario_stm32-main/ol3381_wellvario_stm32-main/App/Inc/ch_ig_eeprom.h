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
#include "ch_ig_xy.h"
#include "ch_ig_piston.h"
#include "ch_ig_utility.h"
#include "ch_ig_config.h"

#define EEPROM_SIZE 512
#define EEPROM_PHYSICAL_SIZE 8000 //64k EEprom: max 8kbyte addr. CAT25640VI-GT3 - dont want to clear, read, init all the 64k

/* Historie: STRUCT_VERSION_EEPROM = 1 start up
			 STRUCT_VERSION_EEPROM = 2 Erweiterung durch float fVerticalSwitchPos
*/
 #define STRUCT_VERSION_EEPROM 3	// Änderung nur durch den Entwickler möglich!!!
 #define EEPROM_TRUE 0 				// Hartmuts neue Definition der Richtigkeit
#define EEPROM_MAX_WRITE_CYCLES 800000 //Maximale Schreibzyklen an einer Stelle.


// EEPROM auf den TQ-Modul
struct EEPROM{
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
  struct UNIAdj udtUniServoAdjustPara;	// Parameter für den Servo zur Steuerung des Universaladapters
  ubyte ubytPMFlag;					// Konfiguration Präsentiermechanik 0	 = > nicht vorhanden
  ubyte ubytMagazineMode;		  	// 1 -> kein Magazin einsetzen am Ende des Kopfwechsels,0 --> Magazin einsetzen ...
  float fVerticalSwitchPos;			// Umschalthöhe des Aushebers in 1/10mm für XY-Bewegung
  ubyte ubytKeyboardLayout;			// Konfiguration des Tastaturlayouts
  struct REV_PARA HrzMotPara;		// Motorkennlinie des Horizontal-Antriebes (Wagen)
  char HeadTypeNo[14];				// Kopfnummer
  uint32_t LifeParaOffset;			// Position der Lifeparameter
  struct LIFE_PARA LifePara;		// Life-Parameter
};

//****************************************************************************
// @Prototypes of global functions
//****************************************************************************
extern uint32_t g_lifePos;
ubyte EEPROMClearAll(void);
ubyte EEPROMSetToZero(void);
ubyte EEPROMClearAllAndCmp(void);
ubyte EEPROMSetStructVersion(void);
ubyte EEPROMReadStructVersion(ubyte *vers);
uint32_t EEPROMGetStructureSizeInByte(void);
uint32_t EEPROMGetSizeInByte(void);
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
ubyte EEPROMSetUniAdapterPara(struct UNIAdj *udtPtr);
ubyte EEPROMReadUniAdapterPara(struct UNIAdj *udtPtr);
ubyte EEPROMReadPMFlag(void);
ubyte EEPROMWritePMFlag(ubyte flg);
ubyte EEPROMReadMagazineMode(void);
ubyte EEPROMWriteMagazineMode(ubyte flg);
ubyte EEPROMReadAndSetVerticalSwitchPos(void);
ubyte EEPROMWriteVerticalSwitchPos(float pos);
ubyte EEPROMReadKeyboardLayout(void);
ubyte EEPROMWriteKeyboardLayout(ubyte layout);
ubyte EEPROMSetHrzMotPara(const struct REV_PARA *scr);
ubyte EEPROMReadHrzMotPara(struct REV_PARA *dst);
ubyte EEPROMReadHeadTypeNumber(char *sptr);
ubyte EEPROMSetHeadTypeNumber(const char *sptr);
ubyte EEPROMReadLifePara(struct LIFE_PARA *life);
ubyte EEPROMWriteLifePara(struct LIFE_PARA *life);
ubyte EEPROMReadLifeParaPosition(uint32_t *lifePos);
ubyte EEPROMWriteLifeParaPosition(uint32_t *lifePos);
ErrorState_t EEPROMRead(const uint8_t address,  uint8_t * const pData, const uint16_t data_length);
ErrorState_t EEPROMWrite(const uint8_t address, const uint8_t * const pData, const uint16_t data_length);


#endif


