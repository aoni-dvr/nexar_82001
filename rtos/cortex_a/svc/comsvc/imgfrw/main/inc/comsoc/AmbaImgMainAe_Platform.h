/**
 *  @file AmbaImgMainAe_Platform.h
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
 *  @details Constants and Definitions of Amba Image Main Ae for Common SOC
 *
 */

#ifndef AMBA_IMG_MAIN_AE_COMSOC_H
#define AMBA_IMG_MAIN_AE_COMSOC_H

#define AMBA_IMG_MAIN_AE_SYNC_DGCID   SYNC_WB_ID_BE

#define AMBA_IMG_MAX_EXPOSURE_NUM    AMBA_IMG_SENSOR_HAL_HDR_SIZE

#define AMBA_IMG_HDR_INFO_FLAG_MASK  0x0000000000003E3EULL

typedef union /*_AMBA_IMG_MAIN_HDR_INFO_OP_u_*/ {
    UINT64 Data;
    struct {
        UINT32 RawOffset:         1;
        UINT32 VinToneCurve:      1;
        UINT32 FeToneCurve:       1;
        UINT32 FeBlc0:            1;
        UINT32 FeBlc1:            1;
        UINT32 FeBlc2:            1;
        UINT32 FeWbGain0:         1;
        UINT32 FeWbGain1:         1;
        UINT32 FeWbGain2:         1;
        UINT32 Blend0:            1;
        UINT32 Blend1:            1;
        UINT32 Ce:                1;
        UINT32 CeInput:           1;
        UINT32 CeOutput:          1;
        UINT32 Reserved_L:       18;
        UINT32 Reserved_H:       28;
        UINT32 ExposureNum:       2;
        UINT32 Enable:            2;
    } Bits;
} AMBA_IMG_MAIN_HDR_INFO_OP_u;

typedef struct /*_AMBA_IMG_MAIN_HDR_INFO_s_*/ {
    AMBA_IMG_MAIN_HDR_INFO_OP_u    Op;
    AMBA_IK_HDR_RAW_INFO_s         RawOffset;
    AMBA_IP_VIN_TONE_CURVE_s       *pVinToneCurve;
    AMBA_IK_FE_TONE_CURVE_s        FeToneCurve;
    AMBA_IK_STATIC_BLC_LVL_s       FeBlc[AMBA_IMG_MAX_EXPOSURE_NUM];
    AMBA_IK_FE_WB_GAIN_s           FeWbGain[AMBA_IMG_MAX_EXPOSURE_NUM];
    AMBA_IK_HDR_BLEND_s            Blend[AMBA_IMG_MAX_EXPOSURE_NUM-1UL];
    AMBA_IK_CE_s                   Ce;
    AMBA_IK_CE_INPUT_TABLE_s       CeInput;
    AMBA_IK_CE_OUT_TABLE_s         CeOutput;
} AMBA_IMG_MAIN_HDR_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_WB_SENSOR_s_*/ {
    AMBA_IMG_SENSOR_WB_s    Wgc[AMBA_IMG_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_WB_SENSOR_s;

typedef struct /*_AMBA_IMG_MAIN_WB_DSP_s_*/ {
    UINT32  WbId;
    UINT32  Wgc[AMBA_IMG_MAX_EXPOSURE_NUM][3];
} AMBA_IMG_MAIN_WB_DSP_s;

typedef struct /*_AMBA_IMG_MAIN_WB_INFO_s_*/ {
    AMBA_IMG_MAIN_WB_SENSOR_s    Sensor;
    AMBA_IMG_MAIN_WB_DSP_s       Dsp;
    AMBA_IK_WB_GAIN_s            Ctx[AMBA_IMG_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_WB_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_AE_SENSOR_s_*/ {
    UINT32  Shr[AMBA_IMG_MAX_EXPOSURE_NUM];
    UINT32  Agc[AMBA_IMG_MAX_EXPOSURE_NUM];
    UINT32  Dgc[AMBA_IMG_MAX_EXPOSURE_NUM];
    UINT32  Sls;
} AMBA_IMG_MAIN_AE_SENSOR_s;

typedef struct /*_AMBA_IMG_MAIN_AE_BE_s_*/ {
    UINT32  SyncDgId;
    UINT32  FeDgc[AMBA_IMG_MAX_EXPOSURE_NUM];
#ifdef IMG_MAIN_FE_WB_IR_GAIN
    UINT32  FeIgc[AMBA_IMG_MAX_EXPOSURE_NUM];
#endif
    UINT32  BeDgc[2][AMBA_IMG_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_AE_DSP_s;

typedef struct /*_AMBA_IMG_MAIN_AE_INFO_s_*/ {
    AMBA_IMG_MAIN_AE_SENSOR_s    Sensor;
    AMBA_IMG_MAIN_AE_DSP_s       Dsp;
    AMBA_AE_INFO_s               Ctx[AMBA_IMG_MAX_EXPOSURE_NUM];
    AMBA_DGAIN_INFO_s            AuxCtx[AMBA_IMG_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_AE_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_AE_USER_DATA_s_*/ {
    AMBA_IMG_MAIN_AE_INFO_s            AeInfo;
    AMBA_IMG_MAIN_WB_INFO_s            WbInfo;
    AMBA_IMG_MAIN_HDR_INFO_s           HdrInfo;
#if defined(CONFIG_BUILD_IMGFRW_RAW_ENC) || defined(CONFIG_BUILD_IMGFRW_EFOV) || defined(CONFIG_BUILD_IMGFRW_RAW_PIV)
    AMBA_IMGPROC_OFFLINE_AAA_INFO_s    AaaInfo;
#endif
} AMBA_IMG_MAIN_AE_USER_DATA_s;

#ifdef CONFIG_BUILD_IMGFRW_EFOV
typedef struct /*_AMBA_IMG_MAIN_AE_EFOV_WB_s_*/ {
    AMBA_IK_WB_GAIN_s    Ctx[AMBA_IMG_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_AE_EFOV_WB_s;

typedef struct /*_AMBA_IMG_MAIN_AE_EFOV_AE_s_*/ {
    AMBA_AE_INFO_s    Ctx[AMBA_IMG_MAX_EXPOSURE_NUM];
} AMBA_IMG_MAIN_AE_EFOV_AE_s;

typedef struct /*_AMBA_IMG_MAIN_ADJ_EFOV_USER_s_*/ {
    AMBA_IMG_MAIN_AE_EFOV_WB_s         WbInfo;
    AMBA_IMG_MAIN_AE_EFOV_AE_s         AeInfo;
    AMBA_IMGPROC_OFFLINE_AAA_INFO_s    AaaInfo;
} AMBA_IMG_MAIN_AE_EFOV_USER_s;

typedef struct /*_AMBA_IMG_MAIN_AE_EFOV_TXRX_s_*/ {
    UINT64                          Msg;
    AMBA_IMG_MAIN_AE_EFOV_USER_s    User;
} AMBA_IMG_MAIN_AE_EFOV_TXRX_s;
#endif

#define AMBA_IMG_MAIN_AE_SNAP_INFO_s AMBA_IMG_MAIN_AE_INFO_s
#define AMBA_IMG_MAIN_WB_SNAP_INFO_s AMBA_IMG_MAIN_WB_INFO_s

typedef union /*_AMBA_IMG_MAIN_HDR_SNAP_INFO_OP_u_*/ {
    UINT64 Data;
    struct {
        UINT32 RawOffset:         1;
        UINT32 VinToneCurve:      1;
        UINT32 Reserved_L1:       4;
        UINT32 FeWbGain0:         1;
        UINT32 FeWbGain1:         1;
        UINT32 FeWbGain2:         1;
        UINT32 Reserved_L2:      23;
        UINT32 Reserved_H:       28;
        UINT32 ExposureNum:       2;
        UINT32 Enable:            2;
    } Bits;
} AMBA_IMG_MAIN_HDR_SNAP_INFO_OP_u;

typedef struct /*_AMBA_IMG_MAIN_HDR_SNAP_INFO_s_*/ {
    AMBA_IMG_MAIN_HDR_SNAP_INFO_OP_u    Op;
    AMBA_IP_VIN_TONE_CURVE_s            *pVinToneCurve;
    AMBA_IK_HDR_RAW_INFO_s              RawOffset;
} AMBA_IMG_MAIN_HDR_SNAP_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s_*/ {
    AMBA_IMG_MAIN_AE_SNAP_INFO_s     AeInfo;
    AMBA_IMG_MAIN_WB_SNAP_INFO_s     WbInfo;
    AMBA_IMG_MAIN_HDR_SNAP_INFO_s    HdrInfo;
} AMBA_IMG_MAIN_AE_USER_SNAP_DATA_s;

extern UINT32 AmbaImgMainAe_VinToneCurveFlag[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];

UINT32 AmbaImgMainAe_UserGainPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, void *pUserData);
UINT32 AmbaImgMainAe_UserDataPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, void *pUserData);
UINT32 AmbaImgMainAe_AikProc(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User);
UINT32 AmbaImgMainAe_SlsCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User);
UINT32 AmbaImgMainAe_DgcPositionGet(UINT32 *pSyncDgId);
#ifdef CONFIG_BUILD_IMGFRW_EFOV
UINT32 AmbaImgMainAe_EFovTxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 TxData);
UINT32 AmbaImgMainAe_EFovRxCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 RxData);
#endif
UINT32 AmbaImgMainAe_UserSnapGainPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, UINT32 AebIndex, void *pUserData);
UINT32 AmbaImgMainAe_UserSnapDataPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, void *pUserData);
UINT32 AmbaImgMainAe_AikSnapProc(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User);
UINT32 AmbaImgMainAe_SlsSnapCb(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User);
UINT32 AmbaImgMainAe_AikSnapIso(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User);
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
UINT32 AmbaImgMainAe_UserPivGainPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, void *pUserData);
UINT32 AmbaImgMainAe_UserPivDataPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 AlgoId, void *pUserData);
UINT32 AmbaImgMainAe_AikPivIso(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User);
#endif
#endif  /* AMBA_IMG_MAIN_AE_COMSOC_H */
