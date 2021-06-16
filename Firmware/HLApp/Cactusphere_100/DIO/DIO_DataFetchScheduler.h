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

#ifndef _DIO_DATA_FETCH_SCHEDULER_H_
#define _DIO_DATA_FETCH_SCHEDULER_H_

#ifndef _DATA_FETCH_SCHEDULER_H_
#include <DataFetchScheduler.h>
#endif

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif

#include "DIO_PropertyItem.h"

extern DataFetchScheduler* DIO_DataFetchScheduler_New(void);
extern void	DIO_DataFetchScheduler_Init(DataFetchScheduler* me,
    vector fetchItemPtrs, vector watchItems);
extern bool DIO_DataFetchScheduler_StartDOOutput(DataFetchScheduler* me,
    int pinId, vector doWatchItems);
extern bool DIO_DataFetchScheduler_StopDOOutput(DataFetchScheduler* me,
    int pinId, vector doWatchItems);

#endif  // _DIO_DATA_FETCH_SCHEDULER_H_
