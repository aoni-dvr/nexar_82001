/**
 *  @file RefCV.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details refcode CV
 *
 */

#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_svccvalgo_ambaspufex.h"

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
static UINT32 CV2_SPUFEX_SCA_WRAPPER_UT_FLOW = 1U;
#else
static UINT32 CV2_SPUFEX_SCA_WRAPPER_UT_FLOW = 0U;
#endif

static const AMBA_STEREO_FD_HANDLE_s* Hdlr = NULL;

static SVC_CV_ALGO_HANDLE_s ScaHandler;
static const char* OutputFileName = NULL;

static void SpuFexV1_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static const AMBA_STEREO_FD_HANDLE_s *CastStereoFd2Const(AMBA_STEREO_FD_HANDLE_s *pHdlr)
{
    const AMBA_STEREO_FD_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    SpuFexV1_CheckRval(AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval)), "AmbaWrap_memcpy", __func__);
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

static UINT32 _FileOpen(const char *pFileName, const char *pMode, AMBA_FS_FILE **pFile)
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

static UINT32 _FileTell(AMBA_FS_FILE * pFile, UINT64* pFilePos)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileTell(pFile, pFilePos);
#else
    (*pFilePos) = ftell(pFile);
#endif
    return Ret;
}

static UINT32 _FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileRead(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fread(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static UINT32 _FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    Ret = AmbaFS_FileWrite(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fwrite(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static inline UINT32 COMPUTE_PITCH (UINT32 Width)
{
    return (((Width + 31U) >> 5U) << 5U);
}

static inline ULONG SpuFexV1_CacheAligned (ULONG Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("SpuFexV1_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return ((((ULONG)Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}
static void SpuFexV1_ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 FsizeAlign;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = _FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
#if defined(CONFIG_BUILD_CV_THREADX)
    if (Ret == 0U) {
        ULONG ULBuf;
        (void)AmbaMisra_TypeCast(&ULBuf, &buf);
        //fix input align
        FsizeAlign = SpuFexV1_CacheAligned(Fsize);
        Ret = AmbaCache_DataClean(ULBuf, FsizeAlign);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
#else
    AmbaMisra_TouchUnused(&Fsize);
    AmbaMisra_TouchUnused(&FsizeAlign);
#endif
}

static void SpuFexV1_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = _FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 SpuFexV1_LoadFlexiDagBin(const char* path, flexidag_memblk_t *BinBuf, UINT32* Size)
{
    UINT32 BinSize;
    UINT32 ret = 0U;

    SpuFexV1_GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        ret = RefCV_MemblkAlloc(BinSize, BinBuf);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("SpuFexV1_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(path, BinBuf);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("SpuFexV1_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *Size = BinBuf->buffer_size;
        }
    } else {
        AmbaPrint_PrintUInt5("SpuFexV1_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
        BinBuf->pBuffer = NULL;
    }
    return ret;
}

static UINT32 OutputNum;
static UINT32 OutputSz[8];
#if 0
static void RefCV_AmbaSpuFexV1_UT_Init(const char *FlexiBinPath)
{
    static UINT32 Init = 0U;
    UINT32 Ret;

    flexidag_memblk_t BinBuf;
    UINT32 BinSize = 0U;
    flexidag_memblk_t StateBuf;
    UINT32 StateSize = 0U;
    flexidag_memblk_t TempBuf;
    UINT32 TempSize = 0U;
    static AMBA_STEREO_FD_HANDLE_s Handler = {0};
    Hdlr = CastStereoFd2Const(&Handler);

    if (Init == 0U) {
        (void)SpuFexV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        Ret = AmbaSpuFex_Openv1(&BinBuf, BinSize,&StateSize, &TempSize, &OutputNum, OutputSz, Hdlr);
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
            (void)AmbaSpuFex_Initv1(Hdlr, &StateBuf, &TempBuf);
            //SpuFexV1_Config_AmbaSpuFex(Hdlr);
        }
        Init = 1U;
    }
}
#endif

static void SpuFexV1_SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;

    Ret = _FileOpen(Fn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile _FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = _FileWrite(Buf, 1U, Size, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile _FileWrite fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_WriteOutputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void SpuFexV1_DumpSpuResult(const AMBA_CV_SPU_DATA_s *out, const char* OutputFn)
{
    const AMBA_CV_SPU_DATA_s *pSpu;
    UINT32 Fsize;
    UINT8 *pChar;

    ULONG ULSpuAddr;


    (void)AmbaMisra_TypeCast(&pSpu, &out);
    Fsize = pSpu->Scales[0].BufSize;
    AmbaPrint_PrintUInt5("pSpu->Scales[0] BufSize: %d DisparityMapOffset: %d w: %d h: %d pitch: %d", Fsize, pSpu->Scales[0].DisparityMapOffset, pSpu->Scales[0].DisparityWidth, pSpu->Scales[0].DisparityHeight, pSpu->Scales[0].DisparityPitch);
    AmbaPrint_PrintUInt5("pSpu->Scales[0] bpp: %d", pSpu->Scales[0].DisparityBpp, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("RoiStartCol: %d RoiStartRow: %d RoiAbsoluteStartCol: %d RoiAbsoluteStartRow: %d"
                         ,pSpu->Scales[0].RoiStartCol, pSpu->Scales[0].RoiStartRow, pSpu->Scales[0].RoiAbsoluteStartCol, pSpu->Scales[0].RoiAbsoluteStartRow, 0U);

    (void)AmbaMisra_TypeCast(&ULSpuAddr, &pSpu);
    ULSpuAddr += pSpu->Scales[0].DisparityMapOffset;
    (void)AmbaMisra_TypeCast(&pChar, &ULSpuAddr);
    SpuFexV1_SaveBin(pChar, OutputFn, Fsize);

}

static void SpuFexV1_DumpFexResult(const AMBA_CV_FEX_DATA_s *out)
{
    const AMBA_CV_FEX_DATA_s *pFex;
    UINT32 Fsize;
    UINT8 *pChar;
    ULONG ULpFex;

    (void)AmbaMisra_TypeCast(&pFex, &out);
    AmbaPrint_PrintUInt5("pFex Primary: %d %d Secondary: %d %d error: %d", pFex->PrimaryList[0].Enable, pFex->PrimaryList[0].Reserved, pFex->SecondaryList[0].Enable, pFex->SecondaryList[0].Reserved,  pFex->Reserved);
    AmbaPrint_PrintUInt5("pFex Primary: 0x%x 0x%x 0x%x", pFex->PrimaryList[0].KeypointsCountOffset, pFex->PrimaryList[0].KeypointsOffset, pFex->PrimaryList[0].DescriptorsOffset, 0U,  0U);
    AmbaPrint_PrintUInt5("pFex Secondary: 0x%x 0x%x 0x%x", pFex->SecondaryList[0].KeypointsCountOffset, pFex->SecondaryList[0].KeypointsOffset, pFex->SecondaryList[0].DescriptorsOffset, 0U,  0U);

    Fsize = CV_FEX_MAX_BUCKETS;
    Fsize += CV_FEX_KEYPOINTS_SIZE*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    Fsize += CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS*CV_FEX_DESCRIPTOR_SIZE;
    Fsize += CV_FEX_KEYPOINTS_SIZE*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;

    (void)AmbaMisra_TypeCast(&ULpFex, &pFex);
    ULpFex += pFex->PrimaryList[0].KeypointsCountOffset;
    (void)AmbaMisra_TypeCast(&pChar, &ULpFex);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    SpuFexV1_SaveBin(pChar, "c:\\Fex_Primary_Data.bin", Fsize);
#elif defined(CONFIG_QNX)
    SpuFexV1_SaveBin(pChar, "/tmp/SD0/Fex_Primary_Data.bin", Fsize);
#else
    SpuFexV1_SaveBin(pChar, "Fex_Primary_Data.bin", Fsize);
#endif

    (void)AmbaMisra_TypeCast(&ULpFex, &pFex);
    ULpFex += pFex->SecondaryList[0].KeypointsCountOffset;
    (void)AmbaMisra_TypeCast(&pChar, &ULpFex);
#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
    SpuFexV1_SaveBin(pChar, "c:\\Fex_Secondary_Data.bin", Fsize);
#elif defined(CONFIG_QNX)
    SpuFexV1_SaveBin(pChar, "/tmp/SD0/Fex_Secondary_Data.bin", Fsize);
#else
    SpuFexV1_SaveBin(pChar, "Fex_Secondary_Data.bin", Fsize);
#endif
}

static UINT32 RefCV_AmbaSpuFexV1_UT_CallBack(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Ret = 0;
    AmbaPrint_PrintUInt5("-->   SpuFex SCA CallBack Event %d", Event, 0U, 0U, 0U, 0U);
    if (pEventData != NULL) {
        if (pEventData->pOutput != NULL) {
            if (Event == CALLBACK_EVENT_OUTPUT) {
                if ((pEventData->pOutput->buf[0].pBuffer != NULL) && (OutputFileName != NULL))  {
                    AMBA_CV_SPU_BUF_s OutSpu;
                    AMBA_CV_FEX_BUF_s OutFex;

                    SpuFexV1_CheckRval(AmbaWrap_memcpy(&OutSpu, &pEventData->pOutput->buf[0], sizeof(AMBA_CV_SPU_BUF_s)), "AmbaWrap_memcpy", __func__);
                    SpuFexV1_DumpSpuResult(OutSpu.pBuffer, OutputFileName);

                    SpuFexV1_CheckRval(AmbaWrap_memcpy(&OutFex, &pEventData->pOutput->buf[1], sizeof(AMBA_CV_FEX_BUF_s)), "AmbaWrap_memcpy", __func__);
                    SpuFexV1_DumpFexResult(OutFex.pBuffer);

#if defined(CONFIG_BUILD_CV_THREADX)
                    SpuFexV1_SaveBin((UINT8*)OutSpu.pBuffer, "c:\\Spu.bin", OutputSz[0]);
                    SpuFexV1_SaveBin((UINT8*)OutFex.pBuffer, "c:\\Fex.bin", OutputSz[1]);
#elif defined(CONFIG_QNX)
                    SpuFexV1_SaveBin((UINT8*)OutSpu.pBuffer, "/tmp/SD0/Spu.bin", OutputSz[0]);
                    SpuFexV1_SaveBin((UINT8*)OutFex.pBuffer, "/tmp/SD0/Fex.bin", OutputSz[1]);
#else
                    SpuFexV1_SaveBin((UINT8*)OutSpu.pBuffer, "Spu.bin", OutputSz[0]);
                    SpuFexV1_SaveBin((UINT8*)OutFex.pBuffer, "Fex.bin", OutputSz[1]);
#endif

                } else {
                    AmbaPrint_PrintUInt5("--> SpuFex SCA CallBack Error: Output buffer[0] NULL", 0U, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            AmbaPrint_PrintUInt5("--> SpuFex SCA CallBack Error: Output data NULL", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("--> SpuFex SCA CallBack Error: Event data NULL", 0U, 0U, 0U, 0U, 0U);
    }

    return Ret;
}

static void RefCV_AmbaSpuFexV1_UT_Init(const char *FlexiBinPath)
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

    if ((CV2_SPUFEX_SCA_WRAPPER_UT_FLOW == 1U) && (Init == 0U)) {
        SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg = {NULL, 1U, NULL};

        ScaQueryCfg.pAlgoObj = &AmbaSpuFexAlgoObj;
        Ret |= AmbaSpuFexAlgoObj.Query(&ScaHandler, &ScaQueryCfg);
        Ret |= RefCV_MemblkAlloc(ScaQueryCfg.TotalReqBufSz, &ScaAlgoBuf);

        (void)AmbaMisra_TypeCast(&ScaHandler.pAlgoCtrl, &Hdlr);
        ScaHandler.Callback[0] = RefCV_AmbaSpuFexV1_UT_CallBack;
    }

    if ((CV2_SPUFEX_SCA_WRAPPER_UT_FLOW == 1U) && (Init == 0U)) {
        SVC_CV_ALGO_CREATE_CFG_s ScaCreateCfg = {
            .NumFD = 0,
            .pBin = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
            .OutputNum = 0,
            .OutputSz = {0, 0, 0, 0, 0, 0, 0, 0},
            .pExtCreateCfg = NULL
        };

        (void)SpuFexV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);

        ScaCreateCfg.pAlgoBuf = &ScaAlgoBuf;
        ScaCreateCfg.pBin[0] = &BinBuf;
        Ret |= AmbaSpuFexAlgoObj.Create(&ScaHandler, &ScaCreateCfg);
        if (Ret != 0U) {
            AmbaPrint_PrintStr5("Err RefCV_AmbaSpuFexV1_UT_Init(): AmbaSpuFexAlgoObj.Create Fail", NULL, NULL, NULL, NULL, NULL);
        }

        OutputNum = ScaCreateCfg.OutputNum;
        for (UINT32 Idex = 0U; (Idex < FLEXIDAG_MAX_OUTPUTS) && (Idex < 8U); Idex ++) {
            OutputSz[Idex] = ScaCreateCfg.OutputSz[Idex];
        }

        Init = 1U;
    }

    if (Init == 0U) {
        (void)SpuFexV1_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        Ret = AmbaSpuFex_Openv1(&BinBuf, BinSize,&StateSize, &TempSize, &OutputNum, OutputSz, Hdlr);
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
            (void)AmbaSpuFex_Initv1(Hdlr, &StateBuf, &TempBuf);
            //SpuFexV1_Config_AmbaSpuFex(Hdlr);
        }
        Init = 1U;
    }
}

static void SpuFexV1_Config_AmbaSpuFex(const AMBA_STEREO_FD_HANDLE_s* pHdlr, const idsp_pyramid_t *pPyramid, UINT32 FusionDisparity, const roi_t *pRoi)
{
    UINT32 Idex;
    UINT8 RoiFlag = 0U;
    AMBA_CV_SPUFEX_CFG_s Cfg;
    AMBA_CV_SPUFEX_CFG_s *pCfg = &Cfg;

    //set pyramid info
    pCfg->PyramidInfo = *pPyramid;

    if((pRoi->m_start_col == 0U) && (pRoi->m_start_row == 0U) && (pRoi->m_width_m1 == 0U) && (pRoi->m_height_m1 == 0U)) {
        RoiFlag = 0U;
        AmbaPrint_PrintUInt5("Disable ROI", 0U, 0U, 0U, 0U, 0U);
    } else {
        RoiFlag = 1U;
        AmbaPrint_PrintUInt5("Enable ROI", 0U, 0U, 0U, 0U, 0U);
    }

    //set spu default config
    pCfg->SpuScaleCfg[0].Enable = 1U;
    pCfg->SpuScaleCfg[0].RoiEnable = RoiFlag;
    pCfg->SpuScaleCfg[0].Roi = *pRoi;
    pCfg->SpuScaleCfg[0].FusionDisparity = (UINT8)FusionDisparity;

    for(Idex=1; Idex<MAX_HALF_OCTAVES; ++Idex) {
        pCfg->SpuScaleCfg[Idex].Enable = 0U;
    }

    //set fex default config
    pCfg->FexScaleCfg[0].Enable = (UINT8)CV_FEX_ENABLE_ALL;
    pCfg->FexScaleCfg[0].RoiEnable = RoiFlag;
    pCfg->FexScaleCfg[0].Roi = *pRoi;
    for(Idex=1; Idex<MAX_HALF_OCTAVES; ++Idex) {
        pCfg->FexScaleCfg[Idex].Enable = CV_FEX_DISABLE;
    }

    if (CV2_SPUFEX_SCA_WRAPPER_UT_FLOW == 1U) {
        SVC_CV_ALGO_CTRL_CFG_s CtrlCfg;
        UINT32 SpuDisplayMode = AMBA_SCA_CV_SPU_DISPLAY_MODE;
        const UINT32 *pSpuDisplayMode = &SpuDisplayMode;

        CtrlCfg.pExtCtrlCfg = NULL;

        CtrlCfg.CtrlType = AMBA_SCA_SPUFEX_CTRL_CFG;
        (void)AmbaMisra_TypeCast(&CtrlCfg.pCtrlParam, &pCfg);
        (void)AmbaSpuFexAlgoObj.Control(&ScaHandler, &CtrlCfg);

        CtrlCfg.CtrlType = AMBA_SCA_SPUFEX_CTRL_DISPLAY_MODE;
        (void)AmbaMisra_TypeCast(&CtrlCfg.pCtrlParam, &pSpuDisplayMode);
        (void)AmbaSpuFexAlgoObj.Control(&ScaHandler, &CtrlCfg);

    } else {
        (void)AmbaSpuFex_Cfg(pHdlr, pCfg);
        (void)AmbaSpuFex_SetSpuMode(pHdlr, AMBA_CV_SPU_DISPLAY_MODE);
    }
}

static void RefCV_AmbaSpuFexV1_UT_Run(const char* InPathL, const char* InPathR, const char* OutputFn, UINT32 FusionDisparity,
                                      UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight)
{
    static UINT32 BufferAlloc = 0U;
    static flexidag_memblk_t InBuf;
    static flexidag_memblk_t OutputBuf[8];
    const AMBA_CV_FEX_DATA_s *pOutFexMisra = NULL;
    UINT32 rval;
    UINT32 Idex;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    UINT32 YuvPitch = 1920U;
    roi_t Roi;
    const memio_source_recv_picinfo_t *MemIOPicInfo = NULL;
    cv_pic_info_t *PicInfo = NULL;
    void* vp;
    void* vp1;
    ULONG ULInBuf;
    UINT32 InputBufSize;

    AMBA_CV_SPU_BUF_s pOutSpu;
    AMBA_CV_FEX_BUF_s pOutFex;
    AMBA_CV_IMG_BUF_s pInInfo;

    YuvWidth = Width;
    YuvHeight = Height;
    YuvPitch = COMPUTE_PITCH(Width);
    InputBufSize = SpuFexV1_CacheAligned(sizeof(memio_source_recv_picinfo_t)) + (SpuFexV1_CacheAligned(YuvWidth*YuvHeight)*2U);

    if (BufferAlloc == 0U) {
        //alloc input buffer
        rval = RefCV_MemblkAlloc(InputBufSize, &InBuf);
        AmbaPrint_PrintUInt5("InputBufSize = %d", InputBufSize, 0U, 0U, 0U, 0U);

        // alloc output buffer
        if (rval == 0U) {
            for(Idex=0U; Idex<OutputNum; ++Idex) {
                rval = RefCV_MemblkAlloc(OutputSz[Idex], &OutputBuf[Idex]);
                AmbaPrint_PrintUInt5("Alloc Output Buf[%d] Sz:%d rval: %d", Idex, OutputSz[Idex], rval, 0U, 0U);
            }
        }
        BufferAlloc = 1U;
    }

    (void)AmbaMisra_TypeCast(&ULInBuf, &InBuf.pBuffer);
    (void)AmbaMisra_TypeCast(&MemIOPicInfo, &InBuf.pBuffer);
    (void)AmbaMisra_TypeCast(&PicInfo, &InBuf.pBuffer);

    if((RoiStartX == 0U) && (RoiStartY == 0U) && (RoiWidth == 0U) && (RoiHeight == 0U)) {
        Roi.m_start_col = 0U;
        Roi.m_start_row = 0U;
        Roi.m_width_m1 = (UINT16)YuvWidth - 1U;
        Roi.m_height_m1= (UINT16)YuvHeight - 1U;
    } else {
        Roi.m_start_col = (UINT16)RoiStartX;
        Roi.m_start_row = (UINT16)RoiStartY;
        Roi.m_width_m1 = (UINT16)RoiWidth - 1U;
        Roi.m_height_m1 = (UINT16)RoiHeight - 1U;
    }

    //PicInfo->capture_time = 0U;
    //PicInfo->channel_id = 0U;
    PicInfo->frame_num = 0U;
    PicInfo->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
    PicInfo->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
    PicInfo->pyramid.image_pitch_m1 = (YuvPitch - 1U);

    PicInfo->pyramid.half_octave[0].ctrl.disable = 0U;
    PicInfo->pyramid.half_octave[0].ctrl.mode = 1U;
    PicInfo->pyramid.half_octave[0].ctrl.octave_mode = 0U;
    PicInfo->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16)YuvPitch;
    PicInfo->pyramid.half_octave[0].roi_start_row = 0;
    PicInfo->pyramid.half_octave[0].roi_start_col = 0;
    PicInfo->pyramid.half_octave[0].roi_height_m1 = (UINT16)YuvHeight - 1U;
    PicInfo->pyramid.half_octave[0].roi_width_m1 =  (UINT16)YuvWidth - 1U;
    PicInfo->pyramid.half_octave[1].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[2].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[3].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[4].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[5].ctrl.disable = 1U;

    //read left Y
    ULInBuf += SpuFexV1_CacheAligned(sizeof(memio_source_recv_picinfo_t));
    (void)AmbaMisra_TypeCast(&vp, &ULInBuf);
    SpuFexV1_ReadInputFile(InPathL, vp);

    //read right Y
    ULInBuf += SpuFexV1_CacheAligned(YuvPitch*YuvHeight);
    (void)AmbaMisra_TypeCast(&vp1, &ULInBuf);
    SpuFexV1_ReadInputFile(InPathR, vp1);

    //assign relate ptr
    ULInBuf = SpuFexV1_CacheAligned(sizeof(memio_source_recv_picinfo_t));
    (void)AmbaMisra_TypeCast(&(PicInfo->rpLumaLeft[0]), &ULInBuf);
    ULInBuf += SpuFexV1_CacheAligned(YuvPitch*YuvHeight);
    (void)AmbaMisra_TypeCast(&(PicInfo->rpLumaRight[0]), &ULInBuf);

    (void)RefCV_MemblkClean(&InBuf);

    SpuFexV1_CheckRval(AmbaWrap_memcpy(&pInInfo, &InBuf, sizeof(AMBA_CV_IMG_BUF_s)), "AmbaWrap_memcpy", __func__);
    SpuFexV1_CheckRval(AmbaWrap_memcpy(&pOutSpu, &OutputBuf[0], sizeof(AMBA_CV_SPU_BUF_s)), "AmbaWrap_memcpy", __func__);
    SpuFexV1_CheckRval(AmbaWrap_memcpy(&pOutFex, &OutputBuf[1], sizeof(AMBA_CV_FEX_BUF_s)), "AmbaWrap_memcpy", __func__);

    SpuFexV1_Config_AmbaSpuFex(Hdlr, &PicInfo->pyramid, FusionDisparity, &Roi);

    if (CV2_SPUFEX_SCA_WRAPPER_UT_FLOW == 1U) {
        static AMBA_CV_FLEXIDAG_IO_s ScaFdIn;
        static AMBA_CV_FLEXIDAG_IO_s ScaFdOut;
        SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg = {NULL, NULL, NULL, NULL};

        SpuFexV1_CheckRval(AmbaWrap_memcpy(&OutputFileName, &OutputFn, sizeof(char*)), "AmbaWrap_memcpy", __func__);

        ScaFdIn.num_of_buf = 1;
        SpuFexV1_CheckRval(AmbaWrap_memcpy(&ScaFdIn.buf[0], &InBuf, sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);
        ScaFeedCfg.pIn = &ScaFdIn;

        ScaFdOut.num_of_buf = 2;
        SpuFexV1_CheckRval(AmbaWrap_memcpy(&ScaFdOut.buf[0], &OutputBuf[0], sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);
        SpuFexV1_CheckRval(AmbaWrap_memcpy(&ScaFdOut.buf[1], &OutputBuf[1], sizeof(flexidag_memblk_t)), "AmbaWrap_memcpy", __func__);

        ScaFeedCfg.pOut = &ScaFdOut;
        AmbaPrint_PrintUInt5("Feed Start", 0U, 0U, 0U, 0U, 0U);
        rval = AmbaSpuFexAlgoObj.Feed(&ScaHandler, &ScaFeedCfg);
        AmbaPrint_PrintUInt5("SpuFex SCA Feed : rval = %d", rval, 0U, 0U, 0U, 0U);

        //AmbaKAL_TaskSleep(2000U);
        //rval = AmbaSpuFexAlgoObj.Delete(&ScaHandler, NULL); //For test Delete, Init should reset to 0 at end of function
        //AmbaPrint_PrintUInt5("SpuFex SCA Delete : rval = %d", rval, 0U, 0U, 0U, 0U);

    } else {
        rval = AmbaSpuFex_Processv1(Hdlr, &pInInfo, &pOutSpu, &pOutFex);
        AmbaPrint_PrintUInt5("AmbaSpuFex_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
        rval = AmbaSpuFex_GetResultv1(Hdlr, &pInInfo, &pOutSpu, &pOutFex);
        AmbaPrint_PrintUInt5("AmbaSpuFex_GetResult : rval = %d", rval, OutputNum, 0U, 0U, 0U);

        SpuFexV1_DumpSpuResult(pOutSpu.pBuffer, OutputFn);
        (void)AmbaMisra_TypeCast(&pOutFexMisra, &pOutFex.pBuffer);
        SpuFexV1_DumpFexResult(pOutFexMisra);

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_LINUX)
        SpuFexV1_SaveBin((UINT8*)pOutSpu.pBuffer, "c:\\Spu.bin", OutputSz[0]);
        SpuFexV1_SaveBin((UINT8*)pOutFex.pBuffer, "c:\\Fex.bin", OutputSz[1]);
        (void)AmbaSpuFex_DumpLog(Hdlr, "c:\\log");
#elif defined(CONFIG_QNX)
        SpuFexV1_SaveBin((UINT8*)pOutSpu.pBuffer, "/tmp/SD0/Spu.bin", OutputSz[0]);
        SpuFexV1_SaveBin((UINT8*)pOutFex.pBuffer, "/tmp/SD0/Fex.bin", OutputSz[1]);
        (void)AmbaSpuFex_DumpLog(Hdlr, "/tmp/SD0/log");
#else
        SpuFexV1_SaveBin((UINT8*)pOutSpu.pBuffer, "Spu.bin", OutputSz[0]);
        SpuFexV1_SaveBin((UINT8*)pOutFex.pBuffer, "Fex.bin", OutputSz[1]);
        (void)AmbaSpuFex_DumpLog(Hdlr, "log");
#endif

    }
}

void RefCV_AmbaSpuFexV1_UT(const char* BinPath, const char* InFileL, const char* InFileR, const char* OutputFn, UINT32 FusionDisparity,
                           UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight)
{
#if (defined CONFIG_BUILD_CV_THREADX || defined CONFIG_QNX)
    static UINT32 init = 0;

    if (init == 0U) {
        AmbaPrint_PrintUInt5("AmbaCV_ProtectInit().....", 0U, 0U, 0U, 0U, 0U);
        (void) AmbaCV_ProtectInit(0x3U);
        (void) AmbaKAL_TaskSleep(300U);
        init = 1U;
    }
#endif
    RefCV_AmbaSpuFexV1_UT_Init(BinPath);
    RefCV_AmbaSpuFexV1_UT_Run(InFileL, InFileR, OutputFn, FusionDisparity, Width, Height, RoiStartX, RoiStartY, RoiWidth, RoiHeight);
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

    //Allow ourselves to be shut down gracefully by a signal
    signal(SIGTERM, RefCV_SignalHandlerShutdown);
    signal(SIGHUP, RefCV_SignalHandlerShutdown);
    signal(SIGUSR1, RefCV_SignalHandlerShutdown);
    signal(SIGQUIT, RefCV_SignalHandlerShutdown);
    signal(SIGINT, RefCV_SignalHandlerShutdown);
    signal(SIGKILL, RefCV_SignalHandlerShutdown);

    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    UINT32 Width, Height;
    UINT32 RoiStartX, RoiStartY, RoiWidth, RoiHeight, FusionOutput;
    (void) AmbaUtility_StringToUInt32(argv[5],&FusionOutput);
    (void) AmbaUtility_StringToUInt32(argv[6],&Width);
    (void) AmbaUtility_StringToUInt32(argv[7],&Height);
    (void) AmbaUtility_StringToUInt32(argv[8],&RoiStartX);
    (void) AmbaUtility_StringToUInt32(argv[9],&RoiStartY);
    (void) AmbaUtility_StringToUInt32(argv[10],&RoiWidth);
    (void) AmbaUtility_StringToUInt32(argv[11],&RoiHeight);

    RefCV_AmbaSpuFexV1_UT(argv[1], argv[2], argv[3], argv[4], FusionOutput,
                          Width, Height, RoiStartX, RoiStartY, RoiWidth, RoiHeight);

    return 0;
}
#endif

