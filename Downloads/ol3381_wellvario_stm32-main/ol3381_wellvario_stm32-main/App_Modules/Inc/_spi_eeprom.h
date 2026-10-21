/*
 * _spi.h
 *
 *  Created on: 09.09.2022
 *      Author: Mario Kuban
 */

#ifndef INC__SPI_EEPROM_H_
#define INC__SPI_EEPROM_H_

// ---------------------------------------------------------------------------
// cat25640
// 64kbit (8k x 1 byte)

// Instruction Sets
#define cat25640_RDSR	0b00000101		// Read Status Register
#define cat25640_WREN	0b00000110		// Enable Write Operations	(WEL = 1)
#define cat25640_WRDI	0b00000100		// Disable Write Operations	(WEL = 0)
#define cat25640_WRSR	0b00000001		// Write Status Register
#define cat25640_READ	0b00000011		// Read Data from Memory
#define cat25640_WRITE	0b00000010		// Write Data to Memory

// Status Bits
#define	cat25640_WPEN	0b10000000		// Write Protect Enable Bit
#define cat25640_BP1	0b00001000		// Block Protection Bits
#define cat25640_BP0	0b00000100
#define cat25640_WEL	0b00000010		// Write Enable Latch Bit
#define cat25640_nRDY	0b00000001		// Ready Bit (1 = Busy)

/* Exported functions prototypes ---------------------------------------------*/
void SPI3_EEPROM_init(void);				// enable Access
uint8_t SPI3_EEPROM_RDSR(void);				// Read Status Register
void SPI3_EEPROM_WAIT_RDY(void);			// Wait of Ready ( = not Busy)
void SPI3_EEPROM_WRITE_Enable(void);// Set Write Enable Latch Bit	(WEL = 1)
void SPI3_EEPROM_WRITE_Disable(void);// Reset Write Enable Latch Bit	(WEL = 0)
void SPI3_EEPROM_WRSR(uint8_t SR_DATA);		// Write Status Register

uint8_t SPI3_EEPROM_READ(uint16_t MEMADR, uint8_t *RD_DATA, uint16_t SIZE);
uint8_t SPI3_EEPROM_WRITE(uint16_t MEMADR, uint8_t *WR_DATA, uint16_t SIZE);

uint8_t cat25640_TransmitReceive(uint8_t *txbuf, uint8_t *rxbuf, uint16_t size);// HAL Translator

/* Exported functions prototypes ---------------------------------------------*/
// Schnittstellenaufrufe original (SEPROM.h/.asm) Konvertierung
bit rd(const unsigned short len, const unsigned int offset, unsigned char *dest);// read
bit wr(const unsigned short len, const unsigned int offset, unsigned char *src);// write

void InitEEprom(void);											// enable Access
bit eral(void);								// erase all (set all bytes to 0xff)
bit eral0(void);										// set all bytes to 0x00
unsigned char ReadEEprom(unsigned short Address);				// read 1 byte
bit WriteEEprom(unsigned char Data, unsigned short Address);	// write 1 byte

void ewen(void);										// erase/write enable
void ewds(void);										// erase/write disable

void SPI3_EEPROM_fill_with_Original_Device_Data(void);

#endif /* INC__SPI_EEPROM_H_ */
