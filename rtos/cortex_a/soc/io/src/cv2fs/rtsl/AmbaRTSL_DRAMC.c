/**
 *  @file AmbaRTSL_DRAMC.c
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
 *  @details DRAM Controller/Arbiter RTSL APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_DRAMC.h"

/**
 *  AmbaRTSL_DramcInit - DRAM controller initializations
 */
void AmbaRTSL_DramcInit(void)
{
    return;
}

/**
 *  AmbaRTSL_DramcIrqInit - DRAM interrupt initializations
 */
void AmbaRTSL_DramcIrqInit(void)
{
    return;
}

/**
 *  AmbaRTSL_DramInitAtt - DRAM controller setup all ATT default value
 *  [TODO] Per AUTOSAR requirement, the default config shall be passed as argument.
 *         That means, AmbaRTSL_DramSetupAtt and AmbaRTSL_DramEnableVirtClient shall be combined into Init function in the future.
 *         The Init function could be called AmbaMemProt_Init which is implemented by above two funcions and MMU api.
 *         The AmbaRTSL_DramInitAtt could be gone if AmbaMemProt_Init is implemented.
 */
void AmbaRTSL_DramInitAtt(void)
{
    return;
}

/**
 *  AmbaRTSL_DramSetupAtt - DRAM controller setup page index to specified ATT entry
 *  @param[in] NumMemRegion number of memory regions
 *  @param[in] pMemRegion pointer to a list of memory region maps
 *  @return error code
 */
UINT32 AmbaRTSL_DramSetupAtt(UINT32 NumMemRegion, const AMBA_DRAM_ATT_INFO_s *pMemRegion)
{
    (void)NumMemRegion;
    (void)pMemRegion;

    return DRAMC_ERR_NONE;
}

/**
 *  AmbaRTSL_DramQueryAttV2P - It is used to get the translated physical address of specified virtual address
 *  @param[in] VirtAddr the virtual address
 *  @param[out] pPhysAddr the translated physical address
 *  @return error code
 */
UINT32 AmbaRTSL_DramQueryAttV2P(ULONG VirtAddr, UINT32 DramClient, ULONG *pPhysAddr)
{
    (void)DramClient;
    (void)VirtAddr;
    (void)pPhysAddr;

    return DRAMC_ERR_NONE;
}

/**
 *  AmbaRTSL_DramQueryAttP2V - It is used to get the translated virtual address of specified physical address
 *  @param[in] ClientId Client ID
 *  @param[in] PhysAddr the physical address
 *  @param[out] pVirtAddr the translated virtual address
 */
UINT32 AmbaRTSL_DramQueryAttP2V(ULONG PhysAddr, UINT32 ClientId, ULONG *pVirtAddr)
{
    (void)ClientId;
    (void)PhysAddr;
    (void)pVirtAddr;

    return DRAMC_ERR_NONE;
}

/**
 *  AmbaRTSL_DramConfigAttSecure - It is used to mark a part of ATT as secure region. The remaining parts of ATT are all treated as non-secure regions.
 *  @param[in] VirtAddr virtual base address of secure region
 *  @param[in] Size number of bytes reserved for secure region
 *  @return error code
 */
UINT32 AmbaRTSL_DramConfigAttSecure(ULONG VirtAddr, ULONG Size)
{
    (void)VirtAddr;
    (void)Size;

    return DRAMC_ERR_NONE;
}

/**
 *  AmbaRTSL_DramEnableVirtClient - DRAM controller setup VPN segment for specified client and enable its virtual attribute
 *  @param[in] ClientId DRAM client ID
 *  @param[in] VirtAddr virtual base address of the specified dram client
 *  @param[in] Size number of bytes reserved for the specified dram client
 *  @return error code
 */
UINT32 AmbaRTSL_DramEnableVirtClient(UINT32 ClientId, ULONG VirtAddr, ULONG Size)
{
    (void)ClientId;
    (void)VirtAddr;
    (void)Size;
    return DRAMC_ERR_NONE;
}

/**
 *  AmbaRTSL_DramDisableVirtClient - DRAM controller disable its vitual attribute
 *  @param[in] ClientId DRAM client ID
 *  @return error code
 */
UINT32 AmbaRTSL_DramDisableVirtClient(UINT32 ClientId)
{
    (void)ClientId;
    return DRAMC_ERR_NONE;
}

/**
 *  AmbaRTSL_DramGetClientInfo - DRAM controller get the VPN segment for specified client
 *  @param[in] ClientId DRAM client ID
 *  @param[out] pVirtAddr virtual base address of the specified dram client
 *  @param[out] pSize number of bytes reserved for the specified dram client
 *  @return error code
 */
UINT32 AmbaRTSL_DramGetClientInfo(UINT32 ClientId, ULONG *pVirtAddr, ULONG *pSize)
{
    (void)ClientId;
    (void)pVirtAddr;
    (void)pSize;
    return DRAMC_ERR_NONE;
}

void AmbaRTSL_DramcEnableStatisCtrl(void)
{
    return;
}

void AmbaRTSL_DramcGetStatisInfo(AMBA_DRAMC_STATIS_s *StatisData)
{
    (void)StatisData;
    return;
}
