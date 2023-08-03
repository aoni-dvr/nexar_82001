/**
*  @file SvcSafe.h
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
*  @details Constants and Definitions for SVC Image
*
*/

#ifndef SVC_SAFE_H
#define SVC_SAFE_H

#define SVC_NUM_VIN_SENSOR  4U

typedef enum /*_SVC_SAFE_CMD_e_*/ {
      SVC_SAFE_CMD_START = 0,
      SVC_SAFE_CMD_STOP,
      SVC_SAFE_CMD_VIN_SRC,
      SVC_SAFE_CMD_VIN,
      SVC_SAFE_CMD_SENSOR,
      SVC_SAFE_CMD_SLOW_SHUTTER,
      SVC_SAFE_CMD_BOOT_LATENCY
} SVC_SAFE_CMD_e;

typedef union /*_SVC_SAFE_CMD_MSG_u_*/ {
    UINT64 Data;
    struct {
        UINT32 Id:             8;
        UINT32 Reserved_L:    24;
        UINT32 Reserved_H:    20;
        UINT32 VinIdx:         4;
        UINT32 Cmd:            8;
    } VinSrc;
    struct {
        UINT32 Id:            32;
        UINT32 Reserved:      16;
        UINT32 SensorIdx:      4;
        UINT32 VinIdx:         4;
        UINT32 Cmd:            8;
    } Sensor;
    struct {
        UINT32 Id:             8;
        UINT32 Reserved_L:    24;
        UINT32 Reserved_H:    20;
        UINT32 VinIdx:         4;
        UINT32 Cmd:            8;
    } SlowShutter;
    struct {
        UINT32 FovId:         16;
        UINT32 SensorId:       4;
        UINT32 SerdesId:       4;
        UINT32 Reserved_L:     8;
        UINT32 Reserved_H:    20;
        UINT32 VinIdx:         4;
        UINT32 Cmd:            8;
    } Vin;
    struct {
        UINT32 FovId:         16;
        UINT32 VinId:         16;
        UINT32 VoutId:         4;
        UINT32 TdId:          16;
        UINT32 Reserved:       4;
        UINT32 Cmd:            8;
    } Liv;
    struct {
        UINT32 Timetick:      32;
        UINT32 Reserved:      20;
        UINT32 VinIdx:         4;
        UINT32 Cmd:            8;
    } BootLatency;
} SVC_SAFE_CMD_MSG_u;

typedef struct /*_SVC_SAFE_CMD_MSG_s_*/ {
    SVC_SAFE_CMD_MSG_u    Ctx;
} SVC_SAFE_CMD_MSG_s;

typedef struct /*_SVC_SAFE_SELECT_s*/ {
    UINT32 Vin;
    UINT32 Sensor[AMBA_NUM_VIN_CHANNEL];
    UINT32 Serdes[AMBA_NUM_VIN_CHANNEL];
    UINT32 FovInVin[AMBA_NUM_VIN_CHANNEL];
    UINT32 Fov;
    UINT32 Vout;
    UINT32 Td;
} SVC_SAFE_SELECT_s;

#define SVC_SAFE_VIN_SRC_SENSOR  0U
#define SVC_SAFE_VIN_SRC_YUV     1U
#define SVC_SAFE_VIN_SRC_ALT     2U

typedef struct /*_SVC_SAFE_CFG_s_*/ {
    UINT32               VinSrc[AMBA_NUM_VIN_CHANNEL];
    SVC_SAFE_SELECT_s    Select;
    UINT32               SensorId[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
    UINT32               SlowShutterId[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
    UINT32               BootLatency[AMBA_NUM_VIN_CHANNEL];
} SVC_SAFE_CFG_s;

  void SvcSafe_Init(void);
  void SvcSafe_InitEx(UINT32 CoreInclusion);
  void SvcSafe_Config(const SVC_SAFE_CFG_s *pSafeCfg);
  void SvcSafe_Stop(void);

#endif  /* SVC_SAFE_H */
