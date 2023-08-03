/**
 *  @file AmbaCSL_I2C.c
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
 *  @details I2C CSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaCSL_I2C.h"

/**
 *  AmbaCSL_I2cSetPrescaler - Set prescaler value
 *  @param[in] pI2cReg pointer to the I2C registers
 *  @param[in] Value Prescaler value
 */
void AmbaCSL_I2cSetPrescaler(AMBA_I2C_REG_s *pI2cReg, UINT32 Value)
{
    /* I2C period = (APB bus clock) / (4 * (prescaler[15:0] + 1) + 2) */
    AmbaCSL_I2cSetPrescalerLowByte(pI2cReg, Value & 0xffU);
    AmbaCSL_I2cSetPrescalerHighByte(pI2cReg, (Value >> 8U) & (UINT32)0xffU);
}

/**
 *  AmbaCSL_I2cGetPrescaler - Get prescaler value
 *  @param[in] pI2cReg pointer to the I2C registers
 *  @return Prescaler value
 */
UINT32 AmbaCSL_I2cGetPrescaler(const AMBA_I2C_REG_s *pI2cReg)
{
    UINT32 PrescalerValue = pI2cReg->PrescalerHighByte.Value;

    PrescalerValue <<= 8U;
    PrescalerValue |= (pI2cReg->PrescalerLowByte.Value);

    return PrescalerValue;
}
