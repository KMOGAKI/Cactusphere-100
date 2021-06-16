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

#ifndef _DIO_DOWATCHITEM_H_
#define _DIO_DOWATCHITEM_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif
#ifndef _STDIDONT_H
#include <stdint.h>
#endif

#ifndef TELEMETRY_NAME_MAX_LEN
#define TELEMETRY_NAME_MAX_LEN	32
#endif

#include "DIO_PropertyItem.h"

typedef enum {
    DO_MODE_NOTSELECTED = 0,
    DO_MODE_SINGLE,
    DO_MODE_RELATE_EDGETRIGGER,
    DO_MODE_RELATE_PULSETRIGGER,
} DOModeType;

typedef enum {
    DO_FUNCTYPE_NOTSELECTED = 0,
    DO_FUNCTYPE_ONESHOT,
    DO_FUNCTYPE_PULSE,
    DO_FUNCTYPE_INTERLOCK,
    DO_FUNCTYPE_INVERT,
    DO_FUNCTYPE_GENERATE,
} DOFunctionType;

typedef struct DIO_DOSingleConfig {
    DOFunctionType doFunctionType;
    bool outputLevel; // true:high, false:low
    uint32_t pulseClock;
    uint32_t pulseEffectiveTime;
    uint32_t pulsePeriod;
    uint32_t delayTime;
    uint32_t outputTime;
} DIO_DOSingleConfig;

typedef struct DIO_DORelateEdgeTriggerConfig {
    DOFunctionType doFunctionType;
    uint32_t relationPort;
    uint32_t edgeType;
    uint32_t chatteringVal;
    bool outputLevel; // true:high, false:low
    uint32_t pulseClock;
    uint32_t pulseEffectiveTime;
    uint32_t pulsePeriod;
    uint32_t delayTime;
    uint32_t outputTime;
} DIO_DORelateEdgeTriggerConfig;

typedef struct DIO_DORelatePulseTriggerConfig {
    DOFunctionType doFunctionType;
    uint32_t relationPort;
    uint32_t startOutputCount;
    uint32_t stopOutputCount;
    bool outputLevel; // true:high, false:low
    uint32_t pulseClock;
    uint32_t pulseEffectiveTime;
    uint32_t pulsePeriod;
    uint32_t delayTime;
    uint32_t outputTime;
} DIO_DORelatePulseTriggerConfig;

typedef struct DIO_DOWatchItem {
    char        telemetryName[TELEMETRY_NAME_MAX_LEN + 1]; // "DOx_Status"
    uint32_t    pinID;
    DOModeType  doMode;
    union{
        DIO_DOSingleConfig single;
        DIO_DORelateEdgeTriggerConfig relateEdgeTrigger;
        DIO_DORelatePulseTriggerConfig relatePulseTrigger;
    } config;
    bool isNotify;
} DIO_DOWatchItem;

#endif  // _DIO_DOWATCHITEM_H_
