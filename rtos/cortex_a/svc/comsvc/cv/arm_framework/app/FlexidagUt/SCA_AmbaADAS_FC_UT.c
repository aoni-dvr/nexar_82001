/**
 *  @file SCA_AmbaADAS_FC_UT.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of AmbaADAS FC Unit Test
 *
 */

#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaKAL.h"

#include "AdasUT_Common.h"
#include "Util_CanbusHdlr.h"
#include "Util_ReadCvoutHdlr.h"
#include "Util_PrepareCalib.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                          /////
/////                                        CONSTANT                                          /////
/////                                                                                          /////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define FILE_NAME_LEN (256U)
#define SEG_BUFFER_SIZE (655360U)   //1280x512 = 655360
#define MK_BUFFER_SIZE (1638400U)   // 1280*640*2


////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                          /////
/////                                       TYPE DEFINE                                        /////
/////                                                                                          /////
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct MAIN_PARA_s {
    UINT32 SimType;
    char SegLogFN[FILE_NAME_LEN];
    UINT32 SegVersion;                  // 0: Seg37, 1: Seg39
    char ODLogFN[FILE_NAME_LEN];
    UINT32 ODVersion;                   // 0: OD36, 1: OD37
    char OD3DLogFN[FILE_NAME_LEN];
    char ODMKLLogFN[FILE_NAME_LEN];
    char ODMKSLogFN[FILE_NAME_LEN];
    char CalibFN[FILE_NAME_LEN];
    DOUBLE FocalLength;
    char CANBusFN[FILE_NAME_LEN];
    UINT32 Channel;
    UINT32 ProcStep;
    char ADASLogFN[FILE_NAME_LEN];
    UINT32 DevMode;
} MAIN_PARA_t;


////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                          /////
/////                                         GLOBAL                                           /////
/////                                                                                          /////
////////////////////////////////////////////////////////////////////////////////////////////////////

static MAIN_PARA_t g_Para = {0};
static UINT8 SegBuf[4][SEG_BUFFER_SIZE];
static UINT8 MklBuf[MK_BUFFER_SIZE];
static UINT8 MksBuf[MK_BUFFER_SIZE];
static REF_FLOW_FC_NN_INFO_s FcNNInfo[AMBA_OD_2DBBX_MAX_BBX_NUM];

static AMBA_FS_FILE *g_FP = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                          /////
/////                                   FUNCTION DECLARATION                                   /////
/////                                                                                          /////
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                          /////
/////                                   FUNCTION DEFINITION                                    /////
/////                                                                                          /////
////////////////////////////////////////////////////////////////////////////////////////////////////

static UINT32 _FillMkStartXY(const RF_LOG_OD_2DBBOX_HEADER_s *pOdHead, const AMBA_OD_2DBBX_LIST_s *pBbx2D, UTIL_NNMK_CVOUT_s *pMk)
{
    UINT32 Rval = ADAS_ERR_NONE;
    UINT32 ObjId;
    DOUBLE Tmp;

    for (UINT32 i = 0U; i < pMk->Header.ObjNum; i++) {
        ObjId = pMk->Data[i].ObjIdx;
        Tmp = (DOUBLE)pBbx2D->Bbx[ObjId].X / (DOUBLE)pOdHead->Width * (DOUBLE)pOdHead->CropInfo.SrcW;
        pMk->MkBbx[i].ObjStartX = (UINT32)Tmp - (UINT32)pOdHead->CropInfo.RoiX;
        Tmp = (DOUBLE)pBbx2D->Bbx[ObjId].Y / (DOUBLE)pOdHead->Height * (DOUBLE)pOdHead->CropInfo.SrcH;
        pMk->MkBbx[i].ObjStartY = (UINT32)Tmp - (UINT32)pOdHead->CropInfo.RoiY;
    }

    return Rval;
}

static UINT32 _NNMkMerge(const UTIL_NNMK_CVOUT_s *pMklIn, const UTIL_NNMK_CVOUT_s *pMksIn, UTIL_NNMK_CVOUT_s *pMkOut)
{
    UINT32 Rval = ADAS_ERR_NONE;

    (void)AmbaWrap_memcpy(pMkOut, pMklIn, sizeof(UTIL_NNMK_CVOUT_s));
    pMkOut->Header.ObjNum = pMklIn->Header.ObjNum + pMksIn->Header.ObjNum;

    for (UINT32 i = 0U; i < pMksIn->Header.ObjNum; i++) {
        (void)AmbaWrap_memcpy(&(pMkOut->Data[i+pMklIn->Header.ObjNum]), &(pMksIn->Data[i]), sizeof(RF_LOG_OD_2ND_DATA_s));
        (void)AmbaWrap_memcpy(&(pMkOut->MkBbx[i+pMklIn->Header.ObjNum]), &(pMksIn->MkBbx[i]), sizeof(RF_LOG_OD_2ND_OBJ_SEG_INFO));
        (void)AmbaWrap_memcpy(&(pMkOut->Addr[i+pMklIn->Header.ObjNum]), &(pMksIn->Addr[i]), sizeof(UINT8*));
    }

    return Rval;
}

static UINT32 _DataFmtTrans_Cvout2Flow(const UTIL_NN3D_CVOUT_s *p3DIn, const UTIL_NNMK_CVOUT_s *pMkIn, REF_FLOW_FC_NN_DATA_s *pNNOut)
{
    UINT32 Rval = ADAS_ERR_NONE;

#if 0
    AmbaPrint_PrintUInt5("[_MergeNNData] 3D_ObjNum = %d", p3DIn->Header.ObjNum, 0U, 0U, 0U, 0U);
    for (UINT32 i = 0U; i < p3DIn->Header.ObjNum; i++) {
        const RF_LOG_OD_2ND_DATA_s *pNNData = &(p3DIn->Data[i]);
        const AMBA_OD_3DBBX_s *pNN3D = &(p3DIn->ThreeDBbx[i]);
        AmbaPrint_PrintUInt5("   [%d] ObjIdx = %d, DataType = %d", i, pNNData->ObjIdx, pNNData->DataType, 0U, 0U);
        AmbaPrint_PrintUInt5("        (clsId, field, track]) = (%d, %d, %d)", pNN3D->clsId, pNN3D->field, pNN3D->track, 0U, 0U);
        AmbaPrint_PrintInt5("         (upper_left_x, upper_left_y, bottom_right_x, bottom_right_y) = (%d, %d, %d, %d)",
                            (INT32)pNN3D->upper_left_x, (INT32)pNN3D->upper_left_y, (INT32)pNN3D->bottom_right_x, (INT32)pNN3D->bottom_right_y, 0);
        AmbaPrint_PrintInt5("         (w, h, l, alpha) = (%d, %d, %d, %d)", (INT32)pNN3D->w, (INT32)pNN3D->h, (INT32)pNN3D->l, (INT32)pNN3D->alpha, 0);
    }

    AmbaPrint_PrintUInt5("[_MergeNNData] MK_ObjNum = %d", pMkIn->Header.ObjNum, 0U, 0U, 0U, 0U);
    for (UINT32 i = 0U; i < pMkIn->Header.ObjNum; i++) {
        const RF_LOG_OD_2ND_DATA_s *pNNData = &(pMkIn->Data[i]);
        const RF_LOG_OD_2ND_OBJ_SEG_INFO *pNNMk = &(pMkIn->MkBbx[i]);
        AmbaPrint_PrintUInt5("   [%d] ObjIdx = %d, DataType = %d", i, pNNData->ObjIdx, pNNData->DataType, 0U, 0U);
        AmbaPrint_PrintUInt5("        (X,Y,W,H,P) = (%d,%d,%d,%d,%d)", pNNMk->ObjStartX, pNNMk->ObjStartY, pNNMk->ObjWidth, pNNMk->ObjHeight, pNNMk->ObjPitch);
    }
#endif

    UINT32 ObjNum = 0U;
    UINT32 MatchFlag;

    (void)AmbaWrap_memset(pNNOut->pObjNNInfoList, 0, (SIZE_t)AMBA_OD_2DBBX_MAX_BBX_NUM* sizeof(REF_FLOW_FC_NN_INFO_s));

    for (UINT32 i = 0U; i < p3DIn->Header.ObjNum; i++) {
        pNNOut->pObjNNInfoList[ObjNum].ObjId = p3DIn->Data[i].ObjIdx;
        pNNOut->pObjNNInfoList[ObjNum].ObjNNDataFlag += 2U;
        (void)AmbaWrap_memcpy(&(pNNOut->pObjNNInfoList[ObjNum].ObjNN3DInfo), &(p3DIn->ThreeDBbx[i]), sizeof(AMBA_OD_3DBBX_s));
        ObjNum++;
    }
    pNNOut->ObjNum = ObjNum;

    for (UINT32 i = 0U; i < pMkIn->Header.ObjNum; i++) {
        MatchFlag = 0U;
        for (UINT32 j = 0U; j < pNNOut->ObjNum; j++) {
            if (pNNOut->pObjNNInfoList[j].ObjId == pMkIn->Data[i].ObjIdx) {
                pNNOut->pObjNNInfoList[j].ObjNNDataFlag += 1U;
                pNNOut->pObjNNInfoList[j].ObjNNSegInfo.ObjStartX = (UINT16)pMkIn->MkBbx[i].ObjStartX;
                pNNOut->pObjNNInfoList[j].ObjNNSegInfo.ObjStartY = (UINT16)pMkIn->MkBbx[i].ObjStartY;
                pNNOut->pObjNNInfoList[j].ObjNNSegInfo.ObjPitch = (UINT16)pMkIn->MkBbx[i].ObjPitch;
                pNNOut->pObjNNInfoList[j].ObjNNSegInfo.ObjWidth = (UINT16)pMkIn->MkBbx[i].ObjWidth;
                pNNOut->pObjNNInfoList[j].ObjNNSegInfo.ObjHeight = (UINT16)pMkIn->MkBbx[i].ObjHeight;
                pNNOut->pObjNNInfoList[j].ObjNNSegInfo.pObjTable = pMkIn->Addr[i];
                MatchFlag = 1U;
                break;
            }
        }
        if (MatchFlag == 0U) {
            pNNOut->pObjNNInfoList[ObjNum].ObjId = pMkIn->Data[i].ObjIdx;
            pNNOut->pObjNNInfoList[ObjNum].ObjNNDataFlag += 1U;
            pNNOut->pObjNNInfoList[ObjNum].ObjNNSegInfo.ObjStartX = (UINT16)pMkIn->MkBbx[i].ObjStartX;
            pNNOut->pObjNNInfoList[ObjNum].ObjNNSegInfo.ObjStartY = (UINT16)pMkIn->MkBbx[i].ObjStartY;
            pNNOut->pObjNNInfoList[ObjNum].ObjNNSegInfo.ObjPitch = (UINT16)pMkIn->MkBbx[i].ObjPitch;
            pNNOut->pObjNNInfoList[ObjNum].ObjNNSegInfo.ObjWidth = (UINT16)pMkIn->MkBbx[i].ObjWidth;
            pNNOut->pObjNNInfoList[ObjNum].ObjNNSegInfo.ObjHeight = (UINT16)pMkIn->MkBbx[i].ObjHeight;
            pNNOut->pObjNNInfoList[ObjNum].ObjNNSegInfo.pObjTable = pMkIn->Addr[i];
            ObjNum++;
        }
    }
    pNNOut->ObjNum = ObjNum;

    return Rval;
}

static UINT32 _GetNexOD37(AMBA_OD_2DBBX_LIST_s* pBbx2dList, RF_LOG_OD_2DBBOX_HEADER_s* pODHeader, REF_FLOW_FC_NN_DATA_s* pFcNNData)
{
    UINT32 Rval = 0U;
    UINT32 Rval3D = 0U;
    UINT32 RvalMkl = 0U;
    UINT32 RvalMks = 0U;
    static UINT32 InitFlag = 0U;
    static AMBA_FS_FILE *FP2d = NULL;
    static AMBA_FS_FILE *FP3d = NULL;
    static AMBA_FS_FILE *FPmkl = NULL;
    static AMBA_FS_FILE *FPmks = NULL;
    static UINT32 FrameIdx[4] = {0xFFU, 0xFFU, 0xFFU, 0xFFU};
    static UINT32 EndFileFlag[4] = {0U, 0U, 0U, 0U};
    static UTIL_NN3D_CVOUT_s NN3dData;
    static UTIL_NNMK_CVOUT_s NNMklData;
    static UTIL_NNMK_CVOUT_s NNMksData;
    UTIL_NNMK_CVOUT_s NNMkData;

    if (InitFlag == 0U) {
        AmbaPrint_PrintStr5("Open file: %s...", g_Para.ODLogFN, NULL, NULL, NULL, NULL);
        (void)AmbaFS_FileOpen(g_Para.ODLogFN, "rb", &FP2d);
        if (FP2d == NULL) {
            AmbaPrint_PrintStr5("[Error] %s, Open 2D logger file error!", __func__, NULL, NULL, NULL, NULL);
            Rval = 1U;
        } else {
            InitFlag = 1U;
        }

        AmbaPrint_PrintStr5("Open file: %s...", g_Para.OD3DLogFN, NULL, NULL, NULL, NULL);
        (void)AmbaFS_FileOpen(g_Para.OD3DLogFN, "rb", &FP3d);
        if (FP3d == NULL) {
            AmbaPrint_PrintStr5("[Warning] %s, Without 3D logger file!", __func__, NULL, NULL, NULL, NULL);
            Rval3D = 1U;
        }

        AmbaPrint_PrintStr5("Open file: %s...", g_Para.ODMKLLogFN, NULL, NULL, NULL, NULL);
        (void)AmbaFS_FileOpen(g_Para.ODMKLLogFN, "rb", &FPmkl);
        if (FPmkl == NULL) {
            AmbaPrint_PrintStr5("[Warning] %s, Without Mkl logger file!", __func__, NULL, NULL, NULL, NULL);
            RvalMkl = 1U;
        }

        AmbaPrint_PrintStr5("Open file: %s...", g_Para.ODMKSLogFN, NULL, NULL, NULL, NULL);
        (void)AmbaFS_FileOpen(g_Para.ODMKSLogFN, "rb", &FPmks);
        if (FPmks == NULL) {
            AmbaPrint_PrintStr5("[Warning] %s, Without Mks logger file!", __func__, NULL, NULL, NULL, NULL);
            RvalMks = 1U;
        }
    }

    if ((Rval == 0U) && (FP2d != NULL)) {
        UINT32 U[2] = {0U, 0U};

        (void)AmbaWrap_memset(&NN3dData, 0, sizeof(UTIL_NN3D_CVOUT_s));
        (void)AmbaWrap_memset(&NNMklData, 0, sizeof(UTIL_NNMK_CVOUT_s));
        (void)AmbaWrap_memset(&NNMksData, 0, sizeof(UTIL_NNMK_CVOUT_s));

        Rval = UtilReadCvoutHdlr_GetNexOD(pBbx2dList, pODHeader, FP2d, &FrameIdx[0], &EndFileFlag[0]);
        if (Rval != 0U) {
            AmbaPrint_PrintStr5("[Error] %s, UtilReadCvoutHdlr_GetNexOD fail!", __func__, NULL, NULL, NULL, NULL);
        }

        if ((Rval == 0U) && (FP3d != NULL)) {
            if (((FrameIdx[1] == 0xFFU) || (FrameIdx[1] < FrameIdx[0]))) {
                Rval3D = UtilReadCvoutHdlr_GetNex3D(FP3d, &NN3dData, &FrameIdx[1], &EndFileFlag[1]);
            }
        }

        if ((Rval == 0U) && (FPmkl != NULL)) {
            if (((FrameIdx[2] == 0xFFU) || (FrameIdx[2] < FrameIdx[0]))) {
                RvalMkl = UtilReadCvoutHdlr_GetNexMk(FPmkl, &MklBuf[0], RF_COM_DFMT_MKLBBOX, MK_BUFFER_SIZE, &U[0], &NNMklData, &FrameIdx[2], &EndFileFlag[2]);
            }
        }

        if ((Rval == 0U) && (FPmks != NULL)) {
            if (((FrameIdx[3] == 0xFFU) || (FrameIdx[3] < FrameIdx[0]))) {
                RvalMks = UtilReadCvoutHdlr_GetNexMk(FPmks, &MksBuf[0], RF_COM_DFMT_MKSBBOX, MK_BUFFER_SIZE, &U[1], &NNMksData, &FrameIdx[3], &EndFileFlag[3]);
            }
        }

        if ((Rval == 0U) && ((Rval3D == 0U) || (RvalMkl == 0U) || (RvalMks == 0U))) {
            if ((FrameIdx[1] == FrameIdx[0]) && ((FrameIdx[2] == FrameIdx[0]) && (FrameIdx[3] == FrameIdx[0]))) {
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMklData);
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMksData);
                (void)_NNMkMerge(&NNMklData, &NNMksData, &NNMkData);
                (void)_DataFmtTrans_Cvout2Flow(&NN3dData, &NNMkData, pFcNNData);
            } else if ((FrameIdx[1] == FrameIdx[0]) && ((FrameIdx[2] != FrameIdx[0]) && (FrameIdx[3] == FrameIdx[0]))) {
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMksData);
                (void)AmbaWrap_memcpy(&NNMkData, &NNMksData, sizeof(UTIL_NNMK_CVOUT_s));
                (void)_DataFmtTrans_Cvout2Flow(&NN3dData, &NNMkData, pFcNNData);
            } else if ((FrameIdx[1] == FrameIdx[0]) && ((FrameIdx[2] == FrameIdx[0]) && (FrameIdx[3] != FrameIdx[0]))) {
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMklData);
                (void)AmbaWrap_memcpy(&NNMkData, &NNMklData, sizeof(UTIL_NNMK_CVOUT_s));
                (void)_DataFmtTrans_Cvout2Flow(&NN3dData, &NNMkData, pFcNNData);
            } else if ((FrameIdx[1] == FrameIdx[0]) && ((FrameIdx[2] != FrameIdx[0]) && (FrameIdx[3] != FrameIdx[0]))) {
                (void)_DataFmtTrans_Cvout2Flow(&NN3dData, &NNMkData, pFcNNData);
            } else if ((FrameIdx[1] != FrameIdx[0]) && ((FrameIdx[2] == FrameIdx[0]) && (FrameIdx[3] == FrameIdx[0]))) {
                UTIL_NN3D_CVOUT_s InvalidNN3d;
                (void)AmbaWrap_memset(&InvalidNN3d, 0, sizeof(UTIL_NN3D_CVOUT_s));
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMklData);
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMksData);
                (void)_NNMkMerge(&NNMklData, &NNMksData, &NNMkData);
                (void)_DataFmtTrans_Cvout2Flow(&InvalidNN3d, &NNMkData, pFcNNData);
            } else if ((FrameIdx[1] != FrameIdx[0]) && ((FrameIdx[2] != FrameIdx[0]) && (FrameIdx[3] == FrameIdx[0]))) {
                UTIL_NN3D_CVOUT_s InvalidNN3d;
                (void)AmbaWrap_memset(&InvalidNN3d, 0, sizeof(UTIL_NN3D_CVOUT_s));
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMksData);
                (void)AmbaWrap_memcpy(&NNMkData, &NNMksData, sizeof(UTIL_NNMK_CVOUT_s));
                (void)_DataFmtTrans_Cvout2Flow(&InvalidNN3d, &NNMkData, pFcNNData);
            } else if ((FrameIdx[1] != FrameIdx[0]) && ((FrameIdx[2] == FrameIdx[0]) && (FrameIdx[3] != FrameIdx[0]))) {
                UTIL_NN3D_CVOUT_s InvalidNN3d;
                (void)AmbaWrap_memset(&InvalidNN3d, 0, sizeof(UTIL_NN3D_CVOUT_s));
                (void)_FillMkStartXY(pODHeader, pBbx2dList, &NNMklData);
                (void)AmbaWrap_memcpy(&NNMkData, &NNMklData, sizeof(UTIL_NNMK_CVOUT_s));
                (void)_DataFmtTrans_Cvout2Flow(&InvalidNN3d, &NNMkData, pFcNNData);
            } else {
                // do nothing
            }
        }

    }

    if (EndFileFlag[1] == 1U) {
        AmbaPrint_PrintStr5("End of file %s\n", g_Para.OD3DLogFN, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintStr5("Close file %s...\n", g_Para.OD3DLogFN, NULL, NULL, NULL, NULL);
        AmbaFS_FileClose(FP3d);
        FP3d = NULL;
    }

    if (EndFileFlag[2] == 1U) {
        AmbaPrint_PrintStr5("End of file %s\n", g_Para.ODMKLLogFN, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintStr5("Close file %s...\n", g_Para.ODMKLLogFN, NULL, NULL, NULL, NULL);
        AmbaFS_FileClose(FPmkl);
        FPmkl = NULL;
    }

    if (EndFileFlag[3] == 1U) {
        AmbaPrint_PrintStr5("End of file %s\n", g_Para.ODMKSLogFN, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintStr5("Close file %s...\n", g_Para.ODMKSLogFN, NULL, NULL, NULL, NULL);
        AmbaFS_FileClose(FPmks);
        FPmks = NULL;
    }

    if (EndFileFlag[0] == 1U) {
        AmbaPrint_PrintStr5("End of file %s\n", g_Para.ODLogFN, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintStr5("Close file %s...\n", g_Para.ODLogFN, NULL, NULL, NULL, NULL);
        AmbaFS_FileClose(FP2d);
        FP2d = NULL;
        InitFlag = 0U;
        Rval = 1U;
    }

    return Rval;
}



static UINT32 _GetNextSeg39(AMBA_SEG_BUF_INFO_s SegBufInfo[4])
{
    UINT32 Rval;
    static UINT32 Init = 0U;
    static AMBA_FS_FILE* fp = NULL;

    if (Init == 0U) {
        Rval = AmbaFS_FileOpen(g_Para.SegLogFN, "rb", &fp);
        if(Rval !=  0U) {
            AmbaPrint_PrintStr5("%s, AmbaFS_FileOpen failed. (file: %s) ", __func__, g_Para.SegLogFN, NULL, NULL, NULL);
        }
        Init = 1U;
    }

    if (fp != NULL) {
        UINT32 frameIdx;
        UINT32 EndFileFlag = 0U;

        SegBufInfo[0].pBuf = SegBuf[0];
        SegBufInfo[1].pBuf = SegBuf[1];
        SegBufInfo[2].pBuf = SegBuf[2];
        SegBufInfo[3].pBuf = SegBuf[3];

        Rval = UtilReadCvoutHdlr_GetNexSeg(&SegBufInfo[0], RF_COM_DFMT_SEG, SEG_BUFFER_SIZE, fp, &frameIdx, &EndFileFlag);
        if (Rval != 0U) {
            AmbaPrint_PrintStr5("%s, UtilReadCvoutHdlr_GetNexSeg failed.", __func__, NULL, NULL, NULL, NULL);
        }

        if ((Rval == 0U) && (EndFileFlag == 0U)) {
            Rval = UtilReadCvoutHdlr_GetNexSeg(&SegBufInfo[1], RF_COM_DFMT_SEG_COLOR, SEG_BUFFER_SIZE, fp, &frameIdx, &EndFileFlag);
            if (Rval != 0U) {
                AmbaPrint_PrintStr5("%s, UtilReadCvoutHdlr_GetNexSeg failed.", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if ((Rval == 0U) && (EndFileFlag == 0U)) {
            Rval = UtilReadCvoutHdlr_GetNexSeg(&SegBufInfo[2], RF_COM_DFMT_SEG_TYPE, SEG_BUFFER_SIZE, fp, &frameIdx, &EndFileFlag);
            if (Rval != 0U) {
                AmbaPrint_PrintStr5("%s, UtilReadCvoutHdlr_GetNexSeg failed.", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if ((Rval == 0U) && (EndFileFlag == 0U)) {
            Rval = UtilReadCvoutHdlr_GetNexSeg(&SegBufInfo[3], RF_COM_DFMT_SEG_INSTANCE, SEG_BUFFER_SIZE, fp, &frameIdx, &EndFileFlag);
            if (Rval != 0U) {
                AmbaPrint_PrintStr5("%s, UtilReadCvoutHdlr_GetNexSeg failed.", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if (EndFileFlag == 1U) {
            AmbaPrint_PrintStr5("End of file %s\n", g_Para.SegLogFN, NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("Close file %s...\n", g_Para.SegLogFN, NULL, NULL, NULL, NULL);
            AmbaFS_FileClose(fp);
            Init = 0U;
            fp = NULL;
            Rval = 1U;
        }

    } else {
        AmbaPrint_PrintStr5("%s, open file failed", __func__, NULL, NULL, NULL, NULL);
        Rval = 1U;
    }

    return Rval;
}


static void DumpResult(RF_LOG_HEADER_s *pHeader)
{
    if (g_FP != NULL) {
        UINT32 s;

        (void)AmbaFS_FileWrite(&pHeader->Magic, sizeof(UINT32), 1, g_FP, &s);
        (void)AmbaFS_FileWrite(&pHeader->HdrVer, sizeof(UINT32), 1, g_FP, &s);
        (void)AmbaFS_FileWrite(&pHeader->DataFmt, sizeof(UINT32), 1, g_FP, &s);
        (void)AmbaFS_FileWrite(&pHeader->Count, sizeof(UINT32), 1, g_FP, &s);
        (void)AmbaFS_FileWrite(&pHeader->Channel, sizeof(UINT32), 1, g_FP, &s);
        (void)AmbaFS_FileWrite(&pHeader->CapPTS, sizeof(UINT32), 1, g_FP, &s);
        (void)AmbaFS_FileWrite(&pHeader->DataSize, sizeof(UINT32), 1, g_FP, &s);
        (void)AmbaFS_FileWrite(pHeader->pDataAddr, pHeader->DataSize, 1, g_FP, &s);
    }
}


static UINT32 GetResult(RF_LOG_HEADER_s *pHeader)
{
    UINT32 DataFmt = pHeader->DataFmt;

    /** Get LD result */
    if (DataFmt == RF_LD_DFMT_SR_LANE_OUT) {
        DumpResult(pHeader);
    } else if (DataFmt == RF_LD_DFMT_LDWS_OUT) {
        DumpResult(pHeader);

        /** Get FC Result */
    } else if (DataFmt == RF_FC_DFMT_2DBBX_OUT) {
        DumpResult(pHeader);
    } else if (DataFmt == RF_FC_DFMT_SR) {
        DumpResult(pHeader);
    } else if (DataFmt == RF_FC_DFMT_FC_OUT_V2) {
        DumpResult(pHeader);
    } else if (DataFmt == RF_FC_DFMT_FCWS_CFG) {
        DumpResult(pHeader);
    } else if (DataFmt == RF_FC_DFMT_FCMD_CFG) {
        DumpResult(pHeader);
    } else if (DataFmt == RF_FC_DFMT_LCS_CFG) {
        DumpResult(pHeader);
    } else if (DataFmt == RF_FC_DFMT_FC_SR_IMAGE_3D_OUT) {
        DumpResult(pHeader);
    } else {
        // do nothing
    }

    return ADAS_ERR_NONE;
}

static void* ADAS_Run(void* Info)
{
    UINT32 Rval;
    void* ptrRet = NULL;
    static UINT32 Cnt = 0U;

    static AMBA_SR_CANBUS_TRANSFER_DATA_s CanBus;
    static AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCfg;
    static AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalibInfo;
    static AMBA_CAL_POINT_DB_2D_s FL;

    static AMBA_OD_2DBBX_LIST_s BbxList;
    static RF_LOG_OD_2DBBOX_HEADER_s ODHeader;
    static AMBA_SEG_BUF_INFO_s SegBufInfo[4];
    static REF_FLOW_FC_NN_DATA_s FcNNData;


    // Open ADAS Logger file
    AmbaPrint_PrintStr5("Open file %s...\n", g_Para.ADASLogFN, NULL, NULL, NULL, NULL);
    Rval = AmbaFS_FileOpen(g_Para.ADASLogFN, "wb", &g_FP);
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("%s, AmbaFS_FileOpen failed.", __func__, NULL, NULL, NULL, NULL);
    }

    // Get Canbus and calib
    if (Rval == 0U) {
        Rval = ReadCanbusDatafromSrt(g_Para.CANBusFN);
        if (Rval != 0U) {
            AmbaPrint_PrintStr5("%s, ReadCanbusDatafromSrt failed.", __func__, NULL, NULL, NULL, NULL);
        }

        Rval = UtilPC_LoadEmirCalibV1(UTIL_PC_CHANNEL_FRONT, g_Para.CalibFN, g_Para.FocalLength, &CalibCfg, &CalibInfo, &FL);
        if (Rval != ADAS_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, UtilPC_LoadEmirCalibV1 fail !", __func__, NULL, NULL, NULL, NULL);
        }
    }

    while (1) {
        // get Canbus result
        if (Rval == 0U) {
            Rval = GetCanbusData(Cnt, &CanBus);
            if (Rval != 0U) {
                AmbaPrint_PrintStr5("%s, GetCanbusData fail !", __func__, NULL, NULL, NULL, NULL);
            }
        }

        // get NN result
        if (Rval == 0U) {
            Rval = _GetNextSeg39(SegBufInfo);
            SegBufInfo[0].TimeStamp = CanBus.CapTS;
            SegBufInfo[1].TimeStamp = CanBus.CapTS;
            SegBufInfo[2].TimeStamp = CanBus.CapTS;
            SegBufInfo[3].TimeStamp = CanBus.CapTS;
        }
        if (Rval == 0U) {
            (void)AmbaWrap_memset(&FcNNInfo, 0, sizeof(REF_FLOW_FC_NN_INFO_s)*AMBA_OD_2DBBX_MAX_BBX_NUM);
            FcNNData.pObjNNInfoList = FcNNInfo;
            Rval = _GetNexOD37(&BbxList, &ODHeader, &FcNNData);
            BbxList.CaptureTime = CanBus.CapTS;

        }

        // run adas
        if ((Rval == 0U) && (Cnt%g_Para.ProcStep == 0U)) {
            ADAS_UT_FCAM_CFG_s Cfg;

            AmbaPrint_PrintUInt5("\rRun ADAS (Cnt = %d)", Cnt, 0U, 0U, 0U, 0U);

            AmbaWrap_memset(&Cfg, 0, sizeof(ADAS_UT_FCAM_CFG_s));
            Cfg.pODHeader = &ODHeader;
            Cfg.pCalibCfg = &CalibCfg;
            Cfg.pCalibInfo = &CalibInfo;
            Cfg.pFocalLength = &FL;
            Cfg.ODVersion = g_Para.ODVersion;
            Cfg.SegVersion = g_Para.SegVersion;
            Cfg.CB = GetResult;
            AdasUT_FrontCamLvTest(SegBufInfo, &CanBus, &BbxList, &FcNNData, &Cfg);
        }

        // Post process
        if (Rval != 0U) {
            break;
        }
        Cnt ++;
    }

    // Close ADAS Logger file
    if (g_FP != NULL) {
        AmbaPrint_PrintStr5("Close file %s...\n", g_Para.ADASLogFN, NULL, NULL, NULL, NULL);
        Rval = AmbaFS_FileClose(g_FP);
        if (Rval != 0U) {
            AmbaPrint_PrintStr5("%s, AmbaFS_FileClose failed.", __func__, NULL, NULL, NULL, NULL);
        }
    }

    (void)Info;
    return ptrRet;
}

void SCA_ADAS_FC_UT(const char* pSegLoggerFile, const char* pODLoggerFile, const char* p3DLoggerFile, const char* pMKLoggerFile, const char* pCanbusFile,
                    const char* pCalibFile, DOUBLE FocalLength, UINT32 ProcStep, const char* pOutFile)
{
    UINT32 Rval = 0U;


    // Parsing parameter
    {
        AmbaUtility_StringCopy(g_Para.SegLogFN, FILE_NAME_LEN, pSegLoggerFile);
        g_Para.SegVersion = 1U;
        AmbaUtility_StringCopy(g_Para.ODLogFN, FILE_NAME_LEN, pODLoggerFile);
        g_Para.ODVersion = 1U;
        AmbaUtility_StringCopy(g_Para.OD3DLogFN, FILE_NAME_LEN, p3DLoggerFile);
        AmbaUtility_StringCopy(g_Para.ODMKLLogFN, FILE_NAME_LEN, pMKLoggerFile);
        AmbaUtility_StringCopy(g_Para.ODMKSLogFN, FILE_NAME_LEN, pMKLoggerFile);
        AmbaUtility_StringCopy(g_Para.CalibFN, FILE_NAME_LEN, pCalibFile);
        g_Para.FocalLength = FocalLength;
        AmbaUtility_StringCopy(g_Para.CANBusFN, FILE_NAME_LEN, pCanbusFile);
        g_Para.ProcStep = ProcStep;
        AmbaUtility_StringCopy(g_Para.ADASLogFN, FILE_NAME_LEN, pOutFile);
    }

    // create task
    if (Rval == 0U) {
#define TestStackSize (0x20000U)
        static AMBA_KAL_TASK_t Task;
        static UINT8 Stack[TestStackSize] __attribute__((section(".bss.noinit")));
        void *pInfo = NULL;
        const UINT8 *pBuf;
        void *ptr;

        (void)AmbaWrap_memset(&Stack[0], 0, TestStackSize);
        pBuf = &Stack[0];
        AmbaMisra_TypeCast32(&ptr, &pBuf);


        AmbaPrint_PrintStr5("%s, ADAS_FC_UT_Task Init", __func__, NULL, NULL, NULL, NULL);
        Rval = AmbaKAL_TaskCreate(&Task,                  /* pTask */
                                  "ADAS_FC_UT_Task",      /* pTaskName */
                                  255U - (120U),          /* Priority */
                                  ADAS_Run,               /* void (*EntryFunction)(void*) */
                                  pInfo,                  /* EntryArg */
                                  ptr,                    /* pStackBase */
                                  TestStackSize,          /* StackByteSize */
                                  0U);
        if (Rval != 0U) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_TaskCreate failed.", __func__, NULL, NULL, NULL, NULL);
        }

        Rval = AmbaKAL_TaskSetSmpAffinity(&Task, 0x7);
        if (Rval != 0U) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_TaskSetSmpAffinity failed.", __func__, NULL, NULL, NULL, NULL);
        }

        Rval = AmbaKAL_TaskResume(&Task);
        if (Rval != 0U) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_TaskResume failed.", __func__, NULL, NULL, NULL, NULL);
        }
    }

}
