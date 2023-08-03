/**
 *  @file AmbaB8_Prescaler.c
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
 *  @details Ambarella B8 Prescaler APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_PwmDec.h"

#include "AmbaB8CSL_PwmDec.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_PwmDecSetCreditPwr
 *
 *  @Description:: Configure B8 PwmDec
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     CreditPwr:        B8  PwmDecCreditPwr
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PwmDecSetCreditPwr(UINT32 ChipID, UINT32 CreditPwr)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_PWM_DEC_CTRL_REG_s Ctrl = {0};

    /* PwmDec: a far-end module which translates and executes register configuration
               instructions received from the DSP via the B8N PWM encoder */
    if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
        Ctrl.Ch0CreditPwr = (UINT8)CreditPwr;
        Ctrl.Ch0WdChunkOption = 1U;
        (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_PwmDecReg->Ctrl, 0U, B8_DATA_WIDTH_32BIT, 1U, &Ctrl);

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;
}
