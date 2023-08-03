/*
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
*/
#include "AmbaIntrinsics.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaShell.h"
#include "cvapi_visutil.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_protection.h"

static const AMBA_STEREO_FD_HANDLE_s* gHdlr02 = NULL;
static const AMBA_STEREO_FD_HANDLE_s* gHdlr024 = NULL;
static UINT32 gOutputNum02;
static UINT32 gOutputSz02[8];
static UINT32 gOutputNum024;
static UINT32 gOutputSz024[8];

static inline UINT32 SpuFusionUT_CacheAligned (UINT32 Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("SpuFusionUT_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}

static void SpuFusionUT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static const AMBA_STEREO_FD_HANDLE_s *CastStereoFd2Const(AMBA_STEREO_FD_HANDLE_s *pHdlr)
{
    const AMBA_STEREO_FD_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    SpuFusionUT_CheckRval(AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval)), "AmbaWrap_memcpy", __func__);
    return pRval;
}

static void SpuFusionUT_ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 U32Buf;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        AmbaMisra_TypeCast32(&U32Buf, &buf);
        Ret = AmbaCache_DataClean(U32Buf, Fsize);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void SpuFusionUT_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}


static UINT32 SpuFusionUT_LoadFlexiDagBin(const char* path, UINT8**BinBuf, UINT32* Size)
{

    flexidag_memblk_t FlexidagBinBuf;
    UINT8 *pU8 = NULL;
    UINT32 BinSize;
    UINT32 AlignedBinSize;
    UINT32 ret = 0U;
    UINT32 addr, Paddr;

    SpuFusionUT_GetFileSize(path, &BinSize);

    if (BinSize != 0U) {

        ret = RefCV_UT_GetCVBuf(&pU8, BinSize, &AlignedBinSize);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("SpuFusionUT_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            //assign memory
            FlexidagBinBuf.buffer_cacheable = 1;
            AmbaMisra_TypeCast32(&FlexidagBinBuf.pBuffer, &pU8);
            FlexidagBinBuf.buffer_size = AlignedBinSize;
            AmbaMisra_TypeCast32(&addr, &pU8);
            (void) AmbaMMU_Virt32ToPhys32(addr, &Paddr);
            FlexidagBinBuf.buffer_daddr = Paddr;
            ret = AmbaCV_UtilityFileLoad(path, &FlexidagBinBuf);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("SpuFusionUT_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *BinBuf = pU8;
            *Size = AlignedBinSize;
        }
    } else {
        AmbaPrint_PrintUInt5("SpuFusionUT_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    }
    return ret;
}

static void SpuFusionUT_ClearBuff(UINT8* pBuf, UINT32 BufSize)
{
    if ( BufSize != 0U ) {
        UINT32 BuffAddr;
        SpuFusionUT_CheckRval(AmbaWrap_memset(pBuf, 0, BufSize), "AmbaWrap_memset", __func__);
        AmbaMisra_TypeCast32(&BuffAddr, &pBuf);
        (void) AmbaCache_DataInvalidate(BuffAddr, BufSize);
    }
}

static void RefCV_AmbaSpuFusion_UT_Init(UINT32 LayerNum, const char *FlexiBinPath)
{
    UINT32 AlignSz = 0U;
    UINT32 Ret;
    UINT8* BinBuf = NULL;
    UINT32 BinSize = 0U;
    UINT8* StateBuf = NULL;
    UINT32 StateSize = 0U;
    UINT8* TempBuf = NULL;
    UINT32 TempSize = 0U;
    static UINT32 gInit02 = 0U;
    static UINT32 gInit024 = 0U;
    static AMBA_STEREO_FD_HANDLE_s Handler02 = {0};
    static AMBA_STEREO_FD_HANDLE_s Handler024 = {0};
    gHdlr02 = CastStereoFd2Const(&Handler02);
    gHdlr024 = CastStereoFd2Const(&Handler024);
    if ( LayerNum == 2U ) {
        if (gInit02 == 0U) {
            (void)SpuFusionUT_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
            Ret = AmbaSpuFusion_Open(BinBuf, BinSize, &StateSize, &TempSize, &gOutputNum02, gOutputSz02, gHdlr02);
            if (Ret == 0U) {
                AmbaPrint_PrintUInt5("OutputNum:%d", gOutputNum02, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_UT_GetCVBuf(&StateBuf, StateSize, &AlignSz);
                AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_UT_GetCVBuf(&TempBuf, TempSize, &AlignSz);
                AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
            }

            if (Ret == 0U) {
                SpuFusionUT_ClearBuff(StateBuf, StateSize);
                SpuFusionUT_ClearBuff(TempBuf, TempSize);
                (void)AmbaSpuFusion_Init(gHdlr02, StateBuf, TempBuf);
                //_Config_AmbaSpuFusion(Hdlr);
            }
            gInit02 = 1U;
        }
    } else if ( LayerNum == 3U ) {
        if (gInit024 == 0U) {
            (void)SpuFusionUT_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
            Ret = AmbaSpuFusion3Layer_Open(BinBuf, BinSize, &StateSize, &TempSize, &gOutputNum024, gOutputSz024, gHdlr024);
            if (Ret == 0U) {
                AmbaPrint_PrintUInt5("OutputNum:%d", gOutputNum024, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_UT_GetCVBuf(&StateBuf, StateSize, &AlignSz);
                AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_UT_GetCVBuf(&TempBuf, TempSize, &AlignSz);
                AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
            }

            if (Ret == 0U) {
                SpuFusionUT_ClearBuff(StateBuf, StateSize);
                SpuFusionUT_ClearBuff(TempBuf, TempSize);
                (void)AmbaSpuFusion_Init(gHdlr024, StateBuf, TempBuf);
                //_Config_AmbaSpuFusion(Hdlr);
            }
            gInit024 = 1U;
        }
    } else {
        AmbaPrint_PrintUInt5("LayerNum = %d is not yet to supported", LayerNum, 0U, 0U, 0U, 0U);
    }
}

static void SpuFusionUT_SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;


    Ret = AmbaFS_FileOpen(Fn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileWrite(Buf, 1U, Size, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileWrite fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void SpuFusionUT_DumpSpuResult(const AMBA_CV_SPU_DATA_s *pSpu, const char* OutputFn)
{
    UINT32 Fsize;
    UINT8 *pChar;
    UINT32 U32pSpu;
    UINT32 ValidScale = 0U;

#if 0
    // Dump all output info
    UINT32 ScaleIdx;
    AmbaPrint_PrintUInt5("pSpu->Reserved_0[0] = %d", pSpu->Reserved_0[0], 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pSpu->Reserved_0[1] = %d", pSpu->Reserved_0[1], 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pSpu->Reserved_0[2] = %d", pSpu->Reserved_0[2], 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pSpu->Reserved_0[3] = %d", pSpu->Reserved_0[3], 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pSpu->Reserved_0[4] = %d", pSpu->Reserved_0[4], 0U, 0U, 0U, 0U);
    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].Status = %d", ScaleIdx, (UINT32)pSpu->Scales[ScaleIdx].Status, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].BufSize = %d", ScaleIdx, pSpu->Scales[ScaleIdx].BufSize, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityHeight = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityHeight, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityWidth = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityWidth, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityPitch = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityPitch, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityBpp = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityBpp, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityQm = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityQm, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityQf = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityQf, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].Reserved_0 = %d", ScaleIdx, pSpu->Scales[ScaleIdx].Reserved_0, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].RoiStartRow = %d", ScaleIdx, pSpu->Scales[ScaleIdx].RoiStartRow, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].RoiStartCol = %d", ScaleIdx, pSpu->Scales[ScaleIdx].RoiStartCol, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].Reserved_1 = %d", ScaleIdx, pSpu->Scales[ScaleIdx].Reserved_1, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].RoiAbsoluteStartCol = %d", ScaleIdx, pSpu->Scales[ScaleIdx].RoiAbsoluteStartCol, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].RoiAbsoluteStartRow = %d", ScaleIdx, pSpu->Scales[ScaleIdx].RoiAbsoluteStartRow, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].InvalidDisparities = %d", ScaleIdx, pSpu->Scales[ScaleIdx].InvalidDisparities, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityMapOffset = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityMapOffset, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].Reserved_2 = %d", ScaleIdx, pSpu->Scales[ScaleIdx].Reserved_2, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("pSpu->Scales[%d].Reserved_3 = %d", ScaleIdx, pSpu->Scales[ScaleIdx].Reserved_3, 0U, 0U, 0U);
#else
    {
        // Dump partial output info
        UINT32 ScaleIdx;
        for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
            if ( pSpu->Scales[ScaleIdx].Status == 0 ) {
                AmbaPrint_PrintUInt5("Fusion Scale(%d) result", ScaleIdx, 0U, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityHeight = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityHeight, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityWidth = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityWidth, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityPitch = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityPitch, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityBpp = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityBpp, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityQm = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityQm, 0U, 0U, 0U);
                AmbaPrint_PrintUInt5("pSpu->Scales[%d].DisparityQf = %d", ScaleIdx, pSpu->Scales[ScaleIdx].DisparityQf, 0U, 0U, 0U);
                ValidScale = ScaleIdx;
            }
        }
    }
#endif
        Fsize = pSpu->Scales[ValidScale].BufSize;
        AmbaMisra_TypeCast32(&U32pSpu, &pSpu);
        U32pSpu += pSpu->Scales[ValidScale].DisparityMapOffset;
        AmbaMisra_TypeCast32(&pChar, &U32pSpu);
        SpuFusionUT_SaveBin(pChar, OutputFn, Fsize);
    }

    static void RefCV_AmbaSpuFusion_Run_2_layer(UINT8* const InputBuf[2U], const char* const InputFn[],
            const char* LogPath, const char* OutputFn)
    {
        static UINT32 OutputBufferAlloc02 = 0U;
        static UINT8* InAlign64Buf[2];
        static UINT8* pOutput02Buf[8];
        UINT32 rval = 0U;
        UINT32 i;
        UINT32 Disp2layer_0Width = 1920UL;
        UINT32 Disp2layer_0Height = 1080UL;
        UINT32 Disp2layer_0Pitch = 2400UL;
        UINT32 Disp2layer_2Width = 960UL;
        UINT32 Disp2layer_2Height = 540UL;
        UINT32 Disp2layer_2Pitch = 1216UL;

        AMBA_CV_SPU_DATA_s *pSpuIn = NULL;
        memio_source_recv_picinfo_t *pPicIn;
        void* vp;
        UINT32 U32InBuf;
        AMBA_CV_SPU_DATA_s *pInInfo0;
        memio_source_recv_picinfo_t *pInInfo1;
        UINT32 AlignedSize;
        AMBA_CV_SPU_DATA_s *pOutputSpu;

        // alloc output buffer
        if ( OutputBufferAlloc02 == 0U ) {
            for(i = 0U; i < gOutputNum02; ++i) {
                rval |= RefCV_UT_GetCVBuf(&pOutput02Buf[i], gOutputSz02[i], &AlignedSize);
                AmbaPrint_PrintUInt5("Scale(0+2) Alloc Output Buf[%d] Sz:%d", i, AlignedSize, 0U, 0U, 0U);
            }
            if (rval == 0U) {
                OutputBufferAlloc02 = 1U;
            } else {
                AmbaPrint_PrintStr5("%s: Error: Alloc Fail!", __func__, NULL, NULL, NULL, NULL);
                rval = 1U;
            }
        }

        if ( rval == 0U ) {
            // Start address of input buffer must align with 64
            {
                UINT32 InBufAddr;
                AmbaMisra_TypeCast32(&InBufAddr, &(InputBuf[0]));
                InBufAddr = SpuFusionUT_CacheAligned(InBufAddr);
                AmbaMisra_TypeCast32(&(InAlign64Buf[0]), &InBufAddr);

                AmbaMisra_TypeCast32(&InBufAddr, &(InputBuf[1]));
                InBufAddr = SpuFusionUT_CacheAligned(InBufAddr);
                AmbaMisra_TypeCast32(&(InAlign64Buf[1]), &InBufAddr);
            }

            AmbaMisra_TypeCast32(&pSpuIn, &(InAlign64Buf[0]));
            AmbaMisra_TypeCast32(&pPicIn, &(InAlign64Buf[1]));

            // Setup SP_DISPARITY_MAP
            {
                // Setup header
                {
                    AmbaMisra_TypeCast32(&pSpuIn->Reserved_0[0], &pSpuIn);
                    pSpuIn->Reserved_0[0] = MAX_HALF_OCTAVES;
                    pSpuIn->Reserved_0[1] = Disp2layer_0Height;
                    pSpuIn->Reserved_0[2] = Disp2layer_0Width;
                    pSpuIn->Reserved_0[3] = 0UL;
                }

                // Zero out all scales
                {
                    UINT32 ScaleIdx;
                    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
                        SpuFusionUT_CheckRval(AmbaWrap_memset(&(pSpuIn->Scales[ScaleIdx]), 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s)), "AmbaWrap_memset", __func__);
                        pSpuIn->Scales[ScaleIdx].Status = (INT32)0xFFFFFFFEUL; // Not valid
                    }
                }

                // Fusion 0 & 2
                // Setup scale 0
                {
                    pSpuIn->Scales[0U].Status = 0;
                    pSpuIn->Scales[0U].BufSize = (Disp2layer_0Pitch*Disp2layer_0Height);
                    pSpuIn->Scales[0U].DisparityHeight = Disp2layer_0Height;
                    pSpuIn->Scales[0U].DisparityWidth = Disp2layer_0Width;
                    pSpuIn->Scales[0U].DisparityPitch = Disp2layer_0Pitch;
                    pSpuIn->Scales[0U].DisparityBpp = 10U;
                    pSpuIn->Scales[0U].DisparityQm = 7U;
                    pSpuIn->Scales[0U].DisparityQf = 3U;
                    pSpuIn->Scales[0U].Reserved_0 = 0U;
                    pSpuIn->Scales[0U].RoiStartRow = 0UL;
                    pSpuIn->Scales[0U].RoiStartCol = 0UL;
                    pSpuIn->Scales[0U].Reserved_1 = 1UL;
                    pSpuIn->Scales[0U].RoiAbsoluteStartCol = 0UL;
                    pSpuIn->Scales[0U].RoiAbsoluteStartRow = 0UL;
                    pSpuIn->Scales[0U].InvalidDisparities = 0UL; //No use
                    pSpuIn->Scales[0U].DisparityMapOffset =
                        SpuFusionUT_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)); // offset to dispmap
                    pSpuIn->Scales[0U].Reserved_2 = 0UL;
                    pSpuIn->Scales[0U].Reserved_3 = 0UL;

                    // Cat disp map
                    AmbaMisra_TypeCast32(&U32InBuf, &pSpuIn);
                    U32InBuf += pSpuIn->Scales[0U].DisparityMapOffset;
                    AmbaMisra_TypeCast32(&vp, &U32InBuf);
                    SpuFusionUT_ReadInputFile(InputFn[0U], vp);
                }
                // Setup scale 2
                {
                    pSpuIn->Scales[2U].Status = 0;
                    pSpuIn->Scales[2U].BufSize = (Disp2layer_2Pitch*Disp2layer_2Height);
                    pSpuIn->Scales[2U].DisparityHeight = Disp2layer_2Height;
                    pSpuIn->Scales[2U].DisparityWidth = Disp2layer_2Width;
                    pSpuIn->Scales[2U].DisparityPitch = Disp2layer_2Pitch;
                    pSpuIn->Scales[2U].DisparityBpp = 10U;
                    pSpuIn->Scales[2U].DisparityQm = 7U;
                    pSpuIn->Scales[2U].DisparityQf = 3U;
                    pSpuIn->Scales[2U].Reserved_0 = 0U;
                    pSpuIn->Scales[2U].RoiStartRow = 0UL;
                    pSpuIn->Scales[2U].RoiStartCol = 0UL;
                    pSpuIn->Scales[2U].Reserved_1 = 1UL; // Set to invalid
                    pSpuIn->Scales[2U].RoiAbsoluteStartCol = 0UL;
                    pSpuIn->Scales[2U].RoiAbsoluteStartRow = 0UL;
                    pSpuIn->Scales[2U].InvalidDisparities = 0UL; //No use
                    pSpuIn->Scales[2U].DisparityMapOffset =
                        SpuFusionUT_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                        SpuFusionUT_CacheAligned(Disp2layer_0Pitch*Disp2layer_0Height); // offset to dispmap
                    pSpuIn->Scales[2U].Reserved_2 = 0UL;
                    pSpuIn->Scales[2U].Reserved_3 = 0UL;

                    // Cat disp map
                    AmbaMisra_TypeCast32(&U32InBuf, &pSpuIn);
                    U32InBuf += pSpuIn->Scales[2U].DisparityMapOffset;
                    AmbaMisra_TypeCast32(&vp, &U32InBuf);
                    SpuFusionUT_ReadInputFile(InputFn[1U], vp);
                }
            }

            // Setup IDSP_PICINFO
            {
                // Setup header
                {
                    pPicIn->pic_info.frame_num = 0U;
                    pPicIn->pic_info.pyramid.image_width_m1 = (UINT16)(Disp2layer_0Width - 1U);
                    pPicIn->pic_info.pyramid.image_height_m1 = (UINT16)(Disp2layer_0Height - 1U);
                    pPicIn->pic_info.pyramid.image_pitch_m1 = (Disp2layer_0Width - 1U);
                }

                // Disable all scales
                {
                    UINT32 ScaleIdx;
                    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
                        pPicIn->pic_info.pyramid.half_octave[ScaleIdx].ctrl.disable = 1U;
                    }
                }

                // Fusion 0 & 2
                // Setup scale 0
                {
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.disable = 0U;
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.mode = 1U; // stereo
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.octave_mode = 0U; // 1/sqrt(2)
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.roi_pitch = (UINT16)Disp2layer_0Width;
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_start_row = 0;
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_start_col = 0;
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_height_m1 = (UINT16)(Disp2layer_0Height - 1U);
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_width_m1 =  (UINT16)(Disp2layer_0Width - 1U);
                }
                // Setup scale 2
                {
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.disable = 0U;
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.mode = 1U; // stereo
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.octave_mode = 0U; // 1/sqrt(2)
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.roi_pitch = (UINT16)Disp2layer_2Width;
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_start_row = 0;
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_start_col = 0;
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_height_m1 = (UINT16)(Disp2layer_2Height - 1U);
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_width_m1 =  (UINT16)(Disp2layer_2Width - 1U);
                }
            }

            {
                const AMBA_STEREO_FD_HANDLE_s* pHdlr = NULL;
                UINT8* pOutputBuf;
                pHdlr = gHdlr02;
                pOutputBuf = pOutput02Buf[0];
                rval = AmbaSpuFusion_Process(pHdlr, pSpuIn, pPicIn, pOutputBuf);
                AmbaPrint_PrintUInt5("AmbaSpuFusion_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
                rval = AmbaSpuFusion_GetResult(pHdlr, &pInInfo0, &pInInfo1, &pOutputSpu);
                AmbaPrint_PrintUInt5("AmbaSpuFusion_GetResult : rval = %d", rval, 0U, 0U, 0U, 0U);
                SpuFusionUT_DumpSpuResult(pOutputSpu, OutputFn);
                (void)AmbaSpuFusion_DumpLog(pHdlr, LogPath);
            }
        }

        return;
    }

    static void RefCV_AmbaSpuFusion_Run_3_layer(UINT8* const InputBuf[2U], const char* const InputFn[],
            const char* LogPath, const char* OutputFn)
    {
        static UINT32 OutputBufferAlloc024 = 0U;
        static UINT8* InAlign64Buf[2];
        static UINT8* pOutput024Buf[8];
        UINT32 rval = 0U;
        UINT32 i;

        UINT32 Disp2layer_0Width = 1920UL;
        UINT32 Disp2layer_0Height = 1080UL;
#if 0
        UINT32 Disp3layer_0Width = 1024UL;
        UINT32 Disp3layer_0Height = 576UL;
        UINT32 Disp3layer_0Pitch = 1280UL;
        UINT32 Disp3layer_2Width = 512UL;
        UINT32 Disp3layer_2Height = 288UL;
        UINT32 Disp3layer_2Pitch = 640UL;
        UINT32 Disp3layer_4Width = 256UL;
        UINT32 Disp3layer_4Height = 144UL;
        UINT32 Disp3layer_4Pitch = 320UL;
#else
        //UINT32 Disp3layer_0Width = 1072UL;
        //UINT32 Disp3layer_0Height = 608UL;
        //UINT32 Disp3layer_0Pitch = 1344UL;
        UINT32 Disp3layer_0Width = 960UL;
        UINT32 Disp3layer_0Height = 1080UL;
        UINT32 Disp3layer_0Pitch = 1216UL;
        UINT32 Disp3layer_2Width = 960UL;
        UINT32 Disp3layer_2Height = 540UL;
        UINT32 Disp3layer_2Pitch = 1216UL;
        UINT32 Disp3layer_4Width = 480UL;
        UINT32 Disp3layer_4Height = 270UL;
        UINT32 Disp3layer_4Pitch = 608UL;
#endif
        AMBA_CV_SPU_DATA_s *pSpuIn = NULL;
        memio_source_recv_picinfo_t *pPicIn;
        void* vp;
        UINT32 U32InBuf;
        AMBA_CV_SPU_DATA_s *pInInfo0;
        memio_source_recv_picinfo_t *pInInfo1;
        UINT32 AlignedSize;
        AMBA_CV_SPU_DATA_s *pOutputSpu;

        if ( OutputBufferAlloc024 == 0U ) {
            for(i = 0U; i < gOutputNum024; ++i) {
                rval |= RefCV_UT_GetCVBuf(&pOutput024Buf[i], gOutputSz024[i], &AlignedSize);
                AmbaPrint_PrintUInt5("Scale(0+2+4) Alloc Output Buf[%d] Sz:%d", i, AlignedSize, 0U, 0U, 0U);
            }
            if (rval == 0U) {
                OutputBufferAlloc024 = 1U;
            } else {
                AmbaPrint_PrintStr5("%s: Error: Alloc Fail!", __func__, NULL, NULL, NULL, NULL);
                rval = 1U;
            }
        }

        if ( rval == 0U ) {
            // Start address of input buffer must align with 64
            {
                UINT32 InBufAddr;
                AmbaMisra_TypeCast32(&InBufAddr, &(InputBuf[0]));
                InBufAddr = SpuFusionUT_CacheAligned(InBufAddr);
                AmbaMisra_TypeCast32(&(InAlign64Buf[0]), &InBufAddr);

                AmbaMisra_TypeCast32(&InBufAddr, &(InputBuf[1]));
                InBufAddr = SpuFusionUT_CacheAligned(InBufAddr);
                AmbaMisra_TypeCast32(&(InAlign64Buf[1]), &InBufAddr);
            }

            AmbaMisra_TypeCast32(&pSpuIn, &(InAlign64Buf[0]));
            AmbaMisra_TypeCast32(&pPicIn, &(InAlign64Buf[1]));

            // Setup SP_DISPARITY_MAP
            {
                // Setup header
                {
                    AmbaMisra_TypeCast32(&pSpuIn->Reserved_0[0], &pSpuIn);
                    pSpuIn->Reserved_0[0] = MAX_HALF_OCTAVES;
                    pSpuIn->Reserved_0[1] = Disp2layer_0Height;
                    pSpuIn->Reserved_0[2] = Disp2layer_0Width;
                    pSpuIn->Reserved_0[3] = 0UL;
                }

                // Zero out all scales
                {
                    UINT32 ScaleIdx;
                    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
                        SpuFusionUT_CheckRval(AmbaWrap_memset(&(pSpuIn->Scales[ScaleIdx]), 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s)), "AmbaWrap_memset", __func__);
                        pSpuIn->Scales[ScaleIdx].Status = (INT32)0xFFFFFFFEUL; // Not valid
                    }
                }

                // Fusion 0 & 2 & 4
                // Setup scale 0
                {
                    pSpuIn->Scales[0U].Status = 0;
                    pSpuIn->Scales[0U].BufSize = (Disp3layer_0Pitch*Disp3layer_0Height);
                    pSpuIn->Scales[0U].DisparityHeight = Disp3layer_0Height;
                    pSpuIn->Scales[0U].DisparityWidth = Disp3layer_0Width;
                    pSpuIn->Scales[0U].DisparityPitch = Disp3layer_0Pitch;
                    pSpuIn->Scales[0U].DisparityBpp = 10U;
                    pSpuIn->Scales[0U].DisparityQm = 7U;
                    pSpuIn->Scales[0U].DisparityQf = 3U;
                    pSpuIn->Scales[0U].Reserved_0 = 1U;
                    pSpuIn->Scales[0U].RoiStartRow = 0UL;
                    pSpuIn->Scales[0U].RoiStartCol = 0UL;
                    pSpuIn->Scales[0U].Reserved_1 = 1UL;
                    pSpuIn->Scales[0U].RoiAbsoluteStartCol = 0UL;
                    pSpuIn->Scales[0U].RoiAbsoluteStartRow = 0UL;
                    pSpuIn->Scales[0U].InvalidDisparities = 0UL; //No use
                    pSpuIn->Scales[0U].DisparityMapOffset =
                        SpuFusionUT_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)); // offset to dispmap
                    pSpuIn->Scales[0U].Reserved_2 = 0UL;
                    pSpuIn->Scales[0U].Reserved_3 = 0UL;

                    // Cat disp map
                    AmbaMisra_TypeCast32(&U32InBuf, &pSpuIn);
                    U32InBuf += pSpuIn->Scales[0U].DisparityMapOffset;
                    AmbaMisra_TypeCast32(&vp, &U32InBuf);
                    SpuFusionUT_ReadInputFile(InputFn[0U], vp);
                }
                // Setup scale 2
                {
                    pSpuIn->Scales[2U].Status = 0;
                    pSpuIn->Scales[2U].BufSize = (Disp3layer_2Pitch*Disp3layer_2Height);
                    pSpuIn->Scales[2U].DisparityHeight = Disp3layer_2Height;
                    pSpuIn->Scales[2U].DisparityWidth = Disp3layer_2Width;
                    pSpuIn->Scales[2U].DisparityPitch = Disp3layer_2Pitch;
                    pSpuIn->Scales[2U].DisparityBpp = 10U;
                    pSpuIn->Scales[2U].DisparityQm = 7U;
                    pSpuIn->Scales[2U].DisparityQf = 3U;
                    pSpuIn->Scales[2U].Reserved_0 = 0U;
                    pSpuIn->Scales[2U].RoiStartRow = 0UL;
                    pSpuIn->Scales[2U].RoiStartCol = 0UL;
                    pSpuIn->Scales[2U].Reserved_1 = 1UL; // Set to invalid
                    pSpuIn->Scales[2U].RoiAbsoluteStartCol = 0UL;
                    pSpuIn->Scales[2U].RoiAbsoluteStartRow = 0UL;
                    pSpuIn->Scales[2U].InvalidDisparities = 0UL; //No use
                    pSpuIn->Scales[2U].DisparityMapOffset =
                        SpuFusionUT_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                        SpuFusionUT_CacheAligned(Disp3layer_0Pitch*Disp3layer_0Height); // offset to dispmap
                    pSpuIn->Scales[2U].Reserved_2 = 0UL;
                    pSpuIn->Scales[2U].Reserved_3 = 0UL;

                    // Cat disp map
                    AmbaMisra_TypeCast32(&U32InBuf, &pSpuIn);
                    U32InBuf += pSpuIn->Scales[2U].DisparityMapOffset;
                    AmbaMisra_TypeCast32(&vp, &U32InBuf);
                    SpuFusionUT_ReadInputFile(InputFn[1U], vp);
                }
                // Setup scale 4
                {
                    pSpuIn->Scales[4U].Status = 0;
                    pSpuIn->Scales[4U].BufSize = (Disp3layer_4Pitch*Disp3layer_4Height);
                    pSpuIn->Scales[4U].DisparityHeight = Disp3layer_4Height;
                    pSpuIn->Scales[4U].DisparityWidth = Disp3layer_4Width;
                    pSpuIn->Scales[4U].DisparityPitch = Disp3layer_4Pitch;
                    pSpuIn->Scales[4U].DisparityBpp = 10U;
                    pSpuIn->Scales[4U].DisparityQm = 7U;
                    pSpuIn->Scales[4U].DisparityQf = 3U;
                    pSpuIn->Scales[4U].Reserved_0 = 0U;
                    pSpuIn->Scales[4U].RoiStartRow = 0UL;
                    pSpuIn->Scales[4U].RoiStartCol = 0UL;
                    pSpuIn->Scales[4U].Reserved_1 = 1UL; // Set to invalid
                    pSpuIn->Scales[4U].RoiAbsoluteStartCol = 0UL;
                    pSpuIn->Scales[4U].RoiAbsoluteStartRow = 0UL;
                    pSpuIn->Scales[4U].InvalidDisparities = 0UL; //No use
                    pSpuIn->Scales[4U].DisparityMapOffset =
                        SpuFusionUT_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                        SpuFusionUT_CacheAligned(Disp3layer_2Pitch*Disp3layer_2Height) +
                        SpuFusionUT_CacheAligned(Disp3layer_0Pitch*Disp3layer_0Height); // offset to dispmap
                    pSpuIn->Scales[4U].Reserved_2 = 0UL;
                    pSpuIn->Scales[4U].Reserved_3 = 0UL;

                    // Cat disp map
                    AmbaMisra_TypeCast32(&U32InBuf, &pSpuIn);
                    U32InBuf += pSpuIn->Scales[4U].DisparityMapOffset;
                    AmbaMisra_TypeCast32(&vp, &U32InBuf);
                    SpuFusionUT_ReadInputFile(InputFn[2U], vp);
                }
            }

            // Setup IDSP_PICINFO
            {
                // Setup header
                {
                    pPicIn->pic_info.frame_num = 0U;
                    pPicIn->pic_info.pyramid.image_width_m1 = (UINT16)(Disp2layer_0Width - 1U);
                    pPicIn->pic_info.pyramid.image_height_m1 = (UINT16)(Disp2layer_0Height - 1U);
                    pPicIn->pic_info.pyramid.image_pitch_m1 = (Disp2layer_0Width - 1U);
                }

                // Disable all scales
                {
                    UINT32 ScaleIdx;
                    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
                        pPicIn->pic_info.pyramid.half_octave[ScaleIdx].ctrl.disable = 1U;
                    }
                }

                // Fusion 0 & 2 & 4
                // Setup scale 0
                {
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.disable = 0U;
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.mode = 1U; // stereo
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.octave_mode = 0U; // 1/sqrt(2)
                    pPicIn->pic_info.pyramid.half_octave[0U].ctrl.roi_pitch = (UINT16)Disp3layer_0Width;
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_start_row = 0;
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_start_col = 0;
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_height_m1 = (UINT16)(Disp3layer_0Height - 1U);
                    pPicIn->pic_info.pyramid.half_octave[0U].roi_width_m1 =  (UINT16)(Disp3layer_0Width - 1U);
                }
                // Setup scale 2
                {
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.disable = 0U;
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.mode = 1U; // stereo
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.octave_mode = 0U; // 1/sqrt(2)
                    pPicIn->pic_info.pyramid.half_octave[2U].ctrl.roi_pitch = (UINT16)Disp3layer_2Width;
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_start_row = 0;
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_start_col = 0;
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_height_m1 = (UINT16)(Disp3layer_2Height - 1U);
                    pPicIn->pic_info.pyramid.half_octave[2U].roi_width_m1 =  (UINT16)(Disp3layer_2Width - 1U);
                }
                // Setup scale 4
                {
                    pPicIn->pic_info.pyramid.half_octave[4U].ctrl.disable = 0U;
                    pPicIn->pic_info.pyramid.half_octave[4U].ctrl.mode = 1U; // stereo
                    pPicIn->pic_info.pyramid.half_octave[4U].ctrl.octave_mode = 0U; // 1/sqrt(2)
                    pPicIn->pic_info.pyramid.half_octave[4U].ctrl.roi_pitch = (UINT16)Disp3layer_4Width;
                    pPicIn->pic_info.pyramid.half_octave[4U].roi_start_row = 0;
                    pPicIn->pic_info.pyramid.half_octave[4U].roi_start_col = 0;
                    pPicIn->pic_info.pyramid.half_octave[4U].roi_height_m1 = (UINT16)(Disp3layer_4Height - 1U);
                    pPicIn->pic_info.pyramid.half_octave[4U].roi_width_m1 =  (UINT16)(Disp3layer_4Width - 1U);
                }
            }

            {
                const AMBA_STEREO_FD_HANDLE_s* pHdlr = NULL;
                UINT8* pOutputBuf;
                pHdlr = gHdlr024;
                pOutputBuf = pOutput024Buf[0];
                rval = AmbaSpuFusion_Process(pHdlr, pSpuIn, pPicIn, pOutputBuf);
                AmbaPrint_PrintUInt5("AmbaSpuFusion_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
                rval = AmbaSpuFusion_GetResult(pHdlr, &pInInfo0, &pInInfo1, &pOutputSpu);
                AmbaPrint_PrintUInt5("AmbaSpuFusion_GetResult : rval = %d", rval, 0U, 0U, 0U, 0U);
                SpuFusionUT_DumpSpuResult(pOutputSpu, OutputFn);
                (void)AmbaSpuFusion_DumpLog(pHdlr, LogPath);
            }
        }

        return;
    }


    static void RefCV_AmbaSpuFusion_UT_Run(UINT32 LayerNum, const char* const InputFn[],
                                           const char* LogPath, const char* OutputFn)
    {
        static UINT32 InBufferAlloc = 0U;
        static UINT8* InBuf[2];
        UINT32 rval = 0U;
        UINT32 Displayer_0Height = 1080UL;
        UINT32 Displayer_0Pitch = 2400UL;
        UINT32 Displayer_2Height = 540UL;
        UINT32 Displayer_2Pitch = 1216UL;
        UINT32 Displayer_4Height = 270UL;
        UINT32 Displayer_4Pitch = 608UL;
        UINT32 AlignedSize;
        UINT32 InputBufSize;

        //alloc input buffer
        if ( InBufferAlloc == 0U ) {
            InputBufSize = SpuFusionUT_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                           SpuFusionUT_CacheAligned(Displayer_0Pitch*Displayer_0Height) +
                           SpuFusionUT_CacheAligned(Displayer_2Pitch*Displayer_2Height) +
                           SpuFusionUT_CacheAligned(Displayer_4Pitch*Displayer_4Height);
            rval |= RefCV_UT_GetCVBuf(&(InBuf[0]), InputBufSize, &AlignedSize);

            InputBufSize = SpuFusionUT_CacheAligned(sizeof(memio_source_recv_picinfo_t));
            rval |= RefCV_UT_GetCVBuf(&(InBuf[1]), InputBufSize, &AlignedSize);
            InBufferAlloc = 1U;
        }

        if ( rval == 0U ) {
            if ( LayerNum == 2U ) {
                RefCV_AmbaSpuFusion_Run_2_layer(InBuf, InputFn, LogPath, OutputFn);
            } else if ( LayerNum == 3U ) {
                RefCV_AmbaSpuFusion_Run_3_layer(InBuf, InputFn, LogPath, OutputFn);
            } else {
                AmbaPrint_PrintUInt5("Error: LayerNum = %d is not supported!", LayerNum, 0U, 0U, 0U, 0U);
            }
        }

        return;
    }

    void RefCV_AmbaSpuFusion_UT(const char* BinPath, const char* LogPath, UINT32 LayerNum,
                                const char* InputScale0Fn, const char* InputScale2Fn, const char* InputScale4Fn, const char* OutputFn)
    {
        const char* InputFn[16U];
        static UINT32 init = 0;

        if (init == 0U) {
            AmbaPrint_PrintUInt5("AmbaCV_ProtectInit()...", 0U, 0U, 0U, 0U, 0U);
            (void) AmbaCV_ProtectInit(0x3U);
            (void) AmbaKAL_TaskSleep(300U);
            init = 1U;
        }

        InputFn[0U] = InputScale0Fn;
        InputFn[1U] = InputScale2Fn;
        InputFn[2U] = InputScale4Fn;
        RefCV_AmbaSpuFusion_UT_Init(LayerNum, BinPath);
        RefCV_AmbaSpuFusion_UT_Run(LayerNum, InputFn, LogPath, OutputFn);
    }
