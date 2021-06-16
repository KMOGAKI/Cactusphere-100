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

#include <applibs/gpio.h>

#include "DIO_DataFetchScheduler.h"

#include "DIO_DIFetchItem.h"
#include "DIO_DIFetchTargets.h"
#include "DIO_DIWatcher.h"
#include "DIO_DIWatchItem.h"
#include "DIO_DOWatcher.h"
#include "DIO_DOWatchItem.h"
#include "LibCloud.h"
#include "LibDIO.h"
#include "StringBuf.h"
#include "TelemetryItems.h"

typedef struct DIO_DataFetchScheduler {
    DataFetchSchedulerBase	Super;

// data member
    DIO_DIFetchTargets*    mFetchTargets;  // acquisition targets of pulse conter
    DIO_DIWatcher*         mWatcher;       // contact input watch targets
    DIO_DOWatcher*         mDOWatcher;     // DO port watch targets
} DIO_DataFetchScheduler;

#define DIO_POLLING_VALUE_OFF  0
#define DIO_POLLING_VALUE_ON   1

//
// DIO_DataFetchScheduler's private procedure/method
//
// Callback procedure of FetchTimers
static void
DIO_FetchTimerCallback(void* arg, const FetchItemBase* fetchTarget)
{
    // This procedure called against the acquisition target which
    // timer expired
    DIO_DataFetchScheduler* scheduler = (DIO_DataFetchScheduler*)arg;

    DIO_DIFetchTargets_Add(
        scheduler->mFetchTargets, (const DIO_DIFetchItem*)fetchTarget);
}

// Virtual method
static void
DIO_DataFetchScheduler_DoDestroy(DataFetchSchedulerBase* me)
{
    // cleanup own member
    DIO_DataFetchScheduler* self = (DIO_DataFetchScheduler*)me;

    DIO_DIFetchTargets_Destroy(self->mFetchTargets);
    DIO_DIWatcher_Destroy(self->mWatcher);
    DIO_DOWatcher_Destroy(self->mDOWatcher);
}

static void
DIO_DataFetchScheduler_ClearFetchTargets(DataFetchSchedulerBase* me)
{
    DIO_DataFetchScheduler* self = (DIO_DataFetchScheduler*)me;

    DIO_DIFetchTargets_Clear(self->mFetchTargets);
}

static void
DIO_DataFetchScheduler_DoSchedule(DataFetchSchedulerBase* me)
{
    // acquire telemetry value from the pulse conter which timer expired and
    // the contact input which input signal changed
    DIO_DataFetchScheduler* self = (DIO_DataFetchScheduler*)me;
    vector	items;

    // pulse conters & polling
    items = DIO_DIFetchTargets_GetFetchItems(self->mFetchTargets);
    if (! vector_is_empty(items)) {
        const DIO_DIFetchItem** itemsCurs = (const DIO_DIFetchItem**)vector_get_data(items);

        for (int i = 0, n = vector_size(items); i < n; i++) {
            const DIO_DIFetchItem* item = *itemsCurs++;

            if (item->isPulseCounter) {
                unsigned long pulseCount = 0;

                if (! DIO_Lib_ReadPulseCount(item->pinID, &pulseCount)) {
                    continue;
                };
                StringBuf_AppendByPrintf(me->mStringBuf, "%lu", pulseCount);
            } else {
                unsigned int currentStatus = 0;

                if (! DIO_Lib_ReadPinLevel(item->pinID, &currentStatus)) {
                    continue;
                };

                // In the case of Active-Low, telemetry value is converted.
                // IsActiveHigh: false(Active-Low) -> GPIO_Value_Low: DI_POLLING_VALUE_ON (1), GPIO_Value_High : DI_POLLING_VALUE_OFF(0)
                // IsActiveHigh: true(Active-High) -> GPIO_Value_Low: DI_POLLING_VALUE_OFF(0), GPIO_Value_High : DI_POLLING_VALUE_ON (1)
                if (!item->isPollingActiveHigh) {
                    currentStatus = (currentStatus == GPIO_Value_Low ? DIO_POLLING_VALUE_ON : DIO_POLLING_VALUE_OFF);
                }
                
                StringBuf_AppendByPrintf(me->mStringBuf, "%ld", currentStatus);
            }

            TelemetryItems_Add(me->mTelemetryItems,
                item->telemetryName, StringBuf_GetStr(me->mStringBuf));
            StringBuf_Clear(me->mStringBuf);
        }
    }

    // contact inputs
    if (DIO_DIWatcher_DoWatch(self->mWatcher)) {
        const vector	lastChanges = DIO_DIWatcher_GetLastChanges(self->mWatcher);

        for (int i = 0, n = vector_size(lastChanges); i < n; ++i) {
            DIO_DIWatchItemStat* wiStat;

            vector_get_at(&wiStat, lastChanges, i);

            StringBuf_AppendByPrintf(me->mStringBuf, "%ld", 1);
            TelemetryItems_Add(me->mTelemetryItems,
                wiStat->watchItem->telemetryName, StringBuf_GetStr(me->mStringBuf));
            StringBuf_Clear(me->mStringBuf);
        }
    }

    // DO status
    items = DIO_DOWatcher_GetWatchItems(self->mDOWatcher);
    if (! vector_is_empty(items)) {
        DIO_DOWatchItem*    curs = (DIO_DOWatchItem*)vector_get_data(items);
        
        for (int i = 0, n = vector_size(items); i < n; ++i) {
            unsigned int status;

            if (curs->isNotify) {
                if (! DIO_Lib_GetRelationStatus(curs->pinID, &status)) {
                    continue;
                }

                StringBuf_AppendByPrintf(me->mStringBuf, "%ld", status);
                TelemetryItems_Add(me->mTelemetryItems,
                    curs->telemetryName, StringBuf_GetStr(me->mStringBuf));
                StringBuf_Clear(me->mStringBuf);
            }

            curs++;
        }
    }
}

bool
DIO_DataFetchScheduler_StartDOOutput(DataFetchScheduler* me, int pinId, vector doWatchItems)
{
    DIO_DataFetchScheduler* self = (DIO_DataFetchScheduler*)me;
    bool ret = true;

    if (0 != vector_size(doWatchItems)) {
        DIO_DOWatchItem*    curs = (DIO_DOWatchItem*)vector_get_data(doWatchItems);
            
        for (int i = 0, n = vector_size(doWatchItems); i < n; ++i) {
            if (curs->pinID == pinId) {
                switch (curs->doMode)
                {
                case DO_MODE_SINGLE:
                    ret = DIO_Lib_ConfigSingle(
                            curs->pinID, curs->config.single.doFunctionType,
                            curs->config.single.outputLevel,
                            curs->config.single.pulseClock,
                            curs->config.single.pulseEffectiveTime,
                            curs->config.single.pulsePeriod,
                            curs->config.single.delayTime,
                            curs->config.single.outputTime);
                    break;                                        
                case DO_MODE_RELATE_EDGETRIGGER:
                    ret = DIO_Lib_ConfigEdgeTrigger(
                            curs->pinID, curs->config.relateEdgeTrigger.doFunctionType,
                            curs->config.relateEdgeTrigger.edgeType,
                            curs->config.relateEdgeTrigger.chatteringVal,
                            curs->config.relateEdgeTrigger.outputLevel,
                            curs->config.relateEdgeTrigger.pulseClock,
                            curs->config.relateEdgeTrigger.pulseEffectiveTime,
                            curs->config.relateEdgeTrigger.pulsePeriod,
                            curs->config.relateEdgeTrigger.delayTime,
                            curs->config.relateEdgeTrigger.outputTime,
                            curs->config.relateEdgeTrigger.relationPort);
                    break;
                case DO_MODE_RELATE_PULSETRIGGER:
                    ret = DIO_Lib_ConfigPulseCountTrigger(
                            curs->pinID, curs->config.relatePulseTrigger.doFunctionType,
                            curs->config.relatePulseTrigger.startOutputCount,
                            curs->config.relatePulseTrigger.stopOutputCount,
                            curs->config.relatePulseTrigger.outputLevel,
                            curs->config.relatePulseTrigger.pulseClock,
                            curs->config.relatePulseTrigger.pulseEffectiveTime,
                            curs->config.relatePulseTrigger.pulsePeriod,
                            curs->config.relatePulseTrigger.delayTime,
                            curs->config.relatePulseTrigger.outputTime,
                            curs->config.relatePulseTrigger.relationPort);
                    break;
                default:
                    ret = false;
                }
                if (ret) {
                    // Add watch item
                    DIO_DOWatcher_AddWatchItems(self->mDOWatcher, curs);
                }
            }
            curs ++;
        }
    }

    return ret;
}

bool
DIO_DataFetchScheduler_StopDOOutput(DataFetchScheduler* me, int pinId, vector doWatchItems)
{
    DIO_DataFetchScheduler* self = (DIO_DataFetchScheduler*)me;
    bool ret = true;

    if (0 != vector_size(doWatchItems)) {
        DIO_DOWatchItem*    curs = (DIO_DOWatchItem*)vector_get_data(doWatchItems);
            
        for (int i = 0, n = vector_size(doWatchItems); i < n; ++i) {
            if (curs->pinID == pinId) {
                if (! DIO_Lib_StopOutput((uint32_t)pinId)) {
                    ret = false;
                } else {
                    // Delete watch Item
                    DIO_DOWatcher_DelWatchItems(self->mDOWatcher, curs);
                }
            }
            curs ++;
        }
    }

    return ret;
}

DataFetchScheduler*
DIO_DataFetchScheduler_New(void)
{
    // initialize own menber and setup virtual method
    DIO_DataFetchScheduler* newObj =
        (DIO_DataFetchScheduler*)malloc(sizeof(DIO_DataFetchScheduler));
    DataFetchSchedulerBase* super;

    if (NULL == newObj) {
        goto err_malloc;
    }

    super = &newObj->Super;
    if (NULL == DataFetchScheduler_InitOnNew(
        super, DIO_FetchTimerCallback, DIO)) {
        goto err;
    }
    newObj->mFetchTargets = DIO_DIFetchTargets_New();
    if (NULL == newObj->mFetchTargets) {
        goto err_delete_super;
    }
    newObj->mWatcher = DIO_DIWatcher_New();
    if (NULL == newObj->mWatcher) {
        goto err_delete_fetchTargets;
    }
    newObj->mDOWatcher = DIO_DOWatcher_New();
    if (NULL == newObj->mDOWatcher) {
        goto err_delete_watcher;
    }

    super->DoDestroy = DIO_DataFetchScheduler_DoDestroy;
//	super->DoInit    = DIO_DataFetchScheduler_DoInit;  // don't override
    super->ClearFetchTargets = DIO_DataFetchScheduler_ClearFetchTargets;
    super->DoSchedule        = DIO_DataFetchScheduler_DoSchedule;

    return super;
err_delete_watcher:
    DIO_DIWatcher_Destroy(newObj->mWatcher);
err_delete_fetchTargets:
    DIO_DIFetchTargets_Destroy(newObj->mFetchTargets);
err_delete_super:
    DataFetchScheduler_Destroy(super);
err:
    free(newObj);
err_malloc:
    return NULL;
}

void
DIO_DataFetchScheduler_Init(DataFetchScheduler* me,
    vector fetchItemPtrs, vector watchItems)
{
    // reinitialize pulse count acquisition and contact inpput monitoring targes
    DIO_DataFetchScheduler* self = (DIO_DataFetchScheduler*)me;

    DataFetchScheduler_Init(me, fetchItemPtrs);
    DIO_DIWatcher_Init(self->mWatcher, watchItems);
}
