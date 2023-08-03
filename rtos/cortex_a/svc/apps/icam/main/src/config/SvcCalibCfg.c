/**
 *  @file SvcCalibCfg.c
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
 *  @details svc IK config
 *
 */

#include "AmbaTypes.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaEEPROM.h"
#include "SvcCalibMgr.h"
#include "SvcCalibCfg.h"

#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaCalib_EmirrorDef.h"
#include "AmbaCalib_AVMIF.h"
#include "SvcCalibAdas.h"
#endif

#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
#include "SvcCalibStitch.h"
#endif

#ifdef CONFIG_ICAM_CV_STEREO
#include "SvcCalibStereo.h"
#endif

#define SVC_CALIB_LDC_TBL_NUM ( 3U )
#define SVC_CALIB_CA_TBL_NUM  ( 3U )
#define SVC_CALIB_WB_TBL_NUM  ( 3U )
#define SVC_CALIB_VIG_TBL_NUM ( 3U )
#ifdef CONFIG_ICAM_EEPROM_USED
#define SVC_CALIB_BPC_TBL_NUM ( 1U )
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS)
#define SVC_CALIB_ADAS_TBL_NUM  ( SVC_CALIB_ADAS_TYPE_NUM - 1U )
#endif

#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
#define SVC_CALIB_ST_LDC_TBL_NUM    (3U)
#define SVC_CALIB_ST_BLD_TBL_NUM    (1U)
#endif

/**
 * calibration configuration getting function
 * @return pointer of calibration configuration
 */
SVC_CALIB_CFG_s* SvcCalibCfg_Get(void)
{
    static SVC_CALIB_CFG_s g_SvcCalibCfg;
    static SVC_CALIB_OBJ_s g_SvcCalibObjs[] = {
        {1U, SVC_CALIB_LDC_ID,          "ldc",   0x20180412U, SVC_CALIB_LDC_TBL_NUM,    SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_LdcCmdFunc, SvcCalib_LdcShellFunc },
        {1U, SVC_CALIB_CA_ID,           "ca",    0x20180927U, SVC_CALIB_CA_TBL_NUM,     SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_CaCmdFunc,  SvcCalib_CaShellFunc  },
        {1U, SVC_CALIB_WB_ID,           "wb",    0x20181017U, SVC_CALIB_WB_TBL_NUM,     SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_WbCmdFunc,  SvcCalib_WbShellFunc  },
        {1U, SVC_CALIB_VIGNETTE_ID,     "vig",   0x20180927U, SVC_CALIB_VIG_TBL_NUM,    SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_VigCmdFunc, SvcCalib_VigShellFunc },

#ifdef CONFIG_ICAM_EEPROM_USED
        {1U, SVC_CALIB_BPC_ID,          "bpc",   0x20181012U, SVC_CALIB_BPC_TBL_NUM,    SVC_CALIB_DATA_SOURCE_USER, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_BpcCmdFunc, SvcCalib_BpcShellFunc },
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS)
        {1U, SVC_CALIB_ADAS_ID,         "adas",  0x20190307U, SVC_CALIB_ADAS_TBL_NUM,   SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_AdasCmdFunc, SvcCalib_AdasShellFunc },
#endif

#ifdef CONFIG_ICAM_IMGCAL_STITCH_USED
        {1U, SVC_CALIB_STITCH_LDC_ID,   "stldc", 0x20191017U, SVC_CALIB_ST_LDC_TBL_NUM, SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_StitchLdcCmdFunc,   SvcCalib_StitchLdcShellFunc },
        {1U, SVC_CALIB_STITCH_BLEND_ID, "stbld", 0x20191017U, SVC_CALIB_ST_BLD_TBL_NUM, SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_StitchBlendCmdFunc, SvcCalib_StitchBlendShellFunc },
#endif
#ifdef CONFIG_ICAM_CV_STEREO

        {1U, SVC_CALIB_STEREO_ID,       "stereo", 0x20200113, SVC_CALIB_STEREO_TBL_NUM, SVC_CALIB_DATA_SOURCE_NAND, "", NULL, 0U, NULL, 0U, 0U, SvcCalib_StereoCmdFunc, SvcCalib_StereoShellFunc },
#endif
    };

    g_SvcCalibCfg.NumOfCalObj = (UINT32)(sizeof(g_SvcCalibObjs)) / ((UINT32)(sizeof(g_SvcCalibObjs[0])));
    g_SvcCalibCfg.pCalObj     = g_SvcCalibObjs;

    return &g_SvcCalibCfg;
}

