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

#ifndef _DIO_DOWATCH_CONFIG_H_
#define _DIO_DOWATCH_CONFIG_H_

#ifndef _STDBOOL_H
#include <stdbool.h>
#endif

#ifndef CONTAINERS_VECTOR_H
#include <vector.h>
#endif

#include "DIO_PropertyItem.h"

typedef struct DIO_DOWatchConfig	DIO_DOWatchConfig;
typedef struct _json_value	json_value;

#ifndef NUM_DI
#define NUM_DI 2
#endif

#ifndef NUM_DO
#define NUM_DO 2
#endif

#ifndef NUM_DIO
#define NUM_DIO (NUM_DI + NUM_DO)
#endif

// Initialization and cleanup
extern DIO_DOWatchConfig*	DIO_DOWatchConfig_New(void);
extern void	DIO_DOWatchConfig_Destroy(DIO_DOWatchConfig* me);

// Add to list
extern bool DIO_DOWatchConfig_AddConfig(DIO_DOWatchConfig* me, int pinId);

// Delete from list
extern bool DIO_DOWatchConfig_DelConfig(DIO_DOWatchConfig* me, int pinId);

extern vector DIO_DOWatchConfig_GetFetchItems(DIO_DOWatchConfig* me);

#endif  // _DIO_DOWATCH_CONFIG_H_
