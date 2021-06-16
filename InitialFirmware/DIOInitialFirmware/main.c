/*
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Copyright (c) 2020 Atmark Techno, Inc.
 * 
 * MIT License
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

// Cactusphere Initial firmware

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

// target hardware.
#include <hw/mt3620.h>

#include "at24c0.h"
#include "cactusphere_eeprom.h"

/// <summary>
/// Exit codes for this application. These are used for the
/// application exit code. They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success     = 0,
    ExitCode_Main_Led    = 1,
    ExitCode_Main_Di     = 2,
    ExitCode_Main_Do     = 3,
} ExitCode;

// EEPROM Data
typedef struct {
    const char name[20];
    char value[20];
} EepromPropertyData;

eepromData eeprom;

// LED
int ledFd = -1;

// DIO
#define NUM_DIO 4

typedef struct DIOPort {
    int fd;
    int gpio;
    bool isInput;
    GPIO_Value_Type state;
} DIOPort;

#define DIOPort_Initializer(GPIO, IsInput)   \
    .fd = -1,               \
    .gpio = GPIO,           \
    .isInput = IsInput,     \
    .state = GPIO_Value_High

static DIOPort port[NUM_DIO] = {
    [0] = {
        DIOPort_Initializer(MT3620_GPIO12, true),
    },
    [1] = {
        DIOPort_Initializer(MT3620_GPIO15, true),
    },
    [2] = {
        DIOPort_Initializer(MT3620_GPIO0, false),
    },
    [3] = {
        DIOPort_Initializer(MT3620_GPIO8, false),
    },
};

// Termination state
static volatile sig_atomic_t exitCode = ExitCode_Success;

static ExitCode InitPeripherals(void);
static void convertEepromData(void);

/// <summary>
///     Set up initialize peripherals.
/// </summary>
/// <returns>ExitCode_Success if all resources were allocated successfully; otherwise another
/// ExitCode value which indicates the specific failure.</returns>
static ExitCode InitPeripherals(void)
{
    ledFd = GPIO_OpenAsOutput(MT3620_GPIO23, GPIO_OutputMode_PushPull, GPIO_Value_High);

    if (ledFd == -1) {
        Log_Debug(
            "Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
            strerror(errno), errno);
        return ExitCode_Main_Led;
    }

    // DIO Initialize
    for (int i = 0; i < NUM_DIO; i++) {
        if (port[i].isInput) { // input
            port[i].fd = GPIO_OpenAsInput(port[i].gpio);
            if (port[i].fd < 0) {
                Log_Debug("ERROR: Could not open DI ch%d: %s (%d).\n", i + 1, strerror(errno), errno);
                return ExitCode_Main_Di;
            }
        } else { // output
            port[i].fd = GPIO_OpenAsOutput(port[i].gpio, GPIO_OutputMode_PushPull, port[i].state);
            if (port[i].fd < 0) {
                Log_Debug("ERROR: Could not open DO ch%d: %s (%d).\n", i + 1, strerror(errno), errno);
                return ExitCode_Main_Do;
            }
        }
    }

    return ExitCode_Success;
}

/// <summary>
///     Convert and Print EEPROM Data.
/// </summary>
static void convertEepromData(void)
{
    static EepromPropertyData getEepromData[] = {
        {"SerialNumber", ""},
        {"LanMacAddr", ""},
        {"ProductId", ""},
        {"VendorId", ""},
        {"WifiMacAddr", ""},
        {"Generation", ""},
    };

    setEepromString(getEepromData[0].value, eeprom.serial, sizeof(eeprom.serial));
    setEepromString(getEepromData[1].value, eeprom.ethernetMac, sizeof(eeprom.ethernetMac));
    setEepromString(getEepromData[2].value, eeprom.productId, sizeof(eeprom.productId));
    setEepromString(getEepromData[3].value, eeprom.venderId, sizeof(eeprom.venderId));
    setEepromString(getEepromData[4].value, eeprom.wlanMac, sizeof(eeprom.wlanMac));
    setEepromString(getEepromData[5].value, eeprom.generation, sizeof(eeprom.generation));
        
    for (int i = 0; i < (sizeof(getEepromData) / sizeof(getEepromData[0])); i++) {
        Log_Debug("%s : %s\n", getEepromData[i].name, getEepromData[i].value);
    }
}

/// <summary>
///     Blink LED (Error).
/// </summary>
static void BlinkLedErrorMode(void)
{
    const struct timespec errorTime = {.tv_sec = 0, .tv_nsec = 500 * 1000 * 1000};

    while (true) {
        GPIO_SetValue(ledFd, GPIO_Value_Low);
        nanosleep(&errorTime, NULL);
        GPIO_SetValue(ledFd, GPIO_Value_High);
        nanosleep(&errorTime, NULL);
    }
}

/// <summary>
///     Blink LED (Error).
/// </summary>
static void BlinkLed(void)
{
    const struct timespec sleepTime = {.tv_sec = 1, .tv_nsec = 0};
    int sum = 0;

    while (true) {
        sum = 0;

        for (int i = 0; i < NUM_DIO; i++) {
            if (!port[i].isInput) {
                port[i].state = !port[i].state; // High <-> Low
                int result = GPIO_SetValue(port[i].fd, port[i].state);
                if (result != 0) {
                    Log_Debug("ERROR: Could not set GPIO: %s (%d).\n", strerror(errno), errno);
                }
                sum += (int)port[i].state; // output
            }
        }

        for (int i = 0; i < NUM_DIO; i++) {
            if (port[i].isInput) {
                int result = GPIO_GetValue(port[i].fd, &port[i].state);
                if (result != 0) {
                    Log_Debug("ERROR: Could not read GPIO: %s (%d).\n", strerror(errno), errno);
                }
            }
        }

        Log_Debug("DI1:%d, DI2:%d, DO1:%d, DO2:%d\n",
            port[0].state, port[1].state, port[2].state, port[3].state);

        if (sum > 0) {
            GPIO_SetValue(ledFd, GPIO_Value_High); // Output High / LED OFF
        } else {
            GPIO_SetValue(ledFd, GPIO_Value_Low); // Output Low / LED ON
        }
        nanosleep(&sleepTime, NULL);
    }
}

/// <summary>
///     Main entry point.
/// </summary>
int main(void)
{
    Log_Debug("Starting Cactusphere DIModel Initial Firmware %s\n", APP_VERSION);

    exitCode = InitPeripherals();

    int retEeprom = GetEepromProperty(&eeprom);
    if (retEeprom < 0) {
        Log_Debug("ERROR: Get EEPROM Data.\n");
    }
    else {
        convertEepromData();
    }
    
    if (retEeprom < 0 || exitCode != ExitCode_Success) {
        // ERROR
        BlinkLedErrorMode();
    } else {
        BlinkLed();
    }

}