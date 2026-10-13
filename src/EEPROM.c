/*
 * EEPROM.c
 *
 *  Created on: 12-Oct-2026
 *      Author: acer
 */

#include "EEPROM_Header.h"
#include "sdk_project_config.h"
#include <stddef.h>

#define EEPROM_ADDR (0xA0U >> 1)

uint8_t EEPROM_Write(uint8_t memAddr,const uint8_t *data, uint8_t len)
{
    uint8_t buf[16];
    if (data == NULL || len == 0U || len > 15U)
        return 0U;
    buf[0] = memAddr;

    for (uint8_t i = 0U; i < len; i++)
        buf[i + 1U] = data[i];

    LPI2C_DRV_MasterSetSlaveAddr(0U, EEPROM_ADDR, false);

    if (LPI2C_DRV_MasterSendDataBlocking( 0U, buf, len + 1U, true, 100U) != STATUS_SUCCESS)
        return 0U;
    OSIF_TimeDelay(1000U);
    return 1U;
}

void EEPROM_Read(uint8_t memAddr, uint8_t *data, uint8_t len)
{
    if (data == NULL || len == 0U)
        return;

    LPI2C_DRV_MasterSetSlaveAddr(0U, EEPROM_ADDR, false);
    LPI2C_DRV_MasterSendDataBlocking(0U, &memAddr, 1U, false, 100U);
    LPI2C_DRV_MasterReceiveDataBlocking(0U, data, len, true, 100U);
}
