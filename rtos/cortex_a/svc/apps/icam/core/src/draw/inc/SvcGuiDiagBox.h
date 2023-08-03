/**
*  @file SvcGuiDiagBox.h
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
*  @details svc gui message box
*
*/

#ifndef SVC_GUI_DIAG_BOX_H
#define SVC_GUI_DIAG_BOX_H

typedef UINT32 (*SVC_DIAG_BOX_CB_f)(UINT32 *pIsSuspend);

typedef struct {
    UINT32                      VoutIdx;
    UINT32                      GuiLevel;
    UINT32                      ActFlag;
#define SVC_DIAG_BOX_ENA_BTN_YES    (0x1U)
#define SVC_DIAG_BOX_ENA_BTN_NO     (0x2U)
#define SVC_DIAG_BOX_ENA_BTN_CANCEL (0x4U)
#define SVC_DIAG_BOX_SEL_BTN_YES    (0x10U)
#define SVC_DIAG_BOX_SEL_BTN_NO     (0x20U)
#define SVC_DIAG_BOX_SEL_BTN_CANCEL (0x40U)
#define SVC_DIAG_BOX_CLI_BTN_YES    (0x100U)
#define SVC_DIAG_BOX_CLI_BTN_NO     (0x200U)
#define SVC_DIAG_BOX_CLI_BTN_CANCEL (0x400U)
#define SVC_DIAG_BOX_LAYER_UPDATE   (0x100000U)
    UINT32                      FormStartX;
    UINT32                      FormStartY;
    UINT32                      FormWidth;
    UINT32                      FormHeight;
    char                        FormContent[128];
    char                        FormExtraOsdPtn[32];
    UINT32                      FormExtraOsdWidth;
    UINT32                      FormExtraOsdHeight;
    void                       *pFormExtraOsdBuf;
} SVC_DIAG_BOX_FORM_s;

UINT32 SvcDiagBox_Initial(const char *pOsdPtn);
UINT32 SvcDiagBox_Create(SVC_DIAG_BOX_FORM_s *pBoxForm);
UINT32 SvcDiagBox_Delete(void);
UINT32 SvcDiagBox_Show(UINT32 *pRetBtnState, UINT32 Timeout, SVC_DIAG_BOX_CB_f pExtraCB);
void   SvcDiagBox_OsdLoad(UINT8 *pWorkBuf, UINT32 WorkBufSize, UINT8 *pExtraData, UINT32 ExtraDataSize);
UINT32 SvcDiagBox_ButtonSel(UINT32 Flag);
UINT32 SvcDiagBox_ButtonClick(UINT32 Flag);

void SvcDiagBox_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

#endif  /* SVC_MSG_BOX_H */
