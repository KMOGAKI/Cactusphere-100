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

#ifndef _DIO_CONFIG_MGR_H_
#define _DIO_CONFIG_MGR_H_

#include "DIO_DIFetchConfig.h"
#include "DIO_DIWatchConfig.h"
#include "DIO_DOWatchConfig.h"
#include "cactusphere_error.h"

#ifndef NUM_DI
#define NUM_DI 2
#endif

#ifndef NUM_DO
#define NUM_DO 2
#endif

#ifndef NUM_DIO
#define NUM_DIO (NUM_DI + NUM_DO)
#endif

#ifndef _STDIDONT_H
#include <stdint.h>
#endif

// Initializaition and cleanup
extern void	DIO_ConfigMgr_Initialize(void);
extern void	DIO_ConfigMgr_Cleanup(void);

// Apply new configuration
extern SphereWarning	DIO_ConfigMgr_LoadAndApplyIfChanged(
    const unsigned char* payload, unsigned int payloadSize, vector item);

// Receive command
extern bool DIO_ConfigMgr_RecvStartCommand(int pinId);
extern bool DIO_ConfigMgr_RecvStopCommand(int pinId);

// Get configuratioin
extern DIO_DIFetchConfig*  DIO_ConfigMgr_GetFetchConfig(void);
extern DIO_DIWatchConfig*  DIO_ConfigMgr_GetWatchConfig(void);
extern DIO_DOWatchConfig*  DIO_ConfigMgr_GetDOWatchConfig(void);

#endif  // _DIO_CONFIG_MGR_H_
