/*
 * Oled_asci.h
 *
 *  Created on: 13-Oct-2026
 *      Author: acer
 */

#ifndef OLED_ASCI_H_
#define OLED_ASCI_H_
#include "sdk_project_config.h"

/* OLED I2C address */
#define OLED_ADDR 0x3CU

/* OLED control commands/data */
#define OLED_COMMAND 0x00U
#define OLED_DATA    0x40U

/* OLED function declarations */
void OLED_Write(uint8_t mode, uint8_t byte);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_PutString(const char *str, uint8_t page, uint8_t col);


#endif /* OLED_ASCI_H_ */
