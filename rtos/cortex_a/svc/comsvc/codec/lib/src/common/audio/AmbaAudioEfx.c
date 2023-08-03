/**
*  @file AmbaAudioEfx.c
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
*  @details audio effect related API.
*
*/
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaAudioEfx.h"

/*! 32xQ16 multiplication */
static UINT32 Efx_Mpy32xQ16 (UINT32 X, UINT32 Y)
{
    UINT64 Result;
    UINT32 Sign = 0U, Tmp_32;

    if (X >= 0x80000000U) {
        Sign = 1U;
        Tmp_32 = ~X;
        Result = (UINT64)Tmp_32 + 1U;
    } else {
        Result = (UINT64)X;
    }

    Result = (Result * (UINT64)Y) >> 16U;
    if (Sign == 1U) {
        Result = (~Result) + 1U;
    }

    return (UINT32)Result;
}

/*! 16xQ16 multiplication */
static UINT16 Efx_Mpy16xQ16 (UINT16 X, UINT32 Y)
{
    UINT64 Result;
    UINT16 Tmp_16;
    UINT32 Sign = 0U;

    if (X >= 0x8000U) {
        Sign = 1U;
        Tmp_16 = ~X;
        Result = (UINT64)Tmp_16 + 1U;
    } else {
        Result = (UINT64)X;
    }

    Result = (Result * (UINT64)Y) >> 16U;
    if (Sign == 1U) {
        Result = (~Result) + 1U;
    }

    return (UINT16)Result;
}

UINT32 AmbaAudioBufEfx_VolumeProc(const AMBA_AENC_FLOW_EFX_VOLUME_s *pVolume, UINT32 *pSrc)
{
    UINT16  *pData_16;
    UINT32  *pData = pSrc;
    UINT32  Loop1, Loop2;
    UINT32  RetVal = AEFX_OK;
    static const UINT32 SSPUT_AudioEfx_VolumeTable[AMBA_AENC_EFX_VOLUME_LEVLE + 1U] = {
        0x00000000,
        0x00000010, 0x00000013, 0x00000015, 0x00000018, 0x0000001C, 0x00000020, 0x00000024, 0x00000029,
        0x0000002F, 0x00000036, 0x0000003D, 0x00000046, 0x00000050, 0x0000005B, 0x00000068, 0x00000076,
        0x00000087, 0x0000009A, 0x000000B0, 0x000000C9, 0x000000E5, 0x00000105, 0x0000012A, 0x00000153,
        0x00000183, 0x000001BA, 0x000001F8, 0x0000023F, 0x0000028F, 0x000002EC, 0x00000355, 0x000003CD,
        0x00000455, 0x000004F1, 0x000005A3, 0x0000066E, 0x00000756, 0x0000085E, 0x0000098B, 0x00000AE2,
        0x00000C6A, 0x00000E29, 0x00001027, 0x0000126D, 0x00001504, 0x000017F8, 0x00001B57, 0x00001F30,
        0x00002392, 0x00002893, 0x00002E47, 0x000034C9, 0x00003C36, 0x000044AD, 0x00004E56, 0x0000595A,
        0x000065EA, 0x0000743F, 0x00008498, 0x0000973E, 0x0000AC82, 0x0000C4C5, 0x0000E070, 0x00010000,
        0x00012400, 0x00014D10, 0x00017BE6, 0x0001B153, 0x0001EE42, 0x000233C4, 0x0002830B, 0x0002DD79,
        0x0003449D, 0x0003BA44, 0x00044075, 0x0004D986, 0x0005881C, 0x00064F40, 0x00073265, 0x0008357B,
        0x00095D01, 0x000AAE15, 0x000C2E90, 0x000DE51C, 0x000FD954, 0x001213E3, 0x00149EAF, 0x00178500,
        0x001AD3B4, 0x001E9979, 0x0022E70D, 0x0027CF8B, 0x002D68BB, 0x0033CB75, 0x003B1411, 0x004362E3,
        0x004CDCCA, 0x0057ABD6, 0x00640000, 0x00721000, 0x00821A3F, 0x009465EF, 0x00A94444, 0x00C111DD,
        0x00DC385F, 0x00FB304B, 0x011E8314, 0x0146CD82, 0x0174C266, 0x01A92DBB, 0x01E4F827, 0x02292B0A,
        0x0276F515, 0x02CFAF88, 0x0334E434, 0x03A85447, 0x042C001C, 0x04C2301B, 0x056D7ED9, 0x0630E4A8,
        0x070FC4C8, 0x080DFC6B, 0x092FF3E0, 0x0A7AB220, 0x0BF3F31F, 0x0DA24140, 0x0F8D125B,
    };

    if (pVolume->Res == 32U) {
        for (Loop1 = 0U; Loop1 < pVolume->FrameSize; Loop1++) {
            for (Loop2 = 0U; Loop2 < pVolume->Ch; Loop2++) {
                *pData = Efx_Mpy32xQ16(*pData, SSPUT_AudioEfx_VolumeTable[pVolume->Level]);
                pData = &pData[1];
            }
        }
    } else {
        if (OK != AmbaWrap_memcpy(&pData_16, &(pSrc), sizeof(UINT16 *))) {/* pData_16 = (UINT16 *)pSrc; */
            AmbaPrint_PrintStr5("Func: [%s] AmbaWrap_memcpy pData_16 fail",__func__, NULL, NULL, NULL, NULL);
        }
        for (Loop1 = 0U; Loop1 < pVolume->FrameSize; Loop1++) {
            for (Loop2 = 0U; Loop2 < pVolume->Ch; Loop2++) {
                *pData_16 = Efx_Mpy16xQ16(*pData_16, SSPUT_AudioEfx_VolumeTable[pVolume->Level]);
                pData_16 = &pData_16[1];
            }
        }
    }

    return RetVal;
}

/*! 32x32 multiplication, Y is always positive */
static UINT32 AU_Mpy32x32 (UINT32 X, UINT32 Y)
{
    UINT64 Result;
    UINT32 Sign = 0U, Tmp_32;

    if (X >= 0x80000000U) {
        Sign = 1U;
        Tmp_32 = ~X;
        Result = (UINT64)Tmp_32 + 1U;
    } else {
        Result = (UINT64)X;
    }

    Result = (Result * (UINT64)Y) >> 32U;
    Result = ((Result + Result) & (UINT64)0x00000000ffffffffUL);

    if (Sign == 1U) {
        Result = (~Result) + 1U;
    }
    return (UINT32)Result;
}

/*! 32-bit add with saturation */
static UINT32 AU_Sadd(UINT32 Data1, UINT32 Data2)
{
    UINT32 Data3;
    INT32 DataS1, DataS2, DataS3;
    INT64 Sum;

    AmbaMisra_TypeCast32(&DataS1, &Data1);
    AmbaMisra_TypeCast32(&DataS2, &Data2);

    Sum = (INT64)DataS1 + (INT64)DataS2;

    if(Sum > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x7fffffff;
    } else if(-Sum > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x80000000U;
    } else {
        DataS3 = (INT32)Sum;
    }
    AmbaMisra_TypeCast32(&Data3, &DataS3);

    return Data3;
}


/*! 32-bit sub with saturation */
static UINT32 AU_Ssub(UINT32 Data1, UINT32 Data2)
{
    UINT32 Data3;
    INT32 DataS1, DataS2, DataS3;
    INT64 Diff;

    AmbaMisra_TypeCast32(&DataS1, &Data1);
    AmbaMisra_TypeCast32(&DataS2, &Data2);

    Diff = (INT64)DataS1 - (INT64)DataS2;

    if(Diff > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x7fffffff;
    } else if(-Diff > (INT64)0x000000007fffffff) {//OVERFLOW++;
        DataS3 = (INT32)0x80000000U;
    } else {
        DataS3 = (INT32)Diff;
    }
    AmbaMisra_TypeCast32(&Data3, &DataS3);

    return Data3;
}

UINT32 AmbaAudioBufEfx_DcBlockerProc(AMBA_AENC_DCBLOCKER_s *pDcBlocker, UINT32 *pSrc)
{
    UINT32        i,j;
    UINT32        *pResult;
    const UINT32  *pSignal;
    UINT16        *pSignal_16, *pResult_16;
    UINT32        ProcSize = pDcBlocker->FrameSize;
    UINT32        Channel  = pDcBlocker->Ch;
    UINT32        RetVal = AEFX_OK;
    UINT32        Tmp1, Tmp2, Input;

    pSignal = pSrc;
    pResult = pSrc;
    AmbaMisra_TypeCast(&pSignal_16, &pSrc);
    AmbaMisra_TypeCast(&pResult_16, &pSrc);

    if (pDcBlocker->Enable != 0U) {
        if (pDcBlocker->Res == 32U) {
            for (j = 0; j < ProcSize; j++) {
                for (i = 0; i < Channel; i++) {
                    /* y[n] = x[n] - x[n-1] + a * y[n-1] */
                    Input = *pSignal;
                    Tmp1 = AU_Mpy32x32(pDcBlocker->LastOutput[i], pDcBlocker->Alpha);
                    Tmp2 = AU_Ssub(Tmp1, pDcBlocker->LastInput[i]);
                    *pResult = AU_Sadd(Input, Tmp2);
                    /* AmbaPrint("0x%x = 0x%x - 0x%x + 0x%x * 0x%x", *pResult, Input, pDcBlockerSelf->LastInput[i], pDcBlockerSelf->Alpha, pDcBlockerSelf->LastOutput[i]); */
                    pDcBlocker->LastInput[i] = Input;
                    pDcBlocker->LastOutput[i] = *pResult;
                    pSignal++;
                    pResult++;
                }
            }
        } else {
            for (j = 0; j <ProcSize; j++) {
                for (i = 0; i < Channel; i++) {
                    /* y[n] = x[n] - x[n-1] + a * y[n-1] */
                    Input = (UINT32)((UINT32)(*pSignal_16) << 16U);
                    Tmp1 = AU_Mpy32x32(pDcBlocker->LastOutput[i], pDcBlocker->Alpha);
                    Tmp2 = AU_Ssub(Tmp1, pDcBlocker->LastInput[i]);
                    *pResult_16 = (UINT16)(AU_Sadd(Input, Tmp2) >> 16);
                    /* AmbaPrint("0x%x = 0x%x - 0x%x + 0x%x * 0x%x", *pResult_16, Input, pDcBlockerSelf->LastInput[i], pDcBlockerSelf->Alpha, pDcBlockerSelf->LastOutput[i]); */
                    pDcBlocker->LastInput[i] = Input;
                    pDcBlocker->LastOutput[i] = ((UINT32)(*pResult_16) << 16U);
                    pSignal_16++;
                    pResult_16++;
                }
            }

        }
    }

    return RetVal;
}

static void SSPUT_FC_16to16(const AMBA_AENC_FLOW_EFX_FC_s *pFc, UINT16 *pSrc, UINT16 *pDst)
{
    UINT32 Idx, J;
    INT32  K;
    const INT32 *pChannelAssignTable = pFc->ChAssignTable;
    UINT16 *pSrcTmp = pSrc;
    UINT16 *pDstTmp = pDst;

    for (Idx = 0; Idx < pFc->FrameSize; Idx++) {
        for (J = 0; J < pFc->DstCh; J++) {
            if (pChannelAssignTable[J] != -1) {
                for (K = 0; K < (INT32)pFc->SrcCh; K++) {
                    if (pChannelAssignTable[J] == K) {
                        break;
                    }
                }
                *pDstTmp = pSrcTmp[K];
                pDstTmp = &pDstTmp[1];
            }
        }
        pSrcTmp = &pSrcTmp[pFc->SrcCh];
    }
}

static void SSPUT_FC_16to32(const AMBA_AENC_FLOW_EFX_FC_s *pFc, UINT16 *pSrc, UINT32 *pDst)
{
    UINT32 Idx, J;
    INT32  K;
    const INT32 *pChannelAssignTable = pFc->ChAssignTable;
    UINT16 *pSrcTmp = pSrc;
    UINT32 *pDstTmp = pDst;

    for (Idx = 0; Idx < pFc->FrameSize; Idx++) {
        for (J = 0; J < pFc->DstCh; J++) {
            if (pChannelAssignTable[J] != -1) {
                for (K = 0; K < (INT32)pFc->SrcCh; K++) {
                    if (pChannelAssignTable[J] == K) {
                        break;
                    }
                }
                *pDstTmp = (UINT32)pSrcTmp[K];
                *pDstTmp <<= 16U;
                pDstTmp = &pDstTmp[1];
            }
        }
        pSrcTmp = &pSrcTmp[pFc->SrcCh];
    }
}

static void SSPUT_FC_32to16(const AMBA_AENC_FLOW_EFX_FC_s *pFc, UINT32 *pSrc, UINT16 *pDst)
{
    UINT32 Idx, J;
    INT32  K;
    const INT32 *pChannelAssignTable = pFc->ChAssignTable;
    UINT32 *pSrcTmp = pSrc;
    UINT16 *pDstTmp = pDst;

    for (Idx = 0; Idx < pFc->FrameSize; Idx++) {
        for (J = 0; J < pFc->DstCh; J++) {
            if (pChannelAssignTable[J] != -1) {
                for (K = 0; K < (INT32)pFc->SrcCh; K++) {
                    if (pChannelAssignTable[J] == K) {
                        break;
                    }
                }
                *pDstTmp = (UINT16)(pSrcTmp[K] >> 16U);
                pDstTmp = &pDstTmp[1];
            }
        }
        pSrcTmp = &pSrcTmp[pFc->SrcCh];
    }
}

static void SSPUT_FC_32to32(const AMBA_AENC_FLOW_EFX_FC_s *pFc, UINT32 *pSrc, UINT32 *pDst)
{
    UINT32 Idx, J;
    INT32  K;
    const INT32 *pChannelAssignTable = pFc->ChAssignTable;
    UINT32 *pSrcTmp = pSrc;
    UINT32 *pDstTmp = pDst;

    for (Idx = 0; Idx < pFc->FrameSize; Idx++) {
        for (J = 0; J < pFc->DstCh; J++) {
            if (pChannelAssignTable[J] != -1) {
                for (K = 0; K < (INT32)pFc->SrcCh; K++) {
                    if (pChannelAssignTable[J] == K) {
                        break;
                    }
                }
                *pDstTmp = pSrcTmp[K];
                pDstTmp = &pDstTmp[1];
            }
        }
        pSrcTmp = &pSrcTmp[pFc->SrcCh];
    }
}

UINT32 AmbaAudioBufEfx_FcProc(const AMBA_AENC_FLOW_EFX_FC_s *pFc, UINT32 *pSrc, UINT32 *pDst)
{
    UINT16  *pRp_16;
    UINT16  *pWp_16;
    UINT32  RetVal = AEFX_OK;

    AmbaMisra_TypeCast(&pRp_16, &(pSrc)); /* pRp_16 = (UINT16 *)pSrc; */
    AmbaMisra_TypeCast(&pWp_16, &(pDst)); /* pWp_16 = (UINT16 *)pDst; */

    if ((pFc->SrcRes == 16U) && (pFc->DstRes == 16U)) {
        SSPUT_FC_16to16(pFc, pRp_16, pWp_16);
    } else if ((pFc->SrcRes == 16U) && (pFc->DstRes == 32U)) {
        SSPUT_FC_16to32(pFc, pRp_16, pDst);
    } else if ((pFc->SrcRes == 32U) && (pFc->DstRes == 16U)) {
        SSPUT_FC_32to16(pFc, pSrc, pWp_16);
    } else if ((pFc->SrcRes == 32U) && (pFc->DstRes == 32U)) {
        SSPUT_FC_32to32(pFc, pSrc, pDst);
    } else {
        RetVal = AEFX_ERR_0000;
        AmbaPrint_PrintUInt5("Audio FC effect wrong resolution setting  SrcRes: %d  DstRes: %d", pFc->SrcRes, pFc->DstRes, 0U, 0U, 0U);
    }

    return RetVal;
}

UINT32 AmbaAudioBufEfx_DwsProc(const AMBA_AENC_FLOW_EFX_DWS_s *pDws, UINT32 *pSrc)
{
    UINT32 Factor;
    UINT32 RetVal = AEFX_OK;
    UINT32 Loop1, Loop2;
    UINT32 *pSrc_32, *pDst_32;
    UINT16 *pSrc_16, *pDst_16;

    Factor = (pDws->SrcFreq / pDws->DstFreq);
    if (pDws->Res == 32U) {
        pSrc_32 = pSrc;
        pDst_32 = pSrc;
        for (Loop1 = 0U; Loop1 < pDws->FrameSize; Loop1++) {
            for (Loop2 = 0U; Loop2 < pDws->Ch; Loop2++) {
                *pDst_32 = *pSrc_32;
                pDst_32 = &pDst_32[1];
                pSrc_32 = &pSrc_32[1];
            }
            pSrc_32 = &pSrc_32[(Factor - 1U ) * pDws->Ch];
        }
    } else {
        AmbaMisra_TypeCast(&(pSrc_16), &(pSrc)); /* pData_16 = (UINT16 *)pSrc; */
        AmbaMisra_TypeCast(&(pDst_16), &(pSrc)); /* pData_16 = (UINT16 *)pSrc; */
        for (Loop1 = 0U; Loop1 < pDws->FrameSize; Loop1++) {
            for (Loop2 = 0U; Loop2 < pDws->Ch; Loop2++) {
                *pDst_16 = *pSrc_16;
                pDst_16 = &pDst_16[1];
                pSrc_16 = &pSrc_16[1];
            }
            pSrc_16 = &pSrc_16[(Factor - 1U ) * pDws->Ch];
        }
    }

    return RetVal;
}
