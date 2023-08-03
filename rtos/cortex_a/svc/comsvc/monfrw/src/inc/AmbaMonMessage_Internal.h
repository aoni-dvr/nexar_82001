/**
 *  @file AmbaMonMessage_Internal.h
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
 *  @details Constants and Definitions for Amba Monitor Message Internal
 *
 */

#ifndef AMBA_MONITOR_MESSAGE_INTERNAL_H
#define AMBA_MONITOR_MESSAGE_INTERNAL_H

#define AMBA_MON_MEM_ADDR    ULONG

typedef struct /*_AMBA_MON_MESSAGE_s_*/ {
    AMBA_KAL_MUTEX_t         Mutex;
    AMBA_MON_MESSAGE_ID_s    *pIdList;
} AMBA_MON_MESSAGE_s;

typedef union /*_AMBA_MON_MESSAGE_MEM_u_*/ {
    AMBA_MON_MEM_ADDR            Data;
    void                         *pVoid;
    UINT8                        *pUint8;
    UINT16                       *pUint16;
    UINT32                       *pUint32;
    AMBA_MON_MESSAGE_PORT_s      *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s    *pMsgHead;
} AMBA_MON_MESSAGE_MEM_u;

typedef struct /*_AMBA_MON_MESSAGE_MEM_s_*/ {
    AMBA_MON_MESSAGE_MEM_u    Ctx;
} AMBA_MON_MESSAGE_MEM_s;

#endif /* AMBA_MON_MESSAGE_INTERNAL_H */
