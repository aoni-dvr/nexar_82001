/**
*  @file SvcBuffer.h
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
*  @details svc buffer operation
*
*/

#ifndef SVC_BUFFER_H
#define SVC_BUFFER_H

#define SVC_BUFFER_FIXED    (0U)
#define SVC_BUFFER_SHARED   (1U)

void   SvcBuffer_Init(SVC_MEM_MAP_s *pFixedMM);
void   SvcBuffer_Config(UINT32 SharedMMNum, SVC_MEM_MAP_s *pSharedMMArr);
UINT32 SvcBuffer_Request(UINT32 MapType, UINT32 MemId, ULONG *pBase, UINT32 *pSize);
UINT32 SvcBuffer_LockFreeSpace(UINT32 MemPoolType, ULONG *pBase, UINT32 *pSize);
UINT32 SvcBuffer_UnLockFreeSpace(UINT32 MemPoolType);

UINT32 SvcBuffer_CheckCached(ULONG Addr, UINT32 Size);
UINT32 SvcBuffer_GetMemPoolType(ULONG Addr, UINT32 Size);
UINT32 SvcBuffer_Vir2Phys(ULONG VirtAddr, ULONG *pPhysAddr);
void   SvcBuffer_Show(void);

#endif  /* SVC_BUFFER_H */
