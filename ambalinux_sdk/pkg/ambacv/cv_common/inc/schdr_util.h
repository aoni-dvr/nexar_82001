/**
 *  @file schdr_util.h
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
 *  @details Definitions & Constants for cv-scheduler utility functions
 *
 */

#ifndef SCHDR_UTIL_H_FILE
#define SCHDR_UTIL_H_FILE

#include "ambint.h"

#define SCHDR_VP_BIN        0U
#define SCHDR_SOD_BIN       1U

#define SCHDR_VISORC_CRC    0x637263U

typedef uint32_t (*schdr_load_cb)(const char *file_name, void* ptr, uint32_t size, uint32_t *real_size);

typedef struct {
    uint32_t magic1;
    uint32_t crc32;
    uint32_t size;
    uint32_t magic2;
} SCHDR_VISORC_POST_HEADER_s;

/*
 * Load VISORC binary by user with filename
 */
int32_t schdr_load_binary_ext(const char *file_name, uint32_t bin_type, schdr_load_cb load_cb);

/*
 * Load VISORC binary orcvp.bin and orcsod.bin located in @path directory
 */
int32_t schdr_load_binary(const char *path);

/*
 * Check visorc from memory
 */
uint32_t schdr_check_visorc(uint32_t bin_type);

/*
 * Check flexibin binary
 */
uint32_t schdr_check_flexibin(const char *pBuffer, uint32_t buffer_size);

/*
 * Dump visorc log to console or to a file
 *
 * @log_file:   path to the log file; dump to console if set to NULL
 * @mode:       bit-mask to enable the type of log
 *                  0x01: enable orc-sod scheduler log
 *                  0x02: enable orc-sod cvtask log
 *                  0x04: enable orc-vp scheduler log
 *                  0x08: enable orc-vo cvtask log
 * @num_lines:  number of lines to be printed
 *
 * This function can be called during or after a superdag execution. It keeps
 * logging util no new logging entry is generated by the superdag.
 */
int32_t schdr_visorc_dmsg(uint32_t mode, const char *log_file, int32_t num_lines);

/*
 * Allow direct dram/register access from user-space for debugging purpose
 *
 * @cmd: multi-line debug command buffer, one line for each commands.
 * Command syntax:
 *              # comments and empty lines are skipped
 *              # read a register value
 *              r 0xed030044
 *              # read a memory chunk of length 0x100 to a file
 *              r 0x40000000 0x100 memory.bin
 *              # write 0xbabeface to a register
 *              w 0xed030044 0xbabeface
 *
 */
int32_t schdr_debug_cmd(const char *cmd);

/*
 * Prefetch all code from External Storage to DRAM
 * returns -1 if failed.
 *
 */
int32_t schdr_prefetch(int32_t reserved);

/*
 * Get the memory area (cvshm) that is allocated by cv-scheduler to a superdag
 * during its exeection. All input/outpu/message/tbar buffers passed to superdag
 * are within this area.
 *
 * @base: page-aligned starting dram physical address of cvshm
 * @size: size of cvshm
 *
 * This function allows any Linux process to mmap and access the cvshm memory,
 * therefore enables zero-copying between superdag process and any other process.
 * To share a memory within cvshm pointed by pFoo in superdag process, we should
 * first get its offset from cvshm base by:
 *              offset = cvtask_v2off(pFoo)
 * This offset is then sent to the other process by any standard Linux method.
 * On the receiving process, we should do a one-time setup:
 *              schdr_get_cvshm_info(&base, &size);
 *              fd = open("/dev/ambacv", O_SYNC | O_RDONLY);
 *              pBase = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, base);
 * then it can access the same data via pointer (pBase+offset).
 */
int32_t schdr_get_cvshm_info(uint64_t *base, uint64_t *size);

/*
 * Set up ROI for pyramid and lane detection
 */
#ifndef MAX_PYRAMID_SCALES
#define MAX_PYRAMID_SCALES      13
#endif

typedef struct {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} roi_rect_t;

/*
 * structure to set up ROIs for one fov
 */
typedef struct {
    uint32_t         roi_tag;

    /* target fov for this setup */
    uint8_t          fov_id;

    /* index of pyramid scale(0 ~12) serving as input to lane-det scale */
    uint8_t          ld_input_scale;

    uint16_t         padding0;

    /* roi rect for each scale */
    /* set w = h = 0 to disable the scale */
    roi_rect_t       half_octave[MAX_PYRAMID_SCALES];
    roi_rect_t       lane_det;
} roi_setup_t;

/*
 * @count: number of roi_setup messages to be sent, one for each FOV.
 * @setup: pointer to the array of roi_setup messages
 */
int32_t schdr_setup_rois(int32_t count, roi_setup_t *setup);

#endif /* !SCHDR_UTIL_H_FILE */

