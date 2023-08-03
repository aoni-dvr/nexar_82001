/**
 *  @file AmbaMemProt.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for memory protection APIs
 *
 */

#ifndef AMBA_MEM_PROT_H
#define AMBA_MEM_PROT_H

#ifndef AMBA_DRAMC_DEF_H
#include "AmbaDRAMC_Def.h"
#endif

/*
 *  The MemProt api remaps memory addresses for hardware modules other than cpu. (NOTE: cpu uses mmu scheme)
 *  The VirtAddr here is only used in dram ATT hardware module.
 *  The virtual-to-physical address translation table could be different to cpu tlbs.
 *  User should be carefully on using these VirtAddr values.
 */
UINT32 AmbaMemProt_Init(UINT32 NumMemRegion, const AMBA_DRAM_ATT_INFO_s *pMemMap);
UINT32 AmbaMemProt_Enable(UINT32 DramClient, ULONG BaseIntmdAddr, ULONG NumIntmdAddr);
UINT32 AmbaMemProt_QueryPhysAddr(ULONG IntmdAddr, UINT32 DramClient, ULONG *pPhysAddr);
UINT32 AmbaMemProt_QueryIntmdAddr(ULONG PhysAddr, UINT32 TgtDramClient, ULONG *pTgtIntmdAddr);
UINT32 AmbaMemProt_TranslAddr(ULONG SrcIntmdAddr, UINT32 SrcDramClient, UINT32 TgtDramClient, ULONG *pTgtIntmdAddr);

#endif /* AMBA_MEM_PROT_H */
