/**
*  @file SvcVinSrc.c
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
*  @details svc vin source
*
*/

#include "AmbaMisraFix.h"
#include "AmbaSensor.h"
#include "AmbaYuv.h"
#include "AmbaDSP.h"
#include "AmbaEEPROM.h"
#include "AmbaSYS.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"

#include "SvcErrCode.h"
#include "SvcTask.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDSP.h"
#include "SvcVinSrc.h"
#include "SvcTiming.h"

#define SVC_LOG_VSRC                "VSRC"

#define VIN_SRC_TASK_STACK_SIZE     (0xA000U)

#ifndef CONFIG_ICAM_PARALLEL_VIN_CONFIG
static SVC_TASK_CTRL_s VinSrcTaskCtrl GNU_SECTION_NOZEROINIT;
#else

typedef struct {
    UINT32              InitCfgId;
    SVC_VIN_SRC_INIT_s *pSrcInit;
    char                TaskName[32U];

    UINT8               TaskStack[VIN_SRC_TASK_STACK_SIZE];
    SVC_TASK_CTRL_s     TaskCtrl;
} SVC_VIN_SRC_TASK_CTRL_s;

static SVC_VIN_SRC_TASK_CTRL_s VinSrcTaskCtrl[AMBA_DSP_MAX_VIN_NUM] GNU_SECTION_NOZEROINIT;
static UINT32                  VinSrcCfgReqBits;
static UINT32                  VinSrcCfgRdyBits;
static UINT32                  VinParaCfgStart = 0U;
#endif

#ifndef CONFIG_ICAM_PARALLEL_VIN_CONFIG
static void* VinSrcTaskEntry(void* EntryArg)
{
    UINT32                    i, Rval = SVC_OK;
    const ULONG               *pArg;
    ULONG                     Arg;
    const SVC_VIN_SRC_INIT_s  *pInit;
    const SVC_VIN_SRC_CFG_s   *pSrcCfg;
    AMBA_SENSOR_CHANNEL_s     SsChan;
    AMBA_SENSOR_OBJ_s         *pSsObj;
    AMBA_YUV_CHANNEL_s        YuvChan;
    AMBA_YUV_OBJ_s            *pYuvObj;

    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin init begin", 0U, 0U);
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_VIN_CONFIG_START, "Vin config START");
    #endif

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Arg = (*pArg);
    AmbaMisra_TypeCast(&pInit, &Arg);

    /* hook and init */
    for (i = 0; i < pInit->CfgNum; i++) {
        pSrcCfg = &(pInit->InitCfgArr[i].Config);

        if (SVC_VIN_SRC_SENSOR == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pSsObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pSrcCfg->VinID;
            SsChan.SensorID = pSrcCfg->SrcBits;

            AmbaSensor_Hook(&SsChan, pSsObj);
            Rval = AmbaSensor_Init(&SsChan);
        } else if (SVC_VIN_SRC_YUV == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pYuvObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(AMBA_YUV_CHANNEL_s));
            YuvChan.VinID = pSrcCfg->VinID;

            AmbaYuv_Hook(&YuvChan, pYuvObj);
            Rval = AmbaYuv_Init(&YuvChan);
        } else if (SVC_VIN_SRC_MEM == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pSsObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pSrcCfg->VinID;
            SsChan.SensorID = pSrcCfg->SrcBits;

            AmbaSensor_Hook(&SsChan, pSsObj);
            Rval = AmbaSensor_Init(&SsChan);
        } else {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_VSRC, "## unknown vin source. VIN (%u) type(%u)", pSrcCfg->VinID, pSrcCfg->SrcType);
        }

        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VSRC, "## fail to hook/init. VIN (%u) rval(%u)", pSrcCfg->VinID, Rval);
            break;
        }
    }

    Rval = OK;
    AmbaMisra_TouchUnused(&Rval);
    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin init end", 0U, 0U);
    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin config begin", 0U, 0U);

    if (OK == Rval) {
        /* enable and config */
        for (i = 0; i < pInit->CfgNum; i++) {
            pSrcCfg = &(pInit->InitCfgArr[i].Config);
            if (pInit->InitCfgArr[i].IsActive == 0U) {
                continue;
            }

            Rval = SvcVinSrc_Enable(1U, pSrcCfg);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_VSRC, "## fail to enable, rval(%u)", Rval, 0U);
            } else {
                Rval = SvcVinSrc_Config(1U, pSrcCfg);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_VSRC, "## fail to config, rval(%u)", Rval, 0U);
                }
            }

            if (Rval != 0U) {
                break;
            }
        }
    }

    Rval = OK;
    AmbaMisra_TouchUnused(&Rval);
    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin config end", 0U, 0U);

    if (OK == Rval) {
        if (pInit->pInitDoneCB != NULL) {
            pInit->pInitDoneCB();
        }
    }

#if defined(CONFIG_ICAM_EEPROM_USED)
    if (OK == Rval) {
        AMBA_EEPROM_CHANNEL_s  EEPROMChan;
        AMBA_EEPROM_OBJ_s      *pEEPROMObj;

        SvcLog_DBG(SVC_LOG_VSRC, "@@ eeprom init begin", 0U, 0U);

        for (i = 0; i < pInit->CfgNum; i++) {
            if (pInit->InitCfgArr[i].pEEPROBObj != NULL) {
                pSrcCfg = &(pInit->InitCfgArr[i].Config);
                AmbaMisra_TypeCast(&pEEPROMObj, &(pInit->InitCfgArr[i].pEEPROBObj));

                AmbaSvcWrap_MisraMemset(&EEPROMChan, 0, sizeof(AMBA_EEPROM_CHANNEL_s));
                EEPROMChan.VinId  = pSrcCfg->VinID;
                EEPROMChan.ChanId = pSrcCfg->SrcBits;

                if (AMBA_EEPROM_ERR_NONE == AmbaEEPROM_Hook(&EEPROMChan, pEEPROMObj)) {
                    Rval = AmbaEEPROM_Init(&EEPROMChan);
                    AmbaMisra_TouchUnused(&Rval);
                }
            }
        }

        SvcLog_DBG(SVC_LOG_VSRC, "@@ eeprom init end", 0U, 0U);
    }
#endif

    return NULL;
}
#else
static void* VinSrcTaskEntry(void* EntryArg)
{
    UINT32                    i, Rval = SVC_OK;
    const ULONG               *pArg;
    ULONG                     Arg;
    const SVC_VIN_SRC_INIT_s  *pInit;
    const SVC_VIN_SRC_CFG_s   *pSrcCfg;
    AMBA_SENSOR_CHANNEL_s     SsChan;
    AMBA_SENSOR_OBJ_s         *pSsObj;
    AMBA_YUV_CHANNEL_s        YuvChan;
    AMBA_YUV_OBJ_s            *pYuvObj;
    const SVC_VIN_SRC_TASK_CTRL_s *pCtrl = NULL;
    UINT32 VinID;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Arg = (*pArg);
    AmbaMisra_TypeCast(&pCtrl, &Arg);
    pInit = pCtrl->pSrcInit;
    VinID = pInit->InitCfgArr[pCtrl->InitCfgId].Config.VinID;

    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin%d init begin", VinID, 0U);
    #if defined(CONFIG_ICAM_TIMING_LOG)
    {
        if (VinParaCfgStart == 0U) {
            VinParaCfgStart = 1U;
            SvcTime(SVC_TIME_VIN_CONFIG_START, "Vin config START");
        }
    }
    #endif

    /* hook and init */
    {
        i = pCtrl->InitCfgId;
        pSrcCfg = &(pInit->InitCfgArr[i].Config);

        if (SVC_VIN_SRC_SENSOR == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pSsObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pSrcCfg->VinID;
            SsChan.SensorID = pSrcCfg->SrcBits;

            AmbaSensor_Hook(&SsChan, pSsObj);
            Rval = AmbaSensor_Init(&SsChan);
        } else if (SVC_VIN_SRC_YUV == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pYuvObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(AMBA_YUV_CHANNEL_s));
            YuvChan.VinID = pSrcCfg->VinID;

            AmbaYuv_Hook(&YuvChan, pYuvObj);
            Rval = AmbaYuv_Init(&YuvChan);
        } else if (SVC_VIN_SRC_MEM == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pSsObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pSrcCfg->VinID;
            SsChan.SensorID = pSrcCfg->SrcBits;

            AmbaSensor_Hook(&SsChan, pSsObj);
            Rval = AmbaSensor_Init(&SsChan);
        } else {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_VSRC, "## unknown vin source. VIN (%u) type(%u)", pSrcCfg->VinID, pSrcCfg->SrcType);
        }

        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VSRC, "## fail to hook/init. VIN (%u) rval(%u)", pSrcCfg->VinID, Rval);
        }
    }

    Rval = OK;
    AmbaMisra_TouchUnused(&Rval);
    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin%d init end", VinID, 0U);
    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin%d config begin", VinID, 0U);

    if (OK == Rval) {
        /* enable and config */
        {
            pSrcCfg = &(pInit->InitCfgArr[pCtrl->InitCfgId].Config);
            Rval = SvcVinSrc_Enable(1U, pSrcCfg);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_VSRC, "## fail to enable, rval(%u)", Rval, 0U);
            } else {
                Rval = SvcVinSrc_Config(1U, pSrcCfg);
                if (OK != Rval) {
                    SvcLog_NG(SVC_LOG_VSRC, "## fail to config, rval(%u)", Rval, 0U);
                }
            }
        }
    }

    Rval = OK;
    AmbaMisra_TouchUnused(&Rval);
    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin%d config end", VinID, 0U);

    if (OK == Rval) {
        VinSrcCfgRdyBits |= (UINT32)1U << pCtrl->InitCfgId;

        if (VinSrcCfgRdyBits == VinSrcCfgReqBits) {
            if (pInit->pInitDoneCB != NULL) {
                pInit->pInitDoneCB();
            }
        }
    }

#if defined(CONFIG_ICAM_EEPROM_USED)
    if (OK == Rval) {
        AMBA_EEPROM_CHANNEL_s  EEPROMChan;
        AMBA_EEPROM_OBJ_s      *pEEPROMObj;

        SvcLog_DBG(SVC_LOG_VSRC, "@@ vin%d eeprom init begin", VinID, 0U);

        {
            i = pCtrl->InitCfgId;
            if (pInit->InitCfgArr[i].pEEPROBObj != NULL) {
                pSrcCfg = &(pInit->InitCfgArr[i].Config);
                AmbaMisra_TypeCast(&pEEPROMObj, &(pInit->InitCfgArr[i].pEEPROBObj));

                AmbaSvcWrap_MisraMemset(&EEPROMChan, 0, sizeof(AMBA_EEPROM_CHANNEL_s));
                EEPROMChan.VinId  = pSrcCfg->VinID;
                EEPROMChan.ChanId = pSrcCfg->SrcBits;

                if (AMBA_EEPROM_ERR_NONE == AmbaEEPROM_Hook(&EEPROMChan, pEEPROMObj)) {
                    Rval = AmbaEEPROM_Init(&EEPROMChan);
                    AmbaMisra_TouchUnused(&Rval);
                }
            }
        }

        SvcLog_DBG(SVC_LOG_VSRC, "@@ vin%d eeprom init end", VinID, 0U);
    }
#endif

    return NULL;
}
#endif
/**
* initialization of vin source task
* @param [in] pInit info block of vin source task
* @param [in] Priority task priority
* @param [in] CpuBits task cpu bits
* @return none
*/
void SvcVinSrc_Init(const SVC_VIN_SRC_INIT_s *pInit, UINT32 Priority, UINT32 CpuBits)
{
#ifndef CONFIG_ICAM_PARALLEL_VIN_CONFIG
    static UINT8 VinSrcTaskStack[VIN_SRC_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
#endif
    const SVC_VIN_SRC_CFG_s  *pSrcCfg;
    AMBA_SENSOR_CHANNEL_s    SsChan;
    AMBA_SENSOR_OBJ_s        *pSsObj;
    AMBA_YUV_CHANNEL_s       YuvChan;
    AMBA_YUV_OBJ_s           *pYuvObj;
    UINT32                   i, Rval = OK;

    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin hook begin", 0U, 0U);
    /* hook and init */
    for (i = 0; i < pInit->CfgNum; i++) {
        pSrcCfg = &(pInit->InitCfgArr[i].Config);

        if (SVC_VIN_SRC_SENSOR == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pSsObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pSrcCfg->VinID;
            SsChan.SensorID = pSrcCfg->SrcBits;

            AmbaSensor_Hook(&SsChan, pSsObj);
        } else if (SVC_VIN_SRC_YUV == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pYuvObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(AMBA_YUV_CHANNEL_s));
            YuvChan.VinID = pSrcCfg->VinID;

            AmbaYuv_Hook(&YuvChan, pYuvObj);
        } else if (SVC_VIN_SRC_MEM == pSrcCfg->SrcType) {
            AmbaMisra_TypeCast(&pSsObj, &(pInit->InitCfgArr[i].pSrcObj));

            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pSrcCfg->VinID;
            SsChan.SensorID = pSrcCfg->SrcBits;

            AmbaSensor_Hook(&SsChan, pSsObj);
        } else {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_VSRC, "## unknown vin source type(%u)", pSrcCfg->SrcType, 0U);
        }

        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VSRC, "## fail to hook/init rval(%u)", Rval, 0U);
            break;
        }
    }
    Rval = OK;
    AmbaMisra_TouchUnused(&Rval);
    SvcLog_DBG(SVC_LOG_VSRC, "@@ vin hook done", 0U, 0U);

#if defined(CONFIG_ICAM_EEPROM_USED)
    if (OK == Rval) {
        AMBA_EEPROM_CHANNEL_s  EEPROMChan;
        AMBA_EEPROM_OBJ_s      *pEEPROMObj;

        SvcLog_DBG(SVC_LOG_VSRC, "@@ eeprom hook begin", 0U, 0U);

        for (i = 0; i < pInit->CfgNum; i++) {
            pSrcCfg = &(pInit->InitCfgArr[i].Config);

            if (pInit->InitCfgArr[i].pEEPROBObj != NULL) {
                AmbaMisra_TypeCast(&pEEPROMObj, &(pInit->InitCfgArr[i].pEEPROBObj));

                AmbaSvcWrap_MisraMemset(&EEPROMChan, 0, sizeof(AMBA_EEPROM_CHANNEL_s));
                EEPROMChan.VinId = pSrcCfg->VinID;

                (void) AmbaEEPROM_Hook(&EEPROMChan, pEEPROMObj);
            }
        }

        SvcLog_DBG(SVC_LOG_VSRC, "@@ eeprom hook end", 0U, 0U);
    }
#endif

    if (OK == Rval) {
#ifndef CONFIG_ICAM_PARALLEL_VIN_CONFIG
        /* trigger task to init vin source */
        VinSrcTaskCtrl.Priority   = Priority;
        VinSrcTaskCtrl.EntryFunc  = VinSrcTaskEntry;
        VinSrcTaskCtrl.pStackBase = VinSrcTaskStack;
        VinSrcTaskCtrl.StackSize  = VIN_SRC_TASK_STACK_SIZE;
        VinSrcTaskCtrl.CpuBits    = CpuBits;
        AmbaMisra_TypeCast(&(VinSrcTaskCtrl.EntryArg), &pInit);

        Rval = SvcTask_Create("SvcVinSrcTask", &VinSrcTaskCtrl);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_VSRC, "vin_srv_task isn't created", 0U, 0U);
        }
#else
        SVC_VIN_SRC_TASK_CTRL_s *pCurTask;
        UINT32 StrLeng;

        AmbaSvcWrap_MisraMemset(&VinSrcTaskCtrl, 0, sizeof(VinSrcTaskCtrl));
        VinSrcCfgRdyBits = 0U;
        VinSrcCfgReqBits = 0U;
        for (i = 0; i < pInit->CfgNum; i++) {

            VinSrcCfgReqBits |= (UINT32)1U << i;

            pCurTask = &(VinSrcTaskCtrl[i]);

            SVC_WRAP_SNPRINT "SvcVinSrcTask_%02d"
            SVC_SNPRN_ARG_S pCurTask->TaskName
            SVC_SNPRN_ARG_BSIZE (UINT32)sizeof(pCurTask->TaskName)
            SVC_SNPRN_ARG_RLEN &StrLeng
            SVC_SNPRN_ARG_UINT32 i SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_E

            pCurTask->InitCfgId = i;
            AmbaMisra_TypeCast(&(pCurTask->pSrcInit), &pInit);

            /* trigger task to init vin source */
            pCurTask->TaskCtrl.Priority   = pInit->CfgTaskInfo[i].TaskPriority;
            pCurTask->TaskCtrl.EntryFunc  = VinSrcTaskEntry;
            pCurTask->TaskCtrl.pStackBase = pCurTask->TaskStack;
            pCurTask->TaskCtrl.StackSize  = VIN_SRC_TASK_STACK_SIZE;
            pCurTask->TaskCtrl.CpuBits    = pInit->CfgTaskInfo[i].TaskCpuBits;
            AmbaMisra_TypeCast(&(pCurTask->TaskCtrl.EntryArg), &pCurTask);

            Rval = SvcTask_Create(pCurTask->TaskName, &(pCurTask->TaskCtrl));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_VSRC, "vin_srv_task(%d) isn't created, ErrCode(0x%08X)", i, Rval);
                SvcLog_NG(SVC_LOG_VSRC, "  TaskPriority: %d, TaskCpuBits: 0x%X"
                         , pInit->CfgTaskInfo[i].TaskPriority
                         , pInit->CfgTaskInfo[i].TaskCpuBits);
            }

            AmbaMisra_TouchUnused(&StrLeng);

            AmbaMisra_TouchUnused(&Priority);
            AmbaMisra_TouchUnused(&CpuBits);
        }

#endif
    }
}

/**
* configuration of vin source
* @param [in] CfgNum number of vin source
* @param [in] pCfgArr configuration array of vin source
* @return none
*/
UINT32 SvcVinSrc_Config(const UINT32 CfgNum, const SVC_VIN_SRC_CFG_s *pCfgArr)
{
    UINT32                 i, Rval = SVC_NG;
    AMBA_SENSOR_CHANNEL_s  SsChan;
    AMBA_SENSOR_CONFIG_s   SsCfg;
    AMBA_YUV_CHANNEL_s     YuvChan;
    AMBA_YUV_CONFIG_s      YuvCfg;

    for (i = 0; i < CfgNum; i++) {
        if (SVC_VIN_SRC_SENSOR == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pCfgArr[i].VinID;
            SsChan.SensorID = pCfgArr[i].SrcBits;

            AmbaSvcWrap_MisraMemset(&SsCfg, 0, sizeof(AMBA_SENSOR_CONFIG_s));
            SsCfg.ModeID   = pCfgArr[i].SrcMode;
            SsCfg.ModeID_1 = pCfgArr[i].SrcMode_1;    /* Only valid for sensor driver supporting virtual channel */
            SsCfg.ModeID_2 = pCfgArr[i].SrcMode_2;    /* Only valid for sensor driver supporting virtual channel */
            SsCfg.ModeID_3 = pCfgArr[i].SrcMode_3;    /* Only valid for sensor driver supporting virtual channel */
            SsCfg.Reserved0[0U] = (UINT8)pCfgArr[i].DisableMasterSync;  /* Master-Sync Disable. If 1, the sensor will NOT enable Master-Sync (If driver support) */
            SsCfg.Reserved0[1U] = (UINT8)pCfgArr[i].VsyncDelay;         /* Take effect only when driver and hardware support */
#ifdef CONFIG_MX01_IMX390_ASIL_ENABLED
            SsCfg.EnableEmbDataCap = 1U;
#endif

            if (SvcDSP_IsCleanBoot() == SVCDSP_OP_DIRTYBOOT) {
                SsCfg.KeepState = 1U;
                Rval = AmbaSensor_Config(&SsChan, &SsCfg);
                SsCfg.KeepState = 0U;
            } else {
                Rval = AmbaSensor_Config(&SsChan, &SsCfg);
            }
        } else if (SVC_VIN_SRC_YUV == pCfgArr[i].SrcType) {
            //AMBA_YUV_MODE_INFO_s ModeInfo = {0};

            AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(AMBA_YUV_CHANNEL_s));
            YuvChan.VinID = pCfgArr[i].VinID;

            AmbaSvcWrap_MisraMemset(&YuvCfg, 0, sizeof(AMBA_YUV_CONFIG_s));
            YuvCfg.ModeID = pCfgArr[i].SrcMode;

            Rval = AmbaKAL_TaskSleep(100);
            AmbaMisra_TouchUnused(&Rval);

            Rval = AmbaYuv_Config(&YuvChan, &YuvCfg);

            //if (Rval == OK) {
            //    Rval = AmbaYuv_GetModeInfo(&YuvChan, &YuvCfg, &ModeInfo);
            //    if (Rval == SVC_NG) {
            //        SvcLog_NG(SVC_LOG_VSRC, "AmbaYuv_GetModeInfo failed, rval(%u)", Rval, 0U);
            //    }
            //}
            //if (Rval == OK) {
            //    if (pCfgArr[i].VinID == 0U) {
            //        Rval = AmbaSYS_SetExtInputClkInfo(AMBA_SYS_EXT_CLK_SENSOR_SPCLK0, ModeInfo.OutputInfo.DataRate >> 2);
            //    } else {
            //        Rval = AmbaSYS_SetExtInputClkInfo(AMBA_SYS_EXT_CLK_SENSOR_SPCLK1, ModeInfo.OutputInfo.DataRate >> 2);
            //    }
            //    if (Rval == SVC_NG) {
            //        SvcLog_NG(SVC_LOG_VSRC, "AmbaSYS_SetExtInputClkInfo failed, rval(%u)", Rval, 0U);
            //    }
            //}
            //if (Rval == OK) {
            //    extern UINT32 AmbaRTSL_PllSetAudioClkConfig(UINT32 AudioClkConfig);
            //    Rval = AmbaRTSL_PllSetAudioClkConfig(3); // AMBA_PLL_AUDIO_CLK_MUXED_LVDS_SPCLK
            //    if (Rval == SVC_NG) {
            //        SvcLog_NG(SVC_LOG_VSRC, "AmbaRTSL_PllSetAudioClkConfig failed, rval(%u)", Rval, 0U);
            //    }
            //}
            //if (Rval == OK) {
            //    extern UINT32 AmbaRTSL_PllSetAudioClk(UINT32 Frequency);
            //    Rval = AmbaRTSL_PllSetAudioClk(12288000);
            //    if (Rval == SVC_NG) {
            //        SvcLog_NG(SVC_LOG_VSRC, "AmbaRTSL_PllSetAudioClk failed, rval(%u)", Rval, 0U);
            //    }
            //}

        } else if (SVC_VIN_SRC_MEM == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pCfgArr[i].VinID;
            SsChan.SensorID = pCfgArr[i].SrcBits;

            AmbaSvcWrap_MisraMemset(&SsCfg, 0, sizeof(AMBA_SENSOR_CONFIG_s));
            SsCfg.ModeID = 0U;

            Rval = AmbaSensor_Config(&SsChan, &SsCfg);
        } else {
            Rval = SVC_NG;
        }

        if (OK != Rval) {
            if (SVC_VIN_SRC_SENSOR == pCfgArr[i].SrcType) {
                if ((VIN_ERR_UNEXPECTED == (Rval | VIN_ERR_UNEXPECTED)) ||
                    (VIN_ERR_TIMEOUT == (Rval | VIN_ERR_TIMEOUT))) {
                    SvcLog_DBG(SVC_LOG_VSRC, "[Warning] AmbaSensor_Config return 0x%x. May be caused by sharing serdes reset gpio pin.", Rval, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_VSRC, "## fail to config, rval(0x%x)", Rval, 0U);
            }
            break;
        }
    }
    Rval = OK;
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_VIN_CONFIG_DONE, "Vin config DONE");
    #endif

    return Rval;
}

/**
* de-initialization of vin source task
* @param [in] pInit info block of vin source
* @return none
*/
void SvcVinSrc_DeInit(const SVC_VIN_SRC_INIT_s *pInit)
{
    UINT32 Rval;
    UINT32 i;

#ifndef CONFIG_ICAM_PARALLEL_VIN_CONFIG
    Rval = SvcTask_Destroy(&VinSrcTaskCtrl);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_VSRC, "## fail to destroy vin src task (%u)", Rval, 0U);
    }
#else
    for (i = 0; i < pInit->CfgNum; i++) {
        Rval = SvcTask_Destroy(&(VinSrcTaskCtrl[i].TaskCtrl));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_VSRC, "## fail to destroy vin src task (%u)", Rval, 0U);
        }
    }
#endif

    for (i = 0; i < pInit->CfgNum; i++) {
        Rval = SvcVinSrc_Disable(1U, &pInit->InitCfgArr[i].Config);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_VSRC, "## fail to vin src disable (%u)", Rval, 0U);
        }
    }
}

/**
* enable vin source
* @param [in] CfgNum number of vin source
* @param [in] pCfgArr configuration array of vin source
* @return 0-OK, 1-NG
*/
UINT32 SvcVinSrc_Enable(const UINT32 CfgNum, const SVC_VIN_SRC_CFG_s *pCfgArr)
{
    UINT32                 i, Rval = SVC_NG;
    AMBA_SENSOR_CHANNEL_s  SsChan;
    AMBA_YUV_CHANNEL_s     YuvChan;

    for (i = 0; i < CfgNum; i++) {
        if (SVC_VIN_SRC_SENSOR == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pCfgArr[i].VinID;
            SsChan.SensorID = pCfgArr[i].SrcBits;

            Rval = AmbaSensor_Enable(&SsChan);
        } else if (SVC_VIN_SRC_YUV == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(AMBA_YUV_CHANNEL_s));
            YuvChan.VinID = pCfgArr[i].VinID;

            Rval = AmbaYuv_Enable(&YuvChan);
        } else if (SVC_VIN_SRC_MEM == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pCfgArr[i].VinID;
            SsChan.SensorID = pCfgArr[i].SrcBits;

            Rval = AmbaSensor_Enable(&SsChan);
        } else {
            Rval = SVC_NG;
        }

        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_VSRC, "## fail to enable, rval(%u)", Rval, 0U);
            break;
        }
    }
    Rval = OK;

    return Rval;
}

/**
* disable vin source
* @param [in] CfgNum number of vin source
* @param [in] pCfgArr configuration array of vin source
* @return 0-OK, 1-NG
*/
UINT32 SvcVinSrc_Disable(const UINT32 CfgNum, const SVC_VIN_SRC_CFG_s *pCfgArr)
{
    UINT32                 i, Rval = SVC_OK, TRval = SVC_OK;
    AMBA_SENSOR_CHANNEL_s  SsChan;
    AMBA_YUV_CHANNEL_s     YuvChan;

    for (i = 0; i < CfgNum; i++) {
        if (SVC_VIN_SRC_SENSOR == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pCfgArr[i].VinID;
            SsChan.SensorID = pCfgArr[i].SrcBits;

            TRval = AmbaSensor_Disable(&SsChan);
        } else if (SVC_VIN_SRC_YUV == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&YuvChan, 0, sizeof(AMBA_YUV_CHANNEL_s));
            YuvChan.VinID = pCfgArr[i].VinID;

            TRval = AmbaYuv_Disable(&YuvChan);
        } else if (SVC_VIN_SRC_MEM == pCfgArr[i].SrcType) {
            AmbaSvcWrap_MisraMemset(&SsChan, 0, sizeof(AMBA_SENSOR_CHANNEL_s));
            SsChan.VinID    = pCfgArr[i].VinID;
            SsChan.SensorID = pCfgArr[i].SrcBits;

            TRval = AmbaSensor_Disable(&SsChan);
        } else {
            Rval = SVC_NG;
        }

        if (SVC_OK != TRval) {
            /* For vin recover proc, the sensor disable failure is validate
             * Do not need to stop other vin's sensor disable proc.
             */
            SvcLog_NG(SVC_LOG_VSRC, "## fail to disable, rval(%u)", TRval, TRval);
        }
        if (SVC_OK != Rval) {
            /* Hit some problem in vin source config */
            SvcLog_NG(SVC_LOG_VSRC, "## fail to disable with wrong source type", TRval, TRval);
            break;
        }
    }

    return Rval;
}
