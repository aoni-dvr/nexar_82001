/**
 *  @file SvcButtonCtrl.h
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
 *  @details svc button control header
 *
 */

#ifndef SVC_BUTTON_CTRL_H
#define SVC_BUTTON_CTRL_H

typedef UINT32 (* SVC_BUTTON_CTRL_FUNC)(void);

#define SVC_BUTTON_TYPE_GPIO    (0U)
#define SVC_BUTTON_TYPE_ADC     (1U)

typedef struct {
    UINT32   Priority;      /* scanning task priority */
    UINT32   CpuBits;       /* core selection which scanning task running at */
    UINT8    NumButton;     /* number of buttons */
    UINT16   SamplePeriod;  /* in msec */
    UINT8    DeBounceTarget;/* debouncing count target */
    UINT8    RepeatStart;   /* repeating start target, 0 for no repeating */
    UINT8    RepeatInterval;/* repeating rate count */
    UINT8    *pPinID;       /* pointer to GPIO pin id array with size NumButton */
    UINT8    CmdLength;     /* cmd length for Action callback */
    char     *pCmd;         /* pointer to cmd with size (NumButton x CmdLength) */
    UINT8    Debug;         /* debugging on */
} SVC_BUTTON_GPIO_TASK_INFO_s;

typedef struct {
    UINT32   AdcChannel;    /* ADC Channel */
    UINT32   SampleRate;    /* ADC Sample Rate */
    UINT32   Period;        /* Period to check ADC value */
    UINT32   ButtonNum;
    UINT32   *pButtonVal;
    UINT8    CmdLength;
    char     *pCmd;
} SVC_BUTTON_ADC_TASK_INFO_s;

typedef struct {
    const char           *ButtonName;
    SVC_BUTTON_CTRL_FUNC pFunc;
} SVC_BUTTON_CTRL_LIST_s;

typedef struct {
    UINT32                 ButtonType;
    SVC_BUTTON_CTRL_LIST_s *pList;
    UINT32                 ListNum;
} SVC_BUTTON_CTRL_LIST_LINKER_s;

typedef UINT32 SVC_BUTTON_CTRL_ID;

UINT32 SvcButtonCtrl_Init(void);
UINT32 SvcButtonCtrl_TypeGpioInit(const SVC_BUTTON_GPIO_TASK_INFO_s *pInfo);
#if defined(CONFIG_ICAM_ADC_BUTTON_USED)
UINT32 SvcButtonCtrl_TypeAdcInit(const SVC_BUTTON_ADC_TASK_INFO_s *pInfo);
#endif
UINT32 SvcButtonCtrl_Register(UINT32 Type, SVC_BUTTON_CTRL_LIST_s *pList, UINT32 ButtonNum, SVC_BUTTON_CTRL_ID *pCtrlID);
UINT32 SvcButtonCtrl_Request(const SVC_BUTTON_CTRL_ID *pCtrlID);
UINT32 SvcButtonCtrl_Release(const SVC_BUTTON_CTRL_ID *pCtrlID);

#endif /* SVC_BUTTON_CTRL_H */