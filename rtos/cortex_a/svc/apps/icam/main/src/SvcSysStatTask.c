/**
*  @file SvcSysStatTask.c
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
*  @details task for enable svc system status
*
*/

#include "AmbaTypes.h"

/* svc-framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcMem.h"

/* app-shared */
#include "SvcSysStat.h"
#include "SvcBuffer.h"

/* app-icam */
#include "SvcAppStat.h"
#include "SvcBufMap.h"

#include "SvcSysStatTask.h"

/**
 * Initialize svc status flags (with hooked status)
 * return 0-OK, 1-NG
 */
UINT32 SvcSysStatTask_Init(void)
{
    static SVC_APP_STAT_UCODE_s       SvcAppStatusUcode      = {0};
    static SVC_APP_STAT_IQ_s          SvcAppStatusIq         = {0};
    static SVC_APP_STAT_VIN_s         SvcAppStatusVin        = {0};
    static SVC_APP_STAT_VOUT_s        SvcAppStatusVout       = {0};
    static SVC_APP_STAT_STG_s         SvcAppStatusSd         = {0};
    static SVC_APP_STAT_LINUX_s       SvcAppStatusLinux      = {0};
    static SVC_APP_STAT_CV_BOOT_s     SvcAppStatusCvBoot     = {0};
    static SVC_APP_STAT_CAMCTRL_s     SvcAppStatusCamCtrl    = {0};
    static SVC_APP_STAT_DSP_BOOT_s    SvcAppStatusDspBoot    = {0};
    static SVC_APP_STAT_MENU_s        SvcAppStatusMenu       = {0};
    static SVC_APP_STAT_BSD_s         SvcAppStatusBsd        = {0};
    static SVC_APP_STAT_OD_s          SvcAppStatusOd         = {0};
    static SVC_APP_STAT_DIRT_DETECT_s SvcAppStatusDirtDetect = {0};
    static SVC_APP_STAT_DEFOG_s       SvcAppStatusDefog      = {0};
    static SVC_APP_STAT_EMR_VER_s     SvcAppStatusEmrVer     = {0};
    static SVC_APP_STAT_PSD_s         SvcAppStatusPsd        = {0};
    static SVC_APP_STAT_OWS_s         SvcAppStatusOws        = {0};
    static SVC_APP_STAT_RCTA_s        SvcAppStatusRcta       = {0};
    static SVC_APP_STAT_CALIB_s       SvcAppStatusCalib      = {0};
    static SVC_APP_STAT_DISP_s        SvcAppStatusDisp       = {0};
    static SVC_APP_STAT_SHMOO_REC_s   SvcAppStatusShmooRec   = {0};
    static SVC_APP_STAT_SHMOO_CV_s    SvcAppStatusShmooCv    = {0};

    static UINT32 SysStatTaskInit = 0U;
    ULONG  MemBase;
    UINT32 MemSize;

    UINT32 RetVal = SVC_OK;

    if (SysStatTaskInit == 0U) {
        RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_SYS_STAT, &MemBase, &MemSize);

        if (SVC_OK == RetVal) {
            RetVal = SvcSysStat_Config(MemBase, MemSize);
            if (SVC_OK == RetVal) {
                SvcLog_OK("SYS_STAT_TASK", "SvcSysStatTask configured done", 0U, 0U);

                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_UCODE,       &SvcAppStatusUcode,      (UINT32)sizeof(SVC_APP_STAT_UCODE_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_IQ,          &SvcAppStatusIq,         (UINT32)sizeof(SVC_APP_STAT_IQ_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_VIN,         &SvcAppStatusVin,        (UINT32)sizeof(SVC_APP_STAT_VIN_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_VOUT,        &SvcAppStatusVout,       (UINT32)sizeof(SVC_APP_STAT_VOUT_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_STG,         &SvcAppStatusSd,         (UINT32)sizeof(SVC_APP_STAT_STG_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_LINUX,       &SvcAppStatusLinux,      (UINT32)sizeof(SVC_APP_STAT_LINUX_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_CV_BOOT,     &SvcAppStatusCvBoot,     (UINT32)sizeof(SVC_APP_STAT_CV_BOOT_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_CAMCTRL,     &SvcAppStatusCamCtrl,    (UINT32)sizeof(SVC_APP_STAT_CAMCTRL_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_DSP_BOOT,    &SvcAppStatusDspBoot,    (UINT32)sizeof(SVC_APP_STAT_DSP_BOOT_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_MENU,        &SvcAppStatusMenu,       (UINT32)sizeof(SVC_APP_STAT_MENU_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_BSD,         &SvcAppStatusBsd,        (UINT32)sizeof(SVC_APP_STAT_BSD_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_OD,          &SvcAppStatusOd,         (UINT32)sizeof(SVC_APP_STAT_OD_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_DIRT_DETECT, &SvcAppStatusDirtDetect, (UINT32)sizeof(SVC_APP_STAT_DIRT_DETECT_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_DEFOG,       &SvcAppStatusDefog,      (UINT32)sizeof(SVC_APP_STAT_DEFOG_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_EMR_VER,     &SvcAppStatusEmrVer,     (UINT32)sizeof(SVC_APP_STAT_EMR_VER_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_PSD,         &SvcAppStatusPsd,        (UINT32)sizeof(SVC_APP_STAT_PSD_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_OWS,         &SvcAppStatusOws,        (UINT32)sizeof(SVC_APP_STAT_OWS_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_RCTA,        &SvcAppStatusRcta,       (UINT32)sizeof(SVC_APP_STAT_RCTA_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_CALIB,       &SvcAppStatusCalib,      (UINT32)sizeof(SVC_APP_STAT_CALIB_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_DISP,        &SvcAppStatusDisp,       (UINT32)sizeof(SVC_APP_STAT_DISP_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_SHMOO_REC,   &SvcAppStatusShmooRec,   (UINT32)sizeof(SVC_APP_STAT_SHMOO_REC_s));
                RetVal |= SvcSysStat_Hook(SVC_APP_STAT_SHMOO_CV,    &SvcAppStatusShmooCv,    (UINT32)sizeof(SVC_APP_STAT_SHMOO_CV_s));
                if (SVC_OK == RetVal) {
                    SvcLog_OK("SYS_STAT_TASK", "SvcSysStatTask Hook done", 0U, 0U);
                }
                SysStatTaskInit = 1U;
            } else {
                SvcLog_NG("SYS_STAT_TASK", "SvcSysStatTask configured failed", 0U, 0U);
            }
        } else {
            SvcLog_NG("SYS_STAT_TASK", "SvcSysStatTask buffer request failed", 0U, 0U);
        }
    } else {
        SvcLog_OK("SYS_STAT_TASK", "SvcSysStatTask has been configured", 0U, 0U);
    }

    return RetVal;
}
