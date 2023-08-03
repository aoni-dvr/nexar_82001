/**
 *  @file SvcSwPll.h
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
 *  @details svc sw pll
 *
 */

#ifndef SVC_SW_PLL_H
#define SVC_SW_PLL_H

#define SVC_LOG_SWPLL        "SW_PLL"
#define SVC_SW_PLL_STACK_SIZE (0x2000U)

#define SVC_SWPLL_MAX_NUM_VIN   (2U)
#define SVC_SWPLL_MAX_NUM_VOUT  (2U)

#define SVC_SWPLL_DEV_VIN0      (0U)
#define SVC_SWPLL_DEV_VIN1      (1U)
#define SVC_SWPLL_DEV_VOUT0     (2U)
#define SVC_SWPLL_DEV_VOUT1     (3U)
#define SVC_SWPLL_DEV_AUDIO     (4U)
#define SVC_SWPLL_MAX_NUM_DEV   (5U)

typedef struct {
    UINT8               Type;
    AMBA_FTCM_DEV_CFG_s Cfg;
} SVC_SW_PLL_DEV_s;

typedef struct {
    UINT32              Priority;      /* PLL adjust task priority */
    UINT32              CpuBits;       /* core selection which SVC SW PLL tasks running at */
} SVC_SW_PLL_CFG_s;

UINT32 SvcSwPll_Create(const SVC_SW_PLL_CFG_s *pCfg);
UINT32 SvcSwPll_Delete(void);
UINT32 SvcSwPll_Enable(UINT32 DevMask, UINT8 Enable);
void   SvcSwPll_Debug(UINT8 On);
void   SvcSwPll_PrintINT(void);

#endif  /* SVC_SW_PLL_H */
