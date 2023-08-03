/**
 *  @file Buttons.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of Button control
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaADC.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "buttons.h"

/***************************************************************
* ADC button releated functions
***************************************************************/
typedef struct {
    INT32   ButtonId;   /**< Button ID */
    UINT32  LowVolt;    /**< (Low range)  voltage */
    UINT32  HighVolt;   /**< (High range) voltage */
} ADC_BUTTON_s;

#define CHAN_NORM_POS       (0x4afecafe)
#define DEBOUNCE_COUNT      (2)
#define ADC_BT_CH_NUM       (1U)

static void AppButtonOp_UpdateStatus(UINT32 buttonId, UINT32 event)
{
    if ((event == UI_BUTTON_DOWN_EVENT) && (buttonId == (UINT32)RECORD_BUTTON)) {
#define REF_EVENT_BUTTON                   (8U)
#define REF_EVENT_INFO_BUTTON_RECORD       (5U)
        extern void AmbaRefCommon_Event_MsgSend(UINT32 EventID, UINT64 Info0);
        AmbaRefCommon_Event_MsgSend(REF_EVENT_BUTTON, (UINT64)REF_EVENT_INFO_BUTTON_RECORD);
    }
}

/**
 *  @brief Handler for for watching ADC voltage level changes.
 *
 *  Handler for for watching ADC voltage level changes.
 *
 *  @param [in] TimerId Timer id
 *
 */
static void* RefButton_AdcScanHandler(void *EntryArg)
{
    static ADC_BUTTON_s GAdcButton0[] = {
        { RECORD_BUTTON, 0x0U, 0x20U},
        { CHAN_NORM_POS, 0x0U, 0x0U }
    };
    static ADC_BUTTON_s *GAdcButtons[ADC_BT_CH_NUM] = {
        GAdcButton0,
    };

    static INT32 DebounceCounts[ADC_BT_CH_NUM] = {0};
    static INT32 DebounceKeys[ADC_BT_CH_NUM] = {-1};
    static INT32 LastChanBIds[ADC_BT_CH_NUM] = {-1};
    UINT32 i, j, DoWhile = 1U;
    UINT32 AdcValues[ADC_BT_CH_NUM] = {0};
    INT32 CurrentButtonIds[ADC_BT_CH_NUM] = {-1};

    AmbaMisra_TouchUnused(&EntryArg);
    while (DoWhile == 1U) {
        (void)AmbaADC_SingleRead(AMBA_ADC_CHANNEL0, &AdcValues[0]);
        for (i = 0U; i<ADC_BT_CH_NUM; i++) {
            for (j = 0U; GAdcButtons[i][j].ButtonId >= 0; j++) {
                if ((AdcValues[i] >= GAdcButtons[i][j].LowVolt) && (AdcValues[i] <= GAdcButtons[i][j].HighVolt)) {
                    CurrentButtonIds[i] = GAdcButtons[i][j].ButtonId;
                    break;
                }
            }
            if (GAdcButtons[i][j].ButtonId == CHAN_NORM_POS) {
                CurrentButtonIds[i] = CHAN_NORM_POS;
            }
            if (CurrentButtonIds[i] == -1) {
                continue;
            }
            if (DebounceKeys[i] != CurrentButtonIds[i]) {
                DebounceKeys[i] = CurrentButtonIds[i];
                DebounceCounts[i] = 0;
                continue;
            } else {
                DebounceCounts[i]++;
            }
            if (DebounceCounts[i] == DEBOUNCE_COUNT) {
                if (LastChanBIds[i] != -1) {
                    AppButtonOp_UpdateStatus((UINT32)LastChanBIds[i], UI_BUTTON_UP_EVENT);
                    LastChanBIds[i] = -1;
                }
                if (DebounceKeys[i] != CHAN_NORM_POS) {
                    LastChanBIds[i] = DebounceKeys[i];
                    AppButtonOp_UpdateStatus((UINT32)LastChanBIds[i], UI_BUTTON_DOWN_EVENT);
                }
            }
        }
        (void)AmbaKAL_TaskSleep(100U);
    }

    return NULL;
}

/**
 *  @brief Initialize the H1 adc button driver.
 *
 *  Initialize the H1 adc button driver.
 *
 */
static void H1RefButton_AdcInit(void)
{
    static AMBA_KAL_TASK_t ButtonTask = {0};
#define STACK_SIZE (8192U)
    static UINT8 TaskStack[STACK_SIZE];
    UINT32 ReturnValue;
    char ButtonTaskName[] = "Adc Button Task";

    ReturnValue = AmbaKAL_TaskCreate(&ButtonTask, ButtonTaskName, 159U,
                                     RefButton_AdcScanHandler, NULL, TaskStack, STACK_SIZE, 1U);

    if (ReturnValue != OK) {
        AmbaPrint_PrintUInt5("[BSP-button] Register timer fail.", 0U, 0U, 0U, 0U, 0U);
    }
}

/**
 *  @brief Initialize the H1 buttons driver.
 *
 *  Initialize the H1 buttons driver.
 *
 *  @return >=0 success, <0 failure
 */
void BspRefButton_Init(void)
{
    H1RefButton_AdcInit();
}

