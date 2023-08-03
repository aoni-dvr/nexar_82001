/**
 *  @file AmbaPCIE_Deprecated.c
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
 *  @details PCIe driver for deprecated APIs.
 */
#include "AmbaPCIE_Drv.h"
#include "AmbaPCIE.h"

void AmbaPCIE_LinkInit(UINT32 mode, UINT32 gen, UINT32 use24mhz)
{
    AmbaMisra_TouchUnused(&mode);
    AmbaMisra_TouchUnused(&gen);
    AmbaMisra_TouchUnused(&use24mhz);
}
void AmbaPCIE_LinkStart(UINT32 mode, UINT32 gen, UINT32 use24mhz)
{
    AmbaMisra_TouchUnused(&mode);
    AmbaMisra_TouchUnused(&gen);
    AmbaMisra_TouchUnused(&use24mhz);
}
UINT32 AmbaPCIE_Ep_Init (UINT64 ep_base)
{
    AmbaMisra_TouchUnused(&ep_base);
    return 0;
}
UINT32 AmbaPCIE_Ep_MemAccessFromRp(UINT64 AxiAddr, UINT8 NumPassBits)
{
    AmbaMisra_TouchUnused(&AxiAddr);
    AmbaMisra_TouchUnused(&NumPassBits);
    return 0;
}

UINT32 AmbaPCIE_Ep_MemAccessToRp(UINT64 AxiAddr, UINT64 PciAddr, UINT8 NumPassBits)
{
    AmbaMisra_TouchUnused(&AxiAddr);
    AmbaMisra_TouchUnused(&PciAddr);
    AmbaMisra_TouchUnused(&NumPassBits);
    return 0;
}
UINT32 AmbaPCIE_Rp_Init(UINT64 rp_base, UINT64 axi_base)
{
    AmbaMisra_TouchUnused(&rp_base);
    AmbaMisra_TouchUnused(&axi_base);
    return 0;
}
UINT32 AmbaPCIE_Rp_MemAccessToEp(UINT64 AxiAddr, UINT64 PciAddr, UINT8 NumPassBits)
{
    AmbaMisra_TouchUnused(&AxiAddr);
    AmbaMisra_TouchUnused(&PciAddr);
    AmbaMisra_TouchUnused(&NumPassBits);
    return 0;
}
UINT32 AmbaPCIE_Rp_MemAccessFromEp(UINT64 AxiAddr, UINT64 PciAddr, UINT8 NumPassBits)
{
    AmbaMisra_TouchUnused(&AxiAddr);
    AmbaMisra_TouchUnused(&PciAddr);
    AmbaMisra_TouchUnused(&NumPassBits);
    return 0;
}
UINT32 AmbaPCIE_Rp_GetEpBarInfo(void)
{
    return 0;
}
