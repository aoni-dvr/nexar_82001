/**
 *  @file SSPUT_ShellCommandMemio.c
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
 *  @details Example for the implementation of memio
 *
 */
#ifndef CONFIG_CV_CONFIG_TX_SDK7
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <rtos/AmbaRTOSWrapper.h>
#endif
#include "AmbaTransfer.h"
#include "AmbaTransfer_impl.h"
#include "cvtask_api.h"
#include "cvapi_memio_interface.h"
#include "cvapi_amba_od_interface.h"
#include "memio.h"
#include "memio_mmap.h"


#ifndef CONFIG_CV_CONFIG_TX_SDK7
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast64
#else
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast32
#endif

typedef struct {
    UINT32  MsgQIndex;
    void    *pMsgQCtrlNative;
    void    *pMsgQCtrlRemote;
    UINT32  MsgQAppOs;
    UINT32  MsgQCvtaskOs;
} Transfer_MsgQ_Ctx_s;

typedef struct{
	char*     va_base;
	uint32_t  pa_base;
	uint32_t  mm_size;
} MEMIO_MEM_REGION_s;


extern void* ambacv_p2v(uint64_t pa);
extern void Transfer_GetCtxByIndex(UINT32 Index, Transfer_MsgQ_Ctx_s **ppCtx);
extern UINT32 Transfer_Native_Send(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner, void *pData, UINT32 Len);


static pthread_t thread_source;
static pthread_t thread_sink;
static pthread_mutex_t memio_mutex_source = {0};
static pthread_mutex_t memio_mutex_sink = {0};
static INT32 MmioHdlr;
static UINT32 SinkFrameID = 0;
static MEMIO_MEM_REGION_s cv_mem;
static UINT32 TypeInfo = 0;

static void* MemIO_MemCheck(uint32_t input_addr)
{
    void *output_addr;

    if ((input_addr >= cv_mem.pa_base) && (input_addr <= cv_mem.pa_base + cv_mem.mm_size)){
        output_addr = ambacv_p2v(input_addr);
    } else {
        fprintf(stderr,"[%s] input_addr(0x%08x) is Not in cv_mem(0x%08x, size=0x%08x)!!\n",
            __FUNCTION__, input_addr, cv_mem.pa_base, cv_mem.mm_size);
        AmbaMisra_TypeCast32(&output_addr, &input_addr);
    }

    return output_addr;
}

static void MemIO_TransferInit(MEMIO_INFO_s *info)
{
    UINT32 Rval;
    AMBA_TRANSFER_DEFCONFIG_s DefConf;
    AMBA_TRANSFER_CHANNEL_CONFIG_s ChConf;
    AMBA_TRANSFER_CONNECT_s ConConf;
    memio_setting_t Setting;
    const memio_setting_t *pSet;

    // Init Ambatransfer framework
    Rval = AmbaTransfer_GetDefConfig(&DefConf);
    if (Rval != TRANSFER_OK) {
        AmbaPrint_PrintStr5("Fail to do AmbaTransfer_GetDefConfig().", NULL, NULL, NULL, NULL, NULL);
    }

    Rval = AmbaTransfer_Init(&DefConf);
    if (Rval != TRANSFER_OK) {
        AmbaPrint_PrintStr5("Fail to do AmbaTransfer_Init().", NULL, NULL, NULL, NULL, NULL);
    }

    Rval = AmbaTransfer_GetConfig(&ChConf);
    if (Rval != TRANSFER_OK) {
        AmbaPrint_PrintStr5("Fail to do AmbaTransfer_GetConfig().", NULL, NULL, NULL, NULL, NULL);
    } else {
        //Create MMIO MsgQ
        AmbaUtility_StringCopy(ChConf.Name, AMBATRANFER_NAME_MAXLENGTH, TRANSFER_CHANNEL_MEMIO);
        ChConf.Impl = &Transfer_Impl_Memio_Inst;
        ChConf.ImplSetting = NULL;
        Rval = AmbaTransfer_Create(&MmioHdlr, &ChConf);
        if (Rval != TRANSFER_OK) {
            AmbaPrint_PrintStr5("Fail to do AmbaTransfer_Create(%s).", ChConf.Name, NULL, NULL, NULL, NULL);
        }
    }

    //Connect MMIO MsgQ channel
    pSet = &Setting;
    AmbaUtility_StringCopy(ConConf.Name, AMBATRANFER_NAME_MAXLENGTH, TRANSFER_CHANNEL_MEMIO);
    if (info->mode == MEMIO_MODE_CVTASK_L_APP_LL){
        Setting.SubChannel = MEMIO_SUBCHAIN_DATASOURCE_3;
        Setting.Owner = MEMIO_OWNER_APP;

        AmbaMisra_TypeCast(&ConConf.Settings, &pSet);
        Rval = AmbaTransfer_Connect(&MmioHdlr, &ConConf);
        if (Rval != TRANSFER_OK) {
            AmbaPrint_PrintUInt5("Fail to do AmbaTransfer_Connect MemIO SubChannel(%s) Owner(%d).", Setting.SubChannel, Setting.Owner, 0U, 0U, 0U);
        }

        Setting.SubChannel = MEMIO_SUBCHAIN_DATASINK_3;
        Setting.Owner = MEMIO_OWNER_APP;

        AmbaMisra_TypeCast(&ConConf.Settings, &pSet);
        Rval = AmbaTransfer_Connect(&MmioHdlr, &ConConf);
        if (Rval != TRANSFER_OK) {
            AmbaPrint_PrintUInt5("Fail to do AmbaTransfer_Connect MemIO SubChannel(%s) Owner(%d).", Setting.SubChannel, Setting.Owner, 0U, 0U, 0U);
        }
    }
}

static void MemIO_TransferDeinit(void)
{
    UINT32 Rval;

    Rval = AmbaTransfer_Delete(MmioHdlr, NULL);
    if (Rval != TRANSFER_OK) {
        AmbaPrint_PrintStr5("Fail to do AmbaTransfer_Delete.", NULL, NULL, NULL, NULL, NULL);
    }
}

static uint32_t MemIO_TransferSourceData(const memio_source_recv_picinfo_t *pInfo)
{
    UINT32 Rval = TRANSFER_OK;
    AMBA_TRANSFER_RECEIVE_s Recv;
    AMBA_TRANSFER_SEND_s    Send;
    memio_setting_t Setting;
    const memio_setting_t *pSet;
    memio_source_send_req_t SourceReq;
    const memio_source_send_req_t *pSourceReq;
    UINT32 Addr;

    pSet = &Setting;
    Setting.SubChannel = MEMIO_SUBCHAIN_DATASOURCE_3;
    Setting.Owner = MEMIO_OWNER_APP;
    // Recv data source
    (void)AmbaWrap_memset(&SourceReq, 0, sizeof(SourceReq));
    pSourceReq = &SourceReq;
    AmbaMisra_TypeCast(&Recv.BufferAddr, &pSourceReq);
    Recv.Size = sizeof(memio_source_send_req_t);
    AmbaMisra_TypeCast(&Recv.Settings, &pSet);
    Recv.Result = NULL;
    Rval = AmbaTransfer_Receive(MmioHdlr, &Recv);
    if (Rval != TRANSFER_OK) {
         AmbaPrint_PrintUInt5("AmbaTransfer_Receive source_send_hdlr fail", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaPrint_PrintUInt5("MemIO_TransferSourceData recv (%d) frameset_id=%d seq_no=%d", Setting.SubChannel, SourceReq.cvtask_frameset_id, SourceReq.seq_no, 0U, 0U);

    // Send data source
    AmbaMisra_TypeCast(&(Send.PayloadAddr), &pInfo);
    Send.Size = sizeof(memio_source_recv_picinfo_t);
    AmbaMisra_TypeCast(&Send.Settings, &pSet);
    Send.Result = NULL;
    Rval = AmbaTransfer_Send(MmioHdlr, &Send);
    if (Rval != TRANSFER_OK) {
         AmbaPrint_PrintUInt5("AmbaTransfer_Send source_recv_hdlr fail", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaMisra_TypeCast32(&Addr, &pInfo);
    AmbaPrint_PrintUInt5("MemIO_TransferSourceData send(%d) addr = 0x%x ", Setting.SubChannel, Addr, 0U, 0U, 0U);

    return Rval;
}


static uint32_t MemIO_TransferSinkData(void** BufOut, uint32_t *BufOutSize, void FpBufOutProc(void* Par))
{
    UINT32 Rval = TRANSFER_OK;
    AMBA_TRANSFER_RECEIVE_s Recv;
    AMBA_TRANSFER_SEND_s    Send;
    memio_setting_t Setting;
    const memio_setting_t *pSet;
    static UINT32 SeqNo = 0;
    memio_sink_send_out_t SinkReq;
    const memio_sink_send_out_t *pSinkReq;
    memio_sink_recv_act_t SinkOut;
    const memio_sink_recv_act_t *pSinkOut;
    void *ptr;
    UINT32 Addr;

    SeqNo++;

    pSet = &Setting;
    Setting.SubChannel = MEMIO_SUBCHAIN_DATASINK_3;
    Setting.Owner = MEMIO_OWNER_APP;
    // Recv data sink
    (void)AmbaWrap_memset(&SinkReq, 0, sizeof(SinkReq));
    pSinkReq = &SinkReq;
    AmbaMisra_TypeCast(&Recv.BufferAddr, &pSinkReq);
    Recv.Size = sizeof(memio_sink_send_out_t);
    AmbaMisra_TypeCast(&Recv.Settings, &pSet);
    Recv.Result = NULL;
    Rval = AmbaTransfer_Receive(MmioHdlr, &Recv);
    if (Rval != TRANSFER_OK) {
         AmbaPrint_PrintUInt5("AmbaTransfer_Receive fail", 0U, 0U, 0U, 0U, 0U);
    }

    ptr = MemIO_MemCheck(SinkReq.io[0].addr);
    if (FpBufOutProc != NULL) {
        FpBufOutProc(ptr);
    }
    SinkFrameID = SinkReq.cvtask_frameset_id;
    printf("MemIO_TransferSinkData recv (%d) frameset_id=%d PA=0x%x VA=0x%lx size=0x%x\n",
        Setting.SubChannel, SinkReq.cvtask_frameset_id,SinkReq.io[0].addr, ptr, SinkReq.io[0].size);

    *BufOut = ptr;
    *BufOutSize = SinkReq.io[0].size;

    SinkOut.seq_no = SeqNo;
    // Send data sink
    pSinkOut = &SinkOut;
    AmbaMisra_TypeCast(&Send.PayloadAddr, &pSinkOut);
    Send.Size = sizeof(memio_sink_recv_act_t);
    AmbaMisra_TypeCast(&Send.Settings, &pSet);
    Send.Result = NULL;
    Rval = AmbaTransfer_Send(MmioHdlr, &Send);
    if (Rval != TRANSFER_OK) {
         AmbaPrint_PrintUInt5("AmbaTransfer_Send fail", 0U, 0U, 0U, 0U, 0U);
    }

    //AmbaPrint_PrintUInt5("MemIO_TransferData send(%d) SeqNo = %d ", Setting.SubChannel, (UINT32)SeqNo, 0U, 0U, 0U);

    return Rval;
}

static void* MemIO_FileInput(void *arg)
{
    FILE *Fp;
    UINT32 i, j;
    UINT32 Rval = TRANSFER_OK;
    UINT32 YuvWidth = 1920U;
    UINT32 YuvHeight = 1080U;
    UINT8* pData;
    UINT32 NumSuccess = 0U;
    static memio_source_recv_picinfo_t Info = {0};
    cv_pic_info_t *pPicInfo;
    MEMIO_INFO_s *info = (MEMIO_INFO_s *)arg;

    memio_mmap_init(YuvWidth * YuvHeight * 3 / 2, (void **)&pData);
    pPicInfo = &Info.pic_info;

    for(j = 1; j < 2; j++){
        pthread_mutex_lock(&memio_mutex_source);
        AmbaPrint_PrintUInt5("MemIO_FileInput:\n", 0U, 0U, 0U, 0U, 0U);
        // prepare input data
        Fp = fopen(info->input, "rb");
        NumSuccess = fread((void *)pData, YuvWidth * YuvHeight * 3 / 2, 1U, Fp);
        if (NumSuccess <= 0U) {
            AmbaPrint_PrintUInt5("fread failed", 0U, 0U, 0U, 0U, 0U);
        }
        fclose(Fp);

        pPicInfo->capture_time = 0U;
        pPicInfo->channel_id = 0U;
        pPicInfo->frame_num = 0U;
        pPicInfo->pyramid.image_width_m1 = YuvWidth - 1U;
        pPicInfo->pyramid.image_height_m1 = YuvHeight - 1U;
        pPicInfo->pyramid.image_pitch_m1 = YuvWidth - 1U;
        pPicInfo->pyramid.half_octave[0].ctrl.roi_pitch = YuvWidth;
        pPicInfo->pyramid.half_octave[0].roi_start_col = 0;
        pPicInfo->pyramid.half_octave[0].roi_start_row = 0;
        pPicInfo->pyramid.half_octave[0].roi_width_m1 = YuvWidth - 1U;
        pPicInfo->pyramid.half_octave[0].roi_height_m1 = YuvHeight - 1U;

        pPicInfo->pyramid.half_octave[1].ctrl.disable = 1U;
        pPicInfo->pyramid.half_octave[2].ctrl.disable = 1U;
        pPicInfo->pyramid.half_octave[3].ctrl.disable = 1U;
        pPicInfo->pyramid.half_octave[4].ctrl.disable = 1U;
        pPicInfo->pyramid.half_octave[5].ctrl.disable = 1U;

        pPicInfo->rpLumaLeft[0] = memio_mmap_v2p(pData);
        pPicInfo->rpChromaLeft[0] = (pPicInfo->rpLumaLeft[0] + (YuvWidth * YuvHeight));
        pPicInfo->rpLumaRight[0] = pPicInfo->rpLumaLeft[0];
        pPicInfo->rpChromaRight[0] = pPicInfo->rpChromaLeft[0];

        AmbaPrint_PrintUInt5("MemIO_FileInput rpLumaLeft(0x%x) rpChromaLeft(0x%x) rpLumaRight(0x%x) rpChromaRight(0x%x)",pPicInfo->rpLumaLeft[0], pPicInfo->rpChromaLeft[0], pPicInfo->rpLumaRight[0], pPicInfo->rpChromaRight[0], 0U);

        Rval = MemIO_TransferSourceData(&Info);
        if (Rval != TRANSFER_OK) {
            pthread_mutex_unlock(&memio_mutex_source);
            break;
        }
        pthread_mutex_unlock(&memio_mutex_source);
    }

    return NULL;
}

static amba_od_candidate_t *OD_getBbx(const amba_od_out_t *pAmbaOD)
{
    amba_od_candidate_t *Bbx = NULL;
    UINT8 *u8ptr = (UINT8 *)pAmbaOD;
    int diff;

    if (pAmbaOD == NULL) {
        printf("%s: pAmbaOD is NULL!!\n",__FUNCTION__);
    } else {
        diff = pAmbaOD->objects_offset;
        Bbx = (amba_od_candidate_t *)(u8ptr + diff);
        //printf("AmbaOD=%p, objects_offset=0x%08x(%d), Bbx=%p\n",AmbaOD, AmbaOD->objects_offset, diff, Bbx);
    }

    return Bbx;
}

static void* MemIO_ODFrameOutput(void *arg)
{
    UINT32 i, j, RecvSize;
    UINT32 Rval = TRANSFER_OK;
    amba_od_out_t *AmbaOD;
    amba_od_candidate_t *Bbx = NULL;
    MEMIO_INFO_s *info = (MEMIO_INFO_s *)arg;

    for(j = 1; j < 2; j++){
        pthread_mutex_lock(&memio_mutex_sink);
        Rval = MemIO_TransferSinkData((void **)&AmbaOD, &RecvSize, NULL);
        if (Rval != TRANSFER_OK) {
            pthread_mutex_unlock(&memio_mutex_sink);
            break;
        }

        //print out result
        AmbaPrint_PrintUInt5("Result (%d)", AmbaOD->num_objects, 0U, 0U, 0U, 0U);
        for (i=0U ; i<AmbaOD->num_objects ; i++) {
            void *class_void;
            char *class_name;

            Bbx = OD_getBbx(AmbaOD);
            printf("  (%d, %d)-(%d,%d) c:%d\n",
                Bbx[i].bb_start_col,
                Bbx[i].bb_start_row,
                (Bbx[i].bb_start_col + Bbx[i].bb_width_m1 + 1U),
                (Bbx[i].bb_start_row + Bbx[i].bb_height_m1 + 1U),
                Bbx[i].class);
        }
        pthread_mutex_unlock(&memio_mutex_sink);
    }

    AmbaPrint_PrintUInt5("MemIO FileInput test finish\n", 0U, 0U, 0U, 0U, 0U);

    return NULL;
}

uint32_t MemIO_Init(MEMIO_INFO_s *info)
{
    UINT32 YuvWidth = 3840U;
    UINT32 YuvHeight = 2160U;
    unsigned char* pData;

    if (memio_get_cvshm_info(&cv_mem.pa_base, &cv_mem.mm_size) != 0) {
        printf("Can't get cv shared memory info !!!\n");
        return -1;
    }

    MemIO_TransferInit(info);
    TypeInfo = info->type;

    return 0U;
}

uint32_t MemIO_Run(MEMIO_INFO_s *info)
{
    printf("[MemIO_Run] mode = %d\n",info->mode);
    if (info->mode == MEMIO_MODE_CVTASK_L_APP_LL){
        pthread_create(&thread_source, NULL, MemIO_FileInput, info);
        pthread_create(&thread_sink, NULL, MemIO_ODFrameOutput, info);
    } else {
        // main thread got nothing to do , just put it to sleep
        sleep(0xFFFFFFFF);
    }

    return 0U;
}

uint32_t MemIO_Stop(MEMIO_INFO_s *info)
{
    if (info->mode == MEMIO_MODE_CVTASK_L_APP_LL){
        pthread_join(thread_source, NULL);
        pthread_join(thread_sink, NULL);
    }

    return 0U;
}


uint32_t MemIO_TriggerTask(MEMIO_INFO_s *info)
{
    UINT32 Rval = TRANSFER_OK;
    UINT32 i;
    AMBA_TRANSFER_SEND_s Send;
    memio_setting_t Setting;
    const memio_setting_t *pSet;
    Transfer_MsgQ_Ctx_s *pCtx;

    pSet = &Setting;
    AmbaMisra_TypeCast(&Send.PayloadAddr, &pSet);
    AmbaMisra_TypeCast(&Send.Settings, &pSet);
    Setting.Owner = MEMIO_OWNER_APP;
    Send.Result = NULL;
    for(i = 0; i < TRANS_MSGQ_MAX; i++) {
        Setting.SubChannel = i;
        Transfer_GetCtxByIndex(i, &pCtx);
        if( pCtx->MsgQCvtaskOs != 0U ) {
            if ((i == MEMIO_SUBCHAIN_DATASOURCE_0) || (i == MEMIO_SUBCHAIN_DATASOURCE_1) ||
                (i == MEMIO_SUBCHAIN_DATASOURCE_2) || (i == MEMIO_SUBCHAIN_DATASOURCE_3)){
                Send.Size = sizeof(memio_source_recv_picinfo_t);
            } else {
                Send.Size = sizeof(memio_sink_recv_act_t);
            }
            Rval = Transfer_Native_Send(pCtx, Setting.Owner, (void *)Send.PayloadAddr, Send.Size);
        }
    }

    return Rval;
}

uint32_t MemIO_Finish(MEMIO_INFO_s *info)
{
    UINT32 Rval = TRANSFER_OK;
    AMBA_TRANSFER_DISCONNECT_s DisconConf;
    memio_setting_t Setting;
    const memio_setting_t *pSet;

    if (info->mode == MEMIO_MODE_CVTASK_L_APP_LL){
        //Disconnet
        pSet = &Setting;
        Setting.SubChannel = MEMIO_SUBCHAIN_DATASOURCE_3;
        Setting.Owner = MEMIO_OWNER_APP;

        AmbaMisra_TypeCast(&DisconConf.Settings, &pSet);
        Rval = AmbaTransfer_Disconnect(MmioHdlr, &DisconConf);
        if (Rval != TRANSFER_OK) {
            AmbaPrint_PrintUInt5("Fail to do AmbaTransfer_Disconnect %d", Setting.SubChannel, 0U, 0U, 0U, 0U);
        }

        Setting.SubChannel = MEMIO_SUBCHAIN_DATASINK_3;
        Setting.Owner = MEMIO_OWNER_APP;

        AmbaMisra_TypeCast(&DisconConf.Settings, &pSet);
        Rval = AmbaTransfer_Disconnect(MmioHdlr, &DisconConf);
        if (Rval != TRANSFER_OK) {
            AmbaPrint_PrintUInt5("Fail to do AmbaTransfer_Disconnect %d", Setting.SubChannel, 0U, 0U, 0U, 0U);
        }
    }

    MemIO_TransferDeinit();

    return Rval;
}

