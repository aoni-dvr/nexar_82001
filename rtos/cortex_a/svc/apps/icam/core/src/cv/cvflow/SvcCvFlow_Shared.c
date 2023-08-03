/**
 *  @file SvcCvFlow_Shared.c
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
 *  @details Implementation of SvcCvFlow Application Shared Utility
 *
 */

#include "AmbaTypes.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "AmbaMMU.h"
#include "AmbaDef.h"
#include "AmbaPrint.h"
#ifndef CONFIG_QNX
#include "AmbaRTSL_PLL.h"
#endif
#include "AmbaSYS.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcMem.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcCvFlow_Shared.h"

#include "cvapi_idsp_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "SvcCvAlgo.h"

#if defined(CONFIG_ICAM_CV_STEREO)
#if defined(CONFIG_SOC_CV2)
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_flexidag_ambastereoerr_cv2.h"
#endif
#if defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#include "cvapi_flexidag_ambaspufusion_cv2fs.h"
#include "cvapi_flexidag_ambastereoerr_cv2fs.h"
#include "cvapi_flexidag_ambaoffex_cv2fs.h"
#endif

#endif

#define SVC_LOG_CV_FLOW         "CV_FLOW"

#define LEFT_CHAN               (0x1U)
#define RIGHT_CHAN              (0x2U)

extern UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
extern UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight);
extern UINT32 SvcCvFlow_PackPicInfo_Lndt(cv_pic_info_t *pPicInfo, const AMBA_DSP_LNDT_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight);
extern void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo);
extern UINT32 SvcCvFlow_PackRawInfo(memio_source_recv_raw_t *pRawInfo, ULONG Addr, UINT32 Size, UINT32 Pitch);
extern void SvcCvFlow_DumpRawInfo(const char *pModuleName, const memio_source_recv_raw_t *pRawInfo);
#if defined(CONFIG_ICAM_CV_STEREO)
extern void SvcCvFlow_DumpSpuOutInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu);
#endif
extern void SvcCvFlow_SaveBin(const char *pModuleName, const char *pFileName, void *pBuf, const UINT32 Size);

/**
* Pack pyramid information to pic info structure
* @param [in] pPicInfo pic info structure
* @param [in] pYuvInfo pyramid information
* @param [in] MainWidth width of main YUV
* @param [in] MainHeight height of main YUV
* @param [in] UseRelative 1-transfer to relative address
* @param [in] LeftRight fill in left or right channel in pic info structure. 1-Left chan only, 2-Right chan only, 0 or 3-Duplicated
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_PackPicInfo_Pyramid(cv_pic_info_t *pPicInfo, const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, DataChan;
    ULONG  BufPhyAddr = 0U, PicInfoAddr = 0U, PicInfoPhyAddr = 0U;

    if ((pPicInfo != NULL) && (pYuvInfo != NULL)) {
        /* Important: The addresses for FlexiDAG are relative to picinfo. */
        if (UseRelative == 1U) {
            AmbaMisra_TypeCast(&PicInfoAddr, &pPicInfo);
            if (SVC_OK != SvcMem_VirtToPhys(PicInfoAddr, &PicInfoPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "PicInfoAddr V2P error", 0U, 0U);
                RetVal |= SVC_NG;
            }
        }

        if (LeftRight == 0U) { /* Duplicate the info */
            DataChan = (LEFT_CHAN | RIGHT_CHAN);
        } else {
            DataChan = LeftRight;
        }

        pPicInfo->capture_time = (UINT32) pYuvInfo->CapPts;
        pPicInfo->channel_id   = (UINT8)  pYuvInfo->ViewZoneId;
        pPicInfo->frame_num    = (UINT32) pYuvInfo->CapSequence;

        if ((pYuvInfo->YuvBuf[0].Window.Width != 0U) && (pYuvInfo->YuvBuf[0].Window.Height != 0U)) {
            pPicInfo->pyramid.image_width_m1  = pYuvInfo->YuvBuf[0].Window.Width - 1U;
            pPicInfo->pyramid.image_height_m1 = pYuvInfo->YuvBuf[0].Window.Height - 1U;
            pPicInfo->pyramid.image_pitch_m1  = (UINT32) pYuvInfo->YuvBuf[0].Pitch - 1U;
        } else {
            pPicInfo->pyramid.image_width_m1  = (UINT16) MainWidth - 1U;
            pPicInfo->pyramid.image_height_m1 = (UINT16) MainHeight - 1U;
            pPicInfo->pyramid.image_pitch_m1  = (UINT32) (GetAlignedValU32(MainWidth, 64U) + 64U) - 1U;
        }

        /* Fill layer 0 information for fusion use */
        pPicInfo->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16) pPicInfo->pyramid.image_pitch_m1;
        pPicInfo->pyramid.half_octave[0].roi_start_col  = 0;
        pPicInfo->pyramid.half_octave[0].roi_start_row  = 0;
        pPicInfo->pyramid.half_octave[0].roi_width_m1   = pPicInfo->pyramid.image_width_m1;
        pPicInfo->pyramid.half_octave[0].roi_height_m1  = pPicInfo->pyramid.image_height_m1;

        for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            if (((pYuvInfo->Config.HierBit >> i) & 0x1U) == 1U) {
                pPicInfo->pyramid.half_octave[i].ctrl.roi_pitch = pYuvInfo->YuvBuf[i].Pitch;
                pPicInfo->pyramid.half_octave[i].roi_start_col  = (INT16) pYuvInfo->YuvBuf[i].Window.OffsetX;
                pPicInfo->pyramid.half_octave[i].roi_start_row  = (INT16) pYuvInfo->YuvBuf[i].Window.OffsetY;
                pPicInfo->pyramid.half_octave[i].roi_width_m1   = pYuvInfo->YuvBuf[i].Window.Width - 1U;
                pPicInfo->pyramid.half_octave[i].roi_height_m1  = pYuvInfo->YuvBuf[i].Window.Height - 1U;

                if ((DataChan & LEFT_CHAN) > 0U) {
                    if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrY, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrY V2P error0", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpLumaLeft[i] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
                    if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrUV, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrUV V2P error0", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpChromaLeft[i] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
                }

                if ((DataChan & RIGHT_CHAN) > 0U) {
                    if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrY, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrY V2P error1", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpLumaRight[i]   = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
                    if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf[i].BaseAddrUV, &BufPhyAddr)) {
                        SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrUV V2P error1", 0U, 0U);
                        RetVal |= SVC_NG;
                    }
                    pPicInfo->rpChromaRight[i] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
                }
            }
        }

        for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            if ((pPicInfo->rpLumaRight[i] != 0U) &&
                (pPicInfo->rpLumaLeft[i] != 0U) &&
                (pPicInfo->rpLumaRight[i] != pPicInfo->rpLumaLeft[i])) {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 0U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 1U;
            } else if ((pPicInfo->rpLumaRight[i] != 0U) || (pPicInfo->rpLumaLeft[i] != 0U)) {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 0U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
            } else {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
            }
        }

        for (i = AMBA_DSP_MAX_HIER_NUM; i < MAX_HALF_OCTAVES; i++) {
            pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
            pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FLOW, "PackPicInfo_Pyramid: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Pack main YUV information to pic info structure
* @param [in] pPicInfo pic info structure
* @param [in] pYuvInfo pyramid information
* @param [in] UseRelative 1-transfer to relative address
* @param [in] LeftRight fill in left or right channel in pic info structure. 1-Left chan only, 2-Right chan only, 0 or 3-Duplicated
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_PackPicInfo_MainYuv(cv_pic_info_t *pPicInfo, const AMBA_DSP_YUV_DATA_RDY_s *pYuvInfo, UINT32 UseRelative, UINT32 LeftRight)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, DataChan;
    ULONG  BufPhyAddr = 0U, PicInfoAddr = 0U, PicInfoPhyAddr = 0U;

    if ((pPicInfo != NULL) && (pYuvInfo != NULL)) {
        /* Important: The addresses for FlexiDAG are relative to picinfo. */
        if (UseRelative == 1U) {
            AmbaMisra_TypeCast(&PicInfoAddr, &pPicInfo);
            if (SVC_OK != SvcMem_VirtToPhys(PicInfoAddr, &PicInfoPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "PicInfoAddr V2P error", 0U, 0U);
                RetVal |= SVC_NG;
            }
        }

        if (LeftRight == 0U) { /* Duplicate the info */
            DataChan = (LEFT_CHAN | RIGHT_CHAN);
        } else {
            DataChan = LeftRight;
        }

        pPicInfo->capture_time = (UINT32) pYuvInfo->CapPts;
        pPicInfo->channel_id   = (UINT8)  pYuvInfo->ViewZoneId;
        pPicInfo->frame_num    = (UINT32) pYuvInfo->CapSequence;

        pPicInfo->pyramid.image_width_m1  = pYuvInfo->Buffer.Window.Width - 1U;
        pPicInfo->pyramid.image_height_m1 = pYuvInfo->Buffer.Window.Height - 1U;
        pPicInfo->pyramid.image_pitch_m1  = (UINT32) pYuvInfo->Buffer.Pitch - 1U;

        /* Fill layer 0 information for fusion use */
        pPicInfo->pyramid.half_octave[0].ctrl.roi_pitch = pYuvInfo->Buffer.Pitch;
        pPicInfo->pyramid.half_octave[0].roi_start_col  = (INT16) pYuvInfo->Buffer.Window.OffsetX;
        pPicInfo->pyramid.half_octave[0].roi_start_row  = (INT16) pYuvInfo->Buffer.Window.OffsetY;
        pPicInfo->pyramid.half_octave[0].roi_width_m1   = pYuvInfo->Buffer.Window.Width - 1U;
        pPicInfo->pyramid.half_octave[0].roi_height_m1  = pYuvInfo->Buffer.Window.Height - 1U;

        if ((DataChan & LEFT_CHAN) > 0U) {
            if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->Buffer.BaseAddrY, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrY V2P error0", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpLumaLeft[0]   = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
            if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->Buffer.BaseAddrUV, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrUV V2P error0", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpChromaLeft[0] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
        }

        if ((DataChan & RIGHT_CHAN) > 0U) {
             if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->Buffer.BaseAddrY, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrY V2P error1", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpLumaRight[0]   = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
            if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->Buffer.BaseAddrUV, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrUV V2P error1", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpChromaRight[0] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
        }

        if ((pPicInfo->rpLumaRight[0] != 0U) &&
            (pPicInfo->rpLumaLeft[0] != 0U) &&
            (pPicInfo->rpLumaRight[0] != pPicInfo->rpLumaLeft[0])) {
            pPicInfo->pyramid.half_octave[0].ctrl.disable = 0U;
            pPicInfo->pyramid.half_octave[0].ctrl.mode    = 1U;
        } else if ((pPicInfo->rpLumaRight[0] != 0U) || (pPicInfo->rpLumaLeft[0] != 0U)) {
            pPicInfo->pyramid.half_octave[0].ctrl.disable = 0U;
            pPicInfo->pyramid.half_octave[0].ctrl.mode    = 0U;
        } else {
            pPicInfo->pyramid.half_octave[0].ctrl.disable = 1U;
            pPicInfo->pyramid.half_octave[0].ctrl.mode    = 0U;
        }

        for (i = 1U; i < MAX_HALF_OCTAVES; i++) {
            pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
            pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U; /* mono */
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FLOW, "PackPicInfo_MainYuv: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Pack pyramid extension scale information to pic info structure
* @param [in] pPicInfo pic info structure
* @param [in] pYuvInfo pyramid information
* @param [in] MainWidth width of main YUV
* @param [in] MainHeight height of main YUV
* @param [in] UseRelative 1-transfer to relative address
* @param [in] LeftRight fill in left or right channel in pic info structure. 1-Left chan only, 2-Right chan only, 0 or 3-Duplicated
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_PackPicInfo_Lndt(cv_pic_info_t *pPicInfo, const AMBA_DSP_LNDT_DATA_RDY_s *pYuvInfo, UINT32 MainWidth, UINT32 MainHeight, UINT32 UseRelative, UINT32 LeftRight)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, DataChan;
    ULONG  BufPhyAddr = 0U, PicInfoAddr = 0U, PicInfoPhyAddr = 0U;

    if ((pPicInfo != NULL) && (pYuvInfo != NULL)) {
        /* Important: The addresses for FlexiDAG are relative to picinfo. */
        if (UseRelative == 1U) {
            AmbaMisra_TypeCast(&PicInfoAddr, &pPicInfo);
            if (SVC_OK != SvcMem_VirtToPhys(PicInfoAddr, &PicInfoPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "PicInfoAddr V2P error", 0U, 0U);
                RetVal |= SVC_NG;
            }
        }

        if (LeftRight == 0U) { /* Duplicate the info */
            DataChan = (LEFT_CHAN | RIGHT_CHAN);
        } else {
            DataChan = LeftRight;
        }

        pPicInfo->capture_time = (UINT32) pYuvInfo->CapPts;
        pPicInfo->channel_id   = (UINT8)  pYuvInfo->ViewZoneId;
        pPicInfo->frame_num    = (UINT32) pYuvInfo->CapSequence;

        if ((pYuvInfo->YuvBuf.Window.Width != 0U) && (pYuvInfo->YuvBuf.Window.Height != 0U)) {
            pPicInfo->pyramid.image_width_m1  = pYuvInfo->YuvBuf.Window.Width - 1U;
            pPicInfo->pyramid.image_height_m1 = pYuvInfo->YuvBuf.Window.Height - 1U;
            pPicInfo->pyramid.image_pitch_m1  = (UINT32) pYuvInfo->YuvBuf.Pitch - 1U;
        } else {
            pPicInfo->pyramid.image_width_m1  = (UINT16) MainWidth - 1U;
            pPicInfo->pyramid.image_height_m1 = (UINT16) MainHeight - 1U;
            pPicInfo->pyramid.image_pitch_m1  = (UINT32) (GetAlignedValU32(MainWidth, 64U) + 64U) - 1U;
        }

        pPicInfo->pyramid.half_octave[0].ctrl.roi_pitch = pYuvInfo->YuvBuf.Pitch;
        pPicInfo->pyramid.half_octave[0].roi_start_col  = (INT16) pYuvInfo->YuvBuf.Window.OffsetX;
        pPicInfo->pyramid.half_octave[0].roi_start_row  = (INT16) pYuvInfo->YuvBuf.Window.OffsetY;
        pPicInfo->pyramid.half_octave[0].roi_width_m1   = pYuvInfo->YuvBuf.Window.Width - 1U;
        pPicInfo->pyramid.half_octave[0].roi_height_m1  = pYuvInfo->YuvBuf.Window.Height - 1U;

        if ((DataChan & LEFT_CHAN) > 0U) {
            if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf.BaseAddrY, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrY V2P error0", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpLumaLeft[0] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
            if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf.BaseAddrUV, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrUV V2P error0", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpChromaLeft[0] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
        }

        if ((DataChan & RIGHT_CHAN) > 0U) {
            if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf.BaseAddrY, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrY V2P error1", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpLumaRight[0]   = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
            if (SVC_OK != SvcMem_VirtToPhys(pYuvInfo->YuvBuf.BaseAddrUV, &BufPhyAddr)) {
                SvcLog_NG(SVC_LOG_CV_FLOW, "BaseAddrUV V2P error1", 0U, 0U);
                RetVal |= SVC_NG;
            }
            pPicInfo->rpChromaRight[0] = (UINT32)(BufPhyAddr - PicInfoPhyAddr);
        }

        for (i = 0; i < AMBA_DSP_MAX_HIER_NUM; i++) {
            if ((pPicInfo->rpLumaRight[i] != 0U) &&
                (pPicInfo->rpLumaLeft[i] != 0U) &&
                (pPicInfo->rpLumaRight[i] != pPicInfo->rpLumaLeft[i])) {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 0U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 1U;
            } else if ((pPicInfo->rpLumaRight[i] != 0U) || (pPicInfo->rpLumaLeft[i] != 0U)) {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 0U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
            } else {
                pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
                pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
            }
        }

        for (i = AMBA_DSP_MAX_HIER_NUM; i < MAX_HALF_OCTAVES; i++) {
            pPicInfo->pyramid.half_octave[i].ctrl.disable = 1U;
            pPicInfo->pyramid.half_octave[i].ctrl.mode    = 0U;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FLOW, "SvcCvFlow_PackPicInfo_Lndt: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Pic info dump function
* @param [in] pModuleName module name string
* @param [in] pPicInfo pic info structure
* @return none
*/
void SvcCvFlow_DumpPicInfo(const char *pModuleName, const cv_pic_info_t *pPicInfo)
{
    UINT32 i;

    if (pModuleName != NULL) {
        AmbaPrint_PrintStr5("[%s]", pModuleName, NULL, NULL, NULL, NULL);
    }
    AmbaPrint_PrintUInt5("-------------------------------------------", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("frame_num    : %d", pPicInfo->frame_num, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("token_id     : %d", pPicInfo->token_id, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("capture time : %d", pPicInfo->capture_time, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("channel_id   : %d", pPicInfo->channel_id, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("addi_flags   : %d", pPicInfo->additional_flags, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("num_tokens   : %d", pPicInfo->num_tokens, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pyramid----------------", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("image_height_m1  : %d", pPicInfo->pyramid.image_height_m1, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("image_width_m1   : %d", pPicInfo->pyramid.image_width_m1, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("image_pitch_m1   : %d", pPicInfo->pyramid.image_pitch_m1, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("half octave------------", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < (UINT32)MAX_HALF_OCTAVES; i++) {
        AmbaPrint_PrintUInt5("[%2d][ctrl] disable: %d, mode: %d, octave_mode: %d, roi_pitch: %d",
            i,
            pPicInfo->pyramid.half_octave[i].ctrl.disable,
            pPicInfo->pyramid.half_octave[i].ctrl.mode,
            pPicInfo->pyramid.half_octave[i].ctrl.octave_mode,
            pPicInfo->pyramid.half_octave[i].ctrl.roi_pitch);
        AmbaPrint_PrintUInt5("[%2d][roi]  %d X %d (%d,%d)",
            i,
            (UINT32)(pPicInfo->pyramid.half_octave[i].roi_width_m1),
            (UINT32)(pPicInfo->pyramid.half_octave[i].roi_height_m1),
            (UINT32)(pPicInfo->pyramid.half_octave[i].roi_start_col),
            (UINT32)(pPicInfo->pyramid.half_octave[i].roi_start_row));
    }
    AmbaPrint_PrintUInt5("Luma/Chroma------------", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < (UINT32)MAX_HALF_OCTAVES; i++) {
        AmbaPrint_PrintUInt5("[%2d][RIGHT]Y: 0x%X UV: 0x%X", i, pPicInfo->rpLumaRight[i], pPicInfo->rpChromaRight[i], 0U, 0U);
        AmbaPrint_PrintUInt5("[%2d][LEFT] Y: 0x%X UV: 0x%X", i, pPicInfo->rpLumaLeft[i], pPicInfo->rpChromaLeft[i], 0U, 0U);
    }
    AmbaPrint_PrintUInt5("-------------------------------------------", 0U, 0U, 0U, 0U, 0U);
}

/**
* Pack main YUV information to pic info structure
* @param [in] pRawInfo raw info structure
* @param [in] Addr address of data
* @param [in] Size data size
* @param [in] Pitch data pitch
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_PackRawInfo(memio_source_recv_raw_t *pRawInfo, ULONG Addr, UINT32 Size, UINT32 Pitch)
{
    UINT32 RetVal = SVC_OK;

    if (pRawInfo != NULL) {
        pRawInfo->magic = 0U;
        pRawInfo->addr  = (UINT32)Addr;
        pRawInfo->size  = Size;
        pRawInfo->pitch = Pitch;
        pRawInfo->rsv = 0U;
    } else {
        SvcLog_NG(SVC_LOG_CV_FLOW, "SvcCvFlow_PackRawInfo: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Raw info dump function
* @param [in] pModuleName module name string
* @param [in] pRawInfo raw info structure
* @return none
*/
void SvcCvFlow_DumpRawInfo(const char *pModuleName, const memio_source_recv_raw_t *pRawInfo)
{
    if (pModuleName != NULL) {
        AmbaPrint_PrintStr5("[%s]", pModuleName, NULL, NULL, NULL, NULL);
    }
    AmbaPrint_PrintUInt5("----------------", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("addr      : 0x%x", pRawInfo->addr, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("size      : %d", pRawInfo->size, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pitch     : %d", pRawInfo->pitch, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("batch_cnt : %d", pRawInfo->batch_cnt, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("----------------", 0U, 0U, 0U, 0U, 0U);
}

/**
* Stereo output dump function
* @param [in] pModuleName module name string
* @param [in] pOutSpu stereo output structure
* @return none
*/
#if defined(CONFIG_ICAM_CV_STEREO)
void SvcCvFlow_DumpSpuOutInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu)
{
    UINT32 i;
    ULONG Addr;
    const AMBA_CV_SPU_SCALE_OUT_s *pScaleOut;

    if (pModuleName != NULL) {
        AmbaPrint_PrintStr5("[%s]", pModuleName, NULL, NULL, NULL, NULL);
    }

    AmbaMisra_TypeCast(&Addr, &pOutSpu);
    AmbaPrint_PrintUInt5("----[SPU out @ 0x%x]----", Addr, 0U, 0U, 0U, 0U);
    for (i = 0; i < MAX_HALF_OCTAVES; i++) {
        pScaleOut = &pOutSpu->Scales[i];
        AmbaPrint_PrintUInt5("[%u]: Status(0x%x)", i, (UINT32)pScaleOut->Status, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("BufSize: %d DisparityMapOffset: 0x%x W: %d H: %d P: %d",
                             pScaleOut->BufSize,
                             pScaleOut->DisparityMapOffset,
                             pScaleOut->DisparityWidth,
                             pScaleOut->DisparityHeight,
                             pScaleOut->DisparityPitch);
        AmbaPrint_PrintUInt5("Bpp: %d RoiStartCol: %d RoiStartRow: %d RoiAbsStartCol: %d RoiAbsStartRow: %d",
                             pScaleOut->DisparityBpp,
                             pScaleOut->RoiStartCol,
                             pScaleOut->RoiStartRow,
                             pScaleOut->RoiAbsoluteStartCol,
                             pScaleOut->RoiAbsoluteStartRow);
    }
}

#if 0
void SvcCvFlow_DumpSpuOutDetailInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu)
{
    UINT32 i;
    const AMBA_CV_SPU_SCALE_OUT_s *pScaleOut;

    if (pModuleName != NULL) {
        AmbaPrint_PrintStr5("[%s]", pModuleName, NULL, NULL, NULL, NULL);
    }
    AmbaPrint_PrintUInt5("----[SPU out @ 0x%x]----", (UINT32)pOutSpu, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("Reserved_0[]: 0x%x 0x%x 0x%x 0x%x 0x%x",
                          pOutSpu->Reserved_0[0],
                          pOutSpu->Reserved_0[1],
                          pOutSpu->Reserved_0[2],
                          pOutSpu->Reserved_0[3],
                          pOutSpu->Reserved_0[4]);
    for (i = 0; i < MAX_HALF_OCTAVES; i++) {
        pScaleOut = &pOutSpu->Scales[i];
        AmbaPrint_PrintUInt5("[%u]: Status(0x%x)", i, (UINT32)pScaleOut->Status, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("BufSize: %d W: %d H: %d P: %d DisparityMapOffset: 0x%x",
                             pScaleOut->BufSize,
                             pScaleOut->DisparityWidth,
                             pScaleOut->DisparityHeight,
                             pScaleOut->DisparityPitch,
                             pScaleOut->DisparityMapOffset);
        AmbaPrint_PrintUInt5("DisparityBpp: %d Qm: %d Qf: %d",
                             pScaleOut->DisparityBpp,
                             pScaleOut->DisparityQm,
                             pScaleOut->DisparityQf,
                             0U,
                             0U);
        AmbaPrint_PrintUInt5("RoiStartCol: %d RoiStartRow: %d RoiAbsStartCol: %d RoiAbsStartRow: %d InvalidDisparities %d",
                             pScaleOut->RoiStartCol,
                             pScaleOut->RoiStartRow,
                             pScaleOut->RoiAbsoluteStartCol,
                             pScaleOut->RoiAbsoluteStartRow,
                             pScaleOut->InvalidDisparities);
        AmbaPrint_PrintUInt5("Reserved: %d %d %d %d",
                             pScaleOut->Reserved_0,
                             pScaleOut->Reserved_1,
                             pScaleOut->Reserved_2,
                             pScaleOut->Reserved_3,
                             0U);
    }
}
#endif
#endif

/**
* Save binary to file from memory
* @param [in] pModuleName module name string
* @param [in] pFileName file name
* @param [in] pBuf source buffer
* @param [in] Size data size
* @return none
*/
void SvcCvFlow_SaveBin(const char *pModuleName, const char *pFileName, void *pBuf, const UINT32 Size)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFile;
    UINT32 OpRes;

    RetVal = AmbaFS_FileOpen(pFileName, "wb", &pFile);
    if(RetVal != 0U) {
        SvcLog_NG(pModuleName, "SvcCvFlow_SaveBin AmbaFS_FileOpen failed", 0U, 0U);
    }
    if (RetVal == 0U) {
        RetVal = AmbaFS_FileWrite(pBuf, 1U, Size, pFile, &OpRes);
        if(RetVal != 0U) {
            SvcLog_NG(pModuleName, "AmbaFS_FileWrite failed", 0U, 0U);
        }
    }
    if (RetVal == 0U) {
        RetVal = AmbaFS_FileClose(pFile);
        if(RetVal != 0U) {
            SvcLog_NG(pModuleName, "AmbaFS_FileClose failed", 0U, 0U);
        }
    }
}

/**
* Get time stamp from ORC timer
* @param [in] pModuleName module name string
* @param [out] pTimeStamp time stamp pointer
* @return none
*/
void SvcCvFlow_GetTimeStamp(const char *pModuleName, UINT64 *pTimeStamp)
{
    UINT32 CurTimeU32;

    if (pTimeStamp != NULL) {
#if defined(CONFIG_LINUX)
        CurTimeU32 = 0U;
#else
        (void)AmbaSYS_GetOrcTimer(&CurTimeU32);
#endif
        *pTimeStamp = (UINT64)CurTimeU32;
    } else {
        SvcLog_NG(pModuleName, "SvcCvFlow_GetTimeStamp: Invalid pTimeStamp", 0U, 0U);
    }
}

static UINT64 CalcDiffTime(UINT64 StartTime, UINT64 EndTime, UINT64 Unit)
{
    #define TIMER_LIMIT     (0xFFFFFFFFU)
    UINT64 TimeDiff;

    if (EndTime > StartTime) {
        TimeDiff = EndTime - StartTime;
    } else {
        /* Timer wrap around */
        TimeDiff = (TIMER_LIMIT - StartTime) + EndTime;
    }
    TimeDiff = (TimeDiff * 1000000U) / Unit;

    return TimeDiff;
}

/**
* Calculate execution time
* @param [in] pModuleName module name string
* @param [in] pInfo structure for configuration and report
* @return none
*/
void SvcCvFlow_CalcFrameTime(const char *pModuleName, UINT32 NumTS, const UINT64 *pTimeStamp, SVC_CV_FRAME_TIME_CALC_s *pInfo)
{
    #define TIMER_LIMIT     (0xFFFFFFFFU)
    UINT32 RetVal = SVC_OK;
    UINT32 TimerFreq, Loop;
    UINT64 ProcTime, FrameTime;
    UINT32 CurFrmIdx, HeadFmIdx;

    if (pInfo->MaxRecFrm == 0U) {
        pInfo->MaxRecFrm = SVC_CV_FLOW_MAX_REC_FRAME;
        pInfo->CurFrmIdx = (UINT8)(SVC_CV_FLOW_MAX_REC_FRAME - 1U);
    }
    if (pInfo->DSRate == 0U) {
        pInfo->DSRate = 4U;
    }

    if ((pInfo->MaxRecFrm < 2U) || (pInfo->CurFrmIdx >= pInfo->MaxRecFrm)) {
        SvcLog_NG(pModuleName, "Invalid param: MaxRecFrm = %d, CurFrmIdx = %d", pInfo->MaxRecFrm, pInfo->CurFrmIdx);
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        /* Increment frame index and Save time stamp */
        CurFrmIdx = (UINT32)pInfo->CurFrmIdx + 1U;
        if (CurFrmIdx >= pInfo->MaxRecFrm) {
            CurFrmIdx = 0U;
        }

#if defined (CONFIG_QNX)
        (void) AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &TimerFreq);
#else
        TimerFreq = AmbaRTSL_PllGetAudioClk();
#endif

        for (UINT32 i = 0; i < NumTS; i++) {
            /* Store time stamp */
            pInfo->TimeStamp[CurFrmIdx][i] = pTimeStamp[i];

            if (i > 0U) {
                /* Calculate the diff time between each time stamp */
                pInfo->ProcTime[CurFrmIdx][i - 1U] = CalcDiffTime(pTimeStamp[i - 1U],
                                                                  pTimeStamp[i],
                                                                  TimerFreq);
            }
        }

        pInfo->CurFrmIdx = (UINT8)CurFrmIdx;
        pInfo->TotalFrm++;

        if ((pInfo->TotalFrm % pInfo->DSRate) == 0U) {
            if (pInfo->TotalFrm >= pInfo->MaxRecFrm) {
                /* Calculate average proc time */
                Loop = SVC_CV_FLOW_MAX_TIME_STAMP - 1U;
                for (UINT32 i = 0; i < Loop; i++) {
                    if (i < (NumTS - 1U)) {
                        ProcTime = 0U;
                        for (UINT32 FrmIdx = 0; FrmIdx < pInfo->MaxRecFrm; FrmIdx++) {
                            ProcTime += pInfo->ProcTime[FrmIdx][i];
                        }
                        ProcTime = ProcTime / (UINT64)pInfo->MaxRecFrm;
                        pInfo->ProcTimeMA[i] = (UINT32)ProcTime;
                    } else {
                        pInfo->ProcTimeMA[i] = 0U;
                    }
                }

                /* Calculate average frame time */
                /* Find head of the record. */
                HeadFmIdx = CurFrmIdx + 1U;
                if (HeadFmIdx >= pInfo->MaxRecFrm) {
                    HeadFmIdx = 0U;
                }

                FrameTime = CalcDiffTime(pInfo->TimeStamp[HeadFmIdx][NumTS - 1U],
                                         pInfo->TimeStamp[CurFrmIdx][NumTS - 1U],
                                         TimerFreq);
                pInfo->FrameTimeMA = (UINT32)FrameTime / ((UINT32)pInfo->MaxRecFrm - 1U);
            } else {
                pInfo->FrameTimeMA = 0U;
                AmbaSvcWrap_MisraMemset(pInfo->ProcTimeMA, 0, sizeof(pInfo->ProcTimeMA));
            }
        }
    }
}

