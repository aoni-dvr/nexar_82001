/**
*  @file SvcBootTiming.h
*
*  @copyright Copyright (c) 2015 Ambarella, Inc.
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
*  @details svc boot timing measurement
*
*/

#ifndef SVC_BOOT_TIMING_H
#define SVC_BOOT_TIMING_H

#if defined(CONFIG_THREADX)
#include "AmbaRTSL_TMR.h"
#endif
#include "AmbaMemMap.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaTimerInfo.h"
#include "AmbaNVM_Partition.h"

#include "SvcErrCode.h"
#include "SvcLog.h"

#define SVC_TIME_POSTOS_CONFIG             (0U)
#define SVC_TIME_CALIB_LOAD_START          (1U)
#define SVC_TIME_CALIB_LOAD_DONE           (2U)
#define SVC_TIME_UCODE_LOAD_START          (3U)
#define SVC_TIME_UCODE_LOAD_DONE_PART      (4U)
#define SVC_TIME_BIST_LOAD_START           (5U)
#define SVC_TIME_BIST_LOAD_DONE            (6U)
#define SVC_TIME_UCODE_LOAD_DONE           (7U)
#define SVC_TIME_IQ_LOAD_START             (8U)
#define SVC_TIME_IQ_LOAD_DONE_PART         (9U)
#define SVC_TIME_IQ_LOAD_DONE              (10U)
#define SVC_TIME_IQ_LOAD_CHECK             (11U)
#define SVC_TIME_VIN_CONFIG_START          (12U)
#define SVC_TIME_VIN_CONFIG_DONE           (13U)
#define SVC_TIME_VOUT_CONFIG_START         (14U)
#define SVC_TIME_VOUT_CONFIG_DONE          (15U)
#define SVC_TIME_DSP_BOOT_START            (16U)
#define SVC_TIME_DSP_BOOT_DONE             (17U)
#define SVC_TIME_REC_START                 (18U)
#if defined(CONFIG_AMBALINK_BOOT_OS)
#define SVC_TIME_LINUX_BOOT_START          (19U)
#define SVC_TIME_LINUX_IPC_INIT_DONE       (20U)
#define SVC_TIME_LINUX_OS_DONE             (21U)
#define SVC_TIME_LINUX_RPMSG_INIT_DONE     (22U)
#define SVC_TIME_LINUX_RPMSG_INIT_APP_DONE (23U)
#define SVC_TIME_LINUX_BOOT_DONE           (24U)
#endif
#define SVC_TIME_CV_INIT_START             (25U)
#define SVC_TIME_CV_INIT_DONE              (26U)
#define SVC_TIME_CV_VISORC_LOAD_START      (27U)
#define SVC_TIME_CV_VISORC_LOAD_DONE       (28U)
#define SVC_TIME_CV_VISORC_BOOT_START      (29U)
#define SVC_TIME_CV_VISORC_BOOT_DONE       (30U)
#define SVC_TIME_CV_FLEXIDAG_LOAD_START    (31U)
#define SVC_TIME_CV_FLEXIDAG_LOAD_DONE     (32U)
#define SVC_TIME_CV_FLEXIDAG_BOOT_START    (33U)
#define SVC_TIME_CV_FLEXIDAG_1ST_RESULT    (34U)
            
extern UINT32 SvcTime_ForcePrint;

void SvcTime_CalUcodeDataSize(UINT32 DataSize);
void SvcTime_CalBistSize(UINT32 DataSize);
void SvcTime_CalCvVisOrcSize(UINT32 DataSize);
void SvcTime_CalCvTotalSize(UINT32 DataSize);
void SvcTime_CalAmbaLinkDataSize(UINT32 DataSize);

void SvcTime(UINT32 id, const char* pMsg);

void SvcTime_PrintBootTime(void);
void SvcTime_PrintBootTimeCv(void);
#if defined(CONFIG_AMBALINK_BOOT_OS)
void SvcTime_PrintBootTimeLinux(void);
#endif

#endif  /* SVC_BOOT_TIMING_H */
