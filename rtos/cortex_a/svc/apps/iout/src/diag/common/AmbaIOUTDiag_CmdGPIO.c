/**
 *  @file AmbaIOUTDiag_CmdGPIO.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details GPIO diagnostic command
 *
 */

#include <AmbaTypes.h>
#include <AmbaMisraFix.h>

#include <AmbaShell.h>
#include <AmbaUtility.h>

#include "AmbaIOUTDiag.h"
#include <AmbaGPIO.h>

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_GpioShowPinInfo(UINT32 GpioID, const AMBA_GPIO_INFO_s *pPinInfo, AMBA_SHELL_PRINT_f PrintFunc)
{
    static const char *GpioFuncStr[AMBA_NUM_GPIO_FUNC] = {
        [AMBA_GPIO_FUNC_GPI]  = "IN  ",
        [AMBA_GPIO_FUNC_GPO]  = "OUT ",
        [AMBA_GPIO_FUNC_ALT1] = "ALT1",
        [AMBA_GPIO_FUNC_ALT2] = "ALT2",
        [AMBA_GPIO_FUNC_ALT3] = "ALT3",
        [AMBA_GPIO_FUNC_ALT4] = "ALT4",
        [AMBA_GPIO_FUNC_ALT5] = "ALT5",
        [AMBA_GPIO_FUNC_ALT6] = "ALT6",
        [AMBA_GPIO_FUNC_ALT7] = "ALT7",
    };
    UINT32 FuncAddr;

    IoDiag_PrintFormattedInt(" [%d]:\t", GpioID, PrintFunc);
    PrintFunc(GpioFuncStr[pPinInfo->PinFunc]);
    PrintFunc("   ");
    PrintFunc((pPinInfo->PinState == AMBA_GPIO_LEVEL_HIGH) ? "HIGH" : "LOW ");
    PrintFunc("   ");
    PrintFunc((pPinInfo->PinPullUpOrDown == AMBA_GPIO_PULL_DOWN) ? "PULL-DOWN" : ((pPinInfo->PinPullUpOrDown == AMBA_GPIO_PULL_UP) ? "PULL-UP  " : "NO-PULL  "));
    PrintFunc("  ");
    PrintFunc((pPinInfo->PinDriveStrength == AMBA_GPIO_DRIVE_STR_WEAKEST) ? "2mA " : ((pPinInfo->PinDriveStrength == AMBA_GPIO_DRIVE_STR_WEAK) ? "4mA " : ((pPinInfo->PinDriveStrength == AMBA_GPIO_DRIVE_STR_STRONG) ? "8mA " : "12mA")));
    PrintFunc("    ");
    PrintFunc((pPinInfo->IntType == GPIO_INT_NO_INTERRUPT) ? "Disabled" : "Enabled ");
    AmbaMisra_TypeCast32(&FuncAddr, &pPinInfo->IntFunc);
    IoDiag_PrintFormattedInt("   0x%08x\n", FuncAddr, PrintFunc);
}

static void IoDiag_GpioShowInfo(UINT32 GpioID, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_GPIO_INFO_s PinInfo;
    UINT32 i;

    PrintFunc(" GPIO\tFUNC   LEVEL  RESISTOR   DRIVE   INTERRUPT  ISR_ADDR\n"
              "---------------------------------------------------------------\n");

    if (GpioID >= AMBA_NUM_GPIO_PIN) {
        for (i = 0; i < AMBA_NUM_GPIO_PIN; i++) {
            if (AmbaGPIO_GetInfo(i, &PinInfo) == OK) {
                IoDiag_GpioShowPinInfo(i, &PinInfo, PrintFunc);
            }
        }
    } else {
        if (AmbaGPIO_GetInfo(GpioID, &PinInfo) == OK) {
            IoDiag_GpioShowPinInfo(GpioID, &PinInfo, PrintFunc);
        }
    }

    PrintFunc(" -------------------------------------------------------------\n");
}

static void IoDiag_GpioLoopBackTest(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    /* For GPO test, the GPO should connect to one or two GPIs for checking.            */
    /* For GPI test, connect another redundant GPI to the same source GPO for checking. */
    /* Diagram:                         */
    /*           GPO ---> GPI1           */
    /*               \--> GPI2           */

    UINT32 GPO, GPI1, GPI2, Val, Level;
    AMBA_GPIO_INFO_s PinInfo;

    (void)AmbaUtility_StringToUInt32(pArgVector[0], &GPO);
    (void)AmbaUtility_StringToUInt32(pArgVector[1], &GPI1);
    (void)AmbaUtility_StringToUInt32(pArgVector[2], &GPI2);
    (void)AmbaUtility_StringToUInt32(pArgVector[3], &Val);

    if (Val == 0U) {
        Level = AMBA_GPIO_LEVEL_LOW;
        (void)AmbaGPIO_SetFuncGPO(GPO, Level);
        PrintFunc("[GPIO]Test case: level low\n");
    } else {
        Level = AMBA_GPIO_LEVEL_HIGH;
        (void)AmbaGPIO_SetFuncGPO(GPO, Level);
        PrintFunc("[GPIO]Test case: level high\n");
    }

    (void)AmbaGPIO_SetFuncGPI(GPI1);
    (void)AmbaGPIO_SetFuncGPI(GPI2);

    (void)AmbaGPIO_GetInfo(GPI1, &PinInfo);
    if (PinInfo.PinState != Level) {
        PrintFunc("[GPIO]Test fail, check input1 fail\n");
    } else {
        (void)AmbaGPIO_GetInfo(GPI2, &PinInfo);
        if (PinInfo.PinState != Level) {
            PrintFunc("[GPIO]Check input1 pass, check input2 fail\n");
        } else {
            PrintFunc("[GPIO]Test pass\n");
        }
    }
}

static void IoDiag_GpioCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [all|<pin_id>] [option]\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" all                         : show all the GPIO pin status\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <pin>                       : show the specified GPIO pin status\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <pin> in                    : set the specified GPIO pin as GPI function\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <pin> out [0|1]             : set the specified GPIO pin as GPO functionet with logic low/high\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <pin> alt <func>            : set the specified GPIO pin as alternate function\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <pin> ds [0|1|2|3]          : drive strength control of the specified GPIO pin\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <pin> pull [up|down|none]   : internal resistor control of the specified GPIO pin\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" test <out_pin> <in_pin1> <in_pin2> [0|1]  : gpio loopback test\n");
}

/**
 *  GPIO diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return none
 */
void AmbaIOUTDiag_CmdGPIO(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 GpioID, Val;

    if (ArgCount < 2U) {
        IoDiag_GpioCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "all", 3U) == 0) {
            IoDiag_GpioShowInfo(AMBA_NUM_GPIO_PIN, PrintFunc);      /* Dump all pin info */
        } else if (AmbaUtility_StringCompare(pArgVector[1], "test", 4U) == 0) {
            if (ArgCount == 6U ) {
                IoDiag_GpioLoopBackTest(&pArgVector[2], PrintFunc);
            } else {
                IoDiag_GpioCmdUsage(pArgVector, PrintFunc);
            }
        } else {
            (void)AmbaUtility_StringToUInt32(pArgVector[1], &GpioID);
            if (ArgCount >= 3U) {
                if (AmbaUtility_StringCompare(pArgVector[2], "in", 2U) == 0) {
                    (void)AmbaGPIO_SetFuncGPI(GpioID);
                } else {
                    if (ArgCount >= 4U) {
                        (void)AmbaUtility_StringToUInt32(pArgVector[3], &Val);
                        if (AmbaUtility_StringCompare(pArgVector[2], "out", 3U) == 0) {
                            if (Val == 0U) {
                                (void)AmbaGPIO_SetFuncGPO(GpioID, AMBA_GPIO_LEVEL_LOW);
                            } else {
                                (void)AmbaGPIO_SetFuncGPO(GpioID, AMBA_GPIO_LEVEL_HIGH);
                            }
                        } else if (AmbaUtility_StringCompare(pArgVector[2], "alt", 3U) == 0) {
                            (void)AmbaGPIO_SetFuncAlt(GPIO_SET_PIN_FUNC(Val, GpioID));
                        } else if (AmbaUtility_StringCompare(pArgVector[2], "ds", 2U) == 0) {
                            (void)AmbaGPIO_SetDriveStrength(GpioID, Val);
                        } else if (AmbaUtility_StringCompare(pArgVector[2], "pull", 4U) == 0) {
                            if (AmbaUtility_StringCompare(pArgVector[3], "up", 2U) == 0) {
                                (void)AmbaGPIO_SetPullUpOrDown(GpioID, AMBA_GPIO_PULL_UP);
                            } else if (AmbaUtility_StringCompare(pArgVector[3], "down", 4U) == 0) {
                                (void)AmbaGPIO_SetPullUpOrDown(GpioID, AMBA_GPIO_PULL_DOWN);
                            } else if (AmbaUtility_StringCompare(pArgVector[3], "none", 4U) == 0) {
                                (void)AmbaGPIO_SetPullUpOrDown(GpioID, AMBA_GPIO_PULL_DISABLE);
                            } else {
                                /* Do nothing! */
                            }
                        } else {
                            /* Do nothing! */
                        }
                    }
                }
            }

            IoDiag_GpioShowInfo(GpioID, PrintFunc);                 /* Dump single pin info */
        }
    }
}

