/**
 *  @file AmbaMonMain_Internal.h
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
 *  @details Constants and Definitions for Amba Monitor Main Internal
 *
 */

#ifndef AMBA_MONITOR_MAIN_INTERNAL_H
#define AMBA_MONITOR_MAIN_INTERNAL_H

//#define AMBA_MON_PRINT_ENABLE_FLAG    (S_PRINT_FLAG_MSG | S_PRINT_FLAG_ERR | S_PRINT_FLAG_DBG | S_PRINT_FLAG_STATE)
#define AMBA_MON_PRINT_ENABLE_FLAG    (S_PRINT_FLAG_MSG | S_PRINT_FLAG_ERR | S_PRINT_FLAG_STATE)

//#define AMBA_MON_MESSAGE_PUT2

/* extern prot for message input in main vin/dsp/vout */
extern AMBA_MON_MESSAGE_PORT_s VinSofMsgPort[AMBA_MON_NUM_VIN_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s VinEofMsgPort[AMBA_MON_NUM_VIN_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspRawMsgPort[AMBA_MON_NUM_VIN_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspYuvMsgPort[AMBA_MON_NUM_FOV_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspYuvAltMsgPort[AMBA_MON_NUM_FOV_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspDefYuvAltMsgPort[AMBA_MON_NUM_FOV_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspDispMsgPort[AMBA_MON_NUM_DISP_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspVoutMsgPort[AMBA_MON_NUM_VOUT_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspDefRawMsgPort[AMBA_MON_NUM_VIN_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspCfaAaaMsgPort[AMBA_MON_NUM_FOV_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspRgbAaaMsgPort[AMBA_MON_NUM_FOV_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspFovLatencyMsgPort[AMBA_MON_NUM_FOV_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspVoutLatencyMsgPort[AMBA_MON_NUM_VOUT_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s DspVinTimeoutMsgPort[AMBA_MON_NUM_VIN_CHANNEL];
extern AMBA_MON_MESSAGE_PORT_s VoutIsrMsgPort[AMBA_MON_NUM_VOUT_CHANNEL];

/* extern event name for finding event in listen vin/dsp/vout task */
extern const char *pVinSofName;
extern const char *pVinEofName;
extern const char *pDspRawName;
extern const char *pDspYuvName;
extern const char *pDspYuvAltName;
extern const char *pDspDefYuvAltName;
extern const char *pDspDispName;
extern const char *pDspVoutName;
extern const char *pDspDefRawName;
extern const char *pDspCfaAaaName;
extern const char *pDspRgbAaaName;
//extern const char *pDspFovLatencyName;
//extern const char *pDspVoutLatencyName;
extern const char *pDspVinTimeoutName;
extern const char *pVoutName;

typedef UINT32 (*AMBA_MON_TIMEOUT_FUNC_f)(const void *pEvent);

typedef struct /*_AMBA_MON_TIMEOUT_CB_s_*/ {
    const char                            *pName;
    union {
        const void                        *pCvoid;
        UINT32                            *pUint32;
        AMBA_MON_VIN_ISR_RDY_s            *pVinIsrRdy;
        AMBA_DSP_RAW_DATA_RDY_s           *pRawRdy;
        AMBA_DSP_YUV_DATA_RDY_EXTEND_s    *pYuvAltRdy;
        AMBA_DSP_VOUT_DATA_INFO_s         *pVoutStatus;
        AMBA_MON_VOUT_ISR_RDY_s           *pVoutIsrRdy;
    } Param;
    AMBA_MON_TIMEOUT_FUNC_f               pFunc;
} AMBA_MON_TIMEOUT_CB_s;

typedef enum /*_AMBA_MON_DEBUG_MSG_ID_e_*/ {
    DEBUG_MSG_ID_VIN = 0,
    DEBUG_MSG_ID_VOUT,
    DEBUG_MSG_ID_DSP,
    DEBUG_MSG_ID_DSPVOUT,
    DEBUG_MSG_ID_LATENCY,
    DEBUG_MSG_ID_CD,
    DEBUG_MSG_ID_TIMEOUT
} AMBA_MON_DEBUG_MSG_ID_e;

#define DEBUG_MSG_FLAG_VIN        (((UINT32) 1U) << (UINT32) DEBUG_MSG_ID_VIN)
#define DEBUG_MSG_FLAG_VOUT       (((UINT32) 1U) << (UINT32) DEBUG_MSG_ID_VOUT)
#define DEBUG_MSG_FLAG_DSP        (((UINT32) 1U) << (UINT32) DEBUG_MSG_ID_DSP)
#define DEBUG_MSG_FLAG_DSPVOUT    (((UINT32) 1U) << (UINT32) DEBUG_MSG_ID_DSPVOUT)
#define DEBUG_MSG_FLAG_LATENCY    (((UINT32) 1U) << (UINT32) DEBUG_MSG_ID_LATENCY)
#define DEBUG_MSG_FLAG_CD         (((UINT32) 1U) << (UINT32) DEBUG_MSG_ID_CD)
#define DEBUG_MSG_FLAG_TIMEOUT    (((UINT32) 1U) << (UINT32) DEBUG_MSG_ID_TIMEOUT)

typedef union /*_AMBA_MON_MAIN_DEBUG_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Vin:          1;
        UINT32 Vout:         1;
        UINT32 Dsp:          1;
        UINT32 DspVout:      1;
        UINT32 Latency:      1;
        UINT32 Cd:           1;
        UINT32 Timeout:      1;
        UINT32 Reserved:    25;
    } Bits;
} AMBA_MON_MAIN_DEBUG_u;

typedef struct /*_AMBA_MON_MAIN_COM_INFO_s_*/ {
    UINT32 Num;
    UINT32 SelectBits;
} AMBA_MON_MAIN_COM_INFO_s;

typedef struct /*_AMBA_MON_MAIN_INFO_s_*/ {
    AMBA_MON_MAIN_DEBUG_u       Debug;
    AMBA_MON_MAIN_COM_INFO_s    Vin;
    AMBA_MON_MAIN_COM_INFO_s    Td;
    AMBA_MON_MAIN_COM_INFO_s    Fov;
    AMBA_MON_MAIN_COM_INFO_s    Vout;
    AMBA_MON_MAIN_COM_INFO_s    Sensor[AMBA_MON_NUM_VIN_CHANNEL];
    AMBA_MON_MAIN_COM_INFO_s    Serdes[AMBA_MON_NUM_VIN_CHANNEL];
    AMBA_MON_MAIN_COM_INFO_s    FovInVin[AMBA_MON_NUM_VIN_CHANNEL];
} AMBA_MON_MAIN_INFO_s;

#define AMBA_MON_DEBUG_ENABLE_FLAG    (DEBUG_MSG_FLAG_VIN | DEBUG_MSG_FLAG_VOUT | DEBUG_MSG_FLAG_DSP | DEBUG_MSG_FLAG_DSPVOUT)

extern AMBA_MON_MAIN_INFO_s AmbaMonMain_Info;
extern UINT32 AmbaMonMain_VinSrcTable[AMBA_MON_NUM_VIN_CHANNEL];
extern UINT32 AmbaMonMain_VinSrcFlag[AMBA_MON_NUM_VIN_CHANNEL];
extern UINT32 AmbaMonMain_SensorIdTable[AMBA_MON_NUM_VIN_CHANNEL][AMBA_MON_NUM_VIN_SENSOR];
extern UINT32 AmbaMonMain_FrameTimeTable[AMBA_MON_NUM_VIN_CHANNEL];
extern UINT32 AmbaMonMain_FrameLineTable[AMBA_MON_NUM_VIN_CHANNEL];
extern UINT32 AmbaMonMain_FrameLineFactor[AMBA_MON_NUM_VIN_CHANNEL];
extern UINT32 AmbaMonMain_SvrDelayTable[AMBA_MON_NUM_VIN_CHANNEL];

typedef union /*_AMBA_MON_MAIN_VIN_SRC_EN_u_*/ {
    UINT32 Data;
    struct {
        UINT32 SofSrc:            1;
        UINT32 SofPut:            1;
        UINT32 SofReserved:       2;
        UINT32 EofSrc:            1;
        UINT32 EofPut:            1;
        UINT32 EofReserved:       2;
        UINT32 Reserved:         24;
    } Bits;
} AMBA_MON_MAIN_VIN_SRC_EN_u;

typedef struct /*_AMBA_MON_MAIN_VIN_SRC_EN_s_*/ {
    AMBA_MON_MAIN_VIN_SRC_EN_u    Ctx;
} AMBA_MON_MAIN_VIN_SRC_EN_s;

typedef union /*_AMBA_MON_MAIN_DSP_SRC_EN_u_*/ {
    UINT32 Data;
    struct {
        UINT32 RawSrc:            1;
        UINT32 RawPut:            1;
        UINT32 RawReserved:       2;
        UINT32 CfaSrc:            1;
        UINT32 CfaPut:            1;
        UINT32 CfaReserved:       2;
        UINT32 RgbSrc:            1;
        UINT32 RgbPut:            1;
        UINT32 RgbReserved:       2;
        UINT32 YuvSrc:            1;
        UINT32 YuvPut:            1;
        UINT32 YuvReserved:       2;
        UINT32 YuvAltSrc:         1;
        UINT32 YuvAltPut:         1;
        UINT32 YuvAltReserved:    2;
        UINT32 DispSrc:           1;
        UINT32 DispPut:           1;
        UINT32 DispReserved:      2;
        UINT32 VoutSrc:           1;
        UINT32 VoutPut:           1;
        UINT32 VoutReserved:      2;
        UINT32 Reserved:          4;
    } Bits;
} AMBA_MON_MAIN_DSP_SRC_EN_u;

typedef struct /*_AMBA_MON_MAIN_DSP_SRC_EN_s_*/ {
    AMBA_MON_MAIN_DSP_SRC_EN_u    Ctx;
} AMBA_MON_MAIN_DSP_SRC_EN_s;

typedef union /*_AMBA_MON_MAIN_VOUT_SRC_EN_u_*/ {
    UINT32 Data;
    struct {
        UINT32 IsrSrc:            1;
        UINT32 IsrPut:            1;
        UINT32 IsrReserved:       2;
        UINT32 Reserved:         28;
    } Bits;
} AMBA_MON_MAIN_VOUT_SRC_EN_u;

typedef struct /*_AMBA_MON_MAIN_VOUT_SRC_EN_s_*/ {
    AMBA_MON_MAIN_VOUT_SRC_EN_u    Ctx;
} AMBA_MON_MAIN_VOUT_SRC_EN_s;

extern AMBA_MON_MAIN_VIN_SRC_EN_s AmbaMonMain_VinSrcEn;
extern AMBA_MON_MAIN_DSP_SRC_EN_s AmbaMonMain_DspSrcEn;
extern AMBA_MON_MAIN_VOUT_SRC_EN_s AmbaMonMain_VoutSrcEn;

extern UINT32 AmbaMonMain_FovLatencySkip[AMBA_MON_NUM_FOV_CHANNEL];
extern UINT32 AmbaMonMain_VoutLatencySkip[AMBA_MON_NUM_VOUT_CHANNEL];

void AmbaMonMain_WatchdogActive(void);
void AmbaMonMain_WatchdogInactive(void);

#endif  /* AMBA_MONITOR_MAIN_INTERNAL_H */
