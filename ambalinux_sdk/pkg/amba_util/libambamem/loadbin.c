/*
 * Author:
 *	Michael Yao<ccyao@ambarella.com>
 * MIT License
 * Copyright (c) 2017 by Ambarella Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* USAGE
1. load file /etc/init.d/rcS to memory addr 0x3585000
   / # loadbin /etc/init.d/rcS 0x3585000
   file size 530
   /dev/ppm 0x3585000 mapped to 0x7f8d434000
   /etc/init.d/rcS mapped to 0x7f8d433000
   memcpy(0x7f8d434000, 0x7f8d433000, 530);
2. save memory addr 0x3585000 length 0x10000 to file /tmp/save.bin
   / # savebin /tmp/save.bin 0x3585000 0x10000
   file length 65536
   /tmp/save.bin mapped to 0x7faec3d000
   /dev/ppm 0x3585000 mapped to 0x7faec2d000
   memcpy(0x7faec3d000, 0x7faec2d000, 65536);
3. read memory addr 0x3585000 length 4 bytes
   / # readl 0x3585000
   / # readl 0x3585000 4
   0x03585000: 0x00000009
4. writel memory addr 0x3585000 value 0x4
   / # writel 0x3585000 0x4
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "libambamem.h"

int main (int argc, char *argv[])
{
	if (NULL != strstr(argv[0], "loadbin")) {
		if (argc < 3) {
			printf("loadbin USAGE: load file /etc/init.d/rcS to memory addr 0x3585000\n"
				"\tloadbin /etc/init.d/rcS 0x3585000\n");

			return -1;
		}
	}
	else if (NULL != strstr(argv[0], "savebin")) {
		if (argc < 4) {
			printf("savebin USAGE: save memory addr 0x3585000 length 0x10000 to file /tmp/save.bin\n"
				"\tsavebin /tmp/save.bin 0x3585000 0x10000\n");

			return -1;
		}
	}
	else if (NULL != strstr(argv[0], "readl")) {
		if (argc < 2) {
			printf("readl USAGE: readl memory addr 0xe0002028 length 4 bytes\n"
				"\treadl 0xe0002028\n"
				"\treadl 0xe0002028 4\n");

			return -1;
		}
	}
	else if (NULL != strstr(argv[0], "writel")) {
		if (argc < 3) {
			printf("writel USAGE: writel memory addr 0xe0002028 value 0x4\n"
				"\twritel 0xe0002028 0x4\n");

			return -1;
		}
	}

	if (NULL != strstr(argv[0], "loadbin")) {
		return loadbin(argv[1], strtoul(argv[2], NULL, 16));
	} else if (NULL != strstr(argv[0], "savebin")) {
		return savebin(argv[1], strtoul(argv[2], NULL, 16), strtoul(argv[3], NULL, 16));
	} else if (NULL != strstr(argv[0], "readl")) {
		return readl(strtoul(argv[1], NULL, 16), (argc > 2) ? atol(argv[2]) : 4);
	} else if (NULL != strstr(argv[0], "writel")) {
		return writel(strtoul(argv[1], NULL, 16), strtoul(argv[2], NULL, 16));
	}
}
