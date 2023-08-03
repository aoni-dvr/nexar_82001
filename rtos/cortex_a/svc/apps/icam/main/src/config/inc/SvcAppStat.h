/**
*  @file SvcAppStat.h
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
*  @details svc menu list
*
*/

#ifndef SVC_APP_STAT_H
#define SVC_APP_STAT_H

#define SVC_APP_STAT_UCODE              (0U)    /* SVC_APP_STAT_UCODE_s */
#define SVC_APP_STAT_IQ                 (1U)    /* SVC_APP_STAT_IQ_s */
#define SVC_APP_STAT_VIN                (2U)    /* SVC_APP_STAT_VIN_s */
#define SVC_APP_STAT_VOUT               (3U)    /* SVC_APP_STAT_VOUT_s */
#define SVC_APP_STAT_STG                (4U)    /* SVC_APP_STAT_STG_s */
#define SVC_APP_STAT_LINUX              (5U)    /* SVC_APP_STAT_LINUX_s */
#define SVC_APP_STAT_CV_BOOT            (6U)    /* SVC_APP_STAT_CV_BOOT_s */
#define SVC_APP_STAT_CAMCTRL            (7U)    /* SVC_APP_STAT_CAMCTRL_s */
#define SVC_APP_STAT_DSP_BOOT           (8U)    /* SVC_APP_STAT_DSP_BOOT_s */
#define SVC_APP_STAT_MENU               (9U)    /* SVC_APP_STAT_MENU_s */
#define SVC_APP_STAT_BSD               (10U)    /* SVC_APP_STAT_BSD_s */
#define SVC_APP_STAT_OD                (11U)    /* SVC_APP_STAT_OD_s */
#define SVC_APP_STAT_DIRT_DETECT       (12U)    /* SVC_APP_STAT_DIRT_DETECT_s */
#define SVC_APP_STAT_DEFOG             (13U)    /* SVC_APP_STAT_DEFOG_s */
#define SVC_APP_STAT_EMR_VER           (14U)    /* SVC_APP_STAT_EMR_2_0_s */
#define SVC_APP_STAT_PSD               (15U)    /* SVC_APP_STAT_PSD_s */
#define SVC_APP_STAT_OWS               (16U)    /* SVC_APP_STAT_OWS_s */
#define SVC_APP_STAT_RCTA              (17U)    /* SVC_APP_STAT_RCTA_s */
#define SVC_APP_STAT_CALIB             (18U)    /* SVC_APP_STAT_CALIB_s */
#define SVC_APP_STAT_DISP              (19U)    /* SVC_APP_STAT_DISP_s */
#define SVC_APP_STAT_SHMOO_REC         (20U)    /* SVC_APP_STAT_SHMOO_REC_s */
#define SVC_APP_STAT_SHMOO_CV          (21U)    /* SVC_APP_STAT_SHMOO_CV_s */
#define SVC_APP_STAT_NUM               (22U)

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_UCODE_NOT_LOAD          (0U)
#define SVC_APP_STAT_UCODE_LIV_DONE          (1U)
#define SVC_APP_STAT_UCODE_ALL_DONE          (2U)
#define SVC_APP_STAT_UCODE_DEFBIN_DONE       (3U)
#define SVC_APP_STAT_UCODE_LOAD_STAGE1_DONE  (4U)
} SVC_APP_STAT_UCODE_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_IQ_NOT_READY       (0U)
#define SVC_APP_STAT_IQ_VID_READY       (1U)
#define SVC_APP_STAT_IQ_ALL_READY       (2U)
#define SVC_APP_STAT_IQ_IMGCFG_READY    (3U)
} SVC_APP_STAT_IQ_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_VIN_NOT_READY      (0U)
#define SVC_APP_STAT_VIN_READY          (1U)
#define SVC_APP_STAT_VIN_CONFIGING      (2U)
} SVC_APP_STAT_VIN_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_VOUT_NOT_READY     (0U)
#define SVC_APP_STAT_VOUT_READY         (1U)
#define SVC_APP_STAT_VOUT_CONFIGING     (2U)
} SVC_APP_STAT_VOUT_s;

typedef struct {
    UINT8 Status;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define SVC_APP_STAT_SD_NONE          (0x1U)
#define SVC_APP_STAT_SD_READY         (0x2U)
#define SVC_APP_STAT_SD_IDLE          (0x4U)
#else
#define SVC_APP_STAT_STG_C_READY        (0x1U)
#define SVC_APP_STAT_STG_D_READY        (0x2U)
#define SVC_APP_STAT_STG_I_READY        (0x4U)
#endif
} SVC_APP_STAT_STG_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_LINUX_NOT_READY    (0U)
#define SVC_APP_STAT_LINUX_READY        (1U)
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define SVC_APP_STAT_WIFI_READY         (2U)
#define SVC_APP_STAT_BT_READY           (4U)
#define SVC_APP_STAT_LTE_ON             (8U)
#define SVC_APP_STAT_LTE_READY          (16U)
#define SVC_APP_STAT_USB_WIFI_READY     (32U)
#endif
} SVC_APP_STAT_LINUX_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_CV_BOOT_NOT_READY  (0U)
#define SVC_APP_STAT_CV_BOOT_DONE       (1U)
#define SVC_APP_STAT_CV_BOOT_TIME_OUT   (2U)
#define SVC_APP_STAT_CV_BOOT_NONE       (3U)
} SVC_APP_STAT_CV_BOOT_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_CAMCTRL_NOT_READY  (0U)
#define SVC_APP_STAT_CAMCTRL_READY      (1U)
#define SVC_APP_STAT_CAMCTRL_NONE       (2U)
} SVC_APP_STAT_CAMCTRL_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_DSP_BOOT_NOT_READY      (0U)
#define SVC_APP_STAT_DSP_WAIT_UCODE_DONE     (1U)
#define SVC_APP_STAT_DSP_BOOT_DONE           (2U)
#define SVC_APP_STAT_DSP_BOOT_START          (3U)
} SVC_APP_STAT_DSP_BOOT_s;

typedef struct {
    UINT8 Vout;
#define SVC_APP_STAT_MENU_VOUT_A        (0U)
#define SVC_APP_STAT_MENU_VOUT_B        (1U)
    UINT8 Type;
#define SVC_APP_STAT_MENU_TYPE_EMR      (0U)
#define SVC_APP_STAT_MENU_TYPE_ADAS_DVR (1U)
#define SVC_APP_STAT_MENU_TYPE_DMS      (2U)
#define SVC_APP_STAT_MENU_TYPE_PLAYBACK (3U)
    UINT8 Operation;
#define SVC_APP_STAT_MENU_OP_MOVE       (0U)
#define SVC_APP_STAT_MENU_OP_ENTER      (1U)
#define SVC_APP_STAT_MENU_OP_MINUS      (2U)
#define SVC_APP_STAT_MENU_OP_PLUS       (3U)
    UINT8 FuncIdx;
    /* For EMR */
#define SVC_APP_STAT_MENU_EMR_MODE          (0U)
#define SVC_APP_STAT_MENU_EMR_RECORD        (1U)
#define SVC_APP_STAT_MENU_EMR_PAN           (2U)
#define SVC_APP_STAT_MENU_EMR_TILT          (3U)
#define SVC_APP_STAT_MENU_EMR_ROTATE        (4U)
#define SVC_APP_STAT_MENU_EMR_DIM           (5U)
#define SVC_APP_STAT_MENU_EMR_BSD           (6U)
#define SVC_APP_STAT_MENU_EMR_FORMAT        (7U)
#define SVC_APP_STAT_MENU_EMR_EMR2_0        (8U)
#define SVC_APP_STAT_MENU_EMR_OD            (9U)
#define SVC_APP_STAT_MENU_EMR_DEFOG         (10U)
#define SVC_APP_STAT_MENU_EMR_DIRT_DETECT   (11U)
#define SVC_APP_STAT_MENU_EMR_LAYOUT        (12U)
    /* For ADAS_DVR */
#define SVC_APP_STAT_MENU_ADAS_MODE         (0U)
#define SVC_APP_STAT_MENU_ADAS_RECORD       (1U)
#define SVC_APP_STAT_MENU_ADAS_PLAYBACK     (2U)
#define SVC_APP_STAT_MENU_ADAS_FCWS         (3U)
#define SVC_APP_STAT_MENU_ADAS_FCMD         (4U)
#define SVC_APP_STAT_MENU_ADAS_LDWS         (5U)
#define SVC_APP_STAT_MENU_ADAS_DMS          (6U)
#define SVC_APP_STAT_MENU_ADAS_WIFI         (7U)
#define SVC_APP_STAT_MENU_ADAS_OD           (8U)
#define SVC_APP_STAT_MENU_ADAS_DEFOG        (9U)
#define SVC_APP_STAT_MENU_ADAS_DIRT_DETECT  (10U)
    /* For DMS */
#define SVC_APP_STAT_MENU_DMS_MODE          (0U)
#define SVC_APP_STAT_MENU_DMS_RECORD        (1U)
#define SVC_APP_STAT_MENU_DMS_PLAYBACK      (2U)
#define SVC_APP_STAT_MENU_DMS_DMS           (3U)
    /* For PLAYBACK */
#define SVC_APP_STAT_MENU_PLAYBACK_MODE     (0U)
#define SVC_APP_STAT_MENU_PLAYBACK_PREV     (1U)
#define SVC_APP_STAT_MENU_PLAYBACK_NEXT     (2U)
#define SVC_APP_STAT_MENU_PLAYBACK_START    (3U)
#define SVC_APP_STAT_MENU_PLAYBACK_STOP     (4U)
#define SVC_APP_STAT_MENU_PLAYBACK_BACKWARD (5U)
#define SVC_APP_STAT_MENU_PLAYBACK_FORWARD  (6U)
} SVC_APP_STAT_MENU_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_BSD_DISABLE        (0U)
#define SVC_APP_STAT_BSD_ENABLE         (1U)
} SVC_APP_STAT_BSD_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_OD_ENABLE          (0U)
#define SVC_APP_STAT_OD_DISABLE         (1U)
} SVC_APP_STAT_OD_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_DIRT_DETECT_ENABLE     (0U)
#define SVC_APP_STAT_DIRT_DETECT_DISABLE    (1U)
} SVC_APP_STAT_DIRT_DETECT_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_DEFOG_ENABLE       (0U)
#define SVC_APP_STAT_DEFOG_DISABLE      (1U)
} SVC_APP_STAT_DEFOG_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_EMR_VER_1_0    (0U)
#define SVC_APP_STAT_EMR_VER_2_0    (1U)
} SVC_APP_STAT_EMR_VER_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_PSD_DISABLE        (0U)
#define SVC_APP_STAT_PSD_ENABLE         (1U)
} SVC_APP_STAT_PSD_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_OWS_DISABLE        (0U)
#define SVC_APP_STAT_OWS_ENABLE         (1U)
} SVC_APP_STAT_OWS_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_RCTA_DISABLE        (0U)
#define SVC_APP_STAT_RCTA_ENABLE         (1U)
} SVC_APP_STAT_RCTA_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_CALIB_NOT_READY      (0U)
#define SVC_APP_STAT_CALIB_LOAD_DONE      (1U)
} SVC_APP_STAT_CALIB_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_DISP_ON              (0U)
#define SVC_APP_STAT_DISP_OFF             (1U)
#define SVC_APP_STAT_DISP_CHG             (2U)
} SVC_APP_STAT_DISP_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_SHMOO_REC_OFF         (0U)
#define SVC_APP_STAT_SHMOO_REC_ON          (1U)
} SVC_APP_STAT_SHMOO_REC_s;

typedef struct {
    UINT8 Status;
#define SVC_APP_STAT_SHMOO_CV_OFF          (0U)
#define SVC_APP_STAT_SHMOO_CV_ON           (1U)

} SVC_APP_STAT_SHMOO_CV_s;





#endif  /* SVC_APP_STAT_H */
