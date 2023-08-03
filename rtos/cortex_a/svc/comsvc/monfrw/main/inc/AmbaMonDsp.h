/**
 *  @file AmbaMonDsp.h
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants and Definitions for Amba Monitor DSP
 *
 */

#ifndef AMBA_MONITOR_DSP_H
#define AMBA_MONITOR_DSP_H

UINT32 AmbaMonDsp_RawRdy(const void *pEvent);
UINT32 AmbaMonDsp_YuvRdy(const void *pEvent);
UINT32 AmbaMonDsp_DispYuvRdy(const void *pEvent);
UINT32 AmbaMonDsp_VoutYuvRdy(const void *pEvent);
UINT32 AmbaMonDsp_CfaAaaRdy(const void *pEvent);
UINT32 AmbaMonDsp_RgbAaaRdy(const void *pEvent);
UINT32 AmbaMonDsp_FovDelay(const void *pEvent);
UINT32 AmbaMonDsp_VoutDelay(const void *pEvent);

UINT32 AmbaMonDsp_RawTimeout(const void *pEvent);
UINT32 AmbaMonDsp_YuvTimeout(const void *pEvent);
UINT32 AmbaMonDsp_DispYuvTimeout(const void *pEvent);
UINT32 AmbaMonDsp_VoutYuvTimeout(const void *pEvent);
UINT32 AmbaMonDsp_CfaAaaTimeout(const void *pEvent);
UINT32 AmbaMonDsp_RgbAaaTimeout(const void *pEvent);
UINT32 AmbaMonDsp_FovDelayTimeout(const void *pEvent);
UINT32 AmbaMonDsp_VoutDelayTimeout(const void *pEvent);

UINT32 AmbaMonDsp_YuvAltTimeout(const void *pEvent);
UINT32 AmbaMonDsp_VinTimeout(const void *pEvent);

extern UINT32 AmbaMonDsp_RawSeqNum[AMBA_MON_NUM_VIN_CHANNEL];
extern UINT32 AmbaMonDsp_CfaRawSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
extern UINT32 AmbaMonDsp_RgbRawSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
extern UINT32 AmbaMonDsp_YuvSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
extern UINT32 AmbaMonDsp_YuvAltSeqNum[AMBA_MON_NUM_FOV_CHANNEL];
extern UINT32 AmbaMonDsp_DispYuvSeqNum[AMBA_MON_NUM_DISP_CHANNEL];
extern  ULONG AmbaMonDsp_VoutBaseAddrY[AMBA_MON_NUM_VOUT_CHANNEL];

#endif  /* AMBA_MONITOR_DSP_H */
