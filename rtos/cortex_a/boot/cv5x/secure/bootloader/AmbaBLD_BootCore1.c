/**
 *  @file AmbaBLD_BootCore1.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details Bootloader for Boot CV5 Core1
 *
 */

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_CORTEX_A76_H
#include "AmbaCortexA76.h"
#endif

#define AMBA_AXI_MISC_CTRL_OFFSET   10U
#define AMBA_AXI_RVBAR_ADDR1_OFFSET 26U

#pragma GCC optimize ("O0")
void AmbaBLD_BootCore1(UINT32 entry)
{
    volatile UINT32 *pAxiConfigReg = (volatile UINT32 *)AMBA_CORTEX_A76_AXI_CONFIG_PHYS_BASE_ADDR;

    pAxiConfigReg[AMBA_AXI_RVBAR_ADDR1_OFFSET] = (entry >> 0x8U);
    pAxiConfigReg[AMBA_AXI_MISC_CTRL_OFFSET] = pAxiConfigReg[AMBA_AXI_MISC_CTRL_OFFSET] & ~(0x2U<<16U);
}
#pragma GCC push_options

