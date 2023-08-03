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
#include "cvapi_flexidag_ambasoffex.h"
#include "cvapi_protection.h"

static SOF_FEX_FD_HANDLE_s* pSofHdlr;

static UINT16 gProcImageWidth[MAX_VP_SOF_FEX_INSTANCE];
static UINT16 gProcImagePitch[MAX_VP_SOF_FEX_INSTANCE];
static UINT16 gProcImageHeight[MAX_VP_SOF_FEX_INSTANCE];
static UINT16 gProcRoiStartX[MAX_VP_SOF_FEX_INSTANCE];
static UINT16 gProcRoiStartY[MAX_VP_SOF_FEX_INSTANCE];
static UINT16 gProcRoiWidth[MAX_VP_SOF_FEX_INSTANCE];
static UINT16 gProcRoiHeight[MAX_VP_SOF_FEX_INSTANCE];

static inline UINT32 SofFexUT_CacheAligned (UINT32 Val)
{
    return (((Val) + (AMBA_CACHE_LINE_SIZE - 1U)) & ~(AMBA_CACHE_LINE_SIZE - 1U));
}

static void SofFexUT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

static void SofFexUT_ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 U32Buf;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        AmbaMisra_TypeCast32(&U32Buf, &buf);
        Ret = AmbaCache_DataClean(U32Buf, SofFexUT_CacheAligned(Fsize));
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void SofFexUT_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SofFexUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 SofFexUT_LoadFlexiDagBin(const char* path, flexidag_memblk_t *BinBuf, UINT32* Size)
{
    UINT32 BinSize;
    UINT32 ret = 0U;

    SofFexUT_GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        ret = RefCV_MemblkAlloc(BinSize, BinBuf);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("SofFexUT_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(path, BinBuf);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("SofFexUT_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *Size = BinBuf->buffer_size;
        }
    } else {
        AmbaPrint_PrintUInt5("SofFexUT_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    }
    return ret;
}

static void RefCV_AmbaSofFex_UT_Init(const char *FlexiBinPath, REF_FD_HANDLE_s *pHdlr)
{
    UINT32 Ret;
    flexidag_memblk_t BinMemblk = {NULL, 0, 0, 0, 0, 0};
    flexidag_memblk_t StateMemblk = {NULL, 0, 0, 0, 0, 0};
    flexidag_memblk_t TempMemblk = {NULL, 0, 0, 0, 0, 0};
    const UINT8 *pBinBuff;
    UINT8 *pStateBuff;
    UINT8 *pTempBuff;
    UINT32 BinSize = 0U;
    UINT32 StateSize = 0U;
    UINT32 TempSize = 0U;
    UINT32 OutputNum;
    UINT32 OutputSz[8U];

    (void)SofFexUT_LoadFlexiDagBin(FlexiBinPath, &BinMemblk, &BinSize);
    AmbaMisra_TypeCast32(&pBinBuff, &BinMemblk.pBuffer);
    Ret = RefCVUtil_FlexiDagOpen(pBinBuff, BinSize,
                                 &StateSize, &TempSize, &OutputNum, OutputSz, pHdlr);
    if (Ret == 0U) {
        AmbaPrint_PrintUInt5("OutputNum:%d", OutputNum, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        if(StateSize == 0U) {
            StateSize = 1U;
        }
        Ret = RefCV_MemblkAlloc(StateSize, &StateMemblk);
        AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", StateSize, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        if(TempSize == 0U) {
            TempSize = 1U;
        }
        Ret = RefCV_MemblkAlloc(TempSize, &TempMemblk);
        AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", TempSize, 0U, 0U, 0U, 0U);
    }

    if (Ret == 0U) {
        (void)RefCV_MemblkClean(&StateMemblk);
        (void)RefCV_MemblkClean(&TempMemblk);
        AmbaMisra_TypeCast32(&pStateBuff, &StateMemblk.pBuffer);
        AmbaMisra_TypeCast32(&pTempBuff, &TempMemblk.pBuffer);
        Ret = RefCVUtil_FlexiDagInit(pHdlr, pStateBuff, pTempBuff);
        AmbaPrint_PrintUInt5("AmbaSofFex_Init : Rval = %d", Ret, 0U, 0U, 0U, 0U);
    }
}

static void SofFexUT_SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
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

#define DUMP_KEY_POINTS_COUNT_SIZE   (sizeof(UINT8)*CV_FEX_MAX_BUCKETS)
#define DUMP_KEY_POINTS_SIZE   (sizeof(SOF_FEX_KEYPOINT_SCORE_s)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS)
#define DUMP_KEY_DESP_SIZE   (sizeof(UINT8)*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS*CV_FEX_DESCRIPTOR_SIZE)
static void RefCV_AmbaSofFex_UT_Dump(UINT32 InLayerNum,
                                     const AMBA_CV_SPU_DATA_s *pOutSof, const AMBA_CV_FEX_DATA_s *pOutFex, const char* pDumpFileName)
{
    static UINT32 BufferAlloc = 0U;
    static UINT32 DumpTotalSize = 0U;
    static flexidag_memblk_t AllocDumpBuff = {NULL, 0, 0, 0, 0, 0};
    UINT32 OctaveId;

    /*
        Dump layout:

        AMBA_CV_SPU_DATA_s
        =================================
        1st layer SOF data
        =================================
        2ed layer SOF data
        =================================
        AMBA_CV_FEX_DATA_s
        =================================
        1st layer Primary key point count data
        =================================
        1st layer Primary key point data
        =================================
        1st layer Primary key description data
        =================================
        1st layer Secondary key point count data
        =================================
        1st layer Secondary key point data
        =================================
        1st layer Secondary key description data
        =================================
        2ed layer Primary key point count data
        =================================
        2ed layer Primary key point data
        =================================
        2ed layer Primary key description data
        =================================
        2ed layer Secondary key point count data
        =================================
        2ed layer Secondary key point data
        =================================
        2ed layer Secondary key description data
       */

    if ( BufferAlloc == 0U ) {
        UINT32 AllocRet = 0U;

        UINT32 SofHeaderSize;
        UINT32 SofDataSize;

        UINT32 FexHeaderSize;
        UINT32 FexDataSize;

        SofHeaderSize = sizeof(AMBA_CV_SPU_DATA_s);
        FexHeaderSize = sizeof(AMBA_CV_FEX_DATA_s);

        SofDataSize = 0U;
        FexDataSize = 0U;
        for(OctaveId = 0U; OctaveId < InLayerNum; OctaveId++) {
            const AMBA_CV_SPU_SCALE_OUT_s *pScale;
            pScale = &pOutSof->Scales[OctaveId];

            SofDataSize += pScale->DisparityWidth*pScale->DisparityHeight*2U;
            // primary + secondary
            FexDataSize += (DUMP_KEY_POINTS_COUNT_SIZE + DUMP_KEY_POINTS_SIZE + DUMP_KEY_DESP_SIZE) * 2U;
        }

        DumpTotalSize = (SofHeaderSize + SofDataSize) + (FexHeaderSize + FexDataSize);
        AllocRet |= RefCV_MemblkAlloc(DumpTotalSize, &AllocDumpBuff);

        if ( AllocRet != 0U ) {
            AmbaPrint_PrintUInt5("RefCV_MemblkAlloc Failed, Rval = %d", AllocRet, 0U, 0U, 0U, 0U);
        } else {
            BufferAlloc = 1U;
        }
    }

    if ( BufferAlloc == 1U ) {
        UINT32 Offset;
        UINT32 CpSize;

        Offset = 0U;

        // Write Sof
        CpSize = sizeof(AMBA_CV_SPU_DATA_s);
        SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pOutSof, CpSize), "AmbaWrap_memcpy", __func__);
        Offset += CpSize;
        for(OctaveId = 0U; OctaveId < InLayerNum; OctaveId++) {
            const AMBA_CV_SPU_SCALE_OUT_s *pScale;
            UINT32 SOfBaseAddr;
            UINT32 SOfDataAddr;
            const void* pSofDataPtr;

            AmbaMisra_TypeCast32(&SOfBaseAddr, &pOutSof);

            pScale = &pOutSof->Scales[OctaveId];
            SOfDataAddr = SOfBaseAddr + pScale->DisparityMapOffset;
            AmbaMisra_TypeCast32(&pSofDataPtr, &SOfDataAddr);

            CpSize = pScale->DisparityWidth*pScale->DisparityHeight*2U;
            SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pSofDataPtr, CpSize), "AmbaWrap_memcpy", __func__);
            Offset += CpSize;
        }

        // Write Fex
        CpSize = sizeof(AMBA_CV_FEX_DATA_s);
        SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pOutFex, CpSize), "AmbaWrap_memcpy", __func__);
        Offset += CpSize;
        for (OctaveId = 0U; OctaveId < InLayerNum; OctaveId++) {
            const AMBA_CV_FEX_FEATURE_LIST_s *pPrimaryList;
            const AMBA_CV_FEX_FEATURE_LIST_s *pSecondaryList;
            UINT32 FexBaseAddr;
            UINT32 DataAddr;
            const void* pDataPtr;

            pPrimaryList = &pOutFex->PrimaryList[OctaveId];
            pSecondaryList = &pOutFex->SecondaryList[OctaveId];

            AmbaMisra_TypeCast32(&FexBaseAddr, &pOutFex);
            DataAddr = FexBaseAddr + pPrimaryList->KeypointsCountOffset;
            AmbaMisra_TypeCast32(&pDataPtr, &DataAddr);
            CpSize = DUMP_KEY_POINTS_COUNT_SIZE;
            SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pDataPtr, CpSize), "AmbaWrap_memcpy", __func__);
            Offset += CpSize;

            AmbaMisra_TypeCast32(&FexBaseAddr, &pOutFex);
            DataAddr = FexBaseAddr + pPrimaryList->KeypointsOffset;
            AmbaMisra_TypeCast32(&pDataPtr, &DataAddr);
            CpSize = DUMP_KEY_POINTS_SIZE;
            SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pDataPtr, CpSize), "AmbaWrap_memcpy", __func__);
            Offset += CpSize;

            AmbaMisra_TypeCast32(&FexBaseAddr, &pOutFex);
            DataAddr = FexBaseAddr + pPrimaryList->DescriptorsOffset;
            AmbaMisra_TypeCast32(&pDataPtr, &DataAddr);
            CpSize = DUMP_KEY_DESP_SIZE;
            SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pDataPtr, CpSize), "AmbaWrap_memcpy", __func__);
            Offset += CpSize;

            AmbaMisra_TypeCast32(&FexBaseAddr, &pOutFex);
            DataAddr = FexBaseAddr + pSecondaryList->KeypointsCountOffset;
            AmbaMisra_TypeCast32(&pDataPtr, &DataAddr);
            CpSize = DUMP_KEY_POINTS_COUNT_SIZE;
            SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pDataPtr, CpSize), "AmbaWrap_memcpy", __func__);
            Offset += CpSize;

            AmbaMisra_TypeCast32(&FexBaseAddr, &pOutFex);
            DataAddr = FexBaseAddr + pSecondaryList->KeypointsOffset;
            AmbaMisra_TypeCast32(&pDataPtr, &DataAddr);
            CpSize = DUMP_KEY_POINTS_SIZE;
            SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pDataPtr, CpSize), "AmbaWrap_memcpy", __func__);
            Offset += CpSize;

            AmbaMisra_TypeCast32(&FexBaseAddr, &pOutFex);
            DataAddr = FexBaseAddr + pSecondaryList->DescriptorsOffset;
            AmbaMisra_TypeCast32(&pDataPtr, &DataAddr);
            CpSize = DUMP_KEY_DESP_SIZE;
            SofFexUT_CheckRval(AmbaWrap_memcpy(&AllocDumpBuff.pBuffer[Offset], pDataPtr, CpSize), "AmbaWrap_memcpy", __func__);
            Offset += CpSize;
        }

        SofFexUT_SaveBin((UINT8*)AllocDumpBuff.pBuffer, pDumpFileName, DumpTotalSize);
    }

    return;
}

static void RefCV_AmbaSofFex_UT_Run(UINT32 InLayerNum, const char* const InFileL[], const char* const InFileR[],
                                    const char* DumpFileName)
{
    static UINT32 BufferAlloc = 0U;
    static flexidag_memblk_t InBuf = {NULL, 0, 0, 0, 0, 0};
    AMBA_CV_SPU_BUF_s OutOf;
    AMBA_CV_FEX_BUF_s OutFex;
    UINT32 Rval;
    void* VoidPtr;
    UINT32 U32Addr;
    AMBA_CV_IMG_BUF_s InInfo;
    UINT32 OctaveId;
    UINT32 VPInstIdx;
    UINT32 InputBufSize;
    AMBA_CV_SPUFEX_CFG_s SofFexCfg;
    UINT32 AllocImagePitch = gProcImagePitch[0U];
    UINT32 AllocImageHeight = gProcImageHeight[0U];
    UINT32 Idex;

    SofFexUT_CheckRval(AmbaWrap_memset(&OutOf, 0, sizeof(OutOf)), "AmbaWrap_memcpy", __func__);
    SofFexUT_CheckRval(AmbaWrap_memset(&OutFex, 0, sizeof(OutFex)), "AmbaWrap_memcpy", __func__);

    // Allocate input buffer
    // Size = size of (memio_source_recv_picinfo_t + image pairs)
    InputBufSize = SOF_FEX_ALIGN128(sizeof(memio_source_recv_picinfo_t)) +
                   (SOF_FEX_ALIGN128(AllocImagePitch*AllocImageHeight)*(InLayerNum*2U));
    if (BufferAlloc == 0U) {
        //alloc input buffer
        if(0U != RefCV_MemblkAlloc(InputBufSize, &InBuf)) {
            AmbaPrint_PrintUInt5("Err RefCV_AmbaSofFex_UT_Run() RefCV_MemblkAlloc Fail", 0U, 0U, 0U, 0U, 0U);
        }
        BufferAlloc = 1U;
    }

    SofFexUT_CheckRval(AmbaWrap_memcpy(&InInfo, &InBuf, sizeof(AMBA_CV_IMG_BUF_s)), "AmbaWrap_memcpy", __func__);

    AmbaMisra_TypeCast32(&U32Addr, &InInfo.pBuffer);
    for (Idex = 0U; Idex < InLayerNum; Idex++) {
        //read I-th left Y
        if ( Idex == 0U ) {
            U32Addr += SOF_FEX_ALIGN128(sizeof(memio_source_recv_picinfo_t));
        } else {
            U32Addr += SOF_FEX_ALIGN128(AllocImagePitch*AllocImageHeight);
        }
        AmbaMisra_TypeCast32(&VoidPtr, &U32Addr);
        SofFexUT_ReadInputFile(InFileL[Idex], VoidPtr);

        //read I-th right Y
        U32Addr += SOF_FEX_ALIGN128(AllocImagePitch*AllocImageHeight);
        AmbaMisra_TypeCast32(&VoidPtr, &U32Addr);
        SofFexUT_ReadInputFile(InFileR[Idex], VoidPtr);
    }

    //Assign y relative address
    for (Idex = 0U; Idex < InLayerNum; Idex++) {
        if ( Idex == 0U ) {
            U32Addr = SOF_FEX_ALIGN128(sizeof(memio_source_recv_picinfo_t));
        } else {
            U32Addr += SOF_FEX_ALIGN128(AllocImagePitch*AllocImageHeight);
        }
        AmbaMisra_TypeCast32(&(InInfo.pBuffer->pic_info.rpLumaLeft[Idex]), &U32Addr);
        U32Addr += SOF_FEX_ALIGN128(AllocImagePitch*AllocImageHeight);
        AmbaMisra_TypeCast32(&(InInfo.pBuffer->pic_info.rpLumaRight[Idex]), &U32Addr);
    }

    // Activate & configure ROI of layers
    VPInstIdx = 0U;
    for(OctaveId = 0U; OctaveId < InLayerNum; OctaveId++) {
        // Sof
        SofFexCfg.SpuScaleCfg[OctaveId].Enable = 1U;
        SofFexCfg.SpuScaleCfg[OctaveId].RoiEnable = 1U;
        SofFexCfg.SpuScaleCfg[OctaveId].Roi.m_start_row = gProcRoiStartY[VPInstIdx];
        SofFexCfg.SpuScaleCfg[OctaveId].Roi.m_start_col = gProcRoiStartX[VPInstIdx];
        SofFexCfg.SpuScaleCfg[OctaveId].Roi.m_width_m1 = gProcRoiWidth[VPInstIdx] - 1U;
        SofFexCfg.SpuScaleCfg[OctaveId].Roi.m_height_m1 = gProcRoiHeight[VPInstIdx] - 1U;

        // Fex
        SofFexCfg.FexScaleCfg[OctaveId].Enable = (UINT8)CV_FEX_ENABLE_ALL;
        SofFexCfg.FexScaleCfg[OctaveId].RoiEnable = 1U;
        SofFexCfg.FexScaleCfg[OctaveId].Roi.m_start_row = gProcRoiStartY[VPInstIdx];
        SofFexCfg.FexScaleCfg[OctaveId].Roi.m_start_col = gProcRoiStartX[VPInstIdx];
        SofFexCfg.FexScaleCfg[OctaveId].Roi.m_width_m1 = gProcRoiWidth[VPInstIdx] - 1U;
        SofFexCfg.FexScaleCfg[OctaveId].Roi.m_height_m1 = gProcRoiHeight[VPInstIdx] - 1U;
        SofFexCfg.FexScaleCfg[OctaveId].SecondaryRoiColOffset = 0;
        SofFexCfg.FexScaleCfg[OctaveId].SecondaryRoiRowOffset = 0;

        VPInstIdx++;
    }

    // Deactivate other layers
    for(OctaveId = InLayerNum; OctaveId < MAX_HALF_OCTAVES; OctaveId++) {
        SofFexCfg.SpuScaleCfg[OctaveId].Enable = 0U;
        SofFexCfg.FexScaleCfg[OctaveId].Enable = (UINT8)CV_FEX_DISABLE;
    }

    Rval = AmbaSofFex_Cfg(pSofHdlr, &SofFexCfg);
    AmbaPrint_PrintUInt5("AmbaSofFex_Cfg : Rval = %d", Rval, 0U, 0U, 0U, 0U);

    InInfo.pBuffer->pic_info.frame_num = 0U;
    InInfo.pBuffer->pic_info.pyramid.image_height_m1 = gProcImageHeight[0U] - 1U;
    InInfo.pBuffer->pic_info.pyramid.image_width_m1 = gProcImageWidth[0U] - 1U;
    InInfo.pBuffer->pic_info.pyramid.image_pitch_m1 = (UINT32)gProcImagePitch[0U] - 1U;

    VPInstIdx = 0U;
    for(OctaveId = 0U; OctaveId < InLayerNum; OctaveId++) {
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].ctrl.disable = 0U;
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].ctrl.mode = 1U;
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].ctrl.octave_mode = 0U;
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].ctrl.roi_pitch = gProcImagePitch[VPInstIdx];
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].roi_start_row = 0;
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].roi_start_col = 0;
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].roi_height_m1 = gProcImageHeight[VPInstIdx] - 1U;
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].roi_width_m1 = gProcImageWidth[VPInstIdx] - 1U;
        VPInstIdx++;
    }

    for(OctaveId = InLayerNum; OctaveId < MAX_HALF_OCTAVES; OctaveId++) {
        InInfo.pBuffer->pic_info.pyramid.half_octave[OctaveId].ctrl.disable = 1U;
    }

    Rval = AmbaSofFex_Process(pSofHdlr, &InInfo, &OutOf, &OutFex);
    AmbaPrint_PrintUInt5("AmbaSofFex_Process : Rval = %d", Rval, 0U, 0U, 0U, 0U);

    Rval = AmbaSofFex_GetResult(pSofHdlr, &InInfo, &OutOf, &OutFex);
    AmbaPrint_PrintUInt5("AmbaSofFex_GetResult : Rval = %d", Rval, 0U, 0U, 0U, 0U);

    RefCV_AmbaSofFex_UT_Dump(InLayerNum, OutOf.pBuffer, OutFex.pBuffer, DumpFileName);

    return;
}

void RefCV_AmbaSofFex_UT(void)
{
    static UINT32 Init = 0U;
    static UINT32 InLayerNum;
    static REF_FD_HANDLE_s gHarrisHandler[MAX_VP_SOF_FEX_INSTANCE];
    static REF_FD_HANDLE_s gLKOpcFlowHandler[MAX_VP_SOF_FEX_INSTANCE];
    UINT32 VPInstIdx;
    const char *InPathL_Name[] = {"C:/SofFex_UT_Cur_1280x720.y", "C:/SofFex_UT_Cur_320x240.y"};
    const char *InPathR_Name[] = {"C:/SofFex_UT_Old_1280x720.y", "C:/SofFex_UT_Old_320x240.y"};
    const char *DumpFileName = "C:/SofFex_UT_Dump.bin";
    UINT32 Rval;
    flexidag_memblk_t HdlrMemblk = {NULL, 0, 0, 0, 0, 0};

    if ( Init == 0U ) {
        Rval = RefCV_MemblkAlloc(SofFexUT_CacheAligned(sizeof(SOF_FEX_FD_HANDLE_s)), &HdlrMemblk);
        AmbaMisra_TypeCast32(&pSofHdlr, &HdlrMemblk.pBuffer);

        // Initialize Sof handler, pSofHdlr must be allocated in CV section
        (void) AmbaSofFex_InitSofHdlr(pSofHdlr);

        // 1st layer
        // Initialize & Register handler pair
        VPInstIdx = 0U;
        InLayerNum = 1U;
        RefCV_AmbaSofFex_UT_Init("C:/HarrisCorner_640_480_ag/flexibin/flexibin0.bin", &gHarrisHandler[VPInstIdx]);
        RefCV_AmbaSofFex_UT_Init("C:/LKOpcFlow_640_480_ag/flexibin/flexibin0.bin", &gLKOpcFlowHandler[VPInstIdx]);
        gProcImageWidth[VPInstIdx] = 1280U;
        gProcImagePitch[VPInstIdx] = 1280U;
        gProcImageHeight[VPInstIdx] = 720U;
        gProcRoiStartX[VPInstIdx] = 320U;
        gProcRoiStartY[VPInstIdx] = 120U;
        gProcRoiWidth[VPInstIdx] = 640U;
        gProcRoiHeight[VPInstIdx] = 480U;
        Rval = AmbaSofFex_RegHarrisLKHdlr(&gHarrisHandler[VPInstIdx], &gLKOpcFlowHandler[VPInstIdx],
                                          gProcRoiWidth[VPInstIdx], gProcRoiHeight[VPInstIdx], gProcImagePitch[VPInstIdx], pSofHdlr);
        AmbaPrint_PrintUInt5("VP instance(%d), AmbaSofFex_RegHarrisLKHdlr : Rval = %d", VPInstIdx, Rval, 0U, 0U, 0U);

        // 2ed layer
        // Initialize & Register handler pair
        VPInstIdx++;
        InLayerNum++;
        RefCV_AmbaSofFex_UT_Init("C:/HarrisCorner_320_240_ag/flexibin/flexibin0.bin", &gHarrisHandler[VPInstIdx]);
        RefCV_AmbaSofFex_UT_Init("C:/LKOpcFlow_320_240_ag/flexibin/flexibin0.bin", &gLKOpcFlowHandler[VPInstIdx]);
        gProcImageWidth[VPInstIdx] = 320U;
        gProcImagePitch[VPInstIdx] = 320U;
        gProcImageHeight[VPInstIdx] = 240U;
        gProcRoiStartX[VPInstIdx] = 0U;
        gProcRoiStartY[VPInstIdx] = 0U;
        gProcRoiWidth[VPInstIdx] = 320U;
        gProcRoiHeight[VPInstIdx] = 240U;
        Rval = AmbaSofFex_RegHarrisLKHdlr(&gHarrisHandler[VPInstIdx], &gLKOpcFlowHandler[VPInstIdx],
                                          gProcRoiWidth[VPInstIdx], gProcRoiHeight[VPInstIdx], gProcImagePitch[VPInstIdx], pSofHdlr);
        AmbaPrint_PrintUInt5("VP instance(%d), AmbaSofFex_RegHarrisLKHdlr : Rval = %d", VPInstIdx, Rval, 0U, 0U, 0U);

        // Initialize & register other layers here, number of total layers are restricted to MAX_VP_SOF_FEX_INSTANCE

        Init = 1U;
    }

    RefCV_AmbaSofFex_UT_Run(InLayerNum, InPathL_Name, InPathR_Name, DumpFileName);

}
