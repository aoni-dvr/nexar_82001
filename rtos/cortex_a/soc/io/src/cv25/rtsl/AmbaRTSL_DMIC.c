/**
 *  @file AmbaRTSL_DMIC.c
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
 *  @details DMIC RTSL Device Driver
 *
 */

#include "AmbaTypes.h"

#include "AmbaRTSL_DMIC.h"
#include "AmbaCSL_DMIC.h"

/**
 *  AmbaRTSL_DmicEnable - Enable dmic
 *  @return error code
 */
UINT32 AmbaRTSL_DmicEnable(const UINT32 *pIirFltrCoeff)
{
    AMBA_DMIC_REG_s *pDmicReg = pAmbaDMIC_Reg;
    UINT32 i;

    /* Initialize */
    AmbaCSL_DmicReset(pDmicReg);
    AmbaCSL_DmicInitClk(pDmicReg);
    AmbaCSL_DmicInitDroopCompFltr(pDmicReg);
    AmbaCSL_DmicInitHalfBandFltr(pDmicReg);
    AmbaCSL_DmicInitWindNoiseFltr(pDmicReg);

    if (pIirFltrCoeff != NULL) {
        /* Apply iir filter coefficients */
        for (i = 0; i < 17U; i++) {
            AmbaCSL_DmicCustomIirFilterCof(pDmicReg, i, pIirFltrCoeff[i]);
        }
    }

    /* Set dmic as i2s input */
    AmbaCSL_DmicI2sSelect(1U);

    /* Enable */
    AmbaCSL_DmicEnable(pDmicReg);

    return OK;
}

/**
 *  AmbaRTSL_DmicDisable - Disable dmic
 *  @return error code
 */
UINT32 AmbaRTSL_DmicDisable(void)
{
    AMBA_DMIC_REG_s *pDmicReg = pAmbaDMIC_Reg;

    AmbaCSL_DmicEn(pDmicReg, 0x0);
    AmbaCSL_DmicEnSt(pDmicReg, 0x0);
    AmbaCSL_DmicWndEn(pDmicReg, 0x0);
    AmbaCSL_DmicWnfEn(pDmicReg, 0x0);
    AmbaCSL_DmicI2sSelect(0U);

    return OK;
}

