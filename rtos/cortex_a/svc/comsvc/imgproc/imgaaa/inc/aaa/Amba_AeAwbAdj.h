/**
 * @file Amba_AeAwbAdj.h
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
 * @Description    :: Definitions & Constants for Sample AE(Auto Exposure) ,AWB(Auto White Balance), ADJ(Auto Adjustment)) algorithms
 *
 */

#ifndef AMBA_SAMPLE_AE_AWB_ADJ_
#define AMBA_SAMPLE_AE_AWB_ADJ_

#include "AmbaImg_Proc.h"
//#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageFilter.h"

/*-----------------------------------------------------------------------------------------------*\
 * Defined in Amba_Ae.c
\*-----------------------------------------------------------------------------------------------*/
void Amba_AeInit(UINT32 chNo);
void Amba_AeControl(UINT32 chNo, AMBA_3A_STATUS_s *aaaVideoStatus, AMBA_3A_STATUS_s *aaaStillStatus);
INT32 AeGetVideoExposureValue(UINT32 chNo, FLOAT *shutter, FLOAT *agc, UINT32 *dgain);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in Amba_Awb.c
\*-----------------------------------------------------------------------------------------------*/
void Amba_AwbInit(UINT32 chNo);
void Amba_AwbControl(UINT32 chNo, AMBA_3A_STATUS_s *aaaVideoStatus, AMBA_3A_STATUS_s *aaaStillStatus);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in Amba_Adj.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 Amba_AdjInit(UINT32 ViewId);
void Amba_AdjControl(UINT32 ViewId);
void Amba_AdjStillControl(UINT32 ViewId, UINT32 aeIndx, /*const AMBA_IK_MODE_CFG_s *mode,*/ UINT16 algoMode);
void Amba_Set_MADJ(UINT32 ViewId, UINT8 enable, UINT32 madj_no);
void Amba_AdjSetDefog(UINT32 ViewId, UINT32 enable, INT32 sceneMode);
/*-----------------------------------------------------------------------------------------------*\
 * Defined in Amba_AeAwbAdj_Control.c
\*-----------------------------------------------------------------------------------------------*/
// void Amba_AeAwbAdj_Init(UINT32 viewID, UINT8 initFlg);
// void Amba_AeAwbAdj_Control(UINT32 ViewId);
// void Amba_Ae_Ctrl(UINT32 ViewId);
// void Amba_Awb_Ctrl(UINT32 ViewId);
// void Amba_Adj_Ctrl(UINT32 ViewId);

#endif  /* _AMBA_DSC_AE_AWB_ */
