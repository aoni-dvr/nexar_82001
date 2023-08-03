/**
*  @file avf.c
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

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <getopt.h>
#include <ftw.h>
#include <errno.h>
#include <ctype.h>

#include "CvCommFlexi.h"
#include "avf.h"
#include "avf_daemon.h"

#define ARM_LOG_AVF             "AVF"

/* AVF control state */
#define STATE_FINISH            (1)

/* Flexidag iteration control state */
#define RUN_STATE_REPEAT        (0)
#define RUN_STATE_DONE          (1)
#define RUN_STATE_SAVE          (2)

/* Settings */
#define MAX_AVF_MSG_QUEUE_NUM   (8)
#define MAX_IMAGE_NUM           (1000)
#define MAX_IMAGE_GROUP         (1000)
#define MAX_MEMPOOL_DEPTH       (8)
#define MAX_AVF_SLOT            (2)
#define MAX_STRING_LEN          (16)
#define MAX_PROFILE_GROUP       (10)
#define MAX_AVG_PROC_TIME_NUM   (10)
#define MAX_YIELD_TEST_NUM      (2)
#define MAX_OUT_BUF_DEPTH       (4)
#define KEEP_ALIVE_TIME         (5 * 60)
#define ENABLE_CVTASK_LOG       (1)
#define ENABLE_PROC_TIME_LOG    (2)

/* Macro */
#define ALIGN4K(x)              (((x) + 4095U) & 0xFFFFF000U)
#define GETTIMESTAMP(s, e)      ((e.tv_usec - s.tv_usec) + (e.tv_sec - s.tv_sec)*1000000)

/* Pic-info mode */
#define MAX_YUV_SIZE            (0x870000U)  // 8MB for 4096 x 2160

/*---------------------------------------------------------------------------*\
 * Resource & Definition
\*---------------------------------------------------------------------------*/
typedef struct {
  flexidag_memblk_t    yuv_data_buf[AVF_MAX_IO];
} AVF_INNER_BUF_CTRL_s;

typedef struct {
  flexidag_memblk_t    raw_data_buf[AVF_MAX_IO];
  uint32_t             raw_size[AVF_MAX_IO];
  char                 folder_name[MAX_PATH_LEN];
  uint32_t             stop_run_flag;
  uint32_t             seq_num;
  AVF_INNER_BUF_CTRL_s inner_buf_ctrl;
} AVF_MQ_ITEM_s;

typedef struct {
  int32_t           result;           // 0: OK, -1: NG
} AVF_ACKQ_ITEM_s;

typedef struct {
  ArmMutex_t        mutex;
  pthread_t         thread_handler;
  uint32_t          timeout_value;   // seconds
  uint32_t volatile keepalive_flag;  // variable may be changed at any time
} AVF_WATCHDOG_s;

typedef struct {
  pthread_t      read_thread;
  pthread_t      feed_thread;

  /* Read -> Feed */
  ArmMsgQueue_t  msg_q;
  char           msg_q_name[MAX_STRING_LEN];
  AVF_MQ_ITEM_s  msg_q_buf[MAX_AVF_MSG_QUEUE_NUM];

  /* Read <- Feed */
  ArmMsgQueue_t  ack_q;
  char           ack_q_name[MAX_STRING_LEN];
  AVF_MQ_ITEM_s  ack_q_buf[MAX_AVF_MSG_QUEUE_NUM];
  uint32_t       enabled_ack;

  /* Eventflag */
  ArmEventFlag_t event_flag;
  char           event_flag_name[MAX_STRING_LEN];

  /* Profile data */
  int32_t        group_num;
  int32_t        image_num_in_group[MAX_PROFILE_GROUP];
  struct timeval tv_start[MAX_PROFILE_GROUP + 1];  // tv_start[0:9]: Each group time ,tv_start[10]: Total time
  struct timeval tv_end[MAX_PROFILE_GROUP + 1];

  /* Watch dog */
  AVF_WATCHDOG_s wd;

  /* Golden compare */
  uint8_t        golden_cmp_success;
} AVF_RUN_CTRL_s;

typedef struct _AVF_SLOT_CTRL {
  uint32_t  aval_index;  // Bit0: Slot-0, Bit1: Slot-1 */
} AVF_SLOT_CTRL_s;

typedef struct {
  flexidag_memblk_t  pool_buf[AVF_MAX_IO];  // Allocated from pool-0
  uint32_t           pool_id[AVF_MAX_IO];
  int32_t            file_size[AVF_MAX_IO];
  uint32_t           mem_allocated_flag;
  flexidag_memblk_t  inner_pool_buf;
  uint32_t           inner_pool_id;
  uint8_t            inner_pool_used;
} AVF_INPUT_INFO_s;

typedef struct {
  char      working_dir[MAX_YIELD_TEST_NUM][MAX_PATH_LEN];
  uint32_t  test_num;
} AVF_YIELD_s;

AVF_CONFIG_s g_avf_config = {0};

static AVF_SLOT_CTRL_s  g_slot = {0};
static AVF_INPUT_INFO_s g_daemon_input_info[MAX_AVF_SLOT];
static AVF_RUN_CTRL_s   g_daemon_run_ctrl[MAX_AVF_SLOT];
static AVF_RUN_INFO_s   g_daemon_run_info[MAX_AVF_SLOT];
static AVF_RUN_INFO_s   g_client_run_info = {0};
static AVF_YIELD_s      g_yield = {0};

/*---------------------------------------------------------------------------*\
 * Command line options
\*---------------------------------------------------------------------------*/
static struct option long_options[] =
{
  {"input",    1, NULL, 'i'},
  {"output",   1, NULL, 'o'},
  {"repeat",   1, NULL, 'r'},
  {"loop",     1, NULL, 'l'},
  {"debug",    1, NULL, 'd'},
  {"path",     1, NULL, 'p'},
  {"pitch",    1, NULL, 't'},
  {"batch",    1, NULL, 'b'},
  {"client",   1, NULL, 'c'},
  {"picinfo",  1, NULL, 'f'},
  {"nbclient", 1, NULL, 'n'},
  {"yield",    1, NULL, 'y'},
  {"depth",    1, NULL, 'e'},
  {"start",    0, NULL, 's'},
  {"stop",     0, NULL, 'x'},
  {"help",     0, NULL, 'h'},
  {"version",  0, NULL, 'v'},
  {0, 0, 0, 0},
};

static const char *short_options = "i:o:r:l:d:p:t:b:c:f:n:y:e:sxhv";  // ':' means one argument

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_print_help
 *
 *  @Description:: Print usage argument of avf
 *
 *  @Input      ::
 *      exe:       Program name
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void avf_print_help(const char *exe)
{
  ArmLog_DBG(ARM_LOG_AVF, "VERSION: %s", VERSION_OF_AVF);
  ArmLog_DBG(ARM_LOG_AVF, "usage: %s [args]", exe);
  ArmLog_DBG(ARM_LOG_AVF, "\t-i [Input File Name] Max to %d input", AVF_MAX_IO);
  ArmLog_DBG(ARM_LOG_AVF, "\t-o [Output File Name] Max to %d output", AVF_MAX_IO);
  ArmLog_DBG(ARM_LOG_AVF, "\t-r [The number of repeated execution]");
  ArmLog_DBG(ARM_LOG_AVF, "\t-d [Dump CVTASK log Flag] >= 1: Enabled 0: Disabled");
  ArmLog_DBG(ARM_LOG_AVF, "\t-p [Input Folder Path]");
  ArmLog_DBG(ARM_LOG_AVF, "\t-t [Input Pitch]");
  ArmLog_DBG(ARM_LOG_AVF, "\t-b [Input Batch]");
  ArmLog_DBG(ARM_LOG_AVF, "\t--picinfo [Input Pic Info]");
  ArmLog_DBG(ARM_LOG_AVF, "\t\t\'[pyramid scale][roi pitch][roi width][roi height][offset x][offset y][data width][data height]\'");
  ArmLog_DBG(ARM_LOG_AVF, "\t--client [Run Client]      Working directory");
  ArmLog_DBG(ARM_LOG_AVF, "\t--start  [Enable Daemon]   No argument");
  ArmLog_DBG(ARM_LOG_AVF, "\t--stop   [Shutdown Daemon] No argument");
  ArmLog_DBG(ARM_LOG_AVF, "for example:");
  ArmLog_DBG(ARM_LOG_AVF, "            1. %s %s", exe, "--start");
  ArmLog_DBG(ARM_LOG_AVF, "            2. %s %s", exe, "--client $working_dir -i IN1.bin -i IN2.bin -o OUT1.out -o OUT2.out\n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_string_parser
 *
 *  @Description:: Compare two binary buffers
 *
 *  @Input      ::
 *      input:     Input string
 *      count:     Output array count
 *
 *  @Output     ::
 *      output:    Output array
 *
 *  @Return     ::
 *     uint32_t    Input word length
\*-----------------------------------------------------------------------------------------------*/
static uint32_t avf_string_parser(const char* input, uint32_t count, uint32_t output[])
{
    uint32_t n = 0;
    const char *p = input;

    while (*p) {
      while (isspace((unsigned char) *p)) ++p;
      n += *p != '\0';
      while (*p && !isspace((unsigned char) *p)) ++p;
    }

    if (n != count)
      return n;

    if (n) {
      uint32_t i = 0;
      p = input;

      while (*p) {
        while (isspace((unsigned char) *p)) ++p;
        if (*p) {
          const char *q = p;
          while (*p && !isspace((unsigned char)*p)) ++p;
          size_t length = p - q;
          char buf[20];  // 20 characters should be enough
          strncpy(buf, q, length);
          buf[length] = '\0';
          output[i++] = atoi(buf);
        }
      }
    }

    return count;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_compare_binary
 *
 *  @Description:: Compare two binary buffers
 *
 *  @Input      ::
 *      bin0_ptr:  Pointer to binary buffer 0
 *      bin1_ptr:  Pointer to binary buffer 1
 *      size:      Size want to combared
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     int      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int32_t avf_compare_binary(const char* bin0_ptr, const char* bin1_ptr, const int32_t size)
{
  if (bin0_ptr == NULL || bin1_ptr == NULL) {
    ArmLog_ERR(ARM_LOG_AVF, "Binary buffer can not be NULL!");
    return -1;
  }

  if (size == 0) {
    ArmLog_ERR(ARM_LOG_AVF, "Compared size can not be zero!");
    return -1;
  }

  int32_t idx = size;

  while (idx > 0) {
    if (*bin0_ptr != *bin1_ptr) {
      break;
    }
    --idx;
  }

  return (idx == 0) ? 0 : -1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_parse_opt
 *
 *  @Description:: Parse the arguments
 *
 *  @Input      ::
 *      argc:      Argument count
 *      argv:      Argument array
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void avf_parse_opt(int32_t argc, char **argv)
{
  int32_t c;
  AVF_RUN_INFO_s *pRunInfo = &g_client_run_info;
  AVF_CONFIG_s *pConfig = &g_avf_config;

  optind = 1;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (c) {
      case 'i':
        if (pRunInfo->in_num < AVF_MAX_IO) {
          strcpy(pRunInfo->in_file_name[pRunInfo->in_num], optarg);
          ArmLog_DBG(ARM_LOG_AVF, "Input file name[%d]:  %s", pRunInfo->in_num, pRunInfo->in_file_name[pRunInfo->in_num]);
        } else {
          ArmLog_ERR(ARM_LOG_AVF, "Exceed the maximum number of input files!!");
          exit(-1);
        }
        pRunInfo->in_num++;
        break;

      case 'o':
        if (pRunInfo->out_num < AVF_MAX_IO) {
          strcpy(pRunInfo->out_file_name[pRunInfo->out_num], optarg);
          ArmLog_DBG(ARM_LOG_AVF, "Output file name[%d]:  %s", pRunInfo->out_num, pRunInfo->out_file_name[pRunInfo->out_num]);
        } else {
          ArmLog_ERR(ARM_LOG_AVF, "Exceed the maximum number of output files!!");
          exit(-1);
        }
        pRunInfo->out_num++;
        break;

      case 'r':
        pRunInfo->iteration_num = strtol(optarg, NULL, 0);
        ArmLog_DBG(ARM_LOG_AVF, "Iteration number: %d", pRunInfo->iteration_num);
        break;

      case 'l':
        ArmLog_DBG(ARM_LOG_AVF, "Loop mode is default on");
        break;

      case 'd':
        pRunInfo->log_flag = strtoul(optarg, NULL, 0);  // For client/normal mode to control cvtask log
        g_avf_config.debug_daemon = pRunInfo->log_flag; // For deamon to dump log on console
        ArmLog_DBG(ARM_LOG_AVF, "Debug flag: %u", pRunInfo->log_flag);
        break;

      case 'p':
        strcpy(pRunInfo->in_folder_path, optarg);
        ArmLog_DBG(ARM_LOG_AVF, "Input path:  %s", pRunInfo->in_folder_path);
        break;

      case 't':
        if (pRunInfo->in_pitch_counter >= AVF_MAX_IO) {
          ArmLog_ERR(ARM_LOG_AVF, "Input-pitch count is larger than the limitation!!!");
          exit(-1);
        } else {
          pRunInfo->in_pitch[pRunInfo->in_pitch_counter] = strtoul(optarg, NULL, 0);
          ArmLog_DBG(ARM_LOG_AVF, "Input-%u pitch:  %u", pRunInfo->in_pitch_counter, pRunInfo->in_pitch[pRunInfo->in_pitch_counter]);
          pRunInfo->in_pitch_counter++;
        }
        break;

      case 'b':
        pRunInfo->batch_num = strtoul(optarg, NULL, 0);
        if (pRunInfo->batch_num > (AVF_MAX_IO / 2)) {
          ArmLog_ERR(ARM_LOG_AVF, "Input-batch needs to be smaller or equal to %d!!!", AVF_MAX_IO / 2);
          exit(-1);
        }
        ArmLog_DBG(ARM_LOG_AVF, "Input batch:  %u", pRunInfo->batch_num);
        break;

      case 'f':
        if (pRunInfo->in_pic_info_counter >= (AVF_MAX_IO / 2)) {
          ArmLog_ERR(ARM_LOG_AVF, "Input-picinfo count is larger than the limitation!!!");
          exit(-1);
        } else {
          char* s = optarg;
          uint32_t info_array[AVF_PIC_INFO_LEN] = {0};
          uint32_t n = avf_string_parser(s, AVF_PIC_INFO_LEN, info_array);
          if (n != AVF_PIC_INFO_LEN) {
            ArmLog_ERR(ARM_LOG_AVF, "Incorrect size for picinfo!!!");
            exit(-1);
          }
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].pyramid_scale = info_array[0];
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].roi_pitch     = info_array[1];
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].roi_width     = info_array[2];
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].roi_height    = info_array[3];
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].offset_x      = info_array[4];
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].offset_y      = info_array[5];
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].data_width    = info_array[6];
          pRunInfo->in_pic_info[pRunInfo->in_pic_info_counter].data_height   = info_array[7];
          pRunInfo->in_pic_info_counter++;
          pRunInfo->pic_info_en = 1;  // enable pit-info mdoe
          break;
        }
      case 's':
        /* Daemon start */
        g_avf_config.start_daemon = 1;
        ArmLog_DBG(ARM_LOG_AVF, "Start daemon...");
        break;

      case 'c':
        g_avf_config.run_client = 1;
        ArmLog_DBG(ARM_LOG_AVF, "Run on client Mode...");
        strcpy(pRunInfo->working_dir, optarg);
        ArmLog_DBG(ARM_LOG_AVF, "Daemon working directory:  %s", pRunInfo->working_dir);
        break;

      case 'n':
        g_avf_config.run_client_nonblock = 1;
        ArmLog_DBG(ARM_LOG_AVF, "Run on client-non-block Mode...");
        strcpy(pRunInfo->working_dir, optarg);
        ArmLog_DBG(ARM_LOG_AVF, "Daemon working directory:  %s", pRunInfo->working_dir);
        break;

      case 'y':
        g_avf_config.test_yield = 1;
        if (g_yield.test_num < MAX_YIELD_TEST_NUM) {
          strcpy(g_yield.working_dir[g_yield.test_num], optarg);
          ArmLog_DBG(ARM_LOG_AVF, "Yield test fd-%u:  %s", g_yield.test_num, g_yield.working_dir[g_yield.test_num]);
        } else {
          ArmLog_ERR(ARM_LOG_AVF, "Exceed the maximum number of Yoeld test!!");
          exit(-1);
        }
        g_yield.test_num++;
        break;

      case 'e':
        pRunInfo->out_buf_depth = strtoul(optarg, NULL, 0);
        if (!(pRunInfo->out_buf_depth > 0 && pRunInfo->out_buf_depth <= MAX_OUT_BUF_DEPTH)) {
          pRunInfo->out_buf_depth = MAX_OUT_BUF_DEPTH;
        }
        ArmLog_DBG(ARM_LOG_AVF, "Output buf depth: %u", pRunInfo->out_buf_depth);
        break;

      case 'x':
        /* Daemon stop */
        g_avf_config.stop_daemon = 1;
        ArmLog_DBG(ARM_LOG_AVF, "Shutdown daemon...");
        break;

      case 'h':
        avf_print_help(argv[0]);
        exit(0);
        break;

      case 'v':
        printf("%s\n", VERSION_OF_AVF);
        exit(0);
        break;

      default:
        avf_print_help(argv[0]);
        exit(-1);
    }
  }

  if (!g_avf_config.start_daemon && !g_avf_config.stop_daemon && !g_avf_config.test_yield) {
    /* Need input and output for flexidag iteration */
    if (pRunInfo->in_num == 0 || pRunInfo->out_num == 0) {
      ArmLog_ERR(ARM_LOG_AVF, "Type input/output file name!!!");
      exit(-1);
    }
  }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_watchdog_thd
 *
 *  @Description:: The thread to monitor if APP is alive or not
 *
 *  @Input      ::
 *      arg:      The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* avf_watchdog_thd(void *arg)
{
  struct timeval  tv_start_watch, tv_end_watch;
  uint32_t slot = *((uint32_t *)arg);
  AVF_RUN_CTRL_s *pRunCtrl;
  AVF_WATCHDOG_s *pAppWd;

  pRunCtrl = &g_daemon_run_ctrl[slot];
  pAppWd = &g_daemon_run_ctrl[slot].wd;

  do {
    /* Reset flag */
    ArmMutex_Take(&(pAppWd->mutex));
    pAppWd->keepalive_flag = 0;
    ArmMutex_Give(&(pAppWd->mutex));

    gettimeofday(&tv_start_watch, NULL);
    sleep(pAppWd->timeout_value);
    gettimeofday(&tv_end_watch, NULL);

    /* Check if flag is set */
    ArmMutex_Take(&(pAppWd->mutex));

    if (1 == pAppWd->keepalive_flag) {
      uint32_t watch_time = (uint32_t) GETTIMESTAMP(tv_start_watch,tv_end_watch) / (1000*1000);
      printf("[WATCHDOG] Monitor %us\n", watch_time);
      avf_daemon_unsolicited_msg(AVFD_UNS_CODE_WATCHDOG, slot, AVFD_RESULT_OK);  // Send msg to client */
    } else {
      ArmLog_ERR(ARM_LOG_AVF, "  ## APP WatchDog timeout(%us)", pAppWd->timeout_value);
      ArmLog_DBG(ARM_LOG_AVF, "Cancel feed/read thread!!");
      avf_daemon_unsolicited_msg(AVFD_UNS_CODE_WATCHDOG_TIMEOUT, slot, AVFD_RESULT_OK);
      ArmMutex_Give(&(pAppWd->mutex));
      pthread_cancel(pRunCtrl->feed_thread);
      pthread_cancel(pRunCtrl->read_thread);
      ArmEventFlag_Set(&(pRunCtrl->event_flag), STATE_FINISH);
      sleep(60);
      ArmLog_DBG(ARM_LOG_AVF, "Cannot terminate threads, exit()!!");
      exit(-1);
      break;
    }

    ArmMutex_Give(&(pAppWd->mutex));

  } while(1);

  return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_watchdog_create
 *
 *  @Description:: Create APP watchdog thread
 *
 *  @Input      ::
 *    slot:          Slot index
 *    timeout_value: Timeout value
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void avf_watchdog_create(uint32_t slot, uint32_t timeout_value)
{
  AVF_RUN_INFO_s *pRunInfo;
  AVF_WATCHDOG_s *pAppWd;

  pRunInfo = &g_daemon_run_info[slot];
  pAppWd = &g_daemon_run_ctrl[slot].wd;

  if (timeout_value > 0) {
    pAppWd->timeout_value = timeout_value;
  } else {
    pAppWd->timeout_value = 5 * 60; /* default, 5 mins */
  }

  {
    char MutexName[MAX_STRING_LEN] = "Avf_WD_Mut_XX";

    MutexName[11] = '0' + slot/10;
    MutexName[12] = '0' + slot%10;
    (void) ArmMutex_Create(&(pAppWd->mutex), MutexName);
  }

  pthread_create(&(pAppWd->thread_handler), NULL, avf_watchdog_thd, &(pRunInfo->slot));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_watchdog_ping
 *
 *  @Description:: APP's keepalive function
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void avf_watchdog_ping(uint32_t slot)
{
  AVF_WATCHDOG_s *pAppWd;

  pAppWd = &g_daemon_run_ctrl[slot].wd;

  ArmMutex_Take(&(pAppWd->mutex));
  pAppWd->keepalive_flag = 1;
  ArmMutex_Give(&(pAppWd->mutex));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_watchdog_delete
 *
 *  @Description:: Delete APP watchdog
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     int      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static void avf_watchdog_delete(uint32_t slot)
{
  AVF_WATCHDOG_s *pAppWd;

  pAppWd = &g_daemon_run_ctrl[slot].wd;

  pthread_cancel(pAppWd->thread_handler);
  printf("[SLOT%u][WATCHDOG] terminated!\n", slot);
  pthread_join(pAppWd->thread_handler, NULL);
  ArmMutex_Delete(&(pAppWd->mutex));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_check_inner_buf_size
 *
 *  @Description:: Check if need to use inner buffer
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     uint32_t    Buffer size
\*-----------------------------------------------------------------------------------------------*/
static int32_t avf_check_inner_buf_size(uint32_t slot)
{
  uint32_t buf_size = 0U;
  AVF_RUN_INFO_s *pRunInfo = &g_daemon_run_info[slot];

  /* For pic-info mode */
  if (pRunInfo->pic_info_en) {
    for (uint32_t i = 0U; i < pRunInfo->in_num; i++) {
      buf_size += MAX_MEMPOOL_DEPTH * ALIGN4K((uint32_t) MAX_YUV_SIZE);
    }
  }

  return buf_size;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_allocate_inner_buf
 *
 *  @Description:: Allocate inner buffer
 *
 *  @Input      ::
 *    slot:        Slot index
 *    p_item:      Pointer to AVF_MQ_ITEM_s struct
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     int      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int32_t avf_allocate_inner_buf(uint32_t slot, AVF_MQ_ITEM_s *p_item)
{
  AVF_INPUT_INFO_s *pInputInfo = &g_daemon_input_info[slot];
  AVF_RUN_INFO_s *pRunInfo = &g_daemon_run_info[slot];
  UINT32 Rval = ARM_OK;

  /* For pic-info mode */
  if (pRunInfo->pic_info_en) {
    for (uint32_t i = 0U; i < pRunInfo->in_num; i++) {
      Rval = ArmMemPool_AllocateBlock(pInputInfo->inner_pool_id, MAX_YUV_SIZE, &p_item->inner_buf_ctrl.yuv_data_buf[i]);

      if (ARM_OK != Rval) {
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMemPool_AllocateBlock()");
        break;
      }
    }
  }

  return (Rval == ARM_OK) ? 0 : -1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_free_inner_buf
 *
 *  @Description:: Free inner buffer
 *
 *  @Input      ::
 *    slot:        Slot index
 *    p_item:      Pointer to AVF_MQ_ITEM_s struct
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     int      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int32_t avf_free_inner_buf(uint32_t slot, AVF_MQ_ITEM_s *p_item)
{
  AVF_INPUT_INFO_s *pInputInfo = &g_daemon_input_info[slot];
  AVF_RUN_INFO_s *pRunInfo = &g_daemon_run_info[slot];
  UINT32 Rval = ARM_OK;

  /* For pic-info mode */
  if (pRunInfo->pic_info_en)  {
    for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
      Rval = ArmMemPool_Free(pInputInfo->inner_pool_id, &p_item->inner_buf_ctrl.yuv_data_buf[i]);

      if (ARM_OK != Rval) {
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMemPool_Free()");
        break;
      }
    }
  }

  return (Rval == ARM_OK) ? 0 : -1;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_delete_mem_pool
 *
 *  @Description:: Delete memory pool
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     int      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int32_t avf_delete_mem_pool(uint32_t slot)
{
  UINT32 UsedSize = 0, FreeSize = 0;
  UINT32 Rval = ARM_OK;
  int32_t ret = 0;
  AVF_RUN_INFO_s *pRunInfo;
  AVF_INPUT_INFO_s *pInputInfo;

  do {
    /* Sanity check */
    if (slot >= MAX_AVF_SLOT) {
      ret = -1;
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_delete_mem_pool() unavailable Slot(%u)", slot);
      break;
    }

    pRunInfo = &g_daemon_run_info[slot];
    pInputInfo = &g_daemon_input_info[slot];

    if (pInputInfo->mem_allocated_flag == 0U) {
      ArmLog_DBG(ARM_LOG_AVF, "  ## avf_delete_mem_pool() mem doesn't be allocated on Slot-%u", slot);
      break;
    }

    for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
      Rval = ArmMemPool_Delete(pInputInfo->pool_id[i]);
      if (Rval == ARM_NG) {
        ret = -1;
      }
      Rval = DaemonIO_FreeCma(&(pInputInfo->pool_buf[i]));
      if (Rval == ARM_NG) {
        ret = -1;
      }
    }

    /* Delete inner pool */
    if (pInputInfo->inner_pool_used) {
      Rval = ArmMemPool_Delete(pInputInfo->inner_pool_id);
      if (Rval == ARM_NG) {
        ret |= -1;
      }
      Rval = DaemonIO_FreeCma(&(pInputInfo->inner_pool_buf));
      if (Rval == ARM_NG) {
        ret |= -1;
      }
    }

    DaemonIO_GetCmaInfo(&UsedSize, &FreeSize);
    if (UsedSize > 0) {
      ArmLog_WARN(ARM_LOG_AVF, "It might has memory leakage. (UsedSize = %u) or SLOT# is running", UsedSize);
    }

    pInputInfo->mem_allocated_flag = 0U;
  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_craete_mem_pool
 *
 *  @Description:: Create memory pool
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     int      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int32_t avf_craete_mem_pool(uint32_t slot)
{
  int32_t ret = 0;
  UINT32 UsedSize, FreeSize;
  UINT32 InputPoolSizeInMB = 0;
  uint32_t default_flag = 1;
  uint32_t estimate_buf_size = 0;
  uint32_t inner_buf_size = 0;
  AVF_RUN_INFO_s *pRunInfo;
  AVF_INPUT_INFO_s *pInputInfo;

  /* Sanity check */
  if (slot >= MAX_AVF_SLOT) {
    ret = -1;
    ArmLog_ERR(ARM_LOG_AVF, "  ## avf_craete_mem_pool() unavailable Slot(%u)", slot);
  }

  /* 1 Query mem info */
  if (ret == 0) {
    DaemonIO_GetCmaInfo(&UsedSize, &FreeSize);

    if (FreeSize == 0) {
      ret = -1;
      ArmLog_ERR(ARM_LOG_AVF, "  ## no available CMA");
    }
  }

  /* 2 Alocate CMA mem */
  if (ret == 0) {
    pRunInfo = &g_daemon_run_info[slot];
    pInputInfo = &g_daemon_input_info[slot];

    /* Check if do adaptive mem allocation according file size */
    for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
      estimate_buf_size += MAX_MEMPOOL_DEPTH * ALIGN4K((uint32_t)pInputInfo->file_size[i]);
    }

    /* Check if need to use inner buffer */
    inner_buf_size = avf_check_inner_buf_size(slot);
    pInputInfo->inner_pool_used = (inner_buf_size > 0) ? 1U : 0U;

    /* Make sure at least system has one MB for each input pool */
    if (FreeSize > (estimate_buf_size + inner_buf_size)) {
      default_flag = 0;
    }

    InputPoolSizeInMB = (FreeSize - inner_buf_size) / (1024 * 1024 * pRunInfo->in_num);

    if (InputPoolSizeInMB > 0) {
      UINT32 InputPoolSize[AVF_MAX_IO] = {0};
      /* Set each pool's size */
      for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
        if (1 == default_flag){
          /* free space is not enough for depth 8; average space size for each input buf */
          InputPoolSize[i] = InputPoolSizeInMB * 1024 * 1024;
          /* check if pool size is larger than input */
          if (InputPoolSize[i] < ALIGN4K((uint32_t)pInputInfo->file_size[i])) {
            ret = -1;
            ArmLog_ERR(ARM_LOG_AVF, "  ## no available CMA for input-%u", i);
            break;
          }
        } else {
          InputPoolSize[i] = MAX_MEMPOOL_DEPTH * ALIGN4K((uint32_t)pInputInfo->file_size[i]);
        }
      }

      /* Allocate mem for each pool */
      if (0 == ret) {
        for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
          UINT32 Rval;
          Rval = DaemonIO_AllocateCma(InputPoolSize[i], &(pInputInfo->pool_buf[i]));
          if (Rval != ARM_OK) {
            ret = -1;
            ArmLog_ERR(ARM_LOG_AVF, "  ## fail to DaemonIO_AllocateCma()");
            break;
          }
          ArmLog_DBG(ARM_LOG_AVF, "Allocate %uB for input_%u", InputPoolSize[i], i);
        }
      }

      /* Allocate mem for inner pool */
      if ((pInputInfo->inner_pool_used) && (0 == ret)) {
        UINT32 Rval;
        Rval = DaemonIO_AllocateCma(inner_buf_size, &(pInputInfo->inner_pool_buf));
        if (Rval != ARM_OK) {
          ret = -1;
          ArmLog_ERR(ARM_LOG_AVF, "  ## fail to DaemonIO_AllocateCma()");
        } else {
          ArmLog_DBG(ARM_LOG_AVF, "Allocate %uB for inner pool", inner_buf_size);
        }
      }
    } else {
      ret = -1;
      ArmLog_ERR(ARM_LOG_AVF, "  ## no available CMA for each input(Free size %uMB)", FreeSize/(1024*1024));
    }
  }

  /* 3 Create mem pool for memory management */
  if (ret == 0) {
    UINT32 Rval;
    for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
      Rval = ArmMemPool_Create(&(pInputInfo->pool_buf[i]), &(pInputInfo->pool_id[i]));
      if (Rval != ARM_OK) {
        ret = -1;
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMemPool_Create()");
        break;
      }
      ArmLog_DBG(ARM_LOG_AVF, "Create mem-pool_%u for input_%u", pInputInfo->pool_id[i], i);
    }

    /* For inner buffer */
    if ((pInputInfo->inner_pool_used) && (ret == 0)) {
      Rval = ArmMemPool_Create(&(pInputInfo->inner_pool_buf), &(pInputInfo->inner_pool_id));
      if (Rval != ARM_OK) {
        ret = -1;
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMemPool_Create()");
      } else {
        ArmLog_DBG(ARM_LOG_AVF, "Create mem-pool for inner buffer");
      }
    }
  }

  if (0 == ret) {
    printf("\n");
    ArmLog_DBG(ARM_LOG_AVF, "-------------------------------");
    for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
      ArmLog_DBG(ARM_LOG_AVF, "[POOL%u] V: %p, P: 0x%X", i, pInputInfo->pool_buf[i].pBuffer, pInputInfo->pool_buf[i].buffer_daddr);
    }
    ArmLog_DBG(ARM_LOG_AVF, "-------------------------------\n");
    pInputInfo->mem_allocated_flag = 1;
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: load_data
 *
 *  @Description:: Load input image to memory
 *
 *  @Input      ::
 *    slot:        Slot index
 *    p_buf:       Pointer to memory block
 *    in_idx:      Input index
 *    in_path:     The path of input image
 *
 *  @Output     ::
 *    file_size:   The size of input image
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static inline uint32_t load_data(uint32_t slot, flexidag_memblk_t *p_buf, uint32_t in_idx, char *in_path, uint32_t *file_size)
{
  int32_t FileSize = 0, ReadSize = 0;
  UINT32 Rval = ARM_OK;
  AVF_INPUT_INFO_s *pInputInfo = &g_daemon_input_info[slot];

  /* 1 Query file size */
  FileSize = ArmFIO_GetSize(in_path);
  *file_size = FileSize;

  /* 2 Allocate buffer from mem pool */
  Rval = ArmMemPool_AllocateBlock(pInputInfo->pool_id[in_idx], FileSize, p_buf);

  /* 3 Load data from file */
  if (ARM_OK == Rval) {
    ReadSize = ArmFIO_Load(p_buf->pBuffer, FileSize, in_path);
    if ((ReadSize > 0) && (ReadSize == FileSize)) {
      if(p_buf->buffer_cacheable != 0){
        ArmMem_CacheClean(p_buf);
      }
      // printf("[LOAD] Addr = %p, size = %d\n", p_buf->pBuffer, ReadSize);
    } else {
      ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmFIO_Load(), %s ReadSize = %d, FileSize = %d", in_path, ReadSize, FileSize);
      Rval = ARM_NG;
    }
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: folder_list_count
 *
 *  @Description:: Count input number
 *
 *  @Input      ::
 *      folder:      The input folder path
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     int      Total input number
\*-----------------------------------------------------------------------------------------------*/
static inline int32_t folder_list_count(const char* folder)
{
  int32_t dir_num = 0;
  DIR *d;
  struct dirent *dir;

  do {
    d = opendir(folder);
    if (d == NULL) {
      perror(folder);
      ArmLog_ERR(ARM_LOG_AVF, "  ## fail to opendir()");
      dir_num = -1;
      break;
    }
    while ((dir = readdir(d)) != NULL) {
      /*  Skip name '.' , '..' and non-folder */
      if (dir->d_name[0] == '.') {
        continue;
      }
      dir_num++;
      if ((MAX_IMAGE_NUM + 1) == dir_num) {
        ArmLog_WARN(ARM_LOG_AVF,"The maximum image/group #: %u", MAX_IMAGE_NUM);
      }
    }
    closedir(d);
  } while(0);

  ArmLog_DBG(ARM_LOG_AVF,"Total input folder number: %d", dir_num);
  return dir_num;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: replace_word
 *
 *  @Description:: Replace word
 *
 *  @Input      ::
 *    inputS:      Input string
 *    oldW:        Word will be replaced
 *    newW:        Word will be used
 *
 *  @Output     :: None
 *    outputS:     Output string
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static inline void replace_word(const char* inputS, const char* oldW, const char* newW, char* outputS)
{
  int i, cnt = 0;
  int newWlen = strlen(newW);
  int oldWlen = strlen(oldW);

  /* Counting the number of times old word occur in the string */
  for (i = 0; inputS[i] != '\0'; i++) {
    if (strstr(&inputS[i], oldW) == &inputS[i]) {
      cnt++;
      /* Jumping to index after the old word */
      i += oldWlen - 1;
    }
  }

  i = 0;
  while (*inputS) {
    /* Compare the substring */
    if (strstr(inputS, oldW) == inputS) {
      strcpy(&outputS[i], newW);
      i += newWlen;
      inputS += oldWlen;
    } else {
      outputS[i++] = *inputS++;
    }
  }

  outputS[i] = '\0';
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: merge_path
 *
 *  @Description:: Merge string as 'src1/src2'
 *
 *  @Input      ::
 *    src1:        Source string #1
 *    src2:        Source string #2
 *
 *  @Output     ::
 *    dest:        Destination buffer
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static inline void merge_path(char* dest, const char* src1, const char* src2)
{
  size_t length;

  /* dest = "src1" */
  strcpy(dest, src1);
  /* dest = "src1/" */
  length = strlen(src1);
  dest[length] = '/';
  /* dest = "src1/src2" */
  strcpy(&dest[length+1], src2);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: unlink_cb
 *
 *  @Description:: call back function of file tree walk to delete a name from file system
 *
 *  @Input      ::
 *    fpath:       The path name of the entry
 *    sb:          A pointer to the stat structure
 *    typeflag:    The entry type
 *    ftwbuf:      The buffer of ftw
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: rmrf
 *
 *  @Description:: function of linux cmd 'rm -rf'
 *
 *  @Input      ::
 *    path:        The directory path
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int rmrf(char *path)
{
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_file_write_cb
 *
 *  @Description:: The callback to save output result to file
 *
 *  @Input      ::
 *    SlotId:      The Slot ID
 *    pEventData:  The output result
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static int32_t avf_file_write_cb(uint32_t SlotId, void *pEventData)
{
  CCF_OUTPUT_s *pSinkOut = (CCF_OUTPUT_s *)pEventData;
  void *ptr;
  AVF_RUN_INFO_s *pRunInfo = &g_daemon_run_info[SlotId];
  AVF_RUN_CTRL_s *pRunCtrl = &g_daemon_run_ctrl[SlotId];
  static uint32_t proc_count[MAX_AVF_SLOT] = {0};
  static uint32_t proc_time[MAX_AVF_SLOT][MAX_AVG_PROC_TIME_NUM];

  do {
    if (pRunInfo->out_num != pSinkOut->NumOfIo) {
       ArmLog_ERR(ARM_LOG_AVF,"Expect %d outputs, but out_num is set %d", pSinkOut->NumOfIo, pRunInfo->out_num);
      break;
    }

    avf_watchdog_ping(SlotId);

    if (RUN_STATE_SAVE & pSinkOut->Flag) {
      for (int32_t i = 0; i < pSinkOut->NumOfIo; i++) {
        char out_path[MAX_PATH_LEN] = {0};
        char golden_dir[MAX_PATH_LEN] = {0};
        int32_t write_size = 0;

        ptr = (void *)pSinkOut->Io[i].pBuffer;
        merge_path(out_path, pSinkOut->FolderName, pRunInfo->out_file_name[i]);
        // ArmLog_DBG(ARM_LOG_AVF, "Save filename: %s, size = %d", out_path, pSinkOut->Io[i].buffer_size);

        /* Compare output data with golden data */
        pRunCtrl->golden_cmp_success = 1;
        merge_path(golden_dir, pRunInfo->working_dir, "golden");
        DIR* dir = opendir(golden_dir);

        if (dir) {
          uint32_t bin_size = pSinkOut->Io[i].buffer_size;
          char golden_path[MAX_PATH_LEN] = {0};
          int32_t check_val = 0;
          uint32_t cma_used_size = 0;
          uint32_t cma_free_size = 0;
          flexidag_memblk_t golden_buf;

          /* Query for memory */
          DaemonIO_GetCmaInfo(&cma_used_size, &cma_free_size);

          if (cma_free_size < bin_size) {
            ArmLog_ERR(ARM_LOG_AVF, "No available CMA for golden[%d] comparison!", i);
            pRunCtrl->golden_cmp_success = 0;
            continue;
          }

          /* Allocate CMA memory */
          if (DaemonIO_AllocateCma(bin_size, &golden_buf) != ARM_OK) {
            ArmLog_ERR(ARM_LOG_AVF, "Allocate CMA memory fail!");
            pRunCtrl->golden_cmp_success = 0;
            continue;
          }

          replace_word(out_path, "bub_out", "golden", golden_path);
          // ArmLog_DBG(ARM_LOG_AVF, "Load golden[%d] at: %s", i, golden_path);

          if (ArmFIO_Load((void*) golden_buf.pBuffer, bin_size, golden_path) != pSinkOut->Io[i].buffer_size) {
            ArmLog_ERR(ARM_LOG_AVF, "Load golden[%d] fail!", i);
            if (DaemonIO_FreeCma(&golden_buf) != ARM_OK) {
              ArmLog_ERR(ARM_LOG_AVF, "Free CMA memory fail!");
            }
            pRunCtrl->golden_cmp_success = 0;
            continue;
          }

          check_val = avf_compare_binary(ptr, golden_buf.pBuffer, bin_size);

          /* Free CMA memory */
          if (DaemonIO_FreeCma(&golden_buf) != ARM_OK) {
            ArmLog_ERR(ARM_LOG_AVF, "Free CMA memory fail!");
            pRunCtrl->golden_cmp_success = 0;
            continue;
          }

          if (check_val != 0) {
            ArmLog_ERR(ARM_LOG_AVF, "Compare output[%d] with golden[%d] fail!", i, i);
            pRunCtrl->golden_cmp_success = 0;
            continue;
          }

          ArmLog_DBG(ARM_LOG_AVF, "Compare output[%d] with golden[%d] pass!", i, i);
          closedir(dir);
        }

        /* Save output[i] */
        write_size = ArmFIO_Save(ptr, pSinkOut->Io[i].buffer_size, out_path);

        if ((uint32_t)(write_size) != pSinkOut->Io[i].buffer_size) {
          ArmLog_ERR(ARM_LOG_AVF, "Output%d expect write size %u, but %d", i, pSinkOut->Io[i].buffer_size, write_size);
        }
      }
    }

    /* Send out avg proc time to client */
    if (proc_count[SlotId] == MAX_AVG_PROC_TIME_NUM) {
      uint32_t avg_proc_time = 0;

      for (int32_t i = 0; i < MAX_AVG_PROC_TIME_NUM; i++) {
        avg_proc_time += proc_time[SlotId][i];
      }
      avg_proc_time /= MAX_AVG_PROC_TIME_NUM;
      avf_daemon_unsolicited_msg(AVFD_UNS_CODE_PROC_TIME, SlotId, (int32_t)avg_proc_time);
      proc_count[SlotId] %= MAX_AVG_PROC_TIME_NUM;
    }

    if (pSinkOut->ProcessingTime != 0) {
      proc_time[SlotId][proc_count[SlotId]] = pSinkOut->ProcessingTime;
      proc_count[SlotId]++;
    }

    if (RUN_STATE_DONE & pSinkOut->Flag) {
      ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]Set state as finish", SlotId);
      proc_count[SlotId] = 0;
      usleep(5000);
      ArmEventFlag_Set(&(pRunCtrl->event_flag), STATE_FINISH);
    }
  } while(0);

  return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_file_read_thd
 *
 *  @Description:: The thread to read input image
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* avf_file_read_thd(void *arg)
{
  uint32_t rval = ARM_OK;
  int32_t ret, dir_num, group_num, processed_group = 0;
  DIR *d, *d_group;
  struct dirent *dir, *dir_group;
  char out_folder_path[MAX_PATH_LEN] = {0};
  uint32_t slot = *((uint32_t *)arg);
  AVF_RUN_INFO_s *pRunInfo;
  AVF_RUN_CTRL_s *pRunCtrl;
  AVF_INPUT_INFO_s *pInputInfo;

  do {
    /* Sanity check */
    if (slot >= MAX_AVF_SLOT) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_file_read_thd() unavailable slot(%u)!", slot);
      rval = ARM_NG;
      break;
    }

    ArmLog_DBG(ARM_LOG_AVF, "READ thread run on slot-%u", slot);
    pRunInfo   = &g_daemon_run_info[slot];
    pRunCtrl   = &g_daemon_run_ctrl[slot];
    pInputInfo = &g_daemon_input_info[slot];

    /* input folder is in working dir if no specific */
    if (0 == pRunInfo->in_folder_path[0]) {
      /* in_folder_path = "working dir/bub_input" */
      merge_path(pRunInfo->in_folder_path, pRunInfo->working_dir, "bub_input");
    }

    ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]READ thread input path: %s", slot, pRunInfo->in_folder_path);

    /* Create output root dir */
    {
      struct stat st = {0};

      /* out_path = "working dir/bub_out" */
      merge_path(out_folder_path, pRunInfo->working_dir, "bub_out");
      ret = stat(out_folder_path, &st);
      if (ret != 0) {
        ret = mkdir(out_folder_path, 0777);
        printf("[READ][%u]mkdir %s, ret %d\n", slot, out_folder_path, ret);
      }
    }

    group_num = folder_list_count(pRunInfo->in_folder_path);
    if ((group_num == -1) || (group_num > MAX_IMAGE_GROUP)) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## fail to folder_list_count()");
      rval = ARM_NG;
      break;
    }
    pRunCtrl->group_num = group_num;  // Profile data setting

    d_group = opendir(pRunInfo->in_folder_path);
    if (d_group == NULL) {
      perror(pRunInfo->in_folder_path);
      ArmLog_ERR(ARM_LOG_AVF, "  ## fail to opendir()");
      rval = ARM_NG;
      break;
    }

    while ((dir_group = readdir(d_group)) != NULL) {
      int32_t processed_num = 0;
      char in_path_group[MAX_PATH_LEN]  = {0};
      char out_path_group[MAX_PATH_LEN] = {0};
      if (dir_group->d_name[0] == '.') {
        // printf("skip %s\n", dir->d_name);
        continue;
      }

      /* path = "xxx_dir/bub_input/group-folder" */
      merge_path(in_path_group, pRunInfo->in_folder_path, dir_group->d_name);

      /* path = "working dir/bub_out/group-folder" */
      merge_path(out_path_group, out_folder_path, dir_group->d_name);

      /* owner:rwx group:rwx others:rwx */
      ret = mkdir(out_path_group, 0777);
      if (ret != 0) {
        perror(out_path_group);
        printf("  ## fail to mkdir()\n");
        rval = ARM_NG;
        break;
      }

      processed_group++;
      printf("[READ][%u] Process group-%u %s\n", slot, processed_group, in_path_group);
      /* Process each group images ++ */
      do {
        /* Count dir num*/
        dir_num = folder_list_count(in_path_group);
        if ((-1 == dir_num) || (dir_num > MAX_IMAGE_NUM)) {
          ArmLog_ERR(ARM_LOG_AVF, "  ## fail to folder_list_count()");
          rval = ARM_NG;
          break;
        }

        pRunCtrl->image_num_in_group[(processed_group - 1) % MAX_PROFILE_GROUP] = dir_num;  // profile data setting

        /* 1 Open input folder */
        d = opendir(in_path_group);
        if (d == NULL) {
          perror(in_path_group);
          ArmLog_ERR(ARM_LOG_AVF, "  ## fail to opendir()");
          rval = ARM_NG;
          break;
        }
        /* 2  Serach sub-directory for each input */
        gettimeofday(&(pRunCtrl->tv_start[(processed_group - 1) % MAX_PROFILE_GROUP]), NULL);
        while ((dir = readdir(d)) != NULL) {
          char in_path[MAX_PATH_LEN]  = {0};
          char out_path[MAX_PATH_LEN] = {0};
          size_t length;
          AVF_MQ_ITEM_s msg_queue_item = {0};
          AVF_ACKQ_ITEM_s ack_queue_item = {0};
          /* 2.1 Skip name '.' , '..' and non-folder */
          if (dir->d_name[0] == '.') {
            // printf("skip %s\n", dir->d_name);
            continue;
          }
          // printf("[1]%s\n", dir->d_name);
          /* 2.2 Fill output path */
          /* path = "working dir/bub_out/group-folder/sub-folder" */
          merge_path(out_path, out_path_group, dir->d_name);

          /* 2.3 mkdir for output */
          /* owner:rwx group:rwx others:rwx */
          ret = mkdir(out_path, 0777);
          if (ret != 0) {
            perror(out_path);
            printf("  ## fail to mkdir()\n");
            rval = ARM_NG;
            break;
          }

          /* 2.4 Find the input raw files */
          /* path = "xxx_dir/bub_input/group-folder/sub-folder" */
          merge_path(in_path, in_path_group, dir->d_name);
          printf("[READ][%u] Seq-%u %s\n", slot, processed_num + 1, in_path);

          /* path = "xxx_dir/bub_input/group-folder/sub-folder/" */
          length = strlen(in_path);
          in_path[length] = '/';
          /* path = "xxx_dir/bub_input/group-folder/sub-folder/data_pad32.bin" */
          /* Allocate mem before loading */
          if (0 == pInputInfo->mem_allocated_flag) {
            for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
              size_t temp_length;
              temp_length = strlen(pRunInfo->in_file_name[i]);//TBD+
              strcpy(&in_path[length+1], pRunInfo->in_file_name[i]);
              in_path[length + 1 + temp_length] = '\0';
              pInputInfo->file_size[i] = ArmFIO_GetSize(in_path);
              if (-1 == pInputInfo->file_size[i]) {
                rval = ARM_NG;
                break;
              }
            }

            /* Get file size fail */
            if (rval == ARM_NG) {
              break;
            }

            /* Create memory pool to manager buffers; each input one pool id */
            ret = avf_craete_mem_pool(slot);
            if (ret != 0) {
              rval = ARM_NG;
              break;
            }

          }

          for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
            size_t temp_length;
            temp_length = strlen(pRunInfo->in_file_name[i]);
            strcpy(&in_path[length+1], pRunInfo->in_file_name[i]);
            in_path[length + 1 + temp_length] = '\0';
            // printf("[1]%s\n", in_path);
            /* 2.5 Load the raw file */
            avf_watchdog_ping(slot);

            rval = load_data(slot, &msg_queue_item.raw_data_buf[i], i, in_path, &msg_queue_item.raw_size[i]);
            if (ARM_OK != rval) {
              ArmLog_ERR(ARM_LOG_AVF, "  ## fail to load_data()");
              break;
            }
          }

          /* For inner buffer */
          if (rval == 0) {
            rval = avf_allocate_inner_buf(slot, &msg_queue_item);
            if (ARM_OK != rval) {
              ArmLog_ERR(ARM_LOG_AVF, "  ## fail to avf_allocate_inner_buf()");
            }
          }

          if (ARM_OK != rval) {
            break;
          }

          /* 2.6 Send msg to feed thread */
          strcpy(msg_queue_item.folder_name, out_path);
          // processed_num++;
          msg_queue_item.seq_num = ++processed_num;
          if ((processed_num == dir_num) && (processed_group == group_num)) {
            msg_queue_item.stop_run_flag = RUN_STATE_DONE;
          }
          rval = ArmMsgQueue_Send(&(pRunCtrl->msg_q), (void *)&msg_queue_item);
          if (ARM_OK != rval) {
            ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMsgQueue_Send()");
            break;
          }
          /* 2.7 Wait ack from feed thread, for debug */
          if (pRunCtrl->enabled_ack != 0) {
            rval = ArmMsgQueue_Recv(&(pRunCtrl->ack_q), &ack_queue_item);
            if (rval != ARM_OK) {
              ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMsgQueue_Recv()");
              break;
            }
          }
        }
        gettimeofday(&(pRunCtrl->tv_end[(processed_group - 1) % MAX_PROFILE_GROUP]), NULL);
        closedir(d);
        /* Check if all the input in group has been send to Feed Thread */
        if (dir_num != processed_num) {
          rval = ARM_NG;
          ArmLog_ERR(ARM_LOG_AVF, "  ## file num(%d) in group != file read num(%u)", dir_num, processed_num);
        }
      } while(0);
      /* Process each group images -- */
      if(ARM_OK != rval) {
        break;
      }
    }
    closedir(d_group);
    /* Check if all the group has been processed */
    if (group_num != processed_group) {
      rval = ARM_NG;
      ArmLog_ERR(ARM_LOG_AVF, "  ## group num(%d) != processed num(%u)", group_num, processed_group);
    }
  } while(0);

  if (ARM_NG == rval) {
    /* Cancel feed thread */
    ret = pthread_cancel(pRunCtrl->feed_thread);
    ArmEventFlag_Set(&(pRunCtrl->event_flag), STATE_FINISH);
    ArmLog_DBG(ARM_LOG_AVF, "Cancel feed thread!!");
    avf_daemon_unsolicited_msg(AVFD_UNS_CODE_GENERAL_ERR, slot, AVFD_RESULT_READ_NG);
  }

  ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]Read task Done!!", slot);

  return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_file_feed_thd
 *
 *  @Description:: The thread to feed input image to cvcomm_flexi
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* avf_file_feed_thd(void *arg)
{
  uint32_t ret = ARM_OK;
  uint32_t slot = *((uint32_t *)arg);
  AVF_RUN_INFO_s *pRunInfo;
  AVF_RUN_CTRL_s *pRunCtrl;
  AVF_INPUT_INFO_s *pInputInfo;
  CCF_PAYLOAD_s PayLoad = {0};
  CCF_RAW_PAYLOAD_s raw_data = {0};
  CCF_PICINFO_PAYLOAD_s PicInfoData = {0};

  while (1){
    AVF_MQ_ITEM_s msg_queue_item = {0};
    AVF_ACKQ_ITEM_s ack_queue_item = {0};
    uint32_t run_number = 1;

    /* Sanity check */
    if (slot >= MAX_AVF_SLOT) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_file_feed_thd() unavailable slot(%u)!", slot);
      ret = ARM_NG;
      break;
    }

    pRunInfo   = &g_daemon_run_info[slot];
    pRunCtrl   = &g_daemon_run_ctrl[slot];
    pInputInfo = &g_daemon_input_info[slot];

    if (pRunInfo->iteration_num >= 0) {  // TBD < 0 , infinite run
      run_number = (uint32_t) pRunInfo->iteration_num;
    }

    // ArmLog_DBG(ARM_LOG_AVF, "avf_file_feed_thd() Wait file ready");
    ret = ArmMsgQueue_Recv(&(pRunCtrl->msg_q), &msg_queue_item);
    if (ret != ARM_OK) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMsgQueue_Recv()");
      break;
    }

    printf("[FEED][%u] Seq-%u %s\n", slot, msg_queue_item.seq_num, msg_queue_item.folder_name);

    /* 1 Fill CCF Payload */
    if (pRunInfo->pic_info_en == 0) {  // Raw mode
      for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
        raw_data.Addr[i]  = msg_queue_item.raw_data_buf[i].buffer_daddr;
        raw_data.Size[i]  = msg_queue_item.raw_size[i];
        raw_data.Pitch[i] = pRunInfo->in_pitch[i];
      }
      raw_data.Num = pRunInfo->in_num;
      raw_data.BatchNum = (pRunInfo->batch_num > 1) ? pRunInfo->batch_num : 1;  // Fill batch number

      PayLoad.pRaw = &raw_data;
      PayLoad.pPicInfo = NULL;
      PayLoad.OutFolderName = msg_queue_item.folder_name;
      PayLoad.Flag = msg_queue_item.stop_run_flag;

    } else {  // Pic-info mode
      PicInfoData.Num = pRunInfo->in_num;
      PicInfoData.BatchNum = (pRunInfo->batch_num > 1) ? pRunInfo->batch_num : 1;  // Fill batch number

      for (UINT32 i = 0U; i < PicInfoData.Num; i++) {  // Loop for input number
        UINT32 CtRoiPitch[6]  = {1984, 1408, 1024, 704, 512, 384};
        UINT32 CtRoiWidth[6]  = {1920, 1360,  960, 680, 480, 340};
        UINT32 CtRoiHeight[6] = {1080,  764,  540, 382, 272, 160};
        UINT32 PyramidScale = pRunInfo->in_pic_info[i/2].pyramid_scale;

        if (pRunInfo->in_pic_info[i].pyramid_scale >= MAX_HALF_OCTAVES) {
          ArmLog_ERR(ARM_LOG_AVF, "## IdspFileFeeder_Start fail (invalid PyramidScale %u)", PyramidScale);
          break;
        }

        /* Sanity check */
        if (PyramidScale < 6U) {
          if (pRunInfo->in_pic_info[i/2].roi_pitch != CtRoiPitch[PyramidScale]) {
            ArmLog_WARN(ARM_LOG_AVF, "The RoiPitch (%d) doens't match to cnn_testbed setting (%d)", pRunInfo->in_pic_info[i/2].roi_pitch, CtRoiPitch[PyramidScale]);
          }
          if (pRunInfo->in_pic_info[i/2].roi_width != CtRoiWidth[PyramidScale]) {
            ArmLog_WARN(ARM_LOG_AVF, "The RoiWidth (%d) doens't match to cnn_testbed setting (%d)", pRunInfo->in_pic_info[i/2].roi_width, CtRoiWidth[PyramidScale]);
          }
          if (pRunInfo->in_pic_info[i/2].roi_height != CtRoiHeight[PyramidScale]) {
            ArmLog_WARN(ARM_LOG_AVF, "The RoiHeight (%d) doens't match to cnn_testbed setting (%d)", pRunInfo->in_pic_info[i/2].roi_height, CtRoiHeight[PyramidScale]);
          }
        }

        /* Fill data */
        PicInfoData.PicInfo[i].Addr         = msg_queue_item.inner_buf_ctrl.yuv_data_buf[i].buffer_daddr;
        PicInfoData.PicInfo[i].PyramidScale = pRunInfo->in_pic_info[i/2].pyramid_scale;
        PicInfoData.PicInfo[i].Pitch        = pRunInfo->in_pic_info[i/2].roi_pitch;
        PicInfoData.PicInfo[i].DataW        = pRunInfo->in_pic_info[i/2].data_width;
        PicInfoData.PicInfo[i].RoiWidth     = pRunInfo->in_pic_info[i/2].roi_width;
        PicInfoData.PicInfo[i].RoiHeight    = pRunInfo->in_pic_info[i/2].roi_height;

        if (i % 2U == 0U) {  // Y
          PicInfoData.PicInfo[i].OffX       = pRunInfo->in_pic_info[i/2].offset_x;
          PicInfoData.PicInfo[i].OffY       = pRunInfo->in_pic_info[i/2].offset_y;
          PicInfoData.PicInfo[i].DataH      = pRunInfo->in_pic_info[i/2].data_height;
        } else {  // UV
          PicInfoData.PicInfo[i].OffX       = pRunInfo->in_pic_info[i/2].offset_x & 0xFFFFFFFEU;
          PicInfoData.PicInfo[i].OffY       = (pRunInfo->in_pic_info[i/2].offset_y & 0xFFFFFFFEU) / 2U;
          PicInfoData.PicInfo[i].DataH      = pRunInfo->in_pic_info[i/2].data_height / 2U;
        }

        /* Copy data to buffer line by line */
        for (UINT32 CopyH = 0U; CopyH < PicInfoData.PicInfo[i].DataH; CopyH++) {
          UINT32 SrcOffset = PicInfoData.PicInfo[i].DataW * CopyH;
          UINT32 DstOffset = PicInfoData.PicInfo[i].OffX + (PicInfoData.PicInfo[i].Pitch * (PicInfoData.PicInfo[i].OffY + CopyH));
          (void) ArmStdC_memcpy(&msg_queue_item.inner_buf_ctrl.yuv_data_buf[i].pBuffer[DstOffset], &msg_queue_item.raw_data_buf[i].pBuffer[SrcOffset], PicInfoData.PicInfo[i].DataW);
        }

        /* Cache clean */
        if (msg_queue_item.inner_buf_ctrl.yuv_data_buf[i].buffer_cacheable != 0U){
          (void) ArmMem_CacheClean(&msg_queue_item.inner_buf_ctrl.yuv_data_buf[i]);
        }
      }

      PayLoad.pRaw = NULL;
      PayLoad.pPicInfo = &PicInfoData;
      PayLoad.OutFolderName = msg_queue_item.folder_name;
      PayLoad.Flag = msg_queue_item.stop_run_flag;
    }

    /* 3 Run N iterations  */
    if (run_number > 1) {
      /* First iteration, save results */
      PayLoad.Flag = RUN_STATE_SAVE;
      avf_watchdog_ping(slot);
      CvCommFlexi_FeedPayload(slot, PayLoad);
      /* N -2 iterations, just run */
      for (uint32_t i = 1; i < (run_number - 1); i++) {
        PayLoad.Flag = RUN_STATE_REPEAT;
        avf_watchdog_ping(slot);
        CvCommFlexi_FeedPayload(slot, PayLoad);
      }
      /* Last iteration, set value of 'stop_run_flag' */
      PayLoad.Flag = msg_queue_item.stop_run_flag;
      avf_watchdog_ping(slot);
      CvCommFlexi_FeedPayload(slot, PayLoad);
    } else {
      /* One iteration + save results */
      PayLoad.Flag |= RUN_STATE_SAVE;
      avf_watchdog_ping(slot);
      CvCommFlexi_FeedPayload(slot, PayLoad);
    }

    /* 4 Free mem pool block */
    for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
      ArmMemPool_Free(pInputInfo->pool_id[i], &msg_queue_item.raw_data_buf[i]);
    }

    avf_free_inner_buf(slot, &msg_queue_item);  // For inner buffer

    /* 5 Dump CVTASK log */
    if ((pRunInfo->log_flag) & (ENABLE_CVTASK_LOG)) {
      char out_path[MAX_PATH_LEN] = {0};
      merge_path(out_path, msg_queue_item.folder_name, "cvtask.log");
      CvCommFlexi_DumpLog(slot, out_path);
    }

    /* 6 Send ACK to READ Thread, for debug */
    if (pRunCtrl->enabled_ack != 0) {
      ack_queue_item.result = 0;
      ret = ArmMsgQueue_Send(&(pRunCtrl->ack_q), (void *)&ack_queue_item);
      if (ARM_OK != ret) {
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmMsgQueue_Send()");
        break;
      }
    }

    if (RUN_STATE_DONE == msg_queue_item.stop_run_flag) {
      ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]Feed task Done!!", slot);
      break;
    }
  }

  /* Notify FEED ERR */
  if (ARM_OK != ret) {
    avf_daemon_unsolicited_msg(AVFD_UNS_CODE_GENERAL_ERR, slot, AVFD_RESULT_FEED_NG);
  }

  return NULL;
}

#if 0
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: sig_handler
 *
 *  @Description:: Handle signal
 *
 *  @Input      ::
 *      sig_no:    Signal
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void sig_handler(int32_t sig_no)
{//TBD: daemon can handle ?
  switch (sig_no) {
    case SIGTERM:
    case SIGINT:
      ArmLog_WARN(ARM_LOG_AVF, "Release resource ...");
      CvCommFlexi_Delete(0);
      ArmMsgQueue_Delete(&avf_msg_queue, msg_q_name);
      ArmMsgQueue_Delete(&avf_ack_queue, ack_q_name);
      avf_delete_mem_pool(0);
      ArmLog_WARN(ARM_LOG_AVF, "Exit");
      exit(0);
      break;
    default:
      break;
  }
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_run_multi
 *
 *  @Description:: Run multiple input image through cvcom_flexi
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_run_multi(uint32_t slot)
{
  uint32_t Rval;
  int32_t ret = 0;
  uint32_t actual_flag = 0;
  AVF_RUN_INFO_s *pRunInfo;
  AVF_RUN_CTRL_s *pRunCtrl;
  CCF_CREATE_CFG_s ccfcfg = {
      .Slot = 0,
      .Mode = 0,
      .BinPath = NULL,
      .OutBufDepth = 1
    };

  /* Sanity check */
  if (slot >= MAX_AVF_SLOT) {
    ret = -1;
    avf_daemon_unsolicited_msg(AVFD_UNS_CODE_SLOT_NUM, slot, AVFD_RESULT_NG);
    return ret;
  }

  pRunCtrl = &g_daemon_run_ctrl[slot];
  pRunInfo = &g_daemon_run_info[slot];
  pRunInfo->slot = slot;

  /* Set ccf config */
  /* BinPath = "working dir/flexibin/flexibin0.bin" */
  merge_path(ccfcfg.BinPath, pRunInfo->working_dir, "flexibin/flexibin0.bin");
  ccfcfg.Slot = slot;

  /* Select raw mode or pic-info mode */
  ccfcfg.Mode = pRunInfo->pic_info_en;

  /* Fill the batch number  */
  ccfcfg.BatchNum = (pRunInfo->batch_num > 1) ? pRunInfo->batch_num : 1;

  /* Output buffer depth */
  ccfcfg.OutBufDepth = (pRunInfo->out_buf_depth == 0) ? 1 : pRunInfo->out_buf_depth;

  avf_daemon_unsolicited_msg(AVFD_UNS_CODE_SLOT_NUM, slot, AVFD_RESULT_OK);

  gettimeofday(&(pRunCtrl->tv_start[MAX_PROFILE_GROUP]), NULL);

  /* 0. Register signal handler  */
  // signal(SIGTERM, sig_handler);
  // signal(SIGINT,  sig_handler);

  /* 1. Create flexidag */
  avf_daemon_unsolicited_msg(AVFD_UNS_CODE_FLEXI_CREATE, slot, AVFD_RESULT_OK);
  Rval = CvCommFlexi_Create(ccfcfg);
  if (Rval != ARM_OK) {
    ArmLog_ERR(ARM_LOG_AVF, "  ## fail to CvCommFlexi_Create()");
    avf_daemon_unsolicited_msg(AVFD_UNS_CODE_GENERAL_ERR, slot, AVFD_RESULT_FLEXI_NG);
    exit(-1);
  } else {
    ArmLog_DBG(ARM_LOG_AVF, "Create flexidag (Slot=%d, Mode=%d)", ccfcfg.Slot, ccfcfg.Mode);
  }

  /* 2. Register flexidag output callback */
  CvCommFlexi_RegisterOutCallback(slot, avf_file_write_cb);
  avf_daemon_unsolicited_msg(AVFD_UNS_CODE_FLEXI_START, slot, AVFD_RESULT_OK);
  {
    char event_flag_name[MAX_STRING_LEN] = "avf_flag_XX";

    event_flag_name[9]  = '0' + slot/10;
    event_flag_name[10] = '0' + slot%10;
    strcpy(pRunCtrl->event_flag_name, event_flag_name);
    ArmEventFlag_Create(&pRunCtrl->event_flag, pRunCtrl->event_flag_name);
  }
  avf_watchdog_create(slot, KEEP_ALIVE_TIME);

  do {
    /* 3. Create message queue for read/feed threads */
    {
      char msg_q_name[MAX_STRING_LEN] = "AVFMsgQ_XX";

      msg_q_name[8] = '0' + slot/10;
      msg_q_name[9] = '0' + slot%10;
      strcpy(pRunCtrl->msg_q_name, msg_q_name);
      Rval = ArmMsgQueue_Create(&(pRunCtrl->msg_q), pRunCtrl->msg_q_name, sizeof(AVF_MQ_ITEM_s), MAX_AVF_MSG_QUEUE_NUM, (void *)&(pRunCtrl->msg_q_buf[0]));
      if (Rval != ARM_OK) {
        break;
      }
    }
    {
      /* for debug */
      char ack_q_name[MAX_STRING_LEN] = "AVFAckQ_XX";

      ack_q_name[8] = '0' + slot/10;
      ack_q_name[9] = '0' + slot%10;
      strcpy(pRunCtrl->ack_q_name, ack_q_name);
      Rval = ArmMsgQueue_Create(&(pRunCtrl->ack_q), pRunCtrl->ack_q_name, sizeof(AVF_ACKQ_ITEM_s), MAX_AVF_MSG_QUEUE_NUM, (void *)&(pRunCtrl->ack_q_buf[0]));
      if (Rval != ARM_OK) {
        break;
      }
    }
    /* 4. Create tasks to read file/feed data at the same time */
    pthread_create(&(pRunCtrl->read_thread), NULL, avf_file_read_thd, &(pRunInfo->slot));
    pthread_create(&(pRunCtrl->feed_thread), NULL, avf_file_feed_thd, &(pRunInfo->slot));

    /* 5. Wait for file_io/feeder threads be terminated */
    pthread_join(pRunCtrl->read_thread, NULL);
    ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]Read thread terminated!", slot);
    pthread_join(pRunCtrl->feed_thread, NULL);
    ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]Feed thread terminated!", slot);

    /* 6. Wait callback function finish job */
    ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]Wait all CB finish!+++", slot);
    ArmEventFlag_Wait(&(pRunCtrl->event_flag), STATE_FINISH, ARM_EF_AND_CLEAR, &actual_flag);
    ArmLog_DBG(ARM_LOG_AVF, "[SLOT%u]Wait all CB finish!---", slot);
  } while(0);

  gettimeofday(&(pRunCtrl->tv_end[MAX_PROFILE_GROUP]), NULL);

  /* 7. Release resource */
  ArmEventFlag_Delete(&(pRunCtrl->event_flag));
  CvCommFlexi_Delete(slot);
  usleep(5000);
  ArmMsgQueue_Delete(&(pRunCtrl->msg_q), pRunCtrl->msg_q_name);
  ArmMsgQueue_Delete(&(pRunCtrl->ack_q), pRunCtrl->ack_q_name);
  avf_delete_mem_pool(slot);
  avf_watchdog_delete(slot);

  /* 8. Dump processing time */
  {
    int32_t split_num = (pRunCtrl->group_num > MAX_PROFILE_GROUP) ? MAX_PROFILE_GROUP : pRunCtrl->group_num;
    printf("\n");
    for (int32_t i = 0; i < split_num; i++) {
      uint32_t run_time = (uint32_t)GETTIMESTAMP(pRunCtrl->tv_start[i], pRunCtrl->tv_end[i])/(1000*1000);
      float avg_time = GETTIMESTAMP(pRunCtrl->tv_start[i], pRunCtrl->tv_end[i])/pRunCtrl->image_num_in_group[i]/(1000.0*1000.0);
      printf("[SLOT%u]Processing time of %d files: ~%u(s), Avg: ~%f(s)\n", slot, pRunCtrl->image_num_in_group[i], run_time, avg_time);
    }
  }
  printf("[SLOT%u]Total time: ~%u(s)\n", slot, GETTIMESTAMP(pRunCtrl->tv_start[MAX_PROFILE_GROUP], pRunCtrl->tv_end[MAX_PROFILE_GROUP])/(1000*1000));

  /* 9. Golden compare check */
  if (pRunCtrl->golden_cmp_success == 0) {
    ret = -1;
    avf_daemon_unsolicited_msg(AVFD_UNS_CODE_GENERAL_ERR, slot, AVFD_RESULT_GOLDEN_COMPARE_NG);
    return ret;
  }

  /* 10. Notify client that flexidag is finished */
  avf_daemon_unsolicited_msg(AVFD_UNS_CODE_DONE, slot, AVFD_RESULT_OK);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_pack_run_info
 *
 *  @Description:: Pack flexidag run info for DaemonIO
 *
 *  @Input      ::
 *    p_runInfo:   Flexidag run info
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_pack_run_info(AVF_RUN_INFO_s *p_runInfo)
{
  int32_t ret = 0;
  do {
    /* Sanity check */
    if (NULL == p_runInfo) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_pack_run_info() Null pointer!");
      ret = -1;
      break;
    }

    *p_runInfo = g_client_run_info;
    p_runInfo->log_flag = ENABLE_CVTASK_LOG;

  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_pack_yield_run_info
 *
 *  @Description:: Pack flexidag run info for DaemonIO
 *
 *  @Input      ::
 *    fd_index:    Yield test sample
 *    repeated_num:Iteration number per input image
 *    rm_out:      Delete bub_out folder
 *
 *  @Output     ::
 *    p_runInfo:   Flexidag run info
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_pack_yield_run_info(AVF_RUN_INFO_s *p_runInfo, uint32_t fd_index, int32_t repeated_num, uint32_t rm_out)
{
  int32_t ret = 0;
  AVF_RUN_INFO_s *pRunInfo;

  do {
    /* Sanity check */
    if (NULL == p_runInfo) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_pack_yield_run_info() Null pointer!");
      ret = -1;
      break;
    }
    if (MAX_YIELD_TEST_NUM <= fd_index) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_pack_yield_run_info() fd_index >= MAX!");
      ret = -1;
      break;
    }
    /* 0. Reset client run info */
    pRunInfo = &g_client_run_info;
    memset(pRunInfo, 0,  sizeof(AVF_RUN_INFO_s));
    /* 1. Working DIR */
    strcpy(pRunInfo->working_dir, g_yield.working_dir[fd_index]);
    /* 2. Input file name, TBD: hard code */
    strcpy(pRunInfo->in_file_name[pRunInfo->in_num], "data_pad32.bin");
    pRunInfo->in_num++;
    /* 3. Output file name, TBD: hard code */
    if (0 == fd_index) { /* take mnet-ssd as ex */
      strcpy(pRunInfo->out_file_name[pRunInfo->out_num], "1.out");
      pRunInfo->out_num++;
      strcpy(pRunInfo->out_file_name[pRunInfo->out_num], "2.out");
      pRunInfo->out_num++;
    } else if (1 == fd_index) {
      strcpy(pRunInfo->out_file_name[pRunInfo->out_num], "1.out");
      pRunInfo->out_num++;
    }
    /* 4. Iteration number per input image */
    pRunInfo->iteration_num = repeated_num;
    /* 5. Input image path */
    merge_path(pRunInfo->in_folder_path, pRunInfo->working_dir, "bub_input");
    /* 6. delete Working DIR/bub_out before running */
    if (rm_out != 0){
      char wdir_bub_out[MAX_PATH_LEN] = {0};
      merge_path(wdir_bub_out, pRunInfo->working_dir, "bub_out");
      rmrf(wdir_bub_out);
    }

    *p_runInfo = g_client_run_info;

  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_backup_yield_output
 *
 *  @Description:: Backup bub_out folder as bub_out_prev
 *
 *  @Input      ::
 *    fd_index:    Yield test sample
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *    INT32:       Split count(>0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_backup_yield_output(uint32_t fd_index)
{
  int32_t ret = 0;
  char wdir_bub_out[MAX_PATH_LEN] = {0};
  char wdir_bub_out_prev[MAX_PATH_LEN] = {0};
  merge_path(wdir_bub_out, g_yield.working_dir[fd_index], "bub_out");
  merge_path(wdir_bub_out_prev, g_yield.working_dir[fd_index], "bub_out_prev");
  /* delete bub_out_prev */
  rmrf(wdir_bub_out_prev);
  /* move bub_out to bub_out_prev */
  rename(wdir_bub_out, wdir_bub_out_prev);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_compare_yield_out
 *
 *  @Description:: Compare bub_out with bub_out_prev
 *
 *  @Input      ::
 *    w_dir:       Working directory
 *    out_num:     Output number
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_compare_yield_out(const char *w_dir, uint32_t out_num)
{
  int32_t ret = 0;
  char wdir_bub_out[MAX_PATH_LEN] = {0};
  char wdir_bub_out_prev[MAX_PATH_LEN] = {0};
  int32_t FileSize[2] = {0}, ReadSize = {0};
  flexidag_memblk_t out_buf[2] = {0};
  flexidag_memblk_t out_buf_prev[2] = {0};
  UINT32 Rval = ARM_OK;

  do {
    if (w_dir == NULL) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_compare_yield_out() NULL pointer!");
      ret = -1;
      break;
    }

    merge_path(wdir_bub_out, w_dir, "bub_out/group_0");
    merge_path(wdir_bub_out_prev, w_dir, "bub_out_prev/group_0");

    /* Allocate mem */
    for (uint32_t i = 0; i < out_num; i++) {
      char temp[MAX_PATH_LEN] = {0};
      size_t path_len = 0;
      merge_path(temp, wdir_bub_out, "1/1.out");
      path_len = strlen(temp);
      temp[path_len - 5] += i;

      FileSize[i] = ArmFIO_GetSize(temp);
      Rval = DaemonIO_AllocateCma(FileSize[i], &out_buf[i]);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to DaemonIO_AllocateCma()");
        ret = -1;
        break;
      }
      Rval = DaemonIO_AllocateCma(FileSize[i], &out_buf_prev[i]);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to DaemonIO_AllocateCma()");
        ret = -1;
        break;
      }
    }

    if (ret == -1) {
      break;
    }

    /* Compare bub_out with bub_out_prev */
    // Hard code for 5 input images
    for (uint32_t i = 0; i < 5; i++) {
      for (uint32_t j = 0; j < out_num; j++) {
        char bub_out_path[MAX_PATH_LEN] = {0};
        char bub_out_prev_path[MAX_PATH_LEN] = {0};
        size_t bub_out_len = 0, bub_out_prev_len = 0;

        merge_path(bub_out_path, wdir_bub_out, "1/1.out");
        bub_out_len = strlen(bub_out_path);
        bub_out_path[bub_out_len - 7] += i;
        bub_out_path[bub_out_len - 5] += j;

        ReadSize = ArmFIO_Load(out_buf[j].pBuffer, FileSize[j], bub_out_path);
        if ((ReadSize > 0) && (ReadSize == FileSize[j])) {
          if(out_buf[j].buffer_cacheable != 0){
            ArmMem_CacheClean(&out_buf[j]);
          }
        } else {
          ret = -1;
          ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmFIO_Load(), %s ReadSize = %d, FileSize = %d", bub_out_path, ReadSize, FileSize[j]);
          break;
        }

        merge_path(bub_out_prev_path, wdir_bub_out_prev, "1/1.out");
        bub_out_prev_len = strlen(bub_out_prev_path);
        bub_out_prev_path[bub_out_prev_len - 7] += i;
        bub_out_prev_path[bub_out_prev_len - 5] += j;

        ReadSize = ArmFIO_Load(out_buf_prev[j].pBuffer, FileSize[j], bub_out_prev_path);
        if ((ReadSize > 0) && (ReadSize == FileSize[j])) {
          if(out_buf_prev[j].buffer_cacheable != 0){
            ArmMem_CacheClean(&out_buf_prev[j]);
          }
        } else {
          ArmLog_ERR(ARM_LOG_AVF, "  ## fail to ArmFIO_Load(), %s ReadSize = %d, FileSize = %d", bub_out_prev_path, ReadSize, FileSize[j]);
          ret = -1;
          break;
        }

        ret = memcmp((void *)out_buf[j].pBuffer, (void *)out_buf_prev[j].pBuffer, FileSize[j]);
        if (0 == ret){
          //printf("Data%u is correct!\n", i);
        } else {
          ArmLog_ERR(ARM_LOG_AVF, "  ## bub_out and bub_out_prev are different!!");
          ArmLog_ERR(ARM_LOG_AVF, "  ## %s", bub_out_path);
          ArmLog_ERR(ARM_LOG_AVF, "  ## %s", bub_out_prev_path);
          ret = -1;
          break;
        }

      }

      if (ret == -1) {
        break;
      }
    }

  } while (0);

  for (uint32_t i = 0; i < out_num; i++) {
    if (out_buf[i].pBuffer != NULL) {
      Rval = DaemonIO_FreeCma(&out_buf[i]);
      if (Rval == ARM_NG) {
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to DaemonIO_FreeCma()");
        ret = -1;
      }
    }
    if (out_buf_prev[i].pBuffer != NULL) {
      Rval = DaemonIO_FreeCma(&out_buf_prev[i]);
      if (Rval == ARM_NG) {
        ArmLog_ERR(ARM_LOG_AVF, "  ## fail to DaemonIO_FreeCma()");
        ret = -1;
      }
    }
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_get_dag_split_count
 *
 *  @Description:: Get DAG split count; yield test only
 *
 *  @Input      ::
 *    fd_index:    Yield test sample
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       Split count(>0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_get_dag_split_count(uint32_t fd_index)
{
  int32_t ret = 0;
  FILE *fp = NULL;
  char line_buf[128] = {0};
  char *p_substring = NULL;
  int32_t ch = 0;
  size_t max_len = 128, act_len = 0;
  char file_path[MAX_PATH_LEN] = {0};

  do {

    if (MAX_YIELD_TEST_NUM <= fd_index) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## avf_get_dag_split_count() fd_index >= MAX!");
      ret = -1;
      break;
    }

    merge_path(file_path, g_yield.working_dir[fd_index], "split_count.txt");
    fp = fopen(file_path, "r");
    if (fp == NULL) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## fail to fopen(), %s", file_path);
      ret = -1;
      break;
    }

    while (((ch = fgetc(fp)) != EOF && ch != '\n')) {
      if (act_len < max_len - 1) {
        line_buf[act_len++] = ch;
      } else {
        ArmLog_WARN(ARM_LOG_AVF, "File stream line size exceeds %d", max_len - 1);
        break;
      }
    }

    p_substring = strstr(line_buf, "(");
    if (p_substring == NULL) {
      ArmLog_ERR(ARM_LOG_AVF, "  ## No DAG SPLIT COUNT!");
      ret = -1;
      break;
    }

    ret = strtoul(p_substring+1, NULL, 0); // skip '('
    printf("DAG Split Count:%u\n", ret);

  } while(0);

  if (fp != NULL) {
    fclose(fp);
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_reset_run_info
 *
 *  @Description:: Reset run info for AVFD
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_reset_run_info(uint32_t slot)
{
  int32_t ret = 0;
  AVF_RUN_INFO_s *pRunInfo;

  do {
    uint32_t slot_mask;
    /* Sanity check */
    if (slot >= MAX_AVF_SLOT) {
      ret = -1;
      ArmLog_ERR(ARM_LOG_AVF, "  ## Unavailable slot(%u)!", slot);
      break;
    }

    /* Reset run info */
    pRunInfo = &g_daemon_run_info[slot];
    memset(pRunInfo, 0, sizeof(AVF_RUN_INFO_s));

    /* Free slot */
    slot_mask = 1 << slot;
    g_slot.aval_index |= slot_mask;
    ArmLog_DBG(ARM_LOG_AVF, "Available slot index:0x%x", g_slot.aval_index);
  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_unpack_run_info
 *
 *  @Description:: Unpack flexidag run info for AVFD
 *
 *  @Input      ::
 *    p_runInfo:   Flexidag run info
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       SLOT Num(>=0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_unpack_run_info(AVF_RUN_INFO_s *p_runInfo)
{
  int32_t ret = 0;
  uint32_t slot;

  /* Sanity check */
  if (NULL == p_runInfo) {
    ArmLog_ERR(ARM_LOG_AVF, "  ## avf_unpack_run_info() Null pointer!");
    ret = -1;
  }

  /* Get available slot */
  if (ret != -1) {
    ret = -1;  // Re-init ret value

    for (uint32_t i = 0; i < MAX_AVF_SLOT; i++) {
      uint32_t slot_mask = 1 << i;
      if (g_slot.aval_index & slot_mask) {
        /* Update available index */
        g_slot.aval_index &= (~slot_mask);
        slot = i;
        ret = (int32_t) slot;
        ArmLog_DBG(ARM_LOG_AVF, "Get AVF SLOT%u!", slot);
        break;
      }
    }

  }
  /* Copy run info to AVF-SLOT# module */
  if (ret != -1) {
    g_daemon_run_info[slot] = *p_runInfo;
  } else {
    ArmLog_ERR(ARM_LOG_AVF, "  ## avf_unpack_run_info() No avaiable slot!");
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_dump_run_info
 *
 *  @Description:: Dump flexidag run info
 *
 *  @Input      ::
 *    slot:        Slot index
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_dump_run_info(uint32_t slot)
{
  int32_t ret = 0;
  AVF_RUN_INFO_s *pRunInfo;

  do {
    /* Sanity check */
    if (slot >= MAX_AVF_SLOT) {
      ret = -1;
      ArmLog_ERR(ARM_LOG_AVF, "  ## Unavailable slot(%u)!", slot);
      break;
    }
    pRunInfo = &g_daemon_run_info[slot];
    ArmLog_DBG(ARM_LOG_AVF, "------------------------------------");
    ArmLog_DBG(ARM_LOG_AVF, "WORKING DIR: %s", pRunInfo->working_dir);
    ArmLog_DBG(ARM_LOG_AVF, "INPUT NUM: %u", pRunInfo->in_num);
    ArmLog_DBG(ARM_LOG_AVF, "INPUT BATCH NUM: %u", pRunInfo->batch_num);

    ArmLog_DBG(ARM_LOG_AVF, "INPUT PICINFO NUM: %u", pRunInfo->in_pic_info_counter);
    for (uint32_t i = 0; i < pRunInfo->in_pic_info_counter; i++) {
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u PYRAMID SCALE: %u", i, pRunInfo->in_pic_info[i].pyramid_scale);
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u ROI PITCH: %u", i, pRunInfo->in_pic_info[i].roi_pitch);
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u ROI WIDTH: %u", i, pRunInfo->in_pic_info[i].roi_width);
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u ROI HEIGHT: %u", i, pRunInfo->in_pic_info[i].roi_height);
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u OFFSET X: %u", i, pRunInfo->in_pic_info[i].offset_x);
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u OFFSET Y: %u", i, pRunInfo->in_pic_info[i].offset_y);
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u DATA WIDTH: %u", i, pRunInfo->in_pic_info[i].data_width);
      ArmLog_DBG(ARM_LOG_AVF, "PICINFO: %u DATA WIDTH: %u", i, pRunInfo->in_pic_info[i].data_height);
    }

    ArmLog_DBG(ARM_LOG_AVF, "INPUT FOLDER PATH: %s", pRunInfo->in_folder_path);
    for (uint32_t i = 0; i < pRunInfo->in_num; i++) {
      ArmLog_DBG(ARM_LOG_AVF, "INPUT FILE: %s PITCH: %u", pRunInfo->in_file_name[i], pRunInfo->in_pitch[i]);
    }

    ArmLog_DBG(ARM_LOG_AVF, "OUTPUT NUM: %u", pRunInfo->out_num);
    for (uint32_t i = 0; i < pRunInfo->out_num; i++) {
      ArmLog_DBG(ARM_LOG_AVF, "OUTPUT FILE: %s", pRunInfo->out_file_name[i]);
    }

    ArmLog_DBG(ARM_LOG_AVF, "ITERATION NUM of EACH INPUT: %u", pRunInfo->iteration_num);
    ArmLog_DBG(ARM_LOG_AVF, "LOG FLAG: %u", pRunInfo->log_flag);
    ArmLog_DBG(ARM_LOG_AVF, "------------------------------------");
  } while(0);

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_check_slot_usage
 *
 *  @Description:: Check if avf# is running
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       SLOT FREE(0)/NO(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_check_slot_usage()
{
  int32_t ret = 0;
  uint32_t aval_slot = (1 << MAX_AVF_SLOT) - 1;

  if(aval_slot == g_slot.aval_index) {
    ret = 0;
  } else {
    /* AVF# is running */
    ret = -1;
  }

  return ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: avf_init
 *
 *  @Description:: Init Amba Vision Flexi module
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    INT32:       OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
int32_t avf_init()
{
  int32_t ret = 0;
  AVF_RUN_INFO_s *pRunInfo;
  AVF_RUN_CTRL_s *pRunCtrl;
  AVF_INPUT_INFO_s *pInputInfo;

  do {
    /* Init available slot */
    g_slot.aval_index = (1 << MAX_AVF_SLOT) - 1;
    for (uint32_t i = 0; i < MAX_AVF_SLOT; i++) {
      uint32_t slot_mask = 1 << i;
      if (g_slot.aval_index & slot_mask) {
        ArmLog_DBG(ARM_LOG_AVF, "SLOT%u is available", i);
      }
      pRunInfo   = &g_daemon_run_info[i];
      pRunCtrl   = &g_daemon_run_ctrl[i];
      pInputInfo = &g_daemon_input_info[i];
      memset(pRunInfo, 0, sizeof(AVF_RUN_INFO_s));
      memset(pRunCtrl, 0, sizeof(AVF_RUN_CTRL_s));
      memset(pInputInfo, 0, sizeof(AVF_INPUT_INFO_s));
    }

  } while(0);

  return ret;
}
