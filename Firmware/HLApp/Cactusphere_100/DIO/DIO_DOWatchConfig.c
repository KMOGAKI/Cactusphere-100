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

#include "DIO_DOWatchConfig.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "json.h"
#include "DIO_DOWatchItem.h"
#include "TelemetryItems.h"
#include "PropertyItems.h"
#include "DIO_PropertyItem.h"

struct DIO_DOWatchConfig {
    vector	mWatchItems;	// vector of DO port configuration
    char	version[32];	// version string (not using)
};

#define DIO_DOWATCH_PORT_OFFSET 1

// Initialization and cleanup
DIO_DOWatchConfig*
DIO_DOWatchConfig_New(void)
{
    DIO_DOWatchConfig*	newObj =
        (DIO_DOWatchConfig*)malloc(sizeof(DIO_DOWatchConfig));

    if (NULL != newObj) {
        newObj->mWatchItems = vector_init(sizeof(DIO_DOWatchItem));
        if (NULL == newObj->mWatchItems) {
            free(newObj);
            return NULL;
        }
        memset(newObj->version, 0, sizeof(newObj->version));
    }

    return newObj;
}

void
DIO_DOWatchConfig_Destroy(DIO_DOWatchConfig* me)
{
    vector_destroy(me->mWatchItems);
    free(me);
}

static void DIO_DOWatchConfig_CalcPulseSetting(uint32_t frequency,
               uint32_t duty,
               uint32_t* clock,
               uint32_t* effectiveTime,
               uint32_t* pulsePeriod)
{
    const double resolution_32KHz = 31.25 * 0.001 * 0.001;
    const double resolution_2MHz = 500 * 0.001 * 0.001 * 0.001;

    double period = 1.00 / (double)frequency;
    double ontime = period * ((double)duty/100.0);
    double ontime_32K = (ontime / resolution_32KHz);
    double ontime_2M  = (ontime / resolution_2MHz);
    
    // default clock : 2MHz 
    if (ontime_2M > 65535){
        *clock = 32 * 1000; // 32KHz
        *effectiveTime = (uint32_t)ontime_32K;
        *pulsePeriod = (uint32_t)(period / resolution_32KHz);
    } else {
        *clock = 2 * 1000 * 1000; // 2MHz
        *effectiveTime = (uint32_t)ontime_2M;
        *pulsePeriod = (uint32_t)(period / resolution_2MHz);
    }
}

static bool DIO_DOWatchConfig_SetSingleConfig(DIO_DOSingleConfig *config, DIO_DOPropertyData *data)
{
    bool ret = true;

    switch (data->singleFunctionType)
    {
    case DOFUNC_SGL_TYPE_ONESHOT:
        // setting : outputLevel, outputTime, delayTime
        config->doFunctionType = DO_FUNCTYPE_ONESHOT;
        config->outputLevel = data->isDOOutputStateHigh;
        config->outputTime = data->doOutputTime;
        config->delayTime = data->doOutputDelayTime;
        break;
    case DOFUNC_SGL_TYPE_PULSEPWM:
        // setting : pulseClock, pulseEffectiveTime, pulsePeriod, outputTime, delayTime
        config->doFunctionType = DO_FUNCTYPE_PULSE;
        DIO_DOWatchConfig_CalcPulseSetting(
            data->doPulseCycle,
            data->doPulseDutyCycle,
            &config->pulseClock,
            &config->pulseEffectiveTime,
            &config->pulsePeriod
        );
        config->outputTime = data->doOutputTime;
        config->delayTime  = data->doOutputDelayTime;
        break;
    default:
        ret = false;
    }
    return ret;
}

static bool DIO_DOWatchConfig_SetRelationEdgeConfig(DIO_DORelateEdgeTriggerConfig *config, DIO_DOPropertyData *data, uint32_t relatePinId)
{
    bool ret = true;
    switch (data->relationFunctionType)
    {
    case DOFUNC_REL_TYPE_INTERLOCK:
        // setting : relationPort, chatteringVal, outputTime, delayTime
        config->doFunctionType = DO_FUNCTYPE_INTERLOCK;
        config->relationPort = relatePinId;
        config->chatteringVal = data->inputChatteringTime;
        config->outputTime = data->doOutputTime;
        config->delayTime = data->doOutputDelayTime;
        break;
    case DOFUNC_REL_TYPE_INVERT:
        // setting : relationPort, chatteringVal, outputTime, delayTime
        config->doFunctionType = DO_FUNCTYPE_INVERT;
        config->relationPort = relatePinId;
        config->chatteringVal = data->inputChatteringTime;
        config->outputTime = data->doOutputTime;
        config->delayTime = data->doOutputDelayTime;
        break;
    case DOFUNC_REL_TYPE_GENERATE:
        // setting : relationPort, edgeType, chatteringVal,
        //           outputLevel, outputTime, delayTime
        config->doFunctionType = DO_FUNCTYPE_GENERATE;
        config->relationPort = relatePinId;
        config->edgeType = data->edgeType - 1; // RTApp: Rising(0),Falling(1),Both(2)
        config->chatteringVal = data->inputChatteringTime;
        config->outputLevel = data->isDOOutputStateHigh;
        config->outputTime = data->doOutputTime;
        config->delayTime = data->doOutputDelayTime;
        break;
    case DOFUNC_REL_TYPE_PULSEPWM:
        // setting : relationPort, edgeType, chatteringVal,
        //           pulseClock, pulseEffectiveTime, pulsePeriod, outputTime, delaytTime
        config->doFunctionType = DO_FUNCTYPE_PULSE;
        config->relationPort = relatePinId;
        config->edgeType = data->edgeType - 1; // RTApp: Rising(0),Falling(1)
        config->chatteringVal = data->inputChatteringTime;
        DIO_DOWatchConfig_CalcPulseSetting(
            data->doPulseCycle,
            data->doPulseDutyCycle,
            &config->pulseClock,
            &config->pulseEffectiveTime,
            &config->pulsePeriod
        );
        config->outputTime = data->doOutputTime;
        config->delayTime = data->doOutputDelayTime;
        break;
    default:
        ret = false;
    }

    return ret;
}

static bool DIO_DOWatchConfig_SetRelationPulseConfig(DIO_DORelatePulseTriggerConfig *config, DIO_DOPropertyData *data, uint32_t relatePinId)
{
    bool ret = true;
    switch (data->relationFunctionType)
    {
    case DOFUNC_REL_TYPE_ONESHOT:
        // setting : relationPort, startOutputCount, stopOutputCount,
        //           outputLevel, outputTime, outputTime, delayTime
        config->doFunctionType = DO_FUNCTYPE_ONESHOT;
        config->relationPort = relatePinId;
        config->startOutputCount = data->startOutputCount;
        config->stopOutputCount = data->stopOutputCount;
        config->outputLevel = data->isDOOutputStateHigh;
        config->outputTime = data->doOutputTime;
        config->delayTime = data->doOutputDelayTime;
        break;
    case DOFUNC_REL_TYPE_PULSEPWM:
        // setting : relationPort, startOutputCount, stopOutputCount,
        //           pulseClock, pulseEffectiveTime, pulsePeriod, doOutputTime, doOutputDelayTime
        config->doFunctionType = DO_FUNCTYPE_PULSE;
        config->relationPort = relatePinId;
        config->startOutputCount = data->startOutputCount;
        config->stopOutputCount = data->stopOutputCount;
        DIO_DOWatchConfig_CalcPulseSetting(
            data->doPulseCycle,
            data->doPulseDutyCycle,
            &config->pulseClock,
            &config->pulseEffectiveTime,
            &config->pulsePeriod
        );
        config->outputTime = data->doOutputTime;
        config->delayTime = data->doOutputDelayTime;
        break;
    default:
        ret = false;
    }

    return ret;
}

bool
DIO_DOWatchConfig_AddConfig(DIO_DOWatchConfig* me, int pinId)
{
    bool ret = true;
    if ((ret = DIO_PropertyItem_VerifyDOSetting(pinId))) {
        if (0 != vector_size(me->mWatchItems)) {
            DIO_DOWatchItem*    curs = (DIO_DOWatchItem*)vector_get_data(me->mWatchItems);
            
            for (int i = 0, n = vector_size(me->mWatchItems); i < n; ++i) {
                if (curs->pinID == pinId) {
                    vector_remove_at(me->mWatchItems, i);
                    TelemetryItems_RemoveDictionaryElem(curs->telemetryName);
                    break;
                }
                curs++;
            }
        }

        DIO_PropertyData *data = DIO_PropertyItem_GetDataPtrs();
        DIO_DOWatchItem item;
        memset(&item, 0, sizeof(DIO_DOWatchItem));

        sprintf(item.telemetryName, "DO%d_Status", pinId + DIO_DOWATCH_PORT_OFFSET);
        item.pinID = (uint32_t)pinId;

        uint32_t diPinId;
        switch (data->doData[pinId].doFunctionType)
        {
        case DOFUNC_TYPE_SINGLE:
            item.doMode = DO_MODE_SINGLE;
            item.isNotify = data->doData[pinId].isDoNotify;
            ret = DIO_DOWatchConfig_SetSingleConfig(&item.config.single, &data->doData[pinId]);
            break;
        case DOFUNC_TYPE_RELATION:
            diPinId = data->doData[pinId].relationDIPort - DIO_DOWATCH_PORT_OFFSET;
            switch (data->doData[pinId].relationFunctionType)
            {
            case DOFUNC_REL_TYPE_ONESHOT:
                item.doMode = DO_MODE_RELATE_PULSETRIGGER;
                break;
            case DOFUNC_REL_TYPE_INTERLOCK:
            case DOFUNC_REL_TYPE_INVERT:
            case DOFUNC_REL_TYPE_GENERATE:
                item.doMode = DO_MODE_RELATE_EDGETRIGGER;
                break;
            case DOFUNC_REL_TYPE_PULSEPWM:
                item.doMode = (data->diData[diPinId].diFunctionType == DIFUNC_TYPE_EDGE) ? DO_MODE_RELATE_EDGETRIGGER : DO_MODE_RELATE_PULSETRIGGER;
                break;
            default:
                ret = false;
                goto err;
            }
            item.isNotify = data->doData[pinId].isDoNotify;

            if (item.doMode == DO_MODE_RELATE_EDGETRIGGER) { // Edge
                ret = DIO_DOWatchConfig_SetRelationEdgeConfig(&item.config.relateEdgeTrigger, &data->doData[pinId], diPinId);
            } else { // PulseCount
                ret = DIO_DOWatchConfig_SetRelationPulseConfig(&item.config.relatePulseTrigger, &data->doData[pinId], diPinId);
            }
            break;
        default:
            ret = false;
        }

        if (ret) {
            vector_add_last(me->mWatchItems, &item);
            TelemetryItems_AddDictionaryElem(item.telemetryName, false);
        }
    } 

err:
    return ret;
}

bool
DIO_DOWatchConfig_DelConfig(DIO_DOWatchConfig* me, int pinId)
{
    if (0 != vector_size(me->mWatchItems)) {
        DIO_DOWatchItem*    curs = (DIO_DOWatchItem*)vector_get_data(me->mWatchItems);
        
        for (int i = 0, n = vector_size(me->mWatchItems); i < n; ++i) {
            if (curs->pinID == pinId) {
                vector_remove_at(me->mWatchItems, i);
                TelemetryItems_RemoveDictionaryElem(curs->telemetryName);
                break;
            }
            curs++;
        }
    }

    return true;
}

vector
DIO_DOWatchConfig_GetFetchItems(DIO_DOWatchConfig* me)
{
    return me->mWatchItems;
}