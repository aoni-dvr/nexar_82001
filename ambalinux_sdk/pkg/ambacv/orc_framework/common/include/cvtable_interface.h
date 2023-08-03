/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVTABLE_INTERFACE_H_FILE
#define CVTABLE_INTERFACE_H_FILE

#include <dsp_types.h>
#include <cvtask_interface.h>

#define CVTABLE_VERSION         0x00000002

typedef struct /* cvtable_header_s */
{
  uint32_t  table_version;
  uint32_t  toc_num_entries;
  uint32_t  toc_entry_size;
  uint32_t  toc_offset;
  uint32_t  total_size;         /*including header*/
} cvtable_header_t;

/* Used for version 0/1 */
typedef struct /* cvtable_toc_entry_v1_s */
{
  uint8_t   cvtask_table_name[32];
  uint32_t  table_offset;
  uint32_t  table_size;
} cvtable_toc_entry_v1_t;

/* Used for version 2 */
typedef struct /* cvtable_toc_entry_v2_s */
{
  uint8_t   cvtask_table_name[120];
  uint32_t  table_offset;
  uint32_t  table_size;
} cvtable_toc_entry_v2_t;

#endif /* ?CVTABLE_INTERFACE_H_FILE */
