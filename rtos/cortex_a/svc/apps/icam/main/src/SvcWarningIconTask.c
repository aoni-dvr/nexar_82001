/**
 *  @file SvcWarningIconTask.c
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
 *  @details svc warning icon control task file
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaShell.h"
#include "AmbaDSP_VOUT.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCmd.h"
#include "SvcGui.h"
#include "SvcOsd.h"

#include "SvcWarningIconTask.h"

#ifdef CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII
#include "warnicon/WarningIcon_ZS095BH301A8H3_BII.c"
#elif defined(CONFIG_FPD_MAXIM_TFT1280768) && defined(CONFIG_FPD_MAXIM_TRULY_VOUT0)
#include "warnicon/WarningIcon_TFT1280768.c"
#else
#include "warnicon/WarningIcon_TV.c"
#endif

#define SVC_WARN_ICON_FLG_INIT       (0x1U)
#define SVC_WARN_ICON_FLG_CFG        (0x2U)
#define SVC_WARN_ICON_FLG_DBG        (0x4U)
#define SVC_WARN_ICON_FLG_GUI_RDY    (0x8U)
#define SVC_WARN_ICON_FLG_MEM_INIT   (0x1000U)
#define SVC_WARN_ICON_FLG_SHELL_INIT (0x2000U)

#define SVC_WARN_ICON_GUI_LVL         (34U)
#define SVC_WARN_ICON_MAX_FRAME_DATA  (10U)

#define SVC_LOG_WICON "WICON"
static void LOG_WICON_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_WICON_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_WICON_API(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_WICON_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
#define     PRN_WICON_NG    SVC_WRAP_PRINT
#define     PRN_WICON_DBG   SVC_WRAP_PRINT
#define     PRN_WICON_DBG_E ; SvcWrapPrint.Argc ++; SvcWarnIconTask_PrnDbg(SVC_WARN_ICON_FLG_DBG, pPrnModuleStr, &SvcWrapPrint); }}
#define     PRN_WICON_ERR_HDLR SvcWarnIconTask_ErrHdlr(__func__, __LINE__, PRetVal);

typedef struct {
    UINT32                  State;
#define SVC_WARN_ICON_INIT        (0x1U)
#define SVC_WARN_ICON_DRAW_DONE   (0x4U)
#define SVC_WARN_ICON_GUI_RDY     (0x10U)
#define SVC_WARN_ICON_GUI_UPD     (0x20U)
#define SVC_WARN_ICON_TIMER_RDY   (0x100U)
#define SVC_WARN_ICON_TIMER_START (0x200U)
    char                    Name[32];
    AMBA_KAL_MUTEX_t        Mutex;
    UINT32                  ActFlag;
    UINT32                  Vout;
    SVC_GUI_CANVAS_s        GuiIconWin;
    UINT32                  GuiIconFrmNum;
    //UINT8                  *pGuiIconFrm[SVC_WARN_ICON_MAX_FRAME_DATA];
    SVC_OSD_BMP_s           pGuiIconFrm[SVC_WARN_ICON_MAX_FRAME_DATA];
    AMBA_KAL_TIMER_t        AniTimer;         /* Software timer */
    UINT32                  AniTimerInterval;
    UINT32                  AniFrameIndex;
    UINT32                  AniFrameSeqNum;
    UINT32                 *pAniFrameSeq;
    AMBA_KAL_TIMER_EXPIRY_f pAniTimerEntry;
} SVC_WARN_ICON_CTRL_s;

typedef struct {
    UINT32               CfgState[SVC_WARN_ICON_TYPE_NUM];
#define SVC_WARN_ICON_CFG_DATA_RDY (1U)
    SVC_WARN_ICON_CTRL_s WarnIconCtrl[SVC_WARN_ICON_NUM];
    SVC_GUI_CANVAS_s     GuiCanvas[2U];  //VoutA(VOUT_IDX_A) and VoutB(VOUT_IDX_B)
} SVC_WARN_ICON_MGR_s;

typedef void (*SVC_WARN_ICON_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void (*SVC_WARN_ICON_SHELL_USAGE_f)(void);
typedef struct {
    char                        ShellCmdName[32];
    SVC_WARN_ICON_SHELL_FUNC_f  pFunc;
    SVC_WARN_ICON_SHELL_USAGE_f pUsage;
} SVC_WARN_ICON_SHELL_FUNC_s;

static UINT32 SvcWarnIconTask_WIconStart(UINT32 IconID, UINT32 EnaAnimation);
static UINT32 SvcWarnIconTask_WIconUpdate(UINT32 IconID, UINT32 ActFlag);
static UINT32 SvcWarnIconTask_WIconStop(UINT32 IconID);
static void   SvcWarnIconTask_TimerEntry(UINT32 EntryArg);
static void   SvcWarnIconTask_DrawEntry(UINT32 VoutID);

static void   SvcWarnIconTask_DrawIconVoutAEntry(UINT32 VoutIdx, UINT32 Level);
static void   SvcWarnIconTask_DrawIconVoutBEntry(UINT32 VoutIdx, UINT32 Level);
static void   SvcWarnIconTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

static SVC_WARN_ICON_MGR_s SvcWarnIconMgr GNU_SECTION_NOZEROINIT;
static UINT32 SvcWarnIconMgrFlag = 0U;//SVC_WARN_ICON_FLG_DBG;

static void   SvcWarnIconTask_ShellTest(UINT32 ArgCount, char * const *pArgVector);
static void   SvcWarnIconTask_ShellTestU(void);
static void   SvcWarnIconTask_ShellDbg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcWarnIconTask_ShellDbgU(void);
static void   SvcWarnIconTask_ShellUsage(void);
static void   SvcWarnIconTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcWarnIconTask_ShellCmdInstall(void);

#define SVC_WARN_ICON_SHELL_NUM (2U)
static SVC_WARN_ICON_SHELL_FUNC_s SvcWarnIconShellFunc[SVC_WARN_ICON_SHELL_NUM] GNU_SECTION_NOZEROINIT;

static void LOG_WICON_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_OK(SVC_LOG_WICON, pStr, Arg1, Arg2);
}
static void LOG_WICON_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_NG(SVC_LOG_WICON, pStr, Arg1, Arg2);
}
static void LOG_WICON_API(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_DBG(SVC_LOG_WICON, pStr, Arg1, Arg2);
}
static void LOG_WICON_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_DBG) > 0U) {
        SvcLog_DBG(SVC_LOG_WICON, pStr, Arg1, Arg2);
    }
}
static void SvcWarnIconTask_PrnDbg(UINT32 Level, const char *pModule, SVC_WRAP_PRINT_s *pPrint)
{
    if ((SvcWarnIconMgrFlag & Level) > 0U) {
        SvcWrap_Print(pModule, pPrint);
    }
}
static void SvcWarnIconTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_DBG) > 0U) {
                SVC_WRAP_PRINT "Catch ErrCode(0x%08x) @ %s, %d"
                    SVC_PRN_ARG_S   SVC_LOG_WICON
                    SVC_PRN_ARG_UINT32 ErrCode  SVC_PRN_ARG_POST
                    SVC_PRN_ARG_CSTR   pCaller  SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 CodeLine SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_DrawIconVoutAEntry
 *
 *  @Description:: warning icon 0 draw entry for VOUT A
 *
 *  @Input      ::
 *         Level : GUI canvas level
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_DrawIconVoutAEntry(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    SvcWarnIconTask_DrawEntry(VOUT_IDX_A);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_DrawIconVoutBEntry
 *
 *  @Description:: warning icon 0 draw entry for VOUT B
 *
 *  @Input      ::
 *         Level : GUI canvas level
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_DrawIconVoutBEntry(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    SvcWarnIconTask_DrawEntry(VOUT_IDX_B);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_DrawUpdate
 *
 *  @Description:: warning icon gui draw update
 *
 *  @Input      ::
 *        VoutIdx : Vout iD
 *        Level   : Level
 *
 *  @Output     ::
 *        pUpdate : Update or not
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    UINT32 IconID;

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    *pUpdate = 0U;

    for (IconID = 0U; IconID < SVC_WARN_ICON_NUM; IconID ++) {
        const SVC_WARN_ICON_CTRL_s *pIconCtrl = &(SvcWarnIconMgr.WarnIconCtrl[IconID]);

        if ((pIconCtrl->State & SVC_WARN_ICON_INIT) > 0U) {
            if (((pIconCtrl->State & SVC_WARN_ICON_GUI_RDY) > 0U) && (pIconCtrl->Vout == VoutIdx)) {
                if ((pIconCtrl->State & SVC_WARN_ICON_GUI_UPD) > 0U) {
                    *pUpdate = 1U;
                    break;
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_WIconStart
 *
 *  @Description:: start to draw warning icon
 *
 *  @Input      ::
 *        IconID : Icon id
 *  EnaAnimation : Enable/Disable icon animation
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcWarnIconTask_WIconStart(UINT32 IconID, UINT32 EnaAnimation)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to start warning icon - initial warning icon task first!", 0U, 0U);
    } else {
        if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_CFG) == 0U) {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to start warning icon - configuration warning icon task first!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        if (IconID < SVC_WARN_ICON_NUM) {
            SVC_WARN_ICON_CTRL_s *pIconCtrl = &(SvcWarnIconMgr.WarnIconCtrl[IconID]);

            if ((pIconCtrl->State & SVC_WARN_ICON_INIT) > 0U) {

                // Disable GUI draw state
                pIconCtrl->State &= ~SVC_WARN_ICON_GUI_UPD;

                // If GUI canvas has been register, unregister first!
                if ((pIconCtrl->State & SVC_WARN_ICON_GUI_RDY) > 0U) {
                    pIconCtrl->State &= ~SVC_WARN_ICON_GUI_RDY;
                }

                // If the timer has been create, stop/delete it first!
                if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_RDY) > 0U) {
                    if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_START) > 0U) {
                        PRetVal = AmbaKAL_TimerStop(&(pIconCtrl->AniTimer)); PRN_WICON_ERR_HDLR
                        pIconCtrl->State &= ~SVC_WARN_ICON_TIMER_START;
                    }
                    PRetVal = AmbaKAL_TimerDelete(&(pIconCtrl->AniTimer)); PRN_WICON_ERR_HDLR
                    pIconCtrl->State &= ~SVC_WARN_ICON_TIMER_RDY;
                }

                pIconCtrl->State |= SVC_WARN_ICON_GUI_RDY;
                pIconCtrl->State &= ~SVC_WARN_ICON_GUI_UPD;

                // Create Timer
                if ((EnaAnimation > 0U) && (pIconCtrl->AniFrameSeqNum > 0U) && (pIconCtrl->pAniFrameSeq != NULL)) {
                    pIconCtrl->AniFrameIndex = 0U;
                    AmbaSvcWrap_MisraMemset(&(pIconCtrl->AniTimer), 0, sizeof((pIconCtrl->AniTimer)));
                    if (0U != AmbaKAL_TimerCreate(&(pIconCtrl->AniTimer),
                                                  pIconCtrl->Name,
                                                  pIconCtrl->pAniTimerEntry,
                                                  IconID, pIconCtrl->AniTimerInterval, pIconCtrl->AniTimerInterval, AMBA_KAL_DONT_START)) {
                        RetVal = SVC_NG;
                        LOG_WICON_NG("Failure to start warn icon - create animation timer fail!", 0U, 0U);
                    } else {
                        pIconCtrl->State |= SVC_WARN_ICON_TIMER_RDY;
                        pIconCtrl->State &= ~SVC_WARN_ICON_TIMER_START;
                        LOG_WICON_DBG("Successful to create animation timer", 0U, 0U);
                    }
                }

                LOG_WICON_DBG("Successful to start warn icon", 0U, 0U);
            } else {
                AmbaMisra_TouchUnused(pIconCtrl);
            }
        }else {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to update warning icon - invalid icon id(%d)", IconID, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_WIconUpdate
 *
 *  @Description:: update to warning icon action
 *
 *  @Input      ::
 *        IconID : Icon id
 *       ActFlag : action flag
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcWarnIconTask_WIconUpdate(UINT32 IconID, UINT32 ActFlag)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to update warning icon - initial warning icon task first!", 0U, 0U);
    } else {
        if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_CFG) == 0U) {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to update warning icon - configure warning icon task first!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        if (IconID < SVC_WARN_ICON_NUM) {
            SVC_WARN_ICON_CTRL_s *pIconCtrl = &(SvcWarnIconMgr.WarnIconCtrl[IconID]);

            if ((pIconCtrl->State & SVC_WARN_ICON_INIT) == 0U) {
                RetVal = SVC_NG;
                LOG_WICON_NG("Failure to update warn icon - create IconID(%d) warning icon", IconID, 0U);
                AmbaMisra_TouchUnused(pIconCtrl);
            }

            if (RetVal == SVC_OK) {

                PRetVal = AmbaKAL_MutexTake(&(pIconCtrl->Mutex), AMBA_KAL_NO_WAIT);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    LOG_WICON_NG("Failure to update warn icon - IconID(%d) warning icon take mutex fail!", IconID, 0U);
                } else {

                    if ((pIconCtrl->State & SVC_WARN_ICON_GUI_RDY) > 0U) {
                        if ((ActFlag & SVC_WARN_ICON_FLG_SHOW) > 0U) {
                            pIconCtrl->State |= SVC_WARN_ICON_GUI_UPD;
                        } else {
                            pIconCtrl->State &= ~SVC_WARN_ICON_GUI_UPD;
                        }

                        if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_RDY) > 0U) {
                            if ((ActFlag & SVC_WANR_ICON_FLG_ANI) > 0U) {
                                if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_START) == 0U) {
                                    if (0U == AmbaKAL_TimerStart(&(pIconCtrl->AniTimer))) {
                                        pIconCtrl->State |= SVC_WARN_ICON_TIMER_START;
                                        LOG_WICON_DBG("Successful to start animation timer 0x%x", pIconCtrl->State, 0U);
                                    }
                                }
                            } else {
                                if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_START) > 0U) {
                                    if (0U == AmbaKAL_TimerStop(&(pIconCtrl->AniTimer))) {
                                        pIconCtrl->State &= ~SVC_WARN_ICON_TIMER_START;
                                        pIconCtrl->AniFrameIndex = 0U;
                                        LOG_WICON_DBG("Successful to stop animation timer", 0U, 0U);
                                    }
                                }
                            }
                        }
                    }

                    LOG_WICON_DBG("Successful to update warn icon", 0U, 0U);

                    if (AmbaKAL_MutexGive(&(pIconCtrl->Mutex)) != OK) {
                        LOG_WICON_NG("fail to AmbaKAL_MutexGive", 0U, 0U);
                    }
                }
            }
        }else {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to update warning icon - invalid icon id(%d)", IconID, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_WIconStop
 *
 *  @Description:: stop to draw warning icon
 *
 *  @Input      ::
 *        IconID : Icon id
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SvcWarnIconTask_WIconStop(UINT32 IconID)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to update warning icon - initial warning icon task first!", 0U, 0U);
    } else {
        if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_CFG) == 0U) {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to update warning icon - configure warning icon task first!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        if (IconID < SVC_WARN_ICON_NUM) {
            SVC_WARN_ICON_CTRL_s *pIconCtrl = &(SvcWarnIconMgr.WarnIconCtrl[IconID]);

            if ((pIconCtrl->State & SVC_WARN_ICON_INIT) > 0U) {
                PRetVal = AmbaKAL_MutexTake(&(pIconCtrl->Mutex), 10000U);
                if (PRetVal != 0U) {
                    RetVal = SVC_NG;
                    LOG_WICON_NG("Failure to stop warning icon - IconID(%d) take mutex timeout! 10 sec", IconID, 0U);
                } else {

                    if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_RDY) > 0U) {
                        if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_START) > 0U) {
                            if (0U == AmbaKAL_TimerStop(&(pIconCtrl->AniTimer))) {
                                pIconCtrl->State &= ~SVC_WARN_ICON_TIMER_START;
                            }
                            if (0U == AmbaKAL_TimerDelete(&(pIconCtrl->AniTimer))) {
                                pIconCtrl->State &= ~SVC_WARN_ICON_TIMER_RDY;
                            }
                        }
                    }

                    if ((pIconCtrl->State & SVC_WARN_ICON_GUI_RDY) > 0U) {
                        UINT32 TimeOut = 1000U;
                        pIconCtrl->State &= ~SVC_WARN_ICON_GUI_UPD;
                        pIconCtrl->State &= ~SVC_WARN_ICON_DRAW_DONE;

                        do {
                            if ((pIconCtrl->State & SVC_WARN_ICON_DRAW_DONE) > 0U) {
                                break;
                            } else {
                                PRetVal = AmbaKAL_TaskSleep(10U); PRN_WICON_ERR_HDLR
                                TimeOut -= 10U;
                            }
                        } while (TimeOut > 0U);

                        pIconCtrl->State &= ~SVC_WARN_ICON_GUI_RDY;
                    }

                    LOG_WICON_DBG("Successful to stop warn icon", 0U, 0U);

                    if (AmbaKAL_MutexGive(&(pIconCtrl->Mutex)) != OK) {
                        LOG_WICON_NG("fail to AmbaKAL_MutexGive", 0U, 0U);
                    }
                }
            } else {
                AmbaMisra_TouchUnused(pIconCtrl);
            }
        } else {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to update warning icon - invalid icon id(%d)", IconID, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_TimerEntry
 *
 *  @Description:: warning icon timer entry
 *
 *  @Input      ::
 *      EntryArg : timer entry argument
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_TimerEntry(UINT32 EntryArg)
{
    UINT32 IconID = EntryArg, PRetVal;

    if (IconID < SVC_WARN_ICON_NUM) {
        SVC_WARN_ICON_CTRL_s *pIconCtrl = &(SvcWarnIconMgr.WarnIconCtrl[IconID]);

        if ((pIconCtrl->State & SVC_WARN_ICON_INIT) > 0U) {

            PRetVal = AmbaKAL_MutexTake(&(pIconCtrl->Mutex), AMBA_KAL_NO_WAIT);
            if (PRetVal != 0U) {
                LOG_WICON_NG("Failure to process timer handler - IconID(%d) take mutex fail!", IconID, 0U);
            } else {

                if ((pIconCtrl->State & SVC_WARN_ICON_GUI_RDY) > 0U) {
                    if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_RDY) > 0U) {
                        if ((pIconCtrl->State & SVC_WARN_ICON_TIMER_START) > 0U) {

                            // Flash animal
                            if (pIconCtrl->GuiIconFrmNum == 1U) {
                                if ((pIconCtrl->State & SVC_WARN_ICON_GUI_UPD) > 0U) {
                                    pIconCtrl->State &= ~SVC_WARN_ICON_GUI_UPD;
                                } else {
                                    pIconCtrl->State |= SVC_WARN_ICON_GUI_UPD;
                                }
                            } else {
                                pIconCtrl->State |= SVC_WARN_ICON_GUI_UPD;
                                pIconCtrl->AniFrameIndex += 1U;
                                if (pIconCtrl->AniFrameIndex >= pIconCtrl->AniFrameSeqNum) {
                                    pIconCtrl->AniFrameIndex = 0U;
                                }
                            }
                        }
                    }
                }

                if (AmbaKAL_MutexGive(&(pIconCtrl->Mutex)) != OK) {
                    LOG_WICON_NG("fail to AmbaKAL_MutexGive", 0U, 0U);
                }
            }
        } else {
            AmbaMisra_TouchUnused(pIconCtrl);
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_DrawEntry
 *
 *  @Description:: warning icon gui draw entry
 *
 *  @Input      ::
 *        IconID : Icon id
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_DrawEntry(UINT32 VoutID)
{
    UINT32 PRetVal, IconID;

    for (IconID = 0U; IconID < SVC_WARN_ICON_NUM; IconID ++) {
        SVC_WARN_ICON_CTRL_s *pIconCtrl = &(SvcWarnIconMgr.WarnIconCtrl[IconID]);

        if ((pIconCtrl->State & SVC_WARN_ICON_INIT) > 0U) {

            if (((pIconCtrl->State & SVC_WARN_ICON_GUI_RDY) > 0U) && (pIconCtrl->Vout == VoutID)) {
                if ((pIconCtrl->State & SVC_WARN_ICON_GUI_UPD) > 0U) {
                    UINT32 DrawFrmIdx;

                    if (pIconCtrl->pAniFrameSeq != NULL) {
                        DrawFrmIdx = pIconCtrl->pAniFrameSeq[pIconCtrl->AniFrameIndex];
                    } else {
                        DrawFrmIdx = 0U;
                    }
                    
                    // PRetVal = SvcOsd_DrawBmp(pIconCtrl->Vout,
                    //                          pIconCtrl->GuiIconWin.StartX,
                    //                          pIconCtrl->GuiIconWin.StartY,
                    //                          pIconCtrl->GuiIconWin.StartX + pIconCtrl->GuiIconWin.Width,
                    //                          pIconCtrl->GuiIconWin.StartY + pIconCtrl->GuiIconWin.Height,
                    //                          pIconCtrl->pGuiIconFrm[DrawFrmIdx]);

                     PRetVal = SvcOsd_DrawBmp(pIconCtrl->Vout,
                                             pIconCtrl->GuiIconWin.StartX,
                                             pIconCtrl->GuiIconWin.StartY,
                                             pIconCtrl->GuiIconWin.StartX + pIconCtrl->GuiIconWin.Width,
                                             pIconCtrl->GuiIconWin.StartY + pIconCtrl->GuiIconWin.Height,
                                             &pIconCtrl->pGuiIconFrm[DrawFrmIdx]);                   

                    PRN_WICON_ERR_HDLR

                    pIconCtrl->State &= ~SVC_WARN_ICON_DRAW_DONE;
                }
            }

        } else {
            AmbaMisra_TouchUnused(pIconCtrl);
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_ShellTest
 *
 *  @Description:: Shell command - test
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_ShellTest(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 4U) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to proc 'test' cmd - invalid ArgCount", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 Vout    = 0U;
        UINT32 ActFlag = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[2U], &Vout );    PRN_WICON_ERR_HDLR
        PRetVal = SvcWrap_strtoul(pArgVector[3U], &ActFlag ); PRN_WICON_ERR_HDLR

        PRetVal = SvcWarnIconTask_Update(Vout, ActFlag); PRN_WICON_ERR_HDLR
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_ShellTest
 *
 *  @Description:: Shell command usage - test
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_ShellTestU(void)
{
    LOG_WICON_API("    test        :", 0U, 0U);
    LOG_WICON_API("    ------------------------------------------------------", 0U, 0U);
    LOG_WICON_API("    [IconID]   : Configure the icon id", 0U, 0U);
    LOG_WICON_API("    [ActFlag]  : Configure the action flag", 0U, 0U);
    LOG_WICON_API("               : bit 0 : Enable warn icon", 0U, 0U);
    LOG_WICON_API("               : bit 1 : Enable animation", 0U, 0U);
    LOG_WICON_API("               : bit 2 : Show warn icon", 0U, 0U);
    LOG_WICON_API("               : bit 3 : Start animation", 0U, 0U);
    LOG_WICON_API("", 0U, 0U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_ShellDbg
 *
 *  @Description:: Shell command - enable debug msg
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_ShellDbg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to proc 'test' cmd - invalid ArgCount", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 DbgOn = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[2U], &DbgOn );    PRN_WICON_ERR_HDLR

        if (DbgOn > 0U) {
            SvcWarnIconMgrFlag |= SVC_WARN_ICON_FLG_DBG;
        } else {
            SvcWarnIconMgrFlag &= ~SVC_WARN_ICON_FLG_DBG;
        }

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_ShellDbgU
 *
 *  @Description:: Shell command usage - enable debug msg
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_ShellDbgU(void)
{
    LOG_WICON_API("    dbg         :", 0U, 0U);
    LOG_WICON_API("    ------------------------------------------------------", 0U, 0U);
    LOG_WICON_API("    [Enable]    : enable/disable debug msg", 0U, 0U);
    LOG_WICON_API("", 0U, 0U);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_ShellUsage
 *
 *  @Description:: Shell command entry usage
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_ShellUsage(void)
{
    UINT32 ShellIdx;
    UINT32 ShellCount = (UINT32)(sizeof(SvcWarnIconShellFunc)) / (UINT32)(sizeof(SvcWarnIconShellFunc[0]));

    LOG_WICON_API("", 0U, 0U);
    LOG_WICON_API("====== Warning Icon Task Command Usage ======", 0U, 0U);

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if (SvcWarnIconShellFunc[ShellIdx].pFunc != NULL) {
            if (SvcWarnIconShellFunc[ShellIdx].pUsage == NULL) {
                SVC_WRAP_PRINT "  %s"
                    SVC_PRN_ARG_S   SVC_LOG_WICON
                    SVC_PRN_ARG_CSTR SvcWarnIconShellFunc[ShellIdx].ShellCmdName SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            } else {
                (SvcWarnIconShellFunc[ShellIdx].pUsage)();
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_ShellEntry
 *
 *  @Description:: Shell command entry
 *
 *  @Input      ::
 *      ArgCount : Input arguments counter
 *    pArgVector : Input arguments data
 *     PrintFunc : shell print function
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK;

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        LOG_WICON_API("", 0U, 0U);
        SVC_WRAP_PRINT "==== %sShell Arguments Setting%s (%s%d%s) ===="
            SVC_PRN_ARG_S SVC_LOG_WICON
            SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 ArgCount                                       SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            SVC_WRAP_PRINT "  pArgVector[%s%d%s] : %s%s%s"
               SVC_PRN_ARG_S SVC_LOG_WICON
               SVC_PRN_ARG_CSTR   ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
               SVC_PRN_ARG_UINT32 SIdx                                             SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[0m")                                    SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[38;2;0;255;128m""\033""[48;2;0;0;0m")   SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   pArgVector[SIdx]                                 SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[0m")                                    SVC_PRN_ARG_POST
               SVC_PRN_ARG_E
        }
    }


    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_SHELL_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcWarnIconShellFunc, 0, sizeof(SvcWarnIconShellFunc));
        SvcWarnIconShellFunc[0U] = (SVC_WARN_ICON_SHELL_FUNC_s) { "test",  SvcWarnIconTask_ShellTest, SvcWarnIconTask_ShellTestU };
        SvcWarnIconShellFunc[1U] = (SVC_WARN_ICON_SHELL_FUNC_s) { "dbg",   SvcWarnIconTask_ShellDbg,  SvcWarnIconTask_ShellDbgU  };

        SvcWarnIconMgrFlag |= SVC_WARN_ICON_FLG_SHELL_INIT;
    }

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        // LOG_WICON_DBG("Failure to proc diag box shell cmd - invalid arg count!", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        // LOG_WICON_DBG("Failure to proc diag box shell cmd - invalid arg vector!", 0U, 0U);
    }

    if (RetVal == SVC_OK) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcWarnIconShellFunc)) / (UINT32)(sizeof(SvcWarnIconShellFunc[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if (SvcWarnIconShellFunc[ShellIdx].pFunc != NULL) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcWarnIconShellFunc[ShellIdx].ShellCmdName)) {
                    (SvcWarnIconShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector);
                    RetVal = SVC_OK;
                    break;
                }
            }
        }
    } else {
        AmbaMisra_TouchUnused(&PrintFunc);
    }

    if (RetVal != SVC_OK) {
        SvcWarnIconTask_ShellUsage();
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_ShellCmdInstall
 *
 *  @Description:: install warning icon task shell
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void SvcWarnIconTask_ShellCmdInstall(void)
{
    AMBA_SHELL_COMMAND_s SvcWarnIconTaskCmdApp;

    SvcWarnIconTaskCmdApp.pName    = "svc_wicon";
    SvcWarnIconTaskCmdApp.MainFunc = SvcWarnIconTask_ShellEntry;
    SvcWarnIconTaskCmdApp.pNext    = NULL;

    if (SHELL_ERR_SUCCESS != SvcCmd_CommandRegister(&SvcWarnIconTaskCmdApp)) {
        LOG_WICON_NG("Failure to install svc warning icon task shell command!", 0U, 0U);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_Init
 *
 *  @Description:: initial warning icon task
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcWarnIconTask_Init(void)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&g_WarnIconCfg);
    AmbaMisra_TouchUnused(&RetVal);

    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_MEM_INIT) == 0U) {
        SvcWarnIconMgrFlag |= SVC_WARN_ICON_FLG_MEM_INIT;
    }

    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_INIT) == 0U) {
        AmbaSvcWrap_MisraMemset(&SvcWarnIconMgr, 0, sizeof(SvcWarnIconMgr));
        SvcWarnIconMgrFlag |= SVC_WARN_ICON_FLG_INIT;
        LOG_WICON_OK("Succseeful to initial warning icon task!", 0U, 0U);
    } else {
        LOG_WICON_API("Svc warning icon task has been initialized!", 0U, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_Config
 *
 *  @Description:: configure warning icon task
 *
 *  @Input      ::
 *      IconType : warning icon type
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcWarnIconTask_Config(UINT32 IconType)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    UINT32 CfgNum = (UINT32)(sizeof(g_WarnIconCfg)) / (UINT32)(sizeof(g_WarnIconCfg[0]));
    UINT32 Width, Height;

    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to config warning icon task - initial warning icon task first!", 0U, 0U);
    }

    if (IconType >= CfgNum) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to config warning icon task - invalid icon type(%d)! max icon type number(%d)", IconType, CfgNum);
    }

    if (RetVal == SVC_OK) {
        if (IconType < SVC_WARN_ICON_TYPE_NUM) {
            const SVC_WARN_ICON_CFG_s *pCurCfg = &(g_WarnIconCfg[IconType]);
            UINT32 Idx, FrmIdx;

            // Configure gui canvas
            AmbaSvcWrap_MisraMemset(&(SvcWarnIconMgr.GuiCanvas), 0, (sizeof(SVC_GUI_CANVAS_s)*2U));
            //Vout A
            RetVal = SvcOsd_GetOsdBufSize(VOUT_IDX_A, &Width, &Height);
            if (RetVal == SVC_OK) {
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].StartX = 0U;
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].StartY = 0U;
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].Width  = Width;
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].Height = Height;
            } else {
                LOG_WICON_NG("Failure to get VOUT A SvcOsd_GetOsdBufSize", 0U, 0U);
            }

            //Vout B
            RetVal = SvcOsd_GetOsdBufSize(VOUT_IDX_B, &Width, &Height);
            if (RetVal == SVC_OK) {
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].StartX = 0U;
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].StartY = 0U;
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].Width  = Width;
                SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].Height = Height;
            } else {
                LOG_WICON_NG("Failure to get VOUT B SvcOsd_GetOsdBufSize", 0U, 0U);
            }

            // Load each icon
            if ((SvcWarnIconMgr.CfgState[IconType] & SVC_WARN_ICON_CFG_DATA_RDY) == 0U) {

                for (Idx = 0U; Idx < pCurCfg->WarnIconNum; Idx ++) {

                    if ((pCurCfg->WarnIcon[Idx].Vout != 0xFFU) &&
                        (pCurCfg->WarnIcon[Idx].pIconData != NULL)) {

                        if ((pCurCfg->WarnIcon[Idx].IconArea.StartX > pCurCfg->WarnIcon[Idx].IconArea.EndX) ||
                            (pCurCfg->WarnIcon[Idx].IconArea.StartY > pCurCfg->WarnIcon[Idx].IconArea.EndY)) {
                            RetVal = SVC_NG;
                            PRN_WICON_NG "Failure to config warning icon task - invalid icon area setting! (%d, %d) (%d, %d)"
                                SVC_PRN_ARG_S SVC_LOG_WICON
                                SVC_PRN_ARG_PROC SvcLog_NG
                                SVC_PRN_ARG_UINT32 pCurCfg->WarnIcon[Idx].IconArea.StartX SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 pCurCfg->WarnIcon[Idx].IconArea.StartY SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 pCurCfg->WarnIcon[Idx].IconArea.EndX   SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 pCurCfg->WarnIcon[Idx].IconArea.EndY   SVC_PRN_ARG_POST
                                SVC_PRN_ARG_E
                        } else {
                            SVC_WARN_ICON_FRAME_s *pIconFrm = pCurCfg->WarnIcon[Idx].pIconData;

                            for (FrmIdx = 0U; FrmIdx < pCurCfg->WarnIcon[Idx].IconDataNum; FrmIdx ++) {
                                if (pIconFrm[FrmIdx].IsDataRdy == 0U) {
                                    AmbaSvcWrap_MisraMemset(pIconFrm[FrmIdx].IconInfo.data, 0, pIconFrm[FrmIdx].IconInfo.data_size);
                                    RetVal = SvcOsd_LoadBmp(pIconFrm[FrmIdx].IconInfo.data, pIconFrm[FrmIdx].IconInfo.data_size, pIconFrm[FrmIdx].IconName);
                                    if (RetVal != SVC_OK) {

                                        PRN_WICON_NG "Failure to config warning icon task - load no.%02d icon frame fail! %s"
                                            SVC_PRN_ARG_S SVC_LOG_WICON
                                            SVC_PRN_ARG_PROC   SvcLog_NG
                                            SVC_PRN_ARG_UINT32 FrmIdx                    SVC_PRN_ARG_POST
                                            SVC_PRN_ARG_CSTR   pIconFrm[FrmIdx].IconName SVC_PRN_ARG_POST
                                            SVC_PRN_ARG_E

                                    } else {
                                        pIconFrm[FrmIdx].IsDataRdy = 1U;
                                    }
                                }
                            }
                        }
                    }

                    if (RetVal != SVC_OK) {
                        break;
                    }
                }

                if (RetVal == SVC_OK) {
                    SvcWarnIconMgr.CfgState[IconType] |= SVC_WARN_ICON_CFG_DATA_RDY;
                } else {
                    PRN_WICON_DBG "Failure to config warning icon task - prepare icon data fail!"
                        SVC_PRN_ARG_S    SVC_LOG_WICON
                        SVC_PRN_ARG_PROC SvcLog_NG
                        PRN_WICON_DBG_E
                }
            }

            // Configure the warning icon control
            if (RetVal == SVC_OK) {
                SVC_WARN_ICON_CTRL_s *pCtrl;
                UINT32 TmpU32;

                // If gui on, unregister first.
                if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_GUI_RDY) > 0U) {
                    SvcGui_Unregister(VOUT_IDX_B, SVC_WARN_ICON_GUI_LVL);
                    SvcWarnIconMgrFlag &= ~SVC_WARN_ICON_FLG_GUI_RDY;
                }

                for (Idx = 0U; ( Idx < SVC_WARN_ICON_NUM ) && ( Idx < pCurCfg->WarnIconNum ); Idx ++) {
                    pCtrl = &(SvcWarnIconMgr.WarnIconCtrl[Idx]);

                    // If warn icon control has been initialized, release resouce first.
                    if ((pCtrl->State & SVC_WARN_ICON_INIT) > 0U) {
                        PRetVal = AmbaKAL_MutexTake(&(pCtrl->Mutex), 10000U);
                        if (PRetVal != 0U) {
                            LOG_WICON_NG("Failure to config warning icon task - IconID(%d) take mutex timeout!", Idx, 0U);
                        } else {

                            if ((pCtrl->State & SVC_WARN_ICON_GUI_RDY) > 0U) {
                                pCtrl->State &= ~SVC_WARN_ICON_GUI_RDY;
                            }

                            // If timer is running, stop/delete first
                            if ((pCtrl->State & SVC_WARN_ICON_TIMER_RDY) > 0U) {
                                if ((pCtrl->State & SVC_WARN_ICON_TIMER_START) > 0U) {
                                    PRetVal = AmbaKAL_TimerStop(&(pCtrl->AniTimer)); PRN_WICON_ERR_HDLR
                                }
                                PRetVal = AmbaKAL_TimerDelete(&(pCtrl->AniTimer)); PRN_WICON_ERR_HDLR
                                pCtrl->State &= ~SVC_WARN_ICON_TIMER_RDY;
                            }

                            if (AmbaKAL_MutexGive(&(pCtrl->Mutex)) != OK) {
                                LOG_WICON_NG("fail to AmbaKAL_MutexGive", 0U, 0U);
                            }

                            if (AmbaKAL_MutexDelete(&(pCtrl->Mutex)) != OK) {
                                LOG_WICON_NG("fail to AmbaKAL_MutexDelete", 0U, 0U);
                            }
                        }
                    }

                    // Reset controller setting
                    AmbaSvcWrap_MisraMemset(pCtrl, 0, sizeof(SVC_WARN_ICON_CTRL_s));

                    LOG_WICON_DBG("------ Warning Icon [%d] ------", Idx, 0U);

                    if (pCurCfg->WarnIcon[Idx].Vout != 0xFFU) {

                        // Create mutex
                        PRetVal = AmbaKAL_MutexCreate(&(pCtrl->Mutex), pCtrl->Name);
                        if (PRetVal != 0U) {
                            RetVal = SVC_NG;
                            LOG_WICON_NG("Failure to config warning icon task - create mutex fail! IconID(%d)", Idx, 0U);
                        } else {

                            pCtrl->Vout = pCurCfg->WarnIcon[Idx].Vout;

                            // Configure gui icon win
                            AmbaSvcWrap_MisraMemset(&(pCtrl->GuiIconWin), 0, sizeof(SVC_GUI_CANVAS_s));
                            pCtrl->GuiIconWin.StartX = pCurCfg->WarnIcon[Idx].IconArea.StartX;
                            pCtrl->GuiIconWin.StartY = pCurCfg->WarnIcon[Idx].IconArea.StartY;
                            pCtrl->GuiIconWin.Width  = pCurCfg->WarnIcon[Idx].IconArea.EndX - pCurCfg->WarnIcon[Idx].IconArea.StartX;
                            pCtrl->GuiIconWin.Height = pCurCfg->WarnIcon[Idx].IconArea.EndY - pCurCfg->WarnIcon[Idx].IconArea.StartY;

                            // Configure gui icon data
                            pCtrl->GuiIconFrmNum = pCurCfg->WarnIcon[Idx].IconDataNum;
                            if (pCtrl->GuiIconFrmNum > SVC_WARN_ICON_MAX_FRAME_DATA) {
                                pCtrl->GuiIconFrmNum = SVC_WARN_ICON_MAX_FRAME_DATA;
                            }
                            for (FrmIdx = 0U; (FrmIdx < pCtrl->GuiIconFrmNum) && (FrmIdx < SVC_WARN_ICON_MAX_FRAME_DATA) ; FrmIdx ++) {
                                pCtrl->pGuiIconFrm[FrmIdx] = pCurCfg->WarnIcon[Idx].pIconData[FrmIdx].IconInfo;
                            }

                                // Configure name
                            SVC_WRAP_SNPRINT "WarnIcon_%d"
                                SVC_SNPRN_ARG_S pCtrl->Name
                                SVC_SNPRN_ARG_UINT32 Idx SVC_SNPRN_ARG_POST
                                SVC_SNPRN_ARG_BSIZE (UINT32)sizeof(pCtrl->Name)
                                SVC_SNPRN_ARG_RLEN &(TmpU32)
                                SVC_SNPRN_ARG_E

                            // Configure animation timer entry
                            pCtrl->pAniTimerEntry = SvcWarnIconTask_TimerEntry;

                            // Configure animation timer interval
                            pCtrl->AniTimerInterval = pCurCfg->WarnIcon[Idx].IconFrmInterval;

                            // Configure animation frame number
                            pCtrl->AniFrameSeqNum = pCurCfg->WarnIcon[Idx].IconFrmSeqNum;

                            // Configure animation frame seq
                            pCtrl->pAniFrameSeq   = pCurCfg->WarnIcon[Idx].pIconFrmSeq;


                            // Update state
                            pCtrl->State = SVC_WARN_ICON_INIT;

                            PRN_WICON_DBG "  State            : %d"   SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_UINT32 pCtrl->State   SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            PRN_WICON_DBG "  ActFlag          : 0x%x" SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_UINT32 pCtrl->ActFlag SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            PRN_WICON_DBG "  Name             : %s"   SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_CSTR   pCtrl->Name    SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            PRN_WICON_DBG "  Vout             : %d"   SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_UINT32 pCtrl->Vout    SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            PRN_WICON_DBG "  GuiCanvas[VOUT_IDX_A]        : %d, %d, %dx%d"
                            SVC_PRN_ARG_S SVC_LOG_WICON
                               SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].StartX SVC_PRN_ARG_POST
                               SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].StartY SVC_PRN_ARG_POST
                               SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].Width  SVC_PRN_ARG_POST
                               SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_A].Height SVC_PRN_ARG_POST
                                PRN_WICON_DBG_E
                            PRN_WICON_DBG "  GuiCanvas[VOUT_IDX_B]        : %d, %d, %dx%d"
                                SVC_PRN_ARG_S SVC_LOG_WICON
                                SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].StartX SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].StartY SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].Width  SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 SvcWarnIconMgr.GuiCanvas[VOUT_IDX_B].Height SVC_PRN_ARG_POST
                                PRN_WICON_DBG_E
                            PRN_WICON_DBG "  GuiIconWin       : %d, %d, %dx%d"
                                SVC_PRN_ARG_S SVC_LOG_WICON
                                SVC_PRN_ARG_UINT32 pCtrl->GuiIconWin.StartX SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 pCtrl->GuiIconWin.StartY SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 pCtrl->GuiIconWin.Width  SVC_PRN_ARG_POST
                                SVC_PRN_ARG_UINT32 pCtrl->GuiIconWin.Height SVC_PRN_ARG_POST
                                PRN_WICON_DBG_E
                            PRN_WICON_DBG "  IconFrame Number : %d"   SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_UINT32 pCtrl->GuiIconFrmNum SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            PRN_WICON_DBG "  IconFrame Index  |    Address | Name" SVC_PRN_ARG_S SVC_LOG_WICON PRN_WICON_DBG_E
                            for (FrmIdx = 0U; FrmIdx < pCtrl->GuiIconFrmNum; FrmIdx ++) {
                                PRN_WICON_DBG "               %2d  | 0x%08x | %s"
                                    SVC_PRN_ARG_S SVC_LOG_WICON
                                    SVC_PRN_ARG_UINT32 FrmIdx                                              SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CPOINT pCtrl->pGuiIconFrm[FrmIdx].data                     SVC_PRN_ARG_POST
                                    SVC_PRN_ARG_CSTR   pCurCfg->WarnIcon[Idx].pIconData[FrmIdx].IconName   SVC_PRN_ARG_POST
                                    PRN_WICON_DBG_E
                            }
                            AmbaMisra_TypeCast(&(TmpU32), &(pCtrl->pAniTimerEntry));
                            PRN_WICON_DBG "  pAniTimerEntry   : 0x%08x" SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_UINT32 TmpU32 SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            PRN_WICON_DBG "  AniTimerIntval   : %d"     SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_UINT32 pCtrl->AniTimerInterval SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            PRN_WICON_DBG "  AniFrameSeqNum   : %d"     SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_UINT32 pCtrl->AniFrameSeqNum SVC_PRN_ARG_POST PRN_WICON_DBG_E
                            if (((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_DBG) > 0U) && (pCtrl->AniFrameSeqNum > 0U) && (pCtrl->pAniFrameSeq != NULL)) {
                                UINT32 PrintStrLen = 0U;
                                char PrintStrBuf[128];

                                AmbaSvcWrap_MisraMemset(PrintStrBuf, 0, sizeof(PrintStrBuf));

                                SVC_WRAP_SNPRINT "%d"
                                    SVC_SNPRN_ARG_S PrintStrBuf
                                    SVC_SNPRN_ARG_BSIZE (UINT32)sizeof(PrintStrBuf)
                                    SVC_SNPRN_ARG_RLEN  &PrintStrLen
                                    SVC_SNPRN_ARG_UINT32 pCtrl->pAniFrameSeq[0U] SVC_SNPRN_ARG_POST
                                    SVC_SNPRN_ARG_E

                                for (FrmIdx = 1U; FrmIdx < pCtrl->AniFrameSeqNum; FrmIdx ++) {

                                    PrintStrLen = (UINT32)SvcWrap_strlen(PrintStrBuf);
                                    SVC_WRAP_SNPRINT " -> %d"
                                        SVC_SNPRN_ARG_S &(PrintStrBuf[PrintStrLen])
                                        SVC_SNPRN_ARG_BSIZE ((UINT32)sizeof(PrintStrBuf) - PrintStrLen)
                                        SVC_SNPRN_ARG_RLEN  &PrintStrLen
                                        SVC_SNPRN_ARG_UINT32 pCtrl->pAniFrameSeq[FrmIdx] SVC_SNPRN_ARG_POST
                                        SVC_SNPRN_ARG_E
                                }

                                if (PrintStrLen > 0U) {
                                    PRN_WICON_DBG "  AniFrameSeq      : %s"     SVC_PRN_ARG_S SVC_LOG_WICON SVC_PRN_ARG_CSTR PrintStrBuf SVC_PRN_ARG_POST PRN_WICON_DBG_E
                                }
                            }
                        }
                    }

                    if (RetVal != SVC_OK) {
                        break;
                    }
                }

                if (RetVal == SVC_OK) {
                    // Register GUI canvas
                    SvcGui_Register(VOUT_IDX_A, SVC_WARN_ICON_GUI_LVL, "W_Icon", SvcWarnIconTask_DrawIconVoutAEntry, SvcWarnIconTask_DrawUpdate);
                    SvcGui_Register(VOUT_IDX_B, SVC_WARN_ICON_GUI_LVL, "W_Icon", SvcWarnIconTask_DrawIconVoutBEntry, SvcWarnIconTask_DrawUpdate);
                    SvcWarnIconMgrFlag |= SVC_WARN_ICON_FLG_GUI_RDY;
                }

            }

            if (RetVal == SVC_OK) {
                SvcWarnIconMgrFlag |= SVC_WARN_ICON_FLG_CFG;
                LOG_WICON_OK("Successful to config warning icon task!", 0U, 0U);
            }

            // Register shell command
            SvcWarnIconTask_ShellCmdInstall();
        }else {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to config warning icon task - input icon type is out-of range!", 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcWarnIconTask_Update
 *
 *  @Description:: update warning icon
 *
 *  @Input      ::
 *        IconID : warning icon id
 *       ActFlag : Action flag
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     SVC_OK(0) : Success
 *     SVC_NG(1) : Failure
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcWarnIconTask_Update(UINT32 IconID, UINT32 ActFlag)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_WICON_NG("Failure to update warning icon task - initial warning icon task first!", 0U, 0U);
    } else {
        if ((SvcWarnIconMgrFlag & SVC_WARN_ICON_FLG_CFG) == 0U) {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to update warning icon task - configure warning icon task first!", 0U, 0U);
        }
    }

    if (RetVal == SVC_OK) {
        if (IconID < SVC_WARN_ICON_NUM) {
            SVC_WARN_ICON_CTRL_s *pIconCtrl = &(SvcWarnIconMgr.WarnIconCtrl[IconID]);

            if ((pIconCtrl->State & SVC_WARN_ICON_INIT) == 0U) {
                RetVal = SVC_NG;
                LOG_WICON_NG("Failure to update warning icon task - initial IconID(%d) ", 0U, 0U);
                AmbaMisra_TouchUnused(pIconCtrl);
            } else {

                if ((ActFlag & SVC_WARN_ICON_FLG_ENA) > 0U) {

                    if ((pIconCtrl->State & SVC_WARN_ICON_GUI_RDY) == 0U) {

                        UINT32 EnaAnimationEffect = 0U;

                        if ((ActFlag & SVC_WANR_ICON_FLG_ANI_ENA) > 0U) {
                            EnaAnimationEffect = 1U;
                        }

                        RetVal = SvcWarnIconTask_WIconStart(IconID, EnaAnimationEffect);
                        if (RetVal != SVC_OK) {
                            LOG_WICON_NG("Failure to update warning icon task - start IconID(%d) warn icon fail!", IconID, 0U);
                        }

                    }

                    if (RetVal == OK) {
                        RetVal = SvcWarnIconTask_WIconUpdate(IconID, ActFlag);
                        if (RetVal != SVC_OK) {
                            LOG_WICON_NG("Failure to update warning icon task - update IconID(%d) warn icon fail!", IconID, 0U);
                        }
                    }
                } else {
                    RetVal = SvcWarnIconTask_WIconStop(IconID);
                    if (RetVal != SVC_OK) {
                        LOG_WICON_NG("Failure to update warning icon task - stop IconID(%d) warn icon fail", IconID, 0U);
                    }
                }

            }
        } else {
            RetVal = SVC_NG;
            LOG_WICON_NG("Failure to update warning icon - invalid icon id(%d)", IconID, 0U);
        }
    }

    return RetVal;
}

