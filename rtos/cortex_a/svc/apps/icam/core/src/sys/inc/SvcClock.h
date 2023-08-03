/**
*  @file SvcClock.h
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
*  @details svc clock functions
*
*/

#ifndef SVC_CLOCK_H
#define SVC_CLOCK_H

#define SVC_CLK_FEA_BIT_ALL     (0x00000001U)
#define SVC_CLK_FEA_BIT_DSP     (0x00000002U)
#define SVC_CLK_FEA_BIT_MCTF    (0x00000004U)
#define SVC_CLK_FEA_BIT_CODEC   (0x00000008U)
#define SVC_CLK_FEA_BIT_HEVC    (0x00000010U)
#define SVC_CLK_FEA_BIT_CNN     (0x00000020U)
#define SVC_CLK_FEA_BIT_FEX     (0x00000040U)
#define SVC_CLK_FEA_BIT_FMA     (0x00000080U)
#define SVC_CLK_FEATURE_NUM     (8U)


typedef struct {
    UINT32  CoreClk;
    UINT32  IdspClk;
    UINT32  VpuClk;
} SVC_CLK_CFG_s;

void SvcClock_Init(UINT32 ClkLine);
void SvcClock_FeatureCtrl(UINT32 Ctrl, UINT32 FeatureBits);
void SvcClock_Ctrl(SVC_CLK_CFG_s *pClkCfg);
void SvcClock_Dump(void);

#endif  /* SVC_CLOCK_H */
