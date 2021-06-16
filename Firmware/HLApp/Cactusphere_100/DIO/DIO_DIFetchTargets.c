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

#include "DIO_DIFetchTargets.h"

#include "DIO_DIFetchItem.h"

// DIO_FetchTargets data members
struct DIO_DIFetchTargets {
    vector	mTargets;
};

// Initialization and cleanup
DIO_DIFetchTargets*
DIO_DIFetchTargets_New(void)
{
    DIO_DIFetchTargets*	newObj =
        (DIO_DIFetchTargets*)malloc(sizeof(DIO_DIFetchTargets));

    if (NULL != newObj) {
        newObj->mTargets = vector_init(sizeof(DIO_DIFetchItem*));
        if (NULL == newObj->mTargets) {
            free(newObj);
            return NULL;
        }
    }

    return newObj;
}

void
DIO_DIFetchTargets_Destroy(DIO_DIFetchTargets* me)
{
    DIO_DIFetchTargets_Clear(me);
    vector_destroy(me->mTargets);
    free(me);
}

// Get current acquisition targets
vector
DIO_DIFetchTargets_GetFetchItems(DIO_DIFetchTargets* me)
{
    return me->mTargets;
}

// Manage acquisition targets
void
DIO_DIFetchTargets_Add(DIO_DIFetchTargets* me, const DIO_DIFetchItem* target)
{
    vector_add_last(me->mTargets, (void*)&target);
}

void
DIO_DIFetchTargets_Clear(DIO_DIFetchTargets* me)
{
    vector_clear(me->mTargets);
}
