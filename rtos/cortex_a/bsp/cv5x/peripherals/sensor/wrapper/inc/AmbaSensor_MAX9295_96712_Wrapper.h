/**
 *  @file AmbaSensor_MAX9295_96712_Wrapper.h
 *
 *  Copyright (c) 2022 Ambarella International LP
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
 *  @details Control APIs of MAX9295_96712 driver wrapper
 *
 */

#ifndef MAX9295_96712_WRAPPER_H
#define MAX9295_96712_WRAPPER_H

#include "AmbaSensor.h"
#include "AmbaSbrg_Max9295_96712.h"

#define MX01_WRAPPER_NUM_MAX_SENSOR_COUNT 4U

/* SensorID definition */
#define MX01_WRAPPER_SENSOR_ID_CHAN_A   (0x00000010U) /* sensor on MAX9295 connecting with Channel-A of MAX96712 */
#define MX01_WRAPPER_SENSOR_ID_CHAN_B   (0x00000100U) /* sensor on MAX9295 connecting with Channel-B of MAX96712 */
#define MX01_WRAPPER_SENSOR_ID_CHAN_C   (0x00001000U) /* sensor on MAX9295 connecting with Channel-C of MAX96712 */
#define MX01_WRAPPER_SENSOR_ID_CHAN_D   (0x00010000U) /* sensor on MAX9295 connecting with Channel-D of MAX96712 */

typedef struct {
    UINT32  InputClk;                                    /* Sensor side input clock frequency */
    UINT32  DataRate;                                    /* output bit/pixel clock frequency from image sensor */
    UINT8   NumDataLanes;                                /* active data channels */
    UINT8   NumDataBits;                                 /* pixel data bit depth */
} MX01_WRAPPER_SENSOR_INFO_s;

typedef struct {
    const AMBA_SENSOR_OBJ_s *pSensorObj;
    const AMBA_SENSOR_OUTPUT_INFO_s *pOutputInfo;
    MAX9295_96712_SERDES_CONFIG2_s *pSerdesCfg;
    UINT32 (*GetSensorInfo)(UINT32 ModeID, MX01_WRAPPER_SENSOR_INFO_s *pMx01SensorInfo);
    UINT32 (*GetFrameRateInfo)(UINT32 ModeID, AMBA_VIN_FRAME_RATE_s *pMx01FrameRate);
    UINT32 (*GetSerdesCfgAddr)(const AMBA_SENSOR_CHANNEL_s *pChan, ULONG *pSerdesCfg);
    UINT32 (*ConfigVinPre)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode);
    UINT32 (*ConfigVin)(UINT32 VinID, const AMBA_SENSOR_MODE_INFO_s *pModeInfo);
    UINT32 (*ConfigVinPost)(const AMBA_SENSOR_CHANNEL_s *pChan, const AMBA_SENSOR_CONFIG_s *pMode);
} MX01_SENSOR_OBJ_s;

#endif /* MAX9295_96712_WRAPPER_H */
