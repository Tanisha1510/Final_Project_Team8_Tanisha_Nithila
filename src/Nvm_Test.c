#include "sdk_project_config.h"
#include "Nvm_Header.h"
#include "EEPROM_Header.h"
#include "OLED_Display.h"
#include "Nvm_Test.h"
#include "Oled_asci.h"

#include <stdint.h>
#include <string.h>

/* Test result flags */
volatile uint8_t deferredResult     = 0U;
volatile uint8_t immediateResult    = 0U;
volatile uint8_t queueTestResult    = 0U;
volatile uint8_t invalidBlockResult = 0U;
volatile uint8_t queueFullResult    = 0U;

/* Request acceptance flags */
volatile uint8_t deferredRequest0 = 0U;
volatile uint8_t deferredRequest1 = 0U;
volatile uint8_t immediateRequest = 0U;

/* Current test information */
volatile uint8_t activeTest  = 0U;
volatile uint8_t activeBlock = 0U;
volatile uint8_t activeMode  = 0U;

/* EEPROM block addresses */
volatile uint8_t block0Address = 0x10U;
volatile uint8_t block1Address = 0x20U;
volatile uint8_t block2Address = 0x30U;

/* Read-back verification flags */
volatile uint8_t block0Verify = 0U;
volatile uint8_t block1Verify = 0U;
volatile uint8_t block2Verify = 0U;

/* Test data: debugger-visible global arrays */
uint8_t deferredData0[NVM_BLOCK_SIZE] = "QUEUE001";
uint8_t deferredData1[NVM_BLOCK_SIZE] = "QUEUE002";
uint8_t immediateData[NVM_BLOCK_SIZE] = "IMMED001";

/* Read-back buffers: debugger-visible */
uint8_t deferredRead0[NVM_BLOCK_SIZE] = {0U};
uint8_t deferredRead1[NVM_BLOCK_SIZE] = {0U};
uint8_t immediateRead[NVM_BLOCK_SIZE] = {0U};

volatile uint8_t invalidBlockRequest = 0U;
volatile uint8_t queueFillCount = 0U;
volatile uint8_t queueFullRequest = 0U;
volatile uint8_t queueFullRejected = 0U;

uint8_t queueTestData[NVM_BLOCK_SIZE] = "QUEUE001";


void Nvm_RunDeferredTest(void)
{
    activeTest = 1U;
    activeMode = 1U;
    activeBlock = 0U;
    deferredResult = 0U;
    queueTestResult = 0U;
    deferredRequest0 = 0U;
    deferredRequest1 = 0U;
    block0Verify = 0U;
    block1Verify = 0U;
    memset(deferredRead0, 0, NVM_BLOCK_SIZE);
    memset(deferredRead1, 0, NVM_BLOCK_SIZE);
    NvM_SetBlockMode(0U, DEFERRED);
    NvM_SetBlockMode(1U, DEFERRED);
    OLED_Clear();
    OLED_PutString("DEFERRED WRITE", 0U, 0U);
    OLED_PutString("QUEUE REQUESTS", 2U, 0U);
    activeBlock = 0U;
    deferredRequest0 = NvM_WriteBlock(0U, deferredData0);
    activeBlock = 1U;
    deferredRequest1 = NvM_WriteBlock(1U, deferredData1);
    if ((deferredRequest0 != 1U) ||
        (deferredRequest1 != 1U))
    {
        OLED_ShowResult("DEFERRED WRITE", "QUEUE: FAIL");
        return;
    }
    while ((NvM_GetBlockStatus(0U) != NVM_STATUS_DONE) ||
           (NvM_GetBlockStatus(1U) != NVM_STATUS_DONE))
    {
        NvM_MainFunction();
        OLED_UpdateStatus();
    }
    activeBlock = 0U;
    EEPROM_Read(block0Address, deferredRead0, NVM_BLOCK_SIZE);
    activeBlock = 1U;
    EEPROM_Read(block1Address, deferredRead1, NVM_BLOCK_SIZE);
    block0Verify =(memcmp(deferredData0, deferredRead0,NVM_BLOCK_SIZE) == 0) ? 1U : 0U;
    block1Verify =(memcmp(deferredData1, deferredRead1,NVM_BLOCK_SIZE) == 0) ? 1U : 0U;
    if ((block0Verify == 1U) && (block1Verify == 1U))
    {
        deferredResult = 1U;
        queueTestResult = 1U;
        OLED_ShowResult("DEFERRED WRITE", "RESULT: PASS");
    }
    else
        OLED_ShowResult("DEFERRED WRITE", "VERIFY: FAIL");
}

void Nvm_RunImmediateTest(void)
{
    activeTest = 2U;
    activeMode = 0U;       /* IMMEDIATE */
    activeBlock = 2U;
    immediateResult = 0U;
    immediateRequest = 0U;
    block2Verify = 0U;
    memset(immediateRead, 0, NVM_BLOCK_SIZE);
    NvM_SetBlockMode(2U, IMMEDIATE);
    OLED_Clear();
    OLED_PutString("IMMEDIATE WRITE", 0U, 0U);
    OLED_PutString("STARTING...", 2U, 0U);

    immediateRequest = NvM_WriteBlock(2U, immediateData);
    if (immediateRequest != 1U)
    {
        OLED_ShowResult("IMMEDIATE WRITE", "WRITE: FAIL");
        return;
    }

    EEPROM_Read(block2Address, immediateRead, NVM_BLOCK_SIZE);
    block2Verify =(memcmp(immediateData, immediateRead,NVM_BLOCK_SIZE) == 0) ? 1U : 0U;
    if ((NvM_GetBlockStatus(2U) == NVM_STATUS_DONE) &&
        (block2Verify == 1U))
    {
        immediateResult = 1U;
        OLED_ShowResult("IMMEDIATE WRITE", "RESULT: PASS");
    }
    else
        OLED_ShowResult("IMMEDIATE WRITE", "VERIFY: FAIL");
    activeBlock = 2U;
}


void Nvm_RunInvalidBlockTest(void)
{
    activeTest = 3U;
    activeBlock = 3U;

    invalidBlockResult = 0U;
    invalidBlockRequest = NvM_WriteBlock(3U, queueTestData);

    if (invalidBlockRequest == 0U)
    {
        invalidBlockResult = 1U;
        OLED_ShowResult("INVALID BLOCK", "RESULT: PASS");
    }
    else
    {
        OLED_ShowResult("INVALID BLOCK", "RESULT: FAIL");
    }
}

void Nvm_RunQueueFullTest(void)
{
    activeTest = 4U;
    activeBlock = 0U;

    queueFullResult = 0U;
    queueFillCount = 0U;
    queueFullRequest = 0U;
    queueFullRejected = 0U;

    NvM_Init();

    NvM_SetBlockMode(0U, DEFERRED);
    NvM_SetBlockMode(1U, DEFERRED);
    NvM_SetBlockMode(2U, DEFERRED);

    if (NvM_WriteBlock(0U, queueTestData) == 1U)
        queueFillCount++;

    if (NvM_WriteBlock(1U, queueTestData) == 1U)
        queueFillCount++;

    if (NvM_WriteBlock(2U, queueTestData) == 1U)
        queueFillCount++;

    if (NvM_WriteBlock(0U, queueTestData) == 1U)
        queueFillCount++;

    queueFullRequest = NvM_WriteBlock(1U, queueTestData);

    if ((queueFillCount == NVM_QUEUE_SIZE) &&
        (queueFullRequest == 0U))
    {
        queueFullRejected = 1U;
        queueFullResult = 1U;

        OLED_ShowResult("QUEUE FULL", "RESULT: PASS");
    }
    else
    {
        OLED_ShowResult("QUEUE FULL", "RESULT: FAIL");
    }

    NvM_Init();
}
