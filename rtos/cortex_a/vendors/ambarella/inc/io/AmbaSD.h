/**
 *  @file AmbaSD.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for SD Controller APIs
 *
 */

#ifndef AMBA_SD_H
#define AMBA_SD_H

#include "AmbaSD_Def.h"

/*
 * Defined in AmbaSD.c
 */
UINT32 AmbaSD_Config(UINT32 SdChanNo, const AMBA_SD_CONFIG_s *pSdConfig);
UINT32 AmbaSD_SetSdSetting(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting);
UINT32 AmbaSD_GetSdSetting(UINT32 SdChanNo, AMBA_SD_SETTING_s * pSdSetting);

/*
 * Defined in AmbaSD_Card.c
 */
UINT32 AmbaSD_GetCardStatus(UINT32 SdChanNo, AMBA_SD_CARD_STATUS_s *pCardStatus);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include "AmbaSD_STD.h"
typedef struct _SD_RawRegs_s_ {
    UINT32 CidRegs[4];
    UINT32 CsdRegs[4];
    UINT32 ScrRegs[2];
} SD_RawRegs_s;
const AMBA_SD_INFO_REG_s *AmbaSD_GetCardRegs(UINT32 SdChanNo);
UINT32 AmbaSD_GetRawRegs(UINT32 SdChanNo, SD_RawRegs_s *Regs);
#endif

/*
 * Defined in AmbaSDIO.c
 */
UINT32 AmbaSDIO_Reset(UINT32 SdChanNo);
UINT32 AmbaSDIO_CardIrqEnable(UINT32 SdChanNo, UINT32 Enable);

UINT32 AmbaSDIO_ReadByte(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, UINT8 *pRxDataBuf);
UINT32 AmbaSDIO_WriteByte(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, UINT8 *pTxDataBuf);
UINT32 AmbaSDIO_ReadAfterWrite(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, const UINT8 *pTxDataBuf);

UINT32 AmbaSDIO_SetBlkSize(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 BlkSize);

UINT32 AmbaSDIO_Read(UINT32 SdChanNo, const AMBA_SDIO_RW_CONFIG_s *pConfig);
UINT32 AmbaSDIO_Write(UINT32 SdChanNo, const AMBA_SDIO_RW_CONFIG_s *pConfig);

#endif /* AMBA_SD_H */
