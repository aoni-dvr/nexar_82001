/**
 * @file idspdrv_error.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @brief Error code message header file
 *
 * History:
 *    2020/03/15 - [Shou-Ling Peng] created the file.
 *
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 **/

#ifndef IDSPDRV_ERROR_H_
#define IDSPDRV_ERROR_H_

#define ERR_CODE_MOD_IDSPDRV 6u

/*****************************************************************************
 * module_idx 6 ERR_CODE_MOD_IDSPDRV
 *****************************************************************************/
#define IDSPDRV_ERR_CODE_UNKNOWN 0x60000u
#define IDSPDRV_ERR_MSG_UNKNOWN "IDSPDRV: unknown error arg0:%d arg1:%d arg2:%d arg3:%d arg4:%d\n"

#define IDSPDRV_ERR_CODE_PARAM_OUT_OF_RANGE 0x60001u
#define IDSPDRV_ERR_MSG_PARAM_OUT_OF_RANGE "parameter (%d) out of range %d\n"

#define IDSPDRV_ERR_CODE_INVALID_ALIGNMENT 0x60002u
#define IDSPDRV_ERR_MSG_INVALID_ALIGNMENT "address (0x%08x) is not %d-byte aligned (%d)\n"

#define IDSPDRV_ERR_CODE_CFG_BUF_TOO_SMALL 0x60003u
#define IDSPDRV_ERR_MSG_CFG_BUF_TOO_SMALL "sect%d cfg_buf too small: %d\n"

#define IDSPDRV_ERR_CODE_INCONSISTENT_SEC_ID 0x60004u
#define IDSPDRV_ERR_MSG_INCONSISTENT_SEC_ID "Inconsistent section ID: lookup %d header %d\n"

#define IDSPDRV_ERR_CODE_NULL_ADDR 0x60005u
#define IDSPDRV_ERR_MSG_NULL_ADDR "memory address is NULL\n"

#define IDSPDRV_ERR_CODE_OUTPUT_TOO_WIDE 0x60006u
#define IDSPDRV_ERR_MSG_OUTPUT_TOO_WIDE "output width %d too big!\n"

#define IDSPDRV_ERR_CODE_UNSUPPORTED_FEATURE 0x60007u
#define IDSPDRV_ERR_MSG_UNSUPPORTED_FEATURE "unsupported feature\n"

#define IDSPDRV_ERR_CODE_WARP_ACTWIN_WIDE 0x60008u
#define IDSPDRV_ERR_MSG_WARP_ACTWIN_WIDE "warp active window too wide: %d\n"

#define IDSPDRV_ERR_CODE_WARP_ACTWIN_TALL 0x60009u
#define IDSPDRV_ERR_MSG_WARP_ACTWIN_TALL "warp active window too tall: %d\n"

#define IDSPDRV_ERR_CODE_WARP_ACTWIN_START_LATE 0x6000au
#define IDSPDRV_ERR_MSG_WARP_ACTWIN_START_LATE "warp active window starts too late: %d\n"

#define IDSPDRV_ERR_CODE_TRANSFER_SIZE_TOO_BIG 0x6000bu
#define IDSPDRV_ERR_MSG_TRANSFER_SIZE_TOO_BIG "transfer size too big: %d\n"

#define IDSPDRV_ERR_NUM 0xcu

extern const char * const idspdrv_err_code[IDSPDRV_ERR_NUM];

#endif

