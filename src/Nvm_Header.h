
#ifndef NVM_HEADER_H_
#define NVM_HEADER_H_

#include <stdint.h>

#define NVM_BLOCK_SIZE 8U
#define NVM_QUEUE_SIZE 4U
#define NVM_MAX_BLOCKS 3U

typedef enum
{
    IDLE,
    WRITE_PENDING,
    WRITE_BUSY,
    DONE
} NvM_State;

typedef enum
{
    DEFERRED,
    IMMEDIATE
} NvM_WriteMode;

typedef enum
{
    NVM_STATUS_IDLE,
    NVM_STATUS_PENDING,
    NVM_STATUS_BUSY,
    NVM_STATUS_DONE,
    NVM_STATUS_ERROR
} NvM_BlockStatus;

void NvM_Init(void);
void NvM_MainFunction(void);

uint8_t NvM_SetBlockMode(uint8_t blockId,NvM_WriteMode mode);
uint8_t NvM_WriteBlock(uint8_t blockId,const uint8_t *data);
NvM_BlockStatus NvM_GetBlockStatus(uint8_t blockId);

NvM_State NvM_GetState(void);
uint8_t NvM_GetCurrentBlock(void);
NvM_WriteMode NvM_GetCurrentMode(void);

#endif
