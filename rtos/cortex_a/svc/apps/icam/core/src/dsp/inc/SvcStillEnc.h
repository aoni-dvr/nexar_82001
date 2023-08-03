/**
*  @file SvcStillEnc.h
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

#ifndef SVC_STL_ENC_H
#define SVC_STL_ENC_H

#define SVC_LOG_STL_ENC        "STL_ENC"
#define SVC_STL_ENC_STACK_SIZE (0x4000U)

typedef struct {
    UINT32   Priority;      /* scanning task priority */
    UINT32   CpuBits;       /* core selection which scanning task running at */
    ULONG    QAddr;
} SVC_STL_ENC_CONFIG_s;

typedef struct {
    UINT16             RawSeq;
    UINT16             PicView;
    UINT8              PicType;
    UINT8              Reserved[3];
    SVC_YUV_IMG_BUF_s  YuvBufInfo;
} SVC_STL_ENC_INFO_s;

UINT32 SvcStillEnc_Create(const SVC_STL_ENC_CONFIG_s *pCfg);
UINT32 SvcStillEnc_Delete(void);
void   SvcStillEnc_Debug(UINT8 On);
UINT32 SvcStillEnc_SetStreamIndex(UINT16 StreamIdx); /* set dsp encoder stream id for JPEG */
UINT32 SvcStillEnc_SetJpegBufer(ULONG BitsBufAddr, UINT32 BitsBufSize);
UINT32 SvcStillEnc_InputYuv(const SVC_STL_ENC_INFO_s *pInfo);
UINT32 SvcStillEnc_DummyInput(UINT16 PicView);
UINT32 SvcStillEnc_WaitJpegDone(void);

#endif  /* SVC_STL_ENC_H */
