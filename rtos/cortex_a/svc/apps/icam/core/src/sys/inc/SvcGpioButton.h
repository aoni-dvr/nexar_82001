/**
*  @file SvcGpioButton.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*
*
*/

#ifndef SVC_GPIO_BUTTON_H
#define SVC_GPIO_BUTTON_H

#include "AmbaGPIO.h"

#define SVC_LOG_G_BTN          "G_BTN"
#define SVC_G_BTN_STACK_SIZE   (0x8000U)

#define MAX_NUM_GPIO_BUTTON    (10U)
#define MAX_CMD_LENGTH         (20U)

#define SAMPLE_PERIOD          (50) /* ms */
#define DEBOUNCE_COUNT         (2U)
#define PRESS_STAT             (AMBA_GPIO_LEVEL_LOW)
#define RELEASE_STAT           (AMBA_GPIO_LEVEL_HIGH)

typedef struct {
    UINT32   Priority;      /* scanning task priority */
    UINT32   CpuBits;       /* core selection which scanning task running at */
    UINT8    NumButton;     /* number of buttons */
    UINT16   SamplePeriod;  /* in msec */
    UINT8    DeBounceTarget;/* debouncing count target */
    UINT8    RepeatStart;   /* repeating start target, 0 for no repeating */
    UINT8    RepeatInterval;/* repeating rate count */
    UINT8    *pPinID;       /* pointer to GPIO pin id array with size NumButton */
    UINT32   (*Action)(const char *pCmd);   /* action callback */
    UINT8    CmdLength;     /* cmd length for Action callback */
    char     *pCmd;         /* pointer to cmd with size (NumButton x CmdLength) */
    UINT8    Debug;         /* debugging on */
} GPIO_BUTTON_CONFIG_s;

UINT32 SvcGpioButton_Init(const GPIO_BUTTON_CONFIG_s *pCfg);
void SvcGpioButton_Debug(UINT8 Debug);

#endif  /* SVC_GPIO_BUTTON_H */
