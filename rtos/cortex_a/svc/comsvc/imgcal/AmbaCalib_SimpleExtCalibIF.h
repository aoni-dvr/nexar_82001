/**
 *  @file AmbaCalib_SimpleExtCalibIF.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Simple method for camera extrinsic factor calibration
 *
 */
#ifndef AMBA_CALIB_SIMPLE_EXT_CALIB_IF_H
#define AMBA_CALIB_SIMPLE_EXT_CALIB_IF_H

#include <AmbaCalib_Def.h>
#include <AmbaCalib_EmirrorDef.h>

#define AMBA_CAL_INFINITE_DIST (1.0e+106)

typedef struct {
    AMBA_CAL_CAM_V2_s CamInfo;            /* Position in AMBA e-mirror world coordinate system. Unit: mm
                                           * OpticalCenter. Unit: pixel
                                           * Sensor.CellSize. Unit: mm */
    DOUBLE FocalLength;                   /* Unit: mm    */
    AMBA_CAL_ROI_s ImageInfo;             /* Unit: pixel */
    AMBA_CAL_ROI_s RoiInfo;               /* Unit: pixel */
    DOUBLE WorldHorizonY;                 /* Unit: mm    */
    DOUBLE ImageHorizonY;                 /* Unit: pixel */
    DOUBLE TuningHorRotDeg;               /* Forcing cam rotation by manual. Unit: degree */
} AMBA_CAL_EXT_PITCH_CFG_s;

typedef struct {
    DOUBLE PitchDegree;
    AMBA_CAL_WORLD_RAW_POINT_s CalibPoints[EMIR_CALIB_POINT_NUM];
    AMBA_CAL_EM_CAM_CALIB_DATA_s CalibData;
} AMBA_CAL_EXT_PITCH_DATA_s;

UINT32 AmbaCal_ExtGetSimpCorrPitchBufSz(SIZE_t *pSize);
UINT32 AmbaCal_ExtSimpCorrPitch(const AMBA_CAL_EXT_PITCH_CFG_s *pCfg, const void *pWorkingBuf, AMBA_CAL_EXT_PITCH_DATA_s *pOutput);

#endif /* AMBA_CALIB_SIMPLE_EXT_CALIB_IF_H */
