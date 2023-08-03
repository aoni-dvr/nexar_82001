/**
 *  @file AmbaB8_SerdesLink.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details B8 driver APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"

#include "AmbaB8_Serdes.h"
#include "AmbaB8_PHY.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Depacker.h"
#include "AmbaB8_Scratchpad.h"

#ifdef BUILT_IN_SERDES
#include "AmbaCortexA9.h"
#include "AmbaDSP_VIN.h"
#include "AmbaSERDES.h"
#include "AmbaCSL_RCT.h"
#include "AmbaCSL_VIN.h"
#include "AmbaCSL_SERDES.h"
#include "AmbaCSL_DebugPort.h"
extern void AmbaSERDES_SetLinkStatus(UINT32 ChipID, UINT32 Status);
#endif

#ifdef VLSI_DEBUG /* memload for debug */
#include "AmbaB8_Calibration.h"
#include <stdio.h>

#define ABS(x)   (((x) < (0)) ? -(x) : (x))

#define TrainSeqMaxNum   1024
#define RelinkSeqMaxNum  256

typedef struct _AMBA_B8_TRAIN_CMD_s_ {
    UINT32 ChipID;
    UINT32 RegAddr;
    UINT32 Value;
    UINT8 AddrInc;
    UINT8 Type; //0: b8 writew, 1: b8 readw, 2: mpause
} AMBA_B8_TRAIN_CMD_s;

UINT32 MemLoadTraining = 0;
UINT32 SerdesTestDebug = 1;

static AMBA_B8_TRAIN_CMD_s TrainSeq[TrainSeqMaxNum] = {0};
static AMBA_B8_TRAIN_CMD_s RelinkSeq[RelinkSeqMaxNum] = {0};

static int TrainSeqNum = 0;
static int RelinkSeqNum = 0;

static INT32 B8_ParsingArgument(int argc, char **argv)
{
    int Addr;
    char Plus[32] = {'\0'};
    AMBA_B8_TRAIN_CMD_s *Cmd;
    int Duplicated = 0;

    Cmd = &TrainSeq[TrainSeqNum];

    /* sleep in millisecond*/
    if (argc >= 4 && strcmp(argv[2], "mpause") == 0) {
        Cmd->Type = 2;
        Cmd->Value = strtoul(argv[3], NULL, 0);

        /* sleep */
    } else if (argc >= 4 && strcmp(argv[2], "mpause+") == 0) {
        Cmd->Type = 2;
        Cmd->Value = strtoul(argv[3], NULL, 0);
        Duplicated = 1;

        /* B8 Read or Write */
    } else {
        if (argc >= 7) {
            if(strcmp(argv[2], "writew+") == 0) {
                Cmd->Type = 0;
                Duplicated = 1;
            } else if (strcmp(argv[2], "readw+") == 0) {
                Cmd->Type = 1;
                Duplicated = 1;
            } else if(strcmp(argv[2], "writew") == 0) {
                Cmd->Type = 0;
            } else if (strcmp(argv[2], "readw") == 0) {
                Cmd->Type = 1;
            } else {
                AmbaPrint("Wrong Argument: \'%s\'", argv[2]);
                return NG;
            }

            /* Chip ID */
            Cmd->ChipID = strtoul(argv[3], NULL, 0);

            /* Address */
            sscanf(argv[5], "%08x%31s", &Addr, Plus);

            /* Bus */
            if (strcmp(argv[4], "ahb") == 0) {
                Cmd->RegAddr = B8_AHB_BASE_ADDR + Addr;

            } else if (strcmp(argv[4], "apb") == 0) {
                Cmd->RegAddr = B8_APB_BASE_ADDR + Addr;

            } else {
                AmbaPrint("Wrong Argument: \'%s\'", argv[4]);
                return NG;
            }

            /* Inc */
            if (Plus[0] == '\0') {
                Cmd->AddrInc = 0;
            } else if (strcmp(Plus, "+") == 0) {
                Cmd->AddrInc = 1;
            } else {
                AmbaPrint("Wrong Argument: \'%s\'", Plus);
                return NG;
            }

            /* Count or Data */
            Cmd->Value = strtoul(argv[6], NULL, 0);

            /* !!!patch: record memload write phy setting */

        } else {
            AmbaPrint("Usage: t %s upload writew <ChipID> [ahb|apb] <addr>(+) data0", argv[0]);
            AmbaPrint("Usage: t %s upload readw <ChipID> [ahb|apb] <addr>(+) <count>", argv[0]);
            AmbaPrint("Usage: t %s upload mpause <millisecond>", argv[0]);
            AmbaPrint("Usage: t %s upload writew+ <ChipID> [ahb|apb] <addr>(+) data0", argv[0]);
            AmbaPrint("Usage: t %s upload readw+ <ChipID> [ahb|apb] <addr>(+) <count>", argv[0]);
            AmbaPrint("Usage: t %s upload mpause+ <millisecond>", argv[0]);
            AmbaPrint("Usage: t %s upload dbg [1|0]\n", argv[0]);
            AmbaPrint("Usage: t %s upload enable [1|0]\n", argv[0]);
            return NG;
        }
    }

    if (Duplicated == 1) {
        memcpy(&RelinkSeq[RelinkSeqNum], Cmd, sizeof(AMBA_B8_TRAIN_CMD_s));
        RelinkSeqNum++;
    }

    return OK;
}

void B8_UploadCommand(int Argc, char **Argv)
{
    if (TrainSeqNum < TrainSeqMaxNum) {
        if (B8_ParsingArgument(Argc, Argv) == OK) {
            TrainSeqNum++;
        }
    } else {
        AmbaPrintColor(RED, "Too many taining command, max = %d / %d", TrainSeqMaxNum, RelinkSeqMaxNum);
    }
    return;
}
void B8_LoadRelinkCommand(void)
{
    int i = 0, j = 0;
    int AddrOffset = 0;
    AMBA_B8_TRAIN_CMD_s *CurCmd = NULL;
    UINT32 DataBuf32[256];

    if (RelinkSeqNum == 0) {
        AmbaPrintColor(RED, "No command to be loaded in the buffer");
        return;
    }
    AmbaPrintColor(GRAY, "Total Relink Sequence Number := %d", RelinkSeqNum);

    for (i = 0; i < RelinkSeqNum; i++) {
        CurCmd = &RelinkSeq[i];

        /* Call B8 Write Word function */
        if(CurCmd->Type == 0) {
            DataBuf32[0] = CurCmd->Value;
            if (SerdesTestDebug) {
                AmbaPrint("[b8 write] ChipID: 0x%x, Regaddr: 0x%08x, Data: 0x%08x", CurCmd->ChipID, CurCmd->RegAddr, DataBuf32[0]);
            }
            AmbaB8_RegWriteU32(CurCmd->ChipID, CurCmd->RegAddr, CurCmd->AddrInc, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

            /* Call B8 Read Word function */
        } else if (CurCmd->Type == 1) {
            if (CurCmd->Value > 128) {
                CurCmd->Value = 128;
                AmbaPrint("max readw size = 128");
            }
            if (AmbaB8_RegReadU32(CurCmd->ChipID, CurCmd->RegAddr, CurCmd->AddrInc, B8_DATA_WIDTH_32BIT, (int)CurCmd->Value, DataBuf32) == OK) {
                for (j = 0, AddrOffset = 0; j < CurCmd->Value; j++, AddrOffset += CurCmd->AddrInc << 2)
                    AmbaPrint("[b8 read] ChipID: 0x%x, %04Xh: %04X", CurCmd->ChipID, CurCmd->RegAddr + AddrOffset, DataBuf32[j]);
            }

            /* Call Task Sleep function */
        } else if(CurCmd->Type == 2) {
            AmbaPrint("msleep %d", (int)CurCmd->Value);
            AmbaKAL_TaskSleep((int)CurCmd->Value);
        }
    }

    return;
}


void B8_LoadTrainCommand(void)
{
    int i = 0, j = 0;
    int AddrOffset = 0;
    AMBA_B8_TRAIN_CMD_s *CurCmd = NULL;
    UINT32 DataBuf32[256];

    if (TrainSeqNum == 0) {
        AmbaPrintColor(RED, "No command to be loaded in the buffer");
        return;
    }
    AmbaPrintColor(GRAY, "Total Training Sequence Number := %d", TrainSeqNum);

    for (i = 0; i < TrainSeqNum; i++) {
        CurCmd = &TrainSeq[i];

        /* Call B8 Write Word function */
        if(CurCmd->Type == 0) {
            DataBuf32[0] = CurCmd->Value;
            if (SerdesTestDebug) {
                AmbaPrint("[b8 write] ChipID: 0x%x, Regaddr: 0x%08x, Data: 0x%08x", CurCmd->ChipID, CurCmd->RegAddr, DataBuf32[0]);
            }
            AmbaB8_RegWriteU32(CurCmd->ChipID, CurCmd->RegAddr, CurCmd->AddrInc, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

            /* Call B8 Read Word function */
        } else if (CurCmd->Type == 1) {
            if (CurCmd->Value > 128) {
                CurCmd->Value = 128;
                AmbaPrint("max readw size = 128");
            }
            if (AmbaB8_RegReadU32(CurCmd->ChipID, CurCmd->RegAddr, CurCmd->AddrInc, B8_DATA_WIDTH_32BIT, (int)CurCmd->Value, DataBuf32) == OK) {
                for (j = 0, AddrOffset = 0; j < CurCmd->Value; j++, AddrOffset += CurCmd->AddrInc << 2)
                    AmbaPrint("[b8 read] ChipID: 0x%x, %04Xh: %04X", CurCmd->ChipID, CurCmd->RegAddr + AddrOffset, DataBuf32[j]);
            }

            /* Call Task Sleep function */
        } else if(CurCmd->Type == 2) {
            AmbaPrint("msleep %d", (int)CurCmd->Value);
            AmbaKAL_TaskSleep((int)CurCmd->Value);
        }
    }

    return;
}
#endif

static UINT32 B8_SerdesCommReset(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_ECC_CONFIG_s EccConfig = {0};

    /* disable-ack mechanism & pwm-enc no retransmission */
    RetVal |= AmbaB8_CommDisableAck(ChipID, 1U);

    /* reset pwm-enc/dec */
    RetVal |= AmbaB8_CommReset(ChipID);

    if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
        if ((ChipID & B8_SELECT_CHIP_ID_B6F) != 0U) {
            /* b6f */
        } else {
            /* b8f */

            /* reset packer */
            RetVal |= AmbaB8_PackerReset(ChipID);

            /* reset ecc-enc */
            RetVal |= AmbaB8_EccEncReset(ChipID, B8_ECC_ENC_CHANNEL0);

            /* initialize ecc-enc (t=1) */
            EccConfig.EncEnable[0] = 1U;
            EccConfig.DecEnable[0] = 0U;
            EccConfig.NumCheckSymbol = 1U;
            RetVal |= AmbaB8_EccConfig(ChipID, &EccConfig);
        }
    }

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) == 0U) {
        if ((ChipID & B8_SELECT_CHIP_ID_B6N) != 0U) {
            /* b6n */
        } else {
            /* b8n */

            /* reset depacker */
            RetVal |= AmbaB8_DepackerReset(ChipID);

            /* reset ecc-dec */
            RetVal |= AmbaB8_EccDecReset(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK, B8_ECC_DEC_CHANNEL0);

            /* initialize ecc-dec (t=1) */
            EccConfig.EncEnable[0] = 0U;
            EccConfig.DecEnable[0] = 1U;
            EccConfig.NumCheckSymbol = 1U;
            RetVal |= AmbaB8_EccConfig((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &EccConfig);
        }
    } else {
#ifdef BUILT_IN_SERDES
        UINT32 DesID;
        /* built-in b6n */
        if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
            for (DesID = 0U; DesID < B8_MAX_NUM_SUBCHIP; DesID++) {
                if (ChipID & (0x1 << DesID)) {

                    /* Reset & Select section */
                    if ((0x1 << DesID) == B8_SUB_CHIP_ID_B8F3) {        /* DES-P3, PIP */
                        AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP_VIN);
                        AmbaCSL_IdspSectionReset(0x101);
                    } else if ((0x1 << DesID) == B8_SUB_CHIP_ID_B8F1) { /* DES-P1, PIP2 */
                        AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP2_VIN);
                        AmbaCSL_IdspSectionReset(0x801);
                    } else if ((0x1 << DesID) == B8_SUB_CHIP_ID_B8F2) { /* DES-P2, PIP3 */
                        AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_PIP3_VIN);
                        AmbaCSL_IdspSectionReset(0x1001);
                    } else {                                            /* DES-P0, VIN */
                        AmbaCSL_IdspSelectSection(AMBA_DBG_PORT_IDSP_VIN);
                        AmbaCSL_IdspSectionReset(0x3);
                    }
                    DelayCycles(0xfff); /* wait for section selection */

                    /* Global configuration */
                    _pAmbaIDSP_VinGlobalReg->GlobalConfig.Data = 0x01004004;

                    /* Enable depacker */
                    _pAmbaIDSP_VinMainReg->DepackerEnable = 1;

                    /* VIN Ctrl */
                    _pAmbaIDSP_VinMainReg->Ctrl.Bits.SwReset = 1;
                    _pAmbaIDSP_VinMainReg->Ctrl.Bits.Enable = 1;
                    _pAmbaIDSP_VinMainReg->Ctrl.Bits.OutputEnable = 1;
                    _pAmbaIDSP_VinMainReg->Ctrl.Bits.BitsPerPixel = 3;  /* b6f output bit width is 14-bits */
                    _pAmbaIDSP_VinMainReg->Ctrl.Bits.DataLaneEnable = 0;

                    //_pAmbaIDSP_VinMainReg->DvpCtrl.Bits.DoubleBufferEnable = 1;

                    /* Enable depacker pkt counter */
                    _pAmbaIDSP_VinMainReg->DepackerStatEnable.Bits.PacketEnable = 1;
                    _pAmbaIDSP_VinMainReg->DepackerStatEnable.Bits.PixelEnable = 1;

                    /* Enable ecc-dec */
                    _pAmbaIDSP_VinMainReg->EccDecoderCtrl.Bits.Enable = 1;
                    _pAmbaIDSP_VinMainReg->EccDecoderCtrl.Bits.ErasureEnable = 1;  /* Disable Erasure */


                    /* Command shift */
                    _pAmbaIDSP_VinMainReg->Debug.Data = 1;
                }
            }
        }
#endif
    }
    return RetVal;
}

static UINT32 B8_SerdesCommInit(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_COMM_CONFIG_s CommConfig = {0};
    B8_PACKER_CONFIG_s PackerConfig = {0};

    /* initialize packer */
    if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
        if ((ChipID & B8_SELECT_CHIP_ID_B6F) != 0U) {
            /* b6f */
        } else {
            /* b8f */
            PackerConfig.PelDepth = B8_PACKER_12PIXEL_DEPTH;
            PackerConfig.PktByteNumMax = 251U; /*255U - 2U;*/ /* ecc 2 extra bytes */
            PackerConfig.PktByteNumMin = 64U;;
            AmbaB8_PackerConfig(ChipID, &PackerConfig);
        }
    }

    /* initialize depacker */
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) == 0U) {
        if ((ChipID & B8_SELECT_CHIP_ID_B6N) != 0U) {
            /* b6n */
        } else {
            /* b8n */
            AmbaB8_DepackerConfig(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
            AmbaB8_DepackerResetPktCounter(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        }
    }

    /* initialize communication */
    CommConfig.ReplayTimes = 7U;
    CommConfig.ChunkSize = 2U;
    CommConfig.CreditPwr = 4U;
    RetVal |= AmbaB8_CommConfig(ChipID, &CommConfig);

    /* enable ack mechanism & pwm-enc retransmission */
    RetVal |= AmbaB8_CommDisableAck(ChipID, 0U);

    return RetVal;
}

UINT32 AmbaB8_SerdesLink(UINT32 ChipID, UINT32 SerDesRate)
{
    static UINT32 HSLinked = 0U;
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 i;

    //temps
    UINT32 StartTime = 0, EndTime = 0;
    extern UINT32 prelstx, poslstx;
    extern UINT32 SwphyClk;

#if (B8_DBG_TEMP)
    /* Hook serdes data rate */
    extern UINT32 gB8DbgSerDesRate;
    if (gB8DbgSerDesRate != 0xffU) {
        SerDesRate = gB8DbgSerDesRate;
    }
#endif

    (void) AmbaB8_KAL_GetSysTickCount(&StartTime);

    /* with Serdes Link */
    if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {

        /* Configure LsTx */
        if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
#ifdef BUILT_IN_SERDES
            AmbaSERDES_SetLinkStatus(0xffU, 1U);
            AmbaCSL_RctClearSysSoftReset();
            RetVal |= AmbaB8_PllSetSwphyClk(ChipID, SwphyClk);
            AmbaB8_SerdesLsTxConfig(ChipID, prelstx);
            AmbaCSL_VinSetRefClkSel(0);
#endif
        } else if ((ChipID & B8_SELECT_CHIP_ID_B6N) != 0U) {    /* B6N */

        } else {                                                /* B8N */
            RetVal |= AmbaB8_PllSetSwphyClk((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), SwphyClk);
            AmbaB8_SerdesLsTxConfig(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK, prelstx);
            AmbaB8_SerdesLsTxPowerUp(ChipID & B8_MAIN_CHIP_ID_B8NF_MASK);
        }

        /* Communication Reset */
        RetVal |= B8_SerdesCommReset(ChipID);

        /* HS-Link Establishment */
        if (RetVal == B8_ERR_NONE) {

#ifdef VLSI_DEBUG
            if (MemLoadTraining && HSLinked == 0U) {
                extern void AmbaB8_SerdesHsRxCdrStatus(UINT32 ChipID);
                GLOBAL_SEARCH_PARAM_s CalibParam = {0};
                UINT32 Data;

                AmbaB8_PrintStr5("Using memload PHY setup flow!!!", NULL, NULL, NULL, NULL, NULL);
                B8_LoadTrainCommand();
                AmbaB8_SerdesHsRxCdrStatus(ChipID);

                /* HsRxStatus: 0xa (digital training ok) */
                if (AmbaB8_SerdesHsRxStatus(ChipID) != 0xa) {
                    RetVal |= B8_ERR_SERDES_LINK;
                }

                /* Update EQ setting */
                AmbaB8_PrintStr5("Update fw phy setting from memload", NULL, NULL, NULL, NULL, NULL);
                AmbaB8_RegReadU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x5268, 0U, B8_DATA_WIDTH_32BIT, 1U, &Data);
                CalibParam.Cap = (Data & 0xfff);
                CalibParam.Res = ((Data >> 12) & 0x3f);
                AmbaB8_RegReadU32((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), B8_AHB_BASE_ADDR + 0x525c, 0U, B8_DATA_WIDTH_32BIT, 1U, &Data);
                CalibParam.Tap1 = ((Data >> 0) & 0x3f);
                CalibParam.Tap2 = ((Data >> 8) & 0x1f);
                CalibParam.Tap3 = ((Data >> 16) & 0xf);
                CalibParam.Tap4 = ((Data >> 20) & 0xf);
                AmbaB8_SerdesSetCalibParam(ChipID, &CalibParam);
            } else
#endif
            {
                AmbaB8_PrintUInt5("HS-Link Establishment @fhs%d", SerDesRate, 0U, 0U, 0U, 0U);

                /* Pause dco filter before tx data come out */
                if (HSLinked != 0U) {
                    AmbaB8_SerdesHsRxDCOPause(ChipID);
                }

                /* Configure HsTx PLL */
                if (HSLinked == 0U) {
                    (void) AmbaB8_PllSetMphyTx0Clk(ChipID, SerDesRate);
                }

                /* Configure HsRx (DCO) */
                if (HSLinked == 0U) {
                    AmbaB8_SerdesHsRxConfig(ChipID, SerDesRate);
                }

                /* Configure HsRx ctle */
                AmbaB8_SerdesHsRxCtleConfig(ChipID);

                /* Configure LsRx */
                if (HSLinked == 0U) {
                    AmbaB8_SerdesLsRxConfig(ChipID);
                }

                /* Configure HsTx */
                if (HSLinked == 0U) {
                    AmbaB8_SerdesHsTxConfig(ChipID);
                }

                /* Reset HsTx PHY */
                AmbaB8_SerdesHsTxReset(ChipID);

                /* Reset HsRx PHY */
                AmbaB8_SerdesHsRxReset(ChipID);

                /* HsRxStatus: 0x0 */
                if (AmbaB8_SerdesHsRxStatus(ChipID) != 0x0U) {
                    RetVal |= B8_ERR_SERDES_LINK;
                }

                /* Release HsRx PHY */
                AmbaB8_SerdesHsRxRelease(ChipID);

                /* Offset Calibration */
#ifdef VLSI_DEBUG
                if (MemLoadTraining) {
                    AmbaB8_PrintStr5("Using memload Calibration flow", NULL, NULL, NULL, NULL, NULL);
                    B8_LoadRelinkCommand();
                } else
#endif
                {
                    AmbaB8_SerdesHsRxCalibration(ChipID, SerDesRate);
                }

                /* Configure HsRx dfe */
                AmbaB8_SerdesHsRxDfeConfig(ChipID);

                /* Release HsTx PHY */
                AmbaB8_SerdesHsTxRelease(ChipID);

                /* HsRxStatus: 0x1 (hibernate) */
                if (AmbaB8_SerdesHsRxStatus(ChipID) != 0x1U) {
                    RetVal |= B8_ERR_SERDES_LINK;
                }

                /* HsTx ready */
                AmbaB8_SerdesHsTxReady(ChipID);

                /* HsTx setup */
                AmbaB8_SerdesHsTxSetup(ChipID);

                /* HsRx Release DCO pause */
                AmbaB8_SerdesHsRxDCORelease(ChipID);

                /* HsRx setup */
                AmbaB8_SerdesHsRxSetup(ChipID);

                /* HsRxStatus: 0x9 (analog training ok) */
                if (AmbaB8_SerdesHsRxStatus(ChipID) != 0x9U) {
                    RetVal |= B8_ERR_SERDES_LINK;
                }

                //AmbaB8_SerdesHsRxSslmsInit(ChipID);

                /* HsTx setup */
                AmbaB8_SerdesHsTxSetup(ChipID);

                /* HsRxStatus: 0xa (digital training ok) */
                if (AmbaB8_SerdesHsRxStatus(ChipID) != 0xaU) {
                    RetVal |= B8_ERR_SERDES_LINK;
                }

                if (RetVal == B8_ERR_NONE) {
                    HSLinked = 1U;
#ifdef VLSI_DEBUG
                    if (!MemLoadTraining)
#endif
                    {
                        /* Enable b8f power saving */
                        AmbaB8_SerdesHsRxPowerDown(ChipID);

                        /* Enable termination auto calibration */
                        AmbaB8_SerdesHsRxAutoTerm(ChipID & B8_MAIN_CHIP_ID_MASK);
                        AmbaB8_SerdesHsTxAutoTerm(ChipID);
                    }
                } else {
                    RetVal = B8_ERR_SERDES_LINK;
                    HSLinked = 0U;
                }
            }

            AmbaB8_PrintUInt5("B8 HS:%d", RetVal, 0U, 0U, 0U, 0U);
        }

        /* Configure LsTx */
        AmbaB8_SerdesLsTxConfig((ChipID), poslstx);

        if (RetVal == B8_ERR_NONE) {
            /* Setup packer, depacker, pwm-enc/dec */
            RetVal |= B8_SerdesCommInit(ChipID);

            /* Read/Write access check */
            for (i = 0U; i < B8_MAX_NUM_SUBCHIP; i++) {
                if ((ChipID & ((UINT32)0x1 << i)) != 0U) {
                    if (AmbaB8_SerdesStatus((ChipID & B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i), 10U) != 10U) {
                        RetVal |= B8_ERR_SERDES_LINK;
                        AmbaB8_PrintUInt5("B8 HS/LS:%d @ChipID:0x%x", B8_ERR_SERDES_LINK, ((ChipID &B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), 0U, 0U, 0U);
                    } else {
                        AmbaB8_PrintUInt5("B8 HS/LS:%d @ChipID:0x%x", B8_ERR_NONE, ((ChipID &B8_MAIN_CHIP_ID_MASK) | ((UINT32)1U << i)), 0U, 0U, 0U);
                    }
                }
            }
        }

        (void) AmbaB8_KAL_GetSysTickCount(&EndTime);
        AmbaB8_PrintUInt5("SpendingTime=%u", (EndTime - StartTime), 0U, 0U, 0U, 0U);
    }
    return RetVal;
}

#ifdef PHY_SHMOO
/* Low Speed Link Shmoo */
#include "AmbaGPIO.h"
#include "AmbaB8CSL_GPIO.h"
#include "AmbaB8CSL_IoMux.h"
#include "AmbaFS.h"
#include "AmbaB8Reg_PHY.h"
#include <stdio.h>

static void B8_GPIO21_Config(UINT32 ChipID, UINT32 PinLevel)
{
    UINT32 DataBuf32;

    DataBuf32 = 0x00017800U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->PinGroup0FuncSelect0, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
    DataBuf32 = 0x001e0600U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->PinGroup0FuncSelect1, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
    DataBuf32 = 0x00000000U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->PinGroup0FuncSelect2, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
    DataBuf32 = 0x00000001U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->Enable, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
    DataBuf32 = 0x00000000U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_IoMuxReg->Enable, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    DataBuf32 = 0x00000001U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->Enable, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
    DataBuf32 = 0x00200004U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinMask, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
    DataBuf32 = 0x00200004U;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinDirection, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    if (PinLevel != 0U) {
        DataBuf32 = 0x00200004U;
    } else {
        DataBuf32 = 0x00000004U;
    }
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->PinLevel, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);

    DataBuf32 = 0xffffffffU;
    (void) AmbaB8_RegWrite(ChipID, & pAmbaB8_GPIOReg->IntClear, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
}

/* B8A:J11:6(VSYNC1) - A9AQ:J19:4(GPIO40) */
void AmbaB8_SerdesLsShmoo(UINT32 ChipID, UINT32 Mode, const char *pFileName)
{
    B8_RCT_MPHY_CTRL30_REG_s LsRxReg = {0};
    UINT32 LsRxPhyGlitch[4U] = {0x00, 0x01, 0x00, 0x01};
    UINT32 LsRxPhyCtr[4U] = {0xd6, 0xd6, 0xff, 0xff};
    /* LS data rate = 12Mbps, 6Mbps, 4Mbps, 3Mbps */
    UINT32 LsTxPhyCtr[4U] = {24000000, 12000000, 8000000, 6000000};
    UINT32 PassPkt, ErrorPkt, CorPkt;
    UINT32 DataBuf;
    UINT32 StopTryingLsTxIb = 0, StopTryingLsTxR = 0U;
    UINT32 Retry = 0;
    UINT8 LsClkStep = 0, LsTxR = 0, LsTxIb = 0, LsRxStep = 0;
    UINT8 IsLinked = 1;
    UINT8 i = 0, j = 0;
    AMBA_GPIO_PIN_INFO_s SocPinInfo;
    AMBA_FS_FILE *Fptr;
    CHAR LineBuf[10];
    extern UINT32 poslstx;
    extern UINT32 SwphyClk;

    if (Mode == 0) {
        AmbaPrintColor(YELLOW, "PRE-LSTX shmoo (check LS Link quality)");
    } else if (Mode == 1) {
        AmbaPrintColor(YELLOW, "POS-LSTX shmoo (check LS Link quality)");
    } else if (Mode == 2) {
        AmbaPrintColor(YELLOW, "POS-LSTX shmoo (check HS Link quality)");
    } else if (Mode == 3) {
        AmbaPrintColor(YELLOW, "LSRX shmoo (check LS Link quality)");
    } else {
        AmbaPrintColor(YELLOW, "Invalid mode number");
        return;
    }

    Fptr = AmbaFS_fopen(pFileName, "w");
    if (Fptr == NULL) {
        AmbaB8_PrintStr5("Can't open file: %s", pFileName, NULL, NULL, NULL, NULL);
        return;
    } else {
        AmbaB8_PrintStr5("File open successfully: %s", pFileName, NULL, NULL, NULL, NULL);
    }

    /* set SOC GPIO40 as input pin */
    AmbaGPIO_ConfigInput(GPIO_PIN_21);

    /* Low Speed link initial steps */
    if (Mode == 0U) {
        if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
#ifdef BUILT_IN_SERDES
            AmbaSERDES_SetLinkStatus(0xffU, 1U);
            AmbaCSL_RctClearSysSoftReset();
            (void) AmbaB8_PllSetSwphyClk(ChipID, LsTxPhyCtr[LsClkStep]);
            AmbaCSL_VinSetRefClkSel(0);
#endif
        } else if ((ChipID & B8_SELECT_CHIP_ID_B6N) != 0U) {    /* B6N */
            /* todo */
        } else {                                                /* B8N */
            (void) AmbaB8_PllSetSwphyClk((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), LsTxPhyCtr[LsClkStep]);
        }

        AmbaB8_SerdesLsTxConfig(ChipID, (((LsTxIb & 0xf) << 4) | (LsTxR & 0xf)));
        AmbaB8_SerdesLsTxPowerUp(ChipID);

        (void) B8_SerdesCommReset(ChipID);
    }

    AmbaB8_CommDisableAck(ChipID, 1U);

    if (Mode == 0U || Mode == 1U || Mode == 2U) {
        /* sweep swphyclk */
        for (LsClkStep = 0U; LsClkStep < 4U; LsClkStep++) {

            if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
                (void) AmbaB8_PllSetSwphyClk(ChipID, LsTxPhyCtr[LsClkStep]);
            } else {
                (void) AmbaB8_PllSetSwphyClk((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), LsTxPhyCtr[LsClkStep]);
            }

            StopTryingLsTxIb = StopTryingLsTxR = 0U;

            /* sweep  lstx_ib(from 0 to 15) */
            for (LsTxIb = 0U; LsTxIb < 16U; LsTxIb++) {

                if (StopTryingLsTxIb == 0U) StopTryingLsTxR = 0U;

                /* sweep lstx_r(from 0 to 15) */
                for (LsTxR = 0U; LsTxR < 16U; LsTxR++) {

                    if (StopTryingLsTxR == 1U) {
                        IsLinked = 0U;
                    } else {
                        AmbaB8_SerdesLsTxConfig(ChipID, (((LsTxIb & 0xfU) << 4) | (LsTxR & 0xfU)));
                        AmbaPrintColor(YELLOW, "LsTx 0x%02x", (((LsTxIb & 0xfU) << 4) | (LsTxR & 0xfU)));

                        /* HS link quality, HS link start to have BER */
                        if (IsLinked == 1U && Mode == 2U) {
                            AmbaB8_DepackerResetPktCounter(ChipID);
                            AmbaKAL_TaskSleep(100);
                            AmbaB8_DepackerGetStatPkt(ChipID, &PassPkt, &ErrorPkt, &CorPkt);
                            if (PassPkt == 0U) {
                                AmbaPrint("HS link start to have BER(%u,%u,%u)(loop:%u,%u,%u) @ LsDataRate %d Mbps, LsTx Amp. 0x%x", \
                                          PassPkt, ErrorPkt, CorPkt, \
                                          LsClkStep, LsTxIb, LsTxR, \
                                          (LsTxPhyCtr[LsClkStep] / 2 / 1000000), \
                                          ((LsTxIb << 4) | LsTxR));
                                return;
                            }

                            if (PassPkt == 0U || ErrorPkt != 0U || CorPkt != 0U) {
                                IsLinked = 0U;

                                StopTryingLsTxR = 1U;
                                if (LsTxR == 0U) StopTryingLsTxIb = 1U;
                            }
                            if (IsLinked == 0U) {
                                AmbaPrint("HS link start to have BER(%u,%u,%u)(loop:%u,%u,%u) @ LsDataRate %d Mbps, LsTx Amp. 0x%x", \
                                          PassPkt, ErrorPkt, CorPkt, \
                                          LsClkStep, LsTxIb, LsTxR, \
                                          (LsTxPhyCtr[LsClkStep] / 2 / 1000000), \
                                          ((LsTxIb << 4) | LsTxR));
                            }
                        }

                        /* LS link quality, LS write access to B8F */
                        /* Set retry times to 2 because of no-replay mechanism in disable-ack mode */
                        if (IsLinked == 1U && (Mode == 0U || Mode == 1U)) {
                            Retry = 2;
                            for (i = 0; i < 10; i++) {
                                B8_GPIO21_Config(ChipID, 1U);
                                AmbaKAL_TaskSleep(1);
                                (void) AmbaGPIO_GetPinInfo(GPIO_PIN_21, &SocPinInfo);
                                if (SocPinInfo.Level != 1U) {
                                    if (Retry) {
                                        Retry--;
                                    } else {
                                        IsLinked = 0U;
                                        break;
                                    }
                                }

                                B8_GPIO21_Config(ChipID, 0U);
                                AmbaKAL_TaskSleep(1);
                                (void) AmbaGPIO_GetPinInfo(GPIO_PIN_21, &SocPinInfo);
                                if (SocPinInfo.Level != 0U) {
                                    if (Retry) {
                                        Retry--;
                                    } else {
                                        IsLinked = 0U;
                                        break;
                                    }
                                }
                            }
                            if (IsLinked == 0U) {
                                AmbaPrint("LS link fail(%d/10)(loop:%u,%u,%u) @ LsDataRate %d Mbps, LsTx Amp. 0x%x", \
                                          i, LsClkStep, LsTxIb, LsTxR, \
                                          (LsTxPhyCtr[LsClkStep] / 2 / 1000000), \
                                          ((LsTxIb << 4) | LsTxR));

                                /* Communication reset */
                                AmbaB8_SerdesLsTxConfig(ChipID, 0xff);
                                AmbaB8_CommReset(ChipID);
                                AmbaB8_SerdesLsTxConfig(ChipID, 0x00);

                            }
                        }

                    }

                    /* export results to saved file */
                    (void)sprintf(LineBuf, "%u,%u,%u,%u\r\n", IsLinked, LsClkStep, LsTxIb, LsTxR);
                    (void)AmbaFS_fwrite(LineBuf, 1, strlen(LineBuf), Fptr);

                    /* Restore Link status */
                    IsLinked = 1U;

                    /* Clear status */
#ifdef BUILT_IN_SERDES
                    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
                        AmbaSERDES_SetLinkStatus(0xffU, 1U);
                    }
#endif
                }
            }
        }
        /* Re-store */
        AmbaB8_SerdesLsTxConfig((ChipID), poslstx);
        if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
            (void) AmbaB8_PllSetSwphyClk(ChipID, SwphyClk);
        } else {
            (void) AmbaB8_PllSetSwphyClk((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), SwphyClk);
        }

    } else if (Mode == 3U) {
        LsRxReg.MphyAfeReadyTxch0 = 1U;
        LsRxReg.LsRxCTxch0 = 1U;
        LsRxReg.TxModeTxch0 = 2U;
        LsRxReg.RctMphyCtrl30Reserved2 = 2U;

        /* sweep swphyclk */
        for (LsClkStep = 0U; LsClkStep < 4U; LsClkStep++) {
            if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
                (void) AmbaB8_PllSetSwphyClk(ChipID, LsTxPhyCtr[LsClkStep]);
            } else {
                (void) AmbaB8_PllSetSwphyClk((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), LsTxPhyCtr[LsClkStep]);
            }

            /* sweep LsRx ctr/res */
            for (LsRxStep = 0U; LsRxStep < 4U; LsRxStep++) {
                LsRxReg.BpGlitchFilterTxch0 = LsRxPhyGlitch[LsRxStep];
                LsRxReg.LsRxCtrTxch0 = LsRxPhyCtr[LsRxStep];

                /* sweep LsRx res from 0x0 to 0xf */
                for (j = 0; j < 16; j++) {
                    LsRxReg.LsRxRTxch0 = j;

                    /* Use robust LsTx to make sure LsRx be configured */
                    AmbaB8_SerdesLsTxConfig(ChipID, 0x2f);
                    /* Communication reset */
                    AmbaB8_CommReset(ChipID);
                    AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5288, 0, B8_DATA_WIDTH_32BIT, 1, &LsRxReg);
                    /* Re-store LsTx */
                    AmbaB8_SerdesLsTxConfig(ChipID, poslstx);

                    /* LS link quality, LS write access to B8F */
                    Retry = 2;
                    for (i = 0; i < 10; i++) {
                        B8_GPIO21_Config(ChipID, 1U);
                        (void) AmbaGPIO_GetPinInfo(GPIO_PIN_21, &SocPinInfo);
                        if (SocPinInfo.Level != 1U) {
                            if (Retry) {
                                Retry--;
                            } else {
                                IsLinked = 0U;
                                break;
                            }
                        }

                        B8_GPIO21_Config(ChipID, 0U);
                        (void) AmbaGPIO_GetPinInfo(GPIO_PIN_21, &SocPinInfo);
                        if (SocPinInfo.Level != 0U) {
                            if (Retry) {
                                Retry--;
                            } else {
                                IsLinked = 0U;
                                break;
                            }
                        }
                    }

                    if (IsLinked == 0U) {
                        AmbaPrint("LS link fail(%d/10)(L:%u,%u,%u), LsClk %dMhz, LsTx Amp. 0x%x, LsRx 0x%x", \
                                  i, LsClkStep, LsRxStep, j, \
                                  LsTxPhyCtr[LsClkStep]/1000000, \
                                  poslstx, \
                                  (0x0a100010 | (LsRxReg.BpGlitchFilterTxch0) | (LsRxReg.LsRxCtrTxch0 << 12) | (LsRxReg.LsRxRTxch0 << 8)));
                    } else {
                        AmbaPrintColor(YELLOW, "LS link OK(L:%u,%u,%u)", LsClkStep, LsRxStep, j);
                    }


                    /* export results to saved file */
                    (void)sprintf(LineBuf, "%u,%u,%u\r\n", IsLinked, ((LsClkStep * 4) + LsRxStep), j);
                    (void)AmbaFS_fwrite(LineBuf, 1, strlen(LineBuf), Fptr);

                    /* Restore Link status */
                    IsLinked = 1U;

                    /* Clear status */
#ifdef BUILT_IN_SERDES
                    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
                        AmbaSERDES_SetLinkStatus(0xffU, 1U);
                    }
#endif
                }
            }
        }

        /* Re-store */
        if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {   /* built-in B6N */
            (void) AmbaB8_PllSetSwphyClk(ChipID, SwphyClk);
        } else {
            (void) AmbaB8_PllSetSwphyClk((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), SwphyClk);
        }
        AmbaB8_SerdesLsTxConfig(ChipID, 0x2f);
        /* Communication reset */
        AmbaB8_CommReset(ChipID);
        DataBuf = 0x0a1d6f11;
        AmbaB8_RegWriteU32(ChipID, B8_AHB_BASE_ADDR + 0x5288, 0, B8_DATA_WIDTH_32BIT, 1, &DataBuf);
        AmbaB8_SerdesLsTxConfig(ChipID, poslstx);

    }
    AmbaB8_CommDisableAck(ChipID, 0U);

    AmbaFS_fclose(Fptr);
    AmbaPrintColor(YELLOW, "Shmoo DONE");
}

#endif

