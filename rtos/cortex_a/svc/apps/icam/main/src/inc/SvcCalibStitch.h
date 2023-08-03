/**
 *  @file SvcCalibStitch.h
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
 *  @details svc calibration stitch warp and blend
 *
 */

#ifndef SVC_CALIB_STITCH_H
#define SVC_CALIB_STITCH_H

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibStitchLdc.c
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CALIB_STITCH_LDC_ID    (14U)

typedef struct {
    UINT32                     Version;
    UINT32                     HorGridNum;
    UINT32                     VerGridNum;
    UINT32                     TileWidthExp;
    UINT32                     TileHeightExp;
    SVC_CALIB_VIN_SENSOR_GEO_s VinSensorGeo;
    AMBA_IK_GRID_POINT_s      *pStLdcVector;
    UINT32                     StLdcVectorLength;
} SVC_CALIB_ST_LDC_TBL_DATA_s;

UINT32 SvcCalib_StitchLdcCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_StitchLdcShellFunc(UINT32 ArgCount, char * const *pArgVector);

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcCalibStitchBlend.c
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CALIB_STITCH_BLEND_ID  (15U)
#define SVC_CALIB_STITCH_BLEND_TBL_SZ   (960U * 480U)

typedef struct {
    UINT32 Width;
    UINT32 Height;
    UINT8 *pTbl;
    UINT32 TblLength;
} SVC_CALIB_ST_BLEND_TBL_DATA_s;

UINT32 SvcCalib_StitchBlendCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_StitchBlendShellFunc(UINT32 ArgCount, char * const *pArgVector);

#endif /* SVC_CALIB_STITCH_H */
