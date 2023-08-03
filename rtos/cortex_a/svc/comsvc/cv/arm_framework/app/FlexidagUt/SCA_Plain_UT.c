/**
 *  @file SCA_Plain_UT.c
 *
 *  @copyright Copyright (c) 2019 Ambarella International LP
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
 *  @details SVC COMSVC CV
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_amba_mask_interface.h"
#include "RefCV_Common.h"
#include "cvapi_flexidag_unittest.h"
#include "idsp_roi_msg.h"
#include "SvcCvAlgo.h"
#include "cvapi_svccvalgo_plain.h"

// PICINFO_INPUT for memio_source_recv_picinfo_t;
// RAW_TWO_INPUT for YUV raw input; RAW_ONE_INPUT for BGR/RGB/Gray raw input
#define RAW_ONE_INPUT

#ifdef RAW_TWO_INPUT
#define MAX_FILE_Y_SIZE             (0x0200000)
#define MAX_FILE_UV_SIZE            (0x0200000)
#define MAX_FILE_SIZE               (MAX_FILE_Y_SIZE + MAX_FILE_UV_SIZE)
#else
#define MAX_FILE_SIZE               (3872*2166*3)
#endif

// Input file settings
#define INPUT_W 224U
#define INPUT_H 224U
#define INPUT_PITCH 224U
#define INPUT_D 3U

// ROI settings used in Config, only for picinfo
#define ROI_START_COL 42U
#define ROI_START_ROW 180U
#define ROI_WIDTH     1280U
#define ROI_HEIGHT    512U

// The index to the first alphabet of the folder/image file name;
// index starts from 0
// e.g. If input file is c:\net\group_0\AMBA1001\data_pad32.bin,
//      then pick the strlen of "c:\net\group_0\"

#define FILENAME_START_IDX 31U
#define INPUT_LAYER_NAME_LENGTH 4U

// Only when using YUV raw-in, VP_IN_NUM be set to 2
#define VP_IN_NUM 1U

static SVC_CV_ALGO_HANDLE_s Handler;
static SVC_CV_ALGO_HANDLE_s* Hdlr = &Handler;
static char OutLayerName[4][64] = {"prob_perm","","",""};
static char OutDirPath[128];
static UINT32 RUN_NEXT_LOCK = 1U;


static void DumpResult(UINT8 *out, const char* OutputFn, UINT32 Fsize)
{
    UINT32 Ret;
    AMBA_FS_FILE *Fp;
    UINT32 OpRes;

    Ret = AmbaFS_FileOpen(OutputFn, "wb", &Fp);
    if(Ret !=  0U) {
        AmbaPrint_PrintUInt5("DumpResult AmbaFS_FileOpen fail ", 0U, 0U, 0U, 0U, 0U);
    } else {
        Ret = AmbaFS_FileWrite(out, 1U, Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("DumpResult AmbaFS_FileRead fail ", 0U, 0U, 0U, 0U, 0U);
        }
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintUInt5("DumpResult AmbaFS_FileClose fail ", 0U, 0U, 0U, 0U, 0U);
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
        AmbaPrint_PrintStr5("ReadInputFile %s AmbaFS_FileOpen fail ", path, NULL, NULL, NULL, NULL);
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_END);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("ReadInputFile AmbaFS_FileSeek fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileTell(Fp, &Fpos);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("ReadInputFile AmbaFS_FileTell fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Fsize = (UINT32)(Fpos & 0xFFFFFFFFU);
        Ret = AmbaFS_FileSeek(Fp, 0LL, AMBA_FS_SEEK_START);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("ReadInputFile AmbaFS_FileSeek fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileRead(buf, 1U, (UINT32)Fsize, Fp, &OpRes);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("ReadInputFile AmbaFS_FileRead fail ", path, NULL, NULL, NULL, NULL);
        }
    }
    if (Ret == 0U) {
        Ret = AmbaFS_FileClose(Fp);
        if(Ret !=  0U) {
            AmbaPrint_PrintStr5("ReadInputFile AmbaFS_FileClose fail ", path, NULL, NULL, NULL, NULL);
        }
    }
}


static void Config_Plain(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    amba_roi_config_t roi_msg;
    amba_roi_config_t *p_roi_msg = &roi_msg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;

    p_roi_msg->msg_type                = AMBA_ROI_CONFIG_MSG;
    p_roi_msg->image_pyramid_index     = 1U;
    p_roi_msg->source_vin              = 0U;
    p_roi_msg->roi_start_col           = ROI_START_COL;
    p_roi_msg->roi_start_row           = ROI_START_ROW;
    p_roi_msg->roi_width               = ROI_WIDTH;
    p_roi_msg->roi_height              = ROI_HEIGHT;

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

static UINT32 SCA_Plain_UT_CB(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 idx, ret;
    char OutFN[128];
    const char SlashStr[3] = "\\";
    const char PostfixStr[10] = "_bub.out";
    if (Event == CALLBACK_EVENT_OUTPUT) {
        for (idx = 0U; idx < OutputNum; idx++) {
            ret = AmbaWrap_memset(OutFN, 0x0, sizeof(OutFN));
            if (ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
            }
            AmbaUtility_StringCopy(OutFN, 128U, OutDirPath);
            AmbaUtility_StringAppend(OutFN, 128U, SlashStr);
            AmbaUtility_StringAppend(OutFN, 128U, OutLayerName[idx]);
            AmbaUtility_StringAppend(OutFN, 128U, PostfixStr);
            AmbaPrint_PrintStr5("%s", OutFN, NULL, NULL, NULL, NULL);
            DumpResult((UINT8*)pEventData->pOutput->buf[idx].pBuffer, OutFN, OutputSz[idx]);
        }
        RUN_NEXT_LOCK = 0U;
    }
    return 0U;
}

static void SCA_Plain_UT_Init(const char *FlexiBinPath)
{
    static flexidag_memblk_t AlgoBuf;
    static flexidag_memblk_t FDBinBuf;
    static UINT32 Init = 0U;
    UINT32 Ret;
    UINT32 AlgoBufSz = 0U;
    SVC_CV_ALGO_QUERY_CFG_s QCfg;
    SVC_CV_ALGO_CREATE_CFG_s CCfg;
    if (Init == 0U) {

        Ret = AmbaWrap_memset(Hdlr, 0, sizeof(SVC_CV_ALGO_HANDLE_s));
        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
        }
        QCfg.pAlgoObj = &PlainAlgoObj;
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
            Ret = SvcCvAlgo_Create(Hdlr, &CCfg);
            OutputNum = CCfg.OutputNum;
            for (idx = 0U; idx < OutputNum; idx++) {
                OutputSz[idx] = CCfg.OutputSz[idx];
            }
        }
        if (Ret == 0U) {
            SVC_CV_ALGO_REGCB_CFG_s CBCfg;
            CBCfg.Mode = 0U;
            CBCfg.Callback = SCA_Plain_UT_CB;
            (void)SvcCvAlgo_RegCallback(Hdlr, &CBCfg);
            Config_Plain(Hdlr);
        }
        Init = 1U;
    }
}

static void SCA_Plain_UT_Run(const char* InPath, const char* LogPath, UINT32 Opt)
{
    static AMBA_CV_FLEXIDAG_IO_s InBuf;
    static AMBA_CV_FLEXIDAG_IO_s OutBuf;
    static AMBA_CV_FLEXIDAG_IO_s TmpBuf;
    static UINT32 BufferAlloc = 0U;
    static const UINT8* pChar;
#ifdef PICINFO_INPUT
    static memio_source_recv_picinfo_t *MemIOPicInfo = NULL;
    cv_pic_info_t *PicInfo = NULL;
#endif
#if defined (RAW_ONE_INPUT) || defined (RAW_TWO_INPUT)
    static memio_source_recv_raw_t *MemIORaw[2];
#endif
    void *vp;
    UINT32 i;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;

    if (BufferAlloc == 0U) {
        //alloc input buffer
#ifdef PICINFO_INPUT
        (void)RefCV_MemblkAlloc(sizeof(memio_source_recv_picinfo_t), &InBuf.buf[0]);
        InBuf.num_of_buf = 1;
        AmbaMisra_TypeCast(&MemIOPicInfo, &InBuf.buf[0].pBuffer);
        PicInfo = &(MemIOPicInfo->pic_info);
#elif (defined (RAW_ONE_INPUT) || defined (RAW_TWO_INPUT))
        InBuf.num_of_buf = VP_IN_NUM;
        for (i = 0U; i < VP_IN_NUM; i++) {
            (void)RefCV_MemblkAlloc(sizeof(memio_source_recv_raw_t), &InBuf.buf[i]);
            AmbaMisra_TypeCast(&MemIORaw[i], &InBuf.buf[i].pBuffer);
        }
#else
        AmbaPrint_PrintStr5("No INPUT type flag (PICINFO_INPUT/RAW_ONE_INPUT/RAW_TWO_INPUT) is set. Please make sure one of the flag is defined.", NULL, NULL, NULL, NULL, NULL);
#endif

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
#ifdef PICINFO_INPUT
    PicInfo = &(MemIOPicInfo->pic_info);
#endif
    AmbaMisra_TypeCast(&vp, &pChar);

    ReadInputFile(InPath, vp);

#if defined (PICINFO_INPUT)
    PicInfo->capture_time = 0U;
    PicInfo->channel_id = 0U;
    PicInfo->frame_num = 0U;
    PicInfo->pyramid.image_width_m1 = (UINT16)(INPUT_W - 1U);
    PicInfo->pyramid.image_height_m1 = (UINT16)(INPUT_H - 1U);
    PicInfo->pyramid.image_pitch_m1 = (INPUT_PITCH - 1U);

    for (i = 0U; i < 5U; i++) {
        PicInfo->pyramid.half_octave[i].ctrl.roi_pitch = (UINT16)INPUT_PITCH;
        PicInfo->pyramid.half_octave[i].roi_start_col = (INT16)0;
        PicInfo->pyramid.half_octave[i].roi_start_row = (INT16)0;
        PicInfo->pyramid.half_octave[i].roi_width_m1 = (UINT16)(INPUT_W - 1U);
        PicInfo->pyramid.half_octave[i].roi_height_m1 = (UINT16)(INPUT_H - 1U);
        PicInfo->pyramid.half_octave[i].ctrl.disable = 0U;

        PicInfo->rpLumaLeft[i] = TmpBuf.buf[0].buffer_daddr  - InBuf.buf[0].buffer_daddr;
        PicInfo->rpChromaLeft[i] = (TmpBuf.buf[0].buffer_daddr  + (INPUT_PITCH * INPUT_H)) - InBuf.buf[0].buffer_daddr;
        PicInfo->rpLumaRight[i] = PicInfo->rpLumaLeft[i];
        PicInfo->rpChromaRight[i] = PicInfo->rpChromaLeft[i];
    }
#elif defined (RAW_ONE_INPUT)
    MemIORaw[0]->addr = TmpBuf.buf[0].buffer_caddr;
    MemIORaw[0]->size = INPUT_W * INPUT_H * INPUT_D;
    MemIORaw[0]->pitch = INPUT_PITCH;
#elif defined (RAW_TWO_INPUT)
    MemIORaw[0]->addr = TmpBuf.buf[0].buffer_caddr;
    MemIORaw[0]->size = INPUT_W * INPUT_H;
    MemIORaw[0]->pitch = INPUT_PITCH;
    MemIORaw[1]->addr = TmpBuf.buf[0].buffer_caddr + (INPUT_W * INPUT_H);
    MemIORaw[1]->size = INPUT_W * INPUT_H / 2U;
    MemIORaw[1]->pitch = INPUT_PITCH;
#else
    AmbaPrint_PrintStr5("No INPUT type flag (PICINFO_INPUT/RAW_ONE_INPUT/RAW_TWO_INPUT) is set. Please make sure one of the flag is defined.", NULL, NULL, NULL, NULL, NULL);
#endif

    (void) AmbaCV_UtilityCmaMemClean(&TmpBuf.buf[0]);
    (void) AmbaCV_UtilityCmaMemClean(&InBuf.buf[0]);

    FeedCfg.pIn = &InBuf;
    FeedCfg.pOut = &OutBuf;

    (void)SvcCvAlgo_Feed(Hdlr, &FeedCfg);

    (void)LogPath;
    (void)Opt;
}

void SCA_Plain_UT(const char* BinPath, const char* LogPath, const char* InFile, UINT32 Opt)
{
    char *FN;
    char folderPath[96], folderName[64];
    const char tempstr1[32] = "bub_out";
    const char tempstr2[32] = "\\group_0";
    UINT32 Ret, path_len;

    /* Get path to the folder e.g. c:\net\ minus for "group_0\" */
    AmbaUtility_StringCopy(folderPath, FILENAME_START_IDX - 7U, InFile);
    /* Create output folder e.g. c:\net\bub_out\ */
    AmbaUtility_StringAppend(folderPath, 128U, tempstr1);
    AmbaPrint_PrintStr5("Create folder %s ......", folderPath, NULL, NULL, NULL, NULL);
    (void) AmbaFS_MakeDir(folderPath);
    /* Create output folder e.g. c:\net\bub_out\group_0\ */
    AmbaUtility_StringAppend(folderPath, 128U, tempstr2);
    AmbaPrint_PrintStr5("Create folder %s ......", folderPath, NULL, NULL, NULL, NULL);
    (void) AmbaFS_MakeDir(folderPath);

    SCA_Plain_UT_Init(BinPath);
    if (Opt == 1U) {
        FN = GetNextFile(InFile);
        while (FN != NULL) {
            Ret = AmbaWrap_memset(folderName, 0x0, sizeof(folderName));
            if (Ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
            }
            Ret = AmbaWrap_memset(OutDirPath, 0x0, sizeof(OutDirPath));
            if (Ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
            }
            AmbaUtility_StringCopy(OutDirPath, 128U, folderPath);
            path_len = AmbaUtility_StringLength(FN);
            AmbaPrint_PrintStr5("Input FN %s", FN, NULL, NULL, NULL, NULL);
            /* Get original file name (without _pad32.bin/_pad32.yuv) e.g. AMBA1001 */
            AmbaUtility_StringCopy(folderName, (path_len - (FILENAME_START_IDX + INPUT_LAYER_NAME_LENGTH + 9U)), &FN[FILENAME_START_IDX-1U]);
            /* Create output folder e.g.  c:\net\bub_out\group_0\AMBA1001 */
            AmbaUtility_StringAppend(OutDirPath, 128U, folderName);
            AmbaPrint_PrintStr5("Create folder %s ......", OutDirPath, NULL, NULL, NULL, NULL);
            (void) AmbaFS_MakeDir(OutDirPath);
            SCA_Plain_UT_Run(FN, LogPath, Opt);
            Ret = AmbaWrap_memset(FN, 0x0, path_len);
            if (Ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
            }
            while (RUN_NEXT_LOCK == 1U) {
                AmbaPrint_PrintStr5("RUN_NEXT_LOCK, wait 10ms", NULL, NULL, NULL, NULL, NULL);
                (void) ArmTask_Sleep(10);
            }
            RUN_NEXT_LOCK = 1U;
            FN = GetNextFile(InFile);
        }
    } else {
        Ret = AmbaWrap_memset(folderName, 0x0, sizeof(folderName));
        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
        }
        Ret = AmbaWrap_memset(OutDirPath, 0x0, sizeof(OutDirPath));
        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
        }
        AmbaUtility_StringCopy(OutDirPath, 128U, folderPath);
        path_len = AmbaUtility_StringLength(InFile);
        /* Get original file name length (without _pad32.bin/.yuv)*/
        path_len = path_len - (FILENAME_START_IDX + INPUT_LAYER_NAME_LENGTH + 9U);
        /* Get original file name (without .bin/.yuv) e.g. AMBA1001 */
        AmbaUtility_StringCopy(folderName, path_len, &InFile[FILENAME_START_IDX-1U]);
        /* Create output folder e.g.  c:\net\bub_out\group_0\AMBA1001 */
        AmbaUtility_StringAppend(OutDirPath, 128U, folderName);
        AmbaPrint_PrintStr5("Create folder %s ......", OutDirPath, NULL, NULL, NULL, NULL);
        (void) AmbaFS_MakeDir(OutDirPath);

        SCA_Plain_UT_Run(InFile, LogPath, Opt);
    }
    // For passing Misra C
    AmbaMisra_TouchUnused(OutLayerName);
}

