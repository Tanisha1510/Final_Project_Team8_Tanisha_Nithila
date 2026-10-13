/*
 * EEPROM_Header.h
 *
 *  Created on: 12-Oct-2026
 *      Author: acer
 */

#ifndef EEPROM_HEADER_H_
#define EEPROM_HEADER_H_

#include <stdint.h>

void EEPROM_Init(void);
uint8_t EEPROM_Write(uint8_t memAddr,const uint8_t *data,uint8_t len);
void EEPROM_Read(uint8_t memAddr,uint8_t *data,uint8_t len);


#endif /* EEPROM_HEADER_H_ */
