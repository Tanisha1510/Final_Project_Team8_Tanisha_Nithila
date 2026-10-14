
#include "sdk_project_config.h"
#include "Nvm_Header.h"
#include "EEPROM_Header.h"
#include "Oled_asci.h"
#include "OLED_Display.h"
#include "Nvm_Test.h"

#include <stdint.h>

lpi2c_master_state_t lpi2c1MasterState;

int main(void)
{
    CLOCK_DRV_Init(&clockMan1_InitConfig0);
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0,g_pin_mux_InitConfigArr0);
    LPI2C_DRV_MasterInit(0U,&lpi2c0_MasterConfig0,&lpi2c1MasterState);
    OLED_Init();
    OLED_Clear();
    NvM_Init();
    OLED_ShowResult("NVM PROJECT", "INITIALIZED");
    OSIF_TimeDelay(1500U);
    OLED_Clear();
    OLED_PutString("TEST 1", 0U, 0U);
    OLED_PutString("DEFERRED WRITE", 2U, 0U);
    Nvm_RunDeferredTest();
    OSIF_TimeDelay(1500U);
    OLED_Clear();
    OLED_PutString("TEST 2", 0U, 0U);
    OLED_PutString("IMMEDIATE WRITE", 2U, 0U);
    Nvm_RunImmediateTest();
    OSIF_TimeDelay(1500U);
    OLED_Clear();
    OLED_PutString("TEST 3", 0U, 0U);
    OLED_PutString("INVALID BLOCK", 2U, 0U);
    Nvm_RunInvalidBlockTest();
    OSIF_TimeDelay(1500U);
    OLED_Clear();
    OLED_PutString("TEST 4", 0U, 0U);
    OLED_PutString("QUEUE FULL", 2U, 0U);
    Nvm_RunQueueFullTest();
    OSIF_TimeDelay(1500U);
    OLED_Clear();
    OLED_PutString("ALL TESTS DONE", 0U, 0U);
    if ((deferredResult == 1U) && (immediateResult == 1U) && (invalidBlockResult == 1U) && (queueFullResult == 1U))
        OLED_PutString("OVERALL: PASS", 2U, 0U);
    else
        OLED_PutString("CHECK DEBUGGER", 2U, 0U);
    while (1)
    {

    }
    return 0;
}
