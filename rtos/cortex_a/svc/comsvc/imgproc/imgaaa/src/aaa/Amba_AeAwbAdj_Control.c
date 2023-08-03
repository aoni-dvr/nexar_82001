/**
 * @file Amba_AeAwbAdj_Control.c
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
 *  @Description    :: Ae/Awb/Adj Algo. control.
 *
 */




#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#ifdef CONFIG_LINUX
#include "AmbaWrap.h"
#endif
#include "AmbaDSP_ImageFilter.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_AeAwb.h"
#include "AmbaImg_AaaDef.h"

#include "AmbaImg_External_CtrlFunc.h"
// #include "Amba_AwbCalibTskSample.h"
#include "AmbaSensor.h"
#include "Amba_AeAwbAdj_Control.h"
// #include "Amba_AwbCalib.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Adj_VideoHdr_Control
 *
 *  @Description:: Video Hdr Control
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    None
\*-----------------------------------------------------------------------------------------------*/

void Amba_Adj_VideoHdr_Control(UINT32 ViewID, AMBA_AE_INFO_s *videoAeInfo, const AMBA_IK_WB_GAIN_s  *hdrWbGain)
{
    AMBA_AAA_OP_INFO_s    AaaOpInfo = {0};
    static HDR_INFO_s    HdrInfo[MAX_VIEW_NUM] = {0};
    AMBA_AE_INFO_s    video_info;
    UINT32    U32RVal = 0U;

    U32RVal |= AmbaImgProc_GetAAAOPInfo(ViewID, &AaaOpInfo);
    if(AaaOpInfo.AeOp > 0U){
        HdrInfo[ViewID].AeInfo[0] = videoAeInfo[0];
        HdrInfo[ViewID].WbGain[0] = *hdrWbGain;

        U32RVal |= AmbaImgProc_HDRVideoCtrl(ViewID, &HdrInfo[ViewID]);
    }
    videoAeInfo[0] = HdrInfo[ViewID].AeInfo[0];
    videoAeInfo[1] = HdrInfo[ViewID].AeInfo[1];
    videoAeInfo[2] = HdrInfo[ViewID].AeInfo[2];
    videoAeInfo[3] = HdrInfo[ViewID].AeInfo[3];

    U32RVal |= AmbaImgProc_AEGetExpInfo(ViewID, 0U, IP_MODE_VIDEO, &video_info);
    video_info.HdrRatio = videoAeInfo[0].HdrRatio;
    U32RVal |= AmbaImgProc_AESetExpInfo(ViewID, 0U, IP_MODE_VIDEO, &video_info);
    if(U32RVal != 0U){
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Adj_StillHdr_Control
 *
 *  @Description:: Video Hdr Control
 *
 *  @Input      ::
 *    None
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    None
\*-----------------------------------------------------------------------------------------------*/
void Amba_Adj_StillHdr_Control(UINT32 ViewID, AMBA_AE_INFO_s (*stillAeInfo)[MAX_AEB_NUM], AMBA_IK_WB_GAIN_s (*hdrWbGain)[MAX_AEB_NUM])
{
    HDR_INFO_s    HdrInfo[MAX_AEB_NUM] = {0};
    UINT32        i = 0U, j = 0U;
    UINT32        U32RVal = 0U;

    U32RVal |= AmbaWrap_memset(HdrInfo, 0, sizeof(HDR_INFO_s) * MAX_AEB_NUM);
    for (i=0; i<MAX_AEB_NUM; i++) {
        for (j=0U; j<(UINT32)MAX_EXP_GROUP_NUM; j++) {
            HdrInfo[i].AeInfo[j] = stillAeInfo[0][i];
            HdrInfo[i].WbGain[j] = hdrWbGain[0][i];
        }
    }
    U32RVal |= AmbaImgProc_HDRStillInit(ViewID, HdrInfo);
    U32RVal |= AmbaImgProc_HDRStillCtrl(ViewID, HdrInfo);

    for (i=0; i<MAX_AEB_NUM; i++) {
        for (j=0; j<(UINT32)MAX_EXP_GROUP_NUM; j++) {
            stillAeInfo[j][i] = HdrInfo[i].AeInfo[j];
        }
    }

    for (i=0; i<MAX_AEB_NUM; i++) {
        for (j=0U; j<(UINT32)MAX_EXP_GROUP_NUM; j++) {
            hdrWbGain[j][i] = HdrInfo[i].WbGain[j];//MISRAC

        }
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Ae_Ctrl
 *
 *  @Description:: Entry point of Ae algo.
 *
 *  @Input      ::
 *    UINT32 ViewId : Index
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    None
\*-----------------------------------------------------------------------------------------------*/
void Amba_Ae_Ctrl(UINT32 ViewId)
{
    AMBA_AAA_OP_INFO_s      AaaOpInfo = {0};
    AMBA_3A_STATUS_s        VideoStatus = {0, 0, 0};
    AMBA_3A_STATUS_s        StillStatus = {0, 0, 0};
    UINT32                  U32RVal = 0U;

    U32RVal |= AmbaImgProc_GetAAAOPInfo(ViewId, &AaaOpInfo);
    U32RVal |= AmbaImgProc_GetAAAStatus(ViewId, &VideoStatus, &StillStatus);

    if (AaaOpInfo.AeOp == ENABLE) {
        U32RVal |= AmbaImgProc_AEControl(ViewId, &VideoStatus, &StillStatus);
        U32RVal |= AmbaImgProc_SetAEStatus(ViewId, VideoStatus.Ae, StillStatus.Ae);
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Awb_Ctrl
 *
 *  @Description:: Entry point of Awb algo.
 *
 *  @Input      ::
 *    UINT32 ViewId : Index
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    None
\*-----------------------------------------------------------------------------------------------*/
void Amba_Awb_Ctrl(UINT32 ViewId)
{
    AMBA_AAA_OP_INFO_s      AaaOpInfo = {0};
    AMBA_3A_STATUS_s        VideoStatus = {0, 0, 0};
    AMBA_3A_STATUS_s        StillStatus = {0, 0, 0};
    UINT32                  U32RVal = 0U;

    U32RVal |= AmbaImgProc_GetAAAOPInfo(ViewId, &AaaOpInfo);
    U32RVal |= AmbaImgProc_GetAAAStatus(ViewId, &VideoStatus, &StillStatus);
    if (AaaOpInfo.AwbOp == ENABLE) {
        U32RVal |= AmbaImgProc_AWBControl(ViewId, &VideoStatus, &StillStatus);
        U32RVal |= AmbaImgProc_SetAWBStatus(ViewId, VideoStatus.Awb, StillStatus.Awb);
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Adj_Ctrl
 *
 *  @Description:: Entry point of ADJ algo.
 *
 *  @Input      ::
 *    UINT32 ViewId : Index
 *
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    None
\*-----------------------------------------------------------------------------------------------*/
void Amba_Adj_Ctrl(UINT32 ViewId)
{

    AMBA_AAA_OP_INFO_s   AaaOpInfo ={0};
    UINT32               U32RVal = 0U;
    U32RVal |= AmbaImgProc_GetAAAOPInfo(ViewId, &AaaOpInfo);
    if (AaaOpInfo.AdjOp == ENABLE) {
        Amba_AdjControl(ViewId);
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
}
