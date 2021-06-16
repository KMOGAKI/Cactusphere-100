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

#ifndef _PWM_CONTROLLER_H_
#define _PWM_CONTROLLER_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif
#ifndef _STDINT_H
#include <stdint.h>
#endif

typedef enum {
    FunctionType_NotSelected,
    FunctionType_Oneshot,
    FunctionType_Pulse,
    FunctionType_Interlock,
    FunctionType_Invert,
    FunctionType_Genarate,
} FunctionType;

typedef enum {
//    PulseClock_None = 0,
    PulseClock_32KHz = 32768,
    PulseClock_2MHz = 2000000,
//    PulseClock_XTAL = 26000000
} PulseClock;

typedef enum {
    TriggerType_NotSelected,
    TriggerType_PulseEdge,
    TriggerType_PulseCount,
}TriggerType;

typedef enum {
    EdgeType_Rising,
    EdgeType_Falling,
    EdgeType_Both,
} EdgeType;

typedef struct PulseCountTrigger{
    int startOutputCount;
    int stopOutputCount;
} PulseCountTrigger;

typedef struct PulseEdgeTrigger {
    EdgeType    edgeType;
    int         chatteringVal;
    int         edgeElapsedTime;
    bool        prevEdge;
    bool        isStart; // 2021-04-28
} PulseEdgeTrigger;

typedef struct PwmSettings {
    PulseClock  pulseClock;         // PWMコントローラのクロック(32kHz, 2MHzのみ使用可能)
    int         pulseEffectiveTime; // パルスを1(High)にする期間(値×分解能=実時間)
    int         pulsePeriod;        // パルス全体の期間(値×分解能=周期)
} PwmSettings;

typedef enum {
    Step_Initialize,
    Step_Start,
    Step_Delay,
    Step_Output,
    Step_Running,
    Step_Stop,
} Step;

typedef struct PwmController
{                                   
    int         pinId;              // DOut pin id
    int         pinNum;             // DOut pin number
    bool        outputLevel;        // output level (H/L)
    int         outputTime;         // finish output time
    int         outputElapsedTime;  // current output continuation length
    int         delayTime;          // finish delay time
    int         delayElapsedTime;   // current delay continuation length
    bool        enableRelation;     // relation is enable
    bool        relationalStatus;   // true: trigger active, false: trigger disable
    int         relationPort;       // relation DI pinId
    TriggerType triggerType;
    PulseEdgeTrigger pulseEdgeTrigger;
    PulseCountTrigger pulseCountTrigger;
    PwmSettings pwmSettings;
    FunctionType functionType;     
    Step        step;
} PwmController;                     

// Initialization
extern void PwmController_Initialize(PwmController* me, int pinId, int pinNum);

// Attribute
extern int  PwmController_GetPinId(PwmController* me);
extern bool PwmController_GetRelationStatus(PwmController* me);

// set config
extern void
PwmController_SetConfig(PwmController* me,
    bool outputLevel,
    int delayTime,
    int outputTime,
    int relationPort,
    bool enableRelation,
    TriggerType triggerType,
    PulseEdgeTrigger pulseEdgeTrigger,
    PulseCountTrigger pulseCountTrigger,
    PwmSettings pwmSettings,
    FunctionType functionType
);

// set trigger
extern void
PwmController_SetTrigger(PwmController* me);
// start / stop
extern void
PwmController_Start(PwmController* me);
extern void
PwmController_Stop(PwmController* me);
// delay
extern void
PwmController_Delay(PwmController* me);
// output
extern void
PwmController_Output(PwmController* me, bool output, bool isNext);
// runnning
extern void 
PwmController_Running(PwmController* me);
// clear
extern void
PwmController_Reset(PwmController* me, int initVal);
#endif  // _PWM_CONTROLLER_H_
