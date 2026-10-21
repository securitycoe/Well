//****************************************************************************
// @Module		Serieller EEPROM
// @Filename	  EEPROM.C
// @Project	   ch_ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CS-L
// @Compiler	  TASKING C166
// @Description   This file contains all functions that use the EEPROM on TQM 167U
//
//
//----------------------------------------------------------------------------
// @Date		  17.08.02
//
//****************************************************************************

//****************************************************************************
// @Project Includes
//****************************************************************************
//#include "reent.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Base
// ----------Inc
#include <_spi_eeprom.h>
// ---------- Master/C167CS

// ---------- Master
//#include "ch_ig_cmd.h"						// original excluded
#include "ch_ig_eeprom.h"
#include "ch_ig_head.h"
#include "ch_ig_hrz.h"
#include "ch_ig_key.h"
#include "ch_ig_piston.h"
#include "ch_ig_ptme.h"
#include "ch_ig_spw.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
// ---------- Pipettor_Allgemein/VolumenKorrektur
// ---------- Pipettor_Allgemein
#include "Well_Error.h"

//Read the EEprom position once at start of device
uint32_t g_lifePos;
/* FUNCTION: 	EEPROMClearAll
   PARAMETERS:	---
   DESCRIPTION:	Löschen des gesamten EEPROM's (0x00)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMClearAll(void)
{ubyte state;
	ewen();
	WDO();
	state = eral();
	ewds();
	if(state == EEPROM_TRUE){
		state = EEPROMSetToZero();
		return state;
	}else state = IERR_EEPROM_WRITE;
  return state;
}

/* FUNCTION: 	EEPROMSetToZero
   PARAMETERS:	---
   DESCRIPTION:	Beschreiben des gesamten EEPROM's mit 0x00
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetToZero(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,EEPROM_SIZE);
	state = wr(EEPROM_SIZE,0,(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMClearAllAndCmp - nur für Erstinbetriebnahme
   PARAMETERS:	---
   DESCRIPTION:	Löschen und überprüfung des gesamten EEPROM's (0xff)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMClearAllAndCmp(void){
	ubyte state; int i;
	if((state = EEPROMClearAll()) == CB_READY){
		for(i = 0; i<EEPROM_SIZE; i++){
			WDO();
			if(ReadEEprom(i) != 0x00){
				state = IERR_EEPROM_MEMORY;
				break;
			}
		}
	}
	return state;
}

/* FUNCTION: 	EEPROMSetStructVersion
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der internen Strukturnummer in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetStructVersion(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	ptr->StructVersNo = STRUCT_VERSION_EEPROM;
	state = wr(sizeof(ptr->StructVersNo),offsetof(struct EEPROM,StructVersNo),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetStructVersionAndCmp
   PARAMETERS:	---
   DESCRIPTION:	Setzen und überprüfen der internen Strukturnummer des EEPROM's
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetStructVersionAndCmp(void){
	ubyte state,vers;
	if((state = EEPROMSetStructVersion()) == CB_READY){
		if((state = EEPROMReadStructVersion(&vers)) == CB_READY){
			if(vers != STRUCT_VERSION_EEPROM) return IERR_EEPROM_MEMORY;
		}
	}
	return state;
}

/* FUNCTION: 	EEPROMReadStructVersion
   PARAMETERS:	---
   DESCRIPTION:	Lesen der internen Strukturnummer des EEPROM's
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadStructVersion(ubyte *vers){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->StructVersNo),offsetof(struct EEPROM,StructVersNo),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
	*vers = ptr->StructVersNo;
	free(ptr);
	return state;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Returns the size in bytes of the EEPROM structure.
/// \return         Size of the EEPROM structure in bytes
/////////////////////////////////////////////////////////////////////////////////
uint32_t EEPROMGetStructureSizeInByte(void)
{
	return sizeof(struct EEPROM);
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Returns the size in bytes of the EEPROM.
/// \return         Size of the EEPROM in bytes
/////////////////////////////////////////////////////////////////////////////////
uint32_t EEPROMGetSizeInByte(void)
{
	return EEPROM_SIZE;
}

/* FUNCTION: 	EEPROMReadDevNumber
   PARAMETERS:	Pointer auf ZK-Speicher
   DESCRIPTION:	Lesen der Gerätenummer
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadDevNumber(char *sptr){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(((struct EEPROM *)NULL)->DeviceNr),offsetof(struct EEPROM,DeviceNr),(unsigned char*)ptr);
	if(state == EEPROM_TRUE)
		state = CB_READY;
	else
		state = IERR_EEPROM_READ;
	strncpy(sptr,ptr->DeviceNr,sizeof(((struct EEPROM *)NULL)->DeviceNr));
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetDevNumber
   PARAMETERS:	Pointer auf ZK mit der Gerätenummer
   DESCRIPTION:	Schreiben der Gerätenummer in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetDevNumber(char *sptr){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	if(strlen(sptr) == (sizeof(ptr->DeviceNr) - 1)){
		strcpy(ptr->DeviceNr,sptr);
		state = wr(sizeof(ptr->DeviceNr),offsetof(struct EEPROM,DeviceNr),(unsigned char*)ptr);
		if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	}else state = IERR_PARA;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMReadAndDisplay
   PARAMETERS:	---
   DESCRIPTION:	Lesen und Anzeigen des Speichers
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadAndDisplay(void){
	int j,i;
	j = 0; printf("\rReading EEPROM:");
	for(i = 0; i<2*EEPROM_SIZE;i++){ //TODO: Remove 2* just for debugging
		WDO();
		if((j%10) == 0) printf("\rAdr: %3d ",i);
		printf("%3x",(int)ReadEEprom(i));
		fflush(stdout);
		j++;
	}
	return CB_READY;
}

/* FUNCTION: 	EEPROMReadASPWFlag
   PARAMETERS:	Zeiger auf Zustand des ASPW-Flags
   DESCRIPTION:	Lesen des ASPW-Flags (Kennung Spitzenparameter vom ASPW)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadASPWFlag(ubyte *flg){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->ubytASPWFlag),offsetof(struct EEPROM,ubytASPWFlag),(unsigned char*)ptr);
	if(state == EEPROM_TRUE){
		state = CB_READY;
		*flg = ptr->ubytASPWFlag;
	}else state = IERR_EEPROM_READ;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMWriteASPWFlag
   PARAMETERS:	Flag
   DESCRIPTION:	Schreiben des ASPW-Flags (Kennung Spitzenparameter vom ASPW)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMWriteASPWFlag(ubyte flg)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->ubytASPWFlag = flg;
  state = wr(sizeof(ptr->ubytASPWFlag),offsetof(struct EEPROM,ubytASPWFlag),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMASPWFlagResetAndCmp
   PARAMETERS:	---
   DESCRIPTION:	Rücksetzen und Vergleichen des ASPW-Flags (Kennung Spitzenparameter vom ASPW)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMASPWFlagResetAndCmp(void)
{ubyte state,help;
  if((state = EEPROMWriteASPWFlag(FALSE)) != CB_READY) return state;
  if((state = EEPROMReadASPWFlag(&help)) != CB_READY) return state;
  if(help == FALSE) state = CB_READY; else state = IERR_EEPROM_MEMORY;
  return state;
}

ubyte EEPROMASPWFlagCtrl(ubyte flg)
{ubyte state,help;
  state = EEPROMReadASPWFlag(&help);
  if(state == CB_READY)
  {if(flg != help) state = EEPROMWriteASPWFlag(flg); }
  return state;
}


ubyte EEPROMSetFlagAndSaveTipsPara(struct TIPS_PARA *udtPtr)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	// Lesen der gespeicherten Werte
  state = rd(sizeof(ptr->udtTipsPara),offsetof(struct EEPROM,udtTipsPara),(unsigned char*)ptr);
  if(state == EEPROM_TRUE)
  {if((ptr->udtTipsPara.MaxVol != udtPtr->MaxVol)||
	   (ptr->udtTipsPara.MinVol != udtPtr->MinVol)||
	   (ptr->udtTipsPara.MoreDisp != udtPtr->MoreDisp)||
	   (ptr->udtTipsPara.MoreSuck != udtPtr->MoreSuck)||
	   (ptr->udtTipsPara.asc != udtPtr->asc)||
	   (ptr->udtTipsPara.zero != udtPtr->zero))
	{//Speicherung aller Komponenten von udtPtr,da sich mindestens eine Komponente untescheidet
	  ptr->udtTipsPara.MaxVol = udtPtr->MaxVol;
	  ptr->udtTipsPara.MinVol = udtPtr->MinVol;
	  ptr->udtTipsPara.MoreDisp = udtPtr->MoreDisp;
	  ptr->udtTipsPara.MoreSuck = udtPtr->MoreSuck;
	  ptr->udtTipsPara.asc = udtPtr->asc;
	  ptr->udtTipsPara.zero = udtPtr->zero;
	  state = wr(sizeof(ptr->udtTipsPara),offsetof(struct EEPROM,udtTipsPara),(unsigned char*)ptr);
	  if(state == EEPROM_TRUE)	state = CB_READY;	//state = EEPROMASPWFlagCtrl(TRUE);
	}
	else state = CB_READY;
	state = EEPROMASPWFlagCtrl(TRUE);
  }
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadTipsPara	(ab Version 1.44)
   PARAMETERS:	Pointer auf Spitzenparameter
   DESCRIPTION:	Lesen von Spitzenparametern,die vom ASPW gespeichert wurden
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadTipsPara(struct TIPS_PARA *udtPtr)
{ubyte state,flg;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  if((state = EEPROMReadASPWFlag(&flg)) == CB_READY)
  {if(flg)
	{// Lesen der gespeicherten Werte
	  state = rd(sizeof(ptr->udtTipsPara),offsetof(struct EEPROM,udtTipsPara),(unsigned char*)ptr);
	  if(state == EEPROM_TRUE)
	  {	udtPtr->MaxVol = ptr->udtTipsPara.MaxVol;
		udtPtr->MinVol = ptr->udtTipsPara.MinVol;
		udtPtr->MoreDisp = ptr->udtTipsPara.MoreDisp;
		udtPtr->MoreSuck = ptr->udtTipsPara.MoreSuck;
		udtPtr->asc = ptr->udtTipsPara.asc;
		udtPtr->zero = ptr->udtTipsPara.zero;
		state = CB_READY;
	  }
	  else state = IERR_EEPROM_READ;
	}
	else state = IERR_NO_TIPS_PARA;
  }
  free(ptr);
  return state;
}


/* FUNCTION: 	EEPROMSetLanguage
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der Spracheinstellung in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetLanguage(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->bytLang = g_ubytLang;
  state = wr(sizeof(ptr->bytLang),offsetof(struct EEPROM,bytLang),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadAndCheckLanguage
   PARAMETERS:	---
   DESCRIPTION:	Lesen und Setzen der Spracheinstellung vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadAndCheckLanguage(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->bytLang),offsetof(struct EEPROM,bytLang),(unsigned char*)ptr);
	if(state == EEPROM_TRUE){
		state = CB_READY;
		g_ubytLang = ptr->bytLang;
		if(LanguageCheck() == FALSE) ProgEnglish();
	}
	else
		state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetDispWaitTime
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der Dispensierwartezeit in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetDispWaitTime(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->DispWaitTime = g_fDispWaitTime;
  state = wr(sizeof(ptr->DispWaitTime),offsetof(struct EEPROM,DispWaitTime),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadDispWaitTime
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Dispensierwartezeit vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadDispWaitTime(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->DispWaitTime),offsetof(struct EEPROM,DispWaitTime),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) {state = CB_READY; g_fDispWaitTime = ptr->DispWaitTime;}
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetVertHeightCorr
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der Korrekturhöhe des Z-Antriebes in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetVertHeightCorr(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->VertHeightCorr = g_fVertHeightCorr;
  state = wr(sizeof(ptr->VertHeightCorr),offsetof(struct EEPROM,VertHeightCorr),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadVertHeightCorr
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Korrekturhöhe des Z-Antriebes vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadVertHeightCorr(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->VertHeightCorr),offsetof(struct EEPROM,VertHeightCorr),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) {state = CB_READY; g_fVertHeightCorr = ptr->VertHeightCorr;}
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetVertManWayLimit
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der Wegbegrenzung des Z-Antriebes in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetVertManWayLimit(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->VertManLimitWay = g_fVertManLimitWay;
  state = wr(sizeof(ptr->VertManLimitWay),offsetof(struct EEPROM,VertManLimitWay),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadVertManWayLimit
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Wegbegrenzung des Z-Antriebes  vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadVertManWayLimit(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->VertManLimitWay),offsetof(struct EEPROM,VertManLimitWay),(unsigned char*)ptr);
  if(state == EEPROM_TRUE)
  {state = CB_READY; g_fVertManLimitWay = ptr->VertManLimitWay;}
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetPstManMotPara
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der manuellen Kolben-Antriebsparameter in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetPstManMotPara(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  /*ptr->PstManMotPara.start = PstManStartRpm;
  ptr->PstManMotPara.end = PstManEndRpm;
  ptr->PstManMotPara.cnt = PstManCounts;*/
  CopyRevPara(&(ptr->PstManMotPara),&g_udtPstRevMan);
  state = wr(sizeof(ptr->PstManMotPara),offsetof(struct EEPROM,PstManMotPara),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadPstManMotPara
   PARAMETERS:	---
   DESCRIPTION:	Lesen der manuellen Kolben-Antriebsparameter vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadPstManMotPara(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));

	// begin test
	//unsigned short len = sizeof(ptr->DispWaitTime);
	//const unsigned int offset = offsetof(struct EEPROM,DispWaitTime);
	// end test

	// bit rd(const unsigned short len,const unsigned int offset,unsigned char * dest)
	state = rd(sizeof(ptr->PstManMotPara),offsetof(struct EEPROM,PstManMotPara),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
	CopyRevPara(&g_udtPstRevMan,&(ptr->PstManMotPara));
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetVertManMotPara
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der manuellen Z-Antriebsparameter in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetVertManMotPara(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  CopyRevPara(&(ptr->VertManMotPara),&g_udtVertRevMan);
  state = wr(sizeof(ptr->VertManMotPara),offsetof(struct EEPROM,VertManMotPara),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadVertManMotPara
   PARAMETERS:	---
   DESCRIPTION:	Lesen der manuellen Z-Antriebsparameter vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadVertManMotPara(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->VertManMotPara),offsetof(struct EEPROM,VertManMotPara),(unsigned char*)ptr);
  if(state == EEPROM_TRUE)
	  state = CB_READY;
  else
	  state = IERR_EEPROM_READ;
  CopyRevPara(&g_udtVertRevMan,&(ptr->VertManMotPara));
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetBeepState
   PARAMETERS:	---
   DESCRIPTION:	Schreiben des Beep-Status in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetBeepState(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->bytBeepState = g_bBeepState;
  state = wr(sizeof(ptr->bytBeepState),offsetof(struct EEPROM,bytBeepState),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadBeepState
   PARAMETERS:	---
   DESCRIPTION:	Lesen und Setzen des Beep-Status vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadBeepState(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->bytBeepState),offsetof(struct EEPROM,bytBeepState),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) {state = CB_READY; g_bBeepState = ptr->bytBeepState;}
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetServoAdjustPara
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der XY-Parameter in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetServoAdjustPara(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->udtServoAdjustPara.intXOffset = g_udtServoAdjustPara.intXOffset;
  ptr->udtServoAdjustPara.intXSlope = g_udtServoAdjustPara.intXSlope;
  ptr->udtServoAdjustPara.intYOffset = g_udtServoAdjustPara.intYOffset;
  ptr->udtServoAdjustPara.intYSlope = g_udtServoAdjustPara.intYSlope;
  state = wr(sizeof(ptr->udtServoAdjustPara),offsetof(struct EEPROM,udtServoAdjustPara),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadServoAdjustPara
   PARAMETERS:	---
   DESCRIPTION:	Lesen der XY-Parameter vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadServoAdjustPara(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->udtServoAdjustPara),offsetof(struct EEPROM,udtServoAdjustPara),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
  g_udtServoAdjustPara.intXOffset = ptr->udtServoAdjustPara.intXOffset;
  g_udtServoAdjustPara.intXSlope = ptr->udtServoAdjustPara.intXSlope;
  g_udtServoAdjustPara.intYOffset = ptr->udtServoAdjustPara.intYOffset;
  g_udtServoAdjustPara.intYSlope = ptr->udtServoAdjustPara.intYSlope;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetServoPlateTypeMode
   PARAMETERS:	---
   DESCRIPTION:	Schreiben des Plattentypmodes in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetServoPlateTypeMode(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->udtActDeviceType = g_pudtActDeviceType;
  state = wr(sizeof(ptr->udtActDeviceType),offsetof(struct EEPROM,udtActDeviceType),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadServoPlateTypeMode
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Plattentypmodus vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadServoPlateTypeMode(struct DEV_TYPE **udtPtr)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->udtActDeviceType),offsetof(struct EEPROM,udtActDeviceType),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
  *udtPtr = ptr->udtActDeviceType;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetHostModeFlag
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Gerätearbeitsmodus
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetHostModeFlag(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->bytHostModeState = g_bHostMode;
  state = wr(sizeof(ptr->bytHostModeState),offsetof(struct EEPROM,bytHostModeState),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadHostModeFlag
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Gerätearbeitsmodus
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadHostModeFlag(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->bytHostModeState),offsetof(struct EEPROM,bytHostModeState),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) {state = CB_READY; g_bHostMode = ptr->bytHostModeState;}
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

ubyte EEPROMHostModeFlagResetAndCmp(void)
{ubyte state;
  g_bHostMode = FALSE;
  if((state = EEPROMSetHostModeFlag()) != CB_READY) return state;
  if((state = EEPROMReadHostModeFlag()) != CB_READY) return state;
  if(g_bHostMode == FALSE) state = CB_READY; else state = IERR_EEPROM_MEMORY;
  return state;
}

/* FUNCTION: 	EEPROMReadHeadExchangeAtDev
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Kopfwechselanzahl des Gerätes
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadHeadExchangeAtDev(ulong *cnt)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->ulngHeadExchange),offsetof(struct EEPROM,ulngHeadExchange),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
  *cnt = ptr->ulngHeadExchange; free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetHeadExchangeAtDev
   PARAMETERS:	---
   DESCRIPTION:	Setzen der Kopfwechselanzahl des Gerätes
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetHeadExchangeAtDev(ulong *cnt)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->ulngHeadExchange = *cnt;
  state = wr(sizeof(ptr->ulngHeadExchange),offsetof(struct EEPROM,ulngHeadExchange),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}


/* FUNCTION: 	EEPROMSetCarrType
   PARAMETERS:	Wagentyp
   DESCRIPTION:	Schreiben des Wagentypes in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetCarrType(ubyte car)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->bytCarrType = car;
  state = wr(sizeof(ptr->bytCarrType),offsetof(struct EEPROM,bytCarrType),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadCarrType
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Wagentypes vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadCarrType(void)
{ubyte state,car;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->bytCarrType),offsetof(struct EEPROM,bytCarrType),(unsigned char*)ptr);
  if(state == EEPROM_TRUE)
  {
  	car = ptr->bytCarrType; state = HrzSetCarType(car);
  }
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

ubyte EEPROMSetCarrTypeAndCmp(ubyte car)
{ubyte help,state;
  if((state = EEPROMSetCarrType(car)) != CB_READY) return state;
  help = car;
  if((state = EEPROMReadCarrType()) != CB_READY) return state;
  if(help == g_pudtHrzPosTabPtr->cartype) return CB_READY;
  return IERR_EEPROM_MEMORY;
}


/* FUNCTION: 	EEPROMReadHeadSafety
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Lichtschrankenflags (Safety light curtain)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadHeadSafety(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->bytSafetyHead),offsetof(struct EEPROM,bytSafetyHead),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) {state = CB_READY; g_ubytSafetyHead = ptr->bytSafetyHead;}
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetHeadSafety
   PARAMETERS:	zu setzende Wert
   DESCRIPTION:	Setzen des Lichtschrankenflags (Safety light curtain),d.h. Gerät arbeitet mit LS
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetHeadSafety(ubyte flg)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->bytSafetyHead = flg;
  state = wr(sizeof(ptr->bytSafetyHead),offsetof(struct EEPROM,bytSafetyHead),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMResetLSAndCmp
   PARAMETERS:	---
   DESCRIPTION:	Setzen des Lichtschrankenflags auf Betrieb ohne Lichtschranke
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMResetLSAndCmp(void)
{ubyte state;
  g_ubytSafetyHead = WORKING_WITH_BLIND;
  state = EEPROMSetHeadSafety(g_ubytSafetyHead); if(state != CB_READY) return state;
  state = EEPROMReadHeadSafety(); if(state != CB_READY) return state;
  if(g_ubytSafetyHead != WORKING_WITH_BLIND) return IERR_EEPROM_MEMORY;
  return CB_READY;
}


/* FUNCTION: 	EEPROMSetShieldWatchFlag
   PARAMETERS:	zu setzende Wert
   DESCRIPTION:	Setzen des überwachungsflags für die Jaloisie
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetShieldWatchFlag(ubyte flg)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->bytShieldWatch = flg;
  state = wr(sizeof(ptr->bytShieldWatch),offsetof(struct EEPROM,bytShieldWatch),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadShieldWatchFlag
   PARAMETERS:	---
   DESCRIPTION:	Lesen des überwachungsflags für die Jaloisie
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadShieldWatchFlag(void)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  state = rd(sizeof(ptr->bytShieldWatch),offsetof(struct EEPROM,bytShieldWatch),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) {state = CB_READY; g_ubytShieldWatch = ptr->bytShieldWatch;}
  else state = IERR_EEPROM_READ;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMSetShiedWatchFlagAndCmp
   PARAMETERS:	---
   DESCRIPTION:	Setzen des ShieldWatchFlag auf Betrieb mit Jaloisie
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetShieldWatchFlagAndCmp(void)
{ubyte state;
  g_ubytShieldWatch = WORKING_WITH_SHIELD;
  state = EEPROMSetShieldWatchFlag(g_ubytShieldWatch); if(state != CB_READY) return state;
  state = EEPROMReadShieldWatchFlag(); if(state != CB_READY) return state;
  if(g_ubytShieldWatch == WORKING_WITHOUT_SHIELD) return IERR_EEPROM_MEMORY;
  return CB_READY;
}

/* FUNCTION: 	EEPROMSetUniAdapterPara
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der Servo-Parameter zur Steuerung des Uniadapters in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetUniAdapterPara(struct UNIAdj *udtPtr)
{ubyte state;
  struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
  ptr->udtUniServoAdjustPara.intUniOffset = udtPtr->intUniOffset;
  ptr->udtUniServoAdjustPara.intUniGain = udtPtr->intUniGain;
  state = wr(sizeof(ptr->udtUniServoAdjustPara),offsetof(struct EEPROM,udtUniServoAdjustPara),(unsigned char*)ptr);
  if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
  free(ptr);
  return state;
}

/* FUNCTION: 	EEPROMReadUniAdapterPara
   PARAMETERS:	---
   DESCRIPTION:	Lesen der Servo-Parameter zur Steuerung des Uniadapters vom EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadUniAdapterPara(struct UNIAdj *udtPtr){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));

	state = rd(sizeof(ptr->udtUniServoAdjustPara),offsetof(struct EEPROM,udtUniServoAdjustPara),(unsigned char*)ptr);

	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
	udtPtr->intUniOffset = ptr->udtUniServoAdjustPara.intUniOffset;
	udtPtr->intUniGain = ptr->udtUniServoAdjustPara.intUniGain;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMReadPMFlag
   PARAMETERS:	---
   DESCRIPTION:	Lesen des PM-Flags (Kennung Präsentiermechanik vorhanden / nicht vorhanden)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadPMFlag(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->ubytPMFlag),offsetof(struct EEPROM,ubytPMFlag),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) {state = CB_READY; usIsPresMechConfig = ptr->ubytPMFlag;}
	else state = IERR_EEPROM_READ;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMWritePMFlag
   PARAMETERS:	Flag
   DESCRIPTION:	Schreiben des PM-Flags (Kennung Präsentiermechanik vorhanden / nicht vorhanden)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMWritePMFlag(ubyte flg){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	ptr->ubytPMFlag = flg;
	state = wr(sizeof(ptr->ubytPMFlag),offsetof(struct EEPROM,ubytPMFlag),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMReadMagazineMode
   PARAMETERS:	---
   DESCRIPTION:	Lesen des Magazin-Kopfwechsel-Flag (0/1)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadMagazineMode(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->ubytMagazineMode),offsetof(struct EEPROM,ubytMagazineMode),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) {state = CB_READY; g_ubytMagNotInsertAfterHeadExchange = ptr->ubytMagazineMode;}
	else state = IERR_EEPROM_READ;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMWriteMagazineMode
   PARAMETERS:	Flag
   DESCRIPTION:	Lesen des Magazin-Kopfwechsel-Flag (0/1)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMWriteMagazineMode(ubyte flg){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	ptr->ubytMagazineMode = flg;
	state = wr(sizeof(ptr->ubytMagazineMode),offsetof(struct EEPROM,ubytMagazineMode),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMReadAndSetVerticalSwitchPos
   PARAMETERS:	---
   DESCRIPTION:	Lesen der minimalen Ausheberhöhe für XY-Positionierung
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadAndSetVerticalSwitchPos(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->fVerticalSwitchPos),offsetof(struct EEPROM,fVerticalSwitchPos),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) {state = CB_READY; g_fVertXYSwitchPos = ptr->fVerticalSwitchPos;}
	else state = IERR_EEPROM_READ;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMWriteVerticalSwitchPos
   PARAMETERS:	---
   DESCRIPTION:	Schreiben der minimalen Ausheberhöhe für XY-Positionierung
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMWriteVerticalSwitchPos(float pos){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	ptr->fVerticalSwitchPos = pos;
	state = wr(sizeof(ptr->fVerticalSwitchPos),offsetof(struct EEPROM,fVerticalSwitchPos),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMReadKeyboardLayout
   PARAMETERS:	---
   DESCRIPTION:	Lersen des Tastaturlayouts
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadKeyboardLayout(void){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->ubytKeyboardLayout),offsetof(struct EEPROM,ubytKeyboardLayout),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) {state = CB_READY; g_ubytKeyboardLayout = ptr->ubytKeyboardLayout;}
	else state = IERR_EEPROM_READ;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMWriteKeyboardLayout
   PARAMETERS:	Layout-Code
   DESCRIPTION:	Schreiben des Layout-Codes in dem EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMWriteKeyboardLayout(ubyte layout){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	ptr->ubytKeyboardLayout = layout;
	state = wr(sizeof(ptr->ubytKeyboardLayout),offsetof(struct EEPROM,ubytKeyboardLayout),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetHrzMotPara
   PARAMETERS:	Pointer auf REV_PARA
   DESCRIPTION:	Schreiben der Wagen-Antriebsparameter in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetHrzMotPara(const struct REV_PARA *scr){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	CopyRevPara(&(ptr->HrzMotPara),scr);
	state = wr(sizeof(ptr->HrzMotPara),offsetof(struct EEPROM,HrzMotPara),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMReadHrzMotPara
   PARAMETERS:	Pointer auf REV_PARA
   DESCRIPTION:	Lesen der Wagen-Antriebsparameter (Start-,Enddrehzahl und Beschleunigung)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadHrzMotPara(struct REV_PARA *dst){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->HrzMotPara),offsetof(struct EEPROM,HrzMotPara),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
	CopyRevPara(dst,&(ptr->HrzMotPara));
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMReadHeadTypeNumber
   PARAMETERS:	Pointer auf ZK-Speicher
   DESCRIPTION:	Lesen des letzen im Gerät befindlichen Kopfes (ab Version 4.70)
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadHeadTypeNumber(char *sptr){
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->HeadTypeNo),offsetof(struct EEPROM,HeadTypeNo),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
	strncpy(sptr,ptr->HeadTypeNo,sizeof(((struct EEPROM*)NULL)->HeadTypeNo));
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetHeadTypeNumber
   PARAMETERS:	Pointer auf ZK mit der Gerätenummer
   DESCRIPTION:	Schreiben der Kopfnummer in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMSetHeadTypeNumber(const char *sptr){
	ubyte state;
	char temp_zk[20];
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	memcpy(temp_zk,sptr,strlen(sptr)+1);
	if(strlen(temp_zk) <= (sizeof(ptr->HeadTypeNo) - 1)){
		strcpy(ptr->HeadTypeNo,temp_zk);
		state = wr(sizeof(ptr->HeadTypeNo),offsetof(struct EEPROM,HeadTypeNo),(unsigned char*)ptr);
		if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	}else state = IERR_PARA;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetHeadTypeNumber
   PARAMETERS:	Pointer auf Adresse
   DESCRIPTION:	Lesen der Life-Parameter Position aus EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadLifeParaPosition(uint32_t *lifePos)
{
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	state = rd(sizeof(ptr->LifeParaOffset),offsetof(struct EEPROM,LifeParaOffset),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_READ;
	*lifePos = ptr->LifeParaOffset;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetHeadTypeNumber
   PARAMETERS:	Pointer auf Adresse
   DESCRIPTION:	Schreiben der Life-Parameter Position in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMWriteLifeParaPosition(uint32_t *lifePos)
{
	ubyte state;
	struct EEPROM * ptr = (struct EEPROM *) calloc(1,sizeof(struct EEPROM));
	ptr->LifeParaOffset = *lifePos;
	state = wr(sizeof(ptr->LifeParaOffset),offsetof(struct EEPROM,LifeParaOffset),(unsigned char*)ptr);
	if(state == EEPROM_TRUE) state = CB_READY; else state = IERR_EEPROM_WRITE;
	free(ptr);
	return state;
}

/* FUNCTION: 	EEPROMSetHeadTypeNumber
   PARAMETERS:	Pointer auf Life Parameter
   DESCRIPTION:	Lesen der Life-Parameter aus EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMReadLifePara(struct LIFE_PARA *life)
{
	//see annotation at  EEPROMWriteLifePara why rd function is not possible
	if(SPI3_EEPROM_READ(offsetof(struct EEPROM,LifePara) + g_lifePos,(unsigned char *) life,sizeof(*life)) != HAL_OK)
		return IERR_EEPROM_READ;

	//printf("Life Para %f, %f, %i, %i, %u at %u\r",life->PstCycleVolume, life->PstTotalVolume,
	//		life->ulngPistonCycles, life->ulngXYCycles, life->writeCycles, offsetof(struct EEPROM,LifePara)+g_lifePos);
	return CB_READY;
}

/* FUNCTION: 	EEPROMSetHeadTypeNumber
   PARAMETERS:	Pointer auf Life Parameter
   DESCRIPTION:	Schreiben der Life-Parameter in den EEPROM
   RETURNS: 	CB_READY / Fehler */
ubyte EEPROMWriteLifePara(struct LIFE_PARA *life)
{
	ubyte state = CB_READY;
	struct LIFE_PARA compare;
	ubyte any = 0;
	ubyte newLoc = 0;
	state = EEPROMReadLifePara(&compare);
	if(state != CB_READY) return state; //somthing went wrong
	if(compare.writeCycles >= EEPROM_MAX_WRITE_CYCLES)
	{
		if(g_lifePos + sizeof(compare) >= EEPROM_PHYSICAL_SIZE - sizeof(compare)) //Struct is completely saveable in EEPROM?
		{
			return IERR_EEPROM_MEMORY; //not enough space. If (8000byte/(5*4byte)*1000000 Cyclen erreicht)
		}
		g_lifePos += sizeof(compare); 			//Save Life-Para in next block
		compare.writeCycles = 0; 				//Reset Write Cycles
		life->writeCycles = 0;
		//printf("Realloc Life Para %f, %f, %i, %i, %u at %u\r",life->PstCycleVolume, life->PstTotalVolume,
		//		life->ulngPistonCycles, life->ulngXYCycles, life->writeCycles, offsetof(struct EEPROM,LifePara)+g_lifePos);
		newLoc = 1;
	}

	if(compare.PstCycleVolume != life->PstCycleVolume) //Value or Position changed
	{
		any = 1;
		//state = wr(sizeof(ptr->LifePara.PstCycleVolume),offsetof(struct EEPROM,LifePara)+ offsetof(struct LIFE_PARA, PstCycleVolume) +lifePos,(unsigned char*)ptr);
		//printf("PstCycleVolume\r");
	}
	if(compare.PstTotalVolume != life->PstTotalVolume) //Value or Position changed
	{
		any = 1;
		//state = wr(sizeof(ptr->LifePara.PstTotalVolume),offsetof(struct EEPROM,LifePara)+ offsetof(struct LIFE_PARA, PstTotalVolume) +lifePos,(unsigned char*)ptr);
		//printf("PstTotalVolume\r");
	}
	if(compare.ulngPistonCycles != life->ulngPistonCycles) //Value or Position changed
	{
		any = 1;
		//state = wr(sizeof(ptr->LifePara.ulngPistonCycles),offsetof(struct EEPROM,LifePara)+ offsetof(struct LIFE_PARA, ulngPistonCycles) + lifePos,(unsigned char*)ptr);
		//printf("PistonCycles\r");
	}
	if(compare.ulngXYCycles != life->ulngXYCycles) //Value or Position changed
	{
		any = 1;
		//state = wr(sizeof(ptr->LifePara.ulngXYCycles),offsetof(struct EEPROM,LifePara)+ offsetof(struct LIFE_PARA, ulngXYCycles)+lifePos,(unsigned char*)ptr);
		//printf("XYCycles\r");
	}
	if(any || newLoc)
	{
		life->writeCycles = compare.writeCycles + 1;
		//printf("Write to %u offset: %u\r", offsetof(struct EEPROM,LifePara), lifePos);

		//the normal wr adds the offset also to the struct, which results in random memory aacess
		if (SPI3_EEPROM_WRITE(offsetof(struct EEPROM,LifePara) + g_lifePos, (unsigned char *)life ,sizeof(*life)) != HAL_OK)
			return IERR_EEPROM_WRITE;
		if(newLoc)	//write new Position only if Reallocation was successfull
		{
			EEPROMWriteLifeParaPosition(&g_lifePos);
		}
	}

	return CB_READY;

}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Reads a block of data from EEPROM starting at a given address.
/// \param[in]      address        Start address in EEPROM to read from
/// \param[out]     pData          Pointer to buffer where read data will be stored
/// \param[in]      data_length    Number of bytes to read
/// \return         CB_READY if successful
/////////////////////////////////////////////////////////////////////////////////
ErrorState_t EEPROMRead(const uint8_t address,  uint8_t * const pData, const uint16_t data_length)
{
	ErrorState_t state = CB_READY;
	uint8_t * ptr_data = pData;

	for(uint16_t i= address; i < (address + data_length);i++ )
	{
		*ptr_data= ReadEEprom(i);
		ptr_data++;
	}
	return state;
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Writes a block of data to EEPROM starting at a given address.
/// \param[in]      address        Start address in EEPROM to write to
/// \param[in]      pData          Pointer to buffer containing data to write
/// \param[in]      data_length    Number of bytes to write
/// \return         CB_READY if successful,
///                 IERR_EEPROM_MEMORY if write operation fails
/////////////////////////////////////////////////////////////////////////////////
ErrorState_t EEPROMWrite(const uint8_t address, const uint8_t * const pData, const uint16_t data_length)
{
	ErrorState_t state = CB_READY;
	const uint8_t * ptr_data = pData;

	for(uint16_t i= address; i < (address + data_length);i++ )
	{
		if(WriteEEprom(*ptr_data, i))
		{
			state= IERR_EEPROM_MEMORY;
			break;
		}
		ptr_data++;
	}
	return state;
}

