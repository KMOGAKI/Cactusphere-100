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

#include "DIO_DIWatchConfig.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "json.h"
#include "DIO_DIWatchItem.h"
#include "TelemetryItems.h"
#include "PropertyItems.h"

struct DIO_DIWatchConfig {
    vector	mWatchItems;	// vector of DIO contact input configuration
    char	version[32];	// version string (not using)
};

#define DIO_WATCH_PORT_OFFSET 1

// Initialization and cleanup
DIO_DIWatchConfig*
DIO_DIWatchConfig_New(void)
{
    DIO_DIWatchConfig*	newObj =
        (DIO_DIWatchConfig*)malloc(sizeof(DIO_DIWatchConfig));

    if (NULL != newObj) {
        newObj->mWatchItems = vector_init(sizeof(DIO_DIWatchItem));
        if (NULL == newObj->mWatchItems) {
            free(newObj);
            return NULL;
        }
        memset(newObj->version, 0, sizeof(newObj->version));
    }

    return newObj;
}

void
DIO_DIWatchConfig_Destroy(DIO_DIWatchConfig* me)
{
    vector_destroy(me->mWatchItems);
    free(me);
}

// Load DIO contact input watcher configuration from JSON
bool
DIO_DIWatchConfig_LoadFromJSON(DIO_DIWatchConfig* me, DIO_PropertyData* data,
    const json_value* json, vector propertyItem, const char* version)
{
    bool ret = true;

    if (! json) {
        return false;
    }

    if (0 != vector_size(me->mWatchItems)) {
        DIO_DIWatchItem*	curs = (DIO_DIWatchItem*)vector_get_data(me->mWatchItems);

        for (int i = 0, n = vector_size(me->mWatchItems); i < n; ++i) {
            TelemetryItems_RemoveDictionaryElem(curs->telemetryName);
            curs ++;
        }
        vector_clear(me->mWatchItems);
        memset(me->version, 0, sizeof(me->version));
    }

    for (uint32_t i = 0; i < NUM_DI; i++){
        DIO_DIWatchItem config =
        // telemetryName, pinID, notifyChangeForHigh, isCountClear
        { "", 0, false, false};

        switch(data->diData[i].diFunctionType) {
            case DIFUNC_TYPE_EDGE:
                // telemetryName
                sprintf(config.telemetryName, "DI%d_EdgeEvent", i + DIO_WATCH_PORT_OFFSET);
                // pinID
                config.pinID = i;
                // notifyChangeForHigh
                config.notifyChangeForHigh = data->diData[i].isEdgeTriggerHigh;
                break;
            case DIFUNC_TYPE_NOTSELECTED:
            case DIFUNC_TYPE_PULSECOUNTER:
            default:
                continue;
        }

        vector_add_last(me->mWatchItems, &config);
    }

    if (! vector_is_empty(me->mWatchItems)) {
        // store entity's pointers
        DIO_DIWatchItem*	curs = (DIO_DIWatchItem*)vector_get_data(me->mWatchItems);

        for (int i = 0, n = vector_size(me->mWatchItems); i < n; ++i) {
            TelemetryItems_AddDictionaryElem(curs->telemetryName, false);
            ++curs;
        }
    }

    return ret;
}

// Get configuration of DIO contact input watchers
vector
DIO_DIWatchConfig_GetFetchItems(DIO_DIWatchConfig* me)
{
    return me->mWatchItems;
}

// Get enable port number of DIO contact input
int
DIO_DIWatchConfig_GetWatchEnablePorts(DIO_DIWatchConfig* me, bool* status)
{
    DIO_DIWatchItem* tmp;
    int enablePort = vector_size(me->mWatchItems);

    for (int i = 0; i < enablePort; i++) {
        tmp = (DIO_DIWatchItem*)vector_get_data(me->mWatchItems);
        status[tmp->pinID] = true;
    }

    return enablePort;
}
