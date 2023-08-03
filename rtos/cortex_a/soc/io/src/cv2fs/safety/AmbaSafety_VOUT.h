/**
 *  @file AmbaSafety_VOUT.h
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for VOUT Safety driver APIs
 *
 */

#ifndef AMBA_SAFETY_VOUT_H
#define AMBA_SAFETY_VOUT_H

extern UINT32 AmbaSafety_VoutInit(UINT32 VoutID);
extern UINT32 AmbaSafety_VoutDeInit(UINT32 VoutID);

extern UINT32 AmbaSafety_VoutChecker(UINT32 *pModuleID, UINT32 *pInstanceID, UINT32 *pApiID, UINT32 *pErrorID);
extern UINT32 AmbaSafety_VoutSetSafeState(UINT32 VoutID, UINT32 State);
extern UINT32 AmbaSafety_VoutGetSafeState(UINT32 VoutID, UINT32 *pState);

extern void AmbaSafety_VoutShadowLock(void);
extern void AmbaSafety_VoutShadowUnLock(void);
extern void AmbaSafety_VoutShadowDispCfg(UINT32 VoutID, const void *pBuf);
extern void AmbaSafety_VoutShadowDispRst(UINT32 VoutID);
extern void AmbaSafety_VoutShadowDsiPhyCfg(UINT32 VoutID, const void *pBuf);
extern void AmbaSafety_VoutShadowDsiPhyRst(UINT32 VoutID);

#endif /* AMBA_SAFETY_VOUT_H */
