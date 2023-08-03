/**
 *  @file SCA_OpenSeg_UT.c
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
 *  @details SVC COMSVC CV
 *
 */

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
#include "cvapi_amba_mask_interface.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "idsp_vis_msg.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_openseg.h"


#define RAW_YUV_INPUT
#ifdef RAW_YUV_INPUT
#define MAX_FILE_Y_SIZE             (0x0200000)
#define MAX_FILE_UV_SIZE            (0x0200000)
#define MAX_FILE_SIZE               (MAX_FILE_Y_SIZE + MAX_FILE_UV_SIZE)
#else
#define MAX_FILE_SIZE               (3872*2166*3)
#endif

static SVC_CV_ALGO_HANDLE_s Handler;
static SVC_CV_ALGO_HANDLE_s* Hdlr = &Handler;
static char RsltOutputFn[64] = "out.bin";

static void DumpSegResult(const UINT8 *out, const char* OutputFn)
{
    UINT32 Ret;
    const amba_mask_out_t *pMask;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;
    UINT32 Fsize;
    void *vp;

    AmbaMisra_TypeCast(&pMask, &out);
    Fsize = ((UINT32) pMask->mask_width) * ((UINT32) pMask->mask_height);

    Ret = AmbaFS_FileOpen(OutputFn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("DumpSegResult AmbaFS_FileOpen fail ",
                             0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(&vp, &pMask->buff_addr);
        Ret = AmbaFS_FileWrite(vp, 1U, Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("DumpSegResult AmbaFS_FileWrite fail ",
                                 0U, 0U, 0U, 0U, 0U);
        }
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("DumpSegResult AmbaFS_FileClose fail ",
                                 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static char* GetNextFile(const char* InPath)
{
    static char FN[128];
    static AMBA_FS_FILE *FP = NULL;
    static UINT32 FL_EOF = 0U;
    UINT32 Rval = 0U;
    UINT32 Cnt = 0U;
    UINT32 RbNum;
    char* Rp;
    char Ch[1];
    UINT32 FileNameEnd;
    if (FL_EOF == 0U) {
        if (FP == NULL) {
            Rval = AmbaFS_FileOpen(InPath, "rb", &FP);
            if (Rval != 0U) {
                AmbaPrint_PrintUInt5("GetNextFile open failed", 0U, 0U, 0U, 0U, 0U);
            }
        }
        FileNameEnd = 0U;
        while(FileNameEnd == 0U) {
            if (Rval == 0U) {
                Rval = AmbaFS_FileRead(Ch, 1U, 1U, FP, &RbNum);
                if ((Rval != 0U) || (RbNum == 0U)) {
                    AmbaPrint_PrintUInt5("End of file list", 0U, 0U, 0U, 0U, 0U);
                    (void) AmbaFS_FileClose(FP);
                    FL_EOF = 1U;
                    FP = NULL;
                    FileNameEnd = 1U;
                } else {
                    if ((Ch[0] == '\n') ||
                        (Ch[0] == '\r') ||
                        (Ch[0] == '\0')) {
                        // new line
                        if (Cnt == 0U) {
                            continue;
                        } else {
                            FileNameEnd = 1U;
                        }
                    } else {
                        // normal char
                        FN[Cnt] = Ch[0];
                        Cnt++;
                    }
                }
            }
        }
        Cnt++;
        FN[Cnt] = '\0';
        Rp = FN;
    } else {
        Rp = NULL;
    }
    if (Cnt == 1U) {
        Rp = NULL;
    }
    return Rp;
}

static void ReadInputFile(const char* path, void* buf)
{
    UINT32 Ret;
    UINT32 Fsize = 0U, OpRes;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;

    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("ReadInputFile AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("ReadInputFile AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("ReadInputFile AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("ReadInputFile AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("ReadInputFile AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void Config_OpenSeg(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    amba_mask_config_t config;
    amba_mask_config_t *cfg = &config;
    amba_roi_config_t roi_msg;
    amba_roi_config_t *p_roi_msg = &roi_msg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    cfg->msg_type               = AMBA_MASK_CONFIG_MSG_TYPE;
    cfg->image_pyramid_index    = 1U;
    cfg->roi_start_col          = 42U;
    cfg->roi_start_row          = 180U;
    cfg->roi_width              = 1280U;
    cfg->roi_height             = 512U;
    cfg->buff_width             = 320U;
    cfg->buff_height            = 128U;
    cfg->mask_width             = 320U;
    cfg->mask_height            = 128U;
    cfg->mask_start_col         = 0U;
    cfg->mask_start_row         = 0U;
    cfg->num_categories         = 4U;

    p_roi_msg->msg_type                = AMBA_ROI_CONFIG_MSG;
    p_roi_msg->image_pyramid_index     = cfg->image_pyramid_index;
    p_roi_msg->source_vin              = 0;
    p_roi_msg->roi_start_col           = cfg->roi_start_col;
    p_roi_msg->roi_start_row           = cfg->roi_start_row;
    p_roi_msg->roi_width               = cfg->roi_width;
    p_roi_msg->roi_height              = cfg->roi_height;

    MsgCfg.CtrlType = 0;
    MsgCfg.pExtCtrlCfg = NULL;

    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &p_roi_msg);
    (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
}

static void GetFileSize(const char* path, UINT32* Size)
{
    UINT32 Ret;
    UINT32 Fsize = 0U;
    UINT64 Fpos = 0U;
    AMBA_FS_FILE *Fp;
    // prepare input data
    Ret = AmbaFS_FileOpen(path, "rb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("GetFileSize AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("GetFileSize AmbaFS_FileSeek fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("GetFileSize AmbaFS_FileTell fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        }
    }

    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("GetFileSize AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
    *Size = Fsize;
}

static UINT32 LoadFlexiDagBin(const char* path, flexidag_memblk_t* MemBlk)
{
    UINT32 BinSize;
    UINT32 ret = 0U;

    GetFileSize(path, &BinSize);

    if (BinSize != 0U) {
        ret = RefCV_MemblkAlloc(BinSize, MemBlk);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            ret = AmbaCV_UtilityFileLoad(path, MemBlk);
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("LoadFlexiDagBin: AmbaCV_UtilityFileLoad failed:%x", ret, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("LoadFlexiDagBin: Open flexibin failed", 0U, 0U, 0U, 0U, 0U);
        ret = 1U;
    }
    return ret;
}

static UINT32 OutputNum;
static UINT32 OutputSz[8];

static UINT32 SCA_OpenSeg_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    const UINT8 *pOut;
    if (Event == CALLBACK_EVENT_OUTPUT) {
        //buf 0 raw
        //buf 1 descriptor
        //AmbaMisra_TypeCast(&pOut, &pEventData->pOutput->buf[1].pBuffer);
        AmbaMisra_TypeCast(&pOut, &(pEventData->pExtOutput));
        DumpSegResult(pOut, RsltOutputFn);
    }
    return 0U;
}

static void SCA_OpenSeg_UT_Init(const char *FlexiBinPath)
{
    static UINT32 Init = 0U;
    static flexidag_memblk_t AlgoBuf;
    static flexidag_memblk_t FDBinBuf;

    UINT32 Ret, AlgoBufSz = 0U;
    SVC_CV_ALGO_QUERY_CFG_s QCfg;
    SVC_CV_ALGO_CREATE_CFG_s CCfg;
    if (Init == 0U) {
        Ret = AmbaWrap_memset(Hdlr, 0, sizeof(SVC_CV_ALGO_HANDLE_s));
        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
        }
        QCfg.pAlgoObj = &OpenSegAlgoObj;
        QCfg.pExtQueryCfg = NULL;
        Ret = SvcCvAlgo_Query(Hdlr, &QCfg);
        if (Ret == 0U) {
            AlgoBufSz = QCfg.TotalReqBufSz;
            Ret = RefCV_MemblkAlloc(AlgoBufSz, &AlgoBuf);
            AmbaPrint_PrintUInt5("Alloc Algo Buf Sz:%d", AlgoBuf.buffer_size, 0U, 0U, 0U, 0U);
        }
        if (Ret == 0U) {
            UINT32 idx;
            CCfg.NumFD = 1;
            CCfg.pAlgoBuf = &AlgoBuf;
            CCfg.pBin[0] = &FDBinBuf;
            (void)LoadFlexiDagBin(FlexiBinPath, CCfg.pBin[0]);
            CCfg.pExtCreateCfg = NULL;
            Ret = SvcCvAlgo_Create(Hdlr, &CCfg);
            OutputNum = CCfg.OutputNum;
            for (idx = 0U; idx < OutputNum; idx++) {
                OutputSz[idx] = CCfg.OutputSz[idx];
            }
        }
        if (Ret == 0U) {
            SVC_CV_ALGO_REGCB_CFG_s CBCfg;
            CBCfg.Mode = 0U;
            CBCfg.Callback = SCA_OpenSeg_UT_CB;
            (void)SvcCvAlgo_RegCallback(Hdlr, &CBCfg);
            Config_OpenSeg(Hdlr);
        }
        Init = 1U;
    }
}

static void SCA_OpenSeg_UT_Run(const char* InPath, const char* OutputFn)
{
    static AMBA_CV_FLEXIDAG_IO_s InBuf;
    static AMBA_CV_FLEXIDAG_IO_s OutBuf;
    static AMBA_CV_FLEXIDAG_IO_s TmpBuf;
    static UINT32 BufferAlloc = 0U;
    static const UINT8* pChar;
    static memio_source_recv_picinfo_t *MemIOPicInfo = NULL;
    UINT32 YuvWidth = 1360U;
    UINT32 YuvHeight = 768U;
    UINT32 YuvPitch;
    UINT32 i;
    cv_pic_info_t *PicInfo = NULL;
    void *vp;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;

    YuvPitch = (((YuvWidth + 31U) >> 5U) << 5U); //32-alignment

    if (BufferAlloc == 0U) {
        //alloc input buffer
        (void)RefCV_MemblkAlloc(sizeof(memio_source_recv_picinfo_t), &InBuf.buf[0]);
        InBuf.num_of_buf = 1U;
        AmbaMisra_TypeCast(&MemIOPicInfo, &InBuf.buf[0].pBuffer);
        PicInfo = &(MemIOPicInfo->pic_info);

        // alloc frame buffer
        (void)RefCV_MemblkAlloc(MAX_FILE_SIZE, &TmpBuf.buf[0]);
        AmbaMisra_TypeCast(&pChar, &TmpBuf.buf[0].pBuffer);

        // alloc output buffer
        for (i = 0U; i < OutputNum; i++) {
            (void)RefCV_MemblkAlloc(OutputSz[i], &OutBuf.buf[i]);
        }
        OutBuf.num_of_buf = OutputNum;
        BufferAlloc = 1U;
    }
    if (PicInfo == NULL) {
        PicInfo = &(MemIOPicInfo->pic_info);
    }

    AmbaMisra_TypeCast(&vp, &pChar);

    ReadInputFile(InPath, vp);

    PicInfo->capture_time = 0U;
    PicInfo->channel_id = 0U;
    PicInfo->frame_num = 0U;
    PicInfo->pyramid.image_width_m1     = (UINT16) (YuvWidth - 1U);
    PicInfo->pyramid.image_height_m1    = (UINT16) (YuvHeight - 1U);
    PicInfo->pyramid.image_pitch_m1     = (YuvPitch - 1U);

    for (i = 0U; i < 5U; i++) {
        PicInfo->pyramid.half_octave[i].ctrl.roi_pitch  = (UINT16) YuvPitch;
        PicInfo->pyramid.half_octave[i].roi_start_col   = (INT16) 0;
        PicInfo->pyramid.half_octave[i].roi_start_row   = (INT16) 0;
        PicInfo->pyramid.half_octave[i].roi_width_m1    = (UINT16) (YuvWidth - 1U);
        PicInfo->pyramid.half_octave[i].roi_height_m1   = (UINT16) (YuvHeight - 1U);
        PicInfo->pyramid.half_octave[i].ctrl.disable    = 0U;

        PicInfo->rpLumaLeft[i]      = TmpBuf.buf[0].buffer_daddr - InBuf.buf[0].buffer_daddr;
        PicInfo->rpChromaLeft[i]    = (TmpBuf.buf[0].buffer_daddr + (YuvPitch * YuvHeight)) - InBuf.buf[0].buffer_daddr;
        PicInfo->rpLumaRight[i]     = PicInfo->rpLumaLeft[i];
        PicInfo->rpChromaRight[i]   = PicInfo->rpChromaLeft[i];
    }

    (void) AmbaCV_UtilityCmaMemClean(&TmpBuf.buf[0]);
    (void) AmbaCV_UtilityCmaMemClean(&InBuf.buf[0]);

    FeedCfg.pIn = &InBuf;
    FeedCfg.pOut = &OutBuf;

    (void) SvcCvAlgo_Feed(Hdlr, &FeedCfg);

    AmbaUtility_StringCopy(RsltOutputFn, 64, OutputFn);
}

void SCA_OpenSeg_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, UINT32 Opt)
{
    const char* FN;
    SCA_OpenSeg_UT_Init(BinPath);
    if (Opt == 1U) {
        FN = GetNextFile(InFile);
        while (FN != NULL) {
            SCA_OpenSeg_UT_Run(FN, OutputFn);
            FN = GetNextFile(InFile);
        }
    } else {
        SCA_OpenSeg_UT_Run(InFile, OutputFn);
    }
    (void) LogPath;
}

