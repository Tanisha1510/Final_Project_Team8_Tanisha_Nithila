
#include "Nvm_Header.h"
#include "EEPROM_Header.h"
#include <stddef.h>

typedef struct
{
    uint8_t id;
    uint8_t data[NVM_BLOCK_SIZE];
} QueueEntry;

static QueueEntry queue[NVM_QUEUE_SIZE];
static uint8_t front = 0U, rear = 0U, count = 0U;
static NvM_State state = IDLE;
static NvM_WriteMode mode[NVM_MAX_BLOCKS];
static NvM_BlockStatus status[NVM_MAX_BLOCKS];
static uint8_t currentBlock;
static uint8_t currentData[NVM_BLOCK_SIZE];

static const uint8_t blockAddress[NVM_MAX_BLOCKS] ={0x10U, 0x20U, 0x30U};

void NvM_Init(void)
{
    front = rear = count = 0U;
    state = IDLE;

    for (uint8_t i = 0U; i < NVM_MAX_BLOCKS; i++)
    {
        mode[i] = DEFERRED;
        status[i] = NVM_STATUS_IDLE;
    }
}

uint8_t NvM_SetBlockMode(uint8_t id, NvM_WriteMode m)
{
    if (id >= NVM_MAX_BLOCKS ||
        (m != IMMEDIATE && m != DEFERRED))
        return 0U;
    mode[id] = m;
    return 1U;
}

NvM_BlockStatus NvM_GetBlockStatus(uint8_t id)
{
    return (id < NVM_MAX_BLOCKS) ?status[id] : NVM_STATUS_ERROR;
}

uint8_t NvM_WriteBlock(uint8_t id, const uint8_t *data)
{
    if (id >= NVM_MAX_BLOCKS || data == NULL ||count >= NVM_QUEUE_SIZE)
        return 0U;

    queue[rear].id = id;

    for (uint8_t i = 0U; i < NVM_BLOCK_SIZE; i++)
        queue[rear].data[i] = data[i];

    rear = (rear + 1U) % NVM_QUEUE_SIZE;
    count++;
    status[id] = NVM_STATUS_PENDING;

    if (mode[id] == IMMEDIATE)
    {
        while (status[id] != NVM_STATUS_DONE &&status[id] != NVM_STATUS_ERROR)
            NvM_MainFunction();
        return (status[id] == NVM_STATUS_DONE) ? 1U : 0U;
    }
    return 1U;
}

void NvM_MainFunction(void)
{
    switch (state)
    {
        case IDLE:
            if (count > 0U)
                state = WRITE_PENDING;
            break;

        case WRITE_PENDING:
            currentBlock = queue[front].id;

            for (uint8_t i = 0U; i < NVM_BLOCK_SIZE; i++)
                currentData[i] = queue[front].data[i];

            status[currentBlock] = NVM_STATUS_BUSY;
            state = WRITE_BUSY;
            break;

        case WRITE_BUSY:
            status[currentBlock] =(EEPROM_Write(blockAddress[currentBlock],currentData,NVM_BLOCK_SIZE) == 1U)? NVM_STATUS_DONE : NVM_STATUS_ERROR;
            state = DONE;
            break;

        case DONE:
            front = (front + 1U) % NVM_QUEUE_SIZE;
            count--;
            state = IDLE;
            break;

        default:
            state = IDLE;
            break;
    }
}

NvM_State NvM_GetState(void)
{
    return state;
}

uint8_t NvM_GetCurrentBlock(void)
{
    return currentBlock;
}

NvM_WriteMode NvM_GetCurrentMode(void)
{
    return (currentBlock < NVM_MAX_BLOCKS) ? mode[currentBlock] : DEFERRED;

}
