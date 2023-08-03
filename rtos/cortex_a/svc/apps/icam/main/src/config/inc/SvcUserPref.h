/**
*  @file SvcUserPref.h
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

#ifndef SVC_USER_PREF_H
#define SVC_USER_PREF_H

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
typedef enum _option_onoff_e_ {
    OPTION_OFF = 0,
    OPTION_ON = 1
} option_onoff_e;

typedef struct _stream_size_s_ {
    int width;
    int height;
    double bitrate;
    int gop_size;
    unsigned char is_h265;
} stream_size_s;

typedef struct _camera_setting_s_ {
    unsigned char Enable;
    unsigned char AutoRecord;
    unsigned char ImageRotation;
    unsigned char Fps;
    stream_size_s MainStream;
    stream_size_s SecStream;
    stream_size_s UvcStream;
} camera_setting_s;

typedef struct _wifi_setting_s_ {
    unsigned char BootMode;
    unsigned char ConnectMode;
    int Channel;
    unsigned char Use5G;
    char ApSSID[32];
    char ApPassword[32];
    char ApIP[32];
    char StaSSID[32];
    char StaPassword[32];
    unsigned int P2pWaitTime;
} wifi_setting_s;

typedef struct _gnss_setting_s_ {
    unsigned char UartChannel;
    unsigned int UartBitRate;
    unsigned char Id;
    unsigned int DataRate;
} gnss_setting_s;

typedef struct _imu_setting_s_ {
    unsigned char AccelOdr;
    unsigned char AccelRange;
    unsigned char GyroOdr;
    unsigned char GyroRange;
    unsigned char AxisPolarity;
} imu_setting_s;

typedef struct _video_encrytion_setting_s_ {
    char encryption_key[64];
    char decryption_key[64];
    int enable;
    int debug;
} video_encrytion_setting_s;

typedef struct _color_s_ {
    unsigned char R;
    unsigned char G;
    unsigned char B;
} color_s;

typedef struct _api_setting_s_ {
    int files_enable;
} api_setting_s;

typedef enum _image_rotation_e_ {
    IMAGE_ROTATION_NONE = 0,
    IMAGE_ROTATION_HORIZONTAL = 1,
    IMAGE_ROTATION_VERTICAL = 2,
    IMAGE_ROTATION_HORIZONTAL_VERTICAL = 3
} image_rotation_e;

typedef enum _water_mark_item_e_ {
    WATER_MARK_NONE = 0,
    WATER_MARK_DATETIME = 1,
    WATER_MARK_SPEED = 2,
    WATER_MARK_LOGO = 4
} water_mark_item_e;

typedef enum _water_mark_pos_e_ {
    WATER_MARK_POS_TOP = 0,
    WATER_MARK_POS_BOTTOM = 1
} water_mark_pos_e;

typedef enum _wifi_mode_e_ {
    WIFI_MODE_AP = 0,
    WIFI_MODE_STA,
    WIFI_MODE_P2P
} wifi_mode_e;

typedef enum _gnss_type_e_ {
    GNSS_TYPE_GPS = 1,
    GNSS_TYPE_SBAS = 2,
    GNSS_TYPE_GALILEO = 4,
    GNSS_TYPE_BEIDOU = 8,
    GNSS_TYPE_IMES = 16,
    GNSS_TYPE_QZSS = 32,
    GNSS_TYPE_QLONASS = 64,
} gnss_type_e;

typedef enum _speed_unit_e_ {
    SPEED_UNIT_KMH = 0,
    SPEED_UNIT_MPH = 1,
} speed_unit_e;
#endif

typedef struct {
    #define ICAM_PREF_VER_LEN       (32U)
    #define ICAM_PRJ_NAME_LEN       (128U)
    char            Version[ICAM_PREF_VER_LEN];
    char            ProjectName[ICAM_PRJ_NAME_LEN];

    UINT32          OperationMode;
    UINT32          FormatId;
    UINT8           ShowDspLog;
    UINT8           ShowSspLog;
    UINT8           ShowSspApiLog;
    UINT8           ShowSensorLog;
    UINT8           ShowBridgeLog;
    UINT8           ShowYuvLog;
    UINT8           ShowIKLog;
    UINT8           ShowSwpllLog;
    UINT8           ShowThmViewLog;
    UINT8           EnableBSD;
    UINT8           EnableVidThm;
    UINT8           UsbClass;
    UINT32          DspDbgThd;
    UINT32          DspDbgLvl;
    UINT32          CVBooFlag;
    UINT8           CVSchdrLog;
    UINT32          MaxBitrate;     /* Unit: Mbps*/
    UINT32          FcwsMode;
    UINT32          FcV2_SR_mode;
    /* DCF */
    UINT8           DcfFormatId;
    /* audio */
    UINT32          AudioAacBitRate;
    UINT32          AudioVolume;
    UINT32          BitsPerSample;
    UINT32          EnableDMIC;

    /* record */
    UINT32          FileSplitTimeMin;
    UINT32          FileSplitTimeSeconds;

    #define SVC_MAX_NUM_PANEL       (3U)
    UINT32          LcdBlOnDytyId[SVC_MAX_NUM_PANEL];
    /* emr */
    UINT32          Emr_1_0;
    UINT32          Emr_2_0;
    struct {
        INT32 ShiftX;
        INT32 ShiftY;
        DOUBLE ZoomX;
        DOUBLE ZoomY;
        DOUBLE Theta;
    } EmrInfo[AMBA_DSP_MAX_VIEWZONE_NUM];

    /* mctf compression */
    UINT32  MctfCmprCtrl;
    /* mcts dramout control */
    UINT32  MctsDOutCtrl;

    /* main storage */
    char    MainStgDrive[2];    /* drive (one char) + '\0' */
    UINT32  MainStgChan;

    /* ADAS usage */
    DOUBLE  CalibFocalLength;
    DOUBLE  CalibCellSize;

    /* Quick calib for ADAS */
    UINT32  SkylineHeight;
    UINT32  WarningZoon_AutoRun;

    /* LDWS ActiveSpeed */
    UINT32  LdwsActiveSpeed;

    /* CAN  Fixed Speed */
    UINT32  CanSpeed;

    /* Video Stream Capture */
    UINT32  VidStrmCapOn;

    /* BootToRec wait storage done timeout */
    UINT32  BootToRecTimeOut;

    /* BSD process channel */
    UINT32  PrefBsdChan;
#if defined(CONFIG_ICAM_PROJECT_SHMOO)
    struct{
        UINT8 ShmooTask;
        UINT8 ShmooMode;
        UINT8 FileWrite;
        UINT8 RecNum;
        UINT8 RecCmpBits[4U];
        UINT8 CvNum;
    struct {
        UINT32 ShmooDuration;
        UINT32 ShmooDevice;
        UINT32 ShmooCoreMask;
        UINT32 ShmooPriority;
        UINT32 ShmooPartitionID;
    }ShmooMisc;
    struct{
        UINT8 pad_dds;
        UINT8 pad_pdds;
        UINT8 pad_ca_dds;
        UINT8 pad_ca_pdds;
        UINT8 pad_term;
        UINT8 pad_vref;
        UINT8 rddly;
        UINT8 wrdly;
        UINT8 dll0;
        UINT8 dll1;
        UINT8 dll2;
        UINT8 ddr4_dds;
        UINT8 ddr4_vref;
        UINT8 ddr4_odt;
        UINT8 lpddr4_pdds;
        UINT8 lpddr4_ca_vref;
        UINT8 lpddr4_dq_vref;
        UINT8 lpddr4_ca_odt;
        UINT8 lpddr4_dq_odt;
    } ShmooItems;
    } ShmooInfo;
#endif

#if defined(CONFIG_ICAM_CVBS_EXIST)
    /* CVBS Out Gain */
    UINT32 CvbsOutGain;
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    /* customer setting start */
    option_onoff_e              LinuxBoot;
    option_onoff_e              LinuxConsole;
    //option_onoff_e              RtosConsole;
    option_onoff_e              LedOnOff;
    UINT8                       LedBrightness;
    option_onoff_e              WatchDogOnOff;
    option_onoff_e              RecordSoundOnOff;
    option_onoff_e              MotionDetect;
    camera_setting_s            InternalCamera;
    camera_setting_s            ExternalCamera;
    UINT8                       IrBrightness;
    UINT8                       EventBufferLength;
    UINT32                      SettingVersion;
    UINT8                       AwbMode;
    INT16                       AeBias;
    option_onoff_e              PowerOnOffSound;
    UINT8                       WaterMark;
    UINT8                       WaterMarkPos;
    UINT8                       BatteryPowerTime;
    UINT32                      NoActiveShutdownTime;
    UINT8                       Use24HMode;
    UINT8                       DebugMode;
    UINT32                      IrThresHold;
    UINT32                      IrSampling;
    int                         TimeZone;
    UINT8                       RtspLiveCamera;
    UINT8                       ImuRecord;
    UINT8                       GnssRecord;
    wifi_setting_s              WifiSetting;
    wifi_setting_s              UsbWifiSetting;
    gnss_setting_s              GnssSetting;
    imu_setting_s               ImuSetting;
    option_onoff_e              UsbMassStorage;
    option_onoff_e              LedRtosControl;
    video_encrytion_setting_s   VideoEncryption;
    char                        AuthorizationToken[64];
    UINT8                       ParkingLevel;
    UINT8                       PowerOffWakeupMinutes;
    UINT8                       EnableLteRemoteWakeup;
    speed_unit_e                SpeedUnit;
    option_onoff_e              CvOnOff;
    option_onoff_e              PivOnOff;
    color_s                     LedColor;
    api_setting_s               ApiSettings;
#endif
} SVC_USER_PREF_s;

UINT32 SvcUserPref_Init(void);
void   SvcUserPref_Reset(void);
UINT32 SvcUserPref_Dump(const char *FileName);
UINT32 SvcUserPref_Get(SVC_USER_PREF_s** ppSvcUserPref) __attribute__((weak));
void SvcUserPref_ResetCameraSetting(SVC_USER_PREF_s *pSvcUserPref);
void SvcUserPref_ResetNetworkSetting(SVC_USER_PREF_s *pSvcUserPref);
void SvcUserPref_ResetApiSetting(SVC_USER_PREF_s *pSvcUserPref);

#endif /* SVC_USER_PREF_H */
