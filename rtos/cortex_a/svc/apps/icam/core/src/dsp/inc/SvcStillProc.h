/**
*  @file SvcStillProc.h
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
*
*/

#ifndef SVC_STL_PROC_H
#define SVC_STL_PROC_H

#define SVC_LOG_STL_PROC        "STL_PROC"
#define SVC_STL_PROC_STACK_SIZE (0x10000U)

#ifdef CONFIG_SOC_H22
#define YUV_ALIGN_BYTE  (32)
#else
#define YUV_ALIGN_BYTE  (64)
#endif

typedef struct {
    UINT32   MaxMainYuvW;
    UINT32   MaxMainYuvH;
    UINT16   ScrnYuvW;
    UINT16   ScrnYuvH;
    UINT32   QviewYuvW;
    UINT32   QviewYuvH;
} SVC_STL_PROC_SETUP_s;

typedef struct {
    UINT32   Priority;      /* scanning task priority */
    UINT32   CpuBits;       /* core selection which scanning task running at */
    ULONG    WorkBufAddr;
    UINT32   WorkBufSize;
    UINT32   IkCtxId;
} SVC_STL_PROC_CONFIG_s;

typedef struct {
    UINT16             RawSeq;
    UINT16             CapInstance;
    UINT16             OutMainW;
    UINT16             OutMainH;
    UINT16             OutScrnW;
    UINT16             OutScrnH;
    UINT16             OutThmbW;
    UINT16             OutThmbH;
    UINT8              PicType;
    UINT8              StreamId;
    UINT8              RestartLiveview;
    UINT8              SensorNum;       /* sensor sequence number for SvcImg */
    SVC_CAP_MSG_s      InputInfo;
    SVC_STILL_QVIEW_CFG_s Qview;
} SVC_STL_PROC_INFO_s;

UINT32 SvcStillProc_QueryMem(const SVC_STL_PROC_SETUP_s *pSetup, UINT32 *pMemSize);
UINT32 SvcStillProc_Create(const SVC_STL_PROC_CONFIG_s *pCfg);
UINT32 SvcStillProc_Delete(void);
void   SvcStillProc_Debug(UINT8 On);
void   SvcStillProc_SetPipe(UINT8 FixPipe, UINT8 FixHiso);
void   SvcStillProc_SetNumCap(UINT32 NumCap);
void   SvcStillProc_SetMaxYuvInputWidth(UINT16 MaxYuvInputWidth);
UINT32 SvcStillProc_InputData(const SVC_STL_PROC_INFO_s *pInfo);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
UINT32 SvcStillProc_SetIkTile(const AMBA_IK_MODE_CFG_s *pImgMode, UINT16 SliceType, UINT16 ImgInWidth, UINT16 ImgInHeight, UINT16 ImgOutWidth, UINT16 ImgOutHeight);
#endif

#define SVC_STL_TEST_PAUSE_AFTER_R2Y  (1U)

void   SvcStillProc_SetTestFlag(UINT8 Flag);

#endif  /* SVC_STL_PROC_H */
