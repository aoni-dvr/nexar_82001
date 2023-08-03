#ifndef AMBA_EIS_CTRL_ALGO_H
#define AMBA_EIS_CTRL_ALGO_H
#include "AmbaWrap.h"
#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaErrorCode.h"
#include "AmbaImg_ImuManager.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaSensor.h"
#include "AmbaShell.h"
//#include "AmbaEIS_Common.h"


#define EIS_Algo_OK             0U          /**< Success */
#define EIS_Algo_NG             EIS_ERR_BASE   /**< Error eis proc not enable */
#define EIS_Algo_NG_0001        (EIS_ERR_BASE | 0x00000001U)   /**< Error invalid input parameters */
#define EIS_Algo_NG_0002        (EIS_ERR_BASE | 0x00000002U)   /**< Error eis proc not enable */



#define EIS_X_AXIS      0
#define EIS_Y_AXIS      1
#define EIS_Z_AXIS      2


//-----Warp Table -----
#define EIS_WARP_VERSION 0x20180401U
#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define MAX_WARP_TBL_H_GRID_NUM (256UL)
#define MAX_WARP_TBL_V_GRID_NUM (192UL)
#else
#define MAX_WARP_TBL_H_GRID_NUM (82UL)
#define MAX_WARP_TBL_V_GRID_NUM (70UL)
#endif


#define EIS_WARP_GRID_SIZE (MAX_WARP_TBL_H_GRID_NUM * MAX_WARP_TBL_V_GRID_NUM)

#define DISTORTABLE_PARAMS_MAX  64       // Max parameter number per distortion table



typedef struct{
    UINT32 Width;
    UINT32 Height;
} AMBA_EISCTRL_WINDOW_GEOMETRY_s;

typedef struct {
    AMBA_IK_WINDOW_SIZE_INFO_s  WindowSizeInfo;
    AMBA_IK_VIN_ACTIVE_WINDOW_s VinActiveWin;
    AMBA_EISCTRL_WINDOW_GEOMETRY_s Cap;
    FLOAT  DownsampleX;
    FLOAT  DownsampleY;
    UINT8  DownsampleXSft;
    UINT8  DownsampleYSft;
} AMBA_EISCTRL_WINPARM_s;


typedef struct {
    AMBA_SENSOR_DEVICE_INFO_s SensorDeviceInfo;
    AMBA_SENSOR_STATUS_INFO_s SensorStatus;
    UINT32 FrameRatePerSec;
    UINT32 VerticalBlankTime;
    UINT32 ExposureTime;
} AMBA_EISCTRL_SENSOR_s;


typedef struct {
    UINT32 ExtImuDataCnt;
    AMBA_IMG_IMUMGR_FIFO_INFO_s *pExtImuInfoBuf;
} AMBA_EISCTRL_IMU_INFO_s;


typedef struct {
    UINT32  ChanID;
    UINT32  Frame;
} AMBA_EISCTRL_SYS_INFO_s;

typedef struct {
    UINT32 HorGridNum;          /* Horizontal grid number. Range to 256 as maximum. */
    UINT32 VerGridNum;          /* Vertical grid number. Range to 192 as maximum. */
    UINT32 TileWidthExp;        /* 3:8, 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    UINT32 TileHeightExp;       /* 3:8, 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
}AMBA_EISCTRL_WARP_INFO_s;


typedef struct {
    UINT32                          UpdateFlag;
    AMBA_IK_VIN_ACTIVE_WINDOW_s     ActWin;
    AMBA_IK_DUMMY_MARGIN_RANGE_s    DummyRange;
    AMBA_IK_DZOOM_INFO_s            DZoomInfo;
    AMBA_IK_WARP_INFO_s             WarpInfo;
} AMBA_EISCTRL_USER_DATA_s;


typedef struct {
    DOUBLE  X;
    DOUBLE  Y;
}AMBA_EISCTRL_RATIO_s;


/*  Eisinfo storage structure */
typedef struct {
    UINT32 ImgWidth;                     /* calibration window width  (sensor)*/
    UINT32 ImgHeight;                    /* calibration window height (sensor)*/
    DOUBLE RealWidth;                    /* Sensor real width (mm)*/
    INT32  CenterX;                      /* optical center on x-axis of calibration window */
    INT32  CenterY;                      /* optical center on y-axis of calibration window */
} AMBA_EISCTRL_CALIB_WIN_s;


typedef struct {
    UINT32 Enable;                          /* Data valid flag */
    AMBA_EISCTRL_CALIB_WIN_s SensorWin;      /* Sensor window info */
    UINT32 DistorParamNum;                  /* Parameter number of distortion table */
    DOUBLE Real[DISTORTABLE_PARAMS_MAX];    /* Real distortion table */
    DOUBLE Expect[DISTORTABLE_PARAMS_MAX];  /* Expect distortion table */
    UINT32 Reserved;
} AMBA_EISCTRL_CALIB_LDC_s;


typedef struct
{
    UINT32 TransRange;
    UINT32 RotateAngle;
} AMBA_EISCTRL_COMP_s;


#if 0

typedef struct {

    UINT32 CurrFrameTime;
    UINT32 FrameReadOutTime;
    UINT32 SensorCellSizeX;
    UINT32 SensorCellSizeY;

#define EIS_X_AXIS      0
#define EIS_Y_AXIS      1
#define EIS_Z_AXIS      2
    UINT64 EisScaleFactorNum;
    UINT64 EisScaleFactorDenX;
    UINT64 EisScaleFactorDenY;                      //64 has problems
    UINT64 EisScaleFactorDenZ;

    //UINT32 EisScaleFactorNum;
    //UINT32 EisScaleFactorDenX;
    //UINT32 EisScaleFactorDenY;
    //UINT32 EisScaleFactorDenZ;

} AMBA_EISCTRL_PARAM_s;
typedef struct {
    UINT32 FactorNum;              /* subsamping factor (numerator) */
    UINT32 FactorDen;              /* subsamping factor (denominator) */
} AMBA_EISCTRL_SENSOR_SUBSAMPLING_s;

typedef struct {
    UINT32 StartX;     // Unit in pixel. Before downsample.
    UINT32 StartY;     // Unit in pixel. Before downsample.
    UINT32 Width;      // Unit in pixel. After downsample.
    UINT32 Height;     // Unit in pixel. After downsample.
    AMBA_EISCTRL_SENSOR_SUBSAMPLING_s HSubSample;
    AMBA_EISCTRL_SENSOR_SUBSAMPLING_s VSubSample;
} AMBA_EISCTRL_VIN_SENSOR_GEOMETRY_s;

typedef struct {
    UINT8   GyroID;             /* gyro sensor id*/
    INT8    GyroXPolar;         /* gyro sensor x polarity*/
    INT8    GyroYPolar;         /* gyro sensor y polarity*/
    INT8    GyroZPolar;         /* gyro sensor z polarity*/
    UINT8   VolDivNum;          /* numerator of voltage divider*/
    UINT8   VolDivDen;          /* denominator of voltage divider*/
    UINT8   MaxRmsNoise;        /* gyro sensor rms noise level*/
    UINT16  MaxBias;            /* max gyro sensor bias*/
    UINT16  MinBias;            /* min gyro sensor bias*/
    UINT16  MaxSense;           /* max gyro sensor sensitivity*/
    UINT16  MinSense;           /* min gyro sensor sensitivity*/
    INT8    PhsDly;             /* gyro sensor phase delay, unit in ms*/
} AMBA_EISCTRL_GYRO_s;


typedef struct {
    UINT8 Enable;
    UINT8 McEnable;
    UINT8 RscEnable;
    UINT8 DbgPortEnable;
    UINT8 MeEnable;
    UINT8 StabilizerType;
    UINT8 Proc;
    UINT8 EnableCmd;
    UINT8 DisableCmd;
    UINT8 AdcSamplingRate; // Bit[5:0]=sampling rate in ms, Bit[7:6]=additional scale factor(2's exponential)
    UINT8 EnablePref;
    UINT8 RscEnablePref;
} AMBA_EISCTRL_MAIN_EIS_s;

#endif



typedef struct{
    UINT32                          Enable;
    UINT32                          CropWidth;
    UINT32                          CropHeight;
    INT16                           ShiftX;
    INT16                           ShiftY;
}AMBA_EISCTRL_POSTFACT_s;



typedef struct{
    char                            ModeName[128];
    UINT32                          FocalLength;
    UINT32                          Reserve1;
    UINT32                          Reserve2;
    UINT32                          Reserve3;
    UINT32                          Reserve4;
    UINT32                          Reserve5;
    AMBA_EISCTRL_WARP_INFO_s        WarpInfo;
    AMBA_EISCTRL_CALIB_LDC_s        LDCInfo;
    AMBA_EISCTRL_COMP_s             CompRange;
    AMBA_EISCTRL_POSTFACT_s         PostFact;
}AMBA_EISCtrl_CFG;

UINT32 AmbaEisCtrl_Enable(UINT32 Enable, UINT32 Mode);
UINT32 AmbaEisCtrl_SetDefCfg(const AMBA_EISCtrl_CFG *DefCfg);
UINT32 AmbaEISCtrl_SetPostProcFact(const AMBA_EISCTRL_POSTFACT_s *pPostFact);

UINT32 AmbaEisCtrl_Init(const AMBA_EISCTRL_WINPARM_s *pWindow, const AMBA_EISCTRL_SENSOR_s *pSensor, const AMBA_IMU_INFO_s *pGyroInfo);
UINT32 AmbaEisCtrl_Proc(const AMBA_EISCTRL_WINPARM_s *pWindow, const AMBA_EISCTRL_SENSOR_s *pSensorInfo,const AMBA_EISCTRL_IMU_INFO_s ImuInfo, AMBA_EISCTRL_SYS_INFO_s SystemInfo, AMBA_EISCTRL_USER_DATA_s *pEisResInfo);
UINT32 AmbaEISCtrl_PostProc(const AMBA_EISCTRL_USER_DATA_s *pPreEisResInfo, AMBA_EISCTRL_USER_DATA_s *pPostEisResInfo);


#ifdef AmbaEIS_Sim
void AmbaEIS_TestCmd(UINT32 ArgCount, char * const * pArgVector);
#else
void AmbaEIS_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#endif
#endif

