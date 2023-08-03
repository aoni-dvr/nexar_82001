

#include "AmbaEIS_Common.h"

#include "AmbaWU_WarpUtility.h"


//#define UTIL_MAX_INT_STR_LEN        22U

#define EIS_WARP_VERSION            0x20180401U


#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32) || defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define MAX_WARP_HGRID_NUM (256UL) /* 7680 / 64 = 120 */
#define MAX_WARP_VGRID_NUM (192UL) /* 4320 / 64 = 67 */
#else
#define MAX_WARP_HGRID_NUM (82UL)
#define MAX_WARP_VGRID_NUM (70UL)
#endif

#define MAX_WARP_MAP_SIZE           (MAX_WARP_HGRID_NUM * MAX_WARP_VGRID_NUM)
#define MaxAutoTestStep             30
#define PI    3.1415926535897932384626433832795

typedef struct{
    AMBA_EISCTRL_SYS_INFO_s          SystemInfo;
    AMBA_EISCTRL_SENSOR_s            Sensor;
    AMBA_EISCTRL_WINPARM_s           Window;
    AMBA_EISCTRL_IMU_INFO_s          ImuInfo;
}AMBA_EISCommon_InputCfg_s;


typedef struct {
    UINT32  Enable;
    UINT32  RotEnable;
    UINT32  ZSEnalbe;
    UINT32  CropEnable;
    UINT32  CropRatio;
    UINT32  Mode;
    INT8   Dir;
    INT32  TestStep;
    INT32  MaxStep;
    DOUBLE StepZ;
    DOUBLE RotateAngle;
    DOUBLE MaxAngle;
}AMBA_EISCommon_Auto_s;

typedef struct {
    INT32 MovX;
    INT32 MovY;
}AMBA_EISCommon_Move_s;


typedef struct {
    UINT8 InWin;
    UINT8 InSen;
    UINT8 InImu;
    UINT8 GVal;
    UINT8 RotInfo;
    UINT8 UserRes;
}AMBA_EISCommon_Show_s;



typedef struct {
    UINT16 Angle;
    UINT32 Width;
    UINT32 Height;
}AMBA_EISCommon_RotateWindowAngle_s;

typedef struct{
    char                             ModeName[128];
    AMBA_EISCTRL_RATIO_s             ZoomRatio; /* vin/actual ratio */
    AMBA_EISCTRL_RATIO_s             DummyRatio;/* dummy/actual ratio */
    AMBA_EISCTRL_WARP_INFO_s         WarpInfo;
    AMBA_EISCTRL_POSTFACT_s          PostFact;
}AMBA_EISCommon_PrefCfg;




static AMBA_EISCommon_Move_s TestLimP   = {0}; /* manual positive movement*/
static AMBA_EISCommon_Move_s TestLimN   = {0}; /* manual negative movement*/
static AMBA_EISCommon_Move_s DummyMaxP  = {0}; /* maxdummy positive movement*/
static AMBA_EISCommon_Move_s DummyMaxN  = {0}; /* maxdummy negative movement*/
static AMBA_EISCommon_Move_s EisMove    = {0}; /* current movement*/

static UINT32 manualSetWarp = 0U;

static UINT32 EISPostProcInit = 0U;
static AMBA_EISCTRL_POSTFACT_s CropInfo = {.Enable = 0U, .CropWidth  = 200U, .CropHeight = 200U, .ShiftX = 0, .ShiftY = 0};


static AMBA_EISCommon_Auto_s AutoTest = {
    .Enable     = 0U,
    .RotEnable  = 0U,
    .ZSEnalbe   = 0U,
    .Mode       = 0U,
    .Dir        = 1,
    .TestStep   = 0,
    .MaxStep    = MaxAutoTestStep,
    .StepZ      = 0.0f,
    .RotateAngle= 0.0f,
    .MaxAngle   = 5.0f,
};


static AMBA_EISCommon_Show_s   ShowCmd = {
    .InWin = 0U,
    .InSen = 0U,
    .InImu = 0U,
    .GVal     = 0U,
    .RotInfo  = 0U,
    .UserRes  = 0U,
};



static AMBA_EISCTRL_WINDOW_GEOMETRY_s   gActualWin;
static AMBA_EISCTRL_WINDOW_GEOMETRY_s   gDummyWin;
//static AMBA_EISCTRL_USER_DATA_s         gUserData;



//static AMBA_IMU_INFO_s GyroInfo;




static AMBA_IK_DZOOM_INFO_s gImgDzoom = {
    .Enable = 1U,
    .ZoomX = 65536U,
    .ZoomY = 65536U,
    .ShiftX = 0,
    .ShiftY = 0
};


static AMBA_IK_DUMMY_MARGIN_RANGE_s gImgDummy = {
    .Enable = 1U,
    .Left   = 0U,
    .Right  = 0U,
    .Top    = 0U,
    .Bottom = 0U
};



static const AMBA_EISCommon_PrefCfg gPrefCfg[] = {
#if 0
{
    .ModeName = "(MODE 0) ActWin(3499x1968)",
    .ZoomRatio  = {.X = 1.140, .Y = 1.520},
    .DummyRatio = {.X = 1.057, .Y = 1.098},
    .WarpInfo   = {.HorGridNum = MAX_WARP_HGRID_NUM, .VerGridNum = MAX_WARP_VGRID_NUM, .TileWidthExp = 7UL, .TileHeightExp = 7UL}, /* HorGridNum: 2^7 = 128; VerGridNum: 2^7 = 128;*/
    .PostFact   ={.Enable = 1U, .CropWidth  = 200U, .CropHeight = 200U, .ShiftX = 0, .ShiftY = 0},
},
#endif

{
    .ModeName = "(MODE 0) ActWin()",
    .ZoomRatio  = {.X = 1.420, .Y = 1.890},
    .DummyRatio = {.X = 1.420, .Y = 1.890},
    .WarpInfo   = {.HorGridNum = MAX_WARP_HGRID_NUM, .VerGridNum = MAX_WARP_VGRID_NUM, .TileWidthExp = 7UL, .TileHeightExp = 7UL}, /* HorGridNum: 2^7 = 128; VerGridNum: 2^7 = 128;*/
    .PostFact   = {.Enable = 0U, .CropWidth  = 200U, .CropHeight = 200U, .ShiftX = 0, .ShiftY = 0},
},
{
    .ModeName = "(MODE 1) ActWin(2876x1587)",
    .ZoomRatio  = {.X = 1.420, .Y = 1.890},
    .DummyRatio = {.X = 1.200, .Y = 1.360},
    .WarpInfo   = {.HorGridNum = MAX_WARP_HGRID_NUM, .VerGridNum = MAX_WARP_VGRID_NUM, .TileWidthExp = 7UL, .TileHeightExp = 7UL}, /* HorGridNum: 2^7 = 128; VerGridNum: 2^7 = 128;*/
    .PostFact   ={.Enable = 0U, .CropWidth  = 0U, .CropHeight = 0U, .ShiftX = 0, .ShiftY = 0},
},

{
    .ModeName = "(MODE 2) ActWin(2402x1351)",
    .ZoomRatio  = {.X = 1.665, .Y = 2.220},
    .DummyRatio = {.X = 1.213, .Y = 1.598},
    .WarpInfo   = {.HorGridNum = MAX_WARP_HGRID_NUM, .VerGridNum = MAX_WARP_VGRID_NUM, .TileWidthExp = 7UL, .TileHeightExp = 7UL}, /* HorGridNum: 2^7 = 128; VerGridNum: 2^7 = 128;*/
    .PostFact   ={.Enable = 0U, .CropWidth  = 0U, .CropHeight = 0U, .ShiftX = 0, .ShiftY = 0},
},
{
    .ModeName = "(MODE 3) ActWin(3165x1780) Dummy full",
    .ZoomRatio  = {.X = 1.213, .Y = 1.213},
    .DummyRatio = {.X = 1.213, .Y = 1.213},
    .WarpInfo   = {.HorGridNum = MAX_WARP_HGRID_NUM, .VerGridNum = MAX_WARP_VGRID_NUM, .TileWidthExp = 6UL, .TileHeightExp = 6UL}, /* HorGridNum: 2^6 = 64; VerGridNum: 2^6 = 64;*/
    .PostFact   ={.Enable = 0U, .CropWidth  = 0U, .CropHeight = 0U, .ShiftX = 0, .ShiftY = 0},
},

{
    .ModeName = "(MODE 4) ActWin(3165x1780)",
    .ZoomRatio  = {.X = 1.213, .Y = 1.213},
    .DummyRatio = {.X = 1.145, .Y = 1.196},
    .WarpInfo   = {.HorGridNum = MAX_WARP_HGRID_NUM, .VerGridNum = MAX_WARP_VGRID_NUM, .TileWidthExp = 6UL, .TileHeightExp = 6UL}, /* HorGridNum: 2^6 = 64; VerGridNum: 2^6 = 64;*/
    .PostFact   ={.Enable = 0U, .CropWidth  = 0U, .CropHeight = 0U, .ShiftX = 0, .ShiftY = 0},
},

};

static AMBA_EISCommon_InputCfg_s gInputCfg;

#ifdef AmbaEIS_Sim
static AMBA_IK_GRID_POINT_s gWarpTb[MAX_WARP_MAP_SIZE];
static AMBA_IK_GRID_POINT_s gWarpTb1[MAX_WARP_MAP_SIZE];
#else
static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT gWarpTb[MAX_WARP_MAP_SIZE];
static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT gWarpTb1[MAX_WARP_MAP_SIZE];
#endif

static UINT32 MODE = 1U;


static void AmbaEISCommon_DummyAfterRotate(UINT16 Angle, AMBA_EISCTRL_WINDOW_GEOMETRY_s In, AMBA_EISCTRL_WINDOW_GEOMETRY_s* Out){

    DOUBLE SinValue, CosValue;
    DOUBLE DoubleInWidth;
    DOUBLE DoubleInHeight;
    DOUBLE DoubleAngle;
    DOUBLE DoubleTemp;

    DoubleInWidth   = (DOUBLE)In.Width;
    DoubleInHeight  = (DOUBLE)In.Height;
    DoubleAngle     = (DOUBLE)Angle*PI/180.0;

    if(0U != AmbaWrap_cos(DoubleAngle, &CosValue)){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_cos Error!!",0U, 0U, 0U, 0U, 0U);
    }
    if(0U != AmbaWrap_sin(DoubleAngle, &SinValue)){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_sin Error!!",0U, 0U, 0U, 0U, 0U);
    }

    DoubleTemp = (DoubleInWidth * CosValue) + (DoubleInHeight * SinValue);
    Out->Width  = (UINT32)DoubleTemp;
    DoubleTemp = (DoubleInWidth * SinValue) + (DoubleInHeight * CosValue);
    Out->Height = (UINT32)DoubleTemp;

}



static void AmbaEISCommon_DummyScan(UINT16 MaxAngle,AMBA_EISCTRL_WINDOW_GEOMETRY_s Actual, AMBA_EISCTRL_WINDOW_GEOMETRY_s MaxDummy, AMBA_EISCommon_RotateWindowAngle_s *ScanWindow ){

    AMBA_EISCTRL_WINDOW_GEOMETRY_s RotActualWin;
    AMBA_EISCTRL_WINDOW_GEOMETRY_s NextRotActualWin;

    for(UINT16 i = 0U; i < MaxAngle; i++ ){

        AmbaEISCommon_DummyAfterRotate(i, Actual, &RotActualWin);
        AmbaEISCommon_DummyAfterRotate((i + 1U), Actual, &NextRotActualWin);

        if((NextRotActualWin.Width > MaxDummy.Width)||(NextRotActualWin.Height > MaxDummy.Height)){
            break;
        }
        else{
            ScanWindow->Width = RotActualWin.Width;
            ScanWindow->Height = RotActualWin.Height;
            ScanWindow->Angle = i;
        }
    }


}


static void AmbaEISCommon_AutoTestXY(void){

    if(manualSetWarp == 0U) {
    switch (AutoTest.Mode) {
        case 0:

            EisMove.MovY = 0;
            if (AutoTest.TestStep >= 0) {
                if (TestLimP.MovX != 0){
                    EisMove.MovX = AutoTest.TestStep * TestLimP.MovX / AutoTest.MaxStep;
                }
                else{
                    EisMove.MovX = AutoTest.TestStep * DummyMaxP.MovX / AutoTest.MaxStep;
                }

            }
            else {
                if (TestLimN.MovX != 0){
                    EisMove.MovX = AutoTest.TestStep * TestLimN.MovX / AutoTest.MaxStep;
                }
                else{
                    EisMove.MovX = AutoTest.TestStep * DummyMaxN.MovX / AutoTest.MaxStep;
                }

            }
        break;

        case 1:
            EisMove.MovX = 0;

            if (AutoTest.TestStep >= 0) {
                if (TestLimP.MovY != 0){
                    EisMove.MovY = AutoTest.TestStep * TestLimP.MovY / AutoTest.MaxStep;
                }
                else{
                    EisMove.MovY = AutoTest.TestStep * DummyMaxP.MovY / AutoTest.MaxStep;
                }

            }
            else {
                if (TestLimN.MovY != 0){
                    EisMove.MovY = AutoTest.TestStep * TestLimN.MovY / AutoTest.MaxStep;
                }
                else{
                    EisMove.MovY = AutoTest.TestStep * DummyMaxN.MovY / AutoTest.MaxStep;
                }

            }
        break;

        case 2:
            EisMove.MovX = 0;
            EisMove.MovY = 0;

        break;

        case 3:
            if (AutoTest.TestStep >= 0){
                EisMove.MovX = DummyMaxP.MovX;
            }
            else{
                EisMove.MovX = -DummyMaxN.MovX ;
            }
        break;

        case 4:
            if (AutoTest.TestStep >= 0){
                EisMove.MovY = DummyMaxP.MovY;
            }
            else{
                EisMove.MovY = -DummyMaxN.MovY ;
            }
        break;


        default:
            AmbaEis_Log_DBG(LOG_EIS_COM,"AutoXY default",0U, 0U, 0U, 0U, 0U);

        break;


    }

    }

}

static void AmbaEISCommon_AutoTestZ(void)
{

    DOUBLE misra_d1, misra_d3;

    if ((manualSetWarp == 0U) && (AutoTest.Enable != 0U)) {

        switch (AutoTest.Mode) {
            case 0:
                AutoTest.RotateAngle = 0.0;
            break;

            case 1:
                AutoTest.RotateAngle = 0.0;
            break;

            case 2:
                misra_d1 = (DOUBLE)AutoTest.TestStep;
                misra_d3 = (DOUBLE)AutoTest.MaxStep;
                AutoTest.RotateAngle = misra_d1 * AutoTest.MaxAngle * PI / 180.0 / misra_d3 ;
                if(ShowCmd.UserRes > 0U) {
                    AmbaEis_Log_DB_DBG(LOG_EIS_COM, " [RotateAngle, MaxAngle] = [%s, %s]", AutoTest.RotateAngle, AutoTest.MaxAngle, 6);
                }
            break;

            case 3:
                AutoTest.RotateAngle = 0.0;
            break;

            case 4:
                AutoTest.RotateAngle = 0.0;
            break;

            default:
                AmbaEis_Log_DBG(LOG_EIS_COM,"AutoZ default",0U, 0U, 0U, 0U, 0U);
            break;


        }
    }
}

static void AmbaEISCommon_AutoTestControl(void)
{

    if (AutoTest.Enable != 0U) {
        INT32 MisraTempA = (INT32)AutoTest.MaxStep;
        if (AutoTest.Dir == 1) {
            AutoTest.TestStep ++;
            if (AutoTest.TestStep == MisraTempA){
                AutoTest.Dir = -1;
            }

        }
        else {
            AutoTest.TestStep --;
            if (AutoTest.TestStep == -MisraTempA){
                AutoTest.Dir = 1;
            }
        }
    }
}



static void AmbaEISCommon_SetUserData(AMBA_EISCTRL_USER_DATA_s *pEisResInfo){



    UINT32 TileWExp = gPrefCfg[MODE].WarpInfo.TileWidthExp;
    UINT32 TileHExp = gPrefCfg[MODE].WarpInfo.TileHeightExp;

    /* Active win */
    pEisResInfo->ActWin.Enable              = gInputCfg.Window.VinActiveWin.Enable;
    pEisResInfo->ActWin.ActiveGeo.StartX    = gInputCfg.Window.VinActiveWin.ActiveGeo.StartX;
    pEisResInfo->ActWin.ActiveGeo.StartY    = gInputCfg.Window.VinActiveWin.ActiveGeo.StartX;
    pEisResInfo->ActWin.ActiveGeo.Width     = gInputCfg.Window.VinActiveWin.ActiveGeo.Width;
    pEisResInfo->ActWin.ActiveGeo.Height    = gInputCfg.Window.VinActiveWin.ActiveGeo.Height;

    /* Dummy win */
    pEisResInfo->DummyRange.Enable  = gImgDummy.Enable;
    pEisResInfo->DummyRange.Right   = gImgDummy.Right;
    pEisResInfo->DummyRange.Left    = gImgDummy.Left;
    pEisResInfo->DummyRange.Top     = gImgDummy.Top;
    pEisResInfo->DummyRange.Bottom  = gImgDummy.Bottom;

    /* Dzoom win */
    pEisResInfo->DZoomInfo.Enable   = gImgDzoom.Enable;
    pEisResInfo->DZoomInfo.ShiftX   = gImgDzoom.ShiftX;
    pEisResInfo->DZoomInfo.ShiftY   = gImgDzoom.ShiftY;
    pEisResInfo->DZoomInfo.ZoomX    = gImgDzoom.ZoomX;
    pEisResInfo->DZoomInfo.ZoomY    = gImgDzoom.ZoomY;

   /* Warp */

   
    if(0U != AmbaWrap_memcpy(&(pEisResInfo->WarpInfo.VinSensorGeo), &(gInputCfg.Window.WindowSizeInfo.VinSensor), sizeof(AMBA_IK_VIN_SENSOR_GEOMETRY_s))){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy VinSensor Error!!",0U, 0U, 0U, 0U, 0U);
    }


    pEisResInfo->WarpInfo.Version       = EIS_WARP_VERSION;
    pEisResInfo->WarpInfo.HorGridNum    = ((gInputCfg.Window.WindowSizeInfo.VinSensor.Width + ((1UL << TileWExp) - 1U)) >> TileWExp) + 1UL;
    pEisResInfo->WarpInfo.VerGridNum    = ((gInputCfg.Window.WindowSizeInfo.VinSensor.Height + ((1UL << TileHExp) - 1U)) >> TileHExp) + 1UL;
    pEisResInfo->WarpInfo.TileWidthExp  = TileWExp;
    pEisResInfo->WarpInfo.TileHeightExp = TileHExp;
    pEisResInfo->WarpInfo.Enb_2StageCompensation = 1U;
    pEisResInfo->WarpInfo.pWarp         = &gWarpTb[0];


    if(EISPostProcInit != 1U){
        if(0U != AmbaEISCtrl_SetPostProcFact(&gPrefCfg[MODE].PostFact)){
            AmbaEis_Log_DBG(LOG_EIS_COM,"AmbaEISCtrl_SetPostProcFact Error!!",0U, 0U, 0U, 0U, 0U);
        }
        EISPostProcInit = 1U;
    }
}
static void AmbaEISCommon_CalcDummy(void)
{

    UINT32 VinWidth;
    UINT32 VinHeight;

    INT32 DummyOffsetX;
    INT32 DummyOffsetY;

    INT32 VarL;

    DOUBLE MisraD;
    UINT32 MisraTemp;




    /* Vin win size */
    VinWidth = gInputCfg.Window.WindowSizeInfo.VinSensor.Width;
    VinHeight = gInputCfg.Window.WindowSizeInfo.VinSensor.Height;

    /* Main win size*/
    //MainWidth = DefEisWin.WindowSizeInfo.Main.Width;
    //MainHeight = DefEisWin.WindowSizeInfo.Main.Height;


    /* Actual win size */
    MisraD = gPrefCfg[MODE].ZoomRatio.X * 1000.0;
    MisraTemp = (UINT32) MisraD;
    gActualWin.Width = (VinWidth * 1000UL) / MisraTemp;

    MisraD = gPrefCfg[MODE].ZoomRatio.Y * 1000.0;
    MisraTemp = (UINT32) MisraD;
    gActualWin.Height = (VinHeight * 1000UL) / MisraTemp;


    /* dzoom  win size */
//    DZoomWidth = (VinWidth * 1000UL) / (EisZoomRatio.X * 1000UL);
//    DZoomHeight = (DZoomWidth * MainHeight) / MainWidth;

    if(CropInfo.Enable == 1U){
        /* Dummy win */
        MisraD = (gPrefCfg[MODE].ZoomRatio.X * 1000.0);
        MisraTemp = (UINT32)MisraD;
        gDummyWin.Width = (gActualWin.Width * MisraTemp) / 1000U;
        MisraD = (gPrefCfg[MODE].ZoomRatio.Y * 1000.0);
        MisraTemp = (UINT32)MisraD;
        gDummyWin.Height = (gActualWin.Height * MisraTemp) / 1000U;
    }
    else{
        /* Dummy win */
        MisraD = (gPrefCfg[MODE].DummyRatio.X * 1000.0);
        MisraTemp = (UINT32)MisraD;
        gDummyWin.Width = (gActualWin.Width * MisraTemp) / 1000U;

        MisraD = (gPrefCfg[MODE].DummyRatio.Y * 1000.0);
        MisraTemp = (UINT32)MisraD;
        gDummyWin.Height = (gActualWin.Height * MisraTemp) / 1000U;
    }

    DummyOffsetX = (((INT32) gDummyWin.Width) - ((INT32) gActualWin.Width)) / 2;
    DummyOffsetY = (((INT32) gDummyWin.Height) - ((INT32) gActualWin.Height )) / 2;

    /* Dummy range (percent, .16 format) */
    gImgDummy.Enable = 1U;

    VarL = (DummyOffsetX * 65536L) / ((INT32) VinWidth);
    gImgDummy.Left = (UINT32) VarL;  /* .16 format */
    gImgDummy.Right = (UINT32) VarL; /* .16 format */

    VarL = (DummyOffsetY * 65536L) / ((INT32) VinHeight);
    gImgDummy.Top = (UINT32) VarL;    /* .16 format */
    gImgDummy.Bottom = (UINT32) VarL; /* .16 format */

    /* Zoom Range (percent, .16 format) */
    gImgDzoom.Enable = 1U;
    gImgDzoom.ShiftX = 0;
    gImgDzoom.ShiftY = 0;
    gImgDzoom.ZoomX = (VinWidth * 65536U) / gActualWin.Width;
    gImgDzoom.ZoomY = (VinHeight * 65536U) / gActualWin.Height;

    return;

}

static void AmbaEISCommon_WindowInfoInit(const AMBA_EISCTRL_WINPARM_s *pWindow){

    FLOAT misra_f1, misra_f2;

    if(0U != AmbaWrap_memcpy(&gInputCfg.Window, pWindow, sizeof(AMBA_EISCTRL_WINPARM_s))){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy Error!!",0U, 0U, 0U, 0U, 0U);
    }

    misra_f1 = (FLOAT)gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorDen;
    misra_f2 = (FLOAT)gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorNum;
    gInputCfg.Window.DownsampleX = misra_f1 / misra_f2;

    misra_f1 = (FLOAT)gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorDen;
    misra_f2 = (FLOAT)gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorNum;
    gInputCfg.Window.DownsampleY = misra_f1 / misra_f2;

    //DefEisWin.CapH = DefEisWin.VinSensor.Height;
    //AmbaEisCtrl_Log_DBG(LOG_EIS_COM,"---------WindowInfo finishes-------",0U, 0U, 0U, 0U, 0U);

    return;
}

static void AmbaEISCommon_SensorInfoInit(const AMBA_EISCTRL_SENSOR_s *pSensor){


    DOUBLE misra_d1,misra_d2,misra_d3,misra_d4;
    UINT32 misra_u32_1, misra_u32_2;//, misra_temp;

    if(0U != AmbaWrap_memcpy(&gInputCfg.Sensor, pSensor, sizeof(AMBA_EISCTRL_SENSOR_s))){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy Error!!",0U, 0U, 0U, 0U, 0U);
    }


    misra_d1 = (DOUBLE)gInputCfg.Sensor.SensorStatus.ModeInfo.FrameRate.TimeScale;
    misra_d2 = (DOUBLE)gInputCfg.Sensor.SensorStatus.ModeInfo.FrameRate.NumUnitsInTick;
    misra_d3 = (DOUBLE)gInputCfg.Sensor.SensorStatus.ModeInfo.FrameRate.Interlace;
    misra_d4 = (misra_d1/(misra_d2 * (1.0 + misra_d3))) + 0.999999;



    gInputCfg.Sensor.FrameRatePerSec = (UINT32)misra_d4;

    misra_u32_1 = (gInputCfg.Sensor.SensorStatus.ModeInfo.FrameLengthLines - gInputCfg.Sensor.SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Height);
    misra_d1    = (gInputCfg.Sensor.SensorStatus.ModeInfo.RowTime*1000000000.0);
    misra_u32_2 = (UINT32)misra_d1;

    gInputCfg.Sensor.VerticalBlankTime = ((misra_u32_1 * misra_u32_2) + 500000U)/1000000U;

    return;
}

static void AmbaEISCommon_CaculatWarp(AMBA_EISCTRL_USER_DATA_s *pEisResInfo){

    UINT32 TileWExp;
    UINT32 TileHExp;

    //INT16 TempX, TempY;
    
    if(AutoTest.ZSEnalbe != 0U){
        for(UINT32 i = 0U; i<MAX_WARP_MAP_SIZE; i++){
          gWarpTb[i].X = 0;
          gWarpTb[i].Y = 0;
          pEisResInfo->DZoomInfo.ShiftX = EisMove.MovX * 65536;
          pEisResInfo->DZoomInfo.ShiftY = EisMove.MovY * 65536;
        }
    }
#if 0    
    else if(AutoTest.CropEnable != 0U){
        for(UINT32 i = 0U; i<MAX_WARP_MAP_SIZE; i++){
            if(AutoTest.TestStep > 0){
                TempX = (INT16)((INT32)(EisMove.MovX * AutoTest.TestStep / 30) * 16);
                TempY = (INT16) ((INT32)(EisMove.MovY * AutoTest.TestStep / 30) * 16);
                gWarpTb[i].X = TempX;
                gWarpTb[i].Y = TempY;
                pEisResInfo->DZoomInfo.ShiftX = 0;
                pEisResInfo->DZoomInfo.ShiftY = 0;
                //pEisResInfo->DZoomInfo.ShiftX = (EisMove.MovX * (30 - AutoTest.TestStep) / 30) * 65536;
                //pEisResInfo->DZoomInfo.ShiftY = (EisMove.MovY * (30 - AutoTest.TestStep) / 30) * 65536;
            }
            else{
                TempX = (INT16)((INT32)(EisMove.MovX * (-1 * AutoTest.TestStep) / 30) * 16);
                TempY = (INT16)((INT32)(EisMove.MovY *(-1 * AutoTest.TestStep) / 30) * 16);
                gWarpTb[i].X = TempX;
                gWarpTb[i].Y = TempY;
                pEisResInfo->DZoomInfo.ShiftX = 0;
                pEisResInfo->DZoomInfo.ShiftY = 0;
                //pEisResInfo->DZoomInfo.ShiftX = (EisMove.MovX * (30 + AutoTest.TestStep) / 30) * 65536;
                //pEisResInfo->DZoomInfo.ShiftY = (EisMove.MovY * (30 + AutoTest.TestStep) / 30) * 65536;
            }
        }
    }
#endif
    else{
        for(UINT32 i = 0U; i<MAX_WARP_MAP_SIZE; i++){
            gWarpTb[i].X = (INT16)EisMove.MovX * 16;
            gWarpTb[i].Y = (INT16)EisMove.MovY * 16;
            pEisResInfo->DZoomInfo.ShiftX = 0;
            pEisResInfo->DZoomInfo.ShiftY = 0;
        }
    }



    if(AutoTest.RotEnable != 0U){

        
        AMBA_WU_WARP_ROTATE_IN_s WarpRotateIn;
        AMBA_WU_WARP_ROTATE_RESULT_s WarpRotateOut;


        TileWExp = gPrefCfg[MODE].WarpInfo.TileWidthExp;
        TileHExp = gPrefCfg[MODE].WarpInfo.TileHeightExp;


        WarpRotateIn.Theta                                          = AutoTest.RotateAngle;
        WarpRotateIn.CenterX                                        = gInputCfg.Window.WindowSizeInfo.VinSensor.Width>>1U;
        WarpRotateIn.CenterY                                        = gInputCfg.Window.WindowSizeInfo.VinSensor.Height>>1U;
        WarpRotateIn.InputInfo.Version                              = EIS_WARP_VERSION;
        WarpRotateIn.InputInfo.HorGridNum                           = ((gInputCfg.Window.WindowSizeInfo.VinSensor.Width + ((1UL << TileWExp) - 1U)) >> TileWExp) + 1UL;
        WarpRotateIn.InputInfo.VerGridNum                           = ((gInputCfg.Window.WindowSizeInfo.VinSensor.Height + ((1UL << TileHExp) - 1U)) >> TileHExp) + 1UL;
        WarpRotateIn.InputInfo.TileWidthExp                         = TileWExp;
        WarpRotateIn.InputInfo.TileHeightExp                        = TileHExp;
        WarpRotateIn.InputInfo.VinSensorGeo.StartX                  = gInputCfg.Window.WindowSizeInfo.VinSensor.StartX;
        WarpRotateIn.InputInfo.VinSensorGeo.StartY                  = gInputCfg.Window.WindowSizeInfo.VinSensor.StartY;
        WarpRotateIn.InputInfo.VinSensorGeo.Width                   = gInputCfg.Window.WindowSizeInfo.VinSensor.Width;
        WarpRotateIn.InputInfo.VinSensorGeo.Height                  = gInputCfg.Window.WindowSizeInfo.VinSensor.Height;
        WarpRotateIn.InputInfo.VinSensorGeo.HSubSample.FactorNum    = gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorNum;
        WarpRotateIn.InputInfo.VinSensorGeo.HSubSample.FactorDen    = gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorDen;
        WarpRotateIn.InputInfo.VinSensorGeo.VSubSample.FactorNum    = gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorNum;
        WarpRotateIn.InputInfo.VinSensorGeo.VSubSample.FactorDen    = gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorDen;
        WarpRotateIn.InputInfo.Enb_2StageCompensation               = 1U;
        WarpRotateIn.InputInfo.pWarp                                = gWarpTb;


        WarpRotateOut.ResultInfo.Version                            = EIS_WARP_VERSION;
        WarpRotateOut.ResultInfo.HorGridNum                         = ((gInputCfg.Window.WindowSizeInfo.VinSensor.Width + ((1UL << TileWExp) - 1U)) >> TileWExp) + 1UL;
        WarpRotateOut.ResultInfo.VerGridNum                         = ((gInputCfg.Window.WindowSizeInfo.VinSensor.Height + ((1UL << TileHExp) - 1U)) >> TileHExp) + 1UL;
        WarpRotateOut.ResultInfo.TileWidthExp                       = TileWExp;
        WarpRotateOut.ResultInfo.TileHeightExp                      = TileHExp;
        WarpRotateOut.ResultInfo.VinSensorGeo.StartX                = gInputCfg.Window.WindowSizeInfo.VinSensor.StartX;
        WarpRotateOut.ResultInfo.VinSensorGeo.StartY                = gInputCfg.Window.WindowSizeInfo.VinSensor.StartY;
        WarpRotateOut.ResultInfo.VinSensorGeo.Width                 = gInputCfg.Window.WindowSizeInfo.VinSensor.Width;
        WarpRotateOut.ResultInfo.VinSensorGeo.Height                = gInputCfg.Window.WindowSizeInfo.VinSensor.Height;
        WarpRotateOut.ResultInfo.VinSensorGeo.HSubSample.FactorNum  = gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorNum;
        WarpRotateOut.ResultInfo.VinSensorGeo.HSubSample.FactorDen  = gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorDen;
        WarpRotateOut.ResultInfo.VinSensorGeo.VSubSample.FactorNum  = gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorNum;
        WarpRotateOut.ResultInfo.VinSensorGeo.VSubSample.FactorDen  = gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorDen;
        WarpRotateOut.ResultInfo.Enb_2StageCompensation             = 1U;
        WarpRotateOut.ResultInfo.pWarp                              = gWarpTb1;

        if(WU_OK!=AmbaWU_WarpRotate(&WarpRotateIn, &WarpRotateOut)){
            AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWU_WarpRotate Error!!",0U, 0U, 0U, 0U, 0U);
        }
        if(0U !=AmbaWrap_memcpy(gWarpTb, gWarpTb1, sizeof(gWarpTb))){
            AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy Error!!",0U, 0U, 0U, 0U, 0U);
        }
        
        if(ShowCmd.RotInfo != 0U){

            UINT32 MisraV, MisraH;
            
            MisraV = pEisResInfo->WarpInfo.VerGridNum / 2U;
            MisraH = pEisResInfo->WarpInfo.HorGridNum / 2U;
            
            AmbaEis_Log_DBG(LOG_EIS_COM,"=== WarpRotateIn === ", 0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"CenterX %d", WarpRotateIn.CenterX, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"CenterY %d", WarpRotateIn.CenterY, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"HorGridNum %d", WarpRotateIn.InputInfo.HorGridNum, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VerGridNum %d", WarpRotateIn.InputInfo.VerGridNum, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"TileWidthExp %d", WarpRotateIn.InputInfo.TileWidthExp, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"TileHeightExp %d", WarpRotateIn.InputInfo.TileHeightExp, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"2StageCompensation %d", WarpRotateIn.InputInfo.Enb_2StageCompensation, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo Width %d", WarpRotateIn.InputInfo.VinSensorGeo.Width, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo Height %d", WarpRotateIn.InputInfo.VinSensorGeo.Height, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo StartX %d", WarpRotateIn.InputInfo.VinSensorGeo.StartX, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo StartY %d", WarpRotateIn.InputInfo.VinSensorGeo.StartY, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo HSubSample N/D [%d, %d]", WarpRotateIn.InputInfo.VinSensorGeo.HSubSample.FactorNum, WarpRotateIn.InputInfo.VinSensorGeo.HSubSample.FactorDen, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo VSubSample N/D [%d, %d]", WarpRotateIn.InputInfo.VinSensorGeo.VSubSample.FactorNum, WarpRotateIn.InputInfo.VinSensorGeo.VSubSample.FactorDen, 0U, 0U, 0U);


            AmbaEis_Log_DBG(LOG_EIS_COM,"=== WarpRotateOut === ", 0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"HorGridNum %d", WarpRotateOut.ResultInfo.HorGridNum, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VerGridNum %d", WarpRotateOut.ResultInfo.VerGridNum, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"TileWidthExp %d", WarpRotateOut.ResultInfo.TileWidthExp, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"TileHeightExp %d", WarpRotateOut.ResultInfo.TileHeightExp, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"2StageCompensation %d", WarpRotateOut.ResultInfo.Enb_2StageCompensation, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo Width %d", WarpRotateOut.ResultInfo.VinSensorGeo.Width, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo Height %d", WarpRotateOut.ResultInfo.VinSensorGeo.Height, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo StartX %d", WarpRotateOut.ResultInfo.VinSensorGeo.StartX, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo StartY %d", WarpRotateOut.ResultInfo.VinSensorGeo.StartY, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo HSubSample N/D [%d, %d]", WarpRotateOut.ResultInfo.VinSensorGeo.HSubSample.FactorNum, WarpRotateIn.InputInfo.VinSensorGeo.HSubSample.FactorDen, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"VinSensorGeo VSubSample N/D [%d, %d]", WarpRotateOut.ResultInfo.VinSensorGeo.VSubSample.FactorNum, WarpRotateIn.InputInfo.VinSensorGeo.VSubSample.FactorDen, 0U, 0U, 0U);

            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"Warp[%d][%d] = [%d,%d] ", (INT32)MisraH, (INT32)MisraV, pEisResInfo->WarpInfo.pWarp[MisraH + (MisraV*pEisResInfo->WarpInfo.HorGridNum)].X, pEisResInfo->WarpInfo.pWarp[MisraH +(MisraV*pEisResInfo->WarpInfo.HorGridNum)].Y, 0);
 

            ShowCmd.RotInfo --;
        }

    if(AutoTest.CropEnable != 0U){
        
        UINT32 MisraV, MisraH;

        /* CROP EIS */
        if(ShowCmd.UserRes != 0U){


            AmbaEis_Log_DBG(LOG_EIS_COM,"[Common, before CROPEIS]",0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"Act",0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   Width        = %d", pEisResInfo->ActWin.ActiveGeo.Width, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   Height       = %d", pEisResInfo->ActWin.ActiveGeo.Height, 0U, 0U, 0U, 0U);
                    
            AmbaEis_Log_DBG(LOG_EIS_COM,"DummyRange: ", 0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   DummyRange.Left   %d", pEisResInfo->DummyRange.Left, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   DummyRange.Right  %d", pEisResInfo->DummyRange.Right, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   DummyRange.Top    %d", pEisResInfo->DummyRange.Top, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   DummyRange.Bottom %d", pEisResInfo->DummyRange.Bottom, 0U, 0U, 0U, 0U);

            AmbaEis_Log_DBG(LOG_EIS_COM,"Dzoom: ", 0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   DZoomInfo.Enable %d", pEisResInfo->DZoomInfo.Enable, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   ImgZoomX %d", pEisResInfo->DZoomInfo.ZoomX, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   ImgZoomY %d", pEisResInfo->DZoomInfo.ZoomY, 0U, 0U, 0U, 0U);
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"   ShiftX %d", (pEisResInfo->DZoomInfo.ShiftX / 65536), 0, 0, 0, 0);
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"   ShiftY %d", (pEisResInfo->DZoomInfo.ShiftY / 65536), 0, 0, 0, 0);

            
            
            MisraV = pEisResInfo->WarpInfo.VerGridNum / 2U;
            MisraH = pEisResInfo->WarpInfo.HorGridNum / 2U;
            
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"Warp[%d][%d] = [%d,%d] ", (INT32)MisraH, (INT32)MisraV, (INT32)pEisResInfo->WarpInfo.pWarp[MisraH + (MisraV * pEisResInfo->WarpInfo.HorGridNum)].X, (INT32)pEisResInfo->WarpInfo.pWarp[MisraH + (MisraV * pEisResInfo->WarpInfo.HorGridNum)].Y, 0);
        }


        if(ShowCmd.UserRes != 0U){

            AmbaEis_Log_DBG(LOG_EIS_COM,"[Common, after CROPEIS]",0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"  Act",0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Width        = %d", pEisResInfo->ActWin.ActiveGeo.Width, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Height       = %d", pEisResInfo->ActWin.ActiveGeo.Height, 0U, 0U, 0U, 0U);
                    
            AmbaEis_Log_DBG(LOG_EIS_COM,"DummyRange: ", 0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   Left   %d", pEisResInfo->DummyRange.Left, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   Right  %d", pEisResInfo->DummyRange.Right, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   Top    %d", pEisResInfo->DummyRange.Top, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   Bottom %d", pEisResInfo->DummyRange.Bottom, 0U, 0U, 0U, 0U);

            AmbaEis_Log_DBG(LOG_EIS_COM,"Dzoom: ", 0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   DZoomInfo.Enable %d", pEisResInfo->DZoomInfo.Enable, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   ZoomX %d", pEisResInfo->DZoomInfo.ZoomX, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"   ZoomX %d", pEisResInfo->DZoomInfo.ZoomY, 0U, 0U, 0U, 0U);
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"   ShiftX %d", (pEisResInfo->DZoomInfo.ShiftX / 65536), 0, 0, 0, 0);
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"   ShiftY %d", (pEisResInfo->DZoomInfo.ShiftY / 65536), 0, 0, 0, 0);


            AmbaEis_Log_DBG(LOG_EIS_COM,"[Common, after CROPEIS] -----------------------", 0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"  Act",0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Width        = %d", pEisResInfo->ActWin.ActiveGeo.Width, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Height       = %d", pEisResInfo->ActWin.ActiveGeo.Height, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"  DummyRange",0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable       = %d", pEisResInfo->DummyRange.Enable, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Right        = %d", pEisResInfo->DummyRange.Right, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Left         = %d", pEisResInfo->DummyRange.Left, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Top          = %d", pEisResInfo->DummyRange.Top, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Bottom       = %d", pEisResInfo->DummyRange.Bottom, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"  Dzoom",0U, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable       = %d", pEisResInfo->DZoomInfo.Enable , 0U, 0U, 0U, 0U);
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      ShiftX       = %d", pEisResInfo->DZoomInfo.ShiftX / 65536, 0, 0, 0, 0);
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      ShiftY       = %d", pEisResInfo->DZoomInfo.ShiftY / 65536, 0, 0, 0, 0);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      ZoomX        = %d", pEisResInfo->DZoomInfo.ZoomX, 0U, 0U, 0U, 0U);
            AmbaEis_Log_DBG(LOG_EIS_COM,"      ZoomY        = %d", pEisResInfo->DZoomInfo.ZoomY, 0U, 0U, 0U, 0U);
        
            MisraV = pEisResInfo->WarpInfo.VerGridNum / 2U;
            MisraH = pEisResInfo->WarpInfo.HorGridNum / 2U;
        
            AmbaEis_Log_INT_DBG(LOG_EIS_COM,"Warp[%d][%d] = [%d,%d] ", (INT32)MisraH, (INT32)MisraV, (INT32)pEisResInfo->WarpInfo.pWarp[MisraH + (MisraV * pEisResInfo->WarpInfo.HorGridNum)].X, (INT32)pEisResInfo->WarpInfo.pWarp[MisraH + (MisraV * pEisResInfo->WarpInfo.HorGridNum)].Y, 0);
        }

    }

    }
}
static void AmbaEISCommon_WarpRing(const AMBA_EISCTRL_USER_DATA_s *pEisResInfo){


    static UINT8 WarpRingIdx = 0U;


#ifdef AmbaEIS_Sim
    static AMBA_IK_GRID_POINT_s CalibWarpTb_1[MAX_WARP_MAP_SIZE];
    static AMBA_IK_GRID_POINT_s CalibWarpTb_2[MAX_WARP_MAP_SIZE];
#else
    static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT CalibWarpTb_1[MAX_WARP_MAP_SIZE];
    static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT CalibWarpTb_2[MAX_WARP_MAP_SIZE];
#endif

    if((WarpRingIdx%2U) == 0U){        
        if(0U !=AmbaWrap_memcpy(CalibWarpTb_1, gWarpTb, sizeof(gWarpTb))){
            AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy gWarpTb Error!!",0U, 0U, 0U, 0U, 0U);
        }
        if(0U !=AmbaWrap_memcpy(pEisResInfo->WarpInfo.pWarp, CalibWarpTb_1, sizeof(CalibWarpTb_1))){
            AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy CalibWarpTb_1 Error!!",0U, 0U, 0U, 0U, 0U);
        }
        WarpRingIdx = 0U;
    }
    else{
        if(0U != AmbaWrap_memcpy(CalibWarpTb_2, gWarpTb, sizeof(gWarpTb))){
            AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy gWarpTb Error!!",0U, 0U, 0U, 0U, 0U);
        }
        if(0U != AmbaWrap_memcpy(pEisResInfo->WarpInfo.pWarp, CalibWarpTb_2, sizeof(CalibWarpTb_2))){
            AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy CalibWarpTb_2 Error!!",0U, 0U, 0U, 0U, 0U);
        }
        WarpRingIdx++;
    }

    //(void)AmbaWrap_memcpy(pEisResInfo->WarpInfo.pWarp, gWarpTb, sizeof(gWarpTb));
}

UINT32 AmbaEISCommon_UnitTest(const AMBA_EISCTRL_WINPARM_s *pWindow, const AMBA_EISCTRL_SENSOR_s *pSensorInfo,const AMBA_EISCTRL_IMU_INFO_s ImuInfo, AMBA_EISCTRL_SYS_INFO_s SystemInfo, AMBA_EISCTRL_USER_DATA_s *pEisResInfo){

    //static AMBA_IK_GRID_POINT_s GNU_SECTION_NOZEROINIT WarpTableTemp[MAX_WARP_MAP_SIZE];

    //gUserData.WarpInfo.pWarp = &WarpTableTemp[0];

    if(0U != AmbaWrap_memcpy(&gInputCfg.Window, pWindow, sizeof(AMBA_EISCTRL_WINPARM_s))){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy pWindow Error!!",0U, 0U, 0U, 0U, 0U);
    }
    if(0U != AmbaWrap_memcpy(&gInputCfg.Sensor, pSensorInfo, sizeof(AMBA_EISCTRL_SENSOR_s))){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy pSensorInfo Error!!",0U, 0U, 0U, 0U, 0U);
    }    
    if(0U != AmbaWrap_memcpy(&gInputCfg.ImuInfo, &ImuInfo, sizeof(AMBA_EISCTRL_IMU_INFO_s))){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy ImuInfo Error!!",0U, 0U, 0U, 0U, 0U);
    }
    if(0U != AmbaWrap_memcpy(&gInputCfg.SystemInfo, &SystemInfo, sizeof(AMBA_EISCTRL_SYS_INFO_s))){
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy SystemInfo Error!!",0U, 0U, 0U, 0U, 0U);
    }
    //if(0U != AmbaWrap_memcpy(&gUserData, pEisResInfo, sizeof(AMBA_EISCTRL_USER_DATA_s))){
    //    AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy pEisResInfo Error!!",0U, 0U, 0U, 0U, 0U);
    //}

    if(0U != AmbaWrap_memset(gWarpTb, 0, sizeof(AMBA_IK_GRID_POINT_s)*MAX_WARP_MAP_SIZE)) {
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy gWarpTb Error!!",0U, 0U, 0U, 0U, 0U);
    }
    if(0U != AmbaWrap_memset(gWarpTb1, 0, sizeof(AMBA_IK_GRID_POINT_s)*MAX_WARP_MAP_SIZE)) {
        AmbaEis_Log_NG(LOG_EIS_COM,"AmbaWrap_memcpy gWarpTb1 Error!!",0U, 0U, 0U, 0U, 0U);
    }

    (void)AmbaEISCommon_WindowInfoInit(pWindow);
    (void)AmbaEISCommon_SensorInfoInit(pSensorInfo);
    (void)AmbaEISCommon_CalcDummy();
    (void)AmbaEISCommon_SetUserData(pEisResInfo);




    if(ShowCmd.InWin == 1U){
        FLOAT MisraA;
        UINT32 MisraB;
        AmbaEis_Log_STR_DBG(LOG_EIS_COM,"%s",gPrefCfg[MODE].ModeName, NULL, NULL, NULL, NULL);
        AmbaEis_Log_DBG(LOG_EIS_COM,"Input Windiw",0U, 0U, 0U, 0U, 0U);

        AmbaEis_Log_DBG(LOG_EIS_COM,"Vin: ", 0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   VinWidth %d", gInputCfg.Window.WindowSizeInfo.VinSensor.Width, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   VinHeight %d", gInputCfg.Window.WindowSizeInfo.VinSensor.Height, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   StartX %d", gInputCfg.Window.WindowSizeInfo.VinSensor.StartX, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   StartY %d", gInputCfg.Window.WindowSizeInfo.VinSensor.StartY, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   VinSensorGeo HSubSample N/D [%d, %d]", gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorNum, gInputCfg.Window.WindowSizeInfo.VinSensor.HSubSample.FactorDen, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   VinSensorGeo VSubSample N/D [%d, %d]", gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorNum, gInputCfg.Window.WindowSizeInfo.VinSensor.VSubSample.FactorDen, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   Main",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Width     = %d",gInputCfg.Window.WindowSizeInfo.Main.Width, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Height    = %d",gInputCfg.Window.WindowSizeInfo.Main.Height, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"   Cap",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Width     = %d",gInputCfg.Window.Cap.Width, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Height    = %d",gInputCfg.Window.Cap.Height, 0U, 0U, 0U, 0U);

        MisraA = gInputCfg.Window.DownsampleX * 1000.0f;
        MisraB = (UINT32)MisraA;
        AmbaEis_Log_DBG(LOG_EIS_COM, "      DownsampleX = %d", MisraB, 0U, 0U, 0U, 0U);

        MisraA = gInputCfg.Window.DownsampleY * 1000.0f;
        MisraB = (UINT32)MisraA;
        AmbaEis_Log_DBG(LOG_EIS_COM, "      DownsampleY = %d", MisraB, 0U, 0U, 0U, 0U);

        AmbaEis_Log_DBG(LOG_EIS_COM, "  DownsampleSft", 0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM, "      XSft = %d", gInputCfg.Window.DownsampleXSft, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM, "      YSft= %d", gInputCfg.Window.DownsampleYSft, 0U, 0U, 0U, 0U);

        ShowCmd.InWin--;
    }

    if(ShowCmd.InSen == 1U){
        FLOAT MisraA;
        UINT32 MisraB;
        MisraA =gInputCfg.Sensor.SensorStatus.ModeInfo.RowTime * 1000000000.0f;
        MisraB = (UINT32)MisraA;

        AmbaEis_Log_STR_DBG(LOG_EIS_COM,"%s",gPrefCfg[MODE].ModeName, NULL, NULL, NULL, NULL);
        AmbaEis_Log_DBG(LOG_EIS_COM,"  VinSensor",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      FPS                  = %d",gInputCfg.Sensor.FrameRatePerSec, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      ExposureTime         = %d",gInputCfg.Sensor.ExposureTime, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      VerticalBlankTime    = %d",gInputCfg.Sensor.VerticalBlankTime, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"  InputInfo", 0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Width %d", (UINT32)gInputCfg.Sensor.SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Width, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Height %d", (UINT32)gInputCfg.Sensor.SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.Height, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      StartX %d", (UINT32)gInputCfg.Sensor.SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartX, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      StartY %d", (UINT32)gInputCfg.Sensor.SensorStatus.ModeInfo.InputInfo.PhotodiodeArray.StartY, 0U, 0U, 0U, 0U);

        AmbaEis_Log_DBG(LOG_EIS_COM,"  Output", 0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Width %d", (UINT32)gInputCfg.Sensor.SensorStatus.ModeInfo.OutputInfo.OutputWidth, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Height %d", (UINT32)gInputCfg.Sensor.SensorStatus.ModeInfo.OutputInfo.OutputHeight, 0U, 0U, 0U, 0U);

        AmbaEis_Log_DBG(LOG_EIS_COM,"  ModeInfo", 0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Interlace %d",(UINT32)gInputCfg.Sensor.SensorStatus.ModeInfo.FrameRate.Interlace, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      TimeScale %d",gInputCfg.Sensor.SensorStatus.ModeInfo.FrameRate.TimeScale, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      NumUnitsInTick %d",gInputCfg.Sensor.SensorStatus.ModeInfo.FrameRate.NumUnitsInTick, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      FrameLengthLines %d",gInputCfg.Sensor.SensorStatus.ModeInfo.FrameLengthLines, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      RowTime %d",MisraB, 0U, 0U, 0U, 0U);

        ShowCmd.InSen--;
    }

    if(ShowCmd.GVal == 1U){
        AmbaEis_Log_STR_DBG(LOG_EIS_COM,"%s",gPrefCfg[MODE].ModeName, NULL, NULL, NULL, NULL);
        AmbaEis_Log_DBG(LOG_EIS_COM,"  Actual",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Width     = %d",gActualWin.Width, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Height    = %d",gActualWin.Height, 0U, 0U, 0U, 0U);

        AmbaEis_Log_DBG(LOG_EIS_COM,"  DummyWin",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Width     = %d",gDummyWin.Width, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Height    = %d",gDummyWin.Height, 0U, 0U, 0U, 0U);

        AmbaEis_Log_DBG(LOG_EIS_COM,"  ImgDzoom",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable    = %d",gImgDzoom.Enable, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      ZoomX     = %d",gImgDzoom.ZoomX, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      ZoomY     = %d",gImgDzoom.ZoomY, 0U, 0U, 0U, 0U);
        AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      ShiftX        = %d", gImgDzoom.ShiftX, 0, 0, 0, 0);
        AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      ShiftY        = %d", gImgDzoom.ShiftY, 0, 0, 0, 0);


        AmbaEis_Log_DBG(LOG_EIS_COM,"  ImgDummy",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable    = %d",gImgDummy.Enable, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Left      = %d",gImgDummy.Left, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Right     = %d",gImgDummy.Right, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Top       = %d",gImgDummy.Top, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Bottom    = %d",gImgDummy.Bottom, 0U, 0U, 0U, 0U);
        ShowCmd.GVal--;
    }


    /*---------------------- Warp Caculate ---------------------------*/

    if((AutoTest.Enable != 0U) || (manualSetWarp != 0U)){
        AmbaEISCommon_AutoTestControl();
        AmbaEISCommon_AutoTestXY();
        AmbaEISCommon_AutoTestZ();
        AmbaEISCommon_CaculatWarp(pEisResInfo);
    }

    /*---------------------- Warp Ring ---------------------------------*/
    AmbaEISCommon_WarpRing(pEisResInfo);

    if(ShowCmd.UserRes == 1U){
        
        UINT32 MisraV, MisraH;

        AmbaPrint_PrintUInt5("=== UserRes === ", 0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"  Act",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable       = %d", pEisResInfo->ActWin.Enable, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      StartX       = %d", pEisResInfo->ActWin.ActiveGeo.StartX, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      StartY       = %d", pEisResInfo->ActWin.ActiveGeo.StartY, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Width        = %d", pEisResInfo->ActWin.ActiveGeo.Width, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Height       = %d", pEisResInfo->ActWin.ActiveGeo.Height, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"  DummyRange",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable       = %d", pEisResInfo->DummyRange.Enable, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Right        = %d", pEisResInfo->DummyRange.Right, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Left         = %d", pEisResInfo->DummyRange.Left, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Top          = %d", pEisResInfo->DummyRange.Top, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Bottom       = %d", pEisResInfo->DummyRange.Bottom, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"  Dzoom",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable       = %d", pEisResInfo->DZoomInfo.Enable , 0U, 0U, 0U, 0U);

        AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      ShiftX        = %d", pEisResInfo->DZoomInfo.ShiftX, 0, 0, 0, 0);
        AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      ShiftY        = %d", pEisResInfo->DZoomInfo.ShiftY, 0, 0, 0, 0);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      ZoomX        = %d", pEisResInfo->DZoomInfo.ZoomX,  0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      ZoomY        = %d", pEisResInfo->DZoomInfo.ZoomY,  0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"  Warp info",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable           = %d", pEisResInfo->WarpInfo.Version, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      HorGridNum       = %d", pEisResInfo->WarpInfo.HorGridNum, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      VerGridNum       = %d", pEisResInfo->WarpInfo.VerGridNum, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      TileWidthExp     = %d", pEisResInfo->WarpInfo.TileWidthExp, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      TileHeightExp    = %d", pEisResInfo->WarpInfo.TileHeightExp, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enb_2StageCompensation = %d", pEisResInfo->WarpInfo.Enb_2StageCompensation, 0U, 0U, 0U, 0U);

        
        
        MisraV = pEisResInfo->WarpInfo.VerGridNum / 2U;
        MisraH = pEisResInfo->WarpInfo.HorGridNum / 2U;
        
        AmbaEis_Log_INT_DBG(LOG_EIS_COM,"Warp[%d][%d] = [%d,%d] ", (INT32)MisraH, (INT32)MisraV, (INT32)pEisResInfo->WarpInfo.pWarp[MisraH + (MisraV * pEisResInfo->WarpInfo.HorGridNum)].X, (INT32)pEisResInfo->WarpInfo.pWarp[MisraH + (MisraV * pEisResInfo->WarpInfo.HorGridNum)].Y, 0);
        


        AmbaEis_Log_DBG(LOG_EIS_COM,"  CropInfo",0U, 0U, 0U, 0U, 0U);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable                  = %d", CropInfo.Enable, 0U, 0U, 0U, 0U);
        AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      ShiftX/ShiftY           = %d/%d", CropInfo.ShiftX, CropInfo.ShiftY, 0, 0, 0);
        AmbaEis_Log_DBG(LOG_EIS_COM,"      CropWidth/CropHeight    = %d/%d", CropInfo.CropWidth, CropInfo.CropHeight, 0U, 0U, 0U);

        ShowCmd.UserRes --;

    }



    return EIS_Algo_OK;

}


#ifdef AmbaEIS_Sim
    void AmbaEISCommon_TestCmd(UINT32 ArgCount, char * const * pArgVector)
#else
    void AmbaEISCommon_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
#endif
{

    UINT32 UArg2 = 0U;
    UINT32 UArg3 = 0U;
    UINT32 UArg4 = 0U;
    UINT32 UArg5 = 0U;



    (void)AmbaUtility_StringToUInt32(pArgVector[2],&UArg2);
    (void)AmbaUtility_StringToUInt32(pArgVector[3],&UArg3);
    (void)AmbaUtility_StringToUInt32(pArgVector[4],&UArg4);
    (void)AmbaUtility_StringToUInt32(pArgVector[5],&UArg5);


    //INT32 Arg2 = 0;
    //INT32 Arg3 = 0;
    //INT32 Arg4 = 0;
    //(void)AmbaEis_StringToInt32(pArgVector[2],&Arg2);
    //(void)AmbaEis_StringToInt32(pArgVector[3],&Arg3);
    //(void)AmbaEis_StringToInt32(pArgVector[4],&Arg4);




    //char IntString[UTIL_MAX_INT_STR_LEN];
    //(void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, ArgCount, 10);
    //PrintFunc(IntString);


    if(ArgCount > 0U){
        if(AmbaUtility_StringCompare(pArgVector[1], "-show", 5U) == 0){

            if(AmbaUtility_StringCompare(pArgVector[2], "inwin", 5U) == 0){
                ShowCmd.InWin= 1U;
            }
            else if(AmbaUtility_StringCompare(pArgVector[2], "insen", 5U) == 0){
                ShowCmd.InSen= 1U;
            }
            else if(AmbaUtility_StringCompare(pArgVector[2], "inimu", 5U) == 0){
                ShowCmd.InImu= 1U;
            }
            else if(AmbaUtility_StringCompare(pArgVector[2], "gval", 4U) == 0){
                ShowCmd.GVal = 1U;
            }
            else if(AmbaUtility_StringCompare(pArgVector[2], "rot", 3U) == 0){
                ShowCmd.RotInfo = 1U;
            }
            else if(AmbaUtility_StringCompare(pArgVector[2], "ures", 4U) == 0){
                ShowCmd.UserRes = 1U;
            }
            else{
               AmbaEis_Log_DBG(LOG_EIS_COM,"-show else",0U, 0U, 0U, 0U, 0U);
            }
        }
        else if(AmbaUtility_StringCompare(pArgVector[1], "-set", 4U) == 0){
            if(AmbaUtility_StringCompare(pArgVector[2], "crop", 4U) == 0){
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &UArg2);
                if(UArg2 == 0U){
                    CropInfo.Enable = 0U;
                }
                if(UArg2 == 1U){
                    (void)AmbaUtility_StringToUInt32(pArgVector[4], &UArg3);
                    (void)AmbaUtility_StringToUInt32(pArgVector[5], &UArg4);

                    CropInfo.Enable = 1U;
                    CropInfo.CropWidth = UArg3;
                    CropInfo.CropHeight = UArg4;
                }
                if(0U!= AmbaEISCtrl_SetPostProcFact(&CropInfo)){
                    AmbaEis_Log_DBG(LOG_EIS_COM,"  Set post fact fail",0U, 0U, 0U, 0U, 0U);
                }
            }
            if(AmbaUtility_StringCompare(pArgVector[2], "warp", 4U) == 0){
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &UArg2);
                manualSetWarp = UArg2;
                /* Resume default value */
                AutoTest.Enable     = 0U;
                AutoTest.RotEnable  = 0U;
                AutoTest.ZSEnalbe   = 0U;
                AutoTest.CropEnable = 0U;
                if(UArg2 != 0U) {
                    INT32 Arg3, Arg4;
                    (void)AmbaEis_StringToInt32(pArgVector[4], &Arg3);
                    (void)AmbaEis_StringToInt32(pArgVector[5], &Arg4);
                    EisMove.MovX = Arg3;
                    EisMove.MovY = Arg4;
                    if(manualSetWarp == 2U) {
                        AutoTest.ZSEnalbe = 1U;
                    }
                    else{
                       AmbaEis_Log_DBG(LOG_EIS_COM,"  know manual cmd",0U, 0U, 0U, 0U, 0U);
                    }
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"Manually set warp X = %d Y = %d", EisMove.MovX, EisMove.MovY, 0, 0, 0);
                }
                else {
                    EisMove.MovX = 0;
                    EisMove.MovY = 0;
                }
            }
            if(AmbaUtility_StringCompare(pArgVector[2], "mode", 4U) == 0){
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &UArg2);
                EISPostProcInit = 0U;
                MODE = UArg2;
            }
        }
        else if(AmbaUtility_StringCompare(pArgVector[1], "-auto", 5U) == 0){
            AutoTest.Enable = 0U;
            AutoTest.RotEnable  = 0U;
            AutoTest.ZSEnalbe   = 0U;
            AutoTest.CropEnable = 0U;

            if(ArgCount >= 5U){
                AutoTest.Enable = UArg3;
                AutoTest.Mode   = UArg4;

                if(AmbaUtility_StringCompare(pArgVector[2], "vin", 3U) == 0){

                    DummyMaxP.MovX = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Width)  - ((INT32) gActualWin.Width)) / 2L;
                    DummyMaxN.MovX = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Width)  - ((INT32) gActualWin.Width)) / 2L;
                    DummyMaxP.MovY = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Height) - ((INT32) gActualWin.Height )) / 2L;
                    DummyMaxN.MovY = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Height) - ((INT32) gActualWin.Height )) / 2L;

                    AmbaEis_Log_DBG(LOG_EIS_COM,"  AutoTest Vin",0U, 0U, 0U, 0U, 0U);
                    AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable     = %d", AutoTest.Enable, 0U, 0U, 0U, 0U);
                    AmbaEis_Log_DBG(LOG_EIS_COM,"      Mode       = %d", AutoTest.Mode, 0U, 0U, 0U, 0U);

                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovX     = %d", DummyMaxP.MovX, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovX     = %d", DummyMaxN.MovX, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovY     = %d", DummyMaxP.MovY, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovY     = %d", DummyMaxN.MovY, 0, 0, 0, 0);

                }
                else if(AmbaUtility_StringCompare(pArgVector[2], "zs", 2U) == 0){

                    AutoTest.Enable = UArg3;
                    AutoTest.Mode   = UArg4;

                    DummyMaxP.MovX = (((INT32) gDummyWin.Width)  - ((INT32) gActualWin.Width)) / 2L;
                    DummyMaxN.MovX = (((INT32) gDummyWin.Width)  - ((INT32) gActualWin.Width)) / 2L;
                    DummyMaxP.MovY = (((INT32) gDummyWin.Height) - ((INT32) gActualWin.Height )) / 2L;
                    DummyMaxN.MovY = (((INT32) gDummyWin.Height) - ((INT32) gActualWin.Height )) / 2L;

                    AmbaEis_Log_DBG(LOG_EIS_COM,"  AutoTest",0U, 0U, 0U, 0U, 0U);
                    AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable     = %d", AutoTest.Enable, 0U, 0U, 0U, 0U);
                    AmbaEis_Log_DBG(LOG_EIS_COM,"      Mode       = %d", AutoTest.Mode, 0U, 0U, 0U, 0U);

                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovX     = %d", DummyMaxP.MovX, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovX     = %d", DummyMaxN.MovX, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovY     = %d", DummyMaxP.MovY, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovY     = %d", DummyMaxN.MovY, 0, 0, 0, 0);

                    AutoTest.ZSEnalbe = 1U;
                }
                else if(AmbaUtility_StringCompare(pArgVector[2], "crop", 4U) == 0){
                    AutoTest.Enable = UArg3;
                    AutoTest.Mode   = UArg4;
                    if(UArg5 == 0U){
                        DummyMaxP.MovX = (((INT32) gDummyWin.Width)  - ((INT32) gActualWin.Width)) / 2L;
                        DummyMaxN.MovX = (((INT32) gDummyWin.Width)  - ((INT32) gActualWin.Width)) / 2L;
                        DummyMaxP.MovY = (((INT32) gDummyWin.Height) - ((INT32) gActualWin.Height )) / 2L;
                        DummyMaxN.MovY = (((INT32) gDummyWin.Height) - ((INT32) gActualWin.Height )) / 2L;
                    }
                    if(UArg5 == 1U){
                        DummyMaxP.MovX = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Width)  - ((INT32) gActualWin.Width)) / 2L;
                        DummyMaxN.MovX = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Width)  - ((INT32) gActualWin.Width)) / 2L;
                        DummyMaxP.MovY = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Height) - ((INT32) gActualWin.Height )) / 2L;
                        DummyMaxN.MovY = (((INT32) gInputCfg.Window.WindowSizeInfo.VinSensor.Height) - ((INT32) gActualWin.Height )) / 2L;
                    }
                    AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable     = %d", AutoTest.Enable, 0U, 0U, 0U, 0U);
                    AmbaEis_Log_DBG(LOG_EIS_COM,"      Mode       = %d", AutoTest.Mode, 0U, 0U, 0U, 0U);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovX     = %d", DummyMaxP.MovX, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovX     = %d", DummyMaxN.MovX, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovY     = %d", DummyMaxP.MovY, 0, 0, 0, 0);
                    AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovY     = %d", DummyMaxN.MovY, 0, 0, 0, 0);
                }
                else{
                    AmbaEis_Log_DBG(LOG_EIS_COM,"unknown case",0U, 0U, 0U, 0U, 0U);
                }

            }
            else{
                AutoTest.Enable = UArg2;
                AutoTest.Mode = UArg3;
                AutoTest.RotEnable = 0U;

                DummyMaxP.MovX = (((INT32) gDummyWin.Width)  - ((INT32) gActualWin.Width)) / 2L;
                DummyMaxN.MovX = (((INT32) gDummyWin.Width)  - ((INT32) gActualWin.Width)) / 2L;
                DummyMaxP.MovY = (((INT32) gDummyWin.Height) - ((INT32) gActualWin.Height )) / 2L;
                DummyMaxN.MovY = (((INT32) gDummyWin.Height) - ((INT32) gActualWin.Height )) / 2L;

                AmbaEis_Log_DBG(LOG_EIS_COM,"  AutoTest",0U, 0U, 0U, 0U, 0U);
                AmbaEis_Log_DBG(LOG_EIS_COM,"      Enable     = %d", AutoTest.Enable, 0U, 0U, 0U, 0U);
                AmbaEis_Log_DBG(LOG_EIS_COM,"      Mode       = %d", AutoTest.Mode, 0U, 0U, 0U, 0U);

                AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovX     = %d", DummyMaxP.MovX, 0, 0, 0, 0);
                AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovX     = %d", DummyMaxN.MovX, 0, 0, 0, 0);
                AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      P_MovY     = %d", DummyMaxP.MovY, 0, 0, 0, 0);
                AmbaEis_Log_INT_DBG(LOG_EIS_COM,"      N_MovY     = %d", DummyMaxN.MovY, 0, 0, 0, 0);

            }

            /*------------rotation case ----------------*/
            if(AutoTest.Mode == 2U){

                AMBA_EISCommon_RotateWindowAngle_s ScanWindow = {0};
                AmbaEISCommon_DummyScan(45U, gActualWin, gDummyWin, &ScanWindow);
                AutoTest.MaxAngle = (DOUBLE)ScanWindow.Angle;

                AmbaEis_Log_DBG(LOG_EIS_COM,"  ScanDummy",0U, 0U, 0U, 0U, 0U);
                AmbaEis_Log_DBG(LOG_EIS_COM,"      LimitMaxAngle   = %d", ScanWindow.Angle, 0U, 0U, 0U, 0U);
                AmbaEis_Log_DBG(LOG_EIS_COM,"      LimitDummyWidth = %d", ScanWindow.Width, 0U, 0U, 0U, 0U);
                AmbaEis_Log_DBG(LOG_EIS_COM,"      LimitDummyHeight= %d", ScanWindow.Height, 0U, 0U, 0U, 0U);

                AutoTest.RotEnable = 1U;
                AmbaEis_Log_DB_DBG(LOG_EIS_COM, " MaxAngle (%s, %s)", AutoTest.MaxAngle, 0.0f, 6);
            }
        }
        else if(AmbaUtility_StringCompare(pArgVector[1], "-mode", 5U) == 0){

            MODE = UArg2;
            AmbaEis_Log_DBG(LOG_EIS_COM,"Mode = %d",MODE, 0U, 0U, 0U, 0U);
        }
        else{
#ifndef AmbaEIS_Sim
            PrintFunc("Usage eisc: ");
            PrintFunc("-show warpcfg: show global warp info\n");
            PrintFunc("-mode [0|1|2: x1.14|x1.42|x1.66]\n");


            PrintFunc(pArgVector[0]);
            PrintFunc(pArgVector[1]);
            PrintFunc(pArgVector[2]);
            PrintFunc(pArgVector[3]);
#endif

        }
    }


}

