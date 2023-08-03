/**
 *  @file SvcCvFileInTask.h
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
 *  @details Header of svc cv file input for OD (Header of SvcCvFileInTask.h)
 *
 */

#ifndef SVC_CV_FILE_IN_TASK_H
#define SVC_CV_FILE_IN_TASK_H

/**
 * @ Usage
 * @ Define SVC_CV_FLOW_REF_OD and REFOD_MIO_0_0_EXT_YUV in g_ResCfg.CvFlow
 * @ Put SuperDAG and cvexec.ash at SD card.
 * @ Put Y file(*.y) and UV file(*.uv) at SD card. The two files should have the same name.
 * @ svc_app cv fin config [CvFlowChan] [Width] [Height] [Pitch]
 * @ svc_app cv fin start [FilePathOfY]
 * @ The output will be generated at FilePathOfY_out.y and FilePathOfY_out.uv
 */

#define SVC_CV_FILE_IN_MAX_INPUT_PER_CHAN       (2U)
#define SVC_CV_FILE_IN_MAX_DATA_PER_INPUT       (6U)
#define SVC_CV_FILE_IN_MAX_FILE_PATH            (64U)

#define SVC_CV_FILE_IN_SET_INPUT_INFO           (1U)        /* SVC_CV_FILE_IN_INPUT_INFO_s */
#define SVC_CV_FILE_IN_SET_FILE_INFO            (2U)        /* SVC_CV_FILE_IN_FILE_INFO_s */
#define SVC_CV_FILE_IN_SET_BATCH_FILE_INFO      (3U)        /* (Batch mode) Enable Batch mode, and configure input and output directory. SVC_CV_FILE_IN_NAMING_CONFIG_s */
#define SVC_CV_FILE_IN_SET_OUPUT_INFO           (4U)        /* (Batch mode) Configure naming of outputs. SVC_CV_FILE_IN_NAMING_CONFIG_s */

typedef struct {
    UINT32  CvFlowChan;
    UINT32  Mode;
#define SVC_CV_FILE_IN_YUV_MODE                 (0x0U)
#define SVC_CV_FILE_IN_RAW_MODE                 (0x1U)
} SVC_CV_FILE_IN_CONFIG_s;

typedef struct {
    UINT32  Width;
    UINT32  Height;
    UINT32  Pitch;
    UINT32  ScaleIdx;
} SVC_CV_FILE_IN_IMG_INFO_s;

typedef struct {
    SVC_CV_FILE_IN_IMG_INFO_s Img;
} SVC_CV_FILE_IN_DATA_INFO_s;

typedef struct {
    UINT32  InputIdx;                                       /* Entry Index of SVC_CV_FLOW_s.FovInput[] */
    UINT32  DataIdx;
    UINT32  DataType;
    SVC_CV_FILE_IN_DATA_INFO_s DataInfo;
} SVC_CV_FILE_IN_INPUT_INFO_s;

typedef struct {
    UINT32  InputIdx;                                       /* Entry Index of SVC_CV_FLOW_s.FovInput[] */
    UINT32  DataIdx;
    char    FileName[SVC_CV_FILE_IN_MAX_FILE_PATH];
} SVC_CV_FILE_IN_FILE_INFO_s;

typedef struct {
    UINT32  Reserved;
} SVC_CV_FILE_IN_START_s;

typedef struct {
    UINT32 MaxNumber;
    char FileNameBuf[16U][32U];
} SVC_CV_FILE_IN_NAMING_CONFIG_s;


UINT32 SvcCvFileInTask_Init(void);
UINT32 SvcCvFileInTask_Config(const SVC_CV_FILE_IN_CONFIG_s *pConfig);
UINT32 SvcCvFileInTask_SetInfo(UINT32 Type, void *pInfo);
UINT32 SvcCvFileInTask_Start(const SVC_CV_FILE_IN_START_s *pStartCfg);
UINT32 SvcCvFileInTask_ConfigOutput(const SVC_CV_FILE_IN_NAMING_CONFIG_s *pNameCfg);
UINT32 SvcCvFileInTask_StartMultiple(const SVC_CV_FILE_IN_START_s *pStartCfg);

#endif /* SVC_CV_FILE_IN_TASK_H */