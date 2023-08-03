/**
 *  @file AmbaDebugPort_ORC.h
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
 *  @details Definitions & Constants for ORCs
 *
 */

#ifndef AMBA_DEBUG_PORT_ORC_H
#define AMBA_DEBUG_PORT_ORC_H

/*
 * ORC: Thread Reset
 */
typedef struct {
    UINT32 PcReset:             4;      /* [3:0]: PC Reset T3 - T0 (8 = T3, 4 = T2, 2 = T1, 1 = T0) */
    UINT32 Suspend:             4;      /* [7:4]: Suspend T3 - T0 (8 = T3, 4 = T2, 2 = T1, 1 = T0) */
    UINT32 Reserved:            23;     /* [30:8] */
    UINT32 OrcReset:            1;      /* [31]: ORC Reset */
} AMBA_ORC_THREAD_RESET_REG_s;

/*
 * ORC: I-cache Invalidate
 */
typedef struct {
    UINT32 InvalidIcache:       1;      /* [0]: Invalid Icache */
    UINT32 Reserved:            31;     /* [31:1] */
} AMBA_ORC_ICACHE_INVALID_REG_s;

/*
 * ORC: All Registers
 */
typedef struct {
    volatile UINT32                                 OrcThreadReset;     /* 0x0000: Thread Reset Register */
    volatile UINT32                                 OrcInvalidIcache;   /* 0x0004: Invalidate I-cache Register */
    volatile UINT32                                 OrcResetPC;         /* 0x0008: Thread Reset PC Address Register */
    volatile UINT32                                 Reserved[14];       /* 0x000C - 0x0040: Reserved */
    volatile UINT32                                 CurrentTimer;       /* 0x0044: Current Timer */
} AMBA_DBG_PORT_ORC_REG_s;

#endif /* AMBA_DEBUG_PORT_ORC_H */
