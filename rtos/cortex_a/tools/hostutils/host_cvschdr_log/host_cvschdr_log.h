/*
* Copyright (c) 2019 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef HOST_CVSCHDR_LOG
#define HOST_CVSCHDR_LOG

typedef unsigned int            uint32_t;
typedef int                     int32_t;
typedef unsigned short          uint16_t;
typedef short                   int16_t;
typedef unsigned char           uint8_t;
typedef signed char             int8_t;
typedef float               	FLOAT;
typedef double              	DOUBLE;

/*
 * define the data structure of a cvtask/scheduler log entry
 */
typedef struct /* cvlog_printf_flags_s */
{
  uint32_t  index       :16;
  uint32_t  unused      : 4;
  uint32_t  dbg_level   : 4;
  uint32_t  src_thread  : 4;
  uint32_t  src_id      : 4;
} cvlog_printf_flags_t;

typedef struct /* cvlog_printf_entry_s */
{
  union
  {
    cvlog_printf_flags_t  entry_flags;
    uint32_t              entry_flags_dword;
  };
  uint32_t  entry_time;
  uint32_t  entry_uuid;
  uint32_t  entry_string_offset;
  uint32_t  entry_arg1;
  uint32_t  entry_arg2;
  uint32_t  entry_arg3;
  uint32_t  entry_arg4;
  uint32_t  entry_arg5;
} cvlog_sched_entry_t, cvlog_cvtask_entry_t;

#endif /* !HOST_CVSCHDR_LOG */
