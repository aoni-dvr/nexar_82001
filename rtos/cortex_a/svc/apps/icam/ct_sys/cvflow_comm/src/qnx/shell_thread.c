/**
*  @file shell_thread.c
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
*   @details A thread to receive shell command
*
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "shell_thread.h"

/* Sample cvalgo */
#include "SingleFD.h"
#include "SSD.h"
#include "DetCls.h"
#include "FrcnnProc.h"

/* data flow header */
#include "FileFeeder.h"
#include "FileSink.h"
#include "shell_thread.h"
#include "idsp.h"
#include "Eth.h"

#define ARM_LOG_SHELL      "Shell"

#define MAX_PARAM_NUM             (16U)
#define MAX_PARAM_LEN             (64U)

static UINT32 g_FlexiMemPoolId;

/*---------------------------------------------------------------------------*\
 * static functions
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: print_file_feeder_cmd_usage
 *
 *  @Description:: Print file feeder command usage
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void print_shell_thread_cmd_usage(void)
{
  printf("shell test command:\n");
  printf("    create [Slot] [CvAlgo] [Flexibin0] [Flexibin1] ...\n");
  printf("        [Slot]: The slot id \n");
  printf("        [CvAlgo]: The hooked CvAlgo object \n");
  printf("                  single - SingleFD cvalgo \n");
  printf("                  ssd    - SSD cvalgo \n");
  printf("                  detcls - Det + Cls cvalgo \n");
  printf("                  frcnnproc: Frcnn flexidag \n");
  printf("        [FlexibinN]: The path of flexibin \n");
  printf("    delete [Slot]\n");
  printf("        [Slot]: The slot id \n");
  printf("\n");
  printf("    feed file [Slot] [FeedFile_0] [FeedFile_1] ... \n");
  printf("        [Slot]:       The slot id \n");
  printf("        [FeedFile_N]: The file to be fed \n");
  printf("    feed idsp start [Slot] [IPC CH] \n");
  printf("        [Slot]:             The slot id \n");
  printf("        [IPC CH]:           The IPC channel \n");
  printf("    feed idsp stop [Slot] \n");
  printf("        [Slot]:             The slot id \n");
  printf("    feed secret [Slot] [SecretFile] \n");
  printf("        [Slot]:       The slot id \n");
  printf("        [SecretFile]: The secret file \n");
  printf("\n");
  printf("    reg_cb file [Slot] \n");
  printf("        [Slot]:      The slot id \n");
  printf("        The output filename is DUMP_XX_YYY.out \n");
  printf("        XX is the order of output node, YYY is the index of test iteration \n");
  printf("\n");
  printf("    ctrl roi [Slot] [PyramidIdx] [ROIStartCol] [ROIStartRow] \n");
  printf("        [Slot]: The slot id \n");
  printf("        [PyramidIdx]: Level of idsp image pyramid [0 - 6] \n");
  printf("        [ROIStartCol]: Start column of roi \n");
  printf("        [ROIStartRow]: Start row of roi \n");
  printf("    ctrl ssd_nms [Slot] [ClassNum] [PriorBoxPath] [ROI_W] [ROI_H]\n");
  printf("    [ROI_X] [ROI_Y] [NET_W] [NET_H] [VIS_W] [VIS_H] [ModelType] \n");
  printf("        [Slot]:         The slot id \n");
  printf("        [ClassNum]:     Number of detect class \n");
  printf("        [PriorBoxPath]: Path of priorbox \n");
  printf("        [ROI_W]:        Width of ROI \n");
  printf("        [ROI_H]:        Height of ROI \n");
  printf("        [ROI_X]:        Start X of ROI \n");
  printf("        [ROI_Y]:        Start Y of ROI \n");
  printf("        [NET_W]:        Width of Net(CNN) \n");
  printf("        [NET_W]:        Height of Net(CNN) \n");
  printf("        [VIS_W]:        Width of Visual(OSD) \n");
  printf("        [VIS_H]:        Height of Visual(OSD) \n");
  printf("        [ModelType]:    0: caffe model, 1: Tensorflow model \n");
  printf("    ctrl frcnn_proc [Slot] [ImgHeight] [ImgWidth] \n");
  printf("        [Slot]:          The slot id \n");
  printf("        [ImgHeight]:     Height of image \n");
  printf("        [ImgWidth]:      Width of image \n");
  printf("    ctrl dmsg [Slot]\n");
  printf("        [Slot]:         The slot id \n");
  printf("\n");
  printf("    eth_start - Start ethernet processing \n");
  printf("\n");
  printf("    exit - exit command mode\n");

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: parse_feeder_cmd
 *
 *  @Description:: Parse the feeder command
 *
 *  @Input      ::
 *      buf:       Input command
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void parse_shell_cmd(char* buf)
{
  char param[MAX_PARAM_NUM][MAX_PARAM_LEN];
  int param_num = 0;
  char * pch;
  char *delim = " \n\r";
  UINT32 Rval;

  /* 1. Split input command by space or newline */
  pch = strtok(buf,delim);
  while (pch != NULL)
  {
    if (param_num < MAX_PARAM_NUM) {
      param[param_num][MAX_PARAM_LEN - 1] = '\0';
      strncpy(param[param_num], pch, MAX_PARAM_LEN - 1);
      param_num ++;
    } else {
      ArmLog_ERR(ARM_LOG_SHELL, "  ## exceed %d param, please increase MAX_PARAM_NUM", MAX_PARAM_NUM, 0U);
      return;
    }
    pch = strtok(NULL, delim);
  }

  /* 2. Parse the command */
  if (param_num > 0) {

    // create [Slot] [CvAlgo] [Flexibin0] [Flexibin1] ...
    if ((strcmp("create", param[0]) == 0) && (param_num >= 4)) {
      UINT32 Slot = 0U;
      SVC_CV_ALGO_OBJ_s              *pCvAlgoObj = NULL;
      CT_CVALGO_WRAPPER_CREATE_CFG_s CreateCfg;

      Slot = strtoul(param[1], NULL, 10);

      /* 1. Hook CvAlgoObj */
      if (strcmp(param[2], "single") == 0) {
        pCvAlgoObj = &CvAlgoObj_SingleFD;
      } else if (strcmp(param[2], "ssd") == 0) {
        pCvAlgoObj = &CvAlgoObj_SSD;
      } else if (strcmp(param[2], "detcls") == 0) {
        pCvAlgoObj = &CvAlgoObj_DetCls;
      } else if (strcmp(param[2], "frcnnproc") == 0) {
        pCvAlgoObj = &CvAlgoObj_FrcnnProc;
      } else {
        ArmLog_ERR(ARM_LOG_SHELL, "  ## can't find cvalgo", 0U, 0U);
      }

      if (pCvAlgoObj != NULL) {
        /* 2. Copy flexibin path */
        CreateCfg.NumFD = param_num - 3U;  // "create [Slot] [CvAlgo]" = 3 extra args
        for (UINT32 i = 0; i < CreateCfg.NumFD; i ++) {
          ArmStdC_strcpy(CreateCfg.Flexibin[i], MAX_PATH_LEN, param[3U + i]);
        }

        /* 3. Call CtCvAlgoWrapper_Create */
        CreateCfg.pCvAlgoObj = pCvAlgoObj;
        CreateCfg.MemPoolId  = g_FlexiMemPoolId;

        Rval = CtCvAlgoWrapper_Create(Slot, &CreateCfg);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## CtCvAlgoWrapper_Create fail", 0U, 0U);
        }
      }

    // delete [Slot]
    } else if ((strcmp("delete", param[0]) == 0) && (param_num == 2)) {
      UINT32 Slot = 0U;

      Slot = strtoul(param[1], NULL, 10);

      Rval = CtCvAlgoWrapper_Delete(Slot);
      if (Rval != ARM_OK) {
        ArmLog_ERR(ARM_LOG_SHELL, "## CtCvAlgoWrapper_Delete fail", 0U, 0U);
      }

    // feed
    } else if ((strcmp("feed", param[0]) == 0) && (param_num >= 2)) {
      if ((strcmp("file", param[1]) == 0) && (param_num >= 4)) {
        // feed file [Slot] [FeedFile_0] ...
        FILE_FEEDER_CFG_s FeederCfg = {0};

        FeederCfg.Slot = strtoul(param[2], NULL, 10);
        FeederCfg.MemPoolId = g_FlexiMemPoolId;

        FeederCfg.Num   = param_num - 3;
        for (UINT32 i = 0; i < FeederCfg.Num; i++) {
          (void) ArmStdC_memcpy(FeederCfg.Filename[i], param[i + 3], MAX_FEEDER_FN_LEN);
        }
        Rval = FileFeeder_Start(&FeederCfg);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## FileFeeder_Start() fail", 0U, 0U);
        }
      } else if ((strcmp("idspfile", param[1]) == 0) && (param_num == 13)) {
        // feed idspfile [Slot] [FileY] [FileUV] [PydScale] [RoiPitch] [RoiW] [RoiH] [OffX] [OffY] [DataW] [DataH]
        IDSPFILE_FEEDER_CFG_s FeederCfg = {0};

        FeederCfg.Slot = strtoul(param[2], NULL, 10);
        FeederCfg.MemPoolId = g_FlexiMemPoolId;
        (void) ArmStdC_memcpy(FeederCfg.FilenameY, param[3], MAX_FEEDER_FN_LEN);
        (void) ArmStdC_memcpy(FeederCfg.FilenameUV, param[4], MAX_FEEDER_FN_LEN);
        FeederCfg.PyramidScale = strtoul(param[5], NULL, 10);
        FeederCfg.RoiPitch     = strtoul(param[6], NULL, 10);
        FeederCfg.RoiWidth     = strtoul(param[7], NULL, 10);
        FeederCfg.RoiHeight    = strtoul(param[8], NULL, 10);
        FeederCfg.OffsetX      = strtoul(param[9], NULL, 10);
        FeederCfg.OffsetY      = strtoul(param[10], NULL, 10);
        FeederCfg.DataWidth    = strtoul(param[11], NULL, 10);
        FeederCfg.DataHeight   = strtoul(param[12], NULL, 10);

        Rval = IdspFileFeeder_Start(&FeederCfg);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## IdspFileFeeder_Start() fail", 0U, 0U);
        }
      } else if ((strcmp("idsp", param[1]) == 0) && (param_num >= 2)) {
        if ((strcmp("start", param[2]) == 0) && (param_num == 5)) {
          // feed idsp start [Slot] [IPC_CH]
          UINT32 Slot, IpcCh;
          Slot  = strtoul(param[3], NULL, 10);
          IpcCh = strtoul(param[4], NULL, 10);

          idsp_start(Slot, IpcCh);

        } else if ((strcmp("stop", param[2]) == 0) && (param_num == 4)) {
          // feed idsp stop [Slot]
          UINT32 Slot;
          Slot = strtoul(param[3], NULL, 10);

          idsp_stop(Slot);

        } else {
          print_shell_thread_cmd_usage();
        }
      } else if ((strcmp("secret", param[1]) == 0) && (param_num == 4)) {
        // feed secret [Slot] [SecretFile]
        SECRET_FEEDER_CFG_s FeederCfg = {0};

        FeederCfg.Slot = strtoul(param[2], NULL, 10);
        FeederCfg.MemPoolId = g_FlexiMemPoolId;
        (void) ArmStdC_memcpy(FeederCfg.Filename, param[3], MAX_FEEDER_FN_LEN);

        Rval = SecretFeeder_Start(&FeederCfg);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## SecretFeeder_Start() fail", 0U, 0U);
        }
      } else {
        print_shell_thread_cmd_usage();
      }

    // reg_cb
    } else if ((strcmp("reg_cb", param[0]) == 0) && (param_num >= 2)) {
      if ((strcmp("file", param[1]) == 0) && (param_num == 3)) {
        // reg_cb file [Slot]
        FILE_SINK_CFG_s SinkCfg = {0};
        SinkCfg.Slot = strtoul(param[2], NULL, 10);

        Rval = FileSink_Register(&SinkCfg);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## FileSink_Register() fail", 0U, 0U);
        }

      } else {
        print_shell_thread_cmd_usage();
      }

    // ctrl
    } else if ((strcmp("ctrl", param[0]) == 0) && (param_num >= 2)) {
      if ((strcmp("roi", param[1]) == 0) && (param_num == 6)) {
        // ctrl roi [Slot] [PyramidIdx] [ROIStartCol] [ROIStartRow]
        amba_roi_config_t RoiCtrl = {0};
        UINT32 Slot;

        Slot                        = strtoul(param[2], NULL, 10);
        RoiCtrl.image_pyramid_index = strtoul(param[3], NULL, 10);
        RoiCtrl.roi_start_col       = strtoul(param[4], NULL, 10);
        RoiCtrl.roi_start_row       = strtoul(param[5], NULL, 10);

        Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_ROI, &RoiCtrl);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
        }

      } else if ((strcmp("ssd_nms", param[1]) == 0) && (param_num == 14)) {
        // ctrl ssd_nms [Slot] [ClassNum] [PriorBoxPath] [ROI_W] [ROI_H] [ROI_X] [ROI_Y] [NET_W] [NET_H] [VIS_W] [VIS_H] [ModelType]
        SSD_NMS_CTRL_s SsdNmsCtrl;
        UINT32 Slot;

        Slot                         = strtoul(param[2], NULL, 10);
        SsdNmsCtrl.ClassNum          = strtoul(param[3], NULL, 10);
        ArmStdC_strcpy(SsdNmsCtrl.PriorBox, MAX_PBOX_FN_LEN, param[4]);
        SsdNmsCtrl.Win.RoiWidth      = strtoul(param[5], NULL, 10);
        SsdNmsCtrl.Win.RoiHeight     = strtoul(param[6], NULL, 10);
        SsdNmsCtrl.Win.RoiStartX     = strtoul(param[7], NULL, 10);
        SsdNmsCtrl.Win.RoiStartY     = strtoul(param[8], NULL, 10);
        SsdNmsCtrl.Win.NetworkWidth  = strtoul(param[9], NULL, 10);
        SsdNmsCtrl.Win.NetworkHeight = strtoul(param[10], NULL, 10);
        SsdNmsCtrl.Win.VisWidth      = strtoul(param[11], NULL, 10);
        SsdNmsCtrl.Win.VisHeight     = strtoul(param[12], NULL, 10);
        SsdNmsCtrl.ModelType         = strtoul(param[13], NULL, 10);

        Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_SSD_NMS, &SsdNmsCtrl);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
        }

      }  else if ((strcmp("frcnn_proc", param[1]) == 0) && (param_num ==5)) {
        FRCNN_PROC_PARAM_s FrcnnProcParam;
        UINT32 Slot;

        Slot                         = strtoul(param[2], NULL, 10);
        FrcnnProcParam.ImgHeight     = strtoul(param[3], NULL, 10);
        FrcnnProcParam.ImgWidth      = strtoul(param[4], NULL, 10);

        Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_FRCNN_PROC_PARAM, &FrcnnProcParam);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
        }

      } else if ((strcmp("dmsg", param[1]) == 0) && (param_num == 3)) {
        // ctrl dmsg [Slot]
        UINT32 Slot;

        Slot = strtoul(param[2], NULL, 10);

        Rval = CtCvAlgoWrapper_Control(Slot, CTRL_TYPE_DMSG, NULL);
        if (Rval != ARM_OK) {
          ArmLog_ERR(ARM_LOG_SHELL, "## CtCvAlgoWrapper_Control fail", 0U, 0U);
        }

      } else {
        print_shell_thread_cmd_usage();
      }

    // eth_start
    } else if (strcmp("eth_start", param[0]) == 0) {
      Eth_Start(g_FlexiMemPoolId);
    } else {
      print_shell_thread_cmd_usage();
    }
  }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: file_feeder_thd
 *
 *  @Description:: The thread to provide command line interface
 *
 *  @Input      ::
 *      arg:       The thread arg
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void* shell_thread(void *arg) {
  char buf[128] = {0};

  while (1){
    ArmLog_DBG(ARM_LOG_SHELL, "Please Enter Shell Command:", 0U, 0U);
    fgets(buf, sizeof(buf), stdin);

    if (strncmp("exit", buf, 4) == 0) {
      break;
    } else {
      parse_shell_cmd(buf);
    }
  }

  ArmLog_DBG(ARM_LOG_SHELL, "Shell thread terminate", 0U, 0U);
  return NULL;
}

/*---------------------------------------------------------------------------*\
 * shell_thread APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: shell_thread_create
 *
 *  @Description:: Create a thread to receive shell command
 *
 *  @Input      ::
 *    mempool_id:  The mempool id
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
uint32_t shell_thread_create(uint32_t mempool_id)
{
  pthread_t thread;

  g_FlexiMemPoolId = mempool_id;

  /* 1. Create task to recieve shell command */
  pthread_create(&thread, NULL, shell_thread, 0);

  /* 2. Wait for file_io_feeder thread terminate */
  if ( pthread_join(thread, NULL) ) {
    ArmLog_ERR(ARM_LOG_SHELL, "## pthread_join() fail", 0U, 0U);
    abort();
  }

  return 0;
}

