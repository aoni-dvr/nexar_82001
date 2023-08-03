/**
*  @file AmbaDSP_ApiCheck.c
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
*  @details DSP API input check
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_ContextUtility.h"

static Dsp_CheckApi_t *pDspApiCheckFunc = NULL;

static UINT32 IsVpMsgCtrlExist(UINT32 *pExist)
{
    UINT32 Rval = OK;

    *pExist = IS_VP_MSG_CTRL_EXIST;

    return Rval;
}

static UINT32 IsHierFilterExist(UINT32 *pExist)
{
    UINT32 Rval = OK;

    *pExist = IS_HEIR_FILTER_EXIST;

    return Rval;
}

static UINT32 IsLndtFilterExist(UINT32 *pExist)
{
    UINT32 Rval = OK;

    *pExist = IS_LNDT_FILTER_EXIST;

    return Rval;
}

static UINT32 IsMainY12FilterExist(UINT32 *pExist)
{
    UINT32 Rval = OK;

    *pExist = IS_MAIN_Y12_FILTER_EXIST;

    return Rval;
}

static inline UINT32 checkStruct_VpMsgCtrlExist(const AMBA_DSP_SYS_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (IsValidULAddr(pCfg->VpMsgCtrlBufAddr) == 1U) {
        const AMBA_DSP_MSG_BUFF_CTRL_s *pMsgCtrl = NULL;

        dsp_osal_typecast(&pMsgCtrl, &(pCfg->VpMsgCtrlBufAddr));
        if (IsValidU32Addr(pMsgCtrl->MsgBufAddr) != 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pMsgCtrl->MsgNum == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pMsgCtrl->RP >= pMsgCtrl->MsgNum) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pMsgCtrl->WP >= pMsgCtrl->MsgNum) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("checkStruct_VpMsgCtrlExist Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DspSysCfg(const AMBA_DSP_SYS_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U, VpCtrlExist = 0U;

    if (pCfg->SysState > AMBA_DSP_SYS_STATE_SENSORLESS) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IsValidULAddr(pCfg->DebugLogDataAreaAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->DebugLogDataAreaSize > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (IsValidULAddr(pCfg->WorkAreaAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->WorkAreaSize > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }

    (void)IsVpMsgCtrlExist(&VpCtrlExist);
    if (VpCtrlExist == 0U) {
        if (pCfg->VpMsgCtrlBufAddr != 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        Rval = checkStruct_VpMsgCtrlExist(pCfg);
        if (Rval != OK) {
            ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_SYS_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DspWindow(const AMBA_DSP_WINDOW_s *pWindow)
{
    UINT32 Rval = OK, ErrLine = 0U;

//    if (pWindow->OffsetX > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pWindow->OffsetY > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pWindow->Width > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pWindow->Height > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pWindow->Width == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pWindow->Height == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_WINDOW_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DspBuffer(const AMBA_DSP_BUF_s *pBuffer)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (IsValidULAddr(pBuffer->BaseAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IS_ALIGN_NUM(pBuffer->Pitch, (UINT32)DSP_BUF_ALIGNMENT) != 1U) {
        Rval = DSP_ERR_0002; ErrLine = __LINE__;
    }
    if (checkStruct_DspWindow(&(pBuffer->Window)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_BUF_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DspFrate(const AMBA_DSP_FRAME_RATE_s *pFrate)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pFrate->Interlace > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pFrate->TimeScale > 4294967295U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pFrate->NumUnitsInTick > 4294967295U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_FRAME_RATE_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DspRawBuffer(const AMBA_DSP_RAW_BUF_s *pBuffer)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pBuffer == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pBuffer->Compressed > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (IsValidULAddr(pBuffer->BaseAddr) != 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
//        if (pBuffer->Pitch > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
        if (checkStruct_DspWindow(&(pBuffer->Window)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_RAW_BUF_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DspYuvImgBuffer(const AMBA_DSP_YUV_IMG_BUF_s *pBuffer)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pBuffer->DataFmt > AMBA_DSP_YUV422_10BIT) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IsValidULAddr(pBuffer->BaseAddrY) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//        if (IsValidULAddr(pBuffer->BaseAddrUV) != 1U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
    if (IS_ALIGN_NUM(pBuffer->Pitch, (UINT32)DSP_BUF_ALIGNMENT) != 1U) {
        Rval = DSP_ERR_0002; ErrLine = __LINE__;
    }
    if (checkStruct_DspWindow(&(pBuffer->Window)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_YUV_IMG_BUF_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_LiveviewRawBuffer(const AMBA_DSP_LIVEVIEW_RAW_BUF_s *pBuffer)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;

    if (pBuffer->AllocType > ALLOC_EXTERNAL_CYCLIC) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pBuffer->BufNum > 255U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pBuffer->RawFormat >= RAW_FORMAT_MEM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    /* only check when ALLOC_EXTERNAL_DISTINCT, or ALLOC_EXTERNAL_CYCLIC */
    if ((pBuffer->AllocType == ALLOC_EXTERNAL_DISTINCT) || \
        (pBuffer->AllocType == ALLOC_EXTERNAL_CYCLIC)) {
        /* Distinct case */
//            if ((checkStruct_DspRawBuffer(&(pBuffer->Buf)) != OK)) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
    } else {
        //
    }
    if ((pBuffer->AllocType == ALLOC_EXTERNAL_DISTINCT) || \
        (pBuffer->AllocType == ALLOC_EXTERNAL_CYCLIC)) {
        /* not all the cases have CE */
//            if ((checkStruct_DspRawBuffer(&(pBuffer->AuxBuf)) != OK)) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
    } else {
        //
    }
    /* only check when ALLOC_EXTERNAL_DISTINCT */
    /* only check when user specified pRawBufTbl */
    if (pBuffer->AllocType == ALLOC_EXTERNAL_DISTINCT) {
        dsp_osal_typecast(&ULAddr, &(pBuffer->pRawBufTbl));
        if (IsValidULAddr(ULAddr) != 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        dsp_osal_typecast(&ULAddr, &(pBuffer->pAuxBufTbl));
        if (IsValidULAddr(ULAddr) != 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_LIVEVIEW_RAW_BUF_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_LiveviewYuvBuffer(const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pBuffer)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;

    if (pBuffer->AllocType > ALLOC_EXTERNAL_CYCLIC) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pBuffer->BufNum > 255U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    /* only check when ALLOC_EXTERNAL_CYCLIC */
    if (pBuffer->AllocType == ALLOC_EXTERNAL_CYCLIC) {
        /* only check when use not using pYuvBufTbl */
        if ((checkStruct_DspYuvImgBuffer(&(pBuffer->YuvBuf)) != OK)) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        // DO NOTHING
    }
    /* only check when ALLOC_EXTERNAL_DISTINCT */
    /* only check when user specified pYuvBufTbl */
    if (pBuffer->AllocType == ALLOC_EXTERNAL_DISTINCT) {
        dsp_osal_typecast(&ULAddr, &(pBuffer->pYuvBufTbl));
        if ((IsValidULAddr(ULAddr) != 1U)) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            //
        }
    } else {
        //
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_LIVEVIEW_YUV_BUF_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_LiveviewChanWin(const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s* pChanCfg, UINT16 NumChan)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 Idx;
    UINT8 BldIdx;
    const AMBA_DSP_BUF_s *pBldBuf = NULL;

    if (pChanCfg != NULL) {
        for (Idx=0U; Idx<NumChan; Idx++) {
            if (pChanCfg[Idx].ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_DspWindow(&(pChanCfg[Idx].ROI)) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_DspWindow(&(pChanCfg[Idx].Window)) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (pChanCfg[Idx].RotateFlip >= AMBA_DSP_ROTATE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            /* check LumaAlphaTable only if user specified */
            if (pChanCfg[Idx].BlendNum > DSP_MAX_PP_STRM_BLEND_NUMBER) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            } else {
                if (IsValidULAddr(pChanCfg[Idx].LumaAlphaTable) == 1U) {
                    dsp_osal_typecast(&pBldBuf, &pChanCfg[Idx].LumaAlphaTable);
                    for (BldIdx=0U; (BldIdx<pChanCfg[Idx].BlendNum); BldIdx++) {
                        if (checkStruct_DspBuffer(&pBldBuf[BldIdx]) != OK) {
                            Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
                        }
                    }
                }
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("checkStruct_LiveviewChanWin Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_LiveviewStrmCfg(const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 NumChan;

    if (pCfg->StreamId >= AMBA_DSP_MAX_YUVSTRM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->Purpose > 0xFU) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->DestVout > (UINT16)DSP_Num2Mask(AMBA_DSP_MAX_VOUT_NUM)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->DestEnc > 65535U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->Width > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->Height > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->MaxWidth > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->MaxHeight > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }

    if (checkStruct_LiveviewYuvBuffer(&(pCfg->StreamBuf)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    NumChan = pCfg->NumChan;
    if (NumChan > AMBA_DSP_MAX_YUVSTRM_VIEW_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (checkStruct_LiveviewChanWin(pCfg->pChanCfg, NumChan) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_LIVEVIEW_STREAM_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 IsValidVinId(const UINT16 VinId)
{
    UINT32 Rval = OK;
    UINT16 TmpVinId;

    if (1U == DSP_GetU16Bit(VinId, VIN_VIRT_IDX, 1U)) {
        TmpVinId = (UINT16)DSP_GetU16Bit(VinId, 0U, 15U);
        if (TmpVinId >= AMBA_DSP_MAX_VIRT_VIN_NUM) {
            Rval = DSP_ERR_0001;
        }
    } else {
        if (VinId >= AMBA_DSP_MAX_VIN_NUM) {
            Rval = DSP_ERR_0001;
        }
    }

    return Rval;
}

static UINT32 checkStruct_LiveviewExtRaw(const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtRaw)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (checkStruct_DspRawBuffer(&(pExtRaw->RawBuffer)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    /* only check when user specified CeBuffer */
//        if ((pExtRaw->CeBuffer != NULL) &&
//            (checkStruct_DspRawBuffer(&(pExtRaw->CeBuffer)) != OK)) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
    if (OK != IsValidVinId(pExtRaw->VinId)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pExtRaw->IsLast > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//        if (pExtRaw->CapPts > 1) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (pExtRaw->CapSequence > 1) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_LIVEVIEW_EXT_RAW_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_LiveviewExtYuv(const AMBA_DSP_EXT_YUV_BUF_s *pExtYuv)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pExtYuv == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (checkStruct_DspYuvImgBuffer(&(pExtYuv->ExtYuvBuf)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        /* only check when user specified pExtME1Buf */
        if (pExtYuv->pExtME1Buf != NULL) {
            if (checkStruct_DspBuffer(pExtYuv->pExtME1Buf) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            } else {
                //
            }
        } else {
            //
        }
        /* only check when user specified pExtME0Buf */
        if (pExtYuv->pExtME0Buf != NULL) {
            if (checkStruct_DspBuffer(pExtYuv->pExtME0Buf) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            } else {
                //
            }
        } else {
            //
        }
        if (pExtYuv->IsLast > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_EXT_YUV_BUF_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_LiveviewPyramid(const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid)
{
    UINT32 Rval = OK, ErrLine = 0U, HierFilterExist = 0U;
    UINT16 IsPoly;

    (void)IsHierFilterExist(&HierFilterExist);
    if (HierFilterExist == 0U) {
        if (pPyramid->HierBit > 0x0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pPyramid->IsPolySqrt2 > 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        if (pPyramid->HierBit > (UINT16)DSP_Num2Mask(AMBA_DSP_MAX_HIER_NUM)) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        IsPoly = (UINT16)DSP_GetU16Bit(pPyramid->IsPolySqrt2, DSP_PYMD_PLOY_IDX, DSP_PYMD_PLOY_LEN);
        if (IsPoly > 0x1U) {
          Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
//            if (pPyramid->HierPolyWidth > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pPyramid->HierPolyHeight > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (checkStruct_DspWindow(&(pPyramid->HierCropWindow[0])) != OK) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pPyramid->HierTag > 4294967295U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_LIVEVIEW_PYRAMID_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_LiveviewLndt(const AMBA_DSP_LIVEVIEW_LANE_DETECT_s *pLndt)
{
    UINT32 Rval = OK, ErrLine = 0U, HierFilterExist = 0U;

    (void)IsHierFilterExist(&HierFilterExist);
    if (HierFilterExist == 0U) {
        if (pLndt->HierSource > 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        if (pLndt->HierSource >= AMBA_DSP_MAX_HIER_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_LIVEVIEW_LANE_DETECT_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 checkStruct_LvVzCfgVinUpper(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    //check height alignment
    //especially allow 1080, which will be converted to 1088 by ALIGN_NUM
    //we shall only need to make sure dsp to prepare alignment buffer
//    if ((IS_ALIGN_NUM(pViewZoneCfg->MainHeight, (UINT32)DSP_VPROC_MAIN_H_ALIGN) != 1U) &&
//        (pViewZoneCfg->MainHeight != 1080U)) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pViewZoneCfg->RotateFlip >= AMBA_DSP_ROTATE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pViewZoneCfg->HdrExposureNum > DSP_VIN_HIST_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

//    if (pViewZoneCfg->HdrBlendHieght > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("Upper AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 checkStruct_LvVzCfgVinLower(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pViewZoneCfg->ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (pViewZoneCfg->VinSource >= VIN_SRC_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pViewZoneCfg->VinSource != VIN_SRC_FROM_DEC) {
        if (OK != IsValidVinId(pViewZoneCfg->VinId)) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
//    if (pViewZoneCfg->VinRoi.OffsetX > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pViewZoneCfg->VinRoi.OffsetY > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pViewZoneCfg->VinRoi.Width == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pViewZoneCfg->VinRoi.Height == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pViewZoneCfg->MainWidth == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if ((pViewZoneCfg->VinSource != VIN_SRC_FROM_DEC) &&
        (pViewZoneCfg->VinSource != VIN_SRC_FROM_RECON)) {
        if (IS_ALIGN_NUM(pViewZoneCfg->MainWidth, DSP_VPROC_MAIN_W_ALIGN) != 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (pViewZoneCfg->MainHeight == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Lower AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 checkStruct_LvVzCfgUpper(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT32 HierFilterExist = 0U;

    if (pViewZoneCfg->LinearCE > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (checkStruct_LiveviewPyramid(&(pViewZoneCfg->Pyramid)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        (void)IsHierFilterExist(&HierFilterExist);
        if (HierFilterExist == 0U) {
            if (pViewZoneCfg->pPyramidBuf != NULL) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else {
            if (pViewZoneCfg->Pyramid.HierBit != 0U) {
                if (pViewZoneCfg->pPyramidBuf == NULL) {
                    Rval = DSP_ERR_0000; ErrLine = __LINE__;
                } else {
                    if (checkStruct_LiveviewYuvBuffer(pViewZoneCfg->pPyramidBuf) != OK) {
                        Rval = DSP_ERR_0001; ErrLine = __LINE__;
                    }
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Upper AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 checkStruct_LvVzCfgLower(const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT32 LndtFilterExist = 0U;
    UINT32 MainY12FilterExist = 0U;

    if (checkStruct_LiveviewLndt(&(pViewZoneCfg->LaneDetect)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        (void)IsLndtFilterExist(&LndtFilterExist);
        if (LndtFilterExist == 0U) {
            if (pViewZoneCfg->pLaneDetectBuf != NULL) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else {
            if (pViewZoneCfg->pLaneDetectBuf != NULL) {
                if (checkStruct_LiveviewYuvBuffer(pViewZoneCfg->pLaneDetectBuf) != OK) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
            }
        }
    }

    (void)IsMainY12FilterExist(&MainY12FilterExist);
    if (MainY12FilterExist == 0U) {
        if (pViewZoneCfg->pMainY12Buf != NULL) {
            if (checkStruct_LiveviewYuvBuffer(pViewZoneCfg->pMainY12Buf) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (pViewZoneCfg->DramUtilityPipe >= DSP_DRAM_PIPE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Lower AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncGopCfg(const AMBA_DSP_VIDEO_GOP_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->GOPStruct > AMBA_DSP_VIDEO_ENC_GOP_HIER_P) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->M > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->N > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pCfg->IdrInterval > 65535U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_GOP_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncNumRef(const AMBA_DSP_VIDEO_ENC_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->NumPRef > 2U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->NumBRef > 4U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_ENC_CONFIG_s:NumRef Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}


static UINT32 checkStruct_EncQpCfg(const AMBA_DSP_VIDEO_QP_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->QpMinIFrame > 51U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QpMaxIFrame > 51U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QpMinPFrame > 51U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QpMaxPFrame > 51U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QpMinBFrame > 51U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QpMaxBFrame > 51U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QpReduceIFrame > 10U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QpReducePFrame > 10U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->Aqp > 20U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_QP_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncDeBlockCtrl(const AMBA_DSP_VIDEO_DEBLOCK_CTRL_s *pCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCtrl->EnableLoopFilter > 0x7U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCtrl->LoopFilterAlpha > 12U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCtrl->LoopFilterBeta > 12U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_DEBLOCK_CTRL_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncFrmCropCtrl(const AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s *pCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCtrl->CroppingFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCtrl->CroppingFlag == 1U) {
        // Refer to HL_VideoEnc_FillCrop
        if (pCtrl->CropLeftOffset > 127U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCtrl->CropRightOffset > 127U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCtrl->CropTopOffset > 255U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCtrl->CropBottomOffset > 127U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncVuiUpper(const AMBA_DSP_VIDEO_ENC_VUI_s *pVui)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pVui->VuiEnable > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->AspectRatioInfoPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->OverscanInfoPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->OverscanAppropriateFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->VideoSignalTypePresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->VideoFullRangeFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->ColourDescriptionPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->ChromaLocInfoPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->TimingInfoPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Upper AMBA_DSP_VIDEO_ENC_VUI_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncVuiLower(const AMBA_DSP_VIDEO_ENC_VUI_s *pVui)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pVui->FixedFrameRateFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->NalHrdParametersPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->VclHrdParametersPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->LowDelayHrdFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->PicStructPresentFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->BitstreamRestrictionFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pVui->MotionVectorsOverPicBoundariesFlag > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pVui->AspectRatioIdc > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->SarWidth > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->SarHeight > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->VideoFormat > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->ColourPrimaries > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->TransferCharacteristics > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->MatrixCoefficients > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->ChromaSampleLocTypeTopField > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->ChromaSampleLocTypeBottomField > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->VbrCbpRate > 4294967295U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->Log2MaxMvLengthHorizontal > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->Log2MaxMvLengthVertical > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->NumReorderFrames > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->MaxDecFrameBuffering > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->MaxBytesPerPicDenom > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pVui->MaxBitsPerMbDenom > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Lower AMBA_DSP_VIDEO_ENC_VUI_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncBRateCfg(const AMBA_DSP_VIDEO_BIT_RATE_CFG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    //FIXME AMBA_DSP_BRC_SMART_VBR = 1U
    if (pCfg->BitRateCtrl >= AMBA_DSP_BRC_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QualityLevel > 100U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->BitRate > 4294967295U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->CPBSize > 4294967295U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->VBRComplexLvl > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pCfg->VBRPercent > 99U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->VBRMinRatio > 100U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if ((pCfg->VBRMaxRatio < 99U) && (pCfg->VBRMaxRatio > 511U)) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->RcPMul > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_BIT_RATE_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncIntraRefreshCtrl(const AMBA_DSP_VIDEO_INTRA_REFRESH_CTRL_s *pCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;

//    if (pCtrl->IntraRefreshCycle > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pCtrl->IntraRefreshMode > 7U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCtrl->IntraRefreshFrame > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if ((pCtrl->IntraRefreshQpAdj < -51) && (pCtrl->IntraRefreshQpAdj > 51)) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCtrl->IntraRefreshMaxDecodeDelay > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_INTRA_REFRESH_CTRL_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncCfgUpper(const AMBA_DSP_VIDEO_ENC_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

//    if (pCfg->ProfileIDC > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (pCfg->LevelIDC > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pCfg->IsCabac > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncGopCfg(&(pCfg->GOPCfg)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncNumRef(pCfg) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->FirstGOPStartB > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncDeBlockCtrl(&(pCfg->DeblockFilter)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncFrmCropCtrl(&(pCfg->FrameCrop)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncVuiUpper(&(pCfg->Vui)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncVuiLower(&(pCfg->Vui)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncBRateCfg(&(pCfg->BitRateCfg)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Upper AMBA_DSP_VIDEO_ENC_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncCfgLower(const AMBA_DSP_VIDEO_ENC_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (IsValidULAddr(pCfg->BitsBufAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->BitsBufSize > DSP_ENC_MAX_BS_BUF_SIZE) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IS_ALIGN_NUM(pCfg->BitsBufSize, DSP_ENC_MAX_BS_BUF_WIDTH) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->NumSlice > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pCfg->NumTile > 3U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->NumExtraRecon > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (checkStruct_EncIntraRefreshCtrl(&(pCfg->IntraRefreshCtrl)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->PicOrderCntType > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (pCfg->SPSPPSHeaderInsertMode > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->AUDMode > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->NonRefPIntvl > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (IsValidULAddr(pCfg->QuantMatrixAddr) != 1U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("Lower AMBA_DSP_VIDEO_ENC_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncStrmCfg(const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->CodingFmt > AMBA_DSP_ENC_CODING_TYPE_H264) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if ((pCfg->SrcYuvStreamId >= AMBA_DSP_MAX_YUVSTRM_NUM) &&
        (pCfg->SrcYuvStreamId != ENC_STRM_EXTMEM_IDX)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_DspWindow(&(pCfg->Window)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_DspFrate(&(pCfg->FrameRate)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->Rotate >= AMBA_DSP_ROTATE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->IntervalCapture > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncCfgUpper(&(pCfg->EncConfig)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_EncCfgLower(&(pCfg->EncConfig)) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncStartCfg(const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->EncDuration > 65535U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->EncSkipFrame > 65535U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_ENC_START_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncRepDropCfg(const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
//        if (pCfg->Count > 255U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
        if (pCfg->DropRepeatOption > AMBA_DSP_VIDEO_REPEAT_FRAME) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_EncQt(const AMBA_DSP_VIDEO_ENC_QUALITY_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->DemandIDR > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->BitRateCtrl > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->GOPCtrl > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->QPCtrl > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->BitRateCtrl == 1U) {
//        if (pCfg->BitRate > 4294967295U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//    }
    if (pCfg->GOPCtrl == 1U) {
        if (checkStruct_EncGopCfg(&(pCfg->GOPCfg)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (pCfg->QPCtrl == 1U) {
        if (checkStruct_EncQpCfg(&(pCfg->QpCfg)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDEO_ENC_QUALITY_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_VoutDefImgCfg(const AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->FieldRepeat > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (pCfg->Pitch > 65535U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (IsValidULAddr(pCfg->BaseAddrY) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IsValidULAddr(pCfg->BaseAddrUV) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if ((pCfg->DataFormat != AMBA_DSP_YUV420) &&
        (pCfg->DataFormat != AMBA_DSP_YUV422)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_VoutVdoCfg(const AMBA_DSP_VOUT_VIDEO_CFG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (checkStruct_DspWindow(&(pCfg->Window)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->RotateFlip >= AMBA_DSP_ROTATE_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->VideoSource > VOUT_SOURCE_INTERNAL) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->VideoSource == VOUT_SOURCE_DEFAULT_IMAGE) {
            if (checkStruct_VoutDefImgCfg(&(pCfg->DefaultImgConfig)) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VOUT_VIDEO_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_VoutOsdBufferCfg(const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pCfg->FieldRepeat > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
//        if (pCfg->Pitch > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (IsValidULAddr(pCfg->BaseAddr) != 1U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (pCfg->InputWidth > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (pCfg->InputHeight > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
        if (checkStruct_DspWindow(&(pCfg->Window)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->DataFormat >= OSD_FORMAT_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        /* only check if using CLUT */
        if (pCfg->DataFormat == OSD_8BIT_CLUT_MODE) {
            if (IsValidULAddr(pCfg->CLUTAddr) != 1U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else {
            //
        }
        if (pCfg->SwapByteEnable > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->PremultipliedEnable > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->GlobalBlendEnable > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->TransparentColorEnable > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        /* only check if using TransparentColor */
        if (pCfg->TransparentColorEnable == 1U) {
//            if (pCfg->TransparentColor > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
        } else {
            //
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VOUT_OSD_BUF_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_VoutDispCfg(const AMBA_DSP_DISPLAY_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
//        if (pCfg->VinVoutSyncDelay > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (pCfg->VoutSyncDelay > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (pCfg->VinTargetIndex >= AMBA_DSP_MAX_VIN_NUM) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_DISPLAY_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_VoutMixerCfg(const AMBA_DSP_VOUT_MIXER_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
//        if (pCfg->ActiveWidth > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (pCfg->ActiveHeight > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
        if (pCfg->VideoHorReverseEnable > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (checkStruct_DspFrate(&(pCfg->FrameRate)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->MixerColorFormat > MIXER_IN_YUV_444_RGB) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VOUT_MIXER_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_VoutDataPathCfg(const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pCfg->UseMixer > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->MixerIdx > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VOUT_DATA_PATH_CFG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkImgWindowRange(UINT16 ImgWindowSize)
{
    UINT32 Rval = OK;

    if ((ImgWindowSize > 8192U) || (ImgWindowSize < 16U)) {
        Rval = DSP_ERR_0001;
    }

    return Rval;
}

static UINT32 checkStruct_DecCfg(const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->StreamID >= AMBA_DSP_MAX_DEC_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->BitsFormat != AMBA_DSP_DEC_BITS_FORMAT_H264) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IsValidULAddr(pCfg->BitsBufAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if ((pCfg->BitsBufSize == 0U) || (pCfg->BitsBufSize > DSP_DEC_MAX_BS_BUF_SIZE)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval == OK) {
        Rval = checkImgWindowRange(pCfg->MaxFrameWidth);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        Rval = checkImgWindowRange(pCfg->MaxFrameHeight);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        Rval = checkImgWindowRange(pCfg->MaxVideoBufferWidth);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        Rval = checkImgWindowRange(pCfg->MaxVideoBufferHeight);
        DSP_FillErrline(Rval, &ErrLine, __LINE__);

        if (pCfg->XcodeMode > AMBA_DSP_XCODE_NON_THROTTLE) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pCfg->XcodeMode > AMBA_DSP_XCODE_NONE) {
            Rval = checkImgWindowRange(pCfg->XcodeWidth);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
            Rval = checkImgWindowRange(pCfg->XcodeHeight);
            DSP_FillErrline(Rval, &ErrLine, __LINE__);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDDEC_START_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DecStart(const AMBA_DSP_VIDDEC_START_CONFIG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->Direction > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->PreloadDataSize == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->SpeedIndex > AMBA_DSP_VIDDEC_SPEED_1_16X) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDDEC_START_CONFIG_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DecBitsInfo(const AMBA_DSP_VIDDEC_BITS_FIFO_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (IsValidULAddr(pCfg->StartAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IsValidULAddr(pCfg->EndAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDDEC_BITS_FIFO_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_DecTrickPlay(const AMBA_DSP_VIDDEC_TRICKPLAY_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->Operation > AMBA_DSP_VIDDEC_STEP) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->ResumeDirection > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pCfg->ResumeSpeedIndex > AMBA_DSP_VIDDEC_SPEED_1_16X) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_VIDDEC_TRICKPLAY_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_TestFrameCtrl(const AMBA_DSP_TEST_FRAME_CTRL_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pCfg->StageId == DSP_TEST_STAGE_VDSP_0) {
        if (pCfg->EnableMask == 0U) {
            //disable
        } else if (pCfg->EnableMask == 1U) {
            // 0: GOP N2M1 for IPIPIP with 1 slice (default)
        } else if (pCfg->EnableMask == 2U) {
            // 1: GOP N4M1 for IPPPIPPP with 4 slices
        } else if (pCfg->EnableMask == 4U) {
            // 2: GOP N5M1 for IPPPPIPPPP with 3 slices
        } else {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else if (pCfg->StageId == DSP_TEST_STAGE_VDSP_1) {
        if (pCfg->EnableMask == 0U) {
            //disable
        } else if (pCfg->EnableMask == 1U) {
            // Orig
        } else if (pCfg->EnableMask == 2U) {
            // cv2a
        } else if (pCfg->EnableMask == 4U) {
            // forman
        } else {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        //
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AMBA_DSP_TEST_FRAME_CTRL_s Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/* Main */
static UINT32 check_MainInit(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pDspSysConfig == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (checkStruct_DspSysCfg(pDspSysConfig) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainInit Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_MainGetDefaultSysCfg(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;

    dsp_osal_typecast(&ULAddr, &pDspSysConfig);
    if (IsValidULAddr(ULAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainGetDefaultSysCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_MainSetWorkArea(ULONG WorkAreaAddr, const UINT32 WorkSize)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (IsValidULAddr(WorkAreaAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (WorkSize == 0U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainSetWorkArea Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_MainGetDspVerInfo(const AMBA_DSP_VERSION_INFO_s *pInfo)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;

    dsp_osal_typecast(&ULAddr, &pInfo);
    if (IsValidULAddr(ULAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainGetDspVerInfo Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_MainMsgParseEntry(const UINT32 EntryArg)
{
    UINT32 Rval = OK;
    (void)EntryArg;
//    if (EntryArg > 4294967295U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
//    if (Rval != OK) {
//        AmbaLL_LogUInt5("MainMsgParseEntry Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
//    }
    return Rval;
}

static UINT32 check_MainWaitVinInterrupt(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;
    UINT32 VinFlag = 0U;

    (void)HL_GetVinWaitFlagTotalMask(&VinFlag);
    if (Flag > VinFlag) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    dsp_osal_typecast(&ULAddr, &pActualFlag);
    if (IsValidULAddr(ULAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (TimeOut > 65535U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainWaitVinInterrupt Check Error[0x%X][%d] [0x%X 0x%X]", Rval, ErrLine, Flag, VinFlag, 0U);
    }
    return Rval;
}

static UINT32 check_MainWaitVoutInterrupt(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;
    UINT32 VoutFlag = 0U;

    (void)HL_GetVoutWaitFlagTotalMask(&VoutFlag);
    if (Flag > VoutFlag) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    dsp_osal_typecast(&ULAddr, &pActualFlag);
    if (IsValidULAddr(ULAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (TimeOut > 65535U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainWaitVoutInterrupt Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_MainWaitFlag(const UINT32 Flag, const UINT32 *pActualFlag, const UINT32 TimeOut)
{
//    UINT32 Rval = OK;
//    (void)Flag;
//    (void)pActualFlag;
//    (void)TimeOut;
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;

    if (Flag > (AMBA_DSP_MESSAGE_ARRIVED)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    dsp_osal_typecast(&ULAddr, &pActualFlag);
    if (IsValidULAddr(ULAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (TimeOut > 65535U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainWaitFlag Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 check_MainResourceLimitLv(const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvResource)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pLvResource->VideoPipe > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pLvResource->LowDelayMode > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pLvResource->MaxVinBit > (UINT16)DSP_Num2Mask(AMBA_DSP_MAX_VIN_NUM)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pLvResource->MaxViewZoneNum > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainResource Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 check_MainResourceLimitLvVZ(const AMBA_DSP_LIVEVIEW_RESOURCE_s *pLvResource)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT32 HierFilterExist = 0U;
    UINT32 LndtFilterExist = 0U;
    UINT16 i = 0U;

    if (pLvResource->MaxViewZoneNum > 0U) {
        for (i=0U; i<AMBA_DSP_MAX_VIN_NUM; i++) {
            if (pLvResource->MaxVinVirtChanOutputNum[i][0U] > AMBA_DSP_MAX_VIEWZONE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
//            if (pLvResource->MaxVinWidth[i] > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pLvResource->MaxVinHeight[i] > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
            if (checkStruct_LiveviewRawBuffer(&(pLvResource->RawBuf[i][0U])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
//            if (pLvResource->MaxVinBootTimeout[i] > 4294967295U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pLvResource->MaxVinTimeout[i] > 4294967295U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pLvResource->MaxRaw2YuvDelay[i] > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
            if (pLvResource->MaxWarpDma[i] > WARP_DMA_MAX_VALUE) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
//            if (pLvResource->MaxWarpWaitLineLuma[i] > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pLvResource->MaxWarpWaitLineChroma[i] > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
            (void)IsHierFilterExist(&HierFilterExist);
            if (HierFilterExist == 0U) {
                if (pLvResource->MaxHierWidth[i] > 0U) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
                if (pLvResource->MaxHierHeight[i] > 0U) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
            } else {
//                if (pLvResource->MaxHierWidth[i] > 65535U) {
//                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
//                }
//                if (pLvResource->MaxHierHeight[i] > 65535U) {
//                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
//                }
            }

            (void)IsLndtFilterExist(&LndtFilterExist);
            if (LndtFilterExist == 0U) {
                if (pLvResource->MaxLndtWidth[i] > 0U) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
                if (pLvResource->MaxLndtHeight[i] > 0U) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
            } else {
//                if (pLvResource->MaxLndtWidth[i] > 65535U) {
//                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
//                }
//                if (pLvResource->MaxLndtHeight[i] > 65535U) {
//                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
//                }
            }

            if (pLvResource->DefaultRawFormat[i] >= RAW_FORMAT_MEM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainResource LvVZ Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 check_MainResourceLimitEnc(const AMBA_DSP_ENCODE_RESOURCE_s *pEncResource)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 i = 0U;

    if (pEncResource->MaxEncodeStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pEncResource->MaxEncodeStream > 0U) {
        for (i=0U; i<AMBA_DSP_MAX_STREAM_NUM; i++) {
//            if (pEncResource->MaxGopM[i] > 255U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pEncResource->MaxExtraReconNum[i] > 255U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pEncResource->MaxSearchRange[i] > 255U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
            if (pEncResource->MaxStrmFmt[i] > 0x87U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainResource Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 check_MainResourceLimitStl(const AMBA_DSP_STILL_RESOURCE_s *pStlResource)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pStlResource->MaxProcessFormat > 0x1FU) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pStlResource->MaxProcessFormat > 0U) {
//        for (i=0U; i<AMBA_DSP_MAX_VIN_NUM i++) {
//            if (pStlResource->MaxVinWidth[i] > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pStlResource->MaxVinHeight[i] > 65535U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (ErrLine != 0U) {
//                break;
//            }
//        }
//        if (pStlResource->MaxMainWidth > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
//        if (pStlResource->MaxMainHeight > 65535U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainResource Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 check_MainResourceLimitDec(const AMBA_DSP_DECODE_RESOURCE_s *pDecResource)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 i = 0U;

    if (pDecResource->MaxDecodeStreamNum > AMBA_DSP_MAX_DEC_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (pDecResource->MaxDecodeStreamNum > 0U) {
        for (i=0U; i<AMBA_DSP_MAX_DEC_STREAM_NUM; i++) {
            if (pDecResource->MaxStrmFmt[i] > 7U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
            }
        }
        if (pDecResource->MaxFrameWidth > 8192U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pDecResource->MaxFrameHeight > 8192U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
//        if (pDecResource->MaxBitRate > 65536U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
        if (pDecResource->MaxVideoPlaneWidth > 8192U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pDecResource->MaxVideoPlaneHeight > 8192U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
//        if (pDecResource->MaxRatioOfGopNM > 65536U) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
        if (pDecResource->BackwardTrickPlayMode > 3U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pDecResource->MaxPicWidth > 8192U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pDecResource->MaxPicHeight > 8192U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("MainResource Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

//static inline UINT32 check_MainResourceLimitLvEx(const AMBA_DSP_LIVEVIEW_RESOURCE_EX_s *pLvResourceEx)
//{
//    UINT32 Rval = OK;
//(void)pLvResourceEx;
//    return Rval;
//}

static UINT32 check_MainResourceLimit(const AMBA_DSP_RESOURCE_s *pResource)
{
    UINT32 Rval = OK, ErrLine = 0U;
//    UINT16 i = 0U;

    if (pResource != NULL) {
        if (check_MainResourceLimitLv(&(pResource->LiveviewResource)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (check_MainResourceLimitLvVZ(&(pResource->LiveviewResource)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (check_MainResourceLimitEnc(&(pResource->EncodeResource)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (check_MainResourceLimitStl(&(pResource->StillResource)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (check_MainResourceLimitDec(&(pResource->DecodeResource)) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }

        {   /* DisplayResource */
//            for (i=0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
//                if (pResource->DisplayResource.MaxVoutWidth[i] > 65535U) {
//                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
//                }
//                if (pResource->DisplayResource.MaxOsdBufWidth[i] > 65535U) {
//                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
//                }
//                if (ErrLine != 0U) {
//                    break;
//                }
//            }
        }
//        if (check_MainResourceLimitLvEx(&(pResource->LiveviewResourceEx)) != OK) {
//            Rval = DSP_ERR_0001; ErrLine = __LINE__;
//        }
    } else {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("MainResource Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_GetCmprRawBufInfo(const UINT16 Width, const UINT16  CmprRate, const UINT16 *pRawWidth, const UINT16 *pRawPitch)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Compressed = (UINT8)DSP_GetU16Bit(CmprRate, 0, 8);
    UINT8 Compact = (UINT8)(CmprRate - 255U);

(void)Width;
    if (pRawWidth == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pRawPitch == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (Compressed >= RAW_COMPRESS_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (CmprRate > 255U) {
        if (Compact >= RAW_COMPACT_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        // TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("GetCmprRawBufInfo Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_ParLoadConfig(const UINT32 Enable, const UINT32 Data)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)Data;
    if ((Enable != 0U) && (Enable != 1U)) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        // TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("ParLoadConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_ParLoadRegionUnlock(const UINT16 RegionIdx, const UINT16  SubRegionIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
(void)SubRegionIdx;
    if (RegionIdx >= DSP_REGION_NUM) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        // TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("ParLoadRegionUnlock Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_CalcEncMvBufInfo(const UINT16 Width, const UINT16  Height, const UINT32 Option, const UINT32 *pBufSize)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Fmtype = (UINT8)Option;
(void)Width;
(void)Height;
    if (pBufSize == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (Fmtype != AMBA_DSP_ENC_MV_FMT_AVC) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        // TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("CalcEncMvBufInfo Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_TestFrameConfig(const UINT32 StageId, const AMBA_DSP_TEST_FRAME_CFG_s *pCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
//    UINT16 i;

    if (StageId >= DSP_TEST_STAGE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pCfg->NumTestFrame > DSP_MAX_TEST_FRAME_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pCfg->Interval == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
//            for (i = 0U; i<pCfg->NumTestFrame; i++) {
//                if (pCfg->TestFrameCfg[i] == 0U) {
//                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
//                }
//                if (ErrLine != 0U) {
//                    break;
//                }
//            }
            if ((StageId == DSP_TEST_STAGE_VDSP_0) &&
                ((pCfg->pBitsBufAddr == NULL) ||
                 (pCfg->BitsBufSize == 0U))) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("TestFrameConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_TestFrameCtrl(const UINT32 NumStage, const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U, Idx;

    if (NumStage > DSP_TEST_STAGE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pCtrl == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx = 0; Idx<NumStage; Idx++) {
            if (checkStruct_TestFrameCtrl(&(pCtrl[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("TestFrameCtrl Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_MainGetBufInfo(const UINT32 Type, const AMBA_DSP_BUF_INFO_s *pBufInfo)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pBufInfo == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (Type >= NUM_DSP_BUF_TYPE) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        // DO NOTHING
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("check_MainGetBufInfo Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_MainSafetyChk(const UINT32 ID, const UINT32 SubID, const UINT32 *pStatus)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (ID >= DSP_RESET_STATUS_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (SubID > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStatus == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        // DO NOTHING
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("check_MainSafetyChk Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_MainSafetyCfg(const UINT32 ID, const UINT32 Val0, const UINT32 Val1)
{
    UINT32 Rval = OK, ErrLine = 0U;

    (void)Val0;
    (void)Val1;
    if (ID > DSP_SAFETY_ENET) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        // DO NOTHING
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("check_MainSafetyCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/* Liveview */
static UINT32 check_LiveviewConfig(const UINT16 NumViewZone,
                                   const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s* pViewZoneCfg,
                                   const UINT16 NumYUVStream,
                                   const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumViewZone > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (NumYUVStream > AMBA_DSP_MAX_YUVSTRM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumViewZone; Idx++) {
            if (checkStruct_LvVzCfgVinUpper(&(pViewZoneCfg[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_LvVzCfgVinLower(&(pViewZoneCfg[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_LvVzCfgUpper(&(pViewZoneCfg[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_LvVzCfgLower(&(pViewZoneCfg[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }

        for (Idx=0U; Idx<NumYUVStream; Idx++) {
            if (checkStruct_LiveviewStrmCfg(&(pYUVStrmCfg[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/* Liveview */
static UINT32 check_LiveviewUpdateConfig(const UINT16 NumYuvStream,
                                         const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

(void)pAttachedRawSeq;
    if (NumYuvStream > AMBA_DSP_MAX_YUVSTRM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pYuvStrmCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumYuvStream; Idx++) {
            if (checkStruct_LiveviewStrmCfg(&(pYuvStrmCfg[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewCtrl(const UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8* pEnable)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

(void)pViewZoneId;
    if (NumViewZone > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumViewZone; Idx++) {
            if (pEnable[Idx] > 1U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewCtrl Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewUpdateIsoCfg(const UINT16 NumViewZone,
                                         const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
//    UINT8 Idx = 0U;

(void)pIsoCfgCtrl;
(void)pAttachedRawSeq;
    if (NumViewZone > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
//        for (Idx=0U; Idx<NumViewZone; Idx++) {
//            if (pIsoCfgCtrl[Idx].ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pIsoCfgCtrl[Idx].CtxIndex > 65536U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pIsoCfgCtrl[Idx].CfgIndex > 65536U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (IsValidULAddr(pIsoCfgCtrl[Idx].CfgAddress) != 1U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (pIsoCfgCtrl[Idx].HdrCfgIndex > 65536U) {
//                Rval = DSP_ERR_0001; ErrLine = __LINE__;
//            }
//            if (ErrLine != 0U) {
//                break;
//            }
//        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateIsoCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 check_LvUptVinCfgCtrl(const UINT16 SubChNum,
                                           const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 Idx = 0U;

    for (Idx=0U; Idx<SubChNum; Idx++) {
        if (pLvVinCfgCtrl[Idx].EnaVinCompand == 1U) {
            if (IsValidULAddr(pLvVinCfgCtrl[Idx].VinCompandTableAddr) != 1U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (IsValidULAddr(pLvVinCfgCtrl[Idx].VinDeCompandTableAddr) != 1U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
        if (ErrLine != 0U) {
            break;
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("check_LvUptVinCfgCtrl Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static inline UINT32 check_LvUptVinCfgSubChan(const UINT16 SubChNum,
                                              const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 Idx = 0U;

    for (Idx=0U; Idx<SubChNum; Idx++) {
        if (pSubCh[Idx].IsVirtual > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (pSubCh[Idx].IsVirtual > 0U) {
            if (pSubCh[Idx].Index >= AMBA_DSP_MAX_VIRT_CHAN_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else {
            if (pSubCh[Idx].Index >= AMBA_DSP_MAX_VIN_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
        if (ErrLine != 0U) {
            break;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("check_LvUptVinCfgSubChan Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewUpdateVinCfg(const UINT16 VinId,
                                         const UINT16 SubChNum,
                                         const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                         const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAttachedRawSeq;
    if (OK != IsValidVinId(VinId)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (SubChNum > AMBA_DSP_MAX_VIRT_CHAN_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pSubCh == NULL) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pLvVinCfgCtrl == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (check_LvUptVinCfgSubChan(SubChNum, pSubCh) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }

        if (check_LvUptVinCfgCtrl(SubChNum, pLvVinCfgCtrl) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateVinCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewUpdatePymdCfg(const UINT16 NumViewZone,
                                          const UINT16 *pViewZoneId,
                                          const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                          const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                          const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAttachedRawSeq;
    if (NumViewZone > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pViewZoneId == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pPyramid == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pPyramidBuf == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (checkStruct_LiveviewPyramid(pPyramid) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
        if (checkStruct_LiveviewYuvBuffer(pPyramidBuf) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdatePymdCfg Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruc_LvCfgVinCapTDCfg(const UINT16 TDNum, const UINT16* pTDFrmNum)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 i;

    if (TDNum > 1U) {
        if (pTDFrmNum == NULL) {
            Rval = DSP_ERR_0000; ErrLine = __LINE__;
        } else {
            for (i=0U; i<TDNum; i++) {
                if (pTDFrmNum[i] > AMBA_DSP_MAX_VIN_TD_FRM_NUM) {
                    Rval = DSP_ERR_0000; ErrLine = __LINE__; break;
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("checkStruc_LvCfgVinCapTDCfg Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 checkStruc_LvCfgVinCapSubChCfg(const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pSubChCfg->SubChan.IsVirtual > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pSubChCfg->SubChan.IsVirtual > 0U) {
        if (pSubChCfg->SubChan.Index >= AMBA_DSP_MAX_VIRT_CHAN_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        if (OK != IsValidVinId(pSubChCfg->SubChan.Index)) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (pSubChCfg->Option >= AMBA_DSP_VIN_CAP_OPT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pSubChCfg->Option == AMBA_DSP_VIN_CAP_OPT_PROG) {
        if (pSubChCfg->ConCatNum > 4U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    } else {
        if (pSubChCfg->IntcNum > 4U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (checkStruct_DspWindow(&pSubChCfg->CaptureWindow) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (checkStruc_LvCfgVinCapTDCfg(pSubChCfg->TDNum, pSubChCfg->pTDFrmNum) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("checkStruc_LvCfgVinCapSubChCfg Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewConfigVinCapture(const UINT16 VinId,
                                             const UINT16 SubChNum,
                                             const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (OK != IsValidVinId(VinId)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (SubChNum > AMBA_DSP_MAX_VIRT_CHAN_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pSubChCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<SubChNum; Idx++) {
            Rval = checkStruc_LvCfgVinCapSubChCfg(&pSubChCfg[Idx]);
            if (Rval != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewConfigVinCapture Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}


static UINT32 check_LiveviewConfigVinPost(const UINT8 Type, const UINT16  VinId)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (Type > 0x3U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (OK != IsValidVinId(VinId)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        //TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewConfigVinPost Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}


static UINT32 check_LiveviewFeedRawData(const UINT16 NumViewZone,
                                        const UINT16 *pViewZoneId,
                                        const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumViewZone > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pViewZoneId == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pExtBuf == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumViewZone; Idx++) {
            if (pViewZoneId[Idx] >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_LiveviewExtRaw(&(pExtBuf[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewFeedRawData Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewFeedYuvData(const UINT16 NumViewZone,
                                        const UINT16 *pViewZoneId,
                                        const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumViewZone > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pViewZoneId == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pExtYuvBuf == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumViewZone; Idx++) {
            if (pViewZoneId[Idx] >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_LiveviewExtYuv(&(pExtYuvBuf[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (IS_ALIGN_NUM(pExtYuvBuf->ExtYuvBuf.Window.Height, (UINT32)DSP_BUF_ALIGNMENT) == 1U) {
                //
            } else {
                ULONG LummaEnd = pExtYuvBuf->ExtYuvBuf.BaseAddrY + \
                                  ((ULONG)pExtYuvBuf->ExtYuvBuf.Pitch * (ULONG)pExtYuvBuf->ExtYuvBuf.Window.Height);
                UINT32 ChromaOffset = (UINT32)(pExtYuvBuf->ExtYuvBuf.BaseAddrUV - LummaEnd);
                UINT32 BufferHeight = pExtYuvBuf->ExtYuvBuf.Window.Height + (ChromaOffset / pExtYuvBuf->ExtYuvBuf.Pitch);

                if (IS_ALIGN_NUM(BufferHeight, 16U) != 1U) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewFeedYuvData Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewGetIDspCfg(const UINT16 ViewZoneId,
                                       const ULONG *CfgAddr)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;
    UINT16 Id = (UINT8)DSP_GetU16Bit(ViewZoneId, DSP_VIEWZONE_ID_BIT_IDX, DSP_VIEWZONE_ID_LEN);
    UINT16 Type = (UINT8)DSP_GetU16Bit(ViewZoneId, DSP_VIEWZONE_TYPE_IDX, DSP_VIEWZONE_TYPE_LEN);

    if (Type >= NUM_DSP_VIEWZONE_TYPE) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        if (Type == DSP_VIEWZONE_TYPE_LV) {
            if (Id >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }

        dsp_osal_typecast(&ULAddr, &CfgAddr);
        if (IsValidULAddr(ULAddr) != 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewGetIDspCfg Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewSidebandUpdate(const UINT16 ViewZoneId,
                                           const UINT8 NumBand,
                                           const ULONG *pSidebandBufAddr)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0U;

    if (ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (NumBand > MAX_SIDEBAND_BUF_ARRAY_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        dsp_osal_typecast(&ULAddr, &pSidebandBufAddr);
        if (IsValidULAddr(ULAddr) != 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            UINT32 Idx;

            for (Idx=0U; Idx<NumBand; Idx++) {
                if (IsValidULAddr(pSidebandBufAddr[Idx]) != 1U) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewSidebandUpdate Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_LiveviewYuvStreamSync(const UINT16 YuvStrmIdx,
                                          const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl,
                                          const UINT32 *pSyncJobId,
                                          const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    ULONG ULAddr = 0u;

(void)pAttachedRawSeq;
    dsp_osal_typecast(&ULAddr, &pSyncJobId);
    if (YuvStrmIdx >= AMBA_DSP_MAX_YUVSTRM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pYuvStrmSyncCtrl == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (IsValidULAddr(ULAddr) != 1U) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pYuvStrmSyncCtrl->TargetViewZoneMask == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pYuvStrmSyncCtrl->Opt > AMBA_DSP_YUVSTRM_SYNC_EXECUTE) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            //TBD
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewYuvStreamSync Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_LiveviewUpdateGeoCfg(const UINT16 ViewZoneId,
                                         const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAttachedRawSeq;
    if (ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pGeoCfgCtrl == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (IsValidULAddr(pGeoCfgCtrl->GeoCfgAddr) != 1U) {
            Rval = DSP_ERR_0000; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateGeoCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_LiveviewUpdateVZSrcCfg(const UINT16 NumViewZone,
                                           const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg,
                                           const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAttachedRawSeq;
    if (NumViewZone > AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pVzSrcCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        // TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewUpdateVZSrcCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_LiveviewSliceCfg(const UINT16 ViewZoneId,
                                     const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pLvSliceCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        // TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewSliceCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_LiveviewVZPostponeCfg(const UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (ViewZoneId >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pVzPostPoneCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pVzPostPoneCfg->StageId >= AMBA_DSP_VZ_POSTPONE_STAGE_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewVZPostponeCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_LiveviewUpdateVinState(const UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumVin > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pVinState == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumVin; Idx++) {
            if (pVinState[Idx].VinId >= AMBA_DSP_MAX_VIN_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (pVinState[Idx].State < AMBA_DSP_VIN_STATE_DISCNNT) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (pVinState[Idx].State >= AMBA_DSP_VIN_STATE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("LiveviewVinLost Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/* StillCapture */
static UINT32 check_DataCapCfgType(const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pDataCapCfg->CapDataType >= DSP_DATACAP_TYPE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_RAW) {
            if (pDataCapCfg->Index >= AMBA_DSP_MAX_VIN_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_YUV) {
            if (pDataCapCfg->Index >= AMBA_DSP_MAX_YUVSTRM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_SYNCED_YUV) {
            if (pDataCapCfg->Index >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else if (pDataCapCfg->CapDataType == DSP_DATACAP_TYPE_COMP_RAW) {
            if (pDataCapCfg->Index >= AMBA_DSP_MAX_VIEWZONE_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else {
            //TBD
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("DataCapCfgType Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_DataCapCfg(const UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (CapInstance >= AMBA_DSP_MAX_DATACAP_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pDataCapCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pDataCapCfg->AuxDataNeeded > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if ((pDataCapCfg->AllocType != ALLOC_EXTERNAL_DISTINCT) &&
                   (pDataCapCfg->AllocType != ALLOC_EXTERNAL_CYCLIC)) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pDataCapCfg->BufNum > EXT_MEM_MAX_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pDataCapCfg->OverFlowCtrl > 1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pDataCapCfg->CmprRate > RAW_COMPRESS_6P5) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            Rval = check_DataCapCfgType(pDataCapCfg);
            if (Rval != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("DataCapCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_UpdateCapBuffer(const UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAttachedRawSeq;
    if (CapInstance >= AMBA_DSP_MAX_DATACAP_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pCapBuf == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pCapBuf->BufNum > 1U) { //SSP only support OneFrm update now
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if ((pCapBuf->AllocType != ALLOC_EXTERNAL_DISTINCT) &&
                   (pCapBuf->AllocType != ALLOC_EXTERNAL_CYCLIC)) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            if (pCapBuf->AllocType == ALLOC_EXTERNAL_CYCLIC) {
                if ((pCapBuf->BufAddr == 0x0U) &&
                    (pCapBuf->AuxBufAddr == 0x0U)) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
            } else {
                if ((pCapBuf->pBufTbl == NULL) &&
                    (pCapBuf->pAuxBufTbl == NULL)) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("UpdateCapBuffer Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_DataCapCtrl(const UINT16 NumCapInstance,
                                const UINT16 *pCapInstance,
                                const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl,
                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT16 i;

(void)pAttachedRawSeq;
    if (NumCapInstance > AMBA_DSP_MAX_DATACAP_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if ((pCapInstance == NULL) ||
               (pDataCapCtrl == NULL)) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (i=0U; i<NumCapInstance; i++) {
            if (pCapInstance[i] >= AMBA_DSP_MAX_DATACAP_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("DataCapCtrl Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn,
                                 const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                 const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                 const UINT32 Opt,
                                 const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)Opt;
(void)pAttachedRawSeq;
    if (pYuvIn == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pYuvOut == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pIsoCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (checkStruct_DspYuvImgBuffer(pYuvIn) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (checkStruct_DspYuvImgBuffer(pYuvOut) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            if (pIsoCfg->CfgAddress == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillYuv2Yuv Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_CalcStillYuvExtBufSize(const UINT16 StreamIdx, const UINT16 BufType, const UINT16 *pBufPitch, const UINT32 *pBufUnitSize)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pBufPitch;
(void)pBufUnitSize;
    if (StreamIdx >= AMBA_DSP_MAX_YUVSTRM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (BufType >= DSP_DATACAP_BUFTYPE_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        //TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("CalcStillYuvExtBufSize Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                 const AMBA_DSP_BUF_s *pAuxBufIn,
                                 const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                                 const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                 const UINT32 Opt,
                                 const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAuxBufIn;
(void)Opt;
(void)pAttachedRawSeq;
    if (pRawIn == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pYuvOut == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pIsoCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (checkStruct_DspRawBuffer(pRawIn) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (checkStruct_DspYuvImgBuffer(pYuvOut) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            if (pIsoCfg->CfgAddress == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Yuv Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                                 const AMBA_DSP_BUF_s *pAuxBufIn,
                                 const AMBA_DSP_RAW_BUF_s *pRawOut,
                                 const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                                 const UINT32 Opt,
                                 const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAuxBufIn;
(void)Opt;
(void)pAttachedRawSeq;
    if (pRawIn == NULL){
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pIsoCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (checkStruct_DspRawBuffer(pRawIn) != OK) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (Opt >= STL_R2R_OPT_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (Opt != STL_R2R_OPT_GEN_AAA) {
            if (checkStruct_DspRawBuffer(pRawOut) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        } else {
            if (pIsoCfg->CfgAddress == 0U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillRaw2Raw Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

/* VideoEncode */
static UINT32 check_VideoEncConfig(const UINT16 NumStream,
                                   const UINT16 *pStreamIdx,
                                   const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pStreamConfig == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_EncStrmCfg(&(pStreamConfig[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncConfig Check Error[0x%X][%d]",
                Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoEncStart(const UINT16 NumStream,
                                  const UINT16 *pStreamIdx,
                                  const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig,
                                  const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

(void)pAttachedRawSeq;
    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pStartConfig == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//         Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_EncStartCfg(&(pStartConfig[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStart Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoEncStop(const UINT16 NumStream,
                                 const UINT16 *pStreamIdx,
                                 const UINT8 *pStopOption,
                                 const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

(void)pAttachedRawSeq;
    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pStopOption == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//         Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (pStopOption[Idx] > AMBA_DSP_VIDEO_ENC_STOP_NEXT_IDR) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncStop Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoEncControlFrameRate(const UINT16 NumStream,
                                             const UINT16 *pStreamIdx,
                                             const UINT32 *pDivisor,
                                             const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

(void)pAttachedRawSeq;
    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pDivisor == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//         Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (pDivisor[Idx] > 65535U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlFrate Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoEncControlRepeatDrop(const UINT16 NumStream,
                                              const UINT16* pStreamIdx,
                                              const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg,
                                              const UINT64* pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

(void)pAttachedRawSeq;
    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//         Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_EncRepDropCfg(&(pRepeatDropCfg[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlRepeatDrop Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoEncControlQuality(const UINT16 NumStream,
                                           const UINT16 *pStreamIdx,
                                           const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pQCtrl == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_EncQt(&(pQCtrl[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncCtrlQt Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 checkStruct_VdoBldCfg(const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pBlendCfg->BlendAreaIdx >= DSP_MAX_OSD_BLEND_AREA_NUMBER) {
        if (pBlendCfg->BlendAreaIdx != AMBA_DSP_OSD_BLEND_AREA_EMBED_CODE) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (pBlendCfg->Enable > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pBlendCfg->EnableCLUT > 0U) { // Not support in MaxRevision when Rev_283573
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pBlendCfg->BlendYuvBuf.DataFmt != AMBA_DSP_YUV420) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (pBlendCfg->AlphaBuf.DataFmt != AMBA_DSP_YUV420) { //TBD, followed VprocOutput
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if ((pBlendCfg->BlendYuvBuf.BaseAddrUV == 0U)) {  // AlphaUv is DONTCARE, use Zero to follow AlphaY
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if ((checkStruct_DspYuvImgBuffer(&(pBlendCfg->BlendYuvBuf)) != OK)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if ((checkStruct_DspYuvImgBuffer(&(pBlendCfg->AlphaBuf)) != OK)) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("checkStruct_VdoBldCfg Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_VideoEncControlBlend(const UINT16 NumStream,
                                         const UINT16 *pStreamIdx,
                                         const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    UINT32 Rval = OK;
    UINT32 i, ErrLine = 0U;

    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else if (pBlendCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (i=0U; i<NumStream; i++) {
            if (pStreamIdx[i] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            } else {
                Rval = checkStruct_VdoBldCfg(&pBlendCfg[i]);
                if (Rval != OK) {
                    Rval = DSP_ERR_0001; ErrLine = __LINE__;
                }
            }

            if (ErrLine != 0U) {
                break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncControlBlend Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_VideoEncExecIntervalCap(const UINT16 NumStream,
                                            const UINT16 *pStreamIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncExecIntCap Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoEncFeedYuvData(const UINT16 NumStream,
                                        const UINT16 *pStreamIdx,
                                        const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx = 0U;

    if (NumStream > AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_LiveviewExtYuv(&(pExtYuvBuf[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncFeedYuvData Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (pGrpCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        if (pGrpCfg->GrpIdx > AMBA_DSP_MAX_ENC_GRP_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pGrpCfg->Purpose != 0x1U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pGrpCfg->NumStrm > AMBA_DSP_MAX_ENC_GRP_STRM_NUM) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pGrpCfg->pStrmIdx == NULL) {
            Rval = DSP_ERR_0000; ErrLine = __LINE__;
        } else {
            //TBD
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncGrpConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

#ifdef SUPPORT_DSP_MV_DUMP
static UINT32 check_VideoEncMvConfig(const UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Fmtype = 0U;

    if (StreamIdx >= AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (pMvCfg == NULL) {
        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        Fmtype = (UINT8)pMvCfg->Option;

        if (pMvCfg->BufAddr == 0x0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pMvCfg->BufSize == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (pMvCfg->UnitSize == 0U) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else if (Fmtype != AMBA_DSP_ENC_MV_FMT_HEVC) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        } else {
            //TBD
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncMvConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}
#endif

static UINT32 check_VideoEncDescFmtConfig(const UINT16 StreamIdx, const UINT16  CatIdx, const UINT32 OptVal)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (StreamIdx >= AMBA_DSP_MAX_STREAM_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (CatIdx >= DSP_ENC_DESC_CAT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        if (OptVal > DSP_ENC_CAPTS_OPT_SEQ_TIME) {
            Rval = DSP_ERR_0001; ErrLine = __LINE__;
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoEncDescFmtConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/* Vout */
static UINT32 check_VoutReset(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutReset Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutVideoConfig(const UINT8 VoutIdx,
                                    const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (checkStruct_VoutVdoCfg(pConfig) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        //TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutVideoConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutVideoCtrl(const UINT8 VoutIdx,
                                  const UINT8 Enable,
                                  const UINT8 SyncWithVin,
                                  const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAttachedRawSeq;
    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (Enable > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (SyncWithVin > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        //TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutVideoCtrl Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutOsdConfigBuf(const UINT8 VoutIdx,
                                     const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_VoutOsdBufferCfg(pBufConfig) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutOsdConfigBuffer Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutOsdCtrl(const UINT8 VoutIdx,
                                const UINT8 Enable,
                                const UINT8 SyncWithVin,
                                const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pAttachedRawSeq;
    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (Enable > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else if (SyncWithVin > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    } else if (IsValidAddrU64(pAttachedRawSeq) != 1U) { // for user dont want to know it
//        Rval = DSP_ERR_0000; ErrLine = __LINE__;
    } else {
        //TBD
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutOsdCtrl Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutDisplayConfig(const UINT8 VoutIdx,
                                      const AMBA_DSP_DISPLAY_CONFIG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_VoutDispCfg(pConfig) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutDisplayCtrl(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayCtrl Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutMixerConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (checkStruct_VoutMixerCfg(pConfig) != OK) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutMixerConfigBackColor(const UINT8 VoutIdx, const UINT32 BackColorYUV)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (BackColorYUV >= 0xFF000000U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfigBackColor Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutMixerConfigHighlightColor(const UINT8 VoutIdx,
                                                  const UINT8 LumaThreshold,
                                                  const UINT32 HighlightColorYUV)
{
    UINT32 Rval = OK, ErrLine = 0U;

    //FIXME MISRA
    (void)LumaThreshold;
    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
//    if (LumaThreshold > 255U) {
//        Rval = DSP_ERR_0001; ErrLine = __LINE__;
//    }
    if (HighlightColorYUV >= 0xFF000000U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfigHighlightColor Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutMixerConfigCsc(const UINT8 VoutIdx, const UINT8 CscCtrl)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (CscCtrl > MIXER_CSC_FOR_OSD) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerConfigCsc Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutMixerCscMatrixConfig(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix)
{
    UINT32 Rval = OK, ErrLine = 0U;

(void)pCscMatrix;
    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerCscMatrixConfig Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutMixerCtrl(const UINT8 VoutIdx)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx >= AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutMixerCtrl Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutDisplayConfigGamma(const UINT8 VoutIdx, ULONG TableAddr)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx != VOUT_IDX_A) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (IsValidULAddr(TableAddr) != 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayConfigGamma Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutDisplayControlGamma(const UINT8 VoutIdx, const UINT8 Enable)
{
    UINT32 Rval = OK, ErrLine = 0U;

    if (VoutIdx != VOUT_IDX_A) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Enable > 1U) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutDisplayControlGamma Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VoutConfigMixerBinding(const UINT8 NumVout,
                                           const UINT8 *pVoutIdx,
                                           const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig)
{
    UINT32 Rval = OK, ErrLine = 0U;
    UINT8 Idx;

    if (NumVout > AMBA_DSP_MAX_VOUT_NUM) {
        Rval = DSP_ERR_0001; ErrLine = __LINE__;
    } else {
        for (Idx=0U; Idx<NumVout; Idx++) {
            if (pVoutIdx[Idx] >= AMBA_DSP_MAX_VOUT_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_VoutDataPathCfg(&(pConfig[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VoutConfigMixerBinding Check Error[0x%X][%d]", Rval, ErrLine, 0U, 0U, 0U);
    }
    return Rval;
}

/* StillDecode */
static UINT32 check_StillDecStart(const UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;

    if (StreamIdx > 1U) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (pDecConfig == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else {
        if (pDecConfig->BitsFormat != AMBA_DSP_DEC_BITS_FORMAT_H264) {
            ErrLine = __LINE__; Rval = DSP_ERR_0001;
        } else if ((pDecConfig->BitsAddr == 0U) ||
                   (pDecConfig->BitsSize == 0U)) {
            ErrLine = __LINE__; Rval = DSP_ERR_0001;
        } else if ((pDecConfig->YuvBufAddr == 0U) ||
                   (pDecConfig->YuvBufSize == 0U)) {
            ErrLine = __LINE__; Rval = DSP_ERR_0001;
        } else {
            // Pass!
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("StillDecStart param chk NG at %d", ErrLine, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

/* VideoDecode */
static UINT32 check_VideoDecConfig(const UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;
    UINT8 Idx;

    if (MaxNumStream == 0U) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (MaxNumStream > AMBA_DSP_MAX_DEC_STREAM_NUM) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (pStreamConfig == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else {
        for (Idx = 0; Idx<MaxNumStream; Idx++) {
            if (checkStruct_DecCfg(&(pStreamConfig[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__; break;
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecConfig param chk NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 check_VideoDecStart(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;
    UINT8 Idx;

    if (NumStream == 0U) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (NumStream > AMBA_DSP_MAX_DEC_STREAM_NUM) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (pStreamIdx == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else if (pStartConfig == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else {
        for (Idx = 0; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_DEC_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_DecStart(&(pStartConfig[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecStart param chk NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_VideoDecBitsFifoUpdate(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;
    UINT8 Idx;

    if (NumStream == 0U) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (NumStream > AMBA_DSP_MAX_DEC_STREAM_NUM) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (pStreamIdx == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else if (pBitsFifo == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else {
        for (Idx = 0; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_DEC_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_DecBitsInfo(&(pBitsFifo[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecBitsFifoUpdate param chk NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_VideoDecStop(const UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;
    UINT8 Idx;

    if (NumStream == 0U) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (NumStream > AMBA_DSP_MAX_DEC_STREAM_NUM) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (pStreamIdx == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else if (pShowLastFrame == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else {
        for (Idx = 0; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_DEC_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (pShowLastFrame[Idx] > 1U) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecStop param chk NG at %d",ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 check_VideoDecTrickPlay(const UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    UINT32 Rval = OK;
    UINT32 ErrLine = 0U;
    UINT8 Idx;

    if ((NumStream == 0U) || (NumStream > AMBA_DSP_MAX_DEC_STREAM_NUM)) {
        ErrLine = __LINE__; Rval = DSP_ERR_0001;
    } else if (pStreamIdx == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else if (pTrickPlay == NULL) {
        ErrLine = __LINE__; Rval = DSP_ERR_0000;
    } else {
        for (Idx = 0; Idx<NumStream; Idx++) {
            if (pStreamIdx[Idx] >= AMBA_DSP_MAX_DEC_STREAM_NUM) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (checkStruct_DecTrickPlay(&(pTrickPlay[Idx])) != OK) {
                Rval = DSP_ERR_0001; ErrLine = __LINE__;
            }
            if (ErrLine != 0U) {
                break;
            }
        }
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("VideoDecTrickPlay param chk NG at %d", ErrLine, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

void CheckApiReg(const UINT8 Disable, const UINT32 Mask, const UINT8 IsAutoInit)
{
    static UINT8 IsUserSetup = 0U;
    static Dsp_CheckApi_t DspApiCheckFunc = {
        /* Main */
        .pMainInit = check_MainInit,
        .pMainGetDefaultSysCfg = check_MainGetDefaultSysCfg,
        .pMainSetWorkArea = check_MainSetWorkArea,
        .pMainGetDspVerInfo = check_MainGetDspVerInfo,
        .pMainMsgParseEntry = check_MainMsgParseEntry,
        .pMainWaitVinInterrupt = check_MainWaitVinInterrupt,
        .pMainWaitVoutInterrupt = check_MainWaitVoutInterrupt,
        .pMainWaitFlag = check_MainWaitFlag,
        .pMainResourceLimit = check_MainResourceLimit,
        .pGetCmprRawBufInfo = check_GetCmprRawBufInfo,
        .pParLoadConfig = check_ParLoadConfig,
        .pParLoadRegionUnlock = check_ParLoadRegionUnlock,
        .pCalcEncMvBufInfo = check_CalcEncMvBufInfo,
        .pTestFrameConfig = check_TestFrameConfig,
        .pTestFrameCtrl = check_TestFrameCtrl,
        .pMainGetBufInfo = check_MainGetBufInfo,
        .pMainSafetyChk = check_MainSafetyChk,
        .pMainSafetyCfg = check_MainSafetyCfg,

        /* Liveview */
        .pLiveviewConfig = check_LiveviewConfig,
        .pLiveviewCtrl = check_LiveviewCtrl,
        .pLiveviewUpdateConfig = check_LiveviewUpdateConfig,
        .pLiveviewUpdateIsoConfig = check_LiveviewUpdateIsoCfg,
        .pLiveviewUpdateVinCfg = check_LiveviewUpdateVinCfg,
        .pLiveviewUpdatePymdCfg = check_LiveviewUpdatePymdCfg,
        .pLiveviewConfigVinCapture = check_LiveviewConfigVinCapture,
        .pLiveviewConfigVinPost = check_LiveviewConfigVinPost,
        .pLiveviewFeedRawData = check_LiveviewFeedRawData,
        .pLiveviewFeedYuvData = check_LiveviewFeedYuvData,
        .pLiveviewGetIDspCfg = check_LiveviewGetIDspCfg,
        .pLiveviewSidebandUpdate = check_LiveviewSidebandUpdate,
        .pLiveviewYuvStreamSync = check_LiveviewYuvStreamSync,
        .pLiveviewUpdateGeoCfg = check_LiveviewUpdateGeoCfg,
        .pLiveviewUpdateVZSrcCfg = check_LiveviewUpdateVZSrcCfg,
        .pLiveviewSliceCfg = check_LiveviewSliceCfg,
        .pLiveviewVZPostponeCfg = check_LiveviewVZPostponeCfg,
        .pLiveviewUpdateVinState = check_LiveviewUpdateVinState,

        /* StillCapture */
        .pDataCapCfg = check_DataCapCfg,
        .pUpdateCapBuffer = check_UpdateCapBuffer,
        .pDataCapCtrl = check_DataCapCtrl,
        .pStillYuv2Yuv = check_StillYuv2Yuv,
        .pCalcStillYuvExtBufSize = check_CalcStillYuvExtBufSize,
        .pStillRaw2Yuv = check_StillRaw2Yuv,
        .pStillRaw2Raw = check_StillRaw2Raw,

        /* VideoEncode */
        .pVideoEncConfig = check_VideoEncConfig,
        .pVideoEncStart = check_VideoEncStart,
        .pVideoEncStop = check_VideoEncStop,
        .pVideoEncControlFrameRate = check_VideoEncControlFrameRate,
        .pVideoEncControlRepeatDrop = check_VideoEncControlRepeatDrop,
        .pVideoEncControlQuality = check_VideoEncControlQuality,
        .pVideoEncControlBlend = check_VideoEncControlBlend,
        .pVideoEncExecIntervalCap = check_VideoEncExecIntervalCap,
        .pVideoEncFeedYuvData = check_VideoEncFeedYuvData,
        .pVideoEncGrpConfig = check_VideoEncGrpConfig,
#ifdef SUPPORT_DSP_MV_DUMP
        .pVideoEncMvConfig = check_VideoEncMvConfig,
#endif
        .pVideoEncDescFmtConfig = check_VideoEncDescFmtConfig,

        /* Vout */
        .pVoutReset = check_VoutReset,
        .pVoutVideoConfig = check_VoutVideoConfig,
        .pVoutVideoCtrl = check_VoutVideoCtrl,
        .pVoutOsdConfigBuf = check_VoutOsdConfigBuf,
        .pVoutOsdCtrl = check_VoutOsdCtrl,
        .pVoutDisplayConfig = check_VoutDisplayConfig,
        .pVoutDisplayCtrl = check_VoutDisplayCtrl,
        .pVoutMixerConfig = check_VoutMixerConfig,
        .pVoutMixerConfigBackColor = check_VoutMixerConfigBackColor,
        .pVoutMixerConfigHighlightColor = check_VoutMixerConfigHighlightColor,
        .pVoutMixerConfigCsc = check_VoutMixerConfigCsc,
        .pVoutMixerCscMatrixConfig = check_VoutMixerCscMatrixConfig,
        .pVoutMixerCtrl = check_VoutMixerCtrl,
        .pVoutDisplayConfigGamma = check_VoutDisplayConfigGamma,
        .pVoutDisplayControlGamma = check_VoutDisplayControlGamma,
        .pVoutConfigMixerBinding = check_VoutConfigMixerBinding,

        /* StillDecode */
        .pStillDecStart = check_StillDecStart,

        /* VideoDecode */
        .pVideoDecConfig = check_VideoDecConfig,
        .pVideoDecStart = check_VideoDecStart,
        .pVideoDecBitsFifoUpdate = check_VideoDecBitsFifoUpdate,
        .pVideoDecStop = check_VideoDecStop,
        .pVideoDecTrickPlay = check_VideoDecTrickPlay,

    };
//FIXME, Misra
    (void)Mask;

    if (IsAutoInit == 0U) {
        IsUserSetup = 1U;
        if (Disable == 0U) {
            pDspApiCheckFunc = &DspApiCheckFunc;
        } else {
            pDspApiCheckFunc = NULL;
        }
    } else if (IsUserSetup == 0U) {
        pDspApiCheckFunc = &DspApiCheckFunc;
    } else {
        //
    }
}

Dsp_CheckApi_t* AmbaDSP_GetCheckApiFunc(void){
    return pDspApiCheckFunc;
}
