/**
 *  @file AmbaMonRing.h
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
 *  @details Constants and Definitions for Amba Monitor Ring
 *
 */

#ifndef AMBA_MON_RING_H
#define AMBA_MON_RING_H

#define MON_RING_RESET           2U
#define MON_RING_OVERFLOW        3U

typedef enum /*_AMBA_MON_RING_RSEEK_FLAG_e_*/ {
    R_SEEK_PASS_BEGIN = 0,
    R_SEEK_PASS_LAST
} AMBA_MON_RING_RSEEK_FLAG_e;

typedef struct /*_AMBA_MON_RING_INDEX_s_*/ {
    UINT32 Rd;
    UINT32 Wr;
} AMBA_MON_RING_INDEX_s;

typedef union /*AMBA_MON_RING_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Reset:        1;
        UINT32 Overflow:     1;
        UINT32 Reserved:    30;
    } Bits;
} AMBA_MON_RING_OP_u;

typedef struct /*_AMBA_MON_RING_CHUNK_INFO_s_*/ {
    UINT32 Num;
    UINT32 Size;
    UINT32 AuxInNum;
      void *pMemBase;
} AMBA_MON_RING_CHUNK_INFO_s;

typedef struct /*_AMBA_MON_RING_BUF_s_*/ {
    AMBA_MON_RING_CHUNK_INFO_s    ChunkInfo;
} AMBA_MON_RING_BUF_s;

typedef struct /*_AMBA_MON_RING_PORT_LINK_s_*/ {
    struct AMBA_MON_RING_PORT_s_    *Up;
    struct AMBA_MON_RING_PORT_s_    *Down;
} AMBA_MON_RING_PORT_LINK_s;

typedef struct AMBA_MON_RING_PORT_s_ {
    UINT32                        Magic;
    AMBA_KAL_MUTEX_t              Mutex;
    AMBA_MON_RING_OP_u            Op;
    AMBA_MON_RING_INDEX_s         Index;
    struct AMBA_MON_RING_ID_s_    *pRingId;
    AMBA_MON_RING_PORT_LINK_s     Link;
} AMBA_MON_RING_PORT_s;

typedef struct /*_AMBA_MON_RING_LINK_s_*/ {
    struct AMBA_MON_RING_ID_s_    *Up;
    struct AMBA_MON_RING_ID_s_    *Down;
} AMBA_MON_RING_ID_LINK_s;

typedef struct AMBA_MON_RING_ID_s_ {
    UINT32                     Magic;
    AMBA_KAL_MUTEX_t           Mutex;
    const char                 *pName;
    AMBA_MON_RING_OP_u         Op;
    AMBA_MON_RING_INDEX_s      Index;
    AMBA_MON_RING_BUF_s        Buf;
    AMBA_MON_RING_PORT_s       *pPortList;
    AMBA_MON_RING_ID_LINK_s    Link;
} AMBA_MON_RING_ID_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaMonRing.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaMonRing_Init(void);

#endif /* AMBA_MONITOR_RING_H */
