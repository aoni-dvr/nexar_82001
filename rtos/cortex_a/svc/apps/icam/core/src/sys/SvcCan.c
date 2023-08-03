/**
 *  @file SvcCan.c
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
 *  @details CAN Test Command
 *
 */

#define CAN_DEBUG         0U
#define FIXED_CANSPEED    0U//Please hack CAN speed here.

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaCAN.h"
#include "AmbaSYS.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaGPIO.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcCan.h"

#define SvcCanMaxBufSize      4096U
//#define SvcCanFDMaxBufSize    4096U
#define SvcCanMaxFilterSize   16U
#define SvcCANIdMask          0x7FF
//#define TRUE                  (UINT32)1U

#define CAN_BRAKE           0x288U
#define CAN_TURNING_LIGHT   0x390U
#define CAN_GEAR            0x540U
#define CAN_WHEEL           0xC2U
#define CAN_ENGINE          0x280U
#define CAN_SPEED           0x5A0U

extern UINT32 DummyFlag;
UINT32 DummyFlag = 1U;

#if defined(CONFIG_BUILD_AMBA_ADAS) || defined(CONFIG_BUILD_AMBA_AVM) || defined(CONFIG_ICAM_CANBUS_USED)
extern AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData;
AMBA_SR_CANBUS_RAW_DATA_s CanbusRawData = { 0 };// for adas
#endif

static AMBA_KAL_MUTEX_t SvcCan_Mutex GNU_SECTION_NOZEROINIT;

#if CAN_DEBUG
static UINT8 Svc_CanDlcTable[AMBA_NUM_CAN_DATA_LENGTH] = {
    0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U
};
#endif

typedef struct {
    UINT32  ShellCanChannelId;
    AMBA_CAN_CONFIG_s   ShellCanConfig;
} AMBA_SHELL_CAN_CTRL_s;

/* Common CANC and CAN 2.0 configurations */
static AMBA_SHELL_CAN_CTRL_s SvcCanCtrl = {
    .ShellCanChannelId = AMBA_CAN_CHANNEL0,
    .ShellCanConfig = { /* Common CANC and CAN 2.0 configurations */
        .OpMode             = 0,        /* 0: Normal mode; 1: Inside-loopback mode; 2: Outside-loopback mode; 3: Listen Mode */

        .TimeQuanta = {                 /* Bit-rate = RefClock / (( 1 + (PropSeg + PhaseSeg1 + 1) + (PhaseSeg2 + 1)) * (Prescaler + 1)) */
            .PhaseSeg2      = 1,        /* E.g., To enable 1M bps transfer, it might set PropSeg = 0, Seg2 = 1, Seg1 = 4, SJW = 0, and  */
            .PhaseSeg1      = 12,       /*       BRP = 2. Then, the bit-rate is 24M/((1 + 2 + 5) * 3) = 1M bps.      */
            .SJW            = 0,
            .BRP            = 2,
            .PropSeg        = 0,
        },

        .MaxRxRingBufSize   = 0,        /* maximum receive ring-buffer size in message number */
        .pRxRingBuf         = NULL,     /* Pointer to the Rx ring buffer */

        .FdTimeQuanta       = {
            .PhaseSeg2      = 1,        /* E.g., To enable 1M bps transfer, it might set PropSeg = 0, Seg2 = 1, Seg1 = 4, SJW = 0, and  */
            .PhaseSeg1      = 12,       /*       BRP = 2. Then, the bit-rate is 24M/((1 + 2 + 5) * 3) = 1M bps.      */
            .SJW            = 0,
            .BRP            = 2,
            .PropSeg        = 0,
        },

        .FdMaxRxRingBufSize = 0,        /* maximum receive ring-buffer size in message number for CAN FD */
        .pFdRxRingBuf       = NULL,     /* Pointer to the Rx ring buffer for CAN FD */

        .NumIdFilter        = 0,        /* Number of ID filters */
        .pIdFilter          = NULL,     /* Pointer to the ID filters */
    },
};

static void SvcCan_PrintInt32(const char *pFmtString,
                                UINT32 Count,
                                UINT32 Arg1,
                                UINT32 Arg2,
                                UINT32 Arg3,
                                UINT32 Arg4,
                                UINT32 Arg5)
{
    char StrBuf[256];
    UINT32 Arg[5];

    Arg[0] = Arg1;
    Arg[1] = Arg2;
    Arg[2] = Arg3;
    Arg[3] = Arg4;
    Arg[4] = Arg5;

    (void)AmbaUtility_StringPrintUInt32(StrBuf, (UINT32)sizeof(StrBuf), pFmtString, Count, Arg);
    AmbaPrint_PrintUInt5(&StrBuf[0U], 0U, 0U, 0U, 0U, 0U);
}

#if defined(CONFIG_BUILD_AMBA_ADAS) || defined(CONFIG_BUILD_AMBA_AVM) || defined(CONFIG_ICAM_CANBUS_USED)
static void SvcCan_DataProcessLight(const AMBA_CAN_MSG_s* CanMsg) {
    UINT8 Level = CanMsg->Data[4] & 0x0CU;
    if (Level == 0x4U) {
        CanbusRawData.CANBusTurnLightStatus = AMBA_SR_CANBUS_TURN_LIGHT_L;
    } else if (Level == 0x8U) {
        CanbusRawData.CANBusTurnLightStatus = AMBA_SR_CANBUS_TURN_LIGHT_R;
    } else {
        CanbusRawData.CANBusTurnLightStatus = AMBA_SR_CANBUS_TURN_LIGHT_N;
    }
#if CAN_DEBUG
    SvcCan_PrintInt32("CAN_TURNING_LIGHT %d", 1, CanbusRawData.CANBusTurnLightStatus, 0, 0, 0, 0);
#endif
}

static void SvcCan_DataProcessGear(const AMBA_CAN_MSG_s* CanMsg) {
    UINT8 Flag = CanMsg->Data[0] & 0xF0U;
    UINT8 Level = CanMsg->Data[7] & 0xF0U;
    if (Flag == 0U) {
        if (Level == 0x20U) {
            CanbusRawData.CANBusGearStatus =  AMBA_SR_CANBUS_GEAR_P;
        } else if (Level == 0x30U) {
            CanbusRawData.CANBusGearStatus =  AMBA_SR_CANBUS_GEAR_R;
        } else if (Level == 0x40U) {
            CanbusRawData.CANBusGearStatus =  AMBA_SR_CANBUS_GEAR_N;
        } else if (Level == 0x50U) {
            CanbusRawData.CANBusGearStatus =  AMBA_SR_CANBUS_GEAR_D;
        } else {
            CanbusRawData.CANBusGearStatus =  AMBA_SR_CANBUS_GEAR_UNDEF;
        }
    }
#if CAN_DEBUG
SvcCan_PrintInt32("CAN_GEAR %d", 1, CanbusRawData.CANBusGearStatus, 0, 0, 0, 0);
#endif
}

static void SvcCan_DataProcessWheel(const AMBA_CAN_MSG_s* CanMsg) {
    CanbusRawData.WheelDir =  (UINT32)CanMsg->Data[1] >> 7U; /* 0: Left, 1: Right */
    CanbusRawData.RawWheelAngle = (((UINT32) CanMsg->Data[1] & 0x7FU) << 8U) + CanMsg->Data[0];
    CanbusRawData.RawWheelAngleSpeed = (((UINT32) CanMsg->Data[3] & 0x7FU) << 8U) + CanMsg->Data[2];
#if CAN_DEBUG
    SvcCan_PrintInt32("WheelDir %d", 1, CanbusRawData.WheelDir, 0, 0, 0, 0);
    SvcCan_PrintInt32("RawWheelAngle %d", 1, CanbusRawData.RawWheelAngle, 0, 0, 0, 0);
    SvcCan_PrintInt32("RawWheelAngleSpeed %d", 1, CanbusRawData.RawWheelAngleSpeed, 0, 0, 0, 0);
#endif
}

static void SvcCan_DataProcessSpeed(const AMBA_CAN_MSG_s* CanMsg) {
    UINT32 Speed = ((UINT32) CanMsg->Data[2] << 8U) + CanMsg->Data[1];
    if (Speed == 0xFF00U) { // tiguan Gear_P's speed
        Speed = 0U;
    }
    CanbusRawData.RawSpeed =Speed;
#if CAN_DEBUG
    SvcCan_PrintInt32("CAN_SPEED %d", 1, CanbusRawData.RawSpeed, 0, 0, 0, 0);
#endif
}

static void SvcCan_MsgProcess(const AMBA_CAN_MSG_s* CanMsg)
{
    UINT32  Time;

    if (AmbaSYS_GetOrcTimer(&Time) == SYS_ERR_NONE) {
        CanbusRawData.CapTS = Time;
        CanbusRawData.FlagValidRawData = 1U;
    }

    switch(CanMsg->Id) {
        case CAN_TURNING_LIGHT:
            SvcCan_DataProcessLight(CanMsg);
            break;
        case CAN_GEAR:
            SvcCan_DataProcessGear(CanMsg);
            break;
        case CAN_WHEEL:
            SvcCan_DataProcessWheel(CanMsg);
            break;
        case CAN_SPEED:
            SvcCan_DataProcessSpeed(CanMsg);
            break;
        case CAN_BRAKE:
        case CAN_ENGINE:
        default:
            // pass vcast
            break;
    }
}
#endif

static void* SvcCan_ReadTaskEntry(void* EntryArg)
{
    AMBA_CAN_MSG_s CanMessage;
    UINT32 RetStatus;
    AMBA_SHELL_PRINT_f PrintFunc;
    UINT32 RetVal;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&PrintFunc, &EntryArg);

    while (DummyFlag == 1U) {
        /* Get one message from Rx Ring Buffer */
        RetStatus = AmbaCAN_Read(SvcCanCtrl.ShellCanChannelId, &CanMessage, AMBA_KAL_WAIT_FOREVER);
        RetVal = AmbaKAL_MutexTake(&SvcCan_Mutex, AMBA_KAL_WAIT_FOREVER);
        if(RetVal != SVC_OK) {
            SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
        }
        if (RetStatus == CAN_ERR_NONE) {
            /* Show the CAN messgae */
            #if CAN_DEBUG
            SvcCan_PrintInt32("CH: %d, ID: 0x%x, EXT: %d, RTR: %d, DLC: %d, Data: ", 5,
                                 SvcCanCtrl.ShellCanChannelId,
                                 CanMessage.Id,
                                 CanMessage.Extension,
                                 CanMessage.RemoteTxReq,
                                 CanMessage.DataLengthCode);

            for (UINT32 i = 0; i < Svc_CanDlcTable[CanMessage.DataLengthCode]; i++) {
                SvcCan_PrintInt32("%x ", 1, CanMessage.Data[i], 0, 0, 0, 0);
            }
            #endif
            #if defined(CONFIG_BUILD_AMBA_ADAS) || defined(CONFIG_BUILD_AMBA_AVM) || defined(CONFIG_ICAM_CANBUS_USED)
            SvcCan_MsgProcess(&CanMessage);
            #endif
        } else {
            SvcCan_PrintInt32("Read fail (%d)\n", 1, RetStatus, 0, 0, 0, 0);
            break;
        }
        RetVal = AmbaKAL_MutexGive(&SvcCan_Mutex);
        if(RetVal != SVC_OK) {
            SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
        }
    }

    return NULL;
}

#if 0
static void* SvcCan_FdReadTaskEntry(void* EntryArg)
{
    AMBA_CAN_FD_MSG_s CanMessage;
    UINT32 RetStatus;
    AMBA_SHELL_PRINT_f PrintFunc;
    UINT32 RetVal;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&PrintFunc, &EntryArg);

    while (DummyFlag == 1U) {
        /* Get one message from Rx Ring Buffer */
        RetStatus = AmbaCAN_ReadFd(SvcCanCtrl.ShellCanChannelId, &CanMessage, AMBA_KAL_WAIT_FOREVER);
        RetVal = AmbaKAL_MutexTake(&SvcCan_Mutex, AMBA_KAL_WAIT_FOREVER);
        if(RetVal != SVC_OK) {
            SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
        }
        if (RetStatus == CAN_ERR_NONE) {
            #if CAN_DEBUG
            /* Show the CAN FD messgae */
            SvcCan_PrintInt32("EXT: %d, ESI: %d, BRS: %d, DLC: %d, Data: ", 4,
                                CanMessage.Extension,
                                CanMessage.ErrorStateIndicator,
                                CanMessage.BitRateSwitch,
                                Svc_CanDlcTable[CanMessage.DataLengthCode],
                                0);

            for (UINT32 i = 0; i < Svc_CanDlcTable[CanMessage.DataLengthCode]; i++) {
                SvcCan_PrintInt32("%x ", 1, CanMessage.Data[i], 0, 0, 0, 0);
            }
            #endif
        } else {
            SvcCan_PrintInt32("Read FD fail (%d)\n", 1, RetStatus, 0, 0, 0, 0);
            break;
        }
        RetVal = AmbaKAL_MutexGive(&SvcCan_Mutex);
        if(RetVal != SVC_OK) {
            SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
        }
    }

    return NULL;
}
#endif

static UINT32 SvcCan_Enable(void)
{
    static UINT8 SvcCanRingBuf[SvcCanMaxBufSize];
    //static UINT8 SvcCanFDRingBuf[SvcCanFDMaxBufSize];
    AMBA_CAN_CONFIG_s *CanConfig = &SvcCanCtrl.ShellCanConfig;

    AmbaSvcWrap_MisraMemset(SvcCanRingBuf, 0, sizeof(SvcCanRingBuf));
    //AmbaSvcWrap_MisraMemset(SvcCanFDRingBuf, 0, sizeof(SvcCanFDRingBuf));

    /* Hoko the message ring buffer. */

    CanConfig->MaxRxRingBufSize = SvcCanMaxBufSize;
    CanConfig->pRxRingBuf = SvcCanRingBuf;

    //CanConfig->FdMaxRxRingBufSize = SvcCanFDMaxBufSize;
    //CanConfig->pFdRxRingBuf = SvcCanFDRingBuf;

    return AmbaCAN_Enable(SvcCanCtrl.ShellCanChannelId, CanConfig);
}

static UINT32 SvcCan_Read(AMBA_SHELL_PRINT_f PrintFunc)
{
    static AMBA_KAL_TASK_t SvcCanRecvTask GNU_SECTION_NOZEROINIT;
    //static AMBA_KAL_TASK_t SvcCanFdRecvTask GNU_SECTION_NOZEROINIT;
    static UINT8 SvcCanRecvTaskStack[2048] GNU_SECTION_NOZEROINIT;
    //static UINT8 SvcCanFdRecvTaskStack[2048] GNU_SECTION_NOZEROINIT;
    UINT32 Retstatus;
    static char CanName[32];
    //static char CanFdName[32];
    const void* pvArg;

    AmbaMisra_TypeCast(&pvArg, &PrintFunc);

    AmbaUtility_StringCopy(CanName, sizeof(CanName), "ShellCanRecvTask");
    Retstatus = AmbaKAL_TaskCreate(&SvcCanRecvTask,     /* pTask */
                   CanName,                             /* pTaskName */
                   249,                                 /* Priority */
                   SvcCan_ReadTaskEntry,                /* EntryFunction */
                   pvArg,                               /* EntryArg */
                   SvcCanRecvTaskStack,                 /* pStackBase */
                   (UINT32)sizeof(SvcCanRecvTaskStack), /* StackByteSize */
                   1);                                  /* AutoStart */
    if (Retstatus != KAL_ERR_NONE) {
    }

    #if 0
    AmbaUtility_StringCopy(CanFdName, sizeof(CanFdName), "ShellCanFdRecvTask");
    Retstatus = AmbaKAL_TaskCreate(&SvcCanFdRecvTask,       /* pTask */
                   CanFdName,                               /* pTaskName */
                   249,                                     /* Priority */
                   SvcCan_FdReadTaskEntry,                  /* EntryFunction */
                   pvArg,                                   /* EntryArg */
                   SvcCanFdRecvTaskStack,                   /* pStackBase */
                   (UINT32)sizeof(SvcCanFdRecvTaskStack),   /* StackByteSize */
                   1);                                      /* AutoStart */
    if (Retstatus != KAL_ERR_NONE) {
    }
    #endif

    return Retstatus;
}

static void SvcCan_AddIdFilter(UINT32 Id, UINT32 IdMask, AMBA_SHELL_PRINT_f PrintFunc)
{
    static AMBA_CAN_FILTER_s SvcCanFilter[SvcCanMaxFilterSize] = {0U};
    AMBA_CAN_CONFIG_s *CanConfig = &SvcCanCtrl.ShellCanConfig;

    CanConfig->pIdFilter = SvcCanFilter;

    if(PrintFunc == NULL){}

    if (CanConfig->NumIdFilter < SvcCanMaxFilterSize) {
        CanConfig->pIdFilter[CanConfig->NumIdFilter].IdFilter = Id;
        CanConfig->pIdFilter[CanConfig->NumIdFilter].IdMask = IdMask;
        CanConfig->NumIdFilter++;
    }
}

#if defined(CONFIG_BUILD_AMBA_ADAS) || defined(CONFIG_BUILD_AMBA_AVM) || defined(CONFIG_ICAM_CANBUS_USED)
#if 0
static SVC_CAN_XFER_PARAM_s  g_XferParams = {
    /* default is Tiguan parameters */
    .SpeedA      = 0.0,
    .SpeedB      = 0.0075 * 0.983,
    .SpeedC      = 0.0,
    .WheelA      = 0.0,
    .WheelB      = 0.0000479540201,
    .WheelC      = -0.0033139086,
    .AngleSpeedA = 0.0,
    .AngleSpeedB = 0.0,
    .AngleSpeedC = 0.0,
};
#else
static SVC_CAN_XFER_PARAM_s g_XferParams = {
    // speed in [km/hr]
    .SpeedA      = -9.34565329e-08,
    .SpeedB      = 8.84089679e-03,
    .SpeedC      = 3.08516383e-02,
    // wheel in [rad]
    .WheelA      = 9.51664634e-10,
    .WheelB      = 3.55033595e-05,
    .WheelC      = 3.47167908e-04,
    .AngleSpeedA = 0.0,
    .AngleSpeedB = 0.0,
    .AngleSpeedC = 0.0,
};
#endif

static DOUBLE transRawSpeed (UINT32 RawSpeed, const SVC_CAN_XFER_PARAM_s *Param)
{
    DOUBLE Rval;
    Rval = (((Param->SpeedA) * ((DOUBLE)(RawSpeed) * (DOUBLE)(RawSpeed))) + ((Param->SpeedB) * (DOUBLE)(RawSpeed))) + (Param->SpeedC);
    return Rval;
}

static DOUBLE transRawWheelAngle(UINT32 RawWheelAngle, const SVC_CAN_XFER_PARAM_s *Param)
{
    DOUBLE Rval;
    Rval = (((Param->WheelA) * ((DOUBLE)(RawWheelAngle) * (DOUBLE)(RawWheelAngle))) + ((Param->WheelB) * (DOUBLE)(RawWheelAngle))) + (Param->WheelC);
    return Rval;
}

static DOUBLE transRawWheelAngleSpeed(UINT32 RawWheelAngleSpeed, const SVC_CAN_XFER_PARAM_s *Param)
{
    DOUBLE Rval;
    Rval = (((Param->AngleSpeedA) * ((DOUBLE)(RawWheelAngleSpeed) * (DOUBLE)(RawWheelAngleSpeed))) + ((Param->AngleSpeedB) * (DOUBLE)(RawWheelAngleSpeed))) + (Param->AngleSpeedC);
    return Rval;
}

/**
* conversion can data to yaw rate
* @param [in] canTrans  transfered can data
* @param [in] wheelDir  wheel direction in can raw data
* @param [in] fps       frame rate per second
* @param [in] wheelBase distance between front and rare wheel [m]
* @param [out] yawRate  yaw speed in [rad/s]
* @return 0-OK, 1-NG
*/
UINT32 SvcCan_transYawRate(const AMBA_SR_CANBUS_TRANSFER_DATA_s *canTrans, const UINT32 wheelDir, const DOUBLE fps, const DOUBLE wheelBase, DOUBLE *yawRate)
{
    UINT32 Rval = SVC_OK;
    DOUBLE dt = 1.0 / fps;
    DOUBLE displacement = (canTrans->TransferSpeed * (1000.0/3600.0)) * dt; // s = v * t in [m]
    DOUBLE tanAngle = 0.0;

    Rval = AmbaWrap_tan(canTrans->TransferWheelAngle, &tanAngle);

    if(Rval == SVC_OK) {
        *yawRate = ((displacement / wheelBase) * tanAngle) / dt;
        *yawRate *= (wheelDir==1U)?(-1.0):(1.0);
    }

    return Rval;
}

/**
* initialization of canbus module
* @return none
*/
void SvcCan_Init(void)
{
    UINT32 RetVal;
    static char SvcCan_MutexName[] = "SvcCan_MutexName";

    RetVal = AmbaKAL_MutexCreate(&SvcCan_Mutex, SvcCan_MutexName);
    if(RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
    }

    SvcCan_AddIdFilter(CAN_BRAKE, SvcCANIdMask, NULL);
    SvcCan_AddIdFilter(CAN_TURNING_LIGHT, SvcCANIdMask, NULL);
    SvcCan_AddIdFilter(CAN_GEAR, SvcCANIdMask, NULL);
    SvcCan_AddIdFilter(CAN_WHEEL, SvcCANIdMask, NULL);
    SvcCan_AddIdFilter(CAN_ENGINE, SvcCANIdMask, NULL);
    SvcCan_AddIdFilter(CAN_SPEED, SvcCANIdMask, NULL);

#if defined(CONFIG_SOC_CV2)
    RetVal  = AmbaGPIO_SetFuncAlt(GPIO_PIN_158_CAN0_RX);
    RetVal |= AmbaGPIO_SetFuncAlt(GPIO_PIN_159_CAN1_RX);
    RetVal |= AmbaGPIO_SetFuncAlt(GPIO_PIN_160_CAN0_TX);
    RetVal |= AmbaGPIO_SetFuncAlt(GPIO_PIN_161_CAN1_TX);
    if(RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d CONFIG_SOC_CV2 GPIO_SetFuncAlt fail", __LINE__, 0U);
    }
#elif defined(CONFIG_SOC_CV22)
    RetVal  = AmbaGPIO_SetFuncAlt(GPIO_PIN_154_CAN0_RX);
    RetVal |= AmbaGPIO_SetFuncAlt(GPIO_PIN_155_CAN0_TX);
    if(RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d CONFIG_SOC_CV22 GPIO_SetFuncAlt fail", __LINE__, 0U);
    }
#elif defined(CONFIG_SOC_CV25)
    RetVal  = AmbaGPIO_SetFuncAlt(GPIO_PIN_95_CAN0_RX);
    RetVal |= AmbaGPIO_SetFuncAlt(GPIO_PIN_96_CAN0_TX);
    if(RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d CONFIG_SOC_CV25 GPIO_SetFuncAlt fail", __LINE__, 0U);
    }
#elif defined(CONFIG_SOC_CV28)
    RetVal  = AmbaGPIO_SetFuncAlt(GPIO_PIN_81_CAN_RX);
    RetVal |= AmbaGPIO_SetFuncAlt(GPIO_PIN_82_CAN_TX);
    if(RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d CONFIG_SOC_CV28 GPIO_SetFuncAlt fail", __LINE__, 0U);
    }
#endif

    RetVal = SvcCan_Enable();
    if(RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
    }
    RetVal = SvcCan_Read(NULL);
    if(RetVal != SVC_OK) {
        SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
    }

    #if FIXED_CANSPEED
    {
        DOUBLE CanSpeed;

        CanSpeed = (DOUBLE)FIXED_CANSPEED /(0.0073725);
        CanbusRawData.RawSpeed = (UINT32)CanSpeed + 1U;
        CanbusRawData.FlagValidRawData = 1U;
    }
    #endif
}

/**
* get raw data from canbus
* @param [out] pCanbusRawData raw data
* @return none
*/
void SvcCan_GetRawData(AMBA_SR_CANBUS_RAW_DATA_s *pCanbusRawData)
{
    AmbaSvcWrap_MisraMemcpy(pCanbusRawData, &CanbusRawData, sizeof(AMBA_SR_CANBUS_RAW_DATA_s));
}

/**
* update parameters of conversion
* @param [in] pParam parameters of conversion
* @return none
*/
void SvcCan_UpdateXferParams(const SVC_CAN_XFER_PARAM_s *pParam)
{
    AmbaSvcWrap_MisraMemcpy(&g_XferParams, pParam, sizeof(SVC_CAN_XFER_PARAM_s));
}

/**
* conversion of raw data from canbus
* @param [in] RawData raw data
* @param [out] TransData data after conversion
* @return 0-OK, 1-NG
*/
UINT32 SvcCan_Transfer(const AMBA_SR_CANBUS_RAW_DATA_s *RawData, AMBA_SR_CANBUS_TRANSFER_DATA_s *TransData)
{
    UINT32 Rval;

    if ((RawData == NULL) || (TransData == NULL)) {
        Rval = SVC_NG;
    } else {
        //Input
        UINT32 RawSpeed = RawData->RawSpeed;
        UINT32 RawWheelAngle = RawData->RawWheelAngle;
        UINT32 RawWheelAngleSpeed = RawData->RawWheelAngleSpeed;

        //Transfer function
        DOUBLE TransferSpeed = transRawSpeed(RawSpeed, &g_XferParams);
        DOUBLE TransferWheelAngle = transRawWheelAngle(RawWheelAngle, &g_XferParams);
        DOUBLE TransferWheelAngleSpeed = transRawWheelAngleSpeed(RawWheelAngleSpeed, &g_XferParams);

        //Output
        TransData->CapTS = RawData->CapTS;
        TransData->FlagValidTransferData = RawData->FlagValidRawData;
        TransData->CANBusTurnLightStatus = RawData->CANBusTurnLightStatus;
        TransData->CANBusGearStatus = RawData->CANBusGearStatus;
        TransData->TransferSpeed = TransferSpeed;
        TransData->WheelDir = RawData->WheelDir;
        TransData->TransferWheelAngle = TransferWheelAngle;
        TransData->TransferWheelAngleSpeed = TransferWheelAngleSpeed;

        Rval = SVC_OK;
    }

    return Rval;
}
#endif
