/**
 *  @file SvcSurTask.c
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
 *  @details svc application Avm Adas Control functions
 *
 */
#include "AmbaTypes.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "SvcCmd.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcGui.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcDisplayTask.h"
#include "SvcSurDrawTask.h"
#include "SvcSegDrawTask.h"
#include "SvcAppStat.h"
#include "SvcSysStat.h"


#ifdef CONFIG_ICAM_ENABLE_VOUT_FRAME_CTRL
#include "SvcVoutFrmCtrlTask.h"
#endif

#ifdef CONFIG_ICAM_BUILD_ENET
#include "AmbaENET_Def.h"
#include "AmbaENET.h"
#ifdef CONFIG_AMBA_AVB
#include "NetStack.h"
#include "AvbStack.h"
#include "avdecc.h"
#include "RefCode_Avdecc.h"
#endif
#endif

#include "AmbaGPIO.h"


#define SVC_SUR_QUEUE_SIZE        32U
#define SVC_SUR_TASK_STACK_SIZE (0xF000)

#define SVC_LOG_SUR_TASK      "SurTask"

typedef enum {
    SVC_SUR_TASK_KEY_UP = 0,
    SVC_SUR_TASK_KEY_DOWN,
    SVC_SUR_TASK_KEY_LEFT,
    SVC_SUR_TASK_KEY_RIGHT,

    SVC_SUR_TASK_KEY_NUM
} SVC_SUR_TASK_KEY_TYPE_e;


static AMBA_KAL_MSG_QUEUE_t    SvcSurQueId GNU_SECTION_NOZEROINIT;

extern void SvcSurTask_Init(void);

static UINT32 SvcSurTask_ChgView(UINT32 ViewId)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    static SVC_RES_CFG_s NewResCfg GNU_SECTION_NOZEROINIT;
    UINT32 Rval = SVC_OK;

    if (pResCfg->FovNum < 2U) {
        SvcLog_NG(SVC_LOG_SUR_TASK, "Current mode doesn't support this command. FOV not enough", 0U, 0U);
    } else if (ViewId < pResCfg->FovNum) {
        /* Copy originally used ResCfg */
        AmbaSvcWrap_MisraMemcpy(&NewResCfg, pResCfg, sizeof(SVC_RES_CFG_s));
        NewResCfg.CvFlow[0U].InputCfg.Input[0].StrmId = ViewId;
        Rval = SvcResCfg_Config(&NewResCfg);
        if (SVC_OK == Rval) {
            SvcLog_OK(SVC_LOG_SUR_TASK, "CV Flow re-configure successfully strmIdx = %d %d", 0, 0);
        }
        Rval = SvcCvFlow_Control(0, SVC_CV_CTRL_SET_SRC_STRMID, &ViewId);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcCvFlow_Control set strmId to %d failed.", ViewId, 0);
        }

        Rval = SvcDisplayTask_Stop();
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcDisplayTask_Stop Fail", 0U, 0U);
        }
        Rval = SvcResCfg_ConfigDispAlt(ViewId);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcResCfg_ConfigDispAlt Fail", 0U, 0U);
        }
        Rval = SvcDisplayTask_Resume();
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcDisplayTask_Resume Fail", 0U, 0U);
        }

        SvcSurDrawTask_ClearOsd();
        if (SVC_OK == Rval) {
            SVC_APP_STAT_PSD_s PsdStatus;
            SVC_APP_STAT_OWS_s OwsStatus;
            SVC_APP_STAT_RCTA_s RctaStatus;

            Rval = SvcSysStat_Get(SVC_APP_STAT_PSD, &PsdStatus);
            if ((PsdStatus.Status == SVC_APP_STAT_PSD_DISABLE) && ((ViewId == 2U) || (ViewId == 3U))) {
                PsdStatus.Status = SVC_APP_STAT_PSD_ENABLE;
                Rval = SvcSysStat_Issue(SVC_APP_STAT_PSD, &PsdStatus);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
                }
            } else if ((PsdStatus.Status == SVC_APP_STAT_PSD_ENABLE) && (ViewId != 2U) && (ViewId != 3U)) {
                PsdStatus.Status = SVC_APP_STAT_PSD_DISABLE;
                Rval = SvcSysStat_Issue(SVC_APP_STAT_PSD, &PsdStatus);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
                }
            } else {
                /**do nothing*/
            }

            Rval = SvcSysStat_Get(SVC_APP_STAT_OWS, &OwsStatus);
            if ((OwsStatus.Status == SVC_APP_STAT_OWS_DISABLE) && (ViewId == 1U)) {
                OwsStatus.Status = SVC_APP_STAT_OWS_ENABLE;
                Rval = SvcSysStat_Issue(SVC_APP_STAT_OWS, &OwsStatus);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
                }
            } else if ((OwsStatus.Status == SVC_APP_STAT_OWS_ENABLE) && (ViewId != 1U)) {
                OwsStatus.Status = SVC_APP_STAT_OWS_DISABLE;
                Rval = SvcSysStat_Issue(SVC_APP_STAT_OWS, &OwsStatus);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
                }
            } else {
                /**do nothing*/
            }

            Rval = SvcSysStat_Get(SVC_APP_STAT_RCTA, &RctaStatus);
            if ((RctaStatus.Status == SVC_APP_STAT_RCTA_DISABLE) && ((ViewId == 0U) || (ViewId == 1U))) {
                RctaStatus.Status = SVC_APP_STAT_RCTA_ENABLE;
                Rval = SvcSysStat_Issue(SVC_APP_STAT_RCTA, &RctaStatus);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
                }
            } else if ((RctaStatus.Status == SVC_APP_STAT_RCTA_ENABLE) && (ViewId != 0U) && (ViewId != 1U)) {
                RctaStatus.Status = SVC_APP_STAT_RCTA_DISABLE;
                Rval = SvcSysStat_Issue(SVC_APP_STAT_RCTA, &RctaStatus);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
                }
            } else {
                /**do nothing*/
            }

        }
    } else {
        /**do nothing*/
    }
    return Rval;
}

#ifdef CONFIG_ICAM_BUILD_ENET
#ifdef CONFIG_AMBA_AVB
static SVC_SUR_TASK_KEY_TYPE_e ControllerDecode(const UINT8 *payload)
{
    SVC_SUR_TASK_KEY_TYPE_e Key = SVC_SUR_TASK_KEY_NUM;

    //AmbaPrint_PrintUInt5("=> 0x%x 0x%x 0x%x 0x%x", payload[0],payload[1],payload[2],payload[3],0);
    //AmbaPrint_PrintUInt5("   0x%x 0x%x 0x%x 0x%x", payload[4],payload[5],payload[6],payload[7],0);
    if (payload != NULL) {
        if (((payload[0] == 0x30U) && (payload[1] == 0x31U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x30U) && (payload[5] == 0x30U) && (payload[6] == 0x35U) && (payload[7] == 0x32U))  /* NCC: 01000052*/
                || ((payload[0] == 0x30U) && (payload[1] == 0x30U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x35U) && (payload[5] == 0x32U) && (payload[6] == 0x30U) && (payload[7] == 0x30U))) { /* non-NCC: 00005200 */
            Key = SVC_SUR_TASK_KEY_UP;
        } else if (((payload[0] == 0x30U) && (payload[1] == 0x31U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x30U) && (payload[5] == 0x30U) && (payload[6] == 0x35U) && (payload[7] == 0x31U))  /* NCC: 01000051 */
                || ((payload[0] == 0x30U) && (payload[1] == 0x30U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x35U) && (payload[5] == 0x31U) && (payload[6] == 0x30U) && (payload[7] == 0x30U))) { /* non-NCC: 00005100 */
            Key = SVC_SUR_TASK_KEY_DOWN;
        } else if (((payload[0] == 0x30U) && (payload[1] == 0x31U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x30U) && (payload[5] == 0x30U) && (payload[6] == 0x35U) && (payload[7] == 0x30U))  /* NCC: 01000050 */
                || ((payload[0] == 0x30U) && (payload[1] == 0x30U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x35U) && (payload[5] == 0x30U) && (payload[6] == 0x30U) && (payload[7] == 0x30U))) { /* non-NCC: 00005000 */
            Key = SVC_SUR_TASK_KEY_LEFT;
        } else if (((payload[0] == 0x30U) && (payload[1] == 0x31U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x30U) && (payload[5] == 0x30U) && (payload[6] == 0x34U) && (payload[7] == 0x66U))  /* NCC: 0100004f */
                || ((payload[0] == 0x30U) && (payload[1] == 0x30U) && (payload[2] == 0x30U) && (payload[3] == 0x30U) && (payload[4] == 0x34U) && (payload[5] == 0x66U) && (payload[6] == 0x30U) && (payload[7] == 0x30U))) { /* non-NCC: 00004f00 */
            Key = SVC_SUR_TASK_KEY_RIGHT;
        } else {
            SvcLog_NG(SVC_LOG_SUR_TASK, "Not support key.", 0U, 0U);
        }
    }

    return Key;
}

static void SvcSurTask_RmtCtrlCbk(const void *packet)
{
    UINT32                    RetVal = SVC_OK;
    const UINT8*              cp;
    const avdecc_pkt_t*       pkt;
    const pkt_aecp_vendor_t*  vendor;
    SVC_SUR_TASK_KEY_TYPE_e       Key;
    UINT32 ViewId = 0xFFU;

    AmbaMisra_TypeCast(&pkt, &packet);
    cp = pkt->pkt;
    AmbaMisra_TypeCast(&vendor, &cp);

    /*Use newest controller*/
    Key = ControllerDecode(&(vendor->payload[0]));
    SvcLog_OK(SVC_LOG_SUR_TASK, "SvcSurTask_RmtCtrlCbk key %d", (UINT32)Key, 0U);

    switch (Key) {
        case SVC_SUR_TASK_KEY_UP:
            ViewId = 0U;
            break;
        case SVC_SUR_TASK_KEY_DOWN:
            ViewId = 1U;
            break;
        case SVC_SUR_TASK_KEY_LEFT:
            ViewId = 2U;
            break;
        case SVC_SUR_TASK_KEY_RIGHT:
            ViewId = 3U;
            break;
        default:
            RetVal = (UINT32)SVC_SUR_TASK_KEY_NUM;
            break;
    }
    if (ViewId != 0xFFU) {
        if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcSurQueId, &ViewId, AMBA_KAL_WAIT_FOREVER)) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "AmbaKAL_MsgQueueSend failed", 0U, 0U);
        } else {
            //Do nothing
        }
    }
    if(SVC_OK != RetVal){
        SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSurTask_RmtCtrlCbk: failed (RetVal: 0x%08X)", RetVal, 0U);
    }
}

static void SvcSurTask_InitRmtCtrl(void)
{
    UINT32 Idx = 0U;

#if defined(CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined(CONFIG_BSP_CV2XFSDK_OPTION_B)
    AmbaGPIO_SetFuncGPO(GPIO_PIN_025, AMBA_GPIO_LEVEL_LOW);
    AmbaGPIO_SetFuncGPO(GPIO_PIN_025, AMBA_GPIO_LEVEL_HIGH);
#endif

    //enet init 00:11:22:33:44:55
    {
        extern AMBA_ENET_CONFIG_s EnetConfig[ENET_INSTANCES];
        extern void EnetUserTaskCreate(UINT32 Idx);
        AMBA_ENET_CONFIG_s *pEnetConfig = &EnetConfig[0];

        (void) AmbaPrint_ModuleSetAllowList(ETH_MODULE_ID, 1U);
        (void) AmbaEnet_SetConfig(Idx, &(EnetConfig[0]));
        (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);

        pEnetConfig->Mac[0] = 0x00;
        pEnetConfig->Mac[1] = 0x11;
        pEnetConfig->Mac[2] = 0x22;
        pEnetConfig->Mac[3] = 0x33;
        pEnetConfig->Mac[4] = 0x44;
        pEnetConfig->Mac[5] = 0x55;

        (void)AmbaEnet_Init(pEnetConfig);
        EnetUserTaskCreate(Idx);
    }

    // avbinit
    {
        AMBA_ENET_CONFIG_s *pEnetConfig;
        AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
        UINT32 Rval;
        extern AMBA_AVB_STACK_CONFIG_s AvbConfig[ENET_INSTANCES];
        (void) AmbaPrint_ModuleSetAllowList(AVB_MODULE_ID, 1U);
        (void) AmbaAvbStack_SetConfig(Idx, &(AvbConfig[0]));
        (void) AmbaEnet_GetConfig(Idx, &pEnetConfig);
        (void) AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
        pAvbConfig->Idx = Idx;
        pAvbConfig->PtpConfig.MstSlv = AVB_PTP_AUTO;
        if (pEnetConfig != NULL) {
            AmbaSvcWrap_MisraMemcpy(pAvbConfig->Mac, pEnetConfig->Mac, 6);
        }

        Rval = AmbaAvbStack_EnetInit(pAvbConfig);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "AmbaAvbStack_EnetInit fail", 0U, 0U);
        }

        {
            extern UINT32 avdecc_process(const UINT32 Idx,
                                         const UINT8 dmac[6],
                                         const UINT8 smac[6],
                                         const void *L3Frame,
                                         const UINT16 L3Len);

            pAvbConfig->pAvdeccCb = avdecc_process;
        }
    }

    // svdecc 0 1
    {
        AvdeccTLStatus(0, 1);
    }

    // AECP callback fuction
    cbf_vendor_unique_command = SvcSurTask_RmtCtrlCbk;

}
#endif
#endif

static void CmdCvUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_sur commands:\n");
    PrintFunc(" chgview [Param0]  :change display and cv view\n");
    PrintFunc("   Param:   'front' 'back' 'left' 'right'.\n");
    PrintFunc(" drawseg [enable/disable].\n");

}

static void CmdSurEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = SVC_OK;

    if (2U < ArgCount) {
        if (0 == SvcWrap_strcmp("chgview", pArgVector[1U])) {
            UINT32 FovId = 0U;
            if (0 == SvcWrap_strcmp("front", pArgVector[2U])) {
                FovId = 0U;
            } else if (0 == SvcWrap_strcmp("back", pArgVector[2U])) {
                FovId = 1U;
            } else if (0 == SvcWrap_strcmp("left", pArgVector[2U])) {
                FovId = 2U;
            } else if (0 == SvcWrap_strcmp("right", pArgVector[2U])) {
                FovId = 3U;
            } else {
                FovId = 0xFFU;
            }

            if (FovId != 0xFFU) {
                if (SVC_OK != AmbaKAL_MsgQueueSend(&SvcSurQueId, &FovId, AMBA_KAL_WAIT_FOREVER)) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "AmbaKAL_MsgQueueSend failed", 0U, 0U);
                } else {
                    //Do nothing
                }
            }
        } else if (0 == SvcWrap_strcmp("drawseg", pArgVector[1U])) {
            if (0 == SvcWrap_strcmp("enable", pArgVector[2U])) {
                /* Enable Seg Draw */
                Rval = SvcSegDrawTask_Start();
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_Start() failed with %d", Rval, 0U);
                }
            } else {
                /* Disable Seg Draw */
                Rval = SvcSegDrawTask_Stop();
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_Stop() failed with %d", Rval, 0U);
                }
            }
        } else {
            /**Do nothing*/
        }
    } else {
        CmdCvUsage(PrintFunc);
    }

}


/*-----------------------------------------------------------------------------------------------*\
*  @RoutineName:: SvcCmdCv_Install
*
*  @Description:: install svc cv command
*
*  @Input      ::
*
*  @Output     ::
*
*  @Return     ::
\*-----------------------------------------------------------------------------------------------*/
static void SvcCmdSur_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdCv;

    UINT32  Rval;

    SvcCmdCv.pName    = "svc_sur";
    SvcCmdCv.MainFunc = CmdSurEntry;
    SvcCmdCv.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdCv);
    if (SHELL_ERR_SUCCESS != Rval) {
     SvcLog_NG(SVC_LOG_SUR_TASK, "## fail to install svc cv command", 0U, 0U);
    }
}

static void* SvcSurTaskEntry(void* EntryArg)
{
    UINT8 IsTrue = 1U;
    UINT32 Rval;
    UINT32 ViewId;

    while (0U != IsTrue) {
        Rval = AmbaKAL_MsgQueueReceive(&SvcSurQueId, &ViewId, AMBA_KAL_WAIT_FOREVER);
        if (SVC_NG == Rval) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcLdwsQueId MsgQue Receive error !", 0U, 0U);
        } else {
            Rval = SvcSurTask_ChgView(ViewId);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSurTask_ChgView() failed with %d", Rval, 0U);
            }
        }
    }
    AmbaMisra_TouchUnused(EntryArg);

    return NULL;
}

void SvcSurTask_Init(void)
{
    static SVC_TASK_CTRL_s     SvcSurTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8               SvcSurTaskStack[SVC_SUR_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char                QueueName[] = "SvcSurQueue";
    static UINT32 SvcSurQue[SVC_SUR_QUEUE_SIZE] GNU_SECTION_NOZEROINIT;
    UINT32 Rval;

    /* create msg queue */
    if (SVC_OK != AmbaKAL_MsgQueueCreate(&SvcSurQueId,
                                    QueueName,
                                    sizeof(SVC_CV_DETRES_SEG_s),
                                    SvcSurQue,
                                    SVC_SUR_QUEUE_SIZE * sizeof(UINT32))) {
        SvcLog_NG(SVC_LOG_SUR_TASK, "SvcRctaTask_Init: Fail to create msg queue", 0U, 0U);
    } else {
        //Do nothing
    }

    /* Create LDWS task */
    SvcSurTaskCtrl.Priority   = 199U;
    SvcSurTaskCtrl.EntryFunc  = SvcSurTaskEntry;
    SvcSurTaskCtrl.pStackBase = SvcSurTaskStack;
    SvcSurTaskCtrl.StackSize  = SVC_SUR_TASK_STACK_SIZE;
    SvcSurTaskCtrl.CpuBits    = 0x1U;
    SvcSurTaskCtrl.EntryArg   = 0x0U;

    Rval = SvcTask_Create("SvcSurTask", &SvcSurTaskCtrl);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSur_TaskCreate create fail", 0U, 0U);
    }

    /**Inital Command*/
    SvcCmdSur_Install();

#ifdef CONFIG_ICAM_BUILD_ENET
#ifdef CONFIG_AMBA_AVB
    /**Inital remote control*/
    SvcSurTask_InitRmtCtrl();
#endif
#endif
    { /**default select OWS gui, because default view will be OWS+RCTA*/
        SVC_APP_STAT_OWS_s OwsStatus;
        OwsStatus.Status = SVC_APP_STAT_OWS_ENABLE;
        Rval = SvcSysStat_Issue(SVC_APP_STAT_OWS, &OwsStatus);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
        }
    }
    { /**default select RCTA gui, because default view will be OWS +RCTA*/
        SVC_APP_STAT_RCTA_s RctaStatus;
        RctaStatus.Status = SVC_APP_STAT_RCTA_ENABLE;
        Rval = SvcSysStat_Issue(SVC_APP_STAT_RCTA, &RctaStatus);
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "App Status issue failed", 0U, 0U);
        }
    }

}
