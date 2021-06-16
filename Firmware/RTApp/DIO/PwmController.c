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

#include "PwmController.h"
#include "cactusphere_pwm.h"


//
// Initialization
//
void
PwmController_Initialize(PwmController* me, int pinId, int pinNum) {
    me->pinId               = pinId;
    me->pinNum              = pinNum;
    me->outputLevel         = true;
    me->outputTime          = 0; 
    me->outputElapsedTime   = 0;
    me->delayTime           = 0;
    me->delayElapsedTime    = 0;
    me->enableRelation      = false;
    me->relationalStatus    = false;
    me->relationPort        = -1;
    me->triggerType = TriggerType_NotSelected;
    me->step        = Step_Initialize;
    // PulseEdgeTrigger
    me->pulseEdgeTrigger.edgeType           = EdgeType_Rising;
    me->pulseEdgeTrigger.edgeElapsedTime    = 0;
    me->pulseEdgeTrigger.chatteringVal      = 10;
    me->pulseEdgeTrigger.prevEdge           = false;
    me->pulseEdgeTrigger.isStart            = false; // 2021-04-28
    // PulseCountTrigger
    me->pulseCountTrigger.startOutputCount  = 0;
    me->pulseCountTrigger.stopOutputCount   = 0;
    // PwmSettings
    me->pwmSettings.pulseClock              = PulseClock_32KHz;
    me->pwmSettings.pulsePeriod             = 0;
    me->pwmSettings.pulseEffectiveTime      = 0;
    me->functionType                        = FunctionType_NotSelected;
}

int
PwmController_GetPinId(PwmController* me)
{
    return me->pinId;
}

bool
PwmController_GetRelationStatus(PwmController* me) {
#if 0 // 2021-04-26
    return me->relationalStatus;
#else
    if (me->triggerType == TriggerType_NotSelected) {
        if (me->step == Step_Running) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return me->relationalStatus;
    }
#endif // 2021-04-26
}

//
// Set config and start
//
void
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
    )
{
    me->outputLevel     = outputLevel;
    me->outputTime      = outputTime;
    me->outputElapsedTime = 0;
    me->delayTime       = delayTime;
    me->enableRelation  = enableRelation;
    me->relationPort    = relationPort;
    me->triggerType     = triggerType;

    // PulseEdgeTrigger
    me->pulseEdgeTrigger = pulseEdgeTrigger;
    // PulseCountTrigger
    me->pulseCountTrigger = pulseCountTrigger;
    // PwmSettings
    me->pwmSettings = pwmSettings;

    me->functionType = functionType;

    if (me->step != Step_Initialize) {
        PwmController_Stop(me);
    }
    me->step = Step_Start;
}

void
PwmController_SetTrigger(PwmController* me) {
    me->relationalStatus = true;
}

void 
PwmController_Start(PwmController* me) {
    if (me->step == Step_Start || me->step == Step_Stop) {
        me->step = Step_Delay;
    }
}

void
PwmController_Stop(PwmController* me) {
#if 0 // 2021-04-26
    Cactusphere_PWM_SetOutput(me->pinNum, false);
#else
    bool output = !me->outputLevel;
    Cactusphere_PWM_SetOutput(me->pinNum, output);
#endif // 2021-04-26
    me->outputElapsedTime = 0;
    me->step = Step_Stop;
}

void
PwmController_Delay(PwmController* me) {
    if (me->delayElapsedTime >= me->delayTime) {
        me->delayElapsedTime = 0;
        me->step = Step_Output;
    }
    else {
        me->delayElapsedTime++;
    }
}

void
PwmController_Output(PwmController* me, bool output, bool isNext) {
    if (me->functionType == FunctionType_Pulse) {
        Cactusphere_PWM_SetOutputPWM(me->pinNum,
            me->pwmSettings.pulseClock,
            me->pwmSettings.pulseEffectiveTime,
            me->pwmSettings.pulsePeriod - me->pwmSettings.pulseEffectiveTime);
    }
    else {
        Cactusphere_PWM_SetOutput(me->pinNum, output);
    }

    if (isNext) {
        me->step = Step_Running;
    }
}

void
PwmController_Running(PwmController* me) {
    if (me->outputTime == 0) {
        return; // output is not stop
    }

    if (me->outputTime > 0 && me->outputElapsedTime >= me->outputTime) {
        PwmController_Stop(me);
        me->step = Step_Stop;
    }
    else {
        me->outputElapsedTime++;
    }
}

void
PwmController_Reset(PwmController* me, int initVal) {
    if (me->pulseCountTrigger.startOutputCount > initVal) {
        PwmController_Stop(me);
        // clear
        me->delayElapsedTime = 0;
        me->outputElapsedTime = 0;
        me->relationalStatus = false;
        if (me->step != Step_Initialize) {
            me->step = Step_Start;
        }
    }
    else {
        if (me->step == Step_Start) {
            PwmController_Delay(me);
        }
    }
}
