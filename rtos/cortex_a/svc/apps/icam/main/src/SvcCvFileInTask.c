/**
 *  @file SvcCvFileInTask.c
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
 *  @details svc cv file input
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaFS.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaSvcWrap.h"
#include "AmbaPrint.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcWrap.h"
#include "SvcPlat.h"

/* app-shared */
#include "SvcResCfg.h"
#include "SvcCvAlgo.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"
#include "SvcCvFlowInputUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_AmbaOD.h"
#include "SvcCvFlow_RefOD.h"
#include "SvcTask.h"

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

/* svc-app */
#include "SvcCvAppDef.h"
#include "SvcCvFileInTask.h"
#include "SvcBufMap.h"

#define SVC_LOG_CV_FILE_IN_TASK     "FinTask"

#define DATA_FMT_YUV420                         (0U)
#define DATA_FMT_Y                              (1U)
#define DATA_FMT_UV420                          (2U)
#define DATA_FMT_RGB                            (3U)

#define CV_FLOW_DONE                            (0x1U)
#define BATCH_TASK_ENABLE                       (0x2U)

typedef struct {
    ULONG                        YAddr;
    ULONG                        UVAddr;
} SVC_CV_FILE_IN_YUV_BUF_s;

typedef struct {
    SVC_CV_FILE_IN_YUV_BUF_s     YuvBuf;
    ULONG                        RgbAddr;
} SVC_CV_FILE_IN_DATA_BUF_s;

typedef struct {
    UINT32                       InputIdx;
    UINT8                        NumData;
    SVC_CV_FILE_IN_DATA_INFO_s   DataInfo[SVC_CV_FILE_IN_MAX_DATA_PER_INPUT];
    UINT8                        FileValid[SVC_CV_FILE_IN_MAX_DATA_PER_INPUT];
    char                         FileName[SVC_CV_FILE_IN_MAX_DATA_PER_INPUT][SVC_CV_FILE_IN_MAX_FILE_PATH];
} SVC_CV_FILE_IN_INPUT_CFG_s;

typedef struct {
    SVC_CV_FILE_IN_CONFIG_s      Config;
    SVC_CV_FILE_IN_INPUT_CFG_s   InputCfg[SVC_CV_FILE_IN_MAX_INPUT_PER_CHAN];

    /* Buffer pool */
    ULONG                        BufBase;
    UINT32                       BufSize;
    ULONG                        BufCurAddr;

    /* Buffer info for specific data */
    SVC_CV_FILE_IN_DATA_BUF_s    DataBuf[SVC_CV_FILE_IN_MAX_INPUT_PER_CHAN][SVC_CV_FILE_IN_MAX_DATA_PER_INPUT];
} SVC_CV_FILE_IN_CTRL_s;

typedef struct {
    UINT32 Enable;
    UINT32 MaxNumber;
    AMBA_FS_DIR *pDrInfo;
    AMBA_FS_DIR *pSubDrInfo;
    char InputPathPrefix[SVC_CV_FILE_IN_MAX_FILE_PATH];
    char OutputPathPrefix[SVC_CV_FILE_IN_MAX_FILE_PATH];
    char OutputLayerName[16U][32U];
} SVC_CV_FILE_IN_MULTIPLE_s;

typedef struct {
    ULONG  YBufAddr;
    ULONG  UVBufAddr;
    UINT32 Pitch;
    UINT32 Height;
    UINT32 StartX;
    UINT32 StartY;
    UINT32 RectWidth;
    UINT32 RectHeight;
    UINT32 Thickness;
    UINT16 Y;
    UINT16 U;
    UINT16 V;
} SVC_CV_FILE_IN_DRAW_RECT_s;

static SVC_CV_FILE_IN_CTRL_s g_FileInCtrl GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t g_FileInEventFlag GNU_SECTION_NOZEROINIT;
static SVC_CV_FILE_IN_MULTIPLE_s g_MultiInfo GNU_SECTION_NOZEROINIT;

static UINT32 FileInTask_StartSingle(const SVC_CV_FILE_IN_START_s *pStartCfg);
static UINT32 FileInTask_StartMultiple(const SVC_CV_FILE_IN_START_s *pStartCfg);
static UINT32 FileInTask_BatchFileInfo(SVC_CV_FILE_IN_NAMING_CONFIG_s *pNameCfg);
static UINT32 FileInTask_ConfigOutput(const SVC_CV_FILE_IN_NAMING_CONFIG_s *pNameCfg);

/* Memory usage
#define SVC_CV_FILE_IN_MAX_WIDTH              (1920U)
#define SVC_CV_FILE_IN_MAX_HEIGHT             (1080U)
#define SVC_CV_FILE_IN_MAX_INPUT_CHAN         (1U)

#define SVC_CV_FILE_IN_MAX_WIDTH              (3840U)
#define SVC_CV_FILE_IN_MAX_HEIGHT             (((3840U * 2160U) + (1920U * 1080U) + (1280U * 720U)) /  SVC_CV_FILE_IN_MAX_WIDTH)
#define SVC_CV_FILE_IN_MAX_INPUT_CHAN         (2U)

#define SVC_CV_FILE_IN_MAX_PITCH              ((SVC_CV_FILE_IN_MAX_WIDTH + 64U) & ~(64U - 1U))
#define SVC_CV_FILE_IN_MAX_BUF_SIZE_PER_CHAN  ((SVC_CV_FILE_IN_MAX_PITCH * SVC_CV_FILE_IN_MAX_HEIGHT * 3U) >> 1U)
#define SVC_CV_FILE_IN_BUF_SIZE               (SVC_CV_FILE_IN_MAX_INPUT_CHAN * SVC_CV_FILE_IN_MAX_BUF_SIZE_PER_CHAN)
*/
static void FileInTask_InitBuf(void)
{
    UINT32 RetVal;

    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CV_FILE_IN, &g_FileInCtrl.BufBase, &g_FileInCtrl.BufSize);
    if (SVC_OK == RetVal) {
        g_FileInCtrl.BufCurAddr = g_FileInCtrl.BufBase;
        SvcWrap_PrintUL("Init Buf: 0x%x, size %u", g_FileInCtrl.BufBase, (ULONG)g_FileInCtrl.BufSize, 0U, 0U, 0U);
    } else {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "fail to SvcBuffer_Request", 0U, 0U);
    }

}

static inline ULONG GetAlignedValULONG   (ULONG Val, ULONG AlignBase) {return (((Val) + (AlignBase - 1U)) & ~(AlignBase - 1U));}
static UINT32 FileInTask_AllocBuf(UINT32 ReqSize, ULONG *pBufAddr)
{
    #define BUF_ALIGN_VALUE  128
    UINT32 RetVal = SVC_OK;
    ULONG  BufLimit = g_FileInCtrl.BufBase + g_FileInCtrl.BufSize - 1U;
    ULONG  BufAddrAlign;
    ULONG  ReqSizeAlign;

    BufAddrAlign = GetAlignedValULONG(g_FileInCtrl.BufCurAddr, BUF_ALIGN_VALUE);
    ReqSizeAlign = GetAlignedValULONG(ReqSize, BUF_ALIGN_VALUE);

    if ((BufAddrAlign + ReqSizeAlign) <= BufLimit) {
        *pBufAddr = BufAddrAlign;
        g_FileInCtrl.BufCurAddr += ReqSizeAlign;
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}
static inline UINT32 FileInTask_CacheAligned (UINT32 Val) {return (UINT32)(((Val) + ((UINT32)AMBA_CACHE_LINE_SIZE - 1U)) & ~((UINT32)AMBA_CACHE_LINE_SIZE - 1U));}

static UINT32 FileInTask_ReadFile(const char *pFileName, void *pBuf, UINT32 Size)
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

static UINT32 FileInTask_FileWrite(const char *pFileName, void *pBuf, UINT32 Size)
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

static UINT32 GetBufferSize(UINT32 DataFmt, UINT32 Pitch, UINT32 Height)
{
    UINT32 Size = Pitch * Height;

    if (DataFmt == DATA_FMT_Y) {
        ;
    } else if (DataFmt == DATA_FMT_UV420) {
        Size = Size >> 1U;
    } else if (DataFmt == DATA_FMT_YUV420) {
        Size = (Size * 3U) >> 1U;
    } else if (DataFmt == DATA_FMT_RGB) {
        Size = (Size * 3U);
    } else {
        /* Pitch * Height */
    }

    return Size;
}

static UINT32 FileInTask_LoadYuvFile(UINT32 InputIdx)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, DataSize;
    const SVC_CV_FILE_IN_INPUT_CFG_s *pInputCfg = &g_FileInCtrl.InputCfg[InputIdx];
    char NewFileName[64];
    char *pStr;
    AMBA_FS_FILE_INFO_s FileInfo;
    void *pAddr;

    for (i = 0; i < pInputCfg->NumData; i++) {
        SvcWrap_strcpy(NewFileName, sizeof(NewFileName), &pInputCfg->FileName[i][0]);
        pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
        if (pStr != NULL) {
            flexidag_memblk_t MemBlkY, MemBlkUV;
            /* Read Y file */
            DataSize = GetBufferSize(DATA_FMT_Y, pInputCfg->DataInfo[i].Img.Pitch, pInputCfg->DataInfo[i].Img.Height);
            RetVal = FileInTask_AllocBuf(FileInTask_CacheAligned(DataSize), &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.YAddr);
            if (RetVal == SVC_OK) {
                AmbaMisra_TypeCast(&pAddr, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.YAddr);
                RetVal = FileInTask_ReadFile(NewFileName, pAddr, DataSize);
                if (RetVal == 0U) {
                    AmbaMisra_TypeCast(&MemBlkY.pBuffer, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.YAddr);
                    (void)SvcMem_VirtToPhys(g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.YAddr, &MemBlkY.buffer_daddr);
                    MemBlkY.buffer_size = DataSize;
                    RetVal = AmbaCV_UtilityCmaMemClean(&MemBlkY);
                    if(RetVal != 0U) {
                        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Clean Cache error(%d)", RetVal, 0U);
                    }
                    AmbaPrint_PrintStr5("[FinTask] Read %s done", NewFileName, NULL, NULL, NULL, NULL);
                } else {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "FileInTask_ReadFile error(%d)", RetVal, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Alloc Y buf error", 0U, 0U);
            }
            if (RetVal == SVC_OK) {
                /* Read UV file */
                pStr[1] = 'u';
                pStr[2] = 'v';
                pStr[3] = '\0';
                if (AMBA_FS_ERR_NONE == AmbaFS_GetFileInfo(NewFileName, &FileInfo)) {
                    /* UV file exist */
                    DataSize = GetBufferSize(DATA_FMT_UV420, pInputCfg->DataInfo[i].Img.Pitch, pInputCfg->DataInfo[i].Img.Height);
                    RetVal = FileInTask_AllocBuf(DataSize, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.UVAddr);

                    if (RetVal == SVC_OK) {
                        AmbaMisra_TypeCast(&pAddr, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.UVAddr);
                        RetVal = FileInTask_ReadFile(NewFileName, pAddr, DataSize);
                        if (RetVal == SVC_OK) {
                            AmbaMisra_TypeCast(&MemBlkUV.pBuffer, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.UVAddr);
                            (void)SvcMem_VirtToPhys(g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.UVAddr, &MemBlkUV.buffer_daddr);
                            MemBlkUV.buffer_size = DataSize;
                            RetVal = AmbaCV_UtilityCmaMemClean(&MemBlkUV);
                            if(RetVal != 0U) {
                                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Clean Cache error(%d)", RetVal, 0U);
                            }
                            AmbaPrint_PrintStr5("[FinTask] Read %s done", NewFileName, NULL, NULL, NULL, NULL);
                        } else {
                            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "FileInTask_ReadFile error(%d)", RetVal, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Alloc UV buf error", 0U, 0U);
                    }
                } else {
                    AmbaPrint_PrintUInt5("UV file not exist", 0U, 0U, 0U, 0U, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "LoadYuvFile: Invalid file name", 0U, 0U);
        }

        if (RetVal != SVC_OK) {
            break;
        }
    }

    return RetVal;
}

static void FileInTask_DrawLine_YUV420(ULONG YBufAddr, ULONG UVBufAddr, UINT32 Pitch, UINT32 Height,
                                       UINT32 OffsetX1, UINT32 OffsetY1,
                                       UINT32 LineWidth, UINT32 LineHeight,
                                       UINT16 Y, UINT16 U, UINT16 V)
{
    UINT32 LineW = LineWidth;
    UINT32 LineH = LineHeight;
    UINT16 *pYAddr, *pUVAddr;
    ULONG  YAddr, UVAddr;
    UINT32 i, j;
    UINT16 YValue = (Y << 8U) | (Y);
    UINT16 UVValue = (V << 8U) | (U);
    UINT32 LineWPtr16;
    UINT32 OffsetX2 = OffsetX1;
    UINT32 OffsetY2 = OffsetY1;

    OffsetX2 &= 0xFFFFFFFEU;
    OffsetY2 &= 0xFFFFFFFEU;
    LineW   &= 0xFFFFFFFEU;
    LineH   &= 0xFFFFFFFEU;

    if ((OffsetX2 + LineW) >= Pitch) {
        LineW = Pitch - OffsetX2 - 1U;
    }
    if ((OffsetY2 + LineH) >= Height) {
        LineH = Height - OffsetY2 - 1U;
    }

    YAddr = YBufAddr + ((ULONG)OffsetY2 * (ULONG)Pitch) + (ULONG)OffsetX2;
    AmbaMisra_TypeCast(&pYAddr, &YAddr);
    UVAddr = UVBufAddr + (((ULONG)OffsetY2 >> 1U) * (ULONG)Pitch) + (ULONG)OffsetX2;    /* shift offset by 1 for YUV420 */
    AmbaMisra_TypeCast(&pUVAddr, &UVAddr);
    LineWPtr16 = (LineW >> 1U);

    for (j = 0; j < LineH; j++) {
        for (i = 0; i < LineWPtr16; i++) {
            pYAddr[i] = YValue;
        }
        YAddr += Pitch;
        AmbaMisra_TypeCast(&pYAddr, &YAddr);
    }

    LineH = (LineH >> 1U);    /* shift height by 1 for YUV420 */
    for (j = 0; j < LineH; j++) {
        for (i = 0; i < LineWPtr16; i++) {
            pUVAddr[i] = UVValue;
        }
        UVAddr += Pitch;
        AmbaMisra_TypeCast(&pUVAddr, &UVAddr);
    }
}

static void FileInTask_DrawRect(const SVC_CV_FILE_IN_DRAW_RECT_s *pRectInfo)
{
    ULONG  YBufAddr = pRectInfo->YBufAddr;
    ULONG  UVBufAddr = pRectInfo->UVBufAddr;
    UINT32 Pitch = pRectInfo->Pitch;
    UINT32 Height = pRectInfo->Height;
    UINT32 StartX = pRectInfo->StartX;
    UINT32 StartY = pRectInfo->StartY;
    UINT32 RectWidth = pRectInfo->RectWidth;
    UINT32 RectHeight = pRectInfo->RectHeight;
    UINT32 Thickness = pRectInfo->Thickness;
    UINT16 Y = pRectInfo->Y;
    UINT16 U = pRectInfo->U;
    UINT16 V = pRectInfo->V;

    StartX     &= 0xFFFFFFFEU;
    StartY     &= 0xFFFFFFFEU;
    RectWidth  &= 0xFFFFFFFEU;
    RectHeight &= 0xFFFFFFFEU;
    Thickness  &= 0xFFFFFFFEU;

    /* Top line */
    FileInTask_DrawLine_YUV420(YBufAddr, UVBufAddr, Pitch, Height,
                               StartX, StartY, RectWidth, Thickness,
                               Y, U, V);
    /* Left line */
    FileInTask_DrawLine_YUV420(YBufAddr, UVBufAddr, Pitch, Height,
                               StartX, StartY, Thickness, RectHeight,
                               Y, U, V);
    /* Right line */
    FileInTask_DrawLine_YUV420(YBufAddr, UVBufAddr, Pitch, Height,
                               (StartX + RectWidth - Thickness), StartY, Thickness, RectHeight,
                               Y, U, V);
    /* Bottom line */
    FileInTask_DrawLine_YUV420(YBufAddr, UVBufAddr, Pitch, Height,
                               StartX, (StartY + RectHeight - Thickness), RectWidth, Thickness,
                               Y, U, V);
}

static void FileInTask_DrawBBX(const SVC_CV_FILE_IN_CTRL_s *pCtrl, const SVC_CV_DETRES_BBX_LIST_s *pList)
{
    UINT32 i;
    SVC_CV_FILE_IN_DRAW_RECT_s RectInfo;
    UINT16 Cat;
    UINT16 Color;

    SvcLog_OK(SVC_LOG_CV_FILE_IN_TASK, "Detect Result: Amount = %d", pList->BbxAmount, 0U);

    RectInfo.YBufAddr   = pCtrl->DataBuf[0][0].YuvBuf.YAddr;
    RectInfo.UVBufAddr  = pCtrl->DataBuf[0][0].YuvBuf.UVAddr;
    RectInfo.Pitch      = pCtrl->InputCfg[0].DataInfo[0].Img.Pitch;
    RectInfo.Height     = pCtrl->InputCfg[0].DataInfo[0].Img.Height;

    for (i = 0; i < pList->BbxAmount; i++) {
        RectInfo.StartX     = pList->Bbx[i].X;
        RectInfo.StartY     = pList->Bbx[i].Y;
        RectInfo.RectWidth  = pList->Bbx[i].W;
        RectInfo.RectHeight = pList->Bbx[i].H;
        RectInfo.Thickness  = 4;
        Cat = pList->Bbx[i].Cat;
        if (Cat == 255U) {
            RectInfo.Y = 255;
            RectInfo.U = 128;
            RectInfo.V = 128;
        } else {
            RectInfo.Y = 128U;
            Color = Cat * 96U;
            RectInfo.U = Color;
            Color = Cat * 80U;
            RectInfo.V = Color;
        }
        FileInTask_DrawRect(&RectInfo);
        AmbaPrint_PrintUInt5("BBX: C %u X %u Y %u W %u H %u",
                              pList->Bbx[i].Cat,
                              pList->Bbx[i].X,
                              pList->Bbx[i].Y,
                              pList->Bbx[i].W,
                              pList->Bbx[i].H);
    }
}

static UINT32 FileInTask_BbxOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    char NewFileName[128];
    char *pStr;
    void *pAddr;
    const SVC_CV_DETRES_BBX_LIST_s *pBbxList;
    UINT32 Pitch, Height;
    AMBA_FS_DIR *pDir;
    const char *ArgS[3U];

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    (void) pOutput;

    AmbaMisra_TypeCast(&pBbxList, &pOutput);
    FileInTask_DrawBBX(&g_FileInCtrl, pBbxList);

    if (g_MultiInfo.Enable == 0U) {
        SvcWrap_strcpy(NewFileName, sizeof(NewFileName), &g_FileInCtrl.InputCfg[0].FileName[0][0]);
    } else {
        /* Create Folder for each element */
        ArgS[0] = g_MultiInfo.OutputPathPrefix;
        ArgS[1] = g_FileInCtrl.InputCfg[0].FileName[0]; //Folder
        if (SvcWrap_sprintfStr(NewFileName, (UINT32)sizeof(NewFileName),"%s\\%s",2,ArgS) >= sizeof(NewFileName)) {
            RetVal = SVC_NG;
        }

        if (AmbaFS_OpenDir(NewFileName, &pDir) != 0U) {
            if (AmbaFS_MakeDir(NewFileName) != 0U) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "MakeDir failed", 0U, 0U);
            }
            //AmbaPrint_PrintStr5("Create Folder : %s", NewFileName, NULL, NULL, NULL, NULL);
        } else {
            if (AmbaFS_CloseDir(pDir) != 0U) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "CloseDir failed", 0U, 0U);
            }
        }

        /* Output Y file path */
        ArgS[2] = g_FileInCtrl.InputCfg[0].FileName[1];
        if (SvcWrap_sprintfStr(NewFileName, (UINT32)sizeof(NewFileName),"%s\\%s\\%s", 3, ArgS) >= sizeof(NewFileName)) {
            RetVal = SVC_NG;
        }
    }

    pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
    if (pStr != NULL) {
        pStr[0] = '_';
        pStr[1] = 'o';
        pStr[2] = 'u';
        pStr[3] = 't';
        pStr[4] = '.';
        pStr[5] = 'y';
        pStr[6] = '\0';
        AmbaMisra_TypeCast(&pAddr, &g_FileInCtrl.DataBuf[0][0].YuvBuf.YAddr);
        Pitch    = g_FileInCtrl.InputCfg[0].DataInfo[0].Img.Pitch;
        Height   = g_FileInCtrl.InputCfg[0].DataInfo[0].Img.Height;
        DataSize = GetBufferSize(DATA_FMT_Y, Pitch, Height);
        RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
        if (RetVal == SVC_OK) {
            AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
        }

        if (g_MultiInfo.Enable != 0U) {
            /* Output UV file path*/
            ArgS[2] = g_FileInCtrl.InputCfg[0].FileName[2];
            if (SvcWrap_sprintfStr(NewFileName, (UINT32)sizeof(NewFileName),"%s\\%s\\%s",3,ArgS) >= sizeof(NewFileName)) {
                RetVal = SVC_NG;
            }
            pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
        }
        if (pStr != NULL) {
            pStr[0] = '_';
            pStr[1] = 'o';
            pStr[2] = 'u';
            pStr[3] = 't';
            pStr[4] = '.';
            pStr[5] = 'u';
            pStr[6] = 'v';
            pStr[7] = '\0';
            AmbaMisra_TypeCast(&pAddr, &g_FileInCtrl.DataBuf[0][0].YuvBuf.UVAddr);
            DataSize = GetBufferSize(DATA_FMT_UV420, Pitch, Height);

            if (RetVal == SVC_OK) {
                RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
                if (RetVal == SVC_OK) {
                    AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "SvcWrap_strrchr return NULL", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 FileInTask_PcptOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    char NewFileName[128];
    char *pStr;
    void *pAddr;
    const SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut;
    const SVC_CV_DETRES_BBX_LIST_s *pBbxList;
    UINT32 Pitch, Height;
    AMBA_FS_DIR *pDir;
    const char *ArgS[3U];

    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    (void) pOutput;

    AmbaMisra_TypeCast(&pPcptOut, &pOutput);
    pBbxList = pPcptOut->pBbxList;
    FileInTask_DrawBBX(&g_FileInCtrl, pBbxList);

    if (g_MultiInfo.Enable == 0U) {
        SvcWrap_strcpy(NewFileName, sizeof(NewFileName), &g_FileInCtrl.InputCfg[0].FileName[0][0]);
    } else {
        /* Create Folder for each element */
        ArgS[0] = g_MultiInfo.OutputPathPrefix;
        ArgS[1] = g_FileInCtrl.InputCfg[0].FileName[0]; //Folder
        if (SvcWrap_sprintfStr(NewFileName, (UINT32)sizeof(NewFileName),"%s\\%s",2,ArgS) >= sizeof(NewFileName)) {
            RetVal = SVC_NG;
        }

        if (AmbaFS_OpenDir(NewFileName, &pDir) != 0U) {
            if (AmbaFS_MakeDir(NewFileName) != 0U) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "MakeDir failed", 0U, 0U);
            }
            //AmbaPrint_PrintStr5("Create Folder : %s", NewFileName, NULL, NULL, NULL, NULL);
        } else {
            if (AmbaFS_CloseDir(pDir) != 0U) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "CloseDir failed", 0U, 0U);
            }
        }

        /* Output Y file path */
        ArgS[2] = g_FileInCtrl.InputCfg[0].FileName[1];
        if (SvcWrap_sprintfStr(NewFileName, (UINT32)sizeof(NewFileName),"%s\\%s\\%s", 3, ArgS) >= sizeof(NewFileName)) {
            RetVal = SVC_NG;
        }
    }

    pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
    if (pStr != NULL) {
        pStr[0] = '_';
        pStr[1] = 'o';
        pStr[2] = 'u';
        pStr[3] = 't';
        pStr[4] = '.';
        pStr[5] = 'y';
        pStr[6] = '\0';
        AmbaMisra_TypeCast(&pAddr, &g_FileInCtrl.DataBuf[0][0].YuvBuf.YAddr);
        Pitch    = g_FileInCtrl.InputCfg[0].DataInfo[0].Img.Pitch;
        Height   = g_FileInCtrl.InputCfg[0].DataInfo[0].Img.Height;
        DataSize = GetBufferSize(DATA_FMT_Y, Pitch, Height);
        RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
        if (RetVal == SVC_OK) {
            AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
        }

        if (g_MultiInfo.Enable != 0U) {
            /* Output UV file path*/
            ArgS[2] = g_FileInCtrl.InputCfg[0].FileName[2];
            if (SvcWrap_sprintfStr(NewFileName, (UINT32)sizeof(NewFileName),"%s\\%s\\%s",3,ArgS) >= sizeof(NewFileName)) {
                RetVal = SVC_NG;
            }
            pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
        }
        if (pStr != NULL) {
            pStr[0] = '_';
            pStr[1] = 'o';
            pStr[2] = 'u';
            pStr[3] = 't';
            pStr[4] = '.';
            pStr[5] = 'u';
            pStr[6] = 'v';
            pStr[7] = '\0';
            AmbaMisra_TypeCast(&pAddr, &g_FileInCtrl.DataBuf[0][0].YuvBuf.UVAddr);
            DataSize = GetBufferSize(DATA_FMT_UV420, Pitch, Height);

            if (RetVal == SVC_OK) {
                RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
                if (RetVal == SVC_OK) {
                    AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "SvcWrap_strrchr return NULL", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

#if defined(CONFIG_ICAM_CV_STEREO)
static UINT32 FileInTask_StereoSpuFexHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
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
    (void) pOutput;

    AmbaMisra_TypeCast(&pStereoOut, &pOutput);
    AmbaMisra_TypeCast(&pStereoData, &(pStereoOut->DataAddr));

#if 0
    {
        extern void SvcCvFlow_DumpSpuOutInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu);
        SvcCvFlow_DumpSpuOutInfo(SVC_LOG_CV_FILE_IN_TASK, pStereoData->pOutSpu);
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

        RetVal = FileInTask_FileWrite(NewFileName, pAddr, 17975936U);
        if (RetVal == SVC_OK) {
            AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "[FinTask] Write NG 1", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "pStr == NULL", 0U, 0U);
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

        RetVal = FileInTask_FileWrite(NewFileName, pAddr2, 2132992U);
        if (RetVal == SVC_OK) {
            AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "[FinTask] Write NG 2", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "pStr == NULL", 0U, 0U);
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
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                RetVal = AmbaUtility_UInt32ToStr(&pStr[5], 2U, (UINT32)(i % 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                pStr[6] = '.';
                pStr[7] = 'b';
                pStr[8] = 'i';
                pStr[9] = 'n';
                pStr[10] = '\0';

                RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
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
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                RetVal = AmbaUtility_UInt32ToStr(&pStr[5], 2U, (UINT32)(i % 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                pStr[6] = '.';
                pStr[7] = 'f';
                pStr[8] = 'e';
                pStr[9] = 'x';
                pStr[10] = '\0';

                RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
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
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                RetVal = AmbaUtility_UInt32ToStr(&pStr[5], 2U, (UINT32)(i % 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                pStr[6] = '.';
                pStr[7] = 'f';
                pStr[8] = 'e';
                pStr[9] = 'x';
                pStr[10] = '\0';

                RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
                if (RetVal == SVC_OK) {
                    AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
#endif
    return RetVal;
}

static UINT32 FileInTask_StereoFusionHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    UINT32 DataSize;
    char NewFileName[64];
    char *pStr;
    void *pAddr;
    const UINT8 *pU8Addr;
    ULONG ULONGAddr;
    const SVC_CV_STEREO_OUTPUT_s *pStereoOut;
    const AMBA_CV_SPU_BUF_s *pOutBuf;
    const AMBA_CV_SPU_DATA_s *pOutSpu;
    UINT32 i;
#if defined(CONFIG_THREADX)
    extern UINT32 AmbaUtility_Crc32Hw(const UINT8 *pBuffer, UINT32 Size);
    UINT32 Crc;
#endif
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&CvType);
    (void) pOutput;

    AmbaMisra_TypeCast(&pStereoOut, &pOutput);
    AmbaMisra_TypeCast(&pOutBuf, &pStereoOut->DataAddr);
    AmbaMisra_TypeCast(&pOutSpu, &pOutBuf->pBuffer);

#if 1
    {
        extern void SvcCvFlow_DumpSpuOutInfo(const char *pModuleName, const AMBA_CV_SPU_DATA_s *pOutSpu);
        SvcCvFlow_DumpSpuOutInfo(SVC_LOG_CV_FILE_IN_TASK, pOutSpu);
    }
#endif

    SvcWrap_strcpy(NewFileName, SVC_CV_FILE_IN_MAX_FILE_PATH - 1U, &g_FileInCtrl.InputCfg[0].FileName[0][0]);
    pStr = SvcWrap_strrchr(NewFileName, (INT32)'.');
    for (i = 0; i < MAX_HALF_OCTAVES; i++) {
        /* SPU */
        if ((pOutSpu->Scales[i].Status == 0) && (pOutSpu->Scales[i].BufSize > 0U)) {
            AmbaMisra_TypeCast(&ULONGAddr, &pOutSpu); /* U32Addr = pOutBuf->pBuffer */
            ULONGAddr += pOutSpu->Scales[i].DisparityMapOffset;
            AmbaMisra_TypeCast(&pAddr, &ULONGAddr);
            DataSize = pOutSpu->Scales[i].BufSize;
            SvcWrap_PrintUL("DisparityMap[%u]: 0x%x Size: %d", (ULONG)i, ULONGAddr, (ULONG)DataSize, 0U, 0U);

            if (pStr != NULL) {
                pStr[0] = '_';
                pStr[1] = 'f';
                pStr[2] = 'u';
                pStr[3] = 's';
                RetVal = AmbaUtility_UInt32ToStr(&pStr[4], 2U, (UINT32)(i / 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                RetVal = AmbaUtility_UInt32ToStr(&pStr[5], 2U, (UINT32)(i % 10U), 10U); /* BufferSize = 2: one char and one \0*/
                if (1U != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "UInt32ToStr failed(0x%x)", RetVal, 0U);
                }
                pStr[6] = '.';
                pStr[7] = 'b';
                pStr[8] = 'i';
                pStr[9] = 'n';
                pStr[10] = '\0';

                RetVal = FileInTask_FileWrite(NewFileName, pAddr, DataSize);
                if (RetVal == SVC_OK) {
                    AmbaMisra_TypeCast(&pU8Addr, &pAddr);
#if defined(CONFIG_THREADX)
                    Crc = AmbaUtility_Crc32Hw(pU8Addr, DataSize);
                    AmbaPrint_PrintUInt5("[FinTask] CRC = 0x%x", Crc, 0U, 0U, 0U, 0U);
#endif
                    AmbaPrint_PrintStr5("[FinTask] Write %s done", NewFileName, NULL, NULL, NULL, NULL);
                } else {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "FileInTask_FileWrite error", 0U, 0U);
                }
            }else {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "pStr == NULL", 0U, 0U);
            }
        }
    }

    return RetVal;
}


static UINT32 FileInTask_StereoOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    const SVC_CV_STEREO_OUTPUT_s *pStereoOut;

    (void) pOutput;

    AmbaMisra_TypeCast(&pStereoOut, &pOutput);
    SvcLog_DBG(SVC_LOG_CV_FILE_IN_TASK, "StereoOutputHandler: Type(0x%x)", pStereoOut->Type, 0U);

    switch (pStereoOut->Type) {
        case STEREO_OUT_TYPE_SPU_FEX:
            RetVal = FileInTask_StereoSpuFexHandler(Chan, CvType, pOutput);
            break;
        case STEREO_OUT_TYPE_FUSION:
            RetVal = FileInTask_StereoFusionHandler(Chan, CvType, pOutput);
            break;
        default:
            /* Do nothing */
            break;
    }

    return RetVal;
}
#endif

static void FileInTask_CvFlowOutputHandler(UINT32 Chan, UINT32 CvType, void *pOutput)
{
    UINT32 RetVal = SVC_OK;
    const UINT32 *pMsgCode;

    AmbaMisra_TouchUnused(&CvType);

    if ((Chan == g_FileInCtrl.Config.CvFlowChan) && (pOutput != NULL)) {
        AmbaMisra_TypeCast(&pMsgCode, &pOutput);
        SvcLog_DBG(SVC_LOG_CV_FILE_IN_TASK, "OutputHandler start. MsgCode(0x%x)", *pMsgCode, 0U);

        switch (*pMsgCode) {
            case SVC_CV_DETRES_BBX:
                RetVal = FileInTask_BbxOutputHandler(Chan, CvType, pOutput);
                break;
            case SVC_CV_PERCEPTION_OUTPUT:
                RetVal = FileInTask_PcptOutputHandler(Chan, CvType, pOutput);
                break;
#if defined(CONFIG_ICAM_CV_STEREO)
            case SVC_CV_STEREO_OUTPUT:
                RetVal = FileInTask_StereoOutputHandler(Chan, CvType, pOutput);;
                break;
#endif
            default:
                /* Do nothing */
                break;
        }

        if (RetVal == SVC_OK) {
            if (SVC_OK != AmbaKAL_EventFlagSet(&g_FileInEventFlag, CV_FLOW_DONE)) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "OutputHandler set flag failed", 0U, 0U);
            }
            SvcLog_OK(SVC_LOG_CV_FILE_IN_TASK, "OutputHandler Done", RetVal, 0U);
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "OutputHandler Done with error", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "OutputHandler Done with invalid info", 0U, 0U);
    }
}

static UINT32 FileInTask_SendInput(UINT32 CvFlowChan, UINT32 NumInput)
{
    UINT32 RetVal;
    const SVC_CV_FILE_IN_INPUT_CFG_s *pInputCfg;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 i, j, k;
    SVC_CV_INPUT_IMG_INFO_s ImgSend;
    AMBA_DSP_PYMD_DATA_RDY_s YuvInfo[SVC_CV_FILE_IN_MAX_INPUT_PER_CHAN];
    const AMBA_DSP_PYMD_DATA_RDY_s *pYuvInfo[SVC_CV_FILE_IN_MAX_INPUT_PER_CHAN] = {NULL, NULL};
    UINT32 ScaleIdx, HierBit;

    RetVal = AmbaWrap_memset(&ImgSend, 0, sizeof(SVC_CV_INPUT_IMG_INFO_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "memset ImgSend failed", 0U, 0U);
    }
    ImgSend.NumInfo = NumInput;


    if (g_FileInCtrl.Config.Mode == SVC_CV_FILE_IN_YUV_MODE) {
        for (i = 0; i < NumInput; i++) {
            pYuvInfo[i] = &YuvInfo[i];
            RetVal = AmbaWrap_memset(&YuvInfo[i], 0, sizeof(AMBA_DSP_PYMD_DATA_RDY_s));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "memset YuvInfo[%u] failed", i, 0U);
            }

            pInputCfg = &g_FileInCtrl.InputCfg[i];

            YuvInfo[i].ViewZoneId = (UINT16) pCvFlow[CvFlowChan].InputCfg.Input[i].StrmId;
            for (j = 0; j < pInputCfg->NumData; j++) {
                ScaleIdx = pInputCfg->DataInfo[j].Img.ScaleIdx;
                HierBit = ((UINT32)1U << ScaleIdx);

                YuvInfo[i].Config.HierBit                 |= (UINT16) HierBit;
                YuvInfo[i].YuvBuf[ScaleIdx].DataFmt        = 0;
                YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrY      = g_FileInCtrl.DataBuf[i][j].YuvBuf.YAddr;
                if (g_FileInCtrl.DataBuf[i][j].YuvBuf.UVAddr == 0U) {
                    YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrUV = YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrY;
                } else {
                    YuvInfo[i].YuvBuf[ScaleIdx].BaseAddrUV = g_FileInCtrl.DataBuf[i][j].YuvBuf.UVAddr;
                }
                YuvInfo[i].YuvBuf[ScaleIdx].Pitch          = (UINT16) pInputCfg->DataInfo[j].Img.Pitch;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.OffsetX = 0;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.OffsetY = 0;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.Width   = (UINT16) pInputCfg->DataInfo[j].Img.Width;
                YuvInfo[i].YuvBuf[ScaleIdx].Window.Height  = (UINT16) pInputCfg->DataInfo[j].Img.Height;
            }

            AmbaPrint_PrintUInt5("# Send YuvInfo: CvFlowChan(%d) ViewZoneId(%d)",
                                CvFlowChan,
                                YuvInfo[i].ViewZoneId,
                                0U,
                                0U,
                                0U);
            for (k = 0; k < AMBA_DSP_MAX_HIER_NUM; k++) {
                SvcWrap_PrintUL("[%d] DataFmt %d Y: 0x%x UV: 0x%x",
                                    (ULONG)i,
                                    (ULONG)YuvInfo[i].YuvBuf[k].DataFmt,
                                    YuvInfo[i].YuvBuf[k].BaseAddrY,
                                    YuvInfo[i].YuvBuf[k].BaseAddrUV,
                                    (ULONG)0U);
                AmbaPrint_PrintUInt5("(%d %d %d %d)",
                                    YuvInfo[i].YuvBuf[k].Window.OffsetX,
                                    YuvInfo[i].YuvBuf[k].Window.OffsetY,
                                    YuvInfo[i].YuvBuf[k].Window.Width,
                                    YuvInfo[i].YuvBuf[k].Window.Height,
                                    0U);
            }

            ImgSend.Info[i].Content.DataSrc = SVC_CV_DATA_SRC_PYRAMID;
            ImgSend.Info[i].Content.StrmId = (UINT16) pCvFlow[CvFlowChan].InputCfg.Input[i].StrmId;
            ImgSend.Info[i].ElementSize = (UINT32)sizeof(AMBA_DSP_PYMD_DATA_RDY_s);
            AmbaMisra_TypeCast(&ImgSend.Info[i].pBase, &pYuvInfo[i]);
        }
    } else{
        // For Raw...
    }


    RetVal = SvcCvFlow_Control(CvFlowChan, SVC_CV_CTRL_SEND_EXT_YUV, &ImgSend);

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "SEND_EXT_YUV error", 0U, 0U);
    }

    return RetVal;
}

/**
 *  Init the CV file input task
 *  @return error code
 */
UINT32 SvcCvFileInTask_Init(void)
{
    return SVC_OK;
}

/**
 *  Configure the CV file input task
 *  @param[in] pConfig CV File-In configs
 *  @return error code
 */
UINT32 SvcCvFileInTask_Config(const SVC_CV_FILE_IN_CONFIG_s *pConfig)
{
    UINT32 RetVal;
    static char FileinFlagName[32] = "FileinFlag";
    static UINT32 g_CvFlowRegisterID = 0xffU;
    static UINT8 Init = 0U;

    if (NULL != pConfig) {
        SvcLog_DBG(SVC_LOG_CV_FILE_IN_TASK, "Config CvFlowChan(%u), Mode(%u)", pConfig->CvFlowChan, pConfig->Mode);

        RetVal = AmbaWrap_memset(&g_FileInCtrl, 0, sizeof(SVC_CV_FILE_IN_CTRL_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "memset g_FileInCtrl failed", 0U, 0U);
        }

        RetVal = AmbaWrap_memcpy(&g_FileInCtrl.Config, pConfig, sizeof(SVC_CV_FILE_IN_CONFIG_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "memcpy Config failed", 0U, 0U);
        }

        RetVal = AmbaWrap_memset(&g_MultiInfo, 0, sizeof(SVC_CV_FILE_IN_MULTIPLE_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "memset g_MultiInfo failed", 0U, 0U);
        }

        if (Init == 0U) {
            Init = 1U;
            RetVal = AmbaKAL_EventFlagCreate(&g_FileInEventFlag, FileinFlagName);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "FileInEventFlag isn't created", 0U, 0U);
            }
        }

        if (g_CvFlowRegisterID == 0xffU) {
            RetVal = SvcCvFlow_Register(pConfig->CvFlowChan, FileInTask_CvFlowOutputHandler, &g_CvFlowRegisterID);
        } else {
            RetVal = SVC_OK;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "pConfig = NULL", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Set input information for the CV file input task
 *  @param[in] Type CV File-In type
 *  @param[in] pInfo CV File-In information
 *  @return error code
 */
UINT32 SvcCvFileInTask_SetInfo(UINT32 Type, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    UINT32 InputIdx, DataIdx;
    SVC_CV_FILE_IN_INPUT_CFG_s *pInputCfg;
    const SVC_CV_FILE_IN_INPUT_INFO_s *pInputInfo;
    const SVC_CV_FILE_IN_FILE_INFO_s *pFileInfo;
    SVC_CV_FILE_IN_NAMING_CONFIG_s *pNameCfg;
    UINT32 StrLength;

    AmbaMisra_TouchUnused(pInfo);

    switch (Type) {
        case SVC_CV_FILE_IN_SET_INPUT_INFO:
            AmbaMisra_TypeCast(&pInputInfo, &pInfo);
            InputIdx = pInputInfo->InputIdx;
            DataIdx  = pInputInfo->DataIdx;

            if ((InputIdx < SVC_CV_FILE_IN_MAX_INPUT_PER_CHAN) && (DataIdx < SVC_CV_FILE_IN_MAX_DATA_PER_INPUT)) {
                pInputCfg = &g_FileInCtrl.InputCfg[InputIdx];
                pInputCfg->InputIdx = InputIdx;
                pInputCfg->NumData++;
                RetVal = AmbaWrap_memcpy(&pInputCfg->DataInfo[DataIdx], &pInputInfo->DataInfo, sizeof(SVC_CV_FILE_IN_DATA_INFO_s));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "memcpy DataInfo failed", 0U, 0U);
                }

                AmbaPrint_PrintUInt5("[InputInfo] InputIdx: %u, NumData: %u, DataIdx: %u",
                                      pInputCfg->InputIdx, pInputCfg->NumData, DataIdx, 0U, 0U);
                AmbaPrint_PrintUInt5("ScaleIdx: %u, W: %u, H: %u, P: %u",
                                      pInputCfg->DataInfo[DataIdx].Img.ScaleIdx,
                                      pInputCfg->DataInfo[DataIdx].Img.Width,
                                      pInputCfg->DataInfo[DataIdx].Img.Height,
                                      pInputCfg->DataInfo[DataIdx].Img.Pitch,
                                      0U);
            } else {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Invalid input info", 0U, 0U);
                RetVal = SVC_NG;
            }
            break;
        case SVC_CV_FILE_IN_SET_FILE_INFO:
            AmbaMisra_TypeCast(&pFileInfo, &pInfo);
            InputIdx = pFileInfo->InputIdx;
            DataIdx  = pFileInfo->DataIdx;

            if ((InputIdx < SVC_CV_FILE_IN_MAX_INPUT_PER_CHAN) && (DataIdx < SVC_CV_FILE_IN_MAX_DATA_PER_INPUT)) {
                StrLength = (UINT32)SvcWrap_strlen(pFileInfo->FileName);
                /* Remain 11 char for renaming */
                if ((SVC_CV_FILE_IN_MAX_FILE_PATH - StrLength) < 11U) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "File name too long", 0U, 0U);
                    RetVal = SVC_NG;
                }

                if (RetVal == SVC_OK) {
                    pInputCfg = &g_FileInCtrl.InputCfg[InputIdx];
                    pInputCfg->FileValid[DataIdx] = 1U;
                    SvcWrap_strcpy(&pInputCfg->FileName[DataIdx][0], SVC_CV_FILE_IN_MAX_FILE_PATH, pFileInfo->FileName);

                    AmbaPrint_PrintUInt5("[FileInfo] InputIdx: %u, NumData: %u, DataIdx: %u",
                                          pInputCfg->InputIdx, pInputCfg->NumData, DataIdx, 0U, 0U);
                    AmbaPrint_PrintStr5("%s", pFileInfo->FileName, NULL, NULL, NULL, NULL);
                } else {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Set file info error", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Invalid file info", 0U, 0U);
                RetVal = SVC_NG;
            }
            break;
        case SVC_CV_FILE_IN_SET_BATCH_FILE_INFO:
            AmbaMisra_TypeCast(&pNameCfg, &pInfo);
            RetVal = FileInTask_BatchFileInfo(pNameCfg);
            break;
        case SVC_CV_FILE_IN_SET_OUPUT_INFO:
            AmbaMisra_TypeCast(&pNameCfg, &pInfo);
            RetVal = FileInTask_ConfigOutput(pNameCfg);
            break;
        default:
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Invalid info type", 0U, 0U);
            RetVal = SVC_NG;
            break;
    }

    return RetVal;
}

/**
 *  Start the CV file input task
 *  @param[in] pStartCfg CV File-In information
 *  @return error code
 */
UINT32 SvcCvFileInTask_Start(const SVC_CV_FILE_IN_START_s *pStartCfg)
{
    UINT32 RetVal;

    if (g_MultiInfo.Enable == 0U) {
        RetVal = FileInTask_StartSingle(pStartCfg);
    } else {
        RetVal = FileInTask_StartMultiple(pStartCfg);
    }

    return RetVal;
}

static UINT32 FileInTask_StartSingle(const SVC_CV_FILE_IN_START_s *pStartCfg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i, j, NumFile, NumInputChan = 0;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    const SVC_CV_FILE_IN_INPUT_CFG_s *pInputCfg;
    UINT32 EventFlag;

    AmbaMisra_TouchUnused(&pStartCfg);

    /* Confirm all input info have ready */
    for (i = 0; i < pCvFlow->InputCfg.InputNum; i++) {
        pInputCfg = &g_FileInCtrl.InputCfg[i];
        NumFile = 0;
        for (j = 0; j < pInputCfg->NumData; j++) {
            if (pInputCfg->FileValid[j] > 0U) {
                NumFile++;
            }
        }

        if (pInputCfg->NumData == NumFile) {
            NumInputChan++;
        }
    }

    if (pCvFlow->InputCfg.InputNum == NumInputChan) {
        FileInTask_InitBuf();

        for (i = 0; i < pCvFlow->InputCfg.InputNum; i++) {
            RetVal |= FileInTask_LoadYuvFile(i);
        }

        if (RetVal == SVC_OK) {
            RetVal = FileInTask_SendInput(g_FileInCtrl.Config.CvFlowChan, pCvFlow->InputCfg.InputNum);
            if (RetVal == SVC_OK) {
                RetVal = AmbaKAL_EventFlagGet(&g_FileInEventFlag,
                                              CV_FLOW_DONE,
                                              AMBA_KAL_FLAGS_ANY,
                                              AMBA_KAL_FLAGS_CLEAR_AUTO,
                                              &EventFlag,
                                              5000);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK,"Wait CV_FLOW_DONE failed", 0U, 0U);
                }
            }
        }
    } else {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Check InputCfg failed", 0U, 0U);
    }

    return RetVal;
}

static UINT32 FileInTask_StartMultiple(const SVC_CV_FILE_IN_START_s *pStartCfg)
{
    UINT32 RetVal;
    AmbaMisra_TouchUnused(&pStartCfg);
    g_MultiInfo.pSubDrInfo = NULL;
    RetVal = AmbaKAL_EventFlagSet(&g_FileInEventFlag, BATCH_TASK_ENABLE);
    return RetVal;
}

static UINT32 FileInTask_GetNextFile(UINT32 InputIdx, UINT32 *isEnd)
{
#if defined(CONFIG_THREADX)
    UINT32 RetVal = SVC_OK, GetMain = 0U, i;
    static AMBA_FS_DIRENT Dirent, SubDirent;
    static char FolderName[128U];
    const char *loc, *pName;
    const char *ArgS[2U];
    SVC_CV_FILE_IN_INPUT_CFG_s *pInputCfg = &g_FileInCtrl.InputCfg[InputIdx];

    if (g_MultiInfo.pSubDrInfo == NULL) {
        *isEnd = AmbaFS_ReadDir(g_MultiInfo.pDrInfo, &(Dirent));
    }

    if ( (*isEnd == 0U) && (Dirent.attr == 0x10U) ) {

        if (g_MultiInfo.pSubDrInfo == NULL) {

            if (SvcWrap_strlen((char *)Dirent.long_dname)==0U) {
                ArgS[0] = g_MultiInfo.InputPathPrefix;
                ArgS[1] = (char *) Dirent.short_dname;
                SvcWrap_strcpy(&(pInputCfg->FileName[0][0]), sizeof(pInputCfg->FileName[0]), (char *)Dirent.short_dname); //FolderPath
            } else {
                ArgS[0] = g_MultiInfo.InputPathPrefix;
                ArgS[1] = (char *)Dirent.long_dname;
                SvcWrap_strcpy(&(pInputCfg->FileName[0][0]), sizeof(pInputCfg->FileName[0]), (char *)Dirent.long_dname);
            }
            if (SvcWrap_sprintfStr(FolderName, (UINT32)sizeof(FolderName), "%s\\%s",2, ArgS) >= sizeof(FolderName)) {
                RetVal = SVC_NG;
            }

            if (RetVal == SVC_OK) {
                RetVal = AmbaFS_OpenDir(FolderName, &g_MultiInfo.pSubDrInfo);
            }
        }

        if (RetVal == 0U) {
            do {
                RetVal = AmbaFS_ReadDir(g_MultiInfo.pSubDrInfo, &(SubDirent));
                if (RetVal == 0U) {
                    if (SvcWrap_strlen((char *)SubDirent.long_dname) == 0U) {
                        pName = (char *)SubDirent.short_dname;
                    } else {
                        pName = (char *)SubDirent.long_dname;
                    }

                    if (g_FileInCtrl.Config.Mode == SVC_CV_FILE_IN_YUV_MODE) {
                        loc = SvcWrap_strstr(pName, "_y_y_pad32.bin");  //.y
                    } else {
                        loc = SvcWrap_strstr(pName, "_pad32.bin");  //rgb
                    }
                    if (loc != NULL) {
                        GetMain = 1U;
                        if (g_FileInCtrl.Config.Mode == SVC_CV_FILE_IN_YUV_MODE) {
                            for(i=0; i<SvcWrap_strlen(pName);i++) {
                                if (pName[i]!='_') {
                                    FolderName[i] = pName[i];
                                } else {
                                    FolderName[i] = '\0';
                                    break;
                                }
                            }
                            SvcWrap_strcpy(&(pInputCfg->FileName[1][0]), sizeof(pInputCfg->FileName[1]), pName); //MainFile , raw or y file
                            ArgS[0] = FolderName;

                            //uv file
                            if (SvcWrap_sprintfStr(&(pInputCfg->FileName[2][0]), (UINT32)sizeof(pInputCfg->FileName[2]), "%s_uv_uv_pad32.bin",1, ArgS) >= sizeof(pInputCfg->FileName[2])) {
                                RetVal = SVC_NG;
                            }

                        } else {
                            SvcWrap_strcpy(&(pInputCfg->FileName[1][0]), sizeof(pInputCfg->FileName[1]), pName);
                        }
                    } else {
                        //
                    }

                } else {
                    if (AmbaFS_CloseDir(g_MultiInfo.pSubDrInfo) != 0U) {
                        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "CloseDir failed", 0U, 0U);
                    }
                    g_MultiInfo.pSubDrInfo = NULL;
                }

            } while((GetMain == 0U) && (RetVal == 0U));
        } else {
            RetVal = SVC_NG;
        }

    } else {
        RetVal = SVC_NG;
    }

    return RetVal;

#else
    AmbaMisra_TouchUnused(&InputIdx);
    AmbaMisra_TouchUnused(isEnd);
    return SVC_OK;
#endif
}

static UINT32 FileInTask_LoadFileByName(UINT32 InputIdx)
{
#if defined(CONFIG_THREADX)
    UINT32 RetVal = SVC_OK, i=0U;
    UINT32 DataSize;
    const SVC_CV_FILE_IN_INPUT_CFG_s *pInputCfg = &g_FileInCtrl.InputCfg[InputIdx];
    AMBA_FS_FILE_INFO_s FileInfo;
    void *pAddr;
    static UINT32 Allocated = 0U;
    char fileName[128];
    const char *ArgS[3U];


    ArgS[0] = g_MultiInfo.InputPathPrefix ;
    ArgS[1] = pInputCfg->FileName[0];
    ArgS[2] = pInputCfg->FileName[1];
    if (SvcWrap_sprintfStr(fileName, (UINT32)sizeof(fileName), "%s\\%s\\%s",3, ArgS) >= (UINT32)sizeof(fileName)) {
        RetVal = SVC_NG;
    }

    if (g_FileInCtrl.Config.Mode == SVC_CV_FILE_IN_YUV_MODE) {
            /* Read Y file */
            DataSize = GetBufferSize(DATA_FMT_Y, pInputCfg->DataInfo[i].Img.Pitch, pInputCfg->DataInfo[i].Img.Height);
        if (Allocated == 0U) {
            RetVal = FileInTask_AllocBuf(DataSize, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.YAddr);
        }
        if (RetVal == SVC_OK) {
            AmbaMisra_TypeCast32(&pAddr, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.YAddr);
            RetVal = FileInTask_ReadFile(fileName, pAddr, DataSize);
            if (RetVal == SVC_OK) {
                if (CACHE_ERR_NONE != SvcPlat_CacheClean(g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.YAddr, DataSize)) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Clean Cache error(%d)", RetVal, 0U);
                }
                AmbaPrint_PrintStr5("[FinTask] Read %s done", fileName, NULL, NULL, NULL, NULL);
            } else {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "FileInTask_ReadFile error(%d)", RetVal, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Alloc Y buf error", 0U, 0U);
        }

        if (RetVal == SVC_OK) {
            /* Read UV file */
            ArgS[2] = pInputCfg->FileName[2];
            if (SvcWrap_sprintfStr(fileName, (UINT32)sizeof(fileName), "%s\\%s\\%s",3, ArgS) >= (UINT32)sizeof(fileName)) {
                RetVal = SVC_NG;
            }

            if (AMBA_FS_ERR_NONE == AmbaFS_GetFileInfo(fileName, &FileInfo)) {
                /* UV file exist */
                    DataSize = GetBufferSize(DATA_FMT_UV420, pInputCfg->DataInfo[i].Img.Pitch, pInputCfg->DataInfo[i].Img.Height);
                if (Allocated == 0U) {
                    RetVal = FileInTask_AllocBuf(DataSize, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.UVAddr);
                }
                if (RetVal == SVC_OK) {
                    AmbaMisra_TypeCast32(&pAddr, &g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.UVAddr);
                    RetVal = FileInTask_ReadFile(fileName, pAddr, DataSize);
                    if (RetVal == SVC_OK) {
                        if (CACHE_ERR_NONE != SvcPlat_CacheClean(g_FileInCtrl.DataBuf[InputIdx][i].YuvBuf.UVAddr, DataSize)) {
                            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Clean Cache error(%d)", RetVal, 0U);
                        }
                        AmbaPrint_PrintStr5("[FinTask] Read %s done", fileName, NULL, NULL, NULL, NULL);
                    } else {
                        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "FileInTask_ReadFile error(%d)", RetVal, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Alloc UV buf error", 0U, 0U);
                }
            } else {
                AmbaPrint_PrintUInt5("UV file not exist", 0U, 0U, 0U, 0U, 0U);
            }
        }
    } else { //Load RawData
        DataSize = GetBufferSize(DATA_FMT_RGB, pInputCfg->DataInfo[i].Img.Pitch, pInputCfg->DataInfo[i].Img.Height);
        if (Allocated == 0U) {
            RetVal = FileInTask_AllocBuf(DataSize, &g_FileInCtrl.DataBuf[InputIdx][i].RgbAddr);
        }

        if (RetVal == SVC_OK) {
            AmbaMisra_TypeCast32(&pAddr, &g_FileInCtrl.DataBuf[InputIdx][i].RgbAddr);
            RetVal = FileInTask_ReadFile(fileName, pAddr, DataSize);
            if (RetVal == SVC_OK) {
                if (CACHE_ERR_NONE != SvcPlat_CacheClean(g_FileInCtrl.DataBuf[InputIdx][i].RgbAddr, DataSize)) {
                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Clean Cache error(%d)", RetVal, 0U);
                }
                AmbaPrint_PrintStr5("[FinTask] Read %s done", fileName, NULL, NULL, NULL, NULL);
            } else {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "FileInTask_ReadFile error(%d)", RetVal, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Alloc Rgb buf error", 0U, 0U);
        }
    }

    Allocated = 1U;

    return RetVal;

#else
    AmbaMisra_TouchUnused(&InputIdx);
    return SVC_OK;
#endif
}

static void* FileInTask_BatchFileInTask(void *EntryArg)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 EventFlag, ProcCount;

    UINT32 isEnd = 0U;
    AmbaMisra_TouchUnused(EntryArg);
    FileInTask_InitBuf();

    while (RetVal == SVC_OK) {
        RetVal = AmbaKAL_EventFlagGet(&g_FileInEventFlag,
                                      BATCH_TASK_ENABLE,
                                      AMBA_KAL_FLAGS_ANY,
                                      AMBA_KAL_FLAGS_CLEAR_AUTO,
                                      &EventFlag,
                                      AMBA_KAL_WAIT_FOREVER);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK,"AmbaKAL_EventFlagGet fail", 0U, 0U);
        }

        SvcLog_OK(SVC_LOG_CV_FILE_IN_TASK,"Start batch file in", 0U, 0U);
        RetVal = AmbaFS_OpenDir(g_MultiInfo.InputPathPrefix, &g_MultiInfo.pDrInfo);

        if (RetVal == 0U) {
            ProcCount = 0U;
            do {
                RetVal = FileInTask_GetNextFile(0U, &isEnd);
                if (RetVal == SVC_OK) {
                    RetVal = FileInTask_LoadFileByName(0U);

                    if (RetVal == SVC_OK) {
                        RetVal = FileInTask_SendInput(g_FileInCtrl.Config.CvFlowChan, pCvFlow->InputCfg.InputNum);

                        if (RetVal == SVC_OK) {
                            RetVal = AmbaKAL_EventFlagGet(&g_FileInEventFlag,
                                                          CV_FLOW_DONE,
                                                          AMBA_KAL_FLAGS_ANY,
                                                          AMBA_KAL_FLAGS_CLEAR_AUTO,
                                                          &EventFlag,
                                                          5000);

                            if (RetVal != SVC_OK) {
                                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Wait CV_FLOW_DONE failed", 0U, 0U);
                            }
                        }
                    }

                    ProcCount++;
                    if (ProcCount >= g_MultiInfo.MaxNumber) {
                        if (AmbaFS_CloseDir(g_MultiInfo.pSubDrInfo) != 0U) {
                            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "CloseDir failed", 0U, 0U);
                        }
                        g_MultiInfo.pSubDrInfo = NULL;
                        break;
                    }
                } else {
                    RetVal = SVC_OK;
                }
            } while ((isEnd == 0U) && (RetVal == SVC_OK));

            if (AmbaFS_CloseDir(g_MultiInfo.pDrInfo) != 0U) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "CloseDir failed", 0U, 0U);
            }
        }

        SvcLog_OK(SVC_LOG_CV_FILE_IN_TASK,"Finish batch file in", 0U, 0U);
    }

    return NULL;
}

static UINT32 FileInTask_BatchFileInfo(SVC_CV_FILE_IN_NAMING_CONFIG_s *pNameCfg)
{
    UINT32 RetVal = SVC_OK;
    AMBA_FS_DIR *pDir, *pDiro;
    UINT32 StringLen;
    const char *pStr;
    char *InputFolder, *OutputFolder;
    const char *ArgS[2U];
    static UINT32 isSet = 0U;
    static SVC_TASK_CTRL_s BatchFileInTask;
    static UINT8  BatchFileInStack[0x4000U] GNU_SECTION_NOZEROINIT;

    g_MultiInfo.Enable = 1U;
    g_MultiInfo.pSubDrInfo = NULL;
    g_MultiInfo.MaxNumber = pNameCfg->MaxNumber;
    InputFolder = pNameCfg->FileNameBuf[0];
    OutputFolder = pNameCfg->FileNameBuf[1];

    StringLen = (UINT32)SvcWrap_strlen(pNameCfg->FileNameBuf[0]);

    if (StringLen != 0U) {
        if (InputFolder[StringLen - 1U] == '\\') {
            InputFolder[StringLen - 1U] = '\0';
        }

        RetVal = AmbaFS_OpenDir(InputFolder, &pDir);

        if (RetVal == 0U) {

            if (AmbaFS_CloseDir(pDir) != 0U) {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "CloseDir failed", 0U, 0U);
            }

            StringLen = (UINT32)SvcWrap_strlen(pNameCfg->FileNameBuf[1]);
            if (StringLen != 0U) {
                if (OutputFolder[StringLen - 1U] == '\\') {
                    OutputFolder[StringLen - 1U] = '\0';
                }

                RetVal = AmbaFS_OpenDir(OutputFolder, &pDiro);

                if (RetVal == 0U) {

                    SvcWrap_strcpy(g_MultiInfo.InputPathPrefix, SVC_CV_FILE_IN_MAX_FILE_PATH, InputFolder);
                    if (AmbaFS_CloseDir(pDiro) != 0U) {
                        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "CloseDir failed", 0U, 0U);
                    }

                    // find last folder name of input file.
                    pStr = SvcWrap_strrchr(g_MultiInfo.InputPathPrefix, (INT32)'\\');
                    if (pStr != NULL) {
                        ArgS[0] = OutputFolder;
                        ArgS[1] = &(pStr[1]);
                        if (SvcWrap_sprintfStr(g_MultiInfo.OutputPathPrefix, (UINT32)sizeof(g_MultiInfo.OutputPathPrefix), "%s\\%s", 2, ArgS) >= sizeof(g_MultiInfo.OutputPathPrefix)) {
                            RetVal = SVC_NG;
                            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "sprintfStr failed", 0U, 0U);
                        }

                        if (SvcWrap_strcmp(g_MultiInfo.InputPathPrefix, g_MultiInfo.OutputPathPrefix) == 0) {
                            RetVal = SVC_NG;
                            SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Input and Output folder should not be the same", 0U, 0U);
                        }

                        if (RetVal == SVC_OK) {
                            if (AmbaFS_MakeDir(g_MultiInfo.OutputPathPrefix) == 0U) {
                                AmbaPrint_PrintStr5("Create output_prefix folder: %s Done", g_MultiInfo.OutputPathPrefix, NULL, NULL, NULL, NULL);
                            }

                            if (isSet == 0U) {
                                BatchFileInTask.Priority = 130;
                                BatchFileInTask.EntryFunc = FileInTask_BatchFileInTask;
                                BatchFileInTask.EntryArg = 0U;
                                BatchFileInTask.pStackBase = &BatchFileInStack[0];
                                BatchFileInTask.StackSize = 0x4000U;
                                BatchFileInTask.CpuBits = 0x01U;
                                RetVal = SvcTask_Create("BatchFileInTask", &BatchFileInTask);
                                if (SVC_OK != RetVal) {
                                    SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "Create BatchFileInTask error", 0U, 0U);
                                }
                                isSet = 1U;
                            }
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "SvcWrap_strrchr return NULL", 0U, 0U);
                        RetVal = SVC_NG;
                    }
                } else {
                    AmbaPrint_PrintStr5("Folder %s does not exist, please create the folder" , OutputFolder, NULL, NULL, NULL, NULL);
                    RetVal = SVC_OK; // OK don't show manual, NG show.
                }
            }else {
                SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "pNameCfg->FileNameBuf[1] strlen = 0", 0U, 0U);
            }
        } else {
           AmbaPrint_PrintStr5("Input folder %s does not exist" , InputFolder, NULL, NULL, NULL, NULL);
           RetVal = SVC_OK;
        }

    }else {
        SvcLog_NG(SVC_LOG_CV_FILE_IN_TASK, "pNameCfg->FileNameBuf[0] strlen = 0", 0U, 0U);
    }

    return RetVal;
}

static UINT32 FileInTask_ConfigOutput(const SVC_CV_FILE_IN_NAMING_CONFIG_s *pNameCfg)
{
    UINT32 RetVal = SVC_OK, i;
    for (i = 0; i < pNameCfg->MaxNumber; i++) {
        (void) SvcWrap_strcpy(g_MultiInfo.OutputLayerName[i], sizeof(g_MultiInfo.OutputLayerName[i]), pNameCfg->FileNameBuf[i]);
        SVC_WRAP_PRINT "OutputLayer[%u] : [%s]" SVC_PRN_ARG_S SVC_LOG_CV_FILE_IN_TASK SVC_PRN_ARG_UINT32 i SVC_PRN_ARG_POST SVC_PRN_ARG_CSTR g_MultiInfo.OutputLayerName[i] SVC_PRN_ARG_POST SVC_PRN_ARG_E
    }

    SvcLog_OK(SVC_LOG_CV_FILE_IN_TASK,"Output LayerName setting successes.", 0U, 0U);

    return RetVal;
}
