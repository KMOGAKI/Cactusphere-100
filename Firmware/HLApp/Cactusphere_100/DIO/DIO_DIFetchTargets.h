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

#ifndef _DIO_DIFETCH_TARGETS_H_
#define _DIO_DIFETCH_TARGETS_H_

#ifndef CONTAINERS_VECTOR_H
#include <vector.h>
#endif

typedef struct DIO_DIFetchTargets	DIO_DIFetchTargets;
typedef struct DIO_DIFetchItem	DIO_DIFetchItem;

// Initialization and cleanup
extern DIO_DIFetchTargets*	DIO_DIFetchTargets_New(void);
extern void	DIO_DIFetchTargets_Destroy(DIO_DIFetchTargets* me);

// Get current acquisition targets
extern vector	DIO_DIFetchTargets_GetFetchItems(DIO_DIFetchTargets* me);

// Manage acquisition targets
extern void	DIO_DIFetchTargets_Add(
    DIO_DIFetchTargets* me, const DIO_DIFetchItem* target);
extern void	DIO_DIFetchTargets_Clear(DIO_DIFetchTargets* me);

#endif  // _DIO_DIFETCH_TARGETS_H_
