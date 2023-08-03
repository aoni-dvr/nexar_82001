/**
*  @file avf.h
*
* Copyright (c) [2020] Ambarella International LP
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
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*
*  @detail xxxx
*
*/
#ifndef AVF_H
#define AVF_H

#include "DaemonIO.h"

#define VERSION_OF_AVF    "V3.7.0"
#define MAX_PATH_LEN      (256U)
#define AVF_MAX_IO        (8U)
#define AVF_PIC_INFO_LEN  (8U)

typedef struct _AVF_CONFIG {
  uint32_t start_daemon;  //  Create a daemon for CMA memory management
  uint32_t run_client;    //  Request daemon to run flexidag
  uint32_t stop_daemon;   //  Stop AVF daemon
  uint32_t debug_daemon;
  uint32_t run_client_nonblock;
  uint32_t test_yield;
} AVF_CONFIG_s;

typedef struct _AVF_PIC_INFO {
  uint32_t pyramid_scale;
  uint32_t roi_pitch;
  uint32_t roi_width;
  uint32_t roi_height;
  uint32_t offset_x;
  uint32_t offset_y;
  uint32_t data_width;
  uint32_t data_height;
} AVF_PIC_INFO_s;

typedef struct _AVF_RUN_INFO {
  uint32_t  slot;
  uint32_t  batch_num;
  char      working_dir[MAX_PATH_LEN];
  uint32_t  pic_info_en;

  /* Input */
  uint32_t       in_num;
  uint32_t       in_pitch_counter;
  uint32_t       in_pitch[AVF_MAX_IO];
  char           in_file_name[AVF_MAX_IO][MAX_PATH_LEN];
  char           in_folder_path[MAX_PATH_LEN];
  uint32_t       in_pic_info_counter;
  AVF_PIC_INFO_s in_pic_info[AVF_MAX_IO];

  /* Output */
  uint32_t  out_num;
  char      out_file_name[AVF_MAX_IO][MAX_PATH_LEN];
  uint32_t  out_buf_depth;

  /* Debug */
  int32_t   iteration_num;
  uint32_t  log_flag;  // 1. Dump cvtask log 2. Show average processing time
} AVF_RUN_INFO_s;

extern AVF_CONFIG_s g_avf_config;

/*---------------------------------------------------------------------------*\
 * AVF Daemon APIs
\*---------------------------------------------------------------------------*/
int32_t avf_init();
int32_t avf_check_slot_usage();
int32_t avf_run_multi(uint32_t slot);
int32_t avf_compare_yield_out(const char *w_dir, uint32_t out_num);
int32_t avf_reset_run_info(uint32_t slot);
int32_t avf_unpack_run_info(AVF_RUN_INFO_s *p_runInfo);
int32_t avf_dump_run_info(uint32_t slot);

/*---------------------------------------------------------------------------*\
 * AVF Client APIs
\*---------------------------------------------------------------------------*/
void avf_parse_opt(int32_t argc, char **argv);
int32_t avf_pack_run_info(AVF_RUN_INFO_s *p_runInfo);
int32_t avf_pack_yield_run_info(AVF_RUN_INFO_s *p_runInfo, uint32_t fd_index, int32_t repeated_num, uint32_t rm_out);
int32_t avf_backup_yield_output(uint32_t fd_index);
int32_t avf_get_dag_split_count(uint32_t fd_index);
#endif
