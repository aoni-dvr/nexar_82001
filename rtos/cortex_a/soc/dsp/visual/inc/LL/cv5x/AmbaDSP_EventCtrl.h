/**
 *  @file AmbaDSP_EventCtrl.h
 *
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
 *  @details Definitions & constants for Ambarella DSP internal vvent handler APIs
 *
 */

#ifndef DSP_EVENT_H
#define DSP_EVENT_H

#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_CommonAPI.h"
#include "dsp_osal.h"

#define WAIT_TIMEOUT 0U

#define LL_EVENT_LV_RAW_RDY                     (0U)
#define LL_EVENT_LV_CFA_AAA_RDY                 (1U)
#define LL_EVENT_LV_PG_AAA_RDY                  (2U)
#define LL_EVENT_LV_HIST_AAA_RDY                (3U)
#define LL_EVENT_LV_YUV_DATA_RDY                (4U)
#define LL_EVENT_LV_PYRAMID_RDY                 (5U)
#define LL_EVENT_VOUT_DATA_RDY                  (6U)
#define LL_EVENT_VIDEO_DATA_RDY                 (7U)
#define LL_EVENT_VIDEO_ENC_START                (8U)
#define LL_EVENT_VIDEO_ENC_STOP                 (9U)
#define LL_EVENT_STL_RAW_RDY                    (10U)
#define LL_EVENT_STL_CFA_AAA_RDY                (11U)
#define LL_EVENT_STL_PG_AAA_RDY                 (12U)
#define LL_EVENT_STL_RAW_POSTPROC_RDY           (13U)
#define LL_EVENT_STL_YUV_DATA_RDY               (14U)
#define LL_EVENT_JPEG_DATA_RDY                  (15U)
#define LL_EVENT_VIDEO_PATH_STATUS              (16U)
#define LL_EVENT_VIDEO_DEC_STATUS               (17U)
#define LL_EVENT_VIDEO_DEC_YUV_DISP_REPORT      (18U)
#define LL_EVENT_VIDEO_DEC_PIC_INFO             (19U)
#define LL_EVENT_STILL_DEC_STATUS               (20U)
#define LL_EVENT_STILL_DEC_YUV_DISP_REPORT      (21U)
#define LL_EVENT_VIN_POST_CONFIG                (22U)
#define LL_EVENT_ERROR                          (23U)
#define LL_EVENT_LV_SYNC_JOB_RDY                (24U)
#define LL_EVENT_VIDEO_MV_DATA_RDY              (25U)
#define LL_EVENT_LV_SIDEBAND_RDY                (26U)
#define LL_EVENT_LV_LNDT_RDY                    (27U)
#define LL_EVENT_LV_MAIN_Y12_RDY                (28U)
#define LL_EVENT_LV_MAIN_IR_RDY                 (29U)
#define LL_EVENT_CLK_UPT_RDY                    (30U)
#define LL_EVENT_NUM_DSP                        (31U)

// Internal event, preproc only
#define LL_EVENT_VCAP_LOCK_VIDEO_BUF            (64U)
#define LL_EVENT_VCAP_SCALE_YUV                 (65U)
#define LL_EVENT_MEM_CREATE_FRM_BUF_POOL        (66U)
#define LL_EVENT_MEM_REQUEST_FRM_BUF            (67U)
#define LL_EVENT_YUV_SRC_BUF_INFO               (68U)
#define LL_EVENT_VIDEO_DATA_TRANSFER_READY      (69U)
#define LL_EVENT_DSP_ISR_CHECK                  (70U)
#define LL_EVENT_FIRST_VIDEO_DATA_READY         (71U)
#define LL_EVENT_VDO_PATH_STATUS_TRANSFER_READY (72U)
#define LL_EVENT_VIDEO_PTS_TRACE                (73U)
#define LL_EVENT_VIEWZONE_MAP_TO_VIN            (74U)
#define LL_EVENT_VPROC_EXT_DATA_READY           (75U)
#define LL_EVENT_VPROC_EXT_STATUS_READY         (76U)
#define LL_EVENT_CFA_MODE_MAPPING               (77U)
#define LL_EVENT_CFA_HDR_HIST_TRANSFER_READY    (78U)
#define LL_EVENT_VIN_MAIN_STAT_TRANSFER_READY   (79U)
#define LL_EVENT_VIN_HDR_STAT_TRANSFER_READY    (80U)
#define LL_EVENT_VIN_MAIN_STAT_READY            (81U)
#define LL_EVENT_VIN_HDR_STAT_READY             (82U)
#define LL_EVENT_VPROC_IDSP_DATA_RDY            (83U)
#define LL_EVENT_VIN_SECTION_CFG_DATA_RDY       (84U)
#define LL_EVENT_EFCT_YUV_DATA_RDY              (85U)
#define LL_EVENT_HIER_IDX_MAPPING               (86U)
#define LL_EVENT_BATCH_INFO_MAPPING             (87U)
#define LL_EVENT_ENC_GET_ACTIVE_BIT             (88U)
#define LL_EVENT_DEC_GET_INFO                   (89U)
#define LL_EVENT_DEC_UPDATE_STATUS              (90U)
#define LL_EVENT_SYS_GET_CLK_INFO               (91U)
#define LL_EVENT_SIM_OP                         (92U) //obsolete in CV5x
#define LL_EVENT_TIME_TICK_UPDATE               (93U)
#define LL_EVENT_ENC_GET_ENGID                  (94U)
#define LL_EVENT_NUM_ALL                        (95U)

/**
 * The types of event callback function
 */
#define LL_EVENT_HANDLER            (0x0U)
#define LL_EVENT_HANDLER_PRE_PROC   (0x1U)
#define LL_EVENT_HANDLER_POST_PROC  (0x2U)

typedef struct {
    UINT32                       MaxNumHandlers;     /* maximum number of Handlers */
    AMBA_DSP_EVENT_HANDLER_f    *pEventHandlers;    /* pointer to the Event Handlers */
} DSP_EVENT_HANDLER_s;

typedef struct {
    osal_mutex_t        Mtx;
    DSP_EVENT_HANDLER_s PreProc;
    DSP_EVENT_HANDLER_s Main;
    DSP_EVENT_HANDLER_s PostProc;
} DSP_EVENT_HANDLER_CTRL_s;

/* Capture information */
typedef struct {
    AMBA_DSP_RAW_DATA_RDY_EXT3_s    RawDataRdy;
    UINT32                          CapSeqU32;
    UINT32                          CapPtsU32;
    UINT16                          TargetVinId;    //use for TD
    UINT8                           CapSliceIdx;
    UINT8                           CapSliceNum;
    UINT8                           IsCompRaw;
    UINT32                          BatchCmdId;
} DSP_RAW_CAPTURED_BUF_s;

typedef struct {
    AMBA_DSP_STL_RAW_DATA_INFO_s    StlRawData;
    UINT16                          VinId;          /* Vin index, Bit[15] means VirtVin */
    UINT16                          VinVirtChId;    /* Vin virtual channel index, B[7:0]VirtChanIdx, B[15:8]TimeDivisionIdx */
    UINT8                           IsVirtChan;     /* Is virtual Channel, B[0]IsVirtChan, B[1]IsTimeDivision */
#define CAP_SRC_TYPE_VIN    (0U)
#define CAP_SRC_TYPE_VPROC  (1U)
    UINT8                           CapSrcType;
    UINT8                           Rsvd[2U];
} DSP_STL_RAW_CAP_BUF_s;

typedef struct {
    AMBA_DSP_VIDEO_PATH_INFO_s      VdoPath;
    UINT8                           CapSliceIdx;
    UINT8                           CapSliceNum;
} DSP_RAW_VDO_PATH_s;

#define DSP_VPROC_YUV_PREVC     (0U)
#define DSP_VPROC_YUV_PREVA     (1U)
#define DSP_VPROC_YUV_PREVB     (2U)
#define DSP_VPROC_YUV_MAIN      (3U)
#define DSP_VPROC_YUV_NUM       (4U)

typedef struct {
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s YuvData;
    UINT8                   YuvType;    /* DSP_VPROC_YUV_NUM */
    UINT32                  ChannelId;
    UINT8                   SrcBufID;
    UINT32                  YuvPtsU32;
    UINT32                  CapSeqU32;
    UINT32                  BatchCmdId; /* BatchCmd id */
    UINT32                  CapPtsU32;
    UINT32                  ExtBufMask;
    AMBA_DSP_BUF_s          Me1Buf;
} AMBA_DSP_YUV_INFO_s;

/* When we give DSP-event for main yuv and effect yuv,
 * we use YuvData.ViewZoneId as an indicator to inform Yuv PreProc */
#define VIEWZONE_ID_INDICATOR_MAIN      (0xFFFFFFFFU)
#define VIEWZONE_ID_INDICATOR_EFFECT    (0xFFFFFFFEU)

typedef struct {
    UINT32  VinIdx;
    UINT32  VprocChan;
} AMBA_DSP_VPROC_VIN_MAPPING_s;

typedef struct {
    UINT16 ChannelId;
    UINT16 CfaMode;
} AMBA_DSP_VPROC_CFA_MODE_MAPPING_s;

typedef struct {
    UINT16 ChannelId;
    UINT16 InputIdx;
    UINT16 OutputIdx; // 0xFFFF means invalid
    UINT16 rsvd;
} AMBA_DSP_VPROC_HIER_IDX_MAPPING_s;

/* Context for Event info pool */
#define DSP_EVENT_INFO_POOL_DEPTH       (256U)

#define EVENT_INFO_POOL_PROT_BUF        (0U)
#define EVENT_INFO_POOL_STAT_CFA        (1U)
#define EVENT_INFO_POOL_STAT_PG         (2U)
#define EVENT_INFO_POOL_STAT_HIST0      (3U)
#define EVENT_INFO_POOL_STAT_HIST1      (4U)
#define EVENT_INFO_POOL_STAT_HIST2      (5U)
#define EVENT_INFO_POOL_NUM             (6U)
typedef struct {
    DSP_POOL_DESC_s PoolDesc;
    UINT8 Used[DSP_EVENT_INFO_POOL_DEPTH]; // The event id, for release check
    osal_mutex_t Mtx;
} AMBA_DSP_EVENT_INFO_CTRL_s;

/* LL Event Info Structures */
typedef struct {
    UINT32 BatchCmdId;
    UINT32 IsoCfgId;
    UINT32 ImgPrmdId;
    UINT32 YuvStrmGrpId;
    UINT32 StlProcId;
} AMBA_LL_BATCH_IDX_INFO_MAPPING_s;

typedef struct {
    UINT16 ActiveBit;
} AMBA_LL_ENC_GET_ACTIVE_BIT_s;

typedef struct {
#define LL_DEC_INFO_BITS_FMT    (0U)
#define LL_DEC_INFO_GLOBAL_ID   (1U)
#define NUM_LL_DEC_INFO         (2U)
    UINT16 Type;
    UINT16 Id;
    UINT32 BitsFmt;
    UINT32 Value;
} AMBA_LL_DEC_GET_INFO_s;

typedef struct {
    UINT16 DecoderId;
    UINT32 Status;
} AMBA_LL_DEC_UPDATE_STATUS_s;

typedef struct {
#define LL_SYS_CLK_IDSP     (0U)
#define LL_SYS_CLK_CORE     (1U)
#define LL_SYS_CLK_DRAM     (2U)
#define LL_SYS_CLK_AUD      (3U)
#define LL_SYS_CLK_IDSPV    (4U)
#define NUM_LL_SYS_CLK      (5U)
    UINT32 Type;
    UINT32 Value;
} AMBA_LL_SYS_CLK_INFO_s;

typedef struct {
    UINT8 StreamId;
    UINT8 EngId;
} AMBA_LL_ENC_GET_ENGID_s;

typedef struct {
#define LL_SIM_OP_QUERY_ENABLE  (0U)
#define LL_SIM_OP_CRC_CALC      (1U)
    UINT32 Op;
    UINT32 Data[3];
    ULONG  Addr;
} AMBA_LL_SIM_OP_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_LL_EVENT_DSP_ERROR
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 ChannelID;
    UINT32 ErrorCode;
} AMBA_LL_DSP_ERROR_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_LL_EVENT_HDR_BLEND_INFO_READY
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT8  OutSelect;   // 0: before cfa prescaler, 1: after cfa prescaler
    UINT32 Width;
    UINT32 Height;
    UINT32 Pitch;
    UINT32 Addr;
} AMBA_LL_EVENT_HDR_BLEND_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
*  Event info structure for AMBA_LL_EVENT_ME_DATA_INFO_s
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    AMBA_DSP_YUV_INFO_s Main;
    AMBA_DSP_YUV_INFO_s ResamplerA;
    AMBA_DSP_YUV_INFO_s ResamplerB;
    AMBA_DSP_YUV_INFO_s ResamplerC;
} AMBA_LL_EVENT_ME_DATA_INFO_s;
/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for LL_EVENT_VIDEO_DATA_TRANSFER_READY
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT8  StreamId;
    UINT8  FrameType;
    UINT8  EnaSpsAndPPSReport;
    UINT8  IsLastPicInOneFrame;
    UINT8  IsFirstEncodeFrame;
    UINT64 CapturePTS;
    UINT64 EncodePTS;
} AMBA_LL_VIDEO_DATA_TRANSFER_INFO;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_LL_EVENT_JPEG_DEC_YUV_DISP_REPORT
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    struct {
        UINT32  VoutUpdated;
        UINT32  VoutYAddr;
        UINT32  VoutUVAddr;
        UINT16  VoutWidth;
        UINT16  VoutHeight;
        UINT16  VoutPitch;
    } DispInfo[2];
} AMBA_LL_DSP_JPEG_DEC_YUV_DISP_REPORT_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Event info structure for AMBA_LL_EVENT_VIDEO_DEC_PIC_INFO_s
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    AMBA_DSP_EVENT_VIDDEC_PIC_EX_s      PicInfo;
    UINT32                              BufferWidth;
    UINT32                              BufferHeight;
} AMBA_LL_EVENT_VIDEO_DEC_PIC_INFO_s;


/*-----------------------------------------------------------------------------------------------*\
 * Events handler APIs
\*-----------------------------------------------------------------------------------------------*/

/**
 * DSP event managing module initializations
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_EventInit(void);

/**
 * Configure the Event Handler control
 * @param [in] EventID Event ID
 * @param [in] Type Event type
 * @param [in] MaxNumHandlers Maximum number of Handlers
 * @param [in] pEventHandlers Pointer to the Event Handlers
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_EventHandlerConfig(UINT16 EventID, UINT8 Type, UINT32 MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers);

/**
 * Reset a specific event handler
 * @param [in] EventID Event ID
 * @param [in] Type Event type
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_EventHandlerReset(UINT16 EventID, UINT8 Type);

/**
 * Register an event handler
 * @param [in] EventID Event ID
 * @param [in] Type Event type
 * @param [in] EventHandler Pointer to the Event Handler
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_RegisterEventHandler(UINT16 EventID, UINT8 Type, AMBA_DSP_EVENT_HANDLER_f EventHandler);

/**
 * Un-register an event handler
 * @param [in] EventID Event ID
 * @param [in] Type Event type
 * @param [in] EventHandler Pointer to the Event Handler
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_UnRegisterEventHandler(UINT16 EventID, UINT8 Type, AMBA_DSP_EVENT_HANDLER_f EventHandler);

/**
 * Give a DSP event by calling back to event handler
 * @param [in] EventID Event ID
 * @param [in] pEventInfo Pointer to the Event Info
 * @param [in] EventBufIdx Index of event info buffer, Bit[15:12]=BufCategory, Bit[11:0]=PoolIdx
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_GiveEvent(UINT16 EventID, const void *pEventInfo, const UINT16 EventBufIdx);

/**
 * Request an event info buffer for DSP_GiveEvent input
 * @param [in] Category Category of event info buffer
 * @param [out] pULAddr Address of event info buffer
 * @param [out] pBufIdx Index of event info buffer, Bit[15:12]=BufCategory, Bit[11:0]=PoolIdx
 * @param [in] EventId Event id of event info buffer
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_RequestEventInfoBuf(const UINT8 Category, ULONG *pULAddr, UINT16 *pBufIdx, const UINT16 EventID);

/**
 * Release an event info buffer
 * @param [in] BufIdx Index of event info buffer, Bit[15:12]=BufCategory, Bit[11:0]=PoolIdx
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_ReleaseEventInfoBuf(const UINT16 BufIdx);

/**
 * Release all the event info buffers for the specified event id
 * @param [in] EventID Event ID
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_ResetEventInfoBuf(const UINT16 EventID);

/*-----------------------------------------------------------------------------------------------*\
 * Event flags control APIs
\*-----------------------------------------------------------------------------------------------*/

/*----------------------------*\
 * DSP_EVENT Category
\*----------------------------*/
/* Each event type occupies a osal_even_t */
#define DSP_EVENT_CAT_GEN      (0U)
#define DSP_EVENT_CAT_VIN      (1U)
#define DSP_EVENT_CAT_VPROC    (2U)
#define DSP_EVENT_CAT_VOUT     (3U)

/*----------------------------*\
 * DSP_EVENT Type
\*----------------------------*/
#define DSP_EVENT_TYPE_INT     (0U)
#define DSP_EVENT_TYPE_PROF    (1U)
#define DSP_EVENT_TYPE_PPVOUT  (2U)
#define DSP_EVENT_TYPE_ENC     (3U)
#define DSP_EVENT_TYPE_DEC     (4U)
#define DSP_EVENT_TYPE_MONITOR (5U)    /* 5 */
#define DSP_NUM_EVENT_TYPE     (6U)

/*----------------------------*\
 * DSP_EVENT Flag
\*----------------------------*/
#define EVENT_FLAG_INT_VDSP0_INT        (1UL )             /* VDSP0 Interrupt */
#define EVENT_FLAG_INT_VDSP1_INT        (1UL << 1UL)       /* VDSP1 Interrupt */
#define EVENT_FLAG_INT_VDSP2_INT        (1UL << 2UL)       /* VDSP2 Interrupt */
#define EVENT_FLAG_INT_VDSP3_INT        (1UL << 3UL)       /* VDSP3 Interrupt */
#define EVENT_FLAG_INT_VDSP4_INT        (1UL << 4UL)       /* VDSP4 Interrupt */
#define EVENT_FLAG_INT_CMD_ACK_ASYNC    (1UL << 5UL)       /* CMD ACK ASYNC Interrupt */

#define EVENT_FLAG_INT_VDSP0_EVENT      (1UL << 8UL)       /* VDSP0 event generated by Task */
#define EVENT_FLAG_INT_VDSP1_EVENT      (1UL << 9UL)       /* VDSP1 event generated by Task */

#define EVENT_FLAG_INT_VDSP0_TIMEOUT    (1UL << 11UL)      /* VDSP0 IRQ Timeout */
#define EVENT_FLAG_INT_VDSP1_TIMEOUT    (1UL << 12UL)      /* VDSP1 IRQ Timeout */

#define EVENT_FLAG_INT_VIN0_SOF         (1UL << 13UL)      /* VIN0 SOF Interrupt */
#define EVENT_FLAG_INT_VIN1_SOF         (1UL << 14UL)      /* VIN1 SOF Interrupt */
#define EVENT_FLAG_INT_VIN0_EOF         (1UL << 15UL)      /* VIN0 EOF Interrupt */
#define EVENT_FLAG_INT_VIN1_EOF         (1UL << 16UL)      /* VIN1 EOF Interrupt */
#define EVENT_FLAG_INT_VOUT0            (1UL << 17UL)      /* VOUT0 Interrupt */
#define EVENT_FLAG_INT_VOUT1            (1UL << 18UL)      /* VOUT1 Interrupt */

#define EVENT_FLAG_INT_VIN2_SOF         (1UL << 19UL)      /* VIN2 SOF Interrupt */
#define EVENT_FLAG_INT_VIN3_SOF         (1UL << 20UL)      /* VIN3 SOF Interrupt */
#define EVENT_FLAG_INT_VIN2_EOF         (1UL << 21UL)      /* VIN2 EOF Interrupt */
#define EVENT_FLAG_INT_VIN3_EOF         (1UL << 22UL)      /* VIN3 EOF Interrupt */

#define EVENT_FLAG_INT_VDSP2_EVENT      (1UL << 23UL)      /* VDSP2 event generated by Task */
#define EVENT_FLAG_INT_VDSP2_TIMEOUT    (1UL << 24UL)      /* VDSP2 IRQ Timeout */

#define EVENT_FLAG_INT_VIN4_SOF         (1UL << 25UL)      /* VIN4 SOF Interrupt */
#define EVENT_FLAG_INT_VIN4_EOF         (1UL << 26UL)      /* VIN4 EOF Interrupt */

#define EVENT_FLAG_INT_VDSP3_EVENT      (1UL << 27UL)      /* VDSP3 event generated by Task */
#define EVENT_FLAG_INT_VDSP3_TIMEOUT    (1UL << 28UL)      /* VDSP3 IRQ Timeout */

#ifdef SUPPORT_DSP_ASYNC_ENCMSG
#define EVENT_FLAG_INT_VDSP4_EVENT      (1UL << 29UL)      /* VDSP3 event generated by Task */
#define EVENT_FLAG_INT_VDSP4_TIMEOUT    (1UL << 30UL)      /* VDSP3 IRQ Timeout */
#endif

/*----------------------------*\
 * AMBA_DSP_EVENT_TYPE_STILL_DEC
\*----------------------------*/
// avoid define conflict flag between "DSP_STILL_DEC_StatePattern" and "AMBA_DSP_STILL_DEC_FLAG_e", which reference the same decode state "AMBA_DSP_DEC_STATE_e"
#define    AMBA_DSP_STILL_DEC_Y2Y_DONE              (1UL << (DEC_STATE_RUN + 1U))
#define    AMBA_DSP_STILL_DEC_BLEND_DONE            (1UL << (DEC_STATE_RUN + 2U))

/*----------------------------*\
 * DSP_EVENT_TYPE_MONITOR
\*----------------------------*/
#define EVENT_FLAG_MONITOR_VIN_FRAMERATE     (1UL )       /* Monitor VIN frame rate */

/**
 * Initialize event flag group used inside AmbaDSP
 * @return 0 - OK, others - ErrorCode
 */
//UINT32 DSP_InitEventFlag(void);

/**
 * Set DSP event flags
 * @param [in] Category category of event flag group
 * @param [in] Type Type of event flag group
 * @param [in] Flag event flags
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_SetEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag);

/**
 * Set input DSP event flags and clear the other event flags
 * @param [in] Category category of event flag group
 * @param [in] Type Type of event flag group
 * @param [in] EventFlag event flags
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_SetEventFlagExclusive(UINT16 Category, UINT16 Type, UINT32 Flag);

/**
 * Clear a specified event flag
 * @param [in] Category category of event flag group
 * @param [in] Type Type of event flag group
 * @param [in] Flag event flags
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_ClearEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag);

/**
 * Check a specified event flag
 * @param [in] Category category of event flag group
 * @param [in] Type Type of event flag group
 * @param [in] Flag event flags
 * @return 0 - OK, others - ErrorCode
 */
//UINT32 DSP_CheckEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag);

/**
 * Wait a specified event flag
 * @param [in] Category category of event flag group
 * @param [in] Type Type of event flag group
 * @param [in] Flag event flags
 * @param [in] All o/1, any or all
 * @param [in] Clear O/1, enable/disable auto clear
 * @param [out] pActualFlags, Current DSP event flags
 * @param [in] Timeout Wait option (timer-ticks)
 * @return 0 - OK, others - ErrorCode
 */
UINT32 DSP_WaitEventFlag(UINT16 Category, UINT16 Type, UINT32 Flag, UINT32 All, UINT32 Clear, UINT32 *pActualFlags, UINT32 Timeout);

/**
 * Reset all the event flags of a specified type
 * @param [in] Category category of event flag group
 * @param [in] Type Type of event flag group
 * @return 0 - OK, others - ErrorCode
 */
//UINT32 DSP_ResetAllEventFlags(UINT16 Category, UINT16 Type);

#endif
