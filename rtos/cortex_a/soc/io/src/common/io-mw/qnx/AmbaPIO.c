/**
 * @file AmbaPIO.c
 *  Peripheral Memory Allocator
 *
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
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/procmgr.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "AmbaTypes.h"
#include "AmbaPIO.h"
#include "AmbaCache.h"

/**
 *  @brief      The function allocates Peripheral continuous buffer
 *
 *  @param      Size : [IN] The size of the request.
 *  @param      Cacheable : [IN] Cacheable (1) or not (0)
 *  @param      PioBuf : [OUT] The PIO buffer struture
 *
 *  @return     PIO error number
 */
UINT32 AmbaPIO_MemAlloc(ULONG Size, UINT32 Cacheable, AMBA_PIO_s *PioBuf)
{
    const ULONG G_PioEnd = (ULONG)CONFIG_PIO_BASE + (ULONG)CONFIG_PIO_SIZE;
    ULONG pAllocated, roffset;
    UINT32 ret;
    UINT32 PioCache;
    char buf[64];
    sem_t *pSem = sem_open("pio_sem", O_CREAT, 0666, 1U);
    int fd;

    /*simulate lockf F_LOCK because /dev/shmem does not support lockf*/
    for (;;) {
        if (sem_trywait(pSem) == 0)  {
            break;
        }
        fprintf(stderr, "%s: sem_trywait\n", __func__);
        usleep(1000);
    }

    /* read last offset */
    ret = PIO_ERR_ENOMEM;
    fd = open("/tmp/AmbaPIO.offset", O_CREAT|O_RDWR, 0666);
    if (fd >= 0) {
        roffset = read(fd, buf, sizeof(buf));
        buf[roffset] = '\0';
        if (roffset == 0) {
            roffset = (ULONG)CONFIG_PIO_BASE;
        } else {
            roffset = strtol(buf, NULL, 16);
        }
        if (lseek(fd, 0, SEEK_SET) == 0U) {
            ret = 0U;
        }
    }

    /* write new offset */
    if (ret == 0U) {
        ret = PIO_ERR_ENOMEM;;
        if ((roffset + Size) <= G_PioEnd) {
            PioBuf->PhyAddr = roffset;
            PioBuf->VirAddr = roffset;
            pAllocated = roffset + Size;
            snprintf(buf, sizeof(buf), "0x%08lx\n", pAllocated);
            if(write(fd, buf, strlen(buf) + 1) == 12) {
                ret = 0U;
            }
        } else {
            fprintf(stderr, "%s: Not enough mem\n", __func__);
        }
    }

    /* do mmap */
    if (ret == 0U) {
        if (Cacheable == 0U) {
            PioCache = PROT_READ | PROT_WRITE | PROT_NOCACHE;
        } else {
            PioCache = PROT_READ | PROT_WRITE;
        }

        PioBuf->Size = Size;
        PioBuf->Cacheable = Cacheable;

        PioBuf->VirAddr = (ULONG)mmap_device_memory((void *)PioBuf->PhyAddr, Size, PioCache, 0, PioBuf->PhyAddr);
    }

    if (fd >= 0) {
        close(fd);
    }

    /*simulate lockf F_ULOCK because /dev/shmem does not support lockf*/
    sem_post(pSem);

    return ret;
}

/**
 *  @brief      Sync CPU data cache to DRAM
 *
 *  @param      PioBuf : [IN] The PIO buffer struture used to run cache clean operation.
 *
 *  @return     PIO error number
 */
UINT32 AmbaPIO_CacheClean(AMBA_PIO_s *PioBuf)
{
    (void)PioBuf;
    return AmbaCache_DataClean(PioBuf->VirAddr, PioBuf->Size);
}

/**
 *  @brief      invalidates CPU data cache
 *
 *  @param      PioBuf : [IN] The PIO buffer struture used to run cache invalidates operation.
 *
 *  @return     PIO error number
 */
UINT32 AmbaPIO_CacheInvalid(AMBA_PIO_s *PioBuf)
{
    (void)PioBuf;
    return AmbaCache_DataInvalidate(PioBuf->VirAddr, PioBuf->Size);
}
