/**
 *  @file SvcGuiOsdPalette.c
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
 *  @details svc application draw osd palette
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaDSP_VOUT.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcOsd.h"
#include "SvcGui.h"

#define SVC_GUI_PAL_MEM_INIT        (0x1U)
#define SVC_GUI_PAL_VOUT_A_INIT     (0x2U)
#define SVC_GUI_PAL_VOUT_B_INIT     (0x4U)
#define SVC_GUI_PAL_VOUT_A_SHOW     (0x20U)
#define SVC_GUI_PAL_VOUT_B_SHOW     (0x40U)
#define SVC_GUI_PAL_VOUT_A_UPDATE   (0x200U)
#define SVC_GUI_PAL_VOUT_B_UPDATE   (0x400U)
#define SVC_GUI_PAL_DBG_MSG_ON      (0x1000U)
#define SVC_GUI_PAL_SHELL_INIT      (0x2000U)

#define SVC_GUI_PAL_VOUT_A_ID       (0U)
#define SVC_GUI_PAL_VOUT_B_ID       (1U)
#define SVC_GUI_PAL_MAX_VOUT_NUM    (2U)
#define SVC_GUI_PAL_BLK_ROW_NUM     (16U)
#define SVC_GUI_PAL_BLK_COL_NUM     (16U)
#define SVC_GUI_PAL_BLK_SIZE        (SVC_GUI_PAL_BLK_ROW_NUM * SVC_GUI_PAL_BLK_COL_NUM)
#define SVC_GUI_PAL_CAVS_LVL        (5U)

#define SVC_GUI_PAL_FORM_BNDARY_COLOR  (64U)
#define SVC_GUI_PAL_BLK_TITLE_FG_COLOR (249U)
#define SVC_GUI_PAL_BLK_TITLE_BG_COLOR (255U)

#define SVC_LOG_GUI_PAL "GUI_PAL"
static void LOG_GUI_PAL_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_GUI_PAL_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_GUI_PAL_API(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_GUI_PAL_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
#define     PRN_GUI_PAL_DBG     SVC_WRAP_PRINT
#define     PRN_GUI_PAL_DBG_E   ; SvcWrapPrint.Argc ++; SvcGuiPalette_PrnDbg(SVC_GUI_PAL_DBG_MSG_ON, pPrnModuleStr, &SvcWrapPrint); }}

typedef struct {
    SVC_GUI_CANVAS_s BlkTleStrWin;
    UINT32           BlkSelRow;
    UINT32           BlkSelCol;
    SVC_GUI_CANVAS_s BlkSelWin;
    SVC_GUI_CANVAS_s BlkSelStrWin;
    UINT32           BlkRowGap;
    UINT32           BlkColGap;
    SVC_GUI_CANVAS_s BlkCavs[SVC_GUI_PAL_BLK_SIZE];
    SVC_GUI_CANVAS_s BlkStrWin;
} SVC_GUI_PAL_BLK_INFO_s;

typedef struct {
    UINT32            State;
#define SVC_GUI_PAL_OSD_ENA (0x1U)
#define SVC_GUI_PAL_OSD_UPD (0x2U)
    UINT32              Level;
    SVC_GUI_CANVAS_s    Canvas;
    SVC_GUI_CANVAS_s    MainWin;
    SVC_GUI_CANVAS_s    TitleWin;
    SVC_GUI_DRAW_FUNC   pDrawFunc;
    SVC_GUI_UPDATE_FUNC pUpdateFunc;
} SVC_GUI_PAL_OSD_CAVS_s;

typedef struct {
    SVC_GUI_PAL_OSD_CAVS_s Form[SVC_GUI_PAL_MAX_VOUT_NUM];
    SVC_GUI_PAL_BLK_INFO_s BlkCtrl[SVC_GUI_PAL_MAX_VOUT_NUM];
} SVC_GUI_PAL_CTRL_s;

// Private function definition for shell command.
typedef UINT32 (*SVC_GUI_PAL_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
typedef void   (*SVC_GUI_PAL_SHELL_USAGE_f)(AMBA_SHELL_PRINT_f PrintFunc);
typedef struct {
    char                      CmdName[32];
    SVC_GUI_PAL_SHELL_FUNC_f  pFunc;
    SVC_GUI_PAL_SHELL_USAGE_f pUsage;
} SVC_GUI_PAL_SHELL_FUNC_s;

static UINT32 SvcGuiPalette_Create(void);
static UINT32 SvcGuiPalette_Delete(void);
static UINT32 SvcGuiPalette_Cfg(UINT32 VoutIdx, UINT32 StartX, UINT32 StartY, UINT32 Width, UINT32 Height);
static UINT32 SvcGuiPalette_Show(UINT32 VoutIdx, UINT32 DrawOn, UINT32 StartX, UINT32 StartY, UINT32 Width, UINT32 Height);
static void   SvcGuiPalette_FormDraw(UINT32 VoutIdx);
static void   SvcGuiPalette_VoutAFormDraw(UINT32 VoutIdx, UINT32 Level);
static void   SvcGuiPalette_VoutBFormDraw(UINT32 VoutIdx, UINT32 Level);
static void   SvcGuiPalette_GuiUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

static void   SvcGuiPalette_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcGuiPalette_ShellEntryInit(void);
static UINT32 SvcGuiPalette_ShellShow(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcGuiPalette_ShellShowU(AMBA_SHELL_PRINT_f PrintFunc);
static UINT32 SvcGuiPalette_ShellMove(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcGuiPalette_ShellMoveU(AMBA_SHELL_PRINT_f PrintFunc);

#define SVC_GUI_PAL_SHELL_CMD_NUM  (2U)
static SVC_GUI_PAL_SHELL_FUNC_s SvcGuiPalShellFunc[SVC_GUI_PAL_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static char SvcGuiPalShellStr[128] GNU_SECTION_NOZEROINIT;
static SVC_GUI_PAL_CTRL_s SvcGuiPalCtrl GNU_SECTION_NOZEROINIT;
static UINT32 SvcGuiPalCtrlFlag = 0U;

static void LOG_GUI_PAL_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_OK(SVC_LOG_GUI_PAL, pStr, Arg1, Arg2);
}
static void LOG_GUI_PAL_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_NG(SVC_LOG_GUI_PAL, pStr, Arg1, Arg2);
}
static void LOG_GUI_PAL_API(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_DBG(SVC_LOG_GUI_PAL, pStr, Arg1, Arg2);
}
static void LOG_GUI_PAL_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    if ((SvcGuiPalCtrlFlag & SVC_GUI_PAL_DBG_MSG_ON) > 0U) {
        SvcLog_DBG(SVC_LOG_GUI_PAL, pStr, Arg1, Arg2);
    }
}
static void SvcGuiPalette_PrnDbg(UINT32 Level, const char *pModule, SVC_WRAP_PRINT_s *pPrint)
{
    if ( ( SvcGuiPalCtrlFlag & Level ) > 0U ) {
        SvcWrap_Print(pModule, pPrint);
    }
}

static UINT32 SvcGuiPalette_Create(void)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcGuiPalCtrlFlag & SVC_GUI_PAL_MEM_INIT) == 0U) {
        SvcGuiPalCtrlFlag |= SVC_GUI_PAL_MEM_INIT;
        AmbaSvcWrap_MisraMemset(&SvcGuiPalCtrl, 0, sizeof(SVC_GUI_PAL_CTRL_s));
    }

    return RetVal;
}

static UINT32 SvcGuiPalette_Cfg(UINT32 VoutIdx, UINT32 StartX, UINT32 StartY, UINT32 Width, UINT32 Height)
{
    UINT32 RetVal = SVC_OK;

    if (VoutIdx >= SVC_GUI_PAL_MAX_VOUT_NUM) {
        RetVal = SVC_NG;
        LOG_GUI_PAL_NG("Failure to cfg gui palette - invalid vout idx!", 0U, 0U);
    }

    if ((Width == 0U) || (Height == 0U)) {
        RetVal = SVC_NG;
        LOG_GUI_PAL_NG("Failure to cfg gui palette - invalid form window!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (VoutIdx < SVC_GUI_PAL_MAX_VOUT_NUM)) {
        UINT32 BlkIdx, BlkIdy, VoutID, OfsX, OfsY, TmpW, TmpH, OsdWidth, OsdHeight;
        SVC_GUI_PAL_OSD_CAVS_s *pForm;
        SVC_GUI_PAL_BLK_INFO_s *pBlkInfo;
        SVC_GUI_CANVAS_s *pBlk;
        UINT32 TmpU32;

        {
            VoutID    = VoutIdx;
            pForm     = &(SvcGuiPalCtrl.Form[VoutID]);
            pBlkInfo  = &(SvcGuiPalCtrl.BlkCtrl[VoutID]);
            OsdWidth  = Width;
            OsdHeight = Height;

            pForm->Canvas.StartX = StartX;
            pForm->Canvas.StartY = StartY;
            pForm->Canvas.Width  = OsdWidth;
            pForm->Canvas.Height = OsdHeight;
            pForm->Level         = SVC_GUI_PAL_CAVS_LVL;
            pForm->State         = SVC_GUI_PAL_OSD_ENA;
            if (VoutID == 0U) {
                pForm->pDrawFunc   = SvcGuiPalette_VoutAFormDraw;
                pForm->pUpdateFunc = SvcGuiPalette_GuiUpdate;
            } else {
                pForm->pDrawFunc = SvcGuiPalette_VoutBFormDraw;
                pForm->pUpdateFunc = SvcGuiPalette_GuiUpdate;
            }

            // Configure form main window
            pForm->MainWin.Width  = ( OsdWidth  * 80U ) / 100U;
            pForm->MainWin.Height = ( OsdHeight * 87U ) / 100U;
            pForm->MainWin.StartX = StartX + ( ( OsdWidth  - pForm->MainWin.Width  ) / 2U );
            pForm->MainWin.StartY = StartY + ( ( OsdHeight - pForm->MainWin.Height ) / 2U );

            // Configure form title window
            pForm->TitleWin.Width  = pForm->MainWin.Width;
            pForm->TitleWin.Height = ( pForm->MainWin.StartY * 85U ) / 100U;
            pForm->TitleWin.StartX = pForm->MainWin.StartX;
            pForm->TitleWin.StartY = StartY + ( ( pForm->MainWin.StartY - pForm->TitleWin.Height ) / 2U );

            // Configure form title block setting
            TmpW = ( pForm->TitleWin.Width * 90U) / 100U;
            TmpH = ( pForm->TitleWin.Height * 90U) / 100U;
            OfsX = pForm->TitleWin.StartX + ( ( pForm->TitleWin.Width - TmpW ) / 2U );
            OfsY = pForm->TitleWin.StartY + ( ( pForm->TitleWin.Height - TmpH ) / 2U );
            pBlkInfo->BlkTleStrWin.StartX = OfsX;
            pBlkInfo->BlkTleStrWin.StartY = OfsY;
            pBlkInfo->BlkTleStrWin.Width  = TmpW;
            pBlkInfo->BlkTleStrWin.Height = TmpH;

            // Configure each block setting
            TmpW = pForm->MainWin.Width  - ( ( pForm->MainWin.Width  / SVC_GUI_PAL_BLK_COL_NUM ) * SVC_GUI_PAL_BLK_COL_NUM );
            OfsX = pForm->MainWin.StartX + ( TmpW / 2U );
            TmpH = pForm->MainWin.Height - ( ( pForm->MainWin.Height / SVC_GUI_PAL_BLK_ROW_NUM ) * SVC_GUI_PAL_BLK_ROW_NUM );
            OfsY = pForm->MainWin.StartY + ( TmpH / 2U );

            TmpW = pForm->MainWin.Width  / SVC_GUI_PAL_BLK_COL_NUM;
            TmpH = pForm->MainWin.Height / SVC_GUI_PAL_BLK_ROW_NUM;
            pBlkInfo->BlkColGap = ( TmpW * 5U ) / 100U;
            pBlkInfo->BlkRowGap = ( TmpH * 5U ) / 100U;

            for (BlkIdy = 0U; BlkIdy < SVC_GUI_PAL_BLK_ROW_NUM; BlkIdy ++) {

                for (BlkIdx = 0U; BlkIdx < SVC_GUI_PAL_BLK_COL_NUM; BlkIdx ++) {

                    pBlk = &(pBlkInfo->BlkCavs[(BlkIdy * SVC_GUI_PAL_BLK_COL_NUM) + BlkIdx]);
                    pBlk->Width  = TmpW - pBlkInfo->BlkColGap;
                    pBlk->Height = TmpH - pBlkInfo->BlkRowGap;
                    pBlk->StartX = OfsX + ( TmpW * BlkIdx );
                    pBlk->StartY = OfsY + ( TmpH * BlkIdy );
                }
            }

            // Configure block string setting
            TmpW = ( pBlk->Width * 95U) / 100U;
            TmpH = ( pBlk->Height * 95U) / 100U;
            OfsX = ( pBlk->Width - TmpW ) / 2U;
            OfsY = ( pBlk->Height - TmpH ) / 2U;
            pBlkInfo->BlkStrWin.Width  = TmpW;
            pBlkInfo->BlkStrWin.Height = TmpH;
            pBlkInfo->BlkStrWin.StartX = OfsX;
            pBlkInfo->BlkStrWin.StartY = OfsY;

            // Configure selected block setting
            pBlkInfo->BlkSelRow = 0U;
            pBlkInfo->BlkSelCol = 0U;
            pBlkInfo->BlkSelWin.Width  = ( pBlk->Width  * 133U ) / 100U;
            pBlkInfo->BlkSelWin.Height = ( pBlk->Height * 133U ) / 100U;
            pBlkInfo->BlkSelWin.StartX = ( pBlkInfo->BlkSelWin.Width - pBlk->Width ) / 2U;
            pBlkInfo->BlkSelWin.StartY = ( pBlkInfo->BlkSelWin.Height - pBlk->Height ) / 2U;

            // Configure selected block string setting
            TmpW = ( pBlkInfo->BlkSelWin.Width * 80U) / 100U;
            TmpH = ( pBlkInfo->BlkSelWin.Height * 80U) / 100U;
            OfsX = ( pBlkInfo->BlkSelWin.Width - TmpW ) / 2U;
            OfsY = ( pBlkInfo->BlkSelWin.Height - TmpH ) / 2U;
            pBlkInfo->BlkSelStrWin.Width  = TmpW;
            pBlkInfo->BlkSelStrWin.Height = TmpH;
            pBlkInfo->BlkSelStrWin.StartX = OfsX;
            pBlkInfo->BlkSelStrWin.StartY = OfsY;

            PRN_GUI_PAL_DBG "==== VoutID(%d) ====" SVC_PRN_ARG_S SVC_LOG_GUI_PAL SVC_PRN_ARG_UINT32 VoutID SVC_PRN_ARG_POST PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "  ---- Canvas ----"   SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        |   %04d |   %04d |   %04d |   %04d"
                SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                SVC_PRN_ARG_UINT32 pForm->Canvas.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->Canvas.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->Canvas.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->Canvas.Height SVC_PRN_ARG_POST
            PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "  ---- Form Main Window ----"     SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        |   %04d |   %04d |   %04d |   %04d"
                SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                SVC_PRN_ARG_UINT32 pForm->MainWin.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->MainWin.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->MainWin.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->MainWin.Height SVC_PRN_ARG_POST
            PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "  ---- Form Title Window ----"     SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        |   %04d |   %04d |   %04d |   %04d"
                SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                SVC_PRN_ARG_UINT32 pForm->TitleWin.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->TitleWin.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->TitleWin.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pForm->TitleWin.Height SVC_PRN_ARG_POST
            PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "  ---- Block Title String Setting ----"     SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        |   %04d |   %04d |   %04d |   %04d"
                SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkTleStrWin.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkTleStrWin.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkTleStrWin.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkTleStrWin.Height SVC_PRN_ARG_POST
            PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "  ---- Block String Setting ----"     SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        |   %04d |   %04d |   %04d |   %04d"
                SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                SVC_PRN_ARG_UINT32 BlkIdy                     SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkStrWin.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkStrWin.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkStrWin.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkStrWin.Height SVC_PRN_ARG_POST
            PRN_GUI_PAL_DBG_E

            PRN_GUI_PAL_DBG "  ---- Selected Block Setting ----"     SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        |   %04d |   %04d |   %04d |   %04d"
                SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelWin.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelWin.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelWin.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelWin.Height SVC_PRN_ARG_POST
            PRN_GUI_PAL_DBG_E

            PRN_GUI_PAL_DBG "  ---- Selected Block String Setting ----"     SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "        |   %04d |   %04d |   %04d |   %04d"
                SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                SVC_PRN_ARG_UINT32 BlkIdy                        SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelStrWin.StartX SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelStrWin.StartY SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelStrWin.Width  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 pBlkInfo->BlkSelStrWin.Height SVC_PRN_ARG_POST
            PRN_GUI_PAL_DBG_E

            PRN_GUI_PAL_DBG "  ---- Block Setting ----"     SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            PRN_GUI_PAL_DBG "  Index | StartX | StartY |  Width | Height" SVC_PRN_ARG_S SVC_LOG_GUI_PAL PRN_GUI_PAL_DBG_E
            for (BlkIdy = 0U; BlkIdy < SVC_GUI_PAL_BLK_ROW_NUM; BlkIdy ++) {
                for (BlkIdx = 0U; BlkIdx < SVC_GUI_PAL_BLK_COL_NUM; BlkIdx ++) {

                    TmpU32 = (BlkIdy * SVC_GUI_PAL_BLK_COL_NUM) + BlkIdx;

                    PRN_GUI_PAL_DBG "    %03d |   %04d |   %04d |   %04d |   %04d"
                        SVC_PRN_ARG_S SVC_LOG_GUI_PAL
                        SVC_PRN_ARG_UINT32 TmpU32                                                                SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 pBlkInfo->BlkCavs[(BlkIdy * SVC_GUI_PAL_BLK_COL_NUM) + BlkIdx].StartX SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 pBlkInfo->BlkCavs[(BlkIdy * SVC_GUI_PAL_BLK_COL_NUM) + BlkIdx].StartY SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 pBlkInfo->BlkCavs[(BlkIdy * SVC_GUI_PAL_BLK_COL_NUM) + BlkIdx].Width  SVC_PRN_ARG_POST
                        SVC_PRN_ARG_UINT32 pBlkInfo->BlkCavs[(BlkIdy * SVC_GUI_PAL_BLK_COL_NUM) + BlkIdx].Height SVC_PRN_ARG_POST
                    PRN_GUI_PAL_DBG_E
                }
            }

            SvcGui_Register(VoutID, pForm->Level, "DiagBox", pForm->pDrawFunc, pForm->pUpdateFunc);
            LOG_GUI_PAL_DBG("Register GUI Palette in VoutIdx(%d) level(%d)", VoutID, pForm->Level);
        }

        if (VoutID == 0U) {
            SvcGuiPalCtrlFlag &= ~(SVC_GUI_PAL_VOUT_A_SHOW);
            SvcGuiPalCtrlFlag |= SVC_GUI_PAL_VOUT_A_INIT;
        } else {
            SvcGuiPalCtrlFlag &= ~(SVC_GUI_PAL_VOUT_B_SHOW);
            SvcGuiPalCtrlFlag |= SVC_GUI_PAL_VOUT_B_INIT;
        }
        LOG_GUI_PAL_OK("Successful to cfg gui palette!", 0U, 0U);
    }

    return RetVal;
}

static UINT32 SvcGuiPalette_Show(UINT32 VoutIdx, UINT32 DrawOn, UINT32 StartX, UINT32 StartY, UINT32 Width, UINT32 Height)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcGuiPalCtrlFlag & SVC_GUI_PAL_MEM_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_GUI_PAL_NG("Failure to show gui palette - create gui palette module first!", 0U, 0U);
    }

    if (VoutIdx >= SVC_GUI_PAL_MAX_VOUT_NUM) {
        RetVal = SVC_NG;
        LOG_GUI_PAL_NG("Failure to create gui palette - invalid vout idx!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (VoutIdx < SVC_GUI_PAL_MAX_VOUT_NUM)) {
        SVC_GUI_PAL_OSD_CAVS_s *pForm = &(SvcGuiPalCtrl.Form[VoutIdx]);

        if ((SvcGuiPalCtrlFlag & ((UINT32)(SVC_GUI_PAL_VOUT_A_INIT) << VoutIdx)) == 0U) {
            RetVal = SvcGuiPalette_Cfg(VoutIdx, StartX, StartY, Width, Height);
            if (RetVal != SVC_OK) {
                LOG_GUI_PAL_NG("Failure to show gui palette - configure fail!", 0U, 0U);
            }
        }

        if ((SvcGuiPalCtrlFlag & ((UINT32)(SVC_GUI_PAL_VOUT_A_INIT) << VoutIdx)) > 0U) {
            if (DrawOn > 0U) {
                if ((pForm->State & SVC_GUI_PAL_OSD_ENA) > 0U) {
                    pForm->State |= SVC_GUI_PAL_OSD_UPD;
                    SvcGuiPalCtrlFlag |= ((UINT32)(SVC_GUI_PAL_VOUT_A_SHOW) << VoutIdx);
                    LOG_GUI_PAL_OK("Successful to show gui palette!", 0U, 0U);
                }
            } else {
                if ((pForm->State & SVC_GUI_PAL_OSD_ENA) > 0U) {
                    pForm->State &= ~SVC_GUI_PAL_OSD_UPD;
                    SvcGuiPalCtrlFlag &= ~((UINT32)(SVC_GUI_PAL_VOUT_A_SHOW) << VoutIdx);
                    LOG_GUI_PAL_OK("Successful to hide gui palette!", 0U, 0U);
                }
            }
        }

        SvcGuiPalCtrlFlag |= ((UINT32) SVC_GUI_PAL_VOUT_A_UPDATE) << VoutIdx;
    }

    return RetVal;
}

static UINT32 SvcGuiPalette_Delete(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcGuiPalCtrlFlag & (SVC_GUI_PAL_VOUT_A_INIT | SVC_GUI_PAL_VOUT_B_INIT)) > 0U) {
        UINT32 Idx;
        for (Idx = 0U; Idx < SVC_GUI_PAL_MAX_VOUT_NUM; Idx ++) {
            if ((SvcGuiPalCtrl.Form[Idx].State & SVC_GUI_PAL_OSD_ENA) > 0U) {
                SvcGuiPalCtrl.Form[Idx].State &= ~(SVC_GUI_PAL_OSD_UPD);
                SvcGui_Unregister(Idx, SvcGuiPalCtrl.Form[Idx].Level);
            }
        }

        AmbaSvcWrap_MisraMemset(&SvcGuiPalCtrl, 0, sizeof(SvcGuiPalCtrl));
        LOG_GUI_PAL_OK("Successful to delete gui palette!", 0U, 0U);

        PRetVal = (SvcGuiPalCtrlFlag & (SVC_GUI_PAL_DBG_MSG_ON | SVC_GUI_PAL_SHELL_INIT));
        SvcGuiPalCtrlFlag = PRetVal;
        SvcGuiPalCtrlFlag |= (SVC_GUI_PAL_VOUT_A_UPDATE | SVC_GUI_PAL_VOUT_B_UPDATE);
    }

    return RetVal;
}

static void SvcGuiPalette_FormDraw(UINT32 VoutIdx)
{
    UINT32 PRetVal;

    if (VoutIdx < SVC_GUI_PAL_MAX_VOUT_NUM) {
        SVC_GUI_PAL_OSD_CAVS_s *pForm    = &(SvcGuiPalCtrl.Form[VoutIdx]);
        SVC_GUI_PAL_BLK_INFO_s *pBlkInfo = &(SvcGuiPalCtrl.BlkCtrl[VoutIdx]);

        if ((pForm->State & SVC_GUI_PAL_OSD_ENA) > 0U) {
            if ((pForm->State & SVC_GUI_PAL_OSD_UPD) > 0U) {
                UINT32 Idx, XStart, XEnd, YStart, YEnd, BlkSelIdx;
                UINT32 OsdColorVal;
                static char TitleStrBuf[40U];
                UINT32 TitleStrLen = 0U;

                // Get osd color lookup table value by selected block index
                BlkSelIdx = ( pBlkInfo->BlkSelRow * SVC_GUI_PAL_BLK_COL_NUM ) + pBlkInfo->BlkSelCol;
                if (SVC_OK != SvcOsd_GetOsdColor(VoutIdx, BlkSelIdx, &OsdColorVal)) {
                    OsdColorVal = 0U;
                }

                // Draw form main boundary
                XStart = pForm->MainWin.StartX - 1U;
                XEnd   = pForm->MainWin.StartX + pForm->MainWin.Width;
                YStart = pForm->MainWin.StartY - 1U;
                YEnd   = pForm->MainWin.StartY + pForm->MainWin.Height;
                PRetVal = SvcOsd_DrawRect(VoutIdx, XStart, YStart, XEnd, YEnd, SVC_GUI_PAL_FORM_BNDARY_COLOR, 1); AmbaMisra_TouchUnused(&PRetVal);

                // Draw form title boundary
                XStart = pForm->TitleWin.StartX - 1U;
                XEnd   = pForm->TitleWin.StartX + pForm->TitleWin.Width;
                YStart = pForm->TitleWin.StartY - 1U;
                YEnd   = pForm->TitleWin.StartY + pForm->TitleWin.Height;
                PRetVal = SvcOsd_DrawRect(VoutIdx, XStart, YStart, XEnd, YEnd, SVC_GUI_PAL_FORM_BNDARY_COLOR, 1); AmbaMisra_TouchUnused(&PRetVal);
                // Draw form title win
                XStart = pForm->TitleWin.StartX;
                XEnd   = pForm->TitleWin.StartX + pForm->TitleWin.Width;
                YStart = pForm->TitleWin.StartY;
                YEnd   = pForm->TitleWin.StartY + pForm->TitleWin.Height;
                PRetVal = SvcOsd_DrawSolidRect(VoutIdx, XStart, YStart, XEnd, YEnd, SVC_GUI_PAL_BLK_TITLE_BG_COLOR); AmbaMisra_TouchUnused(&PRetVal);
                // Draw form title string
                {
                    UINT32 TmpV;
                    INT32 OsdR, OsdG, OsdB;

                    // YUV -> RGB, Clamp to 0 ~ 255
                    TmpV = OsdColorVal & 0x00FF0000U; TmpV = (TmpV >> 16U);
                    OsdR = (INT32)TmpV;
                    TmpV = OsdColorVal & 0x0000FF00U; TmpV = (TmpV >> 8U);
                    OsdG = (INT32)TmpV;
                    TmpV = OsdColorVal & 0x000000FFU;
                    OsdB = (INT32)TmpV;

                    AmbaSvcWrap_MisraMemset(TitleStrBuf, 0, sizeof(TitleStrBuf));
                    SVC_WRAP_SNPRINT "OSD PALETTE - R:%03d G:%03d B:%03d"
                        SVC_SNPRN_ARG_S TitleStrBuf
                        SVC_SNPRN_ARG_BSIZE (UINT32)(sizeof(TitleStrBuf))
                        SVC_SNPRN_ARG_RLEN  &TitleStrLen
                        SVC_SNPRN_ARG_INT32 OsdR SVC_SNPRN_ARG_POST
                        SVC_SNPRN_ARG_INT32 OsdG SVC_SNPRN_ARG_POST
                        SVC_SNPRN_ARG_INT32 OsdB SVC_SNPRN_ARG_POST
                        SVC_SNPRN_ARG_E


                    if (TitleStrLen > 0U) {
                        const SVC_GUI_CANVAS_s *pStrRect;
                        UINT32 StrWidth, StrHeight, FontSize, DrawStrX, DrawStrY;

                        pStrRect = &(pBlkInfo->BlkTleStrWin);

                        StrWidth  = TitleStrLen * ( SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP );
                        StrHeight = SVC_OSD_CHAR_HEIGHT;

                        if (StrWidth > 0U) {
                            FontSize = pStrRect->Width / StrWidth;
                            if (FontSize > (pStrRect->Height / StrHeight)) {
                                FontSize = pStrRect->Height / StrHeight;
                            }

                            if (FontSize > 0U) {
                                StrWidth  = StrWidth * FontSize;
                                StrHeight = StrHeight * FontSize;
                                DrawStrX  = pStrRect->StartX + ( ( pStrRect->Width - StrWidth ) / 2U );
                                DrawStrY  = pStrRect->StartY + ( ( pStrRect->Height - StrHeight ) / 2U );
                                PRetVal = SvcOsd_DrawString(VoutIdx, DrawStrX, DrawStrY, FontSize, SVC_GUI_PAL_BLK_TITLE_FG_COLOR, TitleStrBuf);
                                AmbaMisra_TouchUnused(&PRetVal);
                            }
                        }
                    }
                }

                // Draw color blocks
                for (Idx = 0U; Idx < SVC_GUI_PAL_BLK_SIZE; Idx ++) {

                    // Draw color block
                    XStart = pBlkInfo->BlkCavs[Idx].StartX;
                    XEnd   = pBlkInfo->BlkCavs[Idx].StartX + pBlkInfo->BlkCavs[Idx].Width;
                    YStart = pBlkInfo->BlkCavs[Idx].StartY;
                    YEnd   = pBlkInfo->BlkCavs[Idx].StartY + pBlkInfo->BlkCavs[Idx].Height;
                    PRetVal = SvcOsd_DrawSolidRect(VoutIdx, XStart, YStart, XEnd, YEnd, Idx); AmbaMisra_TouchUnused(&PRetVal);

                    // Draw block index string
                    {
                        AmbaSvcWrap_MisraMemset(TitleStrBuf, 0, sizeof(TitleStrBuf));
                        SVC_WRAP_SNPRINT "%03d"
                            SVC_SNPRN_ARG_S TitleStrBuf
                            SVC_SNPRN_ARG_BSIZE (UINT32)(sizeof(TitleStrBuf))
                            SVC_SNPRN_ARG_RLEN  &TitleStrLen
                            SVC_SNPRN_ARG_UINT32 Idx SVC_SNPRN_ARG_POST
                            SVC_SNPRN_ARG_E

                        if (TitleStrLen > 0U) {
                            const SVC_GUI_CANVAS_s *pStrRect;
                            UINT32 StrWidth, StrHeight, FontSize, DrawStrX, DrawStrY;

                            pStrRect = &(pBlkInfo->BlkStrWin);

                            StrWidth  = TitleStrLen * ( SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP );
                            StrHeight = SVC_OSD_CHAR_HEIGHT;

                            if (StrWidth > 0U) {
                                FontSize = pStrRect->Width / StrWidth;
                                if (FontSize > (pStrRect->Height / StrHeight)) {
                                    FontSize = pStrRect->Height / StrHeight;
                                }

                                if (FontSize > 0U) {
                                    StrWidth  = StrWidth * FontSize;
                                    StrHeight = StrHeight * FontSize;
                                    DrawStrX  = XStart + pStrRect->StartX + ( ( pStrRect->Width - StrWidth ) / 2U );
                                    DrawStrY  = YStart + pStrRect->StartY + ( ( pStrRect->Height - StrHeight ) / 2U );
                                    PRetVal = SvcOsd_DrawString(VoutIdx, DrawStrX, DrawStrY, FontSize, ( SVC_GUI_PAL_BLK_SIZE - 1U ) - Idx, TitleStrBuf);
                                    AmbaMisra_TouchUnused(&PRetVal);
                                }
                            }
                        }
                    }
                }

                // Draw selected block
                if (BlkSelIdx < SVC_GUI_PAL_BLK_SIZE) {
                    XStart = pBlkInfo->BlkCavs[BlkSelIdx].StartX - pBlkInfo->BlkSelWin.StartX;
                    YStart = pBlkInfo->BlkCavs[BlkSelIdx].StartY - pBlkInfo->BlkSelWin.StartY;
                    XEnd   = XStart + pBlkInfo->BlkSelWin.Width;
                    YEnd   = YStart + pBlkInfo->BlkSelWin.Height;
                    PRetVal = SvcOsd_DrawSolidRect(VoutIdx, XStart, YStart, XEnd, YEnd, BlkSelIdx); AmbaMisra_TouchUnused(&PRetVal);

                    // Draw selected block index string
                    {
                        AmbaSvcWrap_MisraMemset(TitleStrBuf, 0, sizeof(TitleStrBuf));
                        SVC_WRAP_SNPRINT "%03d"
                            SVC_SNPRN_ARG_S TitleStrBuf
                            SVC_SNPRN_ARG_BSIZE (UINT32)(sizeof(TitleStrBuf))
                            SVC_SNPRN_ARG_RLEN  &TitleStrLen
                            SVC_SNPRN_ARG_UINT32 BlkSelIdx SVC_SNPRN_ARG_POST
                            SVC_SNPRN_ARG_E

                        if (TitleStrLen > 0U) {
                            const SVC_GUI_CANVAS_s *pStrRect;
                            UINT32 StrWidth, StrHeight, FontSize, DrawStrX, DrawStrY;

                            pStrRect = &(pBlkInfo->BlkSelStrWin);

                            StrWidth  = TitleStrLen * ( SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP );
                            StrHeight = SVC_OSD_CHAR_HEIGHT;

                            if (StrWidth > 0U) {
                                FontSize = pStrRect->Width / StrWidth;
                                if (FontSize > (pStrRect->Height / StrHeight)) {
                                    FontSize = pStrRect->Height / StrHeight;
                                }

                                if (FontSize > 0U) {
                                    StrWidth  = StrWidth * FontSize;
                                    StrHeight = StrHeight * FontSize;
                                    DrawStrX  = XStart + pStrRect->StartX + ( ( pStrRect->Width - StrWidth ) / 2U );
                                    DrawStrY  = YStart + pStrRect->StartY + ( ( pStrRect->Height - StrHeight ) / 2U );
                                    PRetVal = SvcOsd_DrawString(VoutIdx, DrawStrX, DrawStrY, FontSize, ( SVC_GUI_PAL_BLK_SIZE - 1U ) - Idx, TitleStrBuf);
                                    AmbaMisra_TouchUnused(&PRetVal);
                                }
                            }
                        }
                    }
                }
            } else {
                AmbaMisra_TouchUnused(pForm);
                AmbaMisra_TouchUnused(pBlkInfo);
            }
        }
    }
}

static void SvcGuiPalette_VoutAFormDraw(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    SvcGuiPalette_FormDraw(SVC_GUI_PAL_VOUT_A_ID);
}

static void SvcGuiPalette_VoutBFormDraw(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    SvcGuiPalette_FormDraw(SVC_GUI_PAL_VOUT_B_ID);
}

static void SvcGuiPalette_GuiUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((SvcGuiPalCtrlFlag & ((UINT32) SVC_GUI_PAL_VOUT_A_UPDATE << VoutIdx)) > 0U) {
        UINT32 Flag;

        *pUpdate = 1U;
        Flag = ~((UINT32) SVC_GUI_PAL_VOUT_A_UPDATE << VoutIdx);
        SvcGuiPalCtrlFlag &= Flag;
    } else {
        *pUpdate = 0U;
    }
}

static void SvcGuiPalette_ShellEntryInit(void)
{
    if ((SvcGuiPalCtrlFlag & SVC_GUI_PAL_SHELL_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcGuiPalShellFunc, 0, sizeof(SvcGuiPalShellFunc));
        SvcGuiPalShellFunc[0U] = (SVC_GUI_PAL_SHELL_FUNC_s) { "show",     SvcGuiPalette_ShellShow,  SvcGuiPalette_ShellShowU };
        SvcGuiPalShellFunc[1U] = (SVC_GUI_PAL_SHELL_FUNC_s) { "mv",       SvcGuiPalette_ShellMove,  SvcGuiPalette_ShellMoveU };

        SvcGuiPalCtrlFlag |= SVC_GUI_PAL_SHELL_INIT;
    }
}

static UINT32 SvcGuiPalette_ShellShow(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        LOG_GUI_PAL_NG("Failure to proc gui palette 'show' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(&PrintFunc);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 FormOn = 0U;

        PRetVal = SvcWrap_strtoul(pArgVector[2U], &FormOn); AmbaMisra_TouchUnused(&PRetVal);
        if (FormOn == 0U) {
            PRetVal = SvcGuiPalette_Delete(); AmbaMisra_TouchUnused(&PRetVal);
        } else if (FormOn == 1U) {

            if (ArgCount < 7U) {
                RetVal = SVC_NG;
                LOG_GUI_PAL_NG("Failure to proc gui palette 'show' cmd - invalid arg count!", 0U, 0U);
                AmbaMisra_TouchUnused(&PrintFunc);
            } else {
                UINT32 VoutIdx = 0U;
                UINT32 StartX  = 0U;
                UINT32 StartY  = 0U;
                UINT32 Width   = 0U;
                UINT32 Height  = 0U;

                PRetVal = SvcWrap_strtoul(pArgVector[3U], &VoutIdx); AmbaMisra_TouchUnused(&PRetVal);
                PRetVal = SvcWrap_strtoul(pArgVector[4U], &StartX ); AmbaMisra_TouchUnused(&PRetVal);
                PRetVal = SvcWrap_strtoul(pArgVector[5U], &StartY ); AmbaMisra_TouchUnused(&PRetVal);
                PRetVal = SvcWrap_strtoul(pArgVector[6U], &Width  ); AmbaMisra_TouchUnused(&PRetVal);
                PRetVal = SvcWrap_strtoul(pArgVector[7U], &Height ); AmbaMisra_TouchUnused(&PRetVal);

                if (ArgCount > 8U) {
                    if (0 == SvcWrap_strcmp(pArgVector[8U], "1")) {
                        SvcGuiPalCtrlFlag |= SVC_GUI_PAL_DBG_MSG_ON;
                    } else {
                        SvcGuiPalCtrlFlag &= ~SVC_GUI_PAL_DBG_MSG_ON;
                    }
                }

                if (SVC_OK != SvcGuiPalette_Create()) {
                    LOG_GUI_PAL_NG("Failure to proc gui palette 'show' cmd - create diag box fail!", 0U, 0U);
                } else {
                    PRetVal = SvcGuiPalette_Show(VoutIdx, 1U, StartX, StartY, Width, Height); AmbaMisra_TouchUnused(&PRetVal);
                }
            }
        } else {
            RetVal = SVC_NG;
            LOG_GUI_PAL_NG("Failure to proc gui palette 'show' cmd - only support on(1) and off(0)!", 0U, 0U);
        }
    }

    return RetVal;
}

static void SvcGuiPalette_ShellShowU(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 PRetVal;

    AmbaSvcWrap_MisraMemset(SvcGuiPalShellStr, 0, sizeof(SvcGuiPalShellStr));
    SVC_WRAP_SNPRINT "          %sshow%s               : show osd palette\n"
        SVC_SNPRN_ARG_S    SvcGuiPalShellStr
        SVC_SNPRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_CSTR ("\033""[0m")                                    SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_BSIZE sizeof(SvcGuiPalShellStr)
        SVC_SNPRN_ARG_RLEN  &PRetVal
        SVC_SNPRN_ARG_E
    PrintFunc(SvcGuiPalShellStr);
    PrintFunc("          ------------------------------------------------------\n");
    PrintFunc("            [VoutIdx]        : 0: LCD, 1: TV\n");
    PrintFunc("            [On/Off]         : 1: On, 0: Off\n");
    PrintFunc("            [StarX]          : Form X\n");
    PrintFunc("            [StarY]          : Form Y\n");
    PrintFunc("            [Width]          : Form Width\n");
    PrintFunc("            [Height]         : Form Height\n");
    PrintFunc("\n");
}

static UINT32 SvcGuiPalette_ShellMove(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 3U) {
        RetVal = SVC_NG;
        LOG_GUI_PAL_NG("Failure to proc gui palette 'mv' cmd - invalid arg count!", 0U, 0U);
        AmbaMisra_TouchUnused(&PrintFunc);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 Idx;

        PRetVal = SvcWrap_strtoul(pArgVector[2U], &Idx); AmbaMisra_TouchUnused(&PRetVal);


        if (0 == SvcWrap_strcmp(pArgVector[3U], "up")) {

            if (SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelRow == 0U ) {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelRow = SVC_GUI_PAL_BLK_ROW_NUM - 1U;
            } else {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelRow -= 1U;
            }

        } else if (0 == SvcWrap_strcmp(pArgVector[3U], "down")) {

            if (SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelRow == (SVC_GUI_PAL_BLK_ROW_NUM - 1U) ) {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelRow = 0U;
            } else {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelRow += 1U;
            }

        } else if (0 == SvcWrap_strcmp(pArgVector[3U], "left")) {

            if (SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelCol == 0U ) {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelCol = SVC_GUI_PAL_BLK_COL_NUM - 1U;
            } else {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelCol -= 1U;
            }

        } else if (0 == SvcWrap_strcmp(pArgVector[3U], "right")) {

            if (SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelCol == ( SVC_GUI_PAL_BLK_COL_NUM - 1U ) ) {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelCol = 0U;
            } else {
                SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelCol += 1U;
            }

        } else {
            UINT32 SelIdx = 0U;

            PRetVal = SvcWrap_strtoul(pArgVector[3U], &SelIdx); AmbaMisra_TouchUnused(&PRetVal);

            SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelRow = SelIdx / SVC_GUI_PAL_BLK_COL_NUM;
            SvcGuiPalCtrl.BlkCtrl[Idx].BlkSelCol = SelIdx % SVC_GUI_PAL_BLK_COL_NUM;
        }

        PRetVal = SvcGuiPalette_Show(Idx, 1, 0, 0, 0, 0); AmbaMisra_TouchUnused(&PRetVal);
    }

    return RetVal;
}

static void SvcGuiPalette_ShellMoveU(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 PRetVal;

    AmbaSvcWrap_MisraMemset(SvcGuiPalShellStr, 0, sizeof(SvcGuiPalShellStr));
    SVC_WRAP_SNPRINT "          %smv%s                 : move selected block\n"
        SVC_SNPRN_ARG_S    SvcGuiPalShellStr
        SVC_SNPRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_CSTR ("\033""[0m")                                    SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_BSIZE sizeof(SvcGuiPalShellStr)
        SVC_SNPRN_ARG_RLEN  &PRetVal
        SVC_SNPRN_ARG_E
    PrintFunc(SvcGuiPalShellStr);
    PrintFunc("          ------------------------------------------------------\n");
    PrintFunc("            [vout] [up]      : move up\n");
    PrintFunc("            [vout] [down]    : move down\n");
    PrintFunc("            [vout] [left]    : move left\n");
    PrintFunc("            [vout] [right]   : move right\n");
    PrintFunc("            [vout] [index]   : move by table index (0 ~ 255)\n");
    PrintFunc("\n");
}

static void SvcGuiPalette_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ShellIdx, RetLen;
    UINT32 ShellCount = (UINT32)(sizeof(SvcGuiPalShellFunc)) / (UINT32)(sizeof(SvcGuiPalShellFunc[0]));

    PrintFunc("        palette\n");

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if (SvcGuiPalShellFunc[ShellIdx].pFunc != NULL) {
            if (SvcGuiPalShellFunc[ShellIdx].pUsage == NULL) {

                AmbaSvcWrap_MisraMemset(SvcGuiPalShellStr, 0, sizeof(SvcGuiPalShellStr));
                SVC_WRAP_SNPRINT "          %s\n"
                    SVC_SNPRN_ARG_S     SvcGuiPalShellStr
                    SVC_SNPRN_ARG_CSTR  SvcGuiPalShellFunc[ShellIdx].CmdName SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_BSIZE sizeof(SvcGuiPalShellStr)
                    SVC_SNPRN_ARG_RLEN  &RetLen
                    SVC_SNPRN_ARG_E
                PrintFunc(SvcGuiPalShellStr);

            } else {
                (SvcGuiPalShellFunc[ShellIdx].pUsage)(PrintFunc);
            }
        }
    }

    PrintFunc("\n");
}

/**
 * Svc Gui palette shell command entry function
 * @param [in] ArgCount input arguments counter
 * @param [in] pArgVector input arguments data
 * @param [in] PrintFunc print function entry
 * return None
 */
void SvcGuiPalette_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    SvcGuiPalette_ShellEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        LOG_GUI_PAL_API("", 0U, 0U);
        SVC_WRAP_PRINT "==== %sShell Arguments Setting%s (%s%d%s) ===="
            SVC_PRN_ARG_S SVC_LOG_GUI_PAL
            SVC_PRN_ARG_CSTR ("\033""[38;2;255;255;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 ArgCount                                       SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR ("\033""[0m")                                    SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            SVC_WRAP_PRINT "  pArgVector[%s%d%s] : %s%s%s"
               SVC_PRN_ARG_S SVC_LOG_GUI_PAL
               SVC_PRN_ARG_CSTR   ("\033""[38;2;255;128;128m""\033""[48;2;0;0;0m") SVC_PRN_ARG_POST
               SVC_PRN_ARG_UINT32 SIdx                                             SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[0m")                                    SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[38;2;0;255;128m""\033""[48;2;0;0;0m")   SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   pArgVector[SIdx]                                 SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   ("\033""[0m")                                    SVC_PRN_ARG_POST
               SVC_PRN_ARG_E
        }
    }

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
        // LOG_GUI_PAL_DBG("Failure to proc gui palette shell cmd - invalid arg count!", 0U, 0U);
    }

    if (pArgVector == NULL) {
        RetVal = SVC_NG;
        // LOG_GUI_PAL_DBG("Failure to proc gui palette shell cmd - invalid arg vector!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (ArgCount >= 1U) && (pArgVector != NULL)) {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcGuiPalShellFunc)) / (UINT32)(sizeof(SvcGuiPalShellFunc[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if (SvcGuiPalShellFunc[ShellIdx].pFunc != NULL) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcGuiPalShellFunc[ShellIdx].CmdName)) {
                    PRetVal = (SvcGuiPalShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector, PrintFunc); AmbaMisra_TouchUnused(&PRetVal);
                    RetVal = SVC_OK;
                    break;
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcGuiPalette_CmdUsage(PrintFunc);
    }
}
