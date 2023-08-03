/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "diag.h"
#include "AmbaGPIO.h"

static void IoDiag_GpioCmdUsage(char * const *pArgVector)
{
    printf("Usage: ");
    printf(pArgVector[0]);
    printf(" [all|<pin_id>] [option]\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" all                         : show all the GPIO pin status\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" <pin>                       : show the specified GPIO pin status\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" <pin> in                    : set the specified GPIO pin as GPI function\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" <pin> out [0|1]             : set the specified GPIO pin as GPO functionet with logic low/high\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" <pin> alt <func>            : set the specified GPIO pin as alternate function\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" <pin> ds [0|1|2|3]          : drive strength control of the specified GPIO pin\n");
    printf("       ");
    printf(pArgVector[0]);
    printf(" <pin> pull [up|down|none]   : internal resistor control of the specified GPIO pin\n");
    printf(pArgVector[0]);
    printf(" test <out_pin> <in_pin1> <in_pin2> [0|1]  : gpio loopback test\n");
}

static void IoDiag_GpioLoopBackTest(char * const *pArgVector)
{
    /* For GPO test, the GPO should connect to one or two GPIs for checking.            */
    /* For GPI test, connect another redundant GPI to the same source GPO for checking. */
    /* Diagram:                          */
    /*           GPO ---> GPI1           */
    /*               \--> GPI2           */

    UINT32 GPO, GPI1, GPI2, Val, Level;
    AMBA_GPIO_INFO_s PinInfo;
    char **ptr = NULL;

    GPO = strtoul(pArgVector[0], ptr, 0);
    GPI1 = strtoul(pArgVector[1], ptr, 0);
    GPI2 = strtoul(pArgVector[2], ptr, 0);
    Val = strtoul(pArgVector[3], ptr, 0);

    if (Val == 0U) {
        Level = AMBA_GPIO_LEVEL_LOW;
        (void)AmbaGPIO_SetFuncGPO(GPO, Level);
        printf("[GPIO]Test case: level low\n");
    } else {
        Level = AMBA_GPIO_LEVEL_HIGH;
        (void)AmbaGPIO_SetFuncGPO(GPO, Level);
        printf("[GPIO]Test case: level high\n");
    }

    (void)AmbaGPIO_SetFuncGPI(GPI1);
    (void)AmbaGPIO_SetFuncGPI(GPI2);

    (void)AmbaGPIO_GetInfo(GPI1, &PinInfo);
    if (PinInfo.PinState != Level) {
        printf("[GPIO]Test fail, check input1 fail\n");
    } else {
        (void)AmbaGPIO_GetInfo(GPI2, &PinInfo);
        if (PinInfo.PinState != Level) {
            printf("[GPIO]Check input1 pass, check input2 fail\n");
        } else {
            printf("[GPIO]Test pass\n");
        }
    }
}

static void IoDiag_GpioShowPinInfo(UINT32 GpioID, const AMBA_GPIO_INFO_s *pPinInfo)
{
    const char *GpioFuncStr[AMBA_NUM_GPIO_FUNC] = {
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

    printf(" [%d]:\t", GpioID);
    if (pPinInfo->PinFunc < (AMBA_GPIO_FUNC_ALT7 + 1)) {
        printf(GpioFuncStr[pPinInfo->PinFunc]);
    } else {
        printf("errA");
    }
    printf("   ");
    printf((pPinInfo->PinState == AMBA_GPIO_LEVEL_HIGH) ? "HIGH" : "LOW ");
    printf("   ");
    printf((pPinInfo->PinPullUpOrDown == AMBA_GPIO_PULL_DOWN) ? "PULL-DOWN" : ((pPinInfo->PinPullUpOrDown == AMBA_GPIO_PULL_UP) ? "PULL-UP  " : "NO-PULL  "));
    printf("  ");
    printf((pPinInfo->PinDriveStrength == AMBA_GPIO_DRIVE_STR_WEAKEST) ? "2mA " : ((pPinInfo->PinDriveStrength == AMBA_GPIO_DRIVE_STR_WEAK) ? "4mA " : ((pPinInfo->PinDriveStrength == AMBA_GPIO_DRIVE_STR_STRONG) ? "8mA " : "12mA")));
    printf("    \n");
}

static void IoDiag_GpioShowInfo(UINT32 GpioID)
{
    AMBA_GPIO_INFO_s PinInfo = {0};
    UINT32 i;

    printf(" GPIO\tFUNC   LEVEL  RESISTOR   DRIVE\n"
           "---------------------------------------------------------------\n");

    if (GpioID >= AMBA_NUM_GPIO_PIN) {
        for (i = 0; i < AMBA_NUM_GPIO_PIN; i++) {
            if (AmbaGPIO_GetInfo(i, &PinInfo) == OK) {
                IoDiag_GpioShowPinInfo(i, &PinInfo);
            }
        }
    } else {
        if (AmbaGPIO_GetInfo(GpioID, &PinInfo) == OK) {
            IoDiag_GpioShowPinInfo(GpioID, &PinInfo);
        }
    }

    printf("-------------------------------------------------------------\n");
}

int DoGpioDiag(int argc, char *argv[])
{
    char **ptr = NULL;
    unsigned int PinID = 0;
    unsigned int Func, Ds;

    if (argc >= 3) {
        if (strcmp("test", argv[2]) == 0) {
            if (argc == 7U ) {
                IoDiag_GpioLoopBackTest(&argv[3]);
            } else {
                IoDiag_GpioCmdUsage(argv);
            }
        } else if (strcmp("all", argv[2]) == 0) {
            IoDiag_GpioShowInfo(AMBA_NUM_GPIO_PIN);
        } else if (strcmp("show", argv[2]) == 0) {
            if (argc >= 4) {
                PinID = strtoul(argv[3], ptr, 0);
                if (PinID < AMBA_NUM_GPIO_PIN) {
                    IoDiag_GpioShowInfo(PinID);
                }
            } else {
                fprintf(stderr, "%s %d, argv is not correct\n", __FUNCTION__, __LINE__);
            }
        } else if (strcmp("alt", argv[3]) == 0) {
            if (argc >= 5) {
                PinID = strtoul(argv[2], ptr, 0);
                Func = strtoul(argv[4], ptr, 0);
                AmbaGPIO_SetFuncAlt(((Func << 12U) | PinID));
                printf("setup pin %d to function %d\n", PinID, Func);
            } else {
                fprintf(stderr, "%s %d, argv is not correct\n", __FUNCTION__, __LINE__);
            }
        } else if (strcmp("out", argv[3]) == 0) {
            if (argc >= 5) {
                PinID = strtoul(argv[2], ptr, 0);
                Func = strtoul(argv[4], ptr, 0);
                if (Func == 0) {
                    (void)AmbaGPIO_SetFuncGPO(PinID, AMBA_GPIO_LEVEL_LOW);
                    printf("setup pin %d to low\n", PinID);

                } else {
                    (void)AmbaGPIO_SetFuncGPO(PinID, AMBA_GPIO_LEVEL_HIGH);
                    printf("setup pin %d to high\n", PinID);

                }
            } else {
                fprintf(stderr, "%s %d, argv is not correct\n", __FUNCTION__, __LINE__);
            }
        } else if (strcmp("pull", argv[3]) == 0) {
            if (argc >= 5) {
                PinID = strtoul(argv[2], ptr, 0);
                if (strcmp("up", argv[4]) == 0) {
                    (void)AmbaGPIO_SetPullUpOrDown(PinID, AMBA_GPIO_PULL_UP);
                    printf("setup pin %d to pull up\n", PinID);
                } else if (strcmp("down", argv[4]) == 0) {
                    (void)AmbaGPIO_SetPullUpOrDown(PinID, AMBA_GPIO_PULL_DOWN);
                    printf("setup pin %d to pull down\n", PinID);

                } else {
                    (void)AmbaGPIO_SetPullUpOrDown(PinID, AMBA_GPIO_PULL_DISABLE);
                    printf("setup pin %d to pull none\n", PinID);
                }
            } else {
                fprintf(stderr, "%s %d, argv is not correct\n", __FUNCTION__, __LINE__);
            }
        } else if (strcmp("ds", argv[3]) == 0) {
            if (argc >= 5) {
                PinID = strtoul(argv[2], ptr, 0);
                Ds = strtoul(argv[4], ptr, 0);

                if (Ds <= GPIO_DRV_STR_L3 ) {
                    (void)AmbaGPIO_SetDriveStrength(PinID, Ds);
                    printf("setup pin %d drive strength to %d\n", PinID, Ds);
                } else {
                    printf("incorrect drive strength value %d\n");
                }
            } else {
                fprintf(stderr, "%s %d, argv is not correct\n", __FUNCTION__, __LINE__);
            }
        } else if (strcmp("in", argv[3]) == 0) {
            if (argc >= 4) {
                PinID = strtoul(argv[2], ptr, 0);
                (void)AmbaGPIO_SetFuncGPI(PinID);
                printf("setup pin %d as input\n", PinID);
            } else {
                fprintf(stderr, "%s %d, argv is not correct\n", __FUNCTION__, __LINE__);
            }
        } else {
            IoDiag_GpioCmdUsage(argv);
        }
    } else {
        IoDiag_GpioCmdUsage(argv);
    }
    return 0;
}

