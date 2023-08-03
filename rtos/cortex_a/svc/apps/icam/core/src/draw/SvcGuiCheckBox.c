/**
 *  @file SvcGuiCheckBox.c
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

/* svc-framework */
#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcLog.h"

/* apps-shared */
#include "SvcGui.h"
#include "SvcOsd.h"

#include "SvcGuiCheckBox.h"

#define SVC_LOG_GUI_CHECKBOX "CHK_BOX"

/**
 * Svc Gui draw check box function
 * @param [in] VoutIdx Vout index
 * @param [in] pCanvas canvas description
 * @param [in] pCheckBox check box description
 * return None
 */
void SvcGuiCheckBox_Draw(UINT32 VoutIdx, SVC_GUI_CANVAS_s *pCanvas, SVC_GUI_CHECKBOX_s *pCheckBox)
{
    UINT32 i;
    UINT32 RetVal = SVC_OK;
    char   StrLine[512U];

    AmbaMisra_TouchUnused(pCanvas);
    AmbaMisra_TouchUnused(pCheckBox);

    if ((pCheckBox != NULL) && (pCanvas != NULL)) {
        UINT32 CheckBoxShowCase = 0U;
        UINT32 CheckBoxStartX, CheckBoxStartY, CheckBoxEndX, CheckBoxEndY;
        UINT32 RowWidth, RowHeight, RowGap;
        /* StartIdx is the LIST INDEX to be shown in current CheckBox */
        UINT32 ListStartIdx;
        if (pCheckBox->CurIdx <= (pCheckBox->RowNum / 2U)) {
            /* case - 1 */
            ListStartIdx = 0U;
            CheckBoxShowCase = 1U;
        } else if (pCheckBox->CurIdx >= (pCheckBox->ListNum - (pCheckBox->RowNum / 2U))) {
            /* case - 2 */
            ListStartIdx = pCheckBox->ListNum - pCheckBox->RowNum;
            CheckBoxShowCase = 2U;
        } else {
            /* case - 3 */
            ListStartIdx = pCheckBox->CurIdx - (pCheckBox->RowNum / 2U);
            CheckBoxShowCase = 3U;
        }

        CheckBoxStartX = pCanvas->StartX;
        CheckBoxStartY = pCanvas->StartY;
        CheckBoxEndX   = pCanvas->StartX + pCanvas->Width;
        CheckBoxEndY   = pCanvas->StartY + pCanvas->Height;

        RowWidth  = pCanvas->Width;
        RowHeight = SVC_OSD_CHAR_HEIGHT * pCheckBox->FontSize;
        RowGap    = pCheckBox->RowGap;

        RetVal = SvcOsd_DrawSolidRect(VoutIdx, CheckBoxStartX, CheckBoxStartY, CheckBoxEndX, CheckBoxEndY, pCheckBox->BackgroundColor);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, SvcOsd_DrawSolidRect failed", 0U, 0U);
        }

        /* Draw each row */
        for (i = 0U; i < pCheckBox->RowNum; i++) {
            UINT32 OneLineStrLen, StrLen;
            UINT32 CharWidth, GapWidth;
            UINT32 Xstart, Ystart, Xend, Yend;
            UINT32 FontXstart;

            Xstart = pCanvas->StartX;
            Ystart = pCanvas->StartY + (i * (RowHeight + RowGap));
            Xend   = pCanvas->StartX + pCanvas->Width;
            Yend   = Ystart + RowHeight;

            FontXstart = Xstart + 5U + 3U;   /* Checked bar is 5 pixel width */

            /* Draw target list background */
            if (CheckBoxShowCase == 1U) {
                /* case - 1 */
                if (pCheckBox->CurIdx == i) {
                    RetVal = SvcOsd_DrawSolidRect(VoutIdx, Xstart, Ystart, Xend, Yend, pCheckBox->SelectedBoxColor);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, SvcOsd_DrawSolidRect failed, case - 1", 0U, 0U);
                    }
                }
            } else if (CheckBoxShowCase == 2U) {
                /* case - 2 */
                if (pCheckBox->CurIdx == (i + ListStartIdx)) {
                    RetVal = SvcOsd_DrawSolidRect(VoutIdx, Xstart, Ystart, Xend, Yend, pCheckBox->SelectedBoxColor);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, SvcOsd_DrawSolidRect failed, case - 2", 0U, 0U);
                    }
                }
            } else {
                /* case - 3 */
                if (pCheckBox->CurIdx == (i + ListStartIdx)) {
                    RetVal = SvcOsd_DrawSolidRect(VoutIdx, Xstart, Ystart, Xend, Yend, pCheckBox->SelectedBoxColor);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, SvcOsd_DrawSolidRect failed, case - 3", 0U, 0U);
                    }
                }
            }

            /* Draw check bar */
            if ((pCheckBox->CheckBits & ((UINT32) 1U << (ListStartIdx + i))) > 0U) {
                RetVal = SvcOsd_DrawSolidRect(VoutIdx, Xstart, Ystart, Xstart + 5U, Yend, pCheckBox->CheckBarColor);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, SvcOsd_DrawSolidRect failed, check bar", 0U, 0U);
                }
            }

            /* Caluclate how may character can be shown */
            
            GapWidth   = pCheckBox->FontSize * SVC_OSD_CHAR_GAP;
            CharWidth  = pCheckBox->FontSize * SVC_OSD_CHAR_WIDTH;
            OneLineStrLen = (RowWidth - (5U /* Check bar */)) / (CharWidth + GapWidth);
            StrLen = (UINT32)SvcWrap_strlen(pCheckBox->pList[ListStartIdx + i]);

            RetVal = AmbaWrap_memset(StrLine, 0, sizeof(char) * 512U);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, AmbaWrap_memset failed, StrLine", 0U, 0U);
            }

            if (StrLen > 0U) {
                if (StrLen > OneLineStrLen) {
                    RetVal = AmbaWrap_memcpy(StrLine, pCheckBox->pList[ListStartIdx + i], sizeof(char) * OneLineStrLen);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, AmbaWrap_memcpy failed, OneLineStrLen", 0U, 0U);
                    }
                } else {
                    RetVal = AmbaWrap_memcpy(StrLine, pCheckBox->pList[ListStartIdx + i], sizeof(char) * StrLen);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, AmbaWrap_memcpy failed, StrLen", 0U, 0U);
                    }
                }

                /* Draw list content */
                RetVal = SvcOsd_DrawString(VoutIdx, FontXstart, Ystart, pCheckBox->FontSize, pCheckBox->FontColor, StrLine);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_GUI_CHECKBOX, "SvcGuiCheckBox_Draw() err, SvcOsd_DrawString failed, Draw list content", 0U, 0U);
                }
            }

            AmbaMisra_TouchUnused(&RetVal);
        }
    }
}
