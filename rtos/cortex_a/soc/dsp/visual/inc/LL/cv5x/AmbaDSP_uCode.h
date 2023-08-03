/**
 *  @file AmbaDSP_uCode.h
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
 *  @details Definitions & Constants for DSP uCodes of CVx
 *
 */

#ifndef AMBA_DSP_UCODE_H
#define AMBA_DSP_UCODE_H

#include "idspdrv_cmd_msg.h"
#include "cmd_msg_dsp.h"
#include "AmbaDSP_IdspCfg_Def.h"

#if defined(CONFIG_THREADX)
#if !defined (CONFIG_DEVICE_TREE_SUPPORT)
extern void *__ucode_orcode_start, *__ucode_orcode_end;
extern void *__ucode_orcme_start, *__ucode_orcme_end;
extern void *__ucode_orcmd_start, *__ucode_orcmd_end;
extern void *__ucode_default_data_start, *__ucode_default_data_end;
#endif
#endif

/* ucode binary index */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define UCODE_BIN_ORCODE    (0U)
#define UCODE_BIN_ORCODE1   (1U)
#define UCODE_BIN_ORCODE2   (2U)
#define UCODE_BIN_ORCODE3   (3U)
#define UCODE_BIN_ORCME     (4U)
#define UCODE_BIN_ORCME1    (5U)
#define UCODE_BIN_ORCMD     (6U)
#define UCODE_BIN_ORCMD1    (7U)
#define UCODE_BIN_DEFAULT   (8U)
#define UCODE_BIN_NUM       (9U)
#else
#define UCODE_BIN_ORCODE    (0U)
#define UCODE_BIN_ORCME     (1U)
#define UCODE_BIN_ORCMD     (2U)
#define UCODE_BIN_DEFAULT   (3U)
#define UCODE_BIN_NUM       (4U)
#endif

/* ucode binary max size in byte unit */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) //total 0x880000
#define ORCODE_BIN_MAX_SIZE     (0x200000) //orccode
#define ORCODE1_BIN_MAX_SIZE    (0x180000) //orcvin
#define ORCODE2_BIN_MAX_SIZE    (0x200000) //orcidsp0
#define ORCODE3_BIN_MAX_SIZE    (0x200000) //orcidsp1
#define ORCME_BIN_MAX_SIZE      (0x10000)  //orcme
#define ORCMD_BIN_MAX_SIZE      (0x20000)  //orcmdxf
#define DEFAULT_BIN_MAX_SIZE    (0xA0000)
#else // total 0x700000
#define ORCODE_BIN_MAX_SIZE     (0x560000)
#define ORCME_BIN_MAX_SIZE      (0x80000)
#define ORCMD_BIN_MAX_SIZE      (0xA0000)
#define DEFAULT_BIN_MAX_SIZE    (0x80000)
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x500000U)
#define DSP_LOG_TEXT_BASE_ADDR_VIN                  (0x1800000U)
#define DSP_LOG_TEXT_BASE_ADDR_IDSP0                (0x1200000U)
#define DSP_LOG_TEXT_BASE_ADDR_IDSP1                (0x1500000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF                 (0xB00000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD                 (0x800000U)
#define DSP_LOG_TEXT_BASE_ADDR_MDXF1                (0x1E00000U)
#define DSP_LOG_TEXT_BASE_ADDR_MEMD1                (0x1B00000U)
#elif defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x600000U)
#else
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x900000U)
#endif

#define DSP_UCODE_INFO_OFFSET   (0x100U)
#define DSP_INIT_DATA_OFFSET    (0x600U)
#ifndef DSP_INIT_DATA_SIZE
#define DSP_INIT_DATA_SIZE      (sizeof(dsp_init_data_t))
#endif

#define DSP_MAX_NUM_GENERAL_CMD     (32U)          /* maximum number of commands per Buffer */
#define DSP_MAX_NUM_VCAP_CMD        (32U)          /* maximum number of commands per Buffer */
#define DSP_MAX_NUM_ASYNC_CMD       (18U)          /* maximum number of commands */

/// CMD_DSP_SET_PROFILE, cmd code 0x0100000B
typedef struct {
    uint32_t cmd_code;
}cmd_dsp_set_profile_t;

#define DSP_MSG_STATUS_U32_PAD_NUM  ((128U - sizeof(dsp_status_msg_t))/4U)
typedef struct {
    dsp_status_msg_t  Contents;
    UINT32          Padding[DSP_MSG_STATUS_U32_PAD_NUM];
} AMBA_DSP_MSG_STATUS_s;

#endif  /* AMBA_DSP_UCODE_H */
