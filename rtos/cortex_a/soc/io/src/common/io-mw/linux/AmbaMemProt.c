/**
 *  @file AmbaMemProt.c
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
 *  @details Memory Protection APIs
 *
 */

#include "AmbaMemProt.h"
//#include "AmbaRTSL_DRAMC.h"
#include "AmbaMisraFix.h"

/**
*  AmbaMemProt_Init - Initialize a global address translation table for memory protection
*  @param[in] NumMemRegion Number of memory regions defined in memory region array
*  @param[in] pMemMap Pointer to an array to specify the details of each memory region of the memory map. The pointed data shall be kept constant and available during runtime.
*  @return error code (i.e. virtual address conflict; physical address shall be the same as virtual address for the specific client such as Cortex-R52)
*/
UINT32 AmbaMemProt_Init(UINT32 NumMemRegion, const AMBA_DRAM_ATT_INFO_s *pMemMap)
{
    //UINT32 RetVal;

    //RetVal = AmbaRTSL_DramSetupAtt(NumMemRegion, pMemMap);

    //return RetVal;
    AmbaMisra_TouchUnused(&NumMemRegion);
    AmbaMisra_TouchUnused(&pMemMap);

    return DRAMC_ERR_NONE;
}

/**
 *  AmbaMemProt_Enable - Enable access control for the specified dram client
 *  @param[in] DramClient dram client id
 *  @param[in] BaseIntmdAddr the first intermediate memory address for dram client access
 *  @param[in] NumIntmdAddr accessible memory region size in bytes
 *  @return error code
 */
UINT32 AmbaMemProt_Enable(UINT32 DramClient, ULONG BaseIntmdAddr, ULONG NumIntmdAddr)
{
    //UINT32 RetVal;

    //RetVal = AmbaRTSL_DramEnableVirtClient(DramClient, BaseIntmdAddr, NumIntmdAddr);

    //return RetVal;
    AmbaMisra_TouchUnused(&DramClient);
    AmbaMisra_TouchUnused(&BaseIntmdAddr);
    AmbaMisra_TouchUnused(&NumIntmdAddr);

    return DRAMC_ERR_NONE;
}

/**
 *  AmbaMemProt_QueryPhysAddr - Query a physical address by an intermediate memory address
 *  @param[in] IntmdAddr the requested intermediate memory address
 *  @param[out] pPhysAddr translated physical memory address
 *  @return error code
 */
UINT32 AmbaMemProt_QueryPhysAddr(ULONG IntmdAddr, UINT32 DramClient, ULONG *pPhysAddr)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    (void) DramClient;
    *pPhysAddr = IntmdAddr;
    //RetVal = AmbaRTSL_DramQueryAttV2P(IntmdAddr, pPhysAddr);

    return RetVal;
}

/**
 *  AmbaMemProt_QueryIntmdAddr - Query an intermediate memory address of the specified dram client by a physical address
 *  @param[in] PhysAddr the requested physical address
 *  @param[in] TgtDramClient target dram client id
 *  @param[out] pTgtIntmdAddr target intermediate memory address which is accessible by the specified dram client
 */
UINT32 AmbaMemProt_QueryIntmdAddr(ULONG PhysAddr, UINT32 TgtDramClient, ULONG *pTgtIntmdAddr)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    //RetVal = AmbaRTSL_DramQueryAttP2V(TgtDramClient, PhysAddr, pTgtIntmdAddr);
    *pTgtIntmdAddr = PhysAddr;

    AmbaMisra_TouchUnused(&TgtDramClient);

    return RetVal;
}

/**
 *  AmbaMemProt_TranslAddr - Query an intermediate memory address of the specified dram client by an intermediate address
 *  @param[in] SrcIntmdAddr the requested intermediate memory address
 *  @param[in] TgtDramClient target dram client id
 *  @param[out] pTgtIntmdAddr target intermediate memory address which is accessible by the specified dram client
 */
UINT32 AmbaMemProt_TranslAddr(ULONG SrcIntmdAddr, UINT32 SrcDramClient, UINT32 TgtDramClient, ULONG *pTgtIntmdAddr)
{
    UINT32 RetVal = DRAMC_ERR_NONE;
    (void) SrcIntmdAddr;
    (void) TgtDramClient;
    (void) pTgtIntmdAddr;
    (void) SrcDramClient;
    //RetVal = AmbaRTSL_DramQueryAttV2P(SrcIntmdAddr, &PhyAddr);
    //if (RetVal == DRAMC_ERR_NONE) {
    //    RetVal = AmbaRTSL_DramQueryAttP2V(TgtDramClient, PhyAddr, pTgtIntmdAddr);
    //}

    return RetVal;
}
