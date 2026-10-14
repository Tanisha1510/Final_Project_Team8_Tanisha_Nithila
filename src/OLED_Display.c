/*
 * OLED_Display.c
 *
 *  Created on: 14-Oct-2026
 *      Author: acer
 */
#include "sdk_project_config.h"
#include "Nvm_Header.h"
#include "Oled_asci.h"
#include "OLED_Display.h"

void OLED_ShowResult(const char *title, const char *result)
{
OLED_Clear();
OLED_PutString(title, 0U, 0U);
OLED_PutString(result, 2U, 0U);
OSIF_TimeDelay(4000U);
}

void OLED_UpdateStatus(void)
{
NvM_State state = NvM_GetState();
NvM_WriteMode mode = NvM_GetCurrentMode();
uint8_t block = NvM_GetCurrentBlock();
OLED_Clear();

switch (state)
{
    case IDLE:
        OLED_PutString("STATE: IDLE", 2U, 0U);
        break;
    case WRITE_PENDING:
        OLED_PutString("STATE: PENDING", 2U, 0U);
        break;
    case WRITE_BUSY:
        OLED_PutString("STATE: WRITING", 2U, 0U);
        break;
    case DONE:
        OLED_PutString("STATE: DONE", 2U, 0U);
        break;
    default:
        OLED_PutString("STATE: ERROR", 2U, 0U);
        break;
}

if (mode == IMMEDIATE)
    OLED_PutString("MODE: IMMEDIATE", 4U, 0U);
else
    OLED_PutString("MODE: DEFERRED", 4U, 0U);
if (block < NVM_MAX_BLOCKS)
{
    if (block == 0U)
        OLED_PutString("BLOCK: 0", 6U, 0U);
    else if (block == 1U)
        OLED_PutString("BLOCK: 1", 6U, 0U);
    else
        OLED_PutString("BLOCK: 2", 6U, 0U);
}
else
{
    OLED_PutString("BLOCK: NONE", 6U, 0U);
}

}


