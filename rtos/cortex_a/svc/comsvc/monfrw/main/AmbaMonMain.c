/**
 *  @file AmbaMonMain.c
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  @details Amba Monitor Main
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaShell.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaSensor.h"
#include "AmbaYuv.h"

#include "AmbaDspInt.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonInt.h"
#include "AmbaMonDsp.h"
#include "AmbaMonWdog.h"

#include "AmbaMonListenVin.h"
#include "AmbaMonListenDsp.h"
#include "AmbaMonListenAaa.h"
#include "AmbaMonListenVout.h"
#include "AmbaMonListenSerdes.h"

#include "AmbaMonStateVin.h"
#include "AmbaMonStateDsp.h"
#include "AmbaMonStateVout.h"

#include "AmbaMonFrwCmd_Def.h"
#include "AmbaMonFrwCmdApp.h"
#include "AmbaMonFrwCmdMain.h"

#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"
#include "AmbaMonMain_Platform.h"

#ifdef CONFIG_BUILD_IMGFRW_AAA
#include "../../../imgfrw/main/inc/AmbaImgMain.h"
#endif

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_MAIN_MEM_u_*/ {
    void                     *pVoid;
    AMBA_MON_TIMEOUT_CB_s    *pTimeoutCb;
} AMBA_MON_MAIN_MEM_u;

typedef struct /*_AMBA_MON_MAIN_MEM_s_*/ {
    AMBA_MON_MAIN_MEM_u    Ctx;
} AMBA_MON_MAIN_MEM_s;

/* message id: message input for vin/dsp/vout */
static AMBA_MON_MESSAGE_ID_s VinMsgId GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_ID_s DspMsgId GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_ID_s VoutMsgId GNU_SECTION_NOZEROINIT;

/* listen id: global meesge in timeline */
static AMBA_MON_LISTEN_ID_s TimelineId GNU_SECTION_NOZEROINIT;

/* state id: state output for vin/dsp/vout */
static AMBA_MON_MESSAGE_ID_s VinStateId GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_ID_s DspStateId GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_ID_s VoutStateId GNU_SECTION_NOZEROINIT;

/* timeline message input port */
static AMBA_MON_LISTEN_PORT_s TimelinePort GNU_SECTION_NOZEROINIT;

/* connector between vin message and timeline listen */
static AMBA_MON_MESSAGE_CB_ID_s TimelineVinSofCbId[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineVinEofCbId[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

/* connector between dsp message and timeline listen */
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspRawCbId[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspYuvCbId[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspYuvAltCbId[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspDefYuvAltCbId[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspDispCbId[AMBA_MON_NUM_DISP_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspVoutCbId[AMBA_MON_NUM_VOUT_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspDefRawCbId[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspCfaAaaCbId[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspRgbAaaCbId[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspFovLatencyCbId[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspVoutLatencyCbId[AMBA_MON_NUM_VOUT_CHANNEL] GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_CB_ID_s TimelineDspVinTimeoutCbId[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

/* connector between vin message and timeline listen */
static AMBA_MON_MESSAGE_CB_ID_s TimelineVoutIsrCbId[AMBA_MON_NUM_VOUT_CHANNEL] GNU_SECTION_NOZEROINIT;

/* vin message input port */
const char *pVinSofName = "sof_isr";
const char *pVinEofName = "eof_isr";

AMBA_MON_MESSAGE_PORT_s VinSofMsgPort[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s VinEofMsgPort[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

/* dsp message input port */
const char *pDspRawName = "raw_rdy";
const char *pDspYuvName = "yuv_rdy";
const char *pDspYuvAltName = "yuv_alt_rdy";
const char *pDspDefYuvAltName = "def_yuv_alt_rdy";
const char *pDspDispName = "disp_rdy";
const char *pDspVoutName = "vout_rdy";
const char *pDspDefRawName = "def_raw_rdy";
const char *pDspVinTimeoutName = "vin_timeout";

AMBA_MON_MESSAGE_PORT_s DspRawMsgPort[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspYuvMsgPort[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspYuvAltMsgPort[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspDefYuvAltMsgPort[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspDispMsgPort[AMBA_MON_NUM_DISP_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspVoutMsgPort[AMBA_MON_NUM_VOUT_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspDefRawMsgPort[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspFovLatencyMsgPort[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspVoutLatencyMsgPort[AMBA_MON_NUM_VOUT_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspVinTimeoutMsgPort[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

/* vout meesage input port */
const char *pVoutName = "vout_isr";

AMBA_MON_MESSAGE_PORT_s VoutIsrMsgPort[AMBA_MON_NUM_VOUT_CHANNEL] GNU_SECTION_NOZEROINIT;

/* dsp aaa input port */
const char *pDspCfaAaaName = "cfa_aaa_rdy";
const char *pDspRgbAaaName = "rgb_aaa_rdy";

AMBA_MON_MESSAGE_PORT_s DspCfaAaaMsgPort[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;
AMBA_MON_MESSAGE_PORT_s DspRgbAaaMsgPort[AMBA_MON_NUM_FOV_CHANNEL] GNU_SECTION_NOZEROINIT;

/* monitor main info */
AMBA_MON_MAIN_INFO_s AmbaMonMain_Info GNU_SECTION_NOZEROINIT;
UINT32 AmbaMonMain_VinSrcTable[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
UINT32 AmbaMonMain_VinSrcFlag[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
UINT32 AmbaMonMain_SensorIdTable[AMBA_MON_NUM_VIN_CHANNEL][AMBA_MON_NUM_VIN_SENSOR] GNU_SECTION_NOZEROINIT;

UINT32 AmbaMonMain_FrameTimeTable[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
UINT32 AmbaMonMain_FrameLineTable[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;
UINT32 AmbaMonMain_FrameLineFactor[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

UINT32 AmbaMonMain_SvrDelayTable[AMBA_MON_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

static UINT32 AmbaMonMain_SlowShutterTable[AMBA_MON_NUM_VIN_CHANNEL] = {0};

static UINT32 AmbaMonMain_FrameTimeInUs[AMBA_MON_NUM_VIN_CHANNEL] = {0};
static UINT32 AmbaMonMain_FrameCnt[AMBA_MON_NUM_VIN_CHANNEL] = {0};

static UINT32 AmbaMonMain_FrameTimeoutInUs[AMBA_MON_NUM_VIN_CHANNEL] = {0};
static UINT32 AmbaMonMain_MsgFrameTimeoutInUs[AMBA_MON_NUM_VIN_CHANNEL] = {0};
static UINT32 AmbaMonMain_FovFrameTimeoutInUs[AMBA_MON_NUM_VIN_CHANNEL] = {0};

#define AMBA_MON_MAIN_BOOT_LATENCY 500U
static UINT32 AmbaMonMain_BootLatency[AMBA_MON_NUM_VIN_CHANNEL] = {0};

#define AMBA_MON_ISR_TIMEOUT_PERCEND  20U
#define AMBA_MON_MSG_TIMEOUT_PERCEND  50U
#define AMBA_MON_FOV_TIMEOUT_PERCEND  90U

AMBA_MON_MAIN_VIN_SRC_EN_s AmbaMonMain_VinSrcEn = { .Ctx = { .Data = 0xFFFFFFFFU } };
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
AMBA_MON_MAIN_DSP_SRC_EN_s AmbaMonMain_DspSrcEn = { .Ctx = { .Data = 0xFFFF11FFU } };
#else
AMBA_MON_MAIN_DSP_SRC_EN_s AmbaMonMain_DspSrcEn = { .Ctx = { .Data = 0xFFFF111FU } };
#endif
AMBA_MON_MAIN_VOUT_SRC_EN_s AmbaMonMain_VoutSrcEn = { .Ctx = { .Data = 0xFFFFFFFFU } };

UINT32 AmbaMonMain_FovLatencySkip[AMBA_MON_NUM_FOV_CHANNEL] = {0};
UINT32 AmbaMonMain_VoutLatencySkip[AMBA_MON_NUM_VOUT_CHANNEL] = {0};

static AMBA_MON_MESSAGE_PORT_s AmbaMonMain_EchoPort = {0};

/**
 *  Amba monitor main system init ex
 *  @param[in] CoreInclusion core inclusion
 */
void AmbaMonMain_SysInitEx(UINT32 CoreInclusion)
{
    /* ring for vin message */
#define AMBA_MON_VIN_MSG_CHUNK_NUM    (32U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VIN_MSG_CHUNK_RVD    (4U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VIN_MSG_CHUNK_SIZE   (256U)
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE VinMsgRingBuf[AMBA_MON_VIN_MSG_CHUNK_NUM*AMBA_MON_VIN_MSG_CHUNK_SIZE];
    AMBA_MON_MESSAGE_CHUNK_INFO_s VinMsgChunkInfo;

    /* ring for dsp message */
#define AMBA_MON_DSP_MSG_CHUNK_NUM    (72U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_DSP_MSG_CHUNK_RVD    (8U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_DSP_MSG_CHUNK_SIZE   (256U)
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE DspMsgChunkBuf[AMBA_MON_DSP_MSG_CHUNK_NUM*AMBA_MON_DSP_MSG_CHUNK_SIZE];
    AMBA_MON_MESSAGE_CHUNK_INFO_s DspMsgChunkInfo;

    /* ring for vout message */
#define AMBA_MON_VOUT_MSG_CHUNK_NUM    (32U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VOUT_MSG_CHUNK_RVD    (4U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VOUT_MSG_CHUNK_SIZE   (256U)
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE VoutMsgChunkBuf[AMBA_MON_VOUT_MSG_CHUNK_NUM*AMBA_MON_VOUT_MSG_CHUNK_SIZE];
    AMBA_MON_MESSAGE_CHUNK_INFO_s VoutMsgChunkInfo;

    /* ring for timeline listen */
#define AMBA_MON_TIMELINE_CHUNK_NUM    (288U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_TIMELINE_CHUNK_RVD    (32U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_TIMELINE_CHUNK_SIZE   ((UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_u))
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE TimelineLinkChunkBuf[AMBA_MON_TIMELINE_CHUNK_NUM*AMBA_MON_TIMELINE_CHUNK_SIZE];
    AMBA_MON_LISTEN_CHUNK_INFO_s TimelineChunkInfo;

    /* ring for vin state */
#define AMBA_MON_VIN_STATE_CHUNK_NUM    (32U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VIN_STATE_CHUNK_RVD    (4U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VIN_STATE_CHUNK_SIZE   (256U)
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE VinStateChunkBuf[AMBA_MON_VIN_STATE_CHUNK_NUM*AMBA_MON_VIN_STATE_CHUNK_SIZE];
    AMBA_MON_MESSAGE_CHUNK_INFO_s VinStateChunkInfo;

    /* ring for dsp state */
#define AMBA_MON_DSP_STATE_CHUNK_NUM    (32U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_DSP_STATE_CHUNK_RVD    (4U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_DSP_STATE_CHUNK_SIZE   (256U)
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE DspStateChunkBuf[AMBA_MON_DSP_STATE_CHUNK_NUM*AMBA_MON_DSP_STATE_CHUNK_SIZE];
    AMBA_MON_MESSAGE_CHUNK_INFO_s DspStateChunkInfo;

    /* ring for vout state */
#define AMBA_MON_VOUT_STATE_CHUNK_NUM    (32U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VOUT_STATE_CHUNK_RVD    (4U*AMBA_MON_NUM_VIN_CHANNEL)
#define AMBA_MON_VOUT_STATE_CHUNK_SIZE   (256U)
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE VoutStateChunkBuf[AMBA_MON_VOUT_STATE_CHUNK_NUM*AMBA_MON_VOUT_STATE_CHUNK_SIZE];
    AMBA_MON_MESSAGE_CHUNK_INFO_s VoutStateChunkInfo;

    UINT32 i;
    UINT32 FuncRetCode;

    AmbaMonPrint_Enable(AMBA_MON_PRINT_ENABLE_FLAG);
    AmbaMonPrintStr(S_PRINT_FLAG_MSG, "monitor main sys init...");

    /* monitor mem init */
    FuncRetCode = AmbaMonMain_MemInit();
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monitor mem init");
    }
    /* monitor ring init */
    FuncRetCode = AmbaMonRing_Init();
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monitor ring init");
    }
    /* monitor event init */
    FuncRetCode = AmbaMonEvent_Init();
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monitor event init");
    }
    /* monitor watchdog init */
    FuncRetCode = AmbaMonWatchdog_Init();
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monitor watchdog init");
    }
    /* monitor message init */
    FuncRetCode = AmbaMonMessage_Init();
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monitor message init");
    }
    /* monitor listen init */
    FuncRetCode = AmbaMonListen_Init();
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monitor listen init");
    }

    /* vin message */
    VinMsgChunkInfo.Num = AMBA_MON_VIN_MSG_CHUNK_NUM;
    VinMsgChunkInfo.Size = AMBA_MON_VIN_MSG_CHUNK_SIZE;
    VinMsgChunkInfo.AuxInNum = AMBA_MON_VIN_MSG_CHUNK_RVD;
    VinMsgChunkInfo.pMemBase = VinMsgRingBuf;
    FuncRetCode = AmbaMonMessage_Create(&VinMsgId, "vin_msg", &VinMsgChunkInfo);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin message create");
    }
    /* dsp message */
    DspMsgChunkInfo.Num = AMBA_MON_DSP_MSG_CHUNK_NUM;
    DspMsgChunkInfo.Size = AMBA_MON_DSP_MSG_CHUNK_SIZE;
    DspMsgChunkInfo.AuxInNum = AMBA_MON_DSP_MSG_CHUNK_RVD;
    DspMsgChunkInfo.pMemBase = DspMsgChunkBuf;
    FuncRetCode = AmbaMonMessage_Create(&DspMsgId, "dsp_msg", &DspMsgChunkInfo);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp message create");
    }
    /* vout message */
    VoutMsgChunkInfo.Num = AMBA_MON_VOUT_MSG_CHUNK_NUM;
    VoutMsgChunkInfo.Size = AMBA_MON_VOUT_MSG_CHUNK_SIZE;
    VoutMsgChunkInfo.AuxInNum = AMBA_MON_VOUT_MSG_CHUNK_RVD;
    VoutMsgChunkInfo.pMemBase = VoutMsgChunkBuf;
    FuncRetCode = AmbaMonMessage_Create(&VoutMsgId, "vout_msg", &VoutMsgChunkInfo);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout message create");
    }

    /* timeline listen */
    TimelineChunkInfo.Num = AMBA_MON_TIMELINE_CHUNK_NUM;
    TimelineChunkInfo.Size = AMBA_MON_TIMELINE_CHUNK_SIZE;
    TimelineChunkInfo.AuxInNum = AMBA_MON_TIMELINE_CHUNK_RVD;
    TimelineChunkInfo.pMemBase = TimelineLinkChunkBuf;
    FuncRetCode = AmbaMonListen_Create(&TimelineId, "timeline", &TimelineChunkInfo);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: timeline listen create");
    }

    /* vin state */
    VinStateChunkInfo.Num = AMBA_MON_VIN_STATE_CHUNK_NUM;
    VinStateChunkInfo.Size = AMBA_MON_VIN_STATE_CHUNK_SIZE;
    VinStateChunkInfo.AuxInNum = AMBA_MON_VIN_STATE_CHUNK_RVD;
    VinStateChunkInfo.pMemBase = VinStateChunkBuf;
    FuncRetCode = AmbaMonMessage_Create(&VinStateId, "vin_state", &VinStateChunkInfo);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin state create");
    }
    /* dsp state */
    DspStateChunkInfo.Num = AMBA_MON_DSP_STATE_CHUNK_NUM;
    DspStateChunkInfo.Size = AMBA_MON_DSP_STATE_CHUNK_SIZE;
    DspStateChunkInfo.AuxInNum = AMBA_MON_DSP_STATE_CHUNK_RVD;
    DspStateChunkInfo.pMemBase = DspStateChunkBuf;
    FuncRetCode = AmbaMonMessage_Create(&DspStateId, "dsp_state", &DspStateChunkInfo);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp state create");
    }
    /* vout state */
    VoutStateChunkInfo.Num = AMBA_MON_VOUT_STATE_CHUNK_NUM;
    VoutStateChunkInfo.Size = AMBA_MON_VOUT_STATE_CHUNK_SIZE;
    VoutStateChunkInfo.AuxInNum = AMBA_MON_VOUT_STATE_CHUNK_RVD;
    VoutStateChunkInfo.pMemBase = VoutStateChunkBuf;
    FuncRetCode = AmbaMonMessage_Create(&VoutStateId, "vout_state", &VoutStateChunkInfo);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout state create");
    }

    /* interrupt task */
    FuncRetCode = AmbaMonInt_Create(4U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: interrupt task create");
    }

    /* watchdog task */
    FuncRetCode = AmbaMonWdog_Create(7U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: watchdog task create");
    }

    /* listen task */
    FuncRetCode = AmbaMonListenVin_Create(10U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: listen vin task create");
    }
    FuncRetCode = AmbaMonListenDsp_Create(10U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: listen dsp task create");
    }
    FuncRetCode = AmbaMonListenVout_Create(10U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: listen vout task create");
    }
    FuncRetCode = AmbaMonListenSerdes_Create(10U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: listen serdes task create");
    }
    FuncRetCode = AmbaMonListenAaa_Create(20U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: listen aaa task create");
    }

    /* state task (example) */
    FuncRetCode = AmbaMonStateVin_Create(10U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: state vin task create");
    }
    FuncRetCode = AmbaMonStateDsp_Create(10U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: state dsp task create");
    }
    FuncRetCode = AmbaMonStateVout_Create(10U, CoreInclusion);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: state vout task create");
    }

    /* monitor framework command attach */
    {
        FuncRetCode = AmbaMonFrwCmd_Attach();
        if (FuncRetCode != OK_UL) {
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monfrw command attach");
        }
    }
    {
        FuncRetCode = AmbaMonFrwCmdApp_Attach();
        if (FuncRetCode != OK_UL) {
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: monfrw command app attach");
        }
    }

    /* boot latency reset */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        AmbaMonMain_BootLatency[i] = AMBA_MON_MAIN_BOOT_LATENCY;
    }

    /* debug flag init */
    AmbaMonMain_Info.Debug.Data = AMBA_MON_DEBUG_ENABLE_FLAG;
}

/**
 *  Amba monitor main system init
 */
void AmbaMonMain_SysInit(void)
{
    AmbaMonMain_SysInitEx(0x01U);
}

/**
 *  Amba monitor main info get
 *  @param[in] CmdMsg command and message
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_InfoGet(AMBA_MON_MAIN_CMD_MSG_s CmdMsg)
{
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_SENSOR_CHANNEL_s SensorChan = {0};
    AMBA_SENSOR_DEVICE_INFO_s SensorDev;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    AMBA_YUV_CHANNEL_s YuvChan = {0};
    AMBA_YUV_STATUS_INFO_s YuvStatus;

    const AMBA_VIN_FRAME_RATE_s *pFrameRate;

    UINT32 FrameTimeInUs;
    UINT32 FrameTimeoutInUs;
    UINT32 TimeInUs;
    UINT32 MsgFrameTimeoutInUs;
    UINT32 MsgTimeInUs;
    UINT32 FovFrameTimeoutInUs;
    UINT32 FovTimeInUs;

    /* vin info get */
    AmbaMonMain_Info.Vin.Num = 0U;
    AmbaMonMain_Info.Vin.SelectBits = CmdMsg.Ctx.Liv.VinId;
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((((UINT32) CmdMsg.Ctx.Liv.VinId) & (((UINT32) 1U) << i)) > 0U) {
            AmbaMonMain_Info.Vin.Num++;
        }
    }

    /* vin td get */
    AmbaMonMain_Info.Td.Num = 0U;
    AmbaMonMain_Info.Td.SelectBits = CmdMsg.Ctx.Liv.TdId;
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((((UINT32) CmdMsg.Ctx.Liv.TdId) & (((UINT32) 1U) << i)) > 0U) {
            AmbaMonMain_Info.Td.Num++;
        }
    }

    /* fov info get */
    AmbaMonMain_Info.Fov.Num = 0U;
    AmbaMonMain_Info.Fov.SelectBits = CmdMsg.Ctx.Liv.FovId;
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((((UINT32) CmdMsg.Ctx.Liv.FovId) & (((UINT32) 1U) << i)) > 0U) {
            AmbaMonMain_Info.Fov.Num++;
        }
    }

    /* vout info get */
    AmbaMonMain_Info.Vout.Num = 0U;
    AmbaMonMain_Info.Vout.SelectBits = CmdMsg.Ctx.Liv.VoutId;
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((((UINT32) CmdMsg.Ctx.Liv.VoutId) & (((UINT32) 1U) << i)) > 0U) {
            AmbaMonMain_Info.Vout.Num++;
        }
    }

    /* debug msg */
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vin num", AmbaMonMain_Info.Vin.Num, 10U);
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vin select bits", AmbaMonMain_Info.Vin.SelectBits, 16U);
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "td num", AmbaMonMain_Info.Td.Num, 10U);
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "td select bits", AmbaMonMain_Info.Td.SelectBits, 16U);
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "fov num", AmbaMonMain_Info.Fov.Num, 10U);
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "fov select bits", AmbaMonMain_Info.Fov.SelectBits, 16U);
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vout num", AmbaMonMain_Info.Vout.Num, 10U);
    AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vout select bits", AmbaMonMain_Info.Vout.SelectBits, 16U);

    /* vin src flag reset */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        AmbaMonMain_VinSrcFlag[i] = 0U;
    }

    /* frame timetick get */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((((UINT32) CmdMsg.Ctx.Liv.VinId) & (((UINT32) 1U) << i)) > 0U) {
            if (AmbaMonMain_VinSrcTable[i] == AMBA_MON_VIN_SRC_SENSOR) {
                SensorChan.VinID = i;
                FuncRetCode = AmbaSensor_GetDeviceInfo(&SensorChan, &SensorDev);
                if (FuncRetCode == SENSOR_ERR_NONE) {
                    AmbaMonMain_SvrDelayTable[i] = SensorDev.FrameRateCtrlInfo.FirstReflectedFrame;
                }
                FuncRetCode = AmbaSensor_GetStatus(&SensorChan, &SensorStatus);
                if (FuncRetCode == SENSOR_ERR_NONE) {
                    FuncRetCode = AmbaSensor_GetModeInfo(&SensorChan, &(SensorStatus.ModeInfo.Config), &(SensorStatus.ModeInfo));
                    if (FuncRetCode == SENSOR_ERR_NONE) {
                        AmbaMonMain_FrameLineTable[i] = SensorStatus.ModeInfo.FrameLengthLines;
                        pFrameRate = &(SensorStatus.ModeInfo.FrameRate);
                        FrameTimeInUs = ((pFrameRate->NumUnitsInTick * (1U + ((UINT32) pFrameRate->Interlace)))*1000000U)/pFrameRate->TimeScale;
                        AmbaMonMain_FrameTimeTable[i] = FrameTimeInUs;
                        AmbaMonMain_FrameTimeInUs[i] = FrameTimeInUs;
                        AmbaMonPrint2(S_PRINT_FLAG_DBG, "frame time", i, (FrameTimeInUs + 500U)/1000U);
                        /* isr frame timeout */
                        TimeInUs = (FrameTimeInUs * AMBA_MON_ISR_TIMEOUT_PERCEND) / 100U;
                        if (TimeInUs < 2000U) {
                            TimeInUs = 2000U;
                        }
                        FrameTimeoutInUs = FrameTimeInUs + TimeInUs;
                        AmbaMonMain_FrameTimeoutInUs[i] = FrameTimeoutInUs;
                        AmbaMonPrint2(S_PRINT_FLAG_DBG, "frame timeout", i, (FrameTimeoutInUs + 500U)/1000U);
                        /* msg frame timeout */
                        MsgTimeInUs = (FrameTimeInUs * AMBA_MON_MSG_TIMEOUT_PERCEND) / 100U;
                        if (MsgTimeInUs < 2000U) {
                            MsgTimeInUs = 2000U;
                        }
                        MsgFrameTimeoutInUs = FrameTimeInUs + MsgTimeInUs;
                        AmbaMonMain_MsgFrameTimeoutInUs[i] = MsgFrameTimeoutInUs;
                        AmbaMonPrint2(S_PRINT_FLAG_DBG, "msg frame timeout", i, (MsgFrameTimeoutInUs + 500U)/1000U);
                        /* fov frame timeout */
                        FovTimeInUs = (FrameTimeInUs * AMBA_MON_FOV_TIMEOUT_PERCEND) / 100U;
                        if (FovTimeInUs < 2000U) {
                            FovTimeInUs = 2000U;
                        }
                        FovFrameTimeoutInUs = FrameTimeInUs + FovTimeInUs;
                        AmbaMonMain_FovFrameTimeoutInUs[i] = FovFrameTimeoutInUs;
                        AmbaMonPrint2(S_PRINT_FLAG_DBG, "fov frame timeout", i, (FovFrameTimeoutInUs + 500U)/1000U);
                    }
                }
                AmbaMonMain_VinSrcFlag[i] = 1U;
            } else if (AmbaMonMain_VinSrcTable[i] == AMBA_MON_VIN_SRC_YUV) {
                YuvChan.VinID = i;
                FuncRetCode = AmbaYuv_GetStatus(&YuvChan, &YuvStatus);
                if (FuncRetCode == YUV_ERR_NONE) {
                    pFrameRate = &(YuvStatus.ModeInfo.OutputInfo.FrameRate);
                    FrameTimeInUs = ((pFrameRate->NumUnitsInTick * (1U + ((UINT32) pFrameRate->Interlace)))*1000000U)/pFrameRate->TimeScale;
                    AmbaMonMain_FrameTimeTable[i] = FrameTimeInUs;
                    AmbaMonPrint2(S_PRINT_FLAG_DBG, "yuv frame time", i, (FrameTimeInUs + 500U)/1000U);
                    /* isr frame timeout */
                    TimeInUs = (FrameTimeInUs * AMBA_MON_ISR_TIMEOUT_PERCEND) / 100U;
                    if (TimeInUs < 2000U) {
                        TimeInUs = 2000U;
                    }
                    FrameTimeoutInUs = FrameTimeInUs + TimeInUs;
                    AmbaMonMain_FrameTimeoutInUs[i] = FrameTimeoutInUs;
                    AmbaMonPrint2(S_PRINT_FLAG_DBG, "yuv frame timeout", i, (FrameTimeoutInUs + 500U)/1000U);
                    /* msg frame timeout */
                    MsgTimeInUs = (FrameTimeInUs * AMBA_MON_MSG_TIMEOUT_PERCEND) / 100U;
                    if (MsgTimeInUs < 2000U) {
                        MsgTimeInUs = 2000U;
                    }
                    MsgFrameTimeoutInUs = FrameTimeInUs + MsgTimeInUs;
                    AmbaMonMain_MsgFrameTimeoutInUs[i] = MsgFrameTimeoutInUs;
                    AmbaMonPrint2(S_PRINT_FLAG_DBG, "yuv msg frame timeout", i, (MsgFrameTimeoutInUs + 500U)/1000U);
                    /* fov frame timeout */
                    FovTimeInUs = (FrameTimeInUs * AMBA_MON_FOV_TIMEOUT_PERCEND) / 100U;
                    if (FovTimeInUs < 2000U) {
                        FovTimeInUs = 2000U;
                    }
                    FovFrameTimeoutInUs = FrameTimeInUs + FovTimeInUs;
                    AmbaMonMain_FovFrameTimeoutInUs[i] = FovFrameTimeoutInUs;
                    AmbaMonPrint2(S_PRINT_FLAG_DBG, "fov frame timeout", i, (FovFrameTimeoutInUs + 500U)/1000U);
                }
                AmbaMonMain_VinSrcFlag[i] = 1U;
            } else {
                /* */
            }
        }
    }
}

/**
 *  Amba monitor main graph create
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_GraphCreate(void)
{
    static const char *pDspFovLatencyName = "fov_latency";
    static const char *pDspVoutLatencyName = "vout_latency";

    UINT32 FuncRetCode;
    UINT32 i;

    /* vin sof/eof isr port open */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(VinSofMsgPort[i]), "vin_msg", pVinSofName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin sof msg port open", i);
            }
            FuncRetCode = AmbaMonMessage_Open(&(VinEofMsgPort[i]), "vin_msg", pVinEofName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin eof msg port open", i);
            }
        }
    }

    /* dsp raw port open */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspRawMsgPort[i]), "dsp_msg", pDspRawName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp raw msg port open", i);
            }
        }
    }
    /* dsp yuv port open */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspYuvMsgPort[i]), "dsp_msg", pDspYuvName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv msg port open", i);
            }
        }
    }
    /* dsp yuv alt port open */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspYuvAltMsgPort[i]), "dsp_msg", pDspYuvAltName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv alt msg port open", i);
            }
        }
    }
    /* dsp disp yuv port open */
    for (i = 0U; i < AMBA_MON_NUM_DISP_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspDispMsgPort[i]), "dsp_msg", pDspDispName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp disp msg port open", i);
            }
        }
    }
    /* dsp vout yuv port open */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspVoutMsgPort[i]), "dsp_msg", pDspVoutName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout msg port open", i);
            }
        }
    }
    /* dsp def raw port open */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspDefRawMsgPort[i]), "dsp_msg", pDspDefRawName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def raw msg port open", i);
            }
        }
    }
    /* dsp def yuv alt port open */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspDefYuvAltMsgPort[i]), "dsp_msg", pDspDefYuvAltName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def yuv alt msg port open", i);
            }
        }
    }
    /* dsp cfa aaa port open */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspCfaAaaMsgPort[i]), "dsp_msg", pDspCfaAaaName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp cfa aaa msg port open", i);
            }
        }
    }
    /* dsp rgb aaa port open */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspRgbAaaMsgPort[i]), "dsp_msg", pDspRgbAaaName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp rgb aaa msg port open", i);
            }
        }
    }
    /* dsp fov latency port open */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspFovLatencyMsgPort[i]), "dsp_msg", pDspFovLatencyName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: fov latency msg port open", i);
            }
        }
    }
    /* dsp vout latency port open */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspVoutLatencyMsgPort[i]), "dsp_msg", pDspVoutLatencyName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout latency msg port open", i);
            }
        }
    }
    /* dsp vin timeout port open */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(DspVinTimeoutMsgPort[i]), "dsp_msg", pDspVinTimeoutName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vin timeout port open", i);
            }
        }
    }

    /* vout isr port open */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Open(&(VoutIsrMsgPort[i]), "vout_msg", pVoutName);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout isr msg port open", i);
            }
        }
    }

    /* timeline input port open */
    FuncRetCode = AmbaMonListen_Open(&TimelinePort, "timeline");
    if (FuncRetCode == OK_UL) {
        /* vin sof/eof msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(VinSofMsgPort[i]), pVinSofName, &(TimelineVinSofCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin sof msg connect to timeline", i);
                }
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(VinEofMsgPort[i]), pVinEofName, &(TimelineVinEofCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin eof msg connect to timeline", i);
                }
            }
        }

        /* dsp raw msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspRawMsgPort[i]), pDspRawName, &(TimelineDspRawCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp raw msg connect to timeline", i);
                }
            }
        }
        /* dsp yuv msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspYuvMsgPort[i]), pDspYuvName, &(TimelineDspYuvCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv msg connect to timeline", i);
                }
            }
        }
        /* dsp yuv alt msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspYuvAltMsgPort[i]), pDspYuvAltName, &(TimelineDspYuvAltCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv alt msg connect to timeline", i);
                }
            }
        }
        /* dsp disp msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspDispMsgPort[i]), pDspDispName, &(TimelineDspDispCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp disp msg connect to timeline", i);
                }
            }
        }
        /* dsp vout msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspVoutMsgPort[i]), pDspVoutName, &(TimelineDspVoutCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout msg connect to timeline", i);
                }
            }
        }
        /* dsp def raw msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspDefRawMsgPort[i]), pDspDefRawName, &(TimelineDspDefRawCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def raw msg connect to timeline", i);
                }
            }
        }
        /* dsp def yuv alt msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspDefYuvAltMsgPort[i]), pDspDefYuvAltName, &(TimelineDspDefYuvAltCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def yuv alt msg connect to timeline", i);
                }
            }
        }
        /* dsp cfa aaa msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspCfaAaaMsgPort[i]), pDspCfaAaaName, &(TimelineDspCfaAaaCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp cfa aaa msg connect to timeline", i);
                }
            }
        }
        /* dsp rgb aaa msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspRgbAaaMsgPort[i]), pDspRgbAaaName, &(TimelineDspRgbAaaCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp rgb aaa msg connect to timeline", i);
                }
            }
        }
        /* dsp fov latency msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspFovLatencyMsgPort[i]), pDspFovLatencyName, &(TimelineDspFovLatencyCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp fov latency msg connect to timeline", i);
                }
            }
        }
        /* dsp vout latency msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspVoutLatencyMsgPort[i]), pDspVoutLatencyName, &(TimelineDspVoutLatencyCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout latency msg connect to timeline", i);
                }
            }
        }
        /* dsp vin timeout msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(DspVinTimeoutMsgPort[i]), pDspVinTimeoutName, &(TimelineDspVinTimeoutCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vin timeout msg connect to timeline", i);
                }
            }
        }

        /* vout isr msg connect to timeline */
        for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
            if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                FuncRetCode = AmbaMonListen_Attach(&TimelinePort, &(VoutIsrMsgPort[i]), pVoutName, &(TimelineVoutIsrCbId[i]), AmbaMonListen_CbEntry);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout isr msg connect to timeline", i);
                }
            }
        }
    }

    /* echo port in timeline */
    FuncRetCode = AmbaMonMessage_Open(&AmbaMonMain_EchoPort, "timeline", "echo");
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: main echo port close", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor main graph delete
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_GraphDelete(void)
{
    UINT32 FuncRetCode;
    UINT32 i;

    /* vin sof/eof msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineVinSofCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin sof msg disconnect to timeline", i);
            }
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineVinEofCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin eof msg disconnect to timeline", i);
            }
        }
    }

    /* dsp raw msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspRawCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp raw msg disconnect to timeline", i);
            }
        }
    }
    /* dsp yuv msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspYuvCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv msg disconnect to timeline", i);
            }
        }
    }
    /* dsp yuv alt msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspYuvAltCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv alt msg disconnect to timeline", i);
            }
        }
    }
    /* dsp disp msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspDispCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp disp msg disconnect to timeline", i);
            }
        }
    }
    /* dsp vout msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspVoutCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout msg disconnect to timeline", i);
            }
        }
    }
    /* dsp def raw msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspDefRawCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def raw msg disconnect to timeline", i);
            }
        }
    }
    /* dsp def yuv alt msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspDefYuvAltCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def yuv alt msg disconnect to timeline", i);
            }
        }
    }
    /* dsp cfa aaa msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspCfaAaaCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp cfa aaa msg disconnect to timeline", i);
            }
        }
    }
    /* dsp rgb aaa msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspRgbAaaCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp rgb aaa msg disconnect to timeline", i);
            }
        }
    }
    /* dsp fov latency msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspFovLatencyCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp fov latency msg disconnect to timeline", i);
            }
        }
    }
    /* dsp vout latency msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspVoutLatencyCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout latency msg disconnect to timeline", i);
            }
        }
    }
    /* dsp vin timeout msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineDspVinTimeoutCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vin timeout msg disconnect to timeline", i);
            }
        }
    }

    /* vout isr msg disconnect to timeline */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonListen_Remove(&TimelinePort, &(TimelineVoutIsrCbId[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout isr msg disconnect to timeline", i);
            }
        }
    }

    /* timeline input port close */
    FuncRetCode = AmbaMonListen_Close(&TimelinePort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_ERR, "error: timeline port close", i);
    }

    /* vin sof/eof isr port close */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(VinSofMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin sof msg port close", i);
            }
            FuncRetCode = AmbaMonMessage_Close(&(VinEofMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin eof msg port close", i);
            }
        }
    }

    /* dsp raw port close */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspRawMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp raw msg port close", i);
            }
        }
    }
    /* dsp yuv port close */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspYuvMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv msg port close", i);
            }
        }
    }
    /* dsp yuv alt port close */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspYuvAltMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp yuv alt msg port close", i);
            }
        }
    }
    /* dsp disp yuv port close */
    for (i = 0U; i < AMBA_MON_NUM_DISP_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspDispMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp disp msg port close", i);
            }
        }
    }
    /* dsp vout yuv port close */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspVoutMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout msg port close", i);
            }
        }
    }
    /* dsp def raw port close */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspDefRawMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def raw msg port open", i);
            }
        }
    }
    /* dsp def yuv alt port close */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspDefYuvAltMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp def yuv alt msg port close", i);
            }
        }
    }
    /* dsp cfa aaa port close */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspCfaAaaMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp cfa aaa msg port close", i);
            }
        }
    }
    /* dsp rgb aaa port close */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspRgbAaaMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp rgb aaa msg port close", i);
            }
        }
    }
    /* dsp fov latency port close */
    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Fov.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspFovLatencyMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: fov latency msg port close", i);
            }
        }
    }
    /* dsp vout latency port close */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspVoutLatencyMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout latency msg port close", i);
            }
        }
    }
    /* dsp vin timeout port close */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(DspVinTimeoutMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vin timeout port close", i);
            }
        }
    }

    /* vout isr port close */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FuncRetCode = AmbaMonMessage_Close(&(VoutIsrMsgPort[i]));
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout isr msg port close", i);
            }
        }
    }

    /* echo port in timeline */
    FuncRetCode = AmbaMonMessage_Close(&AmbaMonMain_EchoPort);
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: main echo port close", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor main framework dispatch
 *  @param[in] CmdMsg command and message
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_FrameworkDispatch(AMBA_MON_MAIN_CMD_MSG_s CmdMsg)
{
    UINT32 FuncRetCode;
    UINT32 i;

    switch ((UINT8) CmdMsg.Ctx.Bits.Cmd) {
        case (UINT8) AMBA_MON_CMD_VIN_SRC:
            /* debug msg */
            AmbaMonPrint2(S_PRINT_FLAG_DBG, "mon-cmd-vin-src", (UINT32) CmdMsg.Ctx.VinSrc.VinIdx, (UINT32) CmdMsg.Ctx.Bits.Cmd);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "id", (UINT32) CmdMsg.Ctx.VinSrc.Id, 10U);
            /* vin src get */
            AmbaMonMain_VinSrcTable[CmdMsg.Ctx.VinSrc.VinIdx] = CmdMsg.Ctx.VinSrc.Id;
            break;
        case (UINT8) AMBA_MON_CMD_VIN:
            /* debug msg */
            AmbaMonPrint2(S_PRINT_FLAG_DBG, "mon-cmd-vin", (UINT32) CmdMsg.Ctx.Vin.VinIdx, (UINT32) CmdMsg.Ctx.Bits.Cmd);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "sensor select bits", (UINT32) CmdMsg.Ctx.Vin.SensorId, 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "serdes select bits", (UINT32) CmdMsg.Ctx.Vin.SerdesId, 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "fov (vin) select bits", (UINT32) CmdMsg.Ctx.Vin.FovId, 16U);
            /* sensor info get */
            AmbaMonMain_Info.Sensor[CmdMsg.Ctx.Vin.VinIdx].Num = 0U;
            AmbaMonMain_Info.Sensor[CmdMsg.Ctx.Vin.VinIdx].SelectBits = CmdMsg.Ctx.Vin.SensorId;
            for (i = 0U; i < AMBA_MON_NUM_VIN_SENSOR; i++) {
                if ((((UINT32) CmdMsg.Ctx.Vin.SensorId) & (((UINT32) 1U) << i)) > 0U) {
                    AmbaMonMain_Info.Sensor[CmdMsg.Ctx.Vin.VinIdx].Num++;
                }
            }
            /* serdes info get */
            AmbaMonMain_Info.Serdes[CmdMsg.Ctx.Vin.VinIdx].Num = 0U;
            AmbaMonMain_Info.Serdes[CmdMsg.Ctx.Vin.VinIdx].SelectBits = CmdMsg.Ctx.Vin.SerdesId;
            for (i = 0U; i < AMBA_MON_NUM_VIN_SENSOR; i++) {
                if ((((UINT32) CmdMsg.Ctx.Vin.SerdesId) & (((UINT32) 1U) << i)) > 0U) {
                    AmbaMonMain_Info.Serdes[CmdMsg.Ctx.Vin.VinIdx].Num++;
                }
            }
            /* fov vin info get */
            AmbaMonMain_Info.FovInVin[CmdMsg.Ctx.Vin.VinIdx].Num = 0U;
            AmbaMonMain_Info.FovInVin[CmdMsg.Ctx.Vin.VinIdx].SelectBits = CmdMsg.Ctx.Vin.FovId;
            for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                if ((((UINT32) CmdMsg.Ctx.Vin.FovId) & (((UINT32) 1U) << i)) > 0U) {
                    AmbaMonMain_Info.FovInVin[CmdMsg.Ctx.Vin.VinIdx].Num++;
                }
            }
            /* debug msg */
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "sensor num", AmbaMonMain_Info.Sensor[CmdMsg.Ctx.Vin.VinIdx].Num, 10U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "sensor select bits", AmbaMonMain_Info.Sensor[CmdMsg.Ctx.Vin.VinIdx].SelectBits, 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "serdes ch num", AmbaMonMain_Info.Serdes[CmdMsg.Ctx.Vin.VinIdx].Num, 10U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "serdes ch select bits", AmbaMonMain_Info.Serdes[CmdMsg.Ctx.Vin.VinIdx].SelectBits, 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "fov (vin) num", AmbaMonMain_Info.FovInVin[CmdMsg.Ctx.Vin.VinIdx].Num, 10U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "fov (vin) select bits", AmbaMonMain_Info.FovInVin[CmdMsg.Ctx.Vin.VinIdx].SelectBits, 16U);
            break;
        case (UINT8) AMBA_MON_CMD_SENSOR:
            /* debug msg */
            AmbaMonPrint2(S_PRINT_FLAG_DBG, "mon-cmd-sensor", (UINT32) CmdMsg.Ctx.Sensor.VinIdx, (UINT32) CmdMsg.Ctx.Bits.Cmd);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "sensor idx", (UINT32) CmdMsg.Ctx.Sensor.SensorIdx, 10U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "sensor id", (UINT32) CmdMsg.Ctx.Sensor.Id, 16U);
            /* sensor id get */
            AmbaMonMain_SensorIdTable[CmdMsg.Ctx.Sensor.VinIdx][CmdMsg.Ctx.Sensor.SensorIdx] = CmdMsg.Ctx.Sensor.Id;
            break;
        case (UINT8) AMBA_MON_CMD_SLOW_SHUTTER:
            /* slow shutter index get */
            AmbaMonPrint2(S_PRINT_FLAG_DBG, "mon-cmd-slow-shutter", (UINT32) CmdMsg.Ctx.SlowShutter.VinIdx, (UINT32) CmdMsg.Ctx.Bits.Cmd);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "slow shutter id", (UINT32) CmdMsg.Ctx.SlowShutter.Id, 16U);
            AmbaMonMain_SlowShutterTable[CmdMsg.Ctx.SlowShutter.VinIdx] = ((UINT32) CmdMsg.Ctx.SlowShutter.Id) & 0xFU;
            AmbaMonMain_FrameLineFactor[CmdMsg.Ctx.SlowShutter.VinIdx] =  (((UINT32) CmdMsg.Ctx.SlowShutter.Id) & 0xF0U) >> 4U;
            break;
        case (UINT8) AMBA_MON_CMD_BOOT_LATENCY:
            /* debug msg */
            AmbaMonPrint2(S_PRINT_FLAG_DBG, "mon-cmd-boot-latency", (UINT32) CmdMsg.Ctx.BootLatency.VinIdx, (UINT32) CmdMsg.Ctx.Bits.Cmd);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "boot latency", (UINT32) CmdMsg.Ctx.BootLatency.Timetick, 10U);
            /* boot latency get */
            if (CmdMsg.Ctx.BootLatency.Timetick > AMBA_MON_MAIN_BOOT_LATENCY) {
                AmbaMonMain_BootLatency[CmdMsg.Ctx.BootLatency.VinIdx] = CmdMsg.Ctx.BootLatency.Timetick;
            } else {
                AmbaMonMain_BootLatency[CmdMsg.Ctx.BootLatency.VinIdx] = AMBA_MON_MAIN_BOOT_LATENCY;
                AmbaMonPrintEx(S_PRINT_FLAG_DBG, "boot latency min", AMBA_MON_MAIN_BOOT_LATENCY, 10U);
            }
            break;
        case (UINT8) AMBA_MON_CMD_START:
            /* info get */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "amba monitor info get");
            AmbaMonMain_InfoGet(CmdMsg);

            /* graph create */
            AmbaMonMain_GraphCreate();

            /* input message reset */
            FuncRetCode = AmbaMonMessage_Reset(&VinMsgId);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin message reset");
            }
            FuncRetCode = AmbaMonMessage_Reset(&DspMsgId);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp message reset");
            }
            FuncRetCode = AmbaMonMessage_Reset(&VoutMsgId);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout message reset");
            }
            /* listen timeline reset */
            FuncRetCode = AmbaMonListen_Reset(&TimelineId);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: listen timeline reset");
            }
            /* state message reset */
            FuncRetCode = AmbaMonMessage_Reset(&VinStateId);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin state reset");
            }
            FuncRetCode = AmbaMonMessage_Reset(&DspStateId);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp state reset");
            }
            FuncRetCode = AmbaMonMessage_Reset(&VoutStateId);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout state reset");
            }

            /* dsp raw cap seq reset */
            for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                AmbaMonDsp_RawSeqNum[i] = 0xFFFFFFFFU;
            }
            /* dsp fov cap/seq num reset */
            for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                AmbaMonDsp_CfaRawSeqNum[i] = 0xFFFFFFFFU;
                AmbaMonDsp_RgbRawSeqNum[i] = 0xFFFFFFFFU;
                AmbaMonDsp_YuvSeqNum[i] = 0xFFFFFFFFU;
                AmbaMonDsp_YuvAltSeqNum[i] = 0xFFFFFFFFU;
            }
            /* dsp disp seq num reset */
            for (i = 0U; i < AMBA_MON_NUM_DISP_CHANNEL; i++) {
                AmbaMonDsp_DispYuvSeqNum[i] = 0xFFFFFFFFU;
            }
            /* dsp vout base addr y reset */
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                AmbaMonDsp_VoutBaseAddrY[i] = 0UL;
            }
            break;
        case (UINT8) AMBA_MON_CMD_STOP:
            /* graph delete */
            AmbaMonMain_GraphDelete();
            break;
        default:
            /* */
            break;
    }
}

/**
 *  Amba monitor main frame time update
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonMain_FrameTimeUpdate(UINT32 VinId)
{
    static UINT32 AmbaMonMain_FrameLineChage[AMBA_MON_NUM_VIN_CHANNEL] = {0};
    UINT32 RetCode = NG_UL;
    UINT32 FuncRetCode;
    UINT32 i;

    AMBA_SENSOR_CHANNEL_s SensorChan;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    UINT32 FrameTimeInUs;
    UINT32 FrameLine;
    UINT32 FrameCnt;

    if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << (VinId & 0x1FU))) > 0U) {
        SensorChan.VinID = VinId;
        if (AmbaMonMain_VinSrcTable[VinId] == AMBA_MON_VIN_SRC_SENSOR) {
            for (i = 0U; i < AMBA_MON_NUM_VIN_SENSOR; i++) {
                if ((AmbaMonMain_Info.Sensor[VinId].SelectBits & (((UINT32) 1U) << i)) > 0U) {
                    AmbaMonMain_TimingMarkPut(VinId, "Ft_Upd");
                    SensorChan.SensorID = AmbaMonMain_SensorIdTable[VinId][i];
                    /* sensor status */
                    FuncRetCode = AmbaSensor_GetStatus(&SensorChan, &SensorStatus);
                    if (FuncRetCode == SENSOR_ERR_NONE) {
                        /* frame lenght line */
                        FrameLine = SensorStatus.ModeInfo.FrameLengthLines;
                        /* frame cnt */
                        if (AmbaMonMain_FrameLineFactor[VinId] <= 1U) {
                            /* usage for the signal-sensor-config of sensor driver */
                            FrameCnt = FrameLine / AmbaMonMain_FrameLineTable[VinId];
                        } else {
                            /* usage for the multi-sensor-config of sensor driver */
                            FrameCnt = FrameLine / (AmbaMonMain_FrameLineTable[VinId] * AmbaMonMain_FrameLineFactor[VinId]);
                        }
                        if (FrameCnt != AmbaMonMain_FrameCnt[VinId]) {
                            /* frame chg count */
                            AmbaMonMain_FrameLineChage[VinId]++;
                            /* frame svr check */
                            if ((AmbaMonMain_FrameLineChage[VinId] + 1U) >= AmbaMonMain_SvrDelayTable[VinId]) {
                                /* frame time update */
                                FrameTimeInUs = AmbaMonMain_FrameTimeTable[VinId]*FrameCnt;
                                AmbaMonMain_FrameTimeInUs[VinId] = FrameTimeInUs;
                                AmbaMonMain_FrameCnt[VinId] = FrameCnt;
                                /* debug msg */
                                AmbaMonPrintEx(S_PRINT_FLAG_DBG, "frame count", FrameCnt, 16U);
                                AmbaMonPrintEx(S_PRINT_FLAG_DBG, "frame period", FrameTimeInUs, 10U);
                                AmbaMonPrintEx2(S_PRINT_FLAG_DBG, "frame line", AmbaMonMain_FrameLineTable[VinId], 10U, FrameLine, 10U);
                                AmbaMonPrintEx(S_PRINT_FLAG_DBG, "frame line factor", AmbaMonMain_FrameLineFactor[VinId], 10U);
                            }
                        } else {
                            /* frame chg reset */
                            AmbaMonMain_FrameLineChage[VinId] = 0U;
                        }
                        RetCode = OK_UL;
                    }
                }
            }
        }
    }

    return RetCode;
}

/**
 *  Amba monitor main sof timeout update
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonMain_SofTimeoutUpdate(UINT32 VinId)
{
    static UINT32 AmbaMonMain_FrameCntInSof[AMBA_MON_NUM_VIN_CHANNEL] = {0};
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 TimeInUs;
    UINT32 FrameTimeoutInUs;

    AMBA_MON_MAIN_MEM_s MemInfo;

    if (AmbaMonMain_FrameCnt[VinId] != AmbaMonMain_FrameCntInSof[VinId]) {
        AmbaMonMain_TimingMarkPut(VinId, "Sof_Upd");
        /* isr frame timeout */
        TimeInUs = (AmbaMonMain_FrameTimeInUs[VinId] * AMBA_MON_ISR_TIMEOUT_PERCEND)/100U;
        if (TimeInUs < 2000U) {
            TimeInUs = 2000U;
        }
        FrameTimeoutInUs = AmbaMonMain_FrameTimeInUs[VinId] + TimeInUs;
        AmbaMonMain_FrameTimeoutInUs[VinId] = FrameTimeoutInUs;

        /* vin sof timeout */
        MemInfo.Ctx.pTimeoutCb = &(VinSofTimeoutCb[VinId]);
        FuncRetCode = AmbaMonMessage_TimeoutSet(&(VinSofMsgPort[VinId]), (FrameTimeoutInUs + 500U)/1000U, MemInfo.Ctx.pVoid);
        if (FuncRetCode == OK_UL) {
            FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VinSofMsgPort[VinId]), (FrameTimeoutInUs + 500U)/1000U);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin sof watchdog refresh", VinId);
            }
            /* debug msg */
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "sof watchdog refresh", (FrameTimeoutInUs + 500U)/1000U, 10U);
        }

        /* frame update */
        AmbaMonMain_FrameCntInSof[VinId] = AmbaMonMain_FrameCnt[VinId];
    }

    return RetCode;
}

/**
 *  Amba monitor main eof timeout update
 *  @param[in] VinId vin id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonMain_EofTimeoutUpdate(UINT32 VinId)
{
    static UINT32 AmbaMonMain_FrameCntInEof[AMBA_MON_NUM_VIN_CHANNEL] = {0};
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    UINT32 j;

    UINT32 TimeInUs;
    UINT32 FrameTimeoutInUs;

    UINT32 MsgTimeInUs;
    UINT32 MsgFrameTimeoutInUs;

    UINT32 FovTimeInUs;
    UINT32 FovFrameTimeoutInUs;

    UINT32 FovSelectBits;

    AMBA_MON_MAIN_MEM_s MemInfo;

    if (AmbaMonMain_FrameCnt[VinId] != AmbaMonMain_FrameCntInEof[VinId]) {
        AmbaMonMain_TimingMarkPut(VinId, "Eof_Upd");
        /* isr frame timeout */
        TimeInUs = (AmbaMonMain_FrameTimeInUs[VinId] * AMBA_MON_ISR_TIMEOUT_PERCEND)/100U;
        if (TimeInUs < 2000U) {
            TimeInUs = 2000U;
        }
        FrameTimeoutInUs = AmbaMonMain_FrameTimeInUs[VinId] + TimeInUs;
        AmbaMonMain_FrameTimeoutInUs[VinId] = FrameTimeoutInUs;
        /* msg frame timeout */
        MsgTimeInUs = (AmbaMonMain_FrameTimeInUs[VinId] * AMBA_MON_MSG_TIMEOUT_PERCEND)/100U;
        if (MsgTimeInUs < 2000U) {
            MsgTimeInUs = 2000U;
        }
        MsgFrameTimeoutInUs = AmbaMonMain_FrameTimeInUs[VinId] + MsgTimeInUs;
        AmbaMonMain_MsgFrameTimeoutInUs[VinId] = MsgFrameTimeoutInUs;
        /* fov frame timeout */
        FovTimeInUs = (AmbaMonMain_FrameTimeInUs[VinId] * AMBA_MON_FOV_TIMEOUT_PERCEND) / 100U;
        if (FovTimeInUs < 2000U) {
            FovTimeInUs = 2000U;
        }
        FovFrameTimeoutInUs = AmbaMonMain_FrameTimeInUs[VinId] + FovTimeInUs;
        AmbaMonMain_FovFrameTimeoutInUs[VinId] = FovFrameTimeoutInUs;

        /* vin eof timeout refrest */
        MemInfo.Ctx.pTimeoutCb = &(VinEofTimeoutCb[VinId]);
        FuncRetCode = AmbaMonMessage_TimeoutSet(&(VinEofMsgPort[VinId]), (FrameTimeoutInUs + 500U)/1000U, MemInfo.Ctx.pVoid);
        if (FuncRetCode == OK_UL) {
            FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VinEofMsgPort[VinId]), (FrameTimeoutInUs + 500U)/1000U);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin eof watchdog refresh", VinId);
            }
            /* debug msg */
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "eof watchdog refresh", (FrameTimeoutInUs + 500U)/1000U, 10U);
        }

        /* dsp raw timeout refrest */
        MemInfo.Ctx.pTimeoutCb = &(DspRawTimeoutCb[VinId]);
        FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspRawMsgPort[VinId]), (MsgFrameTimeoutInUs + 500U)/1000U, MemInfo.Ctx.pVoid);
        if (FuncRetCode == OK_UL) {
            FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspRawMsgPort[VinId]), (MsgFrameTimeoutInUs + 500U)/1000U);
            if (FuncRetCode != OK_UL) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp raw watchdog refresh", VinId);
            }
            /* debug msg */
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "raw watchdog refresh", (MsgFrameTimeoutInUs + 500U)/1000U, 10U);
        }

        /* dsp fov timeout refrest (ss & smc supported) */
        if ((AmbaMonMain_SlowShutterTable[VinId] & 0x4U) > 0U) {
            AmbaMonMain_TimingMarkPut(VinId, "Fov_Upd");
            FovSelectBits = AmbaMonMain_Info.FovInVin[VinId].SelectBits & AmbaMonMain_Info.Fov.SelectBits;
            for (j = 0U; j < AMBA_MON_NUM_FOV_CHANNEL; j++) {
                if ((FovSelectBits & (((UINT32) 1U) << j)) > 0U) {
                    MemInfo.Ctx.pTimeoutCb = &(DspYuvAltTimeoutCb[j]);
                    FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspYuvAltMsgPort[j]), (FovFrameTimeoutInUs + 500U)/1000U, MemInfo.Ctx.pVoid);
                    if (FuncRetCode == OK_UL) {
                        FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspYuvAltMsgPort[j]), (FovFrameTimeoutInUs + 500U)/1000U);
                        if (FuncRetCode != OK_UL) {
                            AmbaMonPrint2(S_PRINT_FLAG_ERR, "error: dsp fov watchdog refresh", VinId, j);
                        }
                        /* debug msg */
                        AmbaMonPrintEx2(S_PRINT_FLAG_DBG, "fov watchdog refresh", j, 10U, (FovFrameTimeoutInUs + 500U)/1000U, 10U);
                    }
                }
            }
        }

        /* frame cnt update */
        AmbaMonMain_FrameCntInEof[VinId] = AmbaMonMain_FrameCnt[VinId];
    }

    return RetCode;
}

/**
 *  Amba monitor main dsp interrupt callback
 *  @param[in] ActualFlag actual event flag
 *  @param[in] pInfo porinter to the dsp interrupt information
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_DspIntCb(UINT32 ActualFlag, const AMBA_DSP_INT_INFO_s *pInfo)
{
    UINT32 FuncRetCode;
    UINT32 i;

    (void) pInfo;

    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        /* eof timing */
        if ((ActualFlag & (((UINT32) 0x2U) << (i*2U))) > 0U) {
            /* vin src */
            if (AmbaMonMain_VinSrcTable[i] == AMBA_MON_VIN_SRC_SENSOR) {
                /* vin valid */
                if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                    /* vin slow shutter */
                    if (AmbaMonMain_SlowShutterTable[i] > 0U) {
                        /* watchdog update */
                        FuncRetCode = AmbaMonMain_EofTimeoutUpdate(i);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                }
            }
        }
        /* sof timing */
        if ((ActualFlag & (((UINT32) 0x1U) << (i*2U))) > 0U) {
            /* vin src */
            if (AmbaMonMain_VinSrcTable[i] == AMBA_MON_VIN_SRC_SENSOR) {
                /* vin valid */
                if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
                    /* vin slow shutter */
                    if (AmbaMonMain_SlowShutterTable[i] > 0U) {
                        /* frame time update */
                        FuncRetCode = AmbaMonMain_FrameTimeUpdate(i);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                        /* watchdog update */
                        FuncRetCode = AmbaMonMain_SofTimeoutUpdate(i);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                }
            }
        }
    }
}

/**
 *  @private
 *  Amba monitor main watchdog inactive
 *  @note this function is intended for internal use only
 */
void AmbaMonMain_WatchdogInactive(void)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 FovSelectBits;

    AMBA_MON_MAIN_MEM_s MemInfo;

    /* vin sof timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            MemInfo.Ctx.pTimeoutCb = &(VinSofTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(VinSofMsgPort[i]), 0U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VinSofMsgPort[i]), 0U);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin sof watchdog stop refresh", i);
                }
            }
        }
    }
    /* vin eof timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            MemInfo.Ctx.pTimeoutCb = &(VinEofTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(VinEofMsgPort[i]), 0U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VinEofMsgPort[i]), 0U);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin eof watchdog stop refresh", i);
                }
            }
        }
    }
    /* dsp raw timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            MemInfo.Ctx.pTimeoutCb = &(DspRawTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspRawMsgPort[i]), 0U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspRawMsgPort[i]), 0U);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp raw watchdog stop refresh", i);
                }
            }
        }
    }
    /* dsp fov timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            FovSelectBits = AmbaMonMain_Info.FovInVin[i].SelectBits & AmbaMonMain_Info.Fov.SelectBits;
            for (j = 0U; j < AMBA_MON_NUM_FOV_CHANNEL; j++) {
                if ((FovSelectBits & (((UINT32) 1U) << j)) > 0U) {
                    MemInfo.Ctx.pTimeoutCb = &(DspYuvAltTimeoutCb[j]);
                    FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspYuvAltMsgPort[j]), 0U, MemInfo.Ctx.pVoid);
                    if (FuncRetCode == OK_UL) {
                        FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspYuvAltMsgPort[j]), 0U);
                        if (FuncRetCode != OK_UL) {
                            AmbaMonPrint2(S_PRINT_FLAG_ERR, "error: dsp fov watchdog stop refresh", i, j);
                        }
                    }
                }
            }
        }
    }
    /* dsp vout timeout */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            MemInfo.Ctx.pTimeoutCb = &(DspVoutTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspVoutMsgPort[i]), 0U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspVoutMsgPort[i]), 0U);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout watchdog stop refresh", i);
                }
            }
        }
    }
    /* vout isr timeout */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            MemInfo.Ctx.pTimeoutCb = &(VoutIsrTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(VoutIsrMsgPort[i]), 0U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VoutIsrMsgPort[i]), 0U);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout isr watchdog stop refresh", i);
                }
            }
        }
    }
}

/**
 *  @private
 *  Amba monitor main watchdog active
 *  @note this function is intended for internal use only
 */
void AmbaMonMain_WatchdogActive(void)
{
    UINT32 FuncRetCode;
    UINT32 i, j;

    UINT32 FovSelectBits;

    AMBA_MON_MAIN_MEM_s MemInfo;

    /* vin sof timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            if (AmbaMonMain_VinSrcFlag[i] == 0U) {
                continue;
            }
            MemInfo.Ctx.pTimeoutCb = &(VinSofTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(VinSofMsgPort[i]), (AmbaMonMain_FrameTimeoutInUs[i] + 500U)/1000U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VinSofMsgPort[i]), AmbaMonMain_BootLatency[i]);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin sof watchdog first refresh", i);
                }
            }
        }
    }
    /* vin eof timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            if (AmbaMonMain_VinSrcFlag[i] == 0U) {
                continue;
            }
            MemInfo.Ctx.pTimeoutCb = &(VinEofTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(VinEofMsgPort[i]), (AmbaMonMain_FrameTimeoutInUs[i] + 500U)/1000U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VinEofMsgPort[i]), AmbaMonMain_BootLatency[i]);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vin eof watchdog first refresh", i);
                }
            }
        }
    }
    /* dsp raw timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            if (AmbaMonMain_VinSrcFlag[i] == 0U) {
                continue;
            }
            MemInfo.Ctx.pTimeoutCb = &(DspRawTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspRawMsgPort[i]), (AmbaMonMain_MsgFrameTimeoutInUs[i] + 500U)/1000U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspRawMsgPort[i]), AmbaMonMain_BootLatency[i]);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp raw watchdog first refresh", i);
                }
            }
        }
    }
    /* dsp fov timeout */
    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vin.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            if ((AmbaMonMain_Info.Td.SelectBits & (((UINT32) 1U) << i)) == 0U) {
                if (AmbaMonMain_VinSrcFlag[i] == 0U) {
                    continue;
                }
                FovSelectBits = AmbaMonMain_Info.FovInVin[i].SelectBits & AmbaMonMain_Info.Fov.SelectBits;
                for (j = 0U; j < AMBA_MON_NUM_FOV_CHANNEL; j++) {
                    if ((FovSelectBits & (((UINT32) 1U) << j)) > 0U) {
                        MemInfo.Ctx.pTimeoutCb = &(DspYuvAltTimeoutCb[j]);
                        FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspYuvAltMsgPort[j]), (AmbaMonMain_FovFrameTimeoutInUs[i] + 500U)/1000U, MemInfo.Ctx.pVoid);
                        if (FuncRetCode == OK_UL) {
                            FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspYuvAltMsgPort[j]), AmbaMonMain_BootLatency[i]);
                            if (FuncRetCode != OK_UL) {
                                AmbaMonPrint2(S_PRINT_FLAG_ERR, "error: dsp fov watchdog first refresh", i, j);
                            }
                        }
                    }
                }
            }
        }
    }
    /* dsp vout timeout */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            MemInfo.Ctx.pTimeoutCb = &(DspVoutTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(DspVoutMsgPort[i]), 50U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(DspVoutMsgPort[i]), AmbaMonMain_BootLatency[i]);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp vout watchdog first refresh", i);
                }
            }
        }
    }
    /* vout isr timeout */
    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
        if ((AmbaMonMain_Info.Vout.SelectBits & (((UINT32) 1U) << i)) > 0U) {
            MemInfo.Ctx.pTimeoutCb = &(VoutIsrTimeoutCb[i]);
            FuncRetCode = AmbaMonMessage_TimeoutSet(&(VoutIsrMsgPort[i]), 50U, MemInfo.Ctx.pVoid);
            if (FuncRetCode == OK_UL) {
                FuncRetCode = AmbaMonMessage_TimeoutRefresh(&(VoutIsrMsgPort[i]), AmbaMonMain_BootLatency[i]);
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout isr watchdog first refresh", i);
                }
            }
        }
    }
}

/**
 *  Amba monitor main timeout dispatch
 *  @param[in] CmdMsg command and message
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_TimeoutDispatch(AMBA_MON_MAIN_CMD_MSG_s CmdMsg)
{
    static AMBA_DSP_INT_PORT_s VinIntPort;
    UINT32 FuncRetCode;
    UINT32 i;

    switch ((UINT8) CmdMsg.Ctx.Bits.Cmd) {
        case (UINT8) AMBA_MON_CMD_START:
            /* timeout update cb */
            for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                if ((((UINT32) CmdMsg.Ctx.Liv.VinId) & (((UINT32) 1U) << i)) > 0U) {
                    if (AmbaMonMain_VinSrcTable[i] == AMBA_MON_VIN_SRC_SENSOR) {
                        if (AmbaMonMain_SlowShutterTable[i] > 0U) {
                            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "dsp interrupt callback");
                            FuncRetCode = AmbaDspInt_OpenEx(&VinIntPort, 3U, AmbaMonMain_DspIntCb);
                            if (FuncRetCode != OK_UL) {
                                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp interrupt port fail to open", i);
                            }
                            break;
                        }
                    }
                }
            }

            /* watchdog active */
            AmbaMonMain_WatchdogActive();
            break;
        case (UINT8) AMBA_MON_CMD_STOP:
            /* watchdog inactive */
            AmbaMonMain_WatchdogInactive();

            /* timeout update cb */
            for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                if ((((UINT32) CmdMsg.Ctx.Liv.VinId) & (((UINT32) 1U) << i)) > 0U) {
                    if (AmbaMonMain_VinSrcTable[i] == AMBA_MON_VIN_SRC_SENSOR) {
                        if (AmbaMonMain_SlowShutterTable[i] > 0U) {
                            FuncRetCode = AmbaDspInt_Close(&VinIntPort);
                            if (FuncRetCode != OK_UL) {
                                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp interrupt port fail to close", i);
                            }
                            break;
                        }
                    }
                }
            }
            break;
        default:
            /* */
            break;
    }
}

/**
 *  Amba monitor main task dispatch
 *  @param[in] CmdMsg command and message
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_TaskDispatch(AMBA_MON_MAIN_CMD_MSG_s CmdMsg)
{
    UINT32 FuncRetCode;

    switch ((UINT8) CmdMsg.Ctx.Bits.Cmd) {
        case (UINT8) AMBA_MON_CMD_START:
            /* listen task active */
            FuncRetCode = AmbaMonListenVin_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin listen task active");
            }
            FuncRetCode = AmbaMonListenDsp_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp listen task active");
            }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
            FuncRetCode = AmbaMonListenAaa_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: aaa listen task active");
            }
#endif
            if (CmdMsg.Ctx.Liv.VoutId != 0U) {
                FuncRetCode = AmbaMonListenVout_Active();
                if (FuncRetCode != OK_UL) {
                    AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout listen task active");
                }
            }
            FuncRetCode = AmbaMonListenSerdes_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: serdes listen task active");
            }
            /* state task active */
            FuncRetCode = AmbaMonStateVin_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin state task active");
            }
            FuncRetCode = AmbaMonStateDsp_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp state task active");
            }
            FuncRetCode = AmbaMonStateVout_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout state task active");
            }
            /* watchdog task active */
            FuncRetCode = AmbaMonWdog_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: watchdog task active");
            }
            /* sync task active */
            FuncRetCode = AmbaMonInt_Active();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: interrupt task active");
            }
            break;
        case (UINT8) AMBA_MON_CMD_STOP:
            /* listen task inactive */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vin listen task inactive...");
            FuncRetCode = AmbaMonListenVin_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin listen task inactive");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "dsp listen task inactive...");
            FuncRetCode = AmbaMonListenDsp_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp listen task inactive");
            }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "aaa listen task inactive...");
            FuncRetCode = AmbaMonListenAaa_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: aaa listen task inactive");
            }
#endif
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vout listen task inactive...");
            FuncRetCode = AmbaMonListenVout_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout listen task inactive");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "serdes listen task inactive...");
            FuncRetCode = AmbaMonListenSerdes_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: serdes listen task inactive");
            }
            {
                const void *pMem;
                AMBA_MON_MESSAGE_HEADER_s MsgHeader;

                /* echo msg */
                MsgHeader.Ctx.Data[0] = 0ULL;
                MsgHeader.Ctx.Data[1] = 0ULL;
                MsgHeader.Ctx.Data[2] = 0ULL;
                MsgHeader.Ctx.Com.Id = (UINT8) AMBA_MON_MSG_ID_ECHO;
                pMem = AmbaMonMessage_Put(&AmbaMonMain_EchoPort, &MsgHeader, (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s));
                if (pMem == NULL) {
                    /* */
                }
            }
            /* listen task idlewait */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vin listen task idle wait...");
            FuncRetCode = AmbaMonListenVin_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin listen task idle wait");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "dsp listen task idle wait...");
            FuncRetCode = AmbaMonListenDsp_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp listen task idle wait");
            }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "aaa listen task idle wait...");
            FuncRetCode = AmbaMonListenAaa_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: aaa listen task idle wait");
            }
#endif
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vout listen task idle wait...");
            FuncRetCode = AmbaMonListenVout_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout listen task idle wait");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "serdes listen task idle wait...");
            FuncRetCode = AmbaMonListenSerdes_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: serdes listen task idle wait");
            }
            /* state task inactive */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vin state task inactive...");
            FuncRetCode = AmbaMonStateVin_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin state task inactive");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "dsp state task inactive...");
            FuncRetCode = AmbaMonStateDsp_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp state task inactive");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vout state task inactive...");
            FuncRetCode = AmbaMonStateVout_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout state task inactive");
            }
            /* state task idlewait */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vin state task idle wait...");
            FuncRetCode = AmbaMonStateVin_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vin state task idle wait");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "dsp state task idle wait...");
            FuncRetCode = AmbaMonStateDsp_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: dsp state task idle wait");
            }
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "vout state task idle wait...");
            FuncRetCode = AmbaMonStateVout_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: vout state task idle wait");
            }
#if 0
            /* watchdog task inactive */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "wdog timeout task inactive...");
            FuncRetCode = AmbaMonWdog_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: watchdog task inactive");
            }
            /* watchdog task idlewait */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "wdog timeout task idle wait...");
            FuncRetCode = AmbaMonWdog_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: watchdog task idle wait");
            }
#endif
            /* sync task inactive */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "interrupt task inactive...");
            FuncRetCode = AmbaMonInt_Inactive();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: interrupt task inactive");
            }
            FuncRetCode = AmbaMonInt_Push();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: interrupt task push");
            }
            /* sync task idlewait */
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, "interrupt task idle wait...");
            FuncRetCode = AmbaMonInt_IdleWait();
            if (FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: interrupt task idle wait");
            }
            break;
        default:
            /* */
            break;
    }
}

/**
 *  Amba monitor main source dispatch
 *  @param[in] CmdMsg command and message
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_SrcDispatch(AMBA_MON_MAIN_CMD_MSG_s CmdMsg)
{
    UINT32 FuncRetCode;

    switch ((UINT8) CmdMsg.Ctx.Bits.Cmd) {
        case (UINT8) AMBA_MON_CMD_START:
            /* dsp raw event handler */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY , AmbaMonDsp_RawRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: raw event handler register", FuncRetCode);
            }
            /* dsp yuv event handler */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY , AmbaMonDsp_YuvRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: yuv event handler register", FuncRetCode);
            }
            /* dsp disp event handler */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY , AmbaMonDsp_DispYuvRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: disp yuv event handler register", FuncRetCode);
            }
            /* dsp vout event handler */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VOUT_DATA_RDY , AmbaMonDsp_VoutYuvRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout yuv event handler register", FuncRetCode);
            }
            /* dsp cfa aaa event handler */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_CFA_AAA_RDY, AmbaMonDsp_CfaAaaRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp cfa aaa event handler register", FuncRetCode);
            }
            /* dsp rgb aaa event handler */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_PG_AAA_RDY, AmbaMonDsp_RgbAaaRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp rgb aaa event handler register", FuncRetCode);
            }
            /* dsp video path event handler */
            FuncRetCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_PATH_STATUS, AmbaMonDsp_VinTimeout);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp video path event handler register", FuncRetCode);
            }
            break;
        case (UINT8) AMBA_MON_CMD_STOP:
            /* dsp raw event handler */
            FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY , AmbaMonDsp_RawRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: raw event handler unregister", FuncRetCode);
            }
            /* dsp yuv event handler */
            FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY , AmbaMonDsp_YuvRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: yuv event handler unregister", FuncRetCode);
            }
            /* dsp disp event handler */
            FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY , AmbaMonDsp_DispYuvRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: disp yuv event handler unregister", FuncRetCode);
            }
            /* dsp vout event handler */
            FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VOUT_DATA_RDY , AmbaMonDsp_VoutYuvRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: vout yuv event handler unregister", FuncRetCode);
            }
            /* dsp cfa aaa event handler */
            FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_CFA_AAA_RDY, AmbaMonDsp_CfaAaaRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp cfa aaa event handler unregister", FuncRetCode);
            }
            /* dsp rgb aaa event handler */
            FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_PG_AAA_RDY, AmbaMonDsp_RgbAaaRdy);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp rgb aaa event handler unregister", FuncRetCode);
            }
            /* dsp video path event handler */
            FuncRetCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_PATH_STATUS, AmbaMonDsp_VinTimeout);
            if (FuncRetCode != OK) {
                AmbaMonPrint(S_PRINT_FLAG_ERR, "error: dsp video path event handler unregister", FuncRetCode);
            }
            break;
        default:
            /* */
            break;
    }
}

/**
 *  Amba monitor main dispatch
 *  @param[in] CmdMsg command and message
 *  @note this function is intended for internal use only
 */
static void AmbaMonMain_Dispatch(AMBA_MON_MAIN_CMD_MSG_s CmdMsg)
{
    if (CmdMsg.Ctx.Bits.Cmd == (UINT8) AMBA_MON_CMD_STOP) { /* TBD */
        /* timeout dispatch */
        AmbaMonMain_TimeoutDispatch(CmdMsg);
        /* task dispatch */
        AmbaMonMain_TaskDispatch(CmdMsg);
        /* src dispatch */
        AmbaMonMain_SrcDispatch(CmdMsg);
        /* framework dispatch */
        AmbaMonMain_FrameworkDispatch(CmdMsg);
    } else {
        /* framework dispatch */
        AmbaMonMain_FrameworkDispatch(CmdMsg);
        /* timeout dispatch */
        AmbaMonMain_TimeoutDispatch(CmdMsg);
        /* task dispatch */
        AmbaMonMain_TaskDispatch(CmdMsg);
        /* src dispatch */
        AmbaMonMain_SrcDispatch(CmdMsg);
    }
}

/**
 *  Amba monitor main config
 *  @param[in] Param command and message
 */
void AmbaMonMain_Config(UINT64 Param)
{
    AMBA_MON_MAIN_CMD_MSG_s CmdMsg;

    CmdMsg.Ctx.Data = Param;
    AmbaMonMain_Dispatch(CmdMsg);
}

/**
 *  @private
 *  Amba monitor main timing mark put ex
 *  @param[in] VinId vin id
 *  @param[in] pName pointer to the string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonMain_TimingMarkPutEx(UINT32 VinId, const char *pName)
{
#ifdef CONFIG_BUILD_IMGFRW_AAA
    return AmbaImgMain_TimingMarkPut(VinId, pName);
#else
    (void) VinId;
    (void) pName;
    return OK_UL;
#endif
}

/**
 *  @private
 *  Amba monitor main timing mark put
 *  @param[in] VinId vin id
 *  @param[in] pName pointer to the string
 *  @note this function is intended for internal use only
 */
void AmbaMonMain_TimingMarkPut(UINT32 VinId, const char *pName)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonMain_TimingMarkPutEx(VinId, pName);
    if (FuncRetCode != OK_UL) {
        /* */
    }
}

/**
 *  @private
 *  Amba monitor main timing mark put ex fov
 *  @param[in] FovId fov id
 *  @param[in] pName pointer to the string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonMain_TimingMarkPutExFov(UINT32 FovId, const char *pName)
{
#ifdef CONFIG_BUILD_IMGFRW_AAA
    return AmbaImgMain_TimingMarkPutByFov(FovId, pName);
#else
    (void) FovId;
    (void) pName;
    return OK_UL;
#endif
}

/**
 *  @private
 *  Amba monitor main timing mark put by fov
 *  @param[in] FovId fov id
 *  @param[in] pName pointer to the string
 *  @note this function is intended for internal use only
 */
void AmbaMonMain_TimingMarkPutByFov(UINT32 FovId, const char *pName)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonMain_TimingMarkPutExFov(FovId, pName);
    if (FuncRetCode != OK_UL) {
        /* */
    }
}

/**
 *  @private
 *  Amba monitor main timing mark put ex vout
 *  @param[in] VoutId vout id
 *  @param[in] pName pointer to the string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonMain_TimingMarkPutExVout(UINT32 VoutId, const char *pName)
{
#ifdef CONFIG_BUILD_IMGFRW_AAA
    return AmbaImgMain_TimingMarkPutByVout(VoutId, pName);
#else
    (void) VoutId;
    (void) pName;
    return OK_UL;
#endif
}

/**
 *  @private
 *  Amba monitor main timing mark put by vout
 *  @param[in] VoutId vout id
 *  @param[in] pName pointer to the string
 *  @note this function is intended for internal use only
 */
void AmbaMonMain_TimingMarkPutByVout(UINT32 VoutId, const char *pName)
{
    UINT32 FuncRetCode;
    FuncRetCode = AmbaMonMain_TimingMarkPutExVout(VoutId, pName);
    if (FuncRetCode != OK_UL) {
        /* */
    }
}
