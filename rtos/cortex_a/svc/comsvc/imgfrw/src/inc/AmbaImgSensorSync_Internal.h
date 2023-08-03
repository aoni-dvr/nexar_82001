/**
 *  @file AmbaImgSensorSync_Internal.h
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
 *  @details Constants and Definitions for Amba Image Sensor Sync Internal
 *
 */

#ifndef AMBA_IMG_SENSOR_SYNC_INTERNAL_H
#define AMBA_IMG_SENSOR_SYNC_INTERNAL_H

static UINT32 AmbaImgSensorSync_TimingCheck(UINT32 VinId);
static UINT32 SensorSync_TimingPortAdd(UINT32 VinId, AMBA_IMG_CHANNEL_TIMING_PORT_s *pPort, UINT32 Mode);
static UINT32 AmbaImgSensorSync_TimeWaitTo(UINT32 VinId, UINT32 Timetick);

static UINT32 Sync_Sof(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_PostSof(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_Mof(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_AdvDsp(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_AdvAik(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_AdvEof(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_AikExecute(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_SofEnd(AMBA_IMG_CHANNEL_ID_s ImageChanId);
static UINT32 Sync_Eof(AMBA_IMG_CHANNEL_ID_s ImageChanId);

static UINT32 SensorSync_SofCb(UINT32 VinId);
static UINT32 SensorSync_EofCb(UINT32 VinId);

static UINT32 SensorSync_TimingUpdate(UINT32 VinId);

static UINT32 SensorSync_Sof(UINT32 VinId);
static UINT32 SensorSync_PostSof(UINT64 Data);
static UINT32 SensorSync_Mof(UINT64 Data);
static UINT32 SensorSync_AdvDsp(UINT64 Data);
static UINT32 SensorSync_AdvEof(UINT64 Data);
static UINT32 SensorSync_AikExecute(UINT64 Data);
static UINT32 SensorSync_SofEnd(UINT32 VinId);
static UINT32 SensorSync_Eof(UINT32 VinId);
static UINT32 SensorSync_Wait(UINT32 VinId, UINT32 Id);

static UINT32 AmbaImgSensorSync_HdrPreloadSet(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_HDR_DATA_s *pHdrData);
static UINT32 AmbaImgSensorSync_FifoPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_CTRL_REG_s *pCtrl, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData, UINT32 Mode);

static UINT32 AmbaImgSensorSync_HdrPreload(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);
static UINT32 AmbaImgSensorSync_Snap(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);
static UINT32 AmbaImgSensorSync_Direct(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);
static UINT32 AmbaImgSensorSync_Write(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);

#endif  /* AMBA_IMG_SENSOR_SYNC_INTERNAL_H */
