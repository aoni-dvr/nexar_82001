/**
 *  @file SvcStereoTask.c
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
 *  @details svc Object Detection task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaSTU_IF.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaPrint.h"


/* ssp */
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaSvcWrap.h"
#include "AmbaGDMA.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcTask.h"
#include "AmbaVfs.h"
#include "SvcWrap.h"
#include "SvcSysStat.h"
#include "SvcPlat.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcResCfg.h"
#include "SvcBuffer.h"
#include "SvcWinCalc.h"
#include "SvcBufCalc.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcCvFlow_Comm.h"


#if defined(CONFIG_ICAM_CV_STEREO)
#include "SvcCvFlow_Stereo.h"

#if defined(CONFIG_SOC_CV2)
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_flexidag_ambastereoerr_cv2.h"
#endif

#if defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_cv2fs.h"
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#include "cvapi_flexidag_ambaspufusion_cv2fs.h"
#include "cvapi_flexidag_ambastereoerr_cv2fs.h"
#include "cvapi_flexidag_ambaoffex_cv2fs.h"
#endif

#include "cvapi_flexidag_amba_spu_fex_wrap.h"
#include "cvapi_flexidag_amba_spu_fusion_wrap.h"

#endif

#include "SvcCvAppDef.h"


/* svc-icam */
#include "SvcBufMap.h"
#include "SvcStereoTask.h"
#include "SvcStixelTask.h"

#include "cvapi_memio_interface.h"

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
#include "SvcVoutFrmCtrlTask.h"
#endif

#define STEREODRAW_GUI_SHOW                (0x01U)
#define STEREODRAW_GUI_UPDATE              (0x02U)

#define SVC_LOG_STEREO_TASK                     "Stereo_TASK"
#define SVC_STEREO_QUEUE_SIZE                   (128U)
#define SVC_STEREO_MAX_OSD_NUM                  (5U)
#define SVC_STEREO_MAX_OSD_BUF_WIDTH            (1920U)
#define SVC_STEREO_MAX_OSD_BUF_HEIGHT           (1080U)
//#define SVC_STEREO_OSD_CLUT_SIZE                (256U * 4U)
#define SVC_STEREO_OSD_TASK_STACK_SIZE          (0x10000)
#define SVC_STEREO_OSD_LEVEL                    (2U)

#define SVC_STEREO_ENABLE_EXT_OSD_PATH          (0U)
#define PYRAMID_CP_YUV_WIDTH_MAX        (3840U)
#define PYRAMID_CP_YUV_HEIGHT_MAX       (2160U)
#define PYRAMID_CP_YUV_SIZE_MAX         (PYRAMID_CP_YUV_WIDTH_MAX * PYRAMID_CP_YUV_HEIGHT_MAX * 2U)

#define DSIDUMP_TASK_STACK_SIZE         (0x4000)
#define PGMDUMP_TASK_STACK_SIZE         (0x4000)
#define SPUDUMP_TASK_STACK_SIZE         (0x4000)

#define DSIDUMP_TASK_PRIORITY            (50U)
#define PGMDUMP_TASK_PRIORITY            (50U)
#define SPUDUMP_TASK_PRIORITY            (50U)

#define SVC_STEREO_DUMP_QUEUE_SIZE      (4U)
#define SVC_STEREO_PGM_OUTPUT_DEPTH     (4U)

typedef struct {
    UINT32 CvFlowChan;
    AMBA_CV_SPU_DATA_s *pOutSpu;
    AMBA_CV_FEX_DATA_s *pOutFex;
    UINT32 FrameNum;
    memio_source_recv_picinfo_t *pPicInfo;
} STEREO_OSD_MSG_s;

typedef struct {
    UINT32                  Show;
    UINT8                   PostInit;
    UINT32                  Fusion;
    UINT32                  ScaleID;
    UINT32                  VoutID;
    UINT32                  DispStrmIdx;
    UINT8                   OsdBufIdx;                       /* OSD ring buffer index */
    ULONG                   BufAddr[SVC_STEREO_MAX_OSD_NUM]; /* OSD buffer set to DSP */
    UINT32                  DsiWidth;                        /* Width of DSI data in stereo OSD buffer */
    UINT32                  DsiHeight;                       /* Height of DSI data in stereo OSD buffer */
    UINT32                  DsiPitch;                        /* Pitch of DSI data in stereo OSD buffer */
    UINT32                  OsdOffsetX;                      /* Offset with respect to SvcOsd Buffer */
    UINT32                  OsdOffsetY;                      /* Offset with respect to SvcOsd Buffer */

    UINT32                  FrameNum;
    AMBA_KAL_MSG_QUEUE_t    MsgQueId;
    AMBA_KAL_MUTEX_t        OsdMutex;
} OSD_CONFIG_s;

typedef struct {
    ULONG                  YAddr;
    ULONG                  UVAddr;
} SVC_STEREO_YUV_BUF_s;

typedef struct {
    SVC_STEREO_YUV_BUF_s    YuvBuf;
} SVC_STEREO_DATA_BUF_s;

typedef struct {
    UINT32                  InputIdx;
    UINT8                   NumData;
    SVC_STEREO_DATA_INFO_s  DataInfo[SVC_STEREO_MAX_DATA_PER_INPUT];
    UINT8                   FileValid[SVC_STEREO_MAX_DATA_PER_INPUT];
    char                    FileName[SVC_STEREO_MAX_DATA_PER_INPUT][SVC_STEREO_MAX_FILE_PATH];
} SVC_STEREO_INPUT_CFG_s;

typedef struct {
    SVC_STEREO_CONFIG_s     Config;
    SVC_STEREO_INPUT_CFG_s  InputCfg[SVC_STEREO_MAX_INPUT_PER_CHAN];

    /* Buffer pool */
    ULONG                   BufBase;
    UINT32                  BufSize;
    ULONG                   BufCurAddr;

    /* Buffer info for specific data */
    SVC_STEREO_DATA_BUF_s   DataBuf[SVC_STEREO_MAX_INPUT_PER_CHAN][SVC_STEREO_MAX_DATA_PER_INPUT];
} SVC_STEREO_CTRL_s;

typedef struct {
    AMBA_CV_SPU_DATA_s *pOutSpu;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 FrameNum;
} STEREO_SPU_MSG_s;

typedef struct {
    AMBA_CV_SPU_DATA_s *pOutData;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 FrameNum;
} STEREO_DSI_MSG_s;

typedef struct {
    UINT8 *pPgmTbl;
    UINT32 PGMSize;
    memio_source_recv_picinfo_t *pPicInfo;
    UINT32 FrameNum;
} STEREO_PGM_MSG_s;


/* Definition of control Stereo feature */
static UINT32 StereoInit = 0U;
static UINT32 StereoCvfEnable = 0U;
static UINT32 StereoOnOff = 0U;
static UINT32 StereoDebugEnable = 0U;
static UINT32  StereoDumpDsi = 0U;
static UINT32  StereoDumpPgm = 0U;
static UINT32  StereoDumpSpu = 0U;

static SVC_STEREO_CTRL_s g_FileInCtrl;

static OSD_CONFIG_s g_OsdCfg;
static UINT32 HaveStereoData = 0U;

/* Callback function for CV task */
static void StereoTask_Callback(UINT32 CvFlowChan, UINT32 CvType, void *pMsg);

/* OSD function */
static UINT32 StereoTask_GetOsdBufSize(ULONG *pSize);
static UINT32 StereoTask_GetOsdBufIdx(UINT32 *pBufIdx, UINT32 *pBufIdxNext, UINT32 *pFrameNum);
static UINT32 StereoTask_UpdateOsdBufIdx(UINT32 BufIdx, UINT32 FrameNum);
static UINT32 StereoTask_InitOsd(ULONG BaseAddr);

static UINT32 StereoTask_PostInitOsd(STEREO_OSD_MSG_s *pOsdMsg);

static void StereoTask_OsdHandler(STEREO_OSD_MSG_s *pOsdMsg);
static void* StereoTask_OsdTaskEntry(void* EntryArg);
#if (SVC_STEREO_ENABLE_EXT_OSD_PATH == 0U)
static void StereoTask_OsdDraw(UINT32 VoutIdx, UINT32 Level);
static void StereoTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
#endif
static UINT32 g_osd_enable = SVC_STEREO_OSD_ENABLE;
static UINT32 g_dump_serial_num = 0;

static AMBA_KAL_MUTEX_t StereoMutex;

static AMBA_KAL_MSG_QUEUE_t DsiQueId;
static AMBA_KAL_MSG_QUEUE_t PgmQueId;
static AMBA_KAL_MSG_QUEUE_t SpuQueId;


static void Stereo_MutexTake(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&StereoMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Stixel_MutexTake: timeout", 0U, 0U);
    }
}

static void Stereo_MutexGive(void)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&StereoMutex)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Stixel_MutexGive: error", 0U, 0U);
    }
}

void SvcStereoTask_SetOsdEnable(UINT32 Enable) {

    Stereo_MutexTake();

    g_osd_enable = Enable;

    Stereo_MutexGive();
}

static void SVC_STEREO_TASK_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (StereoDebugEnable > 0U) {
        SvcLog_DBG(pModule, pFormat, Arg1, Arg2);
    }
}

static void SvcStereoTask_InitBuf(void)
{
    UINT32 RetVal;

    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CV_FILE_IN, &g_FileInCtrl.BufBase, &g_FileInCtrl.BufSize);
    if (SVC_OK == RetVal) {
        g_FileInCtrl.BufCurAddr = g_FileInCtrl.BufBase;
        SvcLog_DBG(SVC_LOG_STEREO_TASK, "Init Buf: 0x%x, size %u", g_FileInCtrl.BufBase, g_FileInCtrl.BufSize);
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "fail to SvcBuffer_Request", 0U, 0U);
    }

}

static UINT32 SvcStereoTask_AllocBuf(UINT32 ReqSize, ULONG *pBufAddr)
{
    #define BUF_ALIGN_VALUE  128
    UINT32 RetVal = SVC_OK;
    ULONG BufLimit = g_FileInCtrl.BufBase + (ULONG)g_FileInCtrl.BufSize - 1U;
    ULONG BufAddrAlign;
    ULONG ReqSizeAlign;

    BufAddrAlign = (ULONG)GetAlignedValU64(g_FileInCtrl.BufCurAddr, BUF_ALIGN_VALUE);
    ReqSizeAlign = (ULONG)GetAlignedValU64((UINT64)ReqSize, BUF_ALIGN_VALUE);

    if ((BufAddrAlign + ReqSizeAlign) <= BufLimit) {
        *pBufAddr = BufAddrAlign;
        g_FileInCtrl.BufCurAddr += ReqSizeAlign;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}
static inline UINT32 SvcStereoTask_CacheAligned (UINT32 Val) {return (((Val) + (AMBA_CACHE_LINE_SIZE - 1U)) & ~(AMBA_CACHE_LINE_SIZE - 1U));}

static UINT32 SvcStereoTask_ReadFile(const char *pFileName, void *pBuf, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFile = NULL;
    UINT32 DataSize;

    RetVal = AmbaFS_FileOpen(pFileName, "rb", &pFile);
    if ((RetVal == 0U) && (pFile != NULL)) {
        if (0U == AmbaFS_FileRead(pBuf, 1, Size, pFile, &DataSize)) {
            if (DataSize != Size) {
                RetVal = 3U;
            }
        } else {
            RetVal = 2U;
        }

        if (0U != AmbaFS_FileClose(pFile)) {
            RetVal |= 0x10U;
        }
    } else {
        RetVal = 1U;
    }

    return RetVal;
}

static UINT32 SvcStereoTask_FileWrite(const char *pFileName, void *pBuf, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFile = NULL;
    UINT32 DataSize;

    RetVal = AmbaFS_FileOpen(pFileName, "w", &pFile);
    if ((RetVal == 0U) && (pFile != NULL)) {
        if (0U == AmbaFS_FileWrite(pBuf, 1, Size, pFile, &DataSize)) {
            if (DataSize != Size) {
                RetVal = 3U;
            }
        } else {
            RetVal = 2U;
        }

        if (0U != AmbaFS_FileClose(pFile)) {
            RetVal |= 0x10U;
        }
    } else {
        RetVal = 1U;
    }

    return RetVal;
}

static void SvcStereoTask_GetFileSize(const char* path, UINT32* Size)
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

static UINT32 SvcStereoTask_LoadSpuFexBinary(const char* InFile, ULONG *pSpuAddr)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    void *pAddr;

    SvcStereoTask_GetFileSize(InFile, &DataSize);
    RetVal = SvcStereoTask_AllocBuf(DataSize, pSpuAddr);

    if (RetVal == SVC_OK) {
        AmbaMisra_TypeCast(&pAddr, pSpuAddr);
        RetVal = SvcStereoTask_ReadFile(InFile, pAddr, DataSize);
        if (RetVal == SVC_OK) {
            if (CACHE_ERR_NONE != SvcPlat_CacheClean(*pSpuAddr, SvcStereoTask_CacheAligned(DataSize))) {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "Clean Cache error(%d)", RetVal, 0U);
            }
            AmbaPrint_PrintStr5("[FinTask] Read %s done", InFile, NULL, NULL, NULL, NULL);
        } else {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcStereoTask_ReadFile error(%d)", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Alloc Y buf error", 0U, 0U);
    }

    return RetVal;
}

static UINT32 SvcStereoTask_LoadFusionBinary(const char* InFile1, ULONG BufAddr)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    void *pAddr1;

    SvcStereoTask_GetFileSize(InFile1, &DataSize);

    AmbaMisra_TypeCast(&pAddr1, &BufAddr);
    RetVal = SvcStereoTask_ReadFile(InFile1, pAddr1, DataSize);
    if (RetVal == SVC_OK) {
        if (CACHE_ERR_NONE != SvcPlat_CacheClean(BufAddr, SvcStereoTask_CacheAligned(DataSize))) {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "Clean Cache error(%d)", RetVal, 0U);
        }
        AmbaPrint_PrintStr5("[FinTask] Read %s done", InFile1, NULL, NULL, NULL, NULL);
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcStereoTask_ReadFile error(%d)", RetVal, 0U);
    }

    return RetVal;
}

static UINT32 SvcStereoTask_DumpSPU(const AMBA_CV_SPU_DATA_s *pOutSpu, UINT32 SerialNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    UINT32 i;
    void *pAddr;
    ULONG ULONGAddr;
    char NewFileName[64];

    AmbaMisra_TouchUnused(&pOutSpu);
    AmbaMisra_TouchUnused(&SerialNum);

    /* SPU */
    for (i = 0U; i < MAX_HALF_OCTAVES; i++) {
        if ((pOutSpu->Scales[i].Status == 0) && (pOutSpu->Scales[i].BufSize > 0U)) {
            AmbaMisra_TypeCast(&ULONGAddr, &pOutSpu); /* U32Addr = pStereoData->pOutSpu */
            ULONGAddr += (ULONG)pOutSpu->Scales[i].DisparityMapOffset;
            AmbaMisra_TypeCast(&pAddr, &ULONGAddr);
            DataSize = pOutSpu->Scales[i].BufSize;
            //AmbaPrint_PrintUInt5("DisparityMap[%u]: 0x%x Size: %d", i, U32Addr, DataSize, 0U, 0U);
            RetVal |= AmbaWrap_memset(NewFileName, 0, sizeof(NewFileName));
            AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), "c:\\Spu_");

            AmbaUtility_StringAppendUInt32(NewFileName, sizeof(NewFileName), (UINT32)(i / 10U), 10U);
            AmbaUtility_StringAppendUInt32(NewFileName, sizeof(NewFileName), (UINT32)(i % 10U), 10U);

            AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), "_");
            AmbaUtility_StringAppendUInt32(NewFileName, sizeof(NewFileName), SerialNum, 10U);
            AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), ".bin");

            RetVal = SvcStereoTask_FileWrite(NewFileName, pAddr, DataSize);
            if (RetVal == SVC_OK) {
                AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
            }
        }
    }

    return RetVal;
}


static UINT32 StereoTask_DumpDSI(const AMBA_CV_SPU_DATA_s *pOutSpu, UINT32 SerialNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    char NewFileName[SVC_STEREO_MAX_FILE_PATH];
    void *pAddr;
    ULONG ULONGAddr;
    const UINT8 *pU8Addr;
    UINT32 i;
#if defined(CONFIG_THREADX)
    UINT32 Crc;
    extern UINT32 AmbaUtility_Crc32Hw(const UINT8 *pBuffer, UINT32 Size);
#endif
    AmbaMisra_TouchUnused(&pOutSpu);
    AmbaMisra_TouchUnused(&SerialNum);

    RetVal |= AmbaWrap_memset(NewFileName, 0, sizeof(NewFileName));
    AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), "c:\\Fus_");

    /* SPU */
    for (i = 0; i < MAX_HALF_OCTAVES; i++) {
        if ((pOutSpu->Scales[i].Status == 0) && (pOutSpu->Scales[i].BufSize > 0U)) {
            AmbaMisra_TypeCast(&ULONGAddr, &pOutSpu); /* U32Addr = pStereoData->pOutSpu */
            ULONGAddr += (ULONG)pOutSpu->Scales[i].DisparityMapOffset;
            AmbaMisra_TypeCast(&pAddr, &ULONGAddr);
            DataSize = pOutSpu->Scales[i].BufSize;
            SvcWrap_PrintUL("DisparityMap[%u]: 0x%x Size: %d", (ULONG)i, ULONGAddr, (ULONG)DataSize, 0U, 0U);

            AmbaUtility_StringAppendUInt32(NewFileName, sizeof(NewFileName), (UINT32)pOutSpu->Scales[i].DisparityWidth, 10U);
            AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), "_");
            AmbaUtility_StringAppendUInt32(NewFileName, sizeof(NewFileName), (UINT32)pOutSpu->Scales[i].DisparityHeight, 10U);
            AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), "_");
            AmbaUtility_StringAppendUInt32(NewFileName, sizeof(NewFileName), SerialNum, 10U);
            AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), ".bin");
            RetVal = SvcStereoTask_FileWrite(NewFileName, pAddr, DataSize);
            if (RetVal == SVC_OK) {
                AmbaMisra_TypeCast(&pU8Addr, &pAddr);
#if defined(CONFIG_THREADX)
                Crc = AmbaUtility_Crc32Hw(pU8Addr, DataSize);
                AmbaPrint_PrintUInt5("[FinTask] CRC = 0x%x", Crc, 0U, 0U, 0U, 0U);
#endif
                AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcStereoTask_FileWrite error", 0U, 0U);
            }
        }
    }

    return RetVal;
}

static UINT32 StereoTask_DumpPgm(UINT8 *pPgmTbl, UINT32 PGMSize, UINT32 SerialNum)
{
    UINT32 RetVal = SVC_OK;
    char NewFileName[SVC_STEREO_MAX_FILE_PATH];

    AmbaMisra_TouchUnused(&pPgmTbl);
    AmbaMisra_TouchUnused(&PGMSize);
    AmbaMisra_TouchUnused(&SerialNum);

    RetVal |= AmbaWrap_memset(NewFileName, 0, sizeof(NewFileName));
    AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), "c:\\Fus_");
    AmbaUtility_StringAppendUInt32(NewFileName, sizeof(NewFileName), SerialNum, 10U);
    AmbaUtility_StringAppend(NewFileName, (UINT32)sizeof(NewFileName), ".pgm");
    RetVal = SvcStereoTask_FileWrite(NewFileName, pPgmTbl, PGMSize);

    if (RetVal == SVC_OK) {
        AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcStereoTask_FileWrite error", 0U, 0U);
    }

    return RetVal;
}


static UINT32 SvcStereoTask_DumpPyramid(const memio_source_recv_picinfo_t *pPicInfo, UINT32 SerialNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    UINT32 YWidth = 0U;
    UINT32 YHeight = 0U;
    UINT32 Pitch = 0U;
    char NewFileNameLeft[SVC_STEREO_MAX_FILE_PATH];
    char NewFileNameRight[SVC_STEREO_MAX_FILE_PATH];
    static UINT8 Pyramid_YuvBuffer[PYRAMID_CP_YUV_SIZE_MAX] GNU_ALIGNED_CACHESAFE GNU_SECTION_NOZEROINIT;

    AmbaMisra_TouchUnused(&pPicInfo);
    AmbaMisra_TouchUnused(&SerialNum);

    for (i = 0U; i < (UINT32)MAX_HALF_OCTAVES; i++) {
        if (pPicInfo->pic_info.pyramid.half_octave[i].ctrl.disable == 0U) {
            YWidth = (UINT32)pPicInfo->pic_info.pyramid.half_octave[i].roi_width_m1 + 1U;
            YHeight = (UINT32)pPicInfo->pic_info.pyramid.half_octave[i].roi_height_m1 + 1U;
            Pitch = pPicInfo->pic_info.pyramid.half_octave[i].ctrl.roi_pitch;

            if (SVC_OK == RetVal) {
                UINT8 *pLeft_BufferY;
                UINT8 *pLeft_DstBufferY = Pyramid_YuvBuffer;
                UINT8 *pRight_BufferY;
                UINT8 *pRight_DstBufferY = &Pyramid_YuvBuffer[PYRAMID_CP_YUV_WIDTH_MAX * PYRAMID_CP_YUV_HEIGHT_MAX];
                ULONG Pyramid_YuvBufferAddr;

                AmbaMisra_TypeCast(&pLeft_BufferY, &pPicInfo->pic_info.rpLumaLeft[i]);
                AmbaMisra_TypeCast(&pRight_BufferY, &pPicInfo->pic_info.rpLumaRight[i]);
                //AmbaPrint_PrintUInt5("Layer %d Left 0x%x, Right 0x%x", i, PyramidMsg.pPicInfo->pic_info.rpLumaLeft[i], PyramidMsg.pPicInfo->pic_info.rpLumaRight[i], 0U, 0U);

                RetVal = AmbaWrap_memset(Pyramid_YuvBuffer, 0, sizeof(Pyramid_YuvBuffer));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "DirtDetect_GetYuvInfo err. memset failed %d", RetVal, 0U);
                }

                /* GDMA copy */
                if (SVC_OK == RetVal) {
                    AMBA_GDMA_BLOCK_s GdmaBlockLeft = {
                        .pSrcImg = pLeft_BufferY,
                        .pDstImg = pLeft_DstBufferY,
                        .SrcRowStride = Pitch,
                        .DstRowStride = YWidth,
                        .BltWidth  = YWidth,
                        .BltHeight = YHeight,
                        .PixelFormat = AMBA_GDMA_8_BIT,
                    };

                    AMBA_GDMA_BLOCK_s GdmaBlockRight = {
                        .pSrcImg = pRight_BufferY,
                        .pDstImg = pRight_DstBufferY,
                        .SrcRowStride = Pitch,
                        .DstRowStride = YWidth,
                        .BltWidth  = YWidth,
                        .BltHeight = YHeight,
                        .PixelFormat = AMBA_GDMA_8_BIT,
                    };

                    //SvcLog_OK(SVC_LOG_STEREO_TASK, "GDMA Copy start CapSequence Layer %d", i, 0U);

                    if (NULL != pLeft_BufferY) {
                        AmbaMisra_TypeCast(&Pyramid_YuvBufferAddr, &pLeft_DstBufferY);
                        RetVal = SvcPlat_CacheInvalidate(Pyramid_YuvBufferAddr, sizeof(Pyramid_YuvBuffer));
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_STEREO_TASK, "DirtDetect_GetYuvInfo err. Cache clean err %d", RetVal, 0U);
                        }
                        RetVal = AmbaGDMA_BlockCopy(&GdmaBlockLeft, NULL, 0U, 1000U /* Wait 1000ms */);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_STEREO_TASK, "DirtDetect_GetYuvInfo err. Gdma copy 2 Left Y failed %d", RetVal, 0U);
                        }
                        RetVal = AmbaGDMA_WaitAllCompletion(1000U);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_STEREO_TASK, "DirtDetect_GetYuvInfo err. Gdma copy 2 Left Y wait err %d", RetVal, 0U);
                        }
                    }

                    if (NULL != pRight_BufferY) {
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_STEREO_TASK, "DirtDetect_GetYuvInfo err. Cache clean err %d", RetVal, 0U);
                        }
                        RetVal = AmbaGDMA_BlockCopy(&GdmaBlockRight, NULL, 0U, 1000U /* Wait 1000ms */);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_STEREO_TASK, "DirtDetect_GetYuvInfo err. Gdma copy 2 Right Y failed %d", RetVal, 0U);
                        }
                        RetVal = AmbaGDMA_WaitAllCompletion(1000U);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_STEREO_TASK, "DirtDetect_GetYuvInfo err. Gdma copy 2 Right Y wait err %d", RetVal, 0U);
                        }
                    }
                }

                RetVal |= AmbaWrap_memset(NewFileNameLeft, 0, sizeof(NewFileNameLeft));
                AmbaUtility_StringAppend(NewFileNameLeft, (UINT32)sizeof(NewFileNameLeft), "c:\\Left_");
                AmbaUtility_StringAppendUInt32(NewFileNameLeft, sizeof(NewFileNameLeft), (UINT32)YWidth, 10U);
                AmbaUtility_StringAppend(NewFileNameLeft, (UINT32)sizeof(NewFileNameLeft), "x");
                AmbaUtility_StringAppendUInt32(NewFileNameLeft, sizeof(NewFileNameLeft), (UINT32)YHeight, 10U);
                AmbaUtility_StringAppend(NewFileNameLeft, (UINT32)sizeof(NewFileNameLeft), "_");
                AmbaUtility_StringAppendUInt32(NewFileNameLeft, sizeof(NewFileNameLeft), SerialNum, 10U);
                AmbaUtility_StringAppend(NewFileNameLeft, (UINT32)sizeof(NewFileNameLeft), ".y");
                AmbaPrint_PrintStr5("Left FileName %s", NewFileNameLeft, NULL, NULL, NULL, NULL);

                RetVal = SvcStereoTask_FileWrite(NewFileNameLeft, pLeft_DstBufferY, YWidth * YHeight);
                //if (RetVal == SVC_OK) {
                    //SvcLog_OK(SVC_LOG_STEREO_TASK, "save left layer %d ok", i, 0U);
                //}

                RetVal |= AmbaWrap_memset(NewFileNameRight, 0, sizeof(NewFileNameRight));
                AmbaUtility_StringAppend(NewFileNameRight, (UINT32)sizeof(NewFileNameRight), "c:\\Right_");
                AmbaUtility_StringAppendUInt32(NewFileNameRight, sizeof(NewFileNameRight), (UINT32)YWidth, 10U);
                AmbaUtility_StringAppend(NewFileNameRight, (UINT32)sizeof(NewFileNameRight), "x");
                AmbaUtility_StringAppendUInt32(NewFileNameRight, sizeof(NewFileNameRight), (UINT32)YHeight, 10U);
                AmbaUtility_StringAppend(NewFileNameRight, (UINT32)sizeof(NewFileNameRight), "_");
                AmbaUtility_StringAppendUInt32(NewFileNameRight, sizeof(NewFileNameRight), SerialNum, 10U);
                AmbaUtility_StringAppend(NewFileNameRight, (UINT32)sizeof(NewFileNameRight), ".y");
                AmbaPrint_PrintStr5("Right FileName %s", NewFileNameRight, NULL, NULL, NULL, NULL);

                RetVal = SvcStereoTask_FileWrite(NewFileNameRight, pRight_DstBufferY, YWidth * YHeight);
                //if (RetVal == SVC_OK) {
                    //SvcLog_OK(SVC_LOG_STEREO_TASK, "save right leyer %d ok", i, 0U);
                //}
            }
        }
    }

    return RetVal;
}


static void* SvcStereoTask_DumpSpuTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    static STEREO_SPU_MSG_s SpuMsg;

    AmbaMisra_TouchUnused(EntryArg);
    while (RetVal == SVC_OK) {
        RetVal = AmbaKAL_MsgQueueReceive(&SpuQueId, &SpuMsg, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK == RetVal) {
            /* SPU */
            if (SpuMsg.pOutSpu != NULL) {
                RetVal = SvcStereoTask_DumpSPU(SpuMsg.pOutSpu, g_dump_serial_num);
                if (RetVal == SVC_OK) {
                    SvcLog_OK(SVC_LOG_STEREO_TASK, "Dump SPU done, serial num %d FrameNum %d", g_dump_serial_num, SpuMsg.FrameNum);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "SpuMsg.pOutSpu == NULL", 0U, 0U);
            }

            /* Pyramid */
            if (SpuMsg.pPicInfo != NULL) {
                RetVal = SvcStereoTask_DumpPyramid(SpuMsg.pPicInfo, g_dump_serial_num);
                if (RetVal == SVC_OK) {
                    SvcLog_OK(SVC_LOG_STEREO_TASK, "Pyramid Copy done, serial num %d FrameNum %d", g_dump_serial_num, SpuMsg.FrameNum);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "SpuMsg.pPicInfo == NULL", 0U, 0U);
            }

            g_dump_serial_num++;
        }
    }
    return NULL;
}



static void* SvcStereoTask_DumpDSITaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    static STEREO_DSI_MSG_s DsiMsg;

    AmbaMisra_TouchUnused(EntryArg);

    while (RetVal == SVC_OK) {
        RetVal = AmbaKAL_MsgQueueReceive(&DsiQueId, &DsiMsg, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK == RetVal) {
            /* Dsi */
            if (DsiMsg.pOutData != NULL) {
                RetVal = StereoTask_DumpDSI(DsiMsg.pOutData, g_dump_serial_num);
                if (RetVal == SVC_OK) {
                    SvcLog_OK(SVC_LOG_STEREO_TASK, "Dump Dsi done, serial num %d FrameNum %d", g_dump_serial_num, DsiMsg.FrameNum);
                }
            } else {
                 SvcLog_NG(SVC_LOG_STEREO_TASK, "DsiMsg.pOutData == NULL", 0U, 0U);
            }

            if (DsiMsg.pPicInfo != NULL) {
                /* Pyramid */
                RetVal = SvcStereoTask_DumpPyramid(DsiMsg.pPicInfo, g_dump_serial_num);
                if (RetVal == SVC_OK) {
                    SvcLog_OK(SVC_LOG_STEREO_TASK, "Pyramid Copy done, serial num %d FrameNum %d", g_dump_serial_num, DsiMsg.FrameNum);
                }
            } else {
                 SvcLog_NG(SVC_LOG_STEREO_TASK, "DsiMsg.pPicInfo == NULL", 0U, 0U);
            }

            g_dump_serial_num++;
        }
    }

    return NULL;
}

static void* SvcStereoTask_DumpPgmTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    STEREO_PGM_MSG_s PgmMsg;

    AmbaMisra_TouchUnused(EntryArg);

    while (RetVal == SVC_OK) {
        RetVal = AmbaKAL_MsgQueueReceive(&PgmQueId, &PgmMsg, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK == RetVal) {
            /* Pgm */
            if ((PgmMsg.pPgmTbl != NULL) && (PgmMsg.PGMSize != 0U)) {
                RetVal = StereoTask_DumpPgm(PgmMsg.pPgmTbl, PgmMsg.PGMSize, g_dump_serial_num);
                if (RetVal == SVC_OK) {
                    SvcLog_OK(SVC_LOG_STEREO_TASK, "Dump Pgm done, serial num %d FrameNum %d", g_dump_serial_num, PgmMsg.FrameNum);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "PgmMsg.pPgmTbl == NULL or PgmMsg.PGMSize == 0", 0U, 0U);
            }

            /* Pyramid */
            if (PgmMsg.pPicInfo != NULL) {
                RetVal = SvcStereoTask_DumpPyramid(PgmMsg.pPicInfo, g_dump_serial_num);
                if (RetVal == SVC_OK) {
                    SvcLog_OK(SVC_LOG_STEREO_TASK, "Pyramid Copy done, serial num %d FrameNum %d", g_dump_serial_num, PgmMsg.FrameNum);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "PgmMsg.pPicInfo == NULL", 0U, 0U);
            }

            g_dump_serial_num++;
        }
    }
    return NULL;
}

static UINT32 SvcStereoTask_StereoSpuFexHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    char NewFileName[64];
    char *pStr;
    void *pAddr;
    ULONG ULONGAddr;
    const SVC_CV_STEREO_OUTPUT_s *pStereoOut;
    const REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s *pStereoData;
    UINT32 i;

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pOutput);

    AmbaMisra_TypeCast(&pStereoOut, &pOutput);
    AmbaMisra_TypeCast(&pStereoData, &(pStereoOut->DataAddr));

#if 0
    {
        extern void SvcCvFlow_DumpSpuOutInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu);
        SvcCvFlow_DumpSpuOutInfo(SVC_LOG_STEREO_TASK, pStereoData->pOutSpu);
    }
#endif

    SvcWrap_strcpy(NewFileName, sizeof(NewFileName), &g_FileInCtrl.InputCfg[0].FileName[0][0]);
    pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
#if 0  //For save binary
    AmbaMisra_TypeCast(&pAddr, &pStereoData->pOutSpu); /* pAddr = pStereoData->pOutSpu */
    if (pStr != NULL) {
        pStr[0] = '_';
        pStr[1] = 's';
        pStr[2] = 'p';
        pStr[3] = 'u';
        pStr[4] = '.';
        pStr[5] = 'b';
        pStr[6] = 'i';
        pStr[7] = 'n';
        pStr[8] = '\0';

        RetVal = SvcStereoTask_FileWrite(NewFileName, pAddr, 17975936U);
        if (RetVal == SVC_OK) {
            AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
        } else {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "[FinTask] Write NG 1", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "pStr == NULL", 0U, 0U);
    }

    AmbaMisra_TypeCast(&pAddr2, &pStereoData->pOutFex); /* pAddr2 = pStereoData->pOutFex */
    if (pStr != NULL) {
        pStr[0] = '_';
        pStr[1] = 'f';
        pStr[2] = 'e';
        pStr[3] = 'x';
        pStr[4] = '.';
        pStr[5] = 'b';
        pStr[6] = 'i';
        pStr[7] = 'n';
        pStr[8] = '\0';

        RetVal = SvcStereoTask_FileWrite(NewFileName, pAddr2, 2132992U);
        if (RetVal == SVC_OK) {
            AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
        } else {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "[FinTask] Write NG 2", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "pStr == NULL", 0U, 0U);
    }
#else
    for (i = 0U; i < 6U; i++) {
        /* SPU */
        if ((pStereoData->pOutSpu->Scales[i].Status == 0) && (pStereoData->pOutSpu->Scales[i].BufSize > 0U)) {
            AmbaMisra_TypeCast(&ULONGAddr, &pStereoData->pOutSpu); /* U32Addr = pStereoData->pOutSpu */
            ULONGAddr += (ULONG)pStereoData->pOutSpu->Scales[i].DisparityMapOffset;
            AmbaMisra_TypeCast(&pAddr, &ULONGAddr);
            DataSize = pStereoData->pOutSpu->Scales[i].BufSize;
            SvcWrap_PrintUL("DisparityMap[%u]: 0x%x Size: %d", (ULONG)i, ULONGAddr, (ULONG)DataSize, 0U, 0U);

            if (pStr != NULL) {
                pStr[0] = '_';
                pStr[1] = 's';
                pStr[2] = 'p';
                pStr[3] = 'u';
                RetVal = AmbaUtility_UInt32ToStr(&pStr[4], 2U, (UINT32)(i / 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                RetVal = AmbaUtility_UInt32ToStr(&pStr[5], 2U, (UINT32)(i % 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                pStr[6] = '.';
                pStr[7] = 'b';
                pStr[8] = 'i';
                pStr[9] = 'n';
                pStr[10] = '\0';

                RetVal = SvcStereoTask_FileWrite(NewFileName, pAddr, DataSize);
                if (RetVal == SVC_OK) {
                    AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
                }
            }
        }

        /* FEX Primary */
        DataSize = CV_FEX_MAX_BUCKETS;
        DataSize += CV_FEX_KEYPOINTS_SIZE * CV_FEX_MAX_BUCKETS * CV_FEX_MAX_KEYPOINTS;
        DataSize += CV_FEX_MAX_BUCKETS * CV_FEX_MAX_KEYPOINTS * CV_FEX_DESCRIPTOR_SIZE;
        if (pStereoData->pOutFex->PrimaryList[i].Enable > 0U) {
            AmbaMisra_TypeCast(&ULONGAddr, &pStereoData->pOutFex); /* U32Addr = pStereoData->pOutSpu */
            ULONGAddr += (ULONG)pStereoData->pOutFex->PrimaryList[i].KeypointsCountOffset;
            AmbaMisra_TypeCast(&pAddr, &ULONGAddr);
            SvcWrap_PrintUL("PrimaryFex[%u]: 0x%x Size: %d", (ULONG)i, ULONGAddr, (ULONG)DataSize, 0U, 0U);

            if (pStr != NULL) {
                pStr[0] = '_';
                pStr[1] = 'p';
                pStr[2] = 'r';
                pStr[3] = 'i';
                RetVal = AmbaUtility_UInt32ToStr(&pStr[4], 2U, (UINT32)(i / 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                RetVal = AmbaUtility_UInt32ToStr(&pStr[5], 2U, (UINT32)(i % 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                pStr[6] = '.';
                pStr[7] = 'f';
                pStr[8] = 'e';
                pStr[9] = 'x';
                pStr[10] = '\0';

                RetVal = SvcStereoTask_FileWrite(NewFileName, pAddr, DataSize);
                if (RetVal == SVC_OK) {
                    AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
                }
            }
        }

        /* FEX Secondary */
        if (pStereoData->pOutFex->SecondaryList[i].Enable > 0U) {
            AmbaMisra_TypeCast(&ULONGAddr, &pStereoData->pOutFex); /* U32Addr = pStereoData->pOutSpu */
            ULONGAddr += (ULONG)pStereoData->pOutFex->SecondaryList[i].KeypointsCountOffset;
            AmbaMisra_TypeCast(&pAddr, &ULONGAddr);
            SvcWrap_PrintUL("SecondaryFex[%u]: 0x%x Size: %d", (ULONG)i, ULONGAddr, (ULONG)DataSize, 0U, 0U);

            if (pStr != NULL) {
                pStr[0] = '_';
                pStr[1] = 's';
                pStr[2] = 'e';
                pStr[3] = 'c';
                RetVal = AmbaUtility_UInt32ToStr(&pStr[4], 2U, (UINT32)(i / 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                RetVal = AmbaUtility_UInt32ToStr(&pStr[5], 2U, (UINT32)(i % 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                pStr[6] = '.';
                pStr[7] = 'f';
                pStr[8] = 'e';
                pStr[9] = 'x';
                pStr[10] = '\0';

                RetVal = SvcStereoTask_FileWrite(NewFileName, pAddr, DataSize);
                if (RetVal == SVC_OK) {
                    AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
#endif
    return RetVal;
}

static UINT32 SvcStereoTask_StereoFusionHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    const SVC_CV_STEREO_OUTPUT_s *pStereoOut;
    const AMBA_CV_SPU_BUF_s *pOutBuf;
    const AMBA_CV_SPU_DATA_s *pOutData;
    static STEREO_DSI_MSG_s DsiMsg;

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pOutput);

    AmbaMisra_TypeCast(&pStereoOut, &pOutput);
    AmbaMisra_TypeCast(&pOutBuf, &(pStereoOut->DataAddr));
    AmbaMisra_TypeCast(&pOutData, &pOutBuf->pBuffer);

#if 0
    {
        extern void SvcCvFlow_DumpSpuOutInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu);
        SvcCvFlow_DumpSpuOutInfo(SVC_LOG_STEREO_TASK, pOutData);
    }
#endif

    AmbaMisra_TypeCast(&DsiMsg.pOutData, &pOutData);
    AmbaMisra_TypeCast(&DsiMsg.pPicInfo, &pStereoOut->pPicInfo);
    DsiMsg.FrameNum = pStereoOut->FrameNum;

    RetVal = AmbaKAL_MsgQueueSend(&DsiQueId, &DsiMsg, 5000);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_OsdHandler: PgmMsgQueueSend error", 0U, 0U);
    }

    return RetVal;
}


static UINT32 SvcStereoTask_StereoOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    const SVC_CV_STEREO_OUTPUT_s *pStereoOut;

    (void) pOutput;

    AmbaMisra_TypeCast(&pStereoOut, &pOutput);
    SvcLog_DBG(SVC_LOG_STEREO_TASK, "StereoOutputHandler: Type(0x%x)", pStereoOut->Type, 0U);

    switch (pStereoOut->Type) {
        case STEREO_OUT_TYPE_SPU_FEX:
            RetVal = SvcStereoTask_StereoSpuFexHandler(Chan, CvType, pOutput);
            break;
        case STEREO_OUT_TYPE_FUSION:
            RetVal = SvcStereoTask_StereoFusionHandler(Chan, CvType, pOutput);
            break;
        default:
            /* Do nothing */
            break;
    }

    return RetVal;
}

static void SvcStereoTask_CvFlowOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    UINT32 MsgCode;

    AmbaMisra_TouchUnused(&CvType);

    if ((Chan == g_FileInCtrl.Config.CvFlowChan) && (pOutput != NULL)) {
        AmbaMisra_TypeCast(&MsgCode, pOutput);
        SvcLog_DBG(SVC_LOG_STEREO_TASK, "OutputHandler start. MsgCode(0x%x)", MsgCode, 0U);

        switch (MsgCode) {
#if defined(CONFIG_ICAM_CV_STEREO)
            case SVC_CV_STEREO_OUTPUT:
                RetVal = SvcStereoTask_StereoOutputHandler(Chan, CvType, pOutput);;
                break;
#endif
            default:
                /* Do nothing */
                break;
        }

        if (RetVal == SVC_OK) {
            SvcLog_OK(SVC_LOG_STEREO_TASK, "OutputHandler Done", 0U, 0U);
        } else {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "OutputHandler Done with error", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "OutputHandler Done with invalid info", 0U, 0U);
    }
}

static UINT32 SvcStereoTask_SendInput_Binary(UINT32 CvFlowChan, const ULONG *pSpuAddr)
{
    UINT32 RetVal;
    const void *pCVBuf = NULL;
    AmbaMisra_TypeCast(&pCVBuf, pSpuAddr);

    RetVal = SvcCvFlow_Control(CvFlowChan, SVC_CV_CTRL_SEND_EXT_PIC_INFO, pCVBuf);

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "SEND_EXT_YUV error", 0U, 0U);
    }

    return RetVal;
}

static UINT32 SvcStereoTask_SendInput_Fusion(UINT32 CvFlowChan, UINT32 InputNum, const char* InFile1, const char* InFile2, const char* InFile3)
{
    UINT32 RetVal;
    UINT32 LayerNum = InputNum;
    UINT32 Disp2layer_0Width = 1920UL;
    UINT32 Disp2layer_0Height = 1080UL;
    UINT32 Disp2layer_0Pitch = 2400UL;
    UINT32 Disp2layer_2Width = 960UL;
    UINT32 Disp2layer_2Height = 540UL;
    UINT32 Disp2layer_2Pitch = 1216UL;

    UINT32 Disp3layer_0Width = 960UL;
    UINT32 Disp3layer_0Height = 1080UL;
    UINT32 Disp3layer_0Pitch = 1216UL;
    UINT32 Disp3layer_2Width = 960UL;
    UINT32 Disp3layer_2Height = 540UL;
    UINT32 Disp3layer_2Pitch = 1216UL;
    UINT32 Disp3layer_4Width = 480UL;
    UINT32 Disp3layer_4Height = 270UL;
    UINT32 Disp3layer_4Pitch = 608UL;

    ULONG ULONGInBuf;

    ULONG SpuAddr, PicAddr;
    AMBA_CV_SPU_DATA_s *pSpuIn = NULL;
    memio_source_recv_picinfo_t *pPicIn = NULL;
    STEREO_RX1_MSG_s Rx1;
    STEREO_RX1_MSG_s *pRx1 = &Rx1;
    REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s StereoData;
    REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s *pStereoData = &StereoData;
    ULONG ResultAddr;
    SVC_CV_STEREO_OUTPUT_s Output;
    SVC_CV_STEREO_OUTPUT_s *pOutout = &Output;
    UINT32 InputBufSize;
    InputBufSize = SvcStereoTask_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
            SvcStereoTask_CacheAligned(Disp2layer_0Pitch*Disp2layer_0Height) +
            SvcStereoTask_CacheAligned(Disp2layer_2Pitch*Disp2layer_2Height) +
            SvcStereoTask_CacheAligned(Disp3layer_4Pitch*Disp3layer_4Height);

    RetVal = SvcStereoTask_AllocBuf(InputBufSize, &SpuAddr);
    RetVal = SvcStereoTask_AllocBuf(sizeof(memio_source_recv_picinfo_t), &PicAddr);
    AmbaMisra_TypeCast(&pSpuIn, &SpuAddr);
    AmbaMisra_TypeCast(&pPicIn, &PicAddr);

    // Setup SP_DISPARITY_MAP
    {
        // Setup header
        {
            AmbaMisra_TypeCast(&pSpuIn->Reserved_0[0], &pSpuIn);
            pSpuIn->Reserved_0[0] = MAX_HALF_OCTAVES;
            pSpuIn->Reserved_0[1] = Disp2layer_0Height;
            pSpuIn->Reserved_0[2] = Disp2layer_0Width;
            pSpuIn->Reserved_0[3] = 0UL;
        }

        // Zero out all scales
        {
            UINT32 ScaleIdx;
            for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++){
                RetVal = AmbaWrap_memset(&(pSpuIn->Scales[ScaleIdx]), 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "memset Scales failed", 0U, 0U);
                }
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
                    SvcStereoTask_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)); // offset to dispmap
                pSpuIn->Scales[0U].Reserved_2 = 0UL;
                pSpuIn->Scales[0U].Reserved_3 = 0UL;

                // Cat disp map
                AmbaMisra_TypeCast(&ULONGInBuf, &pSpuIn);
                ULONGInBuf += pSpuIn->Scales[0U].DisparityMapOffset;
                RetVal |= SvcStereoTask_LoadFusionBinary(InFile1, ULONGInBuf);
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
                    SvcStereoTask_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                    SvcStereoTask_CacheAligned(Disp2layer_0Pitch*Disp2layer_0Height); // offset to dispmap
                pSpuIn->Scales[2U].Reserved_2 = 0UL;
                pSpuIn->Scales[2U].Reserved_3 = 0UL;

                // Cat disp map
                AmbaMisra_TypeCast(&ULONGInBuf, &pSpuIn);
                ULONGInBuf += pSpuIn->Scales[2U].DisparityMapOffset;
                RetVal |= SvcStereoTask_LoadFusionBinary(InFile2, ULONGInBuf);
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
                pSpuIn->Scales[0U].Reserved_0 = 8U;
                pSpuIn->Scales[0U].RoiStartRow = 0UL;
                pSpuIn->Scales[0U].RoiStartCol = 0UL;
                pSpuIn->Scales[0U].Reserved_1 = 1UL;
                pSpuIn->Scales[0U].RoiAbsoluteStartCol = 0UL;
                pSpuIn->Scales[0U].RoiAbsoluteStartRow = 0UL;
                pSpuIn->Scales[0U].InvalidDisparities = 0UL; //No use
                pSpuIn->Scales[0U].DisparityMapOffset =
                    SvcStereoTask_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)); // offset to dispmap
                pSpuIn->Scales[0U].Reserved_2 = 0UL;
                pSpuIn->Scales[0U].Reserved_3 = 0UL;

                // Cat disp map
                AmbaMisra_TypeCast(&ULONGInBuf, &pSpuIn);
                ULONGInBuf += pSpuIn->Scales[0U].DisparityMapOffset;
                RetVal |= SvcStereoTask_LoadFusionBinary(InFile1, ULONGInBuf);
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
                    SvcStereoTask_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                    SvcStereoTask_CacheAligned(Disp3layer_0Pitch*Disp3layer_0Height); // offset to dispmap
                pSpuIn->Scales[2U].Reserved_2 = 0UL;
                pSpuIn->Scales[2U].Reserved_3 = 0UL;

                // Cat disp map
                AmbaMisra_TypeCast(&ULONGInBuf, &pSpuIn);
                ULONGInBuf += pSpuIn->Scales[2U].DisparityMapOffset;
                RetVal |= SvcStereoTask_LoadFusionBinary(InFile2, ULONGInBuf);
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
                    SvcStereoTask_CacheAligned(sizeof(AMBA_CV_SPU_DATA_s)) +
                    SvcStereoTask_CacheAligned(Disp3layer_2Pitch*Disp3layer_2Height) +
                    SvcStereoTask_CacheAligned(Disp3layer_0Pitch*Disp3layer_0Height); // offset to dispmap
                pSpuIn->Scales[4U].Reserved_2 = 0UL;
                pSpuIn->Scales[4U].Reserved_3 = 0UL;

                // Cat disp map
                AmbaMisra_TypeCast(&ULONGInBuf, &pSpuIn);
                ULONGInBuf += pSpuIn->Scales[4U].DisparityMapOffset;
                RetVal |= SvcStereoTask_LoadFusionBinary(InFile3, ULONGInBuf);
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
            for(ScaleIdx = 0U; ScaleIdx < MAX_HALF_OCTAVES; ScaleIdx++){
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

    if (CACHE_ERR_NONE != SvcPlat_CacheClean(PicAddr, SvcStereoTask_CacheAligned(sizeof(memio_source_recv_picinfo_t)))) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Clean Cache error(%d)", RetVal, 0U);
    }

    if (CACHE_ERR_NONE != SvcPlat_CacheClean(SpuAddr, InputBufSize)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Clean Cache error(%d)", RetVal, 0U);
    }

    pStereoData->pOutSpu = pSpuIn;

    AmbaMisra_TypeCast(&ResultAddr, &pStereoData);
    pOutout->MsgCode    = SVC_CV_STEREO_OUTPUT;
    pOutout->Type        = STEREO_OUT_TYPE_SPU_FEX;
    pOutout->StereoID   = 0;
    pOutout->FrameSetID = 0;
    pOutout->DataAddr   = ResultAddr;    /* REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s */

    pRx1->pStereoOut = pOutout;
    pRx1->pPicInfo = pPicIn;

    RetVal = SvcCvFlow_Control(CvFlowChan, SVC_CV_CTRL_SEND_FUSION_INFO, pRx1);

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "SEND_EXT_YUV error", 0U, 0U);
    }

    return RetVal;
}

UINT32 SvcStereoTask_StartSpuFexBinary(const char* InFile)
{
    UINT32 RetVal = SVC_OK;
    ULONG SpuAddr;

    SvcStereoTask_InitBuf();

    SvcWrap_strcpy(&g_FileInCtrl.InputCfg[0].FileName[0][0], SVC_STEREO_MAX_FILE_PATH - 1U, InFile);
    RetVal |= SvcStereoTask_LoadSpuFexBinary(InFile, &SpuAddr);

    if (RetVal == SVC_OK) {
       RetVal = SvcStereoTask_SendInput_Binary(g_FileInCtrl.Config.CvFlowChan, &SpuAddr);
    }

    return RetVal;
}

UINT32 SvcStereoTask_StartFusionBinary(UINT32 InputNum, const char* InFile1, const char* InFile2, const char* InFile3)
{
    UINT32 RetVal = SVC_OK;

    SvcStereoTask_InitBuf();

    SvcWrap_strcpy(&g_FileInCtrl.InputCfg[0].FileName[0][0], SVC_STEREO_MAX_FILE_PATH - 1U, InFile1);

    RetVal = SvcStereoTask_SendInput_Fusion(g_FileInCtrl.Config.CvFlowChan, InputNum, InFile1, InFile2, InFile3);

    return RetVal;
}

/**
 *  Configure the CV file input task
 *  @param[in] pConfig CV File-In configs
 *  @return error code
 */
UINT32 SvcStereoTask_FileIn_Config(const SVC_STEREO_CONFIG_s *pConfig)
{
    UINT32 RetVal;
    static UINT32 g_CvFlowRegisterID = 0xffU;

    if (NULL != pConfig) {
        SvcLog_DBG(SVC_LOG_STEREO_TASK, "Config CvFlowChan(%u)", pConfig->CvFlowChan, 0U);

        RetVal = AmbaWrap_memset(&g_FileInCtrl, 0, sizeof(SVC_STEREO_CTRL_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "memset g_FileInCtrl failed", 0U, 0U);
        }
        RetVal = AmbaWrap_memcpy(&g_FileInCtrl.Config, pConfig, sizeof(SVC_STEREO_CONFIG_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "memcpy Config failed", 0U, 0U);
        }

        if (g_CvFlowRegisterID == 0xffU) {
            RetVal = SvcCvFlow_Register(pConfig->CvFlowChan, SvcStereoTask_CvFlowOutputHandler, &g_CvFlowRegisterID);
        } else {
            RetVal = SVC_OK;
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "pConfig = NULL", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Init the stereo module
 *  @return error code
 */
UINT32 SvcStereoTask_Init(void)
{
    UINT32 RetVal = SVC_OK;

    /* Initialize global settings */
    {
        RetVal = AmbaWrap_memset(&g_OsdCfg, 0, sizeof(OSD_CONFIG_s));
    }

    if (SVC_OK == RetVal) {
        StereoInit = 1U;
    }

    if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&StereoMutex, NULL)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "MutexCreate error", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Init the stereo module
 *  @return error code
 */
UINT32 SvcStereoTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    if (StereoInit == 1U) {
        /* Init Stereo CV Flow based on ResCfg */
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_STEREO) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL)) {
                    StereoCvfEnable = 1U;
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask has not initialized", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}


/**
 *  Start the stereo task
 *  @return error code
 */
UINT32 SvcStereoTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 RegisterID;
    UINT32 BufSize = 0U;
    ULONG  OsdSize = 0U;
    ULONG  BufBase = 0U;
    UINT32 FusionEnable;
    static SVC_TASK_CTRL_s DSIDump_TaskCtrl;
    static SVC_TASK_CTRL_s PGMDump_TaskCtrl;
    static SVC_TASK_CTRL_s SPUDump_TaskCtrl;
    static UINT8 DsiDump_TaskStack[DSIDUMP_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 PgmDump_TaskStack[PGMDUMP_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 SpuDump_TaskStack[SPUDUMP_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char  DsiDump_TaskName[16] = "DsiDump_Task";
    static char  PgmDump_TaskName[16] = "PgmDump_Task";
    static char  SpuDump_TaskName[16] = "SpuDump_Task";
    static char DsiQueName[] = "DsiQue";
    static char PgmQueName[] = "PgmQue";
    static char SpuQueName[] = "SpuQue";
    static STEREO_DSI_MSG_s DsiQue[SVC_STEREO_DUMP_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static STEREO_PGM_MSG_s PgmQue[SVC_STEREO_DUMP_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    static STEREO_SPU_MSG_s SpuQue[SVC_STEREO_DUMP_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;

    if (StereoInit == 1U) {
        if (StereoCvfEnable == 1U) {
            /* Request memory for OSD/Data Store Buffer*/
            {
                RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_STEREO, &BufBase, &BufSize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcBuffer_Request() failed", 0U, 0U);
                }
                SvcLog_DBG(SVC_LOG_STEREO_TASK, "SvcBuffer_Request() 0x%X 0x%X", BufBase, BufSize);
            }

            if (SVC_OK == RetVal) {
                RetVal = StereoTask_InitOsd(BufBase);
                RetVal |= StereoTask_GetOsdBufSize(&OsdSize);
                if (SVC_OK == RetVal) {
                    //BufBase += ((OsdSize * SVC_STEREO_MAX_OSD_NUM) + SVC_STEREO_OSD_CLUT_SIZE);
                } else {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_InitOsd failed with %d", RetVal, 0U);
                }
            }

            if (SVC_OK == RetVal) {
                /* Start Stereo CV Flow based on ResCfg */
                for (i = 0U; i < CvFlowNum; i++) {
                    if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                        if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_STEREO) ||
                           (pCvFlow[i].CvFlowType == SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL)) {
                            RetVal |= SvcCvFlow_Register(i, StereoTask_Callback, &RegisterID);
                            if (pCvFlow[i].CvFlowType == SVC_CV_FLOW_STEREO) {
                                if (SVC_OK == SvcCvFlow_Control(i, SVC_CV_CTRL_GET_FUSION_INFO, &FusionEnable)) {
                                    if (FusionEnable == 1U) {
                                        g_OsdCfg.Fusion = 1U;
                                    }
                                }
                            } else {
                                g_OsdCfg.Fusion = 1U;
                            }
                        }
                    }
                }
                StereoOnOff = 1U;
            }


            /* Create task for Pyramid Data Copy */
            if (StereoOnOff == 1U) {
                RetVal = AmbaKAL_MsgQueueCreate(&DsiQueId,
                                           DsiQueName,
                                           sizeof(STEREO_DSI_MSG_s),
                                           &DsiQue[0],
                                           SVC_STEREO_DUMP_QUEUE_SIZE * sizeof(STEREO_DSI_MSG_s));

                RetVal = AmbaKAL_MsgQueueCreate(&PgmQueId,
                                           PgmQueName,
                                           sizeof(STEREO_PGM_MSG_s),
                                           &PgmQue[0],
                                           SVC_STEREO_DUMP_QUEUE_SIZE * sizeof(STEREO_PGM_MSG_s));

                RetVal = AmbaKAL_MsgQueueCreate(&SpuQueId,
                                           SpuQueName,
                                           sizeof(STEREO_SPU_MSG_s),
                                           &SpuQue[0],
                                           SVC_STEREO_DUMP_QUEUE_SIZE * sizeof(STEREO_SPU_MSG_s));

                DSIDump_TaskCtrl.Priority   = DSIDUMP_TASK_PRIORITY;
                DSIDump_TaskCtrl.EntryFunc  = SvcStereoTask_DumpDSITaskEntry;
                DSIDump_TaskCtrl.EntryArg   = 0U;
                DSIDump_TaskCtrl.pStackBase = DsiDump_TaskStack;
                DSIDump_TaskCtrl.StackSize  = DSIDUMP_TASK_STACK_SIZE;
                DSIDump_TaskCtrl.CpuBits    = 0x1U;

                RetVal = SvcTask_Create(DsiDump_TaskName, &DSIDump_TaskCtrl);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "DsiDumpTask err. Task create failed %d", RetVal, 0U);
                }

                PGMDump_TaskCtrl.Priority   = PGMDUMP_TASK_PRIORITY;
                PGMDump_TaskCtrl.EntryFunc  = SvcStereoTask_DumpPgmTaskEntry;
                PGMDump_TaskCtrl.EntryArg   = 0U;
                PGMDump_TaskCtrl.pStackBase = PgmDump_TaskStack;
                PGMDump_TaskCtrl.StackSize  = PGMDUMP_TASK_STACK_SIZE;
                PGMDump_TaskCtrl.CpuBits    = 0x1U;

                RetVal = SvcTask_Create(PgmDump_TaskName, &PGMDump_TaskCtrl);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "PgmDumpTask err. Task create failed %d", RetVal, 0U);
                }

                SPUDump_TaskCtrl.Priority   = SPUDUMP_TASK_PRIORITY;
                SPUDump_TaskCtrl.EntryFunc  = SvcStereoTask_DumpSpuTaskEntry;
                SPUDump_TaskCtrl.EntryArg   = 0U;
                SPUDump_TaskCtrl.pStackBase = SpuDump_TaskStack;
                SPUDump_TaskCtrl.StackSize  = SPUDUMP_TASK_STACK_SIZE;
                SPUDump_TaskCtrl.CpuBits    = 0x1U;

                RetVal = SvcTask_Create(SpuDump_TaskName, &SPUDump_TaskCtrl);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "SpuDumpTask err. Task create failed %d", RetVal, 0U);
                }
            }
        } else {
            /* Do nothing */
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask has not initialized", 0U, 0U);
        RetVal = SVC_NG;
    }

    HaveStereoData &= ~(STEREODRAW_GUI_SHOW | STEREODRAW_GUI_UPDATE);

    return RetVal;
}

/**
 *  Stop the object detection
 *  @return error code
 */
UINT32 SvcStereoTask_Stop(void)
{
    UINT32 RetVal;

    if (StereoInit == 1U) {
        StereoCvfEnable = 0U;
        StereoOnOff = 0U;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Stereo module not init yet. Cannot stop", 0U, 0U);
        RetVal = SVC_NG;
    }

    HaveStereoData &= ~(STEREODRAW_GUI_SHOW);
    return RetVal;
}


/**
 *  Get current status of Object detection task
 *  @return error code
 */
UINT32 SvcStereoTask_GetStatus(UINT32 *pEnable)
{
    UINT32 RetVal;

    if (StereoInit == 1U) {
        *pEnable = StereoOnOff;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Stereo module not init yet. Cannot get status", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Control source rate of stereo process
 *  @param[in] RateDivisor pointer to frame rate divisor
 *  @return error code
 */
UINT32 SvcStereoTask_Ctrl(const char *pCmd, void *pParam, UINT32 Value)
{
    UINT32 RetVal = SVC_OK;
    const char *pCharPtr;
    static char StereoDumpName[SVC_STEREO_MAX_FILE_PATH];
    static char StereoPGMDumpName[SVC_STEREO_MAX_FILE_PATH];

    AmbaMisra_TouchUnused(pParam);

    if (StereoInit == 1U) {
        if (0 == SvcWrap_strcmp("dump_fus_dsi", pCmd)) {
            AmbaMisra_TypeCast(&pCharPtr, &pParam);
            SvcWrap_strcpy(StereoDumpName, SVC_STEREO_MAX_FILE_PATH, pCharPtr);
            StereoDumpDsi = Value;
        } else if (0 == SvcWrap_strcmp("dump_fus_pgm", pCmd)) {
            AmbaMisra_TypeCast(&pCharPtr, &pParam);
            SvcWrap_strcpy(StereoPGMDumpName, SVC_STEREO_MAX_FILE_PATH, pCharPtr);
            StereoDumpPgm = Value;
        } else if (0 == SvcWrap_strcmp("dump_spu", pCmd)) {
            AmbaMisra_TouchUnused(&pParam);
            StereoDumpSpu = Value;
        } else {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcStereoTask_Ctrl failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask has not intialized", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Control source rate of stereo process
 *  @param[in] RateDivisor pointer to frame rate divisor
 *  @return error code
 */
UINT32 SvcStereoTask_SetRateCtrl(const UINT32 *RateDivisor)
{
    UINT32 RetVal = SVC_NG;

    if (StereoInit == 1U) {
        UINT32 i;
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
        const void *pCtrl;

        AmbaMisra_TypeCast(&pCtrl, &RateDivisor);
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_STEREO) ||
                     (pCvFlow[i].CvFlowType == SVC_CV_FLOW_LINUX_AC_FUSION_STIXEL)) {
                    RetVal |= SvcCvFlow_Control(i, SVC_CV_CTRL_SET_SRC_RATE, pCtrl);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Stereo module not init yet. Cannot set rate ctrl.", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Enable or disable OSD
 *  @param[in] Enable  1 to enable OSD, 0 to disable
 *  @return error code
 */
UINT32 SvcStereoTask_EnableOSD(UINT32 Enable)
{
    UINT32 RetVal= SVC_OK;
    ULONG  OsdSize = 0U;
    UINT8 *pOsdBuf;
    UINT32 OsdBufIdx = 0U;
    UINT32 FrameNum;

    if (1U != StereoInit) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Stereo task is not initialized or not running", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (0xFFU == g_OsdCfg.DispStrmIdx) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Invalid DispStrmIdx", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        g_OsdCfg.Show = Enable;

        if (0U == Enable) {
            RetVal = StereoTask_GetOsdBufSize(&OsdSize);
            RetVal |= StereoTask_GetOsdBufIdx(&OsdBufIdx, NULL, &FrameNum);
            AmbaMisra_TouchUnused(&FrameNum);

            if (SVC_OK == RetVal) {
                AmbaMisra_TypeCast(&pOsdBuf, &(g_OsdCfg.BufAddr[OsdBufIdx]));
                RetVal = AmbaWrap_memset(pOsdBuf, 1023, OsdSize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "memset OsdBuf failed", 0U, 0U);
                }
#if SVC_STEREO_ENABLE_EXT_OSD_PATH
                if (1U == g_OsdCfg.PostInit) {
                    RetVal = SvcOsd_UpdateExtBuf(g_OsdCfg.VoutID, pOsdBuf);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcOsd_UpdateExtBuf failed", 0U, 0U);
                    }
                }
#endif
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_GetOsdBufSize failed", 0U, 0U);
            }
        }
    }

    return RetVal;
}


/**
 *  Query memory size
 *  @param[out] pMemSize Memory size
 *  @return error code
 */
UINT32 SvcStereoTask_QueryBufSize(UINT32 *pMemSize)
{
#if 1
    *pMemSize = 64; /* Temp fix */
    return SVC_OK;
#else
    UINT32 RetVal = SVC_OK, EnableStereoFlow = 0U;

    if (NULL == pMemSize) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcStereoTask_QueryBufSize input NULL pointer!", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        *pMemSize = 0U;
    }

    if (SVC_OK == RetVal) {
        UINT32 i;
        const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
        UINT32 CvFlowNum = pResCfg->CvFlowNum;
        UINT32 CvFlowBits = pResCfg->CvFlowBits;
        const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

        /* Start Stereo CV Flow based on ResCfg */
        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if (pCvFlow[i].CvFlowType == SVC_CV_FLOW_STEREO) {
                    EnableStereoFlow = 1U;
                }
            }
        }
    }

    if (1U == EnableStereoFlow) {
        if ((SVC_OK == RetVal) && (NULL != pMemSize)) {
            UINT32 OsdBufSize = 0U;
            RetVal = StereoTask_GetOsdBufSize(&OsdBufSize);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_GetOsdBufSize failed with err 0x%x!", RetVal, 0U);
            } else {
                OsdBufSize *= SVC_STEREO_MAX_OSD_NUM;
                OsdBufSize += SVC_STEREO_OSD_CLUT_SIZE;
                *pMemSize += OsdBufSize;
            }
        }

        if (SVC_OK == RetVal) {
            SvcLog_OK(SVC_LOG_STEREO_TASK, "Query stereo task memory size 0x%X", *pMemSize, 0U);
        }
    }

    return RetVal;
#endif
}

/**
 *  Enable stereo task debug flag
 *  @param[in] DebugEnable Debug flag
 *  @return error code
 */
void SvcStereoTask_DebugEnable(UINT32 DebugEnable)
{
    StereoDebugEnable = DebugEnable;
    SvcLog_OK(SVC_LOG_STEREO_TASK, "StereoDebugEnable = %d", StereoDebugEnable, 0U);
}

static void StereoTask_RxCallback(UINT32 CvFlowChan, const SVC_CV_STEREO_OUTPUT_s *pStereoOutput)
{
    UINT32 RetVal;
    static STEREO_OSD_MSG_s OsdMsg;
    const AMBA_CV_SPU_BUF_s *pOutBuf;
    const AMBA_CV_SPU_DATA_s *pOutData;
    const REF_CV_SPU_FEX_WRAP_OUTPUT_DATA_s *pSpuFexData;
#if defined(CONFIG_ICAM_CV_STIXEL)
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 DownstrmChan;
#endif
    static memio_source_recv_picinfo_t *pPicInfo;
    static STEREO_DSI_MSG_s DsiMsg;
    static STEREO_SPU_MSG_s SpuMsg;

    if (1U == g_OsdCfg.Show) {
        OsdMsg.CvFlowChan = CvFlowChan;

        if ((pStereoOutput->Type == STEREO_OUT_TYPE_SPU_FEX) && (g_OsdCfg.Fusion == 0U)) {
            SVC_STEREO_TASK_DBG(SVC_LOG_STEREO_TASK, "STEREO_OUT_TYPE_SPU_FEX", 0U, 0U);

            AmbaMisra_TypeCast(&pSpuFexData, &(pStereoOutput->DataAddr));
            AmbaMisra_TypeCast(&OsdMsg.pOutSpu, &pSpuFexData->pOutSpu);
            AmbaMisra_TypeCast(&OsdMsg.pOutFex, &pSpuFexData->pOutFex);
            AmbaMisra_TypeCast(&OsdMsg.FrameNum, &pStereoOutput->FrameNum);
            AmbaMisra_TypeCast(&OsdMsg.pPicInfo, &pStereoOutput->pPicInfo);

            if (StereoDumpSpu > 0U) {
                /* Dump SPU disparity */
                AmbaMisra_TypeCast(&SpuMsg.pOutSpu, &pSpuFexData->pOutSpu);
                AmbaMisra_TypeCast(&SpuMsg.pPicInfo, &pStereoOutput->pPicInfo);
                SpuMsg.FrameNum = pStereoOutput->FrameNum;

                RetVal = AmbaKAL_MsgQueueSend(&SpuQueId, &SpuMsg, 5000);
                if (SVC_OK != RetVal) {
                    //SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_OsdHandler: SpuMsgQueueSend error", 0U, 0U);
                } else {
                    StereoDumpSpu--;
                }
            }
        } else if ((pStereoOutput->Type == STEREO_OUT_TYPE_FUSION) && (g_OsdCfg.Fusion == 1U)) {
            SVC_STEREO_TASK_DBG(SVC_LOG_STEREO_TASK, "Recv Fusion out", 0U, 0U);

            AmbaMisra_TypeCast(&pOutBuf, &(pStereoOutput->DataAddr));
            AmbaMisra_TypeCast(&pOutData, &pOutBuf->pBuffer);
            AmbaMisra_TypeCast(&pPicInfo, &pStereoOutput->pPicInfo);

            /* Dump disparity */
            if (StereoDumpDsi > 0U) {
                AmbaMisra_TypeCast(&DsiMsg.pOutData, &pOutData);
                AmbaMisra_TypeCast(&DsiMsg.pPicInfo, &pStereoOutput->pPicInfo);
                DsiMsg.FrameNum = pStereoOutput->FrameNum;

                RetVal = AmbaKAL_MsgQueueSend(&DsiQueId, &DsiMsg, 5000);
                if (SVC_OK != RetVal) {
                    //SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_OsdHandler: DsiMsgQueueSend error", 0U, 0U);
                } else {
                    StereoDumpDsi--;
                }
            }

            AmbaMisra_TypeCast(&OsdMsg.pOutSpu, &pOutData);
            OsdMsg.pOutFex = NULL;
            AmbaMisra_TypeCast(&OsdMsg.FrameNum, &pStereoOutput->FrameNum);
            AmbaMisra_TypeCast(&OsdMsg.pPicInfo, &pStereoOutput->pPicInfo);
#if defined(CONFIG_ICAM_CV_STIXEL)
            /* Pass the result to downstream CvFlow */
            if ((pCvFlow[CvFlowChan].DownstrmChanNum > 0U)) {
                DownstrmChan = pCvFlow[CvFlowChan].DownstrmChan[0];
                if (pCvFlow[DownstrmChan].CvFlowType == SVC_CV_FLOW_STIXEL) {
                    SVC_STIXEL_TASK_DSI_IN_s DsiIn;

                    DsiIn.CvFlowChan = DownstrmChan;
                    AmbaMisra_TypeCast(&DsiIn.pDsiInfo, &pStereoOutput);
                    RetVal = SvcStixelTask_SendDsiInput(&DsiIn);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_STEREO_TASK, "SendDsiInput failed", 0U, 0U);
                    }
                }
            }
#endif
        } else {
            /* Do nothing */
        }

        if((g_OsdCfg.PostInit == 0U) && (g_osd_enable == SVC_STEREO_OSD_ENABLE)) {
            /* If fusion is not enabled, initialize when receives STEREO_OUT_TYPE_SPU_FEX */
            /* If fusion is enabled, initialize when receives STEREO_OUT_TYPE_FUSION */
            if (((g_OsdCfg.Fusion == 0U) && (pStereoOutput->Type == STEREO_OUT_TYPE_SPU_FEX)) ||
                ((g_OsdCfg.Fusion == 1U) && (pStereoOutput->Type == STEREO_OUT_TYPE_FUSION))) {
                (void) StereoTask_PostInitOsd(&OsdMsg);
            }
        }

        if (g_osd_enable == SVC_STEREO_OSD_ENABLE) {
            RetVal = AmbaKAL_MsgQueueSend(&(g_OsdCfg.MsgQueId), &OsdMsg, AMBA_KAL_NO_WAIT);
            if (KAL_ERR_NONE != RetVal) {
                //SvcLog_NG(SVC_LOG_STEREO_TASK, "OsdMsgQue send failed with err 0x%X!", RetVal, 0U);
            }
        }
    }
}

static void StereoTask_Callback(UINT32 CvFlowChan, UINT32 CvType, void *pMsg)
{
    const SVC_CV_STEREO_OUTPUT_s *pStereoOutput;
    const UINT32 *pMsgCode;

    AmbaMisra_TouchUnused(&CvType);
    AmbaMisra_TouchUnused(pMsg);
    AmbaMisra_TypeCast(&pMsgCode, &pMsg);

    if (*pMsgCode == SVC_CV_STEREO_OUTPUT) {
        if (1U == StereoOnOff) {
            AmbaMisra_TypeCast(&pStereoOutput, &pMsg);
            switch (pStereoOutput->Type) {
                case STEREO_OUT_TYPE_FUSION:
                case STEREO_OUT_TYPE_SPU_FEX:
                    StereoTask_RxCallback(CvFlowChan, pStereoOutput);
                    break;
                default:
                    /* Do nothing */
                    break;
            }
        }
        HaveStereoData |= (STEREODRAW_GUI_SHOW | STEREODRAW_GUI_UPDATE);
    } else {
       //do nothing
    }
}

static UINT32 StereoTask_GetOsdBufSize(ULONG *pSize)
{

    UINT32 RetVal = SVC_OK;

    if (NULL != pSize) {
        *pSize = (ULONG)(SVC_STEREO_MAX_OSD_BUF_WIDTH * SVC_STEREO_MAX_OSD_BUF_HEIGHT);
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 StereoTask_GetOsdBufIdx(UINT32 *pBufIdx, UINT32 *pBufIdxNext, UINT32 *pFrameNum)
{
    UINT32 RetVal = SVC_OK;
    UINT32 BufIdx;

    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&g_OsdCfg.OsdMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "OsdMutex timeout", 0U, 0U);
    }

    *pBufIdx = g_OsdCfg.OsdBufIdx;
    if (NULL != pBufIdxNext) {
        BufIdx = (UINT32)g_OsdCfg.OsdBufIdx + 1U;
        BufIdx %= SVC_STEREO_MAX_OSD_NUM;
        *pBufIdxNext = BufIdx;
    }

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
    *pFrameNum = g_OsdCfg.FrameNum;
#else
    *pFrameNum = 0U;
#endif


    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&g_OsdCfg.OsdMutex)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "OsdMutex timeout", 0U, 0U);
    }

    return RetVal;
}

static UINT32 StereoTask_UpdateOsdBufIdx(UINT32 BufIdx, UINT32 FrameNum)
{

    UINT32 RetVal = SVC_OK;

    if (KAL_ERR_NONE != AmbaKAL_MutexTake(&g_OsdCfg.OsdMutex, 5000)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "OsdMutex timeout", 0U, 0U);
    }

    g_OsdCfg.OsdBufIdx = (UINT8)BufIdx;
    g_OsdCfg.FrameNum = FrameNum;

    if (KAL_ERR_NONE != AmbaKAL_MutexGive(&g_OsdCfg.OsdMutex)) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "OsdMutex timeout", 0U, 0U);
    }

    return RetVal;
}


static UINT32 StereoTask_InitOsd(ULONG BaseAddr)
{
    UINT32 i, RetVal = SVC_OK;
    ULONG OsdSize = 0U;
    const SVC_RES_CFG_s *pSvcResCfg = SvcResCfg_Get();

    /* Initialize global settings */
    g_OsdCfg.DispStrmIdx = 0xFFU;
    g_OsdCfg.VoutID = VOUT_IDX_B;

    for(i = 0U; i < pSvcResCfg->DispNum; i++) {
        if (g_OsdCfg.VoutID == pSvcResCfg->DispStrm[i].VoutID) {
            g_OsdCfg.DispStrmIdx = i;
        }
    }

    if (0xFFU == g_OsdCfg.DispStrmIdx) {
        g_OsdCfg.Show = 0U;
        SvcLog_NG(SVC_LOG_STEREO_TASK, "Cannot find target VoutID, disable OSD", 0U, 0U);
    } else {
        g_OsdCfg.Show = 1U;

        /* Create Stereo OSD msg queue */
        {
            static char MsgQueName[] = "StereoOsdMsgQue";
            static STEREO_OSD_MSG_s OsdMsgQue[SVC_STEREO_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;

            RetVal = AmbaKAL_MsgQueueCreate(&(g_OsdCfg.MsgQueId), MsgQueName, sizeof(STEREO_OSD_MSG_s),
                                            &OsdMsgQue[0], SVC_STEREO_QUEUE_SIZE * sizeof(STEREO_OSD_MSG_s));
            if (KAL_ERR_NONE != RetVal) {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoOsdMsgQue created failed with err 0x%X!", RetVal, 0U);
                RetVal = SVC_NG;
            }
        }

        /* Create stereo OSD buffer mutex */
        if (SVC_OK == RetVal) {
            RetVal = AmbaKAL_MutexCreate(&(g_OsdCfg.OsdMutex), NULL);
            if (KAL_ERR_NONE != RetVal) {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "OsdMutex created failed with err 0x%X!", RetVal, 0U);
                RetVal = SVC_NG;
            }
        }

        /* Create OSD task  */
        if (SVC_OK == RetVal) {
            static SVC_TASK_CTRL_s OSDTaskCtrl GNU_SECTION_NOZEROINIT;
            static UINT8 OSDTaskStack[SVC_STEREO_OSD_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

            OSDTaskCtrl.Priority   = 191;
            OSDTaskCtrl.EntryFunc  = StereoTask_OsdTaskEntry;
            OSDTaskCtrl.pStackBase = OSDTaskStack;
            OSDTaskCtrl.StackSize  = SVC_STEREO_OSD_TASK_STACK_SIZE;
            OSDTaskCtrl.CpuBits    = 0x03U;

            RetVal = SvcTask_Create("StereoOSDTask", &OSDTaskCtrl);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoOSDTask created failed with err 0x%X!", RetVal, 0U);
            }
        }

        /* Setup OSD buffer address */
        if (SVC_OK == RetVal) {
            RetVal = StereoTask_GetOsdBufSize(&OsdSize);
            if (SVC_OK == RetVal) {
                for (i = 0U; i < SVC_STEREO_MAX_OSD_NUM; i++) {
                    g_OsdCfg.BufAddr[i] = BaseAddr;
                    BaseAddr += OsdSize;
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_GetOsdBufSize failed", 0U, 0U);
            }
        }

#if SVC_STEREO_ENABLE_EXT_OSD_PATH
        /* Setup color lookup table */
        if (SVC_OK == RetVal) {
            const UINT32 *pOsdClut = NULL;
            AMBA_STU_OSD_PALETTE_ORDER_s ColorPaletteOrder;
            static AMBA_STU_8BITS_COLOR_PALETTE_s *pColorPalette;

            AmbaMisra_TypeCast(&pOsdClut, &BaseAddr);
            AmbaMisra_TypeCast(&pColorPalette, &pOsdClut);

            ColorPaletteOrder.Color[0] = AMBA_STU_OSD_BLUE;
            ColorPaletteOrder.Color[1] = AMBA_STU_OSD_GREEN;
            ColorPaletteOrder.Color[2] = AMBA_STU_OSD_RED;
            ColorPaletteOrder.Color[3] = AMBA_STU_OSD_ALPHA;
            RetVal = AmbaSTU_GetDefaultColorPalette(&ColorPaletteOrder, pColorPalette);

            if ((SVC_OK == RetVal) && (NULL != pOsdClut)) {
                /* Set to transparent for Index 0 and 255(if HW cannot detect the disparity) */
                pColorPalette->ColorLUT[(4 * 1) - 1]   = 0U;
                pColorPalette->ColorLUT[(4 * 256) - 1] = 0U;

                RetVal = SvcOsd_SetExtCLUT(g_OsdCfg.VoutID, pOsdClut);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcOsd_SetExtCLUT failed", 0U, 0U);
                }
            }
        }
#else
        if (SVC_OK == RetVal) {
            UINT32 OsdWidth, OsdHeight;
            SVC_GUI_CANVAS_s GuiCanvas;

            RetVal = SvcOsd_GetOsdBufSize(g_OsdCfg.VoutID, &OsdWidth, &OsdHeight);
            if ((OsdWidth == SVC_STEREO_MAX_OSD_BUF_WIDTH) && (OsdHeight == SVC_STEREO_MAX_OSD_BUF_HEIGHT)) {
                GuiCanvas.StartX = 0;
                GuiCanvas.StartY = 0;
                GuiCanvas.Width  = OsdWidth;
                GuiCanvas.Height = OsdHeight;
                SvcGui_Register(g_OsdCfg.VoutID, SVC_STEREO_OSD_LEVEL, "Sto", StereoTask_OsdDraw, StereoTask_OsdUpdate);
                AmbaPrint_PrintUInt5("StereoTask: Register GUI Canvas: Level %u X %u Y %u W %u H %u",
                    SVC_STEREO_OSD_LEVEL,
                    GuiCanvas.StartX,
                    GuiCanvas.StartY,
                    GuiCanvas.Width,
                    GuiCanvas.Height);
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "OSD Buffer is not FHD", 0U, 0U);
                RetVal = SVC_NG;
            }
        }
#endif
    }

    return RetVal;
}


static UINT32 StereoTask_PostInitOsd(STEREO_OSD_MSG_s *pOsdMsg)
{
    UINT32 RetVal;
    UINT32 i, Found = 0, ScaleID = 0;
    UINT32 OsdWidth, OsdHeight;
    const AMBA_CV_SPU_DATA_s *pSpuData = pOsdMsg->pOutSpu;

    AmbaMisra_TouchUnused(pOsdMsg);

    for (i = 0; (i < MAX_HALF_OCTAVES) && (Found == 0U); i++) {
        if (pSpuData->Scales[i].Status == 0) {
            Found = 1U;
            ScaleID = i;
        }
    }

    RetVal = SvcOsd_GetOsdBufSize(g_OsdCfg.VoutID, &OsdWidth, &OsdHeight);
    if ((RetVal == SVC_OK) && ((Found == 1U))) {
        g_OsdCfg.ScaleID    = ScaleID;
        g_OsdCfg.DsiWidth   = pSpuData->Scales[ScaleID].DisparityWidth;
        g_OsdCfg.DsiHeight  = pSpuData->Scales[ScaleID].DisparityHeight;
        g_OsdCfg.DsiPitch   = pSpuData->Scales[ScaleID].DisparityWidth;
        g_OsdCfg.OsdOffsetX = ((OsdWidth - g_OsdCfg.DsiWidth) >> 1U);
        g_OsdCfg.OsdOffsetY = ((OsdHeight - g_OsdCfg.DsiHeight) >> 1U);
#if SVC_STEREO_ENABLE_EXT_OSD_PATH
        {
            SvcOSD_ExtBuffer_CFG_s BufConfig;
            const SVC_RES_CFG_s *pSvcResCfg = SvcResCfg_Get();
            const SVC_STRM_CFG_s *pDispStrm = &pSvcResCfg->DispStrm[g_OsdCfg.DispStrmIdx].StrmCfg;

            BufConfig.PixelFormat   = OSD_8BIT_CLUT_MODE;
            BufConfig.BufferPitch   = g_OsdCfg.DsiPitch;
            BufConfig.BufferWidth   = g_OsdCfg.DsiWidth;
            BufConfig.BufferHeight  = g_OsdCfg.DsiHeight;
            BufConfig.BufferSize    = BufConfig.BufferPitch * BufConfig.BufferHeight;
            BufConfig.WindowOffsetX = ((pDispStrm->MaxWin.Width - pDispStrm->Win.Width) >> 1U);
            BufConfig.WindowOffsetY = ((pDispStrm->MaxWin.Height - pDispStrm->Win.Height) >> 1U);
            BufConfig.WindowWidth   = pDispStrm->Win.Width;
            BufConfig.WindowHeight  = pDispStrm->Win.Height;
            BufConfig.Interlace     = 0U;
            RetVal = SvcOsd_SetExtBufInfo(g_OsdCfg.VoutID, &BufConfig);
        }
#endif
        g_OsdCfg.PostInit = 1U;
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_PostInitOsd failed", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}


static void StereoTask_OsdHandler(STEREO_OSD_MSG_s *pOsdMsg)
{
    UINT32 RetVal = SVC_OK;
    const AMBA_CV_SPU_DATA_s *pSpuData;
    UINT32 ScaleID;
    UINT32 ImgW, ImgH, DisparityBpp;
    ULONG ULONGSpu;
    const UINT8 *pIn10BitsTbl;
    UINT16 *pOut16BitsTbl = NULL;
    static UINT16 Out16BitsTbl[1920*1080] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static UINT8 PgmTbl[SVC_STEREO_PGM_OUTPUT_DEPTH][1920*1080] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static UINT8  StereoPgmOutputDepth = 0U;
    UINT8 *pPgmTbl = NULL;
    UINT8 *pU8OutColorTbl = NULL;
    UINT32 OsdBufIdx = 0U, OsdBufIdxNext = 0U, Size = 0U, PGMSize = 0U, FrameNum = 0U;
    AMBA_STU_PGM_METADATA_S MetaData = {0};
    static STEREO_PGM_MSG_s PgmMsg;

    SVC_STEREO_TASK_DBG(SVC_LOG_STEREO_TASK, "OSD proc start", 0U, 0U);

    AmbaMisra_TouchUnused(pOsdMsg);
    AmbaMisra_TypeCast(&pSpuData, &pOsdMsg->pOutSpu);

    if (pSpuData != NULL) {
        if (g_OsdCfg.PostInit == 1U) {
            ScaleID = g_OsdCfg.ScaleID;
            ImgW = pSpuData->Scales[ScaleID].DisparityWidth;
            ImgH = pSpuData->Scales[ScaleID].DisparityHeight;
            DisparityBpp = pSpuData->Scales[ScaleID].DisparityBpp;

            AmbaMisra_TypeCast(&ULONGSpu, &pSpuData);

            ULONGSpu += (ULONG)pSpuData->Scales[ScaleID].DisparityMapOffset;

            /* 10 bits to 16 bits */
            if (DisparityBpp == 10U) {
                AmbaMisra_TypeCast(&pIn10BitsTbl, &ULONGSpu);
                pOut16BitsTbl = Out16BitsTbl;
                RetVal = AmbaSTU_Unpack10BitsTo16Bits(pIn10BitsTbl, ImgW * ImgH, pOut16BitsTbl);
            } else if (DisparityBpp == 16U) {
                AmbaMisra_TypeCast(&pOut16BitsTbl, &ULONGSpu);
            } else {
                RetVal = SVC_NG;
            }

            if (StereoDumpPgm > 0U) {
                pPgmTbl = &PgmTbl[StereoPgmOutputDepth][0];
                MetaData.IntegerBitNum = 7U;
                MetaData.DecimalBitNum = 3U;

                MetaData.Width = pSpuData->Scales[ScaleID].DisparityWidth;
                MetaData.Height = pSpuData->Scales[ScaleID].DisparityHeight;
                MetaData.ProjectionModel = AMBA_CAL_EM_MODEL_PINHOLE;
                MetaData.Scale = g_OsdCfg.ScaleID;

                Size = MetaData.Width * MetaData.Height * 2U;

                if (NULL == pOut16BitsTbl) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "pOut16BitsTbl or pPgmTbl is NULL, fail to save PGM", 0U, 0U);
                } else {
                    RetVal = AmbaSTU_DsiToPGM(&MetaData, pOut16BitsTbl, Size, pPgmTbl, &PGMSize);
                    if (STU_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_STEREO_TASK, "AmbaSTU_DsiToPGM failed with err 0x%x!", RetVal, 0U);
                        RetVal = SVC_NG;
                    }
                }

                AmbaMisra_TypeCast(&PgmMsg.pPgmTbl, &pPgmTbl);
                PgmMsg.PGMSize = PGMSize;
                PgmMsg.FrameNum = pOsdMsg->FrameNum;
                AmbaMisra_TypeCast(&PgmMsg.pPicInfo, &pOsdMsg->pPicInfo);

                RetVal = AmbaKAL_MsgQueueSend(&PgmQueId, &PgmMsg, 5000);
                if (SVC_OK != RetVal) {
                    //SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_OsdHandler: PgmMsgQueueSend error", 0U, 0U);
                } else {
                    StereoDumpPgm--;
                }

                StereoPgmOutputDepth++;
                if (StereoPgmOutputDepth == SVC_STEREO_PGM_OUTPUT_DEPTH) {
                    StereoPgmOutputDepth = 0U;
                }
            }

            /* 16 bits to 8 bits color index */
            if (RetVal == STU_OK) {
                AMBA_STU_POINT_INT_2D_s FeedStartPos;
                AMBA_STU_SIZE_s ColorTblSize;
                AMBA_STU_DSI_INFO_s FusedDsiInfo;
                FeedStartPos.X = 0;
                FeedStartPos.Y = 0;
                ColorTblSize.Width = ImgW;
                ColorTblSize.Height = ImgH;
                FusedDsiInfo.Size.Width = ImgW;
                FusedDsiInfo.Size.Height = ImgH;
                FusedDsiInfo.Shift = pSpuData->Scales[ScaleID].DisparityQf;

                AmbaMisra_TypeCast(&FusedDsiInfo.pDisparityTbl, &pOut16BitsTbl);

                /* unpack 16 to color index (clut) */
                RetVal |= StereoTask_GetOsdBufIdx(&OsdBufIdx, &OsdBufIdxNext, &FrameNum);
                AmbaMisra_TouchUnused(&FrameNum);

                AmbaMisra_TypeCast(&pU8OutColorTbl, &(g_OsdCfg.BufAddr[OsdBufIdxNext]));

                RetVal |= AmbaSTU_ConvDsi16BitsToColorIdx(&FusedDsiInfo, NULL, &FeedStartPos, &ColorTblSize, pU8OutColorTbl);

                RetVal |= StereoTask_UpdateOsdBufIdx(OsdBufIdxNext, pOsdMsg->FrameNum);

                if (SVC_OK == RetVal) {
#if SVC_STEREO_ENABLE_EXT_OSD_PATH
                    RetVal = SvcOsd_UpdateExtBuf(g_OsdCfg.VoutID, pU8OutColorTbl);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_STEREO_TASK, "SvcOsd_UpdateExtBuf failed", 0U, 0U);
                    }
#endif
                } else {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_UpdateOsdBufIdx failed", 0U, 0U);
                }
            } else {
                AmbaPrint_PrintStr5("AmbaSTU_Unpack10BitsTo16Bits failed", NULL, NULL, NULL, NULL, NULL);
            }


        } else {
            SvcLog_DBG(SVC_LOG_STEREO_TASK, "StereoTask_OsdHandler PostInit(0)", 0U, 0U);
        }
    }

    SVC_STEREO_TASK_DBG(SVC_LOG_STEREO_TASK, "OSD proc done", 0U, 0U);
}

static void* StereoTask_OsdTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK, i;
    STEREO_OSD_MSG_s OsdMsg;
    AMBA_KAL_MSG_QUEUE_INFO_s QueueInfo;

    AmbaMisra_TouchUnused(EntryArg);
    RetVal = AmbaWrap_memset(&OsdMsg, 0, sizeof(STEREO_OSD_MSG_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_STEREO_TASK, "memset OsdMsg failed", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_STEREO_TASK, "StereoTask_OsdTaskEntry run", 0U, 0U);

    while (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MsgQueueQuery(&g_OsdCfg.MsgQueId, &QueueInfo);
        if (KAL_ERR_NONE == RetVal) {
            if (0U == QueueInfo.NumEnqueued) {
                RetVal = AmbaKAL_MsgQueueReceive(&g_OsdCfg.MsgQueId, &OsdMsg, AMBA_KAL_WAIT_FOREVER);
            } else {
                for (i = 0; i < QueueInfo.NumEnqueued; i++) {
                    RetVal = AmbaKAL_MsgQueueReceive(&g_OsdCfg.MsgQueId, &OsdMsg, AMBA_KAL_NO_WAIT);
                }
            }
        }
        if (KAL_ERR_NONE == RetVal) {
            StereoTask_OsdHandler(&OsdMsg);
        } else {
            SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoOsdQue wait msg queue failed with err 0x%X!", RetVal, 0U);
            RetVal = SVC_NG;
        }
    }

    return NULL;
}

#if (SVC_STEREO_ENABLE_EXT_OSD_PATH == 0U)
static void StereoTask_OsdDraw(UINT32 VoutIdx, UINT32 Level)
{
    #define EXCLUDE_V_REGION    60U
    UINT32 RetVal;
    UINT32 OsdBufIdx;
    const void *pBufAddr;
    UINT32 FrameNum;
#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
    UINT64 FrameSync;
#endif

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if((g_OsdCfg.Show == 1U) && (g_OsdCfg.PostInit == 1U) && (g_osd_enable == SVC_STEREO_OSD_ENABLE)) {
        if ((HaveStereoData & STEREODRAW_GUI_SHOW) > 0U) {
            RetVal = StereoTask_GetOsdBufIdx(&OsdBufIdx, NULL, &FrameNum);
            if (SVC_OK == RetVal) {
                AmbaMisra_TypeCast(&pBufAddr, &g_OsdCfg.BufAddr[OsdBufIdx]);

#if defined(CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL)
                FrameSync = (UINT64)FrameNum;
                if (SVC_OK != SvcVoutFrmCtrlTask_Ctrl(SVC_VFC_TASK_CMD_CAP_SEQ, &FrameSync)) {
                    SvcLog_DBG(SVC_LOG_STEREO_TASK, "StereoTask_OsdDraw sync failed %d", 0U, 0U);
                }
                AmbaMisra_TouchUnused(&FrameSync);
#endif
                RetVal = SvcOsd_DrawBin(g_OsdCfg.VoutID,
                                        g_OsdCfg.OsdOffsetX,
                                        g_OsdCfg.OsdOffsetY,
                                        (g_OsdCfg.OsdOffsetX + g_OsdCfg.DsiWidth),
                                        (g_OsdCfg.OsdOffsetY + g_OsdCfg.DsiHeight - EXCLUDE_V_REGION),
                                        pBufAddr);

                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_OsdDraw DrawBin err", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_STEREO_TASK, "StereoTask_OsdDraw GetOsdBuf err", 0U, 0U);
            }
        }
    }
}

static void StereoTask_OsdUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((HaveStereoData & STEREODRAW_GUI_UPDATE) > 0U) {
        HaveStereoData &= ~(STEREODRAW_GUI_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}
#endif


