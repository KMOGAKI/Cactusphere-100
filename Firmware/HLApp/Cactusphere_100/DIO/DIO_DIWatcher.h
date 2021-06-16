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

#ifndef _DIO_DIWATCHER_H_
#define _DIO_DIWATCHER_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif

#ifndef CONTAINERS_VECTOR_H
#include <vector.h>
#endif

typedef struct DIO_DIWatchItem	DIO_DIWatchItem;
typedef struct DIO_DIWatcher	DIO_DIWatcher;

// status of contact input monitoring target
typedef struct DIO_DIWatchItemStat {
    const DIO_DIWatchItem*	watchItem;  // watching specification
    unsigned long	prevPulseCount; // previous counter value
    unsigned long	currPulseCount; // last counter value
} DIO_DIWatchItemStat;

// Initialization and cleanup
extern DIO_DIWatcher*	DIO_DIWatcher_New(void);
extern void	DIO_DIWatcher_Init(DIO_DIWatcher* me, vector watchItems);
extern void	DIO_DIWatcher_Destroy(DIO_DIWatcher* me);

// Check update
extern bool	DIO_DIWatcher_DoWatch(DIO_DIWatcher* me);
extern const vector	DIO_DIWatcher_GetLastChanges(DIO_DIWatcher* me);

#endif  // _DIO_DIWATCHER_H_
