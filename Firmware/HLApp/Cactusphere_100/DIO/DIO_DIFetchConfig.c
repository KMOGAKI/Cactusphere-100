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

#include "DIO_DIFetchConfig.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "json.h"
#include "DIO_DIFetchItem.h"
#include "TelemetryItems.h"
#include "PropertyItems.h"

struct DIO_DIFetchConfig {
    vector	mFetchItems;    // vector of DIO pulse conter configuration
    vector	mFetchItemPtrs;	// vector of pointer which points mFetchItem's elem
    char	version[32];	// version string (not using)
};

#define DIO_FETCH_PORT_OFFSET 1

// Initialization and cleanup
DIO_DIFetchConfig*
DIO_DIFetchConfig_New(void)
{
    DIO_DIFetchConfig*	newObj =
        (DIO_DIFetchConfig*)malloc(sizeof(DIO_DIFetchConfig));

    if (NULL != newObj) {
        newObj->mFetchItems = vector_init(sizeof(DIO_DIFetchItem));
        if (NULL == newObj->mFetchItems) {
            free(newObj);
            return NULL;
        }
        newObj->mFetchItemPtrs = vector_init(sizeof(DIO_DIFetchItem*));
        if (NULL == newObj->mFetchItemPtrs) {
            vector_destroy(newObj->mFetchItems);
            free(newObj);
            return NULL;
        }
        memset(newObj->version, 0, sizeof(newObj->version));
    }

    return newObj;
}

void
DIO_DIFetchConfig_Destroy(DIO_DIFetchConfig* me)
{
    vector_destroy(me->mFetchItemPtrs);
    vector_destroy(me->mFetchItems);
    free(me);
}


// Load DIO pulse conter configuration from JSON
bool
DIO_DIFetchConfig_LoadFromJSON(DIO_DIFetchConfig* me, DIO_PropertyData* data,
    const json_value* json, vector propertyItem, const char* version)
{
    bool ret = true;

    if (! json) {
        return false;
    }

    DIO_DIFetchItem currentValue[NUM_DI] = {
        // telemetryName, intervalSec, pinID, isPulseCounter, isCountClear, isPulseHigh, isPollingActiveHigh, minPulseWidth, maxPulseCount
        {"", DI_INTERVAL_DEFAULT_VALUE, 0, false, false, false, false, DI_MINPULSE_DEFAULT_VALUE, DI_MAXCOUNT_DEFAULT_VALUE},
        {"", DI_INTERVAL_DEFAULT_VALUE, 1, false, false, false, false, DI_MINPULSE_DEFAULT_VALUE, DI_MAXCOUNT_DEFAULT_VALUE}
    };

    if (! vector_is_empty(me->mFetchItems)) {
        DIO_DIFetchItem* tmp = (DIO_DIFetchItem*)vector_get_data(me->mFetchItems);
        for (int i = 0; i < vector_size(me->mFetchItems); i++) {
            memcpy(&currentValue[tmp->pinID], tmp, sizeof(DIO_DIFetchItem));
            tmp ++;
        }
    }

    if (0 != vector_size(me->mFetchItems)) {
        DIO_DIFetchItem*	curs = (DIO_DIFetchItem*)vector_get_data(me->mFetchItems);

        for (int i = 0, n = vector_size(me->mFetchItems); i < n; ++i) {
            TelemetryItems_RemoveDictionaryElem(curs->telemetryName);
            curs ++;
        }
        vector_clear(me->mFetchItemPtrs);
        vector_clear(me->mFetchItems);
    }

    for (uint32_t i = 0; i < NUM_DI; i++) {
        DIO_DIFetchItem config =
        // telemetryName, intervalSec, pinID, isPulseCounter, isCountClear, isPulseHigh, isPollingActiveHigh, minPulseWidth, maxPulseCount
        {"", DI_INTERVAL_DEFAULT_VALUE, 0, false, true, false, false, DI_MINPULSE_DEFAULT_VALUE, DI_MAXCOUNT_DEFAULT_VALUE};

        switch(data->diData[i].diFunctionType) {
            case DIFUNC_TYPE_PULSECOUNTER:
                // telemetryName
                sprintf(config.telemetryName, "DI%d_count", i + DIO_FETCH_PORT_OFFSET);
                // intervalSec
                if(data->diData[i].intervalSec >= DI_INTERVAL_MIN_VALUE && data->diData[i].intervalSec <= DI_INTERVAL_MAX_VALUE) {
                    config.intervalSec = data->diData[i].intervalSec;
                } else {
                    ret = false;
                    continue;
                }
                // pinID
                config.pinID = i;
                // isPulseCounter
                config.isPulseCounter = true;
                // isCountClear
                if (currentValue[i].isPulseCounter) {
                    if((currentValue[i].intervalSec == data->diData[i].intervalSec) &&
                       (currentValue[i].isPulseHigh == data->diData[i].isEdgeTriggerHigh) &&
                       (currentValue[i].minPulseWidth == data->diData[i].minPulseWidth) &&
                       (currentValue[i].maxPulseCount == data->diData[i].maxPulseCount)) {
                        config.isCountClear = false;
                    }
                }
                // isPulseHigh
                config.isPulseHigh = data->diData[i].isEdgeTriggerHigh;
                // minPulseWidth
                if (data->diData[i].minPulseWidth >= DI_MINPULSE_MIN_VALUE && data->diData[i].minPulseWidth <= DI_MINPULSE_MAX_VALUE) {
                    config.minPulseWidth = data->diData[i].minPulseWidth;
                } else {
                    ret = false;
                    continue;
                }
                // maxPulseCount
                if (data->diData[i].maxPulseCount >= DI_MAXCOUNT_MIN_VALUE && data->diData[i].maxPulseCount <= DI_MAXCOUNT_MAX_VALUE) {
                    config.maxPulseCount = data->diData[i].maxPulseCount;
                } else {
                    ret = false;
                    continue;
                }
                break;
            case DIFUNC_TYPE_POLLING:
                // telemetryName
                sprintf(config.telemetryName, "DI%d_PollingStatus", i + DIO_FETCH_PORT_OFFSET);
                // intervalSec
                if(data->diData[i].intervalSec >= DI_INTERVAL_MIN_VALUE && data->diData[i].intervalSec <= DI_INTERVAL_MAX_VALUE) {
                    config.intervalSec = data->diData[i].intervalSec;
                } else {
                    ret = false;
                    continue;
                }
                // pinID
                config.pinID = i;
                // isPollingActiveHigh
                config.isPollingActiveHigh = data->diData[i].isPollingActiveHigh;
                break;
            case DIFUNC_TYPE_NOTSELECTED:
            case DIFUNC_TYPE_EDGE:
            default:
                continue;
        }
        vector_add_last(me->mFetchItems, &config);
    }

    if (! vector_is_empty(me->mFetchItems)) {
        // store entity's pointers
        DIO_DIFetchItem* curs = (DIO_DIFetchItem*)vector_get_data(me->mFetchItems);

        for (int i = 0, n = vector_size(me->mFetchItems); i < n; ++i) {
            vector_add_last(me->mFetchItemPtrs, &curs);
            TelemetryItems_AddDictionaryElem(curs->telemetryName, false);
            ++curs;
        }
    }

    return ret;
}

// Get configuration of DIO pulse conters
vector
DIO_DIFetchConfig_GetFetchItems(DIO_DIFetchConfig* me)
{
    return me->mFetchItems;
}

vector
DIO_DIFetchConfig_GetFetchItemPtrs(DIO_DIFetchConfig* me)
{
    return me->mFetchItemPtrs;
}

// Get enable port number of DIO pulse counter
int
DIO_DIFetchConfig_GetFetchEnablePorts(DIO_DIFetchConfig* me,
    bool* counterStatus, bool* pollingStatus)
{
    DIO_DIFetchItem* tmp = (DIO_DIFetchItem*)vector_get_data(me->mFetchItems);
    int enablePort = vector_size(me->mFetchItems);

    for (int i = 0; i < enablePort; i++){
        if (tmp->isPulseCounter) {
            counterStatus[tmp->pinID] = true;
        } else {
            pollingStatus[tmp->pinID] = true;
        }
        tmp++;
    }

    return enablePort;
}
