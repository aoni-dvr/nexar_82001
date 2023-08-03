/**
 *  @file SvcStill.c
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
 *  @details svc application command functions
 *
 */
 
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSvcGdma.h"

#include "AmbaDSP_EventInfo.h"

#include "SvcStill.h" 
#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcPlat.h"

SVC_STILL_CALLBACK_f pSvcLivStop = NULL;
SVC_STILL_CALLBACK_f pSvcLivStart = NULL;
SVC_STILL_DATAREADY_f pSvDataReady = NULL;

/**
* register still callback functions for stop and restart liveview
* @param [in] pCallback callback function
* @return none
*/
void SvcStill_RegisterCallback(const SVC_STILL_CALLBACK_s *pCallback)
{
    pSvcLivStop  = pCallback->pLivStop;
    pSvcLivStart = pCallback->pLivStart;
}

/**
* register still data ready handler
* @param [in] pHandler callback handler
* @return none
*/
void SvcStill_RegisterDataReady(SVC_STILL_DATAREADY_f pHandler)
{
    pSvDataReady  = pHandler;
}

/**
* tune JPEG Q-Table by Quality
* @param [in] pQTable Q-Table
* @param [in] Quality 1 ~ 100
* @return none
*/
void SvcStill_CalJpegDqt(UINT8 *pQTable, INT32 Quality)
{
    UINT32 Cnt, Scale;
    ULONG TempL;
    const UINT8 StdJpegQTable[SIZE_JPEG_Q_TABLE] = {
        16, 11, 10, 16, 124, 140, 151, 161,
        12, 12, 14, 19, 126, 158, 160, 155,
        14, 13, 16, 24, 140, 157, 169, 156,
        14, 17, 22, 29, 151, 187, 180, 162,
        18, 22, 37, 56, 168, 109, 103, 177,
        24, 35, 55, 64, 181, 104, 113, 192,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99 ,
        17, 18, 24, 47, 99 , 99 , 99 , 99 ,
        18, 21, 26, 66, 99 , 99 , 99 , 99 ,
        24, 26, 56, 99, 99 , 99 , 99 , 99 ,
        47, 66, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99 ,
        99, 99, 99, 99, 99 , 99 , 99 , 99
    };

    /** for jpeg brc; return the quantization table*/
    if(Quality == -1) {
        UINT32 Rval;
        Rval = AmbaWrap_memcpy(pQTable, StdJpegQTable, SIZE_JPEG_Q_TABLE);
        if (Rval != OK) {
            SvcLog_NG("SvcStill_CalJpegDqt", "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
        }
    } else {
        if (Quality <= 0) {
            Scale = 5000;
        } else if (Quality >= 100) {
            Scale = 0;
        } else if (Quality < 50){
            Scale = (5000U / (UINT32)Quality);
        } else {
            Scale = (200U - ((UINT32)Quality * 2U));
        }
        for (Cnt = 0; Cnt < SIZE_JPEG_Q_TABLE; Cnt ++) {
            if ((StdJpegQTable[Cnt] < 200U) && (Scale <= 5000U)) {
                TempL = (((ULONG)StdJpegQTable[Cnt] * Scale) + 50UL) / 100UL;
                /* limit the values to the valid range */
                if (TempL == 0UL) {
                    pQTable[Cnt] = 1;
                } else {
                    if (TempL > 255UL) {
                        /* max quantizer needed for baseline */
                        pQTable[Cnt] = 255;
                    } else {
                        pQTable[Cnt] = (UINT8)TempL;
                    }
                }
            }
        }
    }
}

/**
* structure transfer from SVC_YUV_IMG_BUF_s to AMBA_DSP_YUV_IMG_BUF_s
* @param [in] pSvcYuv structure of application layer
* @param [in] pSspYuv structure of dsp application
* @return none
*/
void SvcStill_SvcYuv2SspYuv(const SVC_YUV_IMG_BUF_s *pSvcYuv, AMBA_DSP_YUV_IMG_BUF_s *pSspYuv)
{
    pSspYuv->DataFmt = pSvcYuv->DataFmt;   
    pSspYuv->BaseAddrY = pSvcYuv->BaseAddrY; 
    pSspYuv->BaseAddrUV = pSvcYuv->BaseAddrUV;
    pSspYuv->Pitch = pSvcYuv->Pitch;     
    pSspYuv->Window.Width = pSvcYuv->Width;    
    pSspYuv->Window.Height = pSvcYuv->Height;    
}

UINT32 SvcStillBufCopy(UINT8 *pDst, UINT8 *pSrc, UINT32 Size)
{
    UINT32  Rval;
#if defined(CONFIG_AMBA_REC_GDMA_USED) && !defined(CONFIG_SOC_CV28)
    ULONG   SrcAddr;
    AmbaMisra_TouchUnused(&pSrc);
    AmbaMisra_TypeCast(&SrcAddr, &pSrc);
    if (0U != SvcPlat_CacheClean(SrcAddr, (ULONG)Size)) {
        // SvcStillLog("[SvcStillBufCopy] proc SrcAddr %p cache clean fail. if memory is non-cahce, ignore it.", SrcAddr, 0, 0, 0, 0);
    }

    Rval = AmbaSvcGdma_Copy(pDst, pSrc, Size);
    if (OK != Rval) {
        SvcLog_NG("SvcStillBufCopy", "[line %u] AmbaSvcGdma_Copy return 0x%x", __LINE__, Rval);
    } else {
        ULONG   DstAddr;
        AmbaMisra_TypeCast(&DstAddr, &pDst);
        if (0U != SvcPlat_CacheInvalidate(DstAddr, (ULONG)Size)) {
            // SvcStillLog("[SvcStillBufCopy] proc DstAddr %p cache invalid fail. if memory is non-cahce, ignore it.", DstAddr, 0, 0, 0, 0);
        }
    }
#else
    AmbaMisra_TouchUnused(&pSrc);
    Rval = AmbaWrap_memcpy(pDst, pSrc, Size);
    if (OK != Rval) {
        SvcLog_NG("SvcStillBufCopy", "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
    }
#endif

    return Rval;
}

static void SvcWrapPrnFunc(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((pModule != NULL) && (pFormat != NULL)) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, 0U, 0U, 0U);
    }
}

void SvcStillLog(const char *pFmt, ULONG x1, ULONG x2, ULONG x3, ULONG x4, ULONG x5)
{
#define DBG_PRN_NATIVE      (0U)

#if DBG_PRN_NATIVE
    printf(pFmt, x1, x2, x3, x4, x5);
    printf("\n");
#else
    SVC_WRAP_PRINT_s SvcWrapPrn;

    if (AmbaWrap_memset(&SvcWrapPrn, 0, sizeof(SvcWrapPrn)) == OK) {
        SvcWrapPrn.pProc = SvcWrapPrnFunc;
        SvcWrapPrn.pStrFmt = pFmt;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x1; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x2; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x3; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x4; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x5; SvcWrapPrn.Argc ++;

        SvcWrap_Print("", &SvcWrapPrn);
    }
#endif
}
