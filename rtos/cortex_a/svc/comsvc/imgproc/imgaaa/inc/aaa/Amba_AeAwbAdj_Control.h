/**
 * @file Amba_AeAwbAdj_Control.h
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
 *
 */
#ifndef AMBA_AEAWBADJ_CONTROL
#define AMBA_AEAWBADJ_CONTROL

#define IMG_VIN_INVALID  0
#define IMG_VIN_VALID    1
#define IMG_VIN_PRIOR    2
#define IMG_VIN_POST     3
#define IMG_VIN_UNKNOW   255

/* function prototype */
void Amba_Ae_Ctrl(UINT32 ViewId);
void Amba_Awb_Ctrl(UINT32 ViewId);
void Amba_Adj_Ctrl(UINT32 ViewId);
void Amba_Adj_StillHdr_Control(UINT32 ViewID, AMBA_AE_INFO_s (*stillAeInfo)[MAX_AEB_NUM], AMBA_IK_WB_GAIN_s (*hdrWbGain)[MAX_AEB_NUM]);
UINT32 AmbaImg_VInInvalid(void);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in Amba_Adj.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 Amba_AdjInit(UINT32 ViewId);
void Amba_AdjControl(UINT32 ViewId);
void Amba_AdjStillControl(UINT32 ViewId, UINT32 aeIndx, /*const AMBA_IK_MODE_CFG_s *mode,*/ UINT16 algoMode);
void Amba_Set_MADJ(UINT32 ViewId, UINT8 enable, UINT32 madj_no);
void Amba_AdjSetDefog(UINT32 ViewId, UINT32 enable, INT32 sceneMode);
void Amba_Set_AdjTableNo(UINT32 ViewId, INT32 AdjTableNo);

void Amba_AdjPivControl(UINT32 ViewId, /*const AMBA_IK_MODE_CFG_s *mode,*/ UINT16 algoMode);

void Amba_Adj_VideoHdr_Control(UINT32 ViewID, AMBA_AE_INFO_s *videoAeInfo, const AMBA_IK_WB_GAIN_s  *hdrWbGain);

INT32 App_Image_Init_Ae_Tile_Config(UINT32 ViewId, const AMBA_IK_MODE_CFG_s *mode);
INT32 App_Image_Init_Awb_Tile_Config(UINT32 ViewId, const AMBA_IK_MODE_CFG_s *mode);
INT32 App_Image_Init_Af_Tile_Config(UINT32 ViewId, const AMBA_IK_MODE_CFG_s *mode);

UINT32 Amba_Adj_SetVideoPipeCtrlParams(UINT32 ViewID, const AMBA_IK_MODE_CFG_s *mode);
UINT32 Amba_Adj_GetVideoUpdFlag(UINT32 ViewID, UINT16 *pUpdateFlag);
UINT32 Amba_Adj_ResetVideoFlags(UINT32 ViewID, UINT8 Mode);

UINT32 Amba_Adj_SetStillPipeCtrlParams(UINT32 ViewID, UINT32 AEBIndex, const AMBA_IK_MODE_CFG_s *mode);
UINT32 Amba_Adj_ResetStillFlags(UINT32 ViewID, UINT8 Mode);
UINT32 Amba_Adj_PrtStillFlags(UINT32 ViewID);
#endif

