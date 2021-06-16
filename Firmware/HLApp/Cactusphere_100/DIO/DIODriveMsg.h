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

#ifndef _DIO_DRIVER_MSG_H_
#define _DIO_DRIVER_MSG_H_

#ifndef _STDIDONT_H
#include <stdint.h>
#endif

// request code
enum {  
    DI_SET_CONFIG_AND_START = 1,  // setting up a pulse conter
    DI_PULSE_COUNT_RESET    = 2,  // reset a pulse counter
    DI_READ_PULSE_COUNT     = 3,  // read the counter value
    DI_READ_DUTY_SUM_TIME   = 4,  // read the time integration of pulse
    DI_READ_PULSE_LEVEL     = 5,  // read the input level of all DIO pin
    DI_READ_PIN_LEVEL       = 6,  // read the input level of specific DIO pin
    DO_SET_CONFIG_SINGLE    = 7,  // setting up single output
    DO_SET_CONFIG_EDGE_TRIGGER = 8,  // setting up pulse edge trigger
    DO_SET_CONFIG_PULSECOUNT_TRIGGER = 9, // setting up pulse count trigger
    DO_STOP_OUTPUT          = 10, // stop output
    DO_READ_RELATIONSTATUS = 11, // get relation status
    DIO_READ_VERSION        = 255,// read the RTApp version
}; // Firmware/RTApp/DIO/DIODriveMsg.hと揃える

//
// message structure
//
// header
typedef struct DIO_DriverMsgHdr
{
    uint32_t requestCode;
    uint32_t messageLen;
} DIO_DriverMsgHdr;

// body
// setting config
typedef struct DI_MsgSetConfig
{
    uint32_t pinId;
    uint32_t minPulseWidth;
    uint32_t maxPulseCount;
    bool isPulseHigh;
} DI_MsgSetConfig;
// sizeof(DI_MsgSetConfig) == messageLen

// DO
// setting config single
typedef struct DO_MsgSetConfigSingle
{
    uint32_t pinId;
    uint32_t functionType;
    bool outputLevel;
    uint32_t pulseClock;
    uint32_t pulseEffectiveTime;
    uint32_t pulsePeriod;
    uint32_t delayTime;
    uint32_t outputTime;
} DO_MsgSetConfigSingle;
// sizeof(DO_MsgSetConfigSingle) == messageLen

// setting config pulse edge trigger
typedef struct DO_MsgSetConfigEdgeTrigger
{
    uint32_t pinId;
    uint32_t functionType;
    uint32_t relationPort;
    uint32_t edgeType;
    uint32_t chatteringVal;
    bool outputLevel;
    uint32_t pulseClock;
    uint32_t pulseEffectiveTime;
    uint32_t pulsePeriod;
    uint32_t delayTime;
    uint32_t outputTime;
} DO_MsgSetConfigEdgeTrigger;
// sizeof(DO_MsgSetConfigEdgeTrigger) == messageLen

// // setting config pulse count trigger
typedef struct DO_MsgSetConfigCountTrigger
{
    uint32_t pinId;
    uint32_t functionType;
    uint32_t relationPort;
    uint32_t startOutputCount;
    uint32_t stopOutputCount;
    bool outputLevel;
    uint32_t pulseClock;
    uint32_t pulseEffectiveTime;
    uint32_t pulsePeriod;
    uint32_t delayTime;
    uint32_t outputTime;
} DO_MsgSetConfigCountTrigger;
// sizeof(DO_MsgSetConfigCountTrigger) == messageLen

// pulse reset
typedef struct DI_MsgResetPulseCount
{
    uint32_t pinId;
    uint32_t initVal;
} DI_MsgResetPulseCount;
// sizeof(DI_MsgResetPulseCount) == messageLen

// pinID
typedef struct DIO_MsgPinId
{
    uint32_t pinId;
} DIO_MsgPinId;
// sizeof(DIO_MsgPinId) == messageLen

// message
typedef struct DIO_DriverMsg
{
    DIO_DriverMsgHdr header;
    union
    {
        DI_MsgSetConfig setConfig;
        DO_MsgSetConfigSingle setDOConfigSingle;
        DO_MsgSetConfigEdgeTrigger setDOConfigEdgeTrigger;
        DO_MsgSetConfigCountTrigger setDOConfigCountTrigger;
        DI_MsgResetPulseCount resetPulseCount;
        DIO_MsgPinId pinId;
    } body;
} DIO_DriverMsg;

// return message
typedef struct DIO_ReturnMsg
{
    uint32_t returnCode;
    uint32_t messageLen;
    union
    {
        bool levels[4];
        char version[256];
    } message;
} DIO_ReturnMsg;

#endif // _DIO_DRIVER_MSG_H_
