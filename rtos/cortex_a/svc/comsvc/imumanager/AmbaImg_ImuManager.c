/**
*  @file AmbaImg_ImuManager.c
*
*  @copyright Copyright (c) 2016 Ambarella, Inc.
*
*  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
*  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
*  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
*  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
*  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
*  return this Software to Ambarella, Inc.
*
*  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
*  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  @details AmbaImg_ImuManager
*
*/



#include "AmbaKAL.h"
#include "AmbaGPIO.h"
#include "AmbaDSP_Event.h"
#include "AmbaCache_Def.h"
#include "AmbaTMR.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaPWM.h"
#include "AmbaGPIO_Def.h"
#include "AmbaSensor.h"
#include "AmbaFS.h"
#include "AmbaDSP.h"
#include "AmbaImg_ImuManager.h"
#include "AmbaIMU_MPU6500.h"

#define AMBA_IMU_TIMER_INVTERVAL              5000  // us, decided by user
#define LOG_IMUMGR              "IMU_MGR"
#define LOG_BUF_SIZE                512U
#define LOG_ARGC                    5U
#define IMUMGR_Wrap_sprintfStr      AmbaUtility_StringPrintStr


/* imu device num */
#define AMBA_IMU_DEVICE_NUM     1U


#if 0
typedef enum {
    AMBA_IMU_NO0,
    AMBA_IMU_NO1,
    AMBA_NUM_NO
} AMBA_IMU_ID_e;


typedef struct {
    UINT32  Freq;                   /* Current souce clk freq, max value is GCLK_APB */
    UINT32  Period;
    UINT32  Duty;
} AMBA_USER_PWM_CONFIG_s;
#endif

typedef struct {
    UINT32 VinIndex;            /* 0:Vin0, 1:Vin1*/
    UINT32 SyncTiming;          /* 0:SOF, 1:EOF */
} AMBA_DSP_VIN_SYNC_INFO_s;

#if 0
typedef struct {
    UINT32  SampleFreq;             /*Current souce clk freq, max value is GCLK_APB*/
                                    /*Current pulse rate is SampleFreq / (OnTicks + OffTicks)*/
    UINT32  OnTicks;                /*Output logic high duration ticks*/
    UINT32  OffTicks;            /*Output logic low duration ticks*/
} AMBA_USER_PWM_CONFIG_s;
#endif

#define AMBA_IMAGE_IMUMGR_TASK_STACK_SIZE   0x2000


static AMBA_KAL_TASK_t AmbaImg_ImuManagerTask[AMBA_IMU_DEVICE_NUM]  __attribute__((section(".bss.noinit")));

static AMBA_KAL_EVENT_FLAG_t AmbaImg_ImuManager_Event[AMBA_IMU_DEVICE_NUM];
static AMBA_KAL_MUTEX_t AmbaImg_ImuManager_Mutex[AMBA_IMU_DEVICE_NUM];

/* vin to imu id map */
static UINT32 AmbaImg_ImuManager_VinImuTable[AMBA_NUM_VIN_CHANNEL] = {0U};

/* imu device info */
static AMBA_IMU_INFO_s AmbaImg_ImuDeviceInfo[AMBA_IMU_DEVICE_NUM];

/* imumgr config */
static AMBA_IMG_IMUMGR_CONFIG_s AmbaImg_ImuManager_Config[AMBA_IMU_DEVICE_NUM] = {{.FiFoDepth = AMBA_IMG_IMUMGR_INFO_NUM, .Config = {.EnablePwmSync = 1, .PwmSyncEdge = 1}}};

/* sync data */
static AMBA_IMG_IMUMGR_SYNC_DATA_s AmbaImg_ImuManager_MofSyncData[AMBA_IMU_DEVICE_NUM], AmbaImg_ImuManager_SyncData[AMBA_IMU_DEVICE_NUM];
/* imumgr fifo buffer */
static AMBA_IMG_IMUMGR_FIFO_s AmbaImg_ImuManager_Buf[AMBA_IMU_DEVICE_NUM]  __attribute__((section(".bss.noinit")));

static INT32 ImuMgr_TaskCreate = 0;
static INT32 ImuMgr_TimerCreate = 0;
static UINT8 ImuMgr_PwmSyncEnabled = 0U;

static AMBA_KAL_TIMER_t MofTimer;
static AMBA_KAL_TIMER_t FIFOTimer;
static AMBA_IMG_IMUMGR_MOF_CB_FUNC_f ImuMgrMofCb = NULL;

#if 0
/* local functions */
static INT32 TimerId = 0xF, TimerId2 = 0xF;
static void ImuMgr_ImuFiFoTimerIsr(UINT32 TimerID, UINT32 Param);
static UINT32 AmbaImg_ImuMgr_ImuRead(UINT32 ImuId, AMBA_IMG_IMUMGR_FIFO_INFO_s *pImuInfo, UINT32 Mode);
static INT32 AmbaImg_ImuMgr_ImuInfoGet(UINT32 ImuId, UINT32 Mode);
static void AmbaImg_ImuMgr_Task(UINT32 Param);
#endif

#define Amba_IMUMGR_DEBUG
#ifdef Amba_IMUMGR_DEBUG

static UINT32 ImuMgrDebugFlag = 0U;
static void ImuMgr_Log_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4,UINT32 Arg5)
{
    const char  *StrBuf[LOG_ARGC];

    char        LogBuf[LOG_BUF_SIZE];
    UINT32      Rval, Argc;
    Argc = 0;
    StrBuf[Argc] = pModule;
    Argc++;
    StrBuf[Argc] = pFormat;
    Argc++;

    Rval = IMUMGR_Wrap_sprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|DBG]: %s", Argc, StrBuf);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}


static void ImuMgr_Log_NG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4,UINT32 Arg5)
{

    char        LogBuf[LOG_BUF_SIZE];
    const char  *StrBuf[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    StrBuf[Argc] = pModule;
    Argc++;
    StrBuf[Argc] = pFormat;
    Argc++;

    Rval = IMUMGR_Wrap_sprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|NG]: %s", Argc, StrBuf);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

#endif

#ifdef TEST_FIFO_DATA_GET
static AMBA_IMG_IMUMGR_FIFO_PORT_s ImuFiFoDataPort;
static AMBA_IMG_IMUMGR_FIFO_INFO_s ImuFiFoInfo[2048]  __attribute__((section(".bss.noinit")));
#endif

/* not yet supported */
#if 0
#define AMBA_IMUMGR_SENSOR_TIME
#define TEST_FIFO_DATA_GET
#endif

#ifdef AMBA_IMUMGR_SENSOR_TIME
static UINT32 AmbaImg_ImuManager_LastSensorTimeStampInSof;
#endif

static UINT32 AmbaImg_ImuManager_LastSofTimetick;
static UINT32 AmbaImg_ImuManager_LastEofTimetick;
static UINT32 AmbaImg_ImuManager_LastFrameTime;
static FLOAT AmbaImg_ImuManager_LastShutterTime;
static FLOAT AmbaImg_ImuManager_LastReadoutTime;




static void ImuMgr_ImuFiFoTimerIsr(UINT32 Param)
{
    /* give imu fifo read event */
    (void)AmbaKAL_EventFlagSet(&AmbaImg_ImuManager_Event[Param], (UINT32)AMBA_IMU_EVENT_FLAG_TIMER);
}

static void ImuMgr_MofTimerIsr(UINT32 Param)
{
    /* give mof event */

    (void)AmbaKAL_EventFlagSet(&AmbaImg_ImuManager_Event[Param], (UINT32)AMBA_IMU_EVENT_FLAG_MOF_TIMER);
}

static void ImuMgr_PWMStart(UINT32 GpioPinID, UINT32 PWMPinID)
{


    UINT32 Freq     = 32000U;//32*1000  20U;
    UINT16 Period   = 6400U;//  10U;
    UINT16 Duty     = 160U;//tick:3  10U;

    (void)AmbaGPIO_SetFuncAlt(GpioPinID);
    (void)AmbaPWM_Config(PWMPinID, Freq);// * Period);
    (void)AmbaPWM_Start(PWMPinID, Period, Duty);
    ImuMgr_PwmSyncEnabled = 1U;

    if ((ImuMgrDebugFlag & 16U) > 0U)
    {
        ImuMgr_Log_DBG(LOG_IMUMGR, "ImuMgr_PWMStart", 0U, 0U, 0U, 0U, 0U);
    }
}

static void ImuMgr_PWMStop(UINT32 PWMPinID)
{
    (void)AmbaPWM_Stop(PWMPinID);
}



static UINT32 AmbaImg_ImuMgr_VinIsrHandler(const AMBA_DSP_VIN_SYNC_INFO_s* HdlrInfo, UINT32 ImuId)
{

    static UINT32 SofIsrCnt = 0;
    static UINT32 EofIsrCnt = 0;

    /* do PWM sync */
    AMBA_IMU_CHANNEL_s IMUChan = {0};

    if (AmbaImg_ImuManager_Config[AmbaImg_ImuManager_VinImuTable[HdlrInfo->VinIndex]].Config.EnablePwmSync != 0U) {
        if (AmbaImg_ImuManager_Config[AmbaImg_ImuManager_VinImuTable[HdlrInfo->VinIndex]].Config.PwmSyncEdge == 0U) {    // use Sof sync
            if (HdlrInfo->SyncTiming == 0U) {
                IMUChan.VinID = (UINT8)AMBA_IMU_SENSOR0 + AmbaImg_ImuManager_VinImuTable[HdlrInfo->VinIndex];
                (void)AmbaIMU_IoCtrl((UINT8)IMUChan.VinID, 10U, 0U, 0U);    // Sof notification for frame sync detect (for simulation)
                ImuMgr_PWMStart(AmbaImg_ImuDeviceInfo[ImuId].PwmSyncGpio, AmbaImg_ImuDeviceInfo[ImuId].PwmSyncChan);
#if 0
                if (ImuMgr_PwmSyncEnabled){
                     AmbaPWM_Start(AmbaImg_ImuDeviceInfo[ImuId].PwmSyncChan, pwmCfg->Period, pwmCfg->Duty);
                     AmbaPrint_PrintUInt5("-- AmbaPWM_Start %d %d %d", AmbaImg_ImuDeviceInfo[ImuId].PwmSyncChan, pwmCfg->Period, pwmCfg->Duty, 0U, 0U);
                }
#endif
                SofIsrCnt ++;
            }
            else {
                EofIsrCnt ++;
            }
        }
        else {                                                                                                    // use Eof sync
            if (HdlrInfo->SyncTiming == 1U) {
                IMUChan.VinID = (UINT8)AMBA_IMU_SENSOR0 + AmbaImg_ImuManager_VinImuTable[HdlrInfo->VinIndex];
                (void)AmbaIMU_IoCtrl((UINT8)IMUChan.VinID, 9U, 0U, 0U);  // Eof notification for frame sync detect (for simulation)
                ImuMgr_PWMStart(AmbaImg_ImuDeviceInfo[ImuId].PwmSyncGpio, AmbaImg_ImuDeviceInfo[ImuId].PwmSyncChan);
                EofIsrCnt ++;
            } else {
                SofIsrCnt ++;
            }
        }

    }

    return IMU_ERR_MANAGER_OK;
}


static UINT32 AmbaImg_ImuMgr_ImuRead(UINT32 ImuId, AMBA_IMG_IMUMGR_FIFO_INFO_s *pImuInfo, UINT32 Mode)
{

    UINT32 retCode;
    UINT32 ImuReadMode;
    UINT32 misra_u32;
    AMBA_IMU_CHANNEL_s IMUChan = {0};
    IMUChan.VinID = ((UINT8)AMBA_IMU_SENSOR0 + ImuId);
    retCode = IMU_ERR_NONE;
    switch (Mode) {
        case (UINT32)AMBA_IMUMGR_READ_FIFO:
            misra_u32 = 1U << IMU_READ_BY_FIFO;
            if ((AmbaImg_ImuDeviceInfo[ImuId].Capability & misra_u32) != 0U){
                ImuReadMode = (UINT32)AMBA_IMU_FIFO_COMBO;
            }
            else{
                ImuReadMode = (UINT32)AMBA_IMU_COMBO;
            }


            retCode = AmbaIMU_Read((UINT8)IMUChan.VinID, (UINT8)ImuReadMode, &(pImuInfo->Data));
            #ifdef CONFIG_IMU_AMBARELLA_DMY0000
            (void)AmbaIMU_IoCtrl((UINT32)(AMBA_IMU_SENSOR0 + ImuId),5U,0U,0U);//set TimerFlag;
            #endif
            break;
        #if 0
        case AMBA_IMUMGR_READ_REG:
        default:
            ImuReadMode = AMBA_IMU_COMBO;
            retCode = AmbaIMU_Read(AMBA_IMU_SENSOR0 + ImuId, ImuReadMode, &(pImuInfo->Data));
            break;
        #endif

        case (UINT32)AMBA_IMUMGR_READ_REG:
            break;


        case (UINT32)AMBA_IMUMGR_READ_TOTAL:
            break;

        default:

            AmbaPrint_PrintUInt5("AmbaImg_ImuMgr_ImuRead unknow mode", 0U, 0U, 0U, 0U, 0U);

            break;

    }

    return retCode;
}



static UINT32 AmbaImg_ImuMgr_ImuInfoGet(UINT32 ImuId, UINT32 Mode)
{

    UINT32 retCode = IMU_ERR_MANAGER_IMU_ERR;
    UINT32 WrIndex;
    UINT32 LastWrIndex;
    UINT32 SystemTick;
    UINT32 misra_u32;
    UINT32 FiFoDepth = AmbaImg_ImuManager_Config[ImuId].FiFoDepth;

    switch (Mode) {
        case (UINT32)AMBA_IMUMGR_READ_FIFO:
            /* get the index of buffer */
            WrIndex = AmbaImg_ImuManager_Buf[ImuId].Index.Wr;
            LastWrIndex = (WrIndex + FiFoDepth - 1U) % FiFoDepth;

            /* read imu data */
            #ifdef CONFIG_IMU_AMBARELLA_DMY0000
            AmbaIMU_IoCtrl(AMBA_IMU_SENSOR0 + ImuId,5,1,0);/*set TimerFlag;*/
            #endif

            while ((AmbaImg_ImuMgr_ImuRead(ImuId, &(AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex]), (UINT32)AMBA_IMUMGR_READ_FIFO)) == OK) {
                if ((ImuMgrDebugFlag & 1U) != 0U)
                {

                    ImuMgr_Log_DBG(LOG_IMUMGR, "Before Fync Wr[%d] Ls[%d]  Sof/Eof  = %d %d ",
                                               WrIndex, LastWrIndex,
                                               AmbaImg_ImuManager_MofSyncData[ImuId].SofSystemTime,
                                               AmbaImg_ImuManager_MofSyncData[ImuId].EofSystemTime, 0U);

                    ImuMgr_Log_DBG(LOG_IMUMGR, " GyroXData = %d, GyroYData = %d, GyroZData = %d, FrameSync = %d",                               
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.GyroXData,
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.GyroYData,
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.GyroZData,
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.FrameSync, 0U);
                
                    ImuMgr_Log_DBG(LOG_IMUMGR, " AccXData = %d,  AccYData = %d, AccZData = %d, TemperatureData = %d",
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.AccXData,
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.AccYData,
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.AccZData,
                               AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.TemperatureData, 0U);
                }
                /* update sync data when the frame sync is detected */
                #if 0
                static UINT32 SyncSampleCnt = 0;
                if (AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.FrameSync == 1) {
                    SyncSampleCnt ++;
                } else {
                    if (SyncSampleCnt) {
                        AmbaPrint("SyncSampleCnt = %d (%d)", SyncSampleCnt, WrIndex);
                    }
                    SyncSampleCnt = 0;
                }
                #endif


                if (AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.FrameSync == 1U) {
                    static UINT32 FsyncIndex = 0U;
                    if (AmbaImg_ImuManager_Buf[ImuId].Info[LastWrIndex].Data.FrameSync == 0U) {

                        AmbaImg_ImuManager_SyncData[ImuId] = AmbaImg_ImuManager_MofSyncData[ImuId];                       
                        if (((WrIndex - FsyncIndex + FiFoDepth) % FiFoDepth) < (AmbaImg_ImuManager_LastFrameTime >> 1)){
                            ImuMgr_Log_DBG(LOG_IMUMGR, "imu fifo fsync interval (%d)", ((WrIndex - FsyncIndex + FiFoDepth) % FiFoDepth), 0U, 0U, 0U, 0U);
                        }
                        FsyncIndex = WrIndex;
                    }
                    AmbaImg_ImuManager_SyncData[ImuId].EofSystemTime = AmbaImg_ImuManager_LastEofTimetick;

                    
                    if ((ImuMgrDebugFlag & 2U) != 0U){
                    
                        ImuMgr_Log_DBG(LOG_IMUMGR, "frame sync detected %d, fsync_cnt = %d, pts = %d %d", WrIndex,
                                            ((WrIndex - FsyncIndex + FiFoDepth) % FiFoDepth),
                                            (UINT32)AmbaImg_ImuManager_SyncData[ImuId].RawCapPts,
                                            (UINT32)AmbaImg_ImuManager_MofSyncData[ImuId].RawCapPts,
                                            0U);
                    }
                }

                /* copy sync data to fifo */
                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Sync = AmbaImg_ImuManager_SyncData[ImuId];
                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Sync.PosIndex = WrIndex;
                (void)AmbaKAL_GetSysTickCount(&SystemTick);
                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Sync.SystemTimeTick = SystemTick;

                /* update the index of buffer */
                AmbaImg_ImuManager_Buf[ImuId].Index.Wr = (WrIndex + 1U) % FiFoDepth;

                /* update the current write index */
                LastWrIndex = WrIndex;
                WrIndex = AmbaImg_ImuManager_Buf[ImuId].Index.Wr;

                /* exit loop if imu device does not have hw fifo */

                misra_u32 = (1U << IMU_READ_BY_FIFO);
                if (0U == (AmbaImg_ImuDeviceInfo[ImuId].Capability & misra_u32)){
                    break;
                }

            }

            if ((ImuMgrDebugFlag & 4U) != 0U)
            {
            
                ImuMgr_Log_DBG(LOG_IMUMGR, "After Fync Wr[%d] Ls[%d]  Sof/Eof  = %d %d ",
                               WrIndex, LastWrIndex,
                               AmbaImg_ImuManager_MofSyncData[ImuId].SofSystemTime,
                               AmbaImg_ImuManager_MofSyncData[ImuId].EofSystemTime, 0U);
            
                ImuMgr_Log_DBG(LOG_IMUMGR, " GyroXData = %d, GyroYData = %d, GyroZData = %d, FrameSync = %d",
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.GyroXData,
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.GyroYData,
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.GyroZData,
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.FrameSync, 0U);
            
                ImuMgr_Log_DBG(LOG_IMUMGR, " AccXData = %d,  AccYData = %d, AccZData = %d, TemperatureData = %d",
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.AccXData,
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.AccYData,
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.AccZData,
                                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Data.TemperatureData, 0U);
            }


            retCode = IMU_ERR_MANAGER_OK;

            break;

        case (UINT32)AMBA_IMUMGR_READ_REG:
            break;
        default:
            AmbaPrint_PrintUInt5("AmbaImg_ImuMgr_ImuInfoGet unknow mode", 0U, 0U, 0U, 0U, 0U);
            break;


        #if 0
        case AMBA_IMUMGR_READ_REG:
        default:
            /* get write index */
            WrIndex = AmbaImg_ImuManager_Buf[ImuId].Index.Wr;
            /* read imu data */
            if ((retCode = AmbaImg_ImuMgr_ImuRead(ImuId, &(AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex]), AMBA_IMUMGR_READ_REG)) == OK) {
                /* attached aux info */

                /* sensor time stamp */
                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Sync.Sts = AmbaTimer_GetSysTickCount();

                /* sof time tick */
                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Sync.Sof = AmbaImg_ImuManager_SofTimetick;

                /* shutter time in sof */
                AmbaImg_ImuManager_Buf[ImuId].Info[WrIndex].Sync.Shr = AmbaImg_ImuManager_ShutterTime;

                /* update the index of buffer */
                AmbaImg_ImuManager_Buf[ImuId].Index.Wr = (WrIndex + 1) % FiFoDepth;
            }
            break;
        #endif
    }

    return retCode;
}

static void AmbaImg_ImuMgr_Task(UINT32 Param)
{

    static char MofTimerName[16] = "MofTimer";
    static char FiFoTimerName[16] = "FiFoTimer";
    static UINT8 TaskFirstStartup = 1U;

    //    AMBA_DSP_VIN_SYNC_INFO_s HdlrInfo = {.VinIndex = 0U, .SyncTiming = 0U}; 
    AMBA_IMU_CHANNEL_s ImuChan = {0U};

    UINT32 VinImuId = Param;
    UINT32 ActualFlag = 0U;
    UINT32 misra_u32;
    UINT8 FlagSof = 0U;
    UINT32 RtVal = OK;
    UINT32 Loop = 1U;

    if (TaskFirstStartup != 0U) {

        if (pAmbaImuObj[(UINT8)AMBA_IMU_SENSOR0 + Param] == NULL) {
            AmbaIMU_Hook((UINT8)AMBA_IMU_SENSOR0, &AmbaIMU_MPU6500Obj);
            ImuMgr_Log_DBG(LOG_IMUMGR, "Hook IMU_MPU6500...", 0U, 0U, 0U, 0U, 0U);
        }

        ImuChan.VinID = (UINT8)AMBA_IMU_SENSOR0 + Param;
        /* must be called at least once for initialization */

        if (AmbaImg_ImuManager_Config[ImuChan.VinID].FiFoDepth > AMBA_IMG_IMUMGR_INFO_NUM) {
            AmbaImg_ImuManager_Config[ImuChan.VinID].FiFoDepth = AMBA_IMG_IMUMGR_INFO_NUM;
        }

        RtVal = AmbaIMU_GetDeviceInfo((UINT8)ImuChan.VinID, &AmbaImg_ImuDeviceInfo[Param]);
        if(RtVal != 0U) {
            ImuMgr_Log_DBG(LOG_IMUMGR, "[Check first start up] RtVal : %d", RtVal, 0U, 0U, 0U, 0U);
        }

        if (pAmbaImuObj[Param] == &AmbaIMU_MPU6500Obj) {
            misra_u32 = (1U << IMU_READ_BY_FIFO);
            if ((AmbaImg_ImuDeviceInfo[Param].Capability & misra_u32) != 0U){
                (void)AmbaIMU_SetSampleRate((UINT8)ImuChan.VinID, 1000U);  /*MPU6500 fifo mode can only support 1kHz sampling rate*/
            }
            else{
                ImuMgr_Log_DBG(LOG_IMUMGR, "AmbaIMU_MPU6500Obj != NULL", 0U, 0U, 0U, 0U, 0U);
            }

        } else{
            ImuMgr_Log_NG(LOG_IMUMGR, "AmbaIMU_MPU6500Obj == NULL", 0U, 0U, 0U, 0U, 0U);
        }

        /* open IMU device */

        RtVal = AmbaIMU_Open((UINT8)Param);
        if(RtVal != OK){
            ImuMgr_Log_NG(LOG_IMUMGR, "AmbaIMU_MPU6500O open failed", 0U, 0U, 0U, 0U, 0U);
        }

        RtVal = AmbaKAL_TaskSleep(AmbaImg_ImuDeviceInfo[Param].StartUpTime);
        if(RtVal != OK){
            ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_TaskSleep failed", 0U, 0U, 0U, 0U, 0U);
        }

        /* reset imumgr fifo */

        RtVal = AmbaImg_ImuMgr_FiFoReset(Param);
        if(RtVal != OK){
            ImuMgr_Log_NG(LOG_IMUMGR, "AmbaImg_ImuMgr_FiFoReset failed", 0U, 0U, 0U, 0U, 0U);
        }



        /* create timer for imu hw fifo access autonomously */
#if 0
        if (AmbaImg_ImuDeviceInfo[Param].Capability & (1 << IMU_READ_BY_FIFO)) {
            TimerConfig.IntervalVal = AMBA_IMU_TIMER_INVTERVAL;  //CV22 Not support
            TimerConfig.CounterVal = AMBA_IMU_TIMER_INVTERVAL;   //CV22 Not support
        } else {
            TimerConfig.IntervalVal = AMBA_IMU_TIMER_INVTERVAL_NO_FIFO;  //CV22 Not support
            TimerConfig.CounterVal = AMBA_IMU_TIMER_INVTERVAL_NO_FIFO;   //CV22 Not support
        }
        TimerConfig.ExpirationFunc = ImuMgr_ImuFiFoTimerIsr;             //CV22 Not support
        TimerConfig.ExpirationArg = Param;  //CV22 Not support
        TimerId = AmbaTimer_Set(&TimerConfig, AMBA_KAL_WAIT_FOREVER);    //CV22 Not support
#endif


        /* create timer for FIFOreadevent & Mofevent*/
        if(ImuMgr_TimerCreate == 0){
            ImuMgr_TimerCreate++;

            RtVal = AmbaKAL_TimerCreate(&FIFOTimer, FiFoTimerName, ImuMgr_ImuFiFoTimerIsr, VinImuId, 5U, 5U, AMBA_KAL_DONT_START);
            if(RtVal != OK){
                ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_TimerCreate FIFOTimer failed", 0U, 0U, 0U, 0U, 0U);
            }

            RtVal = AmbaKAL_TimerCreate(&MofTimer, MofTimerName, ImuMgr_MofTimerIsr, VinImuId, 10U, 10U, AMBA_KAL_DONT_START);
            if(RtVal != OK){
                ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_TimerCreate MofTimer failed", 0U, 0U, 0U, 0U, 0U);
            }

        }

        (void)AmbaKAL_TimerStart(&FIFOTimer);


#if 0
        /* config PWM for fifo data sync */
        if (AmbaImg_ImuManager_Config[Param].Config.EnablePwmSync && AmbaImg_ImuDeviceInfo[Param].PwmSyncGpio >= 0) {

            UINT32 Freq = 20U;
            UINT16 Period  = 10U;
            UINT16 Duty = 1U;
            AmbaRTSL_GpioSetFuncAlt(AmbaImg_ImuDeviceInfo[Param].PwmSyncGpio);
            pwmCfg.Freq = Freq;
            pwmCfg.Period = Period;
            pwmCfg.Duty = Duty;
            pwmCfg.SampleFreq = 3000000/3; // unit: us
            pwmCfg.OnTicks = 1000*5; // 5ms;
            pwmCfg.OffTicks = 65535; // 65.5ms;
            AmbaPWM_Config(AmbaImg_ImuDeviceInfo[Param].PwmSyncChan, pwmCfg.Freq * pwmCfg.Period);
            ImuMgr_PwmSyncEnabled = 1;
        }
#endif
        TaskFirstStartup = 0U;
        /* event clear */
        (void)AmbaKAL_EventFlagClear(&AmbaImg_ImuManager_Event[Param], 0xFFFFFFFFU);

    }


    while (Loop == 1U) {

        RtVal = AmbaKAL_EventFlagGet(&AmbaImg_ImuManager_Event[Param], 0xF, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlag, AMBA_KAL_WAIT_FOREVER);
        if(RtVal != OK){
            ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_EventFlagGet failed", 0U, 0U, 0U, 0U, 0U);
            Loop = 0U;
        }
 
        /*
        *  timer event
        \*/
        if ((ActualFlag & (UINT32)AMBA_IMU_EVENT_FLAG_TIMER) > 0U) {

            if ((ImuMgrDebugFlag & 16U) != 0U)
            {
                //ImuMgr_Log_DBG(LOG_IMUMGR, "GET AMBA_IMU_EVENT_FLAG_TIMER", 0U, 0U, 0U, 0U, 0U);
            }


            RtVal = AmbaKAL_MutexTake(&AmbaImg_ImuManager_Mutex[Param], AMBA_KAL_WAIT_FOREVER);
            if (RtVal != OK)
            {
                ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_MutexTake failed", 0U, 0U, 0U, 0U, 0U);
            }


            RtVal = AmbaImg_ImuMgr_ImuInfoGet(Param, (UINT32)AMBA_IMUMGR_READ_FIFO);
            if (RtVal != IMU_ERR_MANAGER_OK)
            {
                ImuMgr_Log_NG(LOG_IMUMGR, "AmbaImg_ImuMgr_ImuInfoGet failed, RtVal: %d", RtVal, 0U, 0U, 0U, 0U);
            }


            RtVal = AmbaKAL_MutexGive(&AmbaImg_ImuManager_Mutex[Param]);
            if(RtVal != OK){
                ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_MutexGive failed", 0U, 0U, 0U, 0U, 0U);
            }

        }

        /*
        *  sof event
        \*/

        if ((ActualFlag & (UINT32)AMBA_IMU_EVENT_FLAG_SOF) > 0U)
        {
            if ((ImuMgrDebugFlag & 16U) != 0U)
            {
                ImuMgr_Log_DBG(LOG_IMUMGR, "GET AMBA_IMU_EVENT_FLAG_SOF ",0U, 0U, 0U, 0U, 0U);
            }
#if 0
            /* PWM Start */
            if (((AmbaImg_ImuManager_Config[Param].Config.EnablePwmSync > 0U) && (AmbaImg_ImuDeviceInfo[Param].PwmSyncGpio > 0U))) {
                RtVal = AmbaImg_ImuMgr_VinIsrHandler(&HdlrInfo, Param);
                if(RtVal != IMU_ERR_MANAGER_OK){
                    ImuMgr_Log_NG(LOG_IMUMGR, "AmbaImg_ImuMgr_VinIsrHandler failed", 0U, 0U, 0U, 0U, 0U);
                }

            }
#endif

            /* set timer to create mof timing */
            #if 0
            // AMBA_TIMER_CONFIG_s MofTimerConfig;
            // //int TimerId2 = 0xF;
            // MofTimerConfig.IntervalVal      = 0;
            // MofTimerConfig.CounterVal       = (UINT32)(AmbaImg_ImuManager_LastReadoutTime * 1000.0) >> 1;
            // MofTimerConfig.ExpirationFunc   = ImuMgr_MofTimerIsr;
            // MofTimerConfig.ExpirationArg    = VinImuId;
            // TimerId2 = AmbaTimer_Set(&MofTimerConfig, AMBA_KAL_WAIT_FOREVER);
            // //AmbaPrint("timer interval = %d %f", MofTimerConfig.CounterVal, AmbaImg_ImuManager_LastReadoutTime);
            #else
            (void)AmbaKAL_TimerStart(&MofTimer);
            #endif

            FlagSof = 1;


            #ifdef TEST_FIFO_DATA_GET
            {
                static int OpenFiFoPort = -1;


                if (OpenFiFoPort == -1) {
                    OpenFiFoPort = AmbaImg_ImuMgr_OpenFiFoPort((UINT8)AMBA_IMU_SENSOR0, &ImuFiFoDataPort);
                    ImuMgr_Log_DBG(LOG_IMUMGR, "Open FiFo Port...",0U, 0U, 0U, 0U, 0U);

                }
                else {
                    UINT32 ReadSize;
#if 0
                    UINT32 Index;
                    AmbaImg_ImuMgr_FiFoIndexGet(&ImuFiFoDataPort, 1, &Index);
#endif
                    ReadSize = AmbaImg_ImuMgr_FiFoDataGet(&ImuFiFoDataPort, 0, &ImuFiFoInfo[0]);    /* "0" means read until the latest sample*/
                    

                    if ((ImuMgrDebugFlag & 8U) != 0U)
                    {
                        ImuMgr_Log_DBG(LOG_IMUMGR, "readsize = %d, readindex = %d",ReadSize, ImuFiFoDataPort.ReadIndex, 0U, 0U, 0U);

                        ImuMgr_Log_DBG(LOG_IMUMGR, "data = %d %d %d",ImuFiFoInfo[ReadSize-1].Data.GyroXData,
                                                                     ImuFiFoInfo[ReadSize-1].Data.AccYData,
                                                                     ImuFiFoInfo[ReadSize-1].Data.TemperatureData, 0U, 0U);


                        ImuMgr_Log_DBG(LOG_IMUMGR, "sync = %d %d %d",ImuFiFoInfo[ReadSize-1].Sync.SofSystemTime,
                                                                     ImuFiFoInfo[ReadSize-1].Sync.EofSystemTime,
                                                                     (UINT32)ImuFiFoInfo[ReadSize-1].Sync.RawCapPts, 0U, 0U);
                    }
                }
            }
            #endif
        }

        /*
        *  mof timer event
        \*/
        if ((ActualFlag & (UINT32)AMBA_IMU_EVENT_FLAG_MOF_TIMER) != 0U) {

            static const UINT32 AmbaImg_ImuManager_ImuVinTable[AMBA_IMU_DEVICE_NUM] = {0};
            UINT32 ImuVinId;

            VinImuId = Param;
            ImuVinId = AmbaImg_ImuManager_ImuVinTable[VinImuId];

            if ((ImuMgrDebugFlag & 16U) != 0U)
            {
                ImuMgr_Log_DBG(LOG_IMUMGR, "GET AMBA_IMU_EVENT_FLAG_MOF_TIMER", 0U, 0U, 0U, 0U, 0U);
            }
            /* stop and delete sw timer */
            (void)AmbaKAL_TimerStop(&MofTimer);

            if (FlagSof != 0U) {
                /* latch sync data in mof */
                if (ImuMgrMofCb != NULL) {
                    AMBA_IMG_IMUMGR_MOF_EVENT_DATA_s MofData;
                    MofData.VinId = ImuVinId;
                    (void)ImuMgrMofCb(&MofData);
                    AmbaImg_ImuManager_MofSyncData[VinImuId].RawCapPts = MofData.RawCapPts;
                    if ((ImuMgrDebugFlag & 32U) != 0U) {
                        ImuMgr_Log_DBG(LOG_IMUMGR, "vin = %d, pts = %d",MofData.VinId, (UINT32)MofData.RawCapPts, 0U, 0U, 0U);
                    }
                }
                else
                {
                    if ((ImuMgrDebugFlag & 32U) != 0U)
                    {
                        ImuMgr_Log_DBG(LOG_IMUMGR, "MofCB is NULL...", 0U, 0U, 0U, 0U, 0U);
                    }
                }
                AmbaImg_ImuManager_MofSyncData[VinImuId].SofSystemTime      = AmbaImg_ImuManager_LastSofTimetick;
                AmbaImg_ImuManager_MofSyncData[VinImuId].EofSystemTime      = AmbaImg_ImuManager_LastEofTimetick;
                AmbaImg_ImuManager_MofSyncData[VinImuId].FrameTime          = AmbaImg_ImuManager_LastFrameTime;
                AmbaImg_ImuManager_MofSyncData[VinImuId].ExposureTime[0]    = AmbaImg_ImuManager_LastShutterTime;
                AmbaImg_ImuManager_MofSyncData[VinImuId].ReadOutTime        = AmbaImg_ImuManager_LastReadoutTime;
                FlagSof = 0U;

                /* turn off PWM sync */
                if (AmbaImg_ImuManager_Config[AmbaImg_ImuManager_VinImuTable[VinImuId]].Config.EnablePwmSync != 0U)
                {
                    ImuChan.VinID = (UINT8)AMBA_IMU_SENSOR0 + Param;
                    (void)AmbaIMU_IoCtrl((UINT8)ImuChan.VinID, 11U, 0U, 0U);
                    if (ImuMgr_PwmSyncEnabled != 0U)
                    {
                        //(void)AmbaKAL_GetSysTickCount(&SystemTick); 
                        ImuMgr_PWMStop(AmbaImg_ImuDeviceInfo[VinImuId].PwmSyncChan);
                    }
                }
            }
        }
    }
}

static void *AmbaImg_ImuMgr_TaskEx(void *pParam)
{
    UINT32 Argc;

    AmbaMisra_TouchUnused(pParam);
    AmbaMisra_TypeCast(&Argc,&pParam);
    AmbaImg_ImuMgr_Task(Argc);

    return NULL;

}

#if 0
void AmbaTest(UINT32 ImuId){

     UINT32 WrIndex;
     UINT32 LastWrIndex;
     UINT32 FiFoDepth = AmbaImg_ImuManager_Config[ImuId].FiFoDepth;
     AMBA_USER_PWM_CONFIG_s pwmCfg;

     WrIndex = AmbaImg_ImuManager_Buf[ImuId].Index.Wr;
     LastWrIndex = (WrIndex + FiFoDepth - 1) % FiFoDepth;


     // Fifo depth check
     if (AmbaImg_ImuManager_Config[ImuId].FiFoDepth > AMBA_IMG_IMUMGR_INFO_NUM) {
         AmbaImg_ImuManager_Config[ImuId].FiFoDepth = AMBA_IMG_IMUMGR_INFO_NUM;
     }

     // get gyro device info
     AmbaIMU_GetDeviceInfo(ImuId, &AmbaImg_ImuDeviceInfo[ImuId]);
     //Hook Gyro check
     if (pAmbaImuObj[ImuId] == NULL) {
         extern AMBA_IMU_OBJ_s AmbaIMU_MPU6500Obj;
         AmbaIMU_Hook(AMBA_IMU_SENSOR0, &AmbaIMU_MPU6500Obj);
         AmbaPrint_PrintUInt5("hook MPU6500 device !!", 0U, 0U, 0U, 0U, 0U);
     }else{
         if (AmbaImg_ImuDeviceInfo[ImuId].Capability & (1 << IMU_READ_BY_FIFO)){
             AmbaIMU_SetSampleRate(ImuId, 1000);  // MPU6500 fifo mode can only support 1kHz sampling rate
         }
     }

     // open gyro device info
     AmbaIMU_Open(ImuId);

     // Fifo reset
     AmbaImg_ImuManager_Buf[ImuId].Index.Wr = AmbaImg_ImuManager_Buf[ImuId].Index.Rd = 0;
     AmbaWrap_memset(&AmbaImg_ImuManager_Buf[ImuId].Info, 0, AMBA_IMG_IMUMGR_INFO_NUM * sizeof(AMBA_IMG_IMUMGR_FIFO_INDEX_s));

     // config PWM for fifo data sync
     if (AmbaImg_ImuManager_Config[ImuId].Config.EnablePwmSync && AmbaImg_ImuDeviceInfo[ImuId].PwmSyncGpio >= 0) {

         UINT32 Freq = 30U;
         UINT16 Period  = 33U ;
         UINT16 Duty = 5U;

         AmbaRTSL_GpioSetFuncAlt(AmbaImg_ImuDeviceInfo[ImuId].PwmSyncGpio);
         pwmCfg.Freq = Freq;
         pwmCfg.Period = Period;
         pwmCfg.Duty = Duty;
         AmbaPWM_Config(AmbaImg_ImuDeviceInfo[ImuId].PwmSyncChan, pwmCfg.Freq * pwmCfg.Period);
         ImuMgr_PwmSyncEnabled = 1;

     }
     AmbaPWM_Start(AmbaImg_ImuDeviceInfo[ImuId].PwmSyncChan, pwmCfg.Period, pwmCfg.Duty);

 }

#endif

static UINT32 AmbaImg_ImuMgr_TaskCreate(UINT32 Priority, UINT32 CoreExclusion)
{


    static UINT8 AmbaImg_ImuManager_TaskStack[AMBA_IMU_DEVICE_NUM][AMBA_IMAGE_IMUMGR_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char ImuManagerEventName[AMBA_IMU_DEVICE_NUM][16] = {"ImuMgr_Num0"};
    static char AmbaImg_ImuManagerTaskStr[AMBA_IMU_DEVICE_NUM][32] = { "ImuManager_Imu0Task"};

    UINT32 FuncRetCode = OK;

    void *EntryArg = NULL;

    ImuMgr_Log_DBG(LOG_IMUMGR, "ImuMgr Task Create...", 0U, 0U, 0U, 0U, 0U);
    

    for (UINT32 i = 0; i < AMBA_IMU_DEVICE_NUM; i++) {

        /* create event and mutex */
        (void)AmbaKAL_EventFlagCreate(&AmbaImg_ImuManager_Event[i],ImuManagerEventName[i]);
        (void)AmbaKAL_MutexCreate(&AmbaImg_ImuManager_Mutex[i],ImuManagerEventName[i]);


        /* startup task */
        AmbaMisra_TouchUnused(EntryArg);
        AmbaMisra_TypeCast(&EntryArg, &i);
        FuncRetCode = AmbaKAL_TaskCreate(&(AmbaImg_ImuManagerTask[i]),
                        AmbaImg_ImuManagerTaskStr[i],
                        Priority,
                        AmbaImg_ImuMgr_TaskEx,              /*imumgr task per imu device*/
                        EntryArg,                           /* which imu device*/
                        AmbaImg_ImuManager_TaskStack[i],
                        AMBA_IMAGE_IMUMGR_TASK_STACK_SIZE,
                        AMBA_KAL_DONT_START);

        if (FuncRetCode != KAL_ERR_NONE)
        {
            ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_TaskCreate failed %d", FuncRetCode, 0U, 0U, 0U, 0U);
        }
        else
        {

            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaImg_ImuManagerTask[i], CoreExclusion);
            if (FuncRetCode != KAL_ERR_NONE)
            {
                ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_TaskSetSmpAffinity failed %d", FuncRetCode, 0U, 0U, 0U, 0U);
            }

            FuncRetCode = AmbaKAL_TaskResume(&AmbaImg_ImuManagerTask[i]);
            if (FuncRetCode != KAL_ERR_NONE)
            {
                ImuMgr_Log_NG(LOG_IMUMGR, "AmbaKAL_TaskResume failed", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    ImuMgr_TaskCreate = 1;

    return FuncRetCode;
}

UINT32 AmbaImg_ImuMgr_TaskDelete(void)
{
    UINT32 i;
    UINT32 Ret = IMU_ERR_MANAGER_OK;

    for (i = 0U; i < AMBA_IMU_DEVICE_NUM; i++) {

        if (AmbaKAL_TaskTerminate(&AmbaImg_ImuManagerTask[i]) != OK) {
            Ret = IMU_ERR_MANAGER_TSK_ERR;
            break;
        }
        else{
            (void)AmbaKAL_TimerStop(&FIFOTimer);

            /* delete event and mutex */
            (void)AmbaKAL_EventFlagDelete(&AmbaImg_ImuManager_Event[i]);
            (void)AmbaKAL_MutexDelete(&AmbaImg_ImuManager_Mutex[i]);

            /* delete task */
            (void)AmbaKAL_TaskDelete(&AmbaImg_ImuManagerTask[i]);
            /* delete timer */
            (void)AmbaKAL_TimerDelete(&MofTimer);

        }


    }

    ImuMgr_TaskCreate = 0;
    ImuMgr_TimerCreate = 0;

    return Ret;
}



UINT32 AmbaImg_ImuMgr_Init(UINT32 Priority, UINT32 CoreInclusion)
{

    UINT32 RVal = OK;
    ImuMgr_Log_DBG(LOG_IMUMGR, "ImuMgr Init...", 0U, 0U, 0U, 0U, 0U);
    if (ImuMgr_TaskCreate == 0)
    {
        (void)AmbaImg_ImuMgr_TaskCreate(Priority, CoreInclusion);
        RVal = OK;
    }

    return RVal;
}

UINT32 AmbaImg_ImuMgr_OpenFiFoPort(UINT32 ImuId, AMBA_IMG_IMUMGR_FIFO_PORT_s *Port)
{

    UINT32 ImuNo = ImuId - (UINT8)AMBA_IMU_SENSOR0;
    UINT32 SystemTick;
    ImuMgr_Log_DBG(LOG_IMUMGR, "Open FiFo Port...", 0U, 0U, 0U, 0U, 0U);

        /* magic number */
        Port->Magic = 0xbeefU;

        /* imu id */
        Port->ImuId = ImuNo;

        /* read index */
        Port->ReadIndex = 0U;

        /* get system time */
        (void)AmbaKAL_GetSysTickCount(&SystemTick);
        Port->SystemTimeTick = SystemTick;
    ImuMgr_Log_DBG(LOG_IMUMGR, "Port->Magic %x...", Port->Magic, 0U, 0U, 0U, 0U);

    return OK;
}

UINT32 AmbaImg_ImuMgr_CloseFiFoPort(AMBA_IMG_IMUMGR_FIFO_PORT_s *Port)
{

    ImuMgr_Log_DBG(LOG_IMUMGR, "Close FiFo Port...", 0U, 0U, 0U, 0U, 0U);

    /* clear magic number */
    if (Port->Magic == 0xbeefU)
    {
        Port->Magic = 0U;
    }
    ImuMgr_Log_DBG(LOG_IMUMGR, "Port->Magic %x...", Port->Magic, 0U, 0U, 0U, 0U);

    return OK;
}

UINT32 AmbaImg_ImuMgr_Config(UINT32 ImuId, const AMBA_IMG_IMUMGR_CONFIG_s *Config)
{

    UINT32 ImuNo = ImuId - (UINT8)AMBA_IMU_SENSOR0;
    UINT32 Ret = IMU_ERR_MANAGER_OK;

        AmbaImg_ImuManager_Config[ImuNo] = *Config;
        if (AmbaImg_ImuManager_Config[ImuNo].FiFoDepth > AMBA_IMG_IMUMGR_INFO_NUM) {
            AmbaImg_ImuManager_Config[ImuNo].FiFoDepth = AMBA_IMG_IMUMGR_INFO_NUM;
        }

    return Ret;
}

UINT32  AmbaImg_ImuMgr_FiFoReset(UINT32 ImuId)
{

    /* reset fifo index */
    AmbaImg_ImuManager_Buf[ImuId].Index.Wr = 0U;
    AmbaImg_ImuManager_Buf[ImuId].Index.Rd = 0U;

    /* reset sync data */

    if(AmbaWrap_memset(&AmbaImg_ImuManager_Buf[ImuId].Info, 0, AMBA_IMG_IMUMGR_INFO_NUM * sizeof(AMBA_IMG_IMUMGR_FIFO_INDEX_s)) != 0U) {
        ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memset AmbaImg_ImuManager_Buf error", 0U, 0U, 0U, 0U, 0U);
    }
    if(AmbaWrap_memset(&AmbaImg_ImuManager_MofSyncData, 0, sizeof(AmbaImg_ImuManager_MofSyncData)) != 0U) {
        ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memset AmbaImg_ImuManager_MofSyncData error", 0U, 0U, 0U, 0U, 0U);

    }
    if(AmbaWrap_memset(&AmbaImg_ImuManager_SyncData, 0, sizeof(AmbaImg_ImuManager_SyncData)) != 0U) {
        ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memset AmbaImg_ImuManager_SyncData error", 0U, 0U, 0U, 0U, 0U);
    }


    return OK;
}


UINT32 AmbaImg_ImuMgr_FiFoIndexGet(const AMBA_IMG_IMUMGR_FIFO_PORT_s *Port, UINT16 Update, UINT32 *Index)
{
    UINT32 Ret = IMU_ERR_MANAGER_OK;
    UINT32 misra_u32;
    /* check port */

    if (Port->Magic != 0xbeefU) {
        ImuMgr_Log_DBG(LOG_IMUMGR, "Open FiFo Port...", 0U, 0U, 0U, 0U, 0U);
        Ret = IMU_ERR_MANAGER_FIFO_ERR;
    }
    else{
        (void)AmbaKAL_MutexTake(&AmbaImg_ImuManager_Mutex[Port->ImuId], AMBA_KAL_WAIT_FOREVER);
        if (Update != 0U) {
            misra_u32 = (1U << IMU_READ_BY_FIFO);
            if ((AmbaImg_ImuDeviceInfo[Port->ImuId].Capability & misra_u32) != 0U){
                (void)AmbaImg_ImuMgr_ImuInfoGet(Port->ImuId, (UINT32)AMBA_IMUMGR_READ_FIFO);
            }
        }
        *Index = AmbaImg_ImuManager_Buf[Port->ImuId].Index.Wr;
        (void)AmbaKAL_MutexGive(&AmbaImg_ImuManager_Mutex[Port->ImuId]);
    }
    return Ret;
}

UINT32 AmbaImg_ImuMgr_FiFoDataGet(AMBA_IMG_IMUMGR_FIFO_PORT_s *Port, UINT32 Size, AMBA_IMG_IMUMGR_FIFO_INFO_s *FiFo)
{

    UINT32 ReadSize = 0U;
    UINT32 WrIndex;
    UINT32 FifoSize;
    UINT32 SizeTemp;
    UINT32 FiFoDepth = AmbaImg_ImuManager_Config[Port->ImuId].FiFoDepth;
    
    UINT32 TimeTick;


    SizeTemp = Size;

    /* check port */
    if (Port->Magic != 0xbeefU) {
        ImuMgr_Log_NG(LOG_IMUMGR, "FiFo Port error...", 0U, 0U, 0U, 0U, 0U);
    }
    else
    {
        /* get wr index */
        WrIndex = AmbaImg_ImuManager_Buf[Port->ImuId].Index.Wr;

        /* get imumgr fifo */
        if (WrIndex != Port->ReadIndex) {

            (void)AmbaKAL_MutexTake(&AmbaImg_ImuManager_Mutex[Port->ImuId], AMBA_KAL_WAIT_FOREVER);

            /* check available size */
            if (WrIndex > Port->ReadIndex) {
                FifoSize = WrIndex - Port->ReadIndex;
            } else {
                FifoSize = FiFoDepth - Port->ReadIndex + WrIndex;
            }

            /* read size */
            if (SizeTemp == 0U) {
                SizeTemp = FifoSize;
            }


            if (SizeTemp <= FifoSize) {
                ReadSize = SizeTemp;
            }
            else {
                ReadSize = FifoSize;
            }

            /* copy imumgr fifo */
            if ((Port->ReadIndex + ReadSize) < FiFoDepth) {
                if(AmbaWrap_memcpy(FiFo, &(AmbaImg_ImuManager_Buf[Port->ImuId].Info[Port->ReadIndex]), ReadSize * sizeof(AMBA_IMG_IMUMGR_FIFO_INFO_s)) != 0U) {
                    ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memcpy AmbaImg_ImuManager_Buf < FiFoDepth error ", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                UINT32 Num = FiFoDepth - Port->ReadIndex;
                if(AmbaWrap_memcpy(FiFo, &(AmbaImg_ImuManager_Buf[Port->ImuId].Info[Port->ReadIndex]), Num * sizeof(AMBA_IMG_IMUMGR_FIFO_INFO_s)) != 0U) {
                    ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memcpy AmbaImg_ImuManager_Buf > FiFoDepth (1) error", 0U, 0U, 0U, 0U, 0U);
                }
                if(((ReadSize-Num) != 0U)&&(AmbaWrap_memcpy(&(FiFo[Num]) , &(AmbaImg_ImuManager_Buf[Port->ImuId].Info[0]), (ReadSize * sizeof(AMBA_IMG_IMUMGR_FIFO_INFO_s)) - (Num * sizeof(AMBA_IMG_IMUMGR_FIFO_INFO_s))) != 0U)) {
                    ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memcpy AmbaImg_ImuManager_Buf > FiFoDepth (2) error", 0U, 0U, 0U, 0U, 0U);

                }
            }

            /* update pos index */
            Port->ReadIndex = (Port->ReadIndex + ReadSize) % FiFoDepth;
            (void)AmbaKAL_GetSysTickCount(&TimeTick);
            Port->SystemTimeTick = TimeTick;
            (void)AmbaKAL_MutexGive(&AmbaImg_ImuManager_Mutex[Port->ImuId]);


        }
        else {

            if (SizeTemp == 0U) {
                
            }
            else {
                ImuMgr_Log_NG(LOG_IMUMGR, "Fifo Rd/Wr Index Conflict...", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }


    return ReadSize;
}


UINT32 AmbaImg_ImuMgr_FiFoStatusGet(const AMBA_IMG_IMUMGR_FIFO_PORT_s *Port, AMBA_IMG_IMUMGR_FIFO_STATUS_s *Status)
{

    AMBA_IMU_STATUS_s DeviceStatus;
    AMBA_IMU_CHANNEL_s IMUChan = {0};
    UINT32 Ret = IMU_ERR_MANAGER_OK;
    FLOAT misra_f;
    UINT32 misra_u32 = 0U;

    /* check port */
    if (Port->Magic != 0xbeefU) {
        ImuMgr_Log_DBG(LOG_IMUMGR, "FiFo Port Not Opened Yet...", 0U, 0U, 0U, 0U, 0U);
        Ret = IMU_ERR_MANAGER_FIFO_ERR;
    }
    else{
        IMUChan.VinID = (UINT8)AMBA_IMU_SENSOR0 + Port->ImuId;
        (void)AmbaKAL_MutexTake(&AmbaImg_ImuManager_Mutex[Port->ImuId], AMBA_KAL_WAIT_FOREVER);
        /* copy imumgr fifo related config and info */
        Status->FiFoDepth = AmbaImg_ImuManager_Config[Port->ImuId].FiFoDepth;
        Status->SamplingRate = AmbaImg_ImuDeviceInfo[Port->ImuId].SamplingRate;
        misra_u32 = AmbaImg_ImuDeviceInfo[Port->ImuId].FiFoEnable.Channels;
        Status->SampleByteSize = misra_u32 << (UINT32) 1U;
        Status->FiFoEnable = AmbaImg_ImuDeviceInfo[Port->ImuId].FiFoEnable;
        Status->FiFoCurrentIndex = AmbaImg_ImuManager_Buf[Port->ImuId].Index.Wr;
        /* get device overflow status */
        (void)AmbaIMU_GetStatus((UINT8)IMUChan.VinID, &DeviceStatus);
        Status->ImuStatus.FiFoOverFlow = DeviceStatus.FiFoOverFlow;

        {
            /* check potential overflow in imumgr sw fifo */
            UINT32 CurrTimeTick;
            UINT32 LastReadTimeTick = Port->SystemTimeTick;
            (void)AmbaKAL_GetSysTickCount(&CurrTimeTick);

            Status->FiFoStatus = 0U;

            misra_f =(FLOAT)Status->FiFoDepth / ((FLOAT)Status->SamplingRate / 1000.0f);
            misra_u32 = (UINT32)misra_f;
            if ((CurrTimeTick - LastReadTimeTick) > misra_u32){
                Status->FiFoStatus |= 0x1U;     
                /* if current time is further than last read time by fifo_buffer_available_ms, report overflow status*/
            }

            misra_f = (((FLOAT)Status->FiFoDepth / ((FLOAT)Status->SamplingRate / 1000.0f)) - ((FLOAT)AMBA_IMU_TIMER_INVTERVAL * 2.0f / 1000.0f));
            misra_u32 = (UINT32)misra_f;
            if ((CurrTimeTick - LastReadTimeTick) > (UINT32)misra_u32){
                Status->FiFoStatus |= 0x2U;     
                /* if current time is further than last read time by (fifo_buffer_available_ms - 2 * fifo_autonomous_read_ms), report potential overflow status*/
            }
            ImuMgr_Log_DBG(LOG_IMUMGR, "Status = %d %d (%d %d %d)", Status->FiFoStatus, Status->ImuStatus.FiFoOverFlow,
                                                                        Status->FiFoDepth, Status->SamplingRate, Status->SampleByteSize);
        }
        (void)AmbaKAL_MutexGive(&AmbaImg_ImuManager_Mutex[Port->ImuId]);
    }

    return Ret;
}


UINT32 AmbaImg_ImuMgr_SofEventHandler(const void *hdlr, const UINT32 *pEventData)
{
    AMBA_DSP_VIN_SYNC_INFO_s HdlrInfo = {.VinIndex = 0U, .SyncTiming = 0U}; // ADDED
    static UINT32 AmbaImg_ImuManager_LastCapSeqNo;
    UINT32 VinImuId;
    const AMBA_IMG_IMUMGR_EVENT_DATA_s *TimerData = NULL;


    if(AmbaWrap_memcpy(&TimerData, &pEventData, sizeof(void *)) != 0U) {
        ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memcpy Sof TimerData  error", 0U, 0U, 0U, 0U, 0U);
    }

    if(hdlr != NULL){
        ImuMgr_Log_NG(LOG_IMUMGR, "Sof Hdlr Null", 0U, 0U, 0U, 0U, 0U);
    }

    if(pEventData == NULL){
        ImuMgr_Log_NG(LOG_IMUMGR, "Sof EventData Null", 0U, 0U, 0U, 0U, 0U);
    }

    VinImuId = AmbaImg_ImuManager_VinImuTable[TimerData->VinId];
    AmbaImg_ImuManager_LastCapSeqNo = (UINT32)TimerData->CapSeqNo;
    AmbaImg_ImuManager_LastSofTimetick = TimerData->SofSystemTime;
    AmbaImg_ImuManager_LastFrameTime = TimerData->FrameTime;
    AmbaImg_ImuManager_LastShutterTime = TimerData->ExposureTime[0];
    AmbaImg_ImuManager_LastReadoutTime = TimerData->ReadoutTime;

    if ((ImuMgrDebugFlag & 16U) > 0U)
    {
        ImuMgr_Log_DBG(LOG_IMUMGR, "ImuMgr_SofEventHandler: %d", AmbaImg_ImuManager_LastSofTimetick, 0U, 0U, 0U, 0U);
    }

    if ((ImuMgrDebugFlag & 32U) > 0U)
    {   
        ImuMgr_Log_DBG(LOG_IMUMGR, "[%d]", AmbaImg_ImuManager_LastCapSeqNo, 0U, 0U, 0U, 0U);
        ImuMgr_Log_DBG(LOG_IMUMGR, "Sof/FrameTime/Exp/RO %d", 
                        AmbaImg_ImuManager_LastSofTimetick, 
                        AmbaImg_ImuManager_LastFrameTime, 
                        (UINT32)AmbaImg_ImuManager_LastShutterTime, 
                        (UINT32)AmbaImg_ImuManager_LastReadoutTime, 0U);
        
    }


#ifdef AMBA_IMUMGR_SENSOR_TIME
    AmbaIMU_GetSensorTimeStamp((UINT8)AMBA_IMU_SENSOR0 + VinImuId, &AmbaImg_ImuManager_LastSensorTimeStampInSof);
#endif

    (void)AmbaKAL_EventFlagSet(&AmbaImg_ImuManager_Event[VinImuId], (UINT32)AMBA_IMU_EVENT_FLAG_SOF);

    /* PWM Start */
    if (((AmbaImg_ImuManager_Config[VinImuId].Config.EnablePwmSync > 0U) && (AmbaImg_ImuDeviceInfo[VinImuId].PwmSyncGpio > 0U)))
    {
        HdlrInfo.SyncTiming = 0U;
        (void)AmbaImg_ImuMgr_VinIsrHandler(&HdlrInfo, VinImuId);
    }

    return OK;
}

UINT32 AmbaImg_ImuMgr_EofEventHandler(const void *hdlr, const UINT32 *pEventData)
{

    AMBA_DSP_VIN_SYNC_INFO_s HdlrInfo = {.VinIndex = 0U, .SyncTiming = 0U}; // ADDED
    UINT32 VinImuId;                                                        //ADDED
    const AMBA_IMG_IMUMGR_EVENT_DATA_s *TimerData = NULL;


    if (AmbaWrap_memcpy(&TimerData, &pEventData, sizeof(void *)) != 0U)
    {
        ImuMgr_Log_NG(LOG_IMUMGR, "AmbaWrap_memcpy Eof TimerData  error", 0U, 0U, 0U, 0U, 0U);
    }
    if(hdlr != NULL){
        ImuMgr_Log_NG(LOG_IMUMGR, "Eof Hdlr Null", 0U, 0U, 0U, 0U, 0U);
    }
    
    if(pEventData == NULL){
        ImuMgr_Log_NG(LOG_IMUMGR, "Eof EventData Null", 0U, 0U, 0U, 0U, 0U);
    }

    VinImuId = AmbaImg_ImuManager_VinImuTable[TimerData->VinId];
    AmbaImg_ImuManager_LastEofTimetick = TimerData->EofSystemTime;

    if ((ImuMgrDebugFlag & 16U) > 0U)
    {
        ImuMgr_Log_DBG(LOG_IMUMGR, "ImuMgr_EofEventHandler: %d", AmbaImg_ImuManager_LastEofTimetick, 0U, 0U, 0U, 0U);
    }

    if ((ImuMgrDebugFlag & 32U) > 0U)
    {
        ImuMgr_Log_DBG(LOG_IMUMGR, "Eof Time: %d", AmbaImg_ImuManager_LastEofTimetick, 0U, 0U, 0U, 0U);
    }

    /* PWM Start */
    if (((AmbaImg_ImuManager_Config[VinImuId].Config.EnablePwmSync > 0U) && (AmbaImg_ImuDeviceInfo[VinImuId].PwmSyncGpio > 0U)))
    {
        HdlrInfo.SyncTiming = 1U;
        (void)AmbaImg_ImuMgr_VinIsrHandler(&HdlrInfo, VinImuId);
    }

    return OK;
}

UINT32 AmbaImg_ImuMgr_MofCallBackFuncReg(AMBA_IMG_IMUMGR_MOF_CB_FUNC_f CallBackFunc)
{

    ImuMgrMofCb = CallBackFunc;
    ImuMgr_Log_DBG(LOG_IMUMGR, "MofCallBack", 0U, 0U, 0U, 0U, 0U);
    

    return OK;
}

void AmbaImg_ImuMgr_TestCmd(UINT32 ArgCount,char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc){

        char IntString[UTIL_MAX_INT_STR_LEN];
        UINT32 Arg1 = 0;
        UINT32 Arg2 = 0;
        UINT32 Arg3 = 0;
        (void)AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, ArgCount, 10);
        PrintFunc(IntString);

        (void)ArgCount;    

        (void)AmbaUtility_StringToUInt32(pArgVector[2], &Arg1);
        (void)AmbaUtility_StringToUInt32(pArgVector[3], &Arg2);
        (void)AmbaUtility_StringToUInt32(pArgVector[4], &Arg3);
    
        if (AmbaUtility_StringCompare(pArgVector[1], "dbg", 3U) == 0)
        {
            ImuMgrDebugFlag = Arg1;
            /*  
                1: gyro data
                2: time flow 
                4: 
            */
        }
        else if (AmbaUtility_StringCompare(pArgVector[1], "imu", 3U) == 0)
        {
            ImuMgrDebugFlag = Arg1;
            /*  
                1: gyro data
                2: time flow 
                4: 
            */
        }
        else{
            ImuMgr_Log_DBG(LOG_IMUMGR, "Invalid cmd", 0U, 0U, 0U, 0U, 0U);
        }


    return;

}

