/**
 *  @file AmbaImgSensorDrv.h
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
 *  @details Constants and Definitions for Amba Image Senosr Drv
 *
 */

#ifndef AMBA_IMG_SENSOR_DRV_H
#define AMBA_IMG_SENSOR_DRV_H

#define AMBA_NUM_SWB_CHANNEL    4U

typedef struct /*_AMBA_IMG_SENSOR_WB_s_*/ {
    UINT32 Gain[AMBA_NUM_SWB_CHANNEL];
} AMBA_IMG_SENSOR_WB_s;

typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_REG_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT8 *pBuf, UINT8 Size);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_AGC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pAgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_DGC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pDgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SHR_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SVR_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSvr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_MSC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pMsc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SLS_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSls);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_WGC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pWgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_GAIN_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pAgc, UINT32 *pDgc, AMBA_IMG_SENSOR_WB_s *pWgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SHUTTER_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pShr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SVR_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr, UINT32 *pSvr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SLS_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pSls);

typedef struct /*_AMBA_IMG_SENSOR_DRV_FUNC_s_*/ {
    AMBA_IMG_SENSOR_DRV_REG_WRITE_f          RegWrite;
    AMBA_IMG_SENSOR_DRV_AGC_WRITE_f          AgcWrite;
    AMBA_IMG_SENSOR_DRV_DGC_WRITE_f          DgcWrite;
    AMBA_IMG_SENSOR_DRV_SHR_WRITE_f          ShrWrite;
    AMBA_IMG_SENSOR_DRV_SVR_WRITE_f          SvrWrite;
    AMBA_IMG_SENSOR_DRV_MSC_WRITE_f          MscWrite;
    AMBA_IMG_SENSOR_DRV_SLS_WRITE_f          SlsWrite;
    AMBA_IMG_SENSOR_DRV_WGC_WRITE_f          WgcWrite;
    AMBA_IMG_SENSOR_DRV_GAIN_CONVERT_f       GainConvert;
    AMBA_IMG_SENSOR_DRV_SHUTTER_CONVERT_f    ShutterConvert;
    AMBA_IMG_SENSOR_DRV_SVR_CONVERT_f        SvrConvert;
    AMBA_IMG_SENSOR_DRV_SLS_CONVERT_f        SlsConvert;
} AMBA_IMG_SENSOR_DRV_FUNC_s;

typedef struct /*_AMBA_IMG_SENSOR_DRV_CHAN_s_*/ {
    UINT32                        Magic;
    AMBA_IMG_SENSOR_DRV_FUNC_s    *pFunc;
} AMBA_IMG_SENSOR_DRV_CHAN_s;

extern AMBA_IMG_SENSOR_DRV_CHAN_s *AmbaImgSensorDrv_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaImgSensorDrv.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaImgSensorDrv_Init(void);

UINT32 AmbaImgSensorDrv_RegWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT8 *pBuf, UINT8 Size);

UINT32 AmbaImgSensorDrv_AgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pAgc);
UINT32 AmbaImgSensorDrv_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pDgc);
UINT32 AmbaImgSensorDrv_ShrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr);
UINT32 AmbaImgSensorDrv_SvrWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSvr);
UINT32 AmbaImgSensorDrv_MscWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pMsc);
UINT32 AmbaImgSensorDrv_SlsWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSls);
UINT32 AmbaImgSensorDrv_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pWgc);

UINT32 AmbaImgSensorDrv_GainConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pAgc, UINT32 *pDgc, AMBA_IMG_SENSOR_WB_s *pWgc);
UINT32 AmbaImgSensorDrv_ShutterConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pShr);
UINT32 AmbaImgSensorDrv_SvrConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr, UINT32 *pSvr);
UINT32 AmbaImgSensorDrv_SlsConvert(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pSls);

#endif  /* AMBA_IMG_SENSOR_DRV_H */
