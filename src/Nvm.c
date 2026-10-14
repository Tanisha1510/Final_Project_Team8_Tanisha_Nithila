
#include "Nvm_Header.h"
#include "EEPROM_Header.h"

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint8_t id;
    uint8_t data[NVM_BLOCK_SIZE];
} QueueEntry;

static QueueEntry queue[NVM_QUEUE_SIZE];
static uint8_t front = 0U;
static uint8_t rear  = 0U;
static uint8_t count = 0U;

static NvM_State state = IDLE;
static NvM_WriteMode mode[NVM_MAX_BLOCKS];
static NvM_BlockStatus status[NVM_MAX_BLOCKS];
static uint8_t currentBlock = 0U;
static uint8_t currentData[NVM_BLOCK_SIZE];

static const uint8_t blockAddress[NVM_MAX_BLOCKS] ={0x10U,0x20U,0x30U};
void NvM_Init(void)
{
    uint8_t i;
    front = 0U;
    rear = 0U;
    count = 0U;
    state = IDLE;
    currentBlock = 0U;
    for (i = 0U; i < NVM_MAX_BLOCKS; i++)
    {
        mode[i] = DEFERRED;
        status[i] = NVM_STATUS_IDLE;
    }
}

uint8_t NvM_SetBlockMode(uint8_t id, NvM_WriteMode m)
{
    if ((id >= NVM_MAX_BLOCKS) ||((m != IMMEDIATE) && (m != DEFERRED)))
        return 0U;
    mode[id] = m;
    return 1U;
}
NvM_BlockStatus NvM_GetBlockStatus(uint8_t id)
{
    if (id >= NVM_MAX_BLOCKS)
        return NVM_STATUS_ERROR;
    return status[id];
}

uint8_t NvM_WriteBlock(uint8_t id, const uint8_t *data)
{
    uint8_t i;
    if ((id >= NVM_MAX_BLOCKS) || (data == NULL))
        return 0U;

    if (count >= NVM_QUEUE_SIZE)
        return 0U;

    queue[rear].id = id;
    for (i = 0U; i < NVM_BLOCK_SIZE; i++)
    {
        queue[rear].data[i] = data[i];
    }
    rear = (uint8_t)((rear + 1U) % NVM_QUEUE_SIZE);
    count++;
    status[id] = NVM_STATUS_PENDING;

    if (mode[id] == IMMEDIATE)
    {
        while ((status[id] != NVM_STATUS_DONE) &&(status[id] != NVM_STATUS_ERROR))
        {
            NvM_MainFunction();
        }
        return (status[id] == NVM_STATUS_DONE) ? 1U : 0U;
    }
    return 1U;
}

void NvM_MainFunction(void)
{
    uint8_t i;
    uint8_t writeResult;
    switch (state)
    {
        case IDLE:
            if (count > 0U)
                state = WRITE_PENDING;
            break;

        case WRITE_PENDING:
            if (count == 0U)
            {
                state = IDLE;
                break;
            }
            currentBlock = queue[front].id;
            if (currentBlock >= NVM_MAX_BLOCKS)
            {
                state = DONE;
                break;
            }
            for (i = 0U; i < NVM_BLOCK_SIZE; i++)
            {
                currentData[i] = queue[front].data[i];
            }
            status[currentBlock] = NVM_STATUS_BUSY;
            state = WRITE_BUSY;
            break;
        case WRITE_BUSY:
            writeResult = EEPROM_Write(
                blockAddress[currentBlock],
                currentData,
                NVM_BLOCK_SIZE
            );
            if (writeResult == 1U)
                status[currentBlock] = NVM_STATUS_DONE;
            else
                status[currentBlock] = NVM_STATUS_ERROR;
            state = DONE;
            break;
        case DONE:
            if (count > 0U)
            {
                front = (uint8_t)((front + 1U) % NVM_QUEUE_SIZE);
                count--;
            }
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
    if (currentBlock < NVM_MAX_BLOCKS)
        return mode[currentBlock];

    return DEFERRED;
}
