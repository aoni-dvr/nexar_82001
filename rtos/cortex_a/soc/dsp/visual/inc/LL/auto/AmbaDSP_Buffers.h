/**
 *  @file AmbaDSP_Buffers.h
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
 *  @details Declarations of all DSP various non-cached buffers
 *
 */

#ifndef AMBA_DSP_BUFFERS_H
#define AMBA_DSP_BUFFERS_H

#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_IdspCfg_Def.h"
#include "AmbaDSP_Priv.h"
#include "AmbaDSP_EventCtrl.h"
#include "ambadsp_ioctl.h"

#define USE_STAT_IDSP_BUF

//#ifndef NULL
//#define NULL    ((void*)0)
//#endif

#define AMBA_MAX_NUM_DSP_GENERAL_CMD_BUF    (3U)       /* maximum number of Command Buffer */
#define AMBA_MAX_NUM_DSP_VCAP_CMD_BUF       (3U)       /* maximum number of Command Buffer */

/*---------------------------------------------------------------------------*\
 * Defined here and let ucode knows these from DSP init data setting
\*---------------------------------------------------------------------------*/
#define DSP_MAX_NUM_GENERAL_MSG             (64U)      /* maximum number of messages per Buffer */
#define DSP_MAX_NUM_VCAP_MSG                (64U)      /* maximum number of messages per Buffer */
#define DSP_MAX_NUM_VPROC_MSG               (64U)      /* maximum number of messages per Buffer */
#define DSP_MAX_NUM_VP_MSG                  (32U)      /* maximum number of messages per Buffer */
#ifdef SUPPORT_DSP_ASYNC_ENCMSG
#define DSP_MAX_NUM_ENC_MSG                 (128U)     /* maximum number of messages per Buffer */
#endif

/* ExtMem for raw encode relative */
#define MAX_EXT_MEM_BUF_NUM             (1U)
#define MAX_EXT_MEM_BUF_TYPE_NUM        (2U)
#define EXT_MEM_BUF_TABLE_ALIGN_NUM     (16U)   /* cached operation need to be 64 align */

/* slice config Relative */
#define MAX_TOTAL_SLICE_NUM             (48U)  // 4*12

#define DSP_MAX_PP_STRM_BLEND_NUMBER            (2U)
#define DSP_MAX_PP_STRM_BLEND_NUMBER_CTX        (DSP_MAX_PP_STRM_BLEND_NUMBER + DSP_MAX_PP_STRM_BLEND_NUMBER)
#define DSP_MAX_PP_STRM_COPY_NUMBER             (2U)

/* ext_mem_type_t */
#define VPROC_EXT_MEM_TYPE_MAIN                 (0U)
#define VPROC_EXT_MEM_TYPE_PREV_A               (1U)
#define VPROC_EXT_MEM_TYPE_PIP                  VPROC_EXT_MEM_TYPE_PREV_A
#define VPROC_EXT_MEM_TYPE_PREV_B               (2U)
#define VPROC_EXT_MEM_TYPE_PREV_C               (3U)
#define VPROC_EXT_MEM_TYPE_LN_DEC               (4U)
#define VPROC_EXT_MEM_TYPE_HIER_0               (5U)
#define VPROC_EXT_MEM_TYPE_COMP_RAW             (6U)
#define VPROC_EXT_MEM_TYPE_MAIN_ME              (7U)
#define VPROC_EXT_MEM_TYPE_PIP_ME               (8U)
#define NUM_VPROC_EXT_MEM_TYPE                  (9U)
#define VPROC_EXT_MEM_TYPE_PREV_A_ME            NUM_VPROC_EXT_MEM_TYPE
#define VPROC_EXT_MEM_TYPE_PREV_B_ME            (10U)
#define VPROC_EXT_MEM_TYPE_PREV_C_ME            (11U)
#define VPROC_EXT_MEM_TYPE_MCTS                 (12U)
#define VPROC_EXT_MEM_TYPE_MCTF                 (13U)
#define MAX_VPROC_EXT_MEM_TYPE_NUM              (NUM_VPROC_EXT_MEM_TYPE)

/*-----------------------------------------------------------------------------------------------*\
 * CAT_VIN (4)
\*-----------------------------------------------------------------------------------------------*/
#define DSP_VIN_MAX_NUM                 (AMBA_DSP_MAX_VIN_NUM + AMBA_DSP_MAX_VIRT_VIN_NUM)
#define DSP_VIRTUAL_VIN_START_IDX       (AMBA_DSP_MAX_VIN_NUM)

/*---------------------------------------------------------------------------*\
 * Defined in AmbaDSP_Buffer.c
\*---------------------------------------------------------------------------*/
#define DSP_MAX_NUM_DEFAULT_CMD             (256U)
#define DSP_NUM_DEFAULT_HEADER_CMD          (1U)

/* Store physical address inside it */
typedef struct {
    dsp_init_data_t     Data;
} AMBA_DSP_INIT_DATA_s;

typedef struct {
    UINT32 Id;                                          /* Space for ChipId */
    UINT32 Rsvd;
    UINT8  ProtectPattern[AMBA_DSP_PROTECT_DATA_LEN];   /* Space for encrypt data */
    UINT32 Rsvd1[2U];
} AMBA_DSP_CHIP_DATA_s;

#define DEF_CMD_SET_LIVEVIEW    (0U)
#define DEF_CMD_SET_PLAYBACK    (1U)
#define DEF_CMD_SET_SENSORLESS  (2U)
#define DEF_CMD_SET_XCODE       (3U)
#define NUM_DEF_CMD_SET         (4U) /* exclude DVE setup */

#define DSP_MAX_NUM_ASSERT_INFO (8U) // 8 threads's assert info
typedef struct {
    assert_info_t   Info;
    UINT8           Rsvd[28];        // Must allocate 64 bytes align
} AMBA_DSP_ASSERT_INFO_s;

/* slice_layout_t, must be 64 alignment */
typedef struct
{
    UINT8  TileColIdx;
    UINT8  TileRowIdx;
    UINT16 TileColStart;    //W0

    UINT16 TileRowStart;
    UINT16 TileColWidth;    //W1

    UINT16 TileRowHeight;
    UINT16 rsdv0;           //W2

    UINT32 rsdv1[13U];      //W3~W15
} DSP_SLICE_LAYOUT_s;

/*---------------------------------------------------------------------------*\
 * Default configuration commands buffer
\*---------------------------------------------------------------------------*/
typedef struct {
    dsp_cmd_t Header;                                    /* 0x000000AB */
    dsp_cmd_t CmdData[DSP_MAX_NUM_DEFAULT_CMD - DSP_NUM_DEFAULT_HEADER_CMD];
} AMBA_DSP_DEFAULT_CMDS_s;

/*---------------------------------------------------------------------------*\
 * OSD Buffer Info
\*---------------------------------------------------------------------------*/
typedef struct  {
    UINT32   OsdBufDramAddr;
    UINT16   OsdBufPitch;
    UINT8    OsdBufRepeatField;
    UINT8    Reserved;
} AMBA_DSP_OSD_BUF_INFO_s;

#define AMBA_DSP_VIN_MAIN_CFG_LEN   (128U)
typedef struct  {
    UINT8  Data[AMBA_DSP_VIN_MAIN_CFG_LEN];
} AMBA_DSP_VIN_MAIN_CONFIG_DATA_s;

#define AMBA_DSP_VOUT_DISP_CFG_LEN   (508U)
typedef struct  {
    UINT8  Data[AMBA_DSP_VOUT_DISP_CFG_LEN];
} AMBA_DSP_VOUT_DISP_CFG_DATA_s;

typedef struct  {
    UINT32 Data[DSP_VOUT_CSC_MATRIX_DEPTH];
} AMBA_DSP_VOUT_MIXER_CSC_DATA_s;

typedef struct  {
    UINT8  Data[DSP_TVE_CFG_SIZE];
} AMBA_DSP_VOUT_TVE_DATA_s;

#define VP_MSG_PATTERN  (0xBEEFU)

/*---------------------------------------------------------------------------*\
 * H264/JPEG encoder bitstream information buffer
\*---------------------------------------------------------------------------*/
#define AMBA_DSP_DESC_BUFFER_NUM                    (256U)
#define DSP_ENC_BITS_INFO_STEPSIZE                  (64U)
#define DSP_ENC_END_MARK                            (0xFFFFFFU)
#define AMBA_DSP_VIDEOENC_DESC_BUF_SIZE             (AMBA_DSP_DESC_BUFFER_NUM*DSP_ENC_BITS_INFO_STEPSIZE)

/*---------------------------------------------------------------------------*\
 *  Hier info buffer
\*---------------------------------------------------------------------------*/
#define AMBA_DSP_HIER_UNIT_SIZE (488U)   //sizeof(msg_vp_hier_out_lane_out_t)

typedef struct  {
    msg_vp_hier_out_lane_out_t  VpHierOut;
    UINT8                       Padding[24];
} AMBA_DSP_MSG_VP_HIER_INFO_s;

/*---------------------------------------------------------------------------*\
 *  Idsp debug buffer
\*---------------------------------------------------------------------------*/
#ifndef USE_STAT_IDSP_BUF
extern IDSP_INFO_s AmbaDSP_IdspInfo[AMBA_DSP_MAX_VIEWZONE_NUM];
#endif

/*---------------------------------------------------------------------------*\
 * Pp dummy blend table
\*---------------------------------------------------------------------------*/
#define DSP_DMY_BLD_SIZE                    (1024U) //32*32

/*---------------------------------------------------------------------------*\
 *  VprocExtStatus info buffer
\*---------------------------------------------------------------------------*/
#define AMBA_DSP_VPROC_EXT_MSG_UNIT_SIZE    (256U)   //ALIGN_128(MAX(sizeof(vproc_li_c2y_ext_data_t), sizeof(vproc_li_warp_mctf_prev_ext_data_t)))
#define AMBA_DSP_VPROC_EXT_MSG_NUM          (256U)

#define DSP_IDSP_CFG_NUM                    (AMBA_DSP_MAX_VIEWZONE_NUM + 1U) // extra one for user query
typedef struct {
    DSP_MEM_BLK_t StatBuf;
    DSP_MEM_BLK_t CfaStat[DSP_3A_BUFFER];
    DSP_MEM_BLK_t PgStat[DSP_3A_BUFFER];
    DSP_MEM_BLK_t Hist0Stat[DSP_3A_BUFFER];
    DSP_MEM_BLK_t Hist1Stat[DSP_3A_BUFFER];
    DSP_MEM_BLK_t Hist2Stat[DSP_3A_BUFFER];
    DSP_MEM_BLK_t IdspCfg[DSP_IDSP_CFG_NUM];
} AMBA_DSP_STAT_BUF_t;

/*---------------------------------------------------------------------------*\
 *  Event info buffer
\*---------------------------------------------------------------------------*/
typedef struct {
    /* 128align : max of {AMBA_DSP_RAW_DATA_RDY_s(EXT)[80+18],
     *                    dsp_stat_buf_t[8],
     *                    AMBA_DSP_YUV_DATA_RDY_s[72],
     *                    AMBA_DSP_YUV_DATA_RDY_EXTEND_s[104],
     *                    AMBA_DSP_PYMD_DATA_RDY_s[336],
     *                    AMBA_DSP_VOUT_DATA_INFO_s[64],
     *                    AMBA_DSP_ENC_PIC_RDY_s[80],
     *                    AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s[4],
     *                    AMBA_DSP_STL_RAW_DATA_INFO_s[96],
     *                    AMBA_DSP_YUV_IMG_BUF_s[40],
     *                    AMBA_DSP_VIDEO_PATH_INFO_s[8],
     *                    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s[48],
     *                    AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s[40],
     *                    AMBA_DSP_EVENT_VIDDEC_PIC_EX_s[56],
     *                    AMBA_DSP_STLDEC_STATUS_s[40],
     *                    AMBA_DSP_STILL_DEC_YUV_DISP_REPORT_s[40],
     *                    AMBA_DSP_VIN_POST_CONFIG_STATE_s[2],
     *                    AMBA_DSP_LV_SYNC_JOB_INFO_s[8],
     *                    AMBA_DSP_ENC_MV_RDY_s[40],AMBA_DSP_YUV_INFO_s
     *                    AMBA_DSP_SIDEBAND_DATA_RDY_s[72],
     *                    AMBA_DSP_LNDT_DATA_RDY_s[80]}
     */
    UINT8 Data[384];
} AMBA_DSP_EVENT_INFO_s;

typedef struct {
    /* Physical Msg Buffers */
    DSP_MEM_BLK_t GeneralMsgFifoCtrl;
    DSP_MEM_BLK_t VCapMsgFifoCtrl;
    DSP_MEM_BLK_t VProcMsgFifoCtrl;
#ifdef SUPPORT_DSP_ASYNC_ENCMSG
    DSP_MEM_BLK_t AsyncEncMsgFifoCtrl;
#endif
    DSP_MEM_BLK_t VPMsgFifoCtrl;

    /* Common Buffers */
    DSP_MEM_BLK_t InitData;
    DSP_MEM_BLK_t ChipData;
    DSP_MEM_BLK_t AssertInfo;
    DSP_MEM_BLK_t DefaultCmds;

    /* Sync Command Buffer Control */
    DSP_MEM_BLK_t SyncCmdBufCtrl[AMBA_DSP_NUM_SYNC_CMD_BUF_CTRL];
    DSP_MEM_BLK_t VProcMsgBufCtrl;
#ifdef SUPPORT_DSP_ASYNC_ENCMSG
    DSP_MEM_BLK_t AsyncEncMsgBufCtrl;
#endif

    /* Video Encode Buffers */
    DSP_MEM_BLK_t VidEncDescBuf;
    DSP_MEM_BLK_t EncStrmCfg;

    /* Vproc External Buffers */
    DSP_MEM_BLK_t VprocExtMsgQ;
    DSP_MEM_BLK_t VprocExtMemDesc;
    DSP_MEM_BLK_t VprocPymdExtMemTbl[AMBA_DSP_MAX_VIEWZONE_NUM];
    DSP_MEM_BLK_t VprocLndtExtMemTbl[AMBA_DSP_MAX_VIEWZONE_NUM];
    DSP_MEM_BLK_t VprocIntMainExtMemTbl[AMBA_DSP_MAX_VIEWZONE_NUM];
    DSP_MEM_BLK_t YuvStrmExtMemTbl[AMBA_DSP_MAX_YUVSTRM_NUM];
    DSP_MEM_BLK_t YuvStrmAuxExtMemTbl[AMBA_DSP_MAX_YUVSTRM_NUM];

    /* Vin External Memery and Config Buffers */
    DSP_MEM_BLK_t ExtRawBufArray;
    DSP_MEM_BLK_t VinRawExtMemTbl[AMBA_DSP_MAX_VIN_NUM];
    DSP_MEM_BLK_t VinAuxExtMemTbl[AMBA_DSP_MAX_VIN_NUM];
    DSP_MEM_BLK_t VinMainCfg[AMBA_DSP_MAX_VIN_NUM];

    /* Yuv Stream Buffers */
    DSP_MEM_BLK_t PpStrmCtrl;
    DSP_MEM_BLK_t PpStrmCopy;
    DSP_MEM_BLK_t PpStrmBlend;
    DSP_MEM_BLK_t DummyBlend;

    /* Fov Layout Buffers */
    DSP_MEM_BLK_t SideBand;
    DSP_MEM_BLK_t FovLayout;
    DSP_MEM_BLK_t VinFovCfg;
    /* Vproc Group Buffers */
    DSP_MEM_BLK_t GrpingNum;
    DSP_MEM_BLK_t VprocGrpOrder;
    DSP_MEM_BLK_t VprocOrder;
    DSP_MEM_BLK_t VprocSliceLayout;

#ifdef SUPPORT_VPROC_OSD_INSERT
    DSP_MEM_BLK_t VprocOsdInsert[AMBA_DSP_MAX_STREAM_NUM][DSP_MAX_OSD_INSERT_BUF_NUM];
#endif

    /* Command Queue Buffers */
    DSP_MEM_BLK_t BatchCmdQ;
    DSP_MEM_BLK_t BatchCmdSet;
    DSP_MEM_BLK_t GroupCmdQ;
    DSP_MEM_BLK_t ViewZoneGroupCmdQ;
    /* Still Buffers */
    DSP_MEM_BLK_t StlExtMemTbl[MAX_STL_EXT_MEM_STAGE_NUM];
    DSP_MEM_BLK_t StlBatchCmdQ[MAX_STL_EXT_MEM_STAGE_NUM];
    DSP_MEM_BLK_t DataCapMemTbl[AMBA_DSP_MAX_DATACAP_NUM];
    DSP_MEM_BLK_t DataCapAuxMemTbl[AMBA_DSP_MAX_DATACAP_NUM];

    /* Vout Config Buffers */
    DSP_MEM_BLK_t VoutDispCfg[AMBA_DSP_MAX_VOUT_NUM];
    DSP_MEM_BLK_t VoutMixerCscCfg[AMBA_DSP_MAX_VOUT_NUM];
    DSP_MEM_BLK_t VoutTveCfg[AMBA_DSP_MAX_VOUT_NUM];

    /* Chip Depend Command Buffers */
    DSP_MEM_BLK_t GeneralCmdBuf;
    DSP_MEM_BLK_t VinCmdBuf[AMBA_DSP_MAX_VIN_NUM];
    DSP_MEM_BLK_t GroupGeneralCmdBuf;
    DSP_MEM_BLK_t GroupVinCmdBuf[AMBA_DSP_MAX_VIN_NUM];
    /* Chip Depend Message Buffers*/
    DSP_MEM_BLK_t GeneralMsgBuf;
    DSP_MEM_BLK_t VCapMsgBuf;
    DSP_MEM_BLK_t VProcMsgBuf;
#ifdef SUPPORT_DSP_ASYNC_ENCMSG
    DSP_MEM_BLK_t AsyncEncMsgBuf;
#endif
    DSP_MEM_BLK_t VpMsgBuf;

    /* Event Buffers */
    DSP_MEM_BLK_t EventBuf;
    DSP_MEM_BLK_t EventInfo[DSP_EVENT_INFO_POOL_DEPTH];
} AMBA_DSP_PROT_BUF_t;

extern void AmbaDSP_InitBuffers(void);
extern UINT32 AmbaDSP_StatBufCreate(AMBA_DSP_STAT_BUF_t *pDspStatBuf);
extern UINT32 AmbaDSP_ProtBufCreate(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateMsgFifoBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateCommonBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateSyncCtrlBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateEncStrmBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateVprocExtBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateVinExtBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateYuvStrmBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateFovInfoBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateVprocGrpBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateCmdQBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateStillBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateVoutCfgBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateCmdBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateMsgBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);
//extern UINT32 AmbaDSP_CreateEventBuf(AMBA_DSP_PROT_BUF_t *pDspProtBuf);

extern void DSP_GetBitsDescBufferAddr(ULONG *pULAddr);
void DSP_GetVprocExtMsgBufferInfo(ULONG *pULAddr);
extern void DSP_GetDspEncStrmCfg(const UINT16 Index, DSP_ENC_CFG **pInfo);
extern void DSP_GetDspVinCfg(const UINT16 Index, AMBA_DSP_VIN_MAIN_CONFIG_DATA_s **pInfo);
extern void DSP_GetDspVoutDispCfg(const UINT16 Index, AMBA_DSP_VOUT_DISP_CFG_DATA_s **pInfo);
extern void DSP_GetDspVoutMixerCscCfg(const UINT16 Index, AMBA_DSP_VOUT_MIXER_CSC_DATA_s **pInfo);
extern void DSP_GetDspVoutTveCfg(const UINT16 Index, AMBA_DSP_VOUT_TVE_DATA_s **pInfo);
extern void DSP_GetDspExtRawBufArray(const UINT16 VinIdx, const UINT16 Type, UINT32 **pInfo);
extern void DSP_GetFovSideBandInfoCfg(const UINT16 ViewZoneId, const UINT16 TokenIndex, sideband_info_t **pInfo);
extern void DSP_GetDspPpStrmCtrlCfg(const UINT16 YuvStrmIdx, const UINT16 BufIdx, const UINT16 ChanIdx, vproc_pp_stream_cntl_t **pInfo);
extern void DSP_GetDspPpStrmCopyCfg(const UINT16 YuvStrmIdx, const UINT16 BufIdx, const UINT16 ChanIdx, vproc_post_r2y_copy_cfg_t **pInfo);
extern void DSP_GetDspPpStrmBldCfg(const UINT16 YuvStrmIdx, const UINT16 BufIdx, const UINT16 ChanIdx, vproc_y2y_blending_cfg_t **pInfo);
extern void DSP_GetDspPpDummyBldTbl(UINT8 **pInfo);
extern void DSP_GetDspVinFovLayoutCfg(const UINT16 Index, const UINT16 LocalIndex, fov_layout_t **pInfo);
extern void DSP_GetDspVinStartFovLayoutCfg(const UINT16 Index, const UINT16 LocalIndex, vin_fov_cfg_t **pInfo);
extern void DSP_GetDspBatchCmdQ(const UINT16 ViewZoneId, const UINT16 BatchIdx, UINT32 **pInfo);
extern void DSP_GetDspBatchCmdSet(const UINT16 ViewZoneId, const UINT16 Idx, vin_fov_batch_cmd_set_t **pInfo);
extern void DSP_GetDspGroupCmdQ(const UINT16 YuvStreamId, const UINT16 SeqIdx, const UINT16 SubJobIdx, UINT32 **pInfo);
extern void DSP_GetDspViewZoneGroupCmdQ(const UINT16 ViewZoneId, const UINT16 SeqIdx, UINT32 **pInfo);
extern void DSP_GetDspVprocGrpingNumBuf(const UINT16 GrpId, UINT8 **pInfo);
extern void DSP_GetDspVprocGrpingOrdBuf(const UINT16 GrpId, const UINT16 OrdId, UINT8 **pInfo);
extern void DSP_GetDspVprocOrderBuf(const UINT16 GrpId, const UINT16 OrdId, UINT8 **pInfo);
extern void DSP_GetDspVprocSliceLayoutBuf(const UINT16 ViewZoneId, DSP_SLICE_LAYOUT_s **pInfo);
#ifdef SUPPORT_VPROC_OSD_INSERT
extern void DSP_GetDspVprocOsdInsertBuf(const UINT16 StremId, UINT16 BufId, UINT16 Id, osd_insert_buf_t **pInfo);
#endif
extern void DSP_GetDspVprocExtMemDescBuf(const UINT16 ViewZoneId, ext_mem_desc_t **pInfo);
extern void DSP_GetDspExtStlBufArray(UINT8 StageId, UINT32 **pInfo);
extern void DSP_GetDspStlBatchCmdQ(UINT8 StageId, UINT32 **pInfo);
extern void DSP_GetDspExtPymdBufArray(const UINT16 ViewZoneId, UINT32 **pInfo);
extern void DSP_GetDspExtLndtBufArray(const UINT16 ViewZoneId, UINT32 **pInfo);
extern void DSP_GetDspExtIntMainBufArray(const UINT16 ViewZoneId, UINT32 **pInfo);
extern void DSP_GetDspExtYuvStrmBufArray(const UINT16 YuvStrmId, UINT32 **pInfo);
extern void DSP_GetDspExtYuvStrmAuxBufArray(const UINT16 YuvStrmId, UINT32 **pInfo);
extern void DSP_GetDspExtVinRawBufArray(const UINT16 VinId, UINT32 **pInfo);
extern void DSP_GetDspExtVinAuxBufArray(const UINT16 VinId, UINT32 **pInfo);
extern void DSP_GetDspDataCapBufArray(const UINT16 Id, UINT32 **pInfo);
extern void DSP_GetDspDataCapAuxBufArray(const UINT16 Id, UINT32 **pInfo);
extern void DSP_GetDspEventInfoAddr(const UINT16 Id, ULONG *pULAddr);
extern void DSP_GetDspStatBufAddr(const UINT16 StatId, const UINT16 BufId, ULONG *pULAddr);
extern UINT32 DSP_GetProtectBuf(ULONG *pAddr, UINT32 *pSize);
extern void DSP_GetDspChipDataBuf(ULONG *pULAddr);

/* AmbaDSP_GetSubProtBuf01 */
#define DSP_PROT_BUF_INIT_DATA              (0U)
#define DSP_PROT_BUF_CHIP_DATA              (1U)
#define DSP_PROT_BUF_ASSERT_INFO            (2U)
#define DSP_PROT_BUF_DEF_CMD                (3U)
#define DSP_PROT_BUF_SYNC_CMD_CTRL          (4U)
#define DSP_PROT_BUF_VPROC_MSG_BUF_CTRL     (5U)
#define DSP_PROT_BUF_AYNC_ENC_MSG_BUF_CTRL  (6U)
#define DSP_PROT_BUF_VENC_DESC_BUF          (7U)
#define DSP_PROT_BUF_ENC_STRM_CFG           (8U)
/* AmbaDSP_GetSubProtBuf02 */
#define DSP_PROT_BUF_VPROC_EXT_MSG_Q        (9U)
#define DSP_PROT_BUF_VPROC_EXT_MEM_DESC     (10U)
#define DSP_PROT_BUF_VID_EXT_PYMD_TBL       (11U)
#define DSP_PROT_BUF_VID_EXT_LNDT_TBL       (12U)
#define DSP_PROT_BUF_VID_EXT_YUV_STRM_TBL   (13U)
/* AmbaDSP_GetSubProtBuf03 */
#define DSP_PROT_BUF_EXT_RAW_BUF_ARRAY      (14U)
#define DSP_PROT_BUF_VIN_EXT_RAW_TBL        (15U)
#define DSP_PROT_BUF_VIN_EXT_AUX_TBL        (16U)
#define DSP_PROT_BUF_VIN_MAIN_CFG           (17U)
#define DSP_PROT_BUF_PP_STRM_CTRL           (18U)
#define DSP_PROT_BUF_PP_STRM_COPY           (19U)
#define DSP_PROT_BUF_PP_STRM_BLEND          (20U)
/* AmbaDSP_GetSubProtBuf04 */
#define DSP_PROT_BUF_SIDEBAND               (21U)
#define DSP_PROT_BUF_FOV_LAYOUT             (22U)
#define DSP_PROT_BUF_VIN_FOV_CFG            (23U)
#define DSP_PROT_BUF_GRPING_NUM             (24U)
#define DSP_PROT_BUF_VPROC_GRP_ORDER        (25U)   //for CMD_VPROC_MULTI_CHAN_PROC_ORDER (0x02000066)
#define DSP_PROT_BUF_VPROC_ORDER            (26U)   //for CMD_VPROC_SET_VPROC_GRPING (0x02000067)
#define DSP_PROT_BUF_VPROC_SLICE            (27U)
#define DSP_PROT_BUF_BATCH_CMD_Q            (28U)
#define DSP_PROT_BUF_BATCH_CMD_SET          (29U)
#define DSP_PROT_BUF_GROUP_CMD_Q            (30U)
/* AmbaDSP_GetSubProtBuf05 */
#define DSP_PROT_BUF_STL_EXT_MEM_TBL        (31U)
#define DSP_PROT_BUF_STL_BATCH_CMD_Q        (32U)
#define DSP_PROT_BUF_VOUT_DISP_CFG          (33U)
#define DSP_PROT_BUF_VOUT_MIXER_CSC_CFG     (34U)
#define DSP_PROT_BUF_VOUT_TVE_CFG           (35U)
/* AmbaDSP_GetSubProtBuf06 */
#define DSP_PROT_BUF_PP_DUMMY_BLEND         (36U)
#define DSP_PROT_BUF_VID_EXT_YUV_AUX_TBL    (37U)
/* AmbaDSP_GetSubProtBuf07 */
#define DSP_PROT_BUF_DATA_CAP_TBL           (38U)
#define DSP_PROT_BUF_DATA_CAP_AUX_TBL       (39U)
#define DSP_PROT_BUF_VID_EXT_INT_MAIN_TBL   (40U)
/* AmbaDSP_GetSubProtBuf08 */
#define DSP_PROT_BUF_EVENT_ALL              (41U)
#define DSP_PROT_BUF_EVENT                  (42U)
#define DSP_PROT_BUF_VIEW_GROUP_CMD_Q       (43U)
#define DSP_PROT_BUF_VPROC_OSD_INSERT       (44U)
#define DSP_PROT_BUF_NUM                    (45U)

extern UINT32 DSP_GetProtBuf(UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
extern UINT32 AmbaDSP_GetProtBuf(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf01(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf02(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf03(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf04(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf05(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf06(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf07(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubProtBuf08(const AMBA_DSP_PROT_BUF_t *pDspProtBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);

#define DSP_STAT_BUF_ALL                    (0U)
#define DSP_STAT_BUF_CFA                    (1U)
#define DSP_STAT_BUF_PG                     (2U)
#define DSP_STAT_BUF_HIST0                  (3U)
#define DSP_STAT_BUF_HIST1                  (4U)
#define DSP_STAT_BUF_HIST2                  (5U)
#define DSP_STAT_BUF_IDSP                   (6U)
#define DSP_STAT_BUF_NUM                    (7U)
extern UINT32 DSP_GetStatBuf(UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
extern UINT32 AmbaDSP_GetStatBuf(const AMBA_DSP_STAT_BUF_t *pDspStatBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubStatBuf01(const AMBA_DSP_STAT_BUF_t *pDspStatBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);
//extern UINT32 AmbaDSP_GetSubStatBuf02(const AMBA_DSP_STAT_BUF_t *pDspStatBuf, UINT32 Id, UINT32 AuxId, DSP_MEM_BLK_t *pBuf);

#endif
