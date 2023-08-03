/**
*  @file AmbaSvcWrap.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*
*  @details svc wrap functions
*
*/

#include "AmbaMisraFix.h"
#include "AmbaSvcWrap.h"
#include "AmbaPrint.h"
#include "AmbaNVM_Partition.h"
#include "AmbaSYS.h"

typedef struct {
    /* cache operations */
    UINT32 (*pAmbaSvcCacheClean)(ULONG Addr, ULONG Size);
    UINT32 (*pAmbaSvcCacheInvalidate)(ULONG Addr, ULONG Size);
    UINT32 (*pAmbaSvcCacheFlush)(ULONG Addr, ULONG Size);

    UINT32 (*pAmbaSvcCacheChk)(void *pStartAddr, UINT32 Size);

    UINT32 (*pAmbaSvcVir2Phys)(void *VirtAddr, void **PhysAddr);
} AMBA_SVC_CTRL_s;


static AMBA_SVC_CTRL_s AmbaSvcCtrl;

#if defined(CONFIG_ENABLE_NAND_BOOT)
UINT32 PtbNvmID = AMBA_NVM_NAND;
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
UINT32 PtbNvmID = AMBA_NVM_SPI_NAND;
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
UINT32 PtbNvmID = AMBA_NVM_SPI_NOR;
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
UINT32 PtbNvmID = AMBA_NVM_eMMC;
#else
#pragma message ("[AmbaSvcWrap.c] ERROR !! UNKNOWN BOOT DEVICE !!")
#endif

/**
* wrap of type_cast function
* @param [out] pNewType new type
* @param [in] pOldType old type
* @return none
*/
//void AmbaSvcWrap_TypeCast(void *pNewType, const void *pOldType)
//{
//#if defined(CONFIG_ARM32)
//    AmbaMisra_TypeCast32(pNewType, pOldType);
//#else
//    AmbaMisra_TypeCast64(pNewType, pOldType);
//#endif
//}

/**
* id get of current NVM device
* @param [out] pNvmID id of nvm device
* @return 0-OK, 1-NG
*/
UINT32 AmbaSvcWrap_GetNVMID(UINT32 *pNvmID, UINT32 PtbID, UINT32 PartID)
{
    UINT32  BootDev, Rval;

    //NvmID is AMBA_NVM_ID_NULL by default
    *pNvmID = AMBA_NVM_ID_NULL;

    Rval = AmbaSYS_GetBootMode(&BootDev);
    if (SYS_ERR_NONE == Rval) {
        switch (BootDev) {
        #if defined(CONFIG_ENABLE_NAND_BOOT)
        case AMBA_SYS_BOOT_FROM_NAND:
            *pNvmID = AMBA_NVM_NAND;
            break;
        #elif defined(CONFIG_ENABLE_EMMC_BOOT)
        case AMBA_SYS_BOOT_FROM_EMMC:
            *pNvmID = AMBA_NVM_eMMC;
            break;
        #elif defined(CONFIG_ENABLE_SPINOR_BOOT)
        #if defined(CONFIG_MUTI_BOOT_DEVICE)
        case AMBA_SYS_BOOT_FROM_SPI_NOR:
            if (PtbID == AMBA_PTB_ID_SYS) {
                if (PartID >= AMBA_NUM_SYS_PARTITION) {
                    Rval = ERR_ARG;
                } else {
                    *pNvmID = AMBA_NVM_SPI_NOR;
                }
            } else if (PtbID == AMBA_PTB_ID_USER) {
                extern AMBA_PARTITION_CONFIG_s AmbaNORSPI_UserPartConfig[AMBA_NUM_USER_PARTITION];
                if (PartID >= AMBA_NUM_USER_PARTITION) {
                    Rval = ERR_ARG;
                } else {
                    if (PartID == AMBA_USER_PARTITION_PTB) {
                        *pNvmID = AMBA_NVM_SPI_NOR;
                    } else {
                        *pNvmID = (AmbaNORSPI_UserPartConfig[PartID].Attribute & 0xC0U) >> 6;
                    }
                }
            } else {
                Rval = ERR_ARG;
            }
            break;
        #else
        case AMBA_SYS_BOOT_FROM_SPI_NOR:
            *pNvmID = AMBA_NVM_SPI_NOR;
            break;
        #endif
        #elif defined(CONFIG_ENABLE_SPINAND_BOOT)
        case AMBA_SYS_BOOT_FROM_SPI_NAND:
            *pNvmID = AMBA_NVM_SPI_NAND;
            break;
        #endif
        default:
            AmbaPrint_PrintUInt5("AmbaSvcWrap_GetNVMID: undefined boot dev ID %u, input: PtbID %u, PartID %u", BootDev, PtbID, PartID, 0U, 0U);
            *pNvmID = AMBA_NVM_ID_NULL;
            Rval = ERR_ARG;
            break;
        }
    }
    AmbaMisra_TouchUnused(&PartID);
    AmbaMisra_TouchUnused(&PtbID);

    return Rval;
}

/**
* Initialization amba platform
* @param [in]  pPlatInit init config
*/
void AmbaSvcWrap_Init(const AMBA_SVC_WRAP_INIT_s *pInit)
{
    AmbaSvcCtrl.pAmbaSvcCacheClean      = pInit->pAmbaSvcCacheClean;
    AmbaSvcCtrl.pAmbaSvcCacheInvalidate = pInit->pAmbaSvcCacheInvalidate;
    AmbaSvcCtrl.pAmbaSvcCacheFlush      = pInit->pAmbaSvcCacheFlush;
    AmbaSvcCtrl.pAmbaSvcCacheChk        = pInit->pAmbaSvcCacheChk;
    AmbaSvcCtrl.pAmbaSvcVir2Phys        = pInit->pAmbaSvcVir2Phys;
}

/**
* cache clean
* @param [in]  Addr cache address
* @param [in]  Size cache size
* @return ErrorCode
*/
UINT32 AmbaSvcWrap_CacheClean(ULONG Addr, ULONG Size)
{
    UINT32 Rval = 0U;

    if (AmbaSvcCtrl.pAmbaSvcCacheClean != NULL) {
        Rval = AmbaSvcCtrl.pAmbaSvcCacheClean(Addr, Size);
    }

    return Rval;
}

/**
* cache invalidate
* @param [in]  Addr cache address
* @param [in]  Size cache size
* @return ErrorCode
*/
UINT32 AmbaSvcWrap_CacheInvalidate(ULONG Addr, ULONG Size)
{
    UINT32 Rval = 0U;

    if (AmbaSvcCtrl.pAmbaSvcCacheInvalidate != NULL) {
        Rval = AmbaSvcCtrl.pAmbaSvcCacheInvalidate(Addr, Size);
    }

    return Rval;
}

/**
* cache flush
* @param [in]  Addr cache address
* @param [in]  Size cache size
* @return ErrorCode
*/
UINT32 AmbaSvcWrap_CacheFlush(ULONG Addr, ULONG Size)
{
    UINT32 Rval = 0U;

    if (AmbaSvcCtrl.pAmbaSvcCacheFlush != NULL) {
        Rval = AmbaSvcCtrl.pAmbaSvcCacheFlush(Addr, Size);
    }

    return Rval;
}

/**
* cache check
* @param [in]  Addr cache address
* @param [in]  Size cache size
* @return ErrorCode
*/
UINT32 AmbaSvcWrap_CacheChk(ULONG Addr, ULONG Size)
{
    UINT32 Rval = 0U;
    void   *StartAddr;

    if (AmbaSvcCtrl.pAmbaSvcCacheChk != NULL) {
        AmbaMisra_TypeCast(&StartAddr, &Addr);
        Rval = AmbaSvcCtrl.pAmbaSvcCacheChk(StartAddr, (UINT32)Size);
    }

    return Rval;
}

/**
* virtual address to physical
* @param [in]  VirtAddr virtual address
* @param [out]  pPhysAddr physical address
* @return ErrorCode
*/
UINT32 AmbaSvcWrap_Vir2Phys(void *VirtAddr, void **PhysAddr)
{
    UINT32 Rval = 0U;

    if (AmbaSvcCtrl.pAmbaSvcVir2Phys != NULL) {
        Rval = AmbaSvcCtrl.pAmbaSvcVir2Phys(VirtAddr, PhysAddr);
    }

    return Rval;
}
