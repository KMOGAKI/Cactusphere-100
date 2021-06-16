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

#include "LibDIO.h"
#include "SendRTApp.h"
#include "DIODriveMsg.h"

#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

// Initialization and cleanup
bool DIO_Lib_Initialize(void)
{
    return true;
}

void DIO_Lib_Cleanup(void)
{
    // do nothing
}

bool
DIO_Lib_ConfigPulseCounter(unsigned long pinId, bool isPulseHigh,
    unsigned long minPulseWidth, unsigned long maxPulseCount)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    int msgSize;
    int ret = 0;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DI_SET_CONFIG_AND_START;
    msg->header.messageLen = sizeof(DI_MsgSetConfig);
    msg->body.setConfig.pinId = pinId;
    msg->body.setConfig.isPulseHigh = isPulseHigh;
    msg->body.setConfig.minPulseWidth = minPulseWidth;
    msg->body.setConfig.maxPulseCount = maxPulseCount;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)&ret, sizeof(ret));

    return ret;
}

bool  
DIO_Lib_ConfigSingle(unsigned long pinId, 
    unsigned long functionType,
    bool outputLevel,
    unsigned long pulseClock,
    unsigned long pulseEffectiveTime,
    unsigned long pulsePeriod,
    unsigned long delayTime,
    unsigned long outputTime)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    int msgSize;
    int ret = 0;

    if (!(functionType == FunctionType_Oneshot ||
        functionType == FunctionType_Pulse)) {
        return false;
    }

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DO_SET_CONFIG_SINGLE;
    msg->header.messageLen = sizeof(DO_MsgSetConfigSingle);
    msg->body.setDOConfigSingle.pinId = pinId;
    msg->body.setDOConfigSingle.functionType = functionType;
    msg->body.setDOConfigSingle.outputLevel = outputLevel;
    msg->body.setDOConfigSingle.pulseClock = pulseClock;
    msg->body.setDOConfigSingle.pulseEffectiveTime = pulseEffectiveTime;
    msg->body.setDOConfigSingle.pulsePeriod = pulsePeriod;
    msg->body.setDOConfigSingle.delayTime = delayTime;
    msg->body.setDOConfigSingle.outputTime = outputTime;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)&ret, sizeof(ret));

    return ret;
}

bool
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
    unsigned long relationPort)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    int msgSize;
    int ret = 0;

    if (!(functionType == FunctionType_Interlock ||
        functionType == FunctionType_Invert ||
        functionType == FunctionType_Genarate ||
        functionType == FunctionType_Pulse)) {
        return false;
    }

    if (edgeType < EdgeType_Rising || edgeType > EdgeType_Both) {
        return false;
    }

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DO_SET_CONFIG_EDGE_TRIGGER;
    msg->header.messageLen = sizeof(DO_MsgSetConfigEdgeTrigger);
    msg->body.setDOConfigEdgeTrigger.pinId = pinId;
    msg->body.setDOConfigEdgeTrigger.functionType = functionType;
    msg->body.setDOConfigEdgeTrigger.edgeType = edgeType;
    msg->body.setDOConfigEdgeTrigger.chatteringVal = chatteringVal;
    msg->body.setDOConfigEdgeTrigger.outputLevel = outputLevel;
    msg->body.setDOConfigEdgeTrigger.pulseClock = pulseClock;
    msg->body.setDOConfigEdgeTrigger.pulseEffectiveTime = pulseEffectiveTime;
    msg->body.setDOConfigEdgeTrigger.pulsePeriod = pulsePeriod;
    msg->body.setDOConfigEdgeTrigger.delayTime = delayTime;
    msg->body.setDOConfigEdgeTrigger.outputTime = outputTime;
    msg->body.setDOConfigEdgeTrigger.relationPort = relationPort;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)&ret, sizeof(ret));

    return ret;
}

bool
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
    unsigned long relationPort)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    int msgSize;
    int ret = 0;

    if (!(functionType == FunctionType_Oneshot ||
        functionType == FunctionType_Pulse)) {
        return false;
    }

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DO_SET_CONFIG_PULSECOUNT_TRIGGER;
    msg->header.messageLen = sizeof(DO_MsgSetConfigCountTrigger);
    msg->body.setDOConfigCountTrigger.pinId = pinId;
    msg->body.setDOConfigCountTrigger.functionType = functionType;
    msg->body.setDOConfigCountTrigger.startOutputCount = startOutputCount;
    msg->body.setDOConfigCountTrigger.stopOutputCount = stopOutputCount;
    msg->body.setDOConfigCountTrigger.outputLevel = outputLevel;
    msg->body.setDOConfigCountTrigger.pulseClock = pulseClock;
    msg->body.setDOConfigCountTrigger.pulseEffectiveTime = pulseEffectiveTime;
    msg->body.setDOConfigCountTrigger.pulsePeriod = pulsePeriod;
    msg->body.setDOConfigCountTrigger.delayTime = delayTime;
    msg->body.setDOConfigCountTrigger.outputTime = outputTime;
    msg->body.setDOConfigCountTrigger.relationPort = relationPort;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)&ret, sizeof(ret));

    return ret;
}

bool
DIO_Lib_ResetPulseCount(unsigned long pinId, unsigned long initVal)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    int msgSize;
    int ret = 0;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DI_PULSE_COUNT_RESET;
    msg->header.messageLen = sizeof(DI_MsgResetPulseCount);
    msg->body.resetPulseCount.pinId = pinId;
    msg->body.resetPulseCount.initVal = initVal;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)&ret, sizeof(ret));

    return ret;
}

bool
DIO_Lib_ReadPulseCount(unsigned long pinId, unsigned long* outVal)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    unsigned char val[4] = { 0 };
    int msgSize;
    bool ret = false;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DI_READ_PULSE_COUNT;
    msg->header.messageLen = sizeof(DIO_MsgPinId);
    msg->body.pinId.pinId = pinId;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    ret = SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        val, sizeof(val));
    memcpy(outVal, val, sizeof(int));

    return ret;
}

bool
DIO_Lib_ReadDutySumTime(unsigned long pinId, unsigned long* outSecs)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    unsigned char val[4] = { 0 };
    int msgSize;
    bool ret = false;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DI_READ_DUTY_SUM_TIME;
    msg->header.messageLen = sizeof(DIO_MsgPinId);
    msg->body.pinId.pinId = pinId;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    ret = SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        val, sizeof(val));
    memcpy(outSecs, val, sizeof(int));

    return ret;
}

bool
DIO_Lib_ReadLevels(int outLevels[NUM_DI])
{
    unsigned char sendMessage[256];
    unsigned char readMessage[272];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    DIO_ReturnMsg* retMsg = (DIO_ReturnMsg*)readMessage;
    int msgSize;
    bool ret = false;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DI_READ_PULSE_LEVEL;
    msg->header.messageLen = 0;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    ret = SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)retMsg, sizeof(DIO_ReturnMsg));
    for (int i = 0; i < NUM_DI; i++) {
        outLevels[i] = retMsg->message.levels[i];
    }

    return ret;
}

bool
DIO_Lib_ReadPinLevel(unsigned long pinId, unsigned int* outVal)
{
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    unsigned char val[4] = { 0 };
    int msgSize;
    bool ret = false;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DI_READ_PIN_LEVEL;
    msg->header.messageLen = sizeof(DIO_MsgPinId);
    msg->body.pinId.pinId = pinId;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    ret = SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        val, sizeof(val));
    memcpy(outVal, val, sizeof(int));

    return ret;
}

bool 
DIO_Lib_StopOutput(unsigned long pinId) {
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    int msgSize;
    int ret = 0;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DO_STOP_OUTPUT;
    msg->header.messageLen = sizeof(DIO_MsgPinId);
    msg->body.pinId.pinId = pinId;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)&ret, sizeof(ret));

    return ret;
}

bool
DIO_Lib_GetRelationStatus(unsigned long pinId, unsigned int* outVal) {
    unsigned char sendMessage[256];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    unsigned char val[4] = { 0 };
    int msgSize;
    bool ret = false;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DO_READ_RELATIONSTATUS;
    msg->header.messageLen = sizeof(DIO_MsgPinId);
    msg->body.pinId.pinId = pinId;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    ret = SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        val, sizeof(val));
    memcpy(outVal, val, sizeof(int));

    return ret;
}

bool
DIO_Lib_ReadRTAppVersion(char* rtAppVersion)
{
    unsigned char sendMessage[256];
    unsigned char readMessage[272];
    DIO_DriverMsg* msg = (DIO_DriverMsg*)sendMessage;
    DIO_ReturnMsg* retMsg = (DIO_ReturnMsg*)readMessage;
    int msgSize;
    bool ret = false;

    memset(msg, 0, sizeof(DIO_DriverMsg));
    msg->header.requestCode = DIO_READ_VERSION;
    msg->header.messageLen = 0;
    msgSize = (int)(sizeof(msg->header) + msg->header.messageLen);
    ret = SendRTApp_SendMessageToRTCoreAndReadMessage((const unsigned char*)msg, msgSize,
        (unsigned char*)retMsg, sizeof(DIO_ReturnMsg));
    if (ret) {
        strncpy(rtAppVersion, retMsg->message.version, retMsg->messageLen);
    }

    return ret;
}
