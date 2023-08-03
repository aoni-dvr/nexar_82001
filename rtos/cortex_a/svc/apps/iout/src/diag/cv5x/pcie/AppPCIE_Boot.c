/**
 *  @file AppPCIE_Boot.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details application for PCIE boot.
 */

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include <AmbaUtility.h>
#include <AmbaShell.h>
#include <AmbaIOUTDiag.h>
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AppPCIE.h"

#define LOADER_SIZE 16*1024


void AppPCIE_BootActiveEp(void)
{
    UINT8 buf[LOADER_SIZE];// maximum size: 16KB
    UINT8 *buf_ptr = buf;
    UINT64 buf_addr;
    UINT64 mmio_addr = UT_PCIE_AXI_BASE + 4; //toggle target: axi space + 4
    volatile UINT32 *tt;
    UINT32 value = 0;
    PCIER_IB_PARAM_s ib_parm;

    AmbaMisra_TypeCast(&buf_addr, &buf_ptr);
    // setup ib for a device with active endpoint mode
    ib_parm.MemAddr = buf_addr;
    ib_parm.PciAddr = 0x0100000000100000;
    ib_parm.Size = 16*1024;
    (void)AmbaPCIER_Inbound(&ib_parm); // maximum size: 16KB
    // todo: read the loader
    AmbaMisra_TypeCast(&tt, &mmio_addr);
    // toggle IO enable twice
    value = *tt;
    AmbaPrint_PrintUInt5("CSR = 0x%x ", value, 0U, 0U, 0U, 0U);
    value |= 6;
    AmbaPrint_PrintUInt5("CSR = 0x%x ", value, 0U, 0U, 0U, 0U);
    value |= 1UL << 0;
    AmbaPrint_PrintUInt5("CSR = 0x%x ", value, 0U, 0U, 0U, 0U);
    value &= ~(1UL << 0);
    AmbaPrint_PrintUInt5("CSR = 0x%x ", value, 0U, 0U, 0U, 0U);
}

void AppPCIE_BootPassiveEp(void)
{
    UINT64 mmio_addr = 0x2000004000;
    volatile UINT8 *tt;
    PCIER_OB_PARAM_s ob_parm;

    ob_parm.AxiAddr = mmio_addr;
    ob_parm.PciAddr = 0x0000004000;
    ob_parm.Size = 16*1024;
    (void)AmbaPCIER_Outbound(&ob_parm); // make sure enough space for loader and magic number
    AmbaMisra_TypeCast(&tt, &mmio_addr);

    // todo: read the loader
    // fill magic number
    tt[LOADER_SIZE] = 0x65;
    tt[LOADER_SIZE+1] = 0x66;
    tt[LOADER_SIZE+2] = 0x77;
    tt[LOADER_SIZE+3] = 0x65;

}
