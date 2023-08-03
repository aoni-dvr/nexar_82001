/**
*  @file SvcDrawStopwatch.h
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
*  @details svc draw stopwatch
*
*/

#ifndef SVC_DRAW_STP_WCH_H
#define SVC_DRAW_STP_WCH_H

#define SVC_DRAW_STOPWATCH_SEC_IDX   0U
#define SVC_DRAW_STOPWATCH_MIN_IDX   1U
#define SVC_DRAW_STOPWATCH_HOUR_IDX  2U

typedef struct {
    UINT16   Width;           /* Number of pixels per line in the window */
    UINT16   Height;          /* Number of lines in the window */
    ULONG    BaseAddrY;       /* Luma (Y) data buffer address */
    ULONG    BaseAddrUV;      /* Chroma (UV) buffer address */
    ULONG    BaseAddrAY;      /* Alpha Y data buffer address */
    ULONG    BaseAddrAUV;     /* Alpha UV data buffer address */
    UINT16   Pitch;           /* YUV data buffer pitch */
}SVC_DRAW_STOPWATCH_BUF_INFO;

typedef struct {
    SVC_DRAW_STOPWATCH_BUF_INFO Hour;
    SVC_DRAW_STOPWATCH_BUF_INFO Min;
    SVC_DRAW_STOPWATCH_BUF_INFO Sec;
    ULONG                       BufBase;
    UINT32                      BufSize;
}SVC_DRAW_STOPWATCH_INFO;

void SvcDrawStopwatch_Init(void);
void SvcDrawStopwatch_InfoGet(UINT32 StreamId, SVC_DRAW_STOPWATCH_INFO *pInfo, const UINT32 RecBlend);
void SvcDrawStopwatch_BufUpdate(UINT16 StreamId, UINT16 BlendAreaIdx, UINT32 *pAction);

void SvcDrawStopwatch_EvalMemSize(UINT32 *pNeedSize);
void SvcDrawStopwatch_TimerReset(UINT32 StreamBits);

#endif  /* SVC_DRAW_STP_WCH_H */
