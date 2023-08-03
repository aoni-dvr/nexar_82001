/**
 *  @file SvcGui.c
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
 *  @details svc application draw gui
 *
 */

#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaFPD.h"
#include "AmbaShell.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT_Def.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#include "AmbaDspInt.h"

#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcVinSrc.h"
#include "SvcRecMain.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcGuiDiagBox.h"
#include "SvcCmd.h"
#include "SvcResCfg.h"

#include "SvcTask.h"
#include "AmbaSYS.h"

#define SVC_LOG_GUI        "GUI"



#define SVC_GUI_STACK_SIZE      (0x10000U)
#define SVC_GUI_TASK_PRI        (43U)
#define SVC_GUI_TASK_CPU_BITS   (0x02U)

#define SVC_GUI_EVENT_DRAWING_START             (1U)
#define SVC_GUI_EVENT_DRAWING_NO_UPDATE         (2U)
#define SVC_GUI_EVENT_FLUSH                     (4U)
#define SVC_GUI_EVENT_FLUSH_NEED                (8U)
#define SVC_GUI_EVENT_VOUT_POLARITY             (16U)

#define SVC_GUI_FLUSH_DELAY_TIME       (3U)

#ifdef CONFIG_ICAM_32BITS_OSD_USED
#define SVC_GUI_MARGIN_COLOR    (0xFF000000U)   /* 0xFF000000U means black */
#else
#define SVC_GUI_MARGIN_COLOR    (248U)          /* CLUT Idx 248 is 0xFF000000U, means black */
#endif

typedef struct {
    SVC_GUI_SETTING_s Config;
    UINT8             TaskStart;
    UINT8             Interval;
    UINT8             IntervalCnt;
    UINT8             FlushDelayTime;
    UINT8             ForceRedraw;
    UINT32            FlagTick[2U];
} SVC_GUI_CTRL_s;

typedef struct {
    char                TimerName[32];
    AMBA_KAL_TIMER_t    Timer;
    char                VoutTaskName[16];
    SVC_TASK_CTRL_s     VoutTaskCtrl;
    UINT8               VoutTaskStack[SVC_GUI_STACK_SIZE];
    char                VoutFlushTaskName[16];
    SVC_TASK_CTRL_s     FlushTaskCtrl;
    UINT8               FlushTaskStack[SVC_GUI_STACK_SIZE];
    AMBA_DSP_INT_PORT_s VoutIntPort;
} SVC_GUI_UPDATE_s;

typedef struct {
    UINT64              Flag;
    SVC_GUI_DRAW_FUNC   DrawFunc[SVC_GUI_LEVEL_MAX_NUM];
    SVC_GUI_UPDATE_FUNC UpdateFunc[SVC_GUI_LEVEL_MAX_NUM];
    SVC_GUI_EVENT_FUNC  EventFunc[SVC_GUI_LEVEL_MAX_NUM];
    char                Name[SVC_GUI_LEVEL_MAX_NUM][8U];
} SVC_GUI_LEVEL_s;

typedef struct {
    UINT64              LvlSkip;
    UINT8               ForceRedraw;
} SVC_GUI_DBG_s ;

typedef UINT32 (*VOUT_TRIGGER_HANDLER_f)(UINT32 VoutIdx, UINT32 Flag, UINT32 *ActualFlag);

static UINT32 Gui_CanvasClear(UINT32 VoutIdx, UINT32 Level);
static UINT32 Gui_CanvasDraw(UINT32 VoutIdx, UINT32 Level);
static UINT32 Gui_CanvasDebugDraw(UINT32 VoutIdx);
static void* Gui_VoutIntEntry(void* EntryArg);
static void Gui_TimerIntEntry(UINT32 EntryArg);
static void* Gui_Entry(void* EntryArg);
static void* Gui_FlushEntry(void* EntryArg);
static void Gui_CmdInstall(void);
static void Gui_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void Gui_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

static UINT32 VoutPolarityEvent(UINT32 VoutIdx, UINT32 Flag, UINT32 *ActualFlag);
static UINT32 VoutIntEvent(UINT32 VoutIdx, UINT32 Flag, UINT32 *ActualFlag);
static UINT32 VoutDataRdy(const void *pEventData);

static AMBA_KAL_EVENT_FLAG_t GuiEventFlag[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 GuiDebugEnable = 0U;
static UINT32 GuiDebugConut[AMBA_DSP_MAX_VOUT_NUM] = {0U};
static VOUT_TRIGGER_HANDLER_f pVoutTriggerFunc[3U];

static SVC_GUI_CTRL_s        SvcGuiCtrl[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;
static SVC_GUI_UPDATE_s      SvcGuiUpdate[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;
static SVC_GUI_LEVEL_s       SvcGuiLevel[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;
static SVC_GUI_DBG_s         SvcGuiDebug[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;


static void SvcGui_Dbg(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if (GuiDebugEnable > 0U) {
        SvcLog_OK(pModule, pFormat, Arg1, Arg2);
    }
}

/**
 * Svc Gui initialization function
 * return 0-OK, 1-NG
 */
UINT32 SvcGui_Init(void)
{
    UINT32 RetVal;
    static UINT32           GuiInitialize = 0U;
    static char             GuiEventFlagName[AMBA_DSP_MAX_VOUT_NUM][32];
    static char             GuiTaskName[AMBA_DSP_MAX_VOUT_NUM][16];
    static SVC_TASK_CTRL_s  GuiTaskCtrl[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;
    static UINT8            GuiTaskStack[AMBA_DSP_MAX_VOUT_NUM][SVC_GUI_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    if (GuiInitialize == 0U) {
        if (SVC_OK != AmbaWrap_memset(SvcGuiCtrl, 0, sizeof(SvcGuiCtrl))) {
            SvcLog_NG(SVC_LOG_GUI, "SvcGui_Init() err. AmbaWrap_memset(SvcGuiCtrl) failed", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memset(SvcGuiUpdate, 0, sizeof(SvcGuiUpdate))) {
            SvcLog_NG(SVC_LOG_GUI, "SvcGui_Init() err. AmbaWrap_memset(SvcGuiUpdate) failed", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memset(SvcGuiLevel, 0, sizeof(SvcGuiLevel))) {
            SvcLog_NG(SVC_LOG_GUI, "SvcGui_Init() err. AmbaWrap_memset(SvcGuiLevel) failed", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memset(SvcGuiDebug, 0, sizeof(SvcGuiDebug))) {
            SvcLog_NG(SVC_LOG_GUI, "SvcGui_Init() err. AmbaWrap_memset(SvcGuiDebug) failed", 0U, 0U);
        }

        for (UINT32 VoutIdx = 0U; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx++) {

            /* Create gui task entry function waiting for the gui changing event */
            GuiTaskCtrl[VoutIdx].Priority   = SVC_GUI_TASK_PRI;
            GuiTaskCtrl[VoutIdx].EntryFunc  = Gui_Entry;
            GuiTaskCtrl[VoutIdx].EntryArg   = VoutIdx;
            GuiTaskCtrl[VoutIdx].pStackBase = GuiTaskStack[VoutIdx];
            GuiTaskCtrl[VoutIdx].StackSize  = SVC_GUI_STACK_SIZE;
            GuiTaskCtrl[VoutIdx].CpuBits    = SVC_GUI_TASK_CPU_BITS;

            (void) SvcWrap_sprintfU32(GuiEventFlagName[VoutIdx], 32, "SvcGuiTimerEventFlag_%d", 1, &VoutIdx);
            (void) SvcWrap_sprintfU32(GuiTaskName[VoutIdx], 16, "SvcGuiVOUT%d", 1, &VoutIdx);

            RetVal = AmbaKAL_EventFlagCreate(&GuiEventFlag[VoutIdx], GuiEventFlagName[VoutIdx]);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGuiTimerEventFlag isn't created", 0U, 0U);
            }

            /* Create control task */
            if (SVC_OK == RetVal) {
                RetVal = SvcTask_Create(GuiTaskName[VoutIdx], &GuiTaskCtrl[VoutIdx]);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI, "SvcGui isn't created", 0U, 0U);
                }
            }
        }

        Gui_CmdInstall();

        if (SVC_OK == RetVal) {
            GuiInitialize = 1U;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;

}

/**
 * Svc Gui start function
 * @param [in] VoutIdx Vout index
 * @param [in] pConfig Configuration
 * return 0-OK, 1-NG
 */
UINT32 SvcGui_Start(UINT32 VoutIdx, const SVC_GUI_SETTING_s *pConfig)
{
    UINT32 RetVal = SVC_OK;

    if (pConfig == NULL) {
        SvcLog_NG(SVC_LOG_GUI, "SvcGui_Start() pConfig cannot be NULL", 0U, 0U);
        RetVal = SVC_NG;
    } else /* pConfig != NULL */ {
        if (SVC_OK != AmbaWrap_memcpy(&SvcGuiCtrl[VoutIdx].Config, pConfig, sizeof(SVC_GUI_SETTING_s))) {
            SvcLog_NG(SVC_LOG_GUI, "SvcGui_Start() erro. AmbaWrap_memcpy() failed", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    pVoutTriggerFunc[SVC_GUI_TYPE_VOUT] = VoutIntEvent;
    pVoutTriggerFunc[SVC_GUI_TYPE_POLARITY] = VoutPolarityEvent;

    if ((SVC_OK == RetVal) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM) && (SvcGuiCtrl[VoutIdx].TaskStart == 0U)) {
        if ( (SvcGuiCtrl[VoutIdx].Config.Type == (UINT8) SVC_GUI_TYPE_VOUT) || (SvcGuiCtrl[VoutIdx].Config.Type == (UINT8) SVC_GUI_TYPE_POLARITY) ) {
            if ((SvcGuiCtrl[VoutIdx].Config.VoutFrameRate.TimeScale == 0U) || (SvcGuiCtrl[VoutIdx].Config.VoutFrameRate.NumUnitsInTick == 0U)) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGui_Start() pConfig->VoutFrameRate err", 0U, 0U);
                RetVal = SVC_NG;
            } else if ((SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.TimeScale == 0U) || (SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.NumUnitsInTick == 0U)) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGui_Start() pConfig->MaxFrameRate err", 0U, 0U);
                RetVal = SVC_NG;
            } else {
                DOUBLE FrameRate;
                DOUBLE MaxFrameRate;
                DOUBLE DelayTime;
                UINT32 Adjust = 0U;
                DOUBLE Divider = 1.0;

                if (SvcGuiCtrl[VoutIdx].Config.Type == (UINT8) SVC_GUI_TYPE_POLARITY){
                    Divider = 2.0;
                }

                FrameRate = (DOUBLE) SvcGuiCtrl[VoutIdx].Config.VoutFrameRate.TimeScale / (DOUBLE) SvcGuiCtrl[VoutIdx].Config.VoutFrameRate.NumUnitsInTick;
                MaxFrameRate = (DOUBLE) SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.TimeScale / (DOUBLE) SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.NumUnitsInTick;
                if (MaxFrameRate < FrameRate){
                    DelayTime = (1000.0 / MaxFrameRate) - (DOUBLE) SVC_GUI_FLUSH_DELAY_TIME;
                } else {
                    DelayTime = (1000.0 / FrameRate) - (DOUBLE) SVC_GUI_FLUSH_DELAY_TIME;
                }

                SvcGuiCtrl[VoutIdx].IntervalCnt = 0U;
                SvcGuiCtrl[VoutIdx].Interval = 1U;
                SvcGuiCtrl[VoutIdx].FlushDelayTime = (UINT8) DelayTime;

                if ((FrameRate/Divider) > MaxFrameRate) {
                    Adjust = 1U;
                }

                while (Adjust > 0U) {
                    SvcGuiCtrl[VoutIdx].Interval = SvcGuiCtrl[VoutIdx].Interval + 1U;
                    FrameRate = (DOUBLE) SvcGuiCtrl[VoutIdx].Config.VoutFrameRate.TimeScale / (DOUBLE) SvcGuiCtrl[VoutIdx].Config.VoutFrameRate.NumUnitsInTick;
                    FrameRate = (FrameRate / Divider) / (DOUBLE) SvcGuiCtrl[VoutIdx].Interval;

                    if ( FrameRate > MaxFrameRate) {
                        Adjust = 1U;
                    } else {
                        Adjust = 0U;
                    }
                }

                {

                    if (SvcGuiCtrl[VoutIdx].Config.Type == (UINT8) SVC_GUI_TYPE_VOUT){
                        if (SVC_OK != AmbaDspInt_Open(&SvcGuiUpdate[VoutIdx].VoutIntPort)) {
                            SvcLog_NG(SVC_LOG_GUI, "AmbaDspInt_Open() failed", 0U, 0U);
                        }
                    } else { // SVC_GUI_TYPE_POLARITY
                        if (SVC_OK != AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VOUT_DATA_RDY, VoutDataRdy)){
                            SvcLog_NG(SVC_LOG_GUI, "## fail to register Vout Ready", 0U, 0U);
                        }
                    }

                    SvcGuiUpdate[VoutIdx].VoutTaskCtrl.Priority   = 5U;
                    SvcGuiUpdate[VoutIdx].VoutTaskCtrl.EntryFunc  = Gui_VoutIntEntry;
                    SvcGuiUpdate[VoutIdx].VoutTaskCtrl.EntryArg   = VoutIdx;
                    SvcGuiUpdate[VoutIdx].VoutTaskCtrl.pStackBase = SvcGuiUpdate[VoutIdx].VoutTaskStack;
                    SvcGuiUpdate[VoutIdx].VoutTaskCtrl.StackSize  = SVC_GUI_STACK_SIZE;
                    SvcGuiUpdate[VoutIdx].VoutTaskCtrl.CpuBits    = SVC_GUI_TASK_CPU_BITS;

                    (void) SvcWrap_sprintfU32(SvcGuiUpdate[VoutIdx].VoutTaskName, 16, "SvcGuiIntVOUT%d", 1, &VoutIdx);
                    if (SVC_OK != SvcTask_Create(SvcGuiUpdate[VoutIdx].VoutTaskName, &SvcGuiUpdate[VoutIdx].VoutTaskCtrl)) {
                        SvcLog_NG(SVC_LOG_GUI, "GuiVoutTask create failed", 0U, 0U);
                    }


                    SvcGuiUpdate[VoutIdx].FlushTaskCtrl.Priority   = 5U;
                    SvcGuiUpdate[VoutIdx].FlushTaskCtrl.EntryFunc  = Gui_FlushEntry;
                    SvcGuiUpdate[VoutIdx].FlushTaskCtrl.EntryArg   = VoutIdx;
                    SvcGuiUpdate[VoutIdx].FlushTaskCtrl.pStackBase = SvcGuiUpdate[VoutIdx].FlushTaskStack;
                    SvcGuiUpdate[VoutIdx].FlushTaskCtrl.StackSize  = SVC_GUI_STACK_SIZE;
                    SvcGuiUpdate[VoutIdx].FlushTaskCtrl.CpuBits    = SVC_GUI_TASK_CPU_BITS;

                    (void) SvcWrap_sprintfU32(SvcGuiUpdate[VoutIdx].VoutFlushTaskName, 16, "SvcGuiFlushV%d", 1, &VoutIdx);
                    if (SVC_OK != SvcTask_Create(SvcGuiUpdate[VoutIdx].VoutFlushTaskName, &SvcGuiUpdate[VoutIdx].FlushTaskCtrl)) {
                        SvcLog_NG(SVC_LOG_GUI, "VoutFlushTaskName create failed", 0U, 0U);
                    }

                }
            }
        } else /* SvcGuiCtrl[VoutIdx].Config.Type == SVC_GUI_TYPE_TIMER */ {
            if ((SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.TimeScale == 0U) || (SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.NumUnitsInTick == 0U)) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGui_Start() pConfig->MaxFrameRate err", 0U, 0U);
                RetVal = SVC_NG;
            } else {
                DOUBLE MaxFrameRate;
                DOUBLE DelayTime;
                DOUBLE UpdateTime;

                MaxFrameRate = (DOUBLE) SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.TimeScale / (DOUBLE) SvcGuiCtrl[VoutIdx].Config.MaxFrameRate.NumUnitsInTick;
                DelayTime = (1000.0 / MaxFrameRate) - (DOUBLE) SVC_GUI_FLUSH_DELAY_TIME;
                UpdateTime = (1000.0 / MaxFrameRate);

                SvcGuiCtrl[VoutIdx].IntervalCnt = 0U;
                SvcGuiCtrl[VoutIdx].Interval = 1U;
                SvcGuiCtrl[VoutIdx].FlushDelayTime = (UINT8) DelayTime;

                SvcGuiUpdate[VoutIdx].FlushTaskCtrl.Priority   = 10U;
                SvcGuiUpdate[VoutIdx].FlushTaskCtrl.EntryFunc  = Gui_FlushEntry;
                SvcGuiUpdate[VoutIdx].FlushTaskCtrl.EntryArg   = VoutIdx;
                SvcGuiUpdate[VoutIdx].FlushTaskCtrl.pStackBase = SvcGuiUpdate[VoutIdx].FlushTaskStack;
                SvcGuiUpdate[VoutIdx].FlushTaskCtrl.StackSize  = SVC_GUI_STACK_SIZE;
                SvcGuiUpdate[VoutIdx].FlushTaskCtrl.CpuBits    = SVC_GUI_TASK_CPU_BITS;

                (void) SvcWrap_sprintfU32(SvcGuiUpdate[VoutIdx].VoutFlushTaskName, 16, "SvcGuiFlushV%d", 1, &VoutIdx);
                if (SVC_OK != SvcTask_Create(SvcGuiUpdate[VoutIdx].VoutFlushTaskName, &SvcGuiUpdate[VoutIdx].FlushTaskCtrl)) {
                    SvcLog_NG(SVC_LOG_GUI, "VoutFlushTaskName create failed", 0U, 0U);
                }
                (void) SvcWrap_sprintfU32(SvcGuiUpdate[VoutIdx].TimerName, 32, "SvcGuiTimer_%d", 1, &VoutIdx);
                RetVal = AmbaKAL_TimerCreate(&SvcGuiUpdate[VoutIdx].Timer, SvcGuiUpdate[VoutIdx].TimerName, Gui_TimerIntEntry, VoutIdx, (UINT32) UpdateTime, (UINT32) UpdateTime, AMBA_KAL_DONT_START);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI, "SvcGui timer create failed", 0U, 0U);
                }

                RetVal = AmbaKAL_TimerStart(&SvcGuiUpdate[VoutIdx].Timer);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI, "SvcGui_Start() Timer Start err 0x%x", RetVal, 0U);
                }
            }
        }
        SvcGuiCtrl[VoutIdx].TaskStart = 1U;
        SvcGuiCtrl[VoutIdx].ForceRedraw = 1U;
    } else {
        SvcLog_NG(SVC_LOG_GUI, "SvcGui for VoutIdx %d has started already", VoutIdx, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 * Svc Gui stop function
 * @param [in] VoutIdx Vout index
 * return 0-OK, 1-NG
 */
UINT32 SvcGui_Stop(UINT32 VoutIdx)
{
    UINT32 RetVal = SVC_OK;

    if ((VoutIdx < AMBA_DSP_MAX_VOUT_NUM) && (SvcGuiCtrl[VoutIdx].TaskStart == 1U)) {
        SvcGuiCtrl[VoutIdx].TaskStart = 0U;
        if (SvcGuiCtrl[VoutIdx].Config.Type == (UINT8) SVC_GUI_TYPE_VOUT) {
            if (SVC_OK != SvcTask_Destroy(&SvcGuiUpdate[VoutIdx].VoutTaskCtrl)) {
                SvcLog_NG(SVC_LOG_GUI, "GuiVoutTask destroy failed", 0U, 0U);
            }
            if (SVC_OK != AmbaDspInt_Close(&SvcGuiUpdate[VoutIdx].VoutIntPort)) {
                SvcLog_NG(SVC_LOG_GUI, "AmbaDspInt_Close() failed", 0U, 0U);
            }
            if (SVC_OK != SvcTask_Destroy(&SvcGuiUpdate[VoutIdx].FlushTaskCtrl)) {
                SvcLog_NG(SVC_LOG_GUI, "FlushTaskCtrl destroy failed", 0U, 0U);
            }
        } else if (SvcGuiCtrl[VoutIdx].Config.Type == (UINT8) SVC_GUI_TYPE_POLARITY) {
            if (SVC_OK != SvcTask_Destroy(&SvcGuiUpdate[VoutIdx].VoutTaskCtrl)) {
                SvcLog_NG(SVC_LOG_GUI, "GuiVoutTask destroy failed", 0U, 0U);
            }
            if (SVC_OK != SvcTask_Destroy(&SvcGuiUpdate[VoutIdx].FlushTaskCtrl)) {
                SvcLog_NG(SVC_LOG_GUI, "FlushTaskCtrl destroy failed", 0U, 0U);
            }
            if (SVC_OK != AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VOUT_DATA_RDY, VoutDataRdy)){
                SvcLog_NG(SVC_LOG_GUI, "## fail to unregister Vout Ready", 0U, 0U);
            }
        } else /* SvcGuiCtrl[VoutIdx].Config.Type == SVC_GUI_TYPE_TIMER */ {
            if (SVC_OK != SvcTask_Destroy(&SvcGuiUpdate[VoutIdx].FlushTaskCtrl)) {
                SvcLog_NG(SVC_LOG_GUI, "FlushTaskCtrl destroy failed", 0U, 0U);
            }
            if (SVC_OK != AmbaKAL_TimerStop(&SvcGuiUpdate[VoutIdx].Timer)) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGui timer stop failed", 0U, 0U);
            }
            if (SVC_OK != AmbaKAL_TimerDelete(&SvcGuiUpdate[VoutIdx].Timer)) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGui timer delete failed", 0U, 0U);
            }

        }
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_GUI, "SvcGui for VoutIdx %d has stopped already", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 * Svc Gui registration function
 * @param [in] VoutIdx Vout index
 * @param [in] Level level of Gui
 * @param [in] pName canvas description
 * @param [in] pDrawFunc call back draw function
 * @param [in] pUpdateFunc call back update function
 * return None
 */
void SvcGui_Register(UINT32 VoutIdx, UINT32 Level, const char *pName, SVC_GUI_DRAW_FUNC pDrawFunc, SVC_GUI_UPDATE_FUNC pUpdateFunc)
{
    if ((Level < SVC_GUI_LEVEL_MAX_NUM) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM)) {
        if ((SvcGuiLevel[VoutIdx].Flag & ((UINT64) 1U << Level)) == 0U) {
            /* Marked this level is used */
            SvcGuiLevel[VoutIdx].Flag |= ((UINT64) 1U << Level);
            /* Register the draw function */
            SvcGuiLevel[VoutIdx].DrawFunc[Level] = pDrawFunc;
            /* Register the update function */
            SvcGuiLevel[VoutIdx].UpdateFunc[Level] = pUpdateFunc;
            /* Register the level name */
            if (SVC_OK != AmbaWrap_memcpy(SvcGuiLevel[VoutIdx].Name[Level], pName, sizeof(SvcGuiLevel[VoutIdx].Name[Level]))) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGui_Register() err, AmbaWrap_memcpy() failed", 0U, 0U);
            }

            SvcGuiCtrl[VoutIdx].ForceRedraw = 1U;
        } else {
            SvcLog_NG(SVC_LOG_GUI, "SvcGui_Register failed(), the Level (%d) is occupied %d", Level, VoutIdx);
        }
    }
}

/**
 * Svc Gui unregistration function
 * @param [in] VoutIdx Vout index
 * @param [in] Level level of Gui
 * return None
 */
void SvcGui_Unregister(UINT32 VoutIdx, UINT32 Level)
{
    UINT64 Mask;

    if ((Level < SVC_GUI_LEVEL_MAX_NUM) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM)) {
        if ((SvcGuiLevel[VoutIdx].Flag & ((UINT64) 1U << Level)) > 0U) {
            Mask = ~((UINT64) 1U << Level);
            /* Marked this level is unused */
            SvcGuiLevel[VoutIdx].Flag &= Mask;
            /* Unregister the draw function */
            SvcGuiLevel[VoutIdx].DrawFunc[Level] = NULL;
            /* Unregister the update function */
            SvcGuiLevel[VoutIdx].UpdateFunc[Level] = NULL;
            /* Unregister the level name */
            if (SVC_OK != AmbaWrap_memset(SvcGuiLevel[VoutIdx].Name[Level], 0, sizeof(SvcGuiLevel[VoutIdx].Name[Level]))) {
                SvcLog_NG(SVC_LOG_GUI, "SvcGui_Unregister() err, AmbaWrap_memcpy() failed", 0U, 0U);
            }
            SvcGuiCtrl[VoutIdx].ForceRedraw = 1U;
        }
    }
}

void SvcGui_EventRegister(UINT32 VoutIdx, UINT32 Level, SVC_GUI_EVENT_FUNC pEventFunc)
{
    if ((Level < SVC_GUI_LEVEL_MAX_NUM) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM)) {
        if ((SvcGuiLevel[VoutIdx].Flag & ((UINT64) 1U << Level)) == 0U) {
            /* Register the draw function */
            SvcGuiLevel[VoutIdx].EventFunc[Level] = pEventFunc;
        }
    } else {
        SvcLog_NG(SVC_LOG_GUI, "SvcGui_EventRegister failed(), the Level (%d) is occupied %d", Level, VoutIdx);
    }
}

/**
 * Svc Gui debug enable/disable function
 * @param [in] DebugEnable enable/disable debug message
 * return None
 */
void SvcGui_DebugEnable(UINT32 DebugEnable)
{
    if (DebugEnable == 0U) {
        GuiDebugEnable = 0U;
    } else {
        GuiDebugEnable = 1U;
    }

    SvcLog_OK(SVC_LOG_GUI, "SvcGui_DebugEnable = %d", GuiDebugEnable, 0U);
}

static UINT32 Gui_EventIssue(UINT32 VoutIdx, UINT32 Level, UINT32 EventID)
{
    if ((Level < SVC_GUI_LEVEL_MAX_NUM) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM)) {
        if (SvcGuiLevel[VoutIdx].EventFunc[Level] != NULL) {
            SvcGuiLevel[VoutIdx].EventFunc[Level](VoutIdx, Level, EventID, NULL);
        }
    }

    return SVC_OK;
}

static UINT32 Gui_CanvasClear(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal;

    if ((Level == 0U) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM)) {
        RetVal = SvcOsd_Clear(VoutIdx);
    } else {
        RetVal = SVC_NG;
    }
    return RetVal;
}

static UINT32 Gui_CanvasUpdate(UINT32 VoutIdx, UINT32 Level, UINT64 *pUpdateLevel)
{
    UINT32 RetVal;
    if ((Level < SVC_GUI_LEVEL_MAX_NUM) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM)) {
        if (((SvcGuiLevel[VoutIdx].Flag & ((UINT64) 1U << Level)) > 0U)) {
            UINT32 Update = 0U;
            if (SvcGuiLevel[VoutIdx].UpdateFunc[Level] != NULL) {
                SvcGuiLevel[VoutIdx].UpdateFunc[Level](VoutIdx, Level, &Update);
                if (Update > 0U) {
                    *pUpdateLevel = *pUpdateLevel | ((UINT64) 1U << Level);
                }
            }
            RetVal = SVC_OK;
        } else {
            RetVal = SVC_OK;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 Gui_CanvasDraw(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal;
    if ((Level < SVC_GUI_LEVEL_MAX_NUM) && (VoutIdx < AMBA_DSP_MAX_VOUT_NUM)) {
        if (((SvcGuiLevel[VoutIdx].Flag & ((UINT64) 1U << Level)) > 0U)) {
            if (SvcGuiLevel[VoutIdx].DrawFunc[Level] != NULL) {
                /* Draw the canvas */
                SvcGuiLevel[VoutIdx].DrawFunc[Level](VoutIdx, Level);
            }
            RetVal = SVC_OK;
        } else {
            RetVal = SVC_OK;
        }
    } else {
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 Gui_CanvasDebugDraw(UINT32 VoutIdx)
{
#define FONT_SIZE             (5U)
#define FONT_WIDTH            ((FONT_SIZE) * (SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP))
#define FONT_HEIGHT           ((FONT_SIZE) * (SVC_OSD_CHAR_HEIGHT))
#define FONT_BACKGROUND_COLOR (0U)
#define FONT_COLOR            (255U)

    UINT32 RetVal = SVC_OK;

    if (VoutIdx < AMBA_DSP_MAX_VOUT_NUM) {
        char CountString[16];

        RetVal = SvcOsd_DrawSolidRect(VoutIdx, 0U, 0U, FONT_WIDTH * 16U, FONT_HEIGHT, FONT_BACKGROUND_COLOR);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_GUI, "Gui_CanvasDebugDraw SvcOsd_DrawSolidRect failed with 0x%x", RetVal, 0U);
        }

        (void) SvcWrap_sprintfU32(CountString, 16, "%u", 1, &GuiDebugConut[VoutIdx]);

        RetVal = SvcOsd_DrawString(VoutIdx, 0U, 0U, FONT_SIZE, FONT_COLOR, CountString);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_GUI, "Gui_CanvasDebugDraw SvcOsd_DrawString failed with 0x%x", RetVal, 0U);
        }

    }
    return SVC_OK;
}

static UINT32 Gui_MarginDraw(UINT32 VoutIdx)
{
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32 i;

    const SVC_DISP_STRM_s *pDispStrm = NULL;

    for (i = 0U; i < pCfg->DispNum; i++) {
        if (pCfg->DispStrm[i].VoutID == VoutIdx) {
            pDispStrm = &pCfg->DispStrm[i];
            break;
        }
    }

    if (pDispStrm != NULL) {
        INT32 WinXstart, WinXend, WinYstart, WinYend;
        DOUBLE OsdXstart, OsdXend, OsdYstart, OsdYend;
        DOUBLE RotateSwap;

        DOUBLE RatioHor, RatioVer;
        UINT32 OsdBufWidth, OsdBufHeight;
        UINT32 MixerWidth, MixerHeight;     /* MixerOffsetX and MixerOffsetY must be 0 */
        UINT32 VideoWinWidth, VideoWinHeight, VideoWinOffsetX, VideoWinOffsetY;
        UINT32 VideoRotateFlip = pDispStrm->VideoRotateFlip;

        MixerWidth  = (UINT32) pDispStrm->StrmCfg.MaxWin.Width;
        MixerHeight = (UINT32) pDispStrm->StrmCfg.MaxWin.Height;
        VideoWinWidth  = (UINT32) pDispStrm->StrmCfg.Win.Width;
        VideoWinHeight = (UINT32) pDispStrm->StrmCfg.Win.Height;
        VideoWinOffsetX = (MixerWidth - VideoWinWidth) >> 1U;
        VideoWinOffsetY = (MixerHeight - VideoWinHeight) >> 1U;

        (void) SvcOsd_GetOsdBufSize(VoutIdx, &OsdBufWidth, &OsdBufHeight);
        RatioHor = ((DOUBLE) MixerWidth) / ((DOUBLE) OsdBufWidth);
        RatioVer = ((DOUBLE) MixerHeight) / ((DOUBLE) OsdBufHeight);

        for (i = 0U; i < pDispStrm->StrmCfg.NumChan; i++) {
            UINT32 FovChanWidth, FovChanHeight, FovChanOffsetX, FovChanOffsetY;
            UINT32 MarginTop, MarginRight, MarginBot, MarginLeft;

            FovChanWidth   = (UINT32) pDispStrm->StrmCfg.ChanCfg[i].DstWin.Width;
            FovChanHeight  = (UINT32) pDispStrm->StrmCfg.ChanCfg[i].DstWin.Height;
            FovChanOffsetX = (UINT32) pDispStrm->StrmCfg.ChanCfg[i].DstWin.OffsetX;
            FovChanOffsetY = (UINT32) pDispStrm->StrmCfg.ChanCfg[i].DstWin.OffsetY;

            MarginTop   = pDispStrm->StrmCfg.ChanCfg[i].Margin.Top;
            MarginRight = pDispStrm->StrmCfg.ChanCfg[i].Margin.Right;
            MarginBot   = pDispStrm->StrmCfg.ChanCfg[i].Margin.Bottom;
            MarginLeft  = pDispStrm->StrmCfg.ChanCfg[i].Margin.Left;

            /* Mixer coordinate */
            WinXstart = (INT32) VideoWinOffsetX + (INT32) FovChanOffsetX;
            WinXend   = (INT32) VideoWinOffsetX + (INT32) FovChanOffsetX + (INT32) FovChanWidth;
            WinYstart = (INT32) VideoWinOffsetY + (INT32) FovChanOffsetY;
            WinYend   = (INT32) VideoWinOffsetY + (INT32) FovChanOffsetY + (INT32) FovChanHeight;

            /* Margin Top */
            /* Mixer coordinate */
            OsdXstart = (DOUBLE) WinXstart - (DOUBLE) MarginLeft;
            OsdXend   = (DOUBLE) WinXend + (DOUBLE) MarginRight;
            OsdYstart = (DOUBLE) WinYend - (DOUBLE) MarginTop;
            OsdYend   = (DOUBLE) WinYend;

            if (OsdXstart < 0.0) { OsdXstart = 0.0; };
            if (OsdXend > (DOUBLE) MixerWidth) { OsdXend = (DOUBLE) MixerWidth; };
            if (OsdYstart < 0.0) { OsdYstart = 0.0; };
            if (OsdYend > (DOUBLE) MixerHeight) { OsdYend = (DOUBLE) MixerHeight; };

            /* Mixer coordinate -> Osd Buffer coordinate */
            OsdXstart = OsdXstart / RatioHor;
            OsdXend   = OsdXend / RatioHor;
            OsdYstart = OsdYstart / RatioVer;
            OsdYend   = OsdYend / RatioVer;
            if (VideoRotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                OsdXstart = ((DOUBLE) OsdBufWidth) - OsdXstart;
                OsdXend   = ((DOUBLE) OsdBufWidth) - OsdXend;
                /* Swap */
                RotateSwap = OsdXstart;
                OsdXstart  = OsdXend;
                OsdXend    = RotateSwap;
            }
            if ((OsdXstart != OsdXend) && (OsdYstart != OsdYend)) {
                (void) SvcOsd_DrawSolidRect(VoutIdx, (UINT32) OsdXstart, (UINT32) OsdYstart, (UINT32) OsdXend, (UINT32) OsdYend, SVC_GUI_MARGIN_COLOR);
            }

            /* Margin Right */
            /* Mixer coordinate */
            OsdXstart = (DOUBLE) WinXend;
            OsdXend   = (DOUBLE) WinXend + (DOUBLE) MarginRight;
            OsdYstart = (DOUBLE) WinYstart - (DOUBLE) MarginTop;
            OsdYend   = (DOUBLE) WinYend + (DOUBLE) MarginBot;

            if (OsdXstart < 0.0) { OsdXstart = 0.0; };
            if (OsdXend > (DOUBLE) MixerWidth) { OsdXend = (DOUBLE) MixerWidth; };
            if (OsdYstart < 0.0) { OsdYstart = 0.0; };
            if (OsdYend > (DOUBLE) MixerHeight) { OsdYend = (DOUBLE) MixerHeight; };

            /* Mixer coordinate -> Osd Buffer coordinate */
            OsdXstart = OsdXstart / RatioHor;
            OsdXend   = OsdXend / RatioHor;
            OsdYstart = OsdYstart / RatioVer;
            OsdYend   = OsdYend / RatioVer;
            if (VideoRotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                OsdXstart = ((DOUBLE) OsdBufWidth) - OsdXstart;
                OsdXend   = ((DOUBLE) OsdBufWidth) - OsdXend;
                /* Swap */
                RotateSwap = OsdXstart;
                OsdXstart  = OsdXend;
                OsdXend    = RotateSwap;
            }
            if ((OsdXstart != OsdXend) && (OsdYstart != OsdYend)) {
                (void) SvcOsd_DrawSolidRect(VoutIdx, (UINT32) OsdXstart, (UINT32) OsdYstart, (UINT32) OsdXend, (UINT32) OsdYend, SVC_GUI_MARGIN_COLOR);
            }

            /* Margin Bottom */
            /* Mixer coordinate */
            OsdXstart = (DOUBLE) WinXstart - (DOUBLE) MarginLeft;
            OsdXend   = (DOUBLE) WinXend + (DOUBLE) MarginRight;
            OsdYstart = (DOUBLE) WinYend;
            OsdYend   = (DOUBLE) WinYend + (DOUBLE) MarginBot;

            if (OsdXstart < 0.0) { OsdXstart = 0.0; };
            if (OsdXend > (DOUBLE) MixerWidth) { OsdXend = (DOUBLE) MixerWidth; };
            if (OsdYstart < 0.0) { OsdYstart = 0.0; };
            if (OsdYend > (DOUBLE) MixerHeight) { OsdYend = (DOUBLE) MixerHeight; };

            /* Mixer coordinate -> Osd Buffer coordinate */
            OsdXstart = OsdXstart / RatioHor;
            OsdXend   = OsdXend / RatioHor;
            OsdYstart = OsdYstart / RatioVer;
            OsdYend   = OsdYend / RatioVer;
            if (VideoRotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                OsdXstart = ((DOUBLE) OsdBufWidth) - OsdXstart;
                OsdXend   = ((DOUBLE) OsdBufWidth) - OsdXend;
                /* Swap */
                RotateSwap = OsdXstart;
                OsdXstart  = OsdXend;
                OsdXend    = RotateSwap;
            }
            if ((OsdXstart != OsdXend) && (OsdYstart != OsdYend)) {
                (void) SvcOsd_DrawSolidRect(VoutIdx, (UINT32) OsdXstart, (UINT32) OsdYstart, (UINT32) OsdXend, (UINT32) OsdYend, SVC_GUI_MARGIN_COLOR);
            }

            /* Margin Left */
            /* Mixer coordinate */
            OsdXstart = (DOUBLE) WinXstart - (DOUBLE) MarginLeft;
            OsdXend   = (DOUBLE) WinXstart;
            OsdYstart = (DOUBLE) WinYstart - (DOUBLE) MarginTop;
            OsdYend   = (DOUBLE) WinYend + (DOUBLE) MarginBot;

            if (OsdXstart < 0.0) { OsdXstart = 0.0; };
            if (OsdXend > (DOUBLE) MixerWidth) { OsdXend = (DOUBLE) MixerWidth; };
            if (OsdYstart < 0.0) { OsdYstart = 0.0; };
            if (OsdYend > (DOUBLE) MixerHeight) { OsdYend = (DOUBLE) MixerHeight; };

            /* Mixer coordinate -> Osd Buffer coordinate */
            OsdXstart = OsdXstart / RatioHor;
            OsdXend   = OsdXend / RatioHor;
            OsdYstart = OsdYstart / RatioVer;
            OsdYend   = OsdYend / RatioVer;
            if (VideoRotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                OsdXstart = ((DOUBLE) OsdBufWidth) - OsdXstart;
                OsdXend   = ((DOUBLE) OsdBufWidth) - OsdXend;
                /* Swap */
                RotateSwap = OsdXstart;
                OsdXstart  = OsdXend;
                OsdXend    = RotateSwap;
            }
            if ((OsdXstart != OsdXend) && (OsdYstart != OsdYend)) {
                (void) SvcOsd_DrawSolidRect(VoutIdx, (UINT32) OsdXstart, (UINT32) OsdYstart, (UINT32) OsdXend, (UINT32) OsdYend, SVC_GUI_MARGIN_COLOR);
            }
        }
    }

    return SVC_OK;
}

static UINT32 VoutPolarityEvent(UINT32 VoutIdx, UINT32 Flag, UINT32 *ActualFlag)
{
    return AmbaKAL_EventFlagGet(&GuiEventFlag[VoutIdx], Flag, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, ActualFlag, AMBA_KAL_WAIT_FOREVER);
}

static UINT32 VoutIntEvent(UINT32 VoutIdx, UINT32 Flag, UINT32 *ActualFlag)
{
    return AmbaDspInt_Take(&SvcGuiUpdate[VoutIdx].VoutIntPort, Flag, ActualFlag, AMBA_KAL_WAIT_FOREVER);
}

static void Gui_TimerIntEntry(UINT32 EntryArg)
{
    UINT32 RetVal;
    UINT32 VoutIdx = EntryArg;
    AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;

    RetVal = AmbaKAL_EventFlagQuery(&GuiEventFlag[VoutIdx], &EventFlagInfo);
    if (SVC_OK == RetVal) {
        if ((EventFlagInfo.CurrentFlags & SVC_GUI_EVENT_DRAWING_START) > 0U) {
            RetVal = AmbaKAL_EventFlagSet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_FLUSH);
        } else {
            RetVal = AmbaKAL_EventFlagSet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_DRAWING_START | SVC_GUI_EVENT_FLUSH);
        }
        if(RetVal != SVC_OK){
            SvcLog_NG(SVC_LOG_GUI, "Gui_TimerIntEntry (%d) : Set Flag failed!!", VoutIdx, 0U);
        }
    }
    AmbaMisra_TouchUnused(&RetVal);

}

static void* Gui_FlushEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK, i, ReceiveDiffs;
    UINT32 VoutIdx, ActualFlag, SleepTime;
    AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;
    const ULONG  *pArg;
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    VoutIdx = (UINT32)(*pArg);

    while (RetVal == SVC_OK) {
        (void)AmbaKAL_EventFlagGet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_FLUSH, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlag, AMBA_KAL_WAIT_FOREVER);
        (void)AmbaSYS_GetOrcTimer(&(SvcGuiCtrl[VoutIdx].FlagTick[1U]));

        if(SvcGuiCtrl[VoutIdx].FlagTick[0U] > SvcGuiCtrl[VoutIdx].FlagTick[1U]){
            ReceiveDiffs = (SvcGuiCtrl[VoutIdx].FlagTick[1U] + (0xFFFFFFFFU - SvcGuiCtrl[VoutIdx].FlagTick[0U])) / 12288U;
        } else {
            ReceiveDiffs = (SvcGuiCtrl[VoutIdx].FlagTick[1U] - SvcGuiCtrl[VoutIdx].FlagTick[0U]) / 12288U;
        }

        SleepTime = SvcGuiCtrl[VoutIdx].FlushDelayTime - ReceiveDiffs;
        SvcGui_Dbg(SVC_LOG_GUI, "Gui_FlushEntry (%d) : Will Sleep [%u] ms", VoutIdx, SleepTime);

        if (ReceiveDiffs > SvcGuiCtrl[VoutIdx].FlushDelayTime){
            SvcGui_Dbg(SVC_LOG_GUI, "### Gui_FlushEntry (%d): Interrupt diff:[%u] is larger than frame Interval###", VoutIdx, ReceiveDiffs);
            continue;
        }

        (void)AmbaKAL_TaskSleep(SleepTime);

        RetVal = AmbaKAL_EventFlagQuery(&GuiEventFlag[VoutIdx], &EventFlagInfo);
        if (RetVal == SVC_OK){
            if ((EventFlagInfo.CurrentFlags & SVC_GUI_EVENT_FLUSH_NEED) > 0U) {
                RetVal = SvcOsd_Flush(VoutIdx);
                if (SVC_OK == RetVal) {
                    SvcGui_Dbg(SVC_LOG_GUI, "Gui_FlushEntry (%d) : Flush!", VoutIdx, 0U);
                }
                RetVal = AmbaKAL_EventFlagClear(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_FLUSH_NEED);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI, "Gui_FlushEntry (%d) : Clear Flag failed! SVC_GUI_EVENT_FLUSH_NEED", VoutIdx, 0U);
                }
                for (i = 0U; i < SVC_GUI_LEVEL_MAX_NUM; i++) {
                    (void) Gui_EventIssue(VoutIdx, i, SVC_GUI_EVENT_OSD_FLUSH);
                }
            } else if ((EventFlagInfo.CurrentFlags & SVC_GUI_EVENT_DRAWING_NO_UPDATE) > 0U){
                (void)AmbaKAL_EventFlagClear(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_DRAWING_NO_UPDATE);
                SvcGui_Dbg(SVC_LOG_GUI, "Gui_FlushEntry (%d) : Nothing need to be updated", VoutIdx, 0U);
            }
            else {
                SvcGui_Dbg(SVC_LOG_GUI, "Gui_FlushEntry (%d) : OSD buffer is still drawing", VoutIdx, 0U);
                /*Do nothing*/
            }
        } else {
            SvcLog_NG(SVC_LOG_GUI, "Gui_FlushEntry (%d) : Get Flag failed! SVC_GUI_EVENT_FLUSH_TIME", VoutIdx, 0U);
        }
    }

    return NULL;
}


static void* Gui_VoutIntEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 Flag = 0U;
    UINT32 ActualFlag = 0U;
    UINT32 VoutIdx;
    const ULONG  *pArg;
    UINT8 GuiType = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    VoutIdx = (UINT32)(*pArg);
    GuiType = SvcGuiCtrl[VoutIdx].Config.Type;

    while (VoutIdx < AMBA_DSP_MAX_VOUT_NUM) {
#if defined(SVC_DSP_VOUT1_INT)
        if (VoutIdx == VOUT_IDX_A) {
            Flag = SVC_DSP_VOUT0_INT;
        } else {
            Flag = SVC_DSP_VOUT1_INT;
#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            if (VoutIdx == VOUT_IDX_B) {
                Flag = SVC_DSP_VOUT2_INT;
            }
#endif
        }
#else
        Flag = SVC_DSP_VOUT0_INT;
#endif

        if (GuiType == (UINT8) SVC_GUI_TYPE_POLARITY){
            Flag = SVC_GUI_EVENT_VOUT_POLARITY;
        }
         if (SVC_OK == pVoutTriggerFunc[GuiType](VoutIdx, Flag, &ActualFlag)) {
             SvcGui_Dbg(SVC_LOG_GUI, "Gui_VoutIntEntry (%d) : Get Interrupt!", VoutIdx, 0U);
            if ((ActualFlag & Flag) > 0U) {
                if (SvcGuiCtrl[VoutIdx].TaskStart == 1U) {
                    AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;
                    (void)AmbaSYS_GetOrcTimer(&(SvcGuiCtrl[VoutIdx].FlagTick[0U]));
                    GuiDebugConut[VoutIdx]++;
                    SvcGuiCtrl[VoutIdx].IntervalCnt++;
                    if (SvcGuiCtrl[VoutIdx].IntervalCnt == SvcGuiCtrl[VoutIdx].Interval) {
                        SvcGui_Dbg(SVC_LOG_GUI, "Gui_VoutIntEntry (%d) : Set event flag", VoutIdx, 0U);
                        RetVal = AmbaKAL_EventFlagQuery(&GuiEventFlag[VoutIdx], &EventFlagInfo);
                        if (SVC_OK == RetVal) {
                            if ((EventFlagInfo.CurrentFlags & SVC_GUI_EVENT_DRAWING_START) > 0U) {
                                RetVal = AmbaKAL_EventFlagSet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_FLUSH);
                            } else {
                                RetVal = AmbaKAL_EventFlagSet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_DRAWING_START | SVC_GUI_EVENT_FLUSH);
                            }
                        }
                        SvcGuiCtrl[VoutIdx].IntervalCnt = 0U;
                    }

                    if (SVC_OK == RetVal) {
                        /* Do nothing */
                    }
                }
            }
        }

        AmbaMisra_TouchUnused(&VoutIdx);
    }

    return NULL;
}

static UINT32 VoutDataRdy(const void *pEventData)
{
#ifdef CONFIG_ICAM_CVBS_EXIST
    const AMBA_DSP_VOUT_DATA_INFO_s *pVoutStatus;
    UINT8 VoutIdx;
    AmbaMisra_TypeCast(&pVoutStatus, &pEventData);
    VoutIdx = pVoutStatus-> VoutIdx;
    if (pVoutStatus->Status == 0U){
        (void) AmbaKAL_EventFlagSet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_VOUT_POLARITY);
    }
#else
    (void) pEventData;
#endif

    return 0U;
}

static void* Gui_Entry(void* EntryArg)
{
    const ULONG *pArg;
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    /* TimerEventFlag */
    UINT32 TimerEventFlag = 0U;
    UINT32 VoutIdx;
    AMBA_KAL_EVENT_FLAG_INFO_s EventFlagInfo;

    AmbaMisra_TouchUnused(&RetVal);
    AmbaMisra_TouchUnused(EntryArg);

    AmbaMisra_TypeCast(&pArg, &EntryArg);
    VoutIdx = (UINT32)(*pArg);

    while (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagGet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_DRAWING_START, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_NONE, &TimerEventFlag, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK == RetVal) {
            UINT64 Flag = SvcGuiLevel[VoutIdx].Flag;
            UINT64 LevelUpdateFlag = 0U;
            SvcGui_Dbg(SVC_LOG_GUI, "Gui_Entry (%d) : Draw Start", VoutIdx, 0U);


            /* Start to draw new canvas */
            Flag = SvcGuiLevel[VoutIdx].Flag;
            SvcGui_Dbg(SVC_LOG_GUI, "Gui_Entry (%d): Update Check", VoutIdx, 0U);
            for (i = 0U; i < SVC_GUI_LEVEL_MAX_NUM; i++) {
                if (((Flag & ((UINT64) 1U << i)) > 0U) && ((SvcGuiDebug[VoutIdx].LvlSkip & ((UINT64) 1U << i)) == 0U)) {
                    (void) Gui_CanvasUpdate(VoutIdx, i, &LevelUpdateFlag);
                }
            }
            SvcGui_Dbg(SVC_LOG_GUI, "Gui_Entry (%d): Update Check Result:", VoutIdx, 0U);
            {
                UINT32 Msb, Lsb;
                Msb = (UINT32) ((LevelUpdateFlag >> 32U) & 0xFFFFFFFFU);
                Lsb = (UINT32) (LevelUpdateFlag & 0xFFFFFFFFU);
                SvcGui_Dbg(SVC_LOG_GUI, "Update Level Bits 0x%8x %8x", Msb, Lsb);
            }

            RetVal = AmbaKAL_EventFlagQuery(&GuiEventFlag[VoutIdx], &EventFlagInfo);
            if(RetVal == SVC_OK){
                if (( EventFlagInfo.CurrentFlags & SVC_GUI_EVENT_FLUSH_NEED ) == 0U ){
                    if ((LevelUpdateFlag != 0U) || (SvcGuiCtrl[VoutIdx].ForceRedraw != 0U) || (SvcGuiDebug[VoutIdx].ForceRedraw != 0U)) {

                        if (SvcGuiCtrl[VoutIdx].ForceRedraw != 0U) {
                            SvcGuiCtrl[VoutIdx].ForceRedraw = 0U;
                        }
                        #ifdef CONFIG_SVC_LVGL_USED
                        SvcOsd_SetLvglBuffer(VoutIdx);
                        #endif
                        RetVal = Gui_CanvasClear(VoutIdx, 0);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_GUI, "Gui_Entry(%d) : Canvas clear failed", VoutIdx, 0U);
                        }

                        RetVal = Gui_MarginDraw(VoutIdx);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_GUI, "Gui_Entry(%d) : Gui_MarginDraw failed", VoutIdx, 0U);
                        }

                        for (i = 0U; i < SVC_GUI_LEVEL_MAX_NUM; i++) {
                            if (((Flag & ((UINT64) 1U << i)) > 0U) && ((SvcGuiDebug[VoutIdx].LvlSkip & ((UINT64) 1U << i)) == 0U)) {
                                SvcGui_Dbg(SVC_LOG_GUI, "Gui_Entry (%d) : Level: %d Start", VoutIdx, i);
                                RetVal = Gui_CanvasDraw(VoutIdx, i);
                                #ifdef CONFIG_SVC_LVGL_USED
                                SvcOsd_LvglDraw(VoutIdx,0);
                                #endif
                                if (SVC_OK == RetVal) {
                                    SvcGui_Dbg(SVC_LOG_GUI, "Gui_Entry (%d) : Level: %d Done", VoutIdx, i);
                                }
                            }
                        }

                        if (GuiDebugEnable == 2U) {
                            RetVal = Gui_CanvasDebugDraw(VoutIdx);
                        }


                        RetVal = AmbaKAL_EventFlagSet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_FLUSH_NEED);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_GUI, "Gui_Entry(%d) : AmbaKAL_EventFlagSet failed SVC_GUI_EVENT_FLUSH_NEED", VoutIdx, 0U);
                        }
                    } else {
                        RetVal = AmbaKAL_EventFlagSet(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_DRAWING_NO_UPDATE);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_GUI, "Gui_Entry(%d) : AmbaKAL_EventFlagSet failed SVC_GUI_EVENT_DRAWING_NO_UPDATE", VoutIdx, 0U);
                        }
                    }

                }
            }
            (void) AmbaKAL_EventFlagClear(&GuiEventFlag[VoutIdx], SVC_GUI_EVENT_DRAWING_START);
        }
        RetVal = SVC_OK;
        AmbaMisra_TouchUnused(&RetVal);
    }

    return NULL;
}

static void Gui_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcGuiCmd;

    UINT32  RetVal;

    SvcGuiCmd.pName    = "svc_gui";
    SvcGuiCmd.MainFunc = Gui_CmdEntry;
    SvcGuiCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcGuiCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG(SVC_LOG_GUI, "## fail to install svc emr command", 0U, 0U);
    }
}

static void Gui_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("debug", pArgVector[1U])) {
            if (GuiDebugEnable == 0U) {
                GuiDebugEnable = 1U;
            } else {
                GuiDebugEnable = 0U;
            }
            RetVal = SVC_OK;
        } else if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            if (3U <= ArgCount) {
                UINT32 VoutID = 0U;
                UINT32 TimeScale = 60U;
                UINT32 NumUnitsInTick = 1U;

                (void) SvcWrap_strtoul(pArgVector[2U], &VoutID);

                if (5U <= ArgCount) {
                    (void) SvcWrap_strtoul(pArgVector[3U], &TimeScale);
                    (void) SvcWrap_strtoul(pArgVector[4U], &NumUnitsInTick);

                    SvcGuiCtrl[VoutID].Config.MaxFrameRate.TimeScale = TimeScale;
                    SvcGuiCtrl[VoutID].Config.MaxFrameRate.NumUnitsInTick = NumUnitsInTick;

                    SvcLog_DBG(SVC_LOG_GUI, "GUI frame-rate is changed to %u / %u", TimeScale, NumUnitsInTick);
                }

                RetVal = SvcGui_Start(VoutID, &SvcGuiCtrl[VoutID].Config);
                RetVal = SVC_OK;
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            if (3U == ArgCount) {
                UINT32 VoutID = 0U;

                (void) SvcWrap_strtoul(pArgVector[2U], &VoutID);

                RetVal = SvcGui_Stop(VoutID);
                RetVal = SVC_OK;
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("diagbox", pArgVector[1U])) {
            SvcDiagBox_CmdEntry(ArgCount - 1U, &(pArgVector[1U]), PrintFunc);
            RetVal = SVC_OK;
#ifndef CONFIG_ICAM_32BITS_OSD_USED
        } else if (0 == SvcWrap_strcmp("palette", pArgVector[1U])) {
            SvcGuiPalette_CmdEntry(ArgCount - 1U, &(pArgVector[1U]), PrintFunc);
            RetVal = SVC_OK;
#endif
        } else if (0 == SvcWrap_strcmp("level", pArgVector[1U])) {
            if (3U == ArgCount) {
                UINT32 VoutID = 0U;

                (void) SvcWrap_strtoul(pArgVector[2U], &VoutID);

                SvcLog_DBG(SVC_LOG_GUI, "== Registered Level in VOUT_ID (%d)", VoutID, 0U);
                for (UINT32 Level = 0U; Level < SVC_GUI_LEVEL_MAX_NUM; Level++) {
                    if ((SvcGuiLevel[VoutID].Flag & ((UINT64) 1U << Level)) != 0U) {
                        SVC_WRAP_PRINT "Level: %2d - \"%s\""
                        SVC_PRN_ARG_S SVC_LOG_GUI
                        SVC_PRN_ARG_UINT32 Level                           SVC_PRN_ARG_POST
                        SVC_PRN_ARG_CSTR   SvcGuiLevel[VoutID].Name[Level] SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                    }
                }
                RetVal = SVC_OK;
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("force_redraw", pArgVector[1U])) {
            if (3U == ArgCount) {
                UINT32 VoutID = 0U;

                (void) SvcWrap_strtoul(pArgVector[2U], &VoutID);

                if (SvcGuiDebug[VoutID].ForceRedraw == 0U) {
                    SvcGuiDebug[VoutID].ForceRedraw = 1U;
                } else {
                    SvcGuiDebug[VoutID].ForceRedraw = 0U;
                }
                RetVal = SVC_OK;
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("level_skip", pArgVector[1U])) {
            if (5U == ArgCount) {
                UINT32 VoutID = 0U;
                UINT32 LvlMsb = 0U, LvlLsb = 0U;

                (void) SvcWrap_strtoul(pArgVector[2U], &VoutID);
                (void) SvcWrap_strtoul(pArgVector[3U], &LvlMsb);
                (void) SvcWrap_strtoul(pArgVector[4U], &LvlLsb);

                SvcGuiDebug[VoutID].LvlSkip = 0ULL;
                SvcGuiDebug[VoutID].LvlSkip |= ((UINT64) LvlMsb) << 32ULL;
                SvcGuiDebug[VoutID].LvlSkip |= ((UINT64) LvlLsb);

                SvcLog_DBG(SVC_LOG_GUI, "== VOUT_ID (%d) skip lvl", VoutID, 0U);
                SvcLog_DBG(SVC_LOG_GUI, " 0x%8x %8x", LvlMsb, LvlLsb);

                RetVal = SVC_OK;
            } else if (3U == ArgCount) {
                UINT32 VoutID = 0U;
                UINT32 LvlMsb = 0U, LvlLsb = 0U;

                (void) SvcWrap_strtoul(pArgVector[2U], &VoutID);

                LvlMsb = (UINT32) ((SvcGuiDebug[VoutID].LvlSkip >> 32U) & 0xFFFFFFFFU);
                LvlLsb = (UINT32) (SvcGuiDebug[VoutID].LvlSkip & 0xFFFFFFFFU);

                SvcLog_DBG(SVC_LOG_GUI, "== VOUT_ID (%d) skip lvl", VoutID, 0U);
                SvcLog_DBG(SVC_LOG_GUI, " 0x%8x %8x", LvlMsb, LvlLsb);

                RetVal = SVC_OK;
            } else {
                RetVal = SVC_NG;
            }
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (SVC_OK != RetVal) {
        Gui_CmdUsage(PrintFunc);
    }
}

static void Gui_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{

    PrintFunc("svc_gui commands:\n");
    PrintFunc("        debug\n");
    PrintFunc("                Enable\\disable debug message\n");
    PrintFunc("        start [VoutIdx] [GUI frame-rate time scale (optional)] [GUI framer-rate ticks (optional)]\n");
    PrintFunc("                Start GUI. To change GUI frame-rate GUI must stop first\n");
    PrintFunc("        stop [VoutIdx]\n");
    PrintFunc("                Stop GUI.\n");
    PrintFunc("        level [VoutIdx]\n");
    PrintFunc("                Show registered level.\n");
    PrintFunc("        level_skip [VoutIdx] [msb] [lsb]\n");
    PrintFunc("                Skip certain level when GUI is drawing. e.x., to skip level 36 and level 1, 0x00000010 0x00000001 will be set\n");
    SvcDiagBox_CmdEntry(0, NULL, PrintFunc);
#ifndef CONFIG_ICAM_32BITS_OSD_USED
    SvcGuiPalette_CmdEntry(0, NULL, PrintFunc);
#endif
}
