/**
 * @file AmbaLink_core.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details Local private header for core of AmbaLink.
 *
 */

#ifndef AMBALINK_CORE_H
#define AMBALINK_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

extern void AmbaLink_EntryPoint(void);
extern void AmbaLink_RpmsgInitDone(void);

extern UINT32 AmbaIPC_crc32(const void *buf, SIZE_t size);

extern UINT32 AmbaIPC_INTDisable(void);
extern UINT32 AmbaIPC_INTEnable(void);

extern UINT32 AmbaIPC_MutexInit(void);
extern UINT32 AmbaIPC_OWNER_IS_LOCAL(UINT32 id);

extern UINT32 AmbaIPC_SpinInit(void);
extern UINT32 do_SpinLockIrqSave(UINT32 *lock, UINT32 *pFlags);
extern UINT32 do_SpinUnlockIrqRestore(UINT32 *lock, UINT32 Flags);

#ifdef __cplusplus
}
#endif

#endif /* AMBALINK_CORE_H */

