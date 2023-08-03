/**
 *  @file AmbaDSP_ArmComm_Def.h
 *
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
 *
 *  @details Definitions & constants for the APIs of ARM-DSP communication
 *
 */

#ifndef AMBA_DSP_ARM_COMM_DEF_H
#define AMBA_DSP_ARM_COMM_DEF_H

#include "AmbaDSP.h"

#define AMBA_DSP_SYNC_CMD_BUF_VDSP0             (0U) /** for General/VCAP commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1             (1U) /** for VCAP(MainVin) commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP2             (2U) /** for 2nd Vin commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP2_3RD_VIN     (3U) /** for 3rd Vin commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP2_4TH_VIN     (4U) /** for 4th Vin commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP2_5TH_VIN     (5U) /** for 5th Vin commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP2_6TH_VIN     (6U) /** for 6th Vin commands */
#define AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL          (7U) /** Total number of sync command buffers */

#define AMBA_DSP_CMD_DEFAULT_WRITE              (0U)    /** < Default commands */
#define AMBA_DSP_CMD_VDSP_NORMAL_WRITE          (1U)    /** < VDSP normal commands */
#define AMBA_DSP_CMD_VDSP_GROUP_WRITE           (2U)    /** < VDSP group commands */
#define AMBA_DSP_CMD_VCAP_NORMAL_WRITE          (3U)    /** < VCAP normal commands */
#define AMBA_DSP_CMD_VCAP_GROUP_WRITE           (4U)    /** < VCAP group commands */
#define AMBA_DSP_CMD_VCAP2_NORMAL_WRITE         (5U)    /** < 2nd VCAP normal commands */
#define AMBA_DSP_CMD_VCAP2_GROUP_WRITE          (6U)    /** < 2nd VCAP group commands */
#define AMBA_DSP_CMD_ASYNC_WRITE                (7U)    /** < Async commands */
#define AMBA_DSP_CMD_VCAP2_3RD_VIN_NORMAL_WRITE (8U)    /** < 2nd VCAP 3rd Vin normal commands */
#define AMBA_DSP_CMD_VCAP2_3RD_VIN_GROUP_WRITE  (9U)    /** < 2nd VCAP 3rd Vin group commands */
#define AMBA_DSP_CMD_VCAP2_4TH_VIN_NORMAL_WRITE (10U)   /** < 2nd VCAP 4th Vin normal commands */
#define AMBA_DSP_CMD_VCAP2_4TH_VIN_GROUP_WRITE  (11U)   /** < 2nd VCAP 4th Vin group commands */
#define AMBA_DSP_CMD_VCAP2_5TH_VIN_NORMAL_WRITE (12U)   /** < 2nd VCAP 5th Vin normal commands */
#define AMBA_DSP_CMD_VCAP2_5TH_VIN_GROUP_WRITE  (13U)   /** < 2nd VCAP 5th Vin group commands */
#define AMBA_DSP_CMD_VCAP2_6TH_VIN_NORMAL_WRITE (14U)   /** < 2nd VCAP 6th Vin normal commands */
#define AMBA_DSP_CMD_VCAP2_6TH_VIN_GROUP_WRITE  (15U)   /** < 2nd VCAP 6th Vin group commands */
#define AMBA_DSP_CMD_NOP                        (16U)   /** < NOP cmd for debug */
#define AMBA_DSP_NUM_CMD_WRITE                  (17U)   /** Total number of cmmand write mode */

extern void DSP_SetupInitDataEnent(const UINT32 Threshold);

#endif  /* AMBA_DSP_ARM_COMM_DEF_H */
