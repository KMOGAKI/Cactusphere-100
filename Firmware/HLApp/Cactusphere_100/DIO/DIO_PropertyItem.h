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

#ifndef _DIO_PROPERTYITEM_H_
#define _DIO_PROPERTYITEM_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif
#ifndef _STDIDONT_H
#include <stdint.h>
#endif

#ifndef NUM_DI
#define NUM_DI 2
#endif

#ifndef NUM_DO
#define NUM_DO 2
#endif

#ifndef NUM_DIO
#define NUM_DIO (NUM_DI + NUM_DO)
#endif

typedef struct _json_value	json_value;

typedef enum {
    DIFUNC_TYPE_NOTSELECTED = 0,
    DIFUNC_TYPE_PULSECOUNTER,
    DIFUNC_TYPE_EDGE,
    DIFUNC_TYPE_POLLING,
} DIFuncType;

typedef enum {
    DOFUNC_TYPE_NOTSELECTED = 0,
    DOFUNC_TYPE_SINGLE,
    DOFUNC_TYPE_RELATION,
} DOFuncType;

typedef enum {
    DOFUNC_SGL_TYPE_NOTSELECTED = 0,
    DOFUNC_SGL_TYPE_ONESHOT,
    DOFUNC_SGL_TYPE_PULSEPWM,
} DOFuncSingleType;

typedef enum {
    DOFUNC_REL_TYPE_NOTSELECTED = 0,
    DOFUNC_REL_TYPE_ONESHOT,
    DOFUNC_REL_TYPE_INTERLOCK,
    DOFUNC_REL_TYPE_INVERT,
    DOFUNC_REL_TYPE_GENERATE,
    DOFUNC_REL_TYPE_PULSEPWM,
} DOFuncRelationType;

typedef enum {
    DI_REL_PORT_NOTSELECTED = 0,
    DI_REL_PORT_DI1 = 1,
    DI_REL_PORT_DI2 = 2,
} DIRelationPort;

typedef enum {
    DO_EDGE_NOTSELECTED = 0,
    DO_EDGE_RISING,
    DO_EDGE_FALLING,
    DO_EDGE_BOTH,
} DOEdgeType;

#define DI_INTERVAL_DEFAULT_VALUE 1
#define DI_INTERVAL_MIN_VALUE     1
#define DI_INTERVAL_MAX_VALUE     86400

#define DI_MINPULSE_DEFAULT_VALUE 200
#define DI_MINPULSE_MIN_VALUE     1
#define DI_MINPULSE_MAX_VALUE     1000

#define DI_MAXCOUNT_DEFAULT_VALUE 0x7FFFFFFF
#define DI_MAXCOUNT_MIN_VALUE     1
#define DI_MAXCOUNT_MAX_VALUE     0x7FFFFFFF

#define DO_STARTOUTPUTCOUNT_DEFAULT_VALUE 1
#define DO_STARTOUTPUTCOUNT_MIN_VALUE     1
#define DO_STARTOUTPUTCOUNT_MAX_VALUE     0x7FFFFFFF

#define DO_STOPOUTPUTCOUNT_DEFAULT_VALUE 0x7FFFFFFF
#define DO_STOPOUTPUTCOUNT_MIN_VALUE     1
#define DO_STOPOUTPUTCOUNT_MAX_VALUE     0x7FFFFFFF

#define DO_INPUTCHATTERING_DEFAULT_VALUE 10
#define DO_INPUTCHATTERING_MIN_VALUE     0
#define DO_INPUTCHATTERING_MAX_VALUE     1000

#define DO_OUTPUTTIME_DEFAULT_VALUE 0
#define DO_OUTPUTTIME_MIN_VALUE     0
#define DO_OUTPUTTIME_MAX_VALUE     600000

#define DO_OUTPUTDELAYTIME_DEFAULT_VALUE 0
#define DO_OUTPUTDELAYTIME_MIN_VALUE     0
#define DO_OUTPUTDELAYTIME_MAX_VALUE     600000

#define DO_PULSECYCLE_DEFAULT_VALUE 1
#define DO_PULSECYCLE_MIN_VALUE     1
#define DO_PULSECYCLE_MAX_VALUE     100000

#define DO_PULSEDUTYCYCLE_DEFAULT_VALUE 50
#define DO_PULSEDUTYCYCLE_MIN_VALUE     0
#define DO_PULSEDUTYCYCLE_MAX_VALUE     100

typedef struct DIO_DIPropertyData {
    DIFuncType  diFunctionType;
    uint32_t    intervalSec;
    bool        isEdgeTriggerHigh;
    uint32_t    minPulseWidth;
    uint32_t    maxPulseCount;
    bool        isPollingActiveHigh;
} DIO_DIPropertyData;

typedef struct DIO_DOPropertyData {
    DOFuncType          doFunctionType;
    bool                isDoNotify;
    DOFuncSingleType    singleFunctionType;
    DOFuncRelationType  relationFunctionType;
    DIRelationPort      relationDIPort;
    DOEdgeType          edgeType;
    uint32_t            startOutputCount;
    uint32_t            stopOutputCount;
    uint32_t            inputChatteringTime;
    bool                isDOOutputStateHigh;
    uint32_t            doOutputTime;
    uint32_t            doOutputDelayTime;
    uint32_t            doPulseCycle;
    uint32_t            doPulseDutyCycle;
} DIO_DOPropertyData;

typedef struct DIO_PropertyData {
    struct DIO_DIPropertyData diData[NUM_DI];
    struct DIO_DOPropertyData doData[NUM_DO];
} DIO_PropertyData;

extern void DIO_PropertyItem_Init(void);

extern DIO_PropertyData* DIO_PropertyItem_GetDataPtrs(void);
extern DIO_DIPropertyData* DIO_PropertyItem_GetDIDataPtrs(void);
extern DIO_DOPropertyData* DIO_PropertyItem_GetDODataPtrs(void);

extern bool DIO_PropertyItem_ParseJson(
    const json_value* json, vector propertyItem, const char* version);

extern bool DIO_PropertyItem_VerifyDOSetting(int doPinId);

#endif  // _DIO_PROPERTYITEM_H_
