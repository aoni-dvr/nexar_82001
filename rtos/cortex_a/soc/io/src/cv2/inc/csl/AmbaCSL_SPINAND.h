/**
 *  @file AmbaCSL_SPINAND.h
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

#ifndef AMBA_CSL_SPINAND_H
#define AMBA_CSL_SPINAND_H

#ifndef AMBA_NAND_DEF_H
#include "AmbaNAND_Def.h"
#endif

#ifndef AMBA_REG_FIO_H
#include "AmbaReg_FIO.h"
#endif

typedef struct {
    AMBA_SPINAND_TIMING0_REG_s Timing0;
    AMBA_SPINAND_TIMING1_REG_s Timing1;
    AMBA_SPINAND_TIMING2_REG_s Timing2;
} AMBA_SPINAND_TIMING_CTRL_s;

/*
 * Macro Definitions
 */
/* HW will auto clear CmdCode if command is complete done (necessary for Erase command) */
#if 1
static inline void AmbaCSL_SpiNandSetCtrlReg(AMBA_SPINAND_CTRL_REG_s d)
{
    pAmbaFIO_Reg->SCtrl = (d);
}
static inline AMBA_SPINAND_CTRL_REG_s AmbaCSL_SpiNandGetCtrlReg(void)
{
    return pAmbaFIO_Reg->SCtrl;
}

static inline void AmbaCSL_SpiNandSetCmd1OpCode(UINT8 d)
{
    pAmbaFIO_Reg->CusCmdWord.Cmd1Val0 = (d);
}
static inline void AmbaCSL_SpiNandSetCmd2OpCode(UINT8 d)
{
    pAmbaFIO_Reg->CusCmdWord.Cmd2Val0 = (d);
}

static inline void AmbaCSL_SpiNandEnableCMD1_AutoReadStatus(void)
{
    pAmbaFIO_Reg->SpiCusCmd1.AutoReadStatus = 1U;
}
//static inline void AmbaCSL_SpiNandDisableCMD1_AutoReadStatus(void) { pAmbaFIO_Reg->SpiCusCmd1.AutoReadStatus = 0;}

static inline void AmbaCSL_SpiNandEnableCMD1_AutoWrite(void)
{
    pAmbaFIO_Reg->SpiCusCmd1.AutoWriteEnable = 1U;
}
//static inline void AmbaCSL_SpiNandDisableCMD1_AutoWrite(void)      { pAmbaFIO_Reg->SpiCusCmd1.AutoWriteEnable = 0;}

static inline void AmbaCSL_SpiNandEnableCMD2_AutoReadStatus(void)
{
    pAmbaFIO_Reg->SpiCusCmd2.AutoReadStatus = 1U;
}
static inline void AmbaCSL_SpiNandDisableCMD2_AutoReadStatus(void)
{
    pAmbaFIO_Reg->SpiCusCmd2.AutoReadStatus = 0;
}

static inline void AmbaCSL_SpiNandEnableCusCmd2(void)
{
    pAmbaFIO_Reg->SpiCusCmd2.CusCmd2Enable = 1U;
}
static inline void AmbaCSL_SpiNandDisableCusCmd2(void)
{
    pAmbaFIO_Reg->SpiCusCmd2.CusCmd2Enable = 0;
}

static inline void AmbaCSL_SpiNandSetCustCmd1(AMBA_SPINAND_CUSTOM_CMD1_REG_s d)
{
    pAmbaFIO_Reg->SpiCusCmd1 = (d);
}
static inline void AmbaCSL_SpiNandSetCustCmd2(AMBA_SPINAND_CUSTOM_CMD2_REG_s d)
{
    pAmbaFIO_Reg->SpiCusCmd2 = (d);
}

static inline void AmbaCSL_SpiNandSetTiming0(AMBA_SPINAND_TIMING0_REG_s d)
{
    pAmbaFIO_Reg->STiming0 = (d);
}
static inline void AmbaCSL_SpiNandSetTiming1(AMBA_SPINAND_TIMING1_REG_s d)
{
    pAmbaFIO_Reg->STiming1 = (d);
}
static inline void AmbaCSL_SpiNandSetTiming2(AMBA_SPINAND_TIMING2_REG_s d)
{
    pAmbaFIO_Reg->STiming2 = (d);
}

static inline AMBA_SPINAND_TIMING0_REG_s AmbaCSL_SpiNandGetTiming0(void)
{
    return pAmbaFIO_Reg->STiming0;
}
static inline AMBA_SPINAND_TIMING1_REG_s AmbaCSL_SpiNandGetTiming1(void)
{
    return pAmbaFIO_Reg->STiming1;
}
static inline AMBA_SPINAND_TIMING2_REG_s AmbaCSL_SpiNandGetTiming2(void)
{
    return pAmbaFIO_Reg->STiming2;
}

static inline void AmbaCSL_SpiNandSetErrorPatReg(AMBA_SPINAND_ERRORPATTERN_REG_s d)
{
    pAmbaFIO_Reg->ErrorPatn = (d);
}

#else
#define AmbaCSL_SpiNandSetCtrlReg(d)                    pAmbaFIO_Reg->SCtrl.Data = (d)
#define AmbaCSL_SpiNandGetCtrlReg()                     pAmbaFIO_Reg->SCtrl.Data

#define AmbaCSL_SpiNandSetCmd1OpCode(d)                 pAmbaFIO_Reg->CusCmdWord.Bits.Cmd1Val0 = (d)
#define AmbaCSL_SpiNandSetCmd2OpCode(d)                 pAmbaFIO_Reg->CusCmdWord.Bits.Cmd2Val0 = (d)

#define AmbaCSL_SpiNandEnableCMD1_AutoReadStatus()      pAmbaFIO_Reg->SpiCusCmd1.AutoReadStatus = 1
#define AmbaCSL_SpiNandDisableCMD1_AutoReadStatus()     pAmbaFIO_Reg->SpiCusCmd1.AutoReadStatus = 0

#define AmbaCSL_SpiNandEnableCMD1_AutoWrite()           pAmbaFIO_Reg->SpiCusCmd1.AutoWriteEnable = 1
#define AmbaCSL_SpiNandDisableCMD1_AutoWrite()          pAmbaFIO_Reg->SpiCusCmd1.AutoWriteEnable = 0

#define AmbaCSL_SpiNandEnableCMD2_AutoReadStatus()      pAmbaFIO_Reg->SpiCusCmd2.AutoReadStatus = 1
#define AmbaCSL_SpiNandDisableCMD2_AutoReadStatus()     pAmbaFIO_Reg->SpiCusCmd2.AutoReadStatus = 0

#define AmbaCSL_SpiNandEnableCusCmd2()                  pAmbaFIO_Reg->SpiCusCmd2.CusCmd2Enable = 1
#define AmbaCSL_SpiNandDisableCusCmd2()                 pAmbaFIO_Reg->SpiCusCmd2.CusCmd2Enable = 0

#define AmbaCSL_SpiNandSetCustCmd1(d)                   pAmbaFIO_Reg->SpiCusCmd1 = (d)
#define AmbaCSL_SpiNandSetCustCmd2(d)                   pAmbaFIO_Reg->SpiCusCmd2.Data = (d)

#define AmbaCSL_SpiNandSetTiming0(d)                    pAmbaFIO_Reg->STiming0.Data = (d)
#define AmbaCSL_SpiNandSetTiming1(d)                    pAmbaFIO_Reg->STiming1.Data = (d)
#define AmbaCSL_SpiNandSetTiming2(d)                    pAmbaFIO_Reg->STiming2.Data = (d)

#define AmbaCSL_SpiNandGetTiming0()                     pAmbaFIO_Reg->STiming0.Data
#define AmbaCSL_SpiNandGetTiming1()                     pAmbaFIO_Reg->STiming1.Data
#define AmbaCSL_SpiNandGetTiming2()                     pAmbaFIO_Reg->STiming2.Data

#define AmbaCSL_SpiNandSetErrorPatReg(d)                pAmbaFIO_Reg->ErrorPatn.Data = (d)

#define AmbaCSL_SpiNandClearStatus()                    pAmbaFIO_Reg->NandStatus.Data = 0
#endif

/* SpiNand Command Set */
#define AMBA_SPINAND_CMD_READ_ARRAY            (0x13U)
#define AMBA_SPINAND_CMD_READ                  (0x3U)
#define AMBA_SPINAND_CMD_READ_TWOLANE          (0x3BU)
#define AMBA_SPINAND_CMD_READ_FOURLANE         (0x6BU)
#define AMBA_SPINAND_CMD_DUAL_READ             (0xBBU)
#define AMBA_SPINAND_CMD_QUAR_READ             (0xEBU)
#define AMBA_SPINAND_CMD_PROGRAM_LOAD          (0x2U)
#define AMBA_SPINAND_CMD_PROGRAM_LOAD_RANDOM   (0x84U)
#define AMBA_SPINAND_CMD_PROGRAM_FOURLANE      (0x32U)
#define AMBA_SPINAND_CMD_PROGRAM_EXECUTE       (0x10U)
#define AMBA_SPINAND_CMD_BLOCK_ERASE           (0xD8U)
#define AMBA_SPINAND_CMD_WRITE_ENABLE          (0x6U)
#define AMBA_SPINAND_CMD_WRITE_DISABLE         (0x4U)
#define AMBA_SPINAND_CMD_GET_FEATURE           (0x0fU)
#define AMBA_SPINAND_CMD_SET_FEATURE           (0x1fU)
#define AMBA_SPINAND_CMD_READ_ID               (0x9fU)
#define AMBA_SPINAND_CMD_RESET                 (0xffU)

/* SpiNand Error Pattern */
#define AMBA_SPINAND_ERROR_OIP                 1U
#define AMBA_SPINAND_ERROR_ERASE_FAIL          ((UINT8)1 << 2U)
#define AMBA_SPINAND_ERROR_PROGRAM_FAIL        ((UINT8)1 << 3U)
#define AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC   ((UINT8)1 << 5U)
#define AMBA_SPINAND_ONDEVICE_ECC_ERR          (0x2U)

/*
 * Defined in AmbaCSL_NAND.c
 */
void AmbaCSL_SpiNandClearStatus(void);
void AmbaCSL_SpiNandGetCustomCmdData(UINT32 RxDataSize, UINT8 *pRxDataBuf);
void AmbaCSL_SpiNandSendResetCmd(void);

void AmbaCSL_SpiNandSendWriteDisable(void);

void AmbaCSL_SpiNandSetupReadCmd(AMBA_SPINAND_READCMD_SET_s ReadOP, UINT8 RowCycle, UINT8 ColumnCycle, UINT32 AddrHi, UINT32 AddrLow);
void AmbaCSL_SpiNandSetupProgramCmd(AMBA_SPINAND_PROGRAM_SET_s ProgOP, UINT8 RowCylce, UINT8 ColummCylce, UINT32 AddrHi, UINT32 AddrLow);

void AmbaCSL_SpiNandSendReadIdCmd(void);
void AmbaCSL_SpiNandGetIdResponse(UINT8 *pDeviceID);
void AmbaCSL_SpiNandSendWriteEnable(void);
void AmbaCSL_SpiNandWriteDisable(void);
void AmbaCSL_SpiNandSendSetFeature(UINT8 FeatureAddr, UINT8 Value);
void AmbaCSL_SpiNandSendGetFeature(UINT8 FeatureAddr);
//void AmbaCSL_SpiNandSendBlockEraseCmd(UINT64 ByteAddr, UINT8 AddrCycle);
void AmbaCSL_SpiNandSendBlockEraseCmd(UINT32 AddrHi, UINT32 AddrLow, UINT8 AddrCycle);
void AmbaCSL_SpiNandSetTiming(const AMBA_SPINAND_TIMING_CTRL_s *pSpiNandTiming);

#endif /* AMBA_CSL_SPINAND_H */
