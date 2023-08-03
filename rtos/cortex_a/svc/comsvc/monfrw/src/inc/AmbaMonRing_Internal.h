/**
 *  @file AmbaMonRing_Internal.h
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
 *  @details Constants and Definitions for Amba Monitor Ring Internal
 *
 */

#ifndef AMBA_MON_RING_INTERNAL_H
#define AMBA_MON_RING_INTERNAL_H

typedef struct /*_AMBA_MON_RING_s_*/ {
    AMBA_KAL_MUTEX_t      Mutex;
    AMBA_MON_RING_ID_s    *pIdList;
} AMBA_MON_RING_s;

typedef union /*_AMBA_MON_RING_MEM_u_*/ {
    UINT32  Data;
    void    *pVoid;
    UINT8   *pUint8;
    UINT16  *pUint16;
    UINT32  *pUint32;
} AMBA_MON_RING_MEM_u;

typedef struct /*_AMBA_MON_RING_MEM_s_*/ {
    AMBA_MON_RING_MEM_u    Ctx;
} AMBA_MON_RING_MEM_s;

UINT32 AmbaMonRing_Create(AMBA_MON_RING_ID_s *pRingId, const char *pName, const AMBA_MON_RING_CHUNK_INFO_s *pChunkInfo);
UINT32 AmbaMonRing_Reset(AMBA_MON_RING_ID_s *pRingId);
UINT32 AmbaMonRing_Delete(AMBA_MON_RING_ID_s *pRingId);

UINT32 AmbaMonRing_Open(AMBA_MON_RING_PORT_s *pPort, const char *pName);
 void *AmbaMonRing_Write(AMBA_MON_RING_PORT_s *pPort, const void *pData, UINT32 Size);
 void *AmbaMonRing_Write2(AMBA_MON_RING_PORT_s *pPort, const void *pHeader, UINT32 HSize, const void *pData, UINT32 DSize);
UINT32 AmbaMonRing_RSeek(AMBA_MON_RING_PORT_s *pPort, UINT32 Offset, UINT32 Flag);
UINT32 AmbaMonRing_Read(AMBA_MON_RING_PORT_s *pPort, void **pData);
UINT32 AmbaMonRing_Close(AMBA_MON_RING_PORT_s *pPort);

#endif /* AMBA_MONITOR_RING_INTERNAL_H */
