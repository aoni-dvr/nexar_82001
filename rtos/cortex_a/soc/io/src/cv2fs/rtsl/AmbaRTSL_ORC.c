/**
 *  @file AmbaRTSL_ORC.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details ORC Control APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_ORC.h"
#include "AmbaCSL_DebugPort.h"

/**
 *  AmbaRTSL_OrcSetPC - Set up DSP PC Registers
 *  @param[in] MainPC The new value of Main DSP PC register
 *  @param[in] Sub0PC The new value of Sub DSP-0 PC register
 *  @param[in] Sub1PC The new value of Sub DSP-1 PC register
 */
void AmbaRTSL_OrcSetPC(UINT32 MainPC, UINT32 Sub0PC, UINT32 Sub1PC)
{
    if (MainPC != 0U) {
        AmbaCSL_DebugPortSetOrcPC(0, MainPC);
    }

    if (Sub0PC != 0U) {
        AmbaCSL_DebugPortSetOrcPC(1, Sub0PC);
    }

    if (Sub1PC != 0U) {
        AmbaCSL_DebugPortSetOrcPC(2, Sub1PC);
    }
}

/**
 *  AmbaRTSL_OrcEnable - Enable all ORCs
 */
void AmbaRTSL_OrcEnable(void)
{
    AmbaCSL_DebugPortEnableORC(0, 0x00ff);      /* Enable main DSP */
    AmbaCSL_DebugPortEnableORC(1, 0x0f);        /* Enable Sub DSP-0 */
    AmbaCSL_DebugPortEnableORC(2, 0x0f);        /* Enable Sub DSP-1 */
}

/**
 *  AmbaRTSL_OrcSuspend - Suspend all ORCs
 */
void AmbaRTSL_OrcSuspend(void)
{
    AmbaCSL_DebugPortEnableORC(0, 0xff00);      /* Suspend main DSP */
    AmbaCSL_DebugPortEnableORC(1, 0xf0);        /* Suspend Sub DSP-0 */
    AmbaCSL_DebugPortEnableORC(2, 0xf0);        /* Suspend Sub DSP-1 */
}

/**
 *  AmbaRTSL_OrcReset - Reset all ORCs
 */
void AmbaRTSL_OrcReset(void)
{
    AmbaCSL_DebugPortResetORC(0, 0x1U);         /* Reset main DSP */
    AmbaCSL_DebugPortResetORC(1, 0x1U);         /* Reset Sub DSP-0 */
    AmbaCSL_DebugPortResetORC(2, 0x1U);         /* Reset Sub DSP-1 */
}

/**
 *  AmbaRTSL_GetOrcTimer - Get ORC time ticks
 */
UINT32 AmbaRTSL_GetOrcTimer(void)
{
    return AmbaCSL_GetOrcTimer();
}
