/**
 *  @file SvcLiveviewTask.c
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
 *  @details svc liveview task
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaDef.h"
#include "AmbaDSP.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaHDMI_Def.h"
#include "AmbaCache.h"
#include "AmbaSensor.h"
#include "AmbaFPD.h"
#ifdef CONFIG_ICAM_SWPLL_CONTROL
#include "AmbaFTCM.h"
#endif
#include "AmbaVfs.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcIK.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#include "SvcEnc.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcSysStat.h"
#include "SvcUcode.h"
#include "SvcInfoPack.h"
#include "SvcIsoCfg.h"
#include "SvcResCfg.h"
#include "SvcIKCfg.h"
#include "SvcUserPref.h"
#include "SvcTaskList.h"
#include "SvcDataGather.h"
#include "SvcPlat.h"
#ifdef CONFIG_ICAM_PLAYBACK_USED
#include "SvcPbkCtrl.h"
#include "SvcPbkInfoPack.h"
#endif
#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcBuffer.h"
#ifdef CONFIG_ICAM_SWPLL_CONTROL
#include "SvcSwPll.h"
#endif
#include "SvcAppStat.h"
#if defined(CONFIG_ICAM_BIST_UCODE)
#include "SvcUcBIST.h"
#endif
#if defined(CONFIG_ICAM_ENCRYPTION)
#include "SvcEncrypt.h"
#endif
#include "SvcLiveviewTask.h"
#include "SvcSafeStateMgr.h"
#include "SvcDSP.h"
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
#include "SvcSafeStateMgr.h"
#endif

#define SVC_LOG_LIVEVIEW_TASK "LIVEVIEW_TASK"
#define SYS_CFG_QUEUE_LEN   (32U)

typedef struct {
    UINT8   Idx[SYS_CFG_QUEUE_LEN];
    UINT32  Val[SYS_CFG_QUEUE_LEN];
    UINT8   Num;
} LV_DSP_SYS_CFG_s;

static UINT32 LiveviewTaskWaitSrcFlag = 0U;

static LV_DSP_SYS_CFG_s LvDspSysCfg = {0};

/* Internal Function */
static void   LiveviewTask_WaitSrcDone(void);
static UINT32 LiveviewTask_IKConfig(void);
static void   LiveviewTask_FeedYuvData(void);

#if (defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52))
static void LiveviewTask_SetSystemCfg(UINT8 ParIdx, UINT32 Val)
{
#if defined(CONFIG_THREADX)
    extern void AmbaDSP_SetSystemCfg(UINT8 ParIdx, UINT32 Val);

    AmbaDSP_SetSystemCfg(ParIdx, Val);
#else
    extern UINT32 AmbaDSP_SetSystemCfg(UINT8 ParIdx, UINT32 Val);

    if (0U != AmbaDSP_SetSystemCfg(ParIdx, Val)) {
        SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## AmbaDSP_SetSystemCfg() failed with %d, %d", (UINT32)ParIdx, Val);
    }
#endif
}
#endif

/**
 * Init liveview. Prepare the resource needed to control liveview.
 * return 0-OK, 1-NG
 */
UINT32 SvcLiveviewTask_Init(void)
{
    extern UINT32 AmbaDSP_SetDebugLevel(UINT32 Module, UINT32 Level, UINT8 CmdType);
    extern UINT32 AmbaDSP_SetDebugThread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType);

    SVC_USER_PREF_s *pSvcUserPref;

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ Init Begin", 0U, 0U);

    if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
        (void)AmbaDSP_SetDebugLevel(0xFFFFFFFFU, pSvcUserPref->DspDbgLvl, 0U);
        (void)AmbaDSP_SetDebugThread(3U, pSvcUserPref->DspDbgThd, 0U);
    } else {
        SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## SvcLiveviewTask_Init() failed with get preference error", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ Init Done", 0U, 0U);

    return SVC_OK;
}

/**
 * Config Liveview and prepare necessary resource
 * return 0-OK, 1-NG
 */
UINT32 SvcLiveviewTask_Config(void)
{
    UINT32 RetVal;
    SVC_LIV_INFO_s LivInfo;
    SVC_INFO_PACK_LIV_FOV_BUF_ARR_s LivFovBufArr = {NULL};

    AmbaMisra_TouchUnused(&LvDspSysCfg);

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ SvcLiveviewTask_Config begin", 0U, 0U);
    /* liveview configuration */
    AmbaSvcWrap_MisraMemset(&LivInfo, 0, sizeof(LivInfo));
    SvcLiveview_InfoGet(&LivInfo);

    AmbaSvcWrap_MisraMemset(LivInfo.pDspRes, 0, sizeof(AMBA_DSP_RESOURCE_s));

    SvcInfoPack_LivVinCap(LivInfo.pNumVin, LivInfo.pVinPostBitsSet, LivInfo.pVinCapWin);

    LivFovBufArr.pPyramidBufArr = LivInfo.pPyramidBuf;
    LivFovBufArr.pPyramidBufTbl = LivInfo.pPyramidTbl;
    LivFovBufArr.pPyramidExScaleBufArr = LivInfo.pPyramidExScaleBuf;
    LivFovBufArr.pPyramidExScaleBufTbl = LivInfo.pPyramidExScaleTbl;
    LivFovBufArr.pMainY12BufArr = LivInfo.pMainY12Buf;
    LivFovBufArr.pMainY12BufTbl = LivInfo.pMainY12Tbl;
    LivFovBufArr.pFeedExtDataBufArr = LivInfo.pFeedExtDataBuf;
    LivFovBufArr.pFeedExtDataBufTbl = LivInfo.pFeedExtDataTbl;
    SvcInfoPack_LivFovCfg(LivInfo.pNumFov,
                          LivInfo.pFovCfg,
                          &LivFovBufArr);
#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
    SvcInfoPack_LivLowDlyCfg(LivInfo.pLowDlyCfg);
#endif
    SvcInfoPack_LivStrmCfg(LivInfo.pNumStrm, LivInfo.pStrmCfg, LivInfo.pStrmChan);

#if defined(CONFIG_ICAM_PLAYBACK_USED)
    {
        UINT32  i, Src;

        /* only do decode resource config if duplex fov exists */
        for (i = 0U; i < *LivInfo.pNumFov; i++) {
            Src = 255U;
            RetVal = SvcResCfg_GetFovSrc(i, &Src);
            if ((SVC_OK == RetVal) && (Src == SVC_VIN_SRC_MEM_DEC)) {
                SvcInfoPack_PbkResConfig(LivInfo.pDspRes);
                break;
            }
        }
    }
#endif

#if defined(CONFIG_ICAM_BIST_UCODE)
    {
        ULONG   BufAddr;
        UINT32  BufSize, Interval = (UINT32)CONFIG_ICAM_BIST_UCODE_INTERVAL;

        #if defined(CONFIG_ICAM_RECORD_USED)
        if (SvcBuffer_Request(SVC_BUFFER_SHARED,
                              SMEM_PF0_ID_VENC_BS,
                              &BufAddr,
                              &BufSize) != OK) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to get enc buffer", 0U, 0U);
        }
        #else
        BufAddr = 0U;
        BufSize = 0U;
        #endif

        if (SvcUcBIST_Config(Interval, BufAddr, BufSize) != SVC_OK) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to config ucode BIST", 0U, 0U);
        }
    }
#endif

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    {
        const SVC_RES_CFG_MAX_s *pMaxCfg = SvcResCfg_GetMax();
        RetVal = SvcSafeStateMgr_SetIDspWDT(pMaxCfg->MaxVinTimeout);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "SvcSafeStateMgr_SetIDspWDT() failed with %d", RetVal, 0U);
        }
    }
#endif

    SvcInfoPack_DspResource(LivInfo.pDspRes);
    SvcInfoPack_DefRawCfg(&(LivInfo.pDspRes->LiveviewResource));
    SvcLiveview_Config();

#if (defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52))
    {
        LiveviewTask_SetSystemCfg(31U, 50);
        SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "Hack ::: AmbaDSP_SetSystemCfg(31, 50)", 0U, 0U);

#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        LiveviewTask_SetSystemCfg(0U, 240U);       /* max_dram_par_num */
        LiveviewTask_SetSystemCfg(1U, 56U);        /* max_smem_par_num */
        LiveviewTask_SetSystemCfg(2U, 45U);        /* max_sub_par_num */
        LiveviewTask_SetSystemCfg(3U, 44U);
        LiveviewTask_SetSystemCfg(4U, 282U);
        LiveviewTask_SetSystemCfg(7U, 26U);        /* max. cirbuf */
        LiveviewTask_SetSystemCfg(8U, 76U);        /* max_bdt_num */
        LiveviewTask_SetSystemCfg(9U, 4445U);      /* max_bd_num */
        LiveviewTask_SetSystemCfg(11U, 562U);      /* max_fb_num */
        LiveviewTask_SetSystemCfg(12U, 6U);
        LiveviewTask_SetSystemCfg(13U, 132U);      /* max mcb */
        LiveviewTask_SetSystemCfg(17U, 193U);      /* max_mfbp_num */
        LiveviewTask_SetSystemCfg(18U, 1150U);     /* max_mfb_num */
        LiveviewTask_SetSystemCfg(20U, 492U);      /* max_mimginf_num */
        LiveviewTask_SetSystemCfg(30U, 101U);      /* max_orc_all_msg_qs */
        LiveviewTask_SetSystemCfg(41U, 3U);

        {
            UINT32 i = 0U;
            for (i = 0U; i < LvDspSysCfg.Num; i++) {
                UINT8 ParIdx = LvDspSysCfg.Idx[i];
                UINT32 Val = LvDspSysCfg.Val[i];

                LiveviewTask_SetSystemCfg(ParIdx, Val);
            }
        }
#endif
    }
#endif

    LiveviewTask_FeedYuvData();

#if defined(CONFIG_ICAM_ENCRYPTION)
    {
        ULONG                 BufAddr = 0U;
        UINT32                BufSize = 0U, Err;
        SVC_ENCRYPT_INIT_s    Init;

        Err = SvcBuffer_Request(SVC_BUFFER_FIXED,
                                FMEM_ID_ENCRYPT,
                                &BufAddr,
                                &BufSize);

        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to get encryption buffer", 0U, 0U);
        } else {
            Init.EncryptType = SVC_ENCRYPT_TYPE_AES;
            Init.BufBase = BufAddr;
            Init.BufSize = BufSize;

            Err = SvcEncrypt_Init(&Init);
            if (Err != SVC_OK) {
                SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to init encryption", 0U, 0U);
            }
        }
    }
#endif

    /* IK configuration */
    RetVal = LiveviewTask_IKConfig();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "@@ SvcLiveviewTask_Config failed (%d)", RetVal, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ SvcLiveviewTask_Config end", 0U, 0U);
    }

    return RetVal;
}

/**
 * SFix iso configuration for bring up
 * return 0-OK, 1-NG
 */
UINT32 SvcLiveviewTask_FixIsoCfg(void)
{
    UINT32 RetVal = SVC_OK;
    AMBA_IK_MODE_CFG_s ImgMode = {0};
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 i;

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ SvcLiveviewTask_FixIsoCfg begin", 0U, 0U);
    for (i = 0U; i < pCfg->FovNum; i++) {
        ImgMode.ContextId = i;

        SvcIsoCfg_Fixed(&ImgMode, 0);
    }

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ SvcLiveviewTask_FixIsoCfg end", 0U, 0U);

    return RetVal;
}

/**
 * Start Liveview
 * return 0-OK, 1-NG
 */
UINT32 SvcLiveviewTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    SVC_LIV_FOV_CTRL_s LivFovCtrl = {0};
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 i;

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "Liveview start", 0U, 0U);

    /* liveview control */
    for (i = 0U; i < pCfg->FovNum; i++) {
        LivFovCtrl.FovIDArr[i]  = (UINT16)i;
        LivFovCtrl.EnableArr[i] = 1U;
    }
    SvcLiveview_Ctrl(pCfg->FovNum, &LivFovCtrl);

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    if (SvcDSP_IsBootDone() == 1U) {
        RetVal = SvcSafeStateMgr_LiveviewStart();
        if (RetVal != OK) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "SvcSafeStateMgr_LiveviewStart return 0x%x", RetVal, 0U);
        }
    }
#endif

#if defined(CONFIG_ICAM_RECORD_USED) && defined(CONFIG_ICAM_DATA_GATHER_USED)
    {
        UINT32 Err, FovSrc, Enable = 0U;

        for (i = 0U; i < pCfg->FovNum; i++) {
            FovSrc = 255U;
            Err = SvcResCfg_GetFovSrc(i, &FovSrc);
            if ((SVC_OK == Err) && (FovSrc != SVC_VIN_SRC_MEM_DEC)) {
                Enable = 1U;
                break;
            }
        }

        if (Enable == 1U) {
            if (SvcDataGather_Create(SVC_DATA_GATHER_TASK_PRI, SVC_DATA_GATHER_TASK_CPU_BITS) != SVC_OK) {
                SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "fail to create data gather", 0U, 0U);
            }
        }
    }
#endif

#if defined(CONFIG_ICAM_SWPLL_CONTROL)
    {
        SVC_SW_PLL_CFG_s Cfg;
        Cfg.Priority = 60;
        Cfg.CpuBits = 0;
        RetVal = SvcSwPll_Create(&Cfg);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "SvcSwPll_Create fail, return 0x%x", RetVal, 0U);
        }
    }
#endif

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "Liveview start done", 0U, 0U);

    return RetVal;
}

/**
 * Stop Liveview
 * return 0-OK, 1-NG
 */
UINT32 SvcLiveviewTask_Stop(void)
{
    SVC_LIV_FOV_CTRL_s LivFovCtrl = {0};
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 i;

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "Liveview stop", 0U, 0U);

#if defined(CONFIG_ICAM_RECORD_USED) && defined(CONFIG_ICAM_DATA_GATHER_USED)
    if (SvcDataGather_Destroy() != SVC_OK) {
        SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "fail to destroy data gather", 0U, 0U);
    }
#endif

    for (i = 0U; i < pCfg->FovNum; i++) {
        LivFovCtrl.FovIDArr[i] = (UINT16)i;
        LivFovCtrl.EnableArr[i] = 0U;
    }

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    if (SvcDSP_IsBootDone() == 1U) {
        UINT32 RetVal;
        RetVal = SvcSafeStateMgr_LiveviewStop();
        if (RetVal != OK) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "SvcSafeStateMgr_LiveviewStop return 0x%x", RetVal, 0U);
        }
    }
#endif

    SvcLiveview_Ctrl(pCfg->FovNum, &LivFovCtrl);

#if defined(CONFIG_ICAM_SWPLL_CONTROL)
    {
        UINT32 RetVal;
        RetVal = SvcSwPll_Delete();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "SvcSwPll_Delete() failed with %d", RetVal, 0U);
        }
    }
#endif

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "Liveview stop done", 0U, 0U);

    return SVC_OK;
}

/**
 * Update Liveview
 * return 0-OK, 1-NG
 */
UINT32 SvcLiveviewTask_Update(void)
{
    UINT32 StrmNum;
    UINT32 i;
    SVC_LIV_INFO_s LivInfo;

    /* liveview configuration */
    SvcLiveview_InfoGet(&LivInfo);
    SvcInfoPack_LivStrmCfg(LivInfo.pNumStrm, LivInfo.pStrmCfg, LivInfo.pStrmChan);

    StrmNum = *(LivInfo.pNumStrm);

    for (i = 0U; i < StrmNum; i++) {
        SvcLiveview_UpdateStream(i);
    }

    return SVC_OK;
}

/**
* status callback function
* @param [in] StatIdx index of status
* @param [in] pInfo information
* @return none
*/
void SvcLiveviewTask_StatusCB(UINT32 StatIdx, void *pInfo)
{
    const SVC_APP_STAT_UCODE_s *pUcodeStatus = NULL;

    AmbaMisra_TypeCast(&pUcodeStatus, &pInfo);
    AmbaMisra_TouchUnused(pInfo);

    if (pUcodeStatus != NULL) {
        switch (StatIdx) {
            case SVC_APP_STAT_UCODE:
                if (pUcodeStatus->Status == SVC_APP_STAT_UCODE_DEFBIN_DONE) {
                    LiveviewTaskWaitSrcFlag = 1U;
                }
            break;
            default:
                SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "Unkown StatIdx (%d)", StatIdx, 0U);
            break;
        }
    }
}

static void LiveviewTask_WaitSrcDone(void)
{
    INT32 WaitTimeOut = 30000;

    do {
        if (LiveviewTaskWaitSrcFlag == 1U) {
            break;
        }

        WaitTimeOut -= 5;
        (void) AmbaKAL_TaskSleep(5U);
    } while (WaitTimeOut > 0);

    if ((LiveviewTaskWaitSrcFlag != 0U) && (WaitTimeOut <= 0)) {
        SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "Wait source done timeout", 0U, 0U);
    }
}

static UINT32 LiveviewTask_IKConfig(void)
{
    UINT32                           i, RetVal;
    AMBA_IK_MODE_CFG_s               ImgMode = {0};
    AMBA_IK_DZOOM_INFO_s             Dzoom = {0};
    SVC_IK_FOV_WIN_s                 IKFovWin = {0};
    UINT32                           FovIdx, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0}, FovNum = 0U;
    UINT32                           Src, Rval;
    const SVC_RES_CFG_s             *pCfg = SvcResCfg_Get();

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ IKConfig begin", 0U, 0U);

    {
        extern UINT32 AmbaIK_InitDefBinary(void *pBinDataAddr) GNU_WEAK_SYMBOL;
        typedef UINT32 (*PFN_IK_INIT)(void *pBinDataAddr);
        PFN_IK_INIT pfnIKInit = AmbaIK_InitDefBinary;
        static UINT8 IKDefBinInit = 0U;

        if ((IKDefBinInit == 0U) && (pfnIKInit != NULL)) {
            ULONG DefBinAddr = 0U;
            void *pBinDataAddr = NULL;

            LiveviewTask_WaitSrcDone();

            SvcUcode_GetInfo(UCODE_FILE_DEFBIN, &DefBinAddr, NULL, NULL);
            AmbaMisra_TypeCast(&(pBinDataAddr), &(DefBinAddr));

            if (pBinDataAddr != NULL) {
                Rval = pfnIKInit(pBinDataAddr);
                if (Rval != 0U) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to config default binary, rval(0x%08x)", Rval, 0U);
                } else {
                    IKDefBinInit = 1U;
                }
            }
        }
    }

    /* IK context init */
    RetVal = SvcIK_CtxInit();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to context init", 0U, 0U);
    }

    RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to get fov idx and fov num", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        for (i = 0U; i < FovNum; i++) {
            FovIdx = FovIdxs[i];

            Src = 255U;
            Rval = SvcResCfg_GetFovSrc(FovIdx, &Src);
            ImgMode.ContextId = FovIdx;
            if (SVC_OK == Rval) {
                if (Src == SVC_VIN_SRC_MEM_DEC) {
                    continue;
                }
            }

            ImgMode.ContextId = FovIdx;
            if ( (Src == SVC_VIN_SRC_SENSOR) || (Src == SVC_VIN_SRC_YUV)) {
                /* IK sensor config */
                RetVal = SvcIK_SensorConfig(&ImgMode);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to config IK sensor", 0U, 0U);
                }
            }
            /* IK window config */
            SvcInfoPack_IKFovWin(FovIdx, &IKFovWin);
            RetVal = SvcIK_FovWinConfig(&ImgMode, &IKFovWin);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to config window", 0U, 0U);
            }

            if ((Src == SVC_VIN_SRC_SENSOR) || (Src == SVC_VIN_SRC_MEM)) {
                UINT32 RawWidth      = pCfg->FovCfg[FovIdx].RawWin.Width;
                UINT32 RawHeight     = pCfg->FovCfg[FovIdx].RawWin.Height;
                INT32 ActiveOffsetX  = (INT32) pCfg->FovCfg[FovIdx].ActWin.OffsetX;
                INT32 ActiveOffsetY  = (INT32) pCfg->FovCfg[FovIdx].ActWin.OffsetY;
                UINT32 ActiveWidth   = pCfg->FovCfg[FovIdx].ActWin.Width;
                UINT32 ActiveHeight  = pCfg->FovCfg[FovIdx].ActWin.Height;

                /* IK warp control */
                RetVal = SvcIK_ImgWarpCtrl(&ImgMode, 0U);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to control warp", 0U, 0U);
                }

                /* IK dzoom control, ShiftX and ShiftY 0 will be the center of the preview after Dzoom */
                if ((ActiveWidth == 0U) || (ActiveHeight == 0U)) {
                    Dzoom.Enable = 1U;
                    Dzoom.ShiftX = 0;
                    Dzoom.ShiftY = 0;
                    Dzoom.ZoomX  = 65536U;
                    Dzoom.ZoomY  = 65536U;
                } else {
                    Dzoom.Enable = 1U;
                    Dzoom.ShiftX = (INT32) ((ActiveOffsetX - (((INT32) RawWidth - (INT32) ActiveWidth) / 2)) * 65536);
                    Dzoom.ShiftY = (INT32) ((ActiveOffsetY - (((INT32) RawHeight - (INT32) ActiveHeight) / 2)) * 65536);
                    Dzoom.ZoomX  = (UINT32) (65536U * RawWidth / ActiveWidth);
                    Dzoom.ZoomY  = (UINT32) (65536U * RawHeight / ActiveHeight);
                }
                RetVal = SvcIK_ImgDzoomCtrl(&ImgMode, &Dzoom);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "## fail to control dzoom", 0U, 0U);
                } else {
                    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "## Dzoom Ctrl Fov[%d] ##", FovIdx, 0U);
                    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, " Enable : %d", Dzoom.Enable, 0U);
                    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, " ActWin.OffsetX : %d", (UINT32) ActiveOffsetX, 0U);
                    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, " ActWin.OffsetY : %d", (UINT32) ActiveOffsetY, 0U);
                    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, " Actwin.Width   : %d", (UINT32) ActiveWidth, 0U);
                    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, " Actwin.Height  : %d", (UINT32) ActiveHeight, 0U);
                }
            }
        }
    }

    SvcLog_DBG(SVC_LOG_LIVEVIEW_TASK, "@@ IKConfig end", 0U, 0U);

    return RetVal;
}

static void LiveviewTask_FeedYuvData(void)
{
    AMBA_DSP_EXT_YUV_BUF_s  ExtYuv = {0U};
    UINT8                   *pBuf;
    UINT16                  i;
    UINT32                  Err, Src, BufHeight, BufPitch, BufSize = 0U, CalcSize;
    ULONG                   BufAddr = 0U;
    const SVC_RES_CFG_s     *pCfg = SvcResCfg_Get();

    for (i = 0U; i < (UINT16)(pCfg->FovNum & 0xFFFFU); i++) {
        /* feed yuv data if the fov is for duplex decode */
        Src = 255U;
        Err = SvcResCfg_GetFovSrc(i, &Src);
        if ((SVC_OK == Err) && ((Src == SVC_VIN_SRC_MEM_DEC) || (Src == SVC_VIN_SRC_MEM_YUV420) || (Src == SVC_VIN_SRC_MEM_YUV422))) {

            BufPitch   = GetAlignedValU32((UINT32)pCfg->FovCfg[i].RawWin.Width, 64U);
            BufHeight  = GetAlignedValU32((UINT32)pCfg->FovCfg[i].RawWin.Height, 16U);

            if (Src == SVC_VIN_SRC_MEM_YUV422) {
                CalcSize = (BufPitch * BufHeight) * 2U;
            } else {
                CalcSize = ((BufPitch * BufHeight) * 3U) / 2U;
            }

            Err = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_LV_YUVFEED, &BufAddr, &BufSize);
            if ((Err == SVC_OK) && (BufSize >= CalcSize)) {
                ExtYuv.ExtYuvBuf.DataFmt        = AMBA_DSP_YUV420;
                ExtYuv.ExtYuvBuf.Pitch          = (UINT16)BufPitch;
                ExtYuv.ExtYuvBuf.Window.Width   = pCfg->FovCfg[i].RawWin.Width;
                ExtYuv.ExtYuvBuf.Window.Height  = pCfg->FovCfg[i].RawWin.Height;
                ExtYuv.ExtYuvBuf.Window.OffsetX = 0U;
                ExtYuv.ExtYuvBuf.Window.OffsetY = 0U;
                ExtYuv.pExtME1Buf               = NULL;
                ExtYuv.pExtME0Buf               = NULL;
                ExtYuv.ExtYuvBuf.BaseAddrY      = BufAddr;
                ExtYuv.ExtYuvBuf.BaseAddrUV     = BufAddr + ((ULONG)BufPitch * (ULONG)BufHeight);

                CalcSize = BufPitch * BufHeight;

                AmbaMisra_TypeCast(&pBuf, &ExtYuv.ExtYuvBuf.BaseAddrY);
                Err = AmbaWrap_memset(pBuf, 0, (SIZE_t)CalcSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "@@ AmbaWrap_memset failed (%d)", Err, 0U);
                }
                Err = SvcPlat_CacheClean(ExtYuv.ExtYuvBuf.BaseAddrY, (ULONG)CalcSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "@@ SvcPlat_CacheClean failed (%d)", Err, 0U);
                }

                if (Src == SVC_VIN_SRC_MEM_YUV422) {
                    CalcSize = BufPitch * BufHeight;
                } else {
                    CalcSize = (BufPitch * BufHeight) / 2U;
                }

                AmbaMisra_TypeCast(&pBuf, &ExtYuv.ExtYuvBuf.BaseAddrUV);
                Err = AmbaWrap_memset(pBuf, 128, (SIZE_t)CalcSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "@@ AmbaWrap_memset failed (%d)", Err, 0U);
                }
                Err = SvcPlat_CacheClean(ExtYuv.ExtYuvBuf.BaseAddrUV, (ULONG)CalcSize);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "@@ SvcPlat_CacheClean failed (%d)", Err, 0U);
                }

                Err = AmbaDSP_LiveviewFeedYuvData(1U, &i, &ExtYuv);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "@@ AmbaDSP_LiveviewFeedYuvData failed (%d)", Err, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_LIVEVIEW_TASK, "@@ LiveviewFeedYuv buffer not enough(%u/%u)", BufSize, BufPitch * BufHeight);
            }
        }
    }
}

/**
* DSP System Cfg for booting
* @param [in] ParIdx index of Partition
* @param [in] Val value
* @return none
*/
UINT32 SvcLiveviewTask_SetDspSystemCfg(UINT32 ParIdx, UINT32 Val)
{
    UINT32 RetVal = SVC_OK;
    UINT8 Num = LvDspSysCfg.Num;

    if (Num < SYS_CFG_QUEUE_LEN) {
        LvDspSysCfg.Idx[Num] = (UINT8) ParIdx;
        LvDspSysCfg.Val[Num] = Val;
        LvDspSysCfg.Num++;

        RetVal = SVC_OK;
    } else {
        RetVal = SVC_OK;
    }

    return RetVal;
}

