/**
 *  @file SvcStillCap.h
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
 *  @details svc application command functions
 *
 */

#ifndef SVC_STILL_CAP_H
#define SVC_STILL_CAP_H

#define SVC_LOG_STL_CAP        "STL_CAP"
#define SVC_STL_CAP_STACK_SIZE (0x8000U)

#define SVC_STL_INIT          (0x1U)
#define SVC_STL_CAP_BUSY      (0x2U)
#define SVC_STL_PROC_BUSY     (0x4U)
#define SVC_STL_ENC_BUSY      (0x8U)
#define SVC_STL_MUX_BUSY      (0x10U)
#define SVC_STL_QV_BUSY       (0x20U)

// #define SVC_STL_ENABLE_CLEAN_RAW

typedef struct {
    UINT32   Priority;      /* scanning task priority */
    UINT32   CpuBits;       /* core selection which scanning task running at */
    char     StorageDrive;  /* storage drive */
    UINT32   MinStorageSize;/* minimum required storage size */
} SVC_CAP_TSK_s;

typedef struct {
    UINT16 CapW;             /* VIN capture size */
    UINT16 CapH;
    UINT16 EffectW;          /* Effective size to process */
    UINT16 EffectH;
    UINT8  Cmpr;             /* raw output compression */
    UINT8  CapSensorModeID;
    UINT8  ChanMask;
    UINT8  ForceCapCe;       /* Not for DOL capture mode. 0 - do nothing, 1 - force capture with CE, 2 - force capture without CE */
    AMBA_R2Y_WINDOW_s   ChanWin[SVC_CAP_MAX_SENSOR_PER_VIN];
    UINT8  IsHiso[SVC_CAP_MAX_SENSOR_PER_VIN];
} SVC_STILL_CAP_VININFO_s;

typedef struct {
    ULONG  BufferAddr;
    UINT32 CapNum;
    UINT32 AebNum;          /* should be 0 or the same the CapNum. maximum SVC_STL_MAX_AEB_NUM */
    UINT16 StrmMsk;         /* yuv stream mask for CapType = SVC_CAP_TYPE_YUV, or vin mask for CapType = SVC_CAP_TYPE_RAW */
    UINT8  PicType;         /* 0 - normal picture. 1 - video thumbnail. 2 - dump YUV. */
    UINT8  SyncEncStart;
    UINT8  CapOnly;
    UINT8  CapType;         /* SVC_CAP_TYPE_YUV - yuv capture, SVC_CAP_TYPE_RAW - raw capture */
    UINT8  StopLiveview;
    UINT8  Rsvd;
    UINT16 TimelapseMsk;
} SVC_CAP_CFG_s;

typedef struct {
    SVC_CAP_CFG_s           Cfg;
    SVC_PIV_QVIEW_CFG_s     PivQview; /* quick view configuration */
    SVC_STILL_QVIEW_CFG_s   Qview;
    SVC_STILL_CAP_VININFO_s Vin[AMBA_DSP_MAX_VIN_NUM];
} SVC_CAP_CTRL_s;

UINT32 SvcStillCap_Create(const SVC_CAP_TSK_s *pCfg);
UINT32 SvcStillCap_Delete(void);
void   SvcStillCap_Debug(UINT8 On);
UINT32 SvcStillCap_CapStart(const SVC_CAP_CTRL_s *pCapCtrl);
void   SvcStillCap_SetOutputSize(UINT8 SetOutputSize, UINT16 StreamID, UINT16 Width, UINT16 Height);
UINT32 SvcStillCap_ImgStop(void);
void   SvcStillCap_GetStatus(UINT32 *pStatus);
void   SvcStillCap_SetStatus(UINT32 Status);
void   SvcStillCap_ClearStatus(UINT32 Status);
UINT32 SvcStillCap_GetCleanRawDump(void);
void   SvcStillCap_SetQview(const AMBA_DSP_YUV_IMG_BUF_s *pYuv, UINT8 VoutId);

#endif  /* SVC_STILL_CAP_H */

