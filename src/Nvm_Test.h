/*
 * Nvm_Test.h
 *
 *  Created on: 14-Oct-2026
 *      Author: acer
 */

#ifndef NVM_TEST_H_
#define NVM_TEST_H_
#include <stdint.h>
#include <stdint.h>
extern volatile uint8_t deferredResult;
extern volatile uint8_t immediateResult;
extern volatile uint8_t queueTestResult;
extern volatile uint8_t invalidBlockResult;
extern volatile uint8_t queueFullResult;
void Nvm_RunDeferredTest(void);
void Nvm_RunImmediateTest(void);
void Nvm_RunInvalidBlockTest(void);
void Nvm_RunQueueFullTest(void);

#endif /* NVM_TEST_H_ */
