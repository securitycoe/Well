/*
 * _i2c.c
 *
 *  Created on: 23.08.2022
 *      Author: Mario Kuban
 */
#include "stdbool.h"

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Inc
#include <_i2c_fram.h>

extern I2C_HandleTypeDef hi2c1;

/*-------------------------------------------------------------------
	FT:	write data to I2C bus
	EP:	adr:	I2C bus address of slave to be written
		count:	number of bytes to be written
		pdata:	pointer to data to be written
	RV: TRUE = function succesfully; FALSE = function failed
-------------------------------------------------------------------*/
bool I2CWrite(ubyte adr,uword count,ubyte *pdata)
{
//	HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint8_t *pData,uint16_t Size,uint32_t Timeout)
	if (HAL_I2C_Master_Transmit(&hi2c1,(adr << 1),pdata,count,HAL_MAX_DELAY) == HAL_OK) return (TRUE);
	return (FALSE);
}

/*-------------------------------------------------------------------
	FT:	read data from I2C bus
	EP:	adr:	I2C bus address of slave to read from
		count:	number of bytes to be read
		pdata:	pointer to data to be read
	RV:	TRUE = function succesfully; FALSE = function failed
-------------------------------------------------------------------*/
bool I2CRead(ubyte adr,uword count,ubyte *pdata)
{
//	HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint8_t *pData,uint16_t Size,uint32_t Timeout)
	if (HAL_I2C_Master_Receive(&hi2c1,(adr << 1),pdata,count,HAL_MAX_DELAY) == HAL_OK) return (TRUE);
	return (FALSE);
}

/*-------------------------------------------------------------------
	FT:	write bytes into FRAM
	EP:	adr:	slave address
		count:	number of bytes to be write
		offset:	memory address of FRAM
		pdata:	pointer to writing data
	RV:	TRUE = function succesfully; FALSE = function failed

	Attention: use only "I2C_MEMADD_SIZE_8BIT"!
	Attention: Read with "I2C_MEMADD_SIZE_16BIT" OVERWRITE EEPROM Bytes!
-------------------------------------------------------------------*/
bool I2CFRAMWrite(ubyte adr,uword count,const uword offset,ubyte *pdata)
{
	if (HAL_I2C_IsDeviceReady(&hi2c1,(adr << 1) ,5 ,100) != HAL_OK) return (FALSE);

	if (HAL_I2C_Mem_Write(&hi2c1,(adr << 1),offset,I2C_MEMADD_SIZE_8BIT,pdata,count,HAL_MAX_DELAY) == HAL_OK) return (TRUE);
	return (FALSE);
}

/*-------------------------------------------------------------------
	FT:	read data from FRAM
	EP:	adr:	I2C bus address of slave to read from
		count:	number of bytes to be read
		offset:	memory address of FRAM
		pdata:	pointer to data array for reading data
	RV:	TRUE = function succesfully; FALSE = function failed

	Attention: use only "I2C_MEMADD_SIZE_8BIT"!
-------------------------------------------------------------------*/
bool I2CFRAMRead(ubyte adr,uword count,const uword offset,ubyte *pdata)
{
	if (HAL_I2C_IsDeviceReady(&hi2c1,(adr << 1) ,5 ,100) != HAL_OK) return (FALSE);

	if (HAL_I2C_Mem_Read(&hi2c1,(adr << 1),offset,I2C_MEMADD_SIZE_8BIT,pdata,count,HAL_MAX_DELAY) == HAL_OK) return (TRUE);
	return (FALSE);
}

/************************ (C) COPYRIGHT Ilmotronics GmbH *****END OF FILE****/
