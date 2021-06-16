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

#include "DIO_ConfigMgr.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "applibs_versions.h"
#include <applibs/log.h>

#include "json.h"
#include "DIO_PropertyItem.h"
#include "PropertyItems.h"

const char FunctionTypeDIKey[]          = "FunctionType_DI";            // FunctionType_DI
const char IntervalDIKey[]              = "Interval_DI";                // Interval_DI
const char EdgeTriggerDIkey[]           = "EdgeTrigger_DI";             // EdgeTrigger_DI
const char MinPulseWidthDIKey[]         = "MinPulseWidth_DI";           // MinPulseWidth_DI
const char MaxPulseCountDIKey[]         = "MaxPulseCount_DI";           // MaxPulseCount_DI
const char PollIsActiveHighKey[]        = "PollIsActiveHigh_DI";        // PollIsActiveHigh_DI
const char FunctionTypeDOKey[]          = "FunctionType_DO";            // FunctionType_DO
const char IsNotifyDOKey[]              = "IsNotify_DO";                // IsNotify_DO
const char SingleFunctionTypeDOKey[]    = "SingleFunctionType_DO";      // SingleFunctionType_DO
const char RelationPortDOKey[]          = "RelationPort_DO";            // RelationPort_DO
const char RelationFunctionTypeDOKey[]  = "RelationFunctionType_DO";    // RelationFunctionType_DO
const char EdgeTriggerTypeDOKey[]       = "EdgeTriggerType_DO";         // EdgeTriggerType_DO
const char StartOutputCountDOKey[]      = "StartOutputCount_DO";        // StartOutputCount_DO
const char StopOutputCountDOKey[]       = "StopOutputCount_DO";         // StopOutputCount_DO
const char InputChatteringTimeDOKey[]   = "InputChatteringTime_DO";     // InputChatteringTime_DO
const char OutputStatusDOKey[]          = "OutputStatus_DO";            // OutputStatus_DO
const char OutputTimeDOKey[]            = "OutputTime_DO";              // OutputTime_DO
const char OutputDelayTimeDOKey[]       = "OutputDelayTime_DO";         // OutputDelayTime_DO
const char OutputPulseCycleDOKey[]      = "OutputPulseCycle_DO";        // OutputPulseCycle_DO
const char OutputPulseDutyCycleDOKey[]  = "OutputPulseDutyCycle_DO";    // OutputPulseDutyCycle_DO

#define DIO_PORT_OFFSET 1

DIO_PropertyData  settingData;

void DIO_PropertyItem_Init(void) {
    for (int i = 0; i < (NUM_DIO/2); i ++) {
        // DI
        settingData.diData[i].diFunctionType = DIFUNC_TYPE_NOTSELECTED;
        settingData.diData[i].intervalSec = DI_INTERVAL_DEFAULT_VALUE;
        settingData.diData[i].isEdgeTriggerHigh = false;
        settingData.diData[i].minPulseWidth = DI_MINPULSE_DEFAULT_VALUE;
        settingData.diData[i].maxPulseCount = DI_MAXCOUNT_DEFAULT_VALUE;
        settingData.diData[i].isPollingActiveHigh = false;

        // DO
        settingData.doData[i].doFunctionType = DOFUNC_TYPE_NOTSELECTED;
        settingData.doData[i].isDoNotify = false;
        settingData.doData[i].singleFunctionType = DOFUNC_SGL_TYPE_NOTSELECTED;
        settingData.doData[i].relationFunctionType = DOFUNC_REL_TYPE_NOTSELECTED;
        settingData.doData[i].relationDIPort = DI_REL_PORT_NOTSELECTED;
        settingData.doData[i].edgeType = DO_EDGE_NOTSELECTED;
        settingData.doData[i].startOutputCount = DO_STARTOUTPUTCOUNT_DEFAULT_VALUE;
        settingData.doData[i].stopOutputCount = DO_STOPOUTPUTCOUNT_DEFAULT_VALUE;
        settingData.doData[i].inputChatteringTime = DO_INPUTCHATTERING_DEFAULT_VALUE;
        settingData.doData[i].isDOOutputStateHigh = false;
        settingData.doData[i].doOutputTime = DO_OUTPUTTIME_DEFAULT_VALUE;
        settingData.doData[i].doOutputDelayTime = DO_OUTPUTDELAYTIME_DEFAULT_VALUE;
        settingData.doData[i].doPulseCycle = DO_PULSECYCLE_DEFAULT_VALUE;
        settingData.doData[i].doPulseDutyCycle = DO_PULSEDUTYCYCLE_DEFAULT_VALUE;
    }
}

DIO_PropertyData* 
DIO_PropertyItem_GetDataPtrs(void)
{
    return &settingData;
}

DIO_DIPropertyData* 
DIO_PropertyItem_GetDIDataPtrs(void)
{
    return settingData.diData;
}

DIO_DOPropertyData* 
DIO_PropertyItem_GetDODataPtrs(void)
{
    return settingData.doData;
}

static bool DIO_PropertyItem_GetIntValue(const json_value* jsonObj, uint32_t* value, int base,
    uint32_t defaultValue, vector item, const char* itemName) {
    bool ret = (json_GetIntValue(jsonObj, value, 10) || (jsonObj->type == json_null)) ? true : false;
    if (jsonObj->type == json_null) {
        *value = defaultValue;
        PropertyItems_AddItem(item, itemName, TYPE_NULL);
    } else {
        PropertyItems_AddItem(item, itemName, TYPE_NUM, *value);
    }
    return ret;
}

static bool DIO_PropertyItem_GetBoolValue(const json_value* jsonObj, bool* value, vector item, const char* itemName) {
    bool ret = json_GetBoolValue(jsonObj, value) ? true : false;
    PropertyItems_AddItem(item, itemName, TYPE_BOOL, *value);
    return ret;
}

bool DIO_PropertyItem_ParseJson(
    const json_value* json, vector propertyItem, const char* version)
{
    const size_t FunctionTypeDILen          = strlen(FunctionTypeDIKey);            // FunctionType_DI
    const size_t IntervalDILen              = strlen(IntervalDIKey);                // Interval_DI
    const size_t EdgeTriggerDILen           = strlen(EdgeTriggerDIkey);             // EdgeTrigger_DI
    const size_t MinPulseWidthDILen         = strlen(MinPulseWidthDIKey);           // MinPulseWidth_DI
    const size_t MaxPulseCountDILen         = strlen(MaxPulseCountDIKey);           // MaxPulseCount_DI
    const size_t PollIsActiveHighDILen      = strlen(PollIsActiveHighKey);          // PollIsActiveHigh_DI
    const size_t FunctionTypeDOLen          = strlen(FunctionTypeDOKey);            // FunctionType_DO
    const size_t IsNotifyDOLen              = strlen(IsNotifyDOKey);                // IsNotify_DO
    const size_t SingleFunctionTypeDOLen    = strlen(SingleFunctionTypeDOKey);      // SingleFunctionType_DO
    const size_t RelationFunctionTypeDOLen  = strlen(RelationFunctionTypeDOKey);    // RelationFunctionType_DO
    const size_t RelationPortDOLen          = strlen(RelationPortDOKey);            // RelationPort_DO
    const size_t EdgeTriggerTypeDOLen       = strlen(EdgeTriggerTypeDOKey);         // EdgeTriggerType_DO
    const size_t StartOutputCountDOLen      = strlen(StartOutputCountDOKey);        // StartOutputCount_DO
    const size_t StopOutputCountDOLen       = strlen(StopOutputCountDOKey);         // StopOutputCount_DO
    const size_t InputChatteringTimeDOLen   = strlen(InputChatteringTimeDOKey);     // InputChatteringTime_DO
    const size_t OutputStatusDOLen          = strlen(OutputStatusDOKey);            // OutputStatus_DO
    const size_t OutputTimeDOLen            = strlen(OutputTimeDOKey);              // OutputTime_DO
    const size_t OutputDelayTimeDOLen       = strlen(OutputDelayTimeDOKey);         // OutputDelayTime_DO
    const size_t OutputPulseCycleDOLen      = strlen(OutputPulseCycleDOKey);        // OutputPulseCycle_DO
    const size_t OutputPulseDutyCycleDOLen  = strlen(OutputPulseDutyCycleDOKey);    // OutputPulseDutyCycle_DO

    int pinid;
    for (int i = 0; i < json->u.object.length; i++) {
        char* propertyName = json->u.object.values[i].name;
        json_value* item = json->u.object.values[i].value;

        if (0 == strncmp(propertyName, FunctionTypeDIKey, FunctionTypeDILen)) { // DI Function Type
            if ((pinid = strtol(&propertyName[FunctionTypeDILen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DIFUNC_TYPE_NOTSELECTED, propertyItem, propertyName)) {
                settingData.diData[pinid].diFunctionType = value;
            }
        } else if (0 == strncmp(propertyName, IntervalDIKey, IntervalDILen)) { // DI Interval
            if ((pinid = strtol(&propertyName[IntervalDILen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DI_INTERVAL_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.diData[pinid].intervalSec = value;
            }
        } else if (0 == strncmp(propertyName, EdgeTriggerDIkey, EdgeTriggerDILen)) { // DI EdgeTriggerIsHigh
            if ((pinid = strtol(&propertyName[EdgeTriggerDILen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            bool value;
            if (DIO_PropertyItem_GetBoolValue(item, &value, propertyItem, propertyName)) {
                settingData.diData[pinid].isEdgeTriggerHigh = value;
            }
        } else if (0 == strncmp(propertyName, MinPulseWidthDIKey, MinPulseWidthDILen)) { // DI minPulseWidth
            if ((pinid = strtol(&propertyName[MinPulseWidthDILen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DI_MINPULSE_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.diData[pinid].minPulseWidth = value;
            }
        } else if (0 == strncmp(propertyName, MaxPulseCountDIKey, MaxPulseCountDILen)) { // DI maxPulseCount
            if ((pinid = strtol(&propertyName[MaxPulseCountDILen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DI_MAXCOUNT_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.diData[pinid].maxPulseCount = value;
            }
        } else if (0 == strncmp(propertyName, PollIsActiveHighKey, PollIsActiveHighDILen)) { // DI PollIsActiveHigh
            if ((pinid = strtol(&propertyName[PollIsActiveHighDILen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            bool value;
            if (DIO_PropertyItem_GetBoolValue(item, &value, propertyItem, propertyName)) {
                settingData.diData[pinid].isPollingActiveHigh = value;
            }
        } else if (0 == strncmp(propertyName, FunctionTypeDOKey, FunctionTypeDOLen)) { // DO Function Type
            if ((pinid = strtol(&propertyName[FunctionTypeDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DOFUNC_TYPE_NOTSELECTED, propertyItem, propertyName)) {
                settingData.doData[pinid].doFunctionType = value;
            }
        } else if (0 == strncmp(propertyName, IsNotifyDOKey, IsNotifyDOLen)) { // DO IsNotify
            if ((pinid = strtol(&propertyName[IsNotifyDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            bool value;
            if (DIO_PropertyItem_GetBoolValue(item, &value, propertyItem, propertyName)) {
                settingData.doData[pinid].isDoNotify = value;
            }
        } else if (0 == strncmp(propertyName, SingleFunctionTypeDOKey, SingleFunctionTypeDOLen)) { // DO Function Type(Single)
            if ((pinid = strtol(&propertyName[SingleFunctionTypeDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DOFUNC_SGL_TYPE_NOTSELECTED, propertyItem, propertyName)) {
                settingData.doData[pinid].singleFunctionType = value;
            }
        } else if (0 == strncmp(propertyName, RelationFunctionTypeDOKey, RelationFunctionTypeDOLen)) { // DO Function Type(Relation)
            if ((pinid = strtol(&propertyName[RelationFunctionTypeDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DOFUNC_REL_TYPE_NOTSELECTED, propertyItem, propertyName)) {
                settingData.doData[pinid].relationFunctionType = value;
            }
        } else if (0 == strncmp(propertyName, RelationPortDOKey, RelationPortDOLen)) { // DO RelationDIPort
            if ((pinid = strtol(&propertyName[RelationPortDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DI_REL_PORT_NOTSELECTED, propertyItem, propertyName)) {
                settingData.doData[pinid].relationDIPort = value;
            }
        } else if (0 == strncmp(propertyName, EdgeTriggerTypeDOKey, EdgeTriggerTypeDOLen)) { // DO EdgeTriggerType
            if ((pinid = strtol(&propertyName[EdgeTriggerTypeDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_EDGE_NOTSELECTED, propertyItem, propertyName)) {
                settingData.doData[pinid].edgeType = value;
            }
        } else if (0 == strncmp(propertyName, StartOutputCountDOKey, StartOutputCountDOLen)) { // DO StartOutputCount
            if ((pinid = strtol(&propertyName[StartOutputCountDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_STARTOUTPUTCOUNT_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.doData[pinid].startOutputCount = value;
            }
        } else if (0 == strncmp(propertyName, StopOutputCountDOKey, StopOutputCountDOLen)) { // DO StopOutputCount
            if ((pinid = strtol(&propertyName[StopOutputCountDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_STOPOUTPUTCOUNT_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.doData[pinid].stopOutputCount = value;
            }
        } else if (0 == strncmp(propertyName, InputChatteringTimeDOKey, InputChatteringTimeDOLen)) { // DO InputChatteringTime
            if ((pinid = strtol(&propertyName[InputChatteringTimeDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_INPUTCHATTERING_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.doData[pinid].inputChatteringTime = value;
            }
        } else if (0 == strncmp(propertyName, OutputStatusDOKey, OutputStatusDOLen)) { // DO OutputStatus
            if ((pinid = strtol(&propertyName[OutputStatusDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            bool value;
            if (DIO_PropertyItem_GetBoolValue(item, &value, propertyItem, propertyName)) {
                settingData.doData[pinid].isDOOutputStateHigh = value;
            }
        } else if (0 == strncmp(propertyName, OutputTimeDOKey, OutputTimeDOLen)) { // DO OutputTime
            if ((pinid = strtol(&propertyName[OutputTimeDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_OUTPUTTIME_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.doData[pinid].doOutputTime = value;
            }
        } else if (0 == strncmp(propertyName, OutputDelayTimeDOKey, OutputDelayTimeDOLen)) { // DO OutputDelayTime
            if ((pinid = strtol(&propertyName[OutputDelayTimeDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_OUTPUTDELAYTIME_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.doData[pinid].doOutputDelayTime = value;
            }
        } else if (0 == strncmp(propertyName, OutputPulseCycleDOKey, OutputPulseCycleDOLen)) { // DO OutputPulseCycle
            if ((pinid = strtol(&propertyName[OutputPulseCycleDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_PULSECYCLE_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.doData[pinid].doPulseCycle = value;
            }
        } else if (0 == strncmp(propertyName, OutputPulseDutyCycleDOKey, OutputPulseDutyCycleDOLen)) { // DO OutputPulseDutyCycle
            if ((pinid = strtol(&propertyName[OutputPulseDutyCycleDOLen], NULL, 10) - DIO_PORT_OFFSET) < 0) {
                continue;
            }
            uint32_t value;
            if (DIO_PropertyItem_GetIntValue(item, &value, 10, DO_PULSEDUTYCYCLE_DEFAULT_VALUE, propertyItem, propertyName)) {
                settingData.doData[pinid].doPulseDutyCycle = value;
            }
        }
    }
    return true;
}

static bool DIO_PropertyItem_VerifyDOOutputTime(DIO_DOPropertyData* doData) {
    bool ret = true;
    if ((!(doData->doOutputTime >= DO_OUTPUTTIME_MIN_VALUE && 
           doData->doOutputTime <= DO_OUTPUTTIME_MAX_VALUE)) ||
        (!(doData->doOutputDelayTime >= DO_OUTPUTDELAYTIME_MIN_VALUE &&
           doData->doOutputDelayTime <= DO_OUTPUTDELAYTIME_MAX_VALUE))) {
            ret = false;
    }
    return ret;
}

static bool DIO_PropertyItem_VerifyDOPulseSetting(DIO_DOPropertyData* doData) {
    bool ret = true;
    if ((!(doData->doPulseCycle >= DO_PULSECYCLE_MIN_VALUE &&
           doData->doPulseCycle <= DO_PULSECYCLE_MAX_VALUE)) ||
        (!(doData->doPulseDutyCycle >= DO_PULSEDUTYCYCLE_MIN_VALUE &&
           doData->doPulseDutyCycle <= DO_PULSEDUTYCYCLE_MAX_VALUE))) {
            ret = false;
    }
    return ret;
}

static bool DIO_PropertyItem_VerifyDOCountSetting(DIO_DOPropertyData* doData, DIO_DIPropertyData* diData) {
    bool ret = true;
    if ((!(doData->startOutputCount >= DO_STARTOUTPUTCOUNT_MIN_VALUE &&
           doData->startOutputCount <= DO_STARTOUTPUTCOUNT_MAX_VALUE)) ||
        (!(doData->stopOutputCount >= DO_STOPOUTPUTCOUNT_MIN_VALUE &&
           doData->stopOutputCount <= DO_STOPOUTPUTCOUNT_MAX_VALUE)) ||
        (!(doData->startOutputCount < doData->stopOutputCount)) ||
        (!(diData->maxPulseCount > doData->startOutputCount))) {
            ret = false;
    }
    return ret;
}

static bool DIO_PropertyItem_VerifyDOChatteringTime(DIO_DOPropertyData* doData) {
    bool ret = true;
    if ((!(doData->inputChatteringTime >= DO_INPUTCHATTERING_MIN_VALUE && 
           doData->inputChatteringTime <= DO_INPUTCHATTERING_MAX_VALUE))) {
            ret = false;
    }
    return ret;
}

bool DIO_PropertyItem_VerifyDOSetting(int doPinId){
    bool ret = true;
    uint32_t diPinId;

    switch (settingData.doData[doPinId].doFunctionType)
    {
    case DOFUNC_TYPE_NOTSELECTED:
        break;
    case DOFUNC_TYPE_SINGLE:
        switch (settingData.doData[doPinId].singleFunctionType)
        {
        case DOFUNC_SGL_TYPE_NOTSELECTED:
            // Error
            ret = false;
            break;
        case DOFUNC_SGL_TYPE_ONESHOT:
            // Option : doOutputTime, doOutputDelayTime
            ret = DIO_PropertyItem_VerifyDOOutputTime(&settingData.doData[doPinId]);
            break;
        case DOFUNC_SGL_TYPE_PULSEPWM:
            // Option : doOutputTime, doOutputDelayTime, doPulseCycle, doPulseDutyCycle
            ret = DIO_PropertyItem_VerifyDOOutputTime(&settingData.doData[doPinId]) &&
                  DIO_PropertyItem_VerifyDOPulseSetting(&settingData.doData[doPinId]);
            break;
        default:
            break;
        }
        break;
    case DOFUNC_TYPE_RELATION:
        if (settingData.doData[doPinId].relationDIPort == DI_REL_PORT_NOTSELECTED) {
            // Error
            ret = false;
            break;
        }
        if ((diPinId = settingData.doData[doPinId].relationDIPort - DIO_PORT_OFFSET) < 0) {
            // Error
            ret = false;
            break;
        }

        switch (settingData.doData[doPinId].relationFunctionType)
        {
        case DOFUNC_REL_TYPE_NOTSELECTED:
            // Error
            ret = false;
            break;
        case DOFUNC_REL_TYPE_ONESHOT:
            // Relation : PulseCount
            // Option : startOutputCount , stopOutputCount, doOutputTime, doOutputDelayTime
            if ((settingData.diData[diPinId].diFunctionType != DIFUNC_TYPE_PULSECOUNTER) ||
                (!DIO_PropertyItem_VerifyDOCountSetting(&settingData.doData[doPinId], &settingData.diData[diPinId])) ||
                (!DIO_PropertyItem_VerifyDOOutputTime(&settingData.doData[doPinId]))) {
                ret = false;
            }
            break;
        case DOFUNC_REL_TYPE_INTERLOCK:
        case DOFUNC_REL_TYPE_INVERT:
            // Relation : Edge
            // Option : inputChatteringTime, doOutputTime, doOutputDelayTime
            if ((settingData.diData[diPinId].diFunctionType != DIFUNC_TYPE_EDGE) ||
                (!DIO_PropertyItem_VerifyDOChatteringTime(&settingData.doData[doPinId])) ||
                (!DIO_PropertyItem_VerifyDOOutputTime(&settingData.doData[doPinId]))) {
                ret = false;
            }
            break;
        case DOFUNC_REL_TYPE_GENERATE:
            // Relation : Edge
            // Option : edgeType, inputChatteringTime, doOutputTime, doOutputDelayTime
            if ((settingData.diData[diPinId].diFunctionType != DIFUNC_TYPE_EDGE) ||
                (settingData.doData[doPinId].edgeType == DO_EDGE_NOTSELECTED) ||
                (!DIO_PropertyItem_VerifyDOChatteringTime(&settingData.doData[doPinId])) ||
                (!DIO_PropertyItem_VerifyDOOutputTime(&settingData.doData[doPinId]))) {
                ret = false;
            }
            break;
        case DOFUNC_REL_TYPE_PULSEPWM:
            // Relation : PulseCount or Edge
            if (settingData.diData[diPinId].diFunctionType == DIFUNC_TYPE_PULSECOUNTER) { // PulseCount
                // Option : startOutputCount, stopOutputCount, doOutputTime, doOutputDelayTime, doPulseCycle, doPwmDutyCycle
                if ((!DIO_PropertyItem_VerifyDOCountSetting(&settingData.doData[doPinId], &settingData.diData[diPinId])) ||
                    (!DIO_PropertyItem_VerifyDOOutputTime(&settingData.doData[doPinId])) ||
                    (!DIO_PropertyItem_VerifyDOPulseSetting(&settingData.doData[doPinId]))) {
                    ret = false;
                }

            } else if(settingData.diData[diPinId].diFunctionType == DIFUNC_TYPE_EDGE){ // Edge
                // Option : edgeType, inputChatteringTime, doOutputTime, doOutputDelayTime, doPulseCycle, doPwmDutyCycle
                if ((settingData.doData[doPinId].edgeType == DO_EDGE_NOTSELECTED) ||
                    (settingData.doData[doPinId].edgeType == DO_EDGE_BOTH) ||
                    (!DIO_PropertyItem_VerifyDOChatteringTime(&settingData.doData[doPinId])) ||
                    (!DIO_PropertyItem_VerifyDOOutputTime(&settingData.doData[doPinId])) ||
                    (!DIO_PropertyItem_VerifyDOPulseSetting(&settingData.doData[doPinId]))) {
                    ret = false;
                }
            } else{
                ret = false;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return ret;
}
