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
#include "cvapi_flexidag_ambamvac_cv2.h"
//#include "cvapi_mvac_vo_debugger.h"

#if defined(CONFIG_QNX)
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
#include "schdr_api.h"
#include "cvtask_api.h"
#include "ambint.h"
#include "cvtask_ossrv.h"
#include "cavalry_ioctl.h"
#include "cvtask_ossrv.h"
#define AMBA_CACHE_LINE_SIZE    64U
#define AMBA_FS_FILE FILE
#define AMBA_FS_SEEK_CUR SEEK_CUR
#define AMBA_FS_SEEK_START SEEK_SET
#define AMBA_FS_SEEK_END SEEK_END
#define AmbaFS_FileClose fclose
#define AmbaFS_FileSeek fseek
#endif

#define CVUtil_TypeCast AmbaMisra_TypeCast64

static AMBA_MVAC_FD_HANDLE_s Handler = {0};
static AMBA_MVAC_FD_HANDLE_s* Hdlr = &Handler;

static UINT32 _FileOpen(const char *pFileName, const char *pMode, AMBA_FS_FILE **pFile)
{
    UINT32 Ret = 0U;
    *pFile = fopen(pFileName, pMode);
    if (*pFile == NULL) {
        Ret = 1U;
    } else {
        Ret = 0U;
    }
    return Ret;
}

static UINT32 _FileTell(AMBA_FS_FILE * pFile, UINT64* pFilePos)
{
    UINT32 Ret = 0;
    (*pFilePos) = ftell(pFile);
    return Ret;
}

static UINT32 _FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;
    (*pNumSuccess) = fread(pBuf, Size, Count, pFile);
    return Ret;
}

static UINT32 _FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile, UINT32 *pNumSuccess)
{
    UINT32 Ret = 0;
    (*pNumSuccess) = fwrite(pBuf, Size, Count, pFile);
    return Ret;
}

static inline UINT32 _CacheAligned (UINT32 Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}

static void _ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = _FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("_ReadInputFile _FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
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
        //AmbaPrint_PrintUInt5("_ReadInputFile _FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = _FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile _FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 OutputNum;
static UINT32 OutputSz[8];

static void RefCV_AmbaMVAC_UT_Init(void)
{
    static UINT32 Init = 0U;
    UINT32 Ret;
    flexidag_memblk_t BinBuf;
    UINT32 BinSize = 0U;
    flexidag_memblk_t StateBuf;
    UINT32 StateSize = 0U;
    flexidag_memblk_t TempBuf;
    UINT32 TempSize = 0U;

    if (Init == 0U) {
        Ret = AmbaMVAC_Open(&BinBuf, BinSize,&StateSize, &TempSize, &OutputNum, OutputSz, Hdlr);
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
            (void)AmbaMVAC_Init(Hdlr, &StateBuf, &TempBuf);
            AmbaPrint_PrintUInt5("AmbaMVAC_Init Done", 0U, 0U, 0U, 0U, 0U);
        }
        Init = 1U;
    }
}

static void _SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
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

static void RefCV_AmbaMVAC_UT_Run(void)
{
    AMBA_CV_MVAC_CFG_s config;
    static UINT32 BufferAlloc = 0U;
    UINT32 rval;
    UINT32 i;
    UINT32 InputBufSize;
    AMBA_CV_META_BUF_s MetaLeftBuf;
    AMBA_CV_META_BUF_s MetaRightBuf;
    AMBA_CV_VO_BUF_s VOBuf;
    AMBA_CV_FMA_BUF_s FmaBuf;
    AMBA_CV_FEX_BUF_s FexBuf;
    AMBA_CV_MVAC_BUF_s OutBuf;
    char metaleftpath[128] = "./sd0/SD/Input/metadata/fov0_only_meta.bin";
    char metarightpath[128] = "./sd0/SD/Input/metadata/fov1_only_meta.bin";
    char vopath[128] = "./sd0/SD/Golden/vo_out_sc5_all_1d_scale2/vo_0.bin";
    char fmapath[128] = "./sd0/SD/Input/sc5_all_1d_scale2/Fma_0.bin";
    char fexpath[128] = "./sd0/SD/Input/sc5_all_1d_scale2/Fex_0.bin";

    if (BufferAlloc == 0U) {
        //alloc input buffer
        _GetFileSize(metaleftpath, &InputBufSize);
        rval = RefCV_MemblkAlloc(InputBufSize, &MetaLeftBuf);
        AmbaPrint_PrintUInt5("Alloc MetaLeftBuf = %d", InputBufSize, 0U, 0U, 0U, 0U);
        _GetFileSize(metarightpath, &InputBufSize);
        rval = RefCV_MemblkAlloc(InputBufSize, &MetaRightBuf);
        AmbaPrint_PrintUInt5("Alloc MetaRightBuf = %d", InputBufSize, 0U, 0U, 0U, 0U);
        _GetFileSize(vopath, &InputBufSize);
        rval = RefCV_MemblkAlloc(InputBufSize, (flexidag_memblk_t *)&VOBuf);
        AmbaPrint_PrintUInt5("Alloc VOBuf = %d", InputBufSize, 0U, 0U, 0U, 0U);
        _GetFileSize(fmapath, &InputBufSize);
        rval = RefCV_MemblkAlloc(InputBufSize, (flexidag_memblk_t *)&FmaBuf);
        AmbaPrint_PrintUInt5("Alloc FmaBuf = %d", InputBufSize, 0U, 0U, 0U, 0U);
        _GetFileSize(fexpath, &InputBufSize);
        rval = RefCV_MemblkAlloc(InputBufSize, (flexidag_memblk_t *)&FexBuf);
        AmbaPrint_PrintUInt5("Alloc FexBuf = %d", InputBufSize, 0U, 0U, 0U, 0U);

        // alloc output buffer
        rval = RefCV_MemblkAlloc(OutputSz[0], (flexidag_memblk_t *)&OutBuf);
        AmbaPrint_PrintUInt5("Alloc Output = %d", OutputSz[0U], 0U, 0U, 0U, 0U);

        BufferAlloc = 1U;
    }

    //sprintf (metarightpath, "./sd0/SD/Input/metadata/fov1_only_meta.bin");
    _ReadInputFile(metaleftpath, MetaLeftBuf.pBuffer);

    //sc5
    config.View.WarpTableInfo.HorizontalGridNumber = 121;
    config.View.WarpTableInfo.VerticalGridNumber = 61;
    config.View.WarpTableInfo.TileWidthQ16 = 2097152;
    config.View.WarpTableInfo.TileHeightQ16 = 2097152;
    config.View.ScaleId = 2;
    config.pInMetaLeft = &MetaLeftBuf;
    config.MaxIterations = 100;
    config.MinEpipolarScoreThreshold = 0;
    config.MaxFrameSpeedThreshold = 0.5;
    config.MinNumDepthThreshold = 200;
    config.InhibitedFrameSlotSize = 300;
    config.Mode = AMBA_CV_MVAC_MODE_CONFIG;
    AmbaMVAC_Cfg(Hdlr, &config);


    for (i=0; i<470; ++i) {
        AmbaPrint_PrintUInt5("Frame:%d", i, 0U, 0U, 0U, 0U);
        sprintf (vopath,  "./sd0/SD/Golden/vo_out_sc5_all_1d_scale2/vo_%d.bin", i);
        sprintf (fmapath, "./sd0/SD/Input/sc5_all_1d_scale2/Fma_%d.bin", i);
        sprintf (fexpath, "./sd0/SD/Input/sc5_all_1d_scale2/Fex_%d.bin", i);

        _ReadInputFile(vopath, VOBuf.pBuffer);
        _ReadInputFile(fmapath, FmaBuf.pBuffer);
        _ReadInputFile(fexpath, FexBuf.pBuffer);
        AmbaMVAC_GetResult(Hdlr, &VOBuf, &FmaBuf, &FexBuf, &MetaLeftBuf, &MetaRightBuf, &OutBuf);

        AMBA_CV_MVAC_DATA_s *pOutData = OutBuf.pBuffer;
        if(pOutData->pWarpInfo) {
            if((pOutData->pWarpInfo)->CalibrationUpdates[0].WarpTableValid == 1) {
                printf("[ut] generate warp table\n");
                _SaveBin(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable, "./sd0/warp_left.bin", AMBA_CV_MVAC_WARP_TABLE_MAXSIZE*2);
            }
        }

        switch(pOutData->State) {
        case AMBA_CV_MVAC_STATE_RESET:
            printf("[ut]state: RESET\n");
            break;
        case AMBA_CV_MVAC_STATE_WAIT:
            config.Mode = AMBA_CV_MVAC_MODE_CONTROL;
            config.ControlState = AMBA_CV_MVAC_CONTROL_STOP;
            AmbaMVAC_Cfg(&Handler, &config);
            printf("[ut]state: WAIT\n");
            break;
        case AMBA_CV_MVAC_STATE_ACCUMULATION:
            printf("[ut]state: ACCUMULATION\n");
            break;
        case AMBA_CV_MVAC_STATE_SOLVE:
            printf("[ut]state: SOLVE\n");
            break;
        case AMBA_CV_MVAC_STATE_END:
            printf("[ut]state: END\n");
            break;
        case AMBA_CV_MVAC_STATE_SKIP:
            config.Mode = AMBA_CV_MVAC_MODE_CONTROL;
            config.ControlState = AMBA_CV_MVAC_CONTROL_START;
            AmbaMVAC_Cfg(&Handler, &config);
            printf("[ut]state: SKIP\n");
            break;
        default:
            printf("[ut]state: ERROR\n");
        }


    }
}

//static VO_LOG_CONTAINER_s LogPool;
//VO_LOG_MSG_s MsgPool[8192];

void RefCV_AmbaMVAC_UT(void)
{
//    VO_LOG_MSG_s Msg[VO_MAX_MSG_NUM];
//    VO_LOG_MSG_s *pNextMsg = Msg;

//    LogPool.pMsgPool = &MsgPool[0];
//    LogPool.MsgPoolNum = 8192U;
//    VO_LogInit(&LogPool, &MsgPool[0], LogPool.MsgPoolNum,VO_LOG_LEVEL_D|VO_LOG_LEVEL_V|VO_LOG_LEVEL_E|VO_LOG_LEVEL_I);

    (void) AmbaKAL_TaskSleep(5000U);
    RefCV_AmbaMVAC_UT_Init();
    RefCV_AmbaMVAC_UT_Run();

    //VO_LoggingParserDump(&LogPool);
}

