/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 ******************************************************************/
/** include files **/
#ifdef CMX
	#include "savealloc.h"	// wird nur beim Einsatz von CMX benötigt
#else
#include "malloc.h"
#endif

#include "stdbool.h"
#include "stddef.h"
#include "stdio.h"
#include "string.h"

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Base:
#include "ch_head_fram.h"
#include "ig_define.h"
// ---------- Inc:
#include <_i2c_fram.h>
//#include <main.h>
// ---------- Master
#include "ch_ig_piston.h"
// ---------- Pipettor_Allgemein
#include "Well_Error.h"

/** local definitions **/

/*
 ******************************************************************************
 * :ATTENTION:
 * Der Speicherinhalt des FRAM ist nicht byte-kompatibel zwischen dem Infineon
 * C167 und dem STM32F429! Lese/-Schreibzugriffe müssen gesondert behandelt
 * werden!
 *
 * Siehe Hinweise in Datei ch_head_fram.h
 *
 ******************************************************************************
 */

#define SIZEOF_FLOAT32 4

#ifndef STATIC_ASSERT
	#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]
#endif

STATIC_ASSERT(sizeof(float32_t)   == SIZEOF_FLOAT32, float32_must_be_32bit); //Absicherung, dass float32_t eine Größe von 4 Byte hat

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

/** public functions **/

/** private functions **/

static inline void u32_little_endian_bytes_to_c167(const uint8_t in[SIZEOF_FLOAT32], uint8_t out[SIZEOF_FLOAT32]);
static inline void c167_to_u32_little_endian_bytes(const uint8_t in[SIZEOF_FLOAT32], uint8_t out[SIZEOF_FLOAT32]);
static inline bool WriteU8ToFRAM(const uint8_t i2c_adr,const uint16_t count,const uint16_t fram_address, const uint8_t * const pData);
static inline bool ReadU8FromFRAM(const uint8_t i2c_adr,const uint16_t count, const uint16_t fram_address, uint8_t * const pData);
static inline bool WriteFloat32ToFRAM(const uint8_t i2c_adr, const uint16_t fram_address, const float32_t value);
static inline bool ReadFloat32FromFRAM(const uint8_t i2c_adr, const uint16_t fram_address, float32_t* const pValue);

static ErrorState_t I2CFRAMReadPageAndCompareToZero(const uint8_t adr);
static ErrorState_t FRAMSetRes1(const float32_t val);
static ErrorState_t FRAMSetRes2(float val);



/////////////////////////////////////////////////////////////////////////////////
/// \brief          Converts a 32-bit little-endian byte array to the format
///                 expected by a C167 microcontroller.
/// \param[in]      in     Input byte array in little-endian format (4 bytes)
/// \param[out]     out    Output byte array reordered for C167 format (4 bytes)
/////////////////////////////////////////////////////////////////////////////////
static inline void u32_little_endian_bytes_to_c167(const uint8_t in[SIZEOF_FLOAT32], uint8_t out[SIZEOF_FLOAT32])
{
    out[0] = in[2];
    out[1] = in[3];
    out[2] = in[0];
    out[3] = in[1];
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Converts a 32-bit byte array from C167 format to little-endian
///                 format.
/// \param[in]      in     Input byte array in C167 format (4 bytes)
/// \param[out]     out    Output byte array in little-endian format (4 bytes)
/////////////////////////////////////////////////////////////////////////////////
static inline void c167_to_u32_little_endian_bytes(const uint8_t in[SIZEOF_FLOAT32], uint8_t out[SIZEOF_FLOAT32])
{
    out[0] = in[2];
    out[1] = in[3];
    out[2] = in[0];
    out[3] = in[1];
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Writes a block of 8-bit data to FRAM at the specified address
///                 using the given I2C address.
/// \param[in]      i2c_adr        I2C address of the FRAM device
/// \param[in]      count          Number of bytes to write
/// \param[in]      fram_address   Target address in FRAM memory
/// \param[in]      pData          Pointer to the data buffer to be written
/// \return         true if write was successful, false otherwise
/////////////////////////////////////////////////////////////////////////////////
static inline bool WriteU8ToFRAM(const uint8_t i2c_adr,const uint16_t count,const uint16_t fram_address, const uint8_t * const pData)
{
	return I2CFRAMWrite(i2c_adr, count, fram_address, (uint8_t*)pData);
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Reads a block of 8-bit data from FRAM at the specified address
///                 using the given I2C address.
/// \param[in]      i2c_adr        I2C address of the FRAM device
/// \param[in]      count          Number of bytes to read
/// \param[in]      fram_address   Source address in FRAM memory
/// \param[out]     pData          Pointer to the buffer where read data will be stored
/// \return         true if read was successful, false otherwise
/////////////////////////////////////////////////////////////////////////////////
static inline bool ReadU8FromFRAM(const uint8_t i2c_adr,const uint16_t count, const uint16_t fram_address, uint8_t * const pData)
{
	return I2CFRAMRead(i2c_adr, count, fram_address, pData);
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Converts a 32-bit float to C167 byte format and writes it to
///                 FRAM at the specified address using the given I2C address.
/// \param[in]      i2c_adr        I2C address of the FRAM device
/// \param[in]      fram_address   Target address in FRAM memory
/// \param[in]      value          Float value to be written
/// \return         true if write was successful, false otherwise
/////////////////////////////////////////////////////////////////////////////////
static inline bool WriteFloat32ToFRAM(const uint8_t i2c_adr, const uint16_t fram_address, const float32_t value)
{
	uint8_t u32_bytes[SIZEOF_FLOAT32];
	uint8_t u32_bytes_c167[SIZEOF_FLOAT32];

	memcpy(u32_bytes, &value, SIZEOF_FLOAT32);
	u32_little_endian_bytes_to_c167(u32_bytes, u32_bytes_c167);
	return WriteU8ToFRAM(i2c_adr, SIZEOF_FLOAT32, fram_address, (uint8_t*)u32_bytes_c167);
}

/////////////////////////////////////////////////////////////////////////////////
/// \brief          Reads a 32-bit float value from FRAM at the specified address,
///                 converting it from C167 byte format to standard little-endian.
/// \param[in]      i2c_adr        I2C address of the FRAM device
/// \param[in]      fram_address   Source address in FRAM memory
/// \param[out]     pValue         Pointer to float variable where the result will be stored
/// \return         true if read was successful, false otherwise
/////////////////////////////////////////////////////////////////////////////////
static inline bool ReadFloat32FromFRAM(const uint8_t i2c_adr, const uint16_t fram_address, float32_t* const pValue)
{
	bool i2c_state;
	uint8_t u32_bytes[SIZEOF_FLOAT32];
	uint8_t u32_bytes_c167[SIZEOF_FLOAT32];

	i2c_state=  ReadU8FromFRAM(i2c_adr, SIZEOF_FLOAT32, fram_address, (uint8_t*)u32_bytes_c167);
	if(i2c_state)
	{
		c167_to_u32_little_endian_bytes(u32_bytes_c167,u32_bytes);
		memcpy(pValue, u32_bytes, SIZEOF_FLOAT32);
	}
	return i2c_state;
}


/* FUNCTION: 	FRAMCheckAndSetStruktureVersionP1
 PARAMETERS:	---
 DESCRIPTION:	Vergleicht die Strukturversion des FRAM Page 1 und aktualisiert diese gegebenenfalls
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W /IERR_PARA (Aktion fehlgeschlagen) */
ErrorState_t FRAMCheckAndSetStruktureVersionP1(void)
{
	ErrorState_t state;
	uint8_t vers;
	char no[20];

	state = FRAMReadStructVersionP1(&vers);
	if (state == CB_READY)
	{
		if (vers == 1)
		{
			// Update auf Version 2.0
			state = FRAMReadHeadNumber(no);
			if (state == CB_READY)
			{
				state |= FRAMSetStructVersionP1();
				state |= FRAMSetHeadNumber(no);
				state |= FRAMSetAddVolume(0);
				state |= FRAMSetRes1(0);
				state |= FRAMSetRes2(0);
				state |= FRAMSetTipId(0);
				if (state != CB_READY)
					state = IERR_I2C_HEAD_FRAM_W;
			}
		}
	}
	return state;
}

/* FUNCTION: 	FRAMSetTipId
 PARAMETERS:	Spitzen-ID
 DESCRIPTION:	Setzen der Spitzen-ID
 RETURNS: 		CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMSetTipId(const uint16_t tip_id)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = WriteU8ToFRAM(FRAM_PAGE1_ADDR, sizeof(((FRAMStructureHeadPage1_t*)0)->TipId), FRAM_PAGE1_OFF_TIPID, (uint8_t*) &tip_id);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRAMReadTipId
 PARAMETERS:	Pointer zum Speichern des Spitzentype
 DESCRIPTION:	Lesen des Spitzentype
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadTipId(uint16_t * const tip_id)	//unisgned int must be set to unsigned short or uint16_t!
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadU8FromFRAM(FRAM_PAGE1_ADDR,sizeof(((FRAMStructureHeadPage1_t*)0)->TipId), FRAM_PAGE1_OFF_TIPID, (uint8_t*) &tip_id);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;
	return state;
}

/* FUNCTION: 	FRAMSetAddVolume - Serviceroutine
 PARAMETERS:	Zusatzvolumen
 DESCRIPTION:	Setzen des Zusatzvolumens
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMSetAddVolume(const float32_t volume)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = WriteFloat32ToFRAM(FRAM_PAGE1_ADDR, FRAM_PAGE1_OFF_ADDVOLUME, volume);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRAMReadAddVolume - Serviceroutine
 PARAMETERS:	Pointer zum Speichern des Zusatzvolumens
 DESCRIPTION:	Lesen der Zusatzvolumens
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadAddVolume(float32_t * const volume)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadFloat32FromFRAM(FRAM_PAGE1_ADDR, FRAM_PAGE1_OFF_ADDVOLUME, volume);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;
	return state;
}

static ErrorState_t FRAMSetRes1(const float32_t val)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = WriteFloat32ToFRAM(FRAM_PAGE1_ADDR, FRAM_PAGE1_OFF_RES1, val);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

static ErrorState_t FRAMSetRes2(const float32_t val)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = WriteFloat32ToFRAM(FRAM_PAGE1_ADDR, FRAM_PAGE1_OFF_RES1, val);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRAMSetHeadNumber - Serviceroutine
 PARAMETERS:	Pointer auf die zu speichernden Kopfnummer
 DESCRIPTION:	Einschreiben der Kopfnummer in den Kopf
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W /IERR_PARA (Aktion fehlgeschlagen) */
ErrorState_t FRAMSetHeadNumber(const char * const sptr)
{
	ErrorState_t state;
	bool i2c_state;
	char str[20];

	state = IERR_PARA;
	if (strlen(sptr) <= (sizeof(((FRAMStructureHeadPage1_t*)0)->HeadNo) - 1))
	{
		memset(str, 0, sizeof(str));
		strcpy(str, sptr);
		i2c_state = WriteU8ToFRAM(FRAM_PAGE1_ADDR, strlen(str) + 1,	FRAM_PAGE1_OFF_HEADNO, (uint8_t*) str);
		if (i2c_state)
			state = CB_READY;
		else
			state = IERR_I2C_HEAD_FRAM_W;
	}
	return state;
}

/* FUNCTION: 	FRAMReadHeadNumber - Serviceroutine
 PARAMETERS:	Pointer auf Speicher für die zu speichernden Kopfnummer
 DESCRIPTION:	Lesen der Kopfnummer
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadHeadNumber(char * const sptr)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadU8FromFRAM(FRAM_PAGE1_ADDR, sizeof(((FRAMStructureHeadPage1_t*)0)->HeadNo), FRAM_PAGE1_OFF_HEADNO, (uint8_t*) sptr);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;
	return state;
}

/* FUNCTION: 	FRamSetStructVersionP1 - Serviceroutine
 PARAMETERS:	Pointer auf der Strukturversionsnummer
 DESCRIPTION:	Setzen der Strukturversionsnummer
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W (Aktion fehlgeschlagen) */
ErrorState_t FRAMSetStructVersionP1(void)
{
	ErrorState_t state;
	uint8_t vers;
	bool i2c_state;

	vers = STRUCT_VERSION_P1;
	i2c_state = WriteU8ToFRAM(FRAM_PAGE1_ADDR, sizeof(((FRAMStructureHeadPage1_t*)0)->StructVersNr), FRAM_PAGE1_OFF_STRUCT_VERSNR, &vers);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRamReadStructVersionP1 - Serviceroutine
 PARAMETERS:	Pointer zum Speichern der gelesenen Strukturversionsnummer
 DESCRIPTION:	Lesen der Strukturversionsnummer
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadStructVersionP1(uint8_t * const sptr)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadU8FromFRAM(FRAM_PAGE1_ADDR, sizeof(((FRAMStructureHeadPage1_t*)0)->StructVersNr), FRAM_PAGE1_OFF_STRUCT_VERSNR, (uint8_t*) sptr);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;
	return state;
}

/* FUNCTION: 	FRAMSetStructVersionP1AndCmp - Serviceroutine
 PARAMETERS:	---
 DESCRIPTION:	Lesen der Strukturversionsnummer und Vergleich mit der Vorgabe
 RETURNS: 	CB_READY / Fehlercode */
ErrorState_t FRAMSetStructVersionP1AndCmp(void)
{
	ErrorState_t state;
	uint8_t vers;

	if ((state = FRAMSetStructVersionP1()) == CB_READY)
	{
		if ((state = FRAMReadStructVersionP1(&vers)) == CB_READY)
		{
			if (vers != STRUCT_VERSION_P1)
				state = IERR_I2C_MEMORY;
		}
	}
	return state;
}

/* FUNCTION: 	FRamClearAll - Serviceroutine
 PARAMETERS:	---
 DESCRIPTION:	Löschen des gesamten Kopfspeichers
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W (Aktion fehlgeschlagen) */
ErrorState_t FRAMClearAll(void)
{
	ErrorState_t state;
	bool i2c_state;

#ifdef CMX
	uint8_t *ptr = savecalloc(1,FRAM_PAGE_SIZE);
  #else
	uint8_t *ptr = calloc(1, FRAM_PAGE_SIZE);
#endif
	state = IERR_I2C_HEAD_FRAM_W;
	i2c_state = WriteU8ToFRAM(FRAM_PAGE0_ADDR, FRAM_PAGE_SIZE, 0, ptr);
	if (i2c_state)
	{
		i2c_state = WriteU8ToFRAM(FRAM_PAGE1_ADDR, FRAM_PAGE_SIZE, 0, ptr);
		if (i2c_state)
			state = CB_READY;
	}
#ifdef CMX
	CalFree(ptr);
  #else
	free(ptr);
#endif
	return state;
}

/* FUNCTION: 	FRAMClearAllAndCmp - Serviceroutine
 PARAMETERS:	---
 DESCRIPTION:	Löschen des gesamten Kopfspeichers und überprüfung
 RETURNS: 	CB_READY / Fehlermeldung */
ErrorState_t FRAMClearAllAndCmp(void)
{
	unsigned char state;

	state = FRAMClearAll();
	if (state == CB_READY) {
		state = I2CFRAMReadPageAndCompareToZero(FRAM_PAGE0_ADDR);
		if (state == CB_READY)
			state = I2CFRAMReadPageAndCompareToZero(FRAM_PAGE1_ADDR);
	}
	return state;
}

/* FUNCTION: 	I2CFRAMReadPageAndCompareToZero - Serviceroutine
 PARAMETERS:	---
 DESCRIPTION:	Löschen einer Kopfspeicher-Seite und überprüfung
 RETURNS: 	CB_READY / Fehlermeldung */
static ErrorState_t I2CFRAMReadPageAndCompareToZero(const uint8_t adr)
{
	uint8_t *hPtr;
	ErrorState_t state;

#ifdef CMX
	uint8_t *ptr = savecalloc(1,FRAM_PAGE_SIZE);
  #else
	uint8_t *ptr = calloc(1, FRAM_PAGE_SIZE);
#endif
	state = CB_READY;
	if (ReadU8FromFRAM(adr, FRAM_PAGE_SIZE, 0, ptr))
	{
		for (hPtr = ptr; hPtr < (ptr + FRAM_PAGE_SIZE); hPtr++)
		{
			if (*hPtr != 0)
			{
				printf("Error page: %s\r", adr == FRAM_PAGE0_ADDR ? "0" : "1");
				state = IERR_I2C_MEMORY;
				break;
			}
		}
	}
	else
		state = IERR_I2C_HEAD_FRAM_R;	// Lesefehler
#ifdef CMX
	CalFree(ptr);
  #else
	free(ptr);
#endif
	return state;
}

/********** FRAM PAGE 0,Schreib-Lesespeicher **********/

/* FUNCTION: 	FRamSetStructVersionP0 - Serviceroutine
 PARAMETERS:	Pointer auf der Strukturversionsnummer
 DESCRIPTION:	Setzen der Strukturversionsnummer
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W (Aktion fehlgeschlagen) */
ErrorState_t FRAMSetStructVersionP0(void)
{
	ErrorState_t state;
	uint8_t vers;
	bool i2c_state;

	vers = STRUCT_VERSION_P0;
	i2c_state = ReadU8FromFRAM(FRAM_PAGE0_ADDR, sizeof(uint8_t), FRAM_PAGE0_OFF_STRUCT_VERSNR_P0, &vers);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRAMReadStructVersionP0 - Serviceroutine
 PARAMETERS:	Pointer zum Speichern der gelesenen Strukturversionsnummer
 DESCRIPTION:	Lesen der Strukturversionsnummer
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadStructVersionP0(uint8_t * const sptr)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadU8FromFRAM(FRAM_PAGE0_ADDR, sizeof(((FRAMStructureHeadPage0_t*)0)->StructVersNr), FRAM_PAGE0_OFF_STRUCT_VERSNR_P0, (uint8_t*) sptr);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;
	return state;
}

/* FUNCTION: 	FRAMSetStructVersionP0AndCmp - Serviceroutine
 PARAMETERS:	---
 DESCRIPTION:	Lesen der Strukturversionsnummer und Vergleich mit der Vorgabe
 RETURNS: 	CB_READY / Fehlercode */
ErrorState_t FRAMSetStructVersionP0AndCmp(void)
{
	ErrorState_t state;
	uint8_t vers;

	if ((state = FRAMSetStructVersionP0()) == CB_READY)
	{
		if ((state = FRAMReadStructVersionP0(&vers)) == CB_READY)
		{
			if (vers != STRUCT_VERSION_P0)
				state = IERR_I2C_MEMORY;
		}
	}
	return state;
}

/* FUNCTION: 	FRAMWritePstVolume
 PARAMETERS:	---
 DESCRIPTION:	Schreiben des totalen Kolbenvolumens in den FRAM
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W (Aktion fehlgeschlagen) */
ErrorState_t FRAMWritePstVolume(void)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = WriteFloat32ToFRAM(FRAM_PAGE0_ADDR, FRAM_PAGE0_OFF_PSTVOLUME, g_udtLifePara.PstTotalVolume);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRAMReadPstVolume
 PARAMETERS:	---
 DESCRIPTION:	Lesen des Gesamtvolumens des Kopfes
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadPstVolume(void)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadFloat32FromFRAM(FRAM_PAGE0_ADDR, FRAM_PAGE0_OFF_PSTVOLUME, &g_udtLifePara.PstTotalVolume);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;
	return state;
}

/* FUNCTION: 	FRAMWritePstCycles
 PARAMETERS:	---
 DESCRIPTION:	Schreiben der Kolbenzyklen in den FRAM
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W (Aktion fehlgeschlagen) */
ErrorState_t FRAMWritePstCycles(void)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = WriteU8ToFRAM(FRAM_PAGE0_ADDR, sizeof(((FRAMStructureHeadPage0_t*)0)->ulngPstCycles), FRAM_PAGE0_OFF_PSTCYCLES, (uint8_t*) &g_udtLifePara.ulngPistonCycles);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRAMReadPstVolume
 PARAMETERS:	---
 DESCRIPTION:	Lesen der Kolbenzyklen des Kopfes
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadPstCycles(void)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadU8FromFRAM(FRAM_PAGE0_ADDR, sizeof(((FRAMStructureHeadPage0_t*)0)->ulngPstCycles), FRAM_PAGE0_OFF_PSTCYCLES, (uint8_t*) &g_udtLifePara.ulngPistonCycles);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;

	return state;
}

/* FUNCTION: 	FRAMWriteDevExchangeAtHead
 PARAMETERS:	---
 DESCRIPTION:	Schreiben der Gerätewechsel am Kopf in den FRAM
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_W (Aktion fehlgeschlagen) */
ErrorState_t FRAMWriteDevExchangeAtHead(const uint32_t * const cnt)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = WriteU8ToFRAM(FRAM_PAGE0_ADDR, sizeof(((FRAMStructureHeadPage0_t*)0)->ulngDevExchange), FRAM_PAGE0_OFF_DEVEXCHANGE, (uint8_t*) cnt);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_W;
	return state;
}

/* FUNCTION: 	FRAMReadDevExchangeAtHead
 PARAMETERS:	---
 DESCRIPTION:	Lesen der Gerätewechsel am Kopf
 RETURNS: 	CB_READY / IERR_I2C_HEAD_FRAM_R (Aktion fehlgeschlagen) */
ErrorState_t FRAMReadDevExchangeAtHead(uint32_t * const cnt)
{
	ErrorState_t state;
	bool i2c_state;

	i2c_state = ReadU8FromFRAM(FRAM_PAGE0_ADDR, sizeof(((FRAMStructureHeadPage0_t*)0)->ulngDevExchange), FRAM_PAGE0_OFF_DEVEXCHANGE,(uint8_t*) cnt);
	if (i2c_state)
		state = CB_READY;
	else
		state = IERR_I2C_HEAD_FRAM_R;
	return state;
}

/* FUNCTION: 	FRAMReadPageAndDisplay
 PARAMETERS:	---
 DESCRIPTION:	Lesen und Anzeigen der adressierten Speicherseite
 RETURNS: 	CB_READY / Fehler */
ErrorState_t FRAMReadPageAndDisplay(const uint8_t adr)
{
	bool i2c_state;
	uint8_t help;
	int16_t j, i;

	j = 0;
	printf("\rReading FRAM page %s:", adr == FRAM_PAGE0_ADDR ? "0" : "1");
	for (i = 0; i < 256; i++)
	{
		WDO();
		i2c_state = ReadU8FromFRAM(adr, 1, i, &help);
		if (i2c_state == FALSE)
			return IERR_I2C_HEAD_FRAM_R;
		if ((j % 10) == 0)
			printf("\rAdr: %3d ", (int16_t) i);
		printf("%3x", (int16_t) help);
		fflush(stdout);
		j++;
	}
	return CB_READY;
}

/* FUNCTION: 	FRAMGetSizeInByte
 PARAMETERS:	---
 DESCRIPTION:	Lesen der Größe des FRAMs (in bytes)
 RETURNS: 		Größe des FRAMs in bytes*/
uint32_t FRAMGetSizeInByte(void)
{
	return (FRAM_PAGES* FRAM_PAGE_SIZE);
}

/* FUNCTION: 	FRAMGetStructureSizeInByte
 PARAMETERS:	---
 DESCRIPTION:	Lesen der Größe der EEPROM Struktur (in bytes)
 RETURNS: 		Größe des FRAMs in bytes*/
uint32_t FRAMGetStructureSizeInByte(void)
{
	return (FRAM_PAGE0_WIRE_SIZE + FRAM_PAGE1_WIRE_SIZE);
}




/////////////////////////////////////////////////////////////////////////////////
/// \brief          Reads a block of data from FRAM memory starting at a given
///                 address, handling page boundaries internally.
/// \param[in]      address        Start address in FRAM to read from
/// \param[out]     pData          Pointer to buffer where read data will be stored
/// \param[in]      data_length    Number of bytes to read
/// \return         CB_READY if successful,
///                 IERR_PARA if parameters are invalid,
///                 IERR_I2C_HEAD_FRAM_R if I2C read fails
/////////////////////////////////////////////////////////////////////////////////
ErrorState_t FRAMRead(const uint8_t address, uint8_t * const pData, const uint16_t data_length)
{
    if (pData == NULL || data_length == 0) return IERR_PARA;
    if ((address + data_length) > (FRAM_PAGE_SIZE * FRAM_PAGES)) return IERR_PARA;

    uint16_t remaining = data_length;
    uint16_t current_offset = address;
    uint8_t *current_ptr = pData;

    while (remaining > 0)
    {
        uint8_t page_addr = (current_offset < FRAM_PAGE_SIZE) ? FRAM_PAGE0_ADDR : FRAM_PAGE1_ADDR;
        uint16_t page_offset = current_offset % FRAM_PAGE_SIZE;
        uint16_t chunk_size = FRAM_PAGE_SIZE - page_offset;

        if (chunk_size > remaining) chunk_size = remaining;

        if (!ReadU8FromFRAM(page_addr, chunk_size, page_offset, current_ptr))
        {
            return IERR_I2C_HEAD_FRAM_R;
        }

        current_offset += chunk_size;
        current_ptr += chunk_size;
        remaining -= chunk_size;
    }
    return CB_READY;
}


/////////////////////////////////////////////////////////////////////////////////
/// \brief          Writes a block of data to FRAM memory starting at a given
///                 address, handling page boundaries internally.
/// \param[in]      address        Start address in FRAM to write to
/// \param[in]      pData          Pointer to buffer containing data to write
/// \param[in]      data_length    Number of bytes to write
/// \return         CB_READY if successful,
///                 IERR_PARA if parameters are invalid,
///                 IERR_I2C_HEAD_FRAM_W if I2C write fails
/////////////////////////////////////////////////////////////////////////////////
ErrorState_t FRAMWrite(const uint8_t address, const uint8_t * const pData, const uint16_t data_length)
{
    if (pData == NULL || data_length == 0) return IERR_PARA;
    if ((address + data_length) > (FRAM_PAGE_SIZE * FRAM_PAGES)) return IERR_PARA;

    uint16_t remaining = data_length;
    uint16_t current_offset = address;
    const uint8_t *current_ptr = pData;

    while (remaining > 0)
    {
        uint8_t page_addr = (current_offset < FRAM_PAGE_SIZE) ? FRAM_PAGE0_ADDR : FRAM_PAGE1_ADDR;
        uint16_t page_offset = current_offset % FRAM_PAGE_SIZE;
        uint16_t chunk_size = FRAM_PAGE_SIZE - page_offset;

        if (chunk_size > remaining) chunk_size = remaining;

        if (!WriteU8ToFRAM(page_addr, chunk_size, page_offset, (uint8_t *)current_ptr))
        {
            return IERR_I2C_HEAD_FRAM_W;
        }

        current_offset += chunk_size;
        current_ptr += chunk_size;
        remaining -= chunk_size;
    }
    return CB_READY;
}

