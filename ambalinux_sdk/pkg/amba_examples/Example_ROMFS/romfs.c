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
#include <sys/mman.h>

void save(const char *dst_file, void *buffer, const unsigned long length)
{
	int fd, n;

	if ((fd = open(dst_file, O_RDWR | O_CREAT | O_TRUNC, 777)) == -1) {
		perror("open");
		exit(1);
	}
	n = write(fd, buffer, length);
	printf("saved first %d bytes to ./%s\n", n, dst_file);

	close(fd);
}

int main(int argc, char **argv)
{
	struct stat sb;
	int i, n;
	void *buffer;
	int fd;

	if ((argc != 2) ||
           ((argc == 2) && (stat(argv[1], &sb) != 0))) {
		printf("usage:\n");
		printf("  dump romfs in /dev/mtd3\n");
		printf("    cd /tmp/; romfs /dev/mtd3\n");
		printf("  dump romfs in /dev/mmcblk0p3\n");
		printf("    cd /tmp/; romfs /dev/mmcblk0p3\n");
		return 1;
	}

	Amba_LoadNvmRomFileTable(argv[1]);
	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		perror("open");
		return 1;
	}

	if (0xffffffff == RomSysDataCtrl.Header.FileCount) {
		printf("not romfs");
		return 1;
	}

	for (i = 0; i < RomSysDataCtrl.Header.FileCount ; i++) {
		printf("FileName[%d] %s\n", i, RomSysDataCtrl.Header.FileEntry[i].FileName);
		printf("FileOffset[%d] %d\n", i, RomSysDataCtrl.Header.FileEntry[i].FileOffset);
		printf("ByteSize[%d] %d\n", i, RomSysDataCtrl.Header.FileEntry[i].ByteSize);

		if (RomSysDataCtrl.Header.FileEntry[i].ByteSize == 0 ||
		    RomSysDataCtrl.Header.FileEntry[i].FileName[0] == '\0') {
			continue;
		}

		if ((off_t)RomSysDataCtrl.Header.FileEntry[i].FileOffset !=
		    lseek(fd, (off_t)RomSysDataCtrl.Header.FileEntry[i].FileOffset, SEEK_SET)) {
			perror("lseek");
			return 1;
		}

		if ((buffer = malloc(RomSysDataCtrl.Header.FileEntry[i].ByteSize)) == NULL) {
			perror("malloc");
			return 1;
		}

		n = read(fd, buffer, RomSysDataCtrl.Header.FileEntry[i].ByteSize);

		save((char *)RomSysDataCtrl.Header.FileEntry[i].FileName, buffer, n);
		free(buffer);
	}
}
