/**
 *  @file AmbaRTSL_NAND.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for NAND Controller RTSL APIs
 *
 */

#ifndef AMBA_RTSL_SPINAND_H
#define AMBA_RTSL_SPINAND_H

#ifndef AMBA_NAND_DEF_H
#include "AmbaNAND_Def.h"
#endif

typedef struct {
    /* 8 bits */

    UINT32   OIP:        1;      /* [0]: Operation in Process 1: Busy 0: Ready */
    UINT32   WEL:        1;      /* [1]: Internal write enable latch bit */
    UINT32   E_FAIL:     1;      /* [2]: Erase fail (Bit 1: Failed 0: Passesd) */
    UINT32   P_FAIL:     1;      /* [3]: Program failure Bit (1: Failed 0: Passesd) */
    UINT32   ECC:        3;      /* [6:4]: The Status of ECC (00: 0 bit Error 10: Error and not Corrected) */
    UINT32   RSV:        1;      /* [7]: Reserved Bits */
} AMBA_SPINAND_CMDSTATUS_REG_s;

/*
 * Defined in AmbaRTSL_SPINAND.c
 */
UINT32 AmbaRTSL_SpiNandInit(void);

UINT32 AmbaRTSL_SpiNandConfig(AMBA_SPINAND_CONFIG_s *pNandConfig);
void AmbaRTSL_SpiNandSetTiming(const AMBA_SPINAND_DEV_INFO_s *pSpiNandDevInfo);
UINT32 AmbaRTSL_SpiNand_ConfigOnDevECC(void);

#endif /* _AMBA_RTSL_NAND_H_ */
