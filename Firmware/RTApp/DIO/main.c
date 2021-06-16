/*
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Copyright (c) 2020 Atmark Techno, Inc.
 * 
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <ctype.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "mt3620-baremetal.h"
#include "mt3620-intercore.h"
#include "mt3620-timer.h"

#include "InterCoreComm.h"
#include "TimerUtil.h"
#include "PortManager.h"
#include "cactusphere_pwm.h" // output test

#define OK	1
#define NG	-1

// DIO gpio pin number/ID
const int DIPIN_0 = 0;
const int DIPIN_1 = 1;
const int DOPIN_0 = 0;
const int DOPIN_1 = 1;
static const int periodMs = 1;  // 1[ms] (for polling DIn pin's input level)


extern uint32_t StackTop; // &StackTop == end of TCM

static _Noreturn void DefaultExceptionHandler(void);
static _Noreturn void RTCoreMain(void);

// 1ms timer's interrupt handler
static void
Handle1msIrq(void)
{
    PortManager_Running();
    Gpt_LaunchTimerMs(TimerGpt1, periodMs, Handle1msIrq);
}

// ARM DDI0403E.d SB1.5.2-3
// From SB1.5.3, "The Vector table must be naturally aligned to a power of two whose alignment
// value is greater than or equal to (Number of Exceptions supported x 4), with a minimum alignment
// of 128 bytes.". The array is aligned in linker.ld, using the dedicated section ".vector_table".

// The exception vector table contains a stack pointer, 15 exception handlers, and an entry for
// each interrupt.
#define INTERRUPT_COUNT 100 // from datasheet
#define EXCEPTION_COUNT (16 + INTERRUPT_COUNT)
#define INT_TO_EXC(i_) (16 + (i_))
const uintptr_t ExceptionVectorTable[EXCEPTION_COUNT] __attribute__((section(".vector_table")))
__attribute__((used)) = {
    [0] = (uintptr_t)&StackTop,                // Main Stack Pointer (MSP)
    [1] = (uintptr_t)RTCoreMain,               // Reset
    [2] = (uintptr_t)DefaultExceptionHandler,  // NMI
    [3] = (uintptr_t)DefaultExceptionHandler,  // HardFault
    [4] = (uintptr_t)DefaultExceptionHandler,  // MPU Fault
    [5] = (uintptr_t)DefaultExceptionHandler,  // Bus Fault
    [6] = (uintptr_t)DefaultExceptionHandler,  // Usage Fault
    [11] = (uintptr_t)DefaultExceptionHandler, // SVCall
    [12] = (uintptr_t)DefaultExceptionHandler, // Debug monitor
    [14] = (uintptr_t)DefaultExceptionHandler, // PendSV
    [15] = (uintptr_t)DefaultExceptionHandler, // SysTick

    [INT_TO_EXC(0)] = (uintptr_t)DefaultExceptionHandler,
    [INT_TO_EXC(1)] = (uintptr_t)Gpt_HandleIrq1,
    [INT_TO_EXC(2)... INT_TO_EXC(INTERRUPT_COUNT - 1)] = (uintptr_t)DefaultExceptionHandler };

static _Noreturn void
DefaultExceptionHandler(void)
{
    for (;;) {
        // do nothing
    }
}

static _Noreturn void
RTCoreMain(void)
{
    // SCB->VTOR = ExceptionVectorTable
    WriteReg32(SCB_BASE, 0x08, (uint32_t)ExceptionVectorTable);

    // initialization
    if (! TimerUtil_Initialize()) {
        goto err;
    }
    if (! InterCoreComm_Initialize()) {
        goto err;
    }

    // for debugger connection (wait 3[sec])
    {
        uint32_t	prevTickCount = TimerUtil_GetTickCount();
        uint32_t	tickCount     = prevTickCount;

        while (3000 > tickCount - prevTickCount) {
            TimerUtil_SleepUntilIntr();
            tickCount = TimerUtil_GetTickCount();
        }
    }

    // initialize pulse counters and start the polling timer
    PortManager_PulseCounterInitialize(DIPIN_0, DIPIN_1);
    PortManager_PwmControllerInitialize(DOPIN_0, DOPIN_1);

    Gpt_LaunchTimerMs(TimerGpt1, periodMs, Handle1msIrq);

#if 0 // 2021-04-19 test
    // pulse counter set config
//    PortManager_PulseCounterSetConfig(DIPIN_0, true, 10, 100); // pulse counter
    PortManager_PulseCounterSetConfig(DIPIN_1, true, 10, 100); //

    /*single*/
//    PortManager_PwmControllerSetConfigSingle(DOPIN_1, true, 0, 0, 0, 0, 0, 1); // oneshot nonstop 
//    PortManager_PwmControllerSetConfigSingle(DOPIN_1, true, 1000 * 10, 1000 * 10, 0, 0, 0, 1); // oneshot
//    PortManager_PwmControllerSetConfigSingle(DOPIN_1, true, 0, 0, 2 * 1000 * 1000, 100, 0, 2); // pulse nonstop
//    PortManager_PwmControllerSetConfigSingle(DOPIN_1, true, 1000 * 10, 1000 * 10, 2 * 1000 * 1000, 100, 0, 2); // pwm

#if 0
    while (1) {
        bool status = false;
        PortManager_PwmControllerGetRelationStatus(DOPIN_1, &status);

        if (status) {
            status = !status;
        }
        else {
            status = !status;
        }
    }
#endif
    /*edge trigger*/
    // interlock
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 0, 50, DIPIN_0, 3);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 1, 900, DIPIN_0, 3);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 2, 900, DIPIN_0, 3);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 0, 0, 0, 0, 50, DIPIN_0, 3);

    // invert
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 0, 50, DIPIN_0, 4);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 1, 900, DIPIN_0, 4);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 2, 900, DIPIN_0, 4);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 0, 0, 0, 0, 50, DIPIN_0, 4);

    //generate
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 0, 50, DIPIN_0, 5);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 1, 900, DIPIN_0, 5);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 2, 900, DIPIN_0, 5);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 0, 0, 0, 0, 50, DIPIN_0, 5);
// 
    // pwm
    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 2 * 1000 * 1000, 100, 0, 0, 50, DIPIN_1, 2);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 2 * 1000 * 1000, 100, 0, 1, 50, DIPIN_0, 2);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 0, 0, 2 * 1000 * 1000, 100, 0, 2, 50, DIPIN_0, 2);
//    PortManager_PwmControllerSetConfigEdgeTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 2 * 1000 * 1000, 100, 0, 0, 50, DIPIN_0, 2);


/*count trigger*/
    // oneshot
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 10, 30, DIPIN_0, 1); // oneshot
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 0, 0, 0, 0, 0, 10, 0, DIPIN_0, 1);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 0, 0, 0, 10, 0, DIPIN_0, 1);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 0, 0, 0, 10, 60, DIPIN_0, 1);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 1000 * 60, 0, 0, 0, 10, 30, DIPIN_0, 1);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 0, 0, 0, 0, 10, 30, DIPIN_0, 1);

    //pwm
 //   PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 0, 0, 2 * 1000 * 1000, 100, 0, 10, 30, DIPIN_0, 2);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 0, 0, 2 * 1000 * 1000, 100, 0, 10, 0, DIPIN_0, 2);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 2 * 1000 * 1000, 100, 0, 10, 0, DIPIN_0, 2);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 1000 * 10, 2 * 1000 * 1000, 100, 0, 10, 60, DIPIN_0, 2);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 1000 * 60, 2 * 1000 * 1000, 100, 0, 10, 30, DIPIN_0, 2);
//    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000 * 10, 0, 2 * 1000 * 1000, 100, 0, 10, 30, DIPIN_0, 2);

    // reset
#if 0
    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000, 0, 2 * 1000 * 1000, 100, 0, 10, 0, DIPIN_0, 2);
    PortManager_PulseCounterClear(DIPIN_0, 0);
    PortManager_PwmControllerReset(DOPIN_1, 0);
#endif
 
#if 0
    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 1000, 0, 2 * 1000 * 1000, 100, 0, 10, 60, DIPIN_0, 2);
    PortManager_PulseCounterClear(DIPIN_0, 20);
    PortManager_PwmControllerReset(DOPIN_1, 20);
#endif

#if 0
    PortManager_PwmControllerSetConfigCountTrigger(DOPIN_1, true, 0, 0, 2 * 1000 * 1000, 100, 0, 0, 60, DIPIN_0, 2);
    PortManager_PulseCounterClear(DIPIN_0, 20);
    PortManager_PwmControllerReset(DOPIN_1, 20);
#endif

    // output
//    Cactusphere_PWM_SetOutput(8, false); // DI led off, 1kHz
//    Cactusphere_PWM_SetOutput(8, true); // DI led on, 1.6v low
//    Cactusphere_PWM_SetOutputPWM(8, 2 * 1000 * 1000, 100, 0); // 1.0002kHz 
//    Cactusphere_PWM_SetOutputPWM(8, 2 * 1000 * 1000, 1, 0); // 1.0002kHz 
//    Cactusphere_PWM_SetOutputPWM(8, 2 * 1000 * 1000, 0, 0); // 1.6v low
#endif// 2021-03-29 portmanager class test

    // main loop
    for (;;) {
        // wait and receive a request message from HLApp and process it
        const DIO_DriverMsg* msg = InterCoreComm_WaitAndRecvRequest();

        if (msg != NULL) {
            DIO_ReturnMsg    retMsg;
            int val;
            bool ret = false;
            bool status = false;

            switch (msg->header.requestCode) {
            case DI_SET_CONFIG_AND_START:
                ret = PortManager_PulseCounterSetConfig(
                    msg->body.setConfig.pinId, 
                    msg->body.setConfig.isPulseHigh,
                    msg->body.setConfig.minPulseWidth,
                    msg->body.setConfig.maxPulseCount
                );

                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                } else {
                    if (InterCoreComm_SendIntValue(OK)) {
                        // int i = 0;
                    }
                }
                break;
            case DO_SET_CONFIG_SINGLE:
                ret = PortManager_PwmControllerSetConfigSingle(
                    msg->body.setDOConfigSingle.pinId,
                    msg->body.setDOConfigSingle.outputLevel,
                    msg->body.setDOConfigSingle.delayTime,
                    msg->body.setDOConfigSingle.outputTime,
                    msg->body.setDOConfigSingle.pulseClock,
                    msg->body.setDOConfigSingle.pulseEffectiveTime,
                    msg->body.setDOConfigSingle.pulsePeriod,
                    msg->body.setDOConfigSingle.functionType);
                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(OK)) {
                        // int i = 0;
                    }
                }
                break;
            case DO_SET_CONFIG_EDGE_TRIGGER:
                ret = PortManager_PwmControllerSetConfigEdgeTrigger(
                msg->body.setDOConfigEdgeTrigger.pinId,
                msg->body.setDOConfigEdgeTrigger.outputLevel,
                msg->body.setDOConfigEdgeTrigger.delayTime,
                msg->body.setDOConfigEdgeTrigger.outputTime,
                msg->body.setDOConfigEdgeTrigger.pulseClock,
                msg->body.setDOConfigEdgeTrigger.pulseEffectiveTime,
                msg->body.setDOConfigEdgeTrigger.pulsePeriod,
                msg->body.setDOConfigEdgeTrigger.edgeType,
                msg->body.setDOConfigEdgeTrigger.chatteringVal,
                msg->body.setDOConfigEdgeTrigger.relationPort,
                msg->body.setDOConfigEdgeTrigger.functionType);
                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(OK)) {
                        // int i = 0;
                    }
                }
                break;
            case DO_SET_CONFIG_PULSECOUNT_TRIGGER:
                ret = PortManager_PwmControllerSetConfigCountTrigger(
                msg->body.setDOConfigCountTrigger.pinId,
                msg->body.setDOConfigCountTrigger.outputLevel,
                msg->body.setDOConfigCountTrigger.delayTime,
                msg->body.setDOConfigCountTrigger.outputTime,
                msg->body.setDOConfigCountTrigger.pulseClock,
                msg->body.setDOConfigCountTrigger.pulseEffectiveTime,
                msg->body.setDOConfigCountTrigger.pulsePeriod,
                msg->body.setDOConfigCountTrigger.startOutputCount,
                msg->body.setDOConfigCountTrigger.stopOutputCount,
                msg->body.setDOConfigCountTrigger.relationPort,
                msg->body.setDOConfigCountTrigger.functionType);
                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(OK)) {
                        // int i = 0;
                    }
                }
                break;
            case DO_STOP_OUTPUT:
                ret = PortManager_PwmControllerStopOutput(
                msg->body.pinId.pinId);
                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(OK)) {
                        // int i = 0;
                    }
                }
                break;
            case DO_READ_RELATIONSTATUS:
                ret = PortManager_PwmControllerGetRelationStatus(
                    msg->body.pinId.pinId, &status);
                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(status)) {
                        // int i = 0;
                    }
                }
                break;
            case DI_PULSE_COUNT_RESET:
                ret = PortManager_PulseCounterClear(
                    msg->body.resetPulseCount.pinId, 
                    msg->body.resetPulseCount.initVal);

                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    val = 1;
                    PortManager_PwmControllerReset(
                        msg->body.resetPulseCount.pinId,
                        msg->body.resetPulseCount.initVal);
                    if (InterCoreComm_SendIntValue(val)) {
                        // int i = 0;
                    }
                }
                break;
            case DI_READ_PULSE_COUNT:
                ret = PortManager_PulseCounterGetPulseCount(msg->body.pinId.pinId, &val);

                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(val)) {
                        // int i = 0;
                    }
                }
                break;
            case DI_READ_DUTY_SUM_TIME:
                ret = PortManager_PulseCounterGetPulseOnTime(msg->body.pinId.pinId, &val);

                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(val)) {
                        // int i = 0;
                    }
                }
                break;
            case DI_READ_PULSE_LEVEL:
                PortManager_PulseCounterGetLevel(retMsg.message.levels);
                retMsg.returnCode = OK;
                retMsg.messageLen = sizeof(retMsg.message.levels);
                if (InterCoreComm_SendReadData((uint8_t*)&retMsg, sizeof(DIO_ReturnMsg))) {
                    // int i = 0;
                }
                break;
            case DI_READ_PIN_LEVEL:
                ret = PortManager_PulseCounterGetPinLevel(msg->body.pinId.pinId, &val);

                if (!ret) {
                    InterCoreComm_SendIntValue(NG);
                    continue;
                }
                else {
                    if (InterCoreComm_SendIntValue(val)) {
                        // int i = 0;
                    }
                }
                break;
            case DIO_READ_VERSION:
                memset(retMsg.message.version, 0x00, sizeof(retMsg.message.version));
                strncpy(retMsg.message.version, RTAPP_VERSION, strlen(RTAPP_VERSION) + 1);
                retMsg.returnCode = OK;
                retMsg.messageLen = strlen(RTAPP_VERSION);
                if (InterCoreComm_SendReadData((uint8_t*)&retMsg, sizeof(DIO_ReturnMsg))) {
                    // int i = 0;
                }
                break;
            default:
                InterCoreComm_SendIntValue(NG);
                break;
            }
        }
    }

err:
    // error
    DefaultExceptionHandler();
}
