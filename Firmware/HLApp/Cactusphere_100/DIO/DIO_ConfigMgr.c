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
#include "DIO_DIFetchConfig.h"
#include "DIO_DIWatchConfig.h"
#include "DIO_DOWatchConfig.h"
#include "DIO_PropertyItem.h"
#include "PropertyItems.h"

typedef struct DIO_ConfigMgr {
    DIO_DIFetchConfig* fetchConfig;
    DIO_DIWatchConfig* watchConfig;
    DIO_DOWatchConfig* doWatchConfig;
} DIO_ConfigMgr;

#define DIO_PORT_OFFSET 1

static DIO_ConfigMgr sDIO_ConfigMgr;  // singleton

// Initializaition and cleanup
void
DIO_ConfigMgr_Initialize(void)
{
    sDIO_ConfigMgr.fetchConfig   = DIO_DIFetchConfig_New();
    sDIO_ConfigMgr.watchConfig   = DIO_DIWatchConfig_New();
    sDIO_ConfigMgr.doWatchConfig = DIO_DOWatchConfig_New();
    DIO_PropertyItem_Init();
}

void
DIO_ConfigMgr_Cleanup(void)
{
    DIO_DIFetchConfig_Destroy(sDIO_ConfigMgr.fetchConfig);
    DIO_DIWatchConfig_Destroy(sDIO_ConfigMgr.watchConfig);
    DIO_DOWatchConfig_Destroy(sDIO_ConfigMgr.doWatchConfig);
}

// Apply new configuration
SphereWarning
DIO_ConfigMgr_LoadAndApplyIfChanged(const unsigned char* payload,
    unsigned int payloadSize, vector item)
{
    json_value* jsonObj = json_parse(payload, payloadSize);
    json_value* desiredObj = json_GetKeyJson("desired", jsonObj);
    SphereWarning ret = NO_ERROR;

    if (desiredObj) {
        jsonObj = desiredObj;
    }

    DIO_PropertyItem_ParseJson(jsonObj, item, "1.0");
    DIO_PropertyData *data = DIO_PropertyItem_GetDataPtrs();

    if (strstr(payload, "_DI") != NULL){
        if (! DIO_DIFetchConfig_LoadFromJSON(
            sDIO_ConfigMgr.fetchConfig, data, jsonObj, item, "1.0")) {
            Log_Debug("failed to DIO_FetchConfig_LoadFromJSON().\n");
            ret = ILLEGAL_PROPERTY;
        }

        if (! DIO_DIWatchConfig_LoadFromJSON(
            sDIO_ConfigMgr.watchConfig, data, jsonObj, item, "1.0")) {
            Log_Debug("failed to DIO_WatchConfig_LoadFromJSON().\n");
            ret = ILLEGAL_PROPERTY;
        }
    }

    if ( (jsonObj->u.object.length > 1) && (vector_size(item) < 1)) {
        ret = UNSUPPORTED_PROPERTY;
    }

    return ret;
}

bool
DIO_ConfigMgr_RecvStartCommand(int pinId)
{
    return DIO_DOWatchConfig_AddConfig(sDIO_ConfigMgr.doWatchConfig, pinId);
}

bool
DIO_ConfigMgr_RecvStopCommand(int pinId)
{
    return DIO_DOWatchConfig_DelConfig(sDIO_ConfigMgr.doWatchConfig, pinId);
}

// Get configuratioin
DIO_DIFetchConfig*
DIO_ConfigMgr_GetFetchConfig()
{
    return sDIO_ConfigMgr.fetchConfig;
}

DIO_DIWatchConfig*
DIO_ConfigMgr_GetWatchConfig()
{
    return sDIO_ConfigMgr.watchConfig;
}

DIO_DOWatchConfig*
DIO_ConfigMgr_GetDOWatchConfig()
{
    return sDIO_ConfigMgr.doWatchConfig;
}