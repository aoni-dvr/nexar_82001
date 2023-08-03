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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "AmbaMAL.h"
#include "AmbaMAL_Ioctl.h"

#if !defined(CONFIG_MAL_TRANSLATE_TABLE_MAX)
#define AMBA_MAL_TRANS_MAX                 128
#else
#define AMBA_MAL_TRANS_MAX                 CONFIG_MAL_TRANSLATE_TABLE_MAX
#endif
#define PAGE_ALIGN_SIZE   4096UL

typedef struct {
    UINT32 Ref;
    UINT32 Attri;
    UINT64 PhysAddr;
    void *pVirtAddr;
    UINT64 Size;
} AMBA_MAL_TRANS_PRIV_s;

typedef struct {
    AMBA_MAL_INFO_s Info;
    pthread_mutex_t Mutex;
    AMBA_MAL_TRANS_PRIV_s Table[AMBA_MAL_TRANS_MAX];
} AMBA_MAL_INFO_USR_PRIV_s;

static INT32 AmbaMalFd = -1;
static AMBA_MAL_INFO_USR_PRIV_s AmbaMalUsrPriv[AMBA_MAL_ID_MAX];
static AMBA_MAL_IOCTL_SEG_INFO_s AmbaMalUsrSegList;

static UINT32 AmbaMalInit = 0U;
/**
 *  @brief      Init AmbaMal.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Init(void)
{
    UINT32 Ret = 0U;
    UINT32 i;
    AMBA_MAL_IOCTL_VERSION_s ReqVer = {0};

    if(AmbaMalInit == 0) {
        AmbaMalInit = 2U;

        if (AmbaMalFd < 0) {
            AmbaMalFd = open(AMBAMAL_DEV, O_RDWR);
            if(AmbaMalFd < 0) {
                printf("[ERROR] AmbaMAL_Init() : open fail AmbaMalFd = %d errno = %d \n", AmbaMalFd, errno);
                Ret = 1U;
            }
        }

        if(Ret == 0U) {
            if (ioctl(AmbaMalFd, AMBA_MAL_GET_VERSION, &ReqVer) != 0) {
                printf("[ERROR] AmbaMAL_Init() : ioctl 0x%lx fail errno = %d \n", AMBA_MAL_GET_VERSION, errno);
                Ret = 1U;
            } else {
                if(ReqVer.Version != AMBA_MAL_KERNEL_VERSION) {
                    printf("[ERROR] AmbaMAL_Init() : Version (0x%x) != AMBA_MAL_KERNEL_VERSION (0x%x) \n", ReqVer.Version, AMBA_MAL_KERNEL_VERSION);
                    Ret = 1U;
                } else if (ReqVer.Number != AMBA_MAL_ID_MAX) {
                    printf("[ERROR] AmbaMAL_Init() : Max_ID (0x%x) != AMBA_MAL_ID_MAX (0x%x) \n", ReqVer.Number, AMBA_MAL_ID_MAX);
                    Ret = 1U;
                } else {

                }
            }
        }

        if(Ret == 0U) {
            AmbaMalUsrSegList.SegNum = AMBA_MAL_SEG_NUM;
            if (ioctl(AmbaMalFd, AMBA_MAL_GET_SEG_INFO, &AmbaMalUsrSegList) != 0) {
                printf("[ERROR] AmbaMAL_Init() : ioctl 0x%lx fail errno = %d \n",AMBA_MAL_GET_SEG_INFO, errno);
                Ret = 1U;
            }
        }

        if(Ret == 0U) {
            for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
                AmbaMalUsrPriv[i].Info.Id = i;
                pthread_mutex_init(&AmbaMalUsrPriv[i].Mutex, 0);
                if (ioctl(AmbaMalFd, AMBA_MAL_GET_INFO, &AmbaMalUsrPriv[i].Info) != 0) {
                    printf("[ERROR] AmbaMAL_Init() : Id[%d] ioctl 0x%lx fail errno = %d \n",i, AMBA_MAL_GET_INFO, errno);
                    Ret = 1U;
                    break;
                } else {
                    memset(AmbaMalUsrPriv[i].Table, 0, sizeof(AmbaMalUsrPriv[i].Table));
                    if(AmbaMalUsrPriv[i].Info.Type != AMBA_MAL_TYPE_INVALID) {
                        //printf("[INFO] AmbaMAL_Init() : id[%d] base[0x%llx] size[0x%llx] type [0x%x] cape [0x%x] \n",AmbaMalUsrPriv[i].Info.Id, AmbaMalUsrPriv[i].Info.PhysAddr, AmbaMalUsrPriv[i].Info.Size, AmbaMalUsrPriv[i].Info.Type, AmbaMalUsrPriv[i].Info.Capability);
                    }
                }
            }
        }

        if(Ret != 0) {
            AmbaMalInit = 0U;
        } else {
            AmbaMalInit = 1U;
        }
    } else if (AmbaMalInit == 2) {
        do {
            usleep(10000); //sleep 10 ms
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
        printf("[ERROR] AmbaMAL_Alloc() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_Alloc() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if (pBuf == NULL) {
        printf("[ERROR] AmbaMAL_Alloc() : Id [%d] pBuf == NULL \n",Id);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ALLOCATE) == 0U) {
        printf("[ERROR] AmbaMAL_Alloc() : Id [%d] Capability[0x%x] couldn't allocate \n",Id,AmbaMalUsrPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        pBuf->Id = Id;
        pBuf->Size= Size;
        pBuf->Align = Align;
        if (ioctl(AmbaMalFd, AMBA_MAL_ALLOC_BUF, pBuf) != 0) {
            printf("[ERROR] AmbaMAL_Alloc() : Id [%d] ioctl 0x%lx fail errno = %d \n", Id, AMBA_MAL_ALLOC_BUF, errno);
            AmbaMAL_Dump(Id);
            Ret = 1U;
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
        printf("[ERROR] AmbaMAL_Free() : Id [%d] pBuf == NULL \n",Id);
        Ret = 1U;
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_Free() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_FREE) == 0U) {
        printf("[ERROR] AmbaMAL_Free() : Id [%d] Capability[0x%x] couldn't free \n",Id,AmbaMalUsrPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        if (ioctl(AmbaMalFd, AMBA_MAL_FREE_BUF, pBuf) != 0) {
            printf("[ERROR] AmbaMAL_Free() : Id [%d] ioctl 0x%lx fail errno = %d \n", Id, AMBA_MAL_FREE_BUF, errno);
            AmbaMAL_Dump(Id);
            Ret = 1U;
        }
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
    void *pVirtAddr = NULL;
    static INT32 MapCacheFd = -1;
    static INT32 MapNonCacheFd = -1;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(MapCacheFd == -1) {
        MapCacheFd = open(AMBAMAL_DEV, O_RDWR);
    }

    if(MapNonCacheFd == -1) {
        MapNonCacheFd = open(AMBAMAL_DEV, (O_SYNC | O_RDWR));
    }


    if(Ret != 0U) {
        printf("[ERROR] AmbaMAL_Map() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_Map() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_MAP) == 0U) {
        printf("[ERROR] AmbaMAL_Map() : Id [%d] Capability[0x%x] couldn't map \n",Id,AmbaMalUsrPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        UINT32 i,Found = 0U;

        pthread_mutex_lock(&AmbaMalUsrPriv[Id].Mutex);
        for(i = 0U; i < AMBA_MAL_TRANS_MAX; i++) {
            if((AmbaMalUsrPriv[Id].Table[i].Ref > 0U) && (AmbaMalUsrPriv[Id].Table[i].Attri == Attri)) {
                if((AmbaMalUsrPriv[Id].Table[i].PhysAddr <= PhysAddr) && ((PhysAddr + Size) <= (AmbaMalUsrPriv[Id].Table[i].PhysAddr + AmbaMalUsrPriv[Id].Table[i].Size))) {
                    UINT64 Offset;

                    Found = 1U;
                    Offset      = PhysAddr - AmbaMalUsrPriv[Id].Table[i].PhysAddr;
                    *ppVirtAddr  = (void *)(AmbaMalUsrPriv[Id].Table[i].pVirtAddr + Offset);
                    AmbaMalUsrPriv[Id].Table[i].Ref += 1U;
                    break;
                }
            }
        }
        pthread_mutex_unlock(&AmbaMalUsrPriv[Id].Mutex);

        if(Found == 0U) {
            if((MapCacheFd < 0) || (MapNonCacheFd < 0)) {
                printf("[ERROR] AmbaMAL_Map() : Id [%d] open fail MapCacheFd = %d MapCacheFd = %d errno = %d \n",Id, MapCacheFd, MapNonCacheFd,errno);
                Ret = 1U;
            }

            if(Ret == 0U) {
                UINT64 AddrAlign;

                AddrAlign = PhysAddr & (~(PAGE_ALIGN_SIZE - 1UL));
                if(Attri == AMBA_MAL_ATTRI_CACHE) {
                    pVirtAddr = mmap(NULL, Size + (PhysAddr - AddrAlign), (PROT_READ | PROT_WRITE), MAP_SHARED, MapCacheFd, AddrAlign);
                } else {
                    pVirtAddr = mmap(NULL, Size + (PhysAddr - AddrAlign), (PROT_READ | PROT_WRITE), MAP_SHARED, MapNonCacheFd, AddrAlign);
                }
                if (pVirtAddr == MAP_FAILED) {
                    printf("[ERROR] AmbaMAL_Map() : Id [%d] mmap fail pa 0x%llx size 0x%llx errno = %d \n",Id, PhysAddr, Size, errno);
                    AmbaMAL_Dump(Id);
                } else {
                    UINT32 i;

                    *ppVirtAddr = pVirtAddr + (PhysAddr - AddrAlign);
                    pthread_mutex_lock(&AmbaMalUsrPriv[Id].Mutex);
                    for(i = 0U; i < AMBA_MAL_TRANS_MAX; i++) {
                        if(AmbaMalUsrPriv[Id].Table[i].Ref == 0U) {
                            AmbaMalUsrPriv[Id].Table[i].Ref = 1U;
                            AmbaMalUsrPriv[Id].Table[i].PhysAddr = AddrAlign;
                            AmbaMalUsrPriv[Id].Table[i].pVirtAddr = pVirtAddr;
                            AmbaMalUsrPriv[Id].Table[i].Size = Size + (PhysAddr - AddrAlign);
                            AmbaMalUsrPriv[Id].Table[i].Attri = Attri;
                            Found = 1U;
                            break;
                        }
                    }
                    pthread_mutex_unlock(&AmbaMalUsrPriv[Id].Mutex);
                    if(Found == 0U) {
                        printf("[ERROR] AmbaMAL_Map() : Id [%d] table list[%d] is full \n", Id, AMBA_MAL_TRANS_MAX);
                        AmbaMAL_Dump(Id);
                        Ret = 1U;
                    }
                }
            }
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

    if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_Unmap() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_UNMAP) == 0U) {
        printf("[ERROR] AmbaMAL_Unmap() : Id [%d] Capability[0x%x] couldn't unmap \n",Id,AmbaMalUsrPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        UINT32 i,Found = 0U;

        pthread_mutex_lock(&AmbaMalUsrPriv[Id].Mutex);
        for(i = 0U; i < AMBA_MAL_TRANS_MAX; i++) {
            if(AmbaMalUsrPriv[Id].Table[i].Ref > 0U) {
                if((AmbaMalUsrPriv[Id].Table[i].pVirtAddr <= pVirtAddr) && ((pVirtAddr + Size) <= (AmbaMalUsrPriv[Id].Table[i].pVirtAddr + AmbaMalUsrPriv[Id].Table[i].Size))) {
                    Found = 1U;
                    AmbaMalUsrPriv[Id].Table[i].Ref -= 1U;
                    if(AmbaMalUsrPriv[Id].Table[i].Ref == 0U) {
                        if (munmap(AmbaMalUsrPriv[Id].Table[i].pVirtAddr, AmbaMalUsrPriv[Id].Table[i].Size) != 0) {
                            printf("[ERROR] AmbaMAL_Unmap() : Id [%d] unmmap fail va %p size 0x%llx errno = %d \n", Id, pVirtAddr, Size, errno);
                            AmbaMAL_Dump(Id);
                            Ret = 1;
                        }
                        memset(&AmbaMalUsrPriv[Id].Table[i], 0U, sizeof(AmbaMalUsrPriv[Id].Table[i]));
                    }
                    break;
                }
            }
        }
        pthread_mutex_unlock(&AmbaMalUsrPriv[Id].Mutex);

        if(Found == 0U) {
            printf("[ERROR] AmbaMAL_Unmap() : Id [%d] va %p size 0x%llx not found \n", Id, pVirtAddr, Size);
            AmbaMAL_Dump(Id);
            Ret = 1U;
        }
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
    AMBA_MAL_IOCTL_CACHE_s ReqCache;

    if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_CacheClean() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_CACHE_OPS) == 0U) {
        printf("[ERROR] AmbaMAL_CacheClean() : Id [%d] Capability[0x%x] couldn't do cache ops \n",Id,AmbaMalUsrPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        UINT64 PhysAddr;

        Ret = AmbaMAL_Virt2Phys(Id, pVirtAddr, &PhysAddr);
        if(Ret != 0) {
            printf("[ERROR] AmbaMAL_CacheClean() : Id [%d] AmbaMAL_Virt2Phys fail pVirtAddr %p \n",Id, pVirtAddr);
            AmbaMAL_Dump(Id);
        } else {
            ReqCache.Id = Id;
            ReqCache.PhysAddr = PhysAddr;
            ReqCache.Size = Size;
            ReqCache.Ops = 1U;
            if (ioctl(AmbaMalFd, AMBA_MAL_CACHE_OPS, &ReqCache) != 0) {
                printf("[ERROR] AmbaMAL_CacheClean() : Id [%d] ioctl 0x%lx fail errno = %d pVirtAddr %p PhysAddr 0x%llx Size 0x%llx\n",Id, AMBA_MAL_CACHE_OPS, errno, pVirtAddr, PhysAddr, Size);
                AmbaMAL_Dump(Id);
                Ret = 1U;
            }
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
    AMBA_MAL_IOCTL_CACHE_s ReqCache;

    if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_CACHE_OPS) == 0U) {
        printf("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] Capability[0x%x] couldn't do cache ops \n",Id,AmbaMalUsrPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        UINT64 PhysAddr;

        Ret = AmbaMAL_Virt2Phys(Id, pVirtAddr, &PhysAddr);
        if(Ret != 0) {
            printf("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] AmbaMAL_Virt2Phys fail pVirtAddr %p \n",Id, pVirtAddr);
            AmbaMAL_Dump(Id);
        } else {
            ReqCache.Id = Id;
            ReqCache.PhysAddr = PhysAddr;
            ReqCache.Size = Size;
            ReqCache.Ops = 2U;
            if (ioctl(AmbaMalFd, AMBA_MAL_CACHE_OPS, &ReqCache) != 0) {
                printf("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] ioctl 0x%lx fail errno = %d pVirtAddr %p PhysAddr 0x%llx Size 0x%llx\n",Id, AMBA_MAL_CACHE_OPS, errno, pVirtAddr, PhysAddr, Size);
                AmbaMAL_Dump(Id);
                Ret = 1U;
            }
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
        printf("[ERROR] AmbaMAL_GetInfo() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_GetInfo() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else {
        memcpy(pInfo, &AmbaMalUsrPriv[Id].Info, sizeof(AmbaMalUsrPriv[Id].Info));
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
        printf("[ERROR] AmbaMAL_GetInfoByPhys() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else {
        for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
            if(AmbaMalUsrPriv[i].Info.Type != AMBA_MAL_TYPE_INVALID) {
                if((AmbaMalUsrPriv[i].Info.PhysAddr <= PhysAddr) && (PhysAddr < (AmbaMalUsrPriv[i].Info.PhysAddr + AmbaMalUsrPriv[i].Info.Size))) {
                    memcpy(pInfo, &AmbaMalUsrPriv[i].Info, sizeof(AmbaMalUsrPriv[i].Info));
                    Found = 1U;
                    break;
                }
            }
        }

        if(Found == 0U) {
            printf("[ERROR] AmbaMAL_GetInfoByPhys() : pa 0x%llx not found \n", PhysAddr);
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
    UINT32 i;
    UINT32 Found = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        printf("[ERROR] AmbaMAL_Phys2Virt() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_Phys2Virt() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ADDR_TRANS) == 0U) {
        Ret = 1U;
    } else {
        pthread_mutex_lock(&AmbaMalUsrPriv[Id].Mutex);
        for(i = 0U; i < AMBA_MAL_TRANS_MAX; i++) {
            if((AmbaMalUsrPriv[Id].Table[i].Ref != 0U) && (AmbaMalUsrPriv[Id].Table[i].Attri == Attri)) {
                if((AmbaMalUsrPriv[Id].Table[i].PhysAddr <= PhysAddr) && (PhysAddr < (AmbaMalUsrPriv[Id].Table[i].PhysAddr + AmbaMalUsrPriv[Id].Table[i].Size))) {
                    Found = 1U;
                    break;
                }
            }
        }

        if (Found) {
            UINT64 Offset;

            Offset      = PhysAddr - AmbaMalUsrPriv[Id].Table[i].PhysAddr;
            *ppVirtAddr  = (void *)(AmbaMalUsrPriv[Id].Table[i].pVirtAddr + Offset);
        } else {
            *ppVirtAddr = NULL;
            Ret = 1U;
        }
        pthread_mutex_unlock(&AmbaMalUsrPriv[Id].Mutex);
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
    UINT32 i;
    UINT32 Found = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        printf("[ERROR] AmbaMAL_Virt2Phys() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_Virt2Phys() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalUsrPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ADDR_TRANS) == 0U) {
        Ret = 1U;
    } else {
        pthread_mutex_lock(&AmbaMalUsrPriv[Id].Mutex);
        for(i = 0U; i < AMBA_MAL_TRANS_MAX; i++) {
            if(AmbaMalUsrPriv[Id].Table[i].Ref != 0U) {
                if((AmbaMalUsrPriv[Id].Table[i].pVirtAddr <= pVirtAddr) && (pVirtAddr < (AmbaMalUsrPriv[Id].Table[i].pVirtAddr + AmbaMalUsrPriv[Id].Table[i].Size))) {
                    Found = 1U;
                    break;
                }

            }
        }

        if (Found) {
            UINT64 Offset;

            Offset      = pVirtAddr - AmbaMalUsrPriv[Id].Table[i].pVirtAddr;
            *pPhysAddr   = AmbaMalUsrPriv[Id].Table[i].PhysAddr + Offset;
        } else {
            *pPhysAddr  = 0UL;
            Ret = 1U;
        }
        pthread_mutex_unlock(&AmbaMalUsrPriv[Id].Mutex);
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

    *pGlobalAddr = 0xFF00000000000000UL;
    if(Ret != 0U) {
        printf("[ERROR] AmbaMAL_Phys2Global() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else {
        for (i = 0U; i < AMBA_MAL_SEG_NUM; i++) {
            if (0x0U != AmbaMalUsrSegList.SegList[i].Size) {
                if ((PhysAddr >= AmbaMalUsrSegList.SegList[i].PhysAddr) && (PhysAddr < AmbaMalUsrSegList.SegList[i].PhysAddr + AmbaMalUsrSegList.SegList[i].Size)) {
                    *pGlobalAddr = AmbaMalUsrSegList.SegList[i].GlobalAddr + (PhysAddr - AmbaMalUsrSegList.SegList[i].PhysAddr);
                    Found = 1U;
                    break;
                }
            }
        }

        if (Found == 0U) {
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

    *pPhysAddr = 0UL;
    if(Ret != 0U) {
        printf("[ERROR] AmbaMAL_Global2Phys() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else {
        for (i = 0U; i < AMBA_MAL_SEG_NUM; i++) {
            if (0x0U != AmbaMalUsrSegList.SegList[i].Size) {
                if ((GlobalAddr >= AmbaMalUsrSegList.SegList[i].GlobalAddr) && (GlobalAddr < AmbaMalUsrSegList.SegList[i].GlobalAddr + AmbaMalUsrSegList.SegList[i].Size)) {
                    *pPhysAddr = AmbaMalUsrSegList.SegList[i].PhysAddr + (GlobalAddr - AmbaMalUsrSegList.SegList[i].GlobalAddr);
                    Found = 1U;
                    break;
                }
            }
        }

        if (Found == 0U) {
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
    UINT32 i;
    UINT32 Ret = 0U;

    if(AmbaMalInit != 1U) {
        Ret = AmbaMAL_Init();
    }

    if(Ret != 0U) {
        printf("[ERROR] AmbaMAL_Dump() : AmbaMAL_Init fail state (%d)\n",AmbaMalInit);
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printf("[ERROR] AmbaMAL_Dump() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
    } else {
        if(AmbaMalUsrPriv[Id].Info.Type != AMBA_MAL_TYPE_INVALID) {
            printf("Id [%d] Pa 0x%llx - 0x%llx Ga 0x%llx \n", AmbaMalUsrPriv[Id].Info.Id, AmbaMalUsrPriv[Id].Info.PhysAddr, (AmbaMalUsrPriv[Id].Info.PhysAddr + AmbaMalUsrPriv[Id].Info.Size),AmbaMalUsrPriv[Id].Info.GlobalAddr);

            pthread_mutex_lock(&AmbaMalUsrPriv[Id].Mutex);
            for(i = 0U; i < AMBA_MAL_TRANS_MAX; i++) {
                if(AmbaMalUsrPriv[Id].Table[i].Ref != 0U) {
                    printf("        Map : Pa 0x%llx - 0x%llx Va %p Attr %d Ref %d\n", AmbaMalUsrPriv[Id].Table[i].PhysAddr, AmbaMalUsrPriv[Id].Table[i].PhysAddr + AmbaMalUsrPriv[Id].Table[i].Size, AmbaMalUsrPriv[Id].Table[i].pVirtAddr, AmbaMalUsrPriv[Id].Table[i].Attri, AmbaMalUsrPriv[Id].Table[i].Ref);
                }
            }
            pthread_mutex_unlock(&AmbaMalUsrPriv[Id].Mutex);
        }
    }
}

