/**
*  @file SvcDramUtil.h
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
*  @details dram traffic functions
*
*/

#ifndef SVC_DRAM_UTIL_H
#define SVC_DRAM_UTIL_H

#include "AmbaDRAMC.h"

#if !defined(CONFIG_SOC_CV2FS) && defined(AMBA_NUM_DRAM_CLIENT)
#define SVC_DRAMUTIL_CMD_USED   (1U)
#endif

#define DRAM_GROUP_CPU          (0U)
#define DRAM_GROUP_DSP          (1U)
#define DRAM_GROUP_PERI         (2U)
#if defined(AMBA_DRAM_CLIENT_ORCVP)
#define DRAM_GROUP_CVORC        (3U)
#define DRAM_GROUP_VPU          (4U)
#if defined(DRAM_CLIENT_FEX)
#define DRAM_GROUP_SPU          (5U)
#define DRAM_GROUP_NUM          (6U)
#else
#define DRAM_GROUP_NUM          (5U)
#endif
#else
#define DRAM_GROUP_NUM          (3U)
#endif

typedef struct {
    UINT32  MSec;
    #define SVC_DRAMUTIL_LP4_32         (0U)
    #define SVC_DRAMUTIL_LP4_16         (1U)
    #define SVC_DRAMUTIL_DDR4_32        (2U)
    #define SVC_DRAMUTIL_LP5_32         (3U)
    UINT32  DramType;
    UINT32  DramPiece;
    #define SVC_DRAMUTIL_PFMODE_IDEAL   (0U)
    #define SVC_DRAMUTIL_PFMODE_REAL    (1U)
    UINT32  ProfileMode;
} SVC_DRAMUTIL_PAR_s;

typedef struct {
    UINT64               Traffic[DRAM_GROUP_NUM];
    UINT64               TotalTraffic;
    UINT64               TotalBandWidth;
    DOUBLE               TotalUtil;
    AMBA_DRAMC_STATIS_s  DramStatic;
} SVC_DRAMUTIL_RES_s;

void SvcDramUtil_Profile(SVC_DRAMUTIL_PAR_s *pInput, SVC_DRAMUTIL_RES_s *pResult);

void SvcDramUtil_Adjust(UINT32 TaskPri, UINT32 TaskCpuBits, UINT32 AdjDuration);

#endif  /* SVC_DRAM_UTIL_H */
