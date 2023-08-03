/**
 *  @file SvcCvFlowUtil.c
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
 *  @details Implementation of SvcCvFlow utility
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "AmbaFS.h"
#include "AmbaMMU.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcNvm.h"
#include "SvcPlat.h"
#include "SvcMem.h"

#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcTiming.h"

#include "cvapi_ambacv_flexidag.h"
#include "cvtask_ossrv.h"

#define SVC_LOG_CV_FLOW         "CV_FLOW"

#define SVC_CV_FLOW_MEM_NUM             (2U)
#define SVC_CV_FLOW_MEM_BIN             (0U)
#define SVC_CV_FLOW_MEM_WORK            (1U)

#define SVC_CV_FLOW_FLEXI_BIN_MAX       (16U)

typedef UINT32 (*SVC_CV_FLOW_GET_FILE_SIZE_f)(const char *pFileName, UINT32 *pSize);
typedef UINT32 (*SVC_CV_FLOW_LOAD_FILE_f)(const char *pFileName, UINT8 *pBuf, UINT32 BufSize, UINT32 DataSize, UINT32 *pRealSize);

typedef struct {
    ULONG       Base;
    UINT32      Size;
    UINT32      UsedSize;
} SVC_CV_FLOW_MEM_REGION_s;

typedef struct {
    UINT8       Used;
    char        Name[SVC_CV_FLOW_MAX_FILE_PATH];
    UINT8       *pAddr;
    UINT32      Size;
} SVC_CV_FLOW_BIN_INFO_s;

static SVC_CV_FLOW_MEM_REGION_s CvFlowMem[SVC_CV_FLOW_MEM_NUM];

static UINT32 SvcCvFlowDebugEnable = 1U;
static SVC_CV_FLOW_BIN_INFO_s CvFlowFlexiBinMgr[SVC_CV_FLOW_FLEXI_BIN_MAX] GNU_SECTION_NOZEROINIT;

static UINT32 CvFlow_AllocBuf(UINT32 Type, UINT8 **pBuf, UINT32 ReqSize, UINT32 *pAlignedSize);
static UINT32 CvFlow_SearchEmptySlot(SVC_CV_FLOW_BIN_INFO_s *pMgr, UINT32 MaxSlot, UINT32 *pEmptySlot);
static UINT32 CvFlow_FindSlotByName(SVC_CV_FLOW_BIN_INFO_s *pMgr, UINT32 MaxSlot, const char *pTarget, UINT32 *pSlot);

static UINT32 CvFlow_GetFileSize_SD(const char *pFileName, UINT32 *pSize);
static UINT32 CvFlow_GetFileSize_ROMFS(const char *pFileName, UINT32 *pSize);
static UINT32 CvFlow_LoadFile_SD(const char *pFileName, UINT8 *pBuf, UINT32 BufSize, UINT32 DataSize, UINT32 *pRealSize);
static UINT32 CvFlow_LoadFile_ROMFS(const char *pFileName, UINT8 *pBuf, UINT32 BufSize, UINT32 DataSize, UINT32 *pRealSize);
static uint32_t CvFlow_SchdrLoadCallback(const char *pFileName, void* pVoidBuf, uint32_t BufSize, uint32_t *pRealSize);
static int32_t CvFlow_LoadRomfsBinary(char *pBuf, int32_t BufSize, void* pToken);
static void   CvFlow_PackFdagMemBlkInfo(void *pAddr, UINT32 Size, flexidag_memblk_t *pMemBlk);

static SVC_CV_FLOW_GET_FILE_SIZE_f CvFlowGetFileSizeFunc[SVC_CV_FLOW_STORAGE_TYPE_NUM] = {
    CvFlow_GetFileSize_ROMFS,
    CvFlow_GetFileSize_SD
};
static SVC_CV_FLOW_LOAD_FILE_f CvFlowLoadFileFunc[SVC_CV_FLOW_STORAGE_TYPE_NUM] = {
    CvFlow_LoadFile_ROMFS,
    CvFlow_LoadFile_SD
};

static void CvFlow_Dbg(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvFlowDebugEnable > 0U) {
        SvcLog_OK(SVC_LOG_CV_FLOW, pFormat, Arg1, Arg2);
    }
}

static void CvFlow_Err(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (SvcCvFlowDebugEnable > 0U) {
        SvcLog_NG(SVC_LOG_CV_FLOW, pFormat, Arg1, Arg2);
    }
}

/**
* Initialize SvcCvFlow utility
* @param [in] pCfg configuration of SvcCvFlow utility
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_UtilInit(SVC_CV_FLOW_UTIL_CFG_s *pCfg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i = 0;

    AmbaMisra_TouchUnused(pCfg);

    if (NULL != pCfg) {
#ifdef CONFIG_ICAM_CV_FLEXIDAG
        {
            #define SVC_CV_FLOW_MEM_BIN_SIZE        (UINT32)CONFIG_ICAM_CV_FLEXIDAG_BIN_BUF_SIZE
            #define SVC_CV_FLOW_MEM_WORK_SIZE       (UINT32)CONFIG_ICAM_CV_FLEXIDAG_WORK_BUF_SIZE
            ULONG CurBase;
            ULONG MemReq;

            RetVal = AmbaWrap_memset(CvFlowFlexiBinMgr, 0, sizeof(CvFlowFlexiBinMgr));
            if (SVC_OK != RetVal) {
                CvFlow_Err("memset FlexiBinMgr failed", 0U, 0U);
            }

            CurBase = pCfg->MemBase;
            CvFlowMem[SVC_CV_FLOW_MEM_BIN].Base     = CurBase;
            CvFlowMem[SVC_CV_FLOW_MEM_BIN].Size     = SVC_CV_FLOW_MEM_BIN_SIZE;
            CvFlowMem[SVC_CV_FLOW_MEM_BIN].UsedSize = 0;
            CurBase += SVC_CV_FLOW_MEM_BIN_SIZE;

            CvFlowMem[SVC_CV_FLOW_MEM_WORK].Base     = CurBase;
            CvFlowMem[SVC_CV_FLOW_MEM_WORK].Size     = SVC_CV_FLOW_MEM_WORK_SIZE;
            CvFlowMem[SVC_CV_FLOW_MEM_WORK].UsedSize = 0;
            CurBase += SVC_CV_FLOW_MEM_WORK_SIZE;

            for (i = 0; i < SVC_CV_FLOW_MEM_NUM; i++) {
                SvcCvFlow_PrintULong("SvcCvFlow_UtilInit: MemType[%d], Base 0x%x, Size %d", i, CvFlowMem[i].Base, CvFlowMem[i].Size, 0U, 0U);
            }

            MemReq = CurBase - pCfg->MemBase;
            if (MemReq > (ULONG)pCfg->MemSize) {
                CvFlow_Err("Init CvFlowMem error. Size = %u, Req = %d", pCfg->MemSize, (UINT32)MemReq);
                RetVal = SVC_NG;
            }
        }
#else
        for (i = 0; i < SVC_CV_FLOW_MEM_NUM; i++) {
            RetVal = AmbaWrap_memset(&CvFlowMem[i], 0, sizeof(SVC_CV_FLOW_MEM_REGION_s));
            if (SVC_OK != RetVal) {
                CvFlow_Err("memset CvFlowMem[%u] failed", i, 0U);
            }
        }

        RetVal = AmbaWrap_memset(CvFlowFlexiBinMgr, 0, sizeof(CvFlowFlexiBinMgr));
        if (SVC_OK != RetVal) {
            CvFlow_Err("memset FlexiBinMgr failed", 0U, 0U);
        }
#endif

    } else {
        CvFlow_Err("pCfg = NULL", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* De-initialize SvcCvFlow utility
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_UtilDeInit(void)
{
    UINT32 RetVal;

    RetVal = AmbaWrap_memset(CvFlowFlexiBinMgr, 0, sizeof(CvFlowFlexiBinMgr));
    if (SVC_OK != RetVal) {
        CvFlow_Err("memset FlexiBinMgr failed", 0U, 0U);
    }

    return SVC_OK;
}

/**
* Load binary file to CV working buffer from storage
* @param [in] pFileName file name
* @param [in] StorageType type of storage
* @param [out] pBufInfo resulting data buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_LoadBinFile(const char *pFileName, UINT32 StorageType, SVC_CV_FLOW_BUF_INFO_s *pBufInfo)
{
    UINT32 RetVal = SVC_OK;
    UINT8  *pBuf = NULL;
    UINT32 BinSize, BufSize = 0, RealSize = 0;

    RetVal = CvFlowGetFileSizeFunc[StorageType](pFileName, &BinSize);
    if (SVC_OK == RetVal) {
        RetVal = CvFlow_AllocBuf(SVC_CV_FLOW_MEM_WORK, &pBuf, BinSize, &BufSize);
        if (SVC_OK == RetVal) {
            RetVal = CvFlowLoadFileFunc[StorageType](pFileName, pBuf, BufSize, BinSize, &RealSize);
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (RetVal == SVC_OK) {
        pBufInfo->pAddr    = pBuf;
        pBufInfo->Size     = BufSize;
        pBufInfo->DataSize = RealSize;
    } else {
        CvFlow_Err("SvcCvFlow_LoadBinFile error(0x%x)", RetVal, 0U);
    }

    return RetVal;
}

/**
* Attach and load flexidag binary to CV working buffer from storage
* @param [in] pFileName file name
* @param [in] StorageType type of storage
* @param [out] pBufInfo resulting data buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_AttachFlexiBin(const char *pFileName, UINT32 StorageType, SVC_CV_FLOW_BUF_INFO_s *pBufInfo)
{
    UINT32 RetVal = SVC_NG;
    UINT32 Slot = 0;
    UINT32 BinSize = 0U, BufSize, RealSize = 0;
    flexidag_memblk_t MemBlk;

    if ((NULL != pFileName) && (NULL != pBufInfo) && (StorageType < SVC_CV_FLOW_STORAGE_TYPE_NUM)) {
        CvFlow_Dbg("SvcCvFlow_AttachFlexiBin:", 0U, 0U);

        if (0U == CvFlow_FindSlotByName(CvFlowFlexiBinMgr, SVC_CV_FLOW_FLEXI_BIN_MAX, pFileName, &Slot)) {
            /* Not exist */
            RetVal = CvFlowGetFileSizeFunc[StorageType](pFileName, &BinSize);
            if (RetVal == SVC_OK) {
                if (1U == CvFlow_SearchEmptySlot(CvFlowFlexiBinMgr, SVC_CV_FLOW_FLEXI_BIN_MAX, &Slot)) {
                    RetVal = CvFlow_AllocBuf(SVC_CV_FLOW_MEM_BIN,
                                             &CvFlowFlexiBinMgr[Slot].pAddr,
                                             BinSize,
                                             &BufSize);
                    if (RetVal == SVC_OK) {
                        RetVal = CvFlowLoadFileFunc[StorageType](pFileName,
                                                                 CvFlowFlexiBinMgr[Slot].pAddr,
                                                                 BufSize,
                                                                 BinSize,
                                                                 &RealSize);
                        if (RetVal == SVC_OK) {
                            CvFlow_PackFdagMemBlkInfo(CvFlowFlexiBinMgr[Slot].pAddr, BufSize, &MemBlk);
                            RetVal = AmbaCV_UtilityCmaMemClean(&MemBlk);
                            if (ERRCODE_NONE == RetVal) {
                                CvFlowFlexiBinMgr[Slot].Used = 1U;
                                CvFlowFlexiBinMgr[Slot].Size = BufSize;
                                SvcWrap_strcpy(CvFlowFlexiBinMgr[Slot].Name, SVC_CV_FLOW_MAX_FILE_PATH, pFileName);
                                AmbaPrint_PrintStr5("[OK] %s", pFileName, NULL, NULL, NULL, NULL);
                                RetVal = SVC_OK;
                            } else {
                                CvFlow_Err("AttachFlexiBin: clean cache error", 0U, 0U);
                            }
                        } else {
                            AmbaPrint_PrintStr5("[NG] %s", pFileName, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        CvFlow_Err("AttachFlexiBin: alloc mem error", 0U, 0U);
                    }
                } else {
                    CvFlow_Err("AttachFlexiBin: List full", 0U, 0U);
                }
            } else {
                CvFlow_Err("AttachFlexiBin: file error", 0U, 0U);
            }
        } else {
            /* Exist */
            RetVal = SVC_OK;
        }

        if (RetVal == SVC_OK) {
            pBufInfo->pAddr    = CvFlowFlexiBinMgr[Slot].pAddr;
            pBufInfo->Size     = CvFlowFlexiBinMgr[Slot].Size;
            pBufInfo->DataSize = RealSize;
        }
    } else {
        CvFlow_Err("SvcCvFlow_AttachFlexiBin: invalid param", 0U, 0U);
    }

    return RetVal;
}

/**
* Load vision ORC binary to CV section from storage
* @param [in] pFileName file name
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_AddVisOrcBin(const char *pFileName)
{
#if 1
    UINT32 RetVal;

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_VISORC_LOAD_START, "CV VisOrc load START");
    #endif

    RetVal = AmbaCV_SchdrLoadExt(pFileName, FLEXIDAG_SCHDR_VP_BIN, CvFlow_SchdrLoadCallback);
    RetVal |= AmbaCV_SchdrCheckVisorc(FLEXIDAG_SCHDR_VP_BIN);
    if (RetVal == OK) {
        AmbaPrint_PrintStr5("SvcCvFlow_AddVisOrcBin: %s done", pFileName, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5("SvcCvFlow_AddVisOrcBin: %s error", pFileName, NULL, NULL, NULL, NULL);
    }

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_VISORC_LOAD_DONE, "CV VisOrc load DONE");
    #endif

    return RetVal;
#else
    #define VISION_ORC_BUF_OFFSET   0x400000U
    #define VISION_ORC_BUF_SIZE     0x200000U
    extern UINT32 __cv_sys_start;
    UINT32 RetVal = SVC_OK;
    const UINT32 *pU32;
    UINT8 *pBuf;
    UINT32 BufAddr, DataSize, SizeAligned;

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_VISORC_LOAD_START, "CV VisOrc load START");
    #endif

    //Buf = (char*)(&__cv_sys_start) + 0x400000;
    pU32 = &__cv_sys_start;
    AmbaMisra_TypeCast(&BufAddr,&pU32);
    BufAddr += VISION_ORC_BUF_OFFSET;
    AmbaMisra_TypeCast(&pBuf,&BufAddr);

    RetVal = CvFlow_GetFileSize_ROMFS(pFileName, &DataSize);
    SizeAligned = GetAlignedValU32(DataSize, AMBA_CACHE_LINE_SIZE);

    if ((RetVal == SVC_OK) && (SizeAligned <= VISION_ORC_BUF_SIZE)) {
        RetVal = CvFlow_LoadFile_ROMFS(pFileName, pBuf, VISION_ORC_BUF_SIZE, DataSize);
        if (RetVal == SVC_OK) {
            #if defined(CONFIG_ICAM_TIMING_LOG)
            SvcTime_CalCvVisOrcSize(DataSize); // get VisOrc data size
            #endif
            RetVal = SvcPlat_CacheClean(BufAddr, SizeAligned);
            if (CACHE_ERR_NONE != RetVal) {
                CvFlow_Err("Clean cache error(0x%x)", RetVal, 0U);
            }
        } else {
            CvFlow_Err("LoadFile error(0x%x)", RetVal, 0U);
        }
    } else {
        CvFlow_Err("GetFileSize error(0x%x)", RetVal, 0U);
    }

    if (RetVal == SVC_OK) {
        AmbaPrint_PrintStr5("SvcCvFlow_AddVisOrcBin: %s done", pFileName, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5("SvcCvFlow_AddVisOrcBin: %s error", pFileName, NULL, NULL, NULL, NULL);
    }
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_VISORC_LOAD_DONE, "CV VisOrc load DONE");
    #endif

    return RetVal;
#endif
}

/**
* Load CV flow table to CV section from storage
* @param [in] pFileName file name
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_AddCVTable(const char *pFileName)
{
    UINT32 RetVal;

    RetVal = AmbaCV_SchdrAddCvtable(pFileName, CvFlow_LoadRomfsBinary);
    if (RetVal == OK) {
        AmbaPrint_PrintStr5("SvcCvFlow_AddCVTable: %s done", pFileName, NULL, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5("SvcCvFlow_AddCVTable: %s error", pFileName, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

/**
* Allocate CV working buffer of SvcCvFlow utility
* @param [in] ReqSize size to be requested
* @param [out] pBufInfo resulting data buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_AllocWorkBuf(UINT32 ReqSize, SVC_CV_FLOW_BUF_INFO_s *pBufInfo)
{
    UINT32 RetVal;

    if (NULL != pBufInfo) {
        RetVal = CvFlow_AllocBuf(SVC_CV_FLOW_MEM_WORK, &pBufInfo->pAddr, ReqSize, &pBufInfo->Size);
        pBufInfo->DataSize = 0U;
    } else {
        CvFlow_Err("SvcCvFlow_AllocWorkBuf: invalid param", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Dump CV buffer usage
* @return 0-OK, 1-NG
*/
void SvcCvFlow_DumpBufUsage(void)
{
    const SVC_CV_FLOW_MEM_REGION_s *pMemReg;

    AmbaPrint_PrintUInt5("[CV Memory]", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("CONFIG_CV_MEM_SIZE           = %d", CONFIG_CV_MEM_SIZE, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("CONFIG_CV_MEM_SCHDR_SIZE     = %d", CONFIG_CV_MEM_SCHDR_SIZE, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("CONFIG_ICAM_CV_USER_RESERVED = %d", CONFIG_ICAM_CV_USER_RESERVED, 0U, 0U, 0U, 0U);

    AmbaPrint_PrintUInt5("[CvFlowUtil]", 0U, 0U, 0U, 0U, 0U);
    pMemReg = &CvFlowMem[SVC_CV_FLOW_MEM_BIN];
    SvcCvFlow_PrintULong("Binary Buffer:  Base: 0x%x, Size: %d, Used: %d",
        pMemReg->Base, pMemReg->Size, pMemReg->UsedSize, 0U, 0U);

    pMemReg = &CvFlowMem[SVC_CV_FLOW_MEM_WORK];
    SvcCvFlow_PrintULong("Working Buffer: Base: 0x%x, Size: %d, Used: %d",
        pMemReg->Base, pMemReg->Size, pMemReg->UsedSize, 0U, 0U);
}

/* The buffer will be 128 byte aligned. */
static UINT32 CvFlow_AllocBuf(UINT32 Type, UINT8 **pBuf, UINT32 ReqSize, UINT32 *pAlignedSize)
{
    UINT32 RetVal;
    SVC_CV_FLOW_MEM_REGION_s *pMemReg = &CvFlowMem[Type];
    ULONG CurAddr;
    UINT32 SizeAligned, RemainSize;

    //AmbaPrint_PrintUInt5("CvFlow_AllocBuf: MemType[%d], Used %d, ReqSize %d", Type, pMemReg->UsedSize, ReqSize, 0U, 0U);

    if ((Type < SVC_CV_FLOW_MEM_NUM) && (pMemReg->Base != 0U) && (pMemReg->Size != 0U)) {
        SizeAligned = GetAlignedValU32(ReqSize, 128);
        RemainSize  = pMemReg->Size - pMemReg->UsedSize;
        if (RemainSize >= SizeAligned) {
            CurAddr = pMemReg->Base + pMemReg->UsedSize;
            AmbaMisra_TypeCast(pBuf, &CurAddr);
            pMemReg->UsedSize += SizeAligned;
            *pAlignedSize = SizeAligned;

            SvcCvFlow_PrintULong("CvFlow_AllocBuf: 0x%x, Size %d", CurAddr, (ULONG)SizeAligned, 0U, 0U, 0U);
            RetVal = SVC_OK;
        } else {
            CvFlow_Err("CvFlow_AllocBuf: out of mem. Remain %d, Req %d", RemainSize, SizeAligned);
            RetVal = SVC_NG;
        }
    } else {
        CvFlow_Err("CvFlow_AllocBuf: invalid mem type(%d)", Type, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 CvFlow_SearchEmptySlot(SVC_CV_FLOW_BIN_INFO_s *pMgr, UINT32 MaxSlot, UINT32 *pEmptySlot)
{
    UINT32 i, Found = 0;

    AmbaMisra_TouchUnused(pMgr);

    /* Search empty slot */
    for (i = 0; i < MaxSlot; i++) {
        if (0U == (UINT32) pMgr[i].Used) {
            *pEmptySlot = i;
            Found = 1U;
            break;
        }
    }

    return Found;
}

static UINT32 CvFlow_FindSlotByName(SVC_CV_FLOW_BIN_INFO_s *pMgr, UINT32 MaxSlot, const char *pTarget, UINT32 *pSlot)
{
#if 1
    UINT32 i, Found = 0;

    AmbaMisra_TouchUnused(pMgr);

    /* Search existing */
    for (i = 0; i < MaxSlot; i++) {
        if (0U != (UINT32) pMgr[i].Used) {
            if (0 == SvcWrap_strcmp(pMgr[i].Name, pTarget)) {
                Found = 1U;
                *pSlot = i;
                break;
            }
        }
    }

    return Found;
#else
    (void) pMgr;
    (void) MaxSlot;
    (void) pTarget;
    (void) pSlot;

    return 0;
#endif
}

/**
* Get file size
* @param [in] StorageType storage type
* @param [in] pFileName file name
* @param [out] pSize the file size
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_GetFileSize(const char *pFileName, UINT32 StorageType, UINT32 *pSize)
{
    return CvFlowGetFileSizeFunc[StorageType](pFileName, pSize);
}

/**
* Load file data to user defined buffer from storage
* @param [in] pFileName file name
* @param [in] StorageType storage type
* @param [in] pBufInfo buffer information
* @return 0-OK, 1-NG
*/
UINT32 SvcCvFlow_LoadFile(const char *pFileName, UINT32 StorageType, SVC_CV_FLOW_BUF_INFO_s *pBufInfo)
{
    return CvFlowLoadFileFunc[StorageType](pFileName,
                                           pBufInfo->pAddr,
                                           pBufInfo->Size,
                                           pBufInfo->DataSize,
                                           &pBufInfo->DataSize);
}

static UINT32 CvFlow_GetFileSize_SD(const char *pFileName, UINT32 *pSize)
{
    UINT32 RetVal;
    UINT32 FileSize = 0U;
    UINT64 FilePos = 0U;
    AMBA_FS_FILE *pFile;

    if ((NULL != pFileName) && (NULL != pSize)) {
        RetVal = AmbaFS_FileOpen(pFileName, "rb", &pFile);
        if (RetVal == 0U) {
            RetVal = AmbaFS_FileSeek(pFile, 0LL, AMBA_FS_SEEK_END);
            if(RetVal == 0U) {
                RetVal = AmbaFS_FileTell(pFile, &FilePos);
                if(RetVal == 0U) {
                    FileSize = (UINT32)(FilePos & 0xFFFFFFFFU);
                    RetVal = AmbaFS_FileClose(pFile);
                    if(RetVal == 0U) {
                        *pSize = FileSize;
                        RetVal = SVC_OK;
                    } else {
                        RetVal = 5U;
                    }
                } else {
                    RetVal = 4U;
                }
            } else {
                RetVal = 3U;
            }
        } else {
            RetVal = 2U;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (RetVal != SVC_OK) {
        CvFlow_Err("CvFlow_GetFileSize_SD: error(%d)", RetVal, 0U);
        AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
    }

    return RetVal;
}

static UINT32 CvFlow_LoadFile_SD(const char *pFileName, UINT8 *pBuf, UINT32 BufSize, UINT32 DataSize, UINT32 *pRealSize)
{
    UINT32 RetVal;
    AMBA_FS_FILE *pFile;
    UINT32 BytesRead;
    ULONG BufAddr;

    if ((NULL != pFileName) && (NULL != pBuf) && (BufSize >= DataSize)) {
        RetVal = AmbaFS_FileOpen(pFileName, "rb", &pFile);
        if (RetVal == AMBA_FS_ERR_NONE) {
            AmbaPrint_PrintStr5("[CV_FLOW|OK]: Loading %s", pFileName, NULL, NULL, NULL, NULL);

            RetVal = AmbaFS_FileRead(pBuf, 1U, (UINT32)DataSize, pFile, &BytesRead);
            AmbaMisra_TypeCast(&BufAddr, &pBuf);
            SvcCvFlow_PrintULong("[CV_FLOW|OK]: Load done. size = %d, addr = 0x%x", BytesRead, BufAddr, 0U, 0U, 0U);

            if (RetVal == AMBA_FS_ERR_NONE) {
                RetVal = AmbaFS_FileClose(pFile);
                if (RetVal == AMBA_FS_ERR_NONE) {
                    *pRealSize = BytesRead;
                    RetVal = SVC_OK;
                } else {
                    RetVal = 4U;
                }
            } else {
                RetVal = 3U;
            }
        } else {
            RetVal = 2U;
        }
    } else {
        RetVal = 1U;
    }

    if (RetVal != SVC_OK) {
        CvFlow_Err("CvFlow_LoadFile_SD: error(%d)", RetVal, 0U);
        AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
    } else {
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime_CalCvTotalSize(DataSize);
        #else
        /* do nothing */
        #endif
    }

    return RetVal;
}

static UINT32 CvFlow_GetFileSize_ROMFS(const char *pFileName, UINT32 *pSize)
{
    UINT32 RetVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_DSP_uCODE, pFileName, pSize);
    if(RetVal == NVM_ERR_NONE) {
        RetVal = SVC_OK;
    } else {
        CvFlow_Err("CvFlow_GetFileSize_ROMFS: error(%d)", RetVal, 0U);
        AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
        RetVal = 2U;
    }

    return RetVal;
}

static UINT32 CvFlow_LoadFile_ROMFS(const char *pFileName, UINT8 *pBuf, UINT32 BufSize, UINT32 DataSize, UINT32 *pRealSize)
{
    #define SECURE_SIGNATURE_SIZE   (256U)
    UINT32 RetVal = SVC_OK;
    ULONG  BufAddr;

    if (DataSize > BufSize) {
        SvcLog_NG(SVC_LOG_CV_FLOW, "Buf is too small. Buf size = %d, file size = %d", (UINT32)BufSize, DataSize);
        RetVal = 2U;
    } else {
        AmbaMisra_TypeCast(&BufAddr, &pBuf);
        AmbaPrint_PrintStr5("[CV_FLOW|OK]: Loading %s", pFileName, NULL, NULL, NULL, NULL);
        RetVal = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_DSP_uCODE, pFileName, 0U, DataSize, pBuf, 5000);
        SvcCvFlow_PrintULong("[CV_FLOW|OK]: Load done. size = %d, addr = 0x%x", DataSize, BufAddr, 0U, 0U, 0U);

        if (RetVal == NVM_ERR_NONE) {
            /*  256 bytes digital signature are attached at the binaries which are put in secure partition. */
            *pRealSize = DataSize - SECURE_SIGNATURE_SIZE;
            #if defined(CONFIG_ICAM_TIMING_LOG)
            SvcTime_CalCvTotalSize(DataSize);
            #endif
            RetVal = SVC_OK;
        } else {
            RetVal = 3U;
        }
    }

    if (RetVal != SVC_OK) {
        CvFlow_Err("CvFlow_LoadFile_ROMFS: error(%d)", RetVal, 0U);
        AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
    }

    return SVC_OK;
}

static uint32_t CvFlow_SchdrLoadCallback(const char *pFileName, void* pVoidBuf, uint32_t BufSize, uint32_t *pRealSize)
{
    UINT32 RetVal;
    UINT32 DataSize, SizeAligned;
    UINT8  *pBuf;

    AmbaMisra_TouchUnused(pVoidBuf);

    RetVal = CvFlow_GetFileSize_ROMFS(pFileName, &DataSize);
    SizeAligned = GetAlignedValU32(DataSize, (UINT32)AMBA_CACHE_LINE_SIZE);

    if ((RetVal == SVC_OK) && (SizeAligned <= BufSize)) {
        AmbaMisra_TypeCast(&pBuf, &pVoidBuf);
        RetVal = CvFlow_LoadFile_ROMFS(pFileName, pBuf, BufSize, DataSize, pRealSize);
        if (RetVal == SVC_OK) {
            /* Cache clean in CV framework internally */
        } else {
            CvFlow_Err("LoadFile error(0x%x)", RetVal, 0U);
        }
    } else {
        CvFlow_Err("GetFileSize error(0x%x)", RetVal, 0U);
    }

    return RetVal;
}

static int32_t CvFlow_LoadRomfsBinary(char *pBuf, int32_t BufSize, void* pToken)
{
    UINT32 RetVal;
    const char *pFileName;
    UINT32 DataSize, SizeAligned, RealSize = 0U;
    UINT8  *pBufU8;

    AmbaMisra_TouchUnused(pBuf);
    AmbaMisra_TouchUnused(pToken);

    AmbaMisra_TypeCast(&pFileName, &pToken);
    RetVal = CvFlow_GetFileSize_ROMFS(pFileName, &DataSize);
    SizeAligned = GetAlignedValU32(DataSize, (UINT32)AMBA_CACHE_LINE_SIZE);

    if(RetVal == SVC_OK) {
        if (BufSize == 0) {
            /* scheduler query file size */
            DataSize = SizeAligned;
        } else if ((int32_t)SizeAligned > BufSize) {
            SvcLog_OK(SVC_LOG_CV_FLOW, "Buf is too small. Buf Size = %d, File Size = %d", (UINT32)BufSize, DataSize);
            AmbaPrint_PrintStr5("FileName = %s", pFileName, NULL, NULL, NULL, NULL);
            DataSize = 0U;
        } else {
            AmbaMisra_TypeCast(&pBufU8, &pBuf);
            RetVal = CvFlow_LoadFile_ROMFS(pFileName, pBufU8, (UINT32)BufSize, DataSize, &RealSize);
            if (RetVal == SVC_OK) {
                /* Cache clean in CV framework internally */
            } else {
                CvFlow_Err("LoadFile error(0x%x)", RetVal, 0U);
                DataSize = 0U;
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_FLOW, "LoadRomfsBinary cannot find:", 0U, 0U);
        AmbaPrint_PrintStr5("%s", pFileName, NULL, NULL, NULL, NULL);
        DataSize = 0U;
    }

    return (INT32)DataSize;
}


static void CvFlow_PackFdagMemBlkInfo(void *pAddr, UINT32 Size, flexidag_memblk_t *pMemBlk)
{
    ULONG Vaddr, Paddr = 0U;

    pMemBlk->buffer_cacheable = 1;
    AmbaMisra_TypeCast(&Vaddr, &pAddr);
    AmbaMisra_TypeCast(&pMemBlk->pBuffer, &Vaddr);
    pMemBlk->buffer_size = Size;
    if (SVC_OK != SvcMem_VirtToPhys(Vaddr, &Paddr)) {
        SvcLog_NG(SVC_LOG_CV_FLOW, "CvFlow_PackFdagMemBlkInfo error.", 0U, 0U);
    }
    pMemBlk->buffer_daddr = Paddr;
    pMemBlk->buffer_caddr = Paddr;

    AmbaMisra_TouchUnused(pAddr);

    //SvcCvFlow_PrintULong("CvFlow_PackFdagMemBlkInfo: VA 0x%x, PA 0x%x, CA 0x%x, Size 0x%x",
    //    Vaddr, pMemBlk->buffer_daddr, pMemBlk->buffer_caddr, pMemBlk->buffer_size, 0U);
}

/**
* Get CV Chip ID
* @return ChipID
*/
UINT32 SvcCvFlow_GetChipID(void)
{
    UINT32 ChipID;
#if defined (CONFIG_SOC_CV2)
    ChipID = CVCHIP_CV2;
#elif defined (CONFIG_SOC_CV22)
    ChipID = CVCHIP_CV22;
#elif defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
    ChipID = CVCHIP_CV2A;
#elif defined (CONFIG_SOC_CV25)
    ChipID = CVCHIP_CV25;
#elif defined (CONFIG_SOC_CV28)
    ChipID = CVCHIP_CV28;
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    ChipID = CVCHIP_CV5;
#else
    ChipID = 0x0;
#endif

    return ChipID;
}

/**
* Print message with ULONG arguments
* @param [in] pFormat string format
* @param [in] Arg1 argument1
* @param [in] Arg2 argument2
* @param [in] Arg3 argument3
* @param [in] Arg4 argument4
* @param [in] Arg5 argument5
* @return none
*/
void SvcCvFlow_PrintULong(const char *pFormat, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5)
{
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX) || defined (CONFIG_THREADX64)
#define LOG_BUF_SIZE        256U
#define LOG_ARGC            5U
    char        LogBuf[LOG_BUF_SIZE];
    UINT64      ArgS[LOG_ARGC];
    const UINT64 *pArgS = ArgS;
    UINT32      Rval;

    ArgS[0U] = (UINT64)Arg1;
    ArgS[1U] = (UINT64)Arg2;
    ArgS[2U] = (UINT64)Arg3;
    ArgS[3U] = (UINT64)Arg4;
    ArgS[4U] = (UINT64)Arg5;
    Rval = AmbaUtility_StringPrintUInt64(LogBuf, LOG_BUF_SIZE, pFormat, 5U, pArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintStr5(LogBuf, NULL, NULL, NULL, NULL, NULL);
    }
#else
    AmbaPrint_PrintUInt5(pFormat, (UINT32)Arg1, (UINT32)Arg2, (UINT32)Arg3, (UINT32)Arg4, (UINT32)Arg5);
#endif
}

