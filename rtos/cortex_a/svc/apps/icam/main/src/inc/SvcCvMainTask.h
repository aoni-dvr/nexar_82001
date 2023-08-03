/**
*  @file SvcCvMainTask.h
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
*  @details svc cv main task
*
*/

#ifndef SVC_CV_MAIN_TASK_H
#define SVC_CV_MAIN_TASK_H

typedef struct {
    UINT32          Source:          2;
#define SVC_CV_MAIN_BOOT_FLAG_SD             (0U)
#define SVC_CV_MAIN_BOOT_FLAG_ROMFS          (1U)
#define SVC_CV_MAIN_BOOT_FLAG_CMDMODE        (2U)

    UINT32          Framework:       2;
#define SVC_CV_MAIN_BOOT_FLAG_FLEXIDAG       (1U)

        UINT32      Flag:            8;
#define SVC_CV_MAIN_BOOT_FLAG_AUTO_CONTINUE  (1U)

    UINT32          Reserved:        20;
} SVC_CV_MAIN_BOOT_FLAG_s;

typedef struct {
    SVC_CV_MAIN_BOOT_FLAG_s BootFlag;
} SVC_CV_MAIN_STATUS_s;

typedef UINT32 (*SVC_CV_MAIN_TASK_CALLBACK_f)(const SVC_CV_MAIN_STATUS_s *pStatus);

#define SVC_CV_MAIN_TASK_FUNC_TYPES          (1U)

/* Command */
#define SVC_CV_MAIN_TASK_ENABLE_DEFAULT_TBL     ("def_tbl_on")      /* Enable default CV table file. Mainly for CV BIST. */
#define SVC_CV_MAIN_TASK_SET_SCHDR_BIST_CFG     ("schdr_bist_cfg")  /* Configure scheduler BIST related settings. */
#define SVC_CV_MAIN_TASK_SET_SCHDR_RESET_FLAG   ("schdr_reset")     /* Reset scheduler at SvcCvMainTask_Stop */

typedef struct {
    UINT32 AutoRunInterval;                  /* Auto run interval */
    UINT8  DisableFex;                       /* Disable FEX BIST */
} SVC_CV_MAIN_BIST_CFG_s;

UINT32 SvcCvMainTask_Init(void);
UINT32 SvcCvMainTask_Start(void);
UINT32 SvcCvMainTask_Load(void);
UINT32 SvcCvMainTask_Stop(void);
UINT32 SvcCvMainTask_Ctrl(const char *pCmd, void *pParam);
UINT32 SvcCvMainTask_GetBootFlag(SVC_CV_MAIN_BOOT_FLAG_s *pFlag);

UINT32 SvcCvMainTask_RegisterFunc(UINT32 FuncType, SVC_CV_MAIN_TASK_CALLBACK_f pFunc);
UINT32 SvcCvMainTask_UnRegisterFunc(UINT32 FuncType, SVC_CV_MAIN_TASK_CALLBACK_f pFunc);

UINT32 SvcCvMainTask_WaitSrcReady(void);

void SvcCvMainTask_EnableVpuBist(UINT32 AutoRunInterval, UINT8 DisableFex);

#endif  /* SVC_CV_MAIN_TASK_H */
