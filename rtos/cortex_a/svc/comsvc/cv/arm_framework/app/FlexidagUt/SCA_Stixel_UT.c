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
 *  @details SVC COMSVC CV
 *
 */

#ifdef CONFIG_BUILD_CV_THREADX
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
#include "idsp_vis_msg.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_stixel.h"
#include "CameraPitchDetection.h"
#include "AmbaTMR.h"
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
#include "schdr_api.h"
#include "cvtask_api.h"
#include "ambint.h"
#include "cvtask_ossrv.h"
#include "cavalry_ioctl.h"
#include <math.h>
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "idsp_vis_msg.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_stixel.h"
#include "CameraPitchDetection.h"
#define AMBA_CACHE_LINE_SIZE    64U
#define AMBA_FS_FILE FILE
#define AMBA_FS_SEEK_CUR SEEK_CUR
#define AMBA_FS_SEEK_START SEEK_SET
#define AMBA_FS_SEEK_END SEEK_END
#define AmbaFS_FileClose fclose
#define AmbaFS_FileSeek fseek
#define AmbaMMU_Virt32ToPhys32 //for ambalink build
#define AmbaPrint_PrintInt5 AmbaPrint_PrintUInt5
#define UTIL_MAX_INT_STR_LEN (22U)
#define LIBWRAP_ERR_INVAL (0x02000000U + 0x00010000U)
#endif

#ifdef CONFIG_BUILD_CV_THREADX
#define AmbaMisra_TypeCast AmbaMisra_TypeCast32
#else
#define AmbaMisra_TypeCast AmbaMisra_TypeCast64
#endif

#ifndef CONFIG_BUILD_CV_THREADX
#ifndef CONFIG_QNX
static uint32_t RefCV_MemblkClean(const flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemClean(buf);
    if(ret != 0U) {
        printf("RefCV_MemblkClean : AmbaCV_UtilityCmaMemClean fail \n");
    }
    return ret;
}

static uint32_t RefCV_MemblkInvalid(const flexidag_memblk_t* buf)
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

static UINT32 RefCV_UT_GetCVBuf(UINT8** ppU8, UINT32 Size, UINT32* AlignedSize)
{
    UINT32 Ret = 0U;
    flexidag_memblk_t buf;

    buf.pBuffer = NULL;
    buf.buffer_daddr = 0U;
    buf.buffer_cacheable = 0U;
    buf.buffer_size = 0U;
    buf.buffer_caddr = 0U;
    buf.reserved_expansion = 0U;
    Ret = RefCV_MemblkAlloc(Size, &buf);
    if (Ret == 0U) {
        AmbaMisra_TypeCast(ppU8, &buf.pBuffer);
        *AlignedSize = buf.buffer_size;
    } else {
        AmbaPrint_PrintUInt5("RefCV_UT_GetCVBuf: OOM", 0U, 0U, 0U, 0U, 0U);
    }
    return Ret;
}

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

static void AmbaUtility_StringAppend(char *pBuffer, UINT32 BufferSize, const char *pSource)
{
    if ((pBuffer == NULL) || (pSource == NULL) || (BufferSize == 0U)) {
        // no action here
    } else {
        SIZE_t StringLengthDest = AmbaUtility_StringLength(pBuffer);
        SIZE_t StringLengthCopy = AmbaUtility_StringLength(pSource);
        SIZE_t i;

        if ((StringLengthCopy >= (BufferSize - 1U)) || (StringLengthDest >= (BufferSize - 1U))) {
            // buffer overflow, no action here
        } else {
            if ((StringLengthDest + StringLengthCopy) > (BufferSize - 1U)) {
                StringLengthCopy = (BufferSize - StringLengthDest) - 1U;
            }
            for (i = 0; i < StringLengthCopy; i++) {
                pBuffer[i + StringLengthDest] = pSource[i];
            }
            if ((StringLengthDest + StringLengthCopy) <= (BufferSize - 1U)) {
                pBuffer[StringLengthDest + StringLengthCopy] = '\0';
            } else {
                pBuffer[BufferSize - 1U] = '\0';
            }
        }
    }
}

static char num_base16_full[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static char UInt32ToDigit(UINT32 Value)
{
    char cRet = '0';

    if (Value < 16U) {
        cRet = num_base16_full[Value];
    }
    return cRet;
}

static UINT32 AmbaUtility_Int64ToStr(char *pBuffer, UINT32 BufferSize, INT64 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    INT64  t = Value;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix;
    UINT64 ut, uTmp;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (t == 0) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        //----------------------------------------------------------
        if ((t < 0) && (Base == 10U)) {
            t = -t;
            pWorkChar2[idx2] = '-';
            idx2++;
            pWorkChar2[idx2 + 1U] = '\0';   /* attach the null terminator */
        }

        ut = (UINT64)t;
        pWorkChar1 = pWorkChar2;
        idx1 = idx2;

        while (ut > 0U) {
            uTmp = ut % (UINT64)Base;
            pWorkChar1[idx1] = UInt32ToDigit((UINT32)uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= (UINT64)Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength = idx1;

        // change the order of the digits
        swap_len = (idx1 - idx2) / 2U;  /* n = lenght of the string / 2 */
        while (swap_len > 0U) {
            if (idx1 > 0U) {
                idx1--;
            }
            Tmp = pWorkChar1[idx1];
            pWorkChar1[idx1] = pWorkChar2[idx2];
            pWorkChar2[idx2] = Tmp;
            idx2++;
            swap_len--;
        }
    }

    /* make sure at least a \0 is added.*/
    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }

    return StrLength;
}

static INT64 PowerX(INT32 Base, INT32 X)
{
    INT64 result = 1;
    INT32 idx = X;
    while(idx > 0) {
        result *= Base;
        idx--;
    }
    return result;
}

static UINT32 AmbaUtility_UInt64ToStr(char *pBuffer, UINT32 BufferSize, UINT64 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    UINT64 ut = Value, uTmp;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (ut == 0U) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength = 1;
    } else {
        pWorkChar1 = pWorkChar2;
        idx1 = idx2;

        while (ut > 0U) {
            uTmp = ut % (UINT64)Base;
            pWorkChar1[idx1] = UInt32ToDigit((UINT32)uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= (UINT64)Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength = idx1;

        // change the order of the digits
        swap_len = idx1 / 2U;  /* n = lenght of the string / 2 */
        while (swap_len > 0U) {
            if (idx1 > 0U) {
                idx1--;
            }
            Tmp = pWorkChar1[idx1];
            pWorkChar1[idx1] = pWorkChar2[idx2];
            pWorkChar2[idx2] = Tmp;
            idx2++;
            swap_len--;
        }
    }

    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }
    return StrLength;
}

static UINT32 AmbaUtility_DoubleToStr(char *pBuffer, UINT32 BufferSize, DOUBLE Value, UINT32 Afterpoint)
{
    // Extract integer part
    INT64 ipart = (INT64)Value;
    UINT64 uint64_value;
    UINT32 string_length = 0, tmp_len;
    DOUBLE fpart;
    char   uint32_string[UTIL_MAX_INT_STR_LEN];

    // Extract floating part
    fpart = Value - (DOUBLE)ipart;

    // convert integer part to string
    if (ipart == 0) {
        // There is no -0, handle it manually.
        if (BufferSize >= 2U) {
            pBuffer[0] = '\0';
            if (Value < 0.0) {
                AmbaUtility_StringAppend(pBuffer, BufferSize, "-");
                string_length++;
            }
            AmbaUtility_StringAppend(pBuffer, BufferSize, "0");
            string_length++;
        }
    } else {
        string_length = AmbaUtility_Int64ToStr(pBuffer, BufferSize, ipart, 10);
    }

    if (string_length < (BufferSize - 1U)) {
        // check for display option after point
        if (Afterpoint != 0U) {
            pBuffer[string_length] = '.';  // add dot
            string_length++;
            pBuffer[string_length] = '\0';
            if (string_length < (BufferSize - 1U)) {
                // Get the value of fraction part upto given no.
                // of points after dot. The third parameter is needed
                // to handle cases like 233.007
                fpart = fpart * (DOUBLE)PowerX(10, (INT32)Afterpoint);
                if (fpart < 0.0) {
                    fpart *= (DOUBLE)-1;
                }
                uint64_value = (UINT64)fpart;
                tmp_len = AmbaUtility_UInt64ToStr(uint32_string, UTIL_MAX_INT_STR_LEN, uint64_value, 10);
                if (tmp_len < Afterpoint) {
                    // add '0'
                    UINT32 i, count;
                    count = Afterpoint - tmp_len;
                    if ((string_length + count) < BufferSize) {
                        for (i = 0; i < count; i++) {
                            pBuffer[string_length] = '0';
                            string_length++;
                        }
                        pBuffer[string_length] = '\0';
                    }
                }
                AmbaUtility_StringAppend(pBuffer, BufferSize, uint32_string);
                if ((string_length + tmp_len) < BufferSize) {
                    string_length += tmp_len;
                }
            }
        }
    }
    return string_length;
}

static UINT32 AmbaWrap_sin(DOUBLE x, void *pV)
{
    UINT32 err = 0;
    DOUBLE v;

    if (pV == NULL) {
        err = LIBWRAP_ERR_INVAL;
    } else {
        v = sin(x);
        err = AmbaWrap_memcpy(pV, &v, sizeof(v));
    }

    return err;
}

static UINT32 AmbaWrap_cos(DOUBLE x, void *pV)
{
    UINT32 err = 0;
    DOUBLE v;

    if (pV == NULL) {
        err = LIBWRAP_ERR_INVAL;
    } else {
        v = cos(x);
        err = AmbaWrap_memcpy(pV, &v, sizeof(v));
    }

    return err;
}
#endif
#endif

static UINT32 _CacheAlign(UINT32 Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("_CacheAlign : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}

static uint32_t RefCV_MemblkAllocv1(uint32_t buf_size, UINT8** ppU8, flexidag_memblk_t* buf, UINT32* AlignedSize)
{
    uint32_t ret = 0U;

    ret = RefCV_MemblkAlloc(buf_size, buf);
    AmbaMisra_TypeCast(ppU8, &buf->pBuffer);
    *AlignedSize = buf->buffer_size;

    return ret;
}

static UINT32 _FileOpen(const char *pFileName, const char *pMode, AMBA_FS_FILE **pFile)
{
    UINT32 Ret = 0U;

#if (defined CONFIG_BUILD_CV_THREADX || defined CONFIG_QNX)
    Ret = AmbaFS_FileOpen(pFileName, pMode, pFile);
#else
    *pFile = fopen(pFileName, pMode);
    if (*pFile == NULL) {
        Ret = 1U;
    } else {
        Ret = 0U;
    }
#endif
    return Ret;
}

static UINT32 _FileTell(AMBA_FS_FILE * pFile, UINT64* pFilePos)
{
    UINT32 Ret = 0;

#if (defined CONFIG_BUILD_CV_THREADX || defined CONFIG_QNX)
    Ret = AmbaFS_FileTell(pFile, pFilePos);
#else
    (*pFilePos) = ftell(pFile);
#endif
    return Ret;
}

static UINT32 _FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if (defined CONFIG_BUILD_CV_THREADX || defined CONFIG_QNX)
    Ret = AmbaFS_FileRead(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fread(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static UINT32 _FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;

#if (defined CONFIG_BUILD_CV_THREADX || defined CONFIG_QNX)
    Ret = AmbaFS_FileWrite(pBuf, Size, Count, pFile, pNumSuccess);
#else
    (*pNumSuccess) = fwrite(pBuf, Size, Count, pFile);
#endif
    return Ret;
}

static UINT32 _TaskSleep(UINT32 NumTicks)
{
    UINT32 ret = 0U;

#if (defined CONFIG_BUILD_CV_THREADX || defined CONFIG_QNX)
    ret = AmbaKAL_TaskSleep(NumTicks);
#else
    ret = usleep(NumTicks*1000);
#endif
    if(ret != 0U) {
        AmbaPrint_PrintUInt5("_TaskSleep fail \n", 0U, 0U, 0U, 0U, 0U);
    }
    return ret;
}

// Stixel configuration
//---------------------------------------------
#define UT_CFG_FOCAL_LENGTH_U              (1327.1875)
#define UT_CFG_FOCAL_LENGTH_V              (1327.1875)
#define UT_CFG_OPTICAL_CENTER_X            (917.0)
#define UT_CFG_OPTICAL_CENTER_Y            (489.3125)
#define UT_CFG_BASELINE                    (0.3003000020980835)

#define UT_CFG_DEFAULT_CAMERA_HEIGH        (1.520587)
#define UT_CFG_DEFAULT_CAMERA_PITCH        (0.073589)

#define UT_CFG_ROAD_SEARCH_PITCH_STRIDE    (0.25)
#define UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE   (0.02)
#define UT_CFG_ROAD_SEARCH_PITCH_NUM       (64U)
#define UT_CFG_ROAD_SEARCH_HEIGHT_NUM      (10U)

#define UT_CFG_VDISP_X_THR                   (0.2)
#define UT_CFG_VDISP_Y_THR                   (0.15)

#define UT_CFG_DET_OBJECT_HEIGHT           (1.0)
#define UT_CFG_MIN_DISPARITY               (6U)
//---------------------------------------------

#define FX_INDEX_VDISP              (0U)
#define FX_INDEX_GEN_ESTI_ROAD      (1U)
#define FX_INDEX_PREPROCESS         (2U)
#define FX_INDEX_FREE_SPACE         (3U)
#define FX_INDEX_HEIGHT_SEG         (4U)
#define FX_NUM                      (5U)

#define MAX_FX_IN_BUFFER_NUM        (8U)
#define MAX_FX_OUT_BUFFER_NUM       (8U)

static SVC_CV_ALGO_HANDLE_s*     gHdlr[FX_NUM];

static UINT32 gOutputNum[FX_NUM];
static UINT32 gOutputSz[FX_NUM][MAX_FX_OUT_BUFFER_NUM];

// Flag indicating if output buffer for each flexiDag is ready
static UINT32 gStep0_VDispCompleted;
static UINT32 gStep1_GenEstiRoadCompleted;
static UINT32 gStep2_PreprocessCompleted;
static UINT32 gStep3_FreeSpaceCompleted;
static UINT32 gStep4_HeightSegCompleted;

static UINT32 gDebugDumpFlag = 0U;

#ifdef CONFIG_BUILD_CV_THREADX
static UINT32 gRefClkTmrId;
static void _UsecTimerStart(void)
{
    UINT32 Rval;
    UINT32 TimeOut = 5000U;

    //get timer as reference clock
    Rval = AmbaTMR_FindAvail(&gRefClkTmrId, TimeOut);
    if (Rval != OK) {
        AmbaPrint_PrintStr5("AmbaTMR_FindAvail as reference failed!", NULL, NULL, NULL, NULL, NULL);
    } else {
        Rval = AmbaTMR_Acquire(gRefClkTmrId, TimeOut);
        if (Rval != OK) {
            AmbaPrint_PrintStr5("AmbaTMR_Acquire TimerId %u as reference failed!", NULL, NULL, NULL, NULL, NULL);
        }
        // Config usec
        if (Rval == OK) {
            Rval = AmbaTMR_Config(gRefClkTmrId, 1000000, 0x1000000U);
            if (Rval != OK) {
                AmbaPrint_PrintStr5("AmbaTMR_Config TimerId %u as reference failed!", NULL, NULL, NULL, NULL, NULL);
            }
        }
        if (Rval == OK) {
            Rval = AmbaTMR_Start(gRefClkTmrId, 0x1000000U);
            if (Rval != OK) {
                AmbaPrint_PrintStr5("AmbaTMR_Start TimerId %u as reference failed!", NULL, NULL, NULL, NULL, NULL);
            } else {
                AmbaPrint_PrintStr5("_UsecTimerStart", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
}

// For usec
static UINT32 _UsecTimerGetTick(UINT32 *pTimestamp)
{
    UINT32 TimeLeft;
    UINT32 Rval = AmbaTMR_ShowTickCount(gRefClkTmrId, &TimeLeft);
    if (Rval == OK) {
        *pTimestamp = 0xFFFFFFU - (TimeLeft & 0xFFFFFFU);
    } else {
        Rval = 1;
        AmbaPrint_PrintStr5("AmbaTMR_ShowTickCount failed!", NULL, NULL, NULL, NULL, NULL);
    }
    return Rval;
}
#endif

static void _SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
{
    UINT32 Ret;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;


    Ret = _FileOpen(Fn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_WriteOutputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = _FileWrite(Buf, 1U, Size, Fp, &OpRes);
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

static void _DebugSave(UINT32 Debug, UINT32 FileNum, const char* const SaveNameSet[],
                       const flexidag_memblk_t FlexOutBuf[], const UINT32 FileSizeSet[])
{
    UINT32 FileIdx;
    UINT8 *pU8;

    if ( Debug == 1U ) {
        for (FileIdx = 0U; FileIdx < FileNum; FileIdx++) {
            AmbaMisra_TypeCast(&pU8, &FlexOutBuf[FileIdx].pBuffer);
            _SaveBin(pU8, SaveNameSet[FileIdx], FileSizeSet[FileIdx]);
        }
    }
    return;
}

static void _BlockWaitCompleted(const UINT32 *pCompleteFlag, UINT32 TimeOut)
{
    INT32 RemainTime = (INT32)TimeOut;
    UINT32 SleepTime = 1U;

    while ( (*pCompleteFlag == 0U) && (RemainTime > 0) ) {
        (void) _TaskSleep(SleepTime);
        RemainTime -= (INT32)SleepTime;
    }
    return;
}

static void _ReadInputFile(const char* path, void* buf, const flexidag_memblk_t* flexbuf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = _FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = RefCV_MemblkClean(flexbuf);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}


static void _GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = _FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static void _FiLLFlexiDagMemStruct(const flexidag_memblk_t *MemBlkIn, flexidag_memblk_t *MemBlkOut)
{
    MemBlkOut->buffer_cacheable = MemBlkIn->buffer_cacheable;
    MemBlkOut->pBuffer = MemBlkIn->pBuffer;
    MemBlkOut->buffer_size = MemBlkIn->buffer_size;
    MemBlkOut->buffer_daddr = MemBlkIn->buffer_daddr;
}

static UINT32 _LoadFlexiDagBin(const char* path, flexidag_memblk_t *BinBuf, UINT32* Size)
{
    UINT32 BinSize;
    UINT32 ret = 0U;

    _GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        ret = RefCV_MemblkAlloc(BinSize, BinBuf);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(path, BinBuf);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *Size = BinBuf->buffer_size;
        }
    } else {
        AmbaPrint_PrintUInt5("_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
        BinBuf->pBuffer = NULL;
    }
    return ret;
}

static UINT32 SCA_VDisp_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    (void)pEventData;
    if (Event == CALLBACK_EVENT_OUTPUT) {
        gStep0_VDispCompleted = 1U;
    }
    return 0U;
}

static UINT32 SCA_GenEstiRoad_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    (void)pEventData;
    if (Event == CALLBACK_EVENT_OUTPUT) {
        gStep1_GenEstiRoadCompleted = 1U;
    }
    return 0U;
}

static UINT32 SCA_Preprocess_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    (void)pEventData;
    if (Event == CALLBACK_EVENT_OUTPUT) {
        gStep2_PreprocessCompleted = 1U;
    }
    return 0U;
}

static UINT32 SCA_FreeSpace_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    (void)pEventData;
    if (Event == CALLBACK_EVENT_OUTPUT) {
        gStep3_FreeSpaceCompleted = 1U;
    }
    return 0U;
}

static UINT32 SCA_HeightSeg_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    (void)pEventData;
    if (Event == CALLBACK_EVENT_OUTPUT) {
        gStep4_HeightSegCompleted = 1U;
    }
    return 0U;
}

static UINT32 _RunFlexiDag(AMBA_CV_FLEXIDAG_IO_s *InFxBuf, const flexidag_memblk_t FlexInBuf[],
                           UINT32 InBuffNum, const ulong InBuffCaddr[], const UINT32 InBuffSize[], const UINT32 InBuffPitch[],
                           AMBA_CV_FLEXIDAG_IO_s *OutFxBuf, const flexidag_memblk_t FlexOutBuf[],
                           UINT32 OutBuffNum)
{
    UINT32 Ret = 0U;
    UINT32 InputIdx;
    UINT32 OutputIdx;

    // fill AMBA_CV_FLEXIDAG_IO_s: in structure
    InFxBuf->num_of_buf = InBuffNum;
    for (InputIdx = 0U; InputIdx < InBuffNum; InputIdx++) {
        memio_source_recv_raw_t *pMemIORawInfo;
        AmbaMisra_TypeCast(&pMemIORawInfo, &FlexInBuf[InputIdx].pBuffer);
        pMemIORawInfo->magic = 0U;
        pMemIORawInfo->addr = InBuffCaddr[InputIdx];
        pMemIORawInfo->size = InBuffSize[InputIdx];
        pMemIORawInfo->pitch = InBuffPitch[InputIdx];
        pMemIORawInfo->rsv = 0U;
        _FiLLFlexiDagMemStruct(&FlexInBuf[InputIdx], &InFxBuf->buf[InputIdx]);
        (void) RefCV_MemblkClean(&InFxBuf->buf[InputIdx]);
    }

    // fill AMBA_CV_FLEXIDAG_IO_s: out structure
    OutFxBuf->num_of_buf = OutBuffNum;
    for (OutputIdx = 0U; OutputIdx < OutBuffNum; OutputIdx++) {
        _FiLLFlexiDagMemStruct(&FlexOutBuf[OutputIdx], &OutFxBuf->buf[OutputIdx]);
        (void) RefCV_MemblkInvalid(&OutFxBuf->buf[OutputIdx]);
    }

    return Ret;
}

static UINT32 SCA_Stixel_UT_Init(const char *FlexiBinPath,
                                 SVC_CV_ALGO_OBJ_s *pAlgoObj,
                                 UINT32 (*pCbFxn)(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData),
                                 flexidag_memblk_t *pFxAlgoBuf,
                                 flexidag_memblk_t *pFxFdBinBuf,
                                 SVC_CV_ALGO_HANDLE_s *pHdlr,
                                 UINT32 *pOutputNum, UINT32 OutputSz[])
{
    UINT32 AlignSz = 0U;
    UINT32 Ret;
    UINT32 AlgoBufSz = 0U;
    UINT32 BinSize = 0U;
    SVC_CV_ALGO_QUERY_CFG_s QCfg;
    SVC_CV_ALGO_CREATE_CFG_s CCfg;

    (void)AmbaWrap_memset(pHdlr, 0, sizeof(SVC_CV_ALGO_HANDLE_s));
    QCfg.pAlgoObj = pAlgoObj;
    Ret = SvcCvAlgo_Query(pHdlr, &QCfg);

    if (Ret == 0U) {
        AlgoBufSz = QCfg.TotalReqBufSz;
        Ret |= RefCV_MemblkAlloc(AlgoBufSz, pFxAlgoBuf);
        AlignSz = pFxAlgoBuf->buffer_size;
        AmbaPrint_PrintUInt5("Alloc Algo Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
    }

    if (Ret == 0U) {
        UINT32 I;
        CCfg.NumFD = 1;
        CCfg.pAlgoBuf = pFxAlgoBuf;
        CCfg.pBin[0] = pFxFdBinBuf;
        (void)_LoadFlexiDagBin(FlexiBinPath, CCfg.pBin[0], &BinSize);
        Ret = SvcCvAlgo_Create(pHdlr, &CCfg);
        *pOutputNum = CCfg.OutputNum;
        for (I=0U ; I < CCfg.OutputNum; I++) {
            OutputSz[I] = CCfg.OutputSz[I];
        }
    }

    if (Ret == 0U) {
        SVC_CV_ALGO_REGCB_CFG_s CBCfg;
        CBCfg.Mode = 0U;
        CBCfg.Callback = pCbFxn;
        (void)SvcCvAlgo_RegCallback(pHdlr, &CBCfg);
    }
    return Ret;
}

static UINT32 SCA_Stixel_UT_Run(const char* pInFile, const char* pOutFile,
                                const AMBA_CV_STIXEL_CFG_s *pCfg, AMBA_CV_STIXEL_DET_OUT_s *pStixels)
{
    UINT32 RetVal = 0U;

    // static section, assign value in initial stage
    static UINT32 BufferAlloc = 0U;
    static UINT8* FileInputBuf = NULL;

    static AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s *pVPStixelEstiRoadCfg = NULL;
    static UINT32 AllocVPStixelEstiRoadCfgSize;

    static AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s *pVPStixelHeightSegCfg = NULL;
    static UINT32 AllocVPStixelHeightSegCfgSize;

    static UINT8 *pCamPhDetWorkBuff = NULL;
    static CAM_PITCH_DET_MANUAL_HDLR_s *pManualDetHdlr = NULL;

    static UINT32 AllocMemIORawSize = 0U;
    static UINT32 AllocInFileSize = 0U;

    UINT32 InputIdx;
    UINT32 OutputIdx;
    UINT32 FlxiDagIdx;
    UINT32 DispWidth = STIXEL_IN_DISPARITY_WIDTH;
    UINT32 DispHeight = STIXEL_IN_DISPARITY_HEIGHT;

    UINT32 VDispPrecision = STIXEL_OUT_VDISPARITY_PRECISION;
    UINT32 VDispStartY = STIXEL_IN_VDISPARITY_START_Y;
    UINT32 VDispHeight = STIXEL_IN_VDISPARITY_HEIGHT;
    UINT32 VDispOutWidth = STIXEL_OUT_VDISPARITY_WIDTH;

#ifdef CONFIG_BUILD_CV_THREADX
    // For profile
    UINT32 T0 = 0U;
    UINT32 T1 = 0U;
#endif

    void* vp;
    UINT8* pU8;

    CAM_PITCH_DET_LINE_s EstiGCLine;
    DOUBLE EstiCamHeight;
    DOUBLE EstiCamPitch;

    static CAM_PITCH_DET_MANUAL_PRIOR_ESTI_s PriorEstimatedGround;
    UINT32 QualifiedRoadEsti;

    UINT32 FxInBuffNum = 0U;
    ulong FxInBuffCaddr[MAX_FX_IN_BUFFER_NUM] = {0U};
    UINT32 FxInBuffSize[MAX_FX_IN_BUFFER_NUM] = {0U};
    UINT32 FxInBuffPitch[MAX_FX_IN_BUFFER_NUM] = {0U};
    UINT32 FxOutBuffNum = 0U;

    static flexidag_memblk_t FlexInBuf[MAX_FX_OUT_BUFFER_NUM];
    static flexidag_memblk_t FlexOutBuf[FX_NUM][MAX_FX_OUT_BUFFER_NUM];
    static flexidag_memblk_t FlexFileInBuf;
    static flexidag_memblk_t VPStixelEstiRoadCfg;
    static flexidag_memblk_t VPStixelHeightSegCfg;

    AmbaMisra_TouchUnused(&EstiGCLine);
    AmbaMisra_TouchUnused(&EstiCamHeight);
    AmbaMisra_TouchUnused(&EstiCamPitch);

    if (BufferAlloc == 0U) {
        UINT32 AllocRet = 0U;
        UINT32 TotalAllocSize = 0U;
        UINT32 AlignedSize = 0U;

        //alloc flexidag input buffer
        for (InputIdx = 0U; InputIdx < MAX_FX_IN_BUFFER_NUM; InputIdx++) {
            AllocRet |= RefCV_MemblkAllocv1(_CacheAlign(sizeof(memio_source_recv_raw_t)), &pU8, &FlexInBuf[InputIdx], &AllocMemIORawSize);
            TotalAllocSize += AllocMemIORawSize;
        }

        // alloc frame buffer
        AllocRet = RefCV_MemblkAllocv1(_CacheAlign(DispWidth*DispHeight*sizeof(UINT16)), &pU8, &FlexFileInBuf, &AllocInFileSize);
        TotalAllocSize += AllocInFileSize;
        FileInputBuf = pU8;

        // alloc auto detection work buffer
        {
            UINT32 CamPhAutoDetWorkSize;
            (void) CamPhDet_GetAutoDetWorkSize(&CamPhAutoDetWorkSize);
            AllocRet |= RefCV_UT_GetCVBuf(&pU8, CamPhAutoDetWorkSize, &AlignedSize);
            TotalAllocSize += AlignedSize;
            AmbaMisra_TypeCast(&pCamPhDetWorkBuff, &pU8);
        }

        // alloc & initialize manual detection handler
        {
            AllocRet |= RefCV_UT_GetCVBuf(&pU8, sizeof(CAM_PITCH_DET_MANUAL_HDLR_s), &AlignedSize);
            TotalAllocSize += AlignedSize;
            AmbaMisra_TypeCast(&pManualDetHdlr, &pU8);

            AllocRet |= CamPhDet_InitManualDetection(pCfg->ManualDetCfg.ExtParam.Height,
                        pCfg->ManualDetCfg.ExtParam.Pitch,
                        pCfg->IntParam.V0, pCfg->IntParam.Baseline, pCfg->IntParam.Fu,
                        (DOUBLE)VDispPrecision,
                        pCfg->ManualDetCfg.PitchStride,
                        pCfg->ManualDetCfg.HeightStride,
                        pCfg->ManualDetCfg.PitchSetNum,
                        pCfg->ManualDetCfg.HeightSetNum,
                        &PriorEstimatedGround,
                        pManualDetHdlr);
        }

        // alloc VPStixelEstiRoadCfg buffer
        AllocRet |= RefCV_MemblkAllocv1(_CacheAlign(sizeof(AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s)), &pU8, &VPStixelEstiRoadCfg, &AllocVPStixelEstiRoadCfgSize);
        TotalAllocSize += AllocVPStixelEstiRoadCfgSize;
        AmbaMisra_TypeCast(&pVPStixelEstiRoadCfg, &pU8);

        AllocRet |= RefCV_MemblkAllocv1(_CacheAlign(sizeof(AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s)), &pU8, &VPStixelHeightSegCfg, &AllocVPStixelHeightSegCfgSize);
        TotalAllocSize += AllocVPStixelHeightSegCfgSize;
        AmbaMisra_TypeCast(&pVPStixelHeightSegCfg, &pU8);

        // alloc flexidag output buffer
        for (FlxiDagIdx = 0U; FlxiDagIdx < FX_NUM; FlxiDagIdx++) {
            for (OutputIdx = 0U; OutputIdx < gOutputNum[FlxiDagIdx]; OutputIdx++) {
                AllocRet = RefCV_MemblkAllocv1(_CacheAlign(gOutputSz[FlxiDagIdx][OutputIdx]), &pU8, &FlexOutBuf[FlxiDagIdx][OutputIdx], &AlignedSize);
                TotalAllocSize += AlignedSize;
            }
        }
        AmbaPrint_PrintUInt5("SCA_Stixel_UT_Run: AllocRet = %d, Allocate buffer size:(0x%x)",
                             AllocRet, TotalAllocSize, 0U, 0U, 0U);
        BufferAlloc = 1U;
    }

    if ( BufferAlloc == 1U ) {
        AmbaMisra_TypeCast(&vp, &FileInputBuf);
        _ReadInputFile(pInFile, vp, &FlexFileInBuf);
        (void) RefCV_MemblkClean(&FlexFileInBuf);

        // V-disparity
        // ----------------------------------------------------------
        {
            // Flexidag In
            FxInBuffNum = 1U;
            FxInBuffCaddr[0U] = FlexFileInBuf.buffer_caddr + (DispWidth*VDispStartY*sizeof(UINT16));
            FxInBuffSize[0U] = (DispWidth*VDispHeight)*sizeof(UINT16);
            FxInBuffPitch[0U] = DispWidth*sizeof(UINT16);

            // Flexidag Out
            FxOutBuffNum = gOutputNum[FX_INDEX_VDISP];

            {
                SVC_CV_ALGO_FEED_CFG_s FeedCfg;
                AMBA_CV_FLEXIDAG_IO_s InFxBuf;
                AMBA_CV_FLEXIDAG_IO_s OutFxBuf;

                RetVal |= _RunFlexiDag(&InFxBuf, FlexInBuf,
                                       FxInBuffNum, FxInBuffCaddr, FxInBuffSize, FxInBuffPitch,
                                       &OutFxBuf, FlexOutBuf[FX_INDEX_VDISP], FxOutBuffNum);

                FeedCfg.pIn = &InFxBuf;
                FeedCfg.pOut = &OutFxBuf;
                RetVal |= SvcCvAlgo_Feed(gHdlr[FX_INDEX_VDISP], &FeedCfg);
            }
            _BlockWaitCompleted(&gStep0_VDispCompleted, 1000U);

            {
                UINT32 SaveFileNum = 1U;
#ifdef CONFIG_BUILD_CV_THREADX
                const char* SaveNameSet[] = {"C:/Stixel_VDisp_dump0.raw"};
#else
                const char* SaveNameSet[] = {"/tmp/SD0/Stixel_VDisp_dump0.raw"};
#endif
                UINT32 SaveFileSizeSet[] = {gOutputSz[FX_INDEX_VDISP][0U]};
                _DebugSave(gDebugDumpFlag, SaveFileNum, SaveNameSet,
                           FlexOutBuf[FX_INDEX_VDISP], SaveFileSizeSet);
            }

        }

        // Calculate camera pitch angle
        // ----------------------------------------------------------
        {
            CAM_PITCH_DET_V_DISP_s Vdisp;
            char EstiCamHeightStrBuff[50U];
            char EstiCamPitchStrBuff[50U];

            Vdisp.Precision = VDispPrecision;
            Vdisp.StartY = VDispStartY;
            Vdisp.Width = VDispOutWidth;
            Vdisp.Height = VDispHeight;
            AmbaMisra_TypeCast(&Vdisp.pBuff, &FlexOutBuf[FX_INDEX_VDISP][0U].pBuffer);

#ifdef CONFIG_BUILD_CV_THREADX
            (void)_UsecTimerGetTick(&T0);
#endif
            {
                if ( pCfg->RoadEstiMode == STIXEL_ROAD_ESTI_MODE_AUTO ) {
                    CAM_PITCH_DET_AUTO_PARAM_s AutoDetCfg;
                    AutoDetCfg.IntCam.BaseLineMeter = pCfg->IntParam.Baseline;
                    AutoDetCfg.IntCam.FocalLengthPixel = pCfg->IntParam.Fu;
                    AutoDetCfg.IntCam.V0 = pCfg->IntParam.V0;
                    RetVal |= CamPhDet_AutoDetection(pCamPhDetWorkBuff,
                                                     &Vdisp,
                                                     &AutoDetCfg,
                                                     &EstiCamHeight, &EstiCamPitch, &EstiGCLine);
                    AmbaPrint_PrintInt5("(AutoDetection) Estimated Ground Disparity: %d ~ %d",
                                        (INT32)CamPhDet_GetGroundDispByV(0U, EstiGCLine.m, EstiGCLine.c),
                                        (INT32)CamPhDet_GetGroundDispByV(DispHeight - 1U, EstiGCLine.m, EstiGCLine.c), 0, 0, 0);
                } else {
                    CAM_PITCH_DET_MANUAL_THR_s VdispThr;
                    DOUBLE XPixelCountThrDB;

                    XPixelCountThrDB = (DOUBLE)Vdisp.Width;
                    XPixelCountThrDB *= pCfg->ManualDetCfg.VDispParam.XRatioThr;
                    VdispThr.XPixelCountThr = (UINT32)XPixelCountThrDB;
                    VdispThr.YRatioThr = pCfg->ManualDetCfg.VDispParam.YRatioThr;
                    VdispThr.YRatioThr = pCfg->ManualDetCfg.VDispParam.YRatioThr;
                    RetVal |= CamPhDet_ManualDetection(&Vdisp,
                                                       pManualDetHdlr, &VdispThr, &PriorEstimatedGround,
                                                       &EstiCamHeight, &EstiCamPitch, &EstiGCLine, &QualifiedRoadEsti);
                    AmbaPrint_PrintInt5("(ManualDetection) Estimated Ground Disparity: %d ~ %d",
                                        (INT32)CamPhDet_GetGroundDispByV(0U, EstiGCLine.m, EstiGCLine.c),
                                        (INT32)CamPhDet_GetGroundDispByV(DispHeight - 1U, EstiGCLine.m, EstiGCLine.c), 0, 0, 0);
                }
            }

#ifdef CONFIG_BUILD_CV_THREADX
            (void)_UsecTimerGetTick(&T1);
            AmbaPrint_PrintUInt5("Road Detection, Take (%d) us\n", T1 - T0, 0U, 0U, 0U, 0U);
#endif
            (void)AmbaUtility_DoubleToStr(&EstiCamHeightStrBuff[0U],
                                          sizeof(EstiCamHeightStrBuff), EstiCamHeight, 5U);

            (void)AmbaUtility_DoubleToStr(&EstiCamPitchStrBuff[0U],
                                          sizeof(EstiCamPitchStrBuff), EstiCamPitch, 5U);

            //AmbaPrint_PrintStr5("EstiCamHeight = %s, EstiCamPitch = %s\n",
            //    EstiCamHeightStrBuff, EstiCamPitchStrBuff, NULL, NULL, NULL);
        }

        // GenEstiRoad
        // ----------------------------------------------------------
        {
            DOUBLE EstiSinCamPitch = 0.0;
            DOUBLE EstiCosCamPitch = 0.0;
            UINT32 RoadSectionIdx;

            (void) AmbaWrap_sin(EstiCamPitch, &EstiSinCamPitch);
            (void) AmbaWrap_cos(EstiCamPitch, &EstiCosCamPitch);

            pVPStixelEstiRoadCfg->ImageV0 = (FLOAT)pCfg->IntParam.V0;
            pVPStixelEstiRoadCfg->BaseLine = (FLOAT)pCfg->IntParam.Baseline;
            pVPStixelEstiRoadCfg->FocalLength = (FLOAT)pCfg->IntParam.Fu;
            pVPStixelEstiRoadCfg->DetObjectHeight = (FLOAT)pCfg->DetObjectHeight;
            for (RoadSectionIdx = 0U; RoadSectionIdx < STIXEL_ROAD_SECTION_NUM; RoadSectionIdx++) {
                pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].Bottom = (FLOAT)DispHeight - 1.f;
                pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].M = (FLOAT)EstiGCLine.m;
                pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].C = (FLOAT)EstiGCLine.c;
                pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].SinCamPitch = (FLOAT)EstiSinCamPitch;
                pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].CosCamPitch = (FLOAT)EstiCosCamPitch;
            }
            (void) RefCV_MemblkClean(&VPStixelEstiRoadCfg);

            // Flexidag In
            FxInBuffNum = 1U;
            FxInBuffCaddr[0U] = VPStixelEstiRoadCfg.buffer_caddr;
            FxInBuffSize[0U] = AllocVPStixelEstiRoadCfgSize;
            FxInBuffPitch[0U] = AllocVPStixelEstiRoadCfgSize;

            // Flexidag Out
            FxOutBuffNum = gOutputNum[FX_INDEX_GEN_ESTI_ROAD];

            {
                SVC_CV_ALGO_FEED_CFG_s FeedCfg;
                AMBA_CV_FLEXIDAG_IO_s InFxBuf;
                AMBA_CV_FLEXIDAG_IO_s OutFxBuf;

                RetVal |= _RunFlexiDag(&InFxBuf, FlexInBuf,
                                       FxInBuffNum, FxInBuffCaddr, FxInBuffSize, FxInBuffPitch,
                                       &OutFxBuf, FlexOutBuf[FX_INDEX_GEN_ESTI_ROAD], FxOutBuffNum);

                FeedCfg.pIn = &InFxBuf;
                FeedCfg.pOut = &OutFxBuf;
                RetVal |= SvcCvAlgo_Feed(gHdlr[FX_INDEX_GEN_ESTI_ROAD], &FeedCfg);
            }
            _BlockWaitCompleted(&gStep1_GenEstiRoadCompleted, 1000U);

            {
                UINT32 SaveFileNum = 4U;
                const char* SaveNameSet[] = {
#ifdef CONFIG_BUILD_CV_THREADX
                    "C:/Stixel_GenEstiRoad_dump0.raw",
                    "C:/Stixel_GenEstiRoad_dump1.raw",
                    "C:/Stixel_GenEstiRoad_dump2.raw",
                    "C:/Stixel_GenEstiRoad_dump3.raw"
#else
                    "/tmp/SD0/Stixel_GenEstiRoad_dump0.raw",
                    "/tmp/SD0/Stixel_GenEstiRoad_dump1.raw",
                    "/tmp/SD0/Stixel_GenEstiRoad_dump2.raw",
                    "/tmp/SD0/Stixel_GenEstiRoad_dump3.raw"
#endif
                };
                UINT32 SaveFileSizeSet[] = {
                    gOutputSz[FX_INDEX_GEN_ESTI_ROAD][0U],
                    gOutputSz[FX_INDEX_GEN_ESTI_ROAD][1U],
                    gOutputSz[FX_INDEX_GEN_ESTI_ROAD][2U],
                    gOutputSz[FX_INDEX_GEN_ESTI_ROAD][3U],
                };
                _DebugSave(gDebugDumpFlag, SaveFileNum, SaveNameSet,
                           FlexOutBuf[FX_INDEX_GEN_ESTI_ROAD], SaveFileSizeSet);
            }
        }

        // Preprocess
        // ----------------------------------------------------------
        {
            // Flexidag In
            FxInBuffNum = 1U;
            FxInBuffCaddr[0U] = FlexFileInBuf.buffer_caddr;
            FxInBuffSize[0U] = AllocInFileSize;
            FxInBuffPitch[0U] = DispWidth*sizeof(UINT16);

            // Flexidag Out
            FxOutBuffNum = gOutputNum[FX_INDEX_PREPROCESS];

            {
                SVC_CV_ALGO_FEED_CFG_s FeedCfg;
                AMBA_CV_FLEXIDAG_IO_s InFxBuf;
                AMBA_CV_FLEXIDAG_IO_s OutFxBuf;

                RetVal |= _RunFlexiDag(&InFxBuf, FlexInBuf,
                                       FxInBuffNum, FxInBuffCaddr, FxInBuffSize, FxInBuffPitch,
                                       &OutFxBuf, FlexOutBuf[FX_INDEX_PREPROCESS], FxOutBuffNum);

                FeedCfg.pIn = &InFxBuf;
                FeedCfg.pOut = &OutFxBuf;
                RetVal |= SvcCvAlgo_Feed(gHdlr[FX_INDEX_PREPROCESS], &FeedCfg);
            }
            _BlockWaitCompleted(&gStep2_PreprocessCompleted, 1000U);

            {
                UINT32 SaveFileNum = 1U;
                const char* SaveNameSet[] = {
#ifdef CONFIG_BUILD_CV_THREADX
                    "C:/Stixel_preprocess_dump0.raw"
#else
                    "/tmp/SD0/Stixel_preprocess_dump0.raw"
#endif
                };
                UINT32 SaveFileSizeSet[] = {
                    gOutputSz[FX_INDEX_PREPROCESS][0U]
                };
                _DebugSave(gDebugDumpFlag, SaveFileNum, SaveNameSet,
                           FlexOutBuf[FX_INDEX_PREPROCESS], SaveFileSizeSet);
            }
        }

        // Free space
        // ----------------------------------------------------------
        {
            // Flexidag In
            FxInBuffNum = 4U;
            FxInBuffCaddr[0U] = FlexOutBuf[FX_INDEX_PREPROCESS][0U].buffer_caddr;
            FxInBuffSize[0U] = gOutputSz[FX_INDEX_PREPROCESS][0U];
            FxInBuffPitch[0U] = STIXEL_TRANS_DISPARITY_WIDTH*sizeof(UINT16);

            FxInBuffCaddr[1U] = FlexOutBuf[FX_INDEX_GEN_ESTI_ROAD][0U].buffer_caddr;
            FxInBuffSize[1U] = gOutputSz[FX_INDEX_GEN_ESTI_ROAD][0U];
            FxInBuffPitch[1U] = STIXEL_TRANS_DISPARITY_WIDTH*sizeof(UINT16);

            FxInBuffCaddr[2U] = FlexOutBuf[FX_INDEX_GEN_ESTI_ROAD][1U].buffer_caddr;
            FxInBuffSize[2U] = gOutputSz[FX_INDEX_GEN_ESTI_ROAD][1U];
            FxInBuffPitch[2U] = STIXEL_TRANS_DISPARITY_WIDTH*sizeof(UINT16);

            FxInBuffCaddr[3U] = FlexOutBuf[FX_INDEX_GEN_ESTI_ROAD][2U].buffer_caddr;
            FxInBuffSize[3U] = gOutputSz[FX_INDEX_GEN_ESTI_ROAD][2U];
            FxInBuffPitch[3U] = STIXEL_TRANS_DISPARITY_WIDTH*sizeof(UINT16);

            // Flexidag Out
            FxOutBuffNum = gOutputNum[FX_INDEX_FREE_SPACE];

            {
                SVC_CV_ALGO_FEED_CFG_s FeedCfg;
                AMBA_CV_FLEXIDAG_IO_s InFxBuf;
                AMBA_CV_FLEXIDAG_IO_s OutFxBuf;

                RetVal |= _RunFlexiDag(&InFxBuf, FlexInBuf,
                                       FxInBuffNum, FxInBuffCaddr, FxInBuffSize, FxInBuffPitch,
                                       &OutFxBuf, FlexOutBuf[FX_INDEX_FREE_SPACE], FxOutBuffNum);

                FeedCfg.pIn = &InFxBuf;
                FeedCfg.pOut = &OutFxBuf;
                RetVal |= SvcCvAlgo_Feed(gHdlr[FX_INDEX_FREE_SPACE], &FeedCfg);
            }
            _BlockWaitCompleted(&gStep3_FreeSpaceCompleted, 1000U);

            {
                UINT32 SaveFileNum = 2U;
                const char* SaveNameSet[] = {
#ifdef CONFIG_BUILD_CV_THREADX
                    "C:/Stixel_free_space_dump0.raw",
                    "C:/Stixel_free_space_dump1.raw"
#else
                    "/tmp/SD0/Stixel_free_space_dump0.raw",
                    "/tmp/SD0/Stixel_free_space_dump1.raw"
#endif
                };
                UINT32 SaveFileSizeSet[] = {
                    gOutputSz[FX_INDEX_FREE_SPACE][0U],
                    gOutputSz[FX_INDEX_FREE_SPACE][1U]
                };
                _DebugSave(gDebugDumpFlag, SaveFileNum, SaveNameSet,
                           FlexOutBuf[FX_INDEX_FREE_SPACE], SaveFileSizeSet);
            }
        }

        // Height segmentation
        // ----------------------------------------------------------
        {
            pVPStixelHeightSegCfg->MinDisparity = pCfg->MinDisparity;
            (void) RefCV_MemblkClean(&VPStixelHeightSegCfg);

            // Flexidag In
            FxInBuffNum = 4U;
            FxInBuffCaddr[0U] = FlexOutBuf[FX_INDEX_PREPROCESS][0U].buffer_caddr;
            FxInBuffSize[0U] = gOutputSz[FX_INDEX_PREPROCESS][0U];
            FxInBuffPitch[0U] = STIXEL_TRANS_DISPARITY_WIDTH*sizeof(UINT16);

            FxInBuffCaddr[1U] = FlexOutBuf[FX_INDEX_FREE_SPACE][0U].buffer_caddr;
            FxInBuffSize[1U] = gOutputSz[FX_INDEX_FREE_SPACE][0U];
            FxInBuffPitch[1U] = STIXEL_TRANS_DISPARITY_HEIGHT*sizeof(UINT16);

            FxInBuffCaddr[2U] = FlexOutBuf[FX_INDEX_GEN_ESTI_ROAD][3U].buffer_caddr;
            FxInBuffSize[2U] = gOutputSz[FX_INDEX_GEN_ESTI_ROAD][3U];
            FxInBuffPitch[2U] = STIXEL_IN_MAX_DISPARITY*sizeof(UINT16);

            FxInBuffCaddr[3U] = VPStixelHeightSegCfg.buffer_caddr;
            FxInBuffSize[3U] = AllocVPStixelHeightSegCfgSize;
            FxInBuffPitch[3U] = AllocVPStixelHeightSegCfgSize;

            // Flexidag Out
            FxOutBuffNum = gOutputNum[FX_INDEX_HEIGHT_SEG];

            {
                SVC_CV_ALGO_FEED_CFG_s FeedCfg;
                AMBA_CV_FLEXIDAG_IO_s InFxBuf;
                AMBA_CV_FLEXIDAG_IO_s OutFxBuf;

                RetVal |= _RunFlexiDag(&InFxBuf, FlexInBuf,
                                       FxInBuffNum, FxInBuffCaddr, FxInBuffSize, FxInBuffPitch,
                                       &OutFxBuf, FlexOutBuf[FX_INDEX_HEIGHT_SEG], FxOutBuffNum);

                FeedCfg.pIn = &InFxBuf;
                FeedCfg.pOut = &OutFxBuf;
                RetVal |= SvcCvAlgo_Feed(gHdlr[FX_INDEX_HEIGHT_SEG], &FeedCfg);
            }
            _BlockWaitCompleted(&gStep4_HeightSegCompleted, 1000U);

            {
                UINT32 SaveFileNum = 2U;
                const char* SaveNameSet[] = {
#ifdef CONFIG_BUILD_CV_THREADX
                    "C:/Stixel_height_seg_dump0.raw",
                    "C:/Stixel_height_seg_dump1.raw"
#else
                    "/tmp/SD0/Stixel_height_seg_dump0.raw",
                    "/tmp/SD0/Stixel_height_seg_dump1.raw"
#endif
                };
                UINT32 SaveFileSizeSet[] = {
                    gOutputSz[FX_INDEX_HEIGHT_SEG][0U],
                    gOutputSz[FX_INDEX_HEIGHT_SEG][1U]
                };
                _DebugSave(gDebugDumpFlag, SaveFileNum, SaveNameSet,
                           FlexOutBuf[FX_INDEX_HEIGHT_SEG], SaveFileSizeSet);
            }
        }

        // To stixel
        // --------------------------------------------------
        {
            const UINT16 *pStixelBottomPos;
            const UINT16 *pStixelConfidence;
            const UINT16 *pStixelTopPos;
            const UINT16 *pStixelAvgDisparity;
            UINT32 StixelIdx;

            AmbaMisra_TypeCast(&pStixelBottomPos, &FlexOutBuf[FX_INDEX_FREE_SPACE][0U].pBuffer);
            AmbaMisra_TypeCast(&pStixelConfidence, &FlexOutBuf[FX_INDEX_FREE_SPACE][1U].pBuffer);
            AmbaMisra_TypeCast(&pStixelTopPos, &FlexOutBuf[FX_INDEX_HEIGHT_SEG][0U].pBuffer);
            AmbaMisra_TypeCast(&pStixelAvgDisparity, &FlexOutBuf[FX_INDEX_HEIGHT_SEG][1U].pBuffer);
#ifdef CONFIG_BUILD_CV_THREADX
            (void)_UsecTimerGetTick(&T0);
#endif
            pStixels->StixelNum = STIXEL_OUT_STIXEL_NUM;
            pStixels->Width = STIXEL_COLUMN_WIDTH;
            for (StixelIdx = 0U; StixelIdx < pStixels->StixelNum; StixelIdx++) {
                AMBA_CV_STIXEL_s *pStixel = &pStixels->StixelList[StixelIdx];
                pStixel->Top = pStixelTopPos[StixelIdx];
                pStixel->Bottom = pStixelBottomPos[StixelIdx];
                pStixel->AvgDisparity = pStixelAvgDisparity[StixelIdx];
                pStixel->Confidence = pStixelConfidence[StixelIdx];
            }
#ifdef CONFIG_BUILD_CV_THREADX
            (void)_UsecTimerGetTick(&T1);
            AmbaPrint_PrintUInt5("To Stixel Format, Take (%d) us\n", T1 - T0, 0U, 0U, 0U, 0U);
#endif
            // Save stixel result
            {
                AmbaMisra_TypeCast(&pU8, &pStixels);
                _SaveBin(pU8, pOutFile, sizeof(AMBA_CV_STIXEL_DET_OUT_s));
            }

        }

    }
    return RetVal;
}

void SCA_Stixel_UT(const char* pInFile, const char* pOutFile, UINT32 RoadDetMode)
{
    UINT32 RetVal = 0U;
    AMBA_CV_STIXEL_CFG_s StixelCfg;

    static UINT32 Init = 0U;
    static flexidag_memblk_t AlgoBuf[FX_NUM];
    static flexidag_memblk_t FDBinBuf[FX_NUM];
    static SVC_CV_ALGO_HANDLE_s        Handlers[FX_NUM];
    static AMBA_CV_STIXEL_DET_OUT_s Stixels;

    if ( Init == 0U ) {
        const char *pFxBinPath[FX_NUM];
        UINT32 FlexIdx;

        for (FlexIdx = 0U; FlexIdx < FX_NUM; FlexIdx++) {
            gHdlr[FlexIdx] = &Handlers[FlexIdx];
        }

#ifdef CONFIG_BUILD_CV_THREADX
        pFxBinPath[FX_INDEX_VDISP] =            "C:/stixel_v_disparity_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_GEN_ESTI_ROAD] =    "C:/stixel_gen_esti_road_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_PREPROCESS] =       "C:/stixel_preprocess_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_FREE_SPACE] =       "C:/stixel_free_space_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_HEIGHT_SEG] =       "C:/stixel_height_seg_ag/flexibin/flexibin0.bin";
#else
        pFxBinPath[FX_INDEX_VDISP] =            "/tmp/SD0/stixel_v_disparity_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_GEN_ESTI_ROAD] =    "/tmp/SD0/stixel_gen_esti_road_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_PREPROCESS] =       "/tmp/SD0/stixel_preprocess_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_FREE_SPACE] =       "/tmp/SD0/stixel_free_space_ag/flexibin/flexibin0.bin";
        pFxBinPath[FX_INDEX_HEIGHT_SEG] =       "/tmp/SD0/stixel_height_seg_ag/flexibin/flexibin0.bin";
#endif

        FlexIdx = FX_INDEX_VDISP;
        RetVal |= SCA_Stixel_UT_Init(pFxBinPath[FlexIdx], &StixelVDispAlgoObj,
                                     SCA_VDisp_UT_CB,
                                     &AlgoBuf[FlexIdx], &FDBinBuf[FlexIdx], gHdlr[FlexIdx],
                                     &gOutputNum[FlexIdx], gOutputSz[FlexIdx]);

        FlexIdx = FX_INDEX_GEN_ESTI_ROAD;
        RetVal |= SCA_Stixel_UT_Init(pFxBinPath[FlexIdx], &StixelGenEstiRoadAlgoObj,
                                     SCA_GenEstiRoad_UT_CB,
                                     &AlgoBuf[FlexIdx], &FDBinBuf[FlexIdx], gHdlr[FlexIdx],
                                     &gOutputNum[FlexIdx], gOutputSz[FlexIdx]);

        FlexIdx = FX_INDEX_PREPROCESS;
        RetVal |= SCA_Stixel_UT_Init(pFxBinPath[FlexIdx], &StixelPreprocessAlgoObj,
                                     SCA_Preprocess_UT_CB,
                                     &AlgoBuf[FlexIdx], &FDBinBuf[FlexIdx], gHdlr[FlexIdx],
                                     &gOutputNum[FlexIdx], gOutputSz[FlexIdx]);

        FlexIdx = FX_INDEX_FREE_SPACE;
        RetVal |= SCA_Stixel_UT_Init(pFxBinPath[FlexIdx], &StixelFreeSpaceAlgoObj,
                                     SCA_FreeSpace_UT_CB,
                                     &AlgoBuf[FlexIdx], &FDBinBuf[FlexIdx], gHdlr[FlexIdx],
                                     &gOutputNum[FlexIdx], gOutputSz[FlexIdx]);

        FlexIdx = FX_INDEX_HEIGHT_SEG;
        RetVal |= SCA_Stixel_UT_Init(pFxBinPath[FlexIdx], &StixelHeightSegAlgoObj,
                                     SCA_HeightSeg_UT_CB,
                                     &AlgoBuf[FlexIdx], &FDBinBuf[FlexIdx], gHdlr[FlexIdx],
                                     &gOutputNum[FlexIdx], gOutputSz[FlexIdx]);

        if ( RetVal == 0U ) {
            Init = 1U;
#ifdef CONFIG_BUILD_CV_THREADX
            _UsecTimerStart();
#endif
        } else {
            AmbaPrint_PrintUInt5("SCA_Stixel_UT_Init: failed", 0U, 0U, 0U, 0U, 0U);
        }
    }

    if ( Init == 1U ) {
        AmbaMisra_TouchUnused(&gDebugDumpFlag);

        StixelCfg.RoadEstiMode = (UINT16)RoadDetMode;
        StixelCfg.IntParam.Fu = UT_CFG_FOCAL_LENGTH_U;
        StixelCfg.IntParam.Fv = UT_CFG_FOCAL_LENGTH_V;
        StixelCfg.IntParam.U0 = UT_CFG_OPTICAL_CENTER_X;
        StixelCfg.IntParam.V0 = UT_CFG_OPTICAL_CENTER_Y;
        StixelCfg.IntParam.Baseline = UT_CFG_BASELINE;

        StixelCfg.ManualDetCfg.PitchStride = UT_CFG_ROAD_SEARCH_PITCH_STRIDE;
        StixelCfg.ManualDetCfg.HeightStride = UT_CFG_ROAD_SEARCH_HEIGHT_STRIDE;
        StixelCfg.ManualDetCfg.PitchSetNum = UT_CFG_ROAD_SEARCH_PITCH_NUM;
        StixelCfg.ManualDetCfg.HeightSetNum = UT_CFG_ROAD_SEARCH_HEIGHT_NUM;
        if ( StixelCfg.RoadEstiMode == STIXEL_ROAD_ESTI_MODE_MANUAL ) {
            StixelCfg.ManualDetCfg.ExtParam.Height = UT_CFG_DEFAULT_CAMERA_HEIGH;
            StixelCfg.ManualDetCfg.ExtParam.Pitch = UT_CFG_DEFAULT_CAMERA_PITCH;
        }
        StixelCfg.ManualDetCfg.VDispParam.XRatioThr = UT_CFG_VDISP_X_THR;
        StixelCfg.ManualDetCfg.VDispParam.YRatioThr = UT_CFG_VDISP_Y_THR;

        StixelCfg.MinDisparity = UT_CFG_MIN_DISPARITY;
        StixelCfg.DetObjectHeight = UT_CFG_DET_OBJECT_HEIGHT;

        gStep0_VDispCompleted = 0U;
        gStep1_GenEstiRoadCompleted = 0U;
        gStep2_PreprocessCompleted = 0U;
        gStep3_FreeSpaceCompleted = 0U;
        gStep4_HeightSegCompleted = 0U;
        (void) AmbaWrap_memset(&Stixels, 0, sizeof(AMBA_CV_STIXEL_DET_OUT_s));
        RetVal = SCA_Stixel_UT_Run(pInFile, pOutFile, &StixelCfg, &Stixels);
        AmbaPrint_PrintUInt5("SCA_Stixel_UT_Run: RetVal = %d", RetVal, 0U, 0U, 0U, 0U);

#if 0
        (void) AmbaKAL_TaskSleep(1000);
        AmbaPrint_PrintUInt5("(%d)", __LINE__, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
#endif
    }

}
