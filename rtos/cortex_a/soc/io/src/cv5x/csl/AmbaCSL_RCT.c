/**
 *  @file AmbaCSL_RCT.c
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
 *  @details RCT (other than PLL) CSL
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaCSL_RCT.h"

/**
 *  AmbaCSL_RctChipSoftReset - Trigger SoC soft reset
 */
void AmbaCSL_RctChipSoftReset(void)
{
    /* Clear the bit */
    AmbaCSL_RctClearSoftReset();

    AmbaDelayCycles(0xffff);    /* wait for a while */

    /* Set the bit to enable the soft reset */
    AmbaCSL_RctSetSoftReset();

    AmbaAssert();
}

/**
 *  AmbaCSL_RctTimer0GetCounter - Read RCT timer 0 counter value
 *  @return current counter value
 */
UINT32 AmbaCSL_RctTimer0GetCounter(void)
{
    UINT32 Val, Backup;

    Backup = pAmbaRCT_Reg->RctTimer0Ctrl;
    AmbaCSL_RctTimer0Freeze();
    Val = pAmbaRCT_Reg->RctTimer0Counter;
    pAmbaRCT_Reg->RctTimer0Ctrl = Backup;

    return Val;
}

/**
 *  AmbaCSL_RctTimer1GetCounter - Read RCT timer 1 counter value
 *  @return current counter value
 */
UINT32 AmbaCSL_RctTimer1GetCounter(void)
{
    UINT32 Val, Backup;

    Backup = pAmbaRCT_Reg->RctTimer1Ctrl;
    AmbaCSL_RctTimer1Freeze();
    Val = pAmbaRCT_Reg->RctTimer1Counter;
    pAmbaRCT_Reg->RctTimer1Ctrl = Backup;

    return Val;
}

