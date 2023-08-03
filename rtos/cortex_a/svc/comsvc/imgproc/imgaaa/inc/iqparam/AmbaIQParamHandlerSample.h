/**
 * @file AmbaIQParamHandlerSample.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @Description    :: Implementation of IQ param handler
 *
 *
 */
#ifndef AMBA_CSV_IQPARAM_HANDLER_SAMPLE_H__
#define AMBA_CSV_IQPARAM_HANDLER_SAMPLE_H__

// #include ".svc_autogen"

#include "AmbaImg_AaaDef.h"
#ifdef SVC_AMBAIMG_ADJUSTMENT
#include SVC_AMBAIMG_ADJUSTMENT
#endif

#define PATHSIZE  100U


/*sensorID*/
#define IQ_SSR_DUMMY                     (0U)
#define IQ_SSR_IMX117                    (1U)
#define IQ_SSR_OV2710                    (2U)
#define IQ_SSR_OV10823                   (3U)
#define IQ_SSR_IMX206                    (4U)
#define IQ_SSR_AR0330_PARALLEL           (5U)
#define IQ_SSR_AR0230                    (6U)
#define IQ_SSR_OV9750                    (7U)
#define IQ_SSR_MN34120                   (8U)
#define IQ_SSR_B5_OV4689                 (9U)
#define IQ_SSR_IMX290                    (10U)
#define IQ_SSR_MN34222                   (11U)
#define IQ_SSR_IMX377                    (12U)
#define IQ_SSR_IMX317                    (13U)
#define IQ_SSR_B5_OV9750                 (14U)
#define IQ_SSR_B5_AR0230                 (15U)
#define IQ_SSR_B5_OV10640_R1E            (16U)
#define IQ_YUVRAW                        (17U)
#define IQ_SSR_AR0237                    (18U)
#define IQ_SSR_IMX277                    (19U)
#define IQ_SSR_IMX172                    (20U)
#define IQ_SSR_IMX183                    (21U)
#define IQ_SSR_IMX078                    (22U)
#define IQ_SSR_OV2718                    (23U)
#define IQ_YUV_TI5150                    (24U)
#define IQ_YUV_NVP6114A                  (25U)
#define IQ_SSR_B5_IMX206                 (26U)
#define IQ_SSR_IMX377Q                   (27U)
#define IQ_SSR_IMX377D                   (28U)
#define IQ_SSR_IMX204                    (29U)
#define IQ_SSR_IMX317D                   (30U)
#define IQ_SSR_IMX123                    (31U)
#define IQ_SSR_B5_IMX317                 (32U)
#define IQ_SSR_IMX227                    (33U)
#define IQ_SSR_IMX577                    (34U)
#define IQ_SSR_B6N_IMX290                (35U)
#define IQ_SSR_OV4689                    (36U)
#define IQ_YUV_LT6911                    (37U)
#define IQ_SSR_B6N_IMX390                (38U)
#define IQ_SSR_B6N_IMX424                (39U)
#define IQ_SSR_MAX9295_9296_IMX390       (40U)
#define IQ_SSR_MAX9295_96712_IMX390      (41U)
#define IQ_SSR_B6N_IMX390_RCCB           (42U)
#define IQ_SSR_MAX96707_9286_AR0144      (43U)
#define IQ_YUV_AMBAVOUT                  (44U)
#define IQ_SSR_AR0239                    (45U)
#define IQ_SSR_MAX9295_9296_IMX390_24    (46U)
#define IQ_SSR_IMX490                    (47U)
#define IQ_SSR_MAX9295_9296_IMX490       (48U)
#define IQ_SSR_OX03A                     (49U)
#define IQ_SSR_MAX9295_9296_OX03A        (50U)
#define IQ_SSR_MAX9295_9296_AR0239       (51U)
#define IQ_SSR_OV2775                    (52U)
#define IQ_SSR_OV2312_TD_RGBIR           (53U)
#define IQ_SSR_MAX9295_9296_OV9284       (54U)
#define IQ_SSR_MAX9295_9296_AR0233       (55U)
#define IQ_SSR_MAX9295_9296_AR0147       (56U)
#define IQ_SSR_VG6768                    (57U)
#define IQ_SSR_MAX9295_9296_VG6768       (58U)
#define IQ_SSR_MAX9295_9296_OV2778       (59U)
#define IQ_SSR_MAX9295_9296_OX01D        (60U)
#define IQ_SSR_MAX_9295_9296_RCCB_AR0220 (61U)
#define IQ_SSR_MAX_9295_9296_RCCC_AR0220 (62U)
#define IQ_SSR_MAX_9295_9296_AR0820      (63U)
#define IQ_SSR_MAX9295_96712_IMX390_24   (64U)
#define IQ_SSR_MAX9295_9296_IMX424       (65U)
#define IQ_SSR_MAX9295_9296_IMX390RCCB   (66U)
#define IQ_SSR_MAX9295_96712_IMX224      (67U)
#define IQ_SSR_TI953_954_AR0220_RCCB     (68U)
#define IQ_SSR_TI953_954_AR0233          (69U)
#define IQ_SSR_MAX9295_96712_AR0233      (70U)
#define IQ_SSR_MAX_9295_9296_AR0820_RCCB (71U)
#define IQ_SSR_MAX_9295_9296_IMX224      (72U)
#define IQ_SSR_OV2312_TD_IR              (73U)
#define IQ_SSR_OV2312_SIMUL_RGBIR        (74U)
#define IQ_SSR_OV2312_SIMUL_IR           (75U)
#define IQ_SSR_TI913_954_OV10635         (76U)
#define IQ_SSR_MAX96717_96716_AR0233     (77U)
#define IQ_SENSOR_MAX9295_9296_VG1762    (78U)
#define IQ_SENSOR_MAX9295_9296_VG1762D   (79U)
#define IQ_SENSOR_MAX9295_9296_VG1762DIR (80U)
#define IQ_SSR_IMX586                    (81U)
#define IQ_SSR_IMX686                    (82U)
#define IQ_SSR_OV48C40                   (83U)
#define IQ_SSR_MAX9295_9296_OX03C        (84U)
#define IQ_SSR_IMX334                    (85U)
#define IQ_SSR_IMX455                    (86U)
#define IQ_SSR_IMX390_24                 (87U)
#define IQ_SSR_MAX9295_9296_OX03F        (88U)
#define IQ_SSR_IMX728                    (89U)
#define IQ_SSR_OX05B1S                   (90U)
#define IQ_SSR_MAX9295_9296_OX03C_ADAS   (91U)
#define IQ_SSR_MAX9295_9296_OX03C_EMR    (92U)
#define IQ_SSR_MAX9295_9296_OX03F_ADAS   (93U)
#define IQ_SSR_MAX9295_9296_OX03F_EMR    (94U)
#define IQ_SSR_K351P                     (95U)
#define IQ_SSR_IMX415                    (96U)
#define IQ_SSR_GC2053                    (97U)
#define IQ_SSR_GC4653                    (98U)
#define IQ_INPUT_DEVICE_NUM              (99U)

/*IMG_MODE define*/
#define IMG_MODE_TV                        (0U)
#define IMG_MODE_PC                        (1U)

/*max table number define*/
#define ADJ_VIDEO_TABLE_MAX_NO             (3U)
#define ADJ_PHOTO_TABLE_MAX_NO             (2U)
#define ADJ_STILL_LISO_TABLE_MAX_NO        (2U)
#define ADJ_STILL_HISO_TABLE_MAX_NO        (1U)
#define SCENE_TABLE_MAX_NO                 (5U)
#define SCENE_TABLE_CONTAIN_SETS           (8U)
#define AAA_TABLE_MAX_NO                   (2U)
#define DE_VIDEO_TABLE_MAX_NO              (1U)
#define DE_STILL_TABLE_MAX_NO              (1U)
#define ADJ_VIDEO_MSM_TABLE_MAX_NO         (1U)
#define ADJ_VIDEO_MSH_TABLE_MAX_NO         (1U)
#define CC_SET_MAX_NO                      (2U)

/*Struct define*/

typedef struct {
    UINT16          DEColorTableMap[DIGITAL_LAST];
    UINT16          SceneColorTableMap[SCENE_TABLE_CONTAIN_SETS*SCENE_TABLE_MAX_NO];
} COLOR_TABLE_MAP;


typedef struct {
    char path[PATHSIZE];
} TABLE_PATH;

typedef struct {
    TABLE_PATH      videoCC[DEF_CC_TABLE_NO];
    TABLE_PATH      stillCC[DEF_CC_TABLE_NO];
} CC_SET_PATH_s;

typedef struct {
    //ADJ Table Path
    TABLE_PATH      ADJTable;
    TABLE_PATH      ImgParam;
    TABLE_PATH      aaaDefault[AAA_TABLE_MAX_NO];

    TABLE_PATH      video[ADJ_VIDEO_TABLE_MAX_NO];
    TABLE_PATH      videoMSM[ADJ_VIDEO_MSM_TABLE_MAX_NO];
    TABLE_PATH      videoMSH[ADJ_VIDEO_MSH_TABLE_MAX_NO];
    TABLE_PATH      photo[ADJ_PHOTO_TABLE_MAX_NO];

    TABLE_PATH      stillLISO[ADJ_STILL_LISO_TABLE_MAX_NO];
    TABLE_PATH      stillHISO[ADJ_STILL_HISO_TABLE_MAX_NO];
    TABLE_PATH      stillIdxInfo;

    //CC Table Path
    CC_SET_PATH_s   CCSetPaths[CC_SET_MAX_NO]; //10 is the maxium num of cc sets

    //Scene Table path
    TABLE_PATH      scene[SCENE_TABLE_MAX_NO];

    //digital effect
    TABLE_PATH      DEVideo[DE_VIDEO_TABLE_MAX_NO];
    TABLE_PATH      DEStill[DE_STILL_TABLE_MAX_NO];




} IQ_TABLE_PATH_s;

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS) && !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52)
typedef struct {
    TABLE_PATH      CcReg;
} IQ_TABLE_DSP_PATH_s;
#endif

typedef struct {
    UINT8 buf[CC_3D_SIZE];

}IQ_CC_Table;

typedef struct {
    // ADJ Params
    ADJ_TABLE_PARAM_s           ADJTableParam;
    IMG_PARAM_s                 ImgParam;
    AAA_PARAM_s                 AAAParams[AAA_TABLE_MAX_NO];

    // Video Params
    ADJ_VIDEO_PARAM_s           videoParams[ADJ_VIDEO_TABLE_MAX_NO];
    VIDEO_MOTION_PARAM_s        videoMSM[ADJ_VIDEO_MSM_TABLE_MAX_NO];
    VIDEO_MOTION_PARAM_s        videoMSH[ADJ_VIDEO_MSH_TABLE_MAX_NO];
    ADJ_VIDEO_PARAM_s           photoPreviewParams[ADJ_PHOTO_TABLE_MAX_NO];

    // Still Params
    STILL_LISO_FILTER_PARAM_s   stillLISOParams[ADJ_STILL_LISO_TABLE_MAX_NO];
    STILL_HISO_FILTER_PARAM_s   stillHISOParams[ADJ_STILL_HISO_TABLE_MAX_NO];
    ADJ_STILL_IDX_INFO_s        stillIdxInfoParams;
    //CC Params
    IQ_CC_Table                 videoCC[DEF_CC_TABLE_NO];
    IQ_CC_Table                 stillCC[DEF_CC_TABLE_NO];

    //scene
    SCENE_DATA_s                sceneParams[SCENE_TABLE_CONTAIN_SETS*SCENE_TABLE_MAX_NO];

    //digital effect
    DE_PARAM_s                  DEVideoParams[DE_VIDEO_TABLE_MAX_NO];
    DE_PARAM_s                  DEStillParams[DE_STILL_TABLE_MAX_NO];
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
    UINT8                       CCRegParam[CC_REG_SIZE];
#endif
} IQ_PARAM_s;

typedef struct {
    UINT8   ImgParam;
    UINT8   AAAParams[AAA_TABLE_MAX_NO];

    //Video Params
    UINT8   videoParams[ADJ_VIDEO_TABLE_MAX_NO];
    UINT8   videoMSM[ADJ_VIDEO_MSM_TABLE_MAX_NO];
    UINT8   videoMSH[ADJ_VIDEO_MSH_TABLE_MAX_NO];
    UINT8   photoPreviewParams[ADJ_PHOTO_TABLE_MAX_NO];

    //Still Params
    UINT8   stillLISOParams[ADJ_STILL_LISO_TABLE_MAX_NO];
    UINT8   stillHISOParams[ADJ_STILL_HISO_TABLE_MAX_NO];
    UINT8   stillIdxInfoParams;

    // CC Params
    UINT8   videoCC[DEF_CC_TABLE_NO];
    UINT8   stillCC[DEF_CC_TABLE_NO];

    //scene Params
    UINT8   sceneParams[SCENE_TABLE_MAX_NO];

    //Digital effect
    UINT8   DEVideoParams[DE_VIDEO_TABLE_MAX_NO];
    UINT8   DEStillParams[DE_STILL_TABLE_MAX_NO];

} IQ_PARAM_LOAD_FORBID_FLAG;

typedef struct {
    // CC Params
    UINT8   videoCC[DEF_CC_TABLE_NO];
    UINT8   stillCC[DEF_CC_TABLE_NO];

} IQ_PARAM_CC_LOAD_FORBID_FLAG;


/*Load IQ param APIs*/
UINT32 Amba_IQParam_Load_CC_By_Scene(UINT32 viewID, INT32 sceneMode);
UINT32 Amba_IQParam_Load_CC_By_DE(UINT32 viewID, INT32 deMode);
UINT32 Amba_IQParam_Get_IqParams(UINT32 viewID, IQ_PARAM_s **pIQParams);
UINT32 Amba_IQParam_LoadIqParams(UINT32 viewID, UINT32 handlerID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag, UINT32 loadFromRom);
UINT32 Amba_IQParam_LoadCCParams(UINT32 viewID, UINT32 handlerID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag, const IQ_PARAM_CC_LOAD_FORBID_FLAG* pCCLoadForbidFlag, UINT32 loadFromRom);
UINT32 Amba_IQParam_Config_IQ_Table_Path(UINT32 sensorID, UINT32 HdrEnable);
UINT32 Amba_IQParam_IQ_Table_Path_Prt(UINT32 sensorID);

/*Callbacks for app layer*/
typedef void (*AMBA_IQ_PARAM_TABLE_PATH_f)(IQ_TABLE_PATH_s *pIqTblPath);
typedef UINT32 (*READ_ROM_FILE)(UINT32 NvmID, UINT32 RegionID, const char *pFileName, UINT32 StartPos, UINT32 Size, UINT8 *pDataBuf, UINT32 TimeOut);
UINT32 Amba_IQParam_RegisterIQPathProc(AMBA_IQ_PARAM_TABLE_PATH_f IqTblPathProc);
UINT32 Amba_IQParam_RegisterReadRomFile(READ_ROM_FILE pFunc);

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS) && !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52)
typedef void (*AMBA_IQ_PARAM_TABLE_DSP_PATH_f)(IQ_TABLE_DSP_PATH_s *pIqTblPath);
UINT32 Amba_IQParam_RegisterIQDspPathProc(AMBA_IQ_PARAM_TABLE_DSP_PATH_f IqTblPathProc);
#endif

UINT32 Amba_IQParam_QueryIqBufSize(UINT32 *pSize);
UINT32 Amba_IQParam_SetIqBuf(UINT32 viewID,const void *pBuf);

#endif
