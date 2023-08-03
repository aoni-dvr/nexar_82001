/**
*  @file SvcTaskList.h
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
*  @details svc task list
*
*/

#ifndef SVC_TASK_LIST_H
#define SVC_TASK_LIST_H

#include "SvcTask.h"

/* print task */
#define SVC_PRINT_TASK_PRI              (SVC_TASK_LOWEST_PRIORITY)
#define SVC_PRINT_TASK_CPU_BITS         (0x01U)

/* shell task */
#define SVC_SHELL_CLI_TASK_PRI          (SVC_TASK_LOWEST_PRIORITY - 1U)
#define SVC_SHELL_CLI_TASK_CPU_BITS     (0x01U)
#define SVC_SHELL_EXEC_TASK_PRI         (SVC_TASK_LOWEST_PRIORITY - 2U)
#define SVC_SHELL_EXEC_TASK_CPU_BITS    (0x01U)

/* init task */
#define SVC_INIT_TASK_PRI               (SVC_SHELL_EXEC_TASK_PRI - 1U)
#define SVC_INIT_TASK_CPU_BITS          (0x01U)

/* DSP boot task */
#define SVC_DSP_BOOT_TASK_PRI           (100U)
#define SVC_DSP_BOOT_TASK_CPU_BITS      (0x01U)

/* vin source task */
#define SVC_VIN_SRC_TASK_PRI            (54U)
#define SVC_VIN_SRC_TASK_CPU_BITS       (0x01U)

/* vin buffer monitor task */
#define SVC_VIN_BUF_MON_TASK_PRI        (30U)
#define SVC_VIN_BUF_MON_TASK_CPU_BITS   (0x01U)

/* vout source task */
#define SVC_VOUT_SRC_TASK_PRI           (55U)
#define SVC_VOUT_SRC_TASK_CPU_BITS      (0x04U)

/* cv boot task */
#define SVC_APP_CV_BOOT_TASK_PRI        (60U)
#define SVC_APP_CV_BOOT_TASK_CPU_BITS   (0x01U)

/* cv cam-ctrl boot task */
#define SVC_CAM_CTRL_TASK_PRI           (61U)
#define SVC_CAM_CTRL_TASK_CPU_BITS      (0x01U)

/* storage monitor */
#define SVC_STG_MONI_TASK_PRI           (32U)
#define SVC_STG_MONI_TASK_CPU_BITS      (0x01U)

/* dsp message task */
#define SVC_DSP_MSG_TASK_PRI            (6U)
#if defined(CONFIG_QNX)
#define SVC_DSP_MSG_TASK_CPU_BITS       (0x08U)
#else
#define SVC_DSP_MSG_TASK_CPU_BITS       (0x01U)
#endif

/* vfs task */
#define SVC_VFS_TASK_PRI                (56U)
#define SVC_VFS_TASK_CPU_BITS           (0x01U)

/* record master task */
#define SVC_REC_MASTER_TASK_PRI         (60U)
#define SVC_REC_MASTER_TASK_CPU_BITS    (0x01U)

/* sd monitor */
#define SVC_SD_MONI_TASK_PRI            (SVC_TASK_LOWEST_PRIORITY)
#define SVC_SD_MONI_TASK_CPU_BITS       (0x01U)

/* GPIO button */
#define SVC_GPIO_BUTTON_TASK_PRI        SVC_SHELL_EXEC_TASK_PRI
#define SVC_GPIO_BUTTON_TASK_CPU_BITS   (0x01U)

/* storage management task */
#define SVC_STOR_MGR_TASK_PRI           (61U)
#define SVC_STOR_MGR_TASK_CPU_BITS      (0x01U)

/* playback task */
#define SVC_PBK_CTRL_TASK_PRI           (59U)
#define SVC_PBK_CTRLK_TASK_CPU_BITS     (0x01U)

/* svc main flow control task */
#define SVC_CONTROL_TASK_PRI            (SVC_SHELL_EXEC_TASK_PRI - 1U)
#define SVC_CONTROL_TASK_CPU_BITS       (0x01U)

/* svc qp control task */
#define SVC_REC_QPCTRL_TASK_PRI         (70U)
#define SVC_REC_QPCTRL_TASK_CPU_BITS    (0x01U)

/* svc firmware update task */
#define SVC_FW_UPDATE_TASK_PRI          (112U)
#define SVC_FW_UPDATE_TASK_CPU_BITS     (0x01U)

/* svc rec blend task */
#define SVC_REC_BLEND_TASK_PRI          (71U)
#define SVC_REC_BLEND_TASK_CPU_BITS     (0x01U)

/* svc dirt detect task */
#define SVC_DIRT_DETECT_TASK_PRI        (72U)
#define SVC_DIRT_DETECT_TASK_CPU_BITS   (0x01U)

/* svc rec monitor task */
#define SVC_REC_MON_TASK_PRI            (57U)
#define SVC_REC_MON_TASK_CPU_BITS       (0x01U)

/* svc rec MV task */
#define SVC_REC_MV_TASK_PRI             (71U)
#define SVC_REC_MV_TASK_CPU_BITS        (0x01U)

/* svc CAN bus store task */
#define SVC_DATA_GATHER_TASK_PRI        (72U)
#define SVC_DATA_GATHER_TASK_CPU_BITS   (0x01U)

/* svc normal event record task */
#define AMBA_RDT_NML_TASK_PRI            (56U)
#define AMBA_RDT_NML_TASK_CPU_BITS       (0x01U)

/* svc record destination net task */
#define AMBA_RDT_NET_TASK_PRI            (63U)
#define AMBA_RDT_NET_TASK_CPU_BITS       (0x01U)

/* svc raw enc task */
#define SVC_RAW_ENC_TASK_PRI            (112U)
#define SVC_RAW_ENC_TASK_CPU_BITS       (0x01U)

/* svc raw capture task */
#define SVC_RAW_CAP_TASK_PRI            (100U)
#define SVC_RAW_CAP_TASK_CPU_BITS       (0x01U)

/* svc emergency event record task */
#define AMBA_RDT_EMG_TASK_PRI           (59U)
#define AMBA_RDT_EMG_TASK_CPU_BITS      (0x01U)

/* svc vin error control task */
#define SVC_VIN_ERR_TASK_PRI            (100U)
#define SVC_VIN_ERR_TASK_CPU_BITS       (0x02U)

/* svc vin error recover task */
#define SVC_VIN_ERR_REC_TASK_PRI        (101U)
#define SVC_VIN_ERR_REC_TASK_CPU_BITS   (0x02U)

/* svc vout error control task */
#define SVC_VOUT_ERR_TASK_PRI           (105U)
#define SVC_VOUT_ERR_TASK_CPU_BITS      (0x02U)

/* svc rec time-lapse capture task */
#define SVC_REC_TLPS_TASK_PRI           (74U)
#define SVC_REC_TLPS_TASK_CPU_BITS      (0x01U)

/* loop playback task */
#define SVC_LOOP_PBK_TASK_PRI           (SVC_PBK_TASK_0_PRI + 1U)
#define SVC_LOOP_PBK_TASK_CPU_BITS      (0x01U)

#define SVC_PLAYER_TASK_PRI             (62U)
#define SVC_PLAYER_TASK_CPU_BITS        (0x01U)

#define SVC_BITS_FIFO_TASK_PRI          (SVC_PLAYER_TASK_PRI - 5U)
#define SVC_BITS_FIFO_TASK_CPU_BITS     (0x01U)

/* svc color balance task */
#define SVC_COLOR_BALANCE_TASK_PRI      (108U)
#define SVC_COLOR_BALANCE_TASK_CPU_BITS (0x01U)

/* svc emr adaptive task */
#define SVC_EMR_ADAPTIVE_TASK_PRI       (38U)
#define SVC_EMR_ADAPTIVE_TASK_CPU_BITS  (0x01U)

/* svc data loader task */
#define SVC_DATA_LOADER_TASK_PRI        (32U)
#define SVC_DATA_LOADER_TASK_CPU_BITS   (0x02U)

/* audio ain/aenc/aout/adec tasks */
#define SVC_AIN_TASK_PRI                (32U)
#define SVC_AIN_TASK_CPU_BITS           (0x02U)
#define SVC_AENC_TASK_PRI               (33U)
#define SVC_AENC_TASK_CPU_BITS          (0x02U)
#define SVC_AOUT_TASK_PRI               (32U)
#define SVC_AOUT_TASK_CPU_BITS          (0x02U)
#define SVC_ADEC_TASK_PRI               (33U)
#define SVC_ADEC_TASK_CPU_BITS          (0x02U)
#define SVC_ADEC_FEED_TASK_PRI          (32U)
#define SVC_ADEC_FEED_TASK_CPU_BITS     (0x02U)

/* svc nvm crc background checking task */
#define SVC_NVM_CRC_CHK_TASK_PRI        (112U)
#define SVC_NVM_CRC_CHK_TASK_CPU_BITS   (0x1U)

/* svc safe state mgr task */
#define SVC_SAFE_STATE_MGR_TASK_PRI      (42U)
#define SVC_SAFE_STATE_MGR_TASK_CPU_BITS (0x01U)

#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
/* svc liveview feed ext raw task */
#define SVC_LV_FEED_EXT_RAW_TASK_PRI       (0x7U)
#define SVC_LV_FEED_EXT_RAW_TASK_CPU_BITS  (0x01U)
#endif

#if defined(CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED)
#define SVC_LV_FD_EX_YUV_TASK_PRI         (29U)
#define SVC_LV_FD_EX_YUV_TASK_CPU_BITS    (0x01U)

#define SVC_LV_FD_EX_YUV_CP_TASK_PRI      (30U)
#define SVC_LV_FD_EX_YUV_CP_TASK_CPU_BITS (0x01U)
#endif

#define SVC_SENSOR_CRC_CHECK_TASK_PRI       (0x43U)
#define SVC_SENSOR_CRC_CHECK_TASK_CPU_BITS  (0x1U)

/* svc amage tcp server task */
#define SVC_AMAGE_TCP_SERVER_TASK_PRI       (75U)
#define SVC_AMAGE_TCP_SERVER_TASK_CPU_BITS  (0x01U)

/* svc record bitstream data comparison task */
#define SVC_REC_BITSCMP_TASK_PRI            (55U)
#define SVC_REC_BITSCMP_TASK_CPU_BITS       (0x01U)

/* */
#define SVC_REC_AUTO_REC_TASK_PRI           (100U)
#define SVC_REC_AUTO_REC_TASK_CPU_BITS      (0x01U)

#define SVC_LV_FD_FILE_YUV_CP_TASK_PRI      (29U)
#define SVC_LV_FD_FILE_YUV_TASK_CPU_BITS    (0x01U)

/* svc record bitstream data comparison task */
#define SVC_DRAM_ADJ_TASK_PRI               (SVC_TASK_LOWEST_PRIORITY - 10U)
#define SVC_DRAM_ADJ_TASK_CPU_BITS          (0x01U)

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define SVC_UVC_TASK_PRI                (6U)
#define SVC_UVC_TASK_CPU_BITS           (0x02U)
#define SVC_UVC_OUTPUT_TASK_PRI         (6U)
#define SVC_UVC_OUTPUT_TASK_CPU_BITS    (0x02U)

#define SVC_IMU_FEED_TASK_PRI         (32U)
#define SVC_IMU_FEED_TASK_CPU_BITS    (0x02U)

#define SVC_IMU_EAT_TASK_PRI         (80U)
#define SVC_IMU_EAT_TASK_CPU_BITS    (0x02U)

#define SVC_HANDLER_TASK_PRI            (60U)
#define SVC_HANDLER_TASK_CPU_BITS       (0x02U)

#define SVC_BEEP_FEED_TASK_PRI          (32U)
#define SVC_BEEP_FEED_TASK_CPU_BITS     (0x02U)

#define SVC_ASYNC_TASK_PRI              (100U)
#define SVC_ASYNC_TASK_CPU_BITS         (0x2U)

#define SVC_BUTTON_TASK_PRI             (80U)
#define SVC_BUTTON_TASK_CPU_BITS        (0x2U)

#define SVC_PCM_RECORD_TASK_PRI         (60U)
#define SVC_PCM_RECORD_TASK_CPU_BITS    (0x2U)

#define SVC_AAC_RECORD_TASK_PRI         (60U)
#define SVC_AAC_RECORD_TASK_CPU_BITS    (0x2U)
#endif

#endif  /* SVC_TASK_LIST_H */
