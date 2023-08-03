/**
 *  @file AmbaDebugPort_DRAM.h
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
 *  @details Definitions & Constants for DRAM channel Cluster
 *
 */

#ifndef AMBA_DEBUG_PORT_DRAM_H
#define AMBA_DEBUG_PORT_DRAM_H

/*
 * DRAM Debug Port: SYNC counter 0-127
 */
typedef struct {
    UINT32 NewMaxCounterValue:  12;     /* [11:0]: New Maximum Counter Value */
    UINT32 NewCounterValue:     12;     /* [23:12]: New Sync Counter Value */
    UINT32 NotChangeMaxValue:   1;      /* [24]: 1 = Not Change Maximum Sync Counter Value */
    UINT32 NotChangeCurValue:   1;      /* [25]: 1 = Not Change Current Sync Counter Value */
    UINT32 Reserved:            6;      /* [31:26] */
} AMBA_DBG_PORT_SYNC_COUNTER_REG_s;

/*
 * SMEM Debug Port All Registers
 */
typedef struct {
    volatile UINT32 DramChanSub0[384];      /* 0x0000 - 0x05FC: Dram Channel Register, Subregister0 */
    volatile UINT32 Reserved0[640];         /* 0x0600 - 0x0FFC: Reserved */
    volatile UINT32 DramChanSub1[384];      /* 0x1000 - 0x15FC: Dram Channel Register, Subregister1 */
    volatile UINT32 Reserved1[640];         /* 0x1600 - 0x1FFC: Reserved */
    volatile UINT32 DramChanSub2[384];      /* 0x2000 - 0x25FC: Dram Channel Register, Subregister2 */
    volatile UINT32 Reserved2[640];         /* 0x2600 - 0x2FFC: Reserved */
    volatile UINT32 DramChanSub3[384];      /* 0x3000 - 0x35FC: Dram Channel Register, Subregister3 */
    volatile UINT32 Reserved3[640];         /* 0x3600 - 0x3FFC: Reserved */
    volatile UINT32 DramChanSub4[384];      /* 0x4000 - 0x45FC: Dram Channel Register, Subregister4 */
    volatile UINT32 Reserved4[640];         /* 0x4600 - 0x4FFC: Reserved */
    volatile UINT32 DramChanSub5[384];      /* 0x5000 - 0x55FC: Dram Channel Register, Subregister5 */
    volatile UINT32 Reserved5[9856];        /* 0x5600 - 0xEFFC: Reserved */
    volatile UINT32 SyncCounter[128];       /* 0xF000 - 0xF1FC: Smem Sync Counter Registers */
} AMBA_DBG_PORT_DRAM_REG_s;

#endif /* AMBA_DEBUG_PORT_DRAM_H */
