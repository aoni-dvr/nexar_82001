/**
 *  @file AmbaSafety_VIN.h
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
 *  @details Definitions & Constants for VIN Safety driver APIs
 *
 */

#ifndef AMBA_SAFETY_VIN_H
#define AMBA_SAFETY_VIN_H

extern UINT32 AmbaSafety_VinInit(UINT32 VinID);
extern UINT32 AmbaSafety_VinChecker(UINT32 *pModuleID, UINT32 *pInstanceID, UINT32 *pApiID, UINT32 *pErrorID);
extern UINT32 AmbaSafety_VinSetSafeState(UINT32 VinID, UINT32 State);
extern UINT32 AmbaSafety_VinGetSafeState(UINT32 VinID, UINT32 *pState);

extern void AmbaSafety_VinShadowLock(void);
extern void AmbaSafety_VinShadowUnLock(void);
extern void AmbaSafety_VinShadowWrite(UINT32 VinID, volatile UINT32 *pAddr, UINT32 Value);
extern void AmbaSafety_VinShadowMainCfg(UINT32 VinID, const void *pBuf);
extern void AmbaSafety_VinShadowPhyCfg(UINT32 VinID, const void *pBuf);
extern void AmbaSafety_VinShadowMainRst(UINT32 VinID);

#endif /* AMBA_SAFETY_VIN_H */
