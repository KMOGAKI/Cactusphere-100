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

#include "DIO_DOWatcher.h"

#include "DIO_DOWatchItem.h"
#include "LibDIO.h"

// DIO_DOWatcher data members
struct DIO_DOWatcher {
    vector	mBody;         // vector of DIO_DIWatchItem
};

// Initialization and cleanup
DIO_DOWatcher*
DIO_DOWatcher_New(void)
{
    DIO_DOWatcher*	newObj = (DIO_DOWatcher*)malloc(sizeof(DIO_DOWatcher));

    if (NULL != newObj) {
        newObj->mBody = vector_init(sizeof(DIO_DOWatchItem));
        if (NULL == newObj->mBody) {
            free(newObj);
            newObj = NULL;
        }
    }

    return newObj;
}

void
DIO_DOWatcher_Init(DIO_DOWatcher* me, vector watchItems)
{
    // do nothing
}

void
DIO_DOWatcher_Destroy(DIO_DOWatcher* me)
{
    vector_destroy(me->mBody);
    free(me);
}

bool
DIO_DOWatcher_AddWatchItems(DIO_DOWatcher* me, DIO_DOWatchItem* watchItem)
{
    if (0 != vector_size(me->mBody)) {
        DIO_DOWatchItem*    curs = (DIO_DOWatchItem*)vector_get_data(me->mBody);
            
        for (int i = 0, n = vector_size(me->mBody); i < n; ++i) {
            if (curs->pinID == watchItem->pinID) {
                vector_remove_at(me->mBody, i);
                break;
            }
            curs++;
        }
    }
    vector_add_last(me->mBody, watchItem);

    return true;
}

bool
DIO_DOWatcher_DelWatchItems(DIO_DOWatcher* me, DIO_DOWatchItem* watchItem)
{
    if (0 != vector_size(me->mBody)) {
        DIO_DOWatchItem*    curs = (DIO_DOWatchItem*)vector_get_data(me->mBody);
        
        for (int i = 0, n = vector_size(me->mBody); i < n; ++i) {
            if (curs->pinID == watchItem->pinID) {
                vector_remove_at(me->mBody, i);
                break;
            }
            curs++;
        }
    }
    return true;
}

vector
DIO_DOWatcher_GetWatchItems(DIO_DOWatcher* me)
{
    return me->mBody;
}
