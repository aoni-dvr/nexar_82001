/*
 * Copyright (c) 2020 Ambarella International LP
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
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaCSL_PCIE.h"


void AmbaRTSL_PciePhyWrite(UINT32 offset, UINT32 Value)
{
    AmbaCSL_PciePhyWrite(offset, Value);
}

UINT32 AmbaRTSL_PciePhyRead(UINT32 offset)
{
    UINT32 uret = AmbaCSL_PciePhyRead(offset);
    return uret;
}

void AmbaRTSL_PcieSpWrite(UINT32 offset, UINT32 Value)
{
    AmbaCSL_PcieSpWrite(offset, Value);
}

UINT32 AmbaRTSL_PcieSpRead(UINT32 offset)
{
    UINT32 uret = AmbaCSL_PcieSpRead(offset);
    return uret;
}

void AmbaRTSL_PcieWrite(UINT32 offset, UINT32 Value)
{
    AmbaCSL_PcieWrite(offset, Value);
}

UINT32 AmbaRTSL_PcieRead(UINT32 offset)
{
    UINT32 uret = AmbaCSL_PcieRead(offset);
    return uret;
}
