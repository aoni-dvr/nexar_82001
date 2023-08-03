/**
 *  @file AmbaMAL_Krn.c
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
 *  @details Ambarella MAL (Memory Abstraction Layer) APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaFDT.h"
#include "AmbaKAL.h"
#include "AmbaCache.h"
#include "AmbaMMU.h"
#include "AmbaMAL.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaMAL.h"

typedef struct {
    UINT32 Used;
    UINT32 Attri;
    UINT64 PhysAddr;
    void *pVirtAddr;
    UINT64 Size;
} AMBA_MAL_TRANS_PRIV_s;

typedef struct {
    AMBA_MAL_INFO_s Info;
    UINT64 CurrAddr;
} AMBA_MAL_INFO_USR_PRIV_s;

static AMBA_KAL_MUTEX_t AmbaMALMutex;
static AMBA_MAL_INFO_USR_PRIV_s AmbaMalUsrPriv[AMBA_MAL_ID_MAX];
static UINT32 AmbaMalInit = 0U;

static UINT32 AmbaMAL_NodeParse(const void *pFdt, INT32 Node, UINT32 *pId, UINT64 *pBase, UINT64 *pSize, UINT32 *pType, UINT32 *pCapability)
{
    UINT32 Ret = 0U;
    const struct fdt_property *pProp = NULL;
    const char *pChr1 = NULL;
    const UINT32 *pValue = NULL;
    const UINT64 *pValue64 = NULL;
    INT32 Len = 0;
    INT32 SubOffset;
    const fdt32_t *php;
    UINT32 handle;

    /*Parse ID */
    pProp = AmbaFDT_GetProperty(pFdt, Node, "amb_mal_id", &Len);
    if (pProp) {
        pChr1 = (const char *)&pProp->data[0];
        AmbaMisra_TypeCast(&pValue, &pChr1);
        *pId = AmbaFDT_Fdt32ToCpu(pValue[0]);
        if (*pId >= AMBA_MAL_ID_MAX) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_KrnNodeParse() : Id(%d) is over AMBA_MAL_ID_MAX(%d)\n", *pId, AMBA_MAL_ID_MAX, 0U, 0U, 0U);
            Ret = 1U;
        }
    } else {
        Ret = 1U;
    }

    if(Ret == 0U) {
        /*Find handle */
        php = AmbaFDT_Getprop(pFdt, Node, "memory-region", &Len);
        if (php) {
            handle = AmbaFDT_Fdt32ToCpu(*php);
            SubOffset = AmbaFDT_NodeOffsetByPhandle(pFdt, handle);
        } else {
            Ret = 1U;
        }
    }

    if(Ret == 0U) {
        /*Parse type and cap */
        pProp = AmbaFDT_GetProperty(pFdt, SubOffset, "reusable", &Len);
        if (pProp) {
            *pType = AMBA_MAL_TYPE_ALLOCATABLE;
            *pCapability = 0x35U;
        } else {
            *pType = AMBA_MAL_TYPE_DEDICATED;
            *pCapability = 0x31U;
        }

        /*Parse base and size */
        pProp = AmbaFDT_GetProperty(pFdt, SubOffset, "reg", &Len);
        if ((pProp != NULL) && (Len == 8)) {
            pChr1 = (const char *)&pProp->data[0];
            AmbaMisra_TypeCast(&pValue, &pChr1);
            *pBase = (UINT64)AmbaFDT_Fdt32ToCpu(pValue[0]);
            *pSize = (UINT64)AmbaFDT_Fdt32ToCpu(pValue[1]);
        } else if ((pProp != NULL) && (Len == 16)) {
            pChr1 = (const char *)&pProp->data[0];
            AmbaMisra_TypeCast(&pValue64, &pChr1);
            *pBase = (UINT64)AmbaFDT_Fdt64ToCpu(pValue64[0]);
            *pSize = (UINT64)AmbaFDT_Fdt64ToCpu(pValue64[1]);
        }
    }

    return Ret;
}

/**
 *  @brief      Init AmbaMal.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Init(void)
{
    UINT32 Ret = 0U;
    const void *pFdt;
    ULONG DtbAddr;
    INT32 Rval, Offset, Node;

    if(AmbaMalInit == 0) {
        AmbaMalInit = 2U;

        Ret = AmbaKAL_MutexCreate(&AmbaMALMutex, "ambamal");
        if(Ret != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Init() : AmbaKAL_MutexCreate fail Ret 0x%x \n",Ret,0U,0U,0U,0U);
        } else {

            DtbAddr = (ULONG)CONFIG_DTB_LOADADDR;
            AmbaMisra_TypeCast(&pFdt, &DtbAddr);
            Rval = AmbaFDT_CheckHeader(pFdt);
            if (Rval == 0) {
                Offset = AmbaFDT_NodeOffsetByCompatible(pFdt, 0,"ambarella,mal");
                if (Offset >= 0) {
                    for (Node = AmbaFDT_FirstSubnode(pFdt, Offset); Node >= 0; Node = AmbaFDT_NextSubnode(pFdt, Node)) {
                        UINT32 Id = 0U, Type = 0U, Capability = 0U;
                        UINT64 Base = 0UL, Size = 0UL;

                        Ret = AmbaMAL_NodeParse(pFdt, Node, &Id, &Base, &Size, &Type, &Capability);
                        /*Update table */
                        if((Ret == 0U) && (Size > 0)) {
                            AmbaMalUsrPriv[Id].Info.Id = Id;
                            AmbaMalUsrPriv[Id].Info.Type = Type;
                            AmbaMalUsrPriv[Id].Info.Capability = Capability;
                            AmbaMalUsrPriv[Id].Info.PhysAddr = Base;
                            AmbaMalUsrPriv[Id].Info.GlobalAddr = Base;
                            AmbaMalUsrPriv[Id].Info.Size = Size;
                            AmbaMalUsrPriv[Id].CurrAddr = 0UL;
                            AmbaPrint_PrintUInt5("AmbaMal : id[%d] base[0x%x] size[0x%x] type [0x%x] cape [0x%x]\n", Id, (UINT32)Base, (UINT32)Size, (UINT32)Type, (UINT32)Capability);
                        } else if (Ret != 0U) {
                            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Init() : AmbaMAL_NodeParse fail Ret 0x%x \n",Ret,0U,0U,0U,0U);
                        }
                    }
                }
            }
        }
        if(Ret != 0) {
            AmbaMalInit = 0U;
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Init() : fail Ret 0x%x \n",Ret,0U,0U,0U,0U);
        } else {
            AmbaMalInit = 1U;
        }
    } else if (AmbaMalInit == 2) {
        do {
            AmbaKAL_TaskSleep(10); //sleep 10 ms
        } while(AmbaMalInit == 2);
    }

    return Ret;
}

/**
 *  @brief      Allocate buffer from AmbaMAL Memory ID (Only for AMBA_MAL_TYPE_ALLOCATABLE).
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      Size : [IN] Size to allocate.
 *
 *  @param      Align : [IN] Base and Size to align.
 *
 *  @param      pBuf : [OUT] Return buffer structure.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Alloc(UINT32 Id, UINT64 Size, UINT32 Align, AMBA_MAL_BUF_s *pBuf)
{
    UINT32 Ret = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Alloc() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Alloc() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    } else if (pBuf == NULL) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Alloc() : Id [%d] pBuf == NULL \n",Id, 0U, 0U, 0U, 0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ALLOCATE) == 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Alloc() : Id [%d] Capability[0x%x] couldn't allocate \n",Id,AmbaMalUsrPriv[Id].Info.Capability, 0U, 0U, 0U);
        Ret = 1U;
    } else {
        UINT64 AddrAlign,SizeAlign;

        if (AmbaKAL_MutexTake(&AmbaMALMutex, KAL_WAIT_FOREVER) != 0U) {
            Ret = 1U;
        } else {
            SizeAlign = ((Size + Align - 1UL)  & (~((UINT64)Align - 1UL)));
            AddrAlign = ((AmbaMalUsrPriv[Id].CurrAddr + Align - 1UL)  & (~((UINT64)Align - 1UL)));
            if ((SizeAlign + AddrAlign) > AmbaMalUsrPriv[Id].Info.Size) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Alloc() : Id [%d] Size(0x%x) > left Size (0x%x)", Id, (UINT32)SizeAlign, (UINT32)(AmbaMalUsrPriv[Id].Info.Size - AddrAlign), 0U, 0U);
                Ret = 1U;
            } else {
                pBuf->Id = Id;
                pBuf->Size= Size;
                pBuf->Align = Align;
                pBuf->RealSize = SizeAlign;
                pBuf->PhysAddr = (AddrAlign + AmbaMalUsrPriv[Id].Info.PhysAddr);
                pBuf->GlobalAddr = (AddrAlign + AmbaMalUsrPriv[Id].Info.PhysAddr);
                AmbaMalUsrPriv[Id].CurrAddr = (AddrAlign + SizeAlign);
            }
            if(AmbaKAL_MutexGive(&AmbaMALMutex) != 0U) {
                AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Alloc() : Id [%d] AmbaKAL_MutexGive fail \n",Id, 0U, 0U, 0U, 0U);
            }
        }
    }
    return Ret;
}

/**
 *  @brief      Free buffer to AmbaMAL Memory ID (Only for AMBA_MAL_TYPE_ALLOCATABLE).
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pBuf : [IN] Buffer structure.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Free(UINT32 Id, AMBA_MAL_BUF_s *pBuf)
{
    UINT32 Ret = 0U;

    if (pBuf == NULL) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Free() : Id [%d] pBuf == NULL \n",Id, 0U, 0U, 0U, 0U);
        Ret = 1U;
    } else if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Free() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX, 0U, 0U, 0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_FREE) == 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Free() : Id [%d] Capability[0x%x] couldn't free \n",Id,AmbaMalUsrPriv[Id].Info.Capability, 0U, 0U, 0U);
        Ret = 1U;
    } else {

    }
    return Ret;
}

/**
 *  @brief      Map buffer to get virtual address for CPU used.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      Size : [IN] Size to map.
 *
 *  @param      Attri : [IN] Attribute of mmap.
 *
 *  @param      ppVirtAddr : [OUT] Return virtual address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Map(UINT32 Id, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, void **ppVirtAddr)
{
    UINT32 Ret = 0U;

    (void) Attri;
    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Map() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Map() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_MAP) == 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Map() : Id [%d] Capability[0x%x] couldn't map \n",Id,AmbaMalUsrPriv[Id].Info.Capability,0U,0U,0U);
        Ret = 1U;
    } else {
        ULONG CastVirtAddr = 0UL;

        if(AmbaMMU_PhysToVirt((ULONG)PhysAddr, &CastVirtAddr) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Map() : Id [%d] AmbaMMU_PhysToVirt fail PhysAddr 0x%x Size 0x%x\n",Id,(UINT32)PhysAddr,(UINT32)Size,0U,0U);
            Ret = 1U;
        } else {
            AmbaMisra_TypeCast(ppVirtAddr, &CastVirtAddr);
        }
    }
    return Ret;
}

/**
 *  @brief      UnMap buffer.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address.
 *
 *  @param      Size : [IN] Size to unmap.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Unmap(UINT32 Id, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    (void) Size;
    if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Unmap() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    }
    if (pVirtAddr == NULL) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Unmap() : Id [%d] pVirtAddr == NULL \n",Id, 0U, 0U, 0U, 0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_UNMAP) == 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Unmap() : Id [%d] Capability[0x%x] couldn't unmap \n",Id,AmbaMalUsrPriv[Id].Info.Capability,0U,0U,0U);
        Ret = 1U;
    } else {

    }
    return Ret;
}

/**
 *  @brief      Data cache clean.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address to clean.
 *
 *  @param      Size : [IN] Size to clean.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_CacheClean(UINT32 Id, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_CacheClean() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_CACHE_OPS) == 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_CacheClean() : Id [%d] Capability[0x%x] couldn't do cache ops \n",Id,AmbaMalUsrPriv[Id].Info.Capability,0U,0U,0U);
        Ret = 1U;
    } else {
        ULONG CastVirtAddr = 0UL;

        AmbaMisra_TypeCast(&CastVirtAddr, &pVirtAddr);
        if(AmbaCache_DataClean(CastVirtAddr, Size) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_CacheClean() : Id [%d] AmbaCache_DataClean fail VirtAddr 0x%x Size 0x%x\n",Id,(UINT32)CastVirtAddr,(UINT32)Size,0U,0U);
            Ret = 1U;
        }
    }
    return Ret;
}

/**
 *  @brief      Data cache invalidate.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address to invalidate.
 *
 *  @param      Size : [IN] Size to invalidate.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_CacheInvalid(UINT32 Id, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_CACHE_OPS) == 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] Capability[0x%x] couldn't do cache ops \n",Id,AmbaMalUsrPriv[Id].Info.Capability,0U,0U,0U);
        Ret = 1U;
    } else {
        ULONG CastVirtAddr = 0UL;

        AmbaMisra_TypeCast(&CastVirtAddr, &pVirtAddr);
        if(AmbaCache_DataInvalidate(CastVirtAddr, Size) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] AmbaCache_DataInvalidate fail VirtAddr 0x%x Size 0x%x\n",Id,(UINT32)CastVirtAddr,(UINT32)Size,0U,0U);
            Ret = 1U;
        }
    }
    return Ret;
}

/**
 *  @brief      Get AmbaMAL Memory info by ID.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pInfo : [OUT] AmbaMAL memory info structure of AmbaMAL Memory ID.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_GetInfo(UINT32 Id, AMBA_MAL_INFO_s *pInfo)
{
    UINT32 Ret = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_GetInfo() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_GetInfo() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    } else {
        if (AmbaWrap_memcpy(pInfo, &AmbaMalUsrPriv[Id].Info, sizeof(AmbaMalUsrPriv[Id].Info)) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_GetInfo() : Id [%d] AmbaWrap_memcpy fail \n",Id,0U,0U,0U,0U);
            Ret = 1U;
        }
    }
    return Ret;
}

/**
 *  @brief      Get AmbaMAL Memory info by Physical address.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      pInfo : [OUT] AmbaMAL memory info structure of AmbaMAL Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_GetInfoByPhys(UINT64 PhysAddr, AMBA_MAL_INFO_s *pInfo)
{
    UINT32 Ret = 0U;
    UINT32 i,Found = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_GetInfoByPhys() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else {
        for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
            if(AmbaMalUsrPriv[i].Info.Type != AMBA_MAL_TYPE_INVALID) {
                if((AmbaMalUsrPriv[i].Info.PhysAddr <= PhysAddr) && (PhysAddr < (AmbaMalUsrPriv[i].Info.PhysAddr + AmbaMalUsrPriv[i].Info.Size))) {
                    if (AmbaWrap_memcpy(pInfo, &AmbaMalUsrPriv[i].Info, sizeof(AmbaMalUsrPriv[i].Info)) != 0U) {
                        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_GetInfoByPhys() : Id [%d] AmbaWrap_memcpy fail \n",i,0U,0U,0U,0U);
                        Ret = 1U;
                    }
                    Found = 1U;
                    break;
                }
            }
        }

        if(Found == 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_GetInfoByPhys() : pa 0x%x not found \n", (UINT32)PhysAddr, 0U, 0U, 0U, 0U);
            Ret = 1U;
        }
    }
    return Ret;
}

/**
 *  @brief      Physical to virtual address.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      ppVirtAddr : [OUT] Virtual address.
 *
 *  @param      Attri : [IN] AmbaMAL Memory ATTRI.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Phys2Virt(UINT32 Id, UINT64 PhysAddr, UINT32 Attri, void **ppVirtAddr)
{
    UINT32 Ret = 0U;

    (void) Attri;
    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Phys2Virt() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Phys2Virt() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ADDR_TRANS) == 0U) {
        Ret = 1U;
    } else {
        ULONG CastVirtAddr = 0UL;

        if(AmbaMMU_PhysToVirt((ULONG)PhysAddr, &CastVirtAddr) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Phys2Virt() : Id [%d] AmbaMMU_PhysToVirt fail PhysAddr 0x%x \n",Id,(UINT32)PhysAddr,(UINT32)0U,0U,0U);
            Ret = 1U;
        } else {
            AmbaMisra_TypeCast(ppVirtAddr, &CastVirtAddr);
        }
    }
    return Ret;
}


/**
 *  @brief      Virtual to physical address.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address.
 *
 *  @param      pPhysAddr : [OUT] Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Virt2Phys(UINT32 Id, void *pVirtAddr, UINT64 *pPhysAddr)
{
    UINT32 Ret = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Virt2Phys() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Virt2Phys() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ADDR_TRANS) == 0U) {
        Ret = 1U;
    } else {
        ULONG CastVirtAddr = 0UL, CastPhysAddr = 0UL;

        AmbaMisra_TypeCast(&CastVirtAddr, &pVirtAddr);
        if(AmbaMMU_VirtToPhys((ULONG)CastVirtAddr, &CastPhysAddr) != 0U) {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Virt2Phys() : Id [%d] AmbaMMU_VirtToPhys fail VirtAddr 0x%x \n",Id,(UINT32)CastVirtAddr,(UINT32)0U,0U,0U);
            Ret = 1U;
        } else {
            *pPhysAddr = (UINT64)CastPhysAddr;
        }
    }
    return Ret;
}

/**
 *  @brief      Physical to global address.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      pGlobalAddr : [OUT] Global address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Phys2Global(UINT64 PhysAddr, UINT64 *pGlobalAddr)
{
    UINT32 Ret = 0U;
    UINT32 i, Found = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Phys2Global() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else {
        for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
            if((AmbaMalUsrPriv[i].Info.PhysAddr <= PhysAddr) && (PhysAddr < (AmbaMalUsrPriv[i].Info.PhysAddr + AmbaMalUsrPriv[i].Info.Size))) {
                Found = 1U;
                break;
            }
        }

        if (Found) {
            UINT64 Offset;

            Offset      = PhysAddr - AmbaMalUsrPriv[i].Info.PhysAddr;
            *pGlobalAddr  = (AmbaMalUsrPriv[i].Info.GlobalAddr + Offset);
        } else {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Phys2Global() : pa 0x%x not found \n", (UINT32)PhysAddr, 0U, 0U, 0U, 0U);
            Ret = 1U;
        }
    }
    return Ret;
}


/**
 *  @brief      Global to physical address.
 *
 *  @param      GlobalAddr : [IN] Global address.
 *
 *  @param      pPhysAddr : [OUT] Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Global2Phys(UINT64 GlobalAddr, UINT64 *pPhysAddr)
{
    UINT32 Ret = 0U;
    UINT32 i, Found = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Global2Phys() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else {
        for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
            if((AmbaMalUsrPriv[i].Info.GlobalAddr <= GlobalAddr) && (GlobalAddr < (AmbaMalUsrPriv[i].Info.GlobalAddr + AmbaMalUsrPriv[i].Info.Size))) {
                Found = 1U;
                break;
            }
        }

        if (Found) {
            UINT64 Offset;

            Offset      = GlobalAddr - AmbaMalUsrPriv[i].Info.GlobalAddr;
            *pPhysAddr  = (AmbaMalUsrPriv[i].Info.PhysAddr + Offset);
        } else {
            AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Global2Phys() : ga 0x%x not found \n", (UINT32)GlobalAddr, 0U, 0U, 0U, 0U);
            Ret = 1U;
        }
    }

    return Ret;
}

/**
 *  @brief      Dump memory list by Id.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @return     0            on success.    \n
 */
void AmbaMAL_Dump(UINT32 Id)
{
    UINT32 Ret = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Dump() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit, 0U, 0U, 0U, 0U);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        AmbaPrint_PrintUInt5("[ERROR] AmbaMAL_Dump() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX,0U,0U,0U);
    } else {
        if(AmbaMalUsrPriv[Id].Info.Type != AMBA_MAL_TYPE_INVALID) {
            AmbaPrint_PrintUInt5("Id [%d] Pa 0x%lx - 0x%llx Ga 0x%lx \n", AmbaMalUsrPriv[Id].Info.Id, (UINT32)AmbaMalUsrPriv[Id].Info.PhysAddr, (UINT32)(AmbaMalUsrPriv[Id].Info.PhysAddr + AmbaMalUsrPriv[Id].Info.Size),(UINT32)AmbaMalUsrPriv[Id].Info.GlobalAddr, 0U);
        }
    }
}

