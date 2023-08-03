/**
*  @file SvcDSP.h
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
*  @details svc dsp control
*
*/

#ifndef SVC_DSP_H
#define SVC_DSP_H

#define SVC_FRAME_TYPE_YUV      (0U)
#define SVC_FRAME_TYPE_RAW      (1U)

typedef struct {
    UINT32 FrameNum;
    UINT32 FrameW;
    UINT32 FrameH;
    UINT8  FrameType;          /* SVC_FRAME_TYPE_YUV - yuv capture, SVC_FRAME_TYPE_RAW - raw capture */
    UINT16 RawCmpr;
    UINT8  CeNeeded;         /* CE buffer needed for HDR sensor mode */
    UINT8  Rsvd;
} SVC_DSP_FRAME_s;

UINT32 SvcDSP_Boot(const AMBA_DSP_SYS_CONFIG_s *pCfg, UINT32 MsgTaskPriority, UINT32 MsgTaskCpuBits);
UINT8  SvcDSP_IsBootDone(void);
void   SvcDSP_PackVecProcCfg(AMBA_DSP_SYS_CONFIG_s *pCfg, ULONG VpMsgBase, UINT32 VpMSgSize);
UINT32 SvcDSP_QueryFrameBufSize(const SVC_DSP_FRAME_s *pSetup, UINT32 *pMemSize);
UINT32 SvcDSP_DataCapCtrl(UINT16 NumCapInstance, const UINT16 *pCapInstance, void *pDataCapCtrl, UINT64 *pAttachedRawSeq);

#ifdef CONFIG_ICAM_DSP_SUSPEND
UINT32 SvcDSP_Suspend(void);
UINT32 SvcDSP_Resume(void);
#endif

#define SVCDSP_OP_INVALID       (0xFFFFFFFFU)
#define SVCDSP_OP_DIRTYBOOT     (0x0U)
#define SVCDSP_OP_CLEANBOOT     (0x1U)
#define SVCDSP_OP_DONTCARE      (0x2U)
UINT32 SvcDSP_IsCleanBoot(void);

void   SvcDSP_DspMsgTaskCreate(UINT32 MsgTaskPriority, UINT32 MsgTaskCpuBits);

#endif  /* SVC_DSP_H */
