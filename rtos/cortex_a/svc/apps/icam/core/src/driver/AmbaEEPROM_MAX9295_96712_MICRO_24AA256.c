/**
 *  @file AmbaEEPROM_MAX9295_96712_MICRO_24AA256.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 *  @details Definitions for EEPROM MAX9295_96712 MICRO 24AA256
 *
 */
#include "AmbaWrap.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaPrint.h"
#include "AmbaVIN.h"

#include "bsp.h"

#include "AmbaI2C.h"
#include "AmbaSbrg_Max9295_96712.h"
#include "AmbaEEPROM.h"
#include "AmbaSvcWrap.h"
#include "AmbaEEPROM_MAX9295_96712_MICRO_24AA256.h"

#define MX01_EEPROM_CTRL_CHAN_INIT  (0x2U)
#define MX01_EEPROM_CTRL_MUTX_INIT  (0x4U)

#define M01_EEPROM_WRITE_CYCLE_TIME     (5)
#define M01_EEPROM_MAX_CHANNEL      (4U)
#define M01_EEPROM_MAXIM_I2C_SRC_B  (0x44U)
#define M01_EEPROM_MAXIM_I2C_DST_B  (0x45U)

typedef struct {
    UINT32           State;
    AMBA_KAL_MUTEX_t Mutx;
    UINT32           SerDesChipId;
    UINT32           I2cSrcBVal;
    UINT32           I2cDstBVal;
} M01_EEPROM_CHAN_CTRL_s;

typedef struct {
    UINT32                 I2cChan;
    M01_EEPROM_CHAN_CTRL_s Chan[M01_EEPROM_MAX_CHANNEL];
} M01_EEPROM_MAX9295_CTRL_s;

static M01_EEPROM_MAX9295_CTRL_s M01_EEPROMCtrl[AMBA_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

static inline UINT32 GetChanI2CAddr(UINT32 ChanId)
{
    return (MAX9295_96712_EEPROM_VIRTUAL_I2C_BASE_ADDR + (ChanId << 1));
}

static UINT32 TransferI2CAddr(const AMBA_EEPROM_CHANNEL_s *pChan, UINT32 I2cSrcBVal, UINT32 I2cDstBVal)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE, PRetVal;

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->ChanId >= M01_EEPROM_MAX_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        M01_EEPROM_CHAN_CTRL_s *pCtrl = &(M01_EEPROMCtrl[pChan->VinId].Chan[pChan->ChanId]);
        UINT8 RxData;
        UINT32 MaxWriteReg;

        // Pre-process
        if (pCtrl->I2cSrcBVal == 0xDEADBEEFU) {
            RxData = 0U;
            PRetVal = Max9295_96712_RegisterRead(pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_SRC_B, &RxData);
            if (PRetVal != 0U) {
                RetVal |= AMBA_EEPROM_ERR_COMMUNICATE;
                AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] Read VinID(%d) ChipID(%d) Reg(0x%x) fail, RetVal: 0x%x ", pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_SRC_B, PRetVal, 0U);
            } else {
                AmbaMisra_TypeCast(&(pCtrl->I2cSrcBVal), &(RxData)); pCtrl->I2cSrcBVal &= 0xFFU;
            }
        }

        if (pCtrl->I2cDstBVal == 0xDEADBEEFU) {
            RxData = 0U;
            PRetVal = Max9295_96712_RegisterRead(pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_DST_B, &RxData);
            if (PRetVal != 0U) {
                RetVal |= AMBA_EEPROM_ERR_COMMUNICATE;
                AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] Read VinID(%d) ChipID(%d) Reg(0x%x) fail, RetVal: 0x%x ", pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_DST_B, PRetVal, 0U);
            } else {
                AmbaMisra_TypeCast(&(pCtrl->I2cDstBVal), &(RxData)); pCtrl->I2cDstBVal &= 0xFFU;
            }
        }

        if ((I2cSrcBVal != GetChanI2CAddr(pChan->ChanId)) || (I2cDstBVal != MAX9295_96712_MICRO_24AA256_I2C_SLAVE_ADDR)) {
            pCtrl->I2cSrcBVal = 0xDEADBEEFU;
            pCtrl->I2cDstBVal = 0xDEADBEEFU;
        }

        MaxWriteReg = 3U;
        do {
            PRetVal = Max9295_96712_RegisterWrite(pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_SRC_B, (UINT8) I2cSrcBVal);
            MaxWriteReg --;
        } while ((PRetVal != 0U) && (MaxWriteReg > 0U));
        if (PRetVal != 0U) {
            RetVal |= AMBA_EEPROM_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] Write VinID(%d) ChipID(%d) Reg(0x%x) val(%d) fail, RetVal: 0x%x ", pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_SRC_B, I2cSrcBVal, PRetVal);
        }


        MaxWriteReg = 3U;
        do {
            PRetVal = Max9295_96712_RegisterWrite(pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_DST_B, (UINT8) I2cDstBVal);
            MaxWriteReg --;
        } while ((PRetVal != 0U) && (MaxWriteReg > 0U));
        if (PRetVal != 0U) {
            RetVal |= AMBA_EEPROM_ERR_COMMUNICATE;
            AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] Write VinID(%d) ChipID(%d) Reg(0x%x) val(%d) fail, RetVal: 0x%x ", pChan->VinId, pCtrl->SerDesChipId, M01_EEPROM_MAXIM_I2C_DST_B, I2cDstBVal, PRetVal);
        }
    }

    return RetVal;
}

static UINT32 MAX9295_96712_MICRO_24AA256_Lock(const AMBA_EEPROM_CHANNEL_s *pChan)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->ChanId >= M01_EEPROM_MAX_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        M01_EEPROM_CHAN_CTRL_s *pCtrl = &(M01_EEPROMCtrl[pChan->VinId].Chan[pChan->ChanId]);

        RetVal = AmbaKAL_MutexTake(&(pCtrl->Mutx), 30000U);
        if (RetVal != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] VinId(%d) ChanId(%d) Take mutex timeout. RetVal: 0x%x", pChan->VinId, pChan->ChanId, RetVal, 0U, 0U);
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else {
            RetVal = TransferI2CAddr(pChan, GetChanI2CAddr(pChan->ChanId), (UINT32) MAX9295_96712_MICRO_24AA256_I2C_SLAVE_ADDR);
            if (RetVal != 0U) {
                (void) AmbaKAL_MutexGive(&(pCtrl->Mutx));
            }
        }
    }

    return RetVal;
}

static UINT32 MAX9295_96712_MICRO_24AA256_UnLock(const AMBA_EEPROM_CHANNEL_s *pChan)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->ChanId >= M01_EEPROM_MAX_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        M01_EEPROM_CHAN_CTRL_s *pCtrl = &(M01_EEPROMCtrl[pChan->VinId].Chan[pChan->ChanId]);

        (void) TransferI2CAddr(pChan, pCtrl->I2cSrcBVal, pCtrl->I2cDstBVal);

        (void) AmbaKAL_MutexGive(&(pCtrl->Mutx));
    }

    return RetVal;
}

static UINT32 MAX9295_96712_MICRO_24AA256_Init(const AMBA_EEPROM_CHANNEL_s *pChan)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;
    static UINT32 EEPROMCtrlMemInit = 0U;

    if (EEPROMCtrlMemInit == 0U) {
        AmbaSvcWrap_MisraMemset(M01_EEPROMCtrl, 0, sizeof(M01_EEPROMCtrl));

#if   defined(CONFIG_SOC_CV22)
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL0].I2cChan = AMBA_MAXIM_I2C_CHANNEL;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL1].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP;
#elif defined(CONFIG_SOC_CV2)
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL0].I2cChan = AMBA_MAXIM_I2C_CHANNEL;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL1].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL2].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP2;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL3].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP3;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL4].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP4;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL5].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP5;
#elif defined(CONFIG_SOC_CV25)
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL0].I2cChan = AMBA_MAXIM_I2C_CHANNEL;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL1].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL2].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP2;
#elif defined(CONFIG_SOC_CV2FS)
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL0].I2cChan = AMBA_MAXIM_I2C_CHANNEL;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL1].I2cChan = AMBA_MAXIM_I2C_CHANNEL_VIN1;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL2].I2cChan = AMBA_MAXIM_I2C_CHANNEL_VIN2;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL3].I2cChan = AMBA_MAXIM_I2C_CHANNEL_VIN3;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL4].I2cChan = AMBA_MAXIM_I2C_CHANNEL_VIN4;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL8].I2cChan = AMBA_MAXIM_I2C_CHANNEL_VIN8;
#elif defined(CONFIG_SOC_H32)
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL0].I2cChan = AMBA_MAXIM_I2C_CHANNEL;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL1].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP;
        M01_EEPROMCtrl[AMBA_VIN_CHANNEL2].I2cChan = AMBA_MAXIM_I2C_CHANNEL_PIP2;
#endif

        EEPROMCtrlMemInit = 1U;
    }

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {

        UINT32 ChanIdx;
        AMBA_EEPROM_CHANNEL_s LocalChan;
        M01_EEPROM_CHAN_CTRL_s *pCtrl = NULL;
        UINT32 SerDesChipID[M01_EEPROM_MAX_CHANNEL] = {
            MX01_MAX9295_A_ID,
            MX01_MAX9295_B_ID,
            MX01_MAX9295_C_ID,
            MX01_MAX9295_D_ID
        };

        AmbaSvcWrap_MisraMemcpy(&LocalChan, pChan, sizeof(AMBA_EEPROM_CHANNEL_s));

        for (ChanIdx = 0U; ChanIdx < M01_EEPROM_MAX_CHANNEL; ChanIdx ++) {
            if ((pChan->ChanId & (0x10UL << (ChanIdx << 2UL))) > 0U) {
                pCtrl = &(M01_EEPROMCtrl[pChan->VinId].Chan[ChanIdx]);
                pCtrl->SerDesChipId = SerDesChipID[ChanIdx];

                if (RetVal == AMBA_EEPROM_ERR_NONE) {
                    pCtrl->I2cSrcBVal = 0xDEADBEEFU;
                    pCtrl->I2cDstBVal = 0xDEADBEEFU;

                    if ((pCtrl->State & MX01_EEPROM_CTRL_MUTX_INIT) == 0U) {
                        AmbaSvcWrap_MisraMemset(&(pCtrl->Mutx), 0, sizeof(AMBA_KAL_MUTEX_t));
                        RetVal = AmbaKAL_MutexCreate(&(pCtrl->Mutx), AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj.pName);
                        if (RetVal != 0U) {
                            AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] MAX9295_96712 MICRO 24AA256, Create mutex fail, RetVal: 0x%x ", RetVal, 0U, 0U, 0U, 0U);
                        } else {
                            pCtrl->State |= MX01_EEPROM_CTRL_MUTX_INIT;
                        }
                    }

                    if (RetVal == 0U) {
                        pCtrl->State |= MX01_EEPROM_CTRL_CHAN_INIT;
                        AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] Initialize MAX9295_96712 MICRO 24AA256, VinID(%d), ChanId(%d)", pChan->VinId, ChanIdx, 0U, 0U, 0U);
                    }
                }

                if (RetVal != AMBA_EEPROM_ERR_NONE) {
                    break;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 ByteDataWrite(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    if ((pChan == NULL) || (pCmdCfg == NULL)) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->ChanId >= M01_EEPROM_MAX_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pCmdCfg->pDataBuf == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        AMBA_I2C_TRANSACTION_s I2cConfig = {0};
        UINT8 TxDataBuf[3];
        UINT32 TxSize;
        UINT8 *pBuf = NULL;

        AmbaMisra_TypeCast(&(pBuf), &(pCmdCfg->pDataBuf));
        if (pBuf != NULL) {
            I2cConfig.SlaveAddr = GetChanI2CAddr(pChan->ChanId);
            I2cConfig.DataSize  = 3U;
            I2cConfig.pDataBuf  = TxDataBuf;

            TxDataBuf[0] = (UINT8) (pCmdCfg->Offset >> 8U);
            TxDataBuf[1] = (UINT8) (pCmdCfg->Offset & 0xffU);
            TxDataBuf[2] = pBuf[0];

            RetVal = AmbaI2C_MasterWrite(M01_EEPROMCtrl[pChan->VinId].I2cChan, AMBA_I2C_SPEED_STANDARD, &I2cConfig, &TxSize, 1000U);

            if (RetVal != I2C_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] I2C does not work!!!!!", 0U, 0U, 0U, 0U, 0U);
                RetVal = AMBA_EEPROM_ERR_COMMUNICATE;
            }
        }
        AmbaMisra_TouchUnused(pBuf);
    }

    return RetVal;
}

static UINT32 PageDataWrite(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    if ((pChan == NULL) || (pCmdCfg == NULL)) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->ChanId >= M01_EEPROM_MAX_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pCmdCfg->pDataBuf == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        AMBA_I2C_TRANSACTION_s I2cConfig = {0};
        UINT8 TxDataBuf[M01_MICRO_24AA256_EEPROM_PAGE_SIZE + 2U];
        UINT32 TxSize, i;
        UINT8 *pBuf = NULL;

        AmbaMisra_TypeCast(&(pBuf), &(pCmdCfg->pDataBuf));
        if (pBuf != NULL) {
            I2cConfig.SlaveAddr = GetChanI2CAddr(pChan->ChanId);
            I2cConfig.DataSize  = (pCmdCfg->DataSize + 2U);
            I2cConfig.pDataBuf  = TxDataBuf;

            AmbaSvcWrap_MisraMemset(TxDataBuf, 0, sizeof(TxDataBuf));
            TxDataBuf[0] = (UINT8) (pCmdCfg->Offset >> 8U);
            TxDataBuf[1] = (UINT8) (pCmdCfg->Offset & 0xffU);
            for (i = 0; i < pCmdCfg->DataSize; i++) {
                TxDataBuf[i+2U] = pBuf[i];
            }

            RetVal = AmbaI2C_MasterWrite(M01_EEPROMCtrl[pChan->VinId].I2cChan, AMBA_I2C_SPEED_STANDARD, &I2cConfig, &TxSize, 1000U);

            if (RetVal != I2C_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] I2C does not work!!!!!", 0U, 0U, 0U, 0U, 0U);
                RetVal = AMBA_EEPROM_ERR_COMMUNICATE;
            }
        }
        AmbaMisra_TouchUnused(pBuf);
    }

    return RetVal;
}

static inline UINT16 PageUpperBoundAddr(UINT16 CurAddr)
{
    UINT16 PageUpperBound;

    PageUpperBound = (UINT16)(1U + (CurAddr/M01_MICRO_24AA256_EEPROM_PAGE_SIZE)) * M01_MICRO_24AA256_EEPROM_PAGE_SIZE;

    return (PageUpperBound - 1U);
}

static UINT32 MAX9295_96712_MICRO_24AA256_Write(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    if ((pChan == NULL) || (pCmdCfg == NULL)) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pCmdCfg->pDataBuf == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        UINT32 ChanIdx;
        AMBA_EEPROM_CHANNEL_s LocalChan;

        AmbaSvcWrap_MisraMemcpy(&LocalChan, pChan, sizeof(AMBA_EEPROM_CHANNEL_s));

        for (ChanIdx = 0U; ChanIdx < M01_EEPROM_MAX_CHANNEL; ChanIdx ++) {
            if ((pChan->ChanId & (0x10UL << (ChanIdx << 2UL))) > 0U) {
                LocalChan.ChanId = ChanIdx;

                if (0U == MAX9295_96712_MICRO_24AA256_Lock(&LocalChan)) {
                    AMBA_EEPROM_CMD_CFG_s WriteCfg;
                    UINT16 OffsetAddr = (UINT16) pCmdCfg->Offset;
                    UINT8  *pBuf      = NULL;
                    UINT32 RemainSize = pCmdCfg->DataSize;
                    UINT16 PageMaxAddr = 0;
                    UINT16 WriteSize = 0;
                    UINT32 BufId = 0;

                    AmbaMisra_TypeCast(&(pBuf), &(pCmdCfg->pDataBuf));

                    AmbaSvcWrap_MisraMemcpy(&WriteCfg, pCmdCfg, sizeof(AMBA_EEPROM_CMD_CFG_s));

                    if (RemainSize > 1U) {
                        UINT32 OffsetUpper = (pCmdCfg->Offset + ( M01_MICRO_24AA256_EEPROM_PAGE_SIZE - 1U ) ) & ( ~( (UINT32)(M01_MICRO_24AA256_EEPROM_PAGE_SIZE - 1U) ) );

                        if (((pCmdCfg->Offset + pCmdCfg->DataSize) > OffsetUpper) &&
                            (pCmdCfg->Offset < OffsetUpper)) {

                            WriteCfg.Offset   = OffsetAddr;
                            WriteCfg.pDataBuf = &pBuf[BufId];
                            WriteCfg.DataSize = OffsetUpper - pCmdCfg->Offset;

                            RetVal = PageDataWrite(&LocalChan, &WriteCfg);

                            (void)AmbaKAL_TaskSleep(M01_EEPROM_WRITE_CYCLE_TIME + 1);  /* Delay for page write cycle time */

                            if (RetVal == AMBA_EEPROM_ERR_NONE) {
                                // Change the offset
                                OffsetAddr += (UINT16)(WriteCfg.DataSize);
                                BufId += WriteCfg.DataSize;
                                RemainSize -= WriteCfg.DataSize;
                            } else {
                                RemainSize = 0U;
                            }
                        }

                        // Page Write
                        while (RemainSize > M01_MICRO_24AA256_EEPROM_PAGE_SIZE) {
                            PageMaxAddr = PageUpperBoundAddr(OffsetAddr);
                            WriteSize = (PageMaxAddr - OffsetAddr) + 1U;

                            WriteCfg.Offset   = OffsetAddr;
                            WriteCfg.pDataBuf = &pBuf[BufId];
                            WriteCfg.DataSize = WriteSize;

                            RetVal = PageDataWrite(&LocalChan, &WriteCfg);

                            (void)AmbaKAL_TaskSleep(M01_EEPROM_WRITE_CYCLE_TIME + 1);  /* Delay for page write cycle time */

                            if (RetVal == AMBA_EEPROM_ERR_NONE) {
                                // Change the offset
                                OffsetAddr += WriteSize;
                                BufId += WriteSize;
                                RemainSize -= WriteSize;
                            } else {
                                RemainSize = 0U;
                            }
                        }

                        if (RemainSize > 0U) {
                            WriteCfg.Offset   = OffsetAddr;
                            WriteCfg.pDataBuf = &pBuf[BufId];
                            WriteCfg.DataSize = RemainSize;

                            RetVal = PageDataWrite(&LocalChan, &WriteCfg);

                            (void)AmbaKAL_TaskSleep(M01_EEPROM_WRITE_CYCLE_TIME + 1);  /* Delay for page write cycle time */
                        }
                    } else {
                        // Byte Write
                        RetVal = ByteDataWrite(&LocalChan, &WriteCfg);
                    }

                    (void) MAX9295_96712_MICRO_24AA256_UnLock(&LocalChan);
                }
            }
        }
    }

    return RetVal;
}


static UINT32 MAX9295_96712_MICRO_24AA256_Read(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    if ((pChan == NULL) || (pCmdCfg == NULL)) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pCmdCfg->pDataBuf == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        UINT32 ChanIdx;
        AMBA_EEPROM_CHANNEL_s LocalChan;

        AmbaSvcWrap_MisraMemcpy(&LocalChan, pChan, sizeof(AMBA_EEPROM_CHANNEL_s));

        for (ChanIdx = 0U; ChanIdx < M01_EEPROM_MAX_CHANNEL; ChanIdx ++) {
            if ((pChan->ChanId & (0x10UL << (ChanIdx << 2UL))) > 0U) {
                LocalChan.ChanId = ChanIdx;

                if (0U == MAX9295_96712_MICRO_24AA256_Lock(&LocalChan)) {

                    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
                    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
                    UINT8 TxData[2];
                    UINT32 TxSize;

                    I2cTxConfig.SlaveAddr = GetChanI2CAddr(LocalChan.ChanId);
                    I2cTxConfig.DataSize  = 2U;
                    I2cTxConfig.pDataBuf  = TxData;
                    TxData[0] = (UINT8) (pCmdCfg->Offset >> 8U);
                    TxData[1] = (UINT8) (pCmdCfg->Offset & 0xffU);

                    I2cRxConfig.SlaveAddr = GetChanI2CAddr(LocalChan.ChanId) | (UINT32)1U;
                    I2cRxConfig.DataSize  = pCmdCfg->DataSize;
                    AmbaMisra_TypeCast(&(I2cRxConfig.pDataBuf), &(pCmdCfg->pDataBuf));

                    RetVal = AmbaI2C_MasterReadAfterWrite(M01_EEPROMCtrl[LocalChan.VinId].I2cChan, AMBA_I2C_SPEED_FAST_PLUS, 1U,
                                                            &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);

                    if (RetVal != I2C_ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(AMBA_EEPROM_MODULE_ID, "[EEPROM] I2C does not work!!!!!", 0U, 0U, 0U, 0U, 0U);
                        RetVal = AMBA_EEPROM_ERR_COMMUNICATE;
                    }

                    (void) MAX9295_96712_MICRO_24AA256_UnLock(&LocalChan);
                }
            }
        }
    }

    return RetVal;
}

static UINT32 MAX9295_96712_MICRO_24AA256_GetDeviceInfo(const AMBA_EEPROM_CHANNEL_s *pChan, AMBA_EEPROM_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    if ((pChan == NULL) || (pDeviceInfo == NULL)) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        UINT32 Addr;

        if ((pChan->ChanId & M01_EEPROM_24AA256_CHAN_A) > 0U) {
            Addr = GetChanI2CAddr(0);
        } else if ((pChan->ChanId & M01_EEPROM_24AA256_CHAN_B) > 0U) {
            Addr = GetChanI2CAddr(1);
        } else if ((pChan->ChanId & M01_EEPROM_24AA256_CHAN_C) > 0U) {
            Addr = GetChanI2CAddr(2);
        } else if ((pChan->ChanId & M01_EEPROM_24AA256_CHAN_D) > 0U) {
            Addr = GetChanI2CAddr(3);
        } else {
            Addr = 0U;
        }

        if ((Addr == M01_MICRO_24AA256_EEPROM_CHAN_A_ADDR) ||
            (Addr == M01_MICRO_24AA256_EEPROM_CHAN_B_ADDR) ||
            (Addr == M01_MICRO_24AA256_EEPROM_CHAN_C_ADDR) ||
            (Addr == M01_MICRO_24AA256_EEPROM_CHAN_D_ADDR)) {
            pDeviceInfo->MaxCapacity = 32 * 1024;    /* in byte */
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_EEPROM_OBJ_s AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj = {
    .pName           = "MAX9295_96712_MICRO_24AA256",
    .Init            = MAX9295_96712_MICRO_24AA256_Init,
    .Write           = MAX9295_96712_MICRO_24AA256_Write,
    .Read            = MAX9295_96712_MICRO_24AA256_Read,
    .GetDeviceInfo   = MAX9295_96712_MICRO_24AA256_GetDeviceInfo,
};
