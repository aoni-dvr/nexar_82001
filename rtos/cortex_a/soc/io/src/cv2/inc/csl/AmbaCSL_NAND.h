/**
 *  @file AmbaCSL_NAND.h
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
 *  @details Definitions & Constants for H2 CSL APIs of NAND Controller
 *
 */

#ifndef AMBA_CSL_NAND_H
#define AMBA_CSL_NAND_H

#ifndef AMBA_NAND_DEF_H
#include "AmbaNAND_Def.h"
#endif

#ifndef AMBA_REG_FIO_H
#include "AmbaReg_FIO.h"
#endif

typedef struct {
    AMBA_NAND_TIMING0_REG_s Timing0;
    AMBA_NAND_TIMING1_REG_s Timing1;
    AMBA_NAND_TIMING2_REG_s Timing2;
    AMBA_NAND_TIMING3_REG_s Timing3;
    AMBA_NAND_TIMING4_REG_s Timing4;
    AMBA_NAND_TIMING5_REG_s Timing5;
    AMBA_NAND_TIMING6_REG_s Timing6;
} AMBA_NAND_TIMING_CTRL_s;

/*
 * Macro Definitions
 */
#if 1
static inline void AmbaCSL_NandEnableWriteProtect(void)
{
    pAmbaFIO_Reg->NandCtrl.WriteProtectEnable = 1;
}
static inline void AmbaCSL_NandDisableWriteProtect(void)
{
    pAmbaFIO_Reg->NandCtrl.WriteProtectEnable = 0;
}

static inline void AmbaCSL_NandEnableSpare2xArea(void)
{
    pAmbaFIO_Reg->ExtCtrl.Spare2xEnable = 1;
}
static inline void AmbaCSL_NandDisableSpare2xArea(void)
{
    pAmbaFIO_Reg->ExtCtrl.Spare2xEnable = 0;
}

static inline void AmbaCSL_NandSetPageSize4KB(void)
{
    pAmbaFIO_Reg->ExtCtrl.Page4kEnable = 1;
}
static inline void AmbaCSL_NandDisablePageSize4KB(void)
{
    pAmbaFIO_Reg->ExtCtrl.Page4kEnable = 0;
}

static inline void AmbaCSL_NandSetChipSizeOver8Gb(void)
{
    pAmbaFIO_Reg->ExtCtrl.ChipSizeExt = 1;
}

static inline void AmbaCSL_NandSetCtrlReg(AMBA_NAND_CTRL_REG_s d)
{
    pAmbaFIO_Reg->NandCtrl = (d);
}

#else
#define AmbaCSL_NandEnableWriteProtect()        pAmbaFIO_Reg->NandCtrl.WriteProtectEnable = 1
#define AmbaCSL_NandDisableWriteProtect()       pAmbaFIO_Reg->NandCtrl.WriteProtectEnable = 0

#define AmbaCSL_NandEnableSpare2xArea()         pAmbaFIO_Reg->ExtCtrl.Bits.Spare2xEnable = 1
#define AmbaCSL_NandDisableSpare2xArea()        pAmbaFIO_Reg->ExtCtrl.Bits.Spare2xEnable = 0

#define AmbaCSL_NandSetPageSize4KB()            pAmbaFIO_Reg->ExtCtrl.Bits.Page4kEnable = 1
#define AmbaCSL_NandDisablePageSize4KB()        pAmbaFIO_Reg->ExtCtrl.Bits.Page4kEnable = 0

#define AmbaCSL_NandSetChipSizeOver8Gb()        pAmbaFIO_Reg->ExtCtrl.Bits.ChipSizeExt = 1

#define AmbaCSL_NandGetAddress31_4()           pAmbaFIO_Reg->Cmd.Bits.Addr
#define AmbaCSL_NandGetAddress33_32()          pAmbaFIO_Reg->NandCtrl.Addr33_32

/* HW will auto clear CmdCode if command is complete done (necessary for Erase command) */
#define AmbaCSL_NandGetCmdDone()                (pAmbaFIO_Reg->Cmd.Bits.CmdCode == 0)
#endif
/*
 * Defined in AmbaCSL_NAND.c
 */

AMBA_NAND_CTRL_REG_s AmbaCSL_NandGetCtrlReg(void);

void AmbaCSL_NandSendResetCmd(void);

void AmbaCSL_NandSendReadIdCmd(UINT32 NumIdCycle);
void AmbaCSL_NandGetReadIdResponse(UINT32 NumIdCycle, UINT8 *pDeviceID);
void AmbaCSL_NandSendReadStatusCmd(void);
void AmbaCSL_NandGetCmdResponse(UINT8 *pStatus);
void AmbaCSL_NandSendCopyBackCmd(UINT64 SrcAddr, UINT64 DestAddr);
void AmbaCSL_NandSendBlockEraseCmd(UINT32 Addr);

void AmbaCSL_NandSetTiming(const AMBA_NAND_TIMING_CTRL_s *pNandTiming);
void AmbaCSL_NandEnableBCH(UINT32 BchBits);
void AmbaCSL_NandDisableBCH(UINT32 MainByteSize, UINT32 SpareByteSize);
void AmbaCSL_NandEnableCheck1bitECC(void);
//void AmbaCSL_NandEnableGenerate1bitECC(void);
void AmbaCSL_NandEnableGen1bitECC(void);
void AmbaCSL_NandDisable1bitECC(void);

#endif /* AMBA_CSL_NAND_H */
