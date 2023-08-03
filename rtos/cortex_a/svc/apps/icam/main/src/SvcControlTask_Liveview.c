/**
 *  @file SvcControlTask_Liveview.c
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
 *  @details svc control task for liveview
 *
 */

#ifndef CTRL_TASK_LIV_H     /* Dir 4.10 */
#define CTRL_TASK_LIV_H

static UINT32 ControlTask_LiveviewStart(void);
static UINT32 ControlTask_LiveviewStop(void);

/**
* The function to start Liveview with display on VOUT
* @return ErrorCode
*/
static UINT32 ControlTask_LiveviewStart(void)
{
    UINT32 RetVal;
    SVC_USER_PREF_s *pSvcUserPref;

    SvcLog_OK(SVC_LOG_CONTROL_TASK, "This is Liveview mode", 0U, 0U);

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcUserPref_Get() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    } else {
        pSvcUserPref->OperationMode = 1U;   /* Liveview mode */
    }

    /* Re-map the shared memory */
    SvcBufMap_Config(SMAP_ID_LIVEVIEW);

    /* Configure IK if necessary */
    RetVal = SvcIKCfg_Config();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcIKCfg_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Config Vout (including lcd device enable/config) based on current resolution */
    if (SVC_OK == RetVal) {
        RetVal = SvcVoutCtrlTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutCtrlTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Config Vin (including sensor enable/config) based on current resolution */
    RetVal = SvcVinCtrlTask_Config();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinCtrlTask_Config() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Wait Vin Src is ready */
    if (SVC_OK == RetVal) {
        RetVal = SvcVinCtrlTask_WaitSrcReady();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinCtrlTask_WaitSrcReady() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Wait Vout Src is ready */
    if (SVC_OK == RetVal) {
        RetVal = SvcVoutCtrlTask_WaitSrcReady();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutCtrlTask_WaitSrcReady() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcDisplayTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcDisplayTask_Start() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
#ifdef CONFIG_ICAM_AUDIO_USED
    /* audio config */
    if (SVC_OK == RetVal) {
        SvcAudioTask_Config();
    }
#endif
#endif

#if defined(CONFIG_ICAM_PLAYBACK_USED)
    /* For duplex case, playback need to be configured */
    if (SVC_OK == RetVal) {
        RetVal = SvcPlaybackTask_DuplexConfig();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcPlaybackTask_DuplexConfig() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_PROJECT_SURROUND)
        /* For surround case, surround cal need to be initial */
{
        AMBA_FS_FILE_INFO_s FileInfo;
        AMBA_FS_FILE *pFile;
        char FileName[64U] = "c:\\CalibDataFile.bin";
        UINT8 Found = 0U;
        UINT32 Rval;

        Rval = AmbaFS_FileOpen(FileName, "r", &pFile);
        if (Rval == SVC_OK) {
            Rval = AmbaFS_GetFileInfo(FileName, &FileInfo);
            if (Rval == SVC_OK) {
                if (FileInfo.Size != 0U) {
                    Found = 1U;
                }
            }
        }
        if (Found == 1U) {
            if (SVC_OK == RetVal) {
                RetVal = SvcAnimTask_Init();
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAnimTask_Init() failed with %d", RetVal, 0U);
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnimTask_Init() failed! Rval = %u", RetVal, 0U, 0U, 0U, 0U);
                }
            }
            if (SVC_OK == RetVal) {
                RetVal = SvcAnimTask_Config();
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAnimTask_Config() failed with %d ", RetVal, 0U);
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnimTask_Config() failed! Rval = %u", RetVal, 0U, 0U, 0U, 0U);
                }
            }
            if (SVC_OK == RetVal) {
                RetVal = SvcAnimTask_UpdateWarp();
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcAnimTask_UpdateWarp() failed with %d ", RetVal, 0U);
                    AmbaPrint_PrintUInt5("[ERROR] SvcAnimTask_UpdateWarp() failed! Rval = %u", RetVal, 0U, 0U, 0U, 0U);
                }
            }

            if (SVC_OK != RetVal) {
                RetVal = SVC_OK;
            }
        }
}
#endif


    /* Config Liveview Task needed resource */
    if (SVC_OK == RetVal) {
        RetVal = SvcLiveviewTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLiveviewTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#if defined(CONFIG_BUILD_MONFRW_GRAPH)
    /* Enable safe task */
    if (SVC_OK == RetVal) {
        RetVal = SvcSafeTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSafeTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#if defined(CONFIG_SVC_VOUT_ERR_CTRL_USED)
    /* Enable vout error task */
    if (SVC_OK == RetVal) {
        if (SVC_OK != SvcVoutErrTask_Start()) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutErrTask_Start() failed", 0U, 0U);
        }
    }
#endif
#endif

#if defined(CONFIG_SVC_VIN_ERR_CTRL_USED)
    /* Start vin error task */
    if (SVC_OK == RetVal) {
        if (SVC_OK != SvcVinErrTask_Start()) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinErrTask_Start() failed", 0U, 0U);
        }
    }
#endif

#if defined(CONFIG_ICAM_FIXED_ISO_CFG_USED)
    /* Fixed iso cfg update for bring up */
    if (SVC_OK == RetVal) {
        RetVal = SvcLiveviewTask_FixIsoCfg();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLiveviewTask_FixIsoCfg() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_IMGCAL_USED)
    /* Update Calib */
    if (SVC_OK == RetVal) {
        RetVal = SvcCalibTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCalibTask_Start() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    /* Enable image/3A task or FixedIsoConfig for debug usage */
    if (SVC_OK == RetVal) {
        RetVal = SvcImgTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcImgTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_RAWENC_USED)
    /* Enable image/3A task or FixedIsoConfig for debug usage */
    if (SVC_OK == RetVal) {
        RetVal = SvcRawEncTask_IsoConfig();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRawEncTask_IsoConfig() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_VIEWCTRL_USED)
    /* Enable mirror pan/tilt effect feature */
    if (SVC_OK == RetVal) {
        RetVal = SvcEmrTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcEmrTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_RECORD_USED)
    /* Enable recorder feature */
    if (SVC_OK == RetVal) {
        RetVal = SvcRecTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRecTask_Config() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_IMGCAL_STITCH_USED)
    /* config emr adaptive task */
    if (SVC_OK == RetVal) {
        RetVal = SvcEmrAdaptiveTask_Config();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcEmrAdaptiveTask_Config() failed", 0U, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

    if (SVC_OK == RetVal) {
        if (SVC_OK != SvcVinBufMonTask_Create()) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinBufMonTask_Create() failed", 0U, 0U);
        }
    }

    /* Start Liveview Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcLiveviewTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLiveviewTask_Start() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
    /* Start Liveview Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcLvFeedExtRawTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLvFeedExtRawTask_Start() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED)
    /* Start Liveview Task */
    if (SVC_OK == RetVal) {
        RetVal = SvcLvFeedExtYuvTask_Start();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLvFeedExtYuvTask_Start() failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }
#endif

#if defined(CONFIG_ICAM_SWPLL_CONTROL)
    if (SVC_OK == RetVal) {
        SVC_SW_PLL_CFG_s Cfg;
        Cfg.Priority = 60;
        Cfg.CpuBits = 0;
        RetVal = SvcSwPll_Create(&Cfg);
    }
#endif

#if defined(CONFIG_ICAM_SENSOR_ASIL_ENABLED)
    if (SVC_OK == RetVal) {
        RetVal = SvcSensorCrcCheckTask_Start();
        if (RetVal != OK) {
            SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSensorCrcCheckTask_Start() failed with %d", RetVal, 0U);
        }
    }
#endif

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CONTROL_TASK, "ControlTask_LiveviewStart() done", 0U, 0U);
    }

    return RetVal;
}

/**
* The function to stop Liveview with display on VOUT
* @return ErrorCode
*/
static UINT32 ControlTask_LiveviewStop(void)
{
    UINT32 RetVal = SVC_OK;

#if defined(CONFIG_ICAM_VIEWCTRL_USED)
    RetVal = SvcEmrTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcEmrTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_BUILD_IMGFRW_AAA)
    RetVal = SvcImgTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcImgTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_SVC_VIN_ERR_CTRL_USED)
    /* Stop vin error task */
    if (SVC_OK != SvcVinErrTask_Stop()) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinErrTask_Stop() failed", 0U, 0U);
    }
#endif

#if defined(CONFIG_BUILD_MONFRW_GRAPH)
#if defined(CONFIG_SVC_VOUT_ERR_CTRL_USED)
    /* Stop vout error task */
    if (SVC_OK != SvcVoutErrTask_Stop()) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutErrTask_Stop() failed", 0U, 0U);
    }
#endif
    RetVal = SvcSafeTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSafeTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_IMGCAL_USED)
    /* Delete Calib */
    RetVal = SvcCalibTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcCalibTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

#if defined(CONFIG_ICAM_RECORD_USED)
    RetVal = SvcRecTask_DeConfig();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcRecTask_DeConfig() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif

    RetVal = SvcLiveviewTask_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcLiveviewTask_Stop() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    RetVal = SvcVinCtrlTask_Destroy();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinCtrlTask_Destroy() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    if (SVC_OK != SvcVinBufMonTask_Delete()) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVinBufMonTask_Delete() failed", 0U, 0U);
    }

    RetVal = SvcVoutCtrlTask_Destroy();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcVoutCtrlTask_Destroy() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

#if defined(CONFIG_ICAM_SWPLL_CONTROL)
    RetVal = SvcSwPll_Delete();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CONTROL_TASK, "SvcSwPll_Delete() failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }
#endif
    return RetVal;
}

#endif /* CTRL_TASK_LIV_H */
