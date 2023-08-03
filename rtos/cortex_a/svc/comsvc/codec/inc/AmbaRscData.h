/**
*  @file AmbaRscData.h
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
*  @details Amba record source - data
*
*/

#ifndef AMBA_RSC_DATA_H
#define AMBA_RSC_DATA_H

#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"

#define AMBA_RSC_DATA_STATIS_GET      (0U)
#define AMBA_RSC_DATA_RESET           (1U)
#define AMBA_RSC_DATA_DUMMY_EOS       (2U)

#define TICK_TYPE_AUDIO     (0x0000U)
#define TICK_TYPE_SYS       (0x0001U)
#define TICK_TYPE_MAX_NUM   (0x0002U)

typedef struct /*_AMBA_IMG_ALGO_AE_INFO_s_*/ {
    FLOAT ExposureTime;
    FLOAT Gain;
} AMBA_DATG_AE_s;

typedef struct /*_AMBA_IMG_ALGO_AWB_INFO_s_*/ {
    UINT32 Wgc[3];
} AMBA_DATG_AWB_s;

/* 3A data */
typedef struct {
    UINT32          ExposureNum;
    AMBA_DATG_AE_s   Ae[4];
    AMBA_DATG_AWB_s  Awb;
} AMBA_DATG_AAA_s;

/* canbus data */
typedef struct {
    UINT8   TurnLightStatus;
    UINT8   GearStatus;
    DOUBLE  XferSpeed;
    UINT32  WheelDir;
    DOUBLE  XferWheelAngle;
    DOUBLE  XferWheelAngleSpeed;
} AMBA_DATG_CANBUS_s;

/* ethernet PTP data */
typedef struct {
    UINT32  Sec;
    UINT32  NanoSec;
} AMBA_DATG_ETH_PTP_s;

typedef struct {
    UINT32              VinBits;
    UINT32              FovBits;
    UINT64              CapTime[TICK_TYPE_MAX_NUM];
    UINT8               Eos;
    AMBA_DATG_AAA_s      AaaData[AMBA_DSP_MAX_VIEWZONE_NUM];
    AMBA_DATG_CANBUS_s   CanBus;
    AMBA_DATG_ETH_PTP_s  EthPTP;
} AMBA_DATG_s;

UINT32 AmbaRscData_Create(AMBA_REC_SRC_s *pRda);
UINT32 AmbaRscData_Destroy(const AMBA_REC_SRC_s *pRda);
void   AmbaRscData_Control(const AMBA_REC_SRC_s *pRda, UINT32 CtrlType, void *pParam);
void   AmbaRscData_EvalMemSize(UINT32 SrcType, UINT32 MaxQueLen, UINT32 *pSize);
UINT32 AmbaRscData_CanToText(char *pText, const AMBA_DATG_s *pDataG);
UINT32 AmbaRscData_PtpToText(char *pText, const AMBA_DATG_s *pDataG);

void   AmbaRscData_TextToCan(const char *pText, AMBA_DATG_s *pDataG);

#endif  /* AMBA_RSC_DATA_H */
