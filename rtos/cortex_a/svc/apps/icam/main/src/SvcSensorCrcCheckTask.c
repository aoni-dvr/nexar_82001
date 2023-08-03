#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaSensor.h"
#include "AmbaCache.h"
#include "AmbaKAL.h"

#include "AmbaDSP_Def.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDspInt.h"
#include "AmbaSvcWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcResCfg.h"
#include "SvcSafeStateMgr.h"
#include "SvcPlat.h"

#include "SvcTaskList.h"

#include "SvcSensorCrcCheckTask.h"


#define CRC_CHECK_TASK_STACK_SIZE   (0x10000U)
#define CRC_CHECK_TASK_LOG          "CRC_CHK_TASK"

#define CRC_CHECK_LOG_LVL_0         (0x0U)
#define CRC_CHECK_LOG_LVL_1         (0x1U)
#define CRC_CHECK_LOG_LVL_2         (0x2U)

typedef union {
    UINT32 Data[5U];
    struct {
        UINT32 VinID;
        UINT32 FrameCnt;        /* From RAW_DATA_READY CapSeq */
        UINT32 EmbCrcAddr;      /* From RAW_DATA_READY embedded */
        UINT32 EmbBufSize;      /* From RAW_DATA_READY embedded, pitch * height */
        UINT32 Reserved;
    } VinSrc;
} CRC_CHECK_TASK_DATA_u;

typedef struct {
    CRC_CHECK_TASK_DATA_u Ctx;
} CRC_CHECK_TASK_DATA_s;

typedef struct {
    UINT8                   Enable;
    AMBA_KAL_MSG_QUEUE_t    Queue;
    CRC_CHECK_TASK_DATA_u   QueueBuf[8U];
    SVC_TASK_CTRL_s         Task;
    UINT8                   TaskStack[CRC_CHECK_TASK_STACK_SIZE];
    AMBA_DSP_INT_PORT_s     Port;
} CRC_CHECK_TASK_CTRL_s;

static CRC_CHECK_TASK_CTRL_s CrcCheckTaskCtrl = {0};
static UINT32                CrcCheckLvl = CRC_CHECK_LOG_LVL_0;

static void SensorCrcCheckTask_InterruptCallback(UINT32 ActualFlag, const AMBA_DSP_INT_INFO_s *pInfo);
static UINT32 SensorCrcCheckTask_RawDataReady(const void *pEventInfo);
static void *SvcSensorCrcCheckTask_Entry(void *EntryArg);

#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
/* Defined in AmbaSensorSafety_MAX9295_96712_IMX390.h */
extern UINT32 MX01_IMX390_I2cCrcGet(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 FrameCount, UINT16 *pCrcValue);
extern UINT32 MX01_IMX390_EmbI2cCrcGet(const AMBA_SENSOR_CHANNEL_s *pChan, const void* pEmbData, UINT16 *pCrcValue);
extern UINT32 MX01_IMX390_FrameCountUpdate(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 FrameCount);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
/* Defined in AmbaSensorSafety_MAX9295_9296_IMX390 */
extern UINT32 MX00_IMX390_I2cCrcGet(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 FrameCount, UINT16 *pCrcValue);
extern UINT32 MX00_IMX390_EmbI2cCrcGet(const AMBA_SENSOR_CHANNEL_s *pChan, const void* pEmbData, const UINT32 EmbBufPitch, UINT16 *pCrcValue);
extern UINT32 MX00_IMX390_FrameCountUpdate(const AMBA_SENSOR_CHANNEL_s *pChan, const UINT32 FrameCount);
#endif

static void CrcCheckPrint_DBG(UINT32 Lvl, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((Lvl & CrcCheckLvl) > 0U) {
        SvcLog_DBG(CRC_CHECK_TASK_LOG, pFormat, Arg1, Arg2);
    }
}

void SvcSensorCrcCheckTask_DebugLvl(UINT32 Lvl)
{
    CrcCheckLvl = Lvl;
}

UINT32 SvcSensorCrcCheckTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    static char QueueName[] = "SvcSensorCrcCheckQueue";

    if (CrcCheckTaskCtrl.Enable != 0U) {
        SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Start err, the task is already start", 0U, 0U);
        RetVal = SVC_NG;
    }

    /* memory set structure */
    if (SVC_OK == RetVal) {
        RetVal = AmbaWrap_memset(&CrcCheckTaskCtrl, 0, sizeof(CRC_CHECK_TASK_CTRL_s));
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Start err, AmbaWrap_memset failed with 0x%x", RetVal, 0U);
        }
    }

    /* create the msg queue */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MsgQueueCreate(&CrcCheckTaskCtrl.Queue, QueueName, sizeof(CRC_CHECK_TASK_DATA_u), CrcCheckTaskCtrl.QueueBuf, sizeof(CrcCheckTaskCtrl.QueueBuf));
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Start err, AmbaKAL_MsgQueueCreate failed with 0x%x", RetVal, 0U);
        }
    }

    /* create the task for receiving crc result and send to r52 */
    if (SVC_OK == RetVal) {
        CrcCheckTaskCtrl.Task.Priority   = SVC_SENSOR_CRC_CHECK_TASK_PRI;
        CrcCheckTaskCtrl.Task.EntryFunc  = SvcSensorCrcCheckTask_Entry;
        CrcCheckTaskCtrl.Task.EntryArg   = 0U;
        CrcCheckTaskCtrl.Task.pStackBase = CrcCheckTaskCtrl.TaskStack;
        CrcCheckTaskCtrl.Task.StackSize  = CRC_CHECK_TASK_STACK_SIZE;
        CrcCheckTaskCtrl.Task.CpuBits    = SVC_SENSOR_CRC_CHECK_TASK_CPU_BITS;
        RetVal = SvcTask_Create("SvcSensorCrcCheckTask", &CrcCheckTaskCtrl.Task);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Start err, SvcTask_Create failed with 0x%x", RetVal, 0U);
        }
    }

    /* register event RAW_DATA_READY */
    if (SVC_OK == RetVal) {
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SensorCrcCheckTask_RawDataReady);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Start err, AmbaDSP_EventHandlerRegister SensorCrcCheckTask_RawDataReady fail, return 0x%x", RetVal, 0);
        }
    }

    /* open software interrupt port to receive the sof event */
    if (SVC_OK == RetVal) {
        RetVal = AmbaDspInt_OpenEx(&CrcCheckTaskCtrl.Port, 3U /* Priority */, SensorCrcCheckTask_InterruptCallback);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Start err, AmbaDspInt_OpenEx fail, return 0x%x", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, AmbaDspInt_Raw);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Start err, AmbaDSP_EventHandlerRegister AmbaDspInt_Raw fail, return 0x%x", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        CrcCheckTaskCtrl.Enable = 1U;
    }

    return RetVal;
}

UINT32 SvcSensorCrcCheckTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&RetVal);

    if (CrcCheckTaskCtrl.Enable == 0U) {
        SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Stop err, the task is already stop", 0U, 0U);
        RetVal = SVC_NG;
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaDspInt_Close(&CrcCheckTaskCtrl.Port);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Stop err, AmbaDspInt_Close failed with 0x%x", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SensorCrcCheckTask_RawDataReady);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Stop err, AmbaDSP_EventHandlerUnRegister failed with 0x%x", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Destroy(&CrcCheckTaskCtrl.Task);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Stop err, SvcTask_Destroy failed with 0x%x", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MsgQueueDelete(&CrcCheckTaskCtrl.Queue);
        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "SvcSensorCrcCheckTask_Stop err, AmbaKAL_MsgQueueDelete failed with 0x%x", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        CrcCheckTaskCtrl.Enable = 0U;
    }

    return RetVal;
}


static void SensorCrcCheckTask_InterruptCallback(UINT32 ActualFlag, const AMBA_DSP_INT_INFO_s *pInfo)
{
    UINT32 Rval;
    UINT32 VinID, SensorID;
    AMBA_SENSOR_CHANNEL_s Chan;

    AmbaMisra_TouchUnused(&ActualFlag);
    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(&Chan);
    AmbaMisra_TouchUnused(&VinID);
    AmbaMisra_TouchUnused(&SensorID);

#if defined(SVC_DSP_VIN0_SOF)
    if ((ActualFlag & SVC_DSP_VIN0_SOF) > 0U) {
        VinID = 0U;
        Rval = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (SVC_OK == Rval) {
            Chan.VinID = VinID;
            Chan.SensorID = SensorID;

            CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_2, "[SensorCrcCheckTask_InterruptCallback] pInfo->Vin.RawCapSeq[%d] = %d", VinID, pInfo->Vin.RawCapSeq[VinID]);
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
            Rval = MX01_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
            Rval = MX00_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#endif
            AmbaMisra_TouchUnused(&Rval);
        }
    }
#endif
#if defined(SVC_DSP_VIN1_SOF)
    if ((ActualFlag & SVC_DSP_VIN1_SOF) > 0U) {
        VinID = 1U;
        Rval = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (SVC_OK == Rval) {
            Chan.VinID = VinID;
            Chan.SensorID = SensorID;

            CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_2, "[SensorCrcCheckTask_InterruptCallback] pInfo->Vin.RawCapSeq[%d] = %d", VinID, pInfo->Vin.RawCapSeq[VinID]);
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
            Rval = MX01_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
            Rval = MX00_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#endif
            AmbaMisra_TouchUnused(&Rval);
        }

    }
#endif
#if defined(SVC_DSP_VIN2_SOF)
    if ((ActualFlag & SVC_DSP_VIN2_SOF) > 0U) {
        VinID = 2U;
        Rval = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (SVC_OK == Rval) {
            Chan.VinID = VinID;
            Chan.SensorID = SensorID;

            CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_2, "[SensorCrcCheckTask_InterruptCallback] pInfo->Vin.RawCapSeq[%d] = %d", VinID, pInfo->Vin.RawCapSeq[VinID]);
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
            Rval = MX01_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
            Rval = MX00_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#endif
            AmbaMisra_TouchUnused(&Rval);
        }

    }
#endif
#if defined(SVC_DSP_VIN3_SOF)
    if ((ActualFlag & SVC_DSP_VIN3_SOF) > 0U) {
        VinID = 3U;
        Rval = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (SVC_OK == Rval) {
            Chan.VinID = VinID;
            Chan.SensorID = SensorID;

            CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_2, "[SensorCrcCheckTask_InterruptCallback] pInfo->Vin.RawCapSeq[%d] = %d", VinID, pInfo->Vin.RawCapSeq[VinID]);
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
            Rval = MX01_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
            Rval = MX00_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#endif
            AmbaMisra_TouchUnused(&Rval);
        }

    }
#endif
#if defined(SVC_DSP_VIN4_SOF)
    if ((ActualFlag & SVC_DSP_VIN4_SOF) > 0U) {
        VinID = 4U;
        Rval = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (SVC_OK == Rval) {
            Chan.VinID = VinID;
            Chan.SensorID = SensorID;

            CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_2, "[SensorCrcCheckTask_InterruptCallback] pInfo->Vin.RawCapSeq[%d] = %d", VinID, pInfo->Vin.RawCapSeq[VinID]);
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
            Rval = MX01_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
            Rval = MX00_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#endif
            AmbaMisra_TouchUnused(&Rval);
        }

    }
#endif
#if defined(SVC_DSP_VIN5_SOF)
    if ((ActualFlag & SVC_DSP_VIN5_SOF) > 0U) {
        VinID = 5U;
        Rval = SvcResCfg_GetSensorIDInVinID(VinID, &SensorID);
        if (SVC_OK == Rval) {
            Chan.VinID = VinID;
            Chan.SensorID = SensorID;

            CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_2, "[SensorCrcCheckTask_InterruptCallback] pInfo->Vin.RawCapSeq[%d] = %d", VinID, pInfo->Vin.RawCapSeq[VinID]);
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
            Rval = MX01_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
            Rval = MX00_IMX390_FrameCountUpdate(&Chan, pInfo->Vin.RawCapSeq[VinID]);
#else
            AmbaMisra_TouchUnused(&Chan);
#endif
            AmbaMisra_TouchUnused(&Rval);
        }

    }
#endif
}


static UINT32 SensorCrcCheckTask_RawDataReady(const void *pEventInfo)
{
    UINT32 RetVal = SVC_OK;
    const AMBA_DSP_RAW_DATA_RDY_EXT_s *pInfo;
    CRC_CHECK_TASK_DATA_s CrcData = {0};

    AmbaMisra_TypeCast(&pInfo, &pEventInfo);
    CrcData.Ctx.VinSrc.VinID        = pInfo->VinId;
    CrcData.Ctx.VinSrc.FrameCnt     = (UINT32) pInfo->CapSequence;
    CrcData.Ctx.VinSrc.EmbCrcAddr   = pInfo->EmbedBuffer.BaseAddr;
    CrcData.Ctx.VinSrc.EmbBufSize   = (UINT32) pInfo->EmbedBuffer.Pitch * (UINT32) pInfo->EmbedBuffer.Window.Height;

    RetVal = AmbaKAL_MsgQueueSend(&CrcCheckTaskCtrl.Queue, &CrcData.Ctx, AMBA_KAL_NO_WAIT);
    if (RetVal != SVC_OK) {
        SvcLog_NG(CRC_CHECK_TASK_LOG, "[SensorCrcCheckTask_RawDataReady] AmbaKAL_MsgQueueSend return 0x%x", RetVal, 0U);
    }
    return OK;
}


static void *SvcSensorCrcCheckTask_Entry(void *EntryArg)
{
    UINT32 SensorID;
    UINT16 EmbCrc, DataCrc;
    const void   *pAddr;
    CRC_CHECK_TASK_DATA_s CrcData = {0};
    SVC_SAFE_STATE_MGR_VIN_EMBD_s VinEmbdInfo;
    AMBA_SENSOR_CHANNEL_s Chan;
    AMBA_SENSOR_STATUS_INFO_s Status;

    UINT32 RetVal = SVC_OK;

    while (AmbaKAL_MsgQueueReceive(&CrcCheckTaskCtrl.Queue, &CrcData.Ctx, AMBA_KAL_WAIT_FOREVER) == SVC_OK) {
        AmbaSvcWrap_MisraMemset(&Chan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));

        RetVal = SvcResCfg_GetSensorIDInVinID(CrcData.Ctx.VinSrc.VinID, &SensorID);
        if (RetVal != OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "[SvcSensorCrcCheckTask_Entry] SvcResCfg_GetSensorIDInVinID return 0x%x", RetVal, 0U);
        }

        Chan.VinID    = CrcData.Ctx.VinSrc.VinID;
        Chan.SensorID = SensorID;
        RetVal = AmbaSensor_GetStatus(&Chan, &Status);
        if (RetVal != OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "[SvcSensorCrcCheckTask_Entry] AmbaSensor_GetStatus return 0x%x", RetVal, 0U);
        }
        RetVal = SvcPlat_CacheInvalidate(CrcData.Ctx.VinSrc.EmbCrcAddr, CrcData.Ctx.VinSrc.EmbBufSize);
        if (RetVal != OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "[SvcSensorCrcCheckTask_Entry] SvcPlat_CacheInvalidate return 0x%x", RetVal, 0U);
        }
        AmbaMisra_TypeCast(&pAddr, &CrcData.Ctx.VinSrc.EmbCrcAddr);
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
        RetVal = MX01_IMX390_EmbI2cCrcGet(&Chan, pAddr, &EmbCrc);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
        RetVal = MX00_IMX390_EmbI2cCrcGet(&Chan, pAddr, Status.ModeInfo.EmbDataInfo.Width, &EmbCrc);
#else
        EmbCrc = 0U;
#endif
        if (RetVal != OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "[SvcSensorCrcCheckTask_Entry] SensorCrc_EmbI2cCrcGet return 0x%x", RetVal, 0U);
        }
#if defined(CONFIG_MX01_IMX390_ASIL_ENABLED)
        RetVal = MX01_IMX390_I2cCrcGet(&Chan, CrcData.Ctx.VinSrc.FrameCnt, &DataCrc);
#elif defined(CONFIG_MX00_IMX390_ASIL_ENABLED)
        RetVal = MX00_IMX390_I2cCrcGet(&Chan, CrcData.Ctx.VinSrc.FrameCnt, &DataCrc);
#else
        DataCrc = 0U;
#endif
        if (RetVal != OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "[SvcSensorCrcCheckTask_Entry] SensorCrc_I2cCrcGet return 0x%x", RetVal, 0U);
        }

        CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_1, "[SvcSensorCrcCheckTask_Entry] CapSequence %u, embedded 0x%x", (UINT32)CrcData.Ctx.VinSrc.FrameCnt, CrcData.Ctx.VinSrc.EmbCrcAddr);
        CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_1, "               EmbCrc %u DataCrc %u", EmbCrc, DataCrc);
        CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_1, "               VinID %u", CrcData.Ctx.VinSrc.VinID, 0U);
        CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_1, "               EmbCrcAddr 0x%x EmbedBuf size %d", CrcData.Ctx.VinSrc.EmbCrcAddr, CrcData.Ctx.VinSrc.EmbBufSize);
        CrcCheckPrint_DBG(CRC_CHECK_LOG_LVL_1, "               sensor w %u h %u", Status.ModeInfo.EmbDataInfo.Width, Status.ModeInfo.EmbDataInfo.Height);

        VinEmbdInfo.VinID    = CrcData.Ctx.VinSrc.VinID;
        VinEmbdInfo.SensorID = SensorID;
        VinEmbdInfo.FrameCnt = CrcData.Ctx.VinSrc.FrameCnt;
        VinEmbdInfo.EmbCrc   = EmbCrc;
        VinEmbdInfo.DataCrc  = DataCrc;
        RetVal = SvcSafeStateMgr_CheckVinEmbd(&VinEmbdInfo);

        if (RetVal != SVC_OK) {
            SvcLog_NG(CRC_CHECK_TASK_LOG, "[SvcSensorCrcCheckTask_Entry] err, SvcSafeStateMgr_CheckVinEmbd failed with 0x%x", RetVal, 0U);
        }
    }

    AmbaMisra_TouchUnused(EntryArg);
    return NULL;
}
