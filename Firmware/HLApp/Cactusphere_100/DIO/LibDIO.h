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

#ifndef _LIB_DIO_H_
#define _LIB_DIO_H_

#include <stdbool.h>

#define NUM_DI	2

 // function type list
typedef enum {
    FunctionType_NotSelected,
    FunctionType_Oneshot,
    FunctionType_Pulse,
    FunctionType_Interlock,
    FunctionType_Invert,
    FunctionType_Genarate,
} FunctionType;

// edge type list
typedef enum {
    EdgeType_Rising,
    EdgeType_Falling,
    EdgeType_Both,
} EdgeType;

// Initialization and cleanup
extern bool DIO_Lib_Initialize(void);
extern void DIO_Lib_Cleanup(void);

// Configure the pulse counter
extern bool DIO_Lib_ConfigPulseCounter(unsigned long pinId,
    bool isPulseHigh, unsigned long minPulseWidth, unsigned long maxPulseCount);

// Configure the single output
extern bool
DIO_Lib_ConfigSingle(unsigned long pinId,
    unsigned long functionType,
    bool outputLevel,
    unsigned long pulseClock,
    unsigned long pulseEffectiveTime,
    unsigned long pulsePeriod,
    unsigned long delayTime,
    unsigned long outputTime);
 
// Configure output by edge trigger
extern bool
DIO_Lib_ConfigEdgeTrigger(unsigned long pinId,
    unsigned long functionType,
    unsigned long edgeType,
    unsigned long chatteringVal,
    bool outputLevel,
    unsigned long pulseClock,
    unsigned long pulseEffectiveTime,
    unsigned long pulsePeriod,
    unsigned long delayTime,
    unsigned long outputTime,
    unsigned long relationPort);

// Configure output by pulse count trigger
extern bool
DIO_Lib_ConfigPulseCountTrigger(unsigned long pinId,
    unsigned long functionType,
    unsigned long startOutputCount,
    unsigned long stopOutputCount,
    bool outputLevel,
    unsigned long pulseClock,
    unsigned long pulseEffectiveTime,
    unsigned long pulsePeriod,
    unsigned long delayTime,
    unsigned long outputTime,
    unsigned long relationPort);

// Reset the pulse counter
extern bool DIO_Lib_ResetPulseCount(unsigned long pinId, unsigned long initVal);

// Read value of the pulse counter
extern bool DIO_Lib_ReadPulseCount(unsigned long pinId, unsigned long* outVal);

// Read on-time integrated value of the pulse (in seconds)
extern bool	DIO_Lib_ReadDutySumTime(unsigned long pinID, unsigned long* outSecs);

// Get input level of all DIO ports/pins
extern bool DIO_Lib_ReadLevels(int outLevels[NUM_DI]);

// Get input level of specific pin
extern bool DIO_Lib_ReadPinLevel(unsigned long pinId, unsigned int* outVal);

// Stop output
extern bool DIO_Lib_StopOutput(unsigned long pinId);

// Get reration status
extern bool DIO_Lib_GetRelationStatus(unsigned long pinId, unsigned int* outVal);

// Get RTApp Version
extern bool DIO_Lib_ReadRTAppVersion(char* rtAppVersion);

#endif  // _LIB_DIO_H_
