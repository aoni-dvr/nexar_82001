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
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_flexidag_unittest.h"
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#include "cvapi_svccvalgo_ambaspufex.h"

#define SPUFEX_SCA_WRAPPER_UT_FLOW 0

#ifdef CONFIG_BUILD_CV_THREADX
#define CVUtil_TypeCast AmbaMisra_TypeCast32
#else
#define CVUtil_TypeCast AmbaMisra_TypeCast64
#endif

static const AMBA_STEREO_FD_HANDLE_s *Hdlr = NULL;

#if SPUFEX_SCA_WRAPPER_UT_FLOW
static SVC_CV_ALGO_HANDLE_s ScaHandler;
static flexidag_memblk_t ScaAlgoBuf = {NULL, 0, 0, 0, 0, 0};
static const char* OutputSpuFileName = NULL;
static const char* OutputFexFileName = NULL;
#endif

static inline UINT32 _CacheAligned (UINT32 Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size  - 1U)) & ~(flexidag_mem_align_size  - 1U));
}

static const AMBA_STEREO_FD_HANDLE_s *CastStereoFd2Const(AMBA_STEREO_FD_HANDLE_s *pHdlr)
{
    const AMBA_STEREO_FD_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    (void) AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval));
    return pRval;
}

static void _ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    ULONG  ULBufAddr;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
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
        Ret = AmbaFS_FileTell(Fp, &Fpos);
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
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
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
        CVUtil_TypeCast(&ULBufAddr, &buf);
        Ret = AmbaCache_DataClean(ULBufAddr, _CacheAligned(Fsize));
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
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
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
    }
    return ret;
}

static void _SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
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

static UINT32 OutputNum;
static UINT32 OutputSz[8];

#if SPUFEX_SCA_WRAPPER_UT_FLOW
static UINT32 RefCV_AmbaSpuFex_UT_CallBack(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Ret = 0;
    AmbaPrint_PrintUInt5("--> SpuFex SCA CallBack Event %d", Event, 0U, 0U, 0U, 0U);
    if (pEventData != NULL) {
        if (pEventData->pOutput != NULL) {
            if (Event == CALLBACK_EVENT_OUTPUT) {
                if ((pEventData->pOutput->buf[0].pBuffer != NULL) && (OutputSpuFileName != NULL))  {
                    _SaveBin((UINT8*)pEventData->pOutput->buf[0].pBuffer, OutputSpuFileName, OutputSz[0]);
                } else {
                    AmbaPrint_PrintUInt5("--> SpuFex SCA CallBack Error: Output buffer[0] NULL", 0U, 0U, 0U, 0U, 0U);
                }
                if ((pEventData->pOutput->buf[1].pBuffer != NULL) && (OutputFexFileName != NULL))  {
                    _SaveBin((UINT8*)pEventData->pOutput->buf[1].pBuffer, OutputFexFileName, OutputSz[1]);
                } else {
                    AmbaPrint_PrintUInt5("--> SpuFex SCA CallBack Error: Output buffer[1] NULL", 0U, 0U, 0U, 0U, 0U);
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
#endif

static void RefCV_AmbaSpuFex_UT_Init(const char *FlexiBinPath)
{
    UINT32 Ret;
    flexidag_memblk_t BinBuf = {NULL, 0, 0, 0, 0, 0};
    UINT32 BinSize = 0U;
    flexidag_memblk_t StateBuf = {NULL, 0, 0, 0, 0, 0};
    UINT32 StateSize = 0U;
    flexidag_memblk_t TempBuf = {NULL, 0, 0, 0, 0, 0};
    UINT32 TempSize = 0U;
    static UINT32 Init = 0U;
    static AMBA_STEREO_FD_HANDLE_s Handler = {0};
    Hdlr = CastStereoFd2Const(&Handler);
#if SPUFEX_SCA_WRAPPER_UT_FLOW
    if (Init == 0U) {
        SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg = {NULL, 0, NULL};

        ScaQueryCfg.pAlgoObj = &AmbaSpuFexAlgoObj;
        Ret = AmbaSpuFexAlgoObj.Query(&ScaHandler, &ScaQueryCfg);
        RefCV_MemblkAlloc(ScaQueryCfg.TotalReqBufSz, &ScaAlgoBuf);

        ScaHandler.pAlgoCtrl = (void *)Hdlr;
        ScaHandler.Callback[0] = RefCV_AmbaSpuFex_UT_CallBack;
    }

    if (Init == 0U) {
        SVC_CV_ALGO_CREATE_CFG_s ScaCreateCfg = {
            .NumFD = 0,
            .pBin = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
            .OutputNum = 0,
            .OutputSz = {0, 0, 0, 0, 0, 0, 0, 0},
            .pExtCreateCfg = NULL
        };

        (void)_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);

        ScaCreateCfg.pAlgoBuf = &ScaAlgoBuf;
        ScaCreateCfg.pBin[0] = &BinBuf;
        Ret |= AmbaSpuFexAlgoObj.Create(&ScaHandler, &ScaCreateCfg);

        OutputNum = ScaCreateCfg.OutputNum;
        for (UINT32 I = 0; I < FLEXIDAG_MAX_OUTPUTS && I < 8; I ++) {
            OutputSz[I] = ScaCreateCfg.OutputSz[I];
        }

        Init = 1;
    }
#endif

    if (Init == 0U) {
        (void)_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        Ret = AmbaSpuFex_Open(&BinBuf, BinSize, &StateSize, &TempSize, &OutputNum, OutputSz, Hdlr);
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
            (void)AmbaSpuFex_Init(Hdlr, &StateBuf, &TempBuf);
            //_Config_AmbaSpuFex(Hdlr);
        }
        Init = 1U;
    }
}

static void RefCV_AmbaSpuFex_UT_Run(const char* LogPath, const char* InFile, const char* OutputSpFn, const char* OutputFexFn)
{
    static UINT32 BufferAlloc = 0U;
    static flexidag_memblk_t InBuf;
    static flexidag_memblk_t OutputBuf[8];
    AMBA_CV_SPU_BUF_s pOutSpu;
    AMBA_CV_FEX_BUF_s pOutFex;
    UINT32 rval;
    UINT32 i;
    void* vp;
    AMBA_CV_IMG_BUF_s pInInfo;
    UINT32 InputBufSize;

    _GetFileSize(InFile, &InputBufSize);
    if (BufferAlloc == 0U) {
        //alloc input buffer
        rval = RefCV_MemblkAlloc(InputBufSize, &InBuf);

        // alloc output buffer
        if (rval == 0U) {
            for(i=0U; i<OutputNum; ++i) {
                rval = RefCV_MemblkAlloc(OutputSz[i], &OutputBuf[i]);
                AmbaPrint_PrintUInt5("Alloc Output Buf[%d] Sz:%d", i, OutputSz[i], 0U, 0U, 0U);
            }
        }
        BufferAlloc = 1U;
    }


    //read pic info
    CVUtil_TypeCast(&vp, &InBuf.pBuffer);
    _ReadInputFile(InFile, vp);

    (void)RefCV_MemblkClean(&InBuf);

    (void)AmbaWrap_memcpy(&pInInfo, &InBuf, sizeof(AMBA_CV_IMG_BUF_s));
    (void)AmbaWrap_memcpy(&pOutSpu, &OutputBuf[0], sizeof(AMBA_CV_SPU_BUF_s));
    (void)AmbaWrap_memcpy(&pOutFex, &OutputBuf[1], sizeof(AMBA_CV_FEX_BUF_s));

#if SPUFEX_SCA_WRAPPER_UT_FLOW
    (void)LogPath;
    {
        static AMBA_CV_FLEXIDAG_IO_s ScaFdIn;
        static AMBA_CV_FLEXIDAG_IO_s ScaFdOut;
        SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg = {NULL, NULL, NULL, NULL};

        (void)AmbaWrap_memcpy(&OutputSpuFileName, &OutputSpFn, sizeof(char*));
        (void)AmbaWrap_memcpy(&OutputFexFileName, &OutputFexFn, sizeof(char*));

        ScaFdIn.num_of_buf = 1;
        (void)AmbaWrap_memcpy(&ScaFdIn.buf[0], &InBuf, sizeof(flexidag_memblk_t));
        ScaFeedCfg.pIn = &ScaFdIn;

        ScaFdOut.num_of_buf = 2;
        (void)AmbaWrap_memcpy(&ScaFdOut.buf[0], &OutputBuf[0], sizeof(flexidag_memblk_t));
        (void)AmbaWrap_memcpy(&ScaFdOut.buf[1], &OutputBuf[1], sizeof(flexidag_memblk_t));

        ScaFeedCfg.pOut = &ScaFdOut;
        AmbaPrint_PrintUInt5("Feed Start", 0U, 0U, 0U, 0U, 0U);
        rval = AmbaSpuFexAlgoObj.Feed(&ScaHandler, &ScaFeedCfg);

        AmbaPrint_PrintUInt5("SpuFex SCA Feed : rval = %d", rval, 0U, 0U, 0U, 0U);

        //AmbaKAL_TaskSleep(1500U);
        rval = AmbaSpuFexAlgoObj.Delete(&ScaHandler, NULL);
        AmbaPrint_PrintUInt5("SpuFex SCA Delete : rval = %d", rval, 0U, 0U, 0U, 0U);
    }
#else
    rval = AmbaSpuFex_Process(Hdlr, &pInInfo, &pOutSpu, &pOutFex);
    AmbaPrint_PrintUInt5("AmbaSpuFex_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
    rval = AmbaSpuFex_GetResult(Hdlr, &pInInfo, &pOutSpu, &pOutFex);
    AmbaPrint_PrintUInt5("AmbaSpuFex_GetResult : rval = %d", rval, 0U, 0U, 0U, 0U);
    (void)AmbaSpuFex_DumpLog(Hdlr, LogPath);

    _SaveBin((UINT8*)pOutSpu.pBuffer, OutputSpFn, OutputSz[0]);
    _SaveBin((UINT8*)pOutFex.pBuffer, OutputFexFn, OutputSz[1]);
#endif
}

void RefCV_AmbaSpuFex_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputSpFn, const char* OutputFexFn)
{
    RefCV_AmbaSpuFex_UT_Init(BinPath);
    RefCV_AmbaSpuFex_UT_Run(LogPath, InFile, OutputSpFn, OutputFexFn);
}
