/*
Copyright (c) 2020 by Ambarella Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <mqueue.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <sys/timerfd.h>
#include <netdb.h>
#include <assert.h>
#include <stdint.h>
#include <linux/types.h>
#include <syslog.h>

#define AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE  (512U)  /* maximum number of ROM files for System Data */
#define SYSDATA_CTRL_PAD_SIZE sizeof(AMBA_NVM_ROM_SYS_DATA_HEADER_s) % 512U

typedef struct {
	uint8_t   FileName[64];                       /* ROM file name */
	uint32_t  ByteSize;                           /* ROM file size in Bytes */
	uint32_t  FileOffset;                         /* file offset */
	uint32_t  FileCRC32;                          /* CRC32 of the File */
} AMBA_NVM_ROM_FILE_ENTRY_s;

typedef struct {
	uint32_t  Version;                            /* Version of the ROM File Table */
	uint32_t  FileCount;                          /* number of ROM files in the Partition  */
	AMBA_NVM_ROM_FILE_ENTRY_s  FileEntry[AMBA_NVM_MAX_NUM_SYS_DATA_ROM_FILE];
} AMBA_NVM_ROM_SYS_DATA_HEADER_s;

typedef struct {
	AMBA_NVM_ROM_SYS_DATA_HEADER_s Header;
	uint8_t Reserved[512U-SYSDATA_CTRL_PAD_SIZE];
} AMBA_NVM_ROM_SYS_DATA_CTRL_s;

#define MEMGETBADBLOCK		_IOW('M', 11, __kernel_loff_t)
#define ECCGETSTATS             _IOR('M', 18, struct mtd_ecc_stats)
#define MEMGETINFO		_IOR('M', 1, struct mtd_info_user)

#define MTD_NANDFLASH		4

struct mtd_ecc_stats {
	uint32_t corrected;
	uint32_t failed;
	uint32_t badblocks;
	uint32_t bbtblocks;
};

struct mtd_info_user {
	uint8_t type;
	uint32_t flags;
	uint32_t size;	/* Total size of the MTD */
	uint32_t erasesize;
	uint32_t writesize;
	uint32_t oobsize;	/* Amount of OOB data per block (e.g. 16) */
	uint64_t padding;	/* Old obsolete field; do not use */
};

extern AMBA_NVM_ROM_SYS_DATA_CTRL_s RomSysDataCtrl;
uint32_t Amba_LoadNvmRomFileTable(char *device_node);
