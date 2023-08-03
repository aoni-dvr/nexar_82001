/**
*  @file SvcVinSrc.h
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
*  @details svc vin source
*
*/

#ifndef SVC_VIN_SRC_H
#define SVC_VIN_SRC_H

#define SVC_VIN_SRC_SENSOR      (0U)    /* Vin-Sensor input */
#define SVC_VIN_SRC_YUV         (1U)    /* Vin-Yuv input */
#define SVC_VIN_SRC_MEM         (2U)    /* Mem-raw input */
#define SVC_VIN_SRC_MEM_YUV422  (3U)    /* Mem-Yuv422 input */
#define SVC_VIN_SRC_MEM_YUV420  (4U)    /* Mem-Yuv420 input */
#define SVC_VIN_SRC_MEM_DEC     (5U)    /* Mem-decoder input */

typedef void (*SVC_VIN_SRC_INIT_CB)(void);

typedef struct {
    UINT32  VinID;
    UINT32  SrcType;
    UINT32  SrcBits;
    UINT32  SrcMode;
    UINT32  SrcMode_1;  /* For virtual channel_1 */
    UINT32  SrcMode_2;  /* For virtual channel_2 */
    UINT32  SrcMode_3;  /* For virtual channel_3 */
    UINT32  SensorBits;
    UINT32  DisableMasterSync;  /* Master-Sync Disable. If 1, the sensor will NOT enable Master-Sync (If driver support) */
    UINT32  VsyncDelay;         /* Take effect only when driver and hardware support */
} SVC_VIN_SRC_CFG_s;

typedef struct {
    UINT32             IsActive;
    void               *pSrcObj;
    SVC_VIN_SRC_CFG_s  Config;
    void               *pEEPROBObj;
} SVC_VIN_SRC_s;

#ifdef CONFIG_ICAM_PARALLEL_VIN_CONFIG
typedef struct {
    UINT32 State;
#define SVC_VIN_SRC_CFG_DONE (1U)
    UINT32 TaskPriority;
    UINT32 TaskCpuBits;
} SVC_VIN_SRC_TSK_INFO_s;
#endif

typedef struct {
    UINT32                 CfgNum;
    SVC_VIN_SRC_s          InitCfgArr[AMBA_DSP_MAX_VIN_NUM];
#ifdef CONFIG_ICAM_PARALLEL_VIN_CONFIG
    SVC_VIN_SRC_TSK_INFO_s CfgTaskInfo[AMBA_DSP_MAX_VIN_NUM];
#endif
    SVC_VIN_SRC_INIT_CB    pInitDoneCB;
} SVC_VIN_SRC_INIT_s;

void   SvcVinSrc_Init(const SVC_VIN_SRC_INIT_s *pInit, UINT32 Priority, UINT32 CpuBits);
void   SvcVinSrc_DeInit(const SVC_VIN_SRC_INIT_s *pInit);
UINT32 SvcVinSrc_Config(const UINT32 CfgNum, const SVC_VIN_SRC_CFG_s *pCfgArr);
UINT32 SvcVinSrc_Enable(const UINT32 CfgNum, const SVC_VIN_SRC_CFG_s *pCfgArr);
UINT32 SvcVinSrc_Disable(const UINT32 CfgNum, const SVC_VIN_SRC_CFG_s *pCfgArr);

#endif  /* SVC_VIN_SRC_H */
