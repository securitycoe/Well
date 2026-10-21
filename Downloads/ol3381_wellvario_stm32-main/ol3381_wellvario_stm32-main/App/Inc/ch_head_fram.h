/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
#ifndef _FRAM_H_
#define _FRAM_H_

#include "head_piston_tab.h"
#include "aj_typedefs.h"

#define FRAM_PAGE_SIZE 256
#define FRAM_PAGES 2

#define STRUCT_VERSION_P0 1		//Page 0 vom FRAM - Änderung nur durch den Entwickler möglich!!!
#define STRUCT_VERSION_P1 2		//Page 1 vom FRAM - Änderung nur ...

#define FRAM_PAGE0_ADDR 0x52
#define FRAM_PAGE1_ADDR 0x53


/*
 ******************************************************************************
 * :ATTENTION:
 * Der Speicherinhalt des FRAM ist nicht byte-kompatibel zwischen dem Infineon
 * C167 und dem STM32F429! Lese/-Schreibzugriffe müssen gesondert behandelt
 * werden!
 *
 * 1. Endianess
 * 		(+) Integer-Werte werden bei beiden Systemen (C167 & STM32F429) im
 * 			Little-Endian-Format abgespeichert.
 * 		(+) Float-Werte (4 Byte) werden beim Infineon C167 im sog.
 * 			Mid-Big-Endian-Format abgespeichert. Der STM32F429 speichert
 * 			Float-Werte im Little-Indian-Format.
 *
 * 			|---------------------------------------|
 * 			|			Byte-Reihenfolge			|
 * 			|-------------------|-------------------|
 * 			|	 STM32F429		|	Infineon C167	|
 * 			|  (Little-Endian)	|  (Mid-Big-Endian)	|
 * 			|-------------------|-------------------|
 * 			|	[b0 b1 b2 b3]	|	[b2 b3 b0 b1]	|
 * 			|-------------------|-------------------|
 *
 * 		(+)	Format des Infineon C167 muss aus Kompatibilitätsgründen übernommen
 * 			werden! Lese-/Schreibzugrfiffe können mit folgenden Funktionen
 * 			realisiert werden (definiert in ch_head_fram.c):
 *
 * 			(++)  WriteU8ToFRAM			Schreiben von Daten des Datentyps
 * 										uint8_t, uint16_t, uint32_t und char-Arrays
 * 										(evtl. cast notwendig)
 *
 * 			(++)  ReadU8FromFRAM		Lesen von Daten des Datentyps
 * 										uint8_t, uint16_t, uint32_t und char-Arrays
 * 										(evtl. cast notwendig)
 *
 * 			(++)  WriteFloat32ToFRAM	Schreiben von Daten des Datentyps float32_t
 *
 * 			(++)  ReadFloat32FromFRAM	Lesen von Daten des Datentyps float32_t
 *
 * 		(+) Schreiben/Lesen von Daten des Datentyps float64_t (8 byte) und bool
 * 			--> Aktuell nicht implementiert und nicht getestet!
 *
 * 2. Struct - Alignment
 * 		(+)	Das Alignment von Struct-Membern (betrifft "struct FRAM0" und "struct FRAM1")
 * 			beim Infineon C167 und STM32F429 unterscheidet sich.
 *
 * 				(++) Infineon C167 sind es 2Byte (16bit Alignment)
 *
 * 				(++) STM32F429 4Byte(32bit Alignment).
 *
 *		(+)	Alignement des Infineon C167 muss aus Kompatibilitätsgründen übernommen
 * 			werden!
 *
 * 		(+) Funktion "offsetof" kann nicht für die Ermittlung der Addresse genutzt
 * 			werden! Abhängig vom System werden unterscheidliche Addressoffsets ermittelt.
 *
 * 			Bsp.: offsetof(struct FRAM0, PstVolume)
 *
 * 					- Ergebnis beim Infineon C167: 2
 *
 * 					- Ergebnis beim STM32F429: 4
 *
 * 			Als Alternative können folgende enums zur Ermittlung der Adresse genutzt
 * 			werden:
 *
 * 				(++) enum FRAM_PAGE0_WIRE_OFFSET für Page0
 *
 * 				(++) enum FRAM_PAGE1_WIRE_OFFSET für Page1
 *
 * 			Achtung! Diese enums muss manuell gepflegt werden!
 *
 * 		(+) Funktion "sizeof" kann nicht für die Ermittlung der Größe der Struktur
 * 			genutzt werden! Abhängig vom System werden unterscheidliche Größen ermittelt.
 *
 *			Bsp.: sizeof(struct FRAM0)
 *
 * 					- Ergebnis beim Infineon C167: 14
 *
 * 					- Ergebnis beim STM32F429: 16
 *
 * 			Alternativ können folgende Elemente des enums FRAM_PAGE0_WIRE_OFFSET
 * 			und des enums FRAM_PAGE1_WIRE_OFFSET genutzt werden:
 *
 * 				(++) FRAM_PAGE0_WIRE_SIZE		Größe der Struktur in Page 0
 *
 * 				(++) FRAM_PAGE1_WIRE_SIZE		Größe der Struktur in Page 1
 *
 * 			Achtung! Diese Werte müssen manuell gepflegt werden!
 *
 ******************************************************************************
 */

 /*FRAM page0*******************************************************************/
typedef struct FRAM0			//Page 0,im Schreib- und Lesemodus
{
	char 		StructVersNr[1];
	float32_t 	PstVolume;  		// Volumen in ml über die gesamte Lebensdauer
	uint32_t 	ulngPstCycles; 		// Anzahl der Kolbenzyklen über die gesamte Lebensdauer
	uint32_t 	ulngDevExchange;	// Anzahl der Gerätewechsel am Kopf
}FRAMStructureHeadPage0_t;

// --- FRAM page 0 Wire-Format (C167-kompatibel) ---
enum FRAM_PAGE0_WIRE_OFFSET
{
    FRAM_PAGE0_OFF_STRUCT_VERSNR_P0	= 0,	// 1 Byte + 1 alignment byte (16bit alignment ->C167)
    FRAM_PAGE0_OFF_PSTVOLUME       	= 2,	// 4 Byte float32 (mid-big endian)
    FRAM_PAGE0_OFF_PSTCYCLES       	= 6,	// 4 Byte uint32 (little endian)
    FRAM_PAGE0_OFF_DEVEXCHANGE     	= 10,	// 4 Byte uint32 (little endian)
    FRAM_PAGE0_WIRE_SIZE     		= 14	// Structure size (FRAM) in bytes
};

/*FRAM page 1******************************************************************/
typedef struct FRAM1			//Page 1,durch Schalter schreibgeschützt
{
	char 		StructVersNr[1];
	char 		HeadNo[15];			// Gerätenummer ist um eine Stelle erweitert
	float32_t 	AddVolume;			// neu ab Strukturversion 2
	float32_t 	res1;					// Platzhalter
	float32_t 	res2;					// Platzhalter
	uint16_t 	TipId;
}FRAMStructureHeadPage1_t;

// --- FRAM page 1 Wire-Format (C167-kompatibel) ---
enum FRAM_PAGE1_WIRE_OFFSET
{
    FRAM_PAGE1_OFF_STRUCT_VERSNR	= 0,   	// 1 Byte
    FRAM_PAGE1_OFF_HEADNO        	= 1,   	// 15 Byte
    FRAM_PAGE1_OFF_ADDVOLUME     	= 16,  	// 4 Byte float32 (mid-big endian)
    FRAM_PAGE1_OFF_RES1          	= 20,  	// 4 Byte float32 (mid-big endian)
    FRAM_PAGE1_OFF_RES2          	= 24,  	// 4 Byte float32 (mid-big endian)
    FRAM_PAGE1_OFF_TIPID         	= 28,  	// 2 Byte uint16 (little endian)
	FRAM_PAGE1_WIRE_SIZE		 	= 30	// Structure size (FRAM) in bytes
};

unsigned char FRAMReadHoldCurrent(float *fptr);
unsigned char FRAMSetHoldCurrent(float *fptr);
ErrorState_t FRAMSetHeadNumber(const char * const sptr);
ErrorState_t FRAMReadHeadNumber(char * const sptr);
ErrorState_t FRAMSetStructVersionP1(void);
ErrorState_t FRAMReadStructVersionP1(uint8_t * const sptr);
ErrorState_t FRAMSetStructVersionP1AndCmp(void);
ErrorState_t FRAMClearAll(void);
ErrorState_t FRAMClearAllAndCmp(void);
ErrorState_t FRAMSetStructVersionP0(void);
ErrorState_t FRAMReadStructVersionP0(uint8_t * const sptr);
ErrorState_t FRAMSetStructVersionP0AndCmp(void);
ErrorState_t FRAMWritePstVolume(void);
ErrorState_t FRAMReadPstVolume(void);
ErrorState_t FRAMWritePstCycles(void);
ErrorState_t FRAMReadPstCycles(void);
ErrorState_t FRAMWriteDevExchangeAtHead(const uint32_t * const cnt);
ErrorState_t FRAMReadDevExchangeAtHead(uint32_t * const cnt);
ErrorState_t FRAMReadPageAndDisplay(const uint8_t adr);
ErrorState_t FRAMCheckAndSetStruktureVersionP1(void);
ErrorState_t FRAMSetAddVolume(const float32_t volume);
ErrorState_t FRAMReadAddVolume(float32_t * const volume);
ErrorState_t FRAMSetTipId(const uint16_t tip_id);
ErrorState_t FRAMReadTipId(uint16_t * const tip_id);
uint32_t FRAMGetSizeInByte(void);
ErrorState_t FRAMRead(const uint8_t address,  uint8_t * const pData, const uint16_t data_length);
ErrorState_t FRAMWrite(const uint8_t address, const uint8_t * const pData, const uint16_t data_length);
#endif
