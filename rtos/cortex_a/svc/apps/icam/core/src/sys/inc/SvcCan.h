/**
*  @file SvcCan.h
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
*  @details svc error code
*
*/

#ifndef SVC_CAN_H
#define SVC_CAN_H

#if defined(CONFIG_ICAM_CANBUS_USED)

#include "AmbaSurround.h"

typedef struct {
    DOUBLE  SpeedA;                    /**< y = Ax^2 + Bx + C */
    DOUBLE  SpeedB;
    DOUBLE  SpeedC;
    DOUBLE  WheelA;
    DOUBLE  WheelB;
    DOUBLE  WheelC;
    DOUBLE  AngleSpeedA;
    DOUBLE  AngleSpeedB;
    DOUBLE  AngleSpeedC;
} SVC_CAN_XFER_PARAM_s;

void SvcCan_Init(void);

void   SvcCan_GetRawData(AMBA_SR_CANBUS_RAW_DATA_s *pCanbusRawData);
void   SvcCan_UpdateXferParams(const SVC_CAN_XFER_PARAM_s *pParam);
UINT32 SvcCan_Transfer(const AMBA_SR_CANBUS_RAW_DATA_s *RawData,
                       AMBA_SR_CANBUS_TRANSFER_DATA_s *TransData);
UINT32 SvcCan_transYawRate(const AMBA_SR_CANBUS_TRANSFER_DATA_s *canTrans, const UINT32 wheelDir, const DOUBLE fps, const DOUBLE wheelBase, DOUBLE *yawRate);
#endif

#endif  /* SVC_CAN_H */
