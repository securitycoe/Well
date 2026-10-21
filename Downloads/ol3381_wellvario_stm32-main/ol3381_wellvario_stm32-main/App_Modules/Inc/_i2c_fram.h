/*
 * _i2c.h
 *
 *  Created on: 23.08.2022
 *      Author: Mario Kuban
 */

#ifndef INC__I2C_FRAM_H_
#define INC__I2C_FRAM_H_

/* Exported functions prototypes ---------------------------------------------*/
bool I2CWrite(ubyte adr, uword count, ubyte *pdata);
bool I2CRead(ubyte adr, uword count, ubyte *pdata);
bool I2CFRAMWrite(ubyte adr, uword count, const uword offset, ubyte *pdata);
bool I2CFRAMRead(ubyte adr, uword count, const uword offset, ubyte *pdata);
void I2CFRAMR_fill_with_Original_Device_Data(void);

#endif /* INC__I2C_FRAM_H_ */
