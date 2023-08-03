/**
 *  @file AmbaCSL_PWC.c
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
 *  @details PWC (Power Control Circuits) CSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaCSL_PLL.h"
#include "AmbaCSL_PWC.h"
#include "AmbaCSL_RTC.h"

/**
 *  AmbaCSL_PwcSetPseq1Delay - Set delays for the PWC (Power Control Circuits)
 *  @param[in] Delay A delay to separate the occurence of two continuous PWC signals (in ms)
 */
void AmbaCSL_PwcSetPseq1Delay(UINT32 Delay)
{
    pAmbaScratchpadS_Reg->Pseq1Delay.Delay = (UINT8)Delay;
}

/**
 *  AmbaCSL_PwcSetPseq2Delay - Set delays for the PWC (Power Control Circuits)
 *  @param[in] Delay A delay to separate the occurence of two continuous PWC signals (in ms)
 */
void AmbaCSL_PwcSetPseq2Delay(UINT32 Delay)
{
    pAmbaScratchpadS_Reg->Pseq2Delay.Delay = (UINT8)Delay;
}

/**
 *  AmbaCSL_PwcSetPseq3Delay - Set delays for the PWC (Power Control Circuits)
 *  @param[in] Delay A delay to separate the occurence of two continuous PWC signals (in ms)
 */
void AmbaCSL_PwcSetPseq3Delay(UINT32 Delay)
{
    pAmbaScratchpadS_Reg->Pseq3Delay.Delay = (UINT8)Delay;
}

/**
 *  AmbaCSL_PwcSetPseq4Delay - Set delays for the PWC (Power Control Circuits)
 *  @param[in] Delay A delay to separate the occurence of two continuous PWC signals (in ms)
 */
void AmbaCSL_PwcSetPseq4Delay(UINT32 Delay)
{
    pAmbaScratchpadS_Reg->Pseq4Delay.Delay = (UINT8)Delay;
}

/**
 *  AmbaCSL_PwcTogglePCRST - Toggle PCRST
 */
void AmbaCSL_PwcTogglePCRST(void)
{
    UINT32 DelayCycle = AmbaCSL_PllGetNumCpuCycleMs();

    AmbaCSL_RtcSetResetReg(0U);         /* PCRST = 0 */
    AmbaDelayCycles(DelayCycle * 3U);   /* Delay 3ms */
    AmbaCSL_RtcSetResetReg(1U);         /* PCRST = 1: Rising Edge to latch PWC registers */
    AmbaDelayCycles(DelayCycle * 3U);   /* Delay 3ms */
    AmbaCSL_RtcSetResetReg(0U);         /* PCRST = 0 */
}

/**
 *  AmbaCSL_PwcSetScratchpad - Set PWC scratchpad
 *  @param[in] BitMask Data bits to be set
 */
void AmbaCSL_PwcSetScratchpad(UINT32 BitMask)
{
    AmbaCSL_PwcWriteScratchpad(BitMask | AmbaCSL_PwcReadScratchpad());
    AmbaCSL_PwcTogglePCRST();
}

/**
 *  AmbaCSL_PwcClearScratchpad - Set PWC scratchpad
 *  @param[in] BitMask Data bits to be cleared
 */
void AmbaCSL_PwcClearScratchpad(UINT32 BitMask)
{
    AmbaCSL_PwcWriteScratchpad((~BitMask) & AmbaCSL_PwcReadScratchpad());
    AmbaCSL_PwcTogglePCRST();
}

