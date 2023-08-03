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

#include <libromfs.h>

static char g_device_node[32];  /* /dev/mtd3, /dev/mmcblk0p3 */
static int g_fd;

static void AmbaNAND_FindGoodBlkForward(void)
{
	const char MTD[] = "/dev/mtd";
	struct mtd_info_user meminfo;
	struct mtd_ecc_stats mtd_stat;
	loff_t test_ofs;

	if (0 != strncmp(g_device_node, MTD, strlen(MTD))) {
		return;
	}

	if (ioctl(g_fd, MEMGETINFO, &meminfo) != 0) {
		perror("MEMGETINFO");
		return;
	}
	syslog(LOG_NOTICE, "ioctl(%s, MEMGETINFO)\n", g_device_node);
	syslog(LOG_NOTICE, " type: %d\n", meminfo.type);
	syslog(LOG_NOTICE, " flags: %d\n", meminfo.flags);
	syslog(LOG_NOTICE, " size: %d\n", meminfo.size);
	syslog(LOG_NOTICE, " erasesize: %d\n", meminfo.erasesize);
	syslog(LOG_NOTICE, " writesize: %d\n", meminfo.writesize);
	syslog(LOG_NOTICE, " oobsize: %d\n", meminfo.oobsize);
	if (meminfo.type != MTD_NANDFLASH) {
		syslog(LOG_NOTICE, "skip meminfo.type=%d\n", meminfo.type);
		return;
	}

	if (ioctl(g_fd, ECCGETSTATS, &mtd_stat) != 0) {
		perror("ECCGETSTATS");
		return;
	}
	syslog(LOG_NOTICE, "ioctl(%s, ECCGETSTATS)\n", g_device_node);
	syslog(LOG_NOTICE, " ECC failed: %d\n", mtd_stat.failed);
	syslog(LOG_NOTICE, " ECC corrected: %d\n", mtd_stat.corrected);
	syslog(LOG_NOTICE, " Number of bad blocks: %d\n", mtd_stat.badblocks);
	syslog(LOG_NOTICE, " Number of bbt blocks: %d\n", mtd_stat.bbtblocks);
	if (mtd_stat.bbtblocks == 0) {
		syslog(LOG_NOTICE, "no bad block found\n");
		return;
	}

	for (test_ofs = 0; test_ofs < meminfo.size; test_ofs += meminfo.erasesize) {
		if (ioctl(g_fd, MEMGETBADBLOCK, &test_ofs)) {
			syslog(LOG_NOTICE, "Bad block at 0x%08x\n", (unsigned)test_ofs);
			continue;
		}
		if (test_ofs == 0) {
			return;
		}

		syslog(LOG_NOTICE, "lseek to block at 0x%08x\n", (unsigned)test_ofs);
		if ((off_t)test_ofs !=
		    lseek(g_fd, (off_t)test_ofs, SEEK_SET)) {
			perror("lseek");
			exit(1);
		}
	}
}

/* Global */
AMBA_NVM_ROM_SYS_DATA_CTRL_s RomSysDataCtrl;

/* API: use this to set device node, e.g., /dev/mtd3, /dev/mmcblk0p3 */
void set_romfs_device_node(char *device_node)
{
	strcpy(g_device_node, device_node);
}

/* API: use this to get device node, e.g., /dev/mtd3, /dev/mmcblk0p3 */
char *get_romfs_device_node(void)
{
	return g_device_node;
}

/*API: */
uint32_t Amba_LoadNvmRomFileTable(char *device_node)
{
	strcpy(g_device_node, device_node);
	if ((g_fd = open(g_device_node, O_RDONLY)) < 0) {
		perror("open");
		return 1;
	}

	AmbaNAND_FindGoodBlkForward();

	if (sizeof(AMBA_NVM_ROM_SYS_DATA_CTRL_s) !=
	    read(g_fd, (void *)&RomSysDataCtrl, sizeof(AMBA_NVM_ROM_SYS_DATA_CTRL_s))) {
		perror("read");
		return 1;
	}
	syslog(LOG_NOTICE, "ROMFS FileCount 0x%x\n", RomSysDataCtrl.Header.FileCount);
	close(g_fd);
	return 0;
}
