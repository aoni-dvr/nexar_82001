/**
 *  @file AmbaRdtFile.c
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
 *  @details svc record destination - File
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaDef.h"
#include "AmbaFS.h"
#include "AmbaDSP.h"
#include "AmbaAudio_AENC.h"
#include "AmbaPrint.h"
#include "AmbaCodecCom.h"

#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"
#include "AmbaRbxInf.h"
#include "AmbaRdtInf.h"
#include "AmbaRecMaster.h"
#include "AmbaVfs.h"
#include "AmbaSvcWrap.h"
#include "AmbaRdtFile.h"
#include "AmbaRbxMP4.h"
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "../../../../../../apps/icam/cardv/record/imu_record.h"
#include "../../../../../../apps/icam/cardv/record/gnss_record.h"
#endif

#define RDTF_DBG_PRN    1
static inline void RDTF_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if RDTF_DBG_PRN
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#else
    AmbaMisra_TouchUnused(&pFormat);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

static inline void RDTF_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

#define RDT_FILE_STAT_OPEN      (0x00000001U)

#define RDT_FILE_BUFINFO_NUM    ((UINT32)CONFIG_AMBA_REC_RBX_MP4_BUF_NUM)

typedef struct {
    UINT32   IsValid;
    ULONG    Addr;
    UINT32   Size;
} AMBA_RDT_BUFINFO_s;

typedef struct {
    char                     FileName[AMBA_REC_MAX_FILE_NAME_LEN];
    AMBA_VFS_FILE_s          File;
    UINT32                   Status;
    UINT32                   UnsyncSize;
    UINT32                   FileSyncSize;
    UINT32                   StoreDisable;
    AMBA_RDT_STATIS_s        Statis;

    UINT32                   StorageCheck;
    UINT32                   IsBufFull;
    ULONG                    BufInfoLimit;
    AMBA_RDT_BUFINFO_s       BufInfo[RDT_FILE_BUFINFO_NUM];
    UINT32                   BufInfoIdx;
    AMBA_RDT_FSTATUS_INFO_s  FInfo;
} AMBA_RDT_FILE_PRIV_s;

static UINT32 BufferWrite(const AMBA_REC_DST_s *pRdtCtrl, ULONG DataAddr, UINT32 DataSize)
{
    const UINT8                 *pPrivData = pRdtCtrl->PrivData;
    AMBA_RDT_FILE_PRIV_s        *pPriv;
    void                        *pBuf;
    UINT32                      Rval = RECODER_OK, NumSuccess, WSize, RSize;
    ULONG                       WAddr;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    if (pPriv->StoreDisable == 0U) {
        if ((pPriv->FileSyncSize > 0U) && ((pPriv->UnsyncSize + DataSize) > pPriv->FileSyncSize)) {
            WAddr = DataAddr;
            WSize = pPriv->FileSyncSize - pPriv->UnsyncSize;
            RSize = (pPriv->UnsyncSize + DataSize) - pPriv->FileSyncSize;
        } else {
            WAddr = DataAddr;
            WSize = DataSize;
            RSize = 0U;
        }

        AmbaMisra_TypeCast(&pBuf, &WAddr);
        NumSuccess = 0U;
        Rval = AmbaVFS_Write(pBuf, WSize, 1U, &(pPriv->File), &NumSuccess);
        pPriv->UnsyncSize += (WSize * NumSuccess);

        if ((pPriv->FileSyncSize > 0U) && (pPriv->UnsyncSize >= pPriv->FileSyncSize)) {
            Rval = AmbaVFS_Sync(&(pPriv->File));
            pPriv->UnsyncSize = 0U;
        }

        if (RSize > 0U) {
            WAddr = DataAddr + (ULONG)WSize;
            WSize = RSize;

            AmbaMisra_TypeCast(&pBuf, &WAddr);
            NumSuccess = 0U;
            Rval = AmbaVFS_Write(pBuf, WSize, 1U, &(pPriv->File), &NumSuccess);
            pPriv->UnsyncSize += (WSize * NumSuccess);
        }
    }

    return Rval;
}

static void UpdateWrittenData(const AMBA_RDT_FILE_PRIV_s *pPriv, const AMBA_REC_FRWK_DESC_s *pDesc)
{
    UINT32   NumSuccess = 0U, Rval = RECODER_OK;
    UINT32   i, Err;
    void     *pData;

    for (i = 0U; i < pDesc->BoxUpdateCnt; i++) {
        AmbaMisra_TypeCast(&pData, &(pDesc->BoxUpdate[i].Base));

        Err = AmbaVFS_Seek(&(pPriv->File), (INT64)(pDesc->BoxUpdate[i].Offset), AMBA_CFS_SEEK_START);
        if (Err != RECODER_OK) {
            RDTF_NG("AmbaVFS_Seek failed %u", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        if (Rval == RECODER_OK) {
            Err = AmbaVFS_Write(pData, pDesc->BoxUpdate[i].Size, 1U, &(pPriv->File), &NumSuccess);
            if (Err != RECODER_OK) {
                RDTF_NG("AmbaVFS_Write failed %u", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == RECODER_OK) {
            Err = AmbaVFS_Seek(&(pPriv->File), 0, AMBA_CFS_SEEK_END);
            if (Err != RECODER_OK) {
                RDTF_NG("AmbaVFS_Seek failed %u", Err, 0U);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        }
    }

    AmbaMisra_TouchUnused(&Rval);
}

static UINT32 DataStore(const AMBA_REC_DST_s *pRdtCtrl, const AMBA_REC_FRWK_DESC_s *pDesc)
{
    const UINT8                   *pPrivData = pRdtCtrl->PrivData;
    UINT32                        i, Rval = RECODER_OK, Err;
    UINT32                        DataSize = 0U, Left;
    ULONG                         DataAddr, TmpUL;
    const AMBA_DSP_ENC_PIC_RDY_s  *pVDesc;
    const AMBA_AENC_AUDIO_DESC_s  *pADesc;
    AMBA_RDT_FILE_PRIV_s          *pPriv;
    const AMBA_RDT_USR_CFG_s      *pUsrCfg = &(pRdtCtrl->UsrCfg);

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    if ((pPriv->StorageCheck == 0U) && (pPriv->IsBufFull == 0U)) {
        /* for record box source, we only check the storage status one time when the box data is ready */
        if (((0U < CheckBits(pRdtCtrl->UsrCfg.InBoxBits, pDesc->BoxBit)) && (0U < pDesc->BoxBufNum)) ||
             (0U < CheckBits(pRdtCtrl->UsrCfg.InSrcBits, pDesc->SrcBit))) {
            Err = pUsrCfg->pfnCheckStorage(pUsrCfg->StreamID);
            if (Err == RECODER_OK) {
                pPriv->StorageCheck = 1U;
            }

            if (pPriv->StorageCheck == 1U) {
                if (pUsrCfg->pfnFetchFName != NULL) {
                    if (pUsrCfg->BootToRec > 0U) {
                        Rval = pUsrCfg->pfnFetchFName(pPriv->FileName,
                                                    AMBA_REC_MAX_FILE_NAME_LEN,
                                                    pUsrCfg->StreamID);
                        if (Rval == RECODER_OK) {
                            Rval = AmbaVFS_Open(pPriv->FileName, "w", 1U, &(pPriv->File));
                            if (Rval == RECODER_OK) {
                                pPriv->Status = SetBits(pPriv->Status, RDT_FILE_STAT_OPEN);
                            } else {
                                RDTF_NG("fail to open file", 0U, 0U);
                            }
                        }
                    }

                    if (Rval == RECODER_OK) {
                        for (i = 0U; i < pPriv->BufInfoLimit; i++) {
                            if (pPriv->BufInfo[i].IsValid == 1U) {
                                Rval = BufferWrite(pRdtCtrl, pPriv->BufInfo[i].Addr, pPriv->BufInfo[i].Size);
                                pPriv->BufInfo[i].IsValid = 0U;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (pPriv->StorageCheck == 0U) {
        if ((pPriv->IsBufFull == 0U)) {
            if ((0U < CheckBits(pRdtCtrl->UsrCfg.InSrcBits, pDesc->SrcBit)) &&
                (CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_EOS) == 0U)) {
                if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
                    AmbaMisra_TypeCast(&pVDesc, &(pDesc->pSrcData));
                    DataAddr = pVDesc->StartAddr;
                    DataSize = pVDesc->PicSize;

                    TmpUL = pDesc->SrcBufBase + pDesc->SrcBufSize - DataAddr;
                    Left  = (UINT32)TmpUL;
                    if (DataSize <= Left) {
                        if (pPriv->BufInfo[pPriv->BufInfoIdx].IsValid == 0U) {
                            pPriv->BufInfo[pPriv->BufInfoIdx].Addr    = DataAddr;
                            pPriv->BufInfo[pPriv->BufInfoIdx].Size    = DataSize;
                            pPriv->BufInfo[pPriv->BufInfoIdx].IsValid = 1U;

                            pPriv->BufInfoIdx ++;
                            if (pPriv->BufInfoIdx >= pPriv->BufInfoLimit) {
                                pPriv->IsBufFull = 1U;
                                RDTF_NG("FileBuf is fulled %u/%u", pPriv->BufInfoIdx, (UINT32)pPriv->BufInfoLimit);
                            }
                            pPriv->BufInfoIdx %= RDT_FILE_BUFINFO_NUM;
                        }
                    } else {
                        if (pPriv->BufInfo[pPriv->BufInfoIdx].IsValid == 0U) {
                            pPriv->BufInfo[pPriv->BufInfoIdx].Addr    = DataAddr;
                            pPriv->BufInfo[pPriv->BufInfoIdx].Size    = Left;
                            pPriv->BufInfo[pPriv->BufInfoIdx].IsValid = 1U;

                            pPriv->BufInfoIdx ++;
                            if (pPriv->BufInfoIdx >= pPriv->BufInfoLimit) {
                                pPriv->IsBufFull = 1U;
                                RDTF_NG("FileBuf is fulled %u/%u", pPriv->BufInfoIdx, (UINT32)pPriv->BufInfoLimit);
                            }
                            pPriv->BufInfoIdx %= RDT_FILE_BUFINFO_NUM;
                        }

                        if (pPriv->BufInfo[pPriv->BufInfoIdx].IsValid == 0U) {
                            pPriv->BufInfo[pPriv->BufInfoIdx].Addr    = pDesc->SrcBufBase;
                            pPriv->BufInfo[pPriv->BufInfoIdx].Size    = DataSize - Left;
                            pPriv->BufInfo[pPriv->BufInfoIdx].IsValid = 1U;

                            pPriv->BufInfoIdx ++;
                            if (pPriv->BufInfoIdx >= pPriv->BufInfoLimit) {
                                pPriv->IsBufFull = 1U;
                                RDTF_NG("FileBuf is fulled %u/%u", pPriv->BufInfoIdx, (UINT32)pPriv->BufInfoLimit);
                            }
                            pPriv->BufInfoIdx %= RDT_FILE_BUFINFO_NUM;
                        }
                    }
                }
            } else if (0U < CheckBits(pRdtCtrl->UsrCfg.InBoxBits, pDesc->BoxBit)) {
                if (0U < pDesc->BoxBufNum) {
                    for (i = 0U; i < pDesc->BoxBufNum; i++) {
                        if (pPriv->BufInfo[pPriv->BufInfoIdx].IsValid == 0U) {
                            pPriv->BufInfo[pPriv->BufInfoIdx].Addr    = pDesc->BoxBufChain[i].Base;
                            pPriv->BufInfo[pPriv->BufInfoIdx].Size    = pDesc->BoxBufChain[i].Size;
                            pPriv->BufInfo[pPriv->BufInfoIdx].IsValid = 1U;

                            pPriv->BufInfoIdx ++;
                            if (pPriv->BufInfoIdx >= pPriv->BufInfoLimit) {
                                pPriv->IsBufFull = 1U;
                                RDTF_NG("FileBuf is fulled %u/%u", pPriv->BufInfoIdx, (UINT32)pPriv->BufInfoLimit);
                                break;
                            }
                            pPriv->BufInfoIdx %= RDT_FILE_BUFINFO_NUM;
                        }
                    }
                }
            } else {
                /* do nothing */
            }
        }
    } else {
        if ((0U < CheckBits(pPriv->Status, RDT_FILE_STAT_OPEN)) && (pPriv->StoreDisable == 0U)) {
            if ((0U < CheckBits(pRdtCtrl->UsrCfg.InSrcBits, pDesc->SrcBit)) &&
                (CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_EOS) == 0U)) {
                if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
                    AmbaMisra_TypeCast(&pVDesc, &(pDesc->pSrcData));
                    DataAddr = pVDesc->StartAddr;
                    DataSize = pVDesc->PicSize;

                    TmpUL = pDesc->SrcBufBase + pDesc->SrcBufSize - DataAddr;
                    Left  = (UINT32)TmpUL;
                    if (DataSize <= Left) {
                        Rval = BufferWrite(pRdtCtrl, DataAddr, DataSize);
                    } else {
                        Rval = BufferWrite(pRdtCtrl, DataAddr, Left);
                        if (Rval == RECODER_OK) {
                            Rval = BufferWrite(pRdtCtrl, pDesc->SrcBufBase,
                                            DataSize - Left);
                        } else {
                            RDTF_NG("fail to write fifo", 0U, 0U);
                        }
                    }
                } else if (pDesc->SrcType == AMBA_RSC_TYPE_AUDIO) {
                    AmbaMisra_TypeCast(&pADesc, &(pDesc->pSrcData));
                    AmbaMisra_TypeCast(&DataAddr, &(pADesc->pBufAddr));
                    DataSize = pADesc->DataSize;
                } else {
                    /* do nothing */
                }
            } else if (0U < CheckBits(pRdtCtrl->UsrCfg.InBoxBits, pDesc->BoxBit)) {
                if (0U < pDesc->BoxBufNum) {
                    for (i = 0U; i < pDesc->BoxBufNum; i++) {
                        DataAddr = pDesc->BoxBufChain[i].Base;
                        DataSize = pDesc->BoxBufChain[i].Size;

                        Rval = BufferWrite(pRdtCtrl, DataAddr, DataSize);
                    }
                }
            } else {
                /* do nothing */
            }

            if ((pDesc->BoxUpdateCnt > 0U) && (pPriv->StoreDisable == 0U)) {
                UpdateWrittenData(pPriv, pDesc);
            }

            /* is needed to split ? */
            if (0U < CheckBits(pDesc->DescFlags, AMBA_REC_DDESC_FLAG_SPLIT)) {
                /* close file */
                Rval = AmbaVFS_Sync(&(pPriv->File));
                if (Rval == RECODER_OK) {
                    Rval = AmbaVFS_Close(&(pPriv->File));
                    if (Rval == RECODER_OK) {
                        pPriv->Status = ClearBits(pPriv->Status, RDT_FILE_STAT_OPEN);

                        if (pUsrCfg->pfnUpdateFName != NULL) {
                            Rval = pUsrCfg->pfnUpdateFName(pPriv->FileName, pUsrCfg->StreamID);
                            if (Rval != RECODER_OK) {
                                RDTF_NG("fail to update filename", 0U, 0U);
                            }
                        }

                        if (pUsrCfg->pfnFStatusNotify != NULL) {
                            pPriv->FInfo.FileStatus = AMBA_RDT_FSTATUS_FCLOSE;
                            pPriv->FInfo.IsLastFile = 0U;
                            Rval = pUsrCfg->pfnFStatusNotify(&(pPriv->FInfo));
                            if (Rval != RECODER_OK) {
                                RDTF_NG("fail to notify file status", 0U, 0U);
                            }
                        }
                    } else {
                        RDTF_NG("fail to close fifo", 0U, 0U);
                    }
                } else {
                    RDTF_NG("fail to flush fifo", 0U, 0U);
                }

                /* update file info */
                if ((0U < DataSize) && (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO)) {
                    AMBA_RDT_FSTATUS_INFO_s *pInfo = &(pPriv->FInfo);

                    AmbaMisra_TypeCast(&pVDesc, &(pDesc->pSrcData));

                    if (pVDesc->PicSize != AMBA_DSP_ENC_END_MARK) {
                        pInfo->FileName    = pPriv->FileName;
                        pInfo->RecStrmId   = pUsrCfg->StreamID;
                        pInfo->DstType     = pUsrCfg->DstType;
                        pInfo->SubType     = pUsrCfg->SubType;
                        pInfo->VidCapPts   = pVDesc->CaptureTimeStamp;
                    }
                }

                /* open new file */
                if (Rval == RECODER_OK) {
                    if (pUsrCfg->pfnFetchFName != NULL) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                        char LastFileName[AMBA_REC_MAX_FILE_NAME_LEN] = {0};
                        AmbaWrap_memcpy(LastFileName, pPriv->FileName, sizeof(pPriv->FileName));
#endif                        
                        Rval = pUsrCfg->pfnFetchFName(pPriv->FileName,
                                                    AMBA_REC_MAX_FILE_NAME_LEN,
                                                    pUsrCfg->StreamID);
                        if (Rval == RECODER_OK) {
                            Rval = AmbaVFS_Open(pPriv->FileName, "w", 1U, &(pPriv->File));
                            if (Rval == RECODER_OK) {
                                pPriv->Status = SetBits(pPriv->Status, RDT_FILE_STAT_OPEN);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                                imu_record_control(IMU_RECORD_FILE_SPLIT, pUsrCfg->StreamID, pPriv->FileName, NULL);
                                gnss_record_control(GNSS_RECORD_FILE_SPLIT, pPriv->FileName, LastFileName);
#endif
                            } else {
                                RDTF_NG("fail to open file", 0U, 0U);
                            }
                        }

                        if (pUsrCfg->pfnFStatusNotify != NULL) {
                            pPriv->FInfo.FileStatus = AMBA_RDT_FSTATUS_FOPEN;
                            pPriv->FInfo.IsLastFile = 0U;
                            Rval = pUsrCfg->pfnFStatusNotify(&(pPriv->FInfo));
                            if (Rval != RECODER_OK) {
                                RDTF_NG("fail to notify file status", 0U, 0U);
                            }
                        }
                    }
                }
            }
        }
    }

    /* update file info */
    if ((pDesc->SrcType == AMBA_RSC_TYPE_VIDEO)) {
        AMBA_RDT_FSTATUS_INFO_s *pInfo = &(pPriv->FInfo);

        AmbaMisra_TypeCast(&pVDesc, &(pDesc->pSrcData));

        if (pVDesc->PicSize != AMBA_DSP_ENC_END_MARK) {

            pInfo->FileName    = pPriv->FileName;
            pInfo->RecStrmId   = pUsrCfg->StreamID;
            pInfo->DstType     = pUsrCfg->DstType;
            pInfo->SubType     = pUsrCfg->SubType;
            pInfo->VidCapPts   = pVDesc->CaptureTimeStamp;

            if ((pVDesc->FrmNo == 0U) && (pVDesc->TileIdx == 0U) && (pVDesc->SliceIdx == 0U)) {
                if (pUsrCfg->pfnFStatusNotify != NULL) {
                    pPriv->FInfo.FileStatus = AMBA_RDT_FSTATUS_FOPEN;
                    pPriv->FInfo.IsLastFile = 0U;
                    Rval = pUsrCfg->pfnFStatusNotify(&(pPriv->FInfo));
                    if (Rval != RECODER_OK) {
                        RDTF_NG("fail to notify file status", 0U, 0U);
                    }
                }
            }
        }
    }

    if (0U < DataSize) {
        pPriv->Statis.StoreCount += 1U;
        pPriv->Statis.StoreSize += DataSize;
    }

    return Rval;
}

/**
* record destination creation
* @param [out]  pFileRdt record destination info
* @return ErrorCode
*/
UINT32 AmbaRdtFile_Create(AMBA_REC_DST_s *pFileRdt)
{
    static UINT32         RdtFilePriv = (UINT32)sizeof(AMBA_RDT_FILE_PRIV_s);
    const UINT8           *pPrivData = pFileRdt->PrivData;
    UINT32                Rval = RECODER_ERROR_GENERAL_ERROR, Reserved = CONFIG_AMBA_REC_RDT_PRIV_SIZE;
    AMBA_RDT_FILE_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    if (RdtFilePriv <= Reserved) {
        pFileRdt->pfnStore = DataStore;

        /* reset parameters */
        Rval = AmbaWrap_memset(pPriv, 0, sizeof(AMBA_RDT_FILE_PRIV_s));
        if (Rval != RECODER_OK) {
            RDTF_NG("AmbaWrap_memset failed %u", Rval, 0U);
        }

        pPriv->BufInfoLimit = (UINT32)CONFIG_AMBA_REC_RBX_MP4_BUF_NUM;
    }

    if (Rval == RECODER_OK) {
        RDTF_DBG("rdt is created!", 0U, 0U);
    } else {
        RDTF_NG("fail to create rdt, (%u/%u)", RdtFilePriv, Reserved);
    }

    return Rval;
}

/**
* record destination destroy
* @param [in]  pFileRdt record destination info
* @return ErrorCode
*/
UINT32 AmbaRdtFile_Destroy(const AMBA_REC_DST_s *pFileRdt)
{
    const UINT8                *pPrivData = pFileRdt->PrivData;
    const AMBA_RDT_FILE_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    RDTF_DBG("rdt is destroyed! StoreCount/StoreSize(%X/%X)", (UINT32)pPriv->Statis.StoreCount
                                                            , (UINT32)pPriv->Statis.StoreSize);
    return RECODER_OK;
}

/**
* record destination control
* @param [in]  pFileRdt record destination info
* @param [in]  CtrlType control type
* @param [in]  pParam pointer of control parameter
*/
void AmbaRdtFile_Control(const AMBA_REC_DST_s *pFileRdt, UINT32 CtrlType, void *pParam)
{
    const UINT8               *pPrivData = pFileRdt->PrivData;
    UINT32                    Rval, *pBufStatus;
    AMBA_RDT_FILE_PRIV_s      *pPriv;
    const AMBA_RDT_USR_CFG_s  *pUsrCfg;
    const UINT32              *pSyncSize;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    switch (CtrlType) {
    case AMBA_RDTC_FILE_STATIS_GET:
        Rval = AmbaWrap_memcpy(pParam, &(pPriv->Statis), sizeof(AMBA_RDT_STATIS_s));
        if (Rval != RECODER_OK) {
            RDTF_NG("AmbaWrap_memcpy failed %u", Rval, 0U);
        }
        break;
    case AMBA_RDTC_FILE_OPEN:
        AmbaMisra_TypeCast(&pSyncSize, &(pParam));
        pPriv->StoreDisable = 0U;
        pPriv->StorageCheck = 0U;
        pPriv->BufInfoIdx   = 0U;
        pPriv->IsBufFull    = 0U;
        pPriv->UnsyncSize   = 0U;
        if (pParam != NULL) {
            pPriv->FileSyncSize = *pSyncSize;
        }

        Rval = AmbaWrap_memset(pPriv->BufInfo, 0, sizeof(pPriv->BufInfo));
        if (Rval != RECODER_OK) {
            RDTF_NG("AmbaWrap_memset failed %u", Rval, 0U);
        }

        /* fetch file name and open */
        pUsrCfg = &(pFileRdt->UsrCfg);
        if (pUsrCfg->BootToRec == 0U) {
            if (pUsrCfg->pfnFetchFName != NULL) {
                Rval = pUsrCfg->pfnFetchFName(pPriv->FileName, AMBA_REC_MAX_FILE_NAME_LEN, pUsrCfg->StreamID);
                if (Rval == RECODER_OK) {
                    Rval = AmbaVFS_Open(pPriv->FileName, "w", 1U, &(pPriv->File));
                    if (Rval == RECODER_OK) {
                        pPriv->Status = SetBits(pPriv->Status, RDT_FILE_STAT_OPEN);
                    } else {
                        RDTF_NG("fail to open file", 0U, 0U);
                    }
                } else {
                    RDTF_NG("fail to fetch file name", 0U, 0U);
                }
            }
        }
        break;
    case AMBA_RDTC_FILE_CLOSE:
        if (pPriv->StorageCheck == 1U) {
            pUsrCfg = &(pFileRdt->UsrCfg);
            if (0U < CheckBits(pPriv->Status, RDT_FILE_STAT_OPEN)) {
                Rval = AmbaVFS_Sync(&(pPriv->File));
                if (Rval == RECODER_OK) {
                    Rval = AmbaVFS_Close(&(pPriv->File));
                    if (Rval == RECODER_OK) {
                        pPriv->Status = ClearBits(pPriv->Status, RDT_FILE_STAT_OPEN);
                        if (pUsrCfg->pfnUpdateFName != NULL) {
                            Rval = pUsrCfg->pfnUpdateFName(pPriv->FileName, pUsrCfg->StreamID);
                            if (Rval != RECODER_OK) {
                                RDTF_NG("fail to update filename", 0U, 0U);
                            }
                        }

                        if (pUsrCfg->pfnFStatusNotify != NULL) {
                            pPriv->FInfo.FileStatus = AMBA_RDT_FSTATUS_FCLOSE;
                            pPriv->FInfo.IsLastFile = 1U;
                            Rval = pUsrCfg->pfnFStatusNotify(&(pPriv->FInfo));
                            if (Rval != RECODER_OK) {
                                RDTF_NG("fail to notify file status", 0U, 0U);
                            }
                        }
                    } else {
                        RDTF_NG("fail to close fifo", 0U, 0U);
                    }
                } else {
                    RDTF_NG("fail to flush fifo", 0U, 0U);
                }
            }
        }
        break;
    case AMBA_RDTC_FILE_STOP:
        if (pPriv->StorageCheck == 1U) {
            pPriv->StoreDisable = 1U;

            Rval = AmbaVFS_NullWrite(&(pPriv->File));
            if (Rval != RECODER_OK) {
                RDTF_NG("AmbaVFS_NullWrite failed", 0U, 0U);
            }
        }
        break;
    case AMBA_RDTC_FILE_BUF_STATUS:
        AmbaMisra_TypeCast(&pBufStatus, &(pParam));
        *pBufStatus = pPriv->IsBufFull;
        break;
    default:
        RDTF_NG("unknown control type", 0U, 0U);
        break;
    }
}

/**
* record destination memory evaluation
* @param [in]  pEval record info
* @param [out]  pSize memory size
*/
void AmbaRdtFile_EvalMemSize(const AMBA_REC_EVAL_s *pEval, UINT32 *pSize)
{
    AmbaMisra_TouchUnused(&pEval);
    *pSize = 0U;
}
