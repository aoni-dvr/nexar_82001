/**
*  @file SvcDataLoader.c
 *
 * Copyright (c) 2020 Ambarella International
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
*  @details svc data loader
*
*/

#include ".svc_autogen"

#include "AmbaMisraFix.h"
#include "AmbaDef.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaDSP_Liveview.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTaskList.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "SvcDataLoader.h"
#include "SvcUcode.h"
#include "SvcImg.h"
#include "SvcImgTask.h"
#include "SvcTiming.h"
#include "SvcLiveviewTask.h"
#include "SvcDspTask.h"
#if defined(CONFIG_ICAM_BIST_UCODE)
#include "AmbaShell.h"

#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcUcBIST.h"
#endif
#if defined(CONFIG_ICAM_FIXED_ISO_CFG_USED)
#include "SvcIsoCfg.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#endif

#define DTL_LOG          "DTL"
#define DTL_STACK_SIZE   (0x4000U)
#define DTL_MSG_MAX_NUM  (20U)
#define DTL_TIMEOUT      (5000U)

typedef struct {
    UINT32                     ListID;
    SVC_DATA_LOADER_CALLBACK_f pCb;
} DTL_MSG_s;

typedef struct {
    SVC_TASK_CTRL_s       Task;
    UINT8                 Stack[DTL_STACK_SIZE];
    DTL_MSG_s             MsgBuf[DTL_MSG_MAX_NUM];
    AMBA_KAL_MSG_QUEUE_t  MsgQue;
} DTL_HDLR_s;

static DTL_HDLR_s DataLoaderHdlr GNU_SECTION_NOZEROINIT;

static inline void DTL_NG( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_NG( DTL_LOG, pFormat, Arg1, Arg2); }
static inline void DTL_OK( const char *pFormat, UINT32 Arg1, UINT32 Arg2) { SvcLog_OK( DTL_LOG, pFormat, Arg1, Arg2); }

static UINT32 SvcDataLoadList_SwitchMode(void);
static UINT32 SvcDataLoadList_Boot(void);
static void   SendUcodeIssue(UINT8 Event);
#if defined(CONFIG_BUILD_IMGFRW_AAA)
static void SendIqIssue(UINT8 Event);
static void SendIqIssue(UINT8 Event)
{
    UINT32 Rval;
    SVC_APP_STAT_IQ_s IqLoaderStatus;

    IqLoaderStatus.Status = Event;
    Rval = SvcSysStat_Issue(SVC_APP_STAT_IQ, &IqLoaderStatus);
    if (SVC_OK != Rval) {
        DTL_NG("## fail to set IQ status %u flag", Event, 0U);
    }
}
#endif
static void   SendUcodeIssue(UINT8 Event)
{
    UINT32 Rval;
    SVC_APP_STAT_UCODE_s UcodeLoaderStatus;
    UcodeLoaderStatus.Status = Event;

    Rval = SvcSysStat_Issue(SVC_APP_STAT_UCODE, &UcodeLoaderStatus);
    if (SVC_OK != Rval) {
        DTL_NG("## fail to set ucode status %u flag", Event, 0U);
    }
}
#if defined(CONFIG_ICAM_IMGCAL_USED)

static void SendCalibIssue(UINT8 Event)
{
    UINT32 Rval;
    SVC_APP_STAT_CALIB_s CalibLoaderStatus;

    CalibLoaderStatus.Status = Event;
    Rval = SvcSysStat_Issue(SVC_APP_STAT_CALIB, &CalibLoaderStatus);
    if (SVC_OK != Rval) {
        DTL_NG("## fail to set Calib status %u flag", Event, 0U);
    }
}
#endif


/* -----------------------------------------------------------------------------*/
/*    load list for boot up                                                     */
/* -----------------------------------------------------------------------------*/
static UINT32 SvcDataLoadList_Boot(void)
{
    UINT32 Rval;
    UINT32 IsPartial = 0U;
    UINT32 LoadWay   = 0U;
    UINT32 BufOff[UCODE_FILE_NUM_MAX];
    DTL_OK("## SvcDataLoadList_Boot START", 0U, 0U);

    #if defined(CONFIG_ICAM_TIMING_LOG) 
    SvcTime(SVC_TIME_CALIB_LOAD_START, "Calib load START");
    #endif
#if defined(CONFIG_ICAM_IMGCAL_USED)
    {
        extern UINT32 SvcCalib_DataLoadAll(void);
        /* load calib data */
        Rval = SvcCalib_DataLoadAll();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcCalib_DataLoadAll failed", 0U, 0U);
        } else {
            SendCalibIssue(SVC_APP_STAT_CALIB_LOAD_DONE);
        }
    }
#endif
    #if defined(CONFIG_ICAM_TIMING_LOG) 
    SvcTime(SVC_TIME_CALIB_LOAD_DONE, "Calib load DONE");
    #endif

    /* trigger ucode loader to load ucode */
    /* (1) : load ucode part 0 */
    AmbaSvcWrap_MisraMemset(BufOff, 0x0, sizeof(BufOff));
    SvcUcode_Config();

    /* plan A : load ucode from from boot device */
    Rval = SvcUcode_LoadPartial0(BufOff, &IsPartial);
    if (SVC_OK != Rval) {
        DTL_NG("## SvcUcode_LoadPartial0 failed", 0U, 0U);
        LoadWay = 1U;
    } else {
        SendUcodeIssue(SVC_APP_STAT_UCODE_DEFBIN_DONE);
        SendUcodeIssue(SVC_APP_STAT_UCODE_LIV_DONE);
    }

    /* plan B : load ucode from SD card */
    if (LoadWay == 1U) {
        Rval = SvcUcode_LoadFromStorage();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcUcode_LoadFromStorage fail", 0U, 0U);
        } else {
            SendUcodeIssue(SVC_APP_STAT_UCODE_DEFBIN_DONE);
            SendUcodeIssue(SVC_APP_STAT_UCODE_LIV_DONE);
            SendUcodeIssue(SVC_APP_STAT_UCODE_ALL_DONE);
        }
    }

#if defined(CONFIG_ICAM_BIST_UCODE)
    if (SVC_OK == Rval) {
        Rval = SvcUcBIST_LoadData();
        if (Rval != SVC_OK) {
            DTL_NG("## fail to load ucode BIST data", 0U, 0U);
        }
    }
#endif

#if defined(CONFIG_ICAM_FIXED_ISO_CFG_USED)
    {
        ULONG   BufAddrCcThreeD, BufAddrCcReg;
        UINT32  BufSizeCcThreeD, BufSizeCcReg;

        if (SVC_OK == Rval) {
            if (SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ICFG_IQ_CC_3D, &BufAddrCcThreeD, &BufSizeCcThreeD) == OK) {
                if (SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ICFG_IQ_CC_REG, &BufAddrCcReg, &BufSizeCcReg) == OK) {
                    SvcIsoCfg_CcLoad(BufAddrCcThreeD, BufSizeCcThreeD, BufAddrCcReg, BufSizeCcReg);
                }
            }
        }
    }
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* (2) : load IQ table part 1 */
    if (SVC_OK == Rval) {
        Rval = SvcImgTask_LoadIqPartial0();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcImgTask_LoadIqPartial 0 failed", 0U, 0U);
        } else {
            SendIqIssue(SVC_APP_STAT_IQ_VID_READY);
        }
    }
#endif
#if defined(CONFIG_BUILD_CV)
    if (SVC_OK == Rval) {
        extern UINT32 SvcCvMainTask_Load(void);
        Rval = SvcCvMainTask_Load();
        if (SVC_OK != Rval) {
            DTL_NG("SvcCvMainTask load failed ! Rval %u", Rval, 0U);
        }
    }
    if (SVC_OK == Rval) {
        extern UINT32 SvcCvCtrlTask_Load(void);
        Rval = SvcCvCtrlTask_Load();
        if (SVC_OK != Rval) {
            DTL_NG("SvcCvCtrlTask_Load failed ! Rval %u", Rval, 0U);
        }
    }
#endif
    /* (3) : load ucode part 1 */
    if ((SVC_OK == Rval) && (LoadWay == 0U)) {
        Rval = SvcUcode_LoadStage1(BufOff, &IsPartial);
        if (SVC_OK != Rval) {
            DTL_NG("## SvcUcode_LoadStage1 failed", 0U, 0U);
        } else {
            SendUcodeIssue(SVC_APP_STAT_UCODE_ALL_DONE);
        }
    }
#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* (4) : load IQ table part 2 */
    if (SVC_OK == Rval) {
        Rval = SvcImgTask_LoadIqPartial1();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcImgTask_LoadIqPartial 1 failed", 0U, 0U);
        } else {
            SendIqIssue(SVC_APP_STAT_IQ_ALL_READY);
        }
    }

#endif
    DTL_OK("## SvcDataLoadList_Boot DONE", 0U, 0U);

    return Rval;
}

/* -----------------------------------------------------------------------------*/
/*    load list for switch mode                                                 */
/* -----------------------------------------------------------------------------*/
static UINT32 SvcDataLoadList_SwitchMode(void)
{
    UINT32 Rval = SVC_OK;

    DTL_OK("## SvcDataLoadList_SwitchMode START", 0U, 0U);

#if defined(CONFIG_BUILD_CV)
    {
        extern UINT32 SvcCvMainTask_Load(void);
        Rval = SvcCvMainTask_Load();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcCvMainTask_Load failed", 0U, 0U);
        }
    }

    if (SVC_OK == Rval) {
        extern UINT32 SvcCvCtrlTask_Load(void);
        Rval = SvcCvCtrlTask_Load();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcCvCtrlTask_Load failed", 0U, 0U);
        }
    }
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    Rval = AmbaKAL_EventFlagClear(&SvcImgEventId, 0xFFFFFFFFU);
    if (SVC_OK != Rval) {
        SvcLog_NG(DTL_LOG, "## fail to clear SvcImgEventId flag", 0U, 0U);
    }

    /* (2) : load IQ table part 1 */
    if (SVC_OK == Rval) {
        Rval = SvcImgTask_LoadIqPartial0();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcImgTask_LoadIqPartial 0 failed", 0U, 0U);
        }
    }
    /* (3) : load IQ table part 2 */
    if (SVC_OK == Rval) {
        Rval = SvcImgTask_LoadIqPartial1();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcImgTask_LoadIqPartial 1 failed", 0U, 0U);
        }
    }
#endif

    DTL_OK("## SvcDataLoadList_SwitchMode DONE", 0U, 0U);

    return Rval;
}

static UINT32 SvcDataLoadList_CvResume(void)
{
    UINT32 Rval = SVC_OK;

    DTL_OK("## SvcDataLoadList_CvResume START", 0U, 0U);

#if defined(CONFIG_BUILD_CV)
    {
        extern UINT32 SvcCvMainTask_Load(void);
        Rval = SvcCvMainTask_Load();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcCvMainTask_Load failed", 0U, 0U);
        }
    }

    if (SVC_OK == Rval) {
        extern UINT32 SvcCvCtrlTask_Load(void);
        Rval = SvcCvCtrlTask_Load();
        if (SVC_OK != Rval) {
            DTL_NG("## SvcCvCtrlTask_Load failed", 0U, 0U);
        }
    }
#endif

    DTL_OK("## SvcDataLoadList_CvResume DONE", 0U, 0U);

    return Rval;
}

/**
* Data loader load
* @param [in] ListID load list for specified case
* @return none
*/
UINT32 SvcDataLoader_Load(UINT8 ListID)
{
    UINT32    Rval;
    DTL_MSG_s MsgQ;

    MsgQ.ListID = ListID;
    Rval = AmbaKAL_MsgQueueSend(&DataLoaderHdlr.MsgQue, &MsgQ, DTL_TIMEOUT);

    if (SVC_OK != Rval) {
        DTL_NG("SvcDataLoader_Load: msg queue send failed !", 0U, 0U);
    }

    return Rval;
}

UINT32 SvcDataLoader_LoadCallback(SVC_DATA_LOADER_CALLBACK_f pCb)
{
    UINT32 Rval;

    if (pCb == NULL) {
        DTL_NG("SvcDataLoader_LoadCallback: callback func ptr == NULL !", 0U, 0U);
        Rval = SVC_NG;
    } else {
        DTL_MSG_s MsgQ = {
            .ListID = DTL_LIST_CALLBACK,
            .pCb    = pCb,
        };

        Rval = AmbaKAL_MsgQueueSend(&DataLoaderHdlr.MsgQue, &MsgQ, DTL_TIMEOUT);
        if (SVC_OK != Rval) {
            DTL_NG("SvcDataLoader_Load: msg queue send failed !", 0U, 0U);
        }
    }

    return Rval;
}

/**
* Data loader task entry
* @param [in] EntryArg task entry arg
* @return none
*/
static void* DataLoaderEntry(void* EntryArg)
{
    UINT32 Rval;
    DTL_MSG_s Msg;
    const ULONG *pArg;
    const volatile UINT32 condition = 1U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while (condition == 1U) {
        Rval = AmbaKAL_MsgQueueReceive(&DataLoaderHdlr.MsgQue, &Msg, KAL_WAIT_FOREVER);

        if (SVC_OK != Rval) {
            DTL_NG("DataLoaderEntry: msg queue receive failed !", 0U, 0U);
            continue;
        }

        /* set system status flag */
        switch (Msg.ListID) {
            case DTL_LIST_BOOT:
                Rval = SvcDataLoadList_Boot();
                break;

            case DTL_LIST_SWITCH:
                Rval = SvcDataLoadList_SwitchMode();
                break;

            case DTL_LIST_CALLBACK:
                Rval = Msg.pCb();
                break;

            case DTL_LIST_CV:
                Rval = SvcDataLoadList_CvResume();
                break;

            default:
                DTL_NG("DataLoaderEntry: unknown list ID %u", Msg.ListID, 0U);
                break;
        }

        if (SVC_OK != Rval) {
            DTL_NG("DataLoaderEntry: Load list %u error, Rval 0x%X", Msg.ListID, Rval);
        }
    }

    return NULL;
}

/**
* Data loader initialization
* @param [in] none
* @return none
*/
UINT32 SvcDataLoader_Init(void)
{
    UINT32 Rval;
    UINT32 CtrlID = 0U;
    static       char DTL_MSGQ[] = "SvcDataLoaderMsgQ";
    static const char DTL_TASK[] = "SvcDataLoaderTask";

    DTL_OK("### SvcSysDataLoader_Init: START", 0U, 0U);

#if defined(CONFIG_ICAM_BIST_UCODE)
    {
        ULONG   BufAddr;
        UINT32  BufSize;

        if (SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_UCBIST, &BufAddr, &BufSize) == OK) {
            if (SvcUcBIST_Init(BufAddr, BufSize) != SVC_OK) {
                DTL_NG("## fail to init ucode BIST", 0U, 0U);
            }
        } else {
            DTL_NG("## fail to get ucode BIST buffer", 0U, 0U);
        }
    }
#endif

    /* Create data loader msg queue */
    Rval = AmbaKAL_MsgQueueCreate(&DataLoaderHdlr.MsgQue, DTL_MSGQ, (UINT32)sizeof(DTL_MSG_s), DataLoaderHdlr.MsgBuf, (UINT32)sizeof(DataLoaderHdlr.MsgBuf));
    if (SVC_OK != Rval) {
        DTL_NG("AmbaKAL_MsgQueueCreate failed, sizeof(DTL_MSG_s) %u, Rval 0x%X", (UINT32)sizeof(DTL_MSG_s), Rval);
    // } else {
    //     DTL_OK("AmbaKAL_MsgQueueCreate OK, sizeof(DTL_MSG_s) %u, Buffer %u", sizeof(DTL_MSG_s), sizeof(DataLoaderHdlr.MsgBuf));
    }

    /* Create data loader task */
    if (SVC_OK == Rval) {
        DataLoaderHdlr.Task.Priority   = SVC_DATA_LOADER_TASK_PRI;
        DataLoaderHdlr.Task.EntryFunc  = DataLoaderEntry;
        DataLoaderHdlr.Task.EntryArg   = 0U;
        DataLoaderHdlr.Task.pStackBase = DataLoaderHdlr.Stack;
        DataLoaderHdlr.Task.StackSize  = DTL_STACK_SIZE;
        DataLoaderHdlr.Task.CpuBits    = SVC_DATA_LOADER_TASK_CPU_BITS;
        Rval = SvcTask_Create(DTL_TASK, &DataLoaderHdlr.Task);
        if (SVC_OK != Rval) {
            DTL_NG("SvcSysDataLoader_Init: SvcTask_Create failed, Rval 0x%X", Rval, 0U);
        }
    }

    /* register SVC_APP_STAT_UCODE callback */
    if (SVC_OK != SvcSysStat_Register(SVC_APP_STAT_UCODE, SvcLiveviewTask_StatusCB, &CtrlID)) {
        DTL_NG("## SvcLiveviewTask_StatusCB failed", 0U, 0U);
    }

    if (SVC_OK != SvcSysStat_Register(SVC_APP_STAT_UCODE, SvcDspTask_StatusCB, &CtrlID)) {
        DTL_NG("## SvcDspTask_StatusCB failed", 0U, 0U);
    }

    DTL_OK("### SvcSysDataLoader_Init: DONE", 0U, 0U);

    return Rval;
}

