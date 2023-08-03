/**
 *  @file SvcGuiTextView.c
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
 *  @details svc application draw gui CheckBox
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

#include "SvcWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"

#include "SvcGui.h"
#include "SvcOsd.h"

#include "SvcGuiTextView.h"

#define SVC_LOG_GUI_TXTVIEW "TXT_VIEW"
#define DEFAULT_PADDING     (5U)

/**
 * Svc Gui Text View draw function
 * @param [in] VoutIdx Vout index
 * @param [in] pCanvas canvas description
 * @param [in] pTextView text view description
 * return None
 */
void SvcGuiTextView_Draw(UINT32 VoutIdx, SVC_GUI_CANVAS_s *pCanvas, SVC_GUI_TEXTVIEW_s *pTextView)
{
    UINT32 i;
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(pCanvas);
    AmbaMisra_TouchUnused(pTextView);

    if ((pTextView != NULL) && (pCanvas != NULL)) {
        UINT32 FontSize;
        UINT32 TextViewStartX, TextViewStartY, TextViewEndX, TextViewEndY;
        UINT32 TextViewPaddingV = DEFAULT_PADDING, TextViewPaddingH = DEFAULT_PADDING;  /* Padding Vertical and Horizontal */
        UINT32 StrLen, StrIdx, LeftStrLen;
        UINT32 CharWidth, CharHeight, GapWidth, GapHeight, StrHeight;
        UINT32 OneLineStrLen, StrTotalLine;
        char   StrLine[512];

        TextViewStartX = pCanvas->StartX;
        TextViewStartY = pCanvas->StartY;
        TextViewEndX   = pCanvas->StartX + pCanvas->Width;
        TextViewEndY   = pCanvas->StartY + pCanvas->Height;

        RetVal = SvcOsd_DrawSolidRect(VoutIdx, TextViewStartX, TextViewStartY, TextViewEndX, TextViewEndY, pTextView->BackgroundColor);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_GUI_TXTVIEW, "SvcGuiTextView_Draw() err, SvcOsd_DrawSolidRect failed", 0U, 0U);
        }

        StrIdx = 0U;
        StrLen = (UINT32) SvcWrap_strlen(pTextView->pText);
        FontSize   = pTextView->FontSize;
        GapWidth   = FontSize * SVC_OSD_CHAR_GAP;
        GapHeight  = pTextView->LineGap;
        CharWidth  = FontSize * SVC_OSD_CHAR_WIDTH;
        CharHeight = FontSize * SVC_OSD_CHAR_HEIGHT;
        StrHeight  = CharHeight + GapHeight;

        /* Calculate how many character can be inside the line */
        OneLineStrLen = (pCanvas->Width - (TextViewPaddingH * 2U)) / (CharWidth + GapWidth);

        /* Calculate how may line will be */
        StrTotalLine = (StrLen + OneLineStrLen - 1U) / OneLineStrLen;
        TextViewPaddingV = (pCanvas->Height - (StrTotalLine * StrHeight)) / 2U;

        for (i = 0U; i < StrTotalLine; i++) {
            UINT32 Xstart, Ystart;
            const char *pTargetStr = &pTextView->pText[StrIdx];
            LeftStrLen = StrLen - StrIdx;

            RetVal = AmbaWrap_memset(StrLine, 0, sizeof(char) * 512U);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_GUI_TXTVIEW, "SvcGuiTextView_Draw() err, AmbaWrap_memset failed, StrLine", 0U, 0U);
            }

            if (LeftStrLen >= OneLineStrLen) {
                RetVal = AmbaWrap_memcpy(StrLine, pTargetStr, sizeof(char) * OneLineStrLen);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI_TXTVIEW, "SvcGuiTextView_Draw() err, AmbaWrap_memcpy failed, OneLineStrLen", 0U, 0U);
                }
                StrIdx = StrIdx + OneLineStrLen;
            } else {
                TextViewPaddingH = (pCanvas->Width - (LeftStrLen * (CharWidth + GapWidth))) / 2U;
                RetVal = AmbaWrap_memcpy(StrLine, pTargetStr, sizeof(char) * LeftStrLen);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI_TXTVIEW, "SvcGuiTextView_Draw() err, AmbaWrap_memcpy failed, StrLen", 0U, 0U);
                }
                StrIdx = StrLen;
            }

            Xstart = pCanvas->StartX + TextViewPaddingH;
            Ystart = pCanvas->StartY + TextViewPaddingV + (i * StrHeight);

            RetVal = SvcOsd_DrawString(VoutIdx, Xstart, Ystart, pTextView->FontSize, pTextView->FontColor, StrLine);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_GUI_TXTVIEW, "SvcGuiCheckBox_Draw() err, SvcOsd_DrawString failed, Draw list content", 0U, 0U);
            }
        }

        AmbaMisra_TouchUnused(&RetVal);
    }
}
