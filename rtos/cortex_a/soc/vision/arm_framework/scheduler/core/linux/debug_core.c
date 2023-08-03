/**
 *  @file debug_core.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Core APIs for debug
 *
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "os_api.h"
#include "schdr_util.h"

static int32_t              amba_fd;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void *amba_map(uint32_t addr, uint32_t size,
                      void **mbase, uint32_t *msize)
{
    uint32_t aligned = addr & ~0xFFF;

    pthread_mutex_lock(&lock);
    if (amba_fd == 0) {
        amba_fd = open("/dev/ambacv", O_DSYNC | O_RDWR );
        if (amba_fd < 0) {
            printf("Can't open device file /dev/ambacv !!!\n");
            exit(-1);
        } /* if (amba_fd < 0) : open("/dev/ambacv", ...) */
    } /* if (amba_fd == 0)  */
    pthread_mutex_unlock(&lock);

    *msize = size + (addr - aligned);
#if defined (CHIP_CV5) || defined(CHIP_CV52)
    *mbase = mmap(NULL, *msize, PROT_READ | PROT_WRITE, MAP_SHARED,
                  amba_fd, (aligned + 0x2000000000UL));
#elif (defined(CHIP_CV6))
    *mbase = mmap(NULL, *msize, PROT_READ | PROT_WRITE, MAP_SHARED,
                  amba_fd, (aligned + 0xFF00000000UL));
#elif (defined(CHIP_CV2) || defined(CHIP_CV22) || defined(CHIP_CV25) || defined(CHIP_CV28) || defined(CHIP_CV2A) || defined(CHIP_CV2FS) || defined(CHIP_CV22A) || defined(CHIP_CV22FS))
    *mbase = mmap(NULL, *msize, PROT_READ | PROT_WRITE, MAP_SHARED,
                  amba_fd, aligned);
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif
    if (*mbase == MAP_FAILED) {
        printf("Can't mmap cv shared memory !!!\n");
    }
    return (*mbase + (addr - aligned));
}

static void amba_unmap(void *base, uint32_t size)
{
    munmap(base, size);
}

static void run_read(char *cmd)
{
    uint32_t addr, size, pos, sol, eol;
    uint32_t *ptr, *mbase, msize;
    unsigned char *cptr, c;

    addr = strtoul(cmd, &cmd, 0);
    size = strtoul(cmd, &cmd, 0);
    if (size == 0) size = 4;
    size = (size+3) & ~3;
    cptr = amba_map(addr, size, (void**)&mbase, &msize);
    ptr = (uint32_t*)cptr;

    while (isspace(*cmd)) {
        cmd++;
    }

    // read into a file in binary format
    if (cmd[0] != 0) {
        int32_t fd = open(cmd, O_WRONLY | O_CREAT | O_TRUNC, 0664);
#if 0
        uint32_t val;
        for (pos = 0, end = size; pos < end; pos += 4) {
            val = *ptr++;
            write(fd, &val, 4);
        }
#endif
        write(fd, ptr, size);
        close(fd);
        goto exit;
    }

    // simple output for small length
    if (size < 16) {
        for (pos = 0; pos < size; pos +=4)
            printf("%08X ", *ptr++);
        printf("\n");
        goto exit;
    }

    sol  = addr & 0x0F;
    eol  = 16;
    size += sol;
    addr &= ~0xF;
    pos = 0;


    // printout first line
    printf("%08X: ", addr + pos);
    for (; pos < sol; pos += 4) {
        printf("         ");
    }
    for (; pos < 16; pos +=4) {
        printf("%08X ", *ptr++);
    }
    for (pos = 0; pos < sol; pos++) {
        printf(" ");
    }
    for (; pos < 16; pos++) {
        c = *cptr++;
        printf("%c", isprint(c) ? c : '.');
    }
    sol = 16;
    printf("\n");

    // printout middle section
    while (1) {
        if (pos + 16 > size)
            break;
        printf("%08X: ", addr + sol);
        eol = sol + 16;
        for (pos = sol; pos < eol; pos += 4) {
            printf("%08X ", *ptr++);
        }
        for (pos = sol; pos < eol; pos++) {
            c = *cptr++;
            printf("%c", isprint(c) ? c : '.');
        }
        sol = eol;
        printf("\n");
    }

    // printout last line
    eol = (size + 0xF) & ~0xF;
    if (sol != eol) {
        printf("%08X: ", addr + pos);
        for (; pos < size; pos += 4) {
            printf("%08X ", *ptr++);
        }
        for (; pos < eol; pos += 4) {
            printf("         ");
        }
        for (pos = sol; pos < size; pos++) {
            c = *cptr++;
            printf("%c", isprint(c) ? c : '.');
        }
        printf("\n");
    }

exit:
    amba_unmap(mbase, msize);
}

static void run_write(char *cmd)
{
    uint32_t addr, valu, msize;
    uint32_t *ptr, *mbase;

    addr = strtoul(cmd, &cmd, 0);
    ptr  = amba_map(addr, 4, (void**)&mbase, &msize);
    valu = strtoul(cmd, &cmd, 0);
    *ptr = valu;
    printf("Writing %08X to %08X\n", valu, addr);
    amba_unmap(mbase, msize);
}

static void run_cmd(char *cmd)
{
    while (isspace(*cmd))
        cmd++;

    printf("cmd is [%s]\n", cmd);
    switch (cmd[0]) {
    case 'r':
        run_read(cmd+2);
        break;
    case 'w':
        run_write(cmd+2);
        break;
    case '#':
    case '\r':
    case '\n':
    case '\0':
        break;
    default:
        printf("skip command line [%s]\n", cmd);
    }
}

int32_t schdr_debug_cmd(const char *buf)
{
    char c, cmd[512];
    int32_t start, pos, len, total;

    total = strlen(buf);
    start = pos = 0;

    while (start < total) {
        // skip current char until we found line-end
        c = buf[pos++];
        if (c != '\r' && c != '\n' && pos < total)
            continue;

        // run the cmd in the current line
        len = (pos == total) ? (total - start) : (pos - start - 1);
        if (len != 0) {
            (void) thread_strncpy(cmd, buf+start, len);
            cmd[len] = 0;
            run_cmd(cmd);
        }
        start = pos;
    }
    return 0;
}
