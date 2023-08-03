/**
 *  @file AmbaTimerInfo.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Implementation of reference code timer info api
 *
 */

#ifndef AMBA_TIMERINFO_H
#define AMBA_TIMERINFO_H

#define STAMP_NUM (40U)

#define A53_BLD_START                 (0U)
#define A53_BLD_SHELL_START           (1U)
#define A53_BLD_POLL_USER_STOP        (2U)
#define A53_BLD_DEV_INIT_START        (3U)
#define A53_BLD_DEV_INIT_DONE         (4U)
#define A53_BLD_CHK_DRAM_TRAIN_DONE   (5U)
#define A53_BLD_LOAD_START            (6U)
#define A53_BLD_LOAD_ATF_START        (7U)
#define A53_BLD_LOAD_ATF_DONE         (8U)
#define A53_BLD_LOAD_FWUPDATER_START  (9U)
#define A53_BLD_LOAD_FWUPDATER_DONE   (10U)
#define A53_BLD_LOAD_SYS_START        (11U)
#define A53_BLD_LOAD_SYS_DONE         (12U)
#define A53_BLD_LOAD_LINUX_START      (13U)
#define A53_BLD_LOAD_LINUX_DONE       (14U)
#define A53_BLD_LOAD_QNX_START        (15U)
#define A53_BLD_LOAD_QNX_DONE         (16U)
#define A53_BLD_ENTER_ATF             (17U)
#define A53_BLD_PREOS_CONFIG          (18U)
#define A53_BLD_DECOMPRESS_START      (19U)
#define A53_BLD_DECOMPRESS_DONE       (20U)

typedef struct {
    UINT32 A53_BLD_Time[STAMP_NUM];
    UINT32 A53_APP_Time[STAMP_NUM];
    UINT32 A53_BssSize;
} AMBA_TIMER_INFO_s;

void AmbaTimeInfo_Init(void);
void AmbaTime_A53Bld(UINT32 id);

void AmbaTime_BldGetBssInfo(void);

#endif  /* AMBA_TIMERINFO_H */
