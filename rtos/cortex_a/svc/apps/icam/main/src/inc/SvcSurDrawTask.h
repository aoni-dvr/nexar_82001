/**
 *  @file SvcSurDrawTask.h
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
 *  @details svc sur adas draw task header
 *
 */



#define SVC_PSD_DRAW   (0U)
#define SVC_OWS_DRAW   (1U)
#define SVC_RCTA_DRAW  (2U)

#ifdef CONFIG_ICAM_32BITS_OSD_USED
#define SUR_DRAW_RED          0xFFFF0000U
#define SUR_DRAW_YELLOW       0xFFFFFF00U
#define SUR_DRAW_ORANGE       0xFFFF8000U
#define SUR_DRAW_GREEN        0xFF00FF00U
#else
#define SUR_DRAW_RED          158U
#define SUR_DRAW_YELLOW       251U
#define SUR_DRAW_ORANGE       159U
#define SUR_DRAW_GREEN        250U
#endif


UINT32 SvcSurDrawTask_Init(void);

UINT32 SvcSurDrawTask_Start(void);

UINT32 SvcSurDrawTask_DrawMsg(UINT32 MsgCode, const void *pInfo);

void SvcSurDrawTask_ClearOsd(void);


