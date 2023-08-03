/**
 *  @file AmbaEEPROM.h
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

#ifndef AMBA_EEPROM_H
#define AMBA_EEPROM_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

typedef struct {
    UINT32 MaxCapacity;     /* in Byte */
    UINT32 Reserved;
} AMBA_EEPROM_DEVICE_INFO_s;

typedef struct {
    UINT32 VinId;
    UINT32 ChanId;          /* EEPROM channel id */
} AMBA_EEPROM_CHANNEL_s;

typedef struct {
    UINT32  Offset;
    UINT32  DataSize;
    void   *pDataBuf;
} AMBA_EEPROM_CMD_CFG_s;

typedef struct {
    char pName[64];            /* Name of the EEPROM */
    UINT32 (*Init)(const AMBA_EEPROM_CHANNEL_s *pChan);
    UINT32 (*Read)(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg);
    UINT32 (*Write)(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg);
    UINT32 (*GetDeviceInfo)(const AMBA_EEPROM_CHANNEL_s *pChan, AMBA_EEPROM_DEVICE_INFO_s *pDeviceInfo);
} AMBA_EEPROM_OBJ_s;

/* EEPROM ERROR CODE */
#define AMBA_EEPROM_ERR_NONE        (0U)
#define AMBA_EEPROM_ERR_INVALID_API (SENSOR_ERR_BASE + 0x100U)
#define AMBA_EEPROM_ERR_ARG         (SENSOR_ERR_BASE + 0x200U)
#define AMBA_EEPROM_ERR_COMMUNICATE (SENSOR_ERR_BASE + 0x300U)

/* print module */
#define AMBA_EEPROM_MODULE_ID       ((UINT16)(SENSOR_ERR_BASE >> 16U))

/*-----------------------------------------------------------------------------------------------*\
 * Defined in EEPROM.c
\*-----------------------------------------------------------------------------------------------*/
extern UINT32 AmbaEEPROM_Hook(const AMBA_EEPROM_CHANNEL_s *pChan, AMBA_EEPROM_OBJ_s *pEEPROMObj);
extern UINT32 AmbaEEPROM_Init(const AMBA_EEPROM_CHANNEL_s *pChan);
extern UINT32 AmbaEEPROM_Read(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg);
extern UINT32 AmbaEEPROM_Write(const AMBA_EEPROM_CHANNEL_s *pChan, const AMBA_EEPROM_CMD_CFG_s *pCmdCfg);
extern UINT32 AmbaEEPROM_GetDeviceInfo(const AMBA_EEPROM_CHANNEL_s *pChan, AMBA_EEPROM_DEVICE_INFO_s *pDeviceInfo);

#endif /* AMBA_EEPROM_H */
