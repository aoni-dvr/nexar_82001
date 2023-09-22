/**
*  @file SvcUserPref.c
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
*
*/

#include ".svc_autogen"

#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaDSP_Capability.h"
#include "AmbaUtility.h"
#include "AmbaSD.h"
#include "SvcPref.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "AmbaKAL.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "SvcTask.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaI2S_Def.h"
#include "AmbaVfs.h"
#include "SvcStgMonitor.h"
#include "SvcUserPref.h"
#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
#include "AmbaWS_FCWS.h"
#include "RefFlow_FC.h"
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "app_helper.h"
#include "user_setting/user_setting.h"
#include "uvc/uvc.h"
#include "AmbaImg_AaaDef.h"
#endif
#define SVC_LOG_USRPREF     "USRPREF"
#if 1//defined(CONFIG_IMU_BMI160)
#include "../../../../../../bsp/h32/peripherals/imu/bmi160_defs.h"
#endif

static SVC_USER_PREF_s SvcUserPrefDefault = {
    .Version                   = SVCAG_PREF_PREFIX".0035",
#ifdef CONFIG_ICAM_PROJECT_NAME
    .ProjectName               = CONFIG_ICAM_PROJECT_NAME,
#else
    .ProjectName               = "SVC_APP",
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    .OperationMode             = 1U,
#else
    .OperationMode             = 0U,    /* 0 is stand-by mode temporally */
#endif
    .FormatId                  = 0U,
    .ShowDspLog                = 0U,
    .ShowSspLog                = 0U,
    .ShowSspApiLog             = 0U,
    .ShowSensorLog             = 0U,
    .ShowBridgeLog             = 0U,
    .ShowYuvLog                = 0U,
    .ShowIKLog                 = 0U,
    .ShowSwpllLog              = 0U,
    .ShowThmViewLog            = 0U,
    .EnableBSD                 = 1U,
#if defined(SVCAG_DISABLE_VIDTHM)
    .EnableVidThm              = 0U,
#else
    .EnableVidThm              = 1U,
#endif
    .UsbClass                  = SVC_STG_USB_CLASS_NONE,
    .DspDbgThd                 = 0U,
    .DspDbgLvl                 = 1U,
#if defined(CONFIG_ICAM_PROJECT_CNNTESTBED) || (defined(CONFIG_ICAM_REBEL_USAGE) && defined(CONFIG_ICAM_REBEL_CUSTOMER_NN))
    .CVBooFlag                 = 6U,
#else
    .CVBooFlag                 = 5U,
#endif
    .CVSchdrLog                = 0U,
#if defined(CONFIG_ICAM_REBEL_USAGE)
    .MaxBitrate                = 50U,
#else
    .MaxBitrate                = 0U,
#endif
#if defined(CONFIG_BUILD_AMBA_ADAS) && defined(CONFIG_ICAM_PROJECT_ADAS_DVR)
    .FcwsMode                  = AMBA_WS_FCWS_MODE_TTC,
    .FcV2_SR_mode              = RF_FC_SR_MODE_HOR,
#endif
    .DcfFormatId               = 1U,

    /* Audio setting */
    .BitsPerSample = 16U,
    .AudioAacBitRate = 128000U,
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    .AudioVolume = 100U,
#endif
    .EnableDMIC  = 0U,

    /* record */
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
    .FileSplitTimeMin = 1U,
#endif
    .LcdBlOnDytyId    = {9, 9, 9},
    /* emr */
    .Emr_1_0 = 0,
    .Emr_2_0 = 0,
    .EmrInfo = { {0} },

    /* mctf compression */
    .MctfCmprCtrl = 1U,
    /* mcts dramout control */
#if defined(ICAMAQ_MCTS_DRAMOUT_DIS)
    .MctsDOutCtrl = 0U,
#else
    .MctsDOutCtrl = 1U,
#endif

    /* main storage */
    .MainStgDrive = CONFIG_ICAM_MAIN_STG_DRV,
    .MainStgChan  = CONFIG_ICAM_MAIN_STG_CHA,

    /* calib for ADAS */
    .CalibFocalLength = 4.47,    //Unit: mm, CAR53 Lens, imx424
    .CalibCellSize    = 0.00225, //CAR53 Lens, imx424

    /* Quick calib for ADAS */
    .SkylineHeight    = 250, //Set 250 by default
    .WarningZoon_AutoRun = 0U,

    /* LDWS ActiveSpeed */
    .LdwsActiveSpeed = 30U,/* km/hr */

#if defined(CONFIG_ICAM_PROJECT_SHMOO)
    /* SHMOO */
    .ShmooInfo = {0},
#endif

    /* CAN  Fixed Speed */
    .CanSpeed = 0U,

    /* BootToRec wait storage done timeout */
    .BootToRecTimeOut = 5000U,    /* ms, could be 0 if user don't want to wait */

    /* PrefBsdChan */
    .PrefBsdChan = 2U,

#if defined(CONFIG_ICAM_CVBS_ADC_FINE_TUNE)
    .CvbsOutGain = 0U,
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    .LinuxBoot                  = OPTION_ON,
    .LinuxConsole               = OPTION_ON,
    //.RtosConsole                = OPTION_ON,
    .AudioVolume                = 2U,
    .FileSplitTimeSeconds       = 60U,
    .LedOnOff                   = OPTION_ON,
    .WatchDogOnOff              = OPTION_ON,
    .RecordSoundOnOff           = OPTION_ON,
    .MotionDetect               = OPTION_OFF,
    //.InternalCamera             = {1, 1, IMAGE_ROTATION_NONE, 30, {2560, 1440, 20, 30}, {1280, 720, 4.0, 30}},
    //.ExternalCamera             = {1, 1, IMAGE_ROTATION_NONE, 30, {1280, 720, 10.0, 30}, {1280, 720, 4.0, 30}},
    //.RtspLiveCamera             = CAMERA_CHANNEL_INTERNAL,
    .IrBrightness               = 80,
    .LedBrightness              = 1,
    .EventBufferLength          = EVENT_RECORD_DEFAULT_PRE_SECONDS,
    .SettingVersion             = USER_SETTING_VER_NUM,
    .AwbMode                    = WB_AUTOMATIC,
    .AeBias                     = 3,//0-6
    .PowerOnOffSound            = 1,
    .WaterMark                  = WATER_MARK_DATETIME | WATER_MARK_LOGO,
    .WaterMarkPos               = WATER_MARK_POS_BOTTOM,
    .BatteryPowerTime           = 5,
    .NoActiveShutdownTime       = 1800,
    .Use24HMode                 = 1,
    .DebugMode                  = 0,
    .TimeZone                   = 28800,//8 * 3600
    .IrSampling                 = 1,
    .IrThresHold                = 3,
    .ImuRecord                  = 1,
    .GnssRecord                 = 1,
    //.WifiSetting                = {WIFI_MODE_STA, WIFI_MODE_AP, 44, 1, {0}, {0}, {0}, {"Bert_5G"}, {"10.18.96.1,,"}, 0},
    //.UsbWifiSetting             = {WIFI_MODE_AP,  WIFI_MODE_AP, 0, 0, {0}, {0}, {0}, {0}, {0}, 0},
#if defined(CONFIG_BSP_H32_NEXAR_D081)
    .GnssSetting                = {3, 9600, GNSS_TYPE_GPS | GNSS_TYPE_GALILEO | GNSS_TYPE_QZSS | GNSS_TYPE_SBAS | GNSS_TYPE_QLONASS, 1},
#else
    .GnssSetting                = {1, 9600, GNSS_TYPE_GPS | GNSS_TYPE_GALILEO | GNSS_TYPE_QZSS | GNSS_TYPE_SBAS | GNSS_TYPE_QLONASS, 1},
#endif
    .ImuSetting                 = {BMI160_ACCEL_ODR_50HZ, BMI160_ACCEL_RANGE_16G, BMI160_GYRO_ODR_50HZ, BMI160_GYRO_RANGE_2000_DPS, 0},
    .UsbMassStorage             = OPTION_OFF,
    .LedRtosControl             = OPTION_ON,
    .VideoEncryption            = {{0}, {0}, 1, 0},
    .AuthorizationToken         = {0},
    .ParkingLevel               = 0,
    .PowerOffWakeupMinutes      = 0,
    .EnableLteRemoteWakeup      = 0,
    .SpeedUnit                  = SPEED_UNIT_MPH,
    .CvOnOff                    = OPTION_OFF,
    .PivOnOff                   = OPTION_ON,
    .LedColor                   = {0x80, 0x80, 0x80},
#endif
};

static SVC_USER_PREF_s *pSvcUserPref;
static UINT8           InitDone = 0U;

/**
 * User preference getting function
 * @param [out] ppSvcUserPref pointer to user perference pointer
 * @return ErrorCode
 */
UINT32 SvcUserPref_Get(SVC_USER_PREF_s** ppSvcUserPref)
{
    UINT32 Rval = SVC_OK;

    if (InitDone == 1U) {
        *ppSvcUserPref = pSvcUserPref;
    } else {
        SvcLog_NG(SVC_LOG_USRPREF, "Need to do SvcUserPref_Init first!!", 0U, 0U);
        *ppSvcUserPref = NULL;
        Rval = SVC_NG;
    }
    return Rval;
}

void SvcUserPref_ResetNetworkSetting(SVC_USER_PREF_s *pSvcUserPref)
{
    pSvcUserPref->WifiSetting.BootMode = WIFI_MODE_P2P;
    pSvcUserPref->WifiSetting.ConnectMode = WIFI_MODE_AP;
    pSvcUserPref->WifiSetting.Channel = 44;
    pSvcUserPref->WifiSetting.Use5G = 1;
    AmbaWrap_memset(pSvcUserPref->WifiSetting.ApSSID, 0, sizeof(pSvcUserPref->WifiSetting.ApSSID));
    AmbaWrap_memset(pSvcUserPref->WifiSetting.ApPassword, 0, sizeof(pSvcUserPref->WifiSetting.ApPassword));
    AmbaWrap_memset(pSvcUserPref->WifiSetting.ApIP, 0, sizeof(pSvcUserPref->WifiSetting.ApIP));
    AmbaWrap_memset(pSvcUserPref->WifiSetting.StaSSID, 0, sizeof(pSvcUserPref->WifiSetting.ApSSID));
    AmbaUtility_StringCopy(pSvcUserPref->WifiSetting.StaSSID, 128, "Bert_5G");
    AmbaWrap_memset(pSvcUserPref->WifiSetting.StaPassword, 0, sizeof(pSvcUserPref->WifiSetting.StaPassword));
    AmbaUtility_StringCopy(pSvcUserPref->WifiSetting.StaPassword, 128, "10.18.96.1,,");
    pSvcUserPref->WifiSetting.P2pWaitTime = 0;

    pSvcUserPref->UsbWifiSetting.BootMode = WIFI_MODE_STA;
    pSvcUserPref->UsbWifiSetting.ConnectMode = WIFI_MODE_AP;
    pSvcUserPref->UsbWifiSetting.Channel = 0;
    pSvcUserPref->UsbWifiSetting.Use5G = 0;
    AmbaWrap_memset(pSvcUserPref->UsbWifiSetting.ApSSID, 0, sizeof(pSvcUserPref->UsbWifiSetting.ApSSID));
    AmbaWrap_memset(pSvcUserPref->UsbWifiSetting.ApPassword, 0, sizeof(pSvcUserPref->UsbWifiSetting.ApPassword));
    AmbaWrap_memset(pSvcUserPref->UsbWifiSetting.ApIP, 0, sizeof(pSvcUserPref->UsbWifiSetting.ApIP));
    AmbaWrap_memset(pSvcUserPref->UsbWifiSetting.StaSSID, 0, sizeof(pSvcUserPref->UsbWifiSetting.ApSSID));
    AmbaUtility_StringCopy(pSvcUserPref->UsbWifiSetting.StaSSID, 128, "Bert");
    AmbaWrap_memset(pSvcUserPref->UsbWifiSetting.StaPassword, 0, sizeof(pSvcUserPref->UsbWifiSetting.StaPassword));
    AmbaUtility_StringCopy(pSvcUserPref->UsbWifiSetting.StaPassword, 128, "10.18.96.1");
    pSvcUserPref->UsbWifiSetting.P2pWaitTime = 0;
}

void SvcUserPref_ResetCameraSetting(SVC_USER_PREF_s *pSvcUserPref)
{
    pSvcUserPref->InternalCamera.Enable = 1;
    pSvcUserPref->InternalCamera.AutoRecord = 0;
    pSvcUserPref->InternalCamera.ImageRotation = IMAGE_ROTATION_NONE;
    pSvcUserPref->InternalCamera.Fps = 30;
#if defined(CONFIG_BSP_H32_NEXAR_D081)
    pSvcUserPref->InternalCamera.MainStream.width = 1984;
    pSvcUserPref->InternalCamera.MainStream.height = 1984;
#else
    pSvcUserPref->InternalCamera.MainStream.width = 1920;
    pSvcUserPref->InternalCamera.MainStream.height = 1080;
#endif
    pSvcUserPref->InternalCamera.MainStream.bitrate = 14.0;
    pSvcUserPref->InternalCamera.MainStream.gop_size = 30;
    pSvcUserPref->InternalCamera.MainStream.is_h265 = 0;
#if defined(CONFIG_BSP_H32_NEXAR_D081)
    pSvcUserPref->InternalCamera.SecStream.width = 1280;
    pSvcUserPref->InternalCamera.SecStream.height = 1280;
#else
    pSvcUserPref->InternalCamera.SecStream.width = 1280;
    pSvcUserPref->InternalCamera.SecStream.height = 720;
#endif
    pSvcUserPref->InternalCamera.SecStream.bitrate = 4.0;
    pSvcUserPref->InternalCamera.SecStream.gop_size = 30;
    pSvcUserPref->InternalCamera.SecStream.is_h265 = 0;
    pSvcUserPref->InternalCamera.UvcStream.width = UVC_STREAM_WIDTH;
    pSvcUserPref->InternalCamera.UvcStream.height = UVC_STREAM_HEIGHT;

    pSvcUserPref->ExternalCamera.Enable = 1;
    pSvcUserPref->ExternalCamera.AutoRecord = 0;
    pSvcUserPref->ExternalCamera.ImageRotation = IMAGE_ROTATION_NONE;
    pSvcUserPref->ExternalCamera.Fps = 30;
    pSvcUserPref->ExternalCamera.MainStream.width = 1280;
    pSvcUserPref->ExternalCamera.MainStream.height = 720;
    pSvcUserPref->ExternalCamera.MainStream.bitrate = 10.0;
    pSvcUserPref->ExternalCamera.MainStream.gop_size = 30;
    pSvcUserPref->ExternalCamera.MainStream.is_h265 = 0;
    pSvcUserPref->ExternalCamera.SecStream.width = 1280;
    pSvcUserPref->ExternalCamera.SecStream.height = 720;
    pSvcUserPref->ExternalCamera.SecStream.bitrate = 4.0;
    pSvcUserPref->ExternalCamera.SecStream.gop_size = 30;
    pSvcUserPref->ExternalCamera.SecStream.is_h265 = 0;
    pSvcUserPref->ExternalCamera.UvcStream.width = UVC_STREAM_WIDTH;
    pSvcUserPref->ExternalCamera.UvcStream.height = UVC_STREAM_HEIGHT;

    pSvcUserPref->RtspLiveCamera = CAMERA_CHANNEL_INTERNAL;
}

void SvcUserPref_ResetApiSetting(SVC_USER_PREF_s *pSvcUserPref)
{
    pSvcUserPref->ApiSettings.files_enable = 0;
}

/**
 * User preference reseting function
 */
void SvcUserPref_Reset(void)
{
    if (InitDone == 1U) {
        SvcUserPref_ResetCameraSetting(&SvcUserPrefDefault);
        SvcUserPref_ResetNetworkSetting(&SvcUserPrefDefault);
        SvcUserPref_ResetApiSetting(&SvcUserPrefDefault);
        if (SVC_OK != AmbaWrap_memcpy(pSvcUserPref, &SvcUserPrefDefault, sizeof(SVC_USER_PREF_s))) {
            SvcLog_NG(SVC_LOG_USRPREF, "AmbaWrap_memcpy failed!!", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_USRPREF, "Need to do SvcUserPref_Init first!!", 0U, 0U);
    }
}

/**
 * User preference initialization
 * @return ErrorCode
 */
UINT32 SvcUserPref_Init(void)
{
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    ULONG BufAddr = 0U;
    UINT32 BufSize = 0U;

    if (SVC_OK != SvcPref_Init()) {
        SvcLog_NG("USRPREF", "SvcPref_Init failed!!", 0U, 0U);
    }
    SvcPref_GetPrefBuf(&BufAddr, &BufSize);
    AmbaMisra_TypeCast(&pSvcUserPref, &BufAddr);
    InitDone = 1U;
    SvcUserPref_Reset();

    return SVC_OK;
#else
    UINT32  Rval = SVC_OK, BufSize = 0U;
    ULONG   BufAddr = 0U;

    SvcPref_GetPrefBuf(&BufAddr, &BufSize);
    AmbaMisra_TypeCast(&pSvcUserPref, &BufAddr);

    Rval = SvcPref_Init();
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_USRPREF, "SvcPref_Init failed!!", 0U, 0U);
    } else {
        if (BufSize < sizeof(SVC_USER_PREF_s)) {
            SvcLog_NG(SVC_LOG_USRPREF, "!!! please increase pref buffer size, buffer(%u) < used(%u)", BufSize, (UINT32)sizeof(SVC_USER_PREF_s));
        }

        if (SVC_OK != SvcPref_Load(BufAddr, BufSize)) {
            SvcLog_NG(SVC_LOG_USRPREF, "SvcPref_Load failed!!", 0U, 0U);
        } else {
            if (0 != AmbaUtility_StringCompare(pSvcUserPref->ProjectName, SvcUserPrefDefault.ProjectName, AmbaUtility_StringLength(SvcUserPrefDefault.ProjectName))) {
                SvcLog_DBG(SVC_LOG_USRPREF, "The project name is mismatched. Get the default user setting", 0U, 0U);
                Rval = SVC_NG;
            }
            if (0 != AmbaUtility_StringCompare(pSvcUserPref->Version, SvcUserPrefDefault.Version, AmbaUtility_StringLength(SvcUserPrefDefault.Version))) {
                SvcLog_DBG(SVC_LOG_USRPREF, "The version number is mismatched! Get the default user setting!", 0U, 0U);
                Rval = SVC_NG;
            }
        }
    }

    if (SVC_OK != Rval) {
        if (SVC_OK != AmbaWrap_memcpy(pSvcUserPref, &SvcUserPrefDefault, sizeof(SVC_USER_PREF_s))) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_USRPREF, "AmbaWrap_memcpy failed!!", 0U, 0U);
        } else {
            if (SVC_OK != SvcPref_Save(BufAddr, BufSize)) {
                SvcLog_NG(SVC_LOG_USRPREF, "SvcPref_Save failed!!", 0U, 0U);
            } else {
                Rval = SVC_OK;
            }
        }
    }

    if (Rval == SVC_OK) {
        SVC_WRAP_PRINT "ProjectName : %s"
            SVC_PRN_ARG_S      SVC_LOG_USRPREF
            SVC_PRN_ARG_CSTR   pSvcUserPref->ProjectName    SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        SVC_WRAP_PRINT "Version : %s"
            SVC_PRN_ARG_S      SVC_LOG_USRPREF
            SVC_PRN_ARG_CSTR   pSvcUserPref->Version        SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        InitDone = 1U;
    }

    return Rval;
#endif
}

/**
 * User preference dumping to specified file
 * @param [in] FileName pointer file name
 * @return ErrorCode
 */
UINT32 SvcUserPref_Dump(const char *FileName)
{
    UINT32           Rval = 0U, NumSuccess;
    AMBA_VFS_FILE_s  OutputFile;

    if (InitDone == 1U) {
        if (SVC_OK != AmbaVFS_Open(FileName, "w", 0U, &OutputFile)) {
            SvcLog_NG(SVC_LOG_USRPREF, "AmbaVFS_Open failed", 0U, 0U);
            Rval = SVC_NG;
        } else {
            if (SVC_OK != AmbaVFS_Write(pSvcUserPref, 1U, (UINT32)sizeof(SVC_USER_PREF_s), &OutputFile, &NumSuccess)) {
                SvcLog_NG(SVC_LOG_USRPREF, "AmbaVFS_Write failed", 0U, 0U);
                Rval = SVC_NG;
            } else {
                if (SVC_OK != AmbaVFS_Close(&OutputFile)) {
                    SvcLog_NG(SVC_LOG_USRPREF, "AmbaVFS_Close failed", 0U, 0U);
                    Rval = SVC_NG;
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_USRPREF, "Need to do SvcUserPref_Init first!!", 0U, 0U);
        Rval = SVC_NG;
    }

    return Rval;
}
