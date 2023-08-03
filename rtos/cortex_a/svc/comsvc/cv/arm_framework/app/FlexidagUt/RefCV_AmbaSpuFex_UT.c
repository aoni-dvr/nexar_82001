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
#include "cvapi_flexidag_ambaspufex_cv2.h"
#include "cvapi_protection.h"

static const AMBA_STEREO_FD_HANDLE_s* Hdlr = NULL;

static inline UINT32 SpuFexUT_CacheAligned (UINT32 Val)
{
    UINT32 flexidag_mem_align_size = 0;

    if (AmbaCV_SchdrGetHwInfo(FLEXIDAG_MEM_ALIGNMENT, &flexidag_mem_align_size, sizeof(flexidag_mem_align_size)) != ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("SpuFexUT_CacheAligned : AmbaCV_SchdrGetHwInfo fail ", 0U, 0U, 0U, 0U, 0U);
    }

    return (((Val) + (flexidag_mem_align_size - 1U)) & ~(flexidag_mem_align_size - 1U));
}

static const AMBA_STEREO_FD_HANDLE_s *CastStereoFd2Const(AMBA_STEREO_FD_HANDLE_s *pHdlr)
{
    const AMBA_STEREO_FD_HANDLE_s *pRval;
    AmbaMisra_TouchUnused(pHdlr);
    if(AmbaWrap_memcpy(&pRval, &pHdlr, sizeof(pRval)) != 0U) {
        AmbaPrint_PrintUInt5("Err CastStereoFd2Const() AmbaWrap_memcpy() fail ", 0U, 0U, 0U, 0U, 0U);
    }
    return pRval;
}

static void SpuFexUT_ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    UINT32 U32Buf;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        AmbaMisra_TypeCast32(&U32Buf, &buf);
        Ret = AmbaCache_DataClean(U32Buf, Fsize);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void SpuFexUT_GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        //AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}


static UINT32 SpuFexUT_LoadFlexiDagBin(const char* path, UINT8**BinBuf, UINT32* Size)
{

    flexidag_memblk_t FlexidagBinBuf;
    UINT8 *pU8 = NULL;
    UINT32 BinSize;
    UINT32 AlignedBinSize;
    UINT32 ret = 0U;
    UINT32 addr, Paddr;

    SpuFexUT_GetFileSize(path, &BinSize);

    if (BinSize != 0U) {

        ret = RefCV_UT_GetCVBuf(&pU8, BinSize, &AlignedBinSize);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("SpuFexUT_LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            //assign memory
            FlexidagBinBuf.buffer_cacheable = 1;
            AmbaMisra_TypeCast32(&FlexidagBinBuf.pBuffer, &pU8);
            FlexidagBinBuf.buffer_size = AlignedBinSize;
            AmbaMisra_TypeCast32(&addr, &pU8);
            (void) AmbaMMU_Virt32ToPhys32(addr, &Paddr);
            FlexidagBinBuf.buffer_daddr = Paddr;
            ret = AmbaCV_UtilityFileLoad(path, &FlexidagBinBuf);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("SpuFexUT_LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
            *BinBuf = pU8;
            *Size = AlignedBinSize;
        }
    } else {
        AmbaPrint_PrintUInt5("SpuFexUT_LoadFlexiDagBin: Not open flexibin, use internal flexibin", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    }
    return ret;
}

static UINT32 OutputNum;
static UINT32 OutputSz[8];

static void RefCV_AmbaSpuFex_UT_Init(const char *FlexiBinPath)
{

    static UINT32 Init = 0U;
    UINT32 AlignSz = 0U;
    UINT32 Ret;
    UINT8* BinBuf = NULL;
    UINT32 BinSize = 0U;
    UINT8* StateBuf = NULL;
    UINT32 StateSize = 0U;
    UINT8* TempBuf = NULL;
    UINT32 TempSize = 0U;

    static AMBA_STEREO_FD_HANDLE_s Handler = {0};
    Hdlr = CastStereoFd2Const(&Handler);

    if (Init == 0U) {
        (void)SpuFexUT_LoadFlexiDagBin(FlexiBinPath, &BinBuf, &BinSize);
        Ret = AmbaSpuFex_Open(BinBuf, BinSize, &StateSize, &TempSize, &OutputNum, OutputSz, Hdlr);
        if (Ret == 0U) {
            AmbaPrint_PrintUInt5("OutputNum:%d", OutputNum, 0U, 0U, 0U, 0U);
        }
        if (Ret == 0U) {
            Ret = RefCV_UT_GetCVBuf(&StateBuf, StateSize, &AlignSz);
            AmbaPrint_PrintUInt5("Alloc State Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
        }
        if (Ret == 0U) {
            Ret = RefCV_UT_GetCVBuf(&TempBuf, TempSize, &AlignSz);
            AmbaPrint_PrintUInt5("Alloc Temp Buf Sz:%d", AlignSz, 0U, 0U, 0U, 0U);
        }

        if (Ret == 0U) {
            (void)AmbaSpuFex_Init(Hdlr, StateBuf, TempBuf);
            //SpuFexUT_Config_AmbaSpuFex(Hdlr);
        }
        Init = 1U;
    }
}


static void SpuFexUT_SaveBin(UINT8 *Buf, const char* Fn, const UINT32 Size)
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

static void SpuFexUT_DumpSpuResult(const AMBA_CV_SPU_DATA_s *out, const char* OutputFn)
{
    const AMBA_CV_SPU_DATA_s *pSpu;
    UINT32 Fsize;
    UINT8 *pChar;
    UINT32 U32pSpu;

    AmbaMisra_TypeCast32(&pSpu, &out);
    Fsize = pSpu->Scales[0].BufSize;
    AmbaPrint_PrintUInt5("pSpu->Scales[0] BufSize: %d DisparityMapOffset: %d w: %d h: %d pitch: %d", Fsize, pSpu->Scales[0].DisparityMapOffset, pSpu->Scales[0].DisparityWidth, pSpu->Scales[0].DisparityHeight, pSpu->Scales[0].DisparityPitch);
    AmbaPrint_PrintUInt5("pSpu->Scales[0] bpp: %d", pSpu->Scales[0].DisparityBpp, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("RoiStartCol: %d RoiStartRow: %d RoiAbsoluteStartCol: %d RoiAbsoluteStartRow: %d"
                         ,pSpu->Scales[0].RoiStartCol, pSpu->Scales[0].RoiStartRow, pSpu->Scales[0].RoiAbsoluteStartCol, pSpu->Scales[0].RoiAbsoluteStartRow, 0U);


    AmbaMisra_TypeCast32(&U32pSpu, &pSpu);
    U32pSpu += pSpu->Scales[0].DisparityMapOffset;
    AmbaMisra_TypeCast32(&pChar, &U32pSpu);
    SpuFexUT_SaveBin(pChar, OutputFn, Fsize);

}


static void SpuFexUT_DumpFexResult(const AMBA_CV_FEX_DATA_s *out)
{
    const AMBA_CV_FEX_DATA_s *pFex;
    UINT32 Fsize;
    UINT8 *pChar;
    UINT32 U32pFex;

    AmbaMisra_TypeCast32(&pFex, &out);
    AmbaPrint_PrintUInt5("pFex Primary: %d %d Secondary: %d %d error: %d", pFex->PrimaryList[0].Enable, pFex->PrimaryList[0].Reserved, pFex->SecondaryList[0].Enable, pFex->SecondaryList[0].Reserved,  pFex->Reserved);
    AmbaPrint_PrintUInt5("pFex Primary: 0x%x 0x%x 0x%x", pFex->PrimaryList[0].KeypointsCountOffset, pFex->PrimaryList[0].KeypointsOffset, pFex->PrimaryList[0].DescriptorsOffset, 0U,  0U);
    AmbaPrint_PrintUInt5("pFex Secondary: 0x%x 0x%x 0x%x", pFex->SecondaryList[0].KeypointsCountOffset, pFex->SecondaryList[0].KeypointsOffset, pFex->SecondaryList[0].DescriptorsOffset, 0U,  0U);

    Fsize = CV_FEX_MAX_BUCKETS;
    Fsize += CV_FEX_KEYPOINTS_SIZE*CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS;
    Fsize += CV_FEX_MAX_BUCKETS*CV_FEX_MAX_KEYPOINTS*CV_FEX_DESCRIPTOR_SIZE;

    AmbaMisra_TypeCast32(&U32pFex, &pFex);
    U32pFex += pFex->PrimaryList[0].KeypointsCountOffset;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    SpuFexUT_SaveBin(pChar, "c:\\Fex_Primary_Data.bin", Fsize);

    AmbaMisra_TypeCast32(&U32pFex, &pFex);
    U32pFex += pFex->SecondaryList[0].KeypointsCountOffset;
    AmbaMisra_TypeCast32(&pChar, &U32pFex);
    SpuFexUT_SaveBin(pChar, "c:\\Fex_Secondary_Data.bin", Fsize);

}


static void SpuFexUT_Config_AmbaSpuFex(const AMBA_STEREO_FD_HANDLE_s* pHdlr, const idsp_pyramid_t *pPyramid, UINT32 FusionDisparity, const roi_t *pRoi)
{
    UINT32 i;
    UINT8 RoiFlag = 0U;
    AMBA_CV_SPUFEX_CFG_s Cfg;
    AMBA_CV_SPUFEX_CFG_s *pCfg = &Cfg;
    //set pyramid info
    pCfg->PyramidInfo = *pPyramid;

    if((pRoi->m_start_col == 0U) && (pRoi->m_start_row == 0U) && (pRoi->m_width_m1 == 0U) && (pRoi->m_height_m1 == 0U)) {
        RoiFlag = 0U;
        AmbaPrint_PrintUInt5("Disable ROI", 0U, 0U, 0U, 0U, 0U);
    } else {
        RoiFlag = 1U;
        AmbaPrint_PrintUInt5("Enable ROI", 0U, 0U, 0U, 0U, 0U);
    }

    //set spu default config
    pCfg->SpuScaleCfg[0].Enable = 1U;
    pCfg->SpuScaleCfg[0].RoiEnable = RoiFlag;
    pCfg->SpuScaleCfg[0].Roi = *pRoi;
    pCfg->SpuScaleCfg[0].FusionDisparity = (UINT8)FusionDisparity;


    for(i=1; i<MAX_HALF_OCTAVES; ++i) {
        pCfg->SpuScaleCfg[i].Enable = 0U;
    }

    //set fex default config
    pCfg->FexScaleCfg[0].Enable = (UINT8)CV_FEX_ENABLE_ALL;
    pCfg->FexScaleCfg[0].RoiEnable = RoiFlag;
    pCfg->FexScaleCfg[0].Roi = *pRoi;
    for(i=1; i<MAX_HALF_OCTAVES; ++i) {
        pCfg->FexScaleCfg[i].Enable = CV_FEX_DISABLE;
    }
    (void)AmbaSpuFex_Cfg(pHdlr, pCfg);
    (void)AmbaSpuFex_SetSpuMode(pHdlr, AMBA_CV_SPU_DISPLAY_MODE);
}

static void RefCV_AmbaSpuFex_UT_Run(const char* InPathL, const char* InPathR, const char* OutputFn, UINT32 FusionDisparity,
                                    UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight)
{
    static UINT32 BufferAlloc = 0U;
    static UINT8* InBuf;
    static UINT8* OutputBuf[8];
    AMBA_CV_SPU_DATA_s *pOutSpu;
    AMBA_CV_FEX_DATA_s *pOutFex;
    const AMBA_CV_FEX_DATA_s *pOutFexMisra;
    UINT32 rval;
    UINT32 i;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    UINT32 YuvPitch = 1920U;
    roi_t Roi;
    const memio_source_recv_picinfo_t *MemIOPicInfo = NULL;
    cv_pic_info_t *PicInfo = NULL;
    void* vp;
    UINT32 U32InBuf;
    memio_source_recv_picinfo_t *pInInfo;
    UINT32 AlignedSize;
    UINT32 InputBufSize;

    YuvWidth = Width;
    YuvHeight = Height;
    YuvPitch = SpuFexUT_CacheAligned(Width);

    InputBufSize = SpuFexUT_CacheAligned(sizeof(memio_source_recv_picinfo_t)) + SpuFexUT_CacheAligned(YuvWidth*YuvHeight*2U);
    if (BufferAlloc == 0U) {
        //alloc input buffer
        rval = RefCV_UT_GetCVBuf(&InBuf, InputBufSize, &AlignedSize);

        // alloc output buffer
        if (rval == 0U) {
            for(i=0U; i<OutputNum; ++i) {
                rval = RefCV_UT_GetCVBuf(&OutputBuf[i], OutputSz[i], &AlignedSize);
                AmbaPrint_PrintUInt5("Alloc Output Buf[%d] Sz:%d rval: %d", i, AlignedSize, rval, 0U, 0U);
            }
        }
        BufferAlloc = 1U;
    }

    AmbaMisra_TypeCast32(&U32InBuf, &InBuf);
    AmbaMisra_TypeCast32(&MemIOPicInfo, &InBuf);
    AmbaMisra_TypeCast32(&PicInfo, &InBuf);


    if((RoiStartX == 0U) && (RoiStartY == 0U) && (RoiWidth == 0U) && (RoiHeight == 0U)) {
        Roi.m_start_col = 0U;
        Roi.m_start_row = 0U;
        Roi.m_width_m1 = (UINT16)YuvWidth - 1U;
        Roi.m_height_m1= (UINT16)YuvHeight - 1U;
    } else {
        Roi.m_start_col = (UINT16)RoiStartX;
        Roi.m_start_row = (UINT16)RoiStartY;
        Roi.m_width_m1 = (UINT16)RoiWidth - 1U;
        Roi.m_height_m1 = (UINT16)RoiHeight - 1U;
    }

    //PicInfo->capture_time = 0U;
    //PicInfo->channel_id = 0U;
    PicInfo->frame_num = 0U;
    PicInfo->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
    PicInfo->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
    PicInfo->pyramid.image_pitch_m1 = (YuvWidth - 1U);

    PicInfo->pyramid.half_octave[0].ctrl.disable = 0U;
    PicInfo->pyramid.half_octave[0].ctrl.mode = 1U;
    PicInfo->pyramid.half_octave[0].ctrl.octave_mode = 0U;
    PicInfo->pyramid.half_octave[0].ctrl.roi_pitch = (UINT16)YuvPitch;
    PicInfo->pyramid.half_octave[0].roi_start_row = 0;
    PicInfo->pyramid.half_octave[0].roi_start_col = 0;
    PicInfo->pyramid.half_octave[0].roi_height_m1 = (UINT16)YuvHeight - 1U;
    PicInfo->pyramid.half_octave[0].roi_width_m1 =  (UINT16)YuvWidth - 1U;
    PicInfo->pyramid.half_octave[1].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[2].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[3].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[4].ctrl.disable = 1U;
    PicInfo->pyramid.half_octave[5].ctrl.disable = 1U;

    //read left Y
    U32InBuf += SpuFexUT_CacheAligned(sizeof(memio_source_recv_picinfo_t));
    AmbaMisra_TypeCast32(&vp, &U32InBuf);
    SpuFexUT_ReadInputFile(InPathL, vp);

    //read right Y
    U32InBuf += SpuFexUT_CacheAligned(YuvWidth*YuvHeight);
    AmbaMisra_TypeCast32(&vp, &U32InBuf);
    SpuFexUT_ReadInputFile(InPathR, vp);

    //assign relate ptr
    U32InBuf = SpuFexUT_CacheAligned(sizeof(memio_source_recv_picinfo_t));
    AmbaMisra_TypeCast32(&(PicInfo->rpLumaLeft[0]), &U32InBuf);
    U32InBuf += SpuFexUT_CacheAligned(YuvWidth*YuvHeight);
    AmbaMisra_TypeCast32(&(PicInfo->rpLumaRight[0]), &U32InBuf);

    AmbaMisra_TypeCast32(&U32InBuf, &InBuf);
    (void)AmbaCache_DataClean(U32InBuf, SpuFexUT_CacheAligned(InputBufSize));

    SpuFexUT_Config_AmbaSpuFex(Hdlr, &PicInfo->pyramid, FusionDisparity, &Roi);

    rval = AmbaSpuFex_Process(Hdlr, MemIOPicInfo, OutputBuf[0], OutputBuf[1]);
    AmbaPrint_PrintUInt5("AmbaSpuFex_Process : rval = %d", rval, 0U, 0U, 0U, 0U);
    rval = AmbaSpuFex_GetResult(Hdlr, &pInInfo, &pOutSpu, &pOutFex);
    AmbaPrint_PrintUInt5("AmbaSpuFex_GetResult : rval = %d", rval, OutputNum, 0U, 0U, 0U);
    SpuFexUT_DumpSpuResult(pOutSpu, OutputFn);
    AmbaMisra_TypeCast32(&pOutFexMisra, &pOutFex);
    SpuFexUT_DumpFexResult(pOutFexMisra);
    SpuFexUT_SaveBin(OutputBuf[0], "c:\\Spu.bin", OutputSz[0]);
    SpuFexUT_SaveBin(OutputBuf[1], "c:\\Fex.bin", OutputSz[1]);
    (void)AmbaSpuFex_DumpLog(Hdlr, "c:\\log");

}

void RefCV_AmbaSpuFex_UT(const char* BinPath, const char* InFileL, const char* InFileR, const char* OutputFn, UINT32 FusionDisparity,
                         UINT32 Width, UINT32 Height, UINT32 RoiStartX, UINT32 RoiStartY, UINT32 RoiWidth, UINT32 RoiHeight)
{
    static UINT32 init = 0;

    if (init == 0U) {
        AmbaPrint_PrintUInt5("AmbaCV_ProtectInit()...", 0U, 0U, 0U, 0U, 0U);
        (void) AmbaCV_ProtectInit(0x3U);
        (void) AmbaKAL_TaskSleep(300U);
        init = 1U;
    }

    RefCV_AmbaSpuFex_UT_Init(BinPath);
    RefCV_AmbaSpuFex_UT_Run(InFileL, InFileR, OutputFn, FusionDisparity, Width, Height, RoiStartX, RoiStartY, RoiWidth, RoiHeight);
}
