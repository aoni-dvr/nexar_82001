/**
 *  @file AmbaMonMain.h
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
 *  @details Constants and Definitions for Amba Monitor Main
 *
 */

#ifndef AMBA_MONITOR_MAIN_H
#define AMBA_MONITOR_MAIN_H

typedef enum /*_AMBA_MON_MAIN_CMD_e_*/ {
    AMBA_MON_CMD_START = 0,
    AMBA_MON_CMD_STOP,
    AMBA_MON_CMD_VIN_SRC,
    AMBA_MON_CMD_VIN,
    AMBA_MON_CMD_SENSOR,
    AMBA_MON_CMD_SLOW_SHUTTER,
    AMBA_MON_CMD_BOOT_LATENCY
} AMBA_MON_MAIN_CMD_e;

typedef union /*_AMBA_MON_MAIN_CMD_MSG_u_*/ {
    UINT64 Data;
    struct {
        UINT32 Var:           32;
        UINT32 Reserved:      16;
        UINT32 Msg:            8;
        UINT32 Cmd:            8;
    } Bits;
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
} AMBA_MON_MAIN_CMD_MSG_u;

typedef struct /*_AMBA_MON_MAIN_CMD_MSG_s_*/ {
    AMBA_MON_MAIN_CMD_MSG_u    Ctx;
} AMBA_MON_MAIN_CMD_MSG_s;

#define AMBA_MON_VIN_SRC_SENSOR  0U
#define AMBA_MON_VIN_SRC_YUV     1U
#define AMBA_MON_VIN_SRC_ALT     2U

  void AmbaMonMain_SysInit(void);
  void AmbaMonMain_SysInitEx(UINT32 CoreInclusion);
  void AmbaMonMain_Config(UINT64 Param);

/* timing debug only */
UINT32 AmbaMonMain_TimingMarkPutEx(UINT32 VinId, const char *pName);
UINT32 AmbaMonMain_TimingMarkPutExFov(UINT32 FovId, const char *pName);
UINT32 AmbaMonMain_TimingMarkPutExVout(UINT32 VoutId, const char *pName);
  void AmbaMonMain_TimingMarkPut(UINT32 VinId, const char *pName);
  void AmbaMonMain_TimingMarkPutByFov(UINT32 FovId, const char *pName);
  void AmbaMonMain_TimingMarkPutByVout(UINT32 VoutId, const char *pName);

#endif  /* AMBA_MONITOR_MAIN_H */
