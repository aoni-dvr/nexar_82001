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
#include "cvapi_flexidag_ambafma_cv2.h"
#include "cvapi_svccvalgo_ambafma.h"

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
static UINT32 CV2_FMA_SCA_WRAPPER_UT_FLOW = 1U;
#else
static UINT32 CV2_FMA_SCA_WRAPPER_UT_FLOW = 0U;
#endif

static const AMBA_STEREO_FD_HANDLE_s* Hdlr = NULL;

static SVC_CV_ALGO_HANDLE_s ScaHandler;
static const char* OutputFileName = NULL;

static void FmaV1_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static const AMBA_STEREO_FD_HANDLE_s *CastStereoFd2Const(AMBA_STEREO_FD_HANDLE_s *pHdlr)
{
    const AMBA_STEREO_FD_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    FmaV1_CheckRval(AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval)), "AmbaWrap_memcpy", __func__);
    return pRval;
}

#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
static SIZE_t AmbaUtility_StringLength(const char *pString)
{
    SIZE_t Length = 0;
    if (pString == NULL) {
        Length = 0;
    } else {
        while (pString[Length] != '\0') {
            Length++;
        }
    }
    return Length;
}

static UINT32 Digit2Uint32(char Digit, UINT32 Base)
{
    UINT32 uRet = 0xFF;
    if (Base == 10U) {
        switch (Digit) {
        case '0':
            uRet = 0;
            break;
        case '1':
            uRet = 1;
            break;
        case '2':
            uRet = 2;
            break;
        case '3':
            uRet = 3;
            break;
        case '4':
            uRet = 4;
            break;
        case '5':
            uRet = 5;
            break;
        case '6':
            uRet = 6;
            break;
        case '7':
            uRet = 7;
            break;
        case '8':
            uRet = 8;
            break;
        case '9':
            uRet = 9;
            break;
        default:
            // pass vcast
            break;
        }
    } else if (Base == 16U) {
        switch (Digit) {
        case '0':
            uRet = 0;
            break;
        case '1':
            uRet = 1;
            break;
        case '2':
            uRet = 2;
            break;
        case '3':
            uRet = 3;
            break;
        case '4':
            uRet = 4;
            break;
        case '5':
            uRet = 5;
            break;
        case '6':
            uRet = 6;
            break;
        case '7':
            uRet = 7;
            break;
        case '8':
            uRet = 8;
            break;
        case '9':
            uRet = 9;
            break;
        case 'a':
            uRet = 10;
            break;
        case 'A':
            uRet = 10;
            break;
        case 'b':
            uRet = 11;
            break;
        case 'B':
            uRet = 11;
            break;
        case 'c':
            uRet = 12;
            break;
        case 'C':
            uRet = 12;
            break;
        case 'd':
            uRet = 13;
            break;
        case 'D':
            uRet = 13;
            break;
        case 'e':
            uRet = 14;
            break;
        case 'E':
            uRet = 14;
            break;
        case 'f':
            uRet = 15;
            break;
        case 'F':
            uRet = 15;
            break;
        default:
            // pass vcast
            break;
        }
    } else {
        // make vcase happy
    }
    return uRet;
}

static UINT32 AmbaUtility_StringToUInt32(const char *pString, UINT32 *pValue)
{
    UINT32 uRet = 0;
    if ((pString == NULL) || (pValue == NULL)) {
        uRet = 1;
    } else {
        SIZE_t Idex, Count, idx, Mul;
        UINT32 base = 10;
        SIZE_t Length = AmbaUtility_StringLength(pString);
        if (Length == 0U) {
            // '' return error
            uRet = 1;
        } else {
            Count = Length;
            if (Length > 2U) {
                if (((pString[0] == '0') && (pString[1] == 'x')) || ((pString[0] == '0') && (pString[1] == 'X'))) {
                    base = 16;
                    Count = Count - 2U;
                }
            }
            if ((base == 16U) && (Count == 0U)) {
                // '0x' return error
                uRet = 1;
            } else if ((base == 16U) && (Count > 8U)) {
                // '0xABCD12345678' return error
                uRet = 1;
            } else if ((base == 10U) && (Count > 10U)) {
                // max is '4294967295 = 0xFFFFFFFF' return error
                uRet = 1;
            } else {
                *pValue = 0;
                Mul = 1;
                idx = (Length - 1U);
                for (Idex = 0; Idex < Count; Idex++) {
                    UINT32 Value = Digit2Uint32(pString[idx], base);
                    if (Value == 0xFFU) {
                        // no digit
                        uRet = 1;
                        *pValue = 0;
                        break;
                    } else {
                        *pValue = *pValue + (Mul * Value);
                    }
                    idx = idx - 1U;
                    Mul = Mul * base;
                }
            }
        }
    }
    return uRet;
}

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

static UINT32 FmaV1_FileOpen(const char *pFileName, const char *pMode, AMBA_FS_FILE **pFile)
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

static UINT32 FmaV1_FileTell(AMBA_FS_FILE * pFile, UINT64* pFilePos)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileTell(pFile, pFilePos);
#else
    (*pFilePos) = ftell(pFile);
#endif
    return Ret;
}

static UINT32 FmaV1_FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileRead(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fread(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static UINT32 FmaV1_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileWrite(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fwrite(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static inline ULONG FmaV1_CacheAligned (ULONG Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("FmaV1_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}

static void FmaV1_ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 FsizeAlign;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = FmaV1_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("FmaV1_ReadInputFile FmaV1_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = FmaV1_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile FmaV1_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = FmaV1_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile FmaV1_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
#if defined(CONFIG_BUILD_CV_THREADX)
    if (Ret == 0U) {
        ULONG ULBuf;
        (void)AmbaMisra_TypeCast(&ULBuf, &buf);
        //fix input align
        FsizeAlign = FmaV1_CacheAligned(Fsize);
        Ret = AmbaCache_DataClean(ULBuf, FsizeAlign);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
#else
    AmbaMisra_TouchUnused(&Fsize);
    AmbaMisra_TouchUnused(&FsizeAlign);
#endif
}

static void FmaV1_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = FmaV1_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("FmaV1_ReadInputFile FmaV1_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = FmaV1_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile FmaV1_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("FmaV1_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 FmaV1_LoadFlexiDagBin(const char* path, flexidag_memblk_t *BinBuf, UINT32* Size)
{
    UINT32 BinSize;
    UINT32 ret = 0U;

    FmaV1_GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        ret = RefCV_MemblkAlloc(BinSize, BinBuf);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("FmaV1_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(path, BinBuf);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("FmaV1_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *Size = BinBuf->buffer_size;
        }
    } else {
        AmbaPrint_PrintUInt5("FmaV1_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
        BinBuf->pBuffer = NULL;
    }
    return ret;
}

static void FmaV1_SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;

    Ret = FmaV1_FileOpen(Fn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile FmaV1_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = FmaV1_FileWrite(Buf, 1U, Size, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile FmaV1_FileWrite fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void FmaV1_EchoFmaResult(const AMBA_CV_FMA_DATA_s *out,  UINT32 ScaleId)
{
    const AMBA_CV_FMA_DATA_s *pFma;
    ULONG ULFma, IndexAddr;
    const UINT16 *pIndex;
    UINT32 Idex;
    AmbaMisra_TypeCast(&pFma, &out);
    AmbaMisra_TypeCast(&ULFma, &pFma);
    if (pFma->StereoMatch[ScaleId].Valid != 0U) {
        IndexAddr = ULFma + pFma->StereoMatch[ScaleId].MpIndex;
        AmbaMisra_TypeCast(&pIndex, &IndexAddr);
        for (Idex = 0U; Idex < 2048U; Idex++) {
            AmbaPrint_PrintUInt5("StereoMatch[%d] = %d", Idex, pIndex[Idex], 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("StereoMatch[%d] is invalid", ScaleId, 0U, 0U, 0U, 0U);
    }

    if (pFma->TemporalMatch[ScaleId].Valid != 0U) {
        IndexAddr = ULFma + pFma->TemporalMatch[ScaleId].MpIndex;
        AmbaMisra_TypeCast(&pIndex, &IndexAddr);
        for (Idex = 0U; Idex < 2048U; Idex++) {
            AmbaPrint_PrintUInt5("TemporalMatch[%d] = %d", Idex, pIndex[Idex], 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("TemporalMatch[%d] is invalid", ScaleId, 0U, 0U, 0U, 0U);
    }
}

static void FmaV1_DumpFmaResult(const AMBA_CV_FMA_DATA_s *out, const char* OutputFn, UINT32 InputBufSz)
{
    const AMBA_CV_FMA_DATA_s *pFma;
    UINT32 Fsize;
    UINT8 *pChar;
    ULONG ULFexAddr;

    (void)AmbaMisra_TypeCast(&pFma, &out);
    Fsize = (InputBufSz - sizeof(UINT32))/2U;
    AmbaPrint_PrintUInt5("Fsize: %d", Fsize, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pFma->NumHalfOctaves: %d", pFma->NumHalfOctaves, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pFma->StereoMatch[0].Valid: %d", pFma->StereoMatch[0].Valid, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("pFma->TemporalMatch[0].Valid: %d", pFma->TemporalMatch[0].Valid, 0U, 0U, 0U, 0U);
    (void)AmbaMisra_TypeCast(&pChar, &pFma);
    FmaV1_SaveBin(pChar, OutputFn, Fsize);

    Fsize=sizeof(UINT8)*CV_FEX_MAX_BUCKETS;
    (void)AmbaMisra_TypeCast(&ULFexAddr, &pFma);
    ULFexAddr += pFma->FeatureList[0].KeypointsCountOffset;
    (void)AmbaMisra_TypeCast(&pChar, &ULFexAddr);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    FmaV1_SaveBin(pChar, "c:\\Fma_Master_Point_Count.bin", Fsize);
#elif defined(CONFIG_QNX)
    FmaV1_SaveBin(pChar, "/tmp/SD0/Fma_Master_Point_Count.bin", Fsize);
#else
    FmaV1_SaveBin(pChar, "Fma_Master_Point_Count.bin", Fsize);
#endif

    Fsize=sizeof(UINT32)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    (void)AmbaMisra_TypeCast(&ULFexAddr, &pFma);
    ULFexAddr += pFma->FeatureList[0].KeypointsOffset;
    (void)AmbaMisra_TypeCast(&pChar, &ULFexAddr);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    FmaV1_SaveBin(pChar, "c:\\Fma_Master_Point.bin", Fsize);
#elif defined(CONFIG_QNX)
    FmaV1_SaveBin(pChar, "/tmp/SD0/Fma_Master_Point.bin", Fsize);
#else
    FmaV1_SaveBin(pChar, "Fma_Master_Point.bin", Fsize);
#endif

    Fsize=sizeof(UINT8)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    (void)AmbaMisra_TypeCast(&ULFexAddr, &pFma);
    ULFexAddr += pFma->StereoMatch[0].MpScore;
    (void)AmbaMisra_TypeCast(&pChar, &ULFexAddr);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    FmaV1_SaveBin(pChar, "c:\\Fma_Stereo_Score.bin", Fsize);
#elif defined(CONFIG_QNX)
    FmaV1_SaveBin(pChar, "/tmp/SD0/Fma_Stereo_Score.bin", Fsize);
#else
    FmaV1_SaveBin(pChar, "Fma_Stereo_Score.bin", Fsize);
#endif

    Fsize=sizeof(UINT16)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    (void)AmbaMisra_TypeCast(&ULFexAddr, &pFma);
    ULFexAddr += pFma->StereoMatch[0].MpIndex;
    (void)AmbaMisra_TypeCast(&pChar, &ULFexAddr);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    FmaV1_SaveBin(pChar, "c:\\Fma_Stereo_Index.bin", Fsize);
#elif defined(CONFIG_QNX)
    FmaV1_SaveBin(pChar, "/tmp/SD0/Fma_Stereo_Index.bin", Fsize);
#else
    FmaV1_SaveBin(pChar, "Fma_Stereo_Index.bin", Fsize);
#endif

    Fsize=sizeof(UINT8)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    (void)AmbaMisra_TypeCast(&ULFexAddr, &pFma);
    ULFexAddr += pFma->TemporalMatch[0].MpScore;
    (void)AmbaMisra_TypeCast(&pChar, &ULFexAddr);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    FmaV1_SaveBin(pChar, "c:\\Fma_Temporal_Score.bin", Fsize);
#elif defined(CONFIG_QNX)
    FmaV1_SaveBin(pChar, "/tmp/SD0/Fma_Temporal_Score.bin", Fsize);
#else
    FmaV1_SaveBin(pChar, "Fma_Temporal_Score.bin", Fsize);
#endif

    Fsize=sizeof(UINT16)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    (void)AmbaMisra_TypeCast(&ULFexAddr, &pFma);
    ULFexAddr += pFma->TemporalMatch[0].MpIndex;
    (void)AmbaMisra_TypeCast(&pChar, &ULFexAddr);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    FmaV1_SaveBin(pChar, "c:\\Fma_Temporal_Index.bin", Fsize);
#elif defined(CONFIG_QNX)
    FmaV1_SaveBin(pChar, "/tmp/SD0/Fma_Temporal_Index.bin", Fsize);
#else
    FmaV1_SaveBin(pChar, "Fma_Temporal_Index.bin", Fsize);
#endif
}

static UINT32 OutputNum;
static UINT32 OutputSz[8];

static UINT32 RefCV_AmbaSpuFmaV1_UT_CallBack(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Ret = 0;
    AmbaPrint_PrintUInt5("-->   Fma SCA CallBack Event %d", Event, 0U, 0U, 0U, 0U);
    if (pEventData != NULL) {
        if (pEventData->pOutput != NULL) {
            if (Event == CALLBACK_EVENT_OUTPUT) {
                if ((pEventData->pOutput->buf[0].pBuffer != NULL) && (OutputFileName != NULL))  {
                    AMBA_CV_FMA_BUF_s OutFma;

                    FmaV1_CheckRval(AmbaWrap_memcpy(&OutFma, &pEventData->pOutput->buf[0], sizeof(AMBA_CV_FMA_BUF_s)), "AmbaWrap_memcpy", __func__);
                    FmaV1_DumpFmaResult(OutFma.pBuffer, OutputFileName, OutputSz[0]);

                } else {
                    AmbaPrint_PrintUInt5("--> Fma SCA CallBack Error: Output buffer[0] NULL", 0U, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            AmbaPrint_PrintUInt5("--> Fma SCA CallBack Error: Output data NULL", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("--> Fma SCA CallBack Error: Event data NULL", 0U, 0U, 0U, 0U, 0U);
    }

    return Ret;
}

static void RefCV_AmbaFmaV1_UT_Init(const char *FlexiBinPath)
{
    static UINT32 Init = 0U;
    UINT32 Ret = 0U;

    flexidag_memblk_t BinBuf;
    UINT32 BinSize = 0U;
    flexidag_memblk_t StateBuf;
    UINT32 StateSize = 0U;
    flexidag_memblk_t TempBuf;
    UINT32 TempSize = 0U;
    static AMBA_STEREO_FD_HANDLE_s Handler = {0};
    static flexidag_memblk_t ScaAlgoBuf;

    Hdlr = CastStereoFd2Const(&Handler);

    if ((CV2_FMA_SCA_WRAPPER_UT_FLOW == 1U) && (Init == 0U)) {
        SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg = {NULL, 1U, NULL};

        ScaQueryCfg.pAlgoObj = &AmbaFmaAlgoObj;
        Ret |= AmbaFmaAlgoObj.Query(&ScaHandler, &ScaQueryCfg);
        Ret |= RefCV_MemblkAlloc(ScaQueryCfg.TotalReqBufSz, &ScaAlgoBuf);

        (void)AmbaMisra_TypeCast(&ScaHandler.pAlgoCtrl, &Hdlr);
        ScaHandler.Callback[0] = RefCV_AmbaSpuFmaV1_UT_CallBack;
    }

    if ((CV2_FMA_SCA_WRAPPER_UT_FLOW == 1U) && (Init == 0U)) {
        SVC_CV_ALGO_CREATE_CFG_s ScaCreateCfg = {
            .NumFD = 0,
            .pBin = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
            .OutputNum = 0,
            .OutputSz = {0, 0, 0, 0, 0, 0, 0, 0},
            .pExtCreateCfg = NULL
        };

        (void)FmaV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);

        ScaCreateCfg.pAlgoBuf = &ScaAlgoBuf;
        ScaCreateCfg.pBin[0] = &BinBuf;
        Ret |= AmbaFmaAlgoObj.Create(&ScaHandler, &ScaCreateCfg);
        if (Ret != 0U) {
            AmbaPrint_PrintStr5("Err RefCV_AmbaFmaV1_UT_Init(): AmbaFmaAlgoObj.Create Fail", NULL, NULL, NULL, NULL, NULL);
        }

        OutputNum = ScaCreateCfg.OutputNum;
        for (UINT32 Idex = 0U; (Idex < FLEXIDAG_MAX_OUTPUTS) && (Idex < 8U); Idex ++) {
            OutputSz[Idex] = ScaCreateCfg.OutputSz[Idex];
        }

        Init = 1U;
    }

    if (Init == 0U) {
        (void)FmaV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        Ret = AmbaFma_Openv1(&BinBuf, BinSize, &StateSize, &TempSize, &OutputNum, OutputSz, Hdlr);
        if (Ret == 0U) {
            AmbaPrint_PrintUInt5("OutputNum:%d", OutputNum, 0U, 0U, 0U, 0U);
        }
        if (Ret == 0U) {
            if(StateSize == 0U) {
                StateSize = 1U;
            }
            Ret = RefCV_MemblkAlloc(StateSize, &StateBuf);
            AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", StateSize, 0U, 0U, 0U, 0U);
        }

        if (Ret == 0U) {
            if(TempSize == 0U) {
                TempSize = 1U;
            }
            Ret = RefCV_MemblkAlloc(TempSize, &TempBuf);
            AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", TempSize, 0U, 0U, 0U, 0U);
        }

        if (Ret == 0U) {
            (void)AmbaFma_Initv1(Hdlr, &StateBuf, &TempBuf);
            //FmaV1_Config_AmbaFma(Hdlr);
        }
        Init = 1U;
    }
    if (CV2_FMA_SCA_WRAPPER_UT_FLOW == 1U) {
        Init = 0U; // For test Delete()
    }
}

static void FmaV1_Config_AmbaFma(const AMBA_STEREO_FD_HANDLE_s* pHdlr, const UINT32 ScaleId)
{
    UINT32 Idex;
    AMBA_CV_FMA_CFG_s Cfg;
    AMBA_CV_FMA_CFG_s *pCfg = &Cfg;

    //set fma default config
    for(Idex=0; Idex<MAX_HALF_OCTAVES; ++Idex) {
        pCfg->ScaleCfg[Idex].Mode = 0U;
    }
    pCfg->ScaleCfg[ScaleId].Mode = (UINT8)CV_FMA_ENABLE_ALL;
    (void)AmbaFma_Cfg(pHdlr, pCfg);
}

static void FmaV1_Config_AmbaFmaMvac(const AMBA_STEREO_FD_HANDLE_s* pHdlr, const UINT32 ScaleId)
{
    UINT32 Idex;
    AMBA_CV_FMA_MVAC_CFG_s Cfg;
    AMBA_CV_FMA_MVAC_CFG_s *pCfg = &Cfg;

    //set fma default config
    for(Idex=0; Idex<MAX_HALF_OCTAVES; ++Idex) {
        pCfg->ScaleCfg[Idex].Mode = 0U;
    }
    pCfg->ScaleCfg[ScaleId].Mode = (UINT8)CV_FMA_ENABLE_ALL;
    pCfg->ScaleCfg[ScaleId].StereoMatch.SearchRadiusX = 1U;
    pCfg->ScaleCfg[ScaleId].StereoMatch.SearchRadiusY = 1U;
    pCfg->ScaleCfg[ScaleId].StereoMatch.DistanceThUp = 45.7f;
    pCfg->ScaleCfg[ScaleId].StereoMatch.DistanceThDown = 45.7f;
    pCfg->ScaleCfg[ScaleId].StereoMatch.MatchScoreTh = 50U;

    pCfg->ScaleCfg[ScaleId].TemporalMatch.SearchRadiusX = 1U;
    pCfg->ScaleCfg[ScaleId].TemporalMatch.SearchRadiusY = 2U;
    pCfg->ScaleCfg[ScaleId].TemporalMatch.MatchScoreTh = 50U;

    (void)AmbaFma_MvacCfg(pHdlr, pCfg);
}

static void RefCV_AmbaFmaV1_UT_Run(const char* InPath, const char* LogPath, const char* OutputFn, UINT32 ScaleId, UINT32 MvacMode, UINT32 EchoMode)
{
    static UINT32 BufferAlloc = 0U;
    static flexidag_memblk_t InBuf;
    static flexidag_memblk_t OutputBuf[8];
    UINT32 rval;
    UINT32 Idex;
    void* vp;
    UINT32 InputBufSize;
    UINT32 BinSize = 0U;

    AMBA_CV_FEX_BUF_s pInFex;
    AMBA_CV_FMA_BUF_s pOutFma;

    FmaV1_GetFileSize(InPath, &BinSize);

    //buffer constraint
    InputBufSize = FmaV1_CacheAligned(BinSize);

    if (BufferAlloc == 0U) {
        //alloc input buffer
        rval = RefCV_MemblkAlloc(InputBufSize, &InBuf);
        if(rval == 0U) {
            AmbaPrint_PrintUInt5("InputBufSize = %d", InputBufSize, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_PrintUInt5("InBuf RefCV_MemblkAlloc error", 0U, 0U, 0U, 0U, 0U);
        }

        //alloc output buffer
        if (rval == 0U) {
            for(Idex=0U; Idex<OutputNum; ++Idex) {
                rval = RefCV_MemblkAlloc(OutputSz[Idex], &OutputBuf[Idex]);
                if(rval == 0U) {
                    FmaV1_CheckRval(AmbaWrap_memset(OutputBuf[Idex].pBuffer, 0, sizeof(OutputSz[Idex])), "AmbaWrap_memset", __func__);
                    AmbaPrint_PrintUInt5("Alloc Output Buf[%d] Sz:%d", Idex, OutputSz[Idex], 0U, 0U, 0U);
                } else {
                    AmbaPrint_PrintUInt5("OutputBuf[%d] RefCV_MemblkAlloc error", Idex, 0U, 0U, 0U, 0U);
                }
            }
        }
        BufferAlloc = 1U;
    }

    //read input fex
    (void)AmbaMisra_TypeCast(&vp, &InBuf.pBuffer);
    FmaV1_ReadInputFile(InPath, vp);
    //_ReadInputFile_flex(InPath, &InBuf);

    (void)RefCV_MemblkClean(&InBuf);

    FmaV1_CheckRval(AmbaWrap_memcpy(&pInFex, &InBuf, sizeof(AMBA_CV_FEX_BUF_s)), "AmbaWrap_memcpy", __func__);
    FmaV1_CheckRval(AmbaWrap_memcpy(&pOutFma, &OutputBuf[0], sizeof(AMBA_CV_FMA_BUF_s)), "AmbaWrap_memcpy", __func__);

    if(MvacMode == 0U) {
        FmaV1_Config_AmbaFma(Hdlr, ScaleId);
    } else {
        FmaV1_Config_AmbaFmaMvac(Hdlr, ScaleId);
    }

    if (CV2_FMA_SCA_WRAPPER_UT_FLOW == 1U) {
        static AMBA_CV_FLEXIDAG_IO_s ScaFdIn;
        static AMBA_CV_FLEXIDAG_IO_s ScaFdOut;
        SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg = {NULL, NULL, NULL, NULL};

        FmaV1_CheckRval(AmbaWrap_memcpy(&OutputFileName, &OutputFn, sizeof(char*)), "AmbaWrap_memcpy", __func__);

        ScaFdIn.num_of_buf = 1;
        FmaV1_CheckRval(AmbaWrap_memcpy(&ScaFdIn.buf[0], &InBuf, sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);
        ScaFeedCfg.pIn = &ScaFdIn;

        ScaFdOut.num_of_buf = 1;
        FmaV1_CheckRval(AmbaWrap_memcpy(&ScaFdOut.buf[0], &OutputBuf[0], sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);
        ////(void)AmbaWrap_memcpy(&ScaFdOut.buf[1], &OutputBuf[1], sizeof(flexidag_memblk_t));

        ScaFeedCfg.pOut = &ScaFdOut;
        AmbaPrint_PrintUInt5("Feed Start", 0U, 0U, 0U, 0U, 0U);
        rval = AmbaFmaAlgoObj.Feed(&ScaHandler, &ScaFeedCfg);
        AmbaPrint_PrintUInt5("Fma SCA Feed : rval = %d", rval, 0U, 0U, 0U, 0U);

    } else {
        rval = AmbaFma_Processv1(Hdlr, &pInFex, InputBufSize, &pOutFma);
        AmbaPrint_PrintUInt5("AmbaFma_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
        rval = AmbaFma_GetResultv1(Hdlr, &pInFex, &pOutFma);
        AmbaPrint_PrintUInt5("AmbaFma_GetResult : rval = %d", rval, OutputNum, 0U, 0U, 0U);
        if (EchoMode == 1U) {
            FmaV1_EchoFmaResult(pOutFma.pBuffer, ScaleId);
        }
        FmaV1_DumpFmaResult(pOutFma.pBuffer, OutputFn, OutputSz[0]);
        (void)AmbaFma_DumpLog(Hdlr, LogPath);
    }
}

void RefCV_AmbaFmaV1_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, UINT32 ScaleId, UINT32 MvacMode, UINT32 EchoMode)
{
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_QNX) || defined(CONFIG_LINUX)

    static UINT32 init = 0;
    UINT32 rval = 0U;

    if (init == 0U) {
        AmbaPrint_PrintUInt5("AmbaCV_ProtectInit()...", 0U, 0U, 0U, 0U, 0U);
        (void) AmbaCV_ProtectInit(0x3U);
        (void) AmbaKAL_TaskSleep(300U);
        init = 1U;
    }

    RefCV_AmbaFmaV1_UT_Init(BinPath);
    RefCV_AmbaFmaV1_UT_Run(InFile, LogPath, OutputFn, ScaleId, MvacMode, EchoMode);

    if (CV2_FMA_SCA_WRAPPER_UT_FLOW == 1U) {
        (void)AmbaKAL_TaskSleep(2000U);
        rval |= AmbaFmaAlgoObj.Delete(&ScaHandler, NULL); //For test Delete, Init should reset to 0 at end of function
        AmbaPrint_PrintUInt5("Fma SCA Delete : rval = %d", rval, 0U, 0U, 0U, 0U);
    }

#else
    RefCV_AmbaFmaV1_UT_Init(BinPath);
    RefCV_AmbaFmaV1_UT_Run("/tmp/SD0/SD/Input/fma/FEX_0.bin", "log0", "FMA_out0.bin", ScaleId, MvacMode, EchoMode);
    RefCV_AmbaFmaV1_UT_Run("/tmp/SD0/SD/Input/fma/FEX_1.bin", "log1", "FMA_out1.bin", ScaleId, MvacMode, EchoMode);
#endif
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
    UINT32 ScaleId = 0U, MvacMode = 0U, EchoMode = 0U;

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, RefCV_SignalHandlerShutdown);
    signal(SIGHUP, RefCV_SignalHandlerShutdown);
    signal(SIGUSR1, RefCV_SignalHandlerShutdown);
    signal(SIGQUIT, RefCV_SignalHandlerShutdown);
    signal(SIGINT, RefCV_SignalHandlerShutdown);
    signal(SIGKILL, RefCV_SignalHandlerShutdown);

    /* init scheduler */
    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    if(argc == 7) {
        (void) AmbaUtility_StringToUInt32(argv[5],&ScaleId);
        (void) AmbaUtility_StringToUInt32(argv[6],&MvacMode);
    }
    RefCV_AmbaFmaV1_UT(argv[1], argv[2], argv[3], argv[4], ScaleId, MvacMode, EchoMode);

    return 0;
}
#endif

