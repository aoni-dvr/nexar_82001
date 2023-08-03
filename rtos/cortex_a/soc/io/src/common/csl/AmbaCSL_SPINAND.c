/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "AmbaTypes.h"
#include "AmbaWrap.h"

#include "AmbaCSL_FIO.h"
#include "AmbaCSL_DMA.h"
#include "AmbaCSL_NAND.h"
#include "AmbaCSL_SPINAND.h"

void AmbaCSL_SpiNandClearStatus(void)
{
    AMBA_NAND_STATUS_REG_s d;

    if (0U != AmbaWrap_memset(&d, 0, sizeof(d))) { /* Misra C */ };

    pAmbaFIO_Reg->NandStatus = d;
}

/**
 *  AmbaCSL_SpiNandSetErrorPattern -
 */
static void AmbaCSL_SpiNandSetErrorPattern(UINT8 Enable, UINT8 ErrorPattern)
{
    AMBA_SPINAND_ERRORPATTERN_REG_s Pattern = {0};

    if (0U != AmbaWrap_memset(&Pattern, 0, sizeof(Pattern))) { /* Misra C */ };

    if (Enable != 0U) {
        Pattern.ErrorPatterEn = Enable;
        Pattern.ErrorPattern  = ErrorPattern;
        AmbaCSL_SpiNandSetErrorPatReg(Pattern);
    } else {
        AmbaCSL_SpiNandSetErrorPatReg(Pattern);
    }
}

/**
 *  AmbaCSL_SpiNandGetCustomCmdData - Get data form CustomCmd Data Register by Byte
 *  @param[in] RxDataSize receiving data size in Frames
 *  @param[in] pRxDataBuf pointer to the receiving data buffer
 */
void AmbaCSL_SpiNandGetCustomCmdData(UINT32 RxDataSize, UINT8 *pRxDataBuf)
{
    UINT32 CusCmdData[8];

    CusCmdData[0] = pAmbaFIO_Reg->CusCmdData[0];
    CusCmdData[1] = pAmbaFIO_Reg->CusCmdData[1];
    CusCmdData[2] = pAmbaFIO_Reg->CusCmdData[2];
    CusCmdData[3] = pAmbaFIO_Reg->CusCmdData[3];

    CusCmdData[4] = pAmbaFIO_Reg->CusCmdData[4];
    CusCmdData[5] = pAmbaFIO_Reg->CusCmdData[5];
    CusCmdData[6] = pAmbaFIO_Reg->CusCmdData[6];
    CusCmdData[7] = pAmbaFIO_Reg->CusCmdData[7];

    if (0U != AmbaWrap_memcpy(pRxDataBuf, CusCmdData, RxDataSize)) { /* Misra C */ };
}

/**
 *  AmbaCSL_SpiNandSetAddress - Set the value of Address Phase for Spinand Cmd
 *  @param[in] AddrSrc The source of Address
 *  @param[in] ByteAddress Number of ID read cycles
 */
static void AmbaCSL_SpiNandSetAddress(UINT32 AddrSrc, UINT32 AddrHi, UINT32 AddrLow)
{
    UINT8 AddrHiU8;

    if (0U != AmbaWrap_memcpy(&AddrHiU8, &AddrHi, sizeof(AddrHiU8))) { /* Misra C */ };

    switch (AddrSrc) {
    case AMBA_SPINAND_ADRRESS_CUSTOMCMD:
        AmbaCSL_NandSetCopyDestAddr(AddrLow);
        AmbaCSL_NandSetCopyAddr5Byte(AddrHiU8);
        break;

    case AMBA_SPINAND_ADRRESS_ROW:
        AmbaCSL_NandSetAddress31_4(AddrLow >> 4U);
        AmbaCSL_NandSetAddress33_32(AddrHiU8);
        break;

    case AMBA_SPINAND_ADRESS_ROW_COLUMN:
        AmbaCSL_NandSetAddress31_4(AddrLow >> 4U);
        AmbaCSL_NandSetAddress33_32(AddrHiU8);
        break;

    case AMBA_SPINAND_ADRESS_TWO_PLANE:
        AmbaCSL_NandSetAddress31_4(AddrLow >> 4U);
        AmbaCSL_NandSetAddress33_32(AddrHiU8);
        break;

    default:
        /* Do nothing! */
        break;
    }
}

/**
 *  AmbaCSL_NandSendSendReadIdCmd - Retrieve IDs from NAND flash device
 */
void AmbaCSL_SpiNandSendReadIdCmd(void)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    if (0U != AmbaWrap_memset(&CMD2, 0, sizeof(CMD2))) { /* Misra C */ };

    AmbaCSL_SpiNandSetAddress(AMBA_SPINAND_ADRRESS_CUSTOMCMD, 0x0, 0x0);
    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_READ_ID);
    CMD1.DataCycle   = 1U;
    CMD1.CustCmdType = AMBA_SPINAND_RE_TYPE;
    CMD1.AddrCycle   = 1U;
    CMD1.AddrSrc     = AMBA_SPINAND_ADRRESS_CUSTOMCMD;

    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_NandGetReadIdResponse - Retrieve IDs from NAND flash device
 *  @param[in] pDeviceID ID byte sequence
 */
void AmbaCSL_SpiNandGetIdResponse(UINT8 *pDeviceID)
{
    AmbaCSL_SpiNandGetCustomCmdData(2U, pDeviceID);
}

/**
 *  AmbaCSL_SpiNandSendWriteEnable - Send WriteEnable command to SpiNAND device
 */
void AmbaCSL_SpiNandSendWriteEnable(void)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    if (0U != AmbaWrap_memset(&CMD1, 0, sizeof(CMD1))) { /* Misra C */ };
    if (0U != AmbaWrap_memset(&CMD2, 0, sizeof(CMD2))) { /* Misra C */ };

    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_WRITE_ENABLE);

    //CMD1.AutoReadStatus = 1;
    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandSendWriteDisable - Send WriteDisable command to SpiNAND device
 */
void AmbaCSL_SpiNandSendWriteDisable(void)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    if (0U != AmbaWrap_memset(&CMD1, 0, sizeof(CMD1))) { /* Misra C */ };
    if (0U != AmbaWrap_memset(&CMD2, 0, sizeof(CMD2))) { /* Misra C */ };

    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_WRITE_DISABLE);
    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandSendSetFeature - Send SetFeature command to SpiNAND device
 */
void AmbaCSL_SpiNandSendSetFeature(UINT8 FeatureAddr, UINT8 Value)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    if (0U != AmbaWrap_memset(&CMD2, 0, sizeof(CMD2))) { /* Misra C */ };

    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_SET_FEATURE);
    AmbaCSL_SpiNandSetAddress(AMBA_SPINAND_ADRRESS_CUSTOMCMD, 0x0, FeatureAddr);

    CMD1.AddrCycle   = 1U;
    CMD1.DataCycle   = 0U;
    CMD1.CustCmdType = AMBA_SPINAND_WE_TYPE;

    pAmbaFIO_Reg->CusCmdData[0] = (UINT32)Value;
    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandGetFeature - Send GetFeature command to SpiNAND device
 */
void AmbaCSL_SpiNandSendGetFeature(UINT8 FeatureAddr)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    if (0U != AmbaWrap_memset(&CMD2, 0, sizeof(CMD2))) { /* Misra C */ };

    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_GET_FEATURE);
    AmbaCSL_SpiNandSetAddress(AMBA_SPINAND_ADRRESS_CUSTOMCMD, 0x0, FeatureAddr);

    CMD1.AddrCycle = 1U;
    CMD1.DataCycle = 0U;
    CMD1.CustCmdType = AMBA_SPINAND_RE_TYPE;

    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandSendResetCmd - Send Reset command to SpiNAND device
 */
void AmbaCSL_SpiNandSendResetCmd(void)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    if (0U != AmbaWrap_memset(&CMD1, 0, sizeof(CMD1))) { /* Misra C */ };
    if (0U != AmbaWrap_memset(&CMD2, 0, sizeof(CMD2))) { /* Misra C */ };

    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_RESET);
    CMD1.AutoReadStatus = 0;
    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandSetupReadCmd - Send Read command to SpiNAND device
 *  @param[in] ReadOP
 *  @param[in] RowCycle
 *  @param[in] ColumnCycle
 *  @param[in] AddrHi
 *  @param[in] AddrLow
 */
void AmbaCSL_SpiNandSetupReadCmd(AMBA_SPINAND_READCMD_SET_s ReadOP, UINT8 RowCycle,
                                 UINT8 ColumnCycle, UINT32 AddrHi, UINT32 AddrLow)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    /* Setup ReadCellArray CMD for Read Operator */
    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_READ_ARRAY);
    CMD1.AddrPhaseDummy  = ReadOP.ReadArray_AddrDummy;
    CMD1.LaneType        = AMBA_SPINAND_DATAPHASE_ONLY;
    CMD1.AutoReadStatus  = 1U;
    CMD1.AutoWriteEnable = 0;
    CMD1.DataPhaseSrc    = 1U;
    CMD1.CustCmdType     = AMBA_SPINAND_NO_DATAPHASE_TYPE;
    CMD1.LaneNum         = AMBA_SPINAND_DATA_1LANE;
    CMD1.AddrSrc         = AMBA_SPINAND_ADRRESS_ROW;
    CMD1.AddrCycle       = RowCycle;

    /* Setup ReadCache CMD for Read Operator */
    AmbaCSL_SpiNandSetCmd2OpCode(ReadOP.ReadCacheIndex);
    CMD2.CusCmd2Enable  = 1U;
    CMD2.AddrPhaseDummy = ReadOP.AddrDummy;
    CMD2.DataPhaseDummy = ReadOP.DataDummy;
    if ((ReadOP.ReadCacheIndex == AMBA_SPINAND_CMD_QUAR_READ) || (ReadOP.ReadCacheIndex == AMBA_SPINAND_CMD_DUAL_READ)) {
        CMD2.LaneType = AMBA_SPINAND_DUMMY_ADDRESS_DATA_PHASE;
    } else {
        CMD2.LaneType = AMBA_SPINAND_DATAPHASE_ONLY;
    }
    CMD2.CustCmdType    = AMBA_SPINAND_RE_TYPE;
    CMD2.AddrSrc        = AMBA_SPINAND_ADRESS_ROW_COLUMN;
    CMD2.AddrCycle      = ColumnCycle;
    CMD2.DataPhaseSrc   = 1U;
    CMD2.AutoReadStatus = 0;

    switch (ReadOP.CmdType) {
    case AMBA_SPINAND_READCACHE_1LANE:
        CMD2.LaneNum = AMBA_SPINAND_DATA_1LANE;
        break;
    case AMBA_SPINAND_READCACHE_2LANE:
        CMD2.LaneNum = AMBA_SPINAND_DATA_2LANE;
        break;
    case AMBA_SPINAND_READCACHE_4LANE:
        CMD2.LaneNum = AMBA_SPINAND_DATA_4LANE;
        break;
    case AMBA_SPINAND_READCACHE_DUAL:
        CMD2.LaneNum = AMBA_SPINAND_DATA_2LANE;
        break;
    case AMBA_SPINAND_READCACHE_QUAL:
        CMD2.LaneNum = AMBA_SPINAND_DATA_4LANE;
        break;
    default:
        /* Do nothing!! */
        break;
    }
    AmbaCSL_SpiNandSetErrorPattern(AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC, AMBA_SPINAND_ERROR_UNCORRECTABLE_ECC);

    /* Sn_cc1 and Sn_cc2 share the same Address Register */
    AmbaCSL_SpiNandSetAddress(AMBA_SPINAND_ADRESS_ROW_COLUMN, AddrHi, AddrLow);
    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandSetupProgramCmd - Send Write command to SpiNAND device
 *  @param[in] ProgOP
 *  @param[in] RowCylce
 *  @param[in] ColumnCycle
 *  @param[in] AddrHi
 *  @param[in] AddrLow
 */
void AmbaCSL_SpiNandSetupProgramCmd(AMBA_SPINAND_PROGRAM_SET_s ProgOP, UINT8 RowCylce,
                                    UINT8 ColummCylce, UINT32 AddrHi, UINT32 AddrLow)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    /* Setup ProgramLoad CMD for Read Operator */
    AmbaCSL_SpiNandSetCmd1OpCode(ProgOP.LoadCmdIndex);
    CMD1.AddrPhaseDummy  = ProgOP.LoadAddrDummy;
    CMD1.DataPhaseDummy  = ProgOP.LoadDataDummy;
    CMD1.LaneType        = AMBA_SPINAND_DATAPHASE_ONLY;
    CMD1.CustCmdType     = AMBA_SPINAND_WE_TYPE;
    CMD1.AutoReadStatus  = 0;
    CMD1.AutoWriteEnable = 1;
    CMD1.DataPhaseSrc    = 1;
    CMD1.AddrSrc         = AMBA_SPINAND_ADRESS_ROW_COLUMN;
    CMD1.AddrCycle       = ColummCylce;

    switch (ProgOP.LoadCmdType) {
    case AMBA_SPINAND_PROGRAM_1LANE:
        CMD1.LaneNum = AMBA_SPINAND_DATA_1LANE;
        break;
    case AMBA_SPINAND_PROGRAM_4LANE:
        CMD1.LaneNum = AMBA_SPINAND_DATA_4LANE;
        break;
    default:
        /* Do nothing!! */
        break;
    }

    /* Setup ProgramExecute CMD for Write Operator */
    AmbaCSL_SpiNandSetCmd2OpCode(AMBA_SPINAND_CMD_PROGRAM_EXECUTE);
    CMD2.CusCmd2Enable  = 1U;
    CMD2.LaneType       = AMBA_SPINAND_DATAPHASE_ONLY;
    CMD2.LaneType       = AMBA_SPINAND_DATAPHASE_ONLY;
    CMD2.CustCmdType    = AMBA_SPINAND_NO_DATAPHASE_TYPE;
    CMD2.AutoReadStatus = 1U;
    CMD2.AddrSrc        = AMBA_SPINAND_ADRRESS_ROW;
    CMD2.AddrCycle      = RowCylce;
    CMD2.DataPhaseSrc   = 1U;

    /* Sn_cc1 and Sn_cc2 share the same Address Register */
    AmbaCSL_SpiNandSetErrorPattern(AMBA_SPINAND_ERROR_PROGRAM_FAIL, AMBA_SPINAND_ERROR_PROGRAM_FAIL);

    AmbaCSL_SpiNandSetAddress(AMBA_SPINAND_ADRESS_ROW_COLUMN, AddrHi, AddrLow);
    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandSendBlockEraseCmd - Erase SpiNAND block
 *  @param[in] AddrHi
 *  @param[in] AddrLow
 *  @param[in] AddrCycle
 */
void AmbaCSL_SpiNandSendBlockEraseCmd(UINT32 AddrHi,UINT32 AddrLow,UINT8 AddrCycle)
{
    AMBA_SPINAND_CUSTOM_CMD1_REG_s CMD1 = {0};
    AMBA_SPINAND_CUSTOM_CMD2_REG_s CMD2 = {0};

    if (0U != AmbaWrap_memset(&CMD2, 0, sizeof(CMD2))) { /* Misra C */ };

    AmbaCSL_SpiNandSetCmd1OpCode(AMBA_SPINAND_CMD_BLOCK_ERASE);
    CMD1.AutoReadStatus  = 1U;
    CMD1.AutoWriteEnable = 1U;
    CMD1.AddrCycle       = AddrCycle;
    CMD1.AddrSrc         = AMBA_SPINAND_ADRRESS_CUSTOMCMD;

    //xil_printf("Erase : AddrHi 0x%x AddrLow 0x%x \r\n", AddrHi, AddrLow);
    AmbaCSL_SpiNandSetAddress(AMBA_SPINAND_ADRRESS_CUSTOMCMD, AddrHi, AddrLow);
    AmbaCSL_SpiNandSetErrorPattern(AMBA_SPINAND_ERROR_ERASE_FAIL, AMBA_SPINAND_ERROR_ERASE_FAIL);
    AmbaCSL_SpiNandSetCustCmd2(CMD2);
    AmbaCSL_SpiNandSetCustCmd1(CMD1);
}

/**
 *  AmbaCSL_SpiNandSetTiming - Set the SPINAND flash timing
 *  @param[in] pNandTiming pointer to SPINAND timing registers' values
 */
void AmbaCSL_SpiNandSetTiming(const AMBA_SPINAND_TIMING_CTRL_s *pSpiNandTiming)
{
    AmbaCSL_SpiNandSetTiming0(pSpiNandTiming->Timing0);
    AmbaCSL_SpiNandSetTiming1(pSpiNandTiming->Timing1);
    AmbaCSL_SpiNandSetTiming2(pSpiNandTiming->Timing2);
}

