/**
 *  @file AmbaCSL_PWC.h
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
 *  @details Definitions & Constants for Power and PWC (Real-Time Clock) Controller CSL APIs
 *
 */

#ifndef AMBA_CSL_PWC_H
#define AMBA_CSL_PWC_H

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#ifndef AMBA_REG_SCRATCHPAD_S_H
#include "AmbaReg_ScratchpadS.h"
#endif

#define PWC_SCRATCHPAD_RESERVED0            0x01U   /* A software flag to indicate reserved bit 0 */
#define PWC_SCRATCHPAD_RESERVED1            0x02U   /* A software flag to indicate reserved bit 1 */
#define PWC_SCRATCHPAD_RESERVED2            0x04U   /* A hardware flag to indicate reserved bit 2 */
#define PWC_SCRATCHPAD_RESERVED3            0x08U   /* A hardware flag to indicate reserved bit 3 */
#define PWC_SCRATCHPAD_RESERVED4            0x10U   /* A software flag to indicate reserved bit 4 */
#define PWC_SCRATCHPAD_RESERVED5            0x20U   /* A software flag to indicate reserved bit 5 */
#define PWC_SCRATCHPAD_RESERVED6            0x40U   /* A software flag to indicate reserved bit 6 */
#define PWC_SCRATCHPAD_RESERVED7            0x80U   /* A software flag to indicate reserved bit 7 */

/*
 * Inline Function Definitions
 */
static inline UINT32 AmbaCSL_PwcReadScratchpad(void)
{
    return pAmbaScratchpadS_Reg->PwcStatusRead;
}
static inline void AmbaCSL_PwcWriteScratchpad(UINT32 Val)
{
    pAmbaScratchpadS_Reg->PwcStatusWrite = Val;
}

/*
 * Defined in AmbaCSL_PWC.c
 */
void AmbaCSL_PwcTogglePCRST(void);
void AmbaCSL_PwcSetScratchpad(UINT32 BitMask);
void AmbaCSL_PwcClearScratchpad(UINT32 BitMask);

#endif /* AMBA_CSL_PWC_H */
