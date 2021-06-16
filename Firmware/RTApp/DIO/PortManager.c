/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Atmark Techno, Inc.
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

#include "PortManager.h"
#include "PulseCounter.h"
#include "PwmController.h"

#include "lib/GPIO.h" // DI initialize

#define NUM_DI	2	// num of DIO ports
#define NUM_DO	2	// num of DIO ports
#define NUM_DIO	(NUM_DI + NUM_DO)

const int  DIPORT_0 = 12;
const int  DIPORT_1 = 15;
const int  DOPORT_0 = 0;
const int  DOPORT_1 = 8;

 // DIO gpio pin number/ID
static PulseCounter sPulseCounter[NUM_DI];
static PwmController sPwmController[NUM_DO];


//
// private - Get PulseCounter*
// 
static PulseCounter*
PortManager_GetTargetDI(int pinId)
{
    // return PulseCounter which specified with the DIn pin ID
    for (int i = 0; i < NUM_DI; i++) {
        if (pinId == PulseCounter_GetPinId(&sPulseCounter[i])) {
            return &sPulseCounter[i];
        }
    }
    return NULL;
}

//
// private - Get PwmController*
// 
static PwmController*
PortManager_GetTargeDO(int pinId)
{
    // return PWMController which specified with the DOut pin ID
    for (int i = 0; i < NUM_DO; i++) {
        if (pinId == PwmController_GetPinId(&sPwmController[i])) {
            return &sPwmController[i];
        }
    }
    return NULL;
}

//
// private - Check valid relational port
// 
static bool
PortManager_CheckRelationPort(int pinId) {
    for (int i = 0; i < NUM_DI; i++) {
        if (pinId == PulseCounter_GetPinId(&sPulseCounter[i])) {
            return true;
        }
    }
    return false;
}

//
// private - Check pulseclock 
//
static int
PortManager_CheckPulseClock(int pulseClock) {
    if (pulseClock == PulseClock_32KHz ||
        pulseClock == PulseClock_2MHz) {
        return pulseClock;
    }
    return PulseClock_2MHz;
}

//
// private - PwmControllerSetConfig
// 
static bool
PortManager_PwmControllerSetConfig(
    int pinId,
    bool outputLevel,
    int delayTime,
    int outputTime,
    int relationPort,
    bool enableRelation,
    TriggerType triggerType,
    PulseEdgeTrigger pulseEdgeTrigger,
    PulseCountTrigger pulseCountTrigger,
    PwmSettings pwmSettings,
    FunctionType functionType)
{
    PwmController* targetDO = PortManager_GetTargeDO(pinId);

    if (targetDO == NULL) {
        return false;
    }
    else {
        // relation port check 
        if (enableRelation) {
            if (!PortManager_CheckRelationPort(relationPort)) {
                return false; // illegal port
            }
        }

        // set config
        PwmController_SetConfig(
            targetDO,
            outputLevel,
            delayTime,
            outputTime,
            relationPort,
            enableRelation,
            triggerType,
            pulseEdgeTrigger,
            pulseCountTrigger,
            pwmSettings,
            functionType
        );
    }
    return true;
}

//
// private - PwmController running when enable relational
// 
static void
PortManager_RelationalFunc(PwmController* targetDO) {
    PulseCounter* targetDI = PortManager_GetTargetDI(targetDO->relationPort);
    bool currRising = PulseCounter_GetEdge(targetDI);

    switch (targetDO->step) {
    case Step_Start:
        switch (targetDO->triggerType) {
        case TriggerType_NotSelected:
            break;
        case TriggerType_PulseEdge:
#if 1 // 2021-04-28
            if (!targetDO->pulseEdgeTrigger.isStart) {
                if (targetDO->pulseEdgeTrigger.prevEdge != currRising) {
                    targetDO->pulseEdgeTrigger.isStart = true;
                }
                else {
                    break;
                }
            }
#endif // 2021-04-28
            switch (targetDO->pulseEdgeTrigger.edgeType) {
            case EdgeType_Rising:
                if (currRising) {
                    targetDO->pulseEdgeTrigger.edgeElapsedTime++;
                    if (targetDO->pulseEdgeTrigger.edgeElapsedTime > targetDO->pulseEdgeTrigger.chatteringVal) {
                        PwmController_Start(targetDO);
                        PwmController_SetTrigger(targetDO);
                    }
                }
                else {
                    targetDO->pulseEdgeTrigger.edgeElapsedTime = 0;
                }
                break;
            case EdgeType_Falling:
                if (!currRising) {
                    targetDO->pulseEdgeTrigger.edgeElapsedTime++;
                    if (targetDO->pulseEdgeTrigger.edgeElapsedTime > targetDO->pulseEdgeTrigger.chatteringVal) {
                        PwmController_Start(targetDO);
                        PwmController_SetTrigger(targetDO);
                    }
                }
                else {
                    targetDO->pulseEdgeTrigger.edgeElapsedTime = 0;
                }
                break;
            case EdgeType_Both:
                if ((currRising == targetDO->pulseEdgeTrigger.prevEdge) ||
                    (!currRising == targetDO->pulseEdgeTrigger.prevEdge)) {
                    targetDO->pulseEdgeTrigger.edgeElapsedTime++;
                    if (targetDO->pulseEdgeTrigger.edgeElapsedTime > targetDO->pulseEdgeTrigger.chatteringVal) {
                        PwmController_Start(targetDO);
                        PwmController_SetTrigger(targetDO);
                    }
                }
                else {
                    targetDO->pulseEdgeTrigger.prevEdge = currRising;
                    targetDO->pulseEdgeTrigger.edgeElapsedTime = 0;
                }
                break;
             } // switch case edgeType
            break; // switch case pulse edge
        case TriggerType_PulseCount:
            if (targetDO->pulseCountTrigger.startOutputCount <= PulseCounter_GetPulseCount(targetDI)) {
                PwmController_Start(targetDO);
                PwmController_SetTrigger(targetDO);
            }
            break;
        } // switch case triggerType
        break;
    case Step_Delay:
        PwmController_Delay(targetDO);
        break;
    case Step_Output:
        switch (targetDO->triggerType) {
            bool level = false;
        case TriggerType_NotSelected:
            break;
        case TriggerType_PulseEdge:
            switch (targetDO->functionType) {
            case FunctionType_Interlock:
                level = PulseCounter_GetLevel(targetDI);
                PwmController_Output(targetDO, level, false);
                break;
            case FunctionType_Invert:
                level = PulseCounter_GetLevel(targetDI);
                PwmController_Output(targetDO, !level, false);
                break;
            case FunctionType_Genarate:
                PwmController_Output(targetDO, targetDO->outputLevel, true);
                break;
            case FunctionType_Pulse:
                PwmController_Output(targetDO, targetDO->outputLevel, true);
                break;
            case FunctionType_NotSelected:
            case FunctionType_Oneshot:
                break;
            }
            PwmController_Running(targetDO); // use interlock, invert 
            break;
        case TriggerType_PulseCount:
            PwmController_Output(targetDO, targetDO->outputLevel, true);
            break;
        }
        break;
    case Step_Running:
        switch (targetDO->triggerType) {
        case TriggerType_PulseCount:
            if (targetDO->outputTime == 0 && 
                targetDO->pulseCountTrigger.stopOutputCount > 0 &&
                targetDO->pulseCountTrigger.stopOutputCount < PulseCounter_GetPulseCount(targetDI)) {
                PwmController_Stop(targetDO);
            }
        case TriggerType_NotSelected:
        case TriggerType_PulseEdge:
            PwmController_Running(targetDO);
            break;
        }
#if 1 // 2021-04-28
        break;
#endif // 2021-04-28
    case Step_Stop:
#if 1 // 2021-04-28
        if (targetDO->triggerType == TriggerType_PulseEdge) {
            targetDO->step = Step_Start;
            targetDO->relationalStatus = false;
            targetDO->pulseEdgeTrigger.edgeElapsedTime = 0;
            targetDO->pulseEdgeTrigger.isStart = false;
            targetDO->pulseEdgeTrigger.prevEdge = currRising;
        }
        else if(targetDO->triggerType == TriggerType_PulseCount &&
            PulseCounter_GetPulseCount(targetDI) >= targetDI->maxPulseCounter) {
            targetDO->step = Step_Start;
            targetDO->relationalStatus = false;
        }
        break;
#endif // 2021-04-28
    case Step_Initialize:
        break;
    }
}

// 
// Initialize
//
void
PortManager_PulseCounterInitialize(int pin0, int pin1) {
     PulseCounter_Initialize(&sPulseCounter[0], pin0, DIPORT_0);
     PulseCounter_Initialize(&sPulseCounter[1], pin1, DIPORT_1);
     GPIO_ConfigurePinForInput(DIPORT_0);
     GPIO_ConfigurePinForInput(DIPORT_1);
}

void 
PortManager_PwmControllerInitialize(int pin0, int pin1) {
    PwmController_Initialize(&sPwmController[0], pin0, DOPORT_0);
    PwmController_Initialize(&sPwmController[1], pin1, DOPORT_1);
}

//
// Set config and start
//
bool
PortManager_PulseCounterSetConfig(int pinId, bool isCountHigh, int minPulse, int maxPulse) {
    PulseCounter* targetDI = PortManager_GetTargetDI(pinId);

    if (targetDI == NULL) {
        return false;
    }
    else {
        PulseCounter_SetConfigCounter(targetDI, isCountHigh, minPulse, maxPulse);
    }
    return true;
}

//
// Pwm set config single
//
bool
PortManager_PwmControllerSetConfigSingle(
    int pinId,
    bool outputLevel,
    int delayTime,
    int outputTime,
    int pulseClock,
    int pulseEffectiveTime,
    int pulsePeriod,
    int functionType)
{
    PulseEdgeTrigger pulseEdgeTrigger = { 0 };
    PulseCountTrigger pulseCountTrigger = { 0 };
    PwmSettings pwmSettings = { 0 };

    pwmSettings.pulseClock = PortManager_CheckPulseClock(pulseClock);
    pwmSettings.pulseEffectiveTime = pulseEffectiveTime;
    pwmSettings.pulsePeriod = pulsePeriod;

    if (!(functionType == FunctionType_Oneshot ||
        functionType == FunctionType_Pulse)) {
        return false; // error function type
    }

    return PortManager_PwmControllerSetConfig(
    pinId,
    outputLevel,
    delayTime,
    outputTime,
    -1,
    false,
    TriggerType_NotSelected,
    pulseEdgeTrigger,
    pulseCountTrigger,
    pwmSettings,
    functionType);
}

//
// Pwm set config edge trigger
//
bool
PortManager_PwmControllerSetConfigEdgeTrigger(
    int pinId,
    bool outputLevel,
    int delayTime,
    int outputTime,
    int pulseClock,
    int pulseEffectiveTime,
    int pulsePeriod,
    int edgeType,
    int chatteringVal,
    int relationPort,
    int functionType) 
{
    PulseEdgeTrigger pulseEdgeTrigger = { 0 };
    PulseCountTrigger pulseCountTrigger = { 0 };
    PwmSettings pwmSettings = { 0 };
#if 1 // 2021-04-28
    PulseCounter* targetDI = PortManager_GetTargetDI(relationPort);
    bool currRising = PulseCounter_GetEdge(targetDI);
    pulseEdgeTrigger.prevEdge = currRising;
    pulseEdgeTrigger.isStart = false;
#endif // 2021-04-28
    pulseEdgeTrigger.edgeType = edgeType;
    pulseEdgeTrigger.chatteringVal = chatteringVal;

    pwmSettings.pulseClock = PortManager_CheckPulseClock(pulseClock);;
    pwmSettings.pulseEffectiveTime = pulseEffectiveTime;
    pwmSettings.pulsePeriod = pulsePeriod;

    if (!(functionType == FunctionType_Interlock ||
        functionType == FunctionType_Invert ||
        functionType == FunctionType_Genarate ||
        functionType == FunctionType_Pulse)) {
        return false; // error function type
    }

    if (edgeType < EdgeType_Rising || edgeType > EdgeType_Both) {
        return false; // error edge type
    }

    return PortManager_PwmControllerSetConfig(
        pinId,
        outputLevel,
        delayTime,
        outputTime,
        relationPort,
        true,
        TriggerType_PulseEdge,
        pulseEdgeTrigger,
        pulseCountTrigger,
        pwmSettings,
        functionType);
}

//
// Pwm set config count trigger
//
bool
PortManager_PwmControllerSetConfigCountTrigger(
    int pinId,
    bool outputLevel,
    int delayTime,
    int outputTime,
    int pulseClock,
    int pulseEffectiveTime,
    int pulsePeriod,
    int startOutputCount,
    int stopOutputCount,
    int relationPort,
    int functionType)
{
    PulseEdgeTrigger pulseEdgeTrigger = { 0 };
    PulseCountTrigger pulseCountTrigger = { 0 };
    PwmSettings pwmSettings = { 0 };

    pulseCountTrigger.startOutputCount = startOutputCount;
    pulseCountTrigger.stopOutputCount = stopOutputCount;

    pwmSettings.pulseClock = PortManager_CheckPulseClock(pulseClock);
    pwmSettings.pulseEffectiveTime = pulseEffectiveTime;
    pwmSettings.pulsePeriod = pulsePeriod;

    if (!(functionType == FunctionType_Oneshot ||
        functionType == FunctionType_Pulse)) {
        return false; // error function type
    }

    return PortManager_PwmControllerSetConfig(
        pinId,
        outputLevel,
        delayTime,
        outputTime,
        relationPort,
        true,
        TriggerType_PulseCount,
        pulseEdgeTrigger,
        pulseCountTrigger,
        pwmSettings,
        functionType);
}

//
// Pulse Counter - Clear
//
bool
PortManager_PulseCounterClear(int pinId, int initVal) {
    PulseCounter* targetDI = PortManager_GetTargetDI(pinId);

    if (targetDI == NULL) {
        return false;
    }
    else {
        PulseCounter_Clear(targetDI, initVal);
    }
    return true;
}

//
// Pulse Counter - GetPulseCount
//
bool
PortManager_PulseCounterGetPulseCount(int pinId, int* val) {
    PulseCounter* targetDI = PortManager_GetTargetDI(pinId);

    if (targetDI == NULL) {
        return false;
    }
    else {
        *val = PulseCounter_GetPulseCount(targetDI);
    }
    return true;
}

//
// Pulse Counter - GetPulseOnTime
//
bool
PortManager_PulseCounterGetPulseOnTime(int pinId, int* val) {
    PulseCounter* targetDI = PortManager_GetTargetDI(pinId);

    if (targetDI == NULL) {
        return false;
    }
    else {
        *val = PulseCounter_GetPulseOnTime(targetDI);
    }
    return true;
}

//
// Pulse Counter - GetLevel
//
void
PortManager_PulseCounterGetLevel(bool* levels[]) {
    for (int i = 0; i < NUM_DI; i++) {
        levels[i] = (bool*)PulseCounter_GetLevel(&sPulseCounter[i]);
    }
}

//
// Pulse Counter - GetPinLevel
//
bool
PortManager_PulseCounterGetPinLevel(int pinId, int* level) {
    PulseCounter* targetDI = PortManager_GetTargetDI(pinId);

    if (targetDI == NULL) {
        return false;
    }
    else {
        *level = (bool*)PulseCounter_GetPinLevel(targetDI);
    }
    return true;
}

//
// PwmController - Clear
// 
void
PortManager_PwmControllerReset(int relationPort, int initVal) {
    PulseCounter* targetDI = PortManager_GetTargetDI(relationPort);
    PwmController* targetDO = NULL;

    for (int i = 0; i < NUM_DO; i++) {
        if (relationPort == sPwmController[i].relationPort) {
            targetDO = &sPwmController[i];
        }
    }

    if (targetDO == NULL 
        || targetDO->triggerType != TriggerType_PulseCount) {
        return; // single, pulse edge
    }
    else {
        PwmController_Reset(targetDO, initVal);
    }
}

//
// PWM Controller - stop
// 
bool
PortManager_PwmControllerStopOutput(int pinId) {
    PwmController* targetDO = PortManager_GetTargeDO(pinId);

    if (targetDO == NULL) {
        return false;
    }
    PwmController_Stop(targetDO);
    return true;
}

//
// PWM Controller - stop
// 
bool
PortManager_PwmControllerGetRelationStatus(int pinId, bool* status) {
    PwmController* targetDO = PortManager_GetTargeDO(pinId);

    if (targetDO == NULL) {
        return false;
    }
    *status = PwmController_GetRelationStatus(targetDO);
    return true;
}

//
// Runnning 1ms
//
void
PortManager_Running() {
    for (int i = 0; i < NUM_DI; i++) {
        if (sPulseCounter[i].isStart) {
            PulseCounter_Counter(&sPulseCounter[i]);
        }
    }

    for (int i = 0; i < NUM_DO; i++) {
        PwmController* targetDO = &sPwmController[i];

        if (targetDO->enableRelation) {
            PortManager_RelationalFunc(targetDO);
        }
        else { // run single
            switch (targetDO->step) {
            case Step_Initialize:
                break;
            case Step_Start:
                PwmController_Start(targetDO);
                break;
            case Step_Delay:
                PwmController_Delay(targetDO);
                break;
            case Step_Output:
                PwmController_Output(targetDO, targetDO->outputLevel, true);
                break;
            case Step_Running:
                PwmController_Running(targetDO);
                break;
            case Step_Stop:
                break;
            }
        }

    }
}
