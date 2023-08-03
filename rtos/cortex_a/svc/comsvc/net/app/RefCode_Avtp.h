/**
 *  @file RefCode_Avtp.h
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details header for avb refcode
 *
 */


#ifndef AMBA_AVB_REFCODE_H
#define AMBA_AVB_REFCODE_H
#ifdef CONFIG_AMBA_AVB
#include "AmbaFIFO.h"


#define MAX_TALKER_NUM 2U
#define MAX_LISTENER_NUM 2U

typedef struct {
    AMBA_FIFO_HDLR_s *pFifoHdlr;
    UINT8 *pBuffer;
    UINT32 BufferSize;
    AMBA_FIFO_HDLR_s *pVirtFifoHdlr;
} AMBA_AVB_AVTP_FIFO_s;

extern AMBA_AVB_AVTP_FIFO_s FifoConfig[MAX_TALKER_NUM];
extern AMBA_AVB_AVTP_TALKER_s TalkerConfig[MAX_TALKER_NUM];
extern AMBA_AVB_AVTP_LISTENER_s ListenerConfig[MAX_LISTENER_NUM];
extern UINT8 InitFifoConfig;
extern UINT8 InitTalkerConfig;
extern UINT8 InitListenerConfig;

void AvtpTalker_FpsControl(UINT32 Id, UINT32 Fps);
void AvtpTalkerStop(UINT32 Id);
void AvtpTalkerStart(UINT32 Id);
void AvtpListenerStart(UINT32 Id);
void AvtpListenerStop(UINT32 Id);
#endif /* CONFIG_AMBA_AVB */

#endif /* AMBA_AVB_REFCODE_H */
