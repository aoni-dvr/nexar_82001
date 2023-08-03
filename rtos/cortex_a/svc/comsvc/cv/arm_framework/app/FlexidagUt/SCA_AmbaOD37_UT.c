/**
 *  @file RefCV.c
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

#include "AmbaIntrinsics.h"
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
#include "cvapi_amba_odv37_interface_private.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "idsp_vis_msg.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_ambaod.h"


#define RAW_YUV_INPUT
#ifdef RAW_YUV_INPUT
#define FULLFRAME_W                 (1024U)
#define FULLFRAME_H                 (512U)
#define MAX_FILE_Y_SIZE             (0x0200000)
#define MAX_FILE_UV_SIZE            (0x0200000)
#define MAX_FILE_SIZE               (MAX_FILE_Y_SIZE + MAX_FILE_UV_SIZE)
#else
#define FULLFRAME_W                 (3840U)
#define FULLFRAME_H                 (2160U)
#define MAX_FILE_SIZE               (3872*2166*3)
#endif

static SVC_CV_ALGO_HANDLE_s Handler;
static SVC_CV_ALGO_HANDLE_s* Hdlr = &Handler;
static char RsltOutputFn[64] = "out.bin";

//#define DBG {AmbaPrint_PrintInt5("UT@%d", __LINE__, 0, 0, 0, 0);AmbaKAL_TaskSleep(50);}
//extern UINT32 RefCV_UT_GetCVBuf(UINT8** ppU8, UINT32 Size, UINT32* AlignedSize);

static void DumpODResult(const UINT8 *pU8, const char* InPath)
{

    static const char* const kitti_12c_name[] = {
        "Background",
        "Car",
        "Truck",
        "Bus",
        "SUV",
        "Train",
        "Pedestrian",
        "Cyclist",
        "GreenLight",
        "YellowLight",
        "RedLight",
        "TrafficSign",
        "OtherLight"
    };
    UINT32 i;
    UINT32 BBXAddr;
    const amba_od_candidate_t* Obj;
    const amba_od_out_t *BBX;
    char OutputStr[128];
    char TempStr[128];
    char OutFN[64];
    UINT32 Arg[6];
    UINT32 Num;
    AMBA_FS_FILE *FP = NULL;
    AmbaMisra_TypeCast(&BBX, &pU8);

    AmbaUtility_StringCopy(OutFN, 64U, InPath);
    AmbaUtility_StringAppend(OutFN, 64U, ".rst.txt");
    (void) AmbaFS_FileOpen(OutFN, "wb", &FP);
    AmbaPrint_PrintUInt5("Bbx PTS = %d FN: %d NumObj %d", BBX->capture_time, BBX->frame_num
                         , BBX->num_objects, 0U, 0U);

    AmbaMisra_TypeCast(&BBXAddr, &BBX);
    BBXAddr = BBXAddr + BBX->objects_offset;
    AmbaMisra_TypeCast(&Obj, &BBXAddr);
    for (i = 0U; i < BBX->num_objects; i++) {
        //AmbaPrint_PrintUInt5("Bbx id = %u @ %d %d %d %d", Obj->class, Obj->bb_start_col
        //        , Obj->bb_start_row, Obj->bb_width_m1, Obj->bb_height_m1);
        AmbaUtility_StringCopy(OutputStr, 128U, kitti_12c_name[Obj->clsId]);
        Arg[0] = (UINT32)Obj->bb_start_col;
        Arg[1] = (UINT32)Obj->bb_start_row;
        Arg[2] = (UINT32)Obj->bb_width_m1 + 1U;
        Arg[3] = (UINT32)Obj->bb_height_m1 + 1U;
        (void)AmbaUtility_StringPrintUInt32(TempStr, 128U,  " 0 0 0 %d %d %d %d 0 0 0 0 0 0 0\n", 4U, Arg);
        AmbaUtility_StringAppend(OutputStr, 128U, TempStr);
        AmbaPrint_PrintStr5("%s", OutputStr, NULL, NULL, NULL, NULL);
        if (FP!=NULL) {
            (void)AmbaFS_FileWrite(OutputStr, AmbaUtility_StringLength(OutputStr),
                                   1U, FP, &Num);

        }
        Obj = &Obj[1];
    }
    if (FP!=NULL) {
        (void)AmbaFS_FileClose(FP);
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
    ULONG ULBuf;
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
    if (Ret == 0U) {
        AmbaMisra_TypeCast(&ULBuf, &buf);
        Ret = AmbaCache_DataClean(ULBuf, Fsize);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("ReadInputFile AmbaCache_DataClean fail ", 0U, 0U, 0U, 0U, 0U);
        }
    }
}

static void Config_SendLicense(SVC_CV_ALGO_HANDLE_s* pHdlr, const char *LicenseFn)
{
    void* vp;
    UINT32 msg[258];
    const UINT32* pU32;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    msg[0] = AMBANET_MSG_TYPE_LICENSE; // it's a user defined message code which should be the same as used in visorc code
    pU32 = &msg[1];
    AmbaMisra_TypeCast(&vp, &pU32);
    ReadInputFile(LicenseFn, vp);

    pU32 = &msg[0];
    MsgCfg.CtrlType = 0;
    MsgCfg.pExtCtrlCfg = NULL;
    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &pU32);
    (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
}


static void Config_AmbaOD(SVC_CV_ALGO_HANDLE_s* pHdlr, const char* LicenseFn)
{
    AMBA_FS_FILE *Fp;
    const char *CfgFile = "ambaod37_cfg.bin";
    uint32_t BytesRead, FileSize;
    uint32_t Ret;
    uint8_t Buf[sizeof(amba_od_nms_configs_t) + \
            sizeof(amba_od_grouping_t) + \
            (sizeof(ambanet_roi_config_t) * 2U)];
    uint8_t *pBuf = Buf;
    const amba_od_nms_configs_t *Nms;
    const amba_od_grouping_t *Group;
    const ambanet_roi_config_t *RoiCfg[2];
    amba_roi_config_t RoiMsg;
    amba_roi_config_t *pRoiMsg = &RoiMsg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    MsgCfg.pExtCtrlCfg = NULL;
    MsgCfg.CtrlType = 0;

    AmbaPrint_PrintUInt5("Start defaul config!!", 0U, 0U, 0U, 0U, 0U);

    Ret = AmbaFS_FileOpen(CfgFile, "rb", &Fp);

    {
        /* To fix Misra-C not initialized variable issue. */
        pRoiMsg->msg_type            = AMBA_ROI_CONFIG_MSG;
        pRoiMsg->image_pyramid_index = 0;
        pRoiMsg->source_vin          = 0;
        pRoiMsg->roi_start_col       = 0;
        pRoiMsg->roi_start_row       = 0;
        pRoiMsg->roi_width           = 0;
        pRoiMsg->roi_height          = 0;
    }

    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("can't read config file!!", 0U, 0U, 0U, 0U, 0U);
    } else {
        (void) AmbaCV_UtilityFileSize(CfgFile, &FileSize);

        Ret = AmbaFS_FileRead(pBuf, 1U, (UINT32)FileSize, Fp, &BytesRead);
        if ((BytesRead == 0U) || (Ret !=  0U)) {
            AmbaPrint_PrintUInt5("Error while reading file!! (0x%x)",
                                Ret, 0U, 0U, 0U, 0U);
        }

        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("Error while closing file!! (0x%x)",
                                Ret, 0U, 0U, 0U, 0U);
        }

        AmbaMisra_TypeCast32(&Nms, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t)];
        AmbaMisra_TypeCast32(&Group, &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                        sizeof(amba_od_grouping_t)];
        AmbaMisra_TypeCast32(&RoiCfg[0], &pBuf);

        pBuf     = &Buf[sizeof(amba_od_nms_configs_t) + \
                        sizeof(amba_od_grouping_t) + \
                        sizeof(ambanet_roi_config_t)];
        AmbaMisra_TypeCast32(&RoiCfg[1], &pBuf);

        pRoiMsg->msg_type                = AMBA_ROI_CONFIG_MSG;
        pRoiMsg->image_pyramid_index     = RoiCfg[0]->pyramid_index;
        pRoiMsg->source_vin              = 0;
        pRoiMsg->roi_start_col           = RoiCfg[0]->roi_start_col;
        pRoiMsg->roi_start_row           = RoiCfg[0]->roi_start_row;
        pRoiMsg->roi_width               = RoiCfg[0]->roi_width;
        pRoiMsg->roi_height              = RoiCfg[0]->roi_height;

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &pRoiMsg);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &Nms);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &Group);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);

        AmbaMisra_TypeCast32(&MsgCfg.pCtrlParam, &RoiCfg[0]);
        (void)SvcCvAlgo_Control(pHdlr, &MsgCfg);
    }

    Config_SendLicense(pHdlr, LicenseFn);
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

    UINT8 *pU8;
    UINT32 BinSize;
    UINT32 AlignedBinSize;
    UINT32 ret = 0U;
    ULONG addr, Paddr = 0;

    GetFileSize(path, &BinSize);

    if (BinSize != 0U) {

        ret = RefCV_UT_GetCVBuf(&pU8, BinSize, &AlignedBinSize);
        if (ret != 0U) {
            AmbaPrint_PrintUInt5("LoadFlexiDagBin: OOM", 0U, 0U, 0U, 0U, 0U);
        }

        if (ret == 0U) {
            //assign memory
            MemBlk->buffer_cacheable = 1;
            AmbaMisra_TypeCast(&MemBlk->pBuffer, &pU8);
            MemBlk->buffer_size = AlignedBinSize;
            AmbaMisra_TypeCast(&addr, &pU8);
            (void) AmbaMMU_VirtToPhys(addr, &Paddr);
            MemBlk->buffer_daddr = Paddr;
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

static UINT32 SCA_AmbaOD37_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    if (Event == CALLBACK_EVENT_OUTPUT) {
        DumpODResult((UINT8*)pEventData->pOutput->buf[0].pBuffer, RsltOutputFn);
    }
    return 0U;
}

static void SCA_AmbaOD37_UT_Init(const char *FlexiBinPath, const char* LicenseFn)
{
    static UINT32 Init = 0U;
    static flexidag_memblk_t AlgoBuf;
    static flexidag_memblk_t FDBinBuf;

    UINT32 Ret;
    UINT32 AlgoBufSz = 0U;
    SVC_CV_ALGO_QUERY_CFG_s QCfg;
    SVC_CV_ALGO_CREATE_CFG_s CCfg;
    if (Init == 0U) {

        Ret = AmbaWrap_memset(Hdlr, 0, sizeof(SVC_CV_ALGO_HANDLE_s));
        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
        }
#ifdef CONFIG_CV_FLEXIDAG_AMBAODFC
        QCfg.pAlgoObj = &AmbaODAlgoObj;
#else
        QCfg.pAlgoObj = &AmbaOD37AlgoObj;
#endif
        QCfg.pExtQueryCfg = NULL;
        Ret = SvcCvAlgo_Query(Hdlr, &QCfg);
        if (Ret == 0U) {
            AlgoBufSz = QCfg.TotalReqBufSz;
            Ret = RefCV_MemblkAlloc(AlgoBufSz, &AlgoBuf);
            AmbaPrint_PrintUInt5("Alloc Algo Buf Sz:%d", AlgoBuf.buffer_size, 0U, 0U, 0U, 0U);
        }
        if (Ret == 0U) {
            UINT32 i;
            CCfg.NumFD = 1;
            CCfg.pAlgoBuf = &AlgoBuf;
            CCfg.pBin[0] = &FDBinBuf;
            (void)LoadFlexiDagBin(FlexiBinPath, CCfg.pBin[0]);
            Ret = SvcCvAlgo_Create(Hdlr, &CCfg);
            OutputNum = CCfg.OutputNum;
            for (i = 0U; i < OutputNum; i++) {
                OutputSz[i] = CCfg.OutputSz[i];
            }
        }
        if (Ret == 0U) {
            SVC_CV_ALGO_REGCB_CFG_s CBCfg;
            CBCfg.Mode = 0U;
            CBCfg.Callback = SCA_AmbaOD37_UT_CB;
            (void)SvcCvAlgo_RegCallback(Hdlr, &CBCfg);

            Config_AmbaOD(Hdlr, LicenseFn);
        }
        Init = 1U;
    }
}

static void SCA_AmbaOD37_UT_Run(const char* InPath, const char* LogPath, const char* OutputFn, UINT32 Opt)
{
    static AMBA_CV_FLEXIDAG_IO_s InBuf;
    static AMBA_CV_FLEXIDAG_IO_s OutBuf;
    static AMBA_CV_FLEXIDAG_IO_s TmpBuf;
    static UINT32 BufferAlloc = 0U;
    UINT32 i;
    UINT32 YuvWidth = FULLFRAME_W;
    UINT32 YuvHeight = FULLFRAME_H;
    static memio_source_recv_picinfo_t *MemIOPicInfo = NULL;
    cv_pic_info_t *PicInfo = NULL;
    static const UINT8* pChar;
    void* vp;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;

    (void)LogPath;
    (void)Opt;

    if (BufferAlloc == 0U) {
        //alloc input buffer
        (void)RefCV_MemblkAlloc(sizeof(memio_source_recv_picinfo_t), &InBuf.buf[0]);
        InBuf.num_of_buf = 1;
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
    } else {
        PicInfo = &(MemIOPicInfo->pic_info);
    }

    AmbaMisra_TypeCast(&vp, &pChar);

    ReadInputFile(InPath, vp);

    PicInfo->capture_time = 0U;
    PicInfo->channel_id = 0U;
    PicInfo->frame_num = 0U;
    PicInfo->pyramid.image_width_m1 = (UINT16)(YuvWidth - 1U);
    PicInfo->pyramid.image_height_m1 = (UINT16)(YuvHeight - 1U);
    PicInfo->pyramid.image_pitch_m1 = (YuvWidth - 1U);

    for (i=0U ; i<5U ; i++) {
        PicInfo->pyramid.half_octave[i].ctrl.roi_pitch = (UINT16)YuvWidth;
        PicInfo->pyramid.half_octave[i].roi_start_col = (INT16)0;
        PicInfo->pyramid.half_octave[i].roi_start_row = (INT16)0;
        PicInfo->pyramid.half_octave[i].roi_width_m1 = (UINT16)(YuvWidth - 1U);
        PicInfo->pyramid.half_octave[i].roi_height_m1 = (UINT16)(YuvHeight - 1U);
        PicInfo->pyramid.half_octave[i].ctrl.disable = 0U;

        PicInfo->rpLumaLeft[i] = TmpBuf.buf[0].buffer_daddr - InBuf.buf[0].buffer_daddr;
        PicInfo->rpChromaLeft[i] = (TmpBuf.buf[0].buffer_daddr + (YuvWidth * YuvHeight)) - InBuf.buf[0].buffer_daddr;
        PicInfo->rpLumaRight[i] = PicInfo->rpLumaLeft[i];
        PicInfo->rpChromaRight[i] = PicInfo->rpChromaLeft[i];
    }

    (void) AmbaCV_UtilityCmaMemClean(&InBuf.buf[0]);

    FeedCfg.pIn = &InBuf;
    FeedCfg.pOut = &OutBuf;

    (void)SvcCvAlgo_Feed(Hdlr, &FeedCfg);

    (void)AmbaUtility_StringCopy(RsltOutputFn, 64, OutputFn);
}

void SCA_AmbaOD37_UT(const char* BinPath, const char* LogPath, const char* InFile, const char* OutputFn, const char* LicenseFn, UINT32 Opt)
{
    const char* FN;
    SCA_AmbaOD37_UT_Init(BinPath, LicenseFn);
    if (Opt == 1U) {
        FN = GetNextFile(InFile);
        while (FN != NULL) {
            SCA_AmbaOD37_UT_Run(FN, LogPath, OutputFn, Opt);
            FN = GetNextFile(InFile);
        }
    } else {
        SCA_AmbaOD37_UT_Run(InFile, LogPath, OutputFn, Opt);
    }
}

