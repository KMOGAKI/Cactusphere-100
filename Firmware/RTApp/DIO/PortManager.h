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

#ifndef _PORT_MANAGER_H_
#define _PORT_MANAGER_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif
#ifndef _STDINT_H
#include <stdint.h>
#endif


// Initialize
extern void
PortManager_PulseCounterInitialize(int pin0, int pin1);
extern void
PortManager_PwmControllerInitialize(int pin0, int pin1);

// set config
extern bool
PortManager_PulseCounterSetConfig(int pinId, bool isCountHigh, int minPulse, int maxPulse);

extern bool
PortManager_PwmControllerSetConfigSingle(
    int pinId,
    bool outputLevel,
    int delayTime,
    int outputTime,
    int pulseClock,
    int pulseEffectiveTime,
    int pulsePeriod,
    int functionType);

extern bool
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
    int functionType);

extern bool
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
    int functionType);

// Pulse Counter
extern bool
PortManager_PulseCounterClear(int pinId, int initVal);
extern bool
PortManager_PulseCounterGetPulseCount(int pinId, int* val);
extern bool
PortManager_PulseCounterGetPulseOnTime(int pinId, int* val);
extern void
PortManager_PulseCounterGetLevel(bool* levels[]);
extern bool
PortManager_PulseCounterGetPinLevel(int pinId, int* level);

// Pwm Controller
extern bool
PortManager_PwmControllerStopOutput(int pinId);
extern bool
PortManager_PwmControllerGetRelationStatus(int pinId, bool* status);
extern void
PortManager_PwmControllerReset(int relationPort, int initVal);

// PortManager
extern void
PortManager_Running();

#endif  // _PORT_MANAGER_H_
