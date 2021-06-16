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

#include "DIO_DIWatcher.h"

#include "DIO_DIWatchItem.h"
#include "LibDIO.h"

// DIO_DIWatcher data members
struct DIO_DIWatcher {
    vector	mBody;         // vector of DIO_DIWatchItemStat
    vector	mLastChanges;  // pointer vector of changed DIO_DIWatchItemStat
};

// Initialization and cleanup
DIO_DIWatcher*
DIO_DIWatcher_New(void)
{
    DIO_DIWatcher*	newObj = (DIO_DIWatcher*)malloc(sizeof(DIO_DIWatcher));

    if (NULL != newObj) {
        newObj->mBody = vector_init(sizeof(DIO_DIWatchItemStat));
        newObj->mLastChanges = vector_init(sizeof(DIO_DIWatchItemStat*));
        if (NULL == newObj->mBody || NULL == newObj->mLastChanges) {
            if (NULL != newObj->mBody) {
                vector_destroy(newObj->mBody);
            }
            if (NULL != newObj->mLastChanges) {
                vector_destroy(newObj->mLastChanges);
            }
            free(newObj);
            newObj = NULL;
        }
    }

    return newObj;
}

void
DIO_DIWatcher_Init(DIO_DIWatcher* me, vector watchItems)
{
    // clean up old configuration and setting up monitoring with new configuration
    const DIO_DIWatchItem*	curs;

    if (0 != vector_size(me->mBody)) {
        vector_clear(me->mBody);
        vector_clear(me->mLastChanges);
    }

    curs = (const DIO_DIWatchItem*)vector_get_data(watchItems);
    for (int i = 0, n = vector_size(watchItems); i < n; ++i) {
        // configure pulse counter for monitoring contact input
        DIO_DIWatchItemStat	pseudo;

        DIO_Lib_ResetPulseCount(curs->pinID, 0);
        if (! DIO_Lib_ConfigPulseCounter(curs->pinID, curs->notifyChangeForHigh,
                200, 0xFFFFFFFF)) {
            // error !
            continue;  // ignore that target
        }
        pseudo.watchItem      = curs++;
        pseudo.prevPulseCount = pseudo.currPulseCount = 0;
        vector_add_last(me->mBody, &pseudo);
    }
}

void
DIO_DIWatcher_Destroy(DIO_DIWatcher* me)
{
    vector_destroy(me->mBody);
    vector_destroy(me->mLastChanges);
    free(me);
}

// Check update
bool
DIO_DIWatcher_DoWatch(DIO_DIWatcher* me)
{
    // Find state changed contact inputs and store them to the vector.
    // Return whether it has changed.
    DIO_DIWatchItemStat*	curs;

    if (0 != vector_size(me->mLastChanges)) {
        for (int i = 0, n = vector_size(me->mLastChanges); i < n; ++i) {
            DIO_DIWatchItemStat*	changed;

            vector_get_at(&changed, me->mLastChanges, i);
            changed->prevPulseCount = changed->currPulseCount;
        }
        vector_clear(me->mLastChanges);
    }

    curs = (DIO_DIWatchItemStat*)vector_get_data(me->mBody);
    for (int i = 0, n = vector_size(me->mBody); i < n; ++i) {
        // Check status change of contact input from the pulse counter value
        unsigned long	counterVal;

        if (! DIO_Lib_ReadPulseCount(curs->watchItem->pinID, &counterVal)) {
            // error!!
            continue;  // ignore that contact input
        }

        if (curs->prevPulseCount != counterVal) {
            curs->currPulseCount = counterVal;
            vector_add_last(me->mLastChanges, &curs);
        }
        curs++;
    }

    return (0 != vector_size(me->mLastChanges));
}

const vector
DIO_DIWatcher_GetLastChanges(DIO_DIWatcher* me)
{
    return me->mLastChanges;
}
