/**
 *  @file AmbaSensor_OV48C40.c
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
 *  @details Control APIs of OV OV48C40 CMOS sensor with MIPI interface
 *
 */

#include "AmbaLens_OV48C40_LC898249XH.h"
#include <AmbaWrap.h>
#include "AmbaVIN.h"
#include "AmbaUtility.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "bsp.h"
#include "AmbaGPIO_Def.h"


static UINT32 AF_OV48C40I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_SENSOR_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_SENSOR_I2C_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_SENSOR_I2C_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_SENSOR_I2C_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_SENSOR_I2C_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_SENSOR_I2C_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_SENSOR_I2C_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_SENSOR_I2C_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_SENSOR_I2C_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_SENSOR_I2C_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_SENSOR_I2C_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_SENSOR_I2C_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_SENSOR_I2C_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_SENSOR_I2C_CHANNEL_VIN13,
};

UINT32 OV48C40_AF_LensInit(UINT32 VinID)
{
    UINT32 RetVal = LENS_ERR_NONE;
    UINT16 Addr = 0U;
    UINT32 TRSize = 0U;
    UINT8 R_Data8[4] = {0U}; // Specify 16 for MisraC
    UINT8 T_Data8[16] = {0U};

    // slave address check
    Addr = 0xF0;
    TRSize = 1U;
    if (OV48C40_AF_RegRead(VinID, Addr, R_Data8, TRSize) != LENS_ERR_NONE) {
        AmbaPrint_PrintUInt5("Register read error\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_INIT_FAIL;
    }

    if (R_Data8[0] != 0xA5U) {
        AmbaPrint_PrintUInt5("Reg 0xF0 != 0xA5\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_INIT_FAIL;
    }

    if(AmbaKAL_TaskSleep(100) != 0U) {
        AmbaPrint_PrintUInt5("TaskSleep error\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_INIT_FAIL;
    }

    // initial data download
    Addr = 0xE0;
    TRSize = 1U;
    T_Data8[0] = 0x01;
    if (OV48C40_AF_RegWrite(VinID, Addr, T_Data8, TRSize) != LENS_ERR_NONE) {
        AmbaPrint_PrintUInt5("Register read error\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_INIT_FAIL;
    }

    if(AmbaKAL_TaskSleep(100) != 0U) {
        AmbaPrint_PrintUInt5("TaskSleep error\n", 0U, 0U, 0U, 0U, 0U);
    }

    Addr = 0xB3;
    TRSize = 1U;
    if (OV48C40_AF_RegRead(VinID, Addr, R_Data8, TRSize) != LENS_ERR_NONE) {
        AmbaPrint_PrintUInt5("Register read error\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_INIT_FAIL;
    }
    return RetVal;
}

UINT32 OV48C40_AF_ReadPos(UINT32 VinID)
{
    UINT32 RetVal = LENS_ERR_NONE;
    UINT16 Addr = 0U;
    UINT32 TRSize = 0U;
    UINT8 R_Data8[16] = {0U}; // Specify 16 for MisraC

    // slave address
    Addr = 0x0A;
    TRSize = 2U;
    if (OV48C40_AF_RegRead(VinID, Addr, R_Data8, TRSize) != LENS_ERR_NONE) {
        AmbaPrint_PrintUInt5("Register read error\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_REGREAD_FAIL;
    }

    // slave address : pos
    Addr = 0x84;
    if (OV48C40_AF_RegRead(VinID, Addr, R_Data8, TRSize) != LENS_ERR_NONE) {
        AmbaPrint_PrintUInt5("Register read error\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_REGREAD_FAIL;
    }

    return RetVal;
}


UINT32 OV48C40_AF_LensOper(UINT32 VinID, UINT32 u_AfOpt)
{
    UINT32 RetVal = LENS_ERR_NONE;
    UINT16 Addr = 0U;
    UINT32 TRSize = 0U;
    UINT8 T_Data8[16] = {0U};
    UINT8 OptData[4] = {0U};
    UINT32 AfOpt = u_AfOpt;

    // slave address check
    Addr = 0x84;
    TRSize = 2U;

    if(AfOpt > 1023U) {
        AfOpt = 1023U;
    }

    (void)AmbaMisra_TypeCast(&OptData[0], &AfOpt);

    for(UINT8 i = 0U; i<2U; i++) {
        T_Data8[1U-i]= OptData[i];
    }

    if (OV48C40_AF_RegWrite(VinID, Addr, T_Data8, TRSize) != LENS_ERR_NONE) {
        AmbaPrint_PrintUInt5("Register read error\n", 0U, 0U, 0U, 0U, 0U);
        RetVal = LENS_ERR_OPER_FAIL;
    }
    return RetVal;
}


UINT32 OV48C40_AF_RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 * pTxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[128U];
    UINT32 TxSize, i;

    if (Size > 126U) {
        RetVal = LENS_ERR_REGWRITE_FAIL;
    } else {
        I2cConfig.SlaveAddr = 0xE4;
        I2cConfig.DataSize  = (1U + Size);         // Modify by Kychena
        I2cConfig.pDataBuf  = TxDataBuf;

        TxDataBuf[0] = (UINT8) (Addr & 0x00ffU);             // Modify by Kychena
        for (i = 0U; i < Size; i++) {
            TxDataBuf[i + 1U] = pTxData[i];
            AmbaPrint_PrintUInt5("[OV48C40]OV48C40_AF_RegWrite Addr = %x, Data[%d] = %x", Addr, i, TxDataBuf[i + 1U], 0, 0);
        }

        RetVal = AmbaI2C_MasterWrite(AF_OV48C40I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                     &I2cConfig, &TxSize, 1000U);

        if (RetVal != I2C_ERR_NONE) {
            AmbaPrint_PrintStr5("[OV48C40_AF] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return RetVal;
}

UINT32 OV48C40_AF_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;
    UINT32 i = 0U;


    I2cTxConfig.SlaveAddr = 0xE4;
    I2cTxConfig.DataSize  = 1U;                    // Modify by Kychena
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = 0xE5;
    I2cRxConfig.DataSize  = Size;
    I2cRxConfig.pDataBuf  = pRxData;
    RetVal = AmbaI2C_MasterReadAfterWrite(AF_OV48C40I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                          &I2cRxConfig, &TxSize, 1000U);
    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_PrintStr5("[OV48C40_AF] I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    for (i = 0; i < Size; i++) {
        AmbaPrint_PrintUInt5("[OV48C40]OV48C40_AF_RegRead Addr = %x, Data[%d] = %x", Addr, i, pRxData[i], 0, 0);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_LENS_OBJ_s AmbaLens_OV48C40Obj = {
    .Init               = OV48C40_AF_LensInit,
    .Oper                = OV48C40_AF_LensOper,
    .RegRead            = OV48C40_AF_RegRead,
    .RegWrite            = OV48C40_AF_RegWrite,
    .GetStatus            = OV48C40_AF_ReadPos,
};
