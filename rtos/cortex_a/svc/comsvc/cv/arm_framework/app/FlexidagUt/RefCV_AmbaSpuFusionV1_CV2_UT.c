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

#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_svccvalgo_ambaspufusion.h"

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
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
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "cvapi_protection.h"
#elif defined(CONFIG_QNX)
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
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "cvapi_protection.h"
#define AmbaMisra_TouchUnused
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "rtos/AmbaRTOSWrapper.h"
#include "cvapi_common.h"
#include "ambint.h"

#define AMBA_CACHE_LINE_SIZE    64U
#define AMBA_FS_FILE FILE
#define AMBA_FS_SEEK_CUR SEEK_CUR
#define AMBA_FS_SEEK_START SEEK_SET
#define AMBA_FS_SEEK_END SEEK_END
#define AmbaFS_FileClose fclose
#define AmbaFS_FileSeek fseek
#define AmbaMisra_TypeCast AmbaMisra_TypeCast64
#define AMBA_FS_ERR_API 0x00110004U
#endif

#if defined(SCA_STEREO)
static UINT32 CV2_SPUFUSION_SCA_WRAPPER_UT_FLOW = 1U;
#else
static UINT32 CV2_SPUFUSION_SCA_WRAPPER_UT_FLOW = 0U;
#endif

static const AMBA_STEREO_FD_HANDLE_s* gHdlr02 = NULL;
static const AMBA_STEREO_FD_HANDLE_s* gHdlr024 = NULL;
static UINT32 gOutputNum02;
static UINT32 gOutputSz02[8];
static UINT32 gOutputNum024;
static UINT32 gOutputSz024[8];

static SVC_CV_ALGO_HANDLE_s ScaHandler02;
static SVC_CV_ALGO_HANDLE_s ScaHandler024;
static const char* OutputFileName = NULL;

static void SpuFusionV1_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static const AMBA_STEREO_FD_HANDLE_s *CastStereoFd2Const(AMBA_STEREO_FD_HANDLE_s *pHdlr)
{
    const AMBA_STEREO_FD_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval)), "AmbaWrap_memcpy", __func__);
    return pRval;
}

#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
static uint32_t RefCV_MemblkClean(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemClean(buf);
    if(ret != 0U) {
        printf("RefCV_MemblkClean : AmbaCV_UtilityCmaMemClean fail \n");
    }
    return ret;
}

static uint32_t RefCV_MemblkInvalid(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemInvalid(buf);
    if(ret != 0U) {
        printf("RefCV_MemblkClean : AmbaCV_UtilityCmaMemInvalid fail \n");
    }
    return ret;
}

static uint32_t RefCV_MemblkAlloc(uint32_t buf_size, flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    if( AmbaCV_UtilityCmaMemAlloc(buf_size, 1, buf) != 0) {
        printf("RefCV_MemblkAlloc : Out of memory \n");
        ret = 1U;
    }
    return ret;
}

static uint32_t RefCV_MemblkFree(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    if( AmbaCV_UtilityCmaMemFree(buf) != 0) {
        printf("RefCV_MemblkFree : AmbaCV_UtilityCmaMemFree fail \n");
        ret = 1U;
    }
    return ret;
}
#endif

static UINT32 SpuFusionV1_FileOpen(const char *pFileName, const char *pMode, AMBA_FS_FILE **pFile)
{
    UINT32 Ret = 0U;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileOpen(pFileName, pMode, pFile);
#else
    *pFile = fopen(pFileName, pMode);
#endif
    if (*pFile == NULL) {
        Ret = AMBA_FS_ERR_API;
    }
    return Ret;
}

static UINT32 SpuFusionV1_FileTell(AMBA_FS_FILE * pFile, UINT64* pFilePos)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileTell(pFile, pFilePos);
#else
    (*pFilePos) = ftell(pFile);
#endif
    return Ret;
}

static UINT32 SpuFusionV1_FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileRead(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fread(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static UINT32 SpuFusionV1_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileWrite(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fwrite(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static inline ULONG SpuFusionV1_CacheAligned (ULONG Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("SpuFusionV1_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}

static void SpuFusionV1_ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 FsizeAlign;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = SpuFusionV1_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile SpuFusionV1_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = SpuFusionV1_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile SpuFusionV1_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = SpuFusionV1_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile SpuFusionV1_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
#if defined(CONFIG_BUILD_CV_THREADX)
    if (Ret == 0U) {
        ULONG ULBuf;
        (void)AmbaMisra_TypeCast(&ULBuf, &buf);
        //fix input align
        FsizeAlign = SpuFusionV1_CacheAligned(Fsize);
        Ret = AmbaCache_DataClean(ULBuf, FsizeAlign);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
#else
    AmbaMisra_TouchUnused(&Fsize);
    AmbaMisra_TouchUnused(&FsizeAlign);
#endif
}

static void SpuFusionV1_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = SpuFusionV1_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile SpuFusionV1_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = SpuFusionV1_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile SpuFusionV1_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 SpuFusionV1_LoadFlexiDagBin(const char* path, flexidag_memblk_t *BinBuf, UINT32* Size)
{
    UINT32 BinSize;
    UINT32 ret = 0U;

    SpuFusionV1_GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        ret = RefCV_MemblkAlloc(BinSize, BinBuf);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("SpuFusionV1_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(path, BinBuf);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("SpuFusionV1_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *Size = BinBuf->buffer_size;
        }
    } else {
        AmbaPrint_PrintUInt5("SpuFusionV1_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
        BinBuf->pBuffer = NULL;
    }
    return ret;
}

#if 0
static void RefCV_AmbaSpuFusionV1_UT_Init(UINT32 LayerNum, const char *FlexiBinPath)
{
    UINT32 Ret;
    flexidag_memblk_t BinBuf;
    UINT32 BinSize = 0U;
    flexidag_memblk_t StateBuf;
    UINT32 StateSize = 0U;
    flexidag_memblk_t TempBuf;
    UINT32 TempSize = 0U;

    static UINT32 gInit02 = 0U;
    static UINT32 gInit024 = 0U;
    static AMBA_STEREO_FD_HANDLE_s Handler02 = {0};
    static AMBA_STEREO_FD_HANDLE_s Handler024 = {0};
    gHdlr02 = CastStereoFd2Const(&Handler02);
    gHdlr024 = CastStereoFd2Const(&Handler024);

    if ( LayerNum == 2U ) {
        if (gInit02 == 0U) {
            (void)SpuFusionV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
            Ret = AmbaSpuFusion_Openv1(&BinBuf, BinSize, &StateSize, &TempSize, &gOutputNum02, gOutputSz02, gHdlr02);
            if (Ret == 0U) {
                AmbaPrint_PrintUInt5("OutputNum:%d", gOutputNum02, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(StateSize, &StateBuf);
                AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", StateSize, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(TempSize, &TempBuf);
                AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", TempSize, 0U, 0U, 0U, 0U);
            }

            if (Ret == 0U) {
                (void)AmbaSpuFusion_Initv1(gHdlr02, &StateBuf, &TempBuf);
                //_Config_AmbaSpuFusion(Hdlr);
            }
            gInit02 = 1U;
        }
    } else if ( LayerNum == 3U ) {
        if (gInit024 == 0U) {
            (void)SpuFusionV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
            Ret = AmbaSpuFusion3Layer_Openv1(&BinBuf, BinSize, &StateSize, &TempSize, &gOutputNum024, gOutputSz024, gHdlr024);
            if (Ret == 0U) {
                AmbaPrint_PrintUInt5("OutputNum:%d", gOutputNum024, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(StateSize, &StateBuf);
                AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", StateSize, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(TempSize, &TempBuf);
                AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", TempSize, 0U, 0U, 0U, 0U);
            }

            if (Ret == 0U) {
                (void)AmbaSpuFusion_Initv1(gHdlr024, &StateBuf, &TempBuf);
                //_Config_AmbaSpuFusion(Hdlr);
            }
            gInit024 = 1U;
        }
    } else {
        AmbaPrint_PrintUInt5("LayerNum = %d is not yet to supported", LayerNum, 0U, 0U, 0U, 0U);
    }
}
#endif

static void SpuFusionV1_SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;

    Ret = SpuFusionV1_FileOpen(Fn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile SpuFusionV1_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = SpuFusionV1_FileWrite(Buf, 1U, Size, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile SpuFusionV1_FileWrite fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void SpuFusionV1_DumpSpuResult(const AMBA_CV_SPU_DATA_s *pSpu, const char* OutputFn)
{
    UINT32 Fsize;
    UINT8 *pChar;

    ULONG ULSpuAddr;

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
        (void)AmbaMisra_TypeCast(&ULSpuAddr, &pSpu);
        ULSpuAddr += pSpu->Scales[ValidScale].DisparityMapOffset;
        (void)AmbaMisra_TypeCast(&pChar, &ULSpuAddr);
        SpuFusionV1_SaveBin(pChar, OutputFn, Fsize);
    }

    static UINT32 RefCV_AmbaSpuFusionV1_Alloc(UINT32 LayerNum, UINT32* OutputBufferAlloc02, UINT32* OutputBufferAlloc024, flexidag_memblk_t pOutput02Buf[], flexidag_memblk_t pOutput024Buf[])
    {

        UINT32 Idex;
        UINT32 Ret = 0U;

        // alloc output buffer
        if ( LayerNum == 2U ) {
            if ((*OutputBufferAlloc02) == 0U ) {
                for(Idex = 0U; Idex < gOutputNum02; ++Idex) {
                    Ret |= RefCV_MemblkAlloc(gOutputSz02[Idex], &pOutput02Buf[Idex]);
                    AmbaPrint_PrintUInt5("Scale(0+2) Alloc Output Buf[%d] Sz:%d", Idex, gOutputSz02[Idex], 0U, 0U, 0U);
                }
                if (Ret == 0U) {
                    (*OutputBufferAlloc02) = 1U;
                } else {
                    AmbaPrint_PrintStr5("%s: Error: Alloc Fail!", __func__, NULL, NULL, NULL, NULL);
                    Ret = 1U;
                }
            }
        } else if ( LayerNum == 3U ) {
            if ( (*OutputBufferAlloc024) == 0U ) {
                for(Idex = 0U; Idex < gOutputNum024; ++Idex) {
                    Ret |= RefCV_MemblkAlloc(gOutputSz024[Idex], &pOutput024Buf[Idex]);
                    AmbaPrint_PrintUInt5("Scale(0+2+4) Alloc Output Buf[%d] Sz:%d", Idex, gOutputSz024[Idex], 0U, 0U, 0U);
                }
                if (Ret == 0U) {
                    (*OutputBufferAlloc024) = 1U;
                } else {
                    AmbaPrint_PrintStr5("%s: Error: Alloc Fail!", __func__, NULL, NULL, NULL, NULL);
                    Ret = 1U;
                }
            }
        } else {
            AmbaPrint_PrintUInt5("Error: LayerNum = %d is not supported!", LayerNum, 0U, 0U, 0U, 0U);
            Ret = 1U;
        }
        return Ret;

    }

    static UINT32 RefCV_AmbaSpuFusion_UT_CallBack(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
    {
        UINT32 Ret = 0;
        AmbaPrint_PrintUInt5("--> SpuFusion SCA CallBack Event %d", Event, 0U, 0U, 0U, 0U);
        if (pEventData != NULL) {
            if (pEventData->pOutput != NULL) {
                if (Event == CALLBACK_EVENT_OUTPUT) {
                    if ((pEventData->pOutput->buf[0].pBuffer != NULL) && (OutputFileName != NULL))  {
                        AMBA_CV_SPU_BUF_s OutSpu;
                        SpuFusionV1_CheckRval(AmbaWrap_memcpy(&OutSpu, &pEventData->pOutput->buf[0], sizeof(AMBA_CV_SPU_BUF_s)), "AmbaWrap_memcpy", __func__);
                        SpuFusionV1_DumpSpuResult(OutSpu.pBuffer, OutputFileName);
                    } else {
                        AmbaPrint_PrintUInt5("--> SpuFusion SCA CallBack Error: Output buffer[0] NULL", 0U, 0U, 0U, 0U, 0U);
                    }
                }
            } else {
                AmbaPrint_PrintUInt5("--> SpuFusion SCA CallBack Error: Output data NULL", 0U, 0U, 0U, 0U, 0U);
            }
        } else {
            AmbaPrint_PrintUInt5("--> SpuFusion SCA CallBack Error: Event data NULL", 0U, 0U, 0U, 0U, 0U);
        }

        return Ret;
    }

    static UINT32 SpuFusionInit2Layer(const char *FlexiBinPath)
    {
        UINT32 Ret;
        flexidag_memblk_t BinBuf;
        UINT32 BinSize = 0U;
        flexidag_memblk_t StateBuf;
        UINT32 StateSize = 0U;
        flexidag_memblk_t TempBuf;
        UINT32 TempSize = 0U;
        static flexidag_memblk_t ScaAlgoBuf02;

        (void)SpuFusionV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        if (CV2_SPUFUSION_SCA_WRAPPER_UT_FLOW == 1U) {
            SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg = {NULL, 0, NULL};

            ScaQueryCfg.pAlgoObj = &AmbaSpuFusionAlgoObj;
            Ret = AmbaSpuFusionAlgoObj.Query(&ScaHandler02, &ScaQueryCfg);
            (void)RefCV_MemblkAlloc(ScaQueryCfg.TotalReqBufSz, &ScaAlgoBuf02);

            SpuFusionV1_CheckRval(AmbaWrap_memcpy(&ScaHandler02.pAlgoCtrl, &gHdlr02, sizeof(void *)), "AmbaWrap_memcpy", __func__);
            ScaHandler02.Callback[0] = RefCV_AmbaSpuFusion_UT_CallBack;
            {
                UINT32 LayerNum = 2U;
                const UINT32 *pLayerNum = &LayerNum;
                SVC_CV_ALGO_CREATE_CFG_s ScaCreateCfg = {
                    .NumFD = 0U,
                    .pBin = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                    .OutputNum = 0,
                    .OutputSz = {0, 0, 0, 0, 0, 0, 0, 0},
                    .pExtCreateCfg = NULL
                };
                //SVC_CV_ALGO_CTRL_CFG_s ScaControlCfg = {0U, NULL, NULL};

                ScaCreateCfg.pAlgoBuf = &ScaAlgoBuf02;
                ScaCreateCfg.pBin[0] = &BinBuf;
                SpuFusionV1_CheckRval(AmbaWrap_memcpy(&ScaCreateCfg.pExtCreateCfg, &pLayerNum, sizeof(UINT32*)), "AmbaWrap_memcpy", __func__);
                //ScaControlCfg.pCtrlParam = (void*)&Cfg02;

                Ret |= AmbaSpuFusionAlgoObj.Create(&ScaHandler02, &ScaCreateCfg);
                //Ret |= AmbaSpuFusionAlgoObj.Control(&ScaHandler02, &ScaControlCfg);

                gOutputNum02 = ScaCreateCfg.OutputNum;
                for (UINT32 Idex = 0; (Idex < FLEXIDAG_MAX_OUTPUTS) && (Idex < 8U); Idex ++) {
                    gOutputSz02[Idex] = ScaCreateCfg.OutputSz[Idex];
                }
            }

        } else {
            Ret = AmbaSpuFusion_Openv1(&BinBuf, BinSize, &StateSize, &TempSize, &gOutputNum02, gOutputSz02, gHdlr02);
            if (Ret == 0U) {
                AmbaPrint_PrintUInt5("OutputNum:%d", gOutputNum02, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(StateSize, &StateBuf);
                AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", StateSize, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(TempSize, &TempBuf);
                AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", TempSize, 0U, 0U, 0U, 0U);
            }

            if (Ret == 0U) {
                (void)AmbaSpuFusion_Initv1(gHdlr02, &StateBuf, &TempBuf);
                //_Config_AmbaSpuFusion(Hdlr);
            }
        }
        return Ret;
    }

    static UINT32 SpuFusionInit3Layer(const char *FlexiBinPath)
    {
        UINT32 Ret;
        flexidag_memblk_t BinBuf;
        UINT32 BinSize = 0U;
        flexidag_memblk_t StateBuf;
        UINT32 StateSize = 0U;
        flexidag_memblk_t TempBuf;
        UINT32 TempSize = 0U;
        static flexidag_memblk_t ScaAlgoBuf024;

        (void)SpuFusionV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        if (CV2_SPUFUSION_SCA_WRAPPER_UT_FLOW == 1U) {
            SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg = {NULL, 0, NULL};

            ScaQueryCfg.pAlgoObj = &AmbaSpuFusionAlgoObj;
            Ret = AmbaSpuFusionAlgoObj.Query(&ScaHandler024, &ScaQueryCfg);
            (void)RefCV_MemblkAlloc(ScaQueryCfg.TotalReqBufSz, &ScaAlgoBuf024);

            SpuFusionV1_CheckRval(AmbaWrap_memcpy(&ScaHandler024.pAlgoCtrl, &gHdlr024, sizeof(void *)), "AmbaWrap_memcpy", __func__);
            ScaHandler024.Callback[0] = RefCV_AmbaSpuFusion_UT_CallBack;
            {
                UINT32 LayerNum = 3U;
                const UINT32 *pLayerNum = &LayerNum;
                SVC_CV_ALGO_CREATE_CFG_s ScaCreateCfg = {
                    .NumFD = 3U,
                    .pBin = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                    .OutputNum = 0,
                    .OutputSz = {0, 0, 0, 0, 0, 0, 0, 0},
                    .pExtCreateCfg = NULL
                };
                //SVC_CV_ALGO_CTRL_CFG_s ScaControlCfg = {0U, NULL, NULL};

                ScaCreateCfg.pAlgoBuf = &ScaAlgoBuf024;
                ScaCreateCfg.pBin[0] = &BinBuf;
                SpuFusionV1_CheckRval(AmbaWrap_memcpy(&ScaCreateCfg.pExtCreateCfg, &pLayerNum, sizeof(UINT32*)), "AmbaWrap_memcpy", __func__);
                //ScaControlCfg.pCtrlParam = (void*)&Cfg024;

                Ret |= AmbaSpuFusionAlgoObj.Create(&ScaHandler024, &ScaCreateCfg);
                //Ret |= AmbaSpuFusionAlgoObj.Control(&ScaHandler024, &ScaControlCfg);

                gOutputNum024 = ScaCreateCfg.OutputNum;
                for (UINT32 Idex = 0; (Idex < FLEXIDAG_MAX_OUTPUTS) && (Idex < 8U); Idex ++) {
                    gOutputSz024[Idex] = ScaCreateCfg.OutputSz[Idex];
                }
            }

        } else {
            Ret = AmbaSpuFusion3Layer_Openv1(&BinBuf, BinSize, &StateSize, &TempSize, &gOutputNum024, gOutputSz024, gHdlr024);
            if (Ret == 0U) {
                AmbaPrint_PrintUInt5("OutputNum:%d", gOutputNum024, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(StateSize, &StateBuf);
                AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", StateSize, 0U, 0U, 0U, 0U);
            }
            if (Ret == 0U) {
                Ret = RefCV_MemblkAlloc(TempSize, &TempBuf);
                AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", TempSize, 0U, 0U, 0U, 0U);
            }

            if (Ret == 0U) {
                (void)AmbaSpuFusion_Initv1(gHdlr024, &StateBuf, &TempBuf);
                //_Config_AmbaSpuFusion(Hdlr);
            }

        }
        return Ret;
    }

    static void RefCV_AmbaSpuFusionV1_UT_Init(UINT32 LayerNum, const char *FlexiBinPath)
    {
        UINT32 Ret;
        static UINT32 gInit02 = 0U;
        static UINT32 gInit024 = 0U;

        static AMBA_STEREO_FD_HANDLE_s Handler02 = {0};
        static AMBA_STEREO_FD_HANDLE_s Handler024 = {0};
        gHdlr02 = CastStereoFd2Const(&Handler02);
        gHdlr024 = CastStereoFd2Const(&Handler024);

        if ( LayerNum == 2U ) {
            if (gInit02 == 0U) {
                Ret = SpuFusionInit2Layer(FlexiBinPath);
                if (Ret != 0U) {
                    AmbaPrint_PrintUInt5("SpuFusionInit2Layer Ret error", 0U, 0U, 0U, 0U, 0U);
                }
                gInit02 = 1U;
            }
        } else if ( LayerNum == 3U ) {
            if (gInit024 == 0U) {
                Ret = SpuFusionInit3Layer(FlexiBinPath);
                if (Ret != 0U) {
                    AmbaPrint_PrintUInt5("SpuFusionInit3Layer Ret error", 0U, 0U, 0U, 0U, 0U);
                }
                gInit024 = 1U;
            }
        } else {
            AmbaPrint_PrintUInt5("LayerNum = %d is not yet to supported", LayerNum, 0U, 0U, 0U, 0U);
        }

    }

    static void RefCV_AmbaSpuFusionV1_UT_Run(UINT32 LayerNum, const char* const InputFn[],
            const char* LogPath, const char* OutputFn)
    {
        static UINT32 InBufferAlloc = 0U;
        static UINT32 OutputBufferAlloc02 = 0U;
        static UINT32 OutputBufferAlloc024 = 0U;
        static flexidag_memblk_t InBuf[2];
        static flexidag_memblk_t pOutput02Buf[8];
        static flexidag_memblk_t pOutput024Buf[8];
        UINT32 rval = 0U;
        UINT32 Disp2layer_0Width = 1920UL;
        UINT32 Disp2layer_0Height = 1080UL;
        UINT32 Disp2layer_0Pitch = 2400UL;
        UINT32 Disp2layer_2Width = 960UL;
        UINT32 Disp2layer_2Height = 540UL;
        UINT32 Disp2layer_2Pitch = 1216UL;

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
        void* vp;
        UINT64 U32InBuf;
        AMBA_CV_SPU_BUF_s pInInfo0;
        AMBA_CV_IMG_BUF_s pInInfo1;
        UINT32 InputBufSize;
        AMBA_CV_SPU_BUF_s pOutputSpu;

        AMBA_CV_SPU_DATA_s *pSpuIn;
        memio_source_recv_picinfo_t *pPicIn;


        //alloc input buffer
        if ( InBufferAlloc == 0U ) {
            InputBufSize = SpuFusionV1_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                           SpuFusionV1_CacheAligned(Disp2layer_0Pitch*Disp2layer_0Height) +
                           SpuFusionV1_CacheAligned(Disp2layer_2Pitch*Disp2layer_2Height) +
                           SpuFusionV1_CacheAligned(Disp3layer_4Pitch*Disp3layer_4Height);
            rval |= RefCV_MemblkAlloc(InputBufSize, &InBuf[0]);

            InputBufSize = SpuFusionV1_CacheAligned(sizeof(memio_source_recv_picinfo_t));
            rval |= RefCV_MemblkAlloc(InputBufSize, &InBuf[1]);
            InBufferAlloc = 1U;
        }

        // alloc output buffer
        rval |= RefCV_AmbaSpuFusionV1_Alloc(LayerNum, &OutputBufferAlloc02, &OutputBufferAlloc024, pOutput02Buf, pOutput024Buf);

        if ( rval == 0U ) {

            (void)AmbaMisra_TypeCast(&pInInfo0, &InBuf[0]);
            (void)AmbaMisra_TypeCast(&pInInfo1, &InBuf[1]);
            pSpuIn = pInInfo0.pBuffer;
            pPicIn = pInInfo1.pBuffer;

            // Setup SP_DISPARITY_MAP
            {
                // Setup header
                {
                    (void)AmbaMisra_TypeCast(&pSpuIn->Reserved_0[0], &pSpuIn);
                    pSpuIn->Reserved_0[0] = MAX_HALF_OCTAVES;
                    pSpuIn->Reserved_0[1] = Disp2layer_0Height;
                    pSpuIn->Reserved_0[2] = Disp2layer_0Width;
                    pSpuIn->Reserved_0[3] = 0UL;
                }

                // Zero out all scales
                {
                    UINT32 ScaleIdx;
                    for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++) {
                        SpuFusionV1_CheckRval(AmbaWrap_memset(&(pSpuIn->Scales[ScaleIdx]), 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s)), "AmbaWrap_memset", __func__);
                        pSpuIn->Scales[ScaleIdx].Status = (INT32)0xFFFFFFFEUL; // Not valid
                    }
                }

                if ( LayerNum == 2U ) {
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
                            SpuFusionV1_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)); // offset to dispmap
                        pSpuIn->Scales[0U].Reserved_2 = 0UL;
                        pSpuIn->Scales[0U].Reserved_3 = 0UL;

                        // Cat disp map;
                        (void)AmbaMisra_TypeCast(&U32InBuf, &pSpuIn);
                        U32InBuf += pSpuIn->Scales[0U].DisparityMapOffset;
                        (void)AmbaMisra_TypeCast(&vp, &U32InBuf);
                        SpuFusionV1_ReadInputFile(InputFn[0U], vp);
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
                            SpuFusionV1_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                            SpuFusionV1_CacheAligned(Disp2layer_0Pitch*Disp2layer_0Height); // offset to dispmap
                        pSpuIn->Scales[2U].Reserved_2 = 0UL;
                        pSpuIn->Scales[2U].Reserved_3 = 0UL;

                        // Cat disp map
                        (void)AmbaMisra_TypeCast(&U32InBuf, &pSpuIn);
                        U32InBuf += pSpuIn->Scales[2U].DisparityMapOffset;
                        (void)AmbaMisra_TypeCast(&vp, &U32InBuf);
                        SpuFusionV1_ReadInputFile(InputFn[1U], vp);
                    }
                } else {
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
                            SpuFusionV1_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)); // offset to dispmap
                        pSpuIn->Scales[0U].Reserved_2 = 0UL;
                        pSpuIn->Scales[0U].Reserved_3 = 0UL;

                        // Cat disp map
                        (void)AmbaMisra_TypeCast(&U32InBuf, &pSpuIn);
                        U32InBuf += pSpuIn->Scales[0U].DisparityMapOffset;
                        (void)AmbaMisra_TypeCast(&vp, &U32InBuf);
                        SpuFusionV1_ReadInputFile(InputFn[0U], vp);
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
                            SpuFusionV1_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                            SpuFusionV1_CacheAligned(Disp3layer_0Pitch*Disp3layer_0Height); // offset to dispmap
                        pSpuIn->Scales[2U].Reserved_2 = 0UL;
                        pSpuIn->Scales[2U].Reserved_3 = 0UL;

                        // Cat disp map
                        (void)AmbaMisra_TypeCast(&U32InBuf, &pSpuIn);
                        U32InBuf += pSpuIn->Scales[2U].DisparityMapOffset;
                        (void)AmbaMisra_TypeCast(&vp, &U32InBuf);
                        SpuFusionV1_ReadInputFile(InputFn[1U], vp);
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
                            SpuFusionV1_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                            SpuFusionV1_CacheAligned(Disp3layer_2Pitch*Disp3layer_2Height) +
                            SpuFusionV1_CacheAligned(Disp3layer_0Pitch*Disp3layer_0Height); // offset to dispmap
                        pSpuIn->Scales[4U].Reserved_2 = 0UL;
                        pSpuIn->Scales[4U].Reserved_3 = 0UL;

                        // Cat disp map
                        (void)AmbaMisra_TypeCast(&U32InBuf, &pSpuIn);
                        U32InBuf += pSpuIn->Scales[4U].DisparityMapOffset;
                        (void)AmbaMisra_TypeCast(&vp, &U32InBuf);
                        SpuFusionV1_ReadInputFile(InputFn[2U], vp);
                    }
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

                if ( LayerNum == 2U ) {
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
                } else {
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
            }

            {
                const AMBA_STEREO_FD_HANDLE_s* pHdlr = NULL;
                const flexidag_memblk_t *pOutputBuf;
                SVC_CV_ALGO_HANDLE_s* pScaHandler = NULL;

                if ( LayerNum == 2U ) {
                    pHdlr = gHdlr02;
                    pOutputBuf = &pOutput02Buf[0];
                    pScaHandler = &ScaHandler02;
                } else {
                    pHdlr = gHdlr024;
                    pOutputBuf = &pOutput024Buf[0];
                    pScaHandler = &ScaHandler024;
                }

                if (CV2_SPUFUSION_SCA_WRAPPER_UT_FLOW == 1U) {
                    static AMBA_CV_FLEXIDAG_IO_s ScaFdIn;
                    static AMBA_CV_FLEXIDAG_IO_s ScaFdOut;
                    SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg = {NULL, NULL, NULL, NULL};

                    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&OutputFileName, &OutputFn, sizeof(char*)), "AmbaWrap_memcpy", __func__);

                    ScaFdIn.num_of_buf = 2;
                    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&ScaFdIn.buf[0], &InBuf[0], sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);
                    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&ScaFdIn.buf[1], &InBuf[1], sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);
                    ScaFeedCfg.pIn = &ScaFdIn;

                    ScaFdOut.num_of_buf = 1;
                    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&ScaFdOut.buf[0], pOutputBuf, sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);

                    ScaFeedCfg.pOut = &ScaFdOut;
                    rval = AmbaSpuFusionAlgoObj.Feed(pScaHandler, &ScaFeedCfg);
                    AmbaPrint_PrintUInt5("AmbaSpuFusionAlgoObj Feed : rval = %d", rval, 0U, 0U, 0U, 0U);

                    //AmbaKAL_TaskSleep(2000U);
                    //rval = AmbaSpuFusionAlgoObj.Delete(pScaHandler, NULL); //For test Delete, Init should reset to 0 at end of function
                    //AmbaPrint_PrintUInt5("AmbaSpuFusionAlgoObj Delete : rval = %d", rval, 0U, 0U, 0U, 0U);

                } else {
                    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&pInInfo0, &InBuf[0], sizeof(AMBA_CV_SPU_BUF_s)), "AmbaWrap_memcpy", __func__);
                    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&pInInfo1, &InBuf[1], sizeof(AMBA_CV_IMG_BUF_s)), "AmbaWrap_memcpy", __func__);
                    SpuFusionV1_CheckRval(AmbaWrap_memcpy(&pOutputSpu, pOutputBuf, sizeof(AMBA_CV_SPU_BUF_s)), "AmbaWrap_memcpy", __func__);

                    (void) RefCV_MemblkClean(&InBuf[0]);
                    (void) RefCV_MemblkClean(&InBuf[1]);

                    rval = AmbaSpuFusion_Processv1(pHdlr, &pInInfo0, &pInInfo1, &pOutputSpu);
                    AmbaPrint_PrintUInt5("AmbaSpuFusion_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
                    rval = AmbaSpuFusion_GetResultv1(pHdlr, &pInInfo0, &pInInfo1, &pOutputSpu);
                    AmbaPrint_PrintUInt5("AmbaSpuFusion_GetResult : rval = %d", rval, 0U, 0U, 0U, 0U);
                    SpuFusionV1_DumpSpuResult(pOutputSpu.pBuffer, OutputFn);
                    (void)AmbaSpuFusion_DumpLog(pHdlr, LogPath);
                }
            }
        }

        return;
    }

    void RefCV_AmbaSpuFusionV1_UT(const char* BinPath, const char* LogPath, UINT32 LayerNum,
                                  const char* InputScale0Fn, const char* InputScale2Fn, const char* InputScale4Fn, const char* OutputFn)
    {
        const char* InputFn[16U];

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_QNX) || defined(CONFIG_LINUX)

        static UINT32 init = 0;

        if (init == 0U) {
            AmbaPrint_PrintUInt5("AmbaCV_ProtectInit().....", 0U, 0U, 0U, 0U, 0U);
            (void) AmbaCV_ProtectInit(0x3U);
            (void) AmbaKAL_TaskSleep(300U);
            init = 1U;
        }
#endif

        InputFn[0U] = InputScale0Fn;
        InputFn[1U] = InputScale2Fn;
        InputFn[2U] = InputScale4Fn;
        AmbaPrint_PrintUInt5("RefCV_AmbaSpuFusion_UT Layer: %d", LayerNum, 0U, 0U, 0U, 0U);
        RefCV_AmbaSpuFusionV1_UT_Init(LayerNum, BinPath);
        RefCV_AmbaSpuFusionV1_UT_Run(LayerNum, InputFn, LogPath, OutputFn);
    }

#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
    static void RefCV_SignalHandlerShutdown(int sig)
    {
        printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
        exit(0);
    }

    int main(int argc, char **argv)
    {
        static UINT32 init = 0;
        uint32_t Rval = 0U;
        AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

        /* Allow ourselves to be shut down gracefully by a signal */
        signal(SIGTERM, RefCV_SignalHandlerShutdown);
        signal(SIGHUP, RefCV_SignalHandlerShutdown);
        signal(SIGUSR1, RefCV_SignalHandlerShutdown);
        signal(SIGQUIT, RefCV_SignalHandlerShutdown);
        signal(SIGINT, RefCV_SignalHandlerShutdown);
        signal(SIGKILL, RefCV_SignalHandlerShutdown);

        cfg.cpu_map = 0xD;
        cfg.log_level = LVL_DEBUG;
        AmbaCV_FlexidagSchdrStart(&cfg);

        RefCV_AmbaSpuFusionV1_UT(argv[1], argv[2], atoi(argv[3]), argv[4], argv[5], argv[6], argv[7]);

        return 0;
    }
#endif

