/**
 *  @file AmbaCT_TextHdlr.c
 *
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
#include "AmbaCT_LdccTunerIF.h"
#include "AmbaCT_LdccTuner.h"
#include "AmbaCT_LdccRule.h"
#include "AmbaCT_1DVigTuner.h"
#include "AmbaCT_1DVigRule.h"
#include "AmbaCT_AvmTuner.h"
#include "AmbaCT_AvmRule.h"
#include "AmbaCT_EmirTuner.h"
#include "AmbaCT_EmirRule.h"
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
#include "AmbaCT_StereoTuner.h"
#include "AmbaCT_StereoRule.h"
#endif
#include "AmbaCT_LdcTuner.h"
#include "AmbaCT_LdcRule.h"
#include "AmbaCT_OcTuner.h"
#include "AmbaCT_OcRule.h"
#include "AmbaCT_OcCbTuner.h"
#include "AmbaCT_OcCbRule.h"
#include "AmbaCT_CaTuner.h"
#include "AmbaCT_CaRule.h"
#include "AmbaCT_BpcTuner.h"
#include "AmbaCT_BpcRule.h"
#include "AmbaCT_Parser.h"
#include "AmbaCT_ProcRule.h"
#include "AmbaCT_TextHdlr.h"
#include "AmbaFS.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaCT_Logging.h"
#include "AmbaMisraFix.h"
#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif
typedef struct {
    UINT32 (*pInitFunc)(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg);
    UINT32 (*pLoadFunc)(const char *pFileName);
    UINT32 (*pExecuteFunc)(void);
    UINT32 (*pDumpCalibDataFunc)(void);
} AMBA_CT_Handlr_s;

typedef struct {
    struct {
        AMBA_CAL_SIZE_s MaxImageSize;
        UINT32 Resolution;
    } Vig1d;
    struct {
        AMBA_CAL_SIZE_s MaxImageSize;
        UINT32 Resolution;
    } Bpc;
} CT_CONFIG_s;

static CT_Parser_Object_t *p1DVigParserObject = NULL;
static CT_Parser_Object_t *pAvmParserObject = NULL;
static CT_Parser_Object_t *pEmParserObject = NULL;
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
static CT_Parser_Object_t *pStereoParserObject = NULL;
#endif
static CT_Parser_Object_t *pLdcParserObject = NULL;
static CT_Parser_Object_t *pOcParserObject = NULL;
static CT_Parser_Object_t *pLdccParserObject = NULL;
static CT_Parser_Object_t *pOcCbParserObject = NULL;
static CT_Parser_Object_t *pBpcParserObject = NULL;
static CT_Parser_Object_t *pCaParserObject = NULL;

static CT_CONFIG_s CalibTunerCfg = {
    .Vig1d = {
        .MaxImageSize = {
            .Width = 4000U,
            .Height = 3000U
        },
        .Resolution = 16
    },
    .Bpc = {
        .MaxImageSize = {
            .Width = 4000U,
            .Height = 3000U
        },
        .Resolution = 16
    }
};

#define READ_BUF_SIZE 1024

typedef struct {
    UINT32 GridStatus[MAX_WARP_TBL_LEN];
    UINT32 HorTileNum;
    UINT32 VerTileNum;
} AMBA_CT_AVM_GRID_STATUS_s;

static AMBA_CT_AVM_GRID_STATUS_s AvmGridStatus[AMBA_CAL_AVM_CAM_MAX] GNU_SECTION_NOZEROINIT;

#define AMBA_CT_EM_GRID_STATUS_s AMBA_CT_AVM_GRID_STATUS_s
static AMBA_CT_EM_GRID_STATUS_s EmrGridStatus[AMBA_CAL_EM_CAM_MAX] GNU_SECTION_NOZEROINIT;

#define KB (1024U)
#define MB (1024U*KB)
#define AVM_PRE_CAL_BUFF_SIZE (2ULL*MB)
static UINT8 AvmPreCalData[AMBA_CAL_AVM_CAM_MAX][AVM_PRE_CAL_BUFF_SIZE] GNU_SECTION_NOZEROINIT;

#define AVM_PRE_CHECK_BUFF_SIZE (512U*KB)
static UINT8 AvmPreCheckData[AMBA_CAL_AVM_CAM_MAX][AVM_PRE_CHECK_BUFF_SIZE] GNU_SECTION_NOZEROINIT;

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static void AmbaCT_TouchUnused(const void *pData)
{
    const void *pMisraC;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    CT_CheckRval(AmbaWrap_memcpy(&pMisraC, &pData, sizeof(void *)), "AmbaWrap_memcpy", __func__);
    pNextMsg = CT_LogPutStr(Msg, "Hi MisraC");
    pNextMsg = CT_LogPutAddr(pNextMsg, pMisraC);
    CT_CheckRval(CT_LogPackMsg(CT_LOG_LEVEL_DEBUG, Msg, pNextMsg), "CT_LogPackMsg", __func__);
}

static UINT32 AmbaCT_AvmCbMsgReciver(AMBA_CAL_AVM_MSG_TYPE_e Type, AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_MSG_s *pMsg)
{
    UINT32 Rval = CT_OK;
    switch (Type) {
    case AMBA_AVM_MSG_GRID_STATUS:
    {
        UINT32 *pStatusTbl = AvmGridStatus[CamId].GridStatus;
        const AMBA_CAL_AVM_WARP_TBL_STATUS_s *pStatus;
        pStatus = pMsg->pGridStatus;
        if (((SIZE_t)pStatus->HorGridNum * (SIZE_t)pStatus->VerGridNum) > MAX_WARP_TBL_LEN) {
            Rval = CT_ERR_1;
        } else {
            AvmGridStatus[CamId].HorTileNum = pStatus->HorGridNum;
            AvmGridStatus[CamId].VerTileNum = pStatus->VerGridNum;
            pStatusTbl[pStatus->GridPosX + (pStatus->GridPosY * pStatus->HorGridNum)] = pStatus->Status;
        }
        break;
    }
    case AMBA_AVM_MSG_PTN_ERR:
    case AMBA_AVM_MSG_REPORT:
        break;
    default:
        Rval = CT_ERR_1;
        break;
    }
    return Rval;
}

static UINT32 AmbaCT_AvmCbSavePreCheckData(AMBA_CAL_AVM_CAM_ID_e CamId, const void *pData, UINT32 SaveLength)
{
    UINT32 Rval = CT_OK;

    char FilePath[128];
    const char LUT_ChId2Ch[4] = {'F', 'B', 'L', 'R'};
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fid;
    AMBA_CT_AVM_SYSTEM_s System;
    UINT8* DstPtr;

    AmbaCT_AvmGetSystem(&System);

    (void) AmbaUtility_StringCopy(FilePath, (INT32)sizeof(FilePath),System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "/");
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)),  System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "_PreCheckData_");
    (void) CT_ChartoStr(FilePath, 128U, CT_sizeT_to_U32(AmbaUtility_StringLength(FilePath)), LUT_ChId2Ch[CamId]);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), ".bin");



    Fid = CT_fopen(FilePath, "wb");

    if ( Fid == NULL ) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FilePath);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        if ( pData != NULL ) {
            DstPtr = AvmPreCheckData[CamId];
            CT_CheckRval(AmbaWrap_memcpy(DstPtr, pData, SaveLength), "AmbaWrap_memcpy", __func__);
            (void) CT_fwrite(DstPtr, SaveLength, 1, Fid);
            (void) CT_fclose(Fid);
        } else {
            (void) CT_fclose(Fid);
            pNextMsg = CT_LogPutStr(Msg, "Invalid PreCheck data address");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        }
    }

    return Rval;
}

UINT8* AmbaCT_GetAvmPreCalDataMisra(void);
UINT8* AmbaCT_GetAvmPreCalDataMisra(void)
{
    UINT8* DstPtr;
    DstPtr = AvmPreCalData[0];
    return DstPtr;
}

UINT8* AmbaCT_GetAvmPreCheckDataMisra(void);
UINT8* AmbaCT_GetAvmPreCheckDataMisra(void)
{
    UINT8* DstPtr;
    DstPtr = AvmPreCheckData[0];
    return DstPtr;
}

static UINT32 AmbaCT_AvmCbSaveCarVout4Points(const AMBA_CAL_POINT_DB_2D_s CarVout4Points[4U])
{
    UINT32 Rval = CT_OK;
    char FilePath[128];
    char LineBuf[1024U];
    AMBA_FS_FILE *Fid;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 Index;
    SIZE_t Length;

    AMBA_CT_AVM_SYSTEM_s System;

    AmbaCT_AvmGetSystem(&System);

    (void) AmbaUtility_StringCopy(FilePath, (INT32)sizeof(FilePath),System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "/");
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)),  System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "_Car_Vout_Corner_4Points.txt");

    Fid = CT_fopen(FilePath, "wb");

    if ( Fid == NULL ) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FilePath);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        for (Index = 0U; Index < 4U; Index++) {
            (void) CT_DBtoStr(LineBuf, 1024U, CarVout4Points[Index].X);
            Length = AmbaUtility_StringLength(LineBuf);
            AmbaUtility_StringAppend(&LineBuf[Length], CT_sizeT_to_U32(1024U - Length), ", ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(1024U - Length), CarVout4Points[Index].Y);
            Length = AmbaUtility_StringLength(LineBuf);
            AmbaUtility_StringAppend(&LineBuf[Length], CT_sizeT_to_U32(1024U - Length), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fid);
        }

        (void) CT_fclose(Fid);
    }

    return Rval;
}

static UINT32 AmbaCT_AvmCbCalibErrorReport(AMBA_CAL_AVM_CAM_ID_e CamId, UINT32 CalibPointNum, const DOUBLE ReProjectError[])
{
    DOUBLE MaxReProjError;
    UINT32 MaxReProjErrorIdx = 0U;
    DOUBLE ReProjErrorSum;
    DOUBLE AvgReProjError;
    UINT32 Idx;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    MaxReProjError = 0.0;
    ReProjErrorSum = 0.0;
    for (Idx = 0U; Idx < CalibPointNum; Idx++) {
        DOUBLE Err = ReProjectError[Idx];
        ReProjErrorSum += Err;
        if (MaxReProjError < Err) {
            MaxReProjError = Err;
            MaxReProjErrorIdx = Idx;
        }
    }
    AvgReProjError = ReProjErrorSum / (DOUBLE)CalibPointNum;

    pNextMsg = CT_LogPutStr(Msg,"Ch(");
    pNextMsg = CT_LogPutU32(pNextMsg, (UINT32)CamId);
    pNextMsg = CT_LogPutStr(pNextMsg, ") Average ReProjError = ");
    pNextMsg = CT_LogPutDB(pNextMsg, AvgReProjError);
    pNextMsg = CT_LogPutStr(pNextMsg, ", Max ReProjError = ");
    pNextMsg = CT_LogPutDB(pNextMsg, MaxReProjError);
    pNextMsg = CT_LogPutStr(pNextMsg, " at Point Index(");
    pNextMsg = CT_LogPutU32(pNextMsg, MaxReProjErrorIdx);
    pNextMsg = CT_LogPutStr(pNextMsg, ")");
    (void) CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);

    return 0;
}

static UINT32 AmbaCT_AvmCbSavePreCalData(AMBA_CAL_AVM_CAM_ID_e CamId, const AMBA_CAL_AVM_PRE_CAL_DATA_s *pData)
{
    UINT32 Rval = CT_OK;
    char FilePath[128];
    const char LUT_ChId2Ch[4] = {'F', 'B', 'L', 'R'};
    AMBA_FS_FILE *Fid;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 PtrIdx;
    UINT8* DstPtr;
    UINT32 DstPtrOffset;

    AMBA_CT_AVM_SYSTEM_s System;

    AmbaCT_AvmGetSystem(&System);

    (void) AmbaUtility_StringCopy(FilePath, (INT32)sizeof(FilePath),System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "/");
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)),  System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "_PreCalData_");
    (void) CT_ChartoStr(FilePath, 128U, CT_sizeT_to_U32(AmbaUtility_StringLength(FilePath)), LUT_ChId2Ch[CamId]);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), ".bin");

    Fid = CT_fopen(FilePath, "wb");

    if ( Fid == NULL ) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FilePath);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        DstPtr = AvmPreCalData[CamId];
        DstPtrOffset = CT_sizeT_to_U32(sizeof(UINT32))*2U;

        for (PtrIdx=0; PtrIdx < pData->NumOfPtr; PtrIdx++) {
            CT_CheckRval(AmbaWrap_memcpy(&DstPtr[DstPtrOffset], &pData->DataSize[PtrIdx], sizeof(UINT32)), "AmbaWrap_memcpy", __func__);
            DstPtrOffset += CT_sizeT_to_U32(sizeof(UINT32));
            CT_CheckRval(AmbaWrap_memcpy(&DstPtr[DstPtrOffset], pData->pDataPtr[PtrIdx], pData->DataSize[PtrIdx]), "AmbaWrap_memcpy", __func__);
            DstPtrOffset += pData->DataSize[PtrIdx];
        }

        CT_CheckRval(AmbaWrap_memcpy(DstPtr, &pData->Version, sizeof(UINT32)), "AmbaWrap_memcpy", __func__);
        CT_CheckRval(AmbaWrap_memcpy(&DstPtr[sizeof(UINT32)], &pData->NumOfPtr, sizeof(UINT32)), "AmbaWrap_memcpy", __func__);
        (void) CT_fwrite(DstPtr, DstPtrOffset, 1, Fid);
        Rval |= CT_fclose(Fid);
    }

    return Rval;
}

static UINT32 AmbaCT_AvmCbFeedPreCalData(AMBA_CAL_AVM_CAM_ID_e CamId, AMBA_CAL_AVM_PRE_CAL_DATA_s *pData)
{
    UINT32 Rval = CT_OK;
    char FilePath[128];
    const char LUT_ChId2Ch[4] = {'F', 'B', 'L', 'R'};
    AMBA_FS_FILE *Fid;
    AMBA_FS_FILE_INFO_s FileInfo;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 PtrIdx;
    AMBA_CT_AVM_SYSTEM_s System;

    CT_CheckRval(CT_memset(FilePath, 0x0, sizeof(FilePath)), "CT_memset", __func__);
    CT_CheckRval(AmbaWrap_memset(&FileInfo, 0x0, sizeof(FileInfo)), "AmbaWrap_memset", __func__);
    AmbaCT_AvmGetSystem(&System);

    (void) AmbaUtility_StringCopy(FilePath, sizeof(FilePath), System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "_PreCalData_");
    (void) CT_ChartoStr(FilePath, 128U, CT_sizeT_to_U32(AmbaUtility_StringLength(FilePath)), LUT_ChId2Ch[CamId]);
    (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), ".bin");
    (void) AmbaFS_GetFileInfo(FilePath, &FileInfo);
    Fid = CT_fopen(FilePath, "rb");

    if ( Fid == NULL ) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FilePath);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else if (FileInfo.Size < (UINT64) AVM_PRE_CAL_BUFF_SIZE) {
        UINT8 *pPreCalDataBuf = AvmPreCalData[CamId];
        UINT32 DataOffset;
        UINT32 MisraCU32 = 0U;

        (void) CT_fread(pPreCalDataBuf, 1U, CT_U64_to_U32(FileInfo.Size), Fid);
        (void) CT_fclose(Fid);

        CT_CheckRval(AmbaWrap_memcpy(&pData->Version, &pPreCalDataBuf[0], sizeof(pData->Version)), "AmbaWrap_memcpy", __func__);
        DataOffset = CT_sizeT_to_U32(sizeof(pData->Version));
        CT_CheckRval(AmbaWrap_memcpy(&pData->NumOfPtr, &pPreCalDataBuf[DataOffset], sizeof(pData->NumOfPtr)), "AmbaWrap_memcpy", __func__);
        DataOffset += CT_sizeT_to_U32(sizeof(pData->NumOfPtr));

        MisraCU32 = pData->NumOfPtr;
        if(MisraCU32 > 16U) {
            MisraCU32 = 16U;
            Rval |= CT_ERR_1;
        }
        for (PtrIdx = 0U; PtrIdx < MisraCU32; PtrIdx++) {
            if(DataOffset >AVM_PRE_CAL_BUFF_SIZE) {
                Rval |= CT_ERR_1;
                break;
            }
            CT_CheckRval(AmbaWrap_memcpy(&pData->DataSize[PtrIdx], &pPreCalDataBuf[DataOffset], sizeof(pData->DataSize[PtrIdx])), "AmbaWrap_memcpy", __func__);
            DataOffset += CT_sizeT_to_U32(sizeof(pData->DataSize[PtrIdx]));
            pData->pDataPtr[PtrIdx] = &pPreCalDataBuf[DataOffset];
            DataOffset += pData->DataSize[PtrIdx];
        }

    } else {
        // TODO # Print ERR Msg
        (void) CT_fclose(Fid);
    }

    return Rval;
}

static UINT32 AmbaCT_AvmCbGetPreCheckData(const char *pFileName, void *pData, UINT32 DataLength)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fid;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;

    Fid = CT_fopen(pFileName, "rb");

    if (Fid == NULL ) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, pFileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fread(pData, 1, DataLength, Fid);
        (void) CT_fclose(Fid);
    }
    return Rval;
}

static UINT32 AmbaCT_Avm2dCbFeedCustomCalibMat(AMBA_CAL_AVM_CAM_ID_e CamId, DOUBLE *pMat)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_AVM_2D_CUSTOM_CALIB_s CustomCalibSetting;

    if ( pMat != NULL ) {
        AmbaCT_AvmGet2dCustomCalib((UINT32)CamId, &CustomCalibSetting);
        Rval |= AmbaWrap_memcpy(pMat, CustomCalibSetting.CalibMatrix, sizeof(DOUBLE)*9U);
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }
    } else {
        Rval = CT_ERR_1;
    }

    return Rval;
}

static UINT32 AmbaCT_AvmCbFeedOsdMaskFromFile(UINT32 Width, UINT32 Height, const char *TablePath, UINT8 *pTable)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;

    Fd = CT_fopen(TablePath, "rb");

    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, TablePath);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fread(pTable, Width * Height, CT_sizeT_to_U32(sizeof(UINT8)), Fd);
        (void) CT_fclose(Fd);
    }

    return Rval;
}

static UINT32 AmbaCT_EmCbMsgReciver(AMBA_CAL_EM_MSG_TYPE_e Type, AMBA_CAL_EM_CAM_ID_e CamId, const AMBA_CAL_EM_MSG_s *pMsg)
{
    UINT32 Rval = CT_OK;
    switch (Type) {
    case AMBA_EM_MSG_GRID_STATUS:
    {
        UINT32 *pStatusTbl = EmrGridStatus[CamId].GridStatus;
        const AMBA_CAL_EM_WARP_TBL_STATUS_s *pStatus;
        pStatus = pMsg->pGridStatus;
        if (((SIZE_t)pStatus->HorGridNum * (SIZE_t)pStatus->VerGridNum) > MAX_WARP_TBL_LEN) {
            Rval = CT_ERR_1;
        } else {
            EmrGridStatus[CamId].HorTileNum = pStatus->HorGridNum;
            EmrGridStatus[CamId].VerTileNum = pStatus->VerGridNum;
            pStatusTbl[pStatus->GridPosX + (pStatus->GridPosY * pStatus->HorGridNum)] = pStatus->Status;
        }
        break;
    }
    case AMBA_EM_MSG_PTN_ERR:
    case AMBA_EM_MSG_REPORT:
        break;
    default:
        Rval = CT_ERR_1;
        break;
    }
    return Rval;
}

static UINT32 AmbaCT_EmSvCbMsgReciver(AMBA_CAL_EM_MSG_TYPE_e Type, const AMBA_CAL_EM_MSG_s *pMsg)
{
    return AmbaCT_EmCbMsgReciver(Type, AMBA_CAL_EM_CAM_BACK, pMsg);
}

static UINT32 AmbaCT_EmrGridStatus2Text(const char *pFileName, AMBA_CAL_EM_CAM_ID_e CamId)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fid;
    char LineBuf[1024];
    if ((EmrGridStatus[CamId].VerTileNum * EmrGridStatus[CamId].HorTileNum) != 0U) {
        Fid = CT_fopen(pFileName, "w");
        if (Fid == NULL) {
            Rval |= CT_ERR_1;
        } else {
            UINT32 HorIdx, VerIdx;
            for (VerIdx = 0U; VerIdx < EmrGridStatus[CamId].VerTileNum; VerIdx++) {
                for (HorIdx = 0U; HorIdx < EmrGridStatus[CamId].HorTileNum; HorIdx++) {
                    if (HorIdx == (EmrGridStatus[CamId].HorTileNum - 1U)) {
                        (void) CT_U32toHexStr(LineBuf, 1024U, EmrGridStatus[CamId].GridStatus[HorIdx + (VerIdx * EmrGridStatus[CamId].HorTileNum)]);
                    } else {
                        (void) CT_U32toHexStr(LineBuf, 1024U, EmrGridStatus[CamId].GridStatus[HorIdx + (VerIdx * EmrGridStatus[CamId].HorTileNum)]);
                        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    }
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fid);
                }
                AmbaUtility_StringCopy(LineBuf, 2,"\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fid);
            }
            (void) CT_fclose(Fid);
            pNextMsg = CT_LogPutStr(Msg, "Status table(");
            pNextMsg = CT_LogPutU8(pNextMsg, (UINT8)CamId);
            pNextMsg = CT_LogPutStr(pNextMsg, ") output succeed");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_DEBUG, Msg, pNextMsg);
        }
    }
    return Rval;
}

static UINT32 AmbaCT_BpcFeedRaw(const char *pFileName, UINT16 *pRawBuf, SIZE_t RawBufSize)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE_INFO_s FileInfo;
    CT_CheckRval(AmbaWrap_memset(&FileInfo, 0x0, sizeof(AMBA_FS_FILE_INFO_s)), "AmbaWrap_memset", __func__);

    (void) AmbaFS_GetFileInfo(pFileName, &FileInfo);

    if (FileInfo.Size > RawBufSize) {
        AmbaPrint_PrintInt5("AmbaCT_BpcFeedRaw Error: Raw buffer size %d < Raw file size %d", (INT32)RawBufSize, (INT32)FileInfo.Size, 0, 0, 0);
        Rval |= CT_ERR_1;
    } else {
        AMBA_FS_FILE *Fid;
        Fid = CT_fopen(pFileName, "rb");
        if (Fid == NULL) {
            Rval |= CT_ERR_1;
        } else {
            (void) CT_fread(pRawBuf, 1U, (UINT32) FileInfo.Size, Fid);
            (void) CT_fclose(Fid);
        }
    }
    return Rval;
}

static UINT32 AmbaCT_OcCbFeedSrc(SIZE_t SrcBufSize, UINT8 *pSrc)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_OCCB_TUNER_SRC_s SrcInfo;
    SIZE_t NecessarySrcBufSize;
    AmbaCT_OcCbGetSrcInfo(&SrcInfo);
    NecessarySrcBufSize = (SIZE_t)SrcInfo.ImgSize.Width * (SIZE_t)SrcInfo.ImgSize.Height;
    if (NecessarySrcBufSize > SrcBufSize) {
        Rval = CT_ERR_1;
    } else {
        AMBA_FS_FILE *Fid;
        Fid = CT_fopen(SrcInfo.FilePath, "rb");
        if (Fid == NULL) {
            Rval |= CT_ERR_1;
        } else {
            (void) CT_fread(pSrc, 1U, (UINT32) NecessarySrcBufSize, Fid);
            (void) CT_fclose(Fid);
        }
    }
    return Rval;
}
static UINT32 AmbaCT_OcCbMarkCorner(const char *pOutputPrefix, UINT8 *pSrcImg, const AMBA_CAL_SIZE_s *pImgSize, UINT32 CornerNum, const AMBA_CAL_POINT_DB_2D_s *pSubPixCorners, const INT16 *pCorner2dMap)
{
    UINT32 Rval = CT_OK;
    AMBA_CAL_POINT_INT_2D_s CornerPos;
    INT32 CornerIdx;
    UINT32 i, J;
    AMBA_FS_FILE *Fd;
    INT16 CurrentIdx, RightIdx, BottomIdx;
    const AMBA_CAL_POINT_DB_2D_s *pCurrentCorner, *pRightCorner, *pBottomCorner;
    DOUBLE m;
    INT32 XIdx, YIdx;
    DOUBLE MisraDBTmp;
    char FileName[CT_OCCB_MAX_FILE_PATH] = "";
    (void) AmbaUtility_StringCopy(FileName,  CT_OCCB_MAX_FILE_PATH, pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_Debug.y");
    for (CornerIdx = ((INT32)CornerNum - 1); CornerIdx >= 0; CornerIdx--) {
        CornerPos.X = (INT32) pSubPixCorners[CornerIdx].X;
        CornerPos.Y = (INT32) pSubPixCorners[CornerIdx].Y;

        pSrcImg[((INT32)pImgSize->Width * CornerPos.Y) + CornerPos.X] = 255;
        if (CornerPos.X != 0) {
            pSrcImg[((INT32)pImgSize->Width * CornerPos.Y) + CornerPos.X - 1] = 0;
        }
        if (CornerPos.X != ((INT32)pImgSize->Width - 1)) {
            pSrcImg[((INT32)pImgSize->Width * CornerPos.Y) + CornerPos.X + 1] = 0;
        }
        if (CornerPos.Y != 0) {
            pSrcImg[((INT32)pImgSize->Width * (CornerPos.Y - 1)) + CornerPos.X] = 0;
        }
        if (CornerPos.Y != ((INT32)pImgSize->Height -1)) {
            pSrcImg[((INT32)pImgSize->Width * (CornerPos.Y + 1)) + CornerPos.X] = 0;
        }
    }

    for (J = 1U; J < (CORNERS_2D_MAP_RADIUS * 2U); J++) {
        for (i = 1U; i < (CORNERS_2D_MAP_RADIUS * 2U); i++) {
            CurrentIdx = pCorner2dMap[(J * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i];
            RightIdx = pCorner2dMap[(J * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i + 1U];
            BottomIdx = pCorner2dMap[((J + 1U) * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i];

            if ((CurrentIdx >= 0) && (RightIdx >= 0)) {
                pCurrentCorner = &pSubPixCorners[CurrentIdx];
                pRightCorner = &pSubPixCorners[RightIdx];
                m = (pRightCorner->Y - pCurrentCorner->Y) / (pRightCorner->X - pCurrentCorner->X);
                for (XIdx = (INT32)pCurrentCorner->X; XIdx < (INT32)pRightCorner->X; XIdx++) {
                    MisraDBTmp = (m * ((DOUBLE)XIdx - pCurrentCorner->X)) + pCurrentCorner->Y;
                    YIdx = (INT32)MisraDBTmp;
                    pSrcImg[((INT32)pImgSize->Width * YIdx) + XIdx] = 255U;
                }
            }
            if ((CurrentIdx >= 0) && (BottomIdx >= 0)) {
                pCurrentCorner = &pSubPixCorners[CurrentIdx];
                pBottomCorner = &pSubPixCorners[BottomIdx];
                m = (pBottomCorner->Y - pCurrentCorner->Y) / (pBottomCorner->X - pCurrentCorner->X);
                for (YIdx = (INT32)pCurrentCorner->Y; YIdx < (INT32)pBottomCorner->Y; YIdx++) {
                    MisraDBTmp = (((DOUBLE)YIdx - pCurrentCorner->Y) / m) + pCurrentCorner->X;
                    XIdx = (INT32)MisraDBTmp;
                    pSrcImg[((INT32)pImgSize->Width*YIdx) + XIdx] = 255U;
                }
            }
        }
    }

    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        (void) CT_fwrite(pSrcImg, pImgSize->Width * pImgSize->Height, 1, Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;
}
static UINT32 AmbaCT_OcFeedRaw(SIZE_t RawBufSize, UINT16 *pRaw)
{
    UINT32 Rval = CAL_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_CT_OC_TUNER_RAW_INFO_s RawInfo;

    AmbaCT_OcGetRawInfo(&RawInfo);
    if (((SIZE_t)RawInfo.Area.Width * (SIZE_t)RawInfo.Area.Height * sizeof(UINT16)) > RawBufSize) {
        pNextMsg = CT_LogPutStr(Msg, "RawBufSize");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(RawBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, "is tool small, Raw size: 2 *");
        pNextMsg = CT_LogPutU32(pNextMsg, RawInfo.Area.Width);
        pNextMsg = CT_LogPutU32(pNextMsg, RawInfo.Area.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_DEBUG, Msg, pNextMsg);
        Rval |= CAL_ERR_1;
    } else {
        if (RawInfo.FromFile != 0U) {
            AMBA_FS_FILE *Fd;
            char FileName[64];
            AmbaUtility_StringCopy(FileName, 64,RawInfo.Path);
            Fd = CT_fopen(FileName, "rb");
            if (Fd == NULL) {
                pNextMsg = CT_LogPutStr(Msg, "Open");
                pNextMsg = CT_LogPutStr(pNextMsg, FileName);
                pNextMsg = CT_LogPutStr(pNextMsg, "Fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
                Rval |= CAL_ERR_1;
            } else {
                (void)CT_fread(pRaw, RawInfo.Area.Width * RawInfo.Area.Height, CT_sizeT_to_U32(sizeof(UINT16)), Fd);
                Rval |= CT_fclose(Fd);
            }
        } else {
            pNextMsg = CT_LogPutStr(Msg, "Not support yet!");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CAL_ERR_1;
        }
    }
    return Rval;
}

static UINT32 CT_VigFeedRawFuncV1(UINT32 CamId, void *pRawBuf, SIZE_t RawBufSize)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fd;
    AMBA_FS_FILE_INFO_s FileInfo;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_CT_1D_VIG_TUNER_RAW_INFO_s RawInfo;
    AMBA_CT_1D_VIG_TUNER_VIGNETTE_s VigData;
    AmbaCT_1DVigGetRawInfo(CamId, &RawInfo);
    AmbaCT_1DVigGetVignette(CamId, &VigData);
    CT_CheckRval(AmbaWrap_memset(&FileInfo, 0x0, sizeof(FileInfo)), "AmbaWrap_memset", __func__);

    Fd = CT_fopen(RawInfo.Path, "rb");
    if (Fd == NULL) {
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, RawInfo.Path);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        (void) AmbaFS_GetFileInfo(RawInfo.Path, &FileInfo);
        if (FileInfo.Size > RawBufSize) {
            Rval |= CT_ERR_1;
        } else {
            (void) CT_fread(pRawBuf, (UINT32)FileInfo.Size, CT_sizeT_to_U32(sizeof(UINT8)), Fd);
        }
        Rval |= CT_fclose(Fd);
    }
    return Rval;
}

static UINT32 CT_VigFeedGainTbl(UINT32 CamId, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;
    AMBA_CT_1D_VIG_EVA_CALIB_DATA_s CalibData;
    AmbaCT_1DVigGetEvaluationCalibData(CamId, &CalibData);
    Fd = CT_fopen(CalibData.Path, "rb");
    if (Fd == NULL) {
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, CalibData.Path);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        (void) CT_fread(&pCalibData->VigGainTbl_R[0][0], CT_sizeT_to_U32(sizeof(pCalibData->VigGainTbl_R)), 1, Fd);
        (void) CT_fread(&pCalibData->VigGainTbl_Gr[0][0], CT_sizeT_to_U32(sizeof(pCalibData->VigGainTbl_Gr)), 1, Fd);
        (void) CT_fread(&pCalibData->VigGainTbl_B[0][0], CT_sizeT_to_U32(sizeof(pCalibData->VigGainTbl_B)), 1, Fd);
        (void) CT_fread(&pCalibData->VigGainTbl_Gb[0][0], CT_sizeT_to_U32(sizeof(pCalibData->VigGainTbl_Gb)), 1, Fd);
        Rval |= CT_fclose(Fd);
    }
    return Rval;
}

static UINT32 AmbaCT_VigCbFeedRawBufFunc(SIZE_t RawBufSize, UINT32 *pPitch, AMBA_CAL_ROI_s *pValidArea, UINT16 *pRaw)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 ChanIdx = AmbaCT_1DVigGetActiveChanIdx();
    AMBA_CT_1D_VIG_TUNER_RAW_INFO_s RawInfo;
    AMBA_CT_1D_VIG_TUNER_VIGNETTE_s Vignette;
    AmbaCT_1DVigGetRawInfo(ChanIdx, &RawInfo);
    if (((SIZE_t)RawInfo.Area.Width * (SIZE_t)RawInfo.Area.Height * sizeof(UINT16)) > RawBufSize) {
        pNextMsg = CT_LogPutStr(Msg, "RawBufSize");
        pNextMsg = CT_LogPutU32(pNextMsg, CT_sizeT_to_U32(RawBufSize));
        pNextMsg = CT_LogPutStr(pNextMsg, "is too small, Raw size:");
        pNextMsg = CT_LogPutU32(pNextMsg, RawInfo.Area.Width);
        pNextMsg = CT_LogPutStr(pNextMsg, "*");
        pNextMsg = CT_LogPutU32(pNextMsg, RawInfo.Area.Height);
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        Rval |= CT_ERR_1;
    } else {
        if (RawInfo.FromFile != 0U) {
            AMBA_FS_FILE *Fd;
            char FileName[64];
            *pPitch = RawInfo.Pitch;
            pValidArea->StartX= RawInfo.Area.StartX;
            pValidArea->StartY= RawInfo.Area.StartY;
            pValidArea->Width = RawInfo.Area.Width;
            pValidArea->Height= RawInfo.Area.Height;
            AmbaCT_1DVigGetVignette(ChanIdx, &Vignette);
            if (Vignette.RawNum == 1U) {
                AmbaUtility_StringCopy(FileName, sizeof(FileName), RawInfo.Path);
            } else {
                char RawIdxStrMisra[16];
                const char *pExtStrMisra = ".raw";
                (void)CT_U32toStr(RawIdxStrMisra, 16, ChanIdx);
                AmbaUtility_StringCopy(FileName, sizeof(FileName), RawInfo.Path);
                AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), RawIdxStrMisra);
                AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pExtStrMisra);
            }
            Fd = CT_fopen(FileName, "rb");
            if (Fd == NULL) {
                pNextMsg = CT_LogPutStr(Msg, "Open");
                pNextMsg = CT_LogPutStr(pNextMsg, FileName);
                pNextMsg = CT_LogPutStr(pNextMsg, "fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
                Rval |= CT_ERR_1;
            } else {
                (void)CT_fread(pRaw, AMBA_CT_1D_VIG_MAX_RAW_LEN, CT_sizeT_to_U32(sizeof(UINT16)), Fd);
                Rval |= CT_fclose(Fd);
            }
        } else {
            pNextMsg = CT_LogPutStr(Msg, "Not support yet!");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            Rval |= CT_ERR_1;
        }
    }
    return Rval;
}

static inline UINT32 AmbaCT_InitCfg21DVigInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_1D_VIG_INIT_CFG_s *pVigInitCfg)
{
    UINT32 Rval = CT_OK;
    UINT8 *pSrcRawBuf;
    SIZE_t Size;
    AmbaCT_1DVigGetWorkingBufSize(&Size);
    pVigInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    if (pInitCfg->TunerWorkingBufSize < Size) {
        AmbaPrint_PrintStr5("Vig tuner work buf size incorrect", NULL, NULL, NULL, NULL, NULL);
        Rval = CT_ERR_1;
    } else {
        pVigInitCfg->WorkingBufSize = Size;
        pVigInitCfg->SrcRawBufSize = pInitCfg->TunerWorkingBufSize - Size;
    }

    pVigInitCfg->pCalibWorkingBuf = pInitCfg->Vig1d.pCalibWorkingBuf;
    pVigInitCfg->CalibWorkingBufSize = pInitCfg->Vig1d.CalibWorkingBufSize;

    Rval |= AmbaWrap_memcpy(&pSrcRawBuf, &pInitCfg->pTunerWorkingBuf, sizeof(pSrcRawBuf));
    if(Rval != CT_OK) {
        Rval = CT_ERR_1;
    }
    pSrcRawBuf = &pSrcRawBuf[Size];
    pVigInitCfg->pSrcRawBuf = pSrcRawBuf;

    // FIXME # FeedRawFunc should be remove
    if (pInitCfg->Vig.FeedRawFunc == NULL){
        pVigInitCfg->CbFeedRawBufFunc = AmbaCT_VigCbFeedRawBufFunc;
    } else {
        pVigInitCfg->CbFeedRawBufFunc = pInitCfg->Vig.FeedRawFunc;
    }
    pVigInitCfg->pFeedRawFuncV1 = CT_VigFeedRawFuncV1;
    pVigInitCfg->pFeedGainTbl = CT_VigFeedGainTbl;
    return Rval;
}
static inline void AmbaCT_InitCfg2AvmInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_AVM_INIT_CFG_s *pAvmInitCfg)
{
    pAvmInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pAvmInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pAvmInitCfg->CbMsgReciver = AmbaCT_AvmCbMsgReciver;
    pAvmInitCfg->CbFeedPreCalData = AmbaCT_AvmCbFeedPreCalData;
    pAvmInitCfg->CbGetPreCheckData = AmbaCT_AvmCbGetPreCheckData;
    pAvmInitCfg->CbSavePreCalData = AmbaCT_AvmCbSavePreCalData;
    pAvmInitCfg->CbSavePreCheckData = AmbaCT_AvmCbSavePreCheckData;
    pAvmInitCfg->Cb2DCustomCalibCalib = AmbaCT_Avm2dCbFeedCustomCalibMat;
    pAvmInitCfg->CbFeedOSDMaskFunc = AmbaCT_AvmCbFeedOsdMaskFromFile;
    pAvmInitCfg->pCalibWorkingBuf = pInitCfg->Avm.pCalibWorkingBuf;
    pAvmInitCfg->CalibWorkingBufSize = pInitCfg->Avm.CalibWorkingBufSize;
    pAvmInitCfg->CbSaveCarVout4Points = AmbaCT_AvmCbSaveCarVout4Points;
    pAvmInitCfg->CbCalibErrorReport = AmbaCT_AvmCbCalibErrorReport;
}

static inline void AmbaCT_InitCfg2EmInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_EM_INIT_CFG_s *pEmInitCfg)
{
    pEmInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pEmInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pEmInitCfg->pCalibWorkingBuf = pInitCfg->Emirror.pCalibWorkingBuf;
    pEmInitCfg->CalibWorkingBufSize = pInitCfg->Emirror.CalibWorkingBufSize;
    pEmInitCfg->CbMsgReciver = AmbaCT_EmCbMsgReciver;
    pEmInitCfg->SvCbMsgReciver = AmbaCT_EmSvCbMsgReciver;
    pEmInitCfg->CbFeedMaskFunc = pInitCfg->Emirror.CbFeedMaskFunc;
}

static inline void AmbaCT_InitCfg2LdcInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_LDC_INIT_CFG_s *pLdcInitCfg)
{
    pLdcInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pLdcInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pLdcInitCfg->pCalibWorkingBuf= pInitCfg->Ldc.pCalibWorkingBuf;
    pLdcInitCfg->CalibWorkingBufSize= pInitCfg->Ldc.CalibWorkingBufSize;
}

static inline void AmbaCT_InitCfg2CaInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_CA_INIT_CFG_s *pCaInitCfg)
{
    pCaInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pCaInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pCaInitCfg->pCalibWorkingBuf= pInitCfg->Ca.pCalibWorkingBuf;
    pCaInitCfg->CalibWorkingBufSize= pInitCfg->Ca.CalibWorkingBufSize;
}

static UINT32 CT_LdccFeedSinglePlaneSrc(const AMBA_CAL_SIZE_s *pImgSize, const AMBA_CAL_YUV_INFO_s *pYUVInfo)
{
    UINT32 Rval = CT_OK;
    const AMBA_CT_LDCC_USER_SETTING_s *pLdccUsetSetting;
    AMBA_FS_FILE *Fid;
    AMBA_FS_FILE_INFO_s FileInfo;
    UINT32 ReadSize;
    CT_CheckRval(AmbaWrap_memset(&FileInfo, 0x0, sizeof(FileInfo)), "AmbaWrap_memset", __func__);
    AmbaCT_TouchUnused(pImgSize);
    pLdccUsetSetting =  AmbaCT_LdccGetUserSetting();
    if (pYUVInfo->pY != NULL) {
        Fid = CT_fopen(pLdccUsetSetting->SrcInfo.SinglePlanePatternYFilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void)AmbaFS_GetFileInfo(pLdccUsetSetting->SrcInfo.SinglePlanePatternYFilePath, &FileInfo);
            ReadSize = (UINT32)FileInfo.Size;
            (void) CT_fread(pYUVInfo->pY, ReadSize, 1U, Fid);
            (void) CT_fclose(Fid);
        }
    }
    if (pYUVInfo->pUV != NULL) {
        Fid = CT_fopen(pLdccUsetSetting->SrcInfo.SinglePlanePatternUVFilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void)AmbaFS_GetFileInfo(pLdccUsetSetting->SrcInfo.SinglePlanePatternUVFilePath, &FileInfo);
            ReadSize = (UINT32)FileInfo.Size;
            (void) CT_fread(pYUVInfo->pUV, ReadSize, 1U, Fid);
            (void) CT_fclose(Fid);
        }
    }
    return Rval;
}

static UINT32 CT_LdccFeedMultiPlaneSrc(const AMBA_CAL_SIZE_s *pImgSize, const AMBA_CAL_YUV_INFO_s *pYUVInfo)
{
    UINT32 Rval = CT_OK;
    const AMBA_CT_LDCC_USER_SETTING_s *pLdccUsetSetting;
    AMBA_FS_FILE *Fid;
    AMBA_FS_FILE_INFO_s FileInfo;
    UINT32 ReadSize;
    CT_CheckRval(AmbaWrap_memset(&FileInfo, 0x0, sizeof(FileInfo)), "AmbaWrap_memset", __func__);
    AmbaCT_TouchUnused(pImgSize);
    pLdccUsetSetting =  AmbaCT_LdccGetUserSetting();
    if (pYUVInfo->pY != NULL) {
        Fid = CT_fopen(pLdccUsetSetting->SrcInfo.MultiPlanePatternYFilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void)AmbaFS_GetFileInfo(pLdccUsetSetting->SrcInfo.MultiPlanePatternYFilePath, &FileInfo);
            ReadSize = (UINT32)FileInfo.Size;
            (void) CT_fread(pYUVInfo->pY, ReadSize, 1U, Fid);
            (void) CT_fclose(Fid);
        }
    }
    if (pYUVInfo->pUV != NULL) {
        Fid = CT_fopen(pLdccUsetSetting->SrcInfo.MultiPlanePatternUVFilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void)AmbaFS_GetFileInfo(pLdccUsetSetting->SrcInfo.MultiPlanePatternUVFilePath, &FileInfo);
            ReadSize = (UINT32)FileInfo.Size;
            (void) CT_fread(pYUVInfo->pUV, ReadSize, 1U, Fid);
            (void) CT_fclose(Fid);
        }
    }
    return Rval;
}

static UINT32 CT_LdccDrawMultiPlaneDetResult(const AMBA_CAL_DET_CORNER_LIST_s *pCornerList, UINT8 *pYImg) {
    UINT32 Rval = CT_OK;
    const AMBA_CT_LDCC_USER_SETTING_s *pLdccUserSetting;
    char FilePath[128];
    AMBA_FS_FILE *Fid;
    UINT32 Index, MisraX, MisraY;
    UINT8 Color;
    pLdccUserSetting = AmbaCT_LdccGetUserSetting();
    if (pYImg != NULL) {
        Fid = CT_fopen(pLdccUserSetting->SrcInfo.MultiPlanePatternYFilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void) CT_fread(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
            (void) CT_fclose(Fid);
        }
        for (Index = 0U; Index < pCornerList->CornerNum; Index++) {
            MisraX = (UINT32) pCornerList->pCornerPosList[Index].X;
            MisraY = (UINT32) pCornerList->pCornerPosList[Index].Y;
            Color = (pCornerList->pCornersTypeList[Index].Type == AMBA_CAL_DET_PTN_TYPE_BLACK) ? 255U : 0U;
            pYImg[MisraX + (MisraY * pLdccUserSetting->SrcInfo.ImageSize.Width)] = Color;
        }

        (void) AmbaUtility_StringCopy(FilePath, (INT32)sizeof(FilePath),pLdccUserSetting->System.OutputPrefix);
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "/");
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)),  pLdccUserSetting->System.OutputPrefix);
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "_MultiPlaneDetDebugResult.y");

        Fid = CT_fopen(FilePath, "wb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void) CT_fwrite(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
            (void) CT_fclose(Fid);
        }

    }
    return Rval;
}

static UINT32 CT_LdccDrawMultiPlaneDetResultByPlane(const AMBA_CAL_DET_CORNER_LIST_s *pCornersList0,
        const AMBA_CAL_DET_CORNER_LIST_s *pCornersList1,
        const AMBA_CAL_DET_CORNER_LIST_s *pCornersList2,
        UINT8 *pYImg) {
    UINT32 Rval = CT_OK;
    const AMBA_CT_LDCC_USER_SETTING_s *pLdccUserSetting;
    AMBA_FS_FILE *Fid;
    UINT32 Index, PlaneIdx, MisraX, MisraY;
    const AMBA_CAL_DET_CORNER_LIST_s *pCornersList[3];
    const char * const pOutFileName[3] = { "_MultiPlaneDetDebugResult0.y", "_MultiPlaneDetDebugResult1.y", "_MultiPlaneDetDebugResult2.y" };
    char FilePath[128];
    UINT8 Color;
    pCornersList[0] = pCornersList0;
    pCornersList[1] = pCornersList1;
    pCornersList[2] = pCornersList2;

    pLdccUserSetting = AmbaCT_LdccGetUserSetting();
    if (pYImg != NULL) {
        for (PlaneIdx = 0U; PlaneIdx < 3U; PlaneIdx++) {
            Fid = CT_fopen(pLdccUserSetting->SrcInfo.MultiPlanePatternYFilePath, "rb");
            if (Fid == NULL) {
                Rval = CT_ERR_1;
            } else {
                (void) CT_fread(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
                (void) CT_fclose(Fid);
            }
            for (Index = 0U; Index < pCornersList[PlaneIdx]->CornerNum; Index++) {
                MisraX = (UINT32) pCornersList[PlaneIdx]->pCornerPosList[Index].X;
                MisraY = (UINT32) pCornersList[PlaneIdx]->pCornerPosList[Index].Y;
                Color = (pCornersList[PlaneIdx]->pCornersTypeList[Index].Type == AMBA_CAL_DET_PTN_TYPE_BLACK) ? 255U : 0U;
                pYImg[MisraX + (MisraY * pLdccUserSetting->SrcInfo.ImageSize.Width)] = Color;
            }
            (void) AmbaUtility_StringCopy(FilePath, (INT32)sizeof(FilePath),pLdccUserSetting->System.OutputPrefix);
            (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "/");
            (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)),  pLdccUserSetting->System.OutputPrefix);
            (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), pOutFileName[PlaneIdx]);
            Fid = CT_fopen(FilePath, "wb");

            if (Fid == NULL) {
                Rval = CT_ERR_1;
            } else {
                (void) CT_fwrite(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
                (void) CT_fclose(Fid);
            }
        }
    }
    return Rval;
}

static UINT32 CT_LdccFeedMultiPlanePatInfo(UINT32 PlaneIdx, AMBA_CAL_OCCB_CIRLCE_INFO_s *pPatInfo) {
    UINT32 Rval = CT_OK;
    const AMBA_CT_LDCC_USER_SETTING_s *pLdccUserSetting;
    AMBA_FS_FILE *Fid;
    pLdccUserSetting = AmbaCT_LdccGetUserSetting();

    if (pPatInfo != NULL) {
        Fid = CT_fopen(pLdccUserSetting->PatternLayout[PlaneIdx].FilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            SIZE_t Size = (SIZE_t)pLdccUserSetting->PatternLayout[PlaneIdx].LayoutCfg.ColNum * (SIZE_t)pLdccUserSetting->PatternLayout[PlaneIdx].LayoutCfg.RowNum
                    * sizeof(AMBA_CAL_OCCB_CIRLCE_INFO_s);
            (void) CT_fread(pPatInfo, CT_sizeT_to_U32(Size), 1U, Fid);
            (void) CT_fclose(Fid);
        }
#if 0
        // Note # Dump Pat Layout
        printf("pPatInfo: %p\n", pPatInfo);
        for (UINT32 i = 0; i < 50; i++) {
            for (UINT32 j = 0; j < 50; j++) {
                printf("%c",
                        (pPatInfo[i+j * 50].Info.Bits.IsExist == 0) ? '-' : (
                                (pPatInfo[i+j * 50].Info.Bits.Color == 0) ? 'B' : 'R'));
            }
            printf("\n");
        }
#endif
    } else {
        Rval = CT_ERR_1;
    }
    return Rval;
}


static inline void CT_LdccDrawLine(const AMBA_CAL_POINT_DB_2D_s *pCornerA, const AMBA_CAL_POINT_DB_2D_s *pCornerB, UINT32 Width, UINT8 *pYImg) {
    DOUBLE m, MisraDBTmp;
    DOUBLE CornerDiffX, CornerDiffY;
    DOUBLE AbsDiffX, AbsDiffY;
    INT32 XIdx, YIdx;
    // Note # Check which one is longer
    CornerDiffX = pCornerB->X - pCornerA->X;
    CornerDiffY = pCornerB->Y - pCornerA->Y;
    CT_CheckRval(AmbaWrap_fabs(CornerDiffX, &AbsDiffX), "AmbaWrap_fabs", __func__);
    CT_CheckRval(AmbaWrap_fabs(CornerDiffY, &AbsDiffY), "AmbaWrap_fabs", __func__);
    if (AbsDiffY > AbsDiffX) {
        m = CornerDiffX / CornerDiffY;
        if (CornerDiffY > 0.0) {
            for (YIdx = (INT32) pCornerA->Y; YIdx < (INT32) pCornerB->Y; YIdx++) {
                MisraDBTmp = (m * ((DOUBLE) YIdx - pCornerA->Y)) + pCornerA->X;
                XIdx = (INT32) MisraDBTmp;
                pYImg[((INT32) Width * YIdx) + XIdx] = 255U;
            }
        } else {
            for (YIdx = (INT32) pCornerA->Y; YIdx > (INT32) pCornerB->Y; YIdx--) {
                MisraDBTmp = (m * ((DOUBLE) YIdx - pCornerA->Y)) + pCornerA->X;
                XIdx = (INT32) MisraDBTmp;
                pYImg[((INT32) Width * YIdx) + XIdx] = 255U;
            }
        }
    } else {
        m = CornerDiffY / CornerDiffX;
        if (CornerDiffX > 0.0) {
            for (XIdx = (INT32) pCornerA->X; XIdx < (INT32) pCornerB->X; XIdx++) {
                MisraDBTmp = (m * ((DOUBLE) XIdx - pCornerA->X)) + pCornerA->Y;
                YIdx = (INT32) MisraDBTmp;
                pYImg[((INT32) Width * YIdx) + XIdx] = 255U;
            }
        } else {
            for (XIdx = (INT32) pCornerA->X; XIdx > (INT32) pCornerB->X; XIdx--) {
                MisraDBTmp = (m * ((DOUBLE) XIdx - pCornerA->X)) + pCornerA->Y;
                YIdx = (INT32) MisraDBTmp;
                pYImg[((INT32) Width * YIdx) + XIdx] = 255U;
            }
        }
    }
}

static UINT32 CT_LdccDrawMultiPlaneOrgResult(UINT32 PlaneIdx, const AMBA_CAL_POINT_DB_2D_s *pCorners, const AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners, UINT8 *pYImg) {
    UINT32 Rval = CT_OK;
    const AMBA_CT_LDCC_USER_SETTING_s *pLdccUserSetting;
    AMBA_FS_FILE *Fid;
    UINT32 i, J;
    INT32 CurrentIdx, RightIdx, BottomIdx;
    char FilePath[128];
    const char * const pOutFileName[3] = { "_MultiPlaneOrgDebugResult0.y", "_MultiPlaneOrgDebugResult1.y", "_MultiPlaneOrgDebugResult2.y", };
    pLdccUserSetting = AmbaCT_LdccGetUserSetting();
    if ((pYImg != NULL) && (PlaneIdx < 3U)) {
        Fid = CT_fopen(pLdccUserSetting->SrcInfo.MultiPlanePatternYFilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void) CT_fread(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
            (void) CT_fclose(Fid);
        }

        for (J = 1U; J < ((CORNERS_2D_MAP_RADIUS * 2U) + 1U); J++) {
            for (i = 1U; i < ((CORNERS_2D_MAP_RADIUS * 2U) + 1U); i++) {
                CurrentIdx = pOrganizedCorners->pCorner2dMap[(J * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i];
                RightIdx = pOrganizedCorners->pCorner2dMap[(J * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i + 1U];
                BottomIdx = pOrganizedCorners->pCorner2dMap[((J + 1U) * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i];
                if ((CurrentIdx >= 0) && (RightIdx >= 0)) {
                    CT_LdccDrawLine(&pCorners[CurrentIdx], &pCorners[RightIdx], pLdccUserSetting->SrcInfo.ImageSize.Width, pYImg);
                }
                if ((CurrentIdx >= 0) && (BottomIdx >= 0)) {
                    CT_LdccDrawLine(&pCorners[BottomIdx], &pCorners[CurrentIdx], pLdccUserSetting->SrcInfo.ImageSize.Width, pYImg);
                }
            }
        }


        (void) AmbaUtility_StringCopy(FilePath, (INT32)sizeof(FilePath),pLdccUserSetting->System.OutputPrefix);
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "/");
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)),  pLdccUserSetting->System.OutputPrefix);
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), pOutFileName[PlaneIdx]);
        Fid = CT_fopen(FilePath, "wb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void) CT_fwrite(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
            (void) CT_fclose(Fid);
        }
    }
    return Rval;
}

static UINT32 CT_LdccDrawSinglePlaneOrgResult(const AMBA_CAL_POINT_DB_2D_s *pCorners, const AMBA_CAL_OCCB_ORGANIZED_CORNER_s *pOrganizedCorners, UINT8 *pYImg) {
    UINT32 Rval = CT_OK;
    const AMBA_CT_LDCC_USER_SETTING_s *pLdccUserSetting;
    AMBA_FS_FILE *Fid;
    char FilePath[128];
    INT32 CurrentIdx, RightIdx, BottomIdx;
    UINT32 i, J;
    pLdccUserSetting = AmbaCT_LdccGetUserSetting();
    if (pYImg != NULL) {
        Fid = CT_fopen(pLdccUserSetting->SrcInfo.SinglePlanePatternYFilePath, "rb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void) CT_fread(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
            (void) CT_fclose(Fid);
        }

        for (J = 1U; J < ((CORNERS_2D_MAP_RADIUS * 2U) + 1U); J++) {
            for (i = 1U; i < ((CORNERS_2D_MAP_RADIUS * 2U) + 1U); i++) {
                CurrentIdx = pOrganizedCorners->pCorner2dMap[(J * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i];
                RightIdx = pOrganizedCorners->pCorner2dMap[(J * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i + 1U];
                BottomIdx = pOrganizedCorners->pCorner2dMap[((J + 1U) * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i];
                if ((CurrentIdx >= 0) && (RightIdx >= 0)) {
                    CT_LdccDrawLine(&pCorners[CurrentIdx], &pCorners[RightIdx], pLdccUserSetting->SrcInfo.ImageSize.Width, pYImg);
                }
                if ((CurrentIdx >= 0) && (BottomIdx >= 0)) {
                    CT_LdccDrawLine(&pCorners[BottomIdx], &pCorners[CurrentIdx], pLdccUserSetting->SrcInfo.ImageSize.Width, pYImg);
                }
            }
        }
        (void) AmbaUtility_StringCopy(FilePath, (INT32)sizeof(FilePath),pLdccUserSetting->System.OutputPrefix);
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "/");
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)),  pLdccUserSetting->System.OutputPrefix);
        (void) AmbaUtility_StringAppend(FilePath, CT_sizeT_to_U32(sizeof(FilePath)), "_SinglePlaneOrgDebugResult.y");
        Fid = CT_fopen(FilePath, "wb");
        if (Fid == NULL) {
            Rval = CT_ERR_1;
        } else {
            (void) CT_fwrite(pYImg, pLdccUserSetting->SrcInfo.ImageSize.Width * pLdccUserSetting->SrcInfo.ImageSize.Height, 1U, Fid);
            (void) CT_fclose(Fid);
        }
    }
    return Rval;
}

static inline void AmbaCT_InitCfg2LdccInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_LDCC_INIT_CFG_s *pLdccInitCfg)
{
    pLdccInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pLdccInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pLdccInitCfg->pFeedSinglePlaneSrcFunc = CT_LdccFeedSinglePlaneSrc;
    pLdccInitCfg->pDrawSinglePlaneOrgResult = CT_LdccDrawSinglePlaneOrgResult;
    pLdccInitCfg->pFeedMultiPlaneSrcFunc = CT_LdccFeedMultiPlaneSrc;
    pLdccInitCfg->pDrawMultiPlaneDetResult = CT_LdccDrawMultiPlaneDetResult;
    pLdccInitCfg->pFeedMultiPlanePatInfo = CT_LdccFeedMultiPlanePatInfo;
    pLdccInitCfg->pDrawMultiPlaneDetResultByPlane = CT_LdccDrawMultiPlaneDetResultByPlane;
    pLdccInitCfg->pDrawMultiPlaneOrgResult = CT_LdccDrawMultiPlaneOrgResult;
}

static inline void AmbaCT_InitCfg2OcInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_OC_INIT_CFG_s *pOcInitCfg)
{
    pOcInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pOcInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pOcInitCfg->pCalibWorkingBuf = pInitCfg->Oc.pCalibWorkingBuf;
    pOcInitCfg->CalibWorkingBufSize = pInitCfg->Oc.CalibWorkingBufSize;
    pOcInitCfg->CbFeedRawBuf= AmbaCT_OcFeedRaw;
}

static inline void AmbaCT_InitCfg2OcCbInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_OCCB_INIT_CFG_s *pOcCbInitCfg)
{
    pOcCbInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pOcCbInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pOcCbInitCfg->pPatDetWorkingBuf = pInitCfg->OcCb.pPatDetWorkingBuf;
    pOcCbInitCfg->PatDetWorkingBufSize = pInitCfg->OcCb.PatDetWorkingBufSize;
    pOcCbInitCfg->pOrganizeCBCornerWorkingBuf = pInitCfg->OcCb.pOrganizeCBCornerWorkingBuf;
    pOcCbInitCfg->OrganizeCBCornerWorkingBufSize = pInitCfg->OcCb.OrganizeCBCornerWorkingBufSize;
    pOcCbInitCfg->pOCFinderWorkingBuf = pInitCfg->OcCb.pOCFinderWorkingBuf;
    pOcCbInitCfg->OCFinderWorkingBufSize = pInitCfg->OcCb.OCFinderWorkingBufSize;
    pOcCbInitCfg->FeedSrcFunc = AmbaCT_OcCbFeedSrc;
    pOcCbInitCfg->PatDetDebugReport = AmbaCT_OcCbMarkCorner;

    pOcCbInitCfg->pLdccWorkingBuf = pInitCfg->Ldcc.pCalibWorkingBuf;
    pOcCbInitCfg->LdccWorkingBufSize = pInitCfg->Ldcc.CalibWorkingBufSize;
}


static inline UINT32 AmbaCT_InitCfg2BpcInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_BPC_INIT_CFG_s *pBpcInitCfg)
{
    UINT32 Rval = CT_OK;
    UINT8 *pSrcRawBuf = NULL;

    if (pInitCfg->pTunerWorkingBuf != NULL) {
        SIZE_t Size = 0U;
        AmbaCT_BpcGetWorkingBufSize(&Size);

        pBpcInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
        if (pInitCfg->TunerWorkingBufSize < Size) {
            AmbaPrint_PrintUInt5("Error: Spb tuner incorrect initial config TunerWorkingBufSize < BpcGetWorkingBufSize", 0U, 0U, 0U, 0U, 0U);
            pBpcInitCfg->WorkingBufSize = 0U;
            pBpcInitCfg->SrcRawBufSize = 0U;
            pBpcInitCfg->pSrcRawBuf = NULL;
            Rval = CT_ERR_1;

        } else {
            pBpcInitCfg->WorkingBufSize = Size;
            pBpcInitCfg->SrcRawBufSize = pInitCfg->TunerWorkingBufSize - Size;

            CT_CheckRval(AmbaWrap_memcpy(&pSrcRawBuf, &pInitCfg->pTunerWorkingBuf, sizeof(pSrcRawBuf)), "AmbaWrap_memcpy", __func__);
            pSrcRawBuf = &pSrcRawBuf[Size];
            pBpcInitCfg->pSrcRawBuf = pSrcRawBuf;
        }
    } else {
        AmbaPrint_PrintUInt5("Error: Spb tuner incorrect initial config pTunerWorkingBuf is NULL", 0U, 0U, 0U, 0U, 0U);
        pBpcInitCfg->pWorkingBuf = NULL;
        Rval = CT_ERR_1;
    }

    pBpcInitCfg->SrcRawResolution = CalibTunerCfg.Bpc.Resolution;

    pBpcInitCfg->FeedRawFunc = AmbaCT_BpcFeedRaw;

    if ((pInitCfg->Bpc.pCalibWorkingBuf == NULL) || (pInitCfg->Bpc.CalibWorkingBufSize == 0U)) {
        AmbaPrint_PrintUInt5("Error: Spb tuner incorrect initial config for calib working buffer.", 0U, 0U, 0U, 0U, 0U);
        pBpcInitCfg->pCalibWorkingBuf = NULL;
        pBpcInitCfg->CalibWorkingBufSize = 0U;
        Rval = CT_ERR_1;
    } else {
        pBpcInitCfg->pCalibWorkingBuf = pInitCfg->Bpc.pCalibWorkingBuf;
        pBpcInitCfg->CalibWorkingBufSize = pInitCfg->Bpc.CalibWorkingBufSize;
    }

    return Rval;
}

static UINT32 AmbaCT_DummyInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    // Note # To make sure misraC checker silence
    const AMBA_CT_INITIAL_CONFIG_s *pMisraC = pInitCfg;
    AmbaCT_TouchUnused(pMisraC);
    return CT_ERR_1;
}

static UINT32 AmbaCT_DummyLoad(const char *pTunerFileName)
{
    // Note # To make sure misraC checker silence
    const char *pMisraC = pTunerFileName;
    AmbaCT_TouchUnused(pMisraC);
    return CT_ERR_1;
}

static UINT32 AmbaCT_DummyExecute(void)
{
    return CT_ERR_1;
}

static UINT32 AmbaCT_DummyDump(void)
{
    return CT_ERR_1;
}

static UINT32 AmbaCT_1DVigInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Rval = CT_OK;
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_1D_VIG_INIT_CFG_s VigInitCfg;
    CT_CheckRval(AmbaWrap_memset(&VigInitCfg, 0x0, sizeof(VigInitCfg)), "AmbaWrap_memset", __func__);
    Rval |= AmbaCT_InitCfg21DVigInitCfg(pInitCfg, &VigInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&p1DVigParserObject);
    (void) AmbaCT_Get1DVignetteRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], p1DVigParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, p1DVigParserObject);
    Rval |= AmbaCT_1DVigTunerInit(&VigInitCfg);
    return Rval;
}

static UINT32 AmbaCT_AvmInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_AVM_INIT_CFG_s AvmInitCfg;
    CT_CheckRval(AmbaWrap_memset(AvmGridStatus, 0x0, sizeof(AvmGridStatus)), "AmbaWrap_memset", __func__);
    AmbaCT_InitCfg2AvmInitCfg(pInitCfg, &AvmInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pAvmParserObject);
    (void)AmbaCT_GetAvmRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pAvmParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pAvmParserObject);
    return AmbaCT_AvmTunerInit(&AvmInitCfg);
}

static UINT32 AmbaCT_EmInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_EM_INIT_CFG_s EmInitCfg;
    AmbaCT_InitCfg2EmInitCfg(pInitCfg, &EmInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pEmParserObject);
    (void) AmbaCT_GetEmRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pEmParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pEmParserObject);
    return AmbaCT_EmTunerInit(&EmInitCfg);
}

static UINT32 AmbaCT_LdcInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_LDC_INIT_CFG_s LdcInitCfg;
    AmbaCT_InitCfg2LdcInitCfg(pInitCfg, &LdcInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pLdcParserObject);
    (void) AmbaCT_GetLdcRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pLdcParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pLdcParserObject);
    return AmbaCT_LdcTunerInit(&LdcInitCfg);
}

static UINT32 AmbaCT_CaInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_CA_INIT_CFG_s CaInitCfg;
    AmbaCT_InitCfg2CaInitCfg(pInitCfg, &CaInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pCaParserObject);
    (void) AmbaCT_GetCaRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pCaParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pCaParserObject);
    return AmbaCT_CaTunerInit(&CaInitCfg);
}

static UINT32 AmbaCT_LdccInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_LDCC_INIT_CFG_s LdccInitCfg;
    AmbaCT_InitCfg2LdccInitCfg(pInitCfg, &LdccInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pLdccParserObject);
    (void) AmbaCT_GetLdccRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pLdccParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pLdccParserObject);
    return AmbaCT_LdccTunerInit(&LdccInitCfg);
}

static UINT32 AmbaCT_OcInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_OC_INIT_CFG_s OcInitCfg;
    AmbaCT_InitCfg2OcInitCfg(pInitCfg, &OcInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pOcParserObject);
    (void) AmbaCT_GetOpticalCenterRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pOcParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pOcParserObject);
    return AmbaCT_OcTunerInit(&OcInitCfg);
}


static UINT32 AmbaCT_OcCbInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_OCCB_INIT_CFG_s OcCbInitCfg;
    AmbaCT_InitCfg2OcCbInitCfg(pInitCfg, &OcCbInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pOcCbParserObject);
    (void) AmbaCT_GetOcCbRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pOcCbParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pOcCbParserObject);
    return AmbaCT_OcCbTunerInit(&OcCbInitCfg);
}

static UINT32 AmbaCT_BpcInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Rval = CT_OK;
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_BPC_INIT_CFG_s BpcInitCfg;
    Rval |= AmbaCT_InitCfg2BpcInitCfg(pInitCfg, &BpcInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pBpcParserObject);
    (void) AmbaCT_GetBpcRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pBpcParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pBpcParserObject);
    Rval |= AmbaCT_BpcTunerInit(&BpcInitCfg);

    return Rval;
}

static UINT32 AmbaCT_ParseFile(const char *pFileName, char *pLineBuffer, const CT_Parser_Object_t *pObject)
{
    INT32 Line = 0;
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE_INFO_s FileInfo;
    AMBA_FS_FILE *Fid;
    char* pReadBuf;
    char* pStartOfStr;
    char* pDstStr = pLineBuffer;
    INT64 ReadSize;
    UINT32 Len = 0;
    INT32 i;
    ULONG MisraU64_0, MisraU64_1, MisraU64_2, MisraU64_3, MisraU64_4;
    UINT32 MisraU32_2;
    const char *pMisraChar_0, *pMisraChar_1;
    INT32 MisraI32_BreakYes;
    static char ReadBuffer[READ_BUF_SIZE + 32 + 1] GNU_SECTION_NOZEROINIT;

    pReadBuf = &ReadBuffer[0];
    AmbaMisra_TypeCast(&MisraU64_0, &pReadBuf);
    pReadBuf = &ReadBuffer[32U - (MisraU64_0 & 0x1FU)];
    pStartOfStr = pReadBuf;
    CT_CheckRval(AmbaWrap_memset(&FileInfo, 0x0, sizeof(FileInfo)), "AmbaWrap_memset", __func__);
    Fid = CT_fopen(pFileName, "rb");
    if (Fid == NULL) {
        Rval = CT_ERR_1;
    } else {
        (void)AmbaFS_GetFileInfo(pFileName, &FileInfo);
        CT_CheckRval(CT_memset(ReadBuffer, 0x0, sizeof(ReadBuffer)), "CT_memset", __func__);

        ReadSize = (INT64)FileInfo.Size;

        while (ReadSize > 0) {
            if (ReadSize > READ_BUF_SIZE) {
                MisraU32_2 = (UINT32)READ_BUF_SIZE;
            } else {
                MisraU32_2 = (UINT32)ReadSize;
            }
            (void)CT_fread(pReadBuf, MisraU32_2, 1U, Fid);

            for (i = 0; i < (INT32)MisraU32_2; i++) {
                if ((pReadBuf[i] == '\n') || (pReadBuf[i] == '\r')) {
                    pReadBuf[i] = '\0';
                }
            }
            MisraI32_BreakYes = 0;

            for (;;) {
                INT64 AmbaUtilityMisraCTmp, MisraOffset;
                SIZE_t AmbaUtilityMisraCBufSize;
                Len = CT_sizeT_to_U32(AmbaUtility_StringLength(pStartOfStr));
                AmbaMisra_TypeCast(&MisraU64_1, &pStartOfStr);
                AmbaMisra_TypeCast(&MisraU64_2, &pReadBuf);
                MisraOffset = (INT64)MisraU64_1 - (INT64)MisraU64_2;
                pMisraChar_0 = &pStartOfStr[Len];
                AmbaMisra_TypeCast(&MisraU64_3, &pMisraChar_0);
                pMisraChar_1 = &pReadBuf[READ_BUF_SIZE];
                AmbaMisra_TypeCast(&MisraU64_4, &pMisraChar_1);

                if (MisraU64_3 >= MisraU64_4) {
                    AmbaUtilityMisraCTmp = (READ_BUF_SIZE - MisraOffset) + 1;
                    AmbaMisra_TypeCast(&AmbaUtilityMisraCBufSize, &AmbaUtilityMisraCTmp);
                    if(AmbaUtilityMisraCBufSize <=  ((ULONG)READ_BUF_SIZE + 32U + 1U-(MisraU64_0 & 0x1FU))){
                        AmbaUtility_StringCopy(pDstStr, AmbaUtilityMisraCBufSize, pStartOfStr);
                        pDstStr = &pDstStr[AmbaUtilityMisraCBufSize-1U];
                    }
                    pStartOfStr = pReadBuf;
                    // Note # Feed ReadBuf Again
                    MisraI32_BreakYes = 1;
                } else {
                    AmbaUtilityMisraCTmp = (READ_BUF_SIZE - MisraOffset) + 1;
                    AmbaMisra_TypeCast(&AmbaUtilityMisraCBufSize, &AmbaUtilityMisraCTmp);
                    if(AmbaUtilityMisraCBufSize <=  ((ULONG)READ_BUF_SIZE + 32U + 1U-(MisraU64_0 & 0x1FU))){
                        AmbaUtility_StringCopy(pDstStr, AmbaUtilityMisraCBufSize, pStartOfStr);
                    }
                    pStartOfStr = &pStartOfStr[Len + 1U];

                    if (pLineBuffer[0] != '\0') {
                        (void)CT_Parser_Parse_Line(pObject);
                        Line++;
                    }

                    AmbaMisra_TypeCast(&MisraU64_1, &pStartOfStr);
                    MisraOffset = (INT64)MisraU64_1 - (INT64)MisraU64_2;
                    if (MisraOffset >= ReadSize) {
                        MisraI32_BreakYes = 1;
                    }
                    pDstStr = pLineBuffer;
                }
                if (MisraI32_BreakYes == 1) {
                    break;
                }
            }
            ReadSize -= READ_BUF_SIZE;
        }
        (void)CT_fclose(Fid);
    }
    return Rval;
}

static UINT32 AmbaCT_1DVigLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;

    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, p1DVigParserObject);
    (void) CT_Parser_Get_LineBuf(p1DVigParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, p1DVigParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(p1DVigParserObject, AmbaCT_1DVigGetGroupStatus);

    return Rval;
}

static UINT32 AmbaCT_1DVigExecuteFunc(void)
{
    return AmbaCT_1DVigExecute();
}

static UINT32 AmbaCT_AvmLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    Rval |= (UINT32)CT_Parser_Set_Reglist_Valid(0, pAvmParserObject);
    Rval |= (UINT32)CT_Parser_Get_LineBuf(pAvmParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pAvmParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pAvmParserObject, AmbaCT_AvmGetGroupStatus);
    return Rval;
}

static UINT32 AmbaCT_AvmExecuteFunc(void)
{
    return AmbaCT_AvmExecute();// Note # fix --->AmbaCT_AvmExecute
}

static UINT32 AmbaCT_EmLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pEmParserObject);
    (void) CT_Parser_Get_LineBuf(pEmParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pEmParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pEmParserObject, AmbaCT_EmGetGroupStatus);
    return Rval;
}

static UINT32 AmbaCT_EmExecuteFunc(void)
{
    return AmbaCT_EmExecute();
}

static UINT32 AmbaCT_LdcLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pLdcParserObject);
    (void) CT_Parser_Get_LineBuf(pLdcParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pLdcParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pLdcParserObject, AmbaCT_LdcGetGroupStatus);
    return Rval;
}

static UINT32 AmbaCT_CaLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pCaParserObject);
    (void) CT_Parser_Get_LineBuf(pCaParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pCaParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pCaParserObject, AmbaCT_CaGetGroupStatus);
    return Rval;
}

static UINT32 AmbaCT_LdccLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pLdccParserObject);
    (void) CT_Parser_Get_LineBuf(pLdccParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pLdccParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pLdccParserObject, AmbaCT_LdccGetGroupStatus);
    return Rval;
}
static UINT32 AmbaCT_LdcExecuteFunc(void)
{
    return AmbaCT_LdcExecute();
}

static UINT32 AmbaCT_CaExecuteFunc(void)
{
    return AmbaCT_CaExecute();
}

static UINT32 AmbaCT_LdccExecuteFunc(void)
{
    return AmbaCT_LdccExecute();
}

static UINT32 AmbaCT_OcLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pOcParserObject);
    (void) CT_Parser_Get_LineBuf(pOcParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pOcParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pOcParserObject, AmbaCT_OcGetGroupStatus);
    return Rval;
}


static UINT32 AmbaCT_OcExecuteFunc(void)
{
    return AmbaCT_OcExecute();
}


static UINT32 AmbaCT_OcCbLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pOcCbParserObject);
    (void) CT_Parser_Get_LineBuf(pOcCbParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pOcCbParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pOcCbParserObject, AmbaCT_OcCbGetGroupStatus);
    return Rval;
}

static UINT32 AmbaCT_OcCbExecuteFunc(void)
{
    return AmbaCT_OcCbExecute();
}

static UINT32 AmbaCT_BpcLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pBpcParserObject);
    (void) CT_Parser_Get_LineBuf(pBpcParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pBpcParserObject);
    Rval |= CT_Parser_Check_Param_Completeness(pBpcParserObject, AmbaCT_BpcGetGroupStatus);
    return Rval;
}

static UINT32 AmbaCT_BpcExecuteFunc(void)
{
    return AmbaCT_BpcExecute();
}

static UINT32 CT_1DVigCalibData2Text(const char *pOutputPrefix, UINT32 Index, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;
    char FileName[64] = "";
    char LineBuf[512];
    UINT32 TableSize;
    SIZE_t Length;
    AmbaUtility_StringCopy(FileName, (INT32)sizeof(FileName),pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
    Length = AmbaUtility_StringLength(FileName);
    (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(64U - Length), Index);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        UINT32 i, J, K;
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Version: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.StartX: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.StartY: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.Width: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.Height: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.HSubSample.FactorNum: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.HSubSample.FactorDen: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.VSubSample.FactorNum: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "CalibSensorGeo.VSubSample.FactorDen: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "RadialCoarse: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->RadialCoarse);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "RadialCoarseLog: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->RadialCoarseLog);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "RadialBinsFine: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->RadialBinsFine);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "RadialBinsFineLog: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->RadialBinsFineLog);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "ModelCenterX_R,Y_R: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterX_R);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterY_R);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "ModelCenterX_Gr,Y_Gr: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterX_Gr);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterY_Gr);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "ModelCenterX_B,Y_B: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterX_B);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterY_B);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "ModelCenterX_Gb,Y_Gb: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterX_Gb);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->ModelCenterY_Gb);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (i = 0U; i < 4U; i++) {
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "VigGainTbl_R[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), i);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "]: \n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (J = 0U; J < 8U; J++) {
                for (K = 0U; K < 16U; K++) {
                    (void) CT_U32toStr(&LineBuf[0], 512U, pCalibData->VigGainTbl_R[i][(J * 16U) + K]);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }

            }
        }
        for (i = 0U; i < 4U; i++) {
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "VigGainTbl_Gr[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), i);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "]: \n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (J = 0U; J < 8U; J++) {
                for (K = 0U; K < 16U; K++) {
                    (void) CT_U32toStr(&LineBuf[0], 512U, pCalibData->VigGainTbl_Gr[i][(J * 16U) + K]);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }

            }
        }
        for (i = 0U; i < 4U; i++) {
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "VigGainTbl_B[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), i);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "]: \n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (J = 0U; J < 8U; J++) {
                for (K = 0U; K < 16U; K++) {
                    (void) CT_U32toStr(&LineBuf[0], 512U, pCalibData->VigGainTbl_B[i][(J * 16U) + K]);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }

            }
        }
        for (i = 0U; i < 4U; i++) {
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "VigGainTbl_Gb[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), i);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "]: \n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (J = 0U; J < 8U; J++) {
                for (K = 0U; K < 16U; K++) {
                    (void) CT_U32toStr(&LineBuf[0], 512U, pCalibData->VigGainTbl_Gb[i][(J * 16U) + K]);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
            }
        }
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "SyncCalInfo R: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->SyncCalInfo[0]);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "SyncCalInfo Ge: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->SyncCalInfo[1]);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "SyncCalInfo B: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->SyncCalInfo[2]);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "SyncCalInfo Go: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->SyncCalInfo[3]);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
    Length = AmbaUtility_StringLength(FileName);
    (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(64U - Length), Index);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open File:");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        TableSize = (4U*128U)*4U;
        (void) CT_fwrite(&pCalibData->VigGainTbl_R[0][0], 1, TableSize, Fd);
        (void) CT_fwrite(&pCalibData->VigGainTbl_Gr[0][0], 1, TableSize, Fd);
        (void) CT_fwrite(&pCalibData->VigGainTbl_B[0][0], 1, TableSize, Fd);
        (void) CT_fwrite(&pCalibData->VigGainTbl_Gb[0][0], 1, TableSize, Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static UINT32 CT_1DVigEvaReport2Text(const char *pOutputPrefix, UINT32 Index, const AMBA_CAL_1D_VIG_EVA_REPORT_s *pEvaluationReport)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fd;
    char FileName[64] = "";
    char LineBuf[512];
    AMBA_CT_1D_VIG_EVALUATION_LOC_s EvaluationInfo;
    UINT32 i;
    SIZE_t Length;

    AmbaCT_1DVigGetEvaluationLoc(Index, &EvaluationInfo);
    AmbaUtility_StringCopy(FileName, (INT32)sizeof(FileName),pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
    Length = AmbaUtility_StringLength(FileName);
    (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(64U - Length), Index);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_Report.txt");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (i = 0U; i < EvaluationInfo.Number; i++) {
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Sample Index: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), i);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "R: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pEvaluationReport[i].Avg9x9R);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Gr: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pEvaluationReport[i].Avg9x9Gr);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "B: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pEvaluationReport[i].Avg9x9B);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Gb: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pEvaluationReport[i].Avg9x9Gb);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }

    return Rval;
}

static UINT32 CT_1DVigCalibResult2Text(const char *pOutputPrefix, UINT32 Index, const AMBA_CT_1D_VIG_CALIB_DATA_V2_s *pVigData)
{
    UINT32 Rval = CT_OK;

    if (pVigData->pCalibData != NULL) {
        Rval |= CT_1DVigCalibData2Text(pOutputPrefix, Index, pVigData->pCalibData);
    } else {
        Rval |= CT_1DVigEvaReport2Text(pOutputPrefix, Index, pVigData->pEvaluationReport);
    }
    return Rval;
}


static UINT32 AmbaCT_1DVigDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    UINT32 Index;
    AMBA_CT_1D_VIG_TUNER_SYSTEM_s System;
    AMBA_CT_1D_VIG_CALIB_DATA_V2_s VigData;
    AmbaCT_1DVigGetSystemInfo(&System);
    for (Index = 0U; Index < System.ChanNum; Index++) {
        Rval |= AmbaCT_1DVigGetCalibDataV2(Index, &VigData);
        Rval |= CT_1DVigCalibResult2Text(System.OutputPrefix, Index, &VigData);
    }
    return Rval;
}

static UINT32 AmbaCT_AvmGridStatus2Text(const char *pFileName, AMBA_CAL_AVM_CAM_ID_e CamId)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;
    char LineBuf[1024];

    if ((AvmGridStatus[CamId].VerTileNum * AvmGridStatus[CamId].HorTileNum) != 0U) {
        Fd = CT_fopen(pFileName, "w");
        if (Fd == NULL) {
            Rval |= CT_ERR_1;
            pNextMsg = CT_LogPutStr(Msg, "Open");
            pNextMsg = CT_LogPutStr(pNextMsg, pFileName);
            pNextMsg = CT_LogPutStr(pNextMsg, "fail");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        } else {
            UINT32 HorIdx, VerIdx;
            for (VerIdx = 0; VerIdx < AvmGridStatus[CamId].VerTileNum; VerIdx++) {
                for (HorIdx = 0; HorIdx < AvmGridStatus[CamId].HorTileNum; HorIdx++) {
                    if (HorIdx == (AvmGridStatus[CamId].HorTileNum - 1U)) {
                        (void) CT_U32toHexStr(LineBuf, 1024U, AvmGridStatus[CamId].GridStatus[HorIdx + (VerIdx * AvmGridStatus[CamId].HorTileNum)]);
                    } else {
                        (void) CT_U32toHexStr(LineBuf, 1024U, AvmGridStatus[CamId].GridStatus[HorIdx + (VerIdx * AvmGridStatus[CamId].HorTileNum)]);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    }
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                AmbaUtility_StringCopy(LineBuf,  2, "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            Rval |= CT_fclose(Fd);
        }
    }

    return Rval;
}

static UINT32 AmbaCT_Avm2DCalibData2Text(const char *pFileNamePrefix, AMBA_CAL_AVM_2D_DATA_s *p2dAvmData)
{
    UINT32 Rval = CT_OK;
    char FileName[64];
    char LineBuf[1280];
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;
    UINT32 Index, HorIdx, VerIdx;
    SIZE_t Length;
    static const char ChanAbbr[(UINT32)AMBA_CAL_AVM_CAM_MAX] = {'F','B','L','R'};
    static const char * const Chan[(UINT32)AMBA_CAL_AVM_CAM_MAX] = {"Front", "Back", "Left", "Right"};
    for (Index = 0U; Index < (UINT32)AMBA_CAL_AVM_CAM_MAX; Index++) {
        CT_CheckRval(CT_memset(FileName, 0x0, CT_sizeT_to_U32(sizeof(FileName))), "CT_memset", __func__);
        AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName), pFileNamePrefix);
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  pFileNamePrefix);
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
        (void) CT_ChartoStr(FileName, 1280U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval |= CT_ERR_1;
            pNextMsg = CT_LogPutStr(Msg, "Open");
            pNextMsg = CT_LogPutStr(pNextMsg, FileName);
            pNextMsg = CT_LogPutStr(pNextMsg, "fail");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        } else {
            if ( p2dAvmData->Cam[Index].WarpTblStatus == AMBA_CAL_TBL_VALID ) {
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "#define HorGridNum ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p2dAvmData->Cam[Index].WarpTbl.HorGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "#define VerGridNum ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p2dAvmData->Cam[Index].WarpTbl.VerGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "INT32 calib_warp_header_spec[64] =\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toHexStr(LineBuf, 1280U, p2dAvmData->Version);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.HorGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.VerGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.TileWidthExp);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.TileHeightExp);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.StartX);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.StartY);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.Width);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.Height);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.HSubSample.FactorNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.HSubSample.FactorDen);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.VSubSample.FactorNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.VSubSample.FactorDen);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);


                {
                    UINT32 VoutCarIdx;
                    (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p2dAvmData->VoutCarOSDCornerNum);
                    (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                    for ( VoutCarIdx = 0U; VoutCarIdx < 8U; VoutCarIdx++) {
                        if ( VoutCarIdx  < p2dAvmData->VoutCarOSDCornerNum ) {
                            (void) CT_S32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)),p2dAvmData->VoutCarOSDCorner[VoutCarIdx].X);
                            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                            (void) CT_S32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)),p2dAvmData->VoutCarOSDCorner[VoutCarIdx].Y);
                            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                        } else {
                            AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                            AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                        }
                    }
                }

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "[");
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)),  Chan[Index]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "] Vout buffer :\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "StartX : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p2dAvmData->Cam[Index].VoutArea.StartX, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", StartY : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p2dAvmData->Cam[Index].VoutArea.StartY, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Width : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p2dAvmData->Cam[Index].VoutArea.Width, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", Height : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p2dAvmData->Cam[Index].VoutArea.Height, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Rotate type : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), (UINT32)p2dAvmData->Cam[Index].VoutRotation, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "HCoef: ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[0]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[1]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[2]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[3]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[4]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[5]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[6]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), p2dAvmData->Cam[Index].CalibMatrix[7]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", 1.000000");
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Ignore blend table : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), (UINT32)p2dAvmData->Cam[Index].BlendTblStatus, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "};\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                for (VerIdx = 0; VerIdx < p2dAvmData->Cam[Index].WarpTbl.VerGridNum; VerIdx++) {
                    LineBuf[0] = '\0';
                    for (HorIdx = 0; HorIdx < p2dAvmData->Cam[Index].WarpTbl.HorGridNum; HorIdx++) {
                    #if defined(WARP_H_FILE_ADD_INDEX_INFO)
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p2dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + VerIdx * p2dAvmData->Cam[Index].WarpTbl.HorGridNum].X, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "(");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),HorIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),VerIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "),");
                    #else
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p2dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + (VerIdx * p2dAvmData->Cam[Index].WarpTbl.HorGridNum)].X, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    #endif
                    }
                    (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "};\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                for (VerIdx = 0; VerIdx < p2dAvmData->Cam[Index].WarpTbl.VerGridNum; VerIdx++) {
                    LineBuf[0] = '\0';
                    for (HorIdx = 0; HorIdx < p2dAvmData->Cam[Index].WarpTbl.HorGridNum; HorIdx++) {
                    #if defined(WARP_H_FILE_ADD_INDEX_INFO)
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p2dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + VerIdx * p2dAvmData->Cam[Index].WarpTbl.HorGridNum].Y, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "(");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),HorIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),VerIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ")");
                    #else
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p2dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + (VerIdx * p2dAvmData->Cam[Index].WarpTbl.HorGridNum)].Y, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    #endif
                    }
                    (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "};\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_fclose(Fd);
            } else {
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "Not Applicable");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_fclose(Fd);
            }
        }

        if ( p2dAvmData->Cam[Index].WarpTblStatus == AMBA_CAL_TBL_VALID ) {
            static const AMBA_CAL_AVM_CAM_ID_e AVM_CAM_ID_ENUM_LUT[5] =
                {   AMBA_CAL_AVM_CAM_FRONT,
                    AMBA_CAL_AVM_CAM_BACK,
                    AMBA_CAL_AVM_CAM_LEFT,
                    AMBA_CAL_AVM_CAM_RIGHT,
                    AMBA_CAL_AVM_CAM_MAX,
                };
            AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
            (void) CT_ChartoStr(FileName, 72U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RawEncode.bin");

            Fd = CT_fopen(FileName, "wb");
            if (Fd == NULL) {
                Rval |= CT_ERR_1;
                pNextMsg = CT_LogPutStr(Msg, "Open");
                pNextMsg = CT_LogPutStr(pNextMsg, FileName);
                pNextMsg = CT_LogPutStr(pNextMsg, "fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);

            } else {
                (void) CT_fwrite(p2dAvmData->Cam[Index].WarpTbl.WarpVector, 1U, ((p2dAvmData->Cam[Index].WarpTbl.HorGridNum*p2dAvmData->Cam[Index].WarpTbl.VerGridNum)*2U)*2U, Fd);
                (void) CT_fclose(Fd);
            }
            AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/StatusTbl_");
            (void) CT_ChartoStr(FileName, 64U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".csv");
            Rval |= AmbaCT_AvmGridStatus2Text(FileName, AVM_CAM_ID_ENUM_LUT[Index]);

            AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/blend_");
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  Chan[Index]);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
            Length = AmbaUtility_StringLength(FileName);
            (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(72U - Length), p2dAvmData->Cam[Index].VoutArea.Width);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
            Length = AmbaUtility_StringLength(FileName);
            (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(72U - Length), p2dAvmData->Cam[Index].VoutArea.Height);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".bin");
            Fd = CT_fopen(FileName, "wb");
            if (Fd == NULL) {
                Rval |= CT_ERR_1;
                pNextMsg = CT_LogPutStr(Msg, "Open");
                pNextMsg = CT_LogPutStr(pNextMsg, FileName);
                pNextMsg = CT_LogPutStr(pNextMsg, "fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            } else {
                (void) CT_fwrite(p2dAvmData->Cam[Index].BlendTbl.Table, 1, CT_sizeT_to_U32(((SIZE_t)p2dAvmData->Cam[Index].VoutArea.Width*(SIZE_t)p2dAvmData->Cam[Index].VoutArea.Height)*sizeof(UINT8)), Fd);
                (void) CT_fclose(Fd);
            }
        }
    }

    return Rval;
}


static UINT32 AmbaCT_Avm3DCalibData2Text(const char *pFileNamePrefix, AMBA_CAL_AVM_3D_DATA_s *p3dAvmData)
{
    UINT32 Rval = CT_OK;

    char FileName[64];
    char LineBuf[1280];
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;
    UINT32 Index, HorIdx, VerIdx;
    static const char ChanAbbr[(UINT32)AMBA_CAL_AVM_CAM_MAX] = {'F','B','L','R'};
    static const char * const Chan[(UINT32)AMBA_CAL_AVM_CAM_MAX] = {"Front", "Back", "Left", "Right"};
    SIZE_t Length;

    for (Index = 0U; Index < (UINT32)AMBA_CAL_AVM_CAM_MAX; Index++) {
        CT_CheckRval(CT_memset(FileName, 0x0, CT_sizeT_to_U32(sizeof(FileName))), "CT_memset", __func__);
        AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName), pFileNamePrefix);
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  pFileNamePrefix);
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
        (void) CT_ChartoStr(FileName, 1280U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
        (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval |= CT_ERR_1;
            pNextMsg = CT_LogPutStr(Msg, "Open");
            pNextMsg = CT_LogPutStr(pNextMsg, FileName);
            pNextMsg = CT_LogPutStr(pNextMsg, "fail");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        } else {
            if ( p3dAvmData->Cam[Index].WarpTblStatus == AMBA_CAL_TBL_VALID ) {
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "#define HorGridNum ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p3dAvmData->Cam[Index].WarpTbl.HorGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "#define VerGridNum ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p3dAvmData->Cam[Index].WarpTbl.VerGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "INT32 calib_warp_header_spec[64] =\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toHexStr(LineBuf, 1280U, p3dAvmData->Version);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.HorGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.VerGridNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.TileWidthExp);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.TileHeightExp);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.StartX);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.StartY);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.Width);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.Height);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.HSubSample.FactorNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.HSubSample.FactorDen);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.VSubSample.FactorNum);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->Cam[Index].WarpTbl.CalibSensorGeo.VSubSample.FactorDen);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);


                {
                    UINT32 VoutCarIdx;
                    (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), p3dAvmData->VoutCarOSDCornerNum);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                    for ( VoutCarIdx = 0U; VoutCarIdx < 8U; VoutCarIdx++) {
                        if ( VoutCarIdx  < p3dAvmData->VoutCarOSDCornerNum ) {
                            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)),(UINT32)p3dAvmData->VoutCarOSDCorner[VoutCarIdx].X);
                            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)),(UINT32)p3dAvmData->VoutCarOSDCorner[VoutCarIdx].Y);
                            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                        } else {

                            AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                            AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
                            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                        }
                    }
                }

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "[");
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)),  Chan[Index]);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "] Vout buffer :\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "StartX : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p3dAvmData->Cam[Index].VoutArea.StartX, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", StartY : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p3dAvmData->Cam[Index].VoutArea.StartY, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Width : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p3dAvmData->Cam[Index].VoutArea.Width, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", Height : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p3dAvmData->Cam[Index].VoutArea.Height, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Rotate type : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),(UINT32)p3dAvmData->Cam[Index].VoutRotation, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "FrontEnd Rotate type : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), (UINT32)p3dAvmData->Cam[Index].FrontendRotation, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "VoutOrder type : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), p3dAvmData->Cam[Index].VoutOrder, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Ignore blend table : ");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), (UINT32)p3dAvmData->Cam[Index].BlendTblStatus, 5U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "};\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                for (VerIdx = 0; VerIdx < p3dAvmData->Cam[Index].WarpTbl.VerGridNum; VerIdx++) {
                    LineBuf[0] = '\0';
                    for (HorIdx = 0; HorIdx < p3dAvmData->Cam[Index].WarpTbl.HorGridNum; HorIdx++) {
                        #if defined(WARP_H_FILE_ADD_INDEX_INFO)
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p3dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + VerIdx * p3dAvmData->Cam[Index].WarpTbl.HorGridNum].X, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "(");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),HorIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),VerIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "),");
                        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                        #else
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p3dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + (VerIdx * p3dAvmData->Cam[Index].WarpTbl.HorGridNum)].X, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                        #endif
                    }
                    (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "};\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                for (VerIdx = 0; VerIdx < p3dAvmData->Cam[Index].WarpTbl.VerGridNum; VerIdx++) {
                    LineBuf[0] = '\0';
                    for (HorIdx = 0; HorIdx < p3dAvmData->Cam[Index].WarpTbl.HorGridNum; HorIdx++) {
                        #if defined(WARP_H_FILE_ADD_INDEX_INFO)
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p3dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + (VerIdx * p3dAvmData->Cam[Index].WarpTbl.HorGridNum)].Y, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "(");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),HorIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),VerIdx, 2U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "),");
                        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                        #else
                        Length = AmbaUtility_StringLength(LineBuf);
                        (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),p3dAvmData->Cam[Index].WarpTbl.WarpVector[HorIdx + (VerIdx * p3dAvmData->Cam[Index].WarpTbl.HorGridNum)].Y, 6U);
                        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                        #endif
                    }
                    (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "};\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_fclose(Fd);
            } else {

                AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "Not Applicable");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                (void) CT_fclose(Fd);
            }
        }

        if ( p3dAvmData->Cam[Index].WarpTblStatus == AMBA_CAL_TBL_VALID ) {

            static const AMBA_CAL_AVM_CAM_ID_e AVM_CAM_ID_ENUM_LUT[5] =
                {   AMBA_CAL_AVM_CAM_FRONT,
                    AMBA_CAL_AVM_CAM_BACK,
                    AMBA_CAL_AVM_CAM_LEFT,
                    AMBA_CAL_AVM_CAM_RIGHT,
                    AMBA_CAL_AVM_CAM_MAX,
                };
            AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
            (void) CT_ChartoStr(FileName, 72U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RawEncode.bin");

            Fd = CT_fopen(FileName, "wb");
            if (Fd == NULL) {
                Rval |= CT_ERR_1;
                pNextMsg = CT_LogPutStr(Msg, "Open");
                pNextMsg = CT_LogPutStr(pNextMsg, FileName);
                pNextMsg = CT_LogPutStr(pNextMsg, "fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            } else {
                (void) CT_fwrite(p3dAvmData->Cam[Index].WarpTbl.WarpVector, 1, ((p3dAvmData->Cam[Index].WarpTbl.HorGridNum*p3dAvmData->Cam[Index].WarpTbl.VerGridNum)*2U)*2U, Fd);
                (void) CT_fclose(Fd);
            }

            AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/StatusTbl_");
            (void) CT_ChartoStr(FileName, 64U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".csv");
            Rval |= AmbaCT_AvmGridStatus2Text(FileName, AVM_CAM_ID_ENUM_LUT[Index]);


            AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName),  pFileNamePrefix);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/blend_");
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  Chan[Index]);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
            Length = AmbaUtility_StringLength(FileName);
            (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(72U - Length),  p3dAvmData->Cam[Index].VoutArea.Width);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
            Length = AmbaUtility_StringLength(FileName);
            (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(72U - Length), p3dAvmData->Cam[Index].VoutArea.Height);
            (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".bin");
            Fd = CT_fopen(FileName, "wb");
            if (Fd == NULL) {
                Rval |= CT_ERR_1;
                pNextMsg = CT_LogPutStr(Msg, "Open");
                pNextMsg = CT_LogPutStr(pNextMsg, FileName);
                pNextMsg = CT_LogPutStr(pNextMsg, "fail");
                Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
            } else {
                (void) CT_fwrite(p3dAvmData->Cam[Index].BlendTbl.Table, 1, CT_sizeT_to_U32((SIZE_t)p3dAvmData->Cam[Index].VoutArea.Width*(SIZE_t)p3dAvmData->Cam[Index].VoutArea.Height*sizeof(UINT8)), Fd);
                (void) CT_fclose(Fd);
            }
        }
    }

    return Rval;
}

static UINT32 AmbaCT_AvmMVCalibData2Text(const char *pFileNamePrefix, AMBA_CAL_AVM_MV_DATA_s *pMvAvmData)
{
    UINT32 HorIdx, VerIdx;
    char FileName[64];
    char LineBuf[1280];
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;
    UINT32 Rval = CT_OK;
    SIZE_t Length;

    CT_CheckRval(CT_memset(FileName, 0x0, CT_sizeT_to_U32(sizeof(FileName))), "CT_memset", __func__);
    AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName), pFileNamePrefix);
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  pFileNamePrefix);
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "#define HorGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), pMvAvmData->WarpTbl.HorGridNum);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "#define VerGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), pMvAvmData->WarpTbl.VerGridNum);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "INT32 calib_warp_header_spec[64] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toHexStr(LineBuf, 1280U, pMvAvmData->Version);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.HorGridNum);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.VerGridNum);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.TileWidthExp);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.TileHeightExp);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.StartX);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.StartY);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.Width);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.Height);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.HSubSample.FactorNum);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.HSubSample.FactorDen);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.VSubSample.FactorNum);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pMvAvmData->WarpTbl.CalibSensorGeo.VSubSample.FactorDen);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);


        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Vout buffer :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "StartX : ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), pMvAvmData->VoutArea.StartX, 5U);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", StartY : ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), pMvAvmData->VoutArea.StartY, 5U);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);


        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "Width : ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), pMvAvmData->VoutArea.Width, 5U);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", Height : ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), pMvAvmData->VoutArea.Height, 5U);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Rotate type : ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), (UINT32)pMvAvmData->VoutRotation, 5U);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "FrontEnd Rotate type : ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length), (UINT32)pMvAvmData->FrontendRotation, 5U);
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        for (VerIdx = 0; VerIdx < pMvAvmData->WarpTbl.VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pMvAvmData->WarpTbl.HorGridNum; HorIdx++) {
                #if defined(WARP_H_FILE_ADD_INDEX_INFO)
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),pMvAvmData->WarpTbl.WarpVector[HorIdx + (VerIdx * pMvAvmData->WarpTbl.HorGridNum)].X, 6U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "(");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),HorIdx, 2U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),VerIdx, 2U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ")");
                #else
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),pMvAvmData->WarpTbl.WarpVector[HorIdx + (VerIdx * pMvAvmData->WarpTbl.HorGridNum)].X, 6U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                #endif
            }
            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }


        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf),  "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        for (VerIdx = 0; VerIdx < pMvAvmData->WarpTbl.VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pMvAvmData->WarpTbl.HorGridNum; HorIdx++) {
                #if defined(WARP_H_FILE_ADD_INDEX_INFO)
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),pMvAvmData->WarpTbl.WarpVector[HorIdx + VerIdx * pMvAvmData->WarpTbl.HorGridNum].Y, 6U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "(");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),HorIdx, 2U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),VerIdx, 2U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ")");
                #else
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(1280U - Length),pMvAvmData->WarpTbl.WarpVector[HorIdx + (VerIdx * pMvAvmData->WarpTbl.HorGridNum)].Y, 6U);
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                #endif
            }
            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }


        AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }

    AmbaUtility_StringCopy(FileName,  (INT32) sizeof(FileName),  pFileNamePrefix);
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)),  pFileNamePrefix);
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RawEncode.bin");

    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fwrite(pMvAvmData->WarpTbl.WarpVector, 1, ((pMvAvmData->WarpTbl.HorGridNum*pMvAvmData->WarpTbl.VerGridNum)*2U)*2U, Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;
}


static UINT32 AmbaCT_Em3in1CalibData2Text(const char *pFileNamePrefix, AMBA_CAL_EM_3IN1_DATA_s *pEm3in1Data)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    char FileName[72] = "";
    char LineBuf[768];
    AMBA_FS_FILE *Fd;
    UINT32 Index, HorIdx, VerIdx;
    SIZE_t Length;
    static const AMBA_CAL_EM_CAM_ID_e CamID_Lut[(UINT32)AMBA_CAL_EM_CAM_MAX] = {
        AMBA_CAL_EM_CAM_BACK,
        AMBA_CAL_EM_CAM_LEFT,
        AMBA_CAL_EM_CAM_RIGHT,
    };
    static const char ChanAbbr[(UINT32)AMBA_CAL_EM_CAM_MAX] = {'B','L','R'};
    static const char * const Chan[(UINT32)AMBA_CAL_EM_CAM_MAX] = {"Back", "Left", "Right"};
    for (Index = 0U; Index < (UINT32)AMBA_CAL_EM_CAM_MAX; Index++) {

        AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
        (void) CT_ChartoStr(FileName, 768U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval |= CT_ERR_1;
            pNextMsg = CT_LogPutStr(Msg, "Open");
            pNextMsg = CT_LogPutStr(pNextMsg, FileName);
            pNextMsg = CT_LogPutStr(pNextMsg, "fail");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        } else {

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define HorGridNum ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].WarpTbl.HorGridNum);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define VerGridNum ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].WarpTbl.VerGridNum);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT32 calib_warp_header_spec[64] =\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toHexStr(LineBuf, 768U, pEm3in1Data->Version);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.HorGridNum);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.VerGridNum);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.TileWidthExp);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.TileHeightExp);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.StartX);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.StartY);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.Width);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.Height);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.HSubSample.FactorNum);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.HSubSample.FactorDen);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.VSubSample.FactorNum);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEm3in1Data->Cam[Index].WarpTbl.CalibSensorGeo.VSubSample.FactorDen);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "[");
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), Chan[Index]);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "] Vout buffer :\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), " StartX : ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].VoutArea.StartX, 5U);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", StartY : ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].VoutArea.StartY, 5U);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), " Width  : ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].VoutArea.Width, 5U);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", Height : ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].VoutArea.Height, 5U);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), " Rotate type : ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), (UINT32) pEm3in1Data->Cam[Index].Rotation, 5U);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (VerIdx = 0U; VerIdx < pEm3in1Data->Cam[Index].WarpTbl.VerGridNum; VerIdx++) {
                LineBuf[0] = '\0';
                for (HorIdx = 0U; HorIdx < pEm3in1Data->Cam[Index].WarpTbl.HorGridNum; HorIdx++) {
                    Length = AmbaUtility_StringLength(LineBuf);
                    (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].WarpTbl.WarpVector[HorIdx + (VerIdx * pEm3in1Data->Cam[Index].WarpTbl.HorGridNum)].X, 6U);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                }
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (VerIdx = 0U; VerIdx < pEm3in1Data->Cam[Index].WarpTbl.VerGridNum; VerIdx++) {
                LineBuf[0] = '\0';
                for (HorIdx = 0U; HorIdx < pEm3in1Data->Cam[Index].WarpTbl.HorGridNum; HorIdx++) {
                    Length = AmbaUtility_StringLength(LineBuf);
                    (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEm3in1Data->Cam[Index].WarpTbl.WarpVector[HorIdx + (VerIdx * pEm3in1Data->Cam[Index].WarpTbl.HorGridNum)].Y, 6U);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                }
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            (void) CT_fclose(Fd);
        }

        AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
        (void) CT_ChartoStr(FileName, 72U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RawEncode.bin");
        Fd = CT_fopen(FileName, "wb");
        if (Fd == NULL) {
            Rval |= CT_ERR_1;
            pNextMsg = CT_LogPutStr(Msg, "Open");
            pNextMsg = CT_LogPutStr(pNextMsg, FileName);
            pNextMsg = CT_LogPutStr(pNextMsg, "fail");
            Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
        } else {
            (void) CT_fwrite(pEm3in1Data->Cam[Index].WarpTbl.WarpVector, 1, CT_sizeT_to_U32(sizeof(pEm3in1Data->Cam[Index].WarpTbl.WarpVector)), Fd);
            (void) CT_fclose(Fd);
        }

        AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/StatusTbl_");
        (void) CT_ChartoStr(FileName, 72U, CT_sizeT_to_U32(AmbaUtility_StringLength(FileName)), ChanAbbr[Index]);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".csv");
        Rval |= AmbaCT_EmrGridStatus2Text(FileName, CamID_Lut[Index]);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/blend_");
    Length = AmbaUtility_StringLength(FileName);
    (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(72U - Length), pEm3in1Data->BlendTbl.Width);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
    Length = AmbaUtility_StringLength(FileName);
    (void) CT_U32toStr(&FileName[Length], CT_sizeT_to_U32(72U - Length), pEm3in1Data->BlendTbl.Height);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fwrite(pEm3in1Data->BlendTbl.Table, 1, pEm3in1Data->BlendTbl.Width * pEm3in1Data->BlendTbl.Height * CT_sizeT_to_U32(sizeof(UINT8)), Fd);
        (void) CT_fclose(Fd);
    }
    // TODO # dump err_report
    return Rval;
}

static UINT32 AmbaCT_EmSvCalibData2Text(const char *pFileNamePrefix, AMBA_CAL_EM_SV_DATA_s *pEmSingleViewData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
#define LINE_BUF_LEN ((128U * 7U) + 1U)
    char FileName[72] = "";
    char LineBuf[LINE_BUF_LEN];
    AMBA_FS_FILE *Fd;
    UINT32 HorIdx, VerIdx;
    SIZE_t Length;

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define HorGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_LEN - Length), pEmSingleViewData->WarpTbl.HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define VerGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_LEN - Length), pEmSingleViewData->WarpTbl.VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT32 calib_warp_header_spec[64] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toHexStr(LineBuf, LINE_BUF_LEN, pEmSingleViewData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.TileWidthExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.TileHeightExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0U; VerIdx < pEmSingleViewData->WarpTbl.VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0U; HorIdx < pEmSingleViewData->WarpTbl.HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_LEN - Length), pEmSingleViewData->WarpTbl.WarpVector[HorIdx + (VerIdx * pEmSingleViewData->WarpTbl.HorGridNum)].X, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pEmSingleViewData->WarpTbl.VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pEmSingleViewData->WarpTbl.HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_LEN - Length), pEmSingleViewData->WarpTbl.WarpVector[HorIdx + (VerIdx * pEmSingleViewData->WarpTbl.HorGridNum)].Y, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RawEncode.bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fwrite(pEmSingleViewData->WarpTbl.WarpVector, 1, CT_sizeT_to_U32(sizeof(pEmSingleViewData->WarpTbl.WarpVector)), Fd);
        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/StatusTbl.csv");
    Rval |= AmbaCT_EmrGridStatus2Text(FileName, AMBA_CAL_EM_CAM_BACK);
    return Rval;
}

static UINT32 AmbaCT_LdcCalibData2Text(const char *pFileNamePrefix, AMBA_CAL_LDC_DATA_s *pLdcCalibData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    char FileName[64] = "";
#define LINE_BUF_SZ 1794U    //#Note: H_GRID_NUM 256 * 7 + 2
    char LineBuf[LINE_BUF_SZ];
    AMBA_FS_FILE *Fd;
    UINT32 HorIdx, VerIdx;
    SIZE_t Length;

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define HorGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_SZ - Length), pLdcCalibData->WarpTbl.HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define VerGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_SZ - Length), pLdcCalibData->WarpTbl.VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT32 calib_warp_header_spec[64] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toHexStr(LineBuf, LINE_BUF_SZ, pLdcCalibData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.TileWidthExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.TileHeightExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pLdcCalibData->WarpTbl.CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        if ((pLdcCalibData->WarpTbl.HorGridNum <= MAX_WARP_TBL_H_GRID_NUM) && (pLdcCalibData->WarpTbl.VerGridNum <= MAX_WARP_TBL_V_GRID_NUM)) {
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (VerIdx = 0; VerIdx < pLdcCalibData->WarpTbl.VerGridNum; VerIdx++) {
                LineBuf[0] = '\0';
                for (HorIdx = 0; HorIdx < pLdcCalibData->WarpTbl.HorGridNum; HorIdx++) {
                    Length = AmbaUtility_StringLength(LineBuf);
                    (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_SZ - Length), pLdcCalibData->WarpTbl.WarpVector[HorIdx + (VerIdx * pLdcCalibData->WarpTbl.HorGridNum)].X, 6U);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                }
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (VerIdx = 0; VerIdx < pLdcCalibData->WarpTbl.VerGridNum; VerIdx++) {
                LineBuf[0] = '\0';
                for (HorIdx = 0; HorIdx < pLdcCalibData->WarpTbl.HorGridNum; HorIdx++) {
                    Length = AmbaUtility_StringLength(LineBuf);
                    (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(LINE_BUF_SZ - Length), pLdcCalibData->WarpTbl.WarpVector[HorIdx + (VerIdx * pLdcCalibData->WarpTbl.HorGridNum)].Y, 6U);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                }
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        } else {
            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Grid number over limitation. Skip table content");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RawEncode.bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fwrite(pLdcCalibData->WarpTbl.WarpVector, 1, CT_sizeT_to_U32(sizeof(pLdcCalibData->WarpTbl.WarpVector)), Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;

}

static UINT32 AmbaCT_CaCalibData2Text(const char *pFileNamePrefix, const AMBA_CAL_CA_DATA_s *pCaCalibData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    char FileName[64] = "";
    char LineBuf[768];
    AMBA_FS_FILE *Fd;
    UINT32 HorIdx, VerIdx,TableSize;
    SIZE_t Length;

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define HorGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define VerGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT32 calib_warp_header_spec[64] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toHexStr(LineBuf, 768U, pCaCalibData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->TileWidthExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->TileHeightExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_S16toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->RScaleFactor);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_S16toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->BScaleFactor);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_ca_table_spec horizontal [HorGridNum] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pCaCalibData->VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pCaCalibData->HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->pCaTbl[HorIdx + (VerIdx * pCaCalibData->HorGridNum)].X, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_ca_table_spec vertical [VerGridNum] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pCaCalibData->VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pCaCalibData->HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->pCaTbl[HorIdx + (VerIdx * pCaCalibData->HorGridNum)].Y, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        TableSize = (pCaCalibData->HorGridNum * pCaCalibData->VerGridNum * 2U * 2U);
        (void) CT_fwrite(pCaCalibData->pCaTbl, 1,TableSize, Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;

}

static UINT32 AmbaCT_CaSeparateCalibData2Text(const char *pFileNamePrefix, const AMBA_CAL_CA_SEPARATE_DATA_s *pCaCalibData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    char FileName[64] = "";
    char LineBuf[768];
    AMBA_FS_FILE *Fd;
    UINT32 HorIdx, VerIdx,TableSize;
    SIZE_t Length;

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define HorGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define VerGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT32 calib_warp_header_spec[64] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toHexStr(LineBuf, 768U, pCaCalibData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->TileWidthExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->TileHeightExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_S16toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->RScaleFactor);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_S16toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pCaCalibData->BScaleFactor);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "Reserved\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        // Note # Red table
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_ca_Rtable_spec horizontal [HorGridNum] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pCaCalibData->VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pCaCalibData->HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->pRCaTbl[HorIdx + (VerIdx * pCaCalibData->HorGridNum)].X, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        ;
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_ca_Rtable_spec vertical [VerGridNum] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pCaCalibData->VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pCaCalibData->HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->pRCaTbl[HorIdx + (VerIdx * pCaCalibData->HorGridNum)].Y, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        // Note # Blue table
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_ca_Btable_spec horizontal [HorGridNum] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pCaCalibData->VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pCaCalibData->HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->pBCaTbl[HorIdx + (VerIdx * pCaCalibData->HorGridNum)].X, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        ;
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_ca_Btable_spec vertical [VerGridNum] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pCaCalibData->VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pCaCalibData->HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pCaCalibData->pBCaTbl[HorIdx + (VerIdx * pCaCalibData->HorGridNum)].Y, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_fclose(Fd);
    }

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "R.bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        TableSize = (pCaCalibData->HorGridNum * pCaCalibData->VerGridNum * 2U * 2U);
        (void) CT_fwrite(pCaCalibData->pRCaTbl, 1,TableSize, Fd);
        (void) CT_fclose(Fd);
    }

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "B.bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        TableSize = (pCaCalibData->HorGridNum * pCaCalibData->VerGridNum * 2U * 2U);
        (void) CT_fwrite(pCaCalibData->pBCaTbl, 1,TableSize, Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;

}

static UINT32 AmbaCT_LdccCalibData2Text(const char *pFileNamePrefix, AMBA_CT_LDCC_CALIB_DATA_s *pLdccData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    char FileName[64] = "";
    char LineBuf[1024];
    UINT32 Idx;
    AMBA_FS_FILE *Fd;
    SIZE_t Length;

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Optical Center ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->OpticalCenter.X);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->OpticalCenter.Y);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) AmbaUtility_StringCopy(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Real-Angle table length: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->RealAngleTbl.Length);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "real distance (mm) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->RealAngleTbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->RealAngleTbl.pRealTbl[Idx]);
            if(Idx != (pLdccData->RealAngleTbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "angle (degree) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->RealAngleTbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->RealAngleTbl.pAngleTbl[Idx]);
            if(Idx != (pLdccData->RealAngleTbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_fclose(Fd);
    }

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_debug.txt");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Optical Center ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->OpticalCenter.X);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->OpticalCenter.Y);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) AmbaUtility_StringCopy(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"One Plane Real-Expect table length: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->OnePlaneRETbl.Length);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "real distance (mm) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->OnePlaneRETbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->OnePlaneRETbl.pRealTbl[Idx]);
            if(Idx != (pLdccData->OnePlaneRETbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "expect distance (mm) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->OnePlaneRETbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->OnePlaneRETbl.pExpectTbl[Idx]);
            if(Idx != (pLdccData->OnePlaneRETbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) AmbaUtility_StringCopy(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Multi Plane Real-Expect table  (range: 0 ~ user setting degree) length : ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->MultiPlaneRETbl.Length);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "real distance (mm) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->MultiPlaneRETbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->MultiPlaneRETbl.pRealTbl[Idx]);
            if(Idx != (pLdccData->MultiPlaneRETbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "expect distance (mm) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->MultiPlaneRETbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->MultiPlaneRETbl.pExpectTbl[Idx]);
            if(Idx != (pLdccData->MultiPlaneRETbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) AmbaUtility_StringCopy(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Real-Angle table length: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->RealAngleTbl.Length);
        (void)AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "real distance (mm) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->RealAngleTbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->RealAngleTbl.pRealTbl[Idx]);
            if(Idx != (pLdccData->RealAngleTbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void)AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "angle (degree) :\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        LineBuf[0] = '\0';
        for (Idx = 0; Idx < pLdccData->RealAngleTbl.Length; Idx++) {
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pLdccData->RealAngleTbl.pAngleTbl[Idx]);
            if(Idx != (pLdccData->RealAngleTbl.Length-1U)){
                (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ", ");
            }
        }
        (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_OC.bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fwrite(&pLdccData->OpticalCenter, 1, CT_sizeT_to_U32(sizeof(pLdccData->OpticalCenter)), Fd);
        (void) CT_fclose(Fd);
    }


    return Rval;

}

static UINT32 AmbaCT_OcCalibData2Text(const char *pOutputPrefix, const AMBA_CT_OC_CALIB_DATA_s *pOcData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    AMBA_FS_FILE *Fd;
    char FileName[64] = "";
    char LineBuf[512];
    const AMBA_CAL_OC_CALIB_DATA_s *pCalibData = pOcData->pCalibData;
    SIZE_t Length;

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        UINT32 Index;

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Version: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toHexStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.StartX: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.StartY: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.Width: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.Height: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.HSubSample.FactorNum: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.HSubSample.FactorDen: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.VSubSample.FactorNum: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.VSubSample.FactorDen: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (Index = 0U; Index < 3U; ++Index) {

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Ellipse[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), Index);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "].OpticalCenter.X: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pCalibData->Ellipse[Index].OpticalCenter.X);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Ellipse[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), Index);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "].OpticalCenter.Y: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pCalibData->Ellipse[Index].OpticalCenter.Y);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");

            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Ellipse[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), Index);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "].Size.Width: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pCalibData->Ellipse[Index].Size.Width);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Ellipse[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), Index);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "].Size.Height: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pCalibData->Ellipse[Index].Size.Height);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

            AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Ellipse[");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), Index);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "].Angle: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(sizeof(LineBuf) - Length), pCalibData->Ellipse[Index].Angle);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Center.X: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->Center.X);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Center.Y: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->Center.Y);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }

    return Rval;
}

static inline UINT32 CT_OcCbCorners2Text(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fd;
    UINT32 Index;
    char LineBuf[512];
    SIZE_t Length;

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (Index = 0U; Index < pOcCbData->CornerNum; Index++) {
            (void) CT_U32toStr(&LineBuf[0], 512, Index);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ": ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pOcCbData->pSubPixCorners[Index].X);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pOcCbData->pSubPixCorners[Index].Y);
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbCorner2dMap2Csv(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fd;
    UINT32 i, J;
    char LineBuf[512];
    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (J = 0U; J < ((CORNERS_2D_MAP_RADIUS*2U) + 1U); J++) {
            LineBuf[0] = '\0';
            for (i = 0U; i < ((CORNERS_2D_MAP_RADIUS*2U) + 1U); i++) {
                (void) CT_S16toStr(LineBuf, 512U, pOcCbData->pOrganizedCorners->pCorner2dMap[(J * ((CORNERS_2D_MAP_RADIUS * 2U) + 1U)) + i]);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            LineBuf[0] = '\n';
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbOpticalCenter2Text(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fd;
    char LineBuf[512];
    SIZE_t Length;

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pOpticalCenterResult->X);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pOcCbData->pOpticalCenterResult->Y);
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbDistortionTable2Text(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fd;
    UINT32 Index;
    char LineBuf[512];
    SIZE_t Length;

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (UINT32 Table = 0U; Table < 2U; Table ++) {
            const DOUBLE *pTableAddr;
            LineBuf[0] = '\0';
            if (Table == 0U) {
                AmbaUtility_StringAppend(LineBuf, 512U, "Refined Real :");
                pTableAddr = pOcCbData->pRefinedLensSpecRealExpect->pRealTbl;
            } else {
                AmbaUtility_StringAppend(LineBuf, 512U, "Refined Expect :");
                pTableAddr = pOcCbData->pRefinedLensSpecRealExpect->pExpectTbl;
            }
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            for (Index = 0U; Index < pOcCbData->pRefinedLensSpecRealExpect->Length; Index++) {
                LineBuf[0] = ' ';
                LineBuf[1] = '\0';
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_DBtoStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pTableAddr[Index]);
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            LineBuf[0] = '\n';
            LineBuf[1] = '\0';
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbSaveWorldCornerMap(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    AMBA_FS_FILE *Fd;
    Fd = CT_fopen(pFileName, "wb");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        AMBA_CAL_POINT_INT_2D_s BaseCornerIdx;
        AMBA_CAL_SIZE_s CornerPairsImgSize;

        BaseCornerIdx = pOcCbData->BaseCornerIdx;
        CornerPairsImgSize = pOcCbData->CornerPairsImgSize;
        (void) CT_fwrite(&BaseCornerIdx, 1, CT_sizeT_to_U32(sizeof(BaseCornerIdx)), Fd);
        (void) CT_fwrite(&CornerPairsImgSize, 1, CT_sizeT_to_U32(sizeof(CornerPairsImgSize)), Fd);
        (void) CT_fwrite(pOcCbData->pCornerPairs, pOcCbData->CornerPairsImgSize.Width*pOcCbData->CornerPairsImgSize.Height,
            CT_sizeT_to_U32(sizeof(AMBA_CAL_WORLD_RAW_POINT_s)), Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbSaveWldMapImgPosXcsv(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    UINT32 i, J;
    AMBA_FS_FILE *Fd;
    char LineBuf[512];

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
            for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].RawPos.X);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            LineBuf[0] = '\n';
            LineBuf[1] = '\0';
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbSaveWldMapImgPosYcsv(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    UINT32 i, J;
    AMBA_FS_FILE *Fd;
    char LineBuf[512];

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
            for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].RawPos.Y);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            LineBuf[0] = '\n';
            LineBuf[1] = '\0';
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbSaveWldMapWorldPosXcsv(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    UINT32 i, J;
    AMBA_FS_FILE *Fd;
    char LineBuf[512];

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
            for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].WorldPos.X);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            LineBuf[0] = '\n';
            LineBuf[1] = '\0';
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbSaveWldMapWorldPosYcsv(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    UINT32 i, J;
    AMBA_FS_FILE *Fd;
    char LineBuf[512];

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
            for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].WorldPos.Y);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            LineBuf[0] = '\n';
            LineBuf[1] = '\0';
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static inline UINT32 CT_OcCbSaveWldMapWorldPosZcsv(const char *pFileName, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData)
{
    UINT32 Rval = CT_OK;
    UINT32 i, J;
    AMBA_FS_FILE *Fd;
    char LineBuf[512];

    Fd = CT_fopen(pFileName, "w");
    if (Fd == NULL) {
        Rval = CT_ERR_1;
    } else {
        for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
            for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].WorldPos.Z);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            LineBuf[0] = '\n';
            LineBuf[1] = '\0';
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }
        (void) CT_fclose(Fd);
    }
    return Rval;
}



static UINT32 AmbaCT_OcCbCalibData2Text(const char *pOutputPrefix, const AMBA_CT_OCCB_CALIB_DATA_s *pOcCbData,
    UINT32 DumpWolrdMap)
{
    UINT32 Rval = CT_OK;
    char FileName[128] = "";
    (void) AmbaUtility_StringCopy(FileName,  64, pOutputPrefix);

    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_Corners.txt");
    Rval |= CT_OcCbCorners2Text(FileName, pOcCbData);
    (void) AmbaUtility_StringCopy(FileName,  64, pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_CornerMap.csv");
    Rval |= CT_OcCbCorner2dMap2Csv(FileName, pOcCbData);
    if ( DumpWolrdMap != 0U ) {
        (void) AmbaUtility_StringCopy(FileName, 64, pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_WorldCornerMap.bin");
        Rval |= CT_OcCbSaveWorldCornerMap(FileName, pOcCbData);

        (void) AmbaUtility_StringCopy(FileName, 64, pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_WorldCornerMapImgPosX.csv");
        Rval |= CT_OcCbSaveWldMapImgPosXcsv(FileName, pOcCbData);

/*        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval = CT_ERR_1;
        } else {
            for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
                for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                    (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].RawPos.X);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                LineBuf[0] = '\n';
                LineBuf[1] = '\0';
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            (void) CT_fclose(Fd);
        }
*/
        (void) AmbaUtility_StringCopy(FileName, 64, pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_WorldCornerMapImgPosY.csv");
        Rval |= CT_OcCbSaveWldMapImgPosYcsv(FileName, pOcCbData);
/*        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval = CT_ERR_1;
        } else {
            for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
                for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                    (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].RawPos.Y);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                LineBuf[0] = '\n';
                LineBuf[1] = '\0';
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            (void) CT_fclose(Fd);
        }
*/
        (void) AmbaUtility_StringCopy(FileName, 64, pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_WorldCornerMapWorldPosX.csv");
        Rval |= CT_OcCbSaveWldMapWorldPosXcsv(FileName, pOcCbData);
/*        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval = CT_ERR_1;
        } else {
            for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
                for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                    (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].WorldPos.X);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                LineBuf[0] = '\n';
                LineBuf[1] = '\0';
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            (void) CT_fclose(Fd);
        }
*/
        (void) AmbaUtility_StringCopy(FileName, 64, pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_WorldCornerMapWorldPosY.csv");
        Rval |= CT_OcCbSaveWldMapWorldPosYcsv(FileName, pOcCbData);
/*        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval = CT_ERR_1;
        } else {
            for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
                for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                    (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].WorldPos.Y);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                LineBuf[0] = '\n';
                LineBuf[1] = '\0';
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            (void) CT_fclose(Fd);
        }
*/
        (void) AmbaUtility_StringCopy(FileName, 64, pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
        AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_WorldCornerMapWorldPosZ.csv");
        Rval |= CT_OcCbSaveWldMapWorldPosZcsv(FileName, pOcCbData);
/*        Fd = CT_fopen(FileName, "w");
        if (Fd == NULL) {
            Rval = CT_ERR_1;
        } else {
            for (J = 0U; J < pOcCbData->CornerPairsImgSize.Height; J++) {
                for (i = 0U; i < pOcCbData->CornerPairsImgSize.Width; i++) {
                    (void) CT_DBtoStr(LineBuf, 512U, pOcCbData->pCornerPairs[(J * pOcCbData->CornerPairsImgSize.Width) + i].WorldPos.Z);
                    AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
                    (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
                }
                LineBuf[0] = '\n';
                LineBuf[1] = '\0';
                (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
            }
            (void) CT_fclose(Fd);
        }
*/
    }
    (void) AmbaUtility_StringCopy(FileName,  64, pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_OpticalCenter.txt");
    Rval |= CT_OcCbOpticalCenter2Text(FileName, pOcCbData);

    if (pOcCbData->pRefinedLensSpecRealExpect != NULL) {
        if ((pOcCbData->pRefinedLensSpecRealExpect->Length != 0U) &&
            (pOcCbData->pRefinedLensSpecRealExpect->pExpectTbl != NULL) &&
            (pOcCbData->pRefinedLensSpecRealExpect->pRealTbl != NULL)) {
            (void) AmbaUtility_StringCopy(FileName,  64, pOutputPrefix);
            AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
            AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pOutputPrefix);
            AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RefinedDistoTable.txt");
            Rval |= CT_OcCbDistortionTable2Text(FileName, pOcCbData);
        }
    }
    return Rval;
}

static UINT32 AmbaCT_BpcCalibData2Text(const char *pOutputPrefix, const AMBA_CT_BPC_CALIB_DATA_s *pBpcData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    SIZE_t Length;

    AMBA_FS_FILE *Fd;
    char FileName[64] = "";
    char LineBuf[512];
    UINT32 TableSize;
    const AMBA_CAL_BPC_CALIB_DATA_V1_s *pCalibData = pBpcData->pCalibData;

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Version: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toHexStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.StartX: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.StartY: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.Width: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.Height: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.HSubSample.FactorNum: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.HSubSample.FactorDen: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.VSubSample.FactorNum: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"CalibSensorGeo.VSubSample.FactorDen: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"SbpTblSize: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->SbpTblSize);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"Bad pixel number Count: ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pCalibData->Count);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32)sizeof(FileName),pOutputPrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        TableSize = pCalibData->SbpTblSize;
        (void) CT_fwrite(pCalibData->pSbpTbl, 1,TableSize, Fd);
        (void) CT_fclose(Fd);
    }

    return Rval;
}

static UINT32 AmbaCT_AvmDumpCalibPreCheckData(const AMBA_CT_AVM_CALIB_DATA_s *pData)
{
    UINT32 Rval = CT_OK;
    UINT32 Index = 0U;
    char FileName[128];
    char LineBuf[512];
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    SIZE_t Length;

    AMBA_FS_FILE *Fid;
    AMBA_CT_AVM_SYSTEM_s System;
    static const char * const StatusAbbr[2U] = {"OK","NG"};

    AmbaCT_AvmGetSystem(&System);

    CT_CheckRval(CT_memset(FileName, 0x0, CT_sizeT_to_U32(sizeof(FileName))), "CT_memset", __func__);
    (void) AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/PreCheckResult");
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), System.OutputPrefix);
    (void) AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fid = CT_fopen(FileName, "wb");

    if ( Fid == NULL ) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        for (Index = 0; Index < pData->pDataPreCheck->PreCheckNumber; Index++) {

            AmbaUtility_StringCopy(LineBuf,  (INT32) sizeof(LineBuf), "id: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), Index);
            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), " ChID: ");
            Length = AmbaUtility_StringLength(LineBuf);
            (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(512U - Length), pData->pDataPreCheck->pPreCheckResults[Index].ChID);
            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), " , Status: ");
            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)),  StatusAbbr[pData->pDataPreCheck->pPreCheckResults[Index].Status]);
            (void) AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), " \n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fid);
        }

        (void) CT_fclose(Fid);
    }
    return Rval;
}

static UINT32 AmbaCT_AvmDumpCalibTables(const AMBA_CT_AVM_CALIB_DATA_s *pData)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_AVM_SYSTEM_s System;
    AmbaCT_AvmGetSystem(&System);

    switch (pData->Type) {
    case AMBA_CT_AVM_TYPE_2D:
        Rval |= AmbaCT_Avm2DCalibData2Text(System.OutputPrefix, pData->pData2D);
        break;
    case AMBA_CT_AVM_TYPE_3D:
        Rval |= AmbaCT_Avm3DCalibData2Text(System.OutputPrefix, pData->pData3D);
        break;
    case AMBA_CT_AVM_TYPE_MV:
        Rval |= AmbaCT_AvmMVCalibData2Text(System.OutputPrefix, pData->pDataMv);
        break;
    default:
        Rval = CT_ERR_1;
        break;
    }
    return Rval;
}

static UINT32 AmbaCT_AvmDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_AVM_CALIB_DATA_s AvmData;
    CT_CheckRval(AmbaWrap_memset(&AvmData, 0x0, sizeof(AvmData)), "AmbaWrap_memset", __func__);
    (void)AmbaCT_AvmGetCalibData(&AvmData);

    switch(AvmData.Mode) {
        case AMBA_CT_AVM_CALIB_MODE_PRE_CHECK:
            Rval |= AmbaCT_AvmDumpCalibPreCheckData(&AvmData);
            break;
        case AMBA_CT_AVM_CALIB_MODE_NORMAL:
        case AMBA_CT_AVM_CALIB_MODE_PRE_CALC:
        case AMBA_CT_AVM_CALIB_MODE_FAST:
            Rval |= AmbaCT_AvmDumpCalibTables(&AvmData);
            break;
        default:
            Rval = CT_ERR_1;
            break;
    }

    return Rval;
}

static UINT32 AmbaCT_EmDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_EM_CALIB_DATA_s EmData;
    AMBA_CT_EM_SYSTEM_s System;
    AmbaCT_EmGetSystem(&System);
    CT_CheckRval(AmbaWrap_memset(&EmData, 0x0, sizeof(EmData)), "AmbaWrap_memset", __func__);
    Rval |= AmbaCT_EmGetCalibData(&EmData);
    switch (EmData.Type) {
        case AMBA_CT_EM_TYPE_3IN1:
            Rval |= AmbaCT_Em3in1CalibData2Text(System.OutputPrefix, EmData.pData3in1);
            break;
        case AMBA_CT_EM_TYPE_SINGLE_VIEW:
            Rval |= AmbaCT_EmSvCalibData2Text(System.OutputPrefix, EmData.pDataSingleView);
            break;
        default:
            Rval = CT_ERR_1;
            break;
    }
    return Rval;
}

static UINT32 AmbaCT_LdcDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_LDC_CALIB_DATA_s LdcData;
    AMBA_CT_LDC_SYSTEM_s System;
    AmbaCT_LdcGetSystem(&System);
    Rval |= AmbaCT_LdcGetCalibData(&LdcData);
    Rval |= AmbaCT_LdcCalibData2Text(System.OutputPrefix, LdcData.pCalibData);
    return Rval;
}

static UINT32 AmbaCT_CaDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_CA_CALIB_DATA_s CaData;
    AMBA_CT_CA_SYSTEM_s System;

    AmbaCT_CaGetSystem(&System);
    CT_CheckRval(AmbaWrap_memset(&CaData, 0x0, sizeof(CaData)), "AmbaWrap_memset", __func__);
    Rval |= AmbaCT_CaGetCalibData(&CaData);
    switch (CaData.Method) {
        case AMBA_CAL_CA_SEPARATE:
            Rval |= AmbaCT_CaSeparateCalibData2Text(System.OutputPrefix,CaData.pSeparateCalibData);
            break;
        default:
            Rval |= AmbaCT_CaCalibData2Text(System.OutputPrefix,CaData.pCalibData);
            break;
    }
    return Rval;
}

static UINT32 AmbaCT_LdccDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_LDCC_CALIB_DATA_s LdccData;
    AMBA_CT_LDCC_SYSTEM_s System;
    AmbaCT_LdccGetSystem(&System);
    Rval |= AmbaCT_LdccGetCalibData(&LdccData);
    Rval |= AmbaCT_LdccCalibData2Text(System.OutputPrefix, &LdccData);

    return Rval;
}

static UINT32 AmbaCT_OcDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_OC_TUNER_SYSTEM_s System;
    AMBA_CT_OC_CALIB_DATA_s OcData;
    AmbaCT_OcGetSystemInfo(&System);
    Rval |= AmbaCT_OcGetCalibData(&OcData);
    Rval |= AmbaCT_OcCalibData2Text(System.OutputPrefix, &OcData);
    return Rval;
}


static UINT32 AmbaCT_OcCbDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_OCCB_TUNER_SYSTEM_s System;
    AMBA_CT_OCCB_CALIB_DATA_s OcCbData;
    AMBA_CT_OCCB_TUENR_World_Map_s WorldMapCfg;
    AmbaCT_OcCbGetSystemInfo(&System);
    AmbaCT_OcCbGetWorldMapCfg(&WorldMapCfg);
    Rval |= AmbaCT_OcCbGetCalibData(&OcCbData);
    Rval |= AmbaCT_OcCbCalibData2Text(System.OutputPrefix, &OcCbData,
        WorldMapCfg.Output3DWorldMap);
    return Rval;
}

static UINT32 AmbaCT_BpcDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    AMBA_CT_BPC_TUNER_SYSTEM_s System;
    AMBA_CT_BPC_CALIB_DATA_s BpcData;
    AmbaCT_BpcGetSystemInfo(&System);
    Rval |= AmbaCT_BpcGetCalibData(&BpcData);
    Rval |= AmbaCT_BpcCalibData2Text(System.OutputPrefix, &BpcData);
    return Rval;
}
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
static UINT32 AmbaCT_StereoFeedCornerMap(const char *FilePath, AMBA_CAL_SIZE_s *pCorner2dMapSize,
        AMBA_CAL_POINT_INT_2D_s *pBaseCorner2dIdx, AMBA_CAL_EM_MULTI_POINT_MAP_s *pMultiPointSet)
{
    AMBA_FS_FILE *Fd;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    UINT32 RetVal = CAL_OK;
    AMBA_CAL_WORLD_RAW_POINT_s *pMultiCalibPoints;
    UINT32 MisraU32 = 0U;
    UINT32 ReadSize = 0U;

    Fd = CT_fopen(FilePath, "rb");

    if (Fd == NULL) {
        pNextMsg = CT_LogPutStr(Msg, "Open file fail: ");
        pNextMsg = CT_LogPutStr(pNextMsg, FilePath);
       (void)CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
       RetVal = CAL_ERR_1;
    } else {
        (void)CT_fread(pBaseCorner2dIdx, CT_sizeT_to_U32(sizeof(AMBA_CAL_POINT_INT_2D_s)), 1U, Fd);
        (void)CT_fread(pCorner2dMapSize, CT_sizeT_to_U32(sizeof(AMBA_CAL_SIZE_s)), 1U, Fd);
        pMultiPointSet->MultiCalibPointsNumber = pCorner2dMapSize->Width*pCorner2dMapSize->Height;
        pMultiCalibPoints = pMultiPointSet->pMultiCalibPoints;
        ReadSize = pMultiPointSet->MultiCalibPointsNumber*CT_sizeT_to_U32(sizeof(AMBA_CAL_WORLD_RAW_POINT_s));

        MisraU32 = ReadSize;
        if(MisraU32 > EMIR_CALIB_MULTI_POINT_NUM) {
            MisraU32 = EMIR_CALIB_MULTI_POINT_NUM;
            RetVal = CAL_ERR_1;
        }
        (void)CT_fread(pMultiCalibPoints, MisraU32, 1U, Fd);

        (void)CT_fclose(Fd);
    }

    return RetVal;
}

static inline void AmbaCT_InitCfg2StereoInitCfg(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg, AMBA_CT_STEREO_INIT_CFG_s *pStereoInitCfg)
{
    pStereoInitCfg->pWorkingBuf = pInitCfg->pTunerWorkingBuf;
    pStereoInitCfg->WorkingBufSize = pInitCfg->TunerWorkingBufSize;
    pStereoInitCfg->pCalibWorkingBuf = pInitCfg->Stereo.pCalibWorkingBuf;
    pStereoInitCfg->CalibWorkingBufSize = pInitCfg->Stereo.CalibWorkingBufSize;
    pStereoInitCfg->SvCbMsgReciver = AmbaCT_EmSvCbMsgReciver;
    pStereoInitCfg->SvCbMsgFeedCornerMap = AmbaCT_StereoFeedCornerMap;

}

static UINT32 AmbaCT_StereoInit(const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Index;
    Rule_Info_t Rule_Info;
    AMBA_CT_STEREO_INIT_CFG_s StereoInitCfg;
    AmbaCT_InitCfg2StereoInitCfg(pInitCfg, &StereoInitCfg);
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Create(&pStereoParserObject);
    (void) AmbaCT_GetStereoRuleInfo(&Rule_Info);
    for (Index = 0U; Index < Rule_Info.RegCount; Index++) {
        (void) CT_Parser_Add_Reg(&Rule_Info.RegList[Index], pStereoParserObject);
    }
    (void) CT_Parser_Set_Reglist_Valid(0, pStereoParserObject);
    return AmbaCT_StereoTunerInit(&StereoInitCfg);
}

static UINT32 AmbaCT_StereoLoad(const char *pFileName)
{
    UINT32 Rval = CT_OK;
    char *pBuf = NULL;
    INT32 BufSize = 0;
    CT_SetOpMode(TUNER_DEC);
    (void) CT_Parser_Set_Reglist_Valid(0, pStereoParserObject);
    (void) CT_Parser_Get_LineBuf(pStereoParserObject, &pBuf, &BufSize);
    Rval |= AmbaCT_ParseFile(pFileName, pBuf, pStereoParserObject);

    return Rval;
}

static UINT32 AmbaCT_StereoExecuteFunc(void)
{
    return AmbaCT_StereoExecute();
}

static UINT32 AmbaCT_StereoCalibData2Text(const char *pFileNamePrefix, const char *pFileNamePostfix, AMBA_CAL_EM_SV_DATA_s *pEmSingleViewData)
{
    UINT32 Rval = CT_OK;
    CT_LOG_MSG_s Msg[CT_MAX_MSG_NUM];
    CT_LOG_MSG_s *pNextMsg = Msg;
    char FileName[72] = "";
    char LineBuf[768];
    AMBA_FS_FILE *Fd;
    UINT32 HorIdx, VerIdx;
    SIZE_t Length;

    /*
     * Fix me: Check Folder pFileNamePrefix is exist, or fopen would exception
     */

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName),pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePostfix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".h");
    Fd = CT_fopen(FileName, "w");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define HorGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEmSingleViewData->WarpTbl.HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf),"#define VerGridNum ");
        Length = AmbaUtility_StringLength(LineBuf);
        (void) CT_U32toStr(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEmSingleViewData->WarpTbl.VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT32 calib_warp_header_spec[64] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toHexStr(LineBuf, 768U, pEmSingleViewData->Version);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.HorGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.VerGridNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.TileWidthExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.TileHeightExp);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.StartX);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.StartY);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.Width);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.Height);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.HSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.HSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.VSubSample.FactorNum);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        (void) CT_U32toStr(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), pEmSingleViewData->WarpTbl.CalibSensorGeo.VSubSample.FactorDen);
        AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "0\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec horizontal [HorGridNum * VerGridNum *2] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0U; VerIdx < pEmSingleViewData->WarpTbl.VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0U; HorIdx < pEmSingleViewData->WarpTbl.HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEmSingleViewData->WarpTbl.WarpVector[HorIdx + (VerIdx * pEmSingleViewData->WarpTbl.HorGridNum)].X, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "INT16 calib_warp_table_spec vertical [HorGridNum * VerGridNum *2] =\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "{\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        for (VerIdx = 0; VerIdx < pEmSingleViewData->WarpTbl.VerGridNum; VerIdx++) {
            LineBuf[0] = '\0';
            for (HorIdx = 0; HorIdx < pEmSingleViewData->WarpTbl.HorGridNum; HorIdx++) {
                Length = AmbaUtility_StringLength(LineBuf);
                (void) CT_S16toStrinLength(&LineBuf[Length], CT_sizeT_to_U32(768U - Length), pEmSingleViewData->WarpTbl.WarpVector[HorIdx + (VerIdx * pEmSingleViewData->WarpTbl.HorGridNum)].Y, 6U);
                AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), ",");
            }
            AmbaUtility_StringAppend(LineBuf, CT_sizeT_to_U32(sizeof(LineBuf)), "\n");
            (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        }

        AmbaUtility_StringCopy(LineBuf, (INT32) sizeof(LineBuf), "};\n");
        (void) CT_fwrite(LineBuf, 1, CT_sizeT_to_U32(AmbaUtility_StringLength(LineBuf)), Fd);
        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePostfix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_RawEncode.bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fwrite(pEmSingleViewData->WarpTbl.WarpVector, 1, CT_sizeT_to_U32(sizeof(pEmSingleViewData->WarpTbl.WarpVector)), Fd);
        (void) CT_fclose(Fd);
    }


    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "StatusTbl_");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePostfix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), ".csv");
    Rval |= AmbaCT_EmrGridStatus2Text(FileName, AMBA_CAL_EM_CAM_BACK);

    AmbaUtility_StringCopy(FileName, (INT32) sizeof(FileName), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "/");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePrefix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_");
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), pFileNamePostfix);
    AmbaUtility_StringAppend(FileName, CT_sizeT_to_U32(sizeof(FileName)), "_SDK.bin");
    Fd = CT_fopen(FileName, "wb");
    if (Fd == NULL) {
        Rval |= CT_ERR_1;
        pNextMsg = CT_LogPutStr(Msg, "Open");
        pNextMsg = CT_LogPutStr(pNextMsg, FileName);
        pNextMsg = CT_LogPutStr(pNextMsg, "fail");
        Rval |= CT_LogPackMsg(CT_LOG_LEVEL_ERROR, Msg, pNextMsg);
    } else {
        (void) CT_fwrite(pEmSingleViewData, 1, CT_sizeT_to_U32(sizeof(AMBA_CAL_EM_SV_DATA_s)), Fd);
        (void) CT_fclose(Fd);
    }
    return Rval;
}

static UINT32 AmbaCT_StereoDumpCalibData(void)
{
    UINT32 Rval = CT_OK;
    UINT8 CameraId = 0U;
    AMBA_CT_STEREO_SYSTEM_s System;
    AmbaCT_StereoGetSystem(&System);
    do {
        AMBA_CT_STEREO_CALIB_DATA_s StereoData;
        AMBA_CT_STEREO_OUTPUT_CFG_s OutputCfg;
        AmbaCT_StereoGetOutputCfg(CameraId, &OutputCfg);
        Rval |= AmbaCT_StereoGetCalibData(CameraId, &StereoData);

        Rval |= AmbaCT_StereoCalibData2Text(System.OutputPrefix, OutputCfg.OutputPostfix, StereoData.pDataSingleView);

        CameraId ++;
    } while(CameraId < System.CameraNum);
    return Rval;
}

#endif


static AMBA_CT_Handlr_s CT_Func = {
    .pInitFunc = AmbaCT_DummyInit,
    .pLoadFunc = AmbaCT_DummyLoad,
    .pExecuteFunc = AmbaCT_DummyExecute,
    .pDumpCalibDataFunc = AmbaCT_DummyDump,
};


UINT32 AmbaCT_GetBufSize(AMBA_CT_TYPE_e Type, SIZE_t *pSize)
{
    UINT32 Rval = CT_OK;
    SIZE_t RawFileSize;
    switch (Type) {
    case AMBA_CT_TYPE_1D_VIG:
        AmbaCT_1DVigGetWorkingBufSize(pSize);
        if (CalibTunerCfg.Vig1d.Resolution > 16U) {
            RawFileSize = (SIZE_t)CalibTunerCfg.Vig1d.MaxImageSize.Width * (SIZE_t)CalibTunerCfg.Vig1d.MaxImageSize.Height * sizeof(UINT32);
        } else {
            RawFileSize = (SIZE_t)CalibTunerCfg.Vig1d.MaxImageSize.Width * (SIZE_t)CalibTunerCfg.Vig1d.MaxImageSize.Height * sizeof(UINT16);
        }
        *pSize = *pSize + RawFileSize;
        break;
    case AMBA_CT_TYPE_AVM:
        AmbaCT_AvmGetWorkingBufSize(pSize);
        break;
    case AMBA_CT_TYPE_EMIRROR:
        AmbaCT_EmGetWorkingBufSize(pSize);
        break;
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
    case AMBA_CT_TYPE_STEREO:
        AmbaCT_StereoGetWorkingBufSize(pSize);
        break;
#endif
    case AMBA_CT_TYPE_LDC:
        AmbaCT_LdcGetWorkingBufSize(pSize);
        break;
    case AMBA_CT_TYPE_LDCC:
        AmbaCT_LdccGetWorkingBufSize(pSize);
        break;
    case AMBA_CT_TYPE_BPC:
        AmbaCT_BpcGetWorkingBufSize(pSize);
        if (CalibTunerCfg.Bpc.Resolution > 16U) {
            RawFileSize = (SIZE_t)CalibTunerCfg.Bpc.MaxImageSize.Width * (SIZE_t)CalibTunerCfg.Bpc.MaxImageSize.Height * sizeof(UINT32);
        } else {
            RawFileSize = (SIZE_t)CalibTunerCfg.Bpc.MaxImageSize.Width * (SIZE_t)CalibTunerCfg.Bpc.MaxImageSize.Height * sizeof(UINT16);
        }
        *pSize = *pSize + RawFileSize;
        break;
    case AMBA_CT_TYPE_OC:
        AmbaCT_OcGetWorkingBufSize(pSize);
        break;
    case AMBA_CT_TYPE_OCCB:
        AmbaCT_OcCbGetWorkingBufSize(pSize);
        break;
    case AMBA_CT_TYPE_CA:
        AmbaCT_CaGetWorkingBufSize(pSize);
        break;
    default:
        Rval = CT_ERR_1;
        break;
    }
    return Rval;
}

UINT32 AmbaCT_Cfg(AMBA_CT_TYPE_e Type, const AMBA_CT_CONFIG_s *pCfg)
{
    UINT32 Rval = CT_OK;
    switch (Type) {
    case AMBA_CT_TYPE_1D_VIG:
        {
            CalibTunerCfg.Vig1d.MaxImageSize = pCfg->Type.Vig1d.MaxImageSize;
            CalibTunerCfg.Vig1d.Resolution = pCfg->Type.Vig1d.Resolution;
            break;
        }
    case AMBA_CT_TYPE_VIG:
    case AMBA_CT_TYPE_BPC:
        {
            CalibTunerCfg.Bpc.MaxImageSize = pCfg->Type.Bpc.MaxImageSize;
            CalibTunerCfg.Bpc.Resolution = pCfg->Type.Bpc.Resolution;
            break;
        }
    case AMBA_CT_TYPE_AVM:
    case AMBA_CT_TYPE_EMIRROR:
    case AMBA_CT_TYPE_LDC:
    case AMBA_CT_TYPE_LDCC:
    case AMBA_CT_TYPE_OC:
    case AMBA_CT_TYPE_OCCB:
    case AMBA_CT_TYPE_CA:
    default:
        {
            AmbaPrint_PrintStr5("Skip", NULL, NULL, NULL, NULL, NULL);
            break;
        }
    }
    return Rval;
}

UINT32 AmbaCT_Init(AMBA_CT_TYPE_e Type, const AMBA_CT_INITIAL_CONFIG_s *pInitCfg)
{
    UINT32 Rval = CT_OK;
    switch (Type) {
    case AMBA_CT_TYPE_1D_VIG:
        CT_Func.pInitFunc = AmbaCT_1DVigInit;
        CT_Func.pLoadFunc = AmbaCT_1DVigLoad;
        CT_Func.pExecuteFunc = AmbaCT_1DVigExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_1DVigDumpCalibData;
        break;
    case AMBA_CT_TYPE_AVM:
        CT_Func.pInitFunc = AmbaCT_AvmInit;
        CT_Func.pLoadFunc = AmbaCT_AvmLoad;
        CT_Func.pExecuteFunc = AmbaCT_AvmExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_AvmDumpCalibData;
        break;
    case AMBA_CT_TYPE_EMIRROR:
        CT_Func.pInitFunc = AmbaCT_EmInit;
        CT_Func.pLoadFunc = AmbaCT_EmLoad;
        CT_Func.pExecuteFunc = AmbaCT_EmExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_EmDumpCalibData;
        break;
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV2FS)
    case AMBA_CT_TYPE_STEREO:
        CT_Func.pInitFunc = AmbaCT_StereoInit;
        CT_Func.pLoadFunc = AmbaCT_StereoLoad;
        CT_Func.pExecuteFunc = AmbaCT_StereoExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_StereoDumpCalibData;
        break;
#endif
    case AMBA_CT_TYPE_LDC:
        CT_Func.pInitFunc = AmbaCT_LdcInit;
        CT_Func.pLoadFunc = AmbaCT_LdcLoad;
        CT_Func.pExecuteFunc = AmbaCT_LdcExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_LdcDumpCalibData;
        break;
    case AMBA_CT_TYPE_LDCC:
        CT_Func.pInitFunc = AmbaCT_LdccInit;
        CT_Func.pLoadFunc = AmbaCT_LdccLoad;
        CT_Func.pExecuteFunc = AmbaCT_LdccExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_LdccDumpCalibData;
        break;
    case AMBA_CT_TYPE_BPC:
        CT_Func.pInitFunc = AmbaCT_BpcInit;
        CT_Func.pLoadFunc = AmbaCT_BpcLoad;
        CT_Func.pExecuteFunc = AmbaCT_BpcExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_BpcDumpCalibData;
        break;
    case AMBA_CT_TYPE_OC:
        CT_Func.pInitFunc = AmbaCT_OcInit;
        CT_Func.pLoadFunc = AmbaCT_OcLoad;
        CT_Func.pExecuteFunc = AmbaCT_OcExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_OcDumpCalibData;
        break;
    case AMBA_CT_TYPE_CA:
        CT_Func.pInitFunc = AmbaCT_CaInit;
        CT_Func.pLoadFunc = AmbaCT_CaLoad;
        CT_Func.pExecuteFunc = AmbaCT_CaExecuteFunc;
        CT_Func.pDumpCalibDataFunc = AmbaCT_CaDumpCalibData;
        break;
    case AMBA_CT_TYPE_OCCB:
         CT_Func.pInitFunc = AmbaCT_OcCbInit;
         CT_Func.pLoadFunc = AmbaCT_OcCbLoad;
         CT_Func.pExecuteFunc = AmbaCT_OcCbExecuteFunc;
         CT_Func.pDumpCalibDataFunc = AmbaCT_OcCbDumpCalibData;
         break;
    default:
        CT_Func.pInitFunc = AmbaCT_DummyInit;
        CT_Func.pLoadFunc = AmbaCT_DummyLoad;
        CT_Func.pExecuteFunc = AmbaCT_DummyExecute;
        CT_Func.pDumpCalibDataFunc = AmbaCT_DummyDump;
        break;
    }
    Rval |= CT_Func.pInitFunc(pInitCfg);
    return Rval;
}

UINT32 AmbaCT_Load(const char *pTunerFileName)
{
    return CT_Func.pLoadFunc(pTunerFileName);
}

UINT32 AmbaCT_Execute(void)
{
    return CT_Func.pExecuteFunc();
}

UINT32 AmbaCT_DumpCalibData(void)
{
    return CT_Func.pDumpCalibDataFunc();
}
