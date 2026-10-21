/*
 * _spi.c
 *
 *  Created on: 09.09.2022
 *      Author: Mario Kuban
 */

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions


#include <ig_define.h>
#include <_spi_eeprom.h>

extern SPI_HandleTypeDef hspi3;					// used for eeprom cat25640VI,64kbit,8k*8bit,Page = 64byte

uint8_t SPI3_EEPROM_WRITE_page(uint16_t MEMADR,uint8_t *WR_DATA,uint16_t SIZE); // A C H T U N G: Nur innerhalb einer Page (64 byte) verwendbar!

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_init
 *  PARAMETERS: none
 *  DESCRIPTION: Initialisierung des EEPROMs
 *  RETURNS: none
 ******************************************************************************** */
void SPI3_EEPROM_init(void){					// enable Access
	nSPI3_SS_H;									// verwenden falls kein Auto-SS verwendet werden kann
	nSPI3_Hold_H;								// not holding (LD1_OFF)
	nSPI3_WP_H;									// not write protected
	SPI3_EEPROM_WRSR(0);						// clear (all) protection bits
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_RDSR
 *  PARAMETERS: none
 *  DESCRIPTION: Lesen des Status-Registers
 *  RETURNS: uint8_t Status-Reigster
 ******************************************************************************** */
uint8_t SPI3_EEPROM_RDSR(void){
	uint8_t TxDATA[] = {cat25640_RDSR,0};
	uint8_t RxDATA[] = {0xff,0xff};
	cat25640_TransmitReceive(TxDATA,RxDATA,2);
	return (RxDATA[1]);
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_WAIT_RDY
 *  PARAMETERS: none
 *  DESCRIPTION: Warte auf Bereitschaft des EEprom
 *  RETURNS: none
 ******************************************************************************** */
void SPI3_EEPROM_WAIT_RDY(void){				// Wait of Ready (1 = Busy)
	while (SPI3_EEPROM_RDSR() & cat25640_nRDY) {};
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_WRITE_Enable
 *  PARAMETERS: none
 *  DESCRIPTION: Set Write_Enable_Latch Bit (WEL) = Enable Write Operations
 *  RETURNS: none
 ******************************************************************************** */
void SPI3_EEPROM_WRITE_Enable(void){
	SPI3_EEPROM_WAIT_RDY();
	uint8_t TxDATA[] = {cat25640_WREN};
	uint8_t RxDATA[] = {0xff};
	cat25640_TransmitReceive(TxDATA,RxDATA,1);
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_WRITE_Disable
 *  PARAMETERS: none
 *  DESCRIPTION: Reset Write_Enable_Latch Bit (WEL) = Disable Write Operations
 *  RETURNS: none
 ******************************************************************************** */
void SPI3_EEPROM_WRITE_Disable(void){
	SPI3_EEPROM_WAIT_RDY();
	uint8_t TxDATA[] = {cat25640_WRDI};
	uint8_t RxDATA[] = {0xff};
	cat25640_TransmitReceive(TxDATA,RxDATA,1);
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_WRSR
 *  PARAMETERS: uint8_t SR_DATA: zu schreibendes Status-Register
 *  DESCRIPTION: Schreiben des Status-Registers
 *  RETURNS: none
 ******************************************************************************** */
void SPI3_EEPROM_WRSR(uint8_t SR_DATA){			// Write Status Register
	SPI3_EEPROM_WAIT_RDY();
	SPI3_EEPROM_WRITE_Enable();					// (WEL = 1)
	uint8_t TxDATA[] = {cat25640_WRSR,SR_DATA};
	uint8_t RxDATA[] = {0xff,0xff};
	cat25640_TransmitReceive(TxDATA,RxDATA,2);
	// ----- Attention: -----
	// b0 (1) nRDY -> 1 = High,will indicate if the internal write cycle is in progress
	// b1 (2) WREN -> 0 = Low,After completion of the write cycle,the CAT25640 is returned to the write disable state.
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_READ
 *  PARAMETERS: MEMADR: Adresse im Speicher, RD_DATA: Pointer als Puffer, SIZE: Anzahl der Speicherblocks
 *  DESCRIPTION: Lesen des EEPROMs
 *  RETURNS: uint8_t: SPI3 Status
 ******************************************************************************** */
uint8_t SPI3_EEPROM_READ(uint16_t MEMADR,uint8_t *RD_DATA,uint16_t SIZE){
	unsigned char SPI3_STATUS;
	uint8_t txbuf[3];
	txbuf[0] = cat25640_READ;
	txbuf[1] = (MEMADR>>8)	& 0x1f;				// adr_max 8 kbyte (64kbit)
	txbuf[2]  = MEMADR		& 0xff;
	uint8_t rxbuf[3];
	SPI3_EEPROM_WAIT_RDY();
	HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,	nSPI3_SS_Pin,	GPIO_PIN_RESET);
	SPI3_STATUS = HAL_SPI_TransmitReceive(&hspi3,txbuf,rxbuf,3,3);

	uint8_t tx_dummy[SIZE];
	SPI3_STATUS = HAL_SPI_TransmitReceive(&hspi3,tx_dummy,RD_DATA,SIZE,3);
	HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,	nSPI3_SS_Pin,	GPIO_PIN_SET);

	return (SPI3_STATUS);
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_WRITE
 *  PARAMETERS: MEMADR: Adresse im Speicher, WR_DATA: Pointer als Puffer, SIZE: Anzahl der Speicherblocks
 *  DESCRIPTION: Beschreiben des EEPROMs
 *  RETURNS: uint8_t: SPI3-Status
 ******************************************************************************** */
uint8_t SPI3_EEPROM_WRITE(uint16_t MEMADR,uint8_t *WR_DATA,uint16_t SIZE){
	unsigned char SPI3_STATUS;
	// cat25640 has a 64−byte page write buffer!
	uint16_t PAGE_BEG = MEMADR;
	uint16_t PAGE_PNT = 0;
	uint8_t  PAGE_BUF[64];
	int i = 0;
	while ((PAGE_BEG +PAGE_PNT) < (MEMADR +SIZE)){
		//PAGE_BUF[PAGE_PNT] = WR_DATA[PAGE_BEG +PAGE_PNT];
		PAGE_BUF[PAGE_PNT] = WR_DATA[i];
		if ((((PAGE_BEG +PAGE_PNT) & 0x3f) == 0x3f) ||								// Pagegrenze erreicht
			((PAGE_BEG +PAGE_PNT +1) >= (MEMADR +SIZE))){							// Ende Auftrag erreicht
			SPI3_STATUS = SPI3_EEPROM_WRITE_page(PAGE_BEG,PAGE_BUF,PAGE_PNT +1);
			PAGE_BEG += PAGE_PNT +1;
			PAGE_PNT = 0;
		}else{
			PAGE_PNT++;
		}
		i++;
	}
	return (SPI3_STATUS);
}

/* ******************************************************************************
 *  FUNCTION: SPI3_EEPROM_WRITE_page
 *  PARAMETERS: MEMADR: Adresse im Speicher, WR_DATA: Pointer zu Daten, SIZE: Anzahl der Speicherblocks
 *  DESCRIPTION: Beschreiben des EEPROMs, maximale größe sind 64byte
 *  RETURNS: uint8_t: SPI3-Status
 ******************************************************************************** */
uint8_t SPI3_EEPROM_WRITE_page(uint16_t MEMADR,uint8_t *WR_DATA,uint16_t SIZE){
	// A C H T U N G: Nur innerhalb einer Page (64 byte) verwendbar!
	unsigned char SPI3_STATUS;
	uint8_t txbuf[3];
	SPI3_EEPROM_WRITE_Enable();

	txbuf[0] = cat25640_WRITE;
	txbuf[1] = (MEMADR>>8)	& 0x1f;				// adr_max 8 kbyte (64kbit)
	txbuf[2] = 	MEMADR		& 0xff;
	uint8_t rxbuf[3];
	SPI3_EEPROM_WAIT_RDY();

	HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,	nSPI3_SS_Pin,	GPIO_PIN_RESET);
	SPI3_STATUS = HAL_SPI_TransmitReceive(&hspi3,txbuf,rxbuf,3,3);
	uint8_t rx_dummy[SIZE];
	SPI3_STATUS = HAL_SPI_TransmitReceive(&hspi3,WR_DATA,rx_dummy,SIZE,3);
	HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,	nSPI3_SS_Pin,	GPIO_PIN_SET);

	return (SPI3_STATUS);
}


/* ******************************************************************************
 *  FUNCTION: cat25640_TransmitReceive
 *  PARAMETERS: txbuf: Pointer auf Daten,  rxbuf: Pointer als Puffer: size: Anzahl der Speicherblocks
 *  DESCRIPTION: Beschreiben/Lesen des EEPROMs, maximale Länge 64bytes
 *  RETURNS: uint8_t: SPI3-Status
 ******************************************************************************** */
//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  for other HALs,adapt the following subroutines:
//  A T T E N T I O N : cat25640 has a 64−byte page write buffer!
//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
uint8_t cat25640_TransmitReceive(uint8_t *txbuf,uint8_t *rxbuf,uint16_t Size){
	unsigned char SPI3_STATUS;
	HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,	nSPI3_SS_Pin,	GPIO_PIN_RESET);
	SPI3_STATUS = HAL_SPI_TransmitReceive(&hspi3,txbuf,rxbuf,Size,3);
	HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,	nSPI3_SS_Pin,	GPIO_PIN_SET);
	return (SPI3_STATUS);
}

/* ******************************************************************************
 *  FUNCTION: rd
 *  PARAMETERS: len: Anzahl der zu lesenden Bytes, offset: Startadresse,
 *  			dest: Puffer für Leseergebnis
 *  DESCRIPTION: Lesen eines Speicherblocks, Für Lesen von Struct-Inhalten
 *  			aus Eeprom gedacht
 *  RETURNS: bit: 0 -> Erfolgreich gelesen
 *  			  1 -> Fehler beim Lesen
 ******************************************************************************** */
//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  System Calls:
//	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bit rd(const unsigned short len,const unsigned int offset,unsigned char * dest){		// read eeprom (len, offset, destinatin)
	// uint8_t SPI3_EEPROM_READ(uint16_t MEMADR,uint8_t *RD_DATA,uint16_t SIZE)
	if (SPI3_EEPROM_READ((uint16_t)offset,dest+offset,len) != HAL_OK)
		return (1);	// error
	return (0);
}

/* ******************************************************************************
 *  FUNCTION: wr
 *  PARAMETERS: len: Anzahl der zu schreibenden Bytes, offset: Startadresse,
 *  			dest: Pointer auf Schreibdaten
 *  DESCRIPTION: Schreiben eines Speicherblocks, Für Schreiben von Struct-Inhalten
 *  			in den Eeprom gedacht
 *  RETURNS: bit: 0 -> Erfolgreich geschrieben
 *  			  1 -> Fehler beim Schreiben
 ******************************************************************************** */
bit wr(const unsigned short len,const unsigned int offset,unsigned char * src){			// write eeprom (len, offset, source)
	if (SPI3_EEPROM_WRITE((uint16_t)offset,src+offset,len) != HAL_OK) return (1);
	return (0);
}

/* ******************************************************************************
 *  FUNCTION: InitEEprom
 *  PARAMETERS: none
 *  DESCRIPTION: Eeprom Initialisierung
 *  RETURNS: none
 ******************************************************************************** */
void InitEEprom(void){											// Zugriff herstellen
																// alt: setze addressierungsmode auf byte
	SPI3_EEPROM_init();											// neu: freigabe hold & write_protection pins eeprom, freigabe schutzbits
}

/* ******************************************************************************
 *  FUNCTION: eral
 *  PARAMETERS: none
 *  DESCRIPTION: Überschreiben der ersten 512bytes (mehr wird nicht genutzt) mit 0xff
 *  RETURNS: bit: 0 -> Erfolgreich
 *  			  1 -> Fehler
 ******************************************************************************** */
bit eral(void){													// erase all (set all byte to 0xff)
	uint8_t WR_DATA[] = {0xff};
	for (uint16_t MEMADR = 0; MEMADR<512; MEMADR++){			// nur 512 byte löschen, bisheriger 99c66 hatte nur 4kbit (512x8 oder 256x16)
		if (SPI3_EEPROM_WRITE(MEMADR,WR_DATA,1) != HAL_OK) return (1);
	}
	return (0);
}

/* ******************************************************************************
 *  FUNCTION: eral0
 *  PARAMETERS: none
 *  DESCRIPTION: Überschreiben der ersten 512bytes (mehr wird nicht genutzt) mit 0x00
 *  RETURNS: bit: 0 -> Erfolgreich
 *  			  1 -> Fehler
 ******************************************************************************** */
bit eral0(void){												// set all byte to 0x00)
	uint8_t WR_DATA[] = {0x00};
	for (uint16_t MEMADR = 0; MEMADR<512; MEMADR++){			// nur 512 byte löschen, bisheriger 99c66 hatte nur 4kbit (512x8 oder 256x16)
		if (SPI3_EEPROM_WRITE(MEMADR,WR_DATA,1) != HAL_OK) return (1);
	}
	return (0);
}

/* ******************************************************************************
 *  FUNCTION: ReadEEprom
 *  PARAMETERS: Address: Leseadresse
 *  DESCRIPTION: Lesen eines Bytes
 *  RETURNS: unsigned char: Rückgabe des Lesewertes
 ******************************************************************************** */
unsigned char ReadEEprom(unsigned short Address){				// lese 1 byte
	uint8_t RD_DATA[1];
	SPI3_EEPROM_READ(Address,RD_DATA,1);
	return RD_DATA[0];
}

/* ******************************************************************************
 *  FUNCTION: ReadEEprom
 *  PARAMETERS: Data: zu schreibendes Byte Address: Schreibadresse
 *  DESCRIPTION: Schreiben eines Bytes
 *  RETURNS: bit: 0 -> Erfolgreich
 *  			  1 -> Fehler
 ******************************************************************************** */
bit WriteEEprom(unsigned char Data,unsigned short Address){		// schreibe 1 byte
	uint8_t WR_DATA[1];
	WR_DATA[0] = Data;
	if (SPI3_EEPROM_WRITE(Address,WR_DATA,1) != HAL_OK) return (1);
	return (0);
}

/* ******************************************************************************
 *  FUNCTION: ewen
 *  PARAMETERS: none
 *  DESCRIPTION: Dummy
 *  RETURNS: none
 ******************************************************************************** */
void ewen(void){												// erase/write enable
	;															// dummy, gegebenenfalls HW u/o SW Schreibschutz einbinden
}

/* ******************************************************************************
 *  FUNCTION: ewds
 *  PARAMETERS: none
 *  DESCRIPTION: Dummy
 *  RETURNS: none
 ******************************************************************************** */
void ewds(void){												// erase/write disable
	;															// dummy, gegebenenfalls HW u/o SW Schreibschutz einbinden
}


