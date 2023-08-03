/**
 *  @file RefCode_Avtp.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Enet Avb refcode
 *
 */

#include "AmbaDef.h"
#include "AmbaShell.h"
#include "AmbaShell_Enet.h"
#include "AmbaUtility.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AvbStack.h"
#include "RefCode_Avtp.h"
#include "AmbaCFS.h"

#define DATA_IS_READY 0x1U
#define STACK_SIZE              (0x20000U)
#define FIFO_HDLR_BUF_SIZE             (49152U)
#define LISTENER_BUF_SIZE   0x2000000U
#define LISTENER_FRAME_SIZE 1024U
#define TALKER_TMPBUF_SIZE 0x40000U


AMBA_AVB_AVTP_FIFO_s FifoConfig[MAX_TALKER_NUM];
AMBA_AVB_AVTP_TALKER_s TalkerConfig[MAX_TALKER_NUM];
AMBA_AVB_AVTP_LISTENER_s ListenerConfig[MAX_LISTENER_NUM];
UINT8 InitFifoConfig = 0;
UINT8 InitTalkerConfig = 0;
UINT8 InitListenerConfig = 0;

typedef struct {
    AMBA_KAL_EVENT_FLAG_t MuxerFlag;
    AMBA_KAL_MUTEX_t FifoMutex;
    UINT32 InCount;
    UINT32 OutCount;
    UINT32 FirstIdr;
} AMBA_AVB_AVTP_TALKER_TASK_INFO_s;

static UINT32 AvtpTalkerFpsControl[MAX_TALKER_NUM];

static AMBA_AVB_AVTP_TALKER_TASK_INFO_s AvtpTalkerTaskInfo[MAX_TALKER_NUM];
static AMBA_KAL_TASK_t AvtpTalkerTask[MAX_TALKER_NUM] = {0};
static UINT32 AvtpTalkerTaskStop[MAX_TALKER_NUM] = {0U};

static AMBA_KAL_TASK_t AvtpListenerTask[MAX_LISTENER_NUM] = {0};
static UINT32 AvtpListenerTaskStop[MAX_LISTENER_NUM] = {0U};

static UINT32 AvtpListenerCfsOpenFile(const char *name, AMBA_CFS_FILE_s **File)
{
    AMBA_CFS_FILE_PARAM_s CfsParam;
    UINT32 Rval = AmbaCFS_GetFileParam(&CfsParam);
    if (Rval == OK) {
        CfsParam.Mode = AMBA_CFS_FILE_MODE_WRITE_ONLY;
        AmbaUtility_StringCopy(CfsParam.Filename, AMBA_CFS_MAX_FILENAME_LENGTH, name);
        CfsParam.Filename[AMBA_CFS_MAX_FILENAME_LENGTH - 1U] = '\0';
        Rval = AmbaCFS_FileOpen(&CfsParam, File);
        if (Rval != OK) {
            AmbaPrint_PrintStr5("AvtpListenerCfsOpenFile AmbaCFS_fopen() failed", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("AvtpListenerCfsOpenFile AmbaCFS_GetFileParam() failed", NULL, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static void *AvtpListenerTaskFunc(void *EntryArg)
{
    AMBA_AVB_AVTP_LISTENER_s *pListener;
    AMBA_AVB_AVTP_FRAME_INFO_s Frame;
    UINT32 loop = 1U;
    char FileName[64] = "c:\\receive";
    AMBA_CFS_FILE_s *File;
    UINT32 FnLen, Ret, Id;
    UINT i;
    void *ptr;

    /*Id = (UINT32)EntryArg;*/
    if (AmbaWrap_memcpy(&Id, &EntryArg, sizeof(Id))!= 0U) { }

    pListener = &ListenerConfig[Id];
    AmbaPrint_PrintStr5("%s start", __FUNCTION__, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();

    AmbaUtility_StringAppendUInt32(FileName, sizeof(FileName), Id, 10U);
    AmbaUtility_StringAppend(FileName, sizeof(FileName), ".dat");
    Ret = AvtpListenerCfsOpenFile(FileName, &File);
    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaFS_FileOpen fail Ret = %d\n", Ret, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    } else {

        // process muxer pipe.
        while (loop == 1U) {
            if(AvtpListenerTaskStop[Id] == 1U) {
                break;
            }
            if (AmbaWrap_memset(&Frame, 0, sizeof(AMBA_AVB_AVTP_FRAME_INFO_s))!= 0U) { }
            Ret = AmbaAvbStack_AvtpListenerRx(pListener, &Frame, 1000U);
            if( Ret == 0U )
            {
                for(i = 0; i < Frame.VecNum; i++) {
                    AmbaMisra_TypeCast(&ptr, &Frame.Vec[i].pBase);
                    (void) AmbaCFS_FileWrite(ptr, 1U, Frame.Vec[i].Len, File, &FnLen);
                    if (FnLen != Frame.Vec[i].Len) {
                        AmbaPrint_PrintUInt5("file write failed (%x %x)", FnLen, Frame.Vec[i].Len, 0U, 0U, 0U);
                        break;
                    }
                }
                (void) AmbaAvbStack_AvtpListenerRxDone(pListener, &Frame);
            }

        }
    }

    if (File != NULL) {
        (void) AmbaCFS_FileClose(File);
    }
    AmbaPrint_PrintStr5("%s end", __FUNCTION__, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AvtpListenerTaskStop[Id] = 2U;
    return NULL;
}

static void AvtpTalkerStreamerFunc(UINT32 Id, const AMBA_AVB_AVTP_FIFO_s* pFifo, const AMBA_FIFO_BITS_DESC_s* desc)
{
    AMBA_AVB_AVTP_FRAME_INFO_s Frame;
    const AMBA_AVB_AVTP_TALKER_s *pTalker = &TalkerConfig[Id];
    UINT32 FifoBufStart,FifoBufEnd;
    UINT32 BufStart,BufEnd;
    static UINT8 AvtpTalkerTmpBuf[MAX_TALKER_NUM][TALKER_TMPBUF_SIZE];

    if ( desc == NULL ){
        //AmbaPrint_PrintStr5("%s: Invalid input", __func__, NULL, NULL, NULL, NULL);
        //AmbaPrint_Flush();
    } else if ( (desc->FrameType != AMBA_FIFO_TYPE_IDR_FRAME) &&
        (desc->FrameType != AMBA_FIFO_TYPE_I_FRAME) &&
        (desc->FrameType != AMBA_FIFO_TYPE_P_FRAME) &&
        (desc->FrameType != AMBA_FIFO_TYPE_B_FRAME) &&
        (desc->FrameType != AMBA_FIFO_TYPE_EOS) ) {
        AmbaPrint_PrintStr5("%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("frame type %u", desc->FrameType, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    } else {
        if ( (desc->FrameType != AMBA_FIFO_TYPE_EOS) && (desc->Size != 0U) ) {
            // drop until IDR, we need to consider intra refresh case
            if ( (AvtpTalkerTaskInfo[Id].FirstIdr == 0U) && (desc->FrameType != AMBA_FIFO_TYPE_IDR_FRAME)) {

            } else {
                AvtpTalkerTaskInfo[Id].FirstIdr = 1U;
                //AmbaPrint_PrintUInt5("FrameType = %d buf = %x len = %x mark = %d timestamp = %d", desc->FrameType, (UINT32)desc->StartAddr, desc->Size, desc->Completed, timestamp);
                //AmbaPrint_Flush();

                /*FifoBufStart = (UINT32)pFifo->pBuffer;*/
                if (AmbaWrap_memcpy(&FifoBufStart, &pFifo->pBuffer, sizeof(FifoBufStart))!= 0U) { }

                FifoBufEnd = FifoBufStart + pFifo->BufferSize;

                /*BufStart = (UINT32)desc->StartAddr;*/
                if (AmbaWrap_memcpy(&BufStart, &desc->StartAddr, sizeof(BufStart))!= 0U) { }

                BufEnd = BufStart + desc->Size;

                if( (BufStart <= FifoBufEnd) && (BufStart >= FifoBufStart) ) {
                    if( (BufEnd <= FifoBufEnd) || ( (BufEnd > FifoBufEnd) && (TALKER_TMPBUF_SIZE >= desc->Size)) ) {
                        if( BufEnd <= FifoBufEnd ) {
                            Frame.Vec[0].pBase = desc->StartAddr;
                        } else {
                            if (AmbaWrap_memcpy(&AvtpTalkerTmpBuf[Id][0], &desc->StartAddr[0], (desc->Size - (BufEnd - FifoBufEnd)))!= 0U) { }
                            if (AmbaWrap_memcpy(&AvtpTalkerTmpBuf[Id][(desc->Size - (BufEnd - FifoBufEnd))], &pFifo->pBuffer[0], (BufEnd - FifoBufEnd))!= 0U) { }
                            Frame.VecNum = (UINT8)1U;
                            Frame.Vec[0].pBase = &AvtpTalkerTmpBuf[Id][0];
                            //AmbaPrint_PrintUInt5("wrap around : FifoBufStart = %x FifoBufEnd = %x, BufStart = %x BufEnd = %x", FifoBufStart, FifoBufEnd, BufStart, BufEnd, 0U);
                        }
                        Frame.VecNum = (UINT8)1U;
                        Frame.Vec[0].Len = desc->Size;
                        Frame.FrameType = AVTP_FORMAT_SUBTYPE_H264;
                        Frame.FrameTimeStamp = (UINT32)(desc->Pts);
                        Frame.Completed = desc->Completed;
                        (void) AmbaAvbStack_AvtpTalkerTx(pTalker, &Frame);
                    } else {
                        AmbaPrint_PrintStr5("Tmp buffer is not enough", __func__, NULL, NULL, NULL, NULL);
                        AmbaPrint_PrintUInt5("FifoBufStart = %x FifoBufEnd = %x, BufStart = %x BufEnd = %x", FifoBufStart, FifoBufEnd, BufStart, BufEnd, 0U);
                    }
                } else {
                    AmbaPrint_PrintStr5("Fifo buffer is not currect", __func__, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("FifoBufStart = %x FifoBufEnd = %x, BufStart = %x BufEnd = %x", FifoBufStart, FifoBufEnd, BufStart, BufEnd, 0U);
                }
            }
        } else {
            AmbaPrint_PrintStr5("%s: Good-bye", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }

    return;
}

static UINT32 AvtpTalkerGetIdFromFifo(const AMBA_FIFO_HDLR_s *hdlr, UINT32 *Id)
{
    UINT32 i,Ret = 0U;

    *Id = 0xFFU;

    for(i = 0U; i < MAX_TALKER_NUM;i++) {
        if ( (FifoConfig[i].pVirtFifoHdlr != NULL) && (FifoConfig[i].pVirtFifoHdlr == hdlr)) {
            *Id = i;
            break;
        }
    }

    if(*Id == 0xFFU) {
        Ret = 1U;
    }

    return Ret;
}

static UINT32 AvtpTalkerFrameCB(const AMBA_FIFO_HDLR_s *hdlr, UINT8 event, AMBA_FIFO_CALLBACK_INFO_s *info)
{
    UINT32 Ret,Id;

    //AmbaPrint("%s on Event: 0x%x", __FUNCTION__, event);

    AmbaMisra_TouchUnused(info);
    Ret = AvtpTalkerGetIdFromFifo(hdlr, &Id);
    if ( (hdlr != NULL) && (Ret == 0U)) {
        if (event == AMBA_FIFO_EVENT_DATA_READY) {
            if (AmbaKAL_MutexTake(&AvtpTalkerTaskInfo[Id].FifoMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
                AvtpTalkerTaskInfo[Id].InCount++;
                (void) AmbaKAL_EventFlagSet(&AvtpTalkerTaskInfo[Id].MuxerFlag, DATA_IS_READY);
            }
            (void) AmbaKAL_MutexGive(&AvtpTalkerTaskInfo[Id].FifoMutex);
        }
    }
    return 0;
}

static UINT32 AvtpTalkerCheckDataReady(UINT32 Id)
{
    UINT32 Ret,Rval = 1U;
    UINT32 Flags;

    if (AvtpTalkerTaskInfo[Id].InCount <= AvtpTalkerTaskInfo[Id].OutCount) {
        Ret = AmbaKAL_EventFlagGet(&AvtpTalkerTaskInfo[Id].MuxerFlag, DATA_IS_READY, 1U, 1U, &Flags, 1000U);
        if( Ret == 0U) {
            Rval = 0U;
        } else {
            Rval = 1U;
        }
    } else {
        Rval = 0U;
        if (AmbaKAL_MutexTake(&AvtpTalkerTaskInfo[Id].FifoMutex, AMBA_KAL_WAIT_FOREVER) == OK) {
            AvtpTalkerTaskInfo[Id].OutCount++;
        }
        (void) AmbaKAL_MutexGive(&AvtpTalkerTaskInfo[Id].FifoMutex);
    }

    return Rval;
}

static void *AvtpTalkerTaskFunc(void *EntryArg)
{
    UINT32 loop = 1U;
    AMBA_FIFO_BITS_DESC_s *Frame;
    UINT32 Ret, Id;
    UINT32 T1 = 0U,T2 = 0U;

    /*Id = (UINT32)EntryArg;*/
    if (AmbaWrap_memcpy(&Id, &EntryArg, sizeof(Id))!= 0U) { }

    AmbaPrint_PrintStr5("%s start", __FUNCTION__, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();

    (void) AmbaKAL_GetSysTickCount(&T1);
    {
        // process muxer pipe.
        while (loop == 1U) {
            if(AvtpTalkerTaskStop[Id] == 1U) {
                break;
            }

            Ret = AvtpTalkerCheckDataReady(Id);
            if( Ret == 0U ) {
                if (AmbaFIFO_PeekEntry(FifoConfig[Id].pVirtFifoHdlr, &Frame, 0) == OK) {
                    AvtpTalkerStreamerFunc(Id, &FifoConfig[Id], Frame);
                    (void) AmbaFIFO_RemoveEntry(FifoConfig[Id].pVirtFifoHdlr, 1);
                    (void) AmbaKAL_GetSysTickCount(&T2);
                    if(AvtpTalkerFpsControl[Id] != 0U) {
                        if( (T2 >= T1) && ( (T2-T1) < AvtpTalkerFpsControl[Id]) ) {
                            (void) AmbaKAL_TaskSleep(AvtpTalkerFpsControl[Id] - (T2-T1));
                        }
                    }
                    T1 = T2;
                }
            }
        }
    }
    AmbaPrint_PrintStr5("%s end", __FUNCTION__, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AvtpTalkerTaskStop[Id] = 2U;

    return NULL;
}

void AvtpTalker_FpsControl(UINT32 Id, UINT32 Fps)
{
    if (Fps != 0U) {
        AvtpTalkerFpsControl[Id] = Fps;
    }
}

void AvtpTalkerStart(UINT32 Id)
{
    AMBA_FIFO_CFG_s fifoDefCfg;
    UINT32 Rval;
    void *EntryArg;
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    char TaskName[64] = {"AvtpTalkerTask"};
    char MutexName[64] = {"AvtpTalkerMuxerMutex"};
    char EventName[64] = {"AvtpTalkerMuxerFlag"};
    AMBA_AVB_AVTP_TALKER_s *pTalkerConfig = &TalkerConfig[Id];
    static UINT8 AvtpTalkerFifoHdlrBuffer[MAX_TALKER_NUM][FIFO_HDLR_BUF_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 AvtpTalkerTaskStack[MAX_TALKER_NUM][STACK_SIZE] __attribute__((section(".bss.noinit")));

    AmbaPrint_PrintStr5("%s ", __FUNCTION__, NULL, NULL, NULL, NULL);
    if (FifoConfig[Id].pFifoHdlr == NULL) {
        EnetPrint(__func__, __LINE__, ANSI_WHITE, "please set pEncFifoHdlr before start talker");
    } else {
        (void) AmbaAvbStack_GetConfig(0, &pAvbConfig);
        pTalkerConfig->pAvbConfig = pAvbConfig;
        Rval = AmbaAvbStack_AvtpTalkerReg(pTalkerConfig);
        if(Rval != 0U) {
            AmbaPrint_PrintStr5("%s: AmbaAvbStack_AvtpTalkerReg fail", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }

         /* set task info */
        if (AmbaWrap_memset(&AvtpTalkerTaskInfo[Id], 0, sizeof(AMBA_AVB_AVTP_TALKER_TASK_INFO_s))!= 0U) { }
        (void) AmbaKAL_MutexCreate(&AvtpTalkerTaskInfo[Id].FifoMutex, MutexName);
        (void) AmbaKAL_EventFlagCreate(&AvtpTalkerTaskInfo[Id].MuxerFlag, EventName);

        /* create fifo */
        (void) AmbaFIFO_GetDefaultCfg(&fifoDefCfg);
        fifoDefCfg.NumEntries = 1024;
        fifoDefCfg.CbEvent = AvtpTalkerFrameCB;
        fifoDefCfg.BufferSize = FIFO_HDLR_BUF_SIZE;//MAX_FIFO_BUFFER_SIZE * sizeof(AMBA_FIFO_BITS_DESC_s);
        fifoDefCfg.BufferAddr = &(AvtpTalkerFifoHdlrBuffer[Id][0]);//(UINT8 *)VirtFifoBuffer;
        fifoDefCfg.BaseHdlr = FifoConfig[Id].pFifoHdlr;
        Rval = AmbaFIFO_Create(&fifoDefCfg, &FifoConfig[Id].pVirtFifoHdlr);
        if (Rval != OK) {
            AmbaPrint_PrintUInt5("create fifo failed and error = %u", Rval, 0U, 0U, 0U, 0U);
        }

        /* create fifo task */
        /*EntryArg = (void *)Id;*/
        EntryArg = NULL;
        if (AmbaWrap_memcpy(&EntryArg, &Id, sizeof(Id))!= 0U) { }

        (void) AmbaKAL_TaskCreate(&AvtpTalkerTask[Id], /** pTask */
                                  TaskName, /** pTaskName */
                                  153, /** Priority */
                                  AvtpTalkerTaskFunc, /** void (*EntryFunction)(UINT32) */
                                  EntryArg, /** EntryArg */
                                  AvtpTalkerTaskStack[Id],  /** pStackBase */
                                  STACK_SIZE, /** StackByteSize */
                                  1U); /** AutoStart */
    }
}

void AvtpTalkerStop(UINT32 Id)
{
    UINT32 CheckCnt = 20U;
    const AMBA_AVB_AVTP_TALKER_s *pTalkerConfig = &TalkerConfig[Id];
    AmbaPrint_PrintStr5("%s ", __FUNCTION__, NULL, NULL, NULL, NULL);
    AvtpTalkerTaskStop[Id] = 1U;
    do {
        CheckCnt -= 1U;
        if (CheckCnt == 0U) {
            AmbaPrint_PrintStr5("%s wait task fail", __FUNCTION__, NULL, NULL, NULL, NULL);
            break;
        }
        (void) AmbaKAL_TaskSleep(100U);
    } while (AvtpTalkerTaskStop[Id] != 2U);
    (void) AmbaKAL_TaskTerminate(&AvtpTalkerTask[Id]);
    (void) AmbaKAL_TaskDelete(&AvtpTalkerTask[Id]);
    (void) AmbaAvbStack_AvtpTalkerUnReg(pTalkerConfig);
    (void) AmbaFIFO_Delete(FifoConfig[Id].pVirtFifoHdlr);
    (void) AmbaKAL_MutexDelete(&AvtpTalkerTaskInfo[Id].FifoMutex);
    (void) AmbaKAL_EventFlagDelete(&AvtpTalkerTaskInfo[Id].MuxerFlag);
}

void AvtpListenerStart(UINT32 Id)
{
    UINT32 Ret;
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    char TaskName[64] = {"AvtpListenerTask"};
    AMBA_AVB_AVTP_LISTENER_s* pListenerConfig = &ListenerConfig[Id];
    static AMBA_AVB_AVTP_FRAME_INFO_s  FrameQueue[MAX_LISTENER_NUM][LISTENER_FRAME_SIZE];
    static UINT8                       Buf[MAX_LISTENER_NUM][LISTENER_BUF_SIZE];
    static UINT8 AvtpListenerTaskStack[MAX_LISTENER_NUM][STACK_SIZE] __attribute__((section(".bss.noinit")));
    void *EntryArg;

    AmbaPrint_PrintStr5("%s ", __FUNCTION__, NULL, NULL, NULL, NULL);
    AvtpListenerTaskStop[Id] = 0U;
    (void) AmbaAvbStack_GetConfig(0U, &pAvbConfig);

    pListenerConfig->pAvbConfig = pAvbConfig;
    pListenerConfig->pBuf = &Buf[Id][0];
    pListenerConfig->BufSize = LISTENER_BUF_SIZE;
    pListenerConfig->pFrameQueue = &FrameQueue[Id][0];
    pListenerConfig->FrameQueueNum = LISTENER_FRAME_SIZE;

    Ret = AmbaAvbStack_AvtpListenerReg(pListenerConfig);
    if(Ret != 0U) {
        AmbaPrint_PrintStr5("%s: AmbaAvbStack_AvtpListenerReg fail", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }
    /* create task */
    /*EntryArg = (void *)Id;*/
    EntryArg = NULL;
    if (AmbaWrap_memcpy(&EntryArg, &Id, sizeof(Id))!= 0U) { }

    (void) AmbaKAL_TaskCreate(&AvtpListenerTask[Id], /** pTask */
                              TaskName, /** pTaskName */
                              153, /** Priority */
                              AvtpListenerTaskFunc, /** void (*EntryFunction)(UINT32) */
                              EntryArg, /** EntryArg */
                              AvtpListenerTaskStack[Id],  /** pStackBase */
                              STACK_SIZE, /** StackByteSize */
                              1U); /** AutoStart */

}

void AvtpListenerStop(UINT32 Id)
{
    UINT32 CheckCnt = 20U;
    const AMBA_AVB_AVTP_LISTENER_s* pListenerConfig = &ListenerConfig[Id];

    AmbaPrint_PrintStr5("%s ", __FUNCTION__, NULL, NULL, NULL, NULL);
    AvtpListenerTaskStop[Id] = 1U;
    do {
        CheckCnt -= 1U;
        if (CheckCnt == 0U) {
            AmbaPrint_PrintStr5("%s wait task fail", __FUNCTION__, NULL, NULL, NULL, NULL);
            break;
        }
        (void) AmbaKAL_TaskSleep(100U);
    } while (AvtpListenerTaskStop[Id] != 2U);
    (void) AmbaKAL_TaskTerminate(&AvtpListenerTask[Id]);
    (void) AmbaKAL_TaskDelete(&AvtpListenerTask[Id]);
    (void) AmbaAvbStack_AvtpListenerUnReg(pListenerConfig);
}
