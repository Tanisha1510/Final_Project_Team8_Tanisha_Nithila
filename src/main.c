#include "sdk_project_config.h"
#include "Nvm_Header.h"
#include "EEPROM_Header.h"
#include "Oled_asci.h"
#include <string.h>
#include <stdint.h>

lpi2c_master_state_t lpi2c1MasterState;

//(To test in expression window)
volatile uint8_t deferredResult     = 0U;
volatile uint8_t immediateResult    = 0U;
volatile uint8_t queueTestResult    = 0U;
volatile uint8_t invalidBlockResult = 0U;
volatile uint8_t queueFullResult    = 0U;

//Display a test result
static void OLED_ShowResult(const char *title, const char *result)
{
    OLED_Clear();
    OLED_PutString(title, 0U, 0U);
    OLED_PutString(result, 2U, 0U);
    OSIF_TimeDelay(4000U);
}

//Display current NvM state, mode and block
static void OLED_UpdateStatus(void)
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
        OLED_PutString("BLOCK: NONE", 6U, 0U);
}

int main(void)
{
    CLOCK_DRV_Init(&clockMan1_InitConfig0);
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0,g_pin_mux_InitConfigArr0);
    LPI2C_DRV_MasterInit(0U,&lpi2c0_MasterConfig0,&lpi2c1MasterState);

    OLED_Init();
    OLED_Clear();
    NvM_Init();

    OLED_ShowResult("NVM PROJECT", "INITIALIZED");

    /* TEST 1: DEFERRED WRITE: Write two blocks, then process them using. */

    uint8_t deferredData0[NVM_BLOCK_SIZE] = "QUEUE001";
    uint8_t deferredData1[NVM_BLOCK_SIZE] = "QUEUE002";

    uint8_t deferredRead0[NVM_BLOCK_SIZE] = {0};
    uint8_t deferredRead1[NVM_BLOCK_SIZE] = {0};

    OLED_Clear();
    OLED_PutString("DEFERRED WRITE", 0U, 0U);
    OLED_PutString("QUEUEING DATA", 2U, 0U);

    NvM_SetBlockMode(0U, DEFERRED);
    NvM_SetBlockMode(1U, DEFERRED);

    uint8_t deferredAccepted0 =NvM_WriteBlock(0U, deferredData0);
    uint8_t deferredAccepted1 =NvM_WriteBlock(1U, deferredData1);

    if ((deferredAccepted0 == 1U) &&(deferredAccepted1 == 1U))
    {
        OLED_Clear();
        OLED_PutString("DEFERRED WRITE", 0U, 0U);
        OLED_PutString("PROCESSING...", 2U, 0U);

        while ((NvM_GetBlockStatus(0U) != NVM_STATUS_DONE) ||(NvM_GetBlockStatus(1U) != NVM_STATUS_DONE))
        {
            NvM_MainFunction();
            OLED_UpdateStatus();
        }

        EEPROM_Read(0x10U, deferredRead0, NVM_BLOCK_SIZE);
        EEPROM_Read(0x20U, deferredRead1, NVM_BLOCK_SIZE);

        if ((memcmp(deferredData0, deferredRead0,NVM_BLOCK_SIZE) == 0) &&(memcmp(deferredData1, deferredRead1,NVM_BLOCK_SIZE) == 0))
        {
            deferredResult = 1U;
            queueTestResult = 1U;
            OLED_ShowResult("DEFERRED WRITE", "RESULT: PASS");
        }
        else
            OLED_ShowResult("DEFERRED WRITE", "VERIFY: FAIL");
    }
    else
        OLED_ShowResult("DEFERRED WRITE", "QUEUE: FAIL");

    /* TEST 2: IMMEDIATE WRITE: NvM_WriteBlock() processes the write internally and returns only after the status becomes DONE. */

    uint8_t immediateData[NVM_BLOCK_SIZE] = "IMMED001";
    uint8_t immediateRead[NVM_BLOCK_SIZE] = {0};

    NvM_SetBlockMode(2U, IMMEDIATE);

    OLED_Clear();
    OLED_PutString("IMMEDIATE WRITE", 0U, 0U);
    OLED_PutString("STARTING...", 2U, 0U);
    OSIF_TimeDelay(3000U);

    uint8_t immediateAccepted =NvM_WriteBlock(2U, immediateData);

    if (immediateAccepted == 1U)
    {
        EEPROM_Read(0x30U, immediateRead, NVM_BLOCK_SIZE);

        if ((NvM_GetBlockStatus(2U) == NVM_STATUS_DONE) &&(memcmp(immediateData, immediateRead,NVM_BLOCK_SIZE) == 0))
        {
            immediateResult = 1U;
            OLED_ShowResult("IMMEDIATE WRITE", "RESULT: PASS");
        }
        else
            OLED_ShowResult("IMMEDIATE WRITE", "VERIFY: FAIL");
    }
    else
    	OLED_ShowResult("IMMEDIATE WRITE", "WRITE: FAIL");

    OLED_Clear();
    OLED_PutString("ALL TESTS DONE", 0U, 0U);

    if ((deferredResult == 1U) &&
        (immediateResult == 1U) &&
        (invalidBlockResult == 1U) &&
        (queueFullResult == 1U))
    {
        OLED_PutString("OVERALL: PASS", 2U, 0U);
    }
    else
    {
        OLED_PutString("CHECK DEBUGGER", 2U, 0U);
    }

    while (1)
    {
        /* Keep final result visible */
    }
}
