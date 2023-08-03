/**
 *  @file SvcGuiDiagBox.c
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
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER Fail OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details svc application draw gui
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaNVM_Partition.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcNvm.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcButtonCtrl.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcGuiDiagBox.h"

#define SVC_DIAG_BOX_INIT            (0x1U)
#define SVC_DIAG_BOX_READY           (0x2U)
#define SVC_DIAG_BOX_SHOW            (0x4U)
#define SVC_DIAG_BOX_UPDATE          (0x10U)
#define SVC_DIAG_BOX_SHELL_INIT      (0x1000U)
#define SVC_DIAG_BOX_RES_BLK_INIT    (0x2000U)
#define SVC_DIAG_BOX_RES_BLK_LOAD    (0x4000U)
#define SVC_DIAG_BOX_EXTRA_OSD       (0x8000U)

#define SVC_DIAG_BOX_NAME                ("SvcDiagBox")
#define SVC_DIAG_BOX_TIMEOUT_INTERVAL    (100U)
#define SVC_DIAG_BOX_OSD_PTN_FILE        "diagbox.bmp"
#define SVC_DIAG_BOX_OSD_PTN_PITCH       (524U)
#define SVC_DIAG_BOX_OSD_PTN_HEIGHT      (112U)
#define SVC_DIAG_BOX_OSD_PTN_SIZE        ( SVC_DIAG_BOX_OSD_PTN_PITCH * SVC_DIAG_BOX_OSD_PTN_HEIGHT )
#define SVC_DIAG_BOX_OSD_PTN_EXT_SIZE    ( 199232 )

#define SVC_DIAG_BOX_FORM_BLK_L_TOP      (0U)
#define SVC_DIAG_BOX_FORM_BLK_C_TOP      (1U)
#define SVC_DIAG_BOX_FORM_BLK_R_TOP      (2U)
#define SVC_DIAG_BOX_FORM_BLK_L_CEN      (3U)
#define SVC_DIAG_BOX_FORM_BLK_C_CEN      (4U)
#define SVC_DIAG_BOX_FORM_BLK_R_CEN      (5U)
#define SVC_DIAG_BOX_FORM_BLK_L_BOT      (6U)
#define SVC_DIAG_BOX_FORM_BLK_C_BOT      (7U)
#define SVC_DIAG_BOX_FORM_BLK_R_BOT      (8U)
#define SVC_DIAG_BOX_FORM_BLK_NUM        (9U)

#define SVC_DIAG_BOX_STR_BLK_YES         (0U)
#define SVC_DIAG_BOX_STR_BLK_NO          (1U)
#define SVC_DIAG_BOX_STR_BLK_CANCEL      (2U)
#define SVC_DIAG_BOX_STR_BLK_NUM         (3U)

#define SVC_DIAG_BOX_CAVS_FORM       (0U)
#define SVC_DIAG_BOX_CAVS_BTN_YES    (1U)
#define SVC_DIAG_BOX_CAVS_BTN_NO     (2U)
#define SVC_DIAG_BOX_CAVS_BTN_CANCEL (3U)
#define SVC_DIAG_BOX_CAVS_LAYOUT     (4U)
#define SVC_DIAG_BOX_CAVS_NUM        (5U)

#define SVC_DIAG_BOX_BTN_ID_SEL      (0U)
#define SVC_DIAG_BOX_BTN_ID_CLK      (1U)
#define SVC_DIAG_BOX_BTN_ID_NUM      (2U)

#ifdef CONFIG_ICAM_32BITS_OSD_USED
typedef UINT32 SVC_OSD_PIXEL;
#else
typedef UINT8  SVC_OSD_PIXEL;
#endif

typedef struct {
    UINT32         StartX;
    UINT32         StartY;
    UINT32         Width;
    UINT32         Height;
    UINT32         Pitch;
    SVC_OSD_PIXEL *pBlkData;
} SVC_DIAG_BOX_OSD_BLK_s;

typedef struct {
    SVC_DIAG_BOX_OSD_BLK_s *pFormBlks;
    SVC_DIAG_BOX_OSD_BLK_s *pFormSelBlks;
    SVC_DIAG_BOX_OSD_BLK_s *pStrBlks;
    SVC_DIAG_BOX_OSD_BLK_s *pStrSelBlks;
} SVC_DIAG_BOX_FORM_BLK_CTRL_s;

typedef struct {
    UINT32            State;
#define SVC_DIAG_BOX_OSD_ENA    (0x1U)
#define SVC_DIAG_BOX_OSD_UPD    (0x2U)
#define SVC_DIAG_BOX_OSD_BUSY   (0x4U)
    SVC_GUI_CANVAS_s  Canvas;
    SVC_GUI_CANVAS_s  SubCanvas;
} SVC_DIAG_BOX_OSD_CANVAS_s;

typedef struct {
    UINT32                 State;
    SVC_GUI_CANVAS_s       Canvas;
    SVC_DIAG_BOX_OSD_BLK_s OsdBlk;
} SVC_DIAG_BOX_EXTRA_OSD_s;

typedef struct {
    UINT32                 State;
#define SVC_DIAG_BOX_BTN_INIT   (0x1U)
#define SVC_DIAG_BOX_BTN_REG    (0x2U)
#define SVC_DIAG_BOX_BTN_REQ    (0x4U)
    UINT32                 Type;
    SVC_BUTTON_CTRL_ID     ID;
    char                   BtnName[SVC_DIAG_BOX_BTN_ID_NUM][16];
    SVC_BUTTON_CTRL_LIST_s List[SVC_DIAG_BOX_BTN_ID_NUM];
} SVC_DIAG_BOX_BTN_CTRL_s;

typedef struct {
    char                         Name[32];
    AMBA_KAL_EVENT_FLAG_t        EventFlag;
#define SVC_DIAG_BOX_CTRL_BTN_CLICK (0x100U)
    char                         OsdResFile[64];
    SVC_DIAG_BOX_FORM_BLK_CTRL_s OsdResBlkCtrl;
    UINT32                       GuiLevel;
    SVC_DIAG_BOX_FORM_s          FormInfo;
    SVC_DIAG_BOX_OSD_CANVAS_s    Canvas[SVC_DIAG_BOX_CAVS_NUM];
    SVC_DIAG_BOX_EXTRA_OSD_s     ExtraOsd;
    SVC_DIAG_BOX_BTN_CTRL_s      BtnCtrl;
} SVC_DIAG_BOX_CTRL_s;

#define SVC_LOG_DIAGBOX "DBOX"
#define SVC_LOG_DIAG_HL_TITLE_1   ("\033""[38;2;255;255;128m")
#define SVC_LOG_DIAG_HL_NUM       ("\033""[38;2;153;217;234m")
#define SVC_LOG_DIAG_HL_STR       ("\033""[38;2;255;174;201m")
#define SVC_LOG_DIAG_HL_END       ("\033""[0m")
static void LOG_DIAG_BOX_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_DIAG_BOX_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_DIAG_BOX_API(const char *pStr, UINT32 Arg1, UINT32 Arg2);
static void LOG_DIAG_BOX_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2);
#define     LOG_DIAG_BOX_ERR_HDLR SvcDiagBox_ErrHdlr(__func__, __LINE__, PRetVal);

static SVC_OSD_PIXEL          SvcDiagBoxFileBuf[SVC_DIAG_BOX_OSD_PTN_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_OSD_PIXEL          SvcDiagBoxOsdPtnBuf[SVC_DIAG_BOX_OSD_PTN_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_OSD_PIXEL          SvcDiagBoxExtraOsdBuf[SVC_DIAG_BOX_OSD_PTN_EXT_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_DIAG_BOX_OSD_BLK_s SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_NUM] GNU_SECTION_NOZEROINIT;
static SVC_DIAG_BOX_OSD_BLK_s SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_NUM] GNU_SECTION_NOZEROINIT;
static SVC_DIAG_BOX_OSD_BLK_s SvcDiagBoxStrSelBlks[SVC_DIAG_BOX_STR_BLK_NUM] GNU_SECTION_NOZEROINIT;
static SVC_DIAG_BOX_OSD_BLK_s SvcDiagBoxStrBlks[SVC_DIAG_BOX_STR_BLK_NUM] GNU_SECTION_NOZEROINIT;

static SVC_DIAG_BOX_CTRL_s SvcDiagBoxCtrl GNU_SECTION_NOZEROINIT;
static UINT32              SvcDiagBoxCtrlFlag = 0U;

static void   SvcDiagBox_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

static void   SvcDiagBox_ResBlkInit(void);
static UINT32 SvcDiagBox_ResBlkLoad(const char *pFileName, const UINT8 *pBuf, UINT32 BufSize);
static UINT32 SvcDiagBox_ResBlkWaitDone(INT32 TimeOut);
static void   SvcDiagBox_StrBmpRescale(SVC_OSD_PIXEL *pSrcBuf, UINT32 SrcWidth, UINT32 SrcHeight, SVC_OSD_PIXEL *pDstBuf, UINT32 DstWidth, UINT32 DstHeight);
static void   SvcDiagBox_StrBmpDraw(SVC_GUI_CANVAS_s *pCavs, SVC_DIAG_BOX_OSD_BLK_s *pOsdBlk);
// static UINT32 SvcDiagBox_OsdPtnLoad(const char *pFilePath, SVC_OSD_PIXEL *pBuf, UINT32 BufSize);
static UINT32 SvcDiagBox_OsdPtnConfig(void);
static void   SvcDiagBox_GuiDraw(UINT32 VoutIdx, UINT32 Level);
static void   SvcDiagBox_GuiUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void   SvcDiagBox_FormOsdDraw(UINT32 Level);
static void   SvcDiagBox_BtnYesOsdDraw(UINT32 Level);
static void   SvcDiagBox_BtnNoOsdDraw(UINT32 Level);
static void   SvcDiagBox_BtnCancelOsdDraw(UINT32 Level);
static void   SvcDiagBox_LayoutOsdDraw(UINT32 Level);
static UINT32 SvcDiagBox_BtnSelHdlr(void);
static UINT32 SvcDiagBox_BtnClickHdlr(void);
#ifndef CONFIG_SVC_LVGL_USED
static void   SvcDiagBox_RectangleDraw(SVC_GUI_CANVAS_s *pCavs, SVC_DIAG_BOX_OSD_BLK_s *pOsdBlk);
#endif

// Private function definition for shell command.
typedef UINT32 (*SVC_DIAG_BOX_SHELL_FUNC_f)(UINT32 ArgCount, char * const *pArgVector);
typedef void   (*SVC_DIAG_BOX_SHELL_USAGE_f)(AMBA_SHELL_PRINT_f PrintFunc);
typedef struct {
    char                      CmdName[32];
    SVC_DIAG_BOX_SHELL_FUNC_f  pFunc;
    SVC_DIAG_BOX_SHELL_USAGE_f pUsage;
} SVC_DIAG_BOX_SHELL_FUNC_s;

static void   SvcDiagBox_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);
static void   SvcDiagBox_ShellStrToU32(const char *pString, UINT32 *pValue);
static void   SvcDiagBox_ShellEntryInit(void);
static UINT32 SvcDiagBox_ShellShow(UINT32 ArgCount, char * const *pArgVector);
static void   SvcDiagBox_ShellShowU(AMBA_SHELL_PRINT_f PrintFunc);
static UINT32 SvcDiagBox_ShellBtn(UINT32 ArgCount, char * const *pArgVector);
static void   SvcDiagBox_ShellBtnU(AMBA_SHELL_PRINT_f PrintFunc);
static UINT32 SvcDiagBox_ShellDbg(UINT32 ArgCount, char * const *pArgVector);
static void   SvcDiagBox_ShellDbgU(AMBA_SHELL_PRINT_f PrintFunc);

#define SVC_DIAG_BOX_SHELL_CMD_NUM  (3U)
static SVC_DIAG_BOX_SHELL_FUNC_s SvcDiagBoxShellFunc[SVC_DIAG_BOX_SHELL_CMD_NUM] GNU_SECTION_NOZEROINIT;
static char SvcDiagBoxShellStr[128] GNU_SECTION_NOZEROINIT;

static void LOG_DIAG_BOX_OK(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_OK(SVC_LOG_DIAGBOX, pStr, Arg1, Arg2);
}

static void LOG_DIAG_BOX_NG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_NG(SVC_LOG_DIAGBOX, pStr, Arg1, Arg2);
}

static void LOG_DIAG_BOX_API(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_DBG(SVC_LOG_DIAGBOX, pStr, Arg1, Arg2);
}

static void LOG_DIAG_BOX_DBG(const char *pStr, UINT32 Arg1, UINT32 Arg2)
{
    SvcLog_DBG(SVC_LOG_DIAGBOX, pStr, Arg1, Arg2);
}

#if 0
static UINT32 SvcDiagBox_OsdPtnLoad(const char *pFilePath, SVC_OSD_PIXEL *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = SVC_OK;

    if (pFilePath == NULL) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to load osd pattern - input should file path should not null!", 0U, 0U);
    } else if (pBuf == NULL) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to load osd pattern - input buffer should not null!", 0U, 0U);
    } else if (BufSize == 0U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to load osd pattern - input buffer size should not null!", 0U, 0U);
    } else {
        RetVal = SvcOsd_LoadBmp(pBuf, BufSize, pFilePath);
        if (RetVal != SVC_OK) {
            LOG_DIAG_BOX_NG("Fail to load OSD pattern!", 0U, 0U);
        } else {
            LOG_DIAG_BOX_DBG("Successful to load OSD pattern!", 0U, 0U);
        }
    }

    return RetVal;
}
#endif

static UINT32 SvcDiagBox_OsdPtnConfig(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 Idx, Row, Ofs, CopySize;
    SVC_DIAG_BOX_OSD_BLK_s *pBlks;
    SVC_OSD_PIXEL *pSrc, *pDst;
    SVC_OSD_PIXEL *PtnBuf = SvcDiagBoxFileBuf;

    // Reset the osd pattern buffer
    AmbaSvcWrap_MisraMemset(SvcDiagBoxOsdPtnBuf, 0, sizeof(SvcDiagBoxOsdPtnBuf));

    // Configure form osd block setting
    {
        for (Idx = 0U; Idx < SVC_DIAG_BOX_FORM_BLK_NUM; Idx ++) {
            pBlks = &(SvcDiagBoxFormBlks[Idx]);

            Ofs   = (pBlks->Pitch * pBlks->StartY) + pBlks->StartX;
            CopySize = pBlks->Width * (UINT32)sizeof(SVC_OSD_PIXEL);

            pSrc  = &(PtnBuf[Ofs]);
            pDst  = (pBlks->pBlkData);

            for (Row = 0U; Row < pBlks->Height; Row ++) {
                AmbaSvcWrap_MisraMemcpy( pDst, pSrc, CopySize );
                pSrc = &(pSrc[pBlks->Pitch]);
                pDst = &(pDst[pBlks->Width]);
            }
        }
    }

    // Configure selected form osd block setting
    {
        for (Idx = 0U; Idx < SVC_DIAG_BOX_FORM_BLK_NUM; Idx ++) {
            pBlks = &(SvcDiagBoxFormSelBlks[Idx]);

            Ofs   = (pBlks->Pitch * pBlks->StartY) + pBlks->StartX;
            CopySize = pBlks->Width * (UINT32)sizeof(SVC_OSD_PIXEL);

            pSrc  = &(PtnBuf[Ofs]);
            pDst  = (pBlks->pBlkData);

            for (Row = 0U; Row < pBlks->Height; Row ++) {
                AmbaSvcWrap_MisraMemcpy( pDst, pSrc, CopySize );
                pSrc = &(pSrc[pBlks->Pitch]);
                pDst = &(pDst[pBlks->Width]);
            }
        }
    }

    // Configure string osd block setting
    {
        for (Idx = 0U; Idx < SVC_DIAG_BOX_STR_BLK_NUM; Idx ++) {
            pBlks = &(SvcDiagBoxStrSelBlks[Idx]);

            Ofs   = (pBlks->Pitch * pBlks->StartY) + pBlks->StartX;
            CopySize = pBlks->Width * (UINT32)sizeof(SVC_OSD_PIXEL);

            pSrc  = &(PtnBuf[Ofs]);
            pDst  = (pBlks->pBlkData);

            for (Row = 0U; Row < pBlks->Height; Row ++) {
                AmbaSvcWrap_MisraMemcpy( pDst, pSrc, CopySize );
                pSrc = &(pSrc[pBlks->Pitch]);
                pDst = &(pDst[pBlks->Width]);
            }
        }
        for (Idx = 0U; Idx < SVC_DIAG_BOX_STR_BLK_NUM; Idx ++) {
            pBlks = &(SvcDiagBoxStrBlks[Idx]);

            Ofs   = (pBlks->Pitch * pBlks->StartY) + pBlks->StartX;
            CopySize = pBlks->Width * (UINT32)sizeof(SVC_OSD_PIXEL);

            pSrc  = &(PtnBuf[Ofs]);
            pDst  = (pBlks->pBlkData);

            for (Row = 0U; Row < pBlks->Height; Row ++) {
                AmbaSvcWrap_MisraMemcpy( pDst, pSrc, CopySize );
                pSrc = &(pSrc[pBlks->Pitch]);
                pDst = &(pDst[pBlks->Width]);
            }
        }
    }
    AmbaMisra_TouchUnused(pSrc);
    AmbaMisra_TouchUnused(pDst);
    AmbaMisra_TouchUnused(pBlks);

    return RetVal;
}

static void SvcDiagBox_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            SVC_WRAP_PRINT "Catch ErrCode(0x%08X) @ %s, %d"
            SVC_PRN_ARG_S SVC_LOG_DIAGBOX
                SVC_PRN_ARG_PROC   SvcLog_NG
                SVC_PRN_ARG_UINT32 ErrCode  SVC_PRN_ARG_POST
                SVC_PRN_ARG_CSTR   pCaller  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 CodeLine SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        }
    }
}

static void SvcDiagBox_ResBlkInit(void)
{
    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_RES_BLK_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcDiagBoxFormBlks, 0, sizeof(SvcDiagBoxFormBlks));
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_L_TOP] = (SVC_DIAG_BOX_OSD_BLK_s) {   0U,  99U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[0U])    };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_C_TOP] = (SVC_DIAG_BOX_OSD_BLK_s) {  13U,  99U,   3U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[169U])  };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_R_TOP] = (SVC_DIAG_BOX_OSD_BLK_s) {  26U,  99U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[338U])  };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_L_CEN] = (SVC_DIAG_BOX_OSD_BLK_s) {   0U,  86U,  13U,   3U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[507U])  };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_C_CEN] = (SVC_DIAG_BOX_OSD_BLK_s) {  13U,  86U,   3U,   3U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[676U])  };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_R_CEN] = (SVC_DIAG_BOX_OSD_BLK_s) {  26U,  86U,  13U,   3U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[845U])  };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_L_BOT] = (SVC_DIAG_BOX_OSD_BLK_s) {   0U,  73U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[1014U]) };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_C_BOT] = (SVC_DIAG_BOX_OSD_BLK_s) {  13U,  73U,   3U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[1183U]) };
        SvcDiagBoxFormBlks[SVC_DIAG_BOX_FORM_BLK_R_BOT] = (SVC_DIAG_BOX_OSD_BLK_s) {  26U,  73U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[1352U]) };

        AmbaSvcWrap_MisraMemset(SvcDiagBoxFormSelBlks, 0, sizeof(SvcDiagBoxFormSelBlks));
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_L_TOP] = (SVC_DIAG_BOX_OSD_BLK_s) {   0U,  43U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[1521U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_C_TOP] = (SVC_DIAG_BOX_OSD_BLK_s) {  13U,  43U,   3U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[1690U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_R_TOP] = (SVC_DIAG_BOX_OSD_BLK_s) {  26U,  43U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[1859U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_L_CEN] = (SVC_DIAG_BOX_OSD_BLK_s) {   0U,  30U,  13U,   3U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[2028U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_C_CEN] = (SVC_DIAG_BOX_OSD_BLK_s) {  13U,  30U,   3U,   3U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[2197U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_R_CEN] = (SVC_DIAG_BOX_OSD_BLK_s) {  26U,  30U,  13U,   3U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[2366U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_L_BOT] = (SVC_DIAG_BOX_OSD_BLK_s) {   0U,  17U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[2535U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_C_BOT] = (SVC_DIAG_BOX_OSD_BLK_s) {  13U,  17U,   3U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[2704U]) };
        SvcDiagBoxFormSelBlks[SVC_DIAG_BOX_FORM_BLK_R_BOT] = (SVC_DIAG_BOX_OSD_BLK_s) {  26U,  17U,  13U,  13U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[2873U]) };

        AmbaSvcWrap_MisraMemset(SvcDiagBoxStrBlks, 0, sizeof(SvcDiagBoxStrBlks));
        SvcDiagBoxStrBlks[SVC_DIAG_BOX_STR_BLK_YES   ] = (SVC_DIAG_BOX_OSD_BLK_s) {  40U,  56U, 115U,  56U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[3042U])  };
        SvcDiagBoxStrBlks[SVC_DIAG_BOX_STR_BLK_NO    ] = (SVC_DIAG_BOX_OSD_BLK_s) { 155U,  56U, 115U,  56U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[9482U])  };
        SvcDiagBoxStrBlks[SVC_DIAG_BOX_STR_BLK_CANCEL] = (SVC_DIAG_BOX_OSD_BLK_s) { 270U,  56U, 255U,  56U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[15922U]) };

        AmbaSvcWrap_MisraMemset(SvcDiagBoxStrSelBlks, 0, sizeof(SvcDiagBoxStrSelBlks));
        SvcDiagBoxStrSelBlks[SVC_DIAG_BOX_STR_BLK_YES   ] = (SVC_DIAG_BOX_OSD_BLK_s) {  40U,   0U, 115U,  56U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[30202U]) };
        SvcDiagBoxStrSelBlks[SVC_DIAG_BOX_STR_BLK_NO    ] = (SVC_DIAG_BOX_OSD_BLK_s) { 155U,   0U, 115U,  56U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[36642U]) };
        SvcDiagBoxStrSelBlks[SVC_DIAG_BOX_STR_BLK_CANCEL] = (SVC_DIAG_BOX_OSD_BLK_s) { 270U,   0U, 255U,  56U, SVC_DIAG_BOX_OSD_PTN_PITCH, &(SvcDiagBoxOsdPtnBuf[43082U]) };

        SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_RES_BLK_INIT;
    }
}

static UINT32 SvcDiagBox_ResBlkLoad(const char *pFileName, const UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 RetVal = 0U;

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_RES_BLK_LOAD) == 0U) {
        UINT8 *pCurBuf = NULL;
        UINT32 CurBufSize = BufSize;
        UINT32 FileSize = 0U;

        AmbaMisra_TypeCast(&pCurBuf, &pBuf);

        if (pCurBuf == NULL) {
            const SVC_OSD_PIXEL *pTmpBuf = SvcDiagBoxFileBuf;
            AmbaMisra_TypeCast(&pCurBuf, &pTmpBuf);
            CurBufSize = (UINT32)(sizeof(SvcDiagBoxFileBuf));
        }

        // Load the OSD pattern
        RetVal = SvcNvm_GetRomFileSize(AMBA_USER_PARTITION_SYS_DATA, pFileName, &FileSize);
        if (RetVal != 0U) {
            LOG_DIAG_BOX_NG("Fail to initial diagbox - get bmp file size fail! ErrCode(0x%08X)", RetVal, 0U);
        } else if (FileSize > CurBufSize) {
            LOG_DIAG_BOX_NG("Fail to initial diagbox - file size(0x%X) larger than buffer size(0x%X)!", FileSize, CurBufSize);
        } else {
            RetVal = SvcNvm_ReadRomFileCrc(AMBA_USER_PARTITION_SYS_DATA, pFileName, 0U, FileSize, pCurBuf, 10000);
            if (RetVal != 0U) {
                LOG_DIAG_BOX_NG("Fail to initial diagbox - read bmp file fail! ErrCode(0x%08X)", RetVal, 0U);
            } else {
                UINT32 DataOfs = 0U;

                /*
                 * -- Bitmap header --
                 * 2-byte : Type        offset 0
                 * 4-byte : Size        offset 2
                 * 4-byte : reserved    offset 6
                 * 4-byte : offset      offset 10
                 */
                AmbaSvcWrap_MisraMemcpy(&DataOfs, &pCurBuf[10], sizeof(UINT32));

                pCurBuf = &(pCurBuf[DataOfs]);

                // Configure the form OSD pattern
                if (pBuf != NULL) {
                    AmbaSvcWrap_MisraMemcpy(SvcDiagBoxFileBuf, pCurBuf, sizeof(SvcDiagBoxFileBuf));
                }

                RetVal = SvcDiagBox_OsdPtnConfig();
                if (SVC_OK != RetVal) {
                    LOG_DIAG_BOX_NG("Fail to initial diag box - load the form OSD pattern fail!", 0U, 0U);
                } else {
                    SvcDiagBoxCtrl.OsdResBlkCtrl.pFormBlks    = SvcDiagBoxFormBlks;
                    SvcDiagBoxCtrl.OsdResBlkCtrl.pFormSelBlks = SvcDiagBoxFormSelBlks;
                    SvcDiagBoxCtrl.OsdResBlkCtrl.pStrBlks     = SvcDiagBoxStrBlks;
                    SvcDiagBoxCtrl.OsdResBlkCtrl.pStrSelBlks  = SvcDiagBoxStrSelBlks;
                }
            }
        }

        SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_RES_BLK_LOAD;
    }

    return RetVal;
}

static UINT32 SvcDiagBox_ResBlkWaitDone(INT32 TimeOut)
{
    UINT32 RetVal = SVC_OK, PRetVal;
    INT32 RemainTime = TimeOut, ExitLoop = 0;

    while ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_RES_BLK_LOAD) == 0U) {
        PRetVal = AmbaKAL_TaskSleep(10U); LOG_DIAG_BOX_ERR_HDLR

        if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_RES_BLK_LOAD) > 0U) {
            ExitLoop = 1;
        } else {
            RemainTime -= 10;
            if (RemainTime <= 0) {
                RetVal = SVC_NG;
                ExitLoop = 1;
            }
        }

        if (ExitLoop > 0) {
            break;
        }
    }

    return RetVal;
}

#ifndef CONFIG_SVC_LVGL_USED
static void SvcDiagBox_RectangleDraw(SVC_GUI_CANVAS_s *pCavs, SVC_DIAG_BOX_OSD_BLK_s *pOsdBlk)
{
    UINT32 PRetVal;

    if ((pCavs != NULL) && (pOsdBlk != NULL)) {
        UINT32 RowIdx, RowCnt, ColIdx, ColCnt;
        UINT32 DrawStartX, DrawStartY, DrawEndX, DrawEndY;
        SVC_OSD_BMP_s BmpInfo;

        ColCnt  = pCavs->Width;
        ColCnt -= pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_TOP].Width;
        ColCnt -= pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_TOP].Width;
        ColCnt /= pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_TOP].Width;

        RowCnt  = pCavs->Height;
        RowCnt -= pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_TOP].Height;
        RowCnt -= pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_BOT].Height;
        RowCnt /= pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_CEN].Height;

        // Draw the left-top region
        DrawStartX  = pCavs->StartX;
        DrawStartY  = pCavs->StartY;
        DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_TOP].Width;
        DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_TOP].Height;
        BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_TOP].pBlkData;
        PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR

        // Draw the center-top region
        DrawStartX  += pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_TOP].Width;
        BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_TOP].pBlkData;
        for (ColIdx = 0U; ColIdx < ColCnt; ColIdx ++) {
            DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_TOP].Width;
            DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_TOP].Height;
            PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR
            DrawStartX += pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_TOP].Width;
        }

        // Draw the center-top region
        DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_TOP].Width;
        DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_TOP].Height;
        BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_TOP].pBlkData;
        PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR

        // Draw the left/center/right-center region
        DrawStartY  = pCavs->StartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_TOP].Height;
        for (RowIdx = 0U; RowIdx < RowCnt; RowIdx ++) {
            // Draw the left-center region
            DrawStartX  = pCavs->StartX;
            DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_CEN].Width;
            DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_CEN].Height;
            BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_CEN].pBlkData;
            PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR

            // Draw the center-center region
            DrawStartX  += pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_CEN].Width;
            BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_CEN].pBlkData;
            for (ColIdx = 0U; ColIdx < ColCnt; ColIdx ++) {
                DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_CEN].Width;
                DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_CEN].Height;
                PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR
                DrawStartX += pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_CEN].Width;
            }

            // Draw the right-center region
            DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_CEN].Width;
            DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_CEN].Height;
            BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_CEN].pBlkData;
            PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR

            DrawStartY += pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_CEN].Height;
        }

        // Draw the left-bottom region
        DrawStartX  = pCavs->StartX;
        DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_BOT].Width;
        DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_BOT].Height;
        BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_BOT].pBlkData;
        PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR

        // Draw the center-bottom region
        DrawStartX  += pOsdBlk[SVC_DIAG_BOX_FORM_BLK_L_BOT].Width;
        BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_BOT].pBlkData;
        for (ColIdx = 0U; ColIdx < ColCnt; ColIdx ++) {
            DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_BOT].Width;
            DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_BOT].Height;
            
            PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR
            DrawStartX += pOsdBlk[SVC_DIAG_BOX_FORM_BLK_C_BOT].Width;
        }

        // Draw the right-bottom region
        DrawEndX    = DrawStartX + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_BOT].Width;
        DrawEndY    = DrawStartY + pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_BOT].Height;
        BmpInfo.data = (UINT8*)pOsdBlk[SVC_DIAG_BOX_FORM_BLK_R_BOT].pBlkData;
        PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, DrawStartX, DrawStartY, DrawEndX, DrawEndY, &BmpInfo ); LOG_DIAG_BOX_ERR_HDLR
        AmbaMisra_TouchUnused(pOsdBlk);
        AmbaMisra_TouchUnused(pCavs);
    }
}
#endif

static void SvcDiagBox_StrBmpRescale(SVC_OSD_PIXEL *pSrcBuf, UINT32 SrcWidth, UINT32 SrcHeight, SVC_OSD_PIXEL *pDstBuf, UINT32 DstWidth, UINT32 DstHeight)
{
    if (   (pSrcBuf != NULL) && (!((SrcWidth == 0U) || (SrcHeight == 0U)))
        && (pDstBuf != NULL) && (!((DstWidth == 0U) || (DstHeight == 0U)))) {
        UINT32 SrcIdx, SrcIdy, DstIdx, DstIdy;

        for (DstIdy = 0U; DstIdy < DstHeight; DstIdy ++) {
            for (DstIdx = 0U; DstIdx < DstWidth; DstIdx ++) {

                SrcIdx = ( DstIdx * SrcWidth ) / DstWidth;
                if (SrcIdx >= SrcWidth) {
                    SrcIdx = SrcWidth - 1U;
                }

                SrcIdy = ( DstIdy * SrcHeight ) / DstHeight;
                if (SrcIdy >= SrcHeight) {
                    SrcIdy = SrcHeight - 1U;
                }

                pDstBuf[(DstIdy * DstWidth) + DstIdx] = pSrcBuf[(SrcIdy * SrcWidth) + SrcIdx];
            }
        }

        AmbaMisra_TouchUnused(pSrcBuf);
        AmbaMisra_TouchUnused(pSrcBuf);
    }
}

static void SvcDiagBox_StrBmpDraw(SVC_GUI_CANVAS_s *pCavs, SVC_DIAG_BOX_OSD_BLK_s *pOsdBlk)
{
    UINT32 PRetVal;

    if ((pCavs != NULL) && (pOsdBlk != NULL)) {
        static SVC_OSD_PIXEL DrawBuf[SVC_DIAG_BOX_OSD_PTN_EXT_SIZE] GNU_SECTION_NOZEROINIT;
        static SVC_OSD_BMP_s BmpInfo = {{5U,0U,0U,0U,0U},0U,NULL};
        UINT32 BitField = 0U;
        AmbaSvcWrap_MisraMemset(DrawBuf, 0, sizeof(DrawBuf));

        SvcDiagBox_StrBmpRescale(pOsdBlk->pBlkData, pOsdBlk->Width, pOsdBlk->Height,
                                DrawBuf, pCavs->Width, pCavs->Height);
        
        BitField = (BitField | 5U) ;
        BitField = (BitField | (pCavs->Width << 10U));
        BitField = (BitField | (pCavs->Height << 21U));
        AmbaSvcWrap_MisraMemcpy(&(BmpInfo.header), &BitField, sizeof(UINT32));

        BmpInfo.data_size = (pCavs->Width * pCavs->Height) << SVC_OSD_PIXEL_SIZE_SFT;
        BmpInfo.data = (UINT8 *)DrawBuf;

        PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx,
                                 pCavs->StartX,
                                 pCavs->StartY,
                                 pCavs->StartX + pCavs->Width,
                                 pCavs->StartY + pCavs->Height,
                                 &BmpInfo); LOG_DIAG_BOX_ERR_HDLR
        AmbaMisra_TouchUnused(pOsdBlk);
        AmbaMisra_TouchUnused(pCavs);
    }
}

static void SvcDiagBox_GuiDraw(UINT32 VoutIdx, UINT32 Level)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    SvcDiagBox_FormOsdDraw(Level);
    SvcDiagBox_BtnYesOsdDraw(Level);
    SvcDiagBox_BtnNoOsdDraw(Level);
    SvcDiagBox_BtnCancelOsdDraw(Level);
    SvcDiagBox_LayoutOsdDraw(Level);
}

static void SvcDiagBox_GuiUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_UPDATE) > 0U) {
        SvcDiagBoxCtrlFlag &= ~(SVC_DIAG_BOX_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void SvcDiagBox_FormOsdDraw(UINT32 Level)
{
    AmbaMisra_TouchUnused(&Level);

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw form osd - initial diag box first!", 0U, 0U);
    } else if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw form osd - create diag box first!", 0U, 0U);
    } else {
        SVC_DIAG_BOX_OSD_CANVAS_s *pCavs = &(SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_FORM]);
        if ((pCavs->State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
            if ((pCavs->State & SVC_DIAG_BOX_OSD_UPD) > 0U) {
#ifndef CONFIG_SVC_LVGL_USED
                SvcDiagBox_RectangleDraw(&(pCavs->Canvas), SvcDiagBoxCtrl.OsdResBlkCtrl.pFormBlks);
#else
                SvcOsd_DrawDiagBoxRect(SvcDiagBoxCtrl.FormInfo.VoutIdx
                                      , pCavs->Canvas.StartX
                                      , pCavs->Canvas.StartY
                                      , pCavs->Canvas.StartX + pCavs->Canvas.Width
                                      , pCavs->Canvas.StartY + pCavs->Canvas.Height
                                      , 5U, 0xff202040U, 0xffffffffU);
#endif
                if ((SvcDiagBoxCtrl.ExtraOsd.State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
                    if ((SvcDiagBoxCtrl.ExtraOsd.State & SVC_DIAG_BOX_OSD_UPD) == 0U) {
                        if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_EXTRA_OSD) > 0U) {
                            SvcDiagBoxCtrl.ExtraOsd.OsdBlk.StartX   = 0U;
                            SvcDiagBoxCtrl.ExtraOsd.OsdBlk.StartY   = 0U;
                            SvcDiagBoxCtrl.ExtraOsd.OsdBlk.Width    = SvcDiagBoxCtrl.FormInfo.FormExtraOsdWidth;
                            SvcDiagBoxCtrl.ExtraOsd.OsdBlk.Height   = SvcDiagBoxCtrl.FormInfo.FormExtraOsdHeight;
                            SvcDiagBoxCtrl.ExtraOsd.OsdBlk.pBlkData = SvcDiagBoxExtraOsdBuf;

                            SvcDiagBox_StrBmpDraw(&(SvcDiagBoxCtrl.ExtraOsd.Canvas), &(SvcDiagBoxCtrl.ExtraOsd.OsdBlk));
                            SvcDiagBoxCtrl.ExtraOsd.State |= SVC_DIAG_BOX_OSD_UPD;
                        }
                    } else {
                        SvcDiagBox_StrBmpDraw(&(SvcDiagBoxCtrl.ExtraOsd.Canvas), &(SvcDiagBoxCtrl.ExtraOsd.OsdBlk));
                    }
                }
            }

            pCavs->State &= ~SVC_DIAG_BOX_OSD_BUSY;
        }
    }
}

static void SvcDiagBox_BtnYesOsdDraw(UINT32 Level)
{
    AmbaMisra_TouchUnused(&Level);

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw yes button osd - initial diag box first!", 0U, 0U);
    } else if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw yes button osd - create diag box first!", 0U, 0U);
    } else {
        SVC_DIAG_BOX_OSD_CANVAS_s *pCavs = &(SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_BTN_YES]);
        if ((pCavs->State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
            if ((pCavs->State & SVC_DIAG_BOX_OSD_UPD) > 0U) {
#ifndef CONFIG_SVC_LVGL_USED
                SVC_DIAG_BOX_OSD_BLK_s    *pOsdBlk;
                SVC_DIAG_BOX_OSD_BLK_s    *pStrOsdBlk;

                if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_YES) > 0U) {
                    pOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pFormSelBlks;
                    pStrOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pStrSelBlks;
                } else {
                    pOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pFormBlks;
                    pStrOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pStrBlks;
                }
                SvcDiagBox_RectangleDraw(&(pCavs->Canvas), pOsdBlk);
                SvcDiagBox_StrBmpDraw(&(pCavs->SubCanvas), &(pStrOsdBlk[SVC_DIAG_BOX_STR_BLK_YES]));
#else
                UINT32 border_color;
                UINT32 fontSize = 3U;
                UINT32 PRetVal;

                if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_YES) > 0U) {
                    border_color = 0xffe1e114U;
                } else {
                    border_color = 0xffffffffU;
                }
                SvcOsd_DrawDiagBoxRect(SvcDiagBoxCtrl.FormInfo.VoutIdx
                                      , pCavs->Canvas.StartX
                                      , pCavs->Canvas.StartY
                                      , pCavs->Canvas.StartX + pCavs->Canvas.Width
                                      , pCavs->Canvas.StartY + pCavs->Canvas.Height
                                      , 5U, 0xff202040U, border_color);
                fontSize = (pCavs->SubCanvas.Height < 40U) ?  2U : fontSize;
                fontSize = (pCavs->SubCanvas.Height < 13U) ?  1U : fontSize;
                PRetVal  = SvcOsd_DrawString(SvcDiagBoxCtrl.FormInfo.VoutIdx
                                            , pCavs->SubCanvas.StartX
                                            , pCavs->SubCanvas.StartY
                                            , fontSize, border_color, "YES");
                LOG_DIAG_BOX_ERR_HDLR
#endif

            }

            pCavs->State &= ~SVC_DIAG_BOX_OSD_BUSY;
        }
    }
}

static void SvcDiagBox_BtnNoOsdDraw(UINT32 Level)
{
    AmbaMisra_TouchUnused(&Level);

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw no button osd - initial diag box first!", 0U, 0U);
    } else if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw no button osd - create diag box first!", 0U, 0U);
    } else {
        SVC_DIAG_BOX_OSD_CANVAS_s *pCavs = &(SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_BTN_NO]);
    
        if ((pCavs->State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
            if ((pCavs->State & SVC_DIAG_BOX_OSD_UPD) > 0U) {
#ifndef CONFIG_SVC_LVGL_USED
                SVC_DIAG_BOX_OSD_BLK_s    *pOsdBlk;
                SVC_DIAG_BOX_OSD_BLK_s    *pStrOsdBlk;

                if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_NO) > 0U) {
                    pOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pFormSelBlks;
                    pStrOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pStrSelBlks;
                } else {
                    pOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pFormBlks;
                    pStrOsdBlk = SvcDiagBoxCtrl.OsdResBlkCtrl.pStrBlks;
                }

                SvcDiagBox_RectangleDraw(&(pCavs->Canvas), pOsdBlk);
                SvcDiagBox_StrBmpDraw(&(pCavs->SubCanvas), &(pStrOsdBlk[SVC_DIAG_BOX_STR_BLK_NO]));
#else
                UINT32 border_color;
                UINT32 fontSize = 3U;
                UINT32 PRetVal;

                if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_NO) > 0U) {
                    border_color = 0xffe1e114U;
                } else {
                    border_color = 0xffffffffU;
                }

                SvcOsd_DrawDiagBoxRect(SvcDiagBoxCtrl.FormInfo.VoutIdx
                                      , pCavs->Canvas.StartX
                                      , pCavs->Canvas.StartY
                                      , pCavs->Canvas.StartX + pCavs->Canvas.Width
                                      , pCavs->Canvas.StartY + pCavs->Canvas.Height
                                      , 5U, 0xff202040U, border_color);
                fontSize = (pCavs->SubCanvas.Height < 40U) ?  2U : fontSize;
                fontSize = (pCavs->SubCanvas.Height < 13U) ?  1U : fontSize;
                PRetVal  = SvcOsd_DrawString(SvcDiagBoxCtrl.FormInfo.VoutIdx
                                            , pCavs->SubCanvas.StartX
                                            , pCavs->SubCanvas.StartY
                                            , fontSize, border_color, "NO");
                LOG_DIAG_BOX_ERR_HDLR
#endif
                // Request button focus
                if ((SvcDiagBoxCtrl.BtnCtrl.State & SVC_DIAG_BOX_BTN_REG) > 0U) {
                    if ((SvcDiagBoxCtrl.BtnCtrl.State & SVC_DIAG_BOX_BTN_REQ) == 0U) {
                        if ( SVC_OK != SvcButtonCtrl_Request( &(SvcDiagBoxCtrl.BtnCtrl.ID) ) ) {
                            LOG_DIAG_BOX_NG("Fail to request button!", 0U, 0U);
                        }

                        SvcDiagBoxCtrl.BtnCtrl.State |= SVC_DIAG_BOX_BTN_REQ;
                    }
                }
            }

            pCavs->State &= ~SVC_DIAG_BOX_OSD_BUSY;
        }
    }
}

static void SvcDiagBox_BtnCancelOsdDraw(UINT32 Level)
{
    // Do nothing
    AmbaMisra_TouchUnused(&Level);
}

static void SvcDiagBox_LayoutOsdDraw(UINT32 Level)
{
    UINT32 PRetVal;

    AmbaMisra_TouchUnused(&Level);

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw no button osd - initial diag box first!", 0U, 0U);
    } else if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) == 0U) {
        LOG_DIAG_BOX_NG("Fail to draw no button osd - create diag box first!", 0U, 0U);
    } else {
        SVC_DIAG_BOX_OSD_CANVAS_s *pCavs = SvcDiagBoxCtrl.Canvas;
        SVC_OSD_BMP_s BmpInfo = {{5U,0U,0U,0U,0U},0U,NULL};
#if defined(CONFIG_ICAM_32BITS_OSD_USED)
        UINT32 Canvas_Color    = 0x80ffff00U;
        UINT32 SubCanvas_Color = 0x8000a0c0U;
#else
        UINT32 Canvas_Color    = 251U;
        UINT32 SubCanvas_Color = 232U;
#endif
        if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_LAYER_UPDATE) > 0U) {
            UINT32 Idx;

            for (Idx = 0U; Idx < SVC_DIAG_BOX_CAVS_NUM; Idx ++) {
                if ((pCavs->State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
                    PRetVal = SvcOsd_DrawRect(SvcDiagBoxCtrl.FormInfo.VoutIdx,
                                              pCavs->Canvas.StartX,
                                              pCavs->Canvas.StartY,
                                              pCavs->Canvas.StartX + pCavs->Canvas.Width,
                                              pCavs->Canvas.StartY + pCavs->Canvas.Height,
                                              Canvas_Color, 1); LOG_DIAG_BOX_ERR_HDLR

                    if (pCavs->SubCanvas.Width > 0U) {
                        PRetVal = SvcOsd_DrawRect(SvcDiagBoxCtrl.FormInfo.VoutIdx,
                                                  pCavs->SubCanvas.StartX,
                                                  pCavs->SubCanvas.StartY,
                                                  pCavs->SubCanvas.StartX + pCavs->SubCanvas.Width,
                                                  pCavs->SubCanvas.StartY + pCavs->SubCanvas.Height,
                                                  SubCanvas_Color, 1); LOG_DIAG_BOX_ERR_HDLR
                    }

                }

                pCavs ++;
            }

            BmpInfo.header.w = SVC_DIAG_BOX_OSD_PTN_PITCH;
            BmpInfo.header.h = SVC_DIAG_BOX_OSD_PTN_HEIGHT;
            BmpInfo.data_size = (SVC_DIAG_BOX_OSD_PTN_PITCH*SVC_DIAG_BOX_OSD_PTN_HEIGHT) << SVC_OSD_PIXEL_SIZE_SFT ;
            BmpInfo.data = (UINT8*)SvcDiagBoxFileBuf;
            PRetVal = SvcOsd_DrawBmp(SvcDiagBoxCtrl.FormInfo.VoutIdx, 0, 0, SVC_DIAG_BOX_OSD_PTN_PITCH, SVC_DIAG_BOX_OSD_PTN_HEIGHT, &BmpInfo); LOG_DIAG_BOX_ERR_HDLR
            AmbaMisra_TouchUnused(pCavs);
        }

        SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_LAYOUT].State &= ~SVC_DIAG_BOX_OSD_BUSY;
    }
}

static UINT32 SvcDiagBox_BtnSelHdlr(void)
{
    UINT32 Flag = 0U, PRetVal;

    if (((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_ENA_BTN_YES) > 0U) ||
        ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_ENA_BTN_NO) > 0U)) {

        if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_YES) > 0U) {
            Flag = SVC_DIAG_BOX_SEL_BTN_NO;
        } else if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_NO) > 0U) {
            Flag = SVC_DIAG_BOX_SEL_BTN_YES;
        } else {
            Flag = SVC_DIAG_BOX_SEL_BTN_NO;
        }

    } else {
        Flag = SVC_DIAG_BOX_SEL_BTN_CANCEL;
    }

    PRetVal = SvcDiagBox_ButtonSel(Flag); LOG_DIAG_BOX_ERR_HDLR

    return SVC_OK;
}

static UINT32 SvcDiagBox_BtnClickHdlr(void)
{
    UINT32 PRetVal;
    PRetVal = SvcDiagBox_ButtonClick(0); LOG_DIAG_BOX_ERR_HDLR
    return SVC_OK;
}

/**
 * Svc Diag Box pattern initialization by file function
 * @param [in] pOsdPtn Osd pattern
 * return 0-OK, 1-NG
 */
UINT32 SvcDiagBox_Initial(const char *pOsdPtn)
{
    UINT32 RetVal = SVC_OK;

    SvcDiagBox_ResBlkInit();

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(&SvcDiagBoxCtrl, 0, sizeof(SvcDiagBoxCtrl));
        SvcWrap_strcpy(SvcDiagBoxCtrl.Name, sizeof(SvcDiagBoxCtrl.Name), SVC_DIAG_BOX_NAME);
        if (pOsdPtn == NULL) {
            SvcWrap_strcpy(SvcDiagBoxCtrl.OsdResFile, sizeof(SvcDiagBoxCtrl.OsdResFile), SVC_DIAG_BOX_OSD_PTN_FILE);
        } else {
            SvcWrap_strcpy(SvcDiagBoxCtrl.OsdResFile, sizeof(SvcDiagBoxCtrl.OsdResFile), pOsdPtn);
        }

        // Create event flag
        AmbaSvcWrap_MisraMemset(&SvcDiagBoxCtrl.EventFlag, 0, sizeof(SvcDiagBoxCtrl.EventFlag));
        RetVal = AmbaKAL_EventFlagCreate(&(SvcDiagBoxCtrl.EventFlag), SvcDiagBoxCtrl.Name);
        if (0U != RetVal) {
            LOG_DIAG_BOX_NG("Fail to initial diag box - create event flag fail! ErrCode(0x%08X)", RetVal, 0U);
        } else {
            SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_INIT;
            LOG_DIAG_BOX_OK("Successful to initial diag box!", 0U, 0U);
        }
    }

    return RetVal;
}

/**
 * Svc Diag Box create function
 * @param [in] pBoxForm diagbox description
 * return 0-OK, 1-NG
 */
UINT32 SvcDiagBox_Create(SVC_DIAG_BOX_FORM_s *pBoxForm)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_API("Fail to create diag box - initial diag box module first!", 0U, 0U);
    } else if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) > 0U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_API("The diag box has been created!", 0U, 0U);
    } else if (SVC_OK != SvcDiagBox_ResBlkWaitDone(10000)) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to create diag box - wait osd load done timeout!", 0U, 0U);
    } else if (pBoxForm == NULL) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to create diag box - input diag box info should not null!", 0U, 0U);
        AmbaMisra_TouchUnused(pBoxForm);
    } else {
        SVC_DIAG_BOX_OSD_CANVAS_s *pCavs;
        UINT32 CanvsWidth, CanvsHeight, StrCanvsWidth, StrCanvsHeight;
        SVC_DIAG_BOX_BTN_CTRL_s *pBtnCtrl;

        // Configure the form info
        AmbaSvcWrap_MisraMemset(&SvcDiagBoxCtrl.FormInfo, 0, sizeof(SVC_DIAG_BOX_FORM_s));
        AmbaSvcWrap_MisraMemcpy(&SvcDiagBoxCtrl.FormInfo, pBoxForm, sizeof(SVC_DIAG_BOX_FORM_s));

        // Configure the canvas
        AmbaSvcWrap_MisraMemset(SvcDiagBoxCtrl.Canvas, 0, sizeof(SvcDiagBoxCtrl.Canvas));

        // - Configure the form
        pCavs = &SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_FORM];
        pCavs->State = SVC_DIAG_BOX_OSD_ENA;
        pCavs->Canvas.StartX = SvcDiagBoxCtrl.FormInfo.FormStartX;
        pCavs->Canvas.StartY = SvcDiagBoxCtrl.FormInfo.FormStartY;
        pCavs->Canvas.Width  = SvcDiagBoxCtrl.FormInfo.FormWidth;
        pCavs->Canvas.Height = SvcDiagBoxCtrl.FormInfo.FormHeight;
        pCavs->SubCanvas.Width  = pCavs->Canvas.Width * 85U / 100U;
        pCavs->SubCanvas.Height = pCavs->Canvas.Height * 85U / 100U;
        pCavs->SubCanvas.StartX = pCavs->Canvas.StartX + ( ( pCavs->Canvas.Width - pCavs->SubCanvas.Width ) / 2U );
        pCavs->SubCanvas.StartY = pCavs->Canvas.StartY + ( ( pCavs->Canvas.Height - pCavs->SubCanvas.Height ) / 2U );

        // - Configure the extra OSD setting
        if (pBoxForm->FormExtraOsdWidth > 0U) {
            SVC_DIAG_BOX_EXTRA_OSD_s *pExtraOsd = &(SvcDiagBoxCtrl.ExtraOsd);

            if (pBoxForm->FormExtraOsdWidth >= pBoxForm->FormExtraOsdHeight) {
                pExtraOsd->Canvas.Width  = pCavs->SubCanvas.Width;
                pExtraOsd->Canvas.Height = ( pCavs->SubCanvas.Width * pBoxForm->FormExtraOsdHeight ) / pBoxForm->FormExtraOsdWidth;
                pExtraOsd->Canvas.StartX = pCavs->SubCanvas.StartX + ( ( pCavs->SubCanvas.Width - pExtraOsd->Canvas.Width ) / 2U );
                pExtraOsd->Canvas.StartY = pCavs->SubCanvas.StartY + ( ( pCavs->SubCanvas.Height - pExtraOsd->Canvas.Height ) / 2U );
                pExtraOsd->State = SVC_DIAG_BOX_OSD_ENA;
            }
        }

        pCavs = &SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_BTN_YES];
        if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_ENA_BTN_YES) > 0U) {
            UINT32 ColGap = ( SvcDiagBoxCtrl.FormInfo.FormWidth * 2U ) / 100U;
            UINT32 RowGap = ( SvcDiagBoxCtrl.FormInfo.FormHeight * 2U ) / 100U;

            CanvsWidth     = ( ( SvcDiagBoxCtrl.FormInfo.FormWidth - ColGap ) / 2U );
            CanvsHeight    = ( ( SvcDiagBoxCtrl.FormInfo.FormHeight - RowGap ) / 3U );
            StrCanvsWidth  = ( CanvsWidth * 70U / 100U );
            StrCanvsHeight = ( CanvsHeight * 70U / 100U );

            pCavs->State = SVC_DIAG_BOX_OSD_ENA;
            pCavs->Canvas.StartX = SvcDiagBoxCtrl.FormInfo.FormStartX;
            pCavs->Canvas.StartY = SvcDiagBoxCtrl.FormInfo.FormStartY + SvcDiagBoxCtrl.FormInfo.FormHeight + RowGap;
            pCavs->Canvas.Width  = CanvsWidth;
            pCavs->Canvas.Height = CanvsHeight;
            pCavs->SubCanvas.Width  = StrCanvsWidth;
            pCavs->SubCanvas.Height = StrCanvsHeight;
            pCavs->SubCanvas.StartX = pCavs->Canvas.StartX + ( ( CanvsWidth - StrCanvsWidth ) / 2U );
            pCavs->SubCanvas.StartY = pCavs->Canvas.StartY + ( ( CanvsHeight - StrCanvsHeight ) / 2U );

            pCavs = &SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_BTN_NO];
            pCavs->State = SVC_DIAG_BOX_OSD_ENA;
            pCavs->Canvas.StartX = ( SvcDiagBoxCtrl.FormInfo.FormStartX + SvcDiagBoxCtrl.FormInfo.FormWidth - CanvsWidth );
            pCavs->Canvas.StartY = SvcDiagBoxCtrl.FormInfo.FormStartY + SvcDiagBoxCtrl.FormInfo.FormHeight + RowGap;
            pCavs->Canvas.Width  = CanvsWidth;
            pCavs->Canvas.Height = CanvsHeight;
            pCavs->SubCanvas.Width  = StrCanvsWidth;
            pCavs->SubCanvas.Height = StrCanvsHeight;
            pCavs->SubCanvas.StartX = pCavs->Canvas.StartX + ( ( CanvsWidth - StrCanvsWidth ) / 2U );
            pCavs->SubCanvas.StartY = pCavs->Canvas.StartY + ( ( CanvsHeight - StrCanvsHeight ) / 2U );
        }

        pCavs = &SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_BTN_CANCEL];
        if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_ENA_BTN_CANCEL) > 0U) {
            if ((SvcDiagBoxCtrl.FormInfo.ActFlag & (SVC_DIAG_BOX_ENA_BTN_YES | SVC_DIAG_BOX_ENA_BTN_NO)) > 0U) {
                LOG_DIAG_BOX_NG("Ignore cancel button!", 0U, 0U);
            } else {
                pCavs->State = SVC_DIAG_BOX_OSD_ENA;
                pCavs->Canvas.StartX = SvcDiagBoxCtrl.FormInfo.FormStartX;
                pCavs->Canvas.StartY = ( SvcDiagBoxCtrl.FormInfo.FormStartY + ( SvcDiagBoxCtrl.FormInfo.FormHeight * 105U / 100U ) );
                pCavs->Canvas.Width  = SvcDiagBoxCtrl.FormInfo.FormWidth;
                pCavs->Canvas.Height = ( SvcDiagBoxCtrl.FormInfo.FormHeight * 50U / 100U );

                pCavs->SubCanvas.Width  = pCavs->Canvas.Width * 75U / 100U;
                pCavs->SubCanvas.Height = pCavs->Canvas.Height * 75U / 100U;
                pCavs->SubCanvas.StartX = pCavs->Canvas.StartX + ( ( pCavs->Canvas.Width - pCavs->SubCanvas.Width ) / 2U );
                pCavs->SubCanvas.StartY = pCavs->Canvas.StartY + ( ( pCavs->Canvas.Height - pCavs->SubCanvas.Height ) / 2U );
            }
        }

        pCavs = &SvcDiagBoxCtrl.Canvas[SVC_DIAG_BOX_CAVS_LAYOUT];
        pCavs->State = SVC_DIAG_BOX_OSD_ENA;
        pCavs->Canvas.StartX = 0;
        pCavs->Canvas.StartY = 0;
        pCavs->Canvas.Width  = 960;
        pCavs->Canvas.Height = 480;

        // Configure selection button setting
        pBtnCtrl = &(SvcDiagBoxCtrl.BtnCtrl);
        SvcWrap_strcpy(pBtnCtrl->BtnName[SVC_DIAG_BOX_BTN_ID_SEL], 16U, "button_3");
        pBtnCtrl->List[SVC_DIAG_BOX_BTN_ID_SEL].ButtonName = pBtnCtrl->BtnName[SVC_DIAG_BOX_BTN_ID_SEL];
        pBtnCtrl->List[SVC_DIAG_BOX_BTN_ID_SEL].pFunc      = SvcDiagBox_BtnSelHdlr;
        SvcWrap_strcpy(pBtnCtrl->BtnName[SVC_DIAG_BOX_BTN_ID_CLK], 16U, "button_2");
        pBtnCtrl->List[SVC_DIAG_BOX_BTN_ID_CLK].ButtonName = pBtnCtrl->BtnName[SVC_DIAG_BOX_BTN_ID_CLK];
        pBtnCtrl->List[SVC_DIAG_BOX_BTN_ID_CLK].pFunc      = SvcDiagBox_BtnClickHdlr;
        pBtnCtrl->ID    = 0U;
        pBtnCtrl->Type  = SVC_BUTTON_TYPE_GPIO;
        pBtnCtrl->State = SVC_DIAG_BOX_BTN_INIT;

        SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_READY;
        SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_UPDATE;

        SvcDiagBoxCtrl.GuiLevel = pBoxForm->GuiLevel;

        LOG_DIAG_BOX_OK("Successful to create diag box!", 0U, 0U);
    }

    return RetVal;
}

/**
 * Svc Diag Box delete function
 * return 0-OK, 1-NG
 */
UINT32 SvcDiagBox_Delete(void)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) > 0U) {
        UINT32 Idx, TimeOut = 1000U, WaitMask, CurStateMask;
        SVC_DIAG_BOX_OSD_CANVAS_s *pCavs = SvcDiagBoxCtrl.Canvas;

        if ((SvcDiagBoxCtrl.BtnCtrl.State & SVC_DIAG_BOX_BTN_REG) > 0U) {
            PRetVal = SvcButtonCtrl_Release(&(SvcDiagBoxCtrl.BtnCtrl.ID)); LOG_DIAG_BOX_ERR_HDLR
            SvcDiagBoxCtrl.BtnCtrl.State = 0U;
        }

        /* Disable form osd */
        for (Idx = 0U; Idx < SVC_DIAG_BOX_CAVS_NUM; Idx ++) {
            if ((pCavs[Idx].State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
                pCavs[Idx].State &= ~(SVC_DIAG_BOX_OSD_UPD);
            }
        }

        /* Flush form */
        WaitMask = 0U;
        for (Idx = 0U; Idx < SVC_DIAG_BOX_CAVS_NUM; Idx ++) {
            if ((pCavs[Idx].State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
                pCavs[Idx].State |= SVC_DIAG_BOX_OSD_BUSY;
                WaitMask = (UINT32)(0x1UL << Idx);
            }
        }

        SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_UPDATE;

        /* Wait flush done */
        RetVal = SVC_NG;
        do {
            CurStateMask = 0U;
            for (Idx = 0U; Idx < SVC_DIAG_BOX_CAVS_NUM; Idx ++) {
                if ((pCavs[Idx].State & SVC_DIAG_BOX_OSD_BUSY) == 0U) {
                    CurStateMask = (UINT32)(0x1UL << Idx);
                }
            }
            if (CurStateMask == WaitMask) {
                RetVal = SVC_OK;
                break;
            } else {
                PRetVal = AmbaKAL_TaskSleep(10); LOG_DIAG_BOX_ERR_HDLR
                TimeOut -= 10U;
            }
        } while (TimeOut != 0U);

        /* Unregister canvas */
        if (RetVal == SVC_OK) {
            SvcGui_Unregister(SvcDiagBoxCtrl.FormInfo.VoutIdx, SvcDiagBoxCtrl.GuiLevel);
        } else {
            LOG_DIAG_BOX_NG("Fail to delete diag box - update canvas fail! 0x%x", CurStateMask, 0U);
        }

        if (RetVal == SVC_OK) {
            AmbaSvcWrap_MisraMemset(&(SvcDiagBoxCtrl.FormInfo), 0, sizeof(SVC_DIAG_BOX_FORM_s));
            AmbaSvcWrap_MisraMemset(SvcDiagBoxCtrl.Canvas, 0, sizeof(SvcDiagBoxCtrl.Canvas));

            SvcDiagBoxCtrlFlag &= ~(SVC_DIAG_BOX_READY);
            LOG_DIAG_BOX_DBG("Successful to delete diag box!", 0U, 0U);
        }
    }

    return RetVal;
}

/**
 * Svc Diag Box show function
 * @param [out] pRetBtnState diagbox state(yes, no or suspend)
 * @param [in] Timeout time out setting
 * @param [in] pExtraCB extra callback function(for user suspend)
 * return 0-OK, 1-NG
 */
UINT32 SvcDiagBox_Show(UINT32 *pRetBtnState, UINT32 Timeout, SVC_DIAG_BOX_CB_f pExtraCB)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to show diag box - initial diag box first!", 0U, 0U);
    } else if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) == 0U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to show diag box - create diag box first!", 0U, 0U);
    } else {
        UINT32 Idx;
        SVC_DIAG_BOX_OSD_CANVAS_s *pCavs = SvcDiagBoxCtrl.Canvas;

        // Register Canvas
        SvcGui_Register(SvcDiagBoxCtrl.FormInfo.VoutIdx, SvcDiagBoxCtrl.GuiLevel, "DiagBox", SvcDiagBox_GuiDraw, SvcDiagBox_GuiUpdate);

        // Register button
        if ((SvcDiagBoxCtrl.BtnCtrl.State & SVC_DIAG_BOX_BTN_INIT) > 0U) {
            RetVal = SvcButtonCtrl_Register(SvcDiagBoxCtrl.BtnCtrl.Type,
                                            (SvcDiagBoxCtrl.BtnCtrl.List),
                                            SVC_DIAG_BOX_BTN_ID_NUM,
                                            &(SvcDiagBoxCtrl.BtnCtrl.ID));
            if (RetVal != SVC_OK) {
                LOG_DIAG_BOX_NG("Fail to show diag box - register button fail!", 0U, 0U);
            } else {
                SvcDiagBoxCtrl.BtnCtrl.State |= SVC_DIAG_BOX_BTN_REG;
            }
        }

        // Update Canvas
        if (RetVal == SVC_OK) {
            for (Idx = 0U; Idx < SVC_DIAG_BOX_CAVS_NUM; Idx ++) {
                if ((pCavs[Idx].State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
                    pCavs[Idx].State |= SVC_DIAG_BOX_OSD_UPD;
                }
            }

            SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_SHOW;

            if (pRetBtnState != NULL) {
                UINT32 ActualFlag = 0U, IsSuspend = 0U;
                UINT32 WaitTimeout = Timeout, CurTimeout;

                PRetVal = AmbaKAL_EventFlagClear(&(SvcDiagBoxCtrl.EventFlag), SVC_DIAG_BOX_CTRL_BTN_CLICK); LOG_DIAG_BOX_ERR_HDLR

                do {

                    if (pExtraCB != NULL) {
                        PRetVal = pExtraCB(&IsSuspend); LOG_DIAG_BOX_ERR_HDLR
                    }

                    if (IsSuspend == 0U) {

                        ActualFlag = 0U;
                        if (WaitTimeout >= SVC_DIAG_BOX_TIMEOUT_INTERVAL) {
                            CurTimeout = SVC_DIAG_BOX_TIMEOUT_INTERVAL;
                            WaitTimeout -= SVC_DIAG_BOX_TIMEOUT_INTERVAL;
                        } else {
                            CurTimeout = WaitTimeout;
                            WaitTimeout = 0U;
                        }

                        PRetVal = AmbaKAL_EventFlagGet(&(SvcDiagBoxCtrl.EventFlag),
                                                       SVC_DIAG_BOX_CTRL_BTN_CLICK,
                                                       AMBA_KAL_FLAGS_ALL,
                                                       AMBA_KAL_FLAGS_CLEAR_NONE,
                                                       &ActualFlag, CurTimeout);
                        if (((PRetVal == 0U) && ((ActualFlag & SVC_DIAG_BOX_CTRL_BTN_CLICK) > 0U)) || (WaitTimeout == 0U)) {
                            break;
                        }
                    }

                } while (IsSuspend == 0U);

                if (IsSuspend == 1U) {
                    RetVal = SVC_NG;
                    LOG_DIAG_BOX_API("diagbox be suspended!", 0U, 0U);
                    *pRetBtnState = 0U;
                } else if ((ActualFlag & SVC_DIAG_BOX_CTRL_BTN_CLICK) == 0U) {
                    RetVal = SVC_NG;
                    LOG_DIAG_BOX_NG("Fail to show diag box - wait button click timeout!", 0U, 0U);
                    *pRetBtnState = 0U;
                } else {
                    *pRetBtnState = SvcDiagBoxCtrl.FormInfo.ActFlag;
                }
            }
        } else {
            SvcGui_Unregister(SvcDiagBoxCtrl.FormInfo.VoutIdx, SvcDiagBoxCtrl.GuiLevel);
        }
    }

    return RetVal;
}

/**
 * Svc Diag Box OSD data loading
 * @param [in] pBuf working buffer base
 * @param [in] BufSize working buffer size
 * return None
 */
void SvcDiagBox_OsdLoad(UINT8 *pWorkBuf, UINT32 WorkBufSize, UINT8 *pExtraData, UINT32 ExtraDataSize)
{
    if (pWorkBuf == NULL) {
        LOG_DIAG_BOX_NG("Fail to load diag box osd - input buffer should not null!", 0U, 0U);
    } else if (0U != SvcDiagBox_Initial(NULL)) {
        LOG_DIAG_BOX_NG("Fail to load diag box osd - initial diagbox fail!", 0U, 0U);
    } else {
        UINT32 RetVal = SvcDiagBox_ResBlkLoad(SvcDiagBoxCtrl.OsdResFile, pWorkBuf, WorkBufSize);
        if (RetVal != 0U) {
            LOG_DIAG_BOX_NG("Fail to load diag box osd! ErrCode(0x%08X)", RetVal, 0U);
        }

        AmbaSvcWrap_MisraMemset(SvcDiagBoxExtraOsdBuf, 0, sizeof(SvcDiagBoxExtraOsdBuf));
        if (pExtraData != NULL) {
            if (ExtraDataSize <= (UINT32)(sizeof(SvcDiagBoxExtraOsdBuf))) {
                AmbaSvcWrap_MisraMemcpy(SvcDiagBoxExtraOsdBuf, pExtraData, ExtraDataSize);
                SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_EXTRA_OSD;
            } else {
                LOG_DIAG_BOX_NG("Fail to load diag box osd - input extra data size(0x%X) > buffer size(0x%X)", ExtraDataSize, (UINT32)(sizeof(SvcDiagBoxExtraOsdBuf)));
            }
        }
    }

    AmbaMisra_TouchUnused(pExtraData);
    AmbaMisra_TouchUnused(pWorkBuf);
}

/**
 * Svc Diag Box button selection action function
 * @param [in] Flag current action flag
 * return 0-OK, 1-NG
 */
UINT32 SvcDiagBox_ButtonSel(UINT32 Flag)
{
    UINT32 RetVal = SVC_OK;

    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_INIT) == 0U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to show diag box - initial diag box first!", 0U, 0U);
    } else if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_READY) == 0U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to show diag box - create diag box first!", 0U, 0U);
    } else {
        if ((SvcDiagBoxCtrl.FormInfo.ActFlag & ( SVC_DIAG_BOX_CLI_BTN_YES
                                               | SVC_DIAG_BOX_CLI_BTN_NO
                                               | SVC_DIAG_BOX_CLI_BTN_CANCEL)) == 0U) {
            UINT32 NumCanvas = 0U, Idx;
            SVC_DIAG_BOX_OSD_CANVAS_s *pCavs = SvcDiagBoxCtrl.Canvas;

            if ((Flag & SVC_DIAG_BOX_SEL_BTN_YES) > 0U) {
                if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_ENA_BTN_YES) > 0U) {
                    SvcDiagBoxCtrl.FormInfo.ActFlag |= SVC_DIAG_BOX_SEL_BTN_YES;
                    SvcDiagBoxCtrl.FormInfo.ActFlag &= ~SVC_DIAG_BOX_SEL_BTN_NO;
                    NumCanvas = SVC_DIAG_BOX_CAVS_NUM;
                }
            } else {
                if ((Flag & SVC_DIAG_BOX_SEL_BTN_NO) > 0U) {
                    if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_ENA_BTN_NO) > 0U) {
                        SvcDiagBoxCtrl.FormInfo.ActFlag |= SVC_DIAG_BOX_SEL_BTN_NO;
                        SvcDiagBoxCtrl.FormInfo.ActFlag &= ~SVC_DIAG_BOX_SEL_BTN_YES;
                        NumCanvas = SVC_DIAG_BOX_CAVS_NUM;
                    }
                }
            }

            if ((Flag & SVC_DIAG_BOX_SEL_BTN_CANCEL) > 0U) {
                if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_ENA_BTN_CANCEL) > 0U) {
                    SvcDiagBoxCtrl.FormInfo.ActFlag |= SVC_DIAG_BOX_SEL_BTN_CANCEL;
                    SvcDiagBoxCtrl.FormInfo.ActFlag &= ~SVC_DIAG_BOX_SEL_BTN_YES;
                    SvcDiagBoxCtrl.FormInfo.ActFlag &= ~SVC_DIAG_BOX_SEL_BTN_NO;
                    NumCanvas = SVC_DIAG_BOX_CAVS_NUM;
                }
            }

            if ((Flag & SVC_DIAG_BOX_LAYER_UPDATE) > 0U) {
                if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_LAYER_UPDATE) > 0U) {
                    SvcDiagBoxCtrl.FormInfo.ActFlag &= ~SVC_DIAG_BOX_LAYER_UPDATE;
                } else {
                    SvcDiagBoxCtrl.FormInfo.ActFlag |= SVC_DIAG_BOX_LAYER_UPDATE;
                }
                NumCanvas = SVC_DIAG_BOX_CAVS_NUM;
            }

            for (Idx = 0U; Idx < NumCanvas; Idx ++) {
                if ((pCavs[Idx].State & SVC_DIAG_BOX_OSD_ENA) > 0U) {
                    pCavs[Idx].State |= SVC_DIAG_BOX_OSD_UPD;
                }
            }

            SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_UPDATE;
        }
    }

    return RetVal;
}

/**
 * Svc Diag Box button click action function
 * @param [in] Flag current action flag
 * return 0-OK, 1-NG
 */
UINT32 SvcDiagBox_ButtonClick(UINT32 Flag)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (Flag > 0U) {
        PRetVal = SvcDiagBox_ButtonSel(Flag); LOG_DIAG_BOX_ERR_HDLR
    }

    SvcDiagBoxCtrl.FormInfo.ActFlag &= ~(SVC_DIAG_BOX_CLI_BTN_YES | SVC_DIAG_BOX_CLI_BTN_NO | SVC_DIAG_BOX_CLI_BTN_CANCEL);
    if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_YES) > 0U) {
        SvcDiagBoxCtrl.FormInfo.ActFlag |= SVC_DIAG_BOX_CLI_BTN_YES;
    } else if ((SvcDiagBoxCtrl.FormInfo.ActFlag & SVC_DIAG_BOX_SEL_BTN_NO) > 0U) {
        SvcDiagBoxCtrl.FormInfo.ActFlag |= SVC_DIAG_BOX_CLI_BTN_NO;
    } else {
        SvcDiagBoxCtrl.FormInfo.ActFlag |= SVC_DIAG_BOX_CLI_BTN_CANCEL;
    }

    SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_UPDATE;

    PRetVal = AmbaKAL_EventFlagSet(&(SvcDiagBoxCtrl.EventFlag), SVC_DIAG_BOX_CTRL_BTN_CLICK); LOG_DIAG_BOX_ERR_HDLR

    return RetVal;
}

static UINT32 SvcDiagBox_ShellShow(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 2U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to proc diag box 'show' cmd - invalid arg count!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 DiagBoxOn = 0U;

        SvcDiagBox_ShellStrToU32(pArgVector[2U], &DiagBoxOn);
        if (DiagBoxOn == 0U) {
            PRetVal = SvcDiagBox_Delete(); LOG_DIAG_BOX_ERR_HDLR
        } else if (DiagBoxOn == 1U) {
            if (ArgCount < 9U) {
                RetVal = SVC_NG;
                LOG_DIAG_BOX_NG("Fail to proc diag box 'show' cmd - invalid arg count to 'on' diag box!", 0U, 0U);
            } else {
                UINT32 VoutIdx     = 0U;
                UINT32 FormX       = 0U;
                UINT32 FormY       = 0U;
                UINT32 FormWidth   = 0U;
                UINT32 FormHeight  = 0U;
                UINT32 ActFlag     = 0U;
                UINT32 GuiLevel    = 21U;
                SVC_DIAG_BOX_FORM_s DiagBoxForm;

                SvcDiagBox_ShellStrToU32(pArgVector[3U], &VoutIdx   );
                SvcDiagBox_ShellStrToU32(pArgVector[4U], &FormX     );
                SvcDiagBox_ShellStrToU32(pArgVector[5U], &FormY     );
                SvcDiagBox_ShellStrToU32(pArgVector[6U], &FormWidth );
                SvcDiagBox_ShellStrToU32(pArgVector[7U], &FormHeight);
                SvcDiagBox_ShellStrToU32(pArgVector[9U], &ActFlag   );

                AmbaSvcWrap_MisraMemset(&DiagBoxForm, 0, sizeof(DiagBoxForm));
                DiagBoxForm.VoutIdx    = VoutIdx;
                DiagBoxForm.ActFlag    = ActFlag;
                DiagBoxForm.FormStartX = FormX;
                DiagBoxForm.FormStartY = FormY;
                DiagBoxForm.FormWidth  = FormWidth;
                DiagBoxForm.FormHeight = FormHeight;
                SvcWrap_strcpy(DiagBoxForm.FormContent, sizeof(DiagBoxForm.FormContent), pArgVector[8U]);

                if (ArgCount == 11U) {
                    SvcDiagBox_ShellStrToU32(pArgVector[10U], &GuiLevel);
                }

                if (ArgCount >= 12U) {
                    UINT32 ExtraWidth  = 0U;
                    UINT32 ExtraHeight = 0U;

                    SvcDiagBox_ShellStrToU32(pArgVector[10U], &ExtraWidth );
                    SvcDiagBox_ShellStrToU32(pArgVector[11U], &ExtraHeight);

                    DiagBoxForm.FormExtraOsdWidth  = ExtraWidth;
                    DiagBoxForm.FormExtraOsdHeight = ExtraHeight;
                    SvcWrap_strcpy(DiagBoxForm.FormExtraOsdPtn, sizeof(DiagBoxForm.FormExtraOsdPtn), pArgVector[12U]);

                    if (ArgCount >= 13U) {
                        SvcDiagBox_ShellStrToU32(pArgVector[13U], &GuiLevel);
                    }
                }

                DiagBoxForm.GuiLevel = GuiLevel;

                if (SVC_OK != SvcDiagBox_Create(&DiagBoxForm)) {
                    LOG_DIAG_BOX_NG("Fail to proc diag box 'show' cmd - create diag box fail!", 0U, 0U);
                } else {
                    PRetVal = SvcDiagBox_Show(NULL, AMBA_KAL_NO_WAIT, NULL); LOG_DIAG_BOX_ERR_HDLR
                }
            }
        } else {
            RetVal = SVC_NG;
            LOG_DIAG_BOX_NG("Fail to proc diag box 'show' cmd - only support on(1) and off(0)!", 0U, 0U);
        }
    }

    return RetVal;
}

static void SvcDiagBox_ShellShowU(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 PRetVal;

    AmbaSvcWrap_MisraMemset(SvcDiagBoxShellStr, 0, sizeof(SvcDiagBoxShellStr));
    SVC_WRAP_SNPRINT "          %sshow%s                  : create raw encode module\n"
        SVC_SNPRN_ARG_S    SvcDiagBoxShellStr
        SVC_SNPRN_ARG_CSTR SVC_LOG_DIAG_HL_TITLE_1 SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_CSTR SVC_LOG_DIAG_HL_END     SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_BSIZE (UINT32)sizeof(SvcDiagBoxShellStr)
        SVC_SNPRN_ARG_RLEN  &PRetVal
        SVC_SNPRN_ARG_E
    PrintFunc(SvcDiagBoxShellStr);
    PrintFunc("          ------------------------------------------------------\n");
    PrintFunc("            [On/Off]         : 1: On, 0: Off\n");
    PrintFunc("            [VoutIdx]        : 0: LCD, 1: TV\n");
    PrintFunc("            [Form_X]         : Message Box Form X\n");
    PrintFunc("            [Form_Y]         : Message Box Form Y\n");
    PrintFunc("            [Form_Width]     : Message Box Form Width\n");
    PrintFunc("            [Form_Height]    : Message Box Form Height\n");
    PrintFunc("            [String]         : Message Box String\n");
    PrintFunc("            [Flag]           : Message Box Actual Flag\n");
    PrintFunc("                             : 1  = Enable YES button\n");
    PrintFunc("                             : 2  = Enable NO button\n");
    PrintFunc("                             : 4  = Enable CANCEL button\n");
    PrintFunc("                             : 16 = Highlight YES button\n");
    PrintFunc("                             : 32 = Highlight NO button\n");
    PrintFunc("                             : 64 = Highlight CANCEL button\n");
    PrintFunc("\n");
}

static UINT32 SvcDiagBox_ShellBtn(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    if (ArgCount < 2U) {
        RetVal = SVC_NG;
        LOG_DIAG_BOX_NG("Fail to proc diag box 'btn' cmd - invalid arg count!", 0U, 0U);
    }

    if ((RetVal == SVC_OK) && (pArgVector != NULL)) {
        UINT32 Flag = 0U;
        if (0 == SvcWrap_strcmp(pArgVector[2U], "yes")) {
            Flag = SVC_DIAG_BOX_SEL_BTN_YES;
            PRetVal = SvcDiagBox_ButtonSel(Flag); LOG_DIAG_BOX_ERR_HDLR
        } else if (0 == SvcWrap_strcmp(pArgVector[2U], "no")) {
            Flag = SVC_DIAG_BOX_SEL_BTN_NO;
            PRetVal = SvcDiagBox_ButtonSel(Flag); LOG_DIAG_BOX_ERR_HDLR
        } else if (0 == SvcWrap_strcmp(pArgVector[2U], "cancel")) {
            Flag = SVC_DIAG_BOX_SEL_BTN_CANCEL;
            PRetVal = SvcDiagBox_ButtonSel(Flag); LOG_DIAG_BOX_ERR_HDLR
        } else if (0 == SvcWrap_strcmp(pArgVector[2U], "click")) {
            PRetVal = SvcDiagBox_ButtonClick(Flag); LOG_DIAG_BOX_ERR_HDLR
        } else {
            RetVal = SVC_NG;
            LOG_DIAG_BOX_NG("Fail to proc diag box 'btn' cmd - invalid type!", 0U, 0U);
        }
    }

    return RetVal;
}

static void SvcDiagBox_ShellBtnU(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 PRetVal;

    AmbaSvcWrap_MisraMemset(SvcDiagBoxShellStr, 0, sizeof(SvcDiagBoxShellStr));
    SVC_WRAP_SNPRINT "          %sbtn%s                   : message box button\n"
        SVC_SNPRN_ARG_S    SvcDiagBoxShellStr
        SVC_SNPRN_ARG_CSTR SVC_LOG_DIAG_HL_TITLE_1 SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_CSTR SVC_LOG_DIAG_HL_END     SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_BSIZE (UINT32)sizeof(SvcDiagBoxShellStr)
        SVC_SNPRN_ARG_RLEN  &PRetVal
        SVC_SNPRN_ARG_E
    PrintFunc(SvcDiagBoxShellStr);
    PrintFunc("          ------------------------------------------------------\n");
    PrintFunc("            [yes]            : Select YES button\n");
    PrintFunc("            [no]             : Select NO button\n");
    PrintFunc("            [cancel]         : Select CANCEL button\n");
    PrintFunc("            [click]          : Click by selected button\n");
    PrintFunc("\n");
}

static UINT32 SvcDiagBox_ShellDbg(UINT32 ArgCount, char * const *pArgVector)
{
    UINT32 PRetVal;

    if ((ArgCount > 0U) && (pArgVector != NULL)) {
        PRetVal = SvcDiagBox_ButtonSel(SVC_DIAG_BOX_LAYER_UPDATE); LOG_DIAG_BOX_ERR_HDLR
    }

    return SVC_OK;
}

static void SvcDiagBox_ShellDbgU(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 PRetVal;

    AmbaSvcWrap_MisraMemset(SvcDiagBoxShellStr, 0, sizeof(SvcDiagBoxShellStr));
    SVC_WRAP_SNPRINT "          %sdbg%s                : enable message box debug layout\n"
        SVC_SNPRN_ARG_S    SvcDiagBoxShellStr
        SVC_SNPRN_ARG_CSTR SVC_LOG_DIAG_HL_TITLE_1 SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_CSTR SVC_LOG_DIAG_HL_END     SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_BSIZE (UINT32)sizeof(SvcDiagBoxShellStr)
        SVC_SNPRN_ARG_RLEN  &PRetVal
        SVC_SNPRN_ARG_E
    PrintFunc(SvcDiagBoxShellStr);
    PrintFunc("\n");
}

static void SvcDiagBox_ShellStrToU32(const char *pString, UINT32 *pValue)
{
    UINT32 ErrCode = AmbaUtility_StringToUInt32(pString, pValue);
    if (ErrCode != 0U) {
        AmbaMisra_TouchUnused(&ErrCode);
    }
}

static void SvcDiagBox_ShellEntryInit(void)
{
    if ((SvcDiagBoxCtrlFlag & SVC_DIAG_BOX_SHELL_INIT) == 0U) {

        AmbaSvcWrap_MisraMemset(SvcDiagBoxShellFunc, 0, sizeof(SvcDiagBoxShellFunc));
        SvcDiagBoxShellFunc[0U] = (SVC_DIAG_BOX_SHELL_FUNC_s) { "show",  SvcDiagBox_ShellShow,  SvcDiagBox_ShellShowU };
        SvcDiagBoxShellFunc[1U] = (SVC_DIAG_BOX_SHELL_FUNC_s) { "btn",   SvcDiagBox_ShellBtn,   SvcDiagBox_ShellBtnU  };
        SvcDiagBoxShellFunc[2U] = (SVC_DIAG_BOX_SHELL_FUNC_s) { "dbg",   SvcDiagBox_ShellDbg,   SvcDiagBox_ShellDbgU  };

        SvcDiagBoxCtrlFlag |= SVC_DIAG_BOX_SHELL_INIT;
    }
}

/**
 * Svc Diag Box shell command entry function
 * @param [in] ArgCount input arguments counter
 * @param [in] pArgVector input arguments data
 * @param [in] PrintFunc print function entry
 * return None
 */
void SvcDiagBox_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = SVC_OK, PRetVal;

    SvcDiagBox_ShellEntryInit();

    if(ArgCount == 0xCafeU) {
        UINT32 SIdx;

        LOG_DIAG_BOX_API("", 0U, 0U);
        SVC_WRAP_PRINT "==== %sShell Arguments Setting%s (%s%d%s) ===="
            SVC_PRN_ARG_S SVC_LOG_DIAGBOX
            SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_TITLE_1 SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_END     SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_NUM     SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 ArgCount                SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_END     SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        for (SIdx = 0U; SIdx < ArgCount; SIdx ++) {
            SVC_WRAP_PRINT "  pArgVector[%s%d%s] : %s%s%s"
               SVC_PRN_ARG_S SVC_LOG_DIAGBOX
               SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_NUM SVC_PRN_ARG_POST
               SVC_PRN_ARG_UINT32 SIdx                SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_END SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_STR SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   pArgVector[SIdx]    SVC_PRN_ARG_POST
               SVC_PRN_ARG_CSTR   SVC_LOG_DIAG_HL_END SVC_PRN_ARG_POST
               SVC_PRN_ARG_E
        }
    }

    if (ArgCount < 1U) {
        RetVal = SVC_NG;
    } else if (pArgVector == NULL) {
        RetVal = SVC_NG;
    } else {
        UINT32 ShellIdx;
        UINT32 ShellCount = (UINT32)(sizeof(SvcDiagBoxShellFunc)) / (UINT32)(sizeof(SvcDiagBoxShellFunc[0]));

        RetVal = SVC_NG;

        for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
            if (SvcDiagBoxShellFunc[ShellIdx].pFunc != NULL) {
                if (0 == SvcWrap_strcmp(pArgVector[1U], SvcDiagBoxShellFunc[ShellIdx].CmdName)) {
                    PRetVal = (SvcDiagBoxShellFunc[ShellIdx].pFunc)(ArgCount, pArgVector); LOG_DIAG_BOX_ERR_HDLR
                    RetVal = SVC_OK;
                    break;
                }
            }
        }
    }

    if (RetVal != SVC_OK) {
        SvcDiagBox_CmdUsage(PrintFunc);
    }
}

static void SvcDiagBox_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ShellIdx, RetLen;
    UINT32 ShellCount = (UINT32)(sizeof(SvcDiagBoxShellFunc)) / (UINT32)(sizeof(SvcDiagBoxShellFunc[0]));

    PrintFunc("        diagbox\n");

    for (ShellIdx = 0U; ShellIdx < ShellCount; ShellIdx ++) {
        if (SvcDiagBoxShellFunc[ShellIdx].pFunc != NULL) {
            if (SvcDiagBoxShellFunc[ShellIdx].pUsage == NULL) {

                AmbaSvcWrap_MisraMemset(SvcDiagBoxShellStr, 0, sizeof(SvcDiagBoxShellStr));
                SVC_WRAP_SNPRINT "          %s\n"
                    SVC_SNPRN_ARG_S     SvcDiagBoxShellStr
                    SVC_SNPRN_ARG_CSTR  SvcDiagBoxShellFunc[ShellIdx].CmdName SVC_SNPRN_ARG_POST
                    SVC_SNPRN_ARG_BSIZE (UINT32)sizeof(SvcDiagBoxShellStr)
                    SVC_SNPRN_ARG_RLEN  &RetLen
                    SVC_SNPRN_ARG_E
                PrintFunc(SvcDiagBoxShellStr);

            } else {
                (SvcDiagBoxShellFunc[ShellIdx].pUsage)(PrintFunc);
            }
        }
    }

    PrintFunc("\n");
}
