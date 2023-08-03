/**
 *  @file SvcVoutSrc.h
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
 *  @details svc vout src functions
 *
 */

#ifndef SVC_VOUT_SRC_H
#define SVC_VOUT_SRC_H

#define SVC_HDMI_VIC_MASK       (0x7FFFU)

typedef void (*SVC_VOUT_SRC_INIT_CB)(void);

typedef struct {
    UINT32  VoutID;
    UINT32  DevType;
    UINT32  DevMode;

    /* FPD device */
    AMBA_FPD_OBJECT_s  *pFpdObj;
} SVC_VOUT_DEV_s;

typedef struct {
    UINT32          NumInit;
    SVC_VOUT_DEV_s  Dev[AMBA_DSP_MAX_VOUT_NUM];
    SVC_VOUT_SRC_INIT_CB pInitDoneCB;
} SVC_VOUT_SRC_INIT_s;

void SvcVoutSrc_Init(const SVC_VOUT_SRC_INIT_s *pInit, UINT32 EnableFpd, UINT32 Priority, UINT32 CpuBits);
void SvcVoutSrc_DeInit(const SVC_VOUT_SRC_INIT_s *pInit, UINT32 DisableFpd);
void SvcVoutSrc_Ctrl(UINT32 Enable, UINT32 IsModeChg);

#endif /* SVC_VOUT_SRC_H */