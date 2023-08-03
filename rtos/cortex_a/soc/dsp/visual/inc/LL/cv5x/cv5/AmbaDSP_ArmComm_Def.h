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

#define AMBA_DSP_SYNC_CMD_BUF_VDSP0             (0U)  /** for General/VCAP commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE        (1U)  /** for Vin0 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN1        (2U)  /** for Vin1 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN2        (3U)  /** for Vin2 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN3        (4U)  /** for Vin3 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN4        (5U)  /** for Vin4 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN5        (6U)  /** for Vin5 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN6        (7U)  /** for Vin6 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN7        (8U)  /** for Vin7 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN8        (9U)  /** for Vin8 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN9        (10U) /** for Vin9 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN10       (11U) /** for Vin10 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN11       (12U) /** for Vin11 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN12       (13U) /** for Vin12 commands */
#define AMBA_DSP_SYNC_CMD_BUF_VDSP1_VIN13       (14U) /** for Vin13 commands */
#define AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL          (AMBA_DSP_SYNC_CMD_BUF_VDSP1_BASE + AMBA_DSP_MAX_VIN_NUM) /** Total number of sync command buffers */

#define AMBA_DSP_CMD_DEFAULT_WRITE              (0U)    /** Default commands */
#define AMBA_DSP_CMD_ASYNC_WRITE                (1U)    /** Async commands */
#define AMBA_DSP_CMD_VDSP_NORMAL_WRITE          (2U)    /** VDSP normal commands */
#define AMBA_DSP_CMD_VDSP_GROUP_WRITE           (3U)    /** VDSP group commands */
#define AMBA_DSP_CMD_VCAP_NORMAL_WRITE          (4U)    /** VCAP normal commands */
#define AMBA_DSP_CMD_VCAP_GROUP_WRITE           (5U)    /** VCAP group commands */
#define AMBA_DSP_CMD_VCAP_VIN1_NORMAL_WRITE     (6U)    /** VCAP Vin1 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN1_GROUP_WRITE      (7U)    /** VCAP Vin1 group commands */
#define AMBA_DSP_CMD_VCAP_VIN2_NORMAL_WRITE     (8U)    /** VCAP Vin2 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN2_GROUP_WRITE      (9U)    /** VCAP Vin2 group commands */
#define AMBA_DSP_CMD_VCAP_VIN3_NORMAL_WRITE     (10U)   /** VCAP Vin3 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN3_GROUP_WRITE      (11U)   /** VCAP Vin3 group commands */
#define AMBA_DSP_CMD_VCAP_VIN4_NORMAL_WRITE     (12U)   /** VCAP Vin4 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN4_GROUP_WRITE      (13U)   /** VCAP Vin4 group commands */
#define AMBA_DSP_CMD_VCAP_VIN5_NORMAL_WRITE     (14U)   /** VCAP Vin5 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN5_GROUP_WRITE      (15U)   /** VCAP Vin5 group commands */
#define AMBA_DSP_CMD_VCAP_VIN6_NORMAL_WRITE     (16U)   /** VCAP Vin6 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN6_GROUP_WRITE      (17U)   /** VCAP Vin6 group commands */
#define AMBA_DSP_CMD_VCAP_VIN7_NORMAL_WRITE     (18U)   /** VCAP Vin7 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN7_GROUP_WRITE      (19U)   /** VCAP Vin7 group commands */
#define AMBA_DSP_CMD_VCAP_VIN8_NORMAL_WRITE     (20U)   /** VCAP Vin8 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN8_GROUP_WRITE      (21U)   /** VCAP Vin8 group commands */
#define AMBA_DSP_CMD_VCAP_VIN9_NORMAL_WRITE     (22U)   /** VCAP Vin9 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN9_GROUP_WRITE      (23U)   /** VCAP Vin9 group commands */
#define AMBA_DSP_CMD_VCAP_VIN10_NORMAL_WRITE    (24U)   /** VCAP Vin10 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN10_GROUP_WRITE     (25U)   /** VCAP Vin10 group commands */
#define AMBA_DSP_CMD_VCAP_VIN11_NORMAL_WRITE    (26U)   /** VCAP Vin11 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN11_GROUP_WRITE     (27U)   /** VCAP Vin11 group commands */
#define AMBA_DSP_CMD_VCAP_VIN12_NORMAL_WRITE    (28U)   /** VCAP Vin12 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN12_GROUP_WRITE     (29U)   /** VCAP Vin12 group commands */
#define AMBA_DSP_CMD_VCAP_VIN13_NORMAL_WRITE    (30U)   /** VCAP Vin13 normal commands */
#define AMBA_DSP_CMD_VCAP_VIN13_GROUP_WRITE     (31U)   /** VCAP Vin13 group commands */
#define AMBA_DSP_CMD_NOP                        (32U)   /** NOP cmd for debug */
#define AMBA_DSP_NUM_CMD_WRITE                  (33U)   /** Total number of cmmand write mode */

#endif  /* AMBA_DSP_ARM_COMM_DEF_H */

