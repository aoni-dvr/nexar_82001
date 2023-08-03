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

#include "AmbaWrap.h"
#include "AmbaKAL.h"
#include "AmbaPIO.h"
#include "AmbaCache.h"
#include "AmbaMMU.h"
#include "pio/AmbaPIO_IO.h"

#define PIO_DEVNAME     "/dev/pio"
#define MAP_FAILED      ((void *) -1)
#define CACHELINE_SIZE  (64U)
#define PIO_OFFSET_PATH "/tmp/AmbaPIO.offset"

/**
 *  @brief      Initialize pio driver
 *
 *  @return     PIO error number
 */
UINT32 AmbaPIO_DrvEntry(void)
{
    UINT32 ret = PIO_ERR_NONE;
    char buf[128];
    const UINT64 pio_base = CONFIG_PIO_BASE;

    /* Check pio device node. If pio device has not been created, do insmod */
    if (access(PIO_DEVNAME, F_OK) != 0) {
        sprintf(buf, "modprobe ambarella_pio pio_base=0x%08llx pio_size=0x%08x", pio_base, CONFIG_PIO_SIZE);
        if (system(buf) < 0) { }
    } else {
        remove(PIO_OFFSET_PATH);
    }

    return ret;
}

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
    UINT32 ret = PIO_ERR_ENOMEM;
    UINT32 PioCache;
    INT32 fd_mmap, fd_offset;
    void *mapped;
    ULONG pAllocated, roffset = 0;
    ULONG offsize;
    char buf[128];
    sem_t *pSem;

    if ((Size == 0U) || (PioBuf == NULL)) {
        ret = PIO_ERR_EINVAL;
    } else {
        pSem = sem_open("pio_sem", O_CREAT, 0666, 1U);

        /* lock */
        for (;;) {
            if (sem_trywait(pSem) == 0)  {
                break;
            }
            usleep(1000);
        }

        /* set config */
        if (Cacheable == 0U) {
            PioCache = O_RDWR | O_DSYNC;
        } else {
            PioCache = O_RDWR;
        }

        PioBuf->Size = Size;
        PioBuf->Cacheable = Cacheable;

        /* read last offset */
        ret = PIO_ERR_ENOMEM;
        fd_offset = open(PIO_OFFSET_PATH, O_CREAT|O_RDWR, 0666);
        if (fd_offset >= 0) {
            roffset = read(fd_offset, buf, sizeof(buf));
            buf[roffset] = '\0';
            if (roffset == 0) {
                roffset = (ULONG)CONFIG_PIO_BASE;
            } else {
                roffset = strtol(buf, NULL, 16);
                /* the addresss need to align to cacheline size 64 byte */
                if ((roffset & (CACHELINE_SIZE - 1U)) != 0U) {
                    roffset = (roffset & ~(CACHELINE_SIZE - 1U)) + CACHELINE_SIZE;
                }
            }
            if (lseek(fd_offset, 0, SEEK_SET) == 0U) {
                ret = PIO_ERR_NONE;
            }
        }

        /* write new offset */
        if (ret == PIO_ERR_NONE) {
            ret = PIO_ERR_ENOMEM;
            if ((roffset + Size) <= ((ULONG)CONFIG_PIO_BASE + (ULONG)CONFIG_PIO_SIZE)) {
                PioBuf->PhyAddr = roffset;
                PioBuf->VirAddr = roffset;
                pAllocated = roffset + Size;
                snprintf(buf, sizeof(buf), "0x%08lx\n", pAllocated);
                if(write(fd_offset, buf, strlen(buf) + 1) == 12) {
                    ret = PIO_ERR_NONE;
                }
            } else {
                fprintf(stderr, "%s: Not enough mem\n", __func__);
            }
            close (fd_offset);
        }

        /* mmap */
        if (ret == PIO_ERR_NONE) {
            /* open pio device for mmap */
            if ((fd_mmap = open(PIO_DEVNAME, PioCache)) == -1) {
                fprintf(stderr, "open:%s\n", strerror(errno));
                sem_post(pSem);
                return -1;
            }

            /* do mmap */
            offsize = PioBuf->PhyAddr & ~(sysconf(_SC_PAGE_SIZE) - 1);
            if ((mapped = mmap(NULL, Size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mmap, offsize)) == MAP_FAILED) {
                fprintf(stderr, "mmap:%s\n", strerror(errno));
                close (fd_mmap);
                sem_post(pSem);
                return -1;
            }
            PioBuf->VirAddr = (ULONG)mapped + ((PioBuf->PhyAddr - CONFIG_PIO_BASE) % sysconf(_SC_PAGE_SIZE));
            close(fd_mmap);
        }

        /* unlock */
        sem_post(pSem);
    }

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
    INT32 fd;
    INT32 iret;
    UINT32 uret = PIO_ERR_NONE;

    if (PioBuf == NULL) {
        uret = PIO_ERR_EINVAL;
    } else {
        if ((fd = open(PIO_DEVNAME, O_RDWR)) == -1) {
            fprintf(stderr, "open:%s\n", strerror(errno));
            return -1;
        }
        iret = ioctl(fd, IOCTL_PIO_CLEAN_CACHE, PioBuf);
        if (iret < 0) {
            fprintf(stderr, "IOCTL_PIO_CLEAN_CACHE failed:%d\n", iret);
            exit(-1);
        }
        close(fd);
    }

    return uret;
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
    INT32 fd;
    INT32 iret;
    UINT32 uret = PIO_ERR_NONE;

    if (PioBuf == NULL) {
        uret = PIO_ERR_EINVAL;
    } else {
        if ((fd = open(PIO_DEVNAME, O_RDWR)) == -1) {
            fprintf(stderr, "open:%s\n", strerror(errno));
            return -1;
        }
        iret = ioctl(fd, IOCTL_PIO_INV_CACHE, PioBuf);
        if (iret < 0) {
            fprintf(stderr, "IOCTL_PIO_INV_CACHE failed:%d\n", iret);
            exit(-1);
        }
        close(fd);
    }

    return uret;
}
