/**
 *  @file AmbaEEPROM.c
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
 *  @details Definitions for Ambarella EEPROM driver APIs
 *
 */

#include <AmbaWrap.h>
#include "AmbaKAL.h"
#include "AmbaVIN.h"
#include "AmbaEEPROM.h"
#include "AmbaSvcWrap.h"

static AMBA_EEPROM_OBJ_s *pAmbaEEPROMObj[AMBA_NUM_VIN_CHANNEL] GNU_SECTION_NOZEROINIT;

static void EEPROM_MemInit(void)
{
    static UINT32 EEPROMMemInit = 0U;

    if (EEPROMMemInit == 0U) {
        AmbaSvcWrap_MisraMemset(pAmbaEEPROMObj, 0, sizeof(AMBA_EEPROM_OBJ_s *) * AMBA_NUM_VIN_CHANNEL);
        EEPROMMemInit = 1U;
    }
}

/**
 * Hook EEPROM object
 *
 * @param [in] pChan EEPROM channel
 * @param [in] pEEPROMObj EEPROM Object
 * @return ErrCode
 */
UINT32 AmbaEEPROM_Hook(const AMBA_EEPROM_CHANNEL_s *pChan, AMBA_EEPROM_OBJ_s *pEEPROMObj)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    EEPROM_MemInit();

    if ((pEEPROMObj == NULL) || (pChan == NULL)) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
            RetVal = AMBA_EEPROM_ERR_ARG;
        } else {
            pAmbaEEPROMObj[pChan->VinId] = pEEPROMObj;
        }
    }

    return RetVal;
}

/**
 * Initial EEPROM driver
 *
 * @param [in] pChan EEPROM channel
 * @return ErrCode
 */
UINT32 AmbaEEPROM_Init(const AMBA_EEPROM_CHANNEL_s *pChan)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    EEPROM_MemInit();

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
            RetVal = AMBA_EEPROM_ERR_ARG;
        } else if (pAmbaEEPROMObj[pChan->VinId] == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else if (pAmbaEEPROMObj[pChan->VinId]->Init == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else {
            RetVal = (pAmbaEEPROMObj[pChan->VinId]->Init)(pChan);
        }
    }

    return RetVal;
}

/**
 * Read data from EEPROM
 *
 * @param [in] pChan EEPROM channel
 * @param [in] pCmdCfg EEPROM cmd config
 * @return ErrCode
 */
UINT32 AmbaEEPROM_Read(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    EEPROM_MemInit();

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
            RetVal = AMBA_EEPROM_ERR_ARG;
        } else if (pAmbaEEPROMObj[pChan->VinId] == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else if (pAmbaEEPROMObj[pChan->VinId]->Read == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else {
            RetVal = (pAmbaEEPROMObj[pChan->VinId]->Read)(pChan, pCmdCfg);
        }
    }

    return RetVal;
}

/**
 * Write data to EEPROM
 *
 * @param [in] pChan EEPROM channel
 * @param [in] pCmdCfg EEPROM cmd config
 * @return ErrCode
 */
UINT32 AmbaEEPROM_Write(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    EEPROM_MemInit();

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
            RetVal = AMBA_EEPROM_ERR_ARG;
        } else if (pAmbaEEPROMObj[pChan->VinId] == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else if (pAmbaEEPROMObj[pChan->VinId]->Write == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else {
            RetVal = (pAmbaEEPROMObj[pChan->VinId]->Write)(pChan, pCmdCfg);
        }
    }

    return RetVal;
}

/**
 * Get EEPROM device info
 *
 * @param [in] pChan EEPROM channel
 * @param [out] pDeviceInfo device info
 * @return ErrCode
 */
UINT32 AmbaEEPROM_GetDeviceInfo(const AMBA_EEPROM_CHANNEL_s *pChan, AMBA_EEPROM_DEVICE_INFO_s *pDeviceInfo)
{
    UINT32 RetVal = AMBA_EEPROM_ERR_NONE;

    EEPROM_MemInit();

    if (pChan == NULL) {
        RetVal = AMBA_EEPROM_ERR_ARG;
    } else {
        if (pChan->VinId >= AMBA_NUM_VIN_CHANNEL) {
            RetVal = AMBA_EEPROM_ERR_ARG;
        } else if (pAmbaEEPROMObj[pChan->VinId] == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else if (pAmbaEEPROMObj[pChan->VinId]->GetDeviceInfo == NULL) {
            RetVal = AMBA_EEPROM_ERR_INVALID_API;
        } else {
            RetVal = (pAmbaEEPROMObj[pChan->VinId]->GetDeviceInfo)(pChan, pDeviceInfo);
        }
    }

    return RetVal;
}

