/**
 *  @file SvcImg.c
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
 *  @details Svc Image
 *
 */

#include "AmbaTypes.h"
#include "AmbaShell.h"
#include "AmbaVIN_Def.h"
#if 0
#include "AmbaWrap.h"
#endif
#include "AmbaSensor.h"

#include "AmbaDSP_ImageDefine.h"

#include "AmbaDSP_Liveview.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcLiveview.h"
#include "SvcErrCode.h"
#include "SvcVinSrc.h"
#include "SvcCmd.h"

#include "SvcImg.h"
#include "AmbaImgMain.h"

#include "AmbaImg_AaaDef.h"

typedef union /*_SVC_IMG_MEM_INFO_u_*/ {
    SVC_IMG_MEM_ADDR         Data;
    UINT32                   *pUint32;
    void                     *pVoid;
    SVC_IMG_ALGO_MSG_s       *pAlgoMsg;
    void                     **pPVoid;
#ifdef CONFIG_BUILD_IMGFRW_EFOV
    SVC_IMG_EFOV_TX_CB_f     pFunc;
#endif
    SVC_IMG_CMD_MSG_RET_s    *pRetMsg;
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
    AMBA_IK_WARP_INFO_s      *pWarpInfo;
#endif
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
    SVC_IMG_ISO_CB_f         pIsoCb;
#endif
} SVC_IMG_MEM_INFO_u;

typedef struct /*_SVC_IMG_MEM_INFO_s_*/ {
    SVC_IMG_MEM_INFO_u    Ctx;
} SVC_IMG_MEM_INFO_s;

#define SVC_IMG_AAA_GROUP_NUM   4U

static SVC_IMG_INFO_s GNU_SECTION_NOZEROINIT SvcImgInfo;

static UINT32 SvcImg_AvmActive = 0U;
static UINT32 SvcImg_AvmAlgoId = 0xFFFFU;
static UINT32 SvcImg_AvmActiveEx = 0U;

/**
 *  Svc image memory sie query
 *  @return memory size
 */
UINT32 SvcImg_MemSizeQuery(void)
{
    /* image memory size query */
    return AmbaImgMain_MemSizeQuery();
}

/**
 *  Svc image memory init
 *  @param[in] pMem pointer to the memory buffer
 *  @param[in] MemSizeInByte memory size (byte)
 */
void SvcImg_MemInit(void *pMem, UINT32 MemSizeInByte)
{
    /* image memory init */
    AmbaImgMain_MemInit(pMem, MemSizeInByte);
}

/**
 *  Svc image iq memory sie query
 *  @param[in] MaxAlgoNum max algorithm number
 *  @return memory size
 */
UINT32 SvcImg_IqMemSizeQuery(UINT32 MaxAlgoNum)
{
    /* image iq memory size query */
    return AmbaImgMain_IqMemSizeQuery(MaxAlgoNum);
}

/**
 *  Svc image iq memory init
 *  @param[in] MaxAlgoNum max algorithm number
 *  @param[in] pMem pointer to the memory buffer
 *  @param[in] MemSizeInByte memory size (byte)
 */
void SvcImg_IqMemInit(UINT32 MaxAlgoNum, void *pMem, UINT32 MemSizeInByte)
{
    /* image memory init */
    AmbaImgMain_IqMemInit(MaxAlgoNum, pMem, MemSizeInByte);
}

/**
 *  Svc image algo memory sie query
 *  @param[in] MaxAlgoNum max algorithm number
 *  @param[in] MaxExposureNum max exposure number
 *  @return memory size
 */
UINT32 SvcImg_AlgoMemSizeQuery(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)
{
    /* image algo memory size query */
    return AmbaImgMain_AlgoMemSizeQuery(MaxAlgoNum, MaxExposureNum);
}

/**
 *  Svc image algo memory init
 *  @param[in] pMem pointer to the memory buffer
 *  @param[in] MemSizeInByte memory size (byte)
 */
void SvcImg_AlgoMemInit(void *pMem, UINT32 MemSizeInByte)
{
    /* image algo memory init */
    AmbaImgMain_AlgoMemInit(pMem, MemSizeInByte);
}

/**
 *  Svc image info reset
 *  @param[in] pSvcImgInfo pointer to the svc image info
 *  @note this function is intended for internal use only
 */
static void SvcImg_InfoReset(SVC_IMG_INFO_s *pSvcImgInfo)
{
    /* image info reset */
    AmbaSvcWrap_MisraMemset(pSvcImgInfo, 0, sizeof(SVC_IMG_INFO_s));
}

/**
 *  Svc image init
 *  @param[in] MaxAlgoNum max algorithm number
 *  @param[in] MaxExposureNum max exposure number
 */
void SvcImg_Init(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)
{
    /* image sys init */
    AmbaImgMain_SysInit();
    /* image algo init */
    AmbaImgMain_AlgoInit(MaxAlgoNum, MaxExposureNum);

    /* svc image info reset */
    SvcImg_InfoReset(&SvcImgInfo);
}

/**
 *  Svc image config
 *  @param[in] pImgCfg pointer to image configuration data
 *  @param[in] IqSecId iq section id
 */
void SvcImg_ConfigEx(const SVC_IMG_CFG_s *pImgCfg, UINT32 IqSecId)
{
    UINT32 FuncRetVal;
    UINT32 i, j, k, m;

    UINT32 VinId;
    UINT32 SensorNumInVin;

    UINT16 Width;
    UINT16 Height;

    UINT32 ChkCnt;
    UINT32 FovSelectBitsInSensor;

    UINT32 HdrEnable;
    UINT32 ExposureNum;
    UINT32 LCeEnable;

    UINT8 ChainIndex;

    UINT32 ChainCnt;
    UINT32 AlgoCnt = 0U;

    UINT32 AaaGroupId = 0U;
    UINT32 AaaGroupFovMsb = 0U;

    UINT32 AccVinBits[SVC_IMG_AAA_GROUP_NUM];
    UINT32 AccZoneBits[SVC_IMG_AAA_GROUP_NUM];

    UINT32 LastVinId[SVC_IMG_AAA_GROUP_NUM];
    UINT32 LastChainId[SVC_IMG_AAA_GROUP_NUM];

    UINT32 LastAlgoId[SVC_IMG_AAA_GROUP_NUM];

     INT32 n;

    UINT32 AvmCnt = 0U;
    UINT32 AvmGroupId = 0U;
    UINT32 AvmGroupFovMsb = 0U;

    UINT32 AvmAccZoneBits[SVC_IMG_AAA_GROUP_NUM];

    UINT32 AvmLastVinId[SVC_IMG_AAA_GROUP_NUM];
    UINT32 AvmLastChainId[SVC_IMG_AAA_GROUP_NUM];

    UINT8 IqId;

    UINT32 AdjCnt = 0U;
    UINT32 AdjGroupId = 0U;

    UINT32 AdjLastZoneId[SVC_IMG_AAA_GROUP_NUM];
    UINT32 AdjAccZoneBits[SVC_IMG_AAA_GROUP_NUM];

    UINT32 EisCnt = 0U;
    UINT32 EisGroupId = 0U;

    UINT32 EisLastZoneId[SVC_IMG_AAA_GROUP_NUM];
    UINT32 EisAccZoneBits[SVC_IMG_AAA_GROUP_NUM];

    UINT32 MasterZoneId;

    UINT32 MasterVinId;
    UINT32 MasterChainId;
    UINT32 MasterAlgoId;

    UINT16 TempId;
    UINT8 HdrId;

    UINT32 VinSrc = 0U;
    UINT32 PipeMode;

    SVC_LIV_INFO_s LivInfo;

    SVC_IMG_CMD_MSG_s CmdMsg;

    AMBA_SENSOR_CHANNEL_s SensorChan;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;

    /* params zero init */
    for (i = 0U; i < SVC_IMG_AAA_GROUP_NUM; i++) {
        /* aaa */
        AccVinBits[i] = 0U;
        AccZoneBits[i] = 0U;
        LastVinId[i] = 0U;
        LastChainId[i] = 0U;
        LastAlgoId[i] = 0U;
        /* avm */
        AvmAccZoneBits[i] = 0U;
        AvmLastVinId[i] = 0U;
        AvmLastChainId[i] = 0U;
        /* fov */
        AdjLastZoneId[i] = 0U;
        AdjAccZoneBits[i] = 0U;
        /* eis */
        EisLastZoneId[i] = 0U;
        EisAccZoneBits[i] = 0U;
    }

    /* image info reset */
    SvcImg_InfoReset(&SvcImgInfo);

    /* liv info get */
    SvcLiveview_InfoGet(&LivInfo);

    /* vin select bits update */
    SvcImgInfo.Cfg.VinSelectBits = pImgCfg->VinSelectBits;

    /*
     * image channel reset
     */
    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
        /* cmd: image channel select (0xFF: reset) */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanSelect.ChainIndex = 0xFFU;
        CmdMsg.Ctx.ImageChanSelect.Msg = (UINT8) SVC_CHAN_MSG_SELECT;
        CmdMsg.Ctx.ImageChanSelect.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
        AmbaImgMain_Config(i, CmdMsg.Ctx.Data);
    }

    /*
     *  vin cap info & pipe mode/output info
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* vin cap width & height */
            Width = LivInfo.pVinCapWin[i].SubChCfg[0].CaptureWindow.Width;
            Height = LivInfo.pVinCapWin[i].SubChCfg[0].CaptureWindow.Height/LivInfo.pVinCapWin[i].SubChCfg[0].IntcNum;
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division? */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* cmd: vin cap */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.VinCap.Width = Width;
                        CmdMsg.Ctx.VinCap.Height = Height;
                        CmdMsg.Ctx.VinCap.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.VinCap.Cmd = (UINT8) SVC_VIN_CMD_CAP;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                        /* fov num (TD) */
                        ChkCnt = k;
                        /* fov loop */
                        for (m = 0U; m < *(LivInfo.pNumFov); m++) {
                            /* vin check */
                            if ((((UINT32) LivInfo.pFovCfg[m].VinId) & 0xFFU) == VinId) {
                                /* fov check */
                                if ((pImgCfg->FovSelectBitsInSensor[VinId][j] & (((UINT32) 1U) << LivInfo.pFovCfg[m].ViewZoneId)) > 0U) {
                                    if (ChkCnt == 0U) {
                                        /* cmd: pipe mode */
                                        CmdMsg.Ctx.Data = 0ULL;
#ifdef DSP_DRAM_PIPE_NUM
                                        PipeMode = LivInfo.pFovCfg[m].DramUtilityPipe;
#else
                                        PipeMode = LivInfo.pDspRes->LiveviewResource.VideoPipe;
#endif
                                        if (PipeMode == SVC_VDOPIPE_DRAMEFCY) {
                                            CmdMsg.Ctx.PipeMode.Id = (UINT8) SVC_IMAGE_PIPE_DRAM_EFCY;
                                        } else {
                                            CmdMsg.Ctx.PipeMode.Id = (UINT8) SVC_IMAGE_PIPE_NORMAL;
                                        }
                                        CmdMsg.Ctx.PipeMode.ZoneIdx = (UINT8) LivInfo.pFovCfg[m].ViewZoneId;
                                        CmdMsg.Ctx.PipeMode.ChainId = (UINT8) ChainCnt;
                                        CmdMsg.Ctx.PipeMode.Cmd = (UINT8) SVC_PIPE_CMD_MODE;
                                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                        /* cmd: pipe out */
                                        CmdMsg.Ctx.Data = 0ULL;
                                        CmdMsg.Ctx.PipeOut.Width = LivInfo.pFovCfg[m].MainWidth;
                                        CmdMsg.Ctx.PipeOut.Height = LivInfo.pFovCfg[m].MainHeight;
                                        CmdMsg.Ctx.PipeOut.ZoneIdx = (UINT8) LivInfo.pFovCfg[m].ViewZoneId;
                                        CmdMsg.Ctx.PipeOut.ChainId = (UINT8) ChainCnt;
                                        CmdMsg.Ctx.PipeOut.Cmd = (UINT8) SVC_PIPE_CMD_OUT;
                                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                        /* fov found (TD) */
                                        if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                                           break;
                                        }
                                    }
                                    /* fov countdown (TD) */
                                    if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                                        ChkCnt--;
                                    }
                                }
                            }
                        }
                        ChainCnt++;
                    }
                }
            }
        }
    }

    /*
     *  hdr config
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* sensor channel */
                    SensorChan.VinID = VinId;
                    SensorChan.SensorID = pImgCfg->VinSensorId[VinId][j];
                    /* sensor status */
                    FuncRetVal = AmbaSensor_GetStatus(&SensorChan, &SensorStatus);
                    if (FuncRetVal == SENSOR_ERR_NONE) {
                        /* sensor mode info */
                        FuncRetVal = AmbaSensor_GetModeInfo(&SensorChan, &(SensorStatus.ModeInfo.Config), &(SensorStatus.ModeInfo));
                        if (FuncRetVal == SENSOR_ERR_NONE) {
                            if ((SensorStatus.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_MULTI_SLICE) ||
                                (SensorStatus.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_VIRTUAL_CHANNEL)) {
                                /* dol hdr */
                                HdrEnable = 1U;
                                ExposureNum = SensorStatus.ModeInfo.HdrInfo.ActiveChannels;
                                LCeEnable = 1U;
                            } else if (SensorStatus.ModeInfo.HdrInfo.HdrType == AMBA_SENSOR_HDR_TYPE_INTERNAL_BLEND) {
                                /* sensor hdr */
                                HdrEnable = 2U;
                                ExposureNum = SensorStatus.ModeInfo.HdrInfo.ActiveChannels;
                                LCeEnable = 1U;
                            } else {
                                /* linear */
                                HdrEnable = 0U;
                                ExposureNum = 1U;
                                LCeEnable = 0U;
                            }
                        } else {
                            /* mode info fail */
                            HdrEnable = 0U;
                            ExposureNum = 1U;
                            LCeEnable = 0U;
                        }
                    } else {
                        /* status fail */
                        HdrEnable = 0U;
                        ExposureNum = 1U;
                        LCeEnable = 0U;
                    }
                    /* time division? */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* is ce in linear? */
                        if (HdrEnable == 0U) {
                            ChkCnt = k;
                            for (m = 0U; m < *(LivInfo.pNumFov); m++) {
                                if ((((UINT32) LivInfo.pFovCfg[m].VinId) & 0xFFU) == VinId) {
                                    if ((pImgCfg->FovSelectBitsInSensor[VinId][j] & (((UINT32) 1U) << LivInfo.pFovCfg[m].ViewZoneId)) > 0U) {
                                        if (ChkCnt == 0U) {
                                            if ((pImgCfg->FovCfg[LivInfo.pFovCfg[m].ViewZoneId].LinearCE & 0x7FU) > 0U) {
                                                /* ce in linear */
                                                LCeEnable = 1U;
                                            }
                                            break;
                                        }
                                        ChkCnt--;
                                    }
                                }
                            }
                        } else {
                            /* force to disable? (TBD) in hdr */
                            ChkCnt = k;
                            for (m = 0U; m < *(LivInfo.pNumFov); m++) {
                                if ((((UINT32) LivInfo.pFovCfg[m].VinId) & 0xFFU) == VinId) {
                                    if ((pImgCfg->FovSelectBitsInSensor[VinId][j] & (((UINT32) 1U) << LivInfo.pFovCfg[m].ViewZoneId)) > 0U) {
                                        if (ChkCnt == 0U) {
                                            if ((pImgCfg->FovCfg[LivInfo.pFovCfg[m].ViewZoneId].LinearCE & 0x80U) > 0U) {
                                                /* force to disable ce */
                                                LCeEnable = 0U;
                                            }
                                            break;
                                        }
                                        ChkCnt--;
                                    }
                                }
                            }
                        }
                        /* cmd: pipe hdr */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.PipeHdr.Enable = (UINT8) HdrEnable;
                        CmdMsg.Ctx.PipeHdr.ExposureNum = (UINT8) ExposureNum;
                        CmdMsg.Ctx.PipeHdr.LCeEnable = (UINT8) LCeEnable;
                        CmdMsg.Ctx.PipeHdr.BackWgc = 0U;
                        CmdMsg.Ctx.PipeHdr.BackAltWgc = 0U;
                        CmdMsg.Ctx.PipeHdr.BackDgc = 0U;
                        CmdMsg.Ctx.PipeHdr.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.PipeHdr.Cmd = (UINT8) SVC_PIPE_CMD_HDR;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                        ChainCnt++;
                    }
                }
            }
        }
    }

    /*
     *  image channel select & config
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* sensor num in vin */
            SensorNumInVin = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    SensorNumInVin += (pImgCfg->TimeDivision[VinId].Num);
                }
            }

            if (SensorNumInVin > 0U) {
                /* cmd: image channel select */
                CmdMsg.Ctx.Data = 0ULL;
                ChainIndex = (UINT8) (SensorNumInVin - 1U);
                CmdMsg.Ctx.ImageChanSelect.ChainIndex = ChainIndex;
                CmdMsg.Ctx.ImageChanSelect.Msg = (UINT8) SVC_CHAN_MSG_SELECT;
                CmdMsg.Ctx.ImageChanSelect.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
            }

            /* info update */
            SvcImgInfo.Cfg.SensorSelectBits[VinId] = pImgCfg->SensorSelectBits[VinId];
            SvcImgInfo.Cfg.TimeDivision[VinId].Num = pImgCfg->TimeDivision[VinId].Num;

            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division loop */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* cmd: image channel sensor id */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanSensor.Id = (UINT8) (((UINT32) 1U) << j);
                        CmdMsg.Ctx.ImageChanSensor.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanSensor.Msg = (UINT8) SVC_CHAN_MSG_SENSOR;
                        CmdMsg.Ctx.ImageChanSensor.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* cmd: image channel algo id */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanAlgo.Id = (UINT8) AlgoCnt;
                        CmdMsg.Ctx.ImageChanAlgo.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanAlgo.Msg = (UINT8) SVC_CHAN_MSG_ALGO;
                        CmdMsg.Ctx.ImageChanAlgo.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* fov get */
                        FovSelectBitsInSensor = pImgCfg->FovSelectBitsInSensor[VinId][j];
                        /* time division check */
                        if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                            /* search corresponding fov */
                            ChkCnt = k;
                            for (m = 0U; m < SVC_NUM_FOV_CHANNEL; m++) {
                                if ((FovSelectBitsInSensor & (((UINT32) 1U) << m)) > 0U) {
                                    if (ChkCnt == 0U) {
                                        break;
                                    }
                                    ChkCnt--;
                                }
                            }
                            FovSelectBitsInSensor = (((UINT32) 1U) << m);
                        }

                        /* cmd: image channel zone id */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanZone.Id = (UINT32) FovSelectBitsInSensor;
                        CmdMsg.Ctx.ImageChanZone.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanZone.Msg = (UINT8) SVC_CHAN_MSG_ZONE;
                        CmdMsg.Ctx.ImageChanZone.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* cmd: image channel inter id */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanInter.Id = (UINT32) pImgCfg->TimeDivision[VinId].FrameSelectBits[k];
                        CmdMsg.Ctx.ImageChanInter.Num = (UINT8) pImgCfg->TimeDivision[VinId].FrameGroupCnt;
                        CmdMsg.Ctx.ImageChanInter.Skip = (UINT8) 3U;
                        if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                            /* sub sensor mode (td) */
                            CmdMsg.Ctx.ImageChanInter.Mode = (UINT8) ((pImgCfg->TimeDivision[VinId].AaaIqTable[j][k] & 0xFF00U) >> 8U) ;
                        } else {
                            /* sub sensor mode (non-td) */
                            CmdMsg.Ctx.ImageChanInter.Mode = (UINT8) (((pImgCfg->SensorAaaGroup[VinId][j] >> 16U) & 0x3000U) >> 12U);
                        }
                        CmdMsg.Ctx.ImageChanInter.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanInter.Msg = (UINT8) SVC_CHAN_MSG_INTER;
                        CmdMsg.Ctx.ImageChanInter.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* cmd: image channel external fov id */
                        CmdMsg.Ctx.Data = (pImgCfg->SensorEFov[VinId][j] & 0x000000FFFFFFFFFFULL);
                        CmdMsg.Ctx.ImageChanEFov.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanEFov.Msg = (UINT8) SVC_CHAN_MSG_EFOV;
                        CmdMsg.Ctx.ImageChanEFov.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* cmd: image channel af */
                        CmdMsg.Ctx.Data = 0ULL;
                        if (k == 0U) {
                            CmdMsg.Ctx.ImageChanAf.Enable = (UINT8) ((pImgCfg->SensorAaaGroup[VinId][j] & 0x4000U) >> 14U);
                        } else {
                            CmdMsg.Ctx.ImageChanAf.Enable = 0U;
                        }
                        CmdMsg.Ctx.ImageChanAf.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanAf.Msg = (UINT8) SVC_CHAN_MSG_AF;
                        CmdMsg.Ctx.ImageChanAf.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* cmd: image channel post zone id (clear) */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanPostZone.Id = 0U;
                        CmdMsg.Ctx.ImageChanPostZone.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanPostZone.Msg = (UINT8) SVC_CHAN_MSG_POST_ZONE;
                        CmdMsg.Ctx.ImageChanPostZone.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* cmd: image channel aaa core */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanCore.Id = (pImgCfg->SensorAaaGroup[VinId][j] >> 4U) & 0xFFU;
                        CmdMsg.Ctx.ImageChanCore.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanCore.Msg = (UINT8) SVC_CHAN_MSG_AAA_CORE;
                        CmdMsg.Ctx.ImageChanCore.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* cmd: image channel slow shutter */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanSlowShutter.Enable = (UINT8) ((pImgCfg->SensorAaaGroup[VinId][j] & 0x3000U) >> 12U);
                        CmdMsg.Ctx.ImageChanSlowShutter.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanSlowShutter.Msg = (UINT8) SVC_CHAN_MSG_SLOW_SHUTTER;
                        CmdMsg.Ctx.ImageChanSlowShutter.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
#ifdef CONFIG_BUILD_IMGFRW_SMC
                        /* cmd: image channel slow motion capture */
                        CmdMsg.Ctx.Data = 0ULL;
                        CmdMsg.Ctx.ImageChanSmc.I2sId = pImgCfg->SmcI2sId[VinId][j];
                        CmdMsg.Ctx.ImageChanSmc.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanSmc.Msg = (UINT8) SVC_CHAN_MSG_SMC;
                        CmdMsg.Ctx.ImageChanSmc.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
#endif
                        /* cmd: image channel force to disable vin tone cruve */
                        if (((pImgCfg->AaaIqTable[VinId][j] >> 16U) & 0x80U) > 0U) {
                            CmdMsg.Ctx.Data = 0ULL;
                            CmdMsg.Ctx.ImageChanVinToneCurve.Enable = 0U;
                            CmdMsg.Ctx.ImageChanVinToneCurve.ChainId = (UINT8) ChainCnt;
                            CmdMsg.Ctx.ImageChanVinToneCurve.Msg = (UINT8) SVC_CHAN_MSG_VIN_TONE_CURVE;
                            CmdMsg.Ctx.ImageChanVinToneCurve.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                        }
                        /* aaa group id get */
                        if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                            /* time division: not support */
                            AaaGroupId = 0U;
                            AaaGroupFovMsb = 0U;
                        } else {
                            /* normal */
                            AaaGroupId = pImgCfg->SensorAaaGroup[VinId][j] & 0xFU;
                            AaaGroupFovMsb = pImgCfg->SensorAaaGroup[VinId][j] & 0x8000U;
                        }

                        /* cmd: image channel zone msb */
                        CmdMsg.Ctx.Data = 0ULL;
                        if (AaaGroupFovMsb == 0U) {
                            CmdMsg.Ctx.ImageChanZoneMsb.Id = 0U;
                        } else {
                            CmdMsg.Ctx.ImageChanZoneMsb.Id = 1U;
                        }
                        CmdMsg.Ctx.ImageChanZoneMsb.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanZoneMsb.Msg = (UINT8) SVC_CHAN_MSG_ZONE_MSB;
                        CmdMsg.Ctx.ImageChanZoneMsb.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                        /* vin id accumulated */
                        AccVinBits[AaaGroupId] |= (((UINT32) 1U) << VinId);

                        /* zone id accumulated */
                        AccZoneBits[AaaGroupId] |= FovSelectBitsInSensor;

                        /* last vin id */
                        LastVinId[AaaGroupId] = VinId;

                        /* last chain id */
                        LastChainId[AaaGroupId] = ChainCnt;

                        /* last algo id */
                        LastAlgoId[AaaGroupId] = AlgoCnt;

                        /* avm group id get */
                        if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                            /* time division: not support */
                            AvmGroupId = 0U;
                            AvmGroupFovMsb = 0U;
                        } else {
                            /* normal */
                            AvmGroupId = (pImgCfg->SensorAaaGroup[VinId][j] >> 16U) & 0xFU;
                            AvmGroupFovMsb = (pImgCfg->SensorAaaGroup[VinId][j] >> 16U) & 0x8000U;
                        }

                        /* avm zone id get */
                        if (AvmGroupFovMsb == 0U) {
                            /* lsb */
                            for (n = 0; n < (INT32) SVC_NUM_FOV_CHANNEL; n++) {
                                if ((FovSelectBitsInSensor & (((UINT32) 1U) << ((UINT32) n))) > 0U) {
                                    break;
                                }
                            }
                        } else {
                            /* msb */
                            for (n = (((INT32) SVC_NUM_FOV_CHANNEL) - 1); n >= 0; n--) {
                                if ((FovSelectBitsInSensor & (((UINT32) 1U) << ((UINT32) n))) > 0U) {
                                    break;
                                }
                            }
                        }

                        /* avm zone id accumulated */
                        AvmAccZoneBits[AvmGroupId] |= (((UINT32) 1U) << ((UINT32) n));

                        /* avm last vin id */
                        AvmLastVinId[AvmGroupId] = VinId;

                        /* avm last chain id */
                        AvmLastChainId[AvmGroupId] = ChainCnt;

                        /* info update */
                        SvcImgInfo.Cfg.FovSelectBitsInSensor[VinId][j] = pImgCfg->FovSelectBitsInSensor[VinId][j];
                        SvcImgInfo.Cfg.SensorAaaGroup[VinId][j] = pImgCfg->SensorAaaGroup[VinId][j];
                        SvcImgInfo.Cfg.AaaIqTable[VinId][j] = pImgCfg->AaaIqTable[VinId][j];

                        /* algo info update */
                        SvcImgInfo.Algo[VinId][j].Ctx[k].Bits.Valid = 1U;
                        if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                            /* time division */
                            SvcImgInfo.Algo[VinId][j].Ctx[k].Bits.IqTable = (UINT8) pImgCfg->TimeDivision[VinId].AaaIqTable[j][k];
                            HdrId = (UINT8) ((pImgCfg->TimeDivision[VinId].AaaIqTable[j][k] >> 16U) & 0x7FU);
                            SvcImgInfo.Algo[VinId][j].Ctx[k].Bits.HdrId = HdrId;
                        } else {
                            /* normal */
                            SvcImgInfo.Algo[VinId][j].Ctx[k].Bits.IqTable = (UINT8) pImgCfg->AaaIqTable[VinId][j];
                            HdrId = (UINT8) ((pImgCfg->AaaIqTable[VinId][j] >> 16U) & 0x7FU);
                            SvcImgInfo.Algo[VinId][j].Ctx[k].Bits.HdrId = HdrId;
                        }
                        SvcImgInfo.Algo[VinId][j].Ctx[k].Bits.Id = (UINT8) AlgoCnt;

                        /* counter update */
                        ChainCnt++;
                        AlgoCnt++;
                    }
                } else {
                    /* sensor null */
                    SvcImgInfo.Cfg.FovSelectBitsInSensor[VinId][j] = 0U;
                    SvcImgInfo.Cfg.SensorAaaGroup[VinId][j] = 0U;
                    SvcImgInfo.Cfg.AaaIqTable[VinId][j] = 0U;
                    SvcImgInfo.Algo[VinId][j].Ctx[0].Data = 0U;
                }
            }
        }
    }

    /*
     *  image iq table
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division? */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* cmd: image channel iq table */
                        CmdMsg.Ctx.Data = 0ULL;
                        if (pImgCfg->TimeDivision[VinId].Num > 1U) {
                            /* time division */
                            CmdMsg.Ctx.ImageChanIq.Id = (UINT8) pImgCfg->TimeDivision[VinId].AaaIqTable[j][k];
                            HdrId = (UINT8) ((pImgCfg->TimeDivision[VinId].AaaIqTable[j][k] >> 16U) & 0x7FU);
                            CmdMsg.Ctx.ImageChanIq.HdrId = HdrId;
                        } else{
                            /* normal */
                            CmdMsg.Ctx.ImageChanIq.Id = (UINT8) pImgCfg->AaaIqTable[VinId][j];
                            HdrId = (UINT8) ((pImgCfg->AaaIqTable[VinId][j] >> 16U) & 0x7FU);
                            CmdMsg.Ctx.ImageChanIq.HdrId = HdrId;
                        }
                        CmdMsg.Ctx.ImageChanIq.ChainId = (UINT8) ChainCnt;
                        CmdMsg.Ctx.ImageChanIq.Msg = (UINT8) SVC_CHAN_MSG_IQ;
                        CmdMsg.Ctx.ImageChanIq.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                        ChainCnt++;
                    }
                }
            }
        }
    }

    /*
     *  vin sensor id
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* cmd: vin sensor */
                    CmdMsg.Ctx.Data = 0ULL;
                    CmdMsg.Ctx.VinSensor.Id = (UINT32) pImgCfg->VinSensorId[VinId][j];
                    CmdMsg.Ctx.VinSensor.Idx = (UINT8) j;
                    CmdMsg.Ctx.VinSensor.Cmd = (UINT8) SVC_VIN_CMD_SENSOR;
                    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                }
            }
        }
    }

    /*
     *  image channel global AAA (example)
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division? */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* non-td? */
                        if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                            /* aaa group id get */
                            AaaGroupId = pImgCfg->SensorAaaGroup[VinId][j] & 0xFU;
                            /* vr check */
                            if (AaaGroupId > 0U) {
                                /* master id get */
                                MasterVinId = LastVinId[AaaGroupId];
                                /* master chain id get */
                                MasterChainId = LastChainId[AaaGroupId];
                                /* master algo id get */
                                MasterAlgoId = LastAlgoId[AaaGroupId];
                                /* master check */
                                if ((VinId == MasterVinId) && (ChainCnt == MasterChainId)) {
                                    /* vr-master */
                                    /* cmd: image channel vr id */
                                    CmdMsg.Ctx.Data = 0ULL;
                                    CmdMsg.Ctx.ImageChanVr.Id = (UINT16) AccVinBits[AaaGroupId];
                                    CmdMsg.Ctx.ImageChanVr.AltId = (UINT8) (((UINT32) 1U) << MasterChainId);
                                    CmdMsg.Ctx.ImageChanVr.ChainId = (UINT8) ChainCnt;
                                    CmdMsg.Ctx.ImageChanVr.Msg = (UINT8) SVC_CHAN_MSG_VR;
                                    CmdMsg.Ctx.ImageChanVr.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                                    /* cmd: image channel zone id */
                                    CmdMsg.Ctx.Data = 0ULL;
                                    CmdMsg.Ctx.ImageChanZone.Id = AccZoneBits[AaaGroupId];
                                    CmdMsg.Ctx.ImageChanZone.ChainId = (UINT8) ChainCnt;
                                    CmdMsg.Ctx.ImageChanZone.Msg = (UINT8) SVC_CHAN_MSG_ZONE;
                                    CmdMsg.Ctx.ImageChanZone.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                } else {
                                    /* vr-slave */
                                    /* cmd: image channel vr id */
                                    CmdMsg.Ctx.Data = 0ULL;
                                    TempId = (UINT16) (((UINT32) 1U) << MasterVinId);
                                    CmdMsg.Ctx.ImageChanVr.Id = (UINT16) TempId;
                                    CmdMsg.Ctx.ImageChanVr.AltId = (UINT8) (((UINT32) 1U) << MasterChainId);
                                    CmdMsg.Ctx.ImageChanVr.ChainId = (UINT8) ChainCnt;
                                    CmdMsg.Ctx.ImageChanVr.Msg = (UINT8) SVC_CHAN_MSG_VR;
                                    CmdMsg.Ctx.ImageChanVr.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                                    /* algo id update to master */
                                    SvcImgInfo.Algo[VinId][j].Ctx[0].Bits.Id = (UINT8) MasterAlgoId;
                                }
                            } else {
                                /* non-vr */
                                /* cmd: image channel vr id (clear) */
                                CmdMsg.Ctx.Data = 0ULL;
                                CmdMsg.Ctx.ImageChanVr.ChainId = (UINT8) ChainCnt;
                                CmdMsg.Ctx.ImageChanVr.Msg = (UINT8) SVC_CHAN_MSG_VR;
                                CmdMsg.Ctx.ImageChanVr.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                            }
                        }
                        ChainCnt++;
                    }
                }
            }
        }
    }

    /*
     *  image channel avm & image avm iq table (example)
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division? */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* non-td? */
                        if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                            /* avm group id get */
                            AvmGroupId = (pImgCfg->SensorAaaGroup[VinId][j] >> 16U) & 0xFU;
                            /* vr check */
                            if (AvmGroupId > 0U) {
                                /* master id get */
                                MasterVinId = AvmLastVinId[AvmGroupId];
                                /* master chain id get */
                                MasterChainId = AvmLastChainId[AvmGroupId];
                                /* master check */
                                if ((VinId == MasterVinId) && (ChainCnt == MasterChainId)) {
                                    /* cmd: image channel avm */
                                    CmdMsg.Ctx.Data = 0ULL;
                                    CmdMsg.Ctx.ImageChanAvm.ZoneId = (UINT8) AvmAccZoneBits[AvmGroupId];
                                    CmdMsg.Ctx.ImageChanAvm.AlgoId = (UINT8) AlgoCnt;
                                    CmdMsg.Ctx.ImageChanAvm.ChainId = (UINT8) ChainCnt;
                                    CmdMsg.Ctx.ImageChanAvm.Msg = (UINT8) SVC_CHAN_MSG_AVM;
                                    CmdMsg.Ctx.ImageChanAvm.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                    /* cmd: image channel avm iq */
                                    CmdMsg.Ctx.Data = 0ULL;
                                    IqId = (UINT8) (pImgCfg->AaaIqTable[VinId][j] >> 8U);
                                    CmdMsg.Ctx.ImageChanIq.Id = IqId;
                                    HdrId = (UINT8) ((pImgCfg->AaaIqTable[VinId][j] >> 24U) & 0x7FU);
                                    CmdMsg.Ctx.ImageChanIq.HdrId = HdrId;
                                    CmdMsg.Ctx.ImageChanIq.ChainId = (UINT8) ChainCnt;
                                    CmdMsg.Ctx.ImageChanIq.Msg = (UINT8) SVC_CHAN_MSG_AVM_IQ;
                                    CmdMsg.Ctx.ImageChanIq.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                    /* cmd: image channel avm core */
                                    CmdMsg.Ctx.Data = 0ULL;
                                    CmdMsg.Ctx.ImageChanCore.Id = ((pImgCfg->SensorAaaGroup[VinId][j] >> 20U) & 0xFFU);
                                    CmdMsg.Ctx.ImageChanCore.ChainId = (UINT8) ChainCnt;
                                    CmdMsg.Ctx.ImageChanCore.Msg = (UINT8) SVC_CHAN_MSG_AVM_CORE;
                                    CmdMsg.Ctx.ImageChanCore.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                    /* avm algo id get */
                                    SvcImg_AvmAlgoId = AlgoCnt;
                                    AvmCnt++;
                                    AlgoCnt++;
                                }
                            }
                        }
                        ChainCnt++;
                    }
                }
            }
        }
    }
    /* avm image chain id reset? */
    if (AvmCnt == 0U) {
        /* cmd: image channel avm */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanAvm.Msg = (UINT8) SVC_CHAN_MSG_AVM;
        CmdMsg.Ctx.ImageChanAvm.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
        AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
    }

    /*
     *  image channel fov group check
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division loop */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* non-td? */
                        if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                            /* fov select bits */
                            FovSelectBitsInSensor = pImgCfg->FovSelectBitsInSensor[VinId][j];
                            for (m = 0U; (m < SVC_NUM_FOV_CHANNEL) && ((FovSelectBitsInSensor >> m) > 0U); m++) {
                                /* fov check? */
                                if ((FovSelectBitsInSensor & (((UINT32) 1U) << m)) > 0U) {
                                    /* fov iq check? */
                                    if ((pImgCfg->FovAdj[m].IqTable & 0xFFU) > 0U) {
                                        /* adj group id */
                                        AdjGroupId = (pImgCfg->FovAdj[m].GroupId & 0xFU);
                                        /* adj last zone id */
                                        AdjLastZoneId[AdjGroupId] = (((UINT32) 1U) << m);
                                        /* adj acc zone id */
                                        AdjAccZoneBits[AdjGroupId] |= (((UINT32) 1U) << m);
                                    }
                                }
                            }
                        }
                        ChainCnt++;
                    }
                }
            }
        }
    }
    /*
     *  image channel fov & image fov iq table (example)
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division loop */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* non-td? */
                        if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                            /* fov select bits */
                            FovSelectBitsInSensor = pImgCfg->FovSelectBitsInSensor[VinId][j];
                            for (m = 0U; (m < SVC_NUM_FOV_CHANNEL) && ((FovSelectBitsInSensor >> m) > 0U); m++) {
                                /* fov check? */
                                if ((FovSelectBitsInSensor & (((UINT32) 1U) << m)) > 0U) {
                                    /* fov iq check? */
                                    if ((pImgCfg->FovAdj[m].IqTable & 0xFFU) > 0U) {
                                        /* adj group id */
                                        AdjGroupId = (pImgCfg->FovAdj[m].GroupId & 0xFU);
                                        /* group? */
                                        if (AdjGroupId > 0U) {
                                            /* master fov id */
                                            MasterZoneId = AdjLastZoneId[AdjGroupId];
                                            /* master? */
                                            if (MasterZoneId == (((UINT32) 1U) << m)) {
                                                /* cmd: image channel fov */
                                                CmdMsg.Ctx.Data = 0ULL;
                                                CmdMsg.Ctx.ImageChanFov.ZoneId = AdjAccZoneBits[AdjGroupId];
                                                CmdMsg.Ctx.ImageChanFov.AlgoId = (UINT8) AlgoCnt;
                                                CmdMsg.Ctx.ImageChanFov.AdjId = (UINT8) AdjCnt;
                                                CmdMsg.Ctx.ImageChanFov.ChainId = (UINT8) ChainCnt;
                                                CmdMsg.Ctx.ImageChanFov.Msg = (UINT8) SVC_CHAN_MSG_FOV;
                                                CmdMsg.Ctx.ImageChanFov.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                                /* cmd: image channel fov iq */
                                                CmdMsg.Ctx.Data = 0ULL;
                                                IqId = (UINT8) pImgCfg->FovAdj[m].IqTable;
                                                CmdMsg.Ctx.ImageChanFovIq.Id = IqId;
                                                HdrId = (UINT8) ((pImgCfg->FovAdj[m].IqTable >> 16U) & 0x7FU);
                                                CmdMsg.Ctx.ImageChanFovIq.HdrId = HdrId;
                                                CmdMsg.Ctx.ImageChanFovIq.AdjId = (UINT8) AdjCnt;
                                                CmdMsg.Ctx.ImageChanFovIq.Msg = (UINT8) SVC_CHAN_MSG_FOV_IQ;
                                                CmdMsg.Ctx.ImageChanFovIq.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                                AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
                                                AdjCnt++;
                                                AlgoCnt++;
                                            }
                                        } else {
                                            /* cmd: image channel fov */
                                            CmdMsg.Ctx.Data = 0ULL;
                                            CmdMsg.Ctx.ImageChanFov.ZoneId = (((UINT32) 1U) << m);
                                            CmdMsg.Ctx.ImageChanFov.AlgoId = (UINT8) AlgoCnt;
                                            CmdMsg.Ctx.ImageChanFov.AdjId = (UINT8) AdjCnt;
                                            CmdMsg.Ctx.ImageChanFov.ChainId = (UINT8) ChainCnt;
                                            CmdMsg.Ctx.ImageChanFov.Msg = (UINT8) SVC_CHAN_MSG_FOV;
                                            CmdMsg.Ctx.ImageChanFov.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                            /* cmd: image channel fov iq */
                                            CmdMsg.Ctx.Data = 0ULL;
                                            IqId = (UINT8) pImgCfg->FovAdj[m].IqTable;
                                            CmdMsg.Ctx.ImageChanFovIq.Id = IqId;
                                            HdrId = (UINT8) ((pImgCfg->FovAdj[m].IqTable >> 16U) & 0x7FU);
                                            CmdMsg.Ctx.ImageChanFovIq.HdrId = HdrId;
                                            CmdMsg.Ctx.ImageChanFovIq.AdjId = (UINT8) AdjCnt;
                                            CmdMsg.Ctx.ImageChanFovIq.Msg = (UINT8) SVC_CHAN_MSG_FOV_IQ;
                                            CmdMsg.Ctx.ImageChanFovIq.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                            AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
                                            AdjCnt++;
                                            AlgoCnt++;
                                        }
                                    }
                                }
                            }
                        }
                        ChainCnt++;
                    }
                }
            }
        }
    }
    /*
     *  image channel fov (adj) reset?
     */
    if (AdjCnt == 0U) {
        /* cmd: image channel fov */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanFov.Msg = (UINT8) SVC_CHAN_MSG_FOV;
        CmdMsg.Ctx.ImageChanFov.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
        AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
    }

    /*
     *  image channel eis group check (develop only)
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division loop */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* non-td? */
                        if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                            /* fov select bits */
                            FovSelectBitsInSensor = pImgCfg->FovSelectBitsInSensor[VinId][j];
                            for (m = 0U; (m < SVC_NUM_FOV_CHANNEL) && ((FovSelectBitsInSensor >> m) > 0U); m++) {
                                /* fov check? */
                                if ((FovSelectBitsInSensor & (((UINT32) 1U) << m)) > 0U) {
                                    /* fov iq check? */
                                    if (((pImgCfg->FovAdj[m].IqTable >> 8U) & 0x1U) > 0U) {
                                        /* eis group id */
                                        EisGroupId = ((pImgCfg->FovAdj[m].GroupId >> 16U) & 0xFU);
                                        /* eis last zone id */
                                        EisLastZoneId[EisGroupId] = (((UINT32) 1U) << m);
                                        /* eis acc zone id */
                                        EisAccZoneBits[EisGroupId] |= (((UINT32) 1U) << m);
                                    }
                                }
                            }
                        }
                        ChainCnt++;
                    }
                }
            }
        }
    }
    /*
     *  image channel eis (develop only)
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* chain id reset */
            ChainCnt = 0U;
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division loop */
                    for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                        /* non-td? */
                        if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                            /* fov select bits */
                            FovSelectBitsInSensor = pImgCfg->FovSelectBitsInSensor[VinId][j];
                            for (m = 0U; (m < SVC_NUM_FOV_CHANNEL) && ((FovSelectBitsInSensor >> m) > 0U); m++) {
                                /* fov check? */
                                if ((FovSelectBitsInSensor & (((UINT32) 1U) << m)) > 0U) {
                                    /* fov iq check? */
                                    if (((pImgCfg->FovAdj[m].IqTable >> 8U) & 0x1U) > 0U) {
                                        /* eis group id */
                                        EisGroupId = ((pImgCfg->FovAdj[m].GroupId >> 16U) & 0xFU);
                                        /* group? */
                                        if (EisGroupId > 0U) {
                                            /* master fov id */
                                            MasterZoneId = EisLastZoneId[EisGroupId];
                                            /* master? */
                                            if (MasterZoneId == (((UINT32) 1U) << m)) {
                                                /* cmd: image channel eis */
                                                CmdMsg.Ctx.Data = 0ULL;
                                                CmdMsg.Ctx.ImageChanEis.ZoneId = EisAccZoneBits[EisGroupId];
                                                CmdMsg.Ctx.ImageChanEis.AlgoId = (UINT8) EisCnt;
                                                CmdMsg.Ctx.ImageChanEis.Id = (UINT8) EisCnt;
                                                CmdMsg.Ctx.ImageChanEis.ChainId = (UINT8) ChainCnt;
                                                CmdMsg.Ctx.ImageChanEis.Msg = (UINT8) SVC_CHAN_MSG_EIS;
                                                CmdMsg.Ctx.ImageChanEis.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                                /* cmd: image channel post zone add */
                                                CmdMsg.Ctx.Data = 0ULL;
                                                CmdMsg.Ctx.ImageChanPostZone.Id = EisAccZoneBits[EisGroupId];
                                                CmdMsg.Ctx.ImageChanPostZone.ChainId = (UINT8) ChainCnt;
                                                CmdMsg.Ctx.ImageChanPostZone.Msg = (UINT8) SVC_CHAN_MSG_POST_ZONE_ADD;
                                                CmdMsg.Ctx.ImageChanPostZone.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                                /* cmd: image channel eis core */
                                                CmdMsg.Ctx.Data = 0ULL;
                                                CmdMsg.Ctx.ImageChanCore.Id = ((pImgCfg->FovAdj[m].GroupId >> 20U) & 0xFFU);
                                                CmdMsg.Ctx.ImageChanCore.AuxId = (UINT8) EisCnt;
                                                CmdMsg.Ctx.ImageChanCore.ChainId = (UINT8) ChainCnt;
                                                CmdMsg.Ctx.ImageChanCore.Msg = (UINT8) SVC_CHAN_MSG_EIS_CORE;
                                                CmdMsg.Ctx.ImageChanCore.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                                /* cmd: image channel r2y user delay */
                                                CmdMsg.Ctx.Data = 0ULL;
                                                CmdMsg.Ctx.Bits3.Var = ((pImgCfg->FovAdj[m].IqTable >> 9U) & 0x7FU);
                                                CmdMsg.Ctx.Bits3.AuxId = (UINT8) EisCnt;
                                                CmdMsg.Ctx.Bits3.ChainId = (UINT8) ChainCnt;
                                                CmdMsg.Ctx.Bits3.Msg = (UINT8) SVC_CHAN_MSG_R2Y_USER_DELAY;
                                                CmdMsg.Ctx.Bits3.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
                                                /* cmd: image channel gyro user delay */
                                                CmdMsg.Ctx.Data = 0ULL;
                                                CmdMsg.Ctx.Bits3.Var = ((pImgCfg->FovAdj[m].IqTable >> 23U) & 0x7FU);
                                                CmdMsg.Ctx.Bits3.AuxId = (UINT8) EisCnt;
                                                CmdMsg.Ctx.Bits3.ChainId = (UINT8) ChainCnt;
                                                CmdMsg.Ctx.Bits3.Msg = (UINT8) SVC_CHAN_MSG_GYRO_USER_DELAY;
                                                CmdMsg.Ctx.Bits3.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
#endif
                                                EisCnt++;
                                            }
                                        } else {
                                            /* cmd: image channel eis */
                                            CmdMsg.Ctx.Data = 0ULL;
                                            CmdMsg.Ctx.ImageChanEis.ZoneId = (((UINT32) 1U) << m);
                                            CmdMsg.Ctx.ImageChanEis.AlgoId = (UINT8) EisCnt;
                                            CmdMsg.Ctx.ImageChanEis.Id = (UINT8) EisCnt;
                                            CmdMsg.Ctx.ImageChanEis.ChainId = (UINT8) ChainCnt;
                                            CmdMsg.Ctx.ImageChanEis.Msg = (UINT8) SVC_CHAN_MSG_EIS;
                                            CmdMsg.Ctx.ImageChanEis.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                            /* cmd: image channel post zone add */
                                            CmdMsg.Ctx.Data = 0ULL;
                                            CmdMsg.Ctx.ImageChanPostZone.Id = (((UINT32) 1U) << m);
                                            CmdMsg.Ctx.ImageChanPostZone.ChainId = (UINT8) ChainCnt;
                                            CmdMsg.Ctx.ImageChanPostZone.Msg = (UINT8) SVC_CHAN_MSG_POST_ZONE_ADD;
                                            CmdMsg.Ctx.ImageChanPostZone.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                            /* cmd: image channel eis core */
                                            CmdMsg.Ctx.Data = 0ULL;
                                            CmdMsg.Ctx.ImageChanCore.Id = ((pImgCfg->FovAdj[m].GroupId >> 20U) & 0xFFU);
                                            CmdMsg.Ctx.ImageChanCore.AuxId = (UINT8) EisCnt;
                                            CmdMsg.Ctx.ImageChanCore.ChainId = (UINT8) ChainCnt;
                                            CmdMsg.Ctx.ImageChanCore.Msg = (UINT8) SVC_CHAN_MSG_EIS_CORE;
                                            CmdMsg.Ctx.ImageChanCore.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                                            /* cmd: image channel r2y user delay */
                                            CmdMsg.Ctx.Data = 0ULL;
                                            CmdMsg.Ctx.Bits3.Var = ((pImgCfg->FovAdj[m].IqTable >> 9U) & 0x7FU);
                                            CmdMsg.Ctx.Bits3.AuxId = (UINT8) EisCnt;
                                            CmdMsg.Ctx.Bits3.ChainId = (UINT8) ChainCnt;
                                            CmdMsg.Ctx.Bits3.Msg = (UINT8) SVC_CHAN_MSG_R2Y_USER_DELAY;
                                            CmdMsg.Ctx.Bits3.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
                                            /* cmd: image channel gyro user delay */
                                            CmdMsg.Ctx.Data = 0ULL;
                                            CmdMsg.Ctx.Bits3.Var = ((pImgCfg->FovAdj[m].IqTable >> 23U) & 0x7FU);
                                            CmdMsg.Ctx.Bits3.AuxId = (UINT8) EisCnt;
                                            CmdMsg.Ctx.Bits3.ChainId = (UINT8) ChainCnt;
                                            CmdMsg.Ctx.Bits3.Msg = (UINT8) SVC_CHAN_MSG_GYRO_USER_DELAY;
                                            CmdMsg.Ctx.Bits3.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
#endif
                                            EisCnt++;
                                        }
                                    }
                                }
                            }
                        }
                        ChainCnt++;
                    }
                }
            }
        }
    }
    /*
     *  image channel eis reset?
     */
    if (EisCnt == 0U) {
        /* cmd: image channel eis */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.ImageChanEis.Msg = (UINT8) SVC_CHAN_MSG_EIS;
        CmdMsg.Ctx.ImageChanEis.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
        AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
    }
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS /* image channel iso disable (example)(TBD) */
    {
        /*
         *  image channel iso disable check and accumulate
         */
        UINT32 FovIsoDisId = 0U;
        for (i = 0U; i < *(LivInfo.pNumVin); i++) {
            /* vin id get */
            VinId = LivInfo.pVinCapWin[i].VinID;
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* chain id reset */
                ChainCnt = 0U;
                for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                    /* sensor select check */
                    if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                        /* time division loop */
                        for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                            /* non-td? */
                            if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                                /* fov select bits */
                                FovSelectBitsInSensor = pImgCfg->FovSelectBitsInSensor[VinId][j];
                                for (m = 0U; (m < SVC_NUM_FOV_CHANNEL) && ((FovSelectBitsInSensor >> m) > 0U); m++) {
                                    /* fov check? */
                                    if ((FovSelectBitsInSensor & (((UINT32) 1U) << m)) > 0U) {
                                        /* fov iso disable check? */
                                        if ((pImgCfg->FovAdj[m].IqTable & 0x80000000U) > 0U) {
                                            /* iso disable */
                                            FovIsoDisId |= ((UINT32) 1U) << m;
                                        }
                                    }
                                }
                            }
                            ChainCnt++;
                        }
                    }
                }
            }
        }
        /*
         *  image channel iso disable (example)(update to all channel)(TBD)
         */
        for (i = 0U; i < *(LivInfo.pNumVin); i++) {
            /* vin id get */
            VinId = LivInfo.pVinCapWin[i].VinID;
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* chain id reset */
                ChainCnt = 0U;
                for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                    /* sensor select check */
                    if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                        /* time division loop */
                        for (k = 0U; k < pImgCfg->TimeDivision[VinId].Num; k++) {
                            /* non-td? */
                            if (pImgCfg->TimeDivision[VinId].Num == 1U) {
                                /* img cmd iso disbale id */
                                CmdMsg.Ctx.Data = 0ULL;
                                CmdMsg.Ctx.ImageChanIso.DisId = FovIsoDisId;
                                CmdMsg.Ctx.ImageChanIso.ChainId = (UINT8) ChainCnt;
                                CmdMsg.Ctx.ImageChanIso.Msg = (UINT8) SVC_CHAN_MSG_ISO_DIS;
                                CmdMsg.Ctx.ImageChanIso.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
                                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
                            }
                            ChainCnt++;
                        }
                    }
                }
            }
        }
    }
#endif
    /*
     *  image framework reset, aaa task and sync task
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* cmd: liv start */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Video.R2yBackground = 1U;
            CmdMsg.Ctx.Video.Stitch = 0U;
            CmdMsg.Ctx.Video.DeferredBlackLevel = 1U;
            CmdMsg.Ctx.Video.LowDelay = 0U;
            CmdMsg.Ctx.Video.MaxSlowShutterIndex = 0U;
            CmdMsg.Ctx.Video.ForceSlowShutter = 0U;
#ifdef CONFIG_BUILD_IMGFRW_SMC
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                if ((pImgCfg->SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                    if (((pImgCfg->SensorAaaGroup[VinId][j] >> 16U) & 0x4000U) > 0U) {
                        CmdMsg.Ctx.Video.MaxSlowShutterIndex = (UINT8) ((pImgCfg->SensorAaaGroup[VinId][j] & 0x3000U) >> 12U);
                        CmdMsg.Ctx.Video.ForceSlowShutter = (UINT8) (((pImgCfg->SensorAaaGroup[VinId][j] >> 16U) & 0x4000U) >> 14U);
                        break;
                    }
                }
            }
#endif
            CmdMsg.Ctx.Video.IqSecId = (UINT8) IqSecId;
            if (VinSrc == SVC_VIN_SRC_MEM) {
                CmdMsg.Ctx.Video.OpMode = (UINT8) SVC_VIDEO_OP_RAWENC;
            }
            CmdMsg.Ctx.Video.Cmd = (UINT8) SVC_LIVEVIEW_CMD_START;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

            /* cmd: fov start */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_FOV_CMD_START;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

            /* cmd: eis start */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_EIS_CMD_START;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

            /* cmd: sync start */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_SYNC_CMD_START;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
        }
    }

    /*
     *  statistics task
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* cmd: statistics start */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_STATISTICS_CMD_START;
            AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
            break;
        }
    }

    /*
     *  avm task
     */
    if (AvmCnt > 0U) {
        /* avm active */
        SvcImg_AvmActive = 1U;
        SvcImg_AvmActiveEx = 1U;
        for (i = 0U; i < *(LivInfo.pNumVin); i++) {
            /* vin id get */
            VinId = LivInfo.pVinCapWin[i].VinID;
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* cmd: avm start */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_AVM_CMD_START;
                AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
                break;
            }
        }
    } else {
        /* avm inactive */
        SvcImg_AvmActive = 0U;
        SvcImg_AvmAlgoId = 0xFFFFU;
        SvcImg_AvmActiveEx = 0U;
    }
}

/**
 *  Svc image config
 *  @param[in] pImgCfg pointer to image configuration data
 */
void SvcImg_Config(const SVC_IMG_CFG_s *pImgCfg)
{
    UINT32 IqSecId = (UINT32) SVC_ALGO_IQ_LOAD_ALL;
    SvcImg_ConfigEx(pImgCfg, IqSecId);
}

/**
 *  Svc image stop
 */
void SvcImg_Stop(void)
{
    SVC_LIV_INFO_s LivInfo;
    SVC_IMG_CMD_MSG_s CmdMsg;

    UINT32 FuncRetVal;
    UINT32 i;
    UINT32 VinId;
    UINT32 VinSrc = 0U;

    /* liv info get */
    SvcLiveview_InfoGet(&LivInfo);

    /*
     *  aaa task and sync task
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* cmd: liv stop */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Video.Cmd = (UINT8) SVC_LIVEVIEW_CMD_STOP;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

            /* cmd: fov stop */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_FOV_CMD_STOP;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

            /* cmd: eis stop */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_EIS_CMD_STOP;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

            /* cmd: sync stop */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_SYNC_CMD_STOP;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
        }
    }

    /*
     *  statistics task
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinId = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            /* cmd: statistics stop */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_STATISTICS_CMD_STOP;
            AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
            break;
        }
    }

    /*
     *  avm task
     */
    if (SvcImg_AvmActive > 0U) {
        for (i = 0U; i < *(LivInfo.pNumVin); i++) {
            /* vin id get */
            VinId = LivInfo.pVinCapWin[i].VinID;
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* cmd: avm stop */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_AVM_CMD_STOP;
                AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
                break;
            }
        }
        SvcImg_AvmActive = 0U;
        SvcImg_AvmAlgoId = 0xFFFFU;
        SvcImg_AvmActiveEx = 0U;
    }
}

/**
 *  Svc image aaa start
 *  @param[in] VinId vin id
 */
void SvcImg_AaaStart(UINT32 VinId)
{
    SVC_LIV_INFO_s LivInfo;
    SVC_IMG_CMD_MSG_s CmdMsg;

    UINT32 FuncRetVal;
    UINT32 i;
    UINT32 VinSrc = 0U;

    /* liv info get */
    SvcLiveview_InfoGet(&LivInfo);

    /*
     *  image framework reset, aaa/fov/eis task
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id check */
        if (VinId == LivInfo.pVinCapWin[i].VinID) {
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* cmd: liv start */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Video.R2yBackground = 1U;
                CmdMsg.Ctx.Video.Stitch = 0U;
                CmdMsg.Ctx.Video.DeferredBlackLevel = 1U;
                CmdMsg.Ctx.Video.LowDelay = 0U;
                CmdMsg.Ctx.Video.MaxSlowShutterIndex = 0U;
                CmdMsg.Ctx.Video.ForceSlowShutter = 0U;
#ifdef CONFIG_BUILD_IMGFRW_SMC
                {
                    UINT32 j;
                    for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                        if ((SvcImgInfo.Cfg.SensorSelectBits[VinId] & (((UINT32) 1U) << j)) > 0U) {
                            if (((SvcImgInfo.Cfg.SensorAaaGroup[VinId][j] >> 16U) & 0x4000U) > 0U) {
                                CmdMsg.Ctx.Video.MaxSlowShutterIndex = (UINT8) ((SvcImgInfo.Cfg.SensorAaaGroup[VinId][j] & 0x3000U) >> 12U);
                                CmdMsg.Ctx.Video.ForceSlowShutter = (UINT8) (((SvcImgInfo.Cfg.SensorAaaGroup[VinId][j] >> 16U) & 0x4000U) >> 14U);
                                break;
                            }
                        }
                    }
                }
#endif
                CmdMsg.Ctx.Video.IqSecId = 0U;
                if (VinSrc == SVC_VIN_SRC_MEM) {
                    CmdMsg.Ctx.Video.OpMode = (UINT8) SVC_VIDEO_OP_RAWENC;
                }
                CmdMsg.Ctx.Video.Cmd = (UINT8) SVC_LIVEVIEW_CMD_START;
                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                /* cmd: fov start */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_FOV_CMD_START;
                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                /* cmd: eis start */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_EIS_CMD_START;
                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
            }
        }
    }

    /*
     *  avm task
     */
    if ((SvcImg_AvmActive > 0U) &&
        (SvcImg_AvmActiveEx == 0U)) {
        for (i = 0U; i < *(LivInfo.pNumVin); i++) {
            /* vin id get */
            if (VinId == LivInfo.pVinCapWin[i].VinID) {
                /* vin src get */
                FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
                if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                    /* cmd: avm start */
                    CmdMsg.Ctx.Data = 0ULL;
                    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_AVM_CMD_START;
                    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
                    /* avm ex flag */
                    SvcImg_AvmActiveEx = 1U;
                    break;
                }
            }
        }
    }
}

/**
 *  Svc image aaa stop
 *  @param[in] VinId vin id
 */
void SvcImg_AaaStop(UINT32 VinId)
{
    SVC_LIV_INFO_s LivInfo;
    SVC_IMG_CMD_MSG_s CmdMsg;

    UINT32 FuncRetVal;
    UINT32 i;
    UINT32 VinSrc = 0U;

    /* liv info get */
    SvcLiveview_InfoGet(&LivInfo);

    /*
     *  aaa/fov/eis task
     */
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id check */
        if (VinId == LivInfo.pVinCapWin[i].VinID) {
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* cmd: liv stop */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Video.Cmd = (UINT8) SVC_LIVEVIEW_CMD_STOP;
                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                /* cmd: fov stop */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_FOV_CMD_STOP;
                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

                /* cmd: eis stop */
                CmdMsg.Ctx.Data = 0ULL;
                CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_EIS_CMD_STOP;
                AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
            }
        }
    }

    /*
     *  avm task
     */
    if ((SvcImg_AvmActive > 0U) &&
        (SvcImg_AvmActiveEx > 0U)) {
        for (i = 0U; i < *(LivInfo.pNumVin); i++) {
            /* vin id get */
            if (VinId == LivInfo.pVinCapWin[i].VinID) {
                /* vin src get */
                FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
                if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                    /* cmd: avm stop */
                    CmdMsg.Ctx.Data = 0ULL;
                    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_AVM_CMD_STOP;
                    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
                    /* avm ex flag */
                    SvcImg_AvmActiveEx = 0U;
                    break;
                }
            }
        }
    }
}

/**
 *  Svc image command install
 */
void SvcImg_CmdInstall(void)
{
    {
        AMBA_SHELL_COMMAND_s SvcCmd_ImgFrw;

        UINT32  Rval;

        SvcCmd_ImgFrw.pName    = "imgfrw";
        SvcCmd_ImgFrw.MainFunc = AmbaImgMain_ImgfrwCommand;
        SvcCmd_ImgFrw.pNext    = NULL;

        Rval = SvcCmd_CommandRegister(&SvcCmd_ImgFrw);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG("INIT", "install imgfrw command fail...", 0U, 0U);
        }
    }
    {
        extern void AmpUT_ImgProcTest(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
        AMBA_SHELL_COMMAND_s SvcCmd_ImgProc;

        UINT32  Rval;

        SvcCmd_ImgProc.pName    = "imgproc";
        SvcCmd_ImgProc.MainFunc = AmpUT_ImgProcTest;
        SvcCmd_ImgProc.pNext    = NULL;

        Rval = SvcCmd_CommandRegister(&SvcCmd_ImgProc);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG("INIT", "install imgproc command fail...", 0U, 0U);
        }
    }
#ifdef CONFIG_BUILD_IMGFRW_EIS
#if 0
    {
        extern void AmbaImu_TestCmd(UINT32 ArgCount,char * const * pArgVector,AMBA_SHELL_PRINT_f PrintFunc);
        AMBA_SHELL_COMMAND_s SvcCmd_ImgImu;

        UINT32  Rval;

        SvcCmd_ImgImu.pName    = "imu";
        SvcCmd_ImgImu.MainFunc = AmbaImu_TestCmd;
        SvcCmd_ImgImu.pNext    = NULL;

        Rval = SvcCmd_CommandRegister(&SvcCmd_ImgImu);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG("INIT", "install imu command fail...", 0U, 0U);
        }
    }
#endif
    {
        extern void AmbaEIS_TestCmd(UINT32 ArgCount,char * const * pArgVector,AMBA_SHELL_PRINT_f PrintFunc);
        AMBA_SHELL_COMMAND_s SvcCmd_ImgEis;

        UINT32  Rval;

        SvcCmd_ImgEis.pName    = "eis";
        SvcCmd_ImgEis.MainFunc = AmbaEIS_TestCmd;
        SvcCmd_ImgEis.pNext    = NULL;

        Rval = SvcCmd_CommandRegister(&SvcCmd_ImgEis);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG("INIT", "install eis command fail...", 0U, 0U);
        }
    }
#endif
#ifdef CONFIG_BUILD_IMGFRW_AF
    {
        extern void AmbaAf_AfTestCmd(UINT32 ArgCount,char * const * pArgVector,AMBA_SHELL_PRINT_f PrintFunc);
        AMBA_SHELL_COMMAND_s SvcCmd_ImgAf;

        UINT32  Rval;

        SvcCmd_ImgAf.pName    = "af";
        SvcCmd_ImgAf.MainFunc = AmbaAf_AfTestCmd;
        SvcCmd_ImgAf.pNext    = NULL;

        Rval = SvcCmd_CommandRegister(&SvcCmd_ImgAf);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG("INIT", "install af command fail...", 0U, 0U);
        }
    }
#endif
}

/**
 *  Svc image algorithm id get (extend to include the time-division feature)
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[out] pAlgoId pointer to the algorithm id
 *  @param[out] pAlgoNum pointer to the algorithm number
 *  @return error code
 */
UINT32 SvcImg_AlgoIdGetEx(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId, UINT32 *pAlgoNum)
{
    UINT32 RetCode = SVC_OK;
    UINT32 i;

    UINT32 TimeDivisionNum;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        if (SensorId < SVC_NUM_VIN_SENSOR) {
            TimeDivisionNum = SvcImgInfo.Cfg.TimeDivision[VinId].Num;
            if (TimeDivisionNum > 1U) {
                /* time division for vin */
                *pAlgoNum = TimeDivisionNum;
                for (i = 0U; i < TimeDivisionNum; i++) {
                    if (SvcImgInfo.Algo[VinId][SensorId].Ctx[i].Bits.Valid > 0U) {
                        pAlgoId[i] = SvcImgInfo.Algo[VinId][SensorId].Ctx[i].Bits.Id;
                    } else {
                        *pAlgoNum = 0U;
                        RetCode = SVC_NG;
                        break;
                    }
                }
            } else {
                /* normal */
                if (SvcImgInfo.Algo[VinId][SensorId].Ctx[0].Bits.Valid > 0U) {
                    *pAlgoNum = 1U;
                    *pAlgoId = (UINT32) SvcImgInfo.Algo[VinId][SensorId].Ctx[0].Bits.Id;
                } else {
                    *pAlgoNum = 0U;
                    RetCode = SVC_NG;
                }
            }
        } else {
            /* sensor exceed */
            *pAlgoNum = 0U;
            RetCode = SVC_NG;
        }
    } else {
        /* vin exceed */
        *pAlgoNum = 0U;
        RetCode = SVC_NG;
    }

    return RetCode;
}

/**
 *  Svc image algorithm id get (compatible without the time-division feature)
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[out] pAlgoId pointer to the algorithm id
 *  @return error code
 */
UINT32 SvcImg_AlgoIdGet(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId)
{
    UINT32 FuncRetCode;

    UINT32 AlgoNum;
    UINT32 AlgoId[AMBA_DSP_MAX_VIN_TD_NUM];

    FuncRetCode = SvcImg_AlgoIdGetEx(VinId, SensorId, AlgoId, &AlgoNum);
    if (FuncRetCode == SVC_OK) {
        /* compatible without time division */
        *pAlgoId = AlgoId[0];
    }

    return FuncRetCode;
}

/**
 *  Svc image algorithm id pre-get (before configuration, extend to include the time-division feature)
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[out] pAlgoId pointer to the algorithm id
 *  @param[out] pAlgoNum pointer to the algorithm number
 *  @return error code
 */
UINT32 SvcImg_AlgoIdPreGetEx(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId, UINT32 *pAlgoNum)
{
    UINT32 RetCode = SVC_OK;
    UINT32 FuncRetCode;
    UINT32 i, j, k;

    UINT32 FoundFlag = 0U;
    UINT32 BreakFlag = 0U;

    UINT32 VinNum;
    UINT32 VinIndex[AMBA_NUM_VIN_CHANNEL] = {0};

    UINT32 SensorNum;
    UINT32 SensorIndex[SVC_NUM_VIN_SENSOR] = {0};

    UINT32 AlgoCnt = 0U;

    UINT32 TimeDivisionNum;

    UINT32 AaaGroupId;
    UINT32 LastAlgoId[SVC_IMG_AAA_GROUP_NUM] = {0};

    const SVC_RES_CFG_s *pResCfg;

    /* last algo id init */
    for (i = 0U; i < SVC_IMG_AAA_GROUP_NUM; i++) {
        LastAlgoId[i] = 0U;
    }

    /* res cfg get */
    pResCfg = SvcResCfg_Get();

    if ((VinId < AMBA_NUM_VIN_CHANNEL) && (SensorId < SVC_NUM_VIN_SENSOR)) {
        /* vin get */
        FuncRetCode = SvcResCfg_GetVinIDs(VinIndex, &VinNum);
        if (FuncRetCode == SVC_OK) {
            if (VinNum > 0U) {
                /* vin check */
                for (i = 0U; i < VinNum; i++) {
                    if (BreakFlag > 0U) {
                        break;
                    }
                    /* sensor num get */
                    FuncRetCode = SvcResCfg_GetSensorIdxsInVinID(VinIndex[i], SensorIndex, &SensorNum);
                    if (FuncRetCode == SVC_OK) {
                        /* is valid? */
                        if (SensorNum > 0UL) {
                            /* time division? */
                            if (pResCfg->VinCfg[VinIndex[i]].SubChanCfg[0].TDNum > 1U) {
                                /* time division */
                                TimeDivisionNum = pResCfg->VinCfg[VinIndex[i]].SubChanCfg[0].TDNum;
                            } else {
                                /* normal */
                                TimeDivisionNum = 1U;
                            }
                            /* sensor in vin */
                            for (j = 0U; j < SensorNum; j++) {
                                if (BreakFlag > 0U) {
                                    break;
                                }
                                if (TimeDivisionNum > 1U) {
                                    /* time division: not support */
                                    AaaGroupId = 0U;
                                } else {
                                    /* normal */
                                    FuncRetCode = SvcResCfg_GetSensorGroupOfSensorIdx(VinIndex[i], SensorIndex[j], &AaaGroupId);
                                    if (FuncRetCode == SVC_OK) {
                                        /* aaa group id */
                                        AaaGroupId = AaaGroupId & 0xFU;
                                    } else {
                                        /* sensor group fail */
                                        BreakFlag = 1U;
                                        RetCode = SVC_NG;
                                    }
                                }
                                /* time division in vin */
                                if (RetCode == SVC_OK) {
                                    for (k = 0U; k < TimeDivisionNum; k++) {
                                        /*last algo id */
                                        LastAlgoId[AaaGroupId] = AlgoCnt;
                                        AlgoCnt++;
                                    }
                                }
                            }
                        } else {
                            /* sensor null */
                            RetCode = SVC_NG;
                        }
                    } else {
                        /* sensor fail */
                        RetCode = SVC_NG;
                    }
                }
            } else {
                /* vin null */
                RetCode = SVC_NG;
            }
        } else {
            /* vin fail */
            RetCode = SVC_NG;
        }

        if (RetCode == SVC_OK) {
            /* algo cnt reset */
            AlgoCnt = 0U;
            /* vin get */
            FuncRetCode = SvcResCfg_GetVinIDs(VinIndex, &VinNum);
            if (FuncRetCode == SVC_OK) {
                if (VinNum > 0U) {
                    /* vin check */
                    for (i = 0U; i < VinNum; i++) {
                        if (BreakFlag > 0U) {
                            break;
                        }
                        /* sensor num get */
                        FuncRetCode = SvcResCfg_GetSensorIdxsInVinID(VinIndex[i], SensorIndex, &SensorNum);
                        if (FuncRetCode == SVC_OK) {
                            /* is valid? */
                            if (SensorNum > 0U) {
                                if (pResCfg->VinCfg[VinIndex[i]].SubChanCfg[0].TDNum > 1U) {
                                    /* time division */
                                    TimeDivisionNum = pResCfg->VinCfg[VinIndex[i]].SubChanCfg[0].TDNum;
                                    /* algo num */
                                    *pAlgoNum = TimeDivisionNum;
                                } else {
                                    /* normal */
                                    TimeDivisionNum = 1U;
                                    /* algo num */
                                    *pAlgoNum = 1U;
                                }
                                /* sensor in vin */
                                for (j = 0U; j < SensorNum; j++) {
                                    if (BreakFlag > 0U) {
                                        break;
                                    }
                                    /* time division? */
                                    if (TimeDivisionNum > 1U) {
                                        /* time division: not support */
                                        AaaGroupId = 0U;
                                    } else {
                                        /* normal */
                                        FuncRetCode = SvcResCfg_GetSensorGroupOfSensorIdx(VinIndex[i], SensorIndex[j], &AaaGroupId);
                                        if (FuncRetCode == SVC_OK) {
                                            /* aaa group id */
                                            AaaGroupId = AaaGroupId & 0xFU;
                                        } else {
                                            /* sensor group fail */
                                            BreakFlag = 1U;
                                            RetCode = SVC_NG;
                                        }
                                    }
                                    /* time division in vin */
                                    if (RetCode == SVC_OK) {
                                        for (k = 0U; k < TimeDivisionNum; k++) {
                                            if ((VinIndex[i] == VinId) && (SensorIndex[j] == SensorId)) {
                                                if (TimeDivisionNum > 1U) {
                                                    /* time division */
                                                    pAlgoId[k] = AlgoCnt;
                                                } else {
                                                    /* normal */
                                                    if (AaaGroupId == 0U) {
                                                        /* individual */
                                                        pAlgoId[k] = AlgoCnt;
                                                    } else {
                                                        /* global */
                                                        pAlgoId[k] = LastAlgoId[AaaGroupId];
                                                    }
                                                }
                                                FoundFlag = 1U;
                                                BreakFlag = 1U;
                                            }
                                            AlgoCnt++;
                                        }
                                    }
                                }
                            } else {
                                /* sensor null */
                                BreakFlag = 1U;
                                RetCode = SVC_NG;
                            }
                        } else {
                            /* sensor fail */
                            BreakFlag = 1U;
                            RetCode = SVC_NG;
                        }
                    }

                    if (FoundFlag == 0U) {
                        /* not found */
                        RetCode = SVC_NG;
                    }
                } else {
                    /* vin null */
                    RetCode = SVC_NG;
                }
            } else {
                /* vin fail */
                RetCode = SVC_NG;
            }
        }
    } else {
        /* vin or sensor exceed */
        RetCode = SVC_NG;
    }

    return RetCode;
}

/**
 *  Svc image algorithm id pre-get (compatible without the time-division feature)
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[out] pAlgoId pointer to the algorithm id
 *  @return error code
 */
UINT32 SvcImg_AlgoIdPreGet(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId)
{
    UINT32 FuncRetCode;

    UINT32 AlgoNum;
    UINT32 AlgoId[AMBA_DSP_MAX_VIN_TD_NUM];

    FuncRetCode = SvcImg_AlgoIdPreGetEx(VinId, SensorId, AlgoId, &AlgoNum);
    if (FuncRetCode == SVC_OK) {
        /* compatible without time division */
        *pAlgoId = AlgoId[0];
    }

    return FuncRetCode;
}

/**
 *  Svc image algorithm IQ pre-load ex
 *  @param[in] SecId section id
 */
void SvcImg_AlgoIqPreloadEx(UINT32 SecId)
{
    UINT32 FuncRetVal;
    UINT32 i, j, k;

     UINT8 IqtableId;
     UINT8 HdrId;
     UINT8 AlgoId;
     UINT8 IqSecId;
    UINT32 VinSrc = 0U;
    UINT32 AlgoCnt = 0U;
    UINT32 SensorNum;
    UINT32 SensorIndex[SVC_NUM_VIN_SENSOR] = {0};

    UINT32 TimeDivisionNum;

    SVC_IMG_CMD_MSG_s CmdMsg;
    const SVC_RES_CFG_s *pResCfg;

    /* res cfg get */
    pResCfg = SvcResCfg_Get();

    /* cfg info get */
    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
        /* sensor idx in vin */
        FuncRetVal = SvcResCfg_GetSensorIdxsInVinID(i, SensorIndex, &SensorNum);
        if (FuncRetVal == SVC_OK) {
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(i, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* is valid? */
                if (SensorNum > 0U) {
                    if (pResCfg->VinCfg[i].SubChanCfg[0].TDNum > 1U) {
                        /* time division */
                        TimeDivisionNum = pResCfg->VinCfg[i].SubChanCfg[0].TDNum;
                    } else {
                        /* normal */
                        TimeDivisionNum = 1U;
                    }
                    /* sensor in vin */
                    for (j = 0U; j < SensorNum; j++) {
                        /* time division */
                        for (k = 0U; k < TimeDivisionNum; k++) {
                            if (TimeDivisionNum > 1U) {
                                /* time division */
                                IqtableId = (UINT8) (pResCfg->SensorCfg[i][SensorIndex[j]].TimeDivisionIQTable[k] & 0xFFU);
                                HdrId = (UINT8) ((pResCfg->SensorCfg[i][SensorIndex[j]].TimeDivisionIQTable[k] >> 16U) & 0x7FU);
                            } else {
                                /* normal */
                                IqtableId = (UINT8) (pResCfg->SensorCfg[i][SensorIndex[j]].IQTable & 0xFFU);
                                HdrId = (UINT8) ((pResCfg->SensorCfg[i][SensorIndex[j]].IQTable >> 16U) & 0x7FU);
                            }
                            /* cmd: image channel iq table (preload) */
                            CmdMsg.Ctx.Data = 0ULL;
                            CmdMsg.Ctx.ImageAlgoIqPreload.Id = IqtableId;
                            AlgoId = (UINT8) (AlgoCnt & 0xFFU);
                            CmdMsg.Ctx.ImageAlgoIqPreload.AlgoId = AlgoId;
                            CmdMsg.Ctx.ImageAlgoIqPreload.HdrId = HdrId;
                            IqSecId = (UINT8) (SecId & 0xFFU);
                            CmdMsg.Ctx.ImageAlgoIqPreload.SecId = IqSecId;
                            CmdMsg.Ctx.ImageAlgoIqPreload.Msg = (UINT8) SVC_ALGO_MSG_IQ_PRELOAD;
                            CmdMsg.Ctx.ImageAlgoIqPreload.Cmd = (UINT8) SVC_IMAGE_CMD_ALGO;
                            AmbaImgMain_Config(i, CmdMsg.Ctx.Data);
                            AlgoCnt++;
                        }
                    }
                }
            }
        }
    }
}

/**
 *  Svc image algorithm IQ pre-load
 *  @param[in] SecId section id
 */
void SvcImg_AlgoIqPreload(void)
{
    UINT32 SecId = (UINT32) SVC_ALGO_IQ_LOAD_ALL;
    SvcImg_AlgoIqPreloadEx(SecId);
}

/**
 *  Svc image algorithm IQ init ex
 *  @param[in] SecId section id
 */
void SvcImg_AlgoIqInitEx(UINT32 SecId)
{
    UINT32 FuncRetVal;
    UINT32 i, j, k;

     UINT8 IqtableId;
     UINT8 HdrId;
     UINT8 AlgoId;
     UINT8 IqSecId;
    UINT32 VinSrc = 0U;
    UINT32 AlgoCnt = 0U;
    UINT32 SensorNum;
    UINT32 SensorIndex[SVC_NUM_VIN_SENSOR] = {0};

    UINT32 TimeDivisionNum;

    SVC_IMG_CMD_MSG_s CmdMsg;
    const SVC_RES_CFG_s *pResCfg;

    /* res cfg get */
    pResCfg = SvcResCfg_Get();

    /* cfg info get */
    for (i = 0U; i < AMBA_NUM_VIN_CHANNEL; i++) {
        /* sensor idx in vin */
        FuncRetVal = SvcResCfg_GetSensorIdxsInVinID(i, SensorIndex, &SensorNum);
        if (FuncRetVal == SVC_OK) {
            /* vin src get */
            FuncRetVal = SvcResCfg_GetVinSrc(i, &VinSrc);
            if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
                /* is valid? */
                if (SensorNum > 0U) {
                    if (pResCfg->VinCfg[i].SubChanCfg[0].TDNum > 1U) {
                        /* time division */
                        TimeDivisionNum = pResCfg->VinCfg[i].SubChanCfg[0].TDNum;
                    } else {
                        /* normal */
                        TimeDivisionNum = 1U;
                    }
                    /* sensor in vin */
                    for (j = 0U; j < SensorNum; j++) {
                        /* time division */
                        for (k = 0U; k < TimeDivisionNum; k++) {
                            if (TimeDivisionNum > 1U) {
                                /* time division */
                                IqtableId = (UINT8) (pResCfg->SensorCfg[i][SensorIndex[j]].TimeDivisionIQTable[k] & 0xFFU);
                                HdrId = (UINT8) ((pResCfg->SensorCfg[i][SensorIndex[j]].TimeDivisionIQTable[k] >> 16U) & 0x7FU);
                            } else {
                                /* normal */
                                IqtableId = (UINT8) (pResCfg->SensorCfg[i][SensorIndex[j]].IQTable & 0xFFU);
                                HdrId = (UINT8) ((pResCfg->SensorCfg[i][SensorIndex[j]].IQTable >> 16U) & 0x7FU);
                            }
                            /* cmd: image channel iq table (init) */
                            CmdMsg.Ctx.Data = 0ULL;
                            CmdMsg.Ctx.ImageAlgoIqInit.Id = IqtableId;
                            AlgoId = (UINT8) (AlgoCnt & 0xFFU);
                            CmdMsg.Ctx.ImageAlgoIqInit.AlgoId = AlgoId;
                            CmdMsg.Ctx.ImageAlgoIqInit.HdrId = HdrId;
                            IqSecId = (UINT8) (SecId & 0xFFU);
                            CmdMsg.Ctx.ImageAlgoIqInit.SecId = IqSecId;
                            CmdMsg.Ctx.ImageAlgoIqInit.Msg = (UINT8) SVC_ALGO_MSG_IQ_INIT;
                            CmdMsg.Ctx.ImageAlgoIqInit.Cmd = (UINT8) SVC_IMAGE_CMD_ALGO;
                            AmbaImgMain_Config(i, CmdMsg.Ctx.Data);
                            AlgoCnt++;
                        }
                    }
                }
            }
        }
    }
}

/**
 *  Svc image algorithm defog
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[in] Enable enable (1) or disable (0)
 */
void SvcImg_AlgoDefog(UINT32 VinId, UINT32 SensorId, UINT32 Enable)
{
    UINT32 FuncRetVal;
    UINT32 AlgoId;

     UINT8 DefogEnable;
     UINT8 DefogAlogId;

    SVC_IMG_CMD_MSG_s CmdMsg;

    FuncRetVal = SvcImg_AlgoIdGet(VinId, SensorId, &AlgoId);
    if (FuncRetVal == SVC_OK) {
        /* cmd: image algo defog */
        CmdMsg.Ctx.Data = 0ULL;
        DefogEnable = (UINT8) (Enable & 0xFFU);
        CmdMsg.Ctx.ImageAlgoDefog.Enable = DefogEnable;
        if (Enable == 0U) {
            CmdMsg.Ctx.ImageAlgoDefog.SceneMode = (UINT8) SCENE_OFF;
        } else {
            /* defog */
            CmdMsg.Ctx.ImageAlgoDefog.SceneMode = (UINT8) SCENE_THROUGH_GLASS;
        }
        DefogAlogId = (UINT8) (AlgoId & 0xFFU);
        CmdMsg.Ctx.ImageAlgoDefog.AlgoId = DefogAlogId;
        CmdMsg.Ctx.ImageAlgoDefog.Msg = (UINT8) SVC_ALGO_MSG_DEFOG;
        CmdMsg.Ctx.ImageAlgoDefog.Cmd = (UINT8) SVC_IMAGE_CMD_ALGO;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    }
}

/**
 *  Svc image statistics mask
 *  @param[in] Mask statistics mask (bits)
 */
void SvcImg_StatisticsMask(UINT32 Mask)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    /* cmd: statistics mask */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Stat.Mask = Mask;
    CmdMsg.Ctx.Stat.Cmd = (UINT8) SVC_STATISTICS_CMD_MASK;
    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
}

/**
 *  Svc image sync (SOF/EOF) enable
 *  @param[in] VinId vin id
 *  @param[in] Enable enable (1) or disable (0)
 */
void SvcImg_SyncEnable(UINT32 VinId, UINT32 Enable)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    /* cmd: sync enable */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Sync.Enable = (UINT8) (Enable & 0xFFU);
    CmdMsg.Ctx.Sync.Cmd = (UINT8) SVC_SYNC_CMD_ENABLE;
    AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
}

/**
 *  Svc image algorithm information get
 *  @param[in] VinId vin id
 *  @param[in] RawCapSeq raw capture sequence number
 *  @param[out] pAlgoInfo pointer to the algorithm information
 *  @param[out] pAlgoNum pointer to the algorithm number
 */
void SvcImg_AlgoInfoGet(UINT32 VinId, UINT32 RawCapSeq, SVC_IMG_ALGO_INFO_s *pAlgoInfo, UINT32 *pAlgoNum)
{
    UINT32 FuncRetVal;
    UINT32 i, j, k;

    UINT32 VinIdx;
    UINT32 VinSrc = 0U;

    UINT32 AlgoCnt = 0U;

    SVC_IMG_ALGO_MSG_s AlgoMsg;

    SVC_IMG_MEM_INFO_s MemInfo;
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_LIV_INFO_s LivInfo;

    /* algo msg put */
    AlgoMsg.RawCapSeq = RawCapSeq;

    /* liv info get */
    SvcLiveview_InfoGet(&LivInfo);
    for (i = 0U; i < *(LivInfo.pNumVin); i++) {
        /* vin id get */
        VinIdx = LivInfo.pVinCapWin[i].VinID;
        /* vin src get */
        FuncRetVal = SvcResCfg_GetVinSrc(VinId, &VinSrc);
        if ((FuncRetVal == SVC_OK) && ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_MEM))) {
            for (j = 0U; j < SVC_NUM_VIN_SENSOR; j++) {
                /* sensor select check */
                if ((SvcImgInfo.Cfg.SensorSelectBits[VinIdx] & (((UINT32) 1U) << j)) > 0U) {
                    /* time division loop */
                    for (k = 0U; k < SvcImgInfo.Cfg.TimeDivision[VinIdx].Num; k++) {
                        if (VinIdx == VinId) {
                            /* aux data (algo msg) */
                            AuxData.Ctx.Data = 0ULL;
                            MemInfo.Ctx.pAlgoMsg = &AlgoMsg;
                            AuxData.Ctx.ImageAlgoInfo.Var = (UINT64) MemInfo.Ctx.Data;
                            /* cmd: algo msg get */
                            CmdMsg.Ctx.Data = 0ULL;
                            CmdMsg.Ctx.ImageAlgoInfo.Id = SvcImgInfo.Algo[VinIdx][j].Ctx[k].Bits.Id;
                            CmdMsg.Ctx.ImageAlgoInfo.Msg = (UINT8) SVC_ALGO_MSG_INFO_GET;
                            CmdMsg.Ctx.ImageAlgoInfo.Cmd = (UINT8) SVC_IMAGE_CMD_ALGO;
                            AlgoMsg.RetCode = SVC_NG;
                            AmbaImgMain_ConfigEx(VinId, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
                            /* algo msg put */
                            if (AlgoMsg.RetCode == SVC_OK) {
                                AlgoMsg.Info.Ctx.Id = SvcImgInfo.Algo[VinIdx][j].Ctx[k].Bits.Id;
                                AmbaSvcWrap_MisraMemcpy(&(pAlgoInfo[AlgoCnt]), &(AlgoMsg.Info), sizeof(SVC_IMG_ALGO_INFO_s));
                                AlgoCnt++;
                            }
                        }
                    }
                }
            }
        }
    }

    /* algo num put */
    *pAlgoNum = AlgoCnt;
}

/**
 *  Svc image avm algo id get
 *  @param[out] pAlgoId pointer to the algorihm id
 *  @return error code
 */
UINT32 SvcImg_AvmAlgoIdGet(UINT32 *pAlgoId)
{
    UINT32 RetCode = SVC_OK;

    if (SvcImg_AvmAlgoId != 0xFFFFU) {
        *pAlgoId = SvcImg_AvmAlgoId;
    } else {
        RetCode = SVC_NG;
    }

    return RetCode;
}

/**
 *  Svc image avm algo mode
 *  @param[in] Mode avm mode
 */
void SvcImg_AvmAlgoMode(UINT32 Mode)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    /* cmd: avm mode */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Var = Mode;
    CmdMsg.Ctx.Bits.Msg = (UINT8) SVC_ALGO_MSG_AVM_MODE;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_IMAGE_CMD_ALGO;
    AmbaImgMain_Config(0U/*don't care*/, CmdMsg.Ctx.Data);
}

/**
 *  Svc image avm roi rgb put
 *  @param[in] pData pointer to the roi rgb data
 */
void SvcImg_AvmRoiRgbPut(void *pData)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    /* aux data (avm roi) */
    AuxData.Ctx.Data = 0ULL;
    MemInfo.Ctx.pVoid = pData;
    AuxData.Ctx.Bits.Var = (UINT64) MemInfo.Ctx.Data;
    /* cmd: avm roi */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Msg = (UINT8) SVC_ALGO_MSG_AVM_ROI;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_IMAGE_CMD_ALGO;
    AmbaImgMain_ConfigEx(0U/*don't care*/, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
}

/**
 *  Svc image aeb (for still capture feater, TBD)
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[in] AebCnt aeb capture count
 *  @param[in] AebNum aeb step num
 *  @param[in] AebDen aeb step den
 */
void SvcImg_Aeb(UINT32 VinId, UINT32 SensorId, UINT32 AebCnt, UINT32 AebNum, UINT32 AebDen)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        if (SensorId < SVC_NUM_VIN_SENSOR) {
            /* cmd: chan snap aeb */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.ImageChanSnapAeb.Cnt = (UINT8) (AebCnt & 0xFFUL);
            CmdMsg.Ctx.ImageChanSnapAeb.Num = (UINT8) (AebNum & 0xFFUL);
            CmdMsg.Ctx.ImageChanSnapAeb.Den = (UINT8) (AebDen & 0xFFUL);
            CmdMsg.Ctx.ImageChanSnapAeb.ChainId = (UINT8) SensorId;
            CmdMsg.Ctx.ImageChanSnapAeb.Msg = (UINT8) SVC_CHAN_MSG_SNAP_AEB;
            CmdMsg.Ctx.ImageChanSnapAeb.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
            AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
        }
    }
}

/**
 * Svc image lock
 * @param[in] VinId vin id
 */
void SvcImg_Lock(UINT32 VinId)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        /* cmd: liveview lock */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_LIVEVIEW_CMD_LOCK;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    }
}

/**
 *  Svc image unlock
 *  @param[in] VinId vin id
 */
void SvcImg_Unlock(UINT32 VinId)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        /* cmd: liveview unlock */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_LIVEVIEW_CMD_UNLOCK;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    }
}

/**
 *  Svc image still capture start
 *  @param[in] VinId vin id
 */
void SvcImg_StillStart(UINT32 VinId)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        /* cmd: capture start */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Still.R2yBackground = (UINT8) 1U;
        CmdMsg.Ctx.Still.OpMode = (UINT8) SVC_STILL_OP_CAPTURE;
        CmdMsg.Ctx.Still.Cmd = (UINT8) SVC_CAPTURE_CMD_START;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

        /* cmd: sync start */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_SYNC_CMD_START;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    }
}

/**
 *  Svc image still capture aaa
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[in] SnapCount snap capture count
 *  @param[out] pIsHiso pointer to the high iso indication
 */
void SvcImg_StillAaa(UINT32 VinId, UINT32 SensorId, UINT32 SnapCount, UINT32 *pIsHiso)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        if (SensorId < SVC_NUM_VIN_SENSOR) {
            /* aux data (aaa) */
            AuxData.Ctx.Data = 0ULL;
            MemInfo.Ctx.pUint32 = pIsHiso;
            AuxData.Ctx.StillAaa.pIsHiso = (UINT64) MemInfo.Ctx.Data;
            /* cmd: aaa */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.StillAaa.SnapCount = (UINT16) (SnapCount & 0xFFFFU);
            CmdMsg.Ctx.StillAaa.ChainId = (UINT8) SensorId;
            CmdMsg.Ctx.StillAaa.Cmd = (UINT8) SVC_CAPTURE_CMD_AAA;
            AmbaImgMain_ConfigEx(VinId, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
        }
    }
}

/**
 *  Svc image still captuer iso configuration
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[in] pCfa pointer to the cfa statistics
 *  @param[in] IsPiv piv indication
 *  @param[in] IsHiso high iso indication
 *  @param[in] pImgMode pointer to image mode configuration
 */
void SvcImg_StillIso(UINT32 VinId, UINT32 SensorId, void *pCfa, UINT32 IsPiv, UINT32 IsHiso, void *pImgMode)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        if (SensorId < SVC_NUM_VIN_SENSOR) {
            /* aux data (image mode) */
            AuxData.Ctx.Data = 0ULL;
            MemInfo.Ctx.pVoid = pImgMode;
            AuxData.Ctx.ImageChanImgMode.Data = (UINT64) MemInfo.Ctx.Data;
            /* cmd: image mode */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.ImageChanImgMode.Type = (UINT8) AMBA_IK_PIPE_STILL;
            CmdMsg.Ctx.ImageChanImgMode.ChainId = (UINT8) SensorId;
            CmdMsg.Ctx.ImageChanImgMode.Msg = (UINT8) SVC_CHAN_MSG_IMG_MODE;
            CmdMsg.Ctx.ImageChanImgMode.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
            AmbaImgMain_ConfigEx(VinId, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
            /* aux data (iso) */
            AuxData.Ctx.Data = 0ULL;
            MemInfo.Ctx.pVoid = pCfa;
            AuxData.Ctx.StillIso.pCfa = (UINT64) MemInfo.Ctx.Data;
            /* cmd: iso */
            CmdMsg.Ctx.Data = 0UL;
            CmdMsg.Ctx.StillIso.IsPiv = (UINT8) (IsPiv & 0xFFU);
            CmdMsg.Ctx.StillIso.IsHiso = (UINT8) (IsHiso & 0xFFU);
            CmdMsg.Ctx.StillIso.ChainId = (UINT8) SensorId;
            CmdMsg.Ctx.StillIso.Cmd = (UINT8) SVC_CAPTURE_CMD_ISO;
            AmbaImgMain_ConfigEx(VinId, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
        }
    }
}

/**
 *  Svc image still capture stop
 *  @param[in] VinId vin id
 */
void SvcImg_StillStop(UINT32 VinId)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        /* cmd: capture stop */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_CAPTURE_CMD_STOP;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);

        /* cmd: sync stop */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_SYNC_CMD_STOP;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    }
}

/**
 *  Svc image still capture exif information
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[out] pExifInfo Pointer to the exif information pointer
 */
void SvcImg_StillExif(UINT32 VinId, UINT32 SensorId, void **pExifInfo)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        if (SensorId < SVC_NUM_VIN_SENSOR) {
            /* aux data (still exif) */
            AuxData.Ctx.Data = 0ULL;
            MemInfo.Ctx.pPVoid = pExifInfo;
            AuxData.Ctx.StillExif.Var = (UINT64) MemInfo.Ctx.Data;
            /* cmd: still exif */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.StillExif.ChainId = (UINT8) SensorId;
            CmdMsg.Ctx.StillExif.Cmd = (UINT8) SVC_CAPTURE_CMD_EXIF;
            AmbaImgMain_ConfigEx(VinId, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
        }
    }
}
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
/**
 *  Svc image still capture piv
 *  @param[in] VinId vin id
 *  @param[in] SensorId sensor id
 *  @param[in] RawCapSeq
 *  @param[out] pIsHiso pointer to the high iso indication
 */
void SvcImg_StillPiv(UINT32 VinId, UINT32 SensorId, UINT32 RawCapSeq, UINT32 *pIsHiso)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        if (SensorId < SVC_NUM_VIN_SENSOR) {
            /* aux data (still piv) */
            AuxData.Ctx.Data = 0ULL;
            MemInfo.Ctx.pUint32 = pIsHiso;
            AuxData.Ctx.StillPiv.pIsHiso = (UINT64) MemInfo.Ctx.Data;
            /* cmd: still piv */
            CmdMsg.Ctx.Data = 0ULL;
            CmdMsg.Ctx.StillPiv.RawCapSeq = RawCapSeq;
            CmdMsg.Ctx.StillPiv.ChainId = (UINT8) SensorId;
            CmdMsg.Ctx.StillPiv.Cmd = (UINT8) SVC_CAPTURE_CMD_PIV;
            AmbaImgMain_ConfigEx(VinId, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
        }
    }
}
#endif
#ifdef CONFIG_BUILD_IMGFRW_SMC
/**
 *  Svc image slow motion captue on
 *  @param[in] VinId vin id
 */
void SvcImg_SmcOn(UINT32 VinId)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        /* cmd: smc on */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_SMC_CMD_ON;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    }
}

/**
 *  Svc image slow motion captue off
 *  @param[in] VinId vin id
 */
void SvcImg_SmcOff(UINT32 VinId)
{
    SVC_IMG_CMD_MSG_s CmdMsg;

    if (VinId < AMBA_NUM_VIN_CHANNEL) {
        /* cmd: smc on */
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_SMC_CMD_OFF;
        AmbaImgMain_Config(VinId, CmdMsg.Ctx.Data);
    }
}
#endif
#ifdef CONFIG_BUILD_IMGFRW_EFOV
/**
 *  Svc image external fov tx hook
 *  @param[in] pFunc Pointer to the tx callback function
 */
void SvcImg_EFovTxHook(SVC_IMG_EFOV_TX_CB_f pFunc)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    /* aux data (efov) */
    AuxData.Ctx.Data = 0ULL;
    MemInfo.Ctx.pFunc = pFunc;
    AuxData.Ctx.Bits.Var = (UINT64) MemInfo.Ctx.Data;
    /* cmd: efov tx cb */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_EFOV_CMD_TXCB;
    AmbaImgMain_ConfigEx(0U, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
}

/**
 *  Svc image external fov rx put
 *  @param[in] RxData rx data
 *  @param[in] Size rx data size
 */
void SvcImg_EFovRxPut(UINT64 RxData, UINT32 Size)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    /* aux data (efov) */
    AuxData.Ctx.Data = 0ULL;
    AuxData.Ctx.Bits.Var = RxData;
    /* cmd: efov tx cb */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Var = Size;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_EFOV_CMD_RX;
    AmbaImgMain_ConfigEx(0U, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
}
#endif
/**
 *  Svc image ae stable wait
 *  @param[in] VinId vin id
 *  @param[in] ChainId sensor id
 */
UINT32 SvcImg_AeStableWait(UINT32 VinId, UINT32 SensorId, UINT32 Timeout)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_CMD_MSG_RET_s RetMsg = {0};
    SVC_IMG_MEM_INFO_s MemInfo;

    /* aux data (ret code) */
    AuxData.Ctx.Data = 0ULL;
    MemInfo.Ctx.pRetMsg = &RetMsg;
    AuxData.Ctx.Bits.Var = (UINT64) MemInfo.Ctx.Data;
    /* cmd: ae stable wait */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits2.Var = Timeout;
    CmdMsg.Ctx.Bits2.ChainId = (UINT8) SensorId;
    CmdMsg.Ctx.Bits2.Msg = (UINT8) SVC_ALGO_MSG_AE_STABLE_WAIT;
    CmdMsg.Ctx.Bits2.Cmd = (UINT8) SVC_IMAGE_CMD_ALGO;
    AmbaImgMain_ConfigEx(VinId, CmdMsg.Ctx.Data, AuxData.Ctx.Data);

    return RetMsg.RetCode;
}
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
/**
 *  Svc image liv warp info
 *  @param[in] FovId fov id (bitwise)
 *  @param[in] pWarpInfo pointer of warp information
 */
void SvcImg_LivWarpInfo(UINT32 FovId, AMBA_IK_WARP_INFO_s *pWarpInfo)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    /* aux data (ret code) */
    AuxData.Ctx.Data = 0ULL;
    MemInfo.Ctx.pWarpInfo = pWarpInfo;
    AuxData.Ctx.Bits.Var = (UINT64) MemInfo.Ctx.Data;
    /* cmd: ae stable wait */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Var = FovId;
    CmdMsg.Ctx.Bits.Msg = (UINT8) SVC_CHAN_MSG_FOV_WARP;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_IMAGE_CMD_CHAN;
    AmbaImgMain_ConfigEx(0U/*don't care*/, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
}
#endif
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
/**
 *  Svc image iso update callback hook
 *  @param[in] pFunc Pointer to the iso update callback function
 */
void SvcImg_IsoDisCbHook(SVC_IMG_ISO_CB_f pFunc)
{
    SVC_IMG_CMD_MSG_s CmdMsg;
    SVC_IMG_AUX_DATA_s AuxData;

    SVC_IMG_MEM_INFO_s MemInfo;

    /* aux data (iso dis cb) */
    AuxData.Ctx.Data = 0ULL;
    MemInfo.Ctx.pIsoCb = pFunc;
    AuxData.Ctx.Bits.Var = (UINT64) MemInfo.Ctx.Data;
    /* cmd: iso dis cb */
    CmdMsg.Ctx.Data = 0ULL;
    CmdMsg.Ctx.Bits.Cmd = (UINT8) SVC_IMAGE_CMD_ISO_CB;
    AmbaImgMain_ConfigEx(0U, CmdMsg.Ctx.Data, AuxData.Ctx.Data);
}
#endif
#if 0
/**
 *  Svc image convert value to pointer
 *  @param [out] pParam1 pointer to dest parameter
 *  @param [in] Param2 src parameter
 */
void SvcImg_MemV2P(void *pParam1, const SVC_IMG_MEM_ADDR Param2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &Param2, sizeof(SVC_IMG_MEM_ADDR));
    if (FuncRetCode != 0UL) {
        /* */
    }
}

/**
 *  Svc image convert pointer to pointer
 *  @param [out] pParam1 pointer to dest parameter
 *  @param [in] pParam2 pointer to src parameter
 */
void SvcImg_MemP2P(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &pParam2, sizeof(void *));
    if (FuncRetCode != 0UL) {
        /* */
    }
}

/**
 *  Svc image convert pointer to value
 *  @param [out] pParam1 pointer to dest parameter
 *  @param [in] pParam2 pointer to src parameter
 */
void SvcImg_MemP2V(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, &pParam2, sizeof(void *));
    if (FuncRetCode != 0UL) {
        /* */
    }
}

/**
 *  Svc image convert alt pointer to pointer
 *  @param [out] pParam1 pointer to dest parameter
 *  @param [in] pParam2 pointer to src parameter
 */
void SvcImg_MemF2P(void *pParam1, const void *pParam2)
{
    UINT32 FuncRetCode;

    FuncRetCode = AmbaWrap_memcpy(pParam1, pParam2, sizeof(void *));
    if (FuncRetCode != 0UL) {
        /* */
    }
}
#endif
