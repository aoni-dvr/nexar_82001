/**
 *  @file AmbaMonEvent.h
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
 *  @details Constants and Definitions for Amba Monitor Event
 *
 */

#ifndef AMBA_MONITOR_EVENT_H
#define AMBA_MONITOR_EVENT_H

#define MON_EVENT_FLAG_OR_CLR        0
#define MON_EVENT_FLAG_AND_CLR       1

typedef union /*_AMBA_MON_EVENT_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 AndOrClr:     1;
        UINT32 Reserved:    31;
    } Bits;
} AMBA_MON_EVENT_OP_u;

typedef struct /*_AMBA_MON_EVENT_OP_s_*/ {
    AMBA_MON_EVENT_OP_u    Ctx;
} AMBA_MON_EVENT_OP_s;

typedef struct /*_AMBA_MON_EVENT_SEM_s_*/ {
    AMBA_KAL_SEMAPHORE_t    Req;
    AMBA_KAL_SEMAPHORE_t    Ack;
} AMBA_MON_EVENT_SEM_s;

typedef struct /*_AMBA_MON_EVENT_PORT_FLAG_s_*/ {
    UINT64  Req;
    UINT64  Acc;
    UINT64  Ret;
} AMBA_MON_EVENT_PORT_FLAG_s;

typedef struct /*_AMBA_MON_EVENT_PORT_LINK_s_*/ {
    struct AMBA_MON_EVENT_PORT_s_    *Up;
    struct AMBA_MON_EVENT_PORT_s_    *Down;
} AMBA_MON_EVENT_PORT_LINK_s;

typedef struct AMBA_MON_EVENT_PORT_s_ {
    UINT32                         Magic;
    AMBA_KAL_MUTEX_t               Mutex;
    AMBA_MON_EVENT_OP_u            Op;
    AMBA_MON_EVENT_SEM_s           Sem;
    AMBA_MON_EVENT_PORT_FLAG_s     Flags;
    struct AMBA_MON_EVENT_ID_s_    *pEventId;
    AMBA_MON_EVENT_PORT_LINK_s     Link;
} AMBA_MON_EVENT_PORT_s;

typedef struct /*_AMBA_MON_EVENT_FLAG_s_*/ {
    UINT64      BitAlloc;
    UINT32      BitCnt[64];
    const char  *pNameTable[64];
} AMBA_MON_EVENT_FLAG_s;

typedef struct /*_AMBA_MON_EVENT_ID_LINK_s_*/ {
    struct AMBA_MON_EVENT_ID_s_    *Up;
    struct AMBA_MON_EVENT_ID_s_    *Down;
} AMBA_MON_EVENT_ID_LINK_s;

typedef struct AMBA_MON_EVENT_ID_s_ {
    UINT32                      Magic;
    AMBA_KAL_MUTEX_t            Mutex;
    const char                  *pName;
    AMBA_MON_EVENT_FLAG_s       Flags;
    AMBA_MON_EVENT_PORT_s       *pPortList;
    AMBA_MON_EVENT_ID_LINK_s    Link;
} AMBA_MON_EVENT_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonEvent.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMonEvent_Init(void);

#endif /* AMBA_MONITOR_EVENT_H */
