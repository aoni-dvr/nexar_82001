/**
 *  @file AmbaAf_AfProc.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Constants and Definitions for Amba Image Auto Focus Process
 *
*/

#ifndef AMBA_AF_PROC_H
#define AMBA_AF_PROC_H

#include "AmbaTypes.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaErrorCode.h"
#include <AmbaShell.h>
#include "AmbaMisraFix.h"    // for misra

#ifdef CONFIG_SOC_CV5
#define AF_UTIL_MAX_HEX_STR_LEN        (13)
#endif

/* Error Code */
#define AF_ERR_NONE                   (0U)
#define AF_ERR_MANAGER_OK          (AF_ERR_BASE + 0U)
#define AF_ERR_MANAGER_TSK_ERR     (AF_ERR_BASE + 1U)


#define AMBA_AF_DEBUG_DISABLE  0U
#define AMBA_AF_DEBUG_STAT_PRT_EN  1U
#define AMBA_AF_DEBUG_INFO_PRT_EN  2U

/* global function prototypes */
UINT32 AmbaAf_AfProcess(const AMBA_IK_CFA_3A_DATA_s *pCfaStat, const AMBA_IK_PG_3A_DATA_s *pRgbStat);

extern void AmbaAf_AfTestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#endif
