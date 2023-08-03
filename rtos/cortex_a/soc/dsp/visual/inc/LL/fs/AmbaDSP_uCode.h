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
#include "cmd_msg_prod_dv.h"
#include "AmbaDSP_IdspCfg_Def.h"

#if defined(CONFIG_THREADX)
extern void *__ucode_orcode_start, *__ucode_orcode_end;
extern void *__ucode_orcme_start, *__ucode_orcme_end;
extern void *__ucode_orcmd_start, *__ucode_orcmd_end;
extern void *__ucode_default_data_start, *__ucode_default_data_end;
#endif

/* ucode binary index */
#define UCODE_BIN_ORCODE    (0U)
#define UCODE_BIN_ORCME     (1U)
#define UCODE_BIN_ORCMD     (2U)
#define UCODE_BIN_DEFAULT   (3U)
#define UCODE_BIN_NUM       (4U)

/* ucode binary max size in byte unit */
// total 0x700000
#define ORCODE_BIN_MAX_SIZE     (0x560000)
#define ORCME_BIN_MAX_SIZE      (0x80000)
#define ORCMD_BIN_MAX_SIZE      (0xA0000)
#define DEFAULT_BIN_MAX_SIZE    (0x80000)

#ifndef USE_GCOVR
/* Following defines shall be update base on different chip */
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x600000U)
#else
#define DSP_LOG_TEXT_BASE_ADDR_CORE                 (0x0U)
#endif

#define DSP_UCODE_INFO_OFFSET   (0x100U)
#define DSP_INIT_DATA_OFFSET    (0x200U)
#define DSP_INIT_DATA_SIZE      (sizeof(dsp_init_data_t))

#define DSP_MAX_NUM_GENERAL_CMD     (32U)          /* maximum number of commands per Buffer */
#define DSP_MAX_NUM_VCAP_CMD        (32U)          /* maximum number of commands per Buffer */

#define DSP_DEFAULT_BIN_DATA_SIZE   (0x00100000U)  /* default binary data size in byte */

/// CMD_DSP_SET_PROFILE, cmd code 0x0100000B
typedef struct {
    uint32_t cmd_code;
}cmd_dsp_set_profile_t;

typedef struct {
    dsp_status_msg_t  Contents;
#ifdef SUPPORT_DSP_MSG_CRC
    UINT32          Padding[25];
#else
    UINT32          Padding[26];
#endif
} AMBA_DSP_MSG_STATUS_s;

#endif  /* AMBA_DSP_UCODE_H */
