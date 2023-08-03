/**
 *  @file AmbaImgSensorHAL_Internal.h
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
 *  @details Constants and Definitions for Amba Image Sensor HAL Internal
 *
 */

#ifndef AMBA_IMG_SENSOR_HAL_INTERNAL_H
#define AMBA_IMG_SENSOR_HAL_INTERNAL_H

#ifdef CONFIG_BUILD_IMGFRW_FIFO_PROT
#define AMBA_IMG_SENSOR_HAL_FIFO_LEVEL    (AMBA_IMG_SENSOR_HAL_FIFO_SIZE - 1U)
#endif
#define AMBA_IMG_SENSOR_HAL_SS_LEVEL    4U

typedef enum /*_AMBA_IMG_SENSOR_HAL_SS_STATUS_e_*/ {
    SS_STATE_NONE = 0,
    SS_STATE_ENTER,
    SS_STATE_MORE,
    SS_STATE_LEAVE,
    SS_STATE_LESS,
    SS_STATE_STILL
} AMBA_IMG_SENSOR_HAL_SS_STATUS_e;

UINT32 AmbaImgSensorHAL_RawCapSeqPut(UINT32 VinId, UINT32 RawCapSeq);

UINT32 AmbaImgSensorHAL_SsStatusGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_SS_STATUS_s **pSsStatus);

UINT32 AmbaImgSensorHAL_SvrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Shr, UINT32 *pSvr);
UINT32 AmbaImgSensorHAL_ShrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Shr, UINT32 *pSsIndex, UINT32 *pSsState);

UINT32 AmbaImgSensorHAL_HdrSvrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pShr, UINT32 *pSvr);
UINT32 AmbaImgSensorHAL_HdrShrCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pShr, UINT32 *pSsIndex, UINT32 *pSsState);

UINT32 AmbaImgSensorHAL_UserBufferCfg(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pMem);
 void *AmbaImgSensorHAL_UserBufferGet(AMBA_IMG_CHANNEL_ID_s ImageChanId);

UINT32 AmbaImgSensorHAL_RingPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData, UINT32 RawCapSeq);

UINT32 AmbaImgSensorHAL_DataIn(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData);
UINT32 AmbaImgSensorHAL_DataGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData);
UINT32 AmbaImgSensorHAL_ActDataIn(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData);
UINT32 AmbaImgSensorHAL_ActDataGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData);

UINT32 AmbaImgSensorHAL_FifoPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_CTRL_REG_s *pCtrl, const AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData);
UINT32 AmbaImgSensorHAL_FifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);

UINT32 AmbaImgSensorHAL_HdrFifoPut(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_HAL_HDR_DATA_s *pData);

UINT32 AmbaImgSensorHAL_HdrAgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData);
UINT32 AmbaImgSensorHAL_HdrDgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData);
UINT32 AmbaImgSensorHAL_HdrWgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData);
UINT32 AmbaImgSensorHAL_HdrShrLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData);
UINT32 AmbaImgSensorHAL_HdrDDgcLoad(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pData);

UINT32 AmbaImgSensorHAL_AgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl);
UINT32 AmbaImgSensorHAL_DgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl);
UINT32 AmbaImgSensorHAL_WgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl);
UINT32 AmbaImgSensorHAL_ShrFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl);
UINT32 AmbaImgSensorHAL_DDgcFifoTiming(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 *pCtrl);

UINT32 AmbaImgSensorHAL_SsiFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_SvrFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_ShrFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_AgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_DgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_WgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_DDgcFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_UserFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgSensorHAL_AikFifoCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);

UINT32 AmbaImgSensorHAL_SvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);
UINT32 AmbaImgSensorHAL_ShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);
UINT32 AmbaImgSensorHAL_AgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);
UINT32 AmbaImgSensorHAL_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);
UINT32 AmbaImgSensorHAL_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);
UINT32 AmbaImgSensorHAL_DDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);
UINT32 AmbaImgSensorHAL_UserWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Ctrl, UINT64 Data);
UINT32 AmbaImgSensorHAL_MscWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Data);
UINT32 AmbaImgSensorHAL_SlsWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Data);
UINT32 AmbaImgSensorHAL_AikWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User);
UINT32 AmbaImgSensorHAL_EFovWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User);

UINT32 AmbaImgSensorHAL_SlowShutter(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 SsIndex);

#endif  /* AMBA_IMG_SENSOR_HAL_INTERNAL_H */
