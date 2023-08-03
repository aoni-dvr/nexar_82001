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
#include "cvapi_flexidag_ambafma_cv2.h"
#include "cvapi_protection.h"

static const AMBA_STEREO_FD_HANDLE_s* Hdlr = NULL;

static inline UINT32 FmaUT_CacheAligned (UINT32 Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("FmaUT_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}

static const AMBA_STEREO_FD_HANDLE_s *CastStereoFd2Const(AMBA_STEREO_FD_HANDLE_s *pHdlr)
{
    const AMBA_STEREO_FD_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    if (0U != AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval))) {
        AmbaPrint_PrintStr5("Error CastStereoFd2Const() call AmbaWrap_memcpy() Fail", NULL, NULL, NULL, NULL, NULL);
    }
    return pRval;
}

static void FmaUT_ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 U32Buf;
    UINT32 FsizeAlign;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        AmbaMisra_TypeCast32(&U32Buf, &buf);
        //fix input align
        FsizeAlign = FmaUT_CacheAligned(Fsize);
        Ret = AmbaCache_DataClean(U32Buf, FsizeAlign);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void FmaUT_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 FmaUT_LoadFlexiDagBin(const char* path, UINT8**BinBuf, UINT32* Size)
{
    flexidag_memblk_t FlexidagBinBuf;
    UINT8 *pU8 = NULL;
    UINT32 BinSize;
    UINT32 AlignedBinSize;
    UINT32 ret = 0U;
    UINT32 addr, Paddr;

    FmaUT_GetFileSize(path, &BinSize);

    if (BinSize != 0U) {

        ret = RefCV_UT_GetCVBuf(&pU8, BinSize, &AlignedBinSize);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("FmaUT_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
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
                AmbaPrint_PrintUInt5("FmaUT_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *BinBuf = pU8;
            *Size = AlignedBinSize;
        }
    } else {
        AmbaPrint_PrintUInt5("FmaUT_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    }
    return ret;
}

static UINT32 OutputNum;
static UINT32 OutputSz[8];

static void RefCV_AmbaFma_UT_Init(const char *FlexiBinPath)
{

    static UINT32 Init = 0U;
    UINT32 AlignSz = 0U;
    UINT32 Ret;
    UINT8* BinBuf = NULL;
    UINT32 BinSize = 0U;
    UINT8* StateBuf = NULL;
    UINT32 StateSize = 0U;
    UINT8* TempBuf = NULL;
    UINT32 TempSize = 0U;
    static AMBA_STEREO_FD_HANDLE_s Handler = {0};
    Hdlr = CastStereoFd2Const(&Handler);
    if (Init == 0U) {
        (void)FmaUT_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        Ret = AmbaFma_Open(BinBuf, BinSize, &StateSize, &TempSize, &OutputNum, OutputSz, Hdlr);
        if (Ret == 0U) {
            AmbaPrint_PrintUInt5("OutputNum:%d", OutputNum, 0U, 0U, 0U, 0U);
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
            (void)AmbaFma_Init(Hdlr, StateBuf, TempBuf);
            //FmaUT_Config_AmbaFma(Hdlr);
        }
        Init = 1U;
    }
}

static void FmaUT_SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
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

static void FmaUT_DumpFmaResult(const AMBA_CV_FMA_DATA_s *out, const char* OutputFn, UINT32 InputBufSz)
{
    const AMBA_CV_FMA_DATA_s *pFma;
    UINT32 Fsize;
    UINT8 *pChar;
    UINT32 U32pFex;

    AmbaMisra_TypeCast32(&pFma, &out);
    Fsize = InputBufSz;
    AmbaPrint_PrintUInt5("Fsize: %d", Fsize, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pFma->NumHalfOctaves: %d", pFma->NumHalfOctaves, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pFma->StereoMatch[0].Valid: %d", pFma->StereoMatch[0].Valid, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pFma->TemporalMatch[0].Valid: %d", pFma->TemporalMatch[0].Valid, 0U, 0U, 0U, 0U);
    AmbaMisra_TypeCast32(&pChar, &pFma);
    FmaUT_SaveBin(pChar, OutputFn, Fsize);

    Fsize=sizeof(UINT8)*CV_FEX_MAX_BUCKETS;
    AmbaMisra_TypeCast32(&U32pFex, &pFma);
    U32pFex += pFma->FeatureList[0].KeypointsCountOffset;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    FmaUT_SaveBin(pChar, "c:\\Fma_Master_Point_Count.bin", Fsize);

    Fsize=sizeof(UINT32)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    AmbaMisra_TypeCast32(&U32pFex, &pFma);
    U32pFex += pFma->FeatureList[0].KeypointsOffset;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    FmaUT_SaveBin(pChar, "c:\\Fma_Master_Point.bin", Fsize);

    Fsize=sizeof(UINT8)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    AmbaMisra_TypeCast32(&U32pFex, &pFma);
    U32pFex += pFma->StereoMatch[0].MpScore;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    FmaUT_SaveBin(pChar, "c:\\Fma_Stereo_Score.bin", Fsize);

    Fsize=sizeof(UINT16)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    AmbaMisra_TypeCast32(&U32pFex, &pFma);
    U32pFex += pFma->StereoMatch[0].MpIndex;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    FmaUT_SaveBin(pChar, "c:\\Fma_Stereo_Index.bin", Fsize);

    Fsize=sizeof(UINT8)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    AmbaMisra_TypeCast32(&U32pFex, &pFma);
    U32pFex += pFma->TemporalMatch[0].MpScore;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    FmaUT_SaveBin(pChar, "c:\\Fma_Temporal_Score.bin", Fsize);

    Fsize=sizeof(UINT16)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    AmbaMisra_TypeCast32(&U32pFex, &pFma);
    U32pFex += pFma->TemporalMatch[0].MpIndex;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    FmaUT_SaveBin(pChar, "c:\\Fma_Temporal_Index.bin", Fsize);
}

static void FmaUT_Config_AmbaFma(const AMBA_STEREO_FD_HANDLE_s* pHdlr)
{
    UINT32 Idex;
    AMBA_CV_FMA_CFG_s Cfg;
    AMBA_CV_FMA_CFG_s *pCfg = &Cfg;
    //set fma default config
    pCfg->ScaleCfg[0].Mode = (UINT8)CV_FMA_ENABLE_ALL;
    for(Idex=1; Idex<MAX_HALF_OCTAVES; ++Idex) {
        pCfg->ScaleCfg[Idex].Mode = 0U;
    }
    (void)AmbaFma_Cfg(pHdlr, pCfg);
}

static void RefCV_AmbaFma_UT_Run(const char* InPath, const char* LogPath, const char* OutputFn)
{
    static UINT32 BufferAlloc = 0U;
    static UINT8* InBuf;
    static UINT8* OutputBuf[8];
    static AMBA_CV_FMA_DATA_s* OutputBufFma[8];
    UINT32 rval;
    UINT32 Idex;
    void* vp;
    UINT32 U32InBuf;
    UINT32 AlignedSize;
    UINT32 InputBufSize;
    UINT32 BinSize = 0U;
    const AMBA_CV_FEX_DATA_s *MemIOFexOut = NULL;
    AMBA_CV_FEX_DATA_s *pFexOut;

    FmaUT_GetFileSize(InPath, &BinSize);
    //buffer constraint
    InputBufSize = FmaUT_CacheAligned(BinSize);

    if (BufferAlloc == 0U) {
        //alloc input buffer
        rval = RefCV_UT_GetCVBuf(&InBuf, InputBufSize, &AlignedSize);
        AmbaPrint_PrintUInt5("InputBufSize = %d", InputBufSize, 0U, 0U, 0U, 0U);

        //alloc output buffer
        if (rval == 0U) {
            for(Idex=0U; Idex<OutputNum; ++Idex) {
                rval = RefCV_UT_GetCVBuf(&OutputBuf[Idex], OutputSz[Idex], &AlignedSize);
                rval |= AmbaWrap_memset(OutputBuf[Idex], 0, sizeof(OutputSz[Idex]));
                AmbaPrint_PrintUInt5("Alloc Output Buf[%d] Sz:%d", Idex, AlignedSize, 0U, 0U, 0U);
            }
        }
        BufferAlloc = 1U;
        if(rval != 0U) {
            AmbaPrint_PrintStr5("Error RefCV_AmbaFma_UT_Run() call RefCV_UT_GetCVBuf() or AmbaWrap_memset() Fail", NULL, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TypeCast32(&U32InBuf, &InBuf);
    AmbaMisra_TypeCast32(&MemIOFexOut, &InBuf);
    AmbaMisra_TypeCast32(&vp, &U32InBuf);
    FmaUT_ReadInputFile(InPath, vp);

    FmaUT_Config_AmbaFma(Hdlr);
    AmbaMisra_TypeCast32(&OutputBufFma[0], &OutputBuf[0]);

    rval = AmbaFma_Process(Hdlr, MemIOFexOut, InputBufSize, OutputBufFma[0]);
    AmbaPrint_PrintUInt5("AmbaFma_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
    rval = AmbaFma_GetResult(Hdlr, &pFexOut, &OutputBufFma[0]);
    AmbaPrint_PrintUInt5("AmbaFma_GetResult : rval = %d", rval, OutputNum, 0U, 0U, 0U);
    FmaUT_DumpFmaResult(OutputBufFma[0], OutputFn, OutputSz[0]);
    (void)AmbaFma_DumpLog(Hdlr, LogPath);

}

void RefCV_AmbaFma_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn)
{
    static UINT32 init = 0;

    if (init == 0U) {
        AmbaPrint_PrintUInt5("AmbaCV_ProtectInit()...", 0U, 0U, 0U, 0U, 0U);
        (void) AmbaCV_ProtectInit(0x3U);
        (void) AmbaKAL_TaskSleep(300U);
        init = 1U;
    }

    RefCV_AmbaFma_UT_Init(BinPath);
    RefCV_AmbaFma_UT_Run(InFile, LogPath, OutputFn);
}
