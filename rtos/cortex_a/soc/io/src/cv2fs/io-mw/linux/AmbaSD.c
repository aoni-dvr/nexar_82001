/**
 *  @file AmbaSD.c
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
 *  @details SD Controller APIs
 *
 */

#include "AmbaDef.h"
#include "AmbaWrap.h"

#include "AmbaDrvEntry.h"
#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

//#include "AmbaRTSL_SD.h"

#include "AmbaCache.h"

#include "AmbaMisraFix.h"
//#include "AmbaPrint.h"


#define SdCmdRetry 0x3

#define SD_CMD_TIMEOUT (1000U * AMBA_KAL_SYS_TICK_MS)
#define SD_DATA_TIMEOUT (5000U * AMBA_KAL_SYS_TICK_MS)

#define CLK_SWITCH_DELAY_MS         10U
#define CMDLINE_SWITCH_DELAY_MS     5U

typedef struct {
    int i;
} AMBA_SD_EVENT_CTRL_s;



UINT32 AmbaSD_WaitIrqStatus(UINT32 SdChanNo, UINT32 SdEventFlag, UINT32 AnyOrAll, UINT32 AutoClear, UINT32 Timeout);
UINT32 AmbaSD_ResetAllEventFlags(void);
extern UINT32 AmbaFS_PrFile2SdMount(UINT32 SdChanNo);
void GNU_WEAK_SYMBOL AmbaUserSD_DelayCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID);

/*
 * Default implementations of functions (can be replaced by strong ones at link-time)
 */
void GNU_WEAK_SYMBOL AmbaUserSD_DelayCtrl(UINT32 SdChanNo, AMBA_SD_INFO_CID_INFO_s *pCardID)
{
    AmbaMisra_TouchUnused(pCardID);
    AmbaMisra_TouchUnused(&SdChanNo);
}

#if 0
static INT32 UserSDDelayCtrlEnable = 1;

/**
 *  AmbaSD_SDDelayCtrlEnable - Enable/Disable  AmbaSD_SDDelayCtrl
 *  @param[in] Enable: 1-Enable 0-Disable
 */
void AmbaSD_SDDelayCtrlEnable(INT32 Enable)
{
    UserSDDelayCtrlEnable = Enable;
}
#endif

/**
 *  AmbaSD_Init - SD device driver initializations
 *  @return error code
 */
UINT32 AmbaSD_DrvEntry(void)
{
    UINT32 RetVal = OK;
    return RetVal;
}

/**
 *  AmbaSD_SetSdSetting - Set SD Card Singal setting
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdSetting Pointer to the sd card signal setting
 *  @return error code
 */
UINT32 AmbaSD_SetSdSetting(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting)
{
    (void) SdChanNo;
    (void) pSdSetting;
    return OK;
}

/**
 *  AmbaSD_GetCardConfig - Get SD Card Singal Delay Register
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pSdSetting Pointer to the sd card signal setting
 *  @return error code
 */
UINT32 AmbaSD_GetSdSetting(UINT32 SdChanNo, AMBA_SD_SETTING_s * pSdSetting)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) pSdSetting;
    return RetVal;
}

/**
 *  AmbaSD_SetHsRdLatency - Set SD Card high speed rdlatency
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] HsRdLatency high speed rdlatency
 *  @return error code
 */
void AmbaSD_SetHsRdLatency(UINT32 SdChanNo, UINT8 HsRdLatency)
{
    (void) SdChanNo;
    (void) HsRdLatency;
    //AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency = HsRdLatency;
}

/**
 *  AmbaSD_GetHsRdLatency - Set SD Card high speed rdlatency
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT8 AmbaSD_GetHsRdLatency(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 0;//AmbaSD_Ctrl[SdChanNo].SdConfig.SdSetting.HsRdLatency;
}

/**
 *  AmbaSD_Config - SD device driver configurations
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdConfig pointer to the SD controller configurations
 *  @return error code
 */
UINT32 AmbaSD_Config(UINT32 SdChanNo, const AMBA_SD_CONFIG_s *pSdConfig)
{
    (void) SdChanNo;
    (void) pSdConfig;
    return 1;
}

/**
 *  AmbaSD_SetEventFlag - Set SD event flags
 *  @param[in] SdEventFlag SD event flags
 *  @return error code
 */
UINT32 AmbaSD_SetEventFlag(UINT32 SdEventFlag)
{
    (void) SdEventFlag;
    return 1;//AmbaKAL_EventFlagSet(&_SD_EventFlag, SdEventFlag);
}

/**
 *  AmbaSD_ClearEventFlag - Clear SD event flags
 *  @param[in] SdEventFlag SD event flags
 *  @return error code
 */
UINT32 AmbaSD_ClearEventFlag(UINT32 SdEventFlag)
{
    (void) SdEventFlag;
    return 1;//AmbaKAL_EventFlagClear(&_SD_EventFlag, SdEventFlag);
}

/**
 *  AmbaSD_WaitEventFlag - Wait for SD event flags
 *  @param[in] SdEventFlag SD event flags
 *  @param[in] AnyOrAll
 *  @param[in] AutoClear
 *  @return Current SD event flags
 */
UINT32 AmbaSD_WaitEventFlag(UINT32 SdEventFlag, UINT32 AnyOrAll, UINT32 AutoClear, UINT32 Timeout)
{
    (void) SdEventFlag;
    (void) AnyOrAll;
    (void) AutoClear;
    (void) Timeout;
    return 1;
}

/**
 *  AmbaSD_ClearCtrlFlag - Clear SD Ctrl flags
 *  @param[in] SdChanNo
 *  @return error code
 */
UINT32 AmbaSD_ClearCtrlFlag(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  SdCheckIrqStatus - Check SD status after command done
 *  @param[in] Status
 *  @param[in] Flag SD event flags
 *  @return error code
 */

/**
 *  AmbaSD_ResetAllEventFlags - Reset All SD event flags
 *  @return error code
 */
UINT32 AmbaSD_ResetAllEventFlags(void)
{
    return 1;//AmbaKAL_EventFlagClear(&_SD_EventFlag, 0xffffffffU);
}

/**
 *  AmbaSD_Lock - Lock SD mutex and enable SD clock
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_Lock(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_Unlock - Unlock SD mutex and enable SD clock
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_Unlock(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return OK;
}

/**
 *  AmbaSD_IsCardPresent - Check if SD card is present or not in the slot
 *  @param[in] SdChanNo SD Channel Number
 *  @return 0 - not Present; no-0 - Present
 */
UINT32 AmbaSD_IsCardPresent(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_IsWriteProtect - Check if SD card is write protect or not
 *  @param[in] SdChanNo SD Channel Number
 *  @return 1 - write protect; 0 - no
 */
UINT32 AmbaSD_IsWriteProtect(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}


/**
 *  AmbaSD_UhsSupportChk - Check status to support UHS mode
 *  @param[in] SdChanNo SD Channel Number
 *  @return Current SD event flags
 */
UINT32 AmbaSD_UhsSupportChk(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_GetSpeedMode -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pArgCmd6
 *  @param[in] pCmd6Status
 */

/**
 *  AmbaSD_GetCurrentLimit -
 *  @param[out] pArgCmd6
 *  @param[in] pCmd6Status
 */

/**
 *  AmbaSD_GetUhsPram -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pArgCmd6
 *  @param[in] pCmd6Status
 */

/**
 *  AmbaSD_SetupUhsMode - Setup UHS mode
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */

/**
 *  SdGeteMMCHS200Pram -
 *  @param[out] pArgCmd6
 *  @param[in] pExtCsd
 *  @return error code
 */

/**
 *  AmbaSD_SetupHS200Mode - Setup HS200 mode
 *  @param[in] SdChanNo SD Channel Number
 *  @return Current SD event flags
 */
/**
 *  AmbaSD_WaitIrqStatus - Wait for SD interrupt status
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] SdEventFlag SD event flags
 *  @param[in] AnyOrAll
 *  @param[in] AutoClear
 *  @param[in] Timeout Wait option (timer-ticks)
 *  @return Current SD event flags
 */
UINT32 AmbaSD_WaitIrqStatus(UINT32 SdChanNo, UINT32 SdEventFlag, UINT32 AnyOrAll, UINT32 AutoClear, UINT32 Timeout)
{
    (void) SdChanNo;
    (void) SdEventFlag;
    (void) AnyOrAll;
    (void) AutoClear;
    (void) Timeout;
    return 1;
}

/**
 *  Mmc4DetermineBusWidth - Determine MMC 4.0 bus width
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */

/**
 *  AmbaSD_SdCardInit - Setup SD Card
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */

/**
 *  AmbaSD_MemCardInit - SD Memory Card Initialization
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
void AmbaSD_MemCardInit(UINT32 SdChanNo)
{
    (void) SdChanNo;
}

/**
 *  AmbaSD_SetupSdioCard -
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */


/**
 *  AmbaSD_IoCardInit - Setup MMC Card
 *  @param[in] SdChanNo SD Channel Number
 */
void AmbaSD_IoCardInit(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return;

}

/**
 *  AmbaSD_SendCMD0 - Send CMD0 ((bc): Resets all cards to idle state)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD0(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_SendCMD1 - Send CMD1 ((bcr): Asks the card, in idlestate, to send its Operat-ing Conditions Register contents in the response on the CMD line
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pCmd1
 *  @param[out] pOcr
 *  @return error code
 */
UINT32 AmbaSD_SendCMD1(UINT32 SdChanNo, const AMBA_SD_CMD1_ARGUMENT_s * pCmd1, UINT32 * pOcr)
{
    (void) SdChanNo;
    (void) pCmd1;
    (void) pOcr;
    return 1;
}

/**
 *  AmbaSD_SendCMD2 - Send CMD2 ((bcr): Asks any card to send the CID numbers on the CMD line)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pCid
 *  @return error code
 */
UINT32 AmbaSD_SendCMD2(UINT32 SdChanNo, AMBA_SD_INFO_CID_REG_s * pCid)
{
    (void) SdChanNo;
    (void) pCid;
    return 1;
}

/**
 *  AmbaSD_SendCMD3 - Send CMD3 ((bcr): Asks the card to publish a new relative address (RCA)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pRCA
 *  @return error code
 */

UINT32 AmbaSD_SendCMD3(UINT32 SdChanNo, UINT16 * pRCA)
{
    (void) SdChanNo;
    (void) pRCA;
    return 1;
}

/**
 *  AmbaSD_SendCMD5 - Send CMD5 ((SDIO): The function of CMD5 for SDIO cards is similar to the operation of ACMD41 for SD memory card)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdioCmd5Arg
 *  @param[out] pOcr
 *  @return error code
 */
UINT32 AmbaSD_SendCMD5(UINT32 SdChanNo, const AMBA_SDIO_CMD5_ARGUMENT_s *pSdioCmd5Arg, UINT32 *pOcr)
{
    UINT32 RetVal = 0U;
    (void) SdChanNo;
    (void) pSdioCmd5Arg;
    (void) pOcr;

    return RetVal;
}

/**
 *  _SWAPWORD -
 *  @param[in] data
 *  @return
 */

/**
 *  AmbaSD_SendCMD6 - Send CMD6 ((adtc): Checks switchable function (mode 0) and switch card function (mode 1))
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pArgCmd6
 *  @param[out] pCmd6Status
 *  return error code
 */
UINT32 AmbaSD_SendCMD6(UINT32 SdChanNo, const AMBA_SD_CMD6_ARGUMENT_s * pArgCmd6, AMBA_SD_CMD6_STATUS_s * pCmd6Status)
{
    (void) SdChanNo;
    (void) pArgCmd6;
    (void) pCmd6Status;
    return 1;
}

/**
 *  AmbaSD_SendEmmcCMD6 - Switches the mode of operation of the selected card or modifies the EXT_CSD registers (CMD6).
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pArgCmd6
 *  @return error code
 */
UINT32 AmbaSD_SendEmmcCMD6(UINT32 SdChanNo, const AMBA_SD_EMMC_CMD6_ARGUMENT_s *pArgCmd6)
{
    (void) SdChanNo;
    (void) pArgCmd6;
    return 1;
}

/**
 *  AmbaSD_SendCMD7 - Send CMD7 ((ac): Toggles a card between the stand-by and transfer states or programming and disconnect states)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] RCA
 *  @return error code
 */
UINT32 AmbaSD_SendCMD7(UINT32 SdChanNo, UINT16 RCA)
{
    (void) SdChanNo;
    (void) RCA;
    return 1;
}

/**
 *  AmbaSD_SendCMD8 - Send CMD8 ((bcr): Sends SD Memory Card interface condition)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD8(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_SendMmcCMD8 - Send CMD6 ((adtc): Checks switchable function (mode 0) and switch card function (mode 1))
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pExtCsd
 *  @return error code
 */
UINT32 AmbaSD_SendEmmcCMD8(UINT32 SdChanNo, AMBA_SD_INFO_EXT_CSD_REG_s *pExtCsd)
{
    (void) SdChanNo;
    (void) pExtCsd;
    return 1;
}

/**
 *  AmbaSD_SendCMD9 - Send CMD9 ((ac): Addressed card sends its card-specific data (CSD) on the CMD line)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pCsd
 *  @return error code
 */
UINT32 AmbaSD_SendCMD9(UINT32 SdChanNo, AMBA_SD_INFO_CSD_REG_s *pCsd)
{
    (void) SdChanNo;
    (void) pCsd;
    return 1;
}

/**
 *  AmbaSD_SendCMD11 - Send CMD11 ((acr): Switch to 1.8V bus signal level)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD11(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_SendCMD12 - Send CMD12 ((ac): Forces the card to stop transmission)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD12(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_SendCMD16 - Send CMD16 ((ac): Set block length)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] BlockLength
 *  @return error code
 */
UINT32 AmbaSD_SendCMD16(UINT32 SdChanNo, UINT32 BlockLength)
{
    (void) SdChanNo;
    (void) BlockLength;
    return 1;
}

/**
 *  AmbaSD_SendCMD17 - Send CMD17 ((adtc): Read one block)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Sector start sector
 *  @param[in] pBuf pointer to the data buffer
 *  @return error code
 */
UINT32 AmbaSD_SendCMD17(UINT32 SdChanNo, UINT32 Sector, UINT8 * pBuf)
{
    (void) SdChanNo;
    (void) Sector;
    (void) pBuf;
    return 1;
}

/**
 *  AmbaSD_SendCMD18 - Send CMD18 ((adtc): Read multiple blocks)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Sector start sector
 *  @param[in] Sectors
 *  @param[in] pBuf pointer to the data buffer
 *  @return error code
 */
UINT32 AmbaSD_SendCMD18(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 * pBuf)
{
    (void) SdChanNo;
    (void) Sector;
    (void) Sectors;
    (void) pBuf;
    return 1;
}

/**
 *  AmbaSD_SendCMD19 - Send CMD19 ((adtc): 64 bytes tuning pattern is sent for SDR50 & SDR104)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD19(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_SendEmmcCMD21 - Send CMD21 ((adtc): 64 bytes or 128Bytes tuning pattern is sent for HS200)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendEmmcCMD21(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_SendCMD24 - Send CMD24 ((adtc): Write one block)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Sector
 *  @param[in] pBuf
 *  @return error code
 */
UINT32 AmbaSD_SendCMD24(UINT32 SdChanNo, UINT32 Sector, UINT8 * pBuf)
{
    (void) SdChanNo;
    (void) Sector;
    (void) pBuf;
    return 1;
}

/**
 *  AmbaSD_SendCMD25 - Send CMD25 ((adtc): Write multiple blocks
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Sector start sector
 *  @param[in] Sectors
 *  @param[in] pBuf pointer to the data buffer
 *  @return error code
 */
UINT32 AmbaSD_SendCMD25(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors, UINT8 * pBuf)
{
    (void) SdChanNo;
    (void) Sector;
    (void) Sectors;
    (void) pBuf;
    return 1;
}

/**
 *  AmbaSD_SendCMD32 - Send CMD32 (ac): Sets the address of the first write block to be erased
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] StartSector
 *  @return error code
 */
UINT32 AmbaSD_SendCMD32(UINT32 SdChanNo, UINT32 StartSector)
{
    (void) SdChanNo;
    (void) StartSector;
    return 1;
}

/**
 *  AmbaSD_SendCMD33 - Send CMD33 (ac): Sets the address of the last write block of the continuous range to be erased
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] EndSector
 *  @return error code
 */
UINT32 AmbaSD_SendCMD33(UINT32 SdChanNo, UINT32 EndSector)
{
    (void) SdChanNo;
    (void) EndSector;
    return 1;
}

/**
 *  AmbaSD_SendCMD38 - Send CMD38 (ac): (ac): Erases all previously selected write blocks
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD38(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_CheckR5Status -
 *  @param[in] R5CardStatus
 *  @return error code
 */

/**
 *  AmbaSD_SendCMD52 - Send CMD52 ((SDIO): I/O Reset, I/O about, or I/O Block Length, Bus_Width)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pArgCmd52
 *  @param[out] pDataBuf
 *  @return error code
 */
UINT32 AmbaSD_SendCMD52(UINT32 SdChanNo, const AMBA_SDIO_CMD52_ARGUMENT_s * pArgCmd52, UINT8 * pDataBuf)
{
    (void) SdChanNo;
    (void) pArgCmd52;
    (void) pDataBuf;
    return 1;
}

/**
 *  AmbaSD_SendCMD53 - Send CMD53 ((SDIO): I/O block operations)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pArgCmd53
 *  @param[in] BlockSize
 *  @param[in] pDataBuf
 *  @return error code
 */
UINT32 AmbaSD_SendCMD53(UINT32 SdChanNo, const AMBA_SDIO_CMD53_ARGUMENT_s * pArgCmd53, UINT16 BlockSize, UINT8 * pDataBuf)
{
    (void) SdChanNo;
    (void) pArgCmd53;
    (void) BlockSize;
    (void) pDataBuf;
    return 1;
}

/**
 *  AmbaSD_SendCMD55 - Send CMD55 ((ac): Indicates to the card that the next CMD is an application specific CMD)
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSD_SendCMD55(UINT32 SdChanNo)
{
    (void) SdChanNo;
    return 1;
}

/**
 *  AmbaSD_SendACMD6 - Send ACMD6 ((ac): Defines the data bus width ('00'=1bit or '10'=4 bits bus) to be used for data transfer)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] BusWidth
 *  @return error code
 */
UINT32 AmbaSD_SendACMD6(UINT32 SdChanNo, UINT8 BusWidth)
{
    (void) SdChanNo;
    (void) BusWidth;
    return 1;
}

/**
 *  AmbaSD_SendACMD41 - Send ACMD41 ((bcr): Sends host capacity support info (HCS) and asks the card to send OCR)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pAcmd41
 *  @param[out] pOcr
 *  @return error code
 */
UINT32 AmbaSD_SendACMD41(UINT32 SdChanNo, const AMBA_SD_ACMD41_ARGUMENT_s * pAcmd41, UINT32 * pOcr)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) pAcmd41;
    (void) pOcr;
    return RetVal;
}

/**
 *  AmbaSD_SendACMD42 - Send ACMD42 ((ac):Connect[1]/Disconnect[0] the 50KOhm pull-up resistor on CD/DAT3 (pin 1) of the card)
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] SetCardDetect
 *  @return error code
 */
UINT32 AmbaSD_SendACMD42(UINT32 SdChanNo, UINT8 SetCardDetect)
{
    (void) SdChanNo;
    (void) SetCardDetect;
    return 1;
}

/**
 *  AmbaSD_SendACMD51 - Send ACMD51 ((adtc): Reads the SD Configuration Register (SCR))
 *  @param[in] SdChanNo SD Channel Number
 *  @param[out] pScr
 *  @return error code
 */
UINT32 AmbaSD_SendACMD51(UINT32 SdChanNo, AMBA_SD_INFO_SCR_REG_s * pScr)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) pScr;
    return RetVal;
}

/**
 *  AmbaSD_GetResponse -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] OpCode
 *  @param[out] pResp
 *  @return error code
 */
INT32 AmbaSD_GetResponse(UINT32 SdChanNo, UINT32 OpCode, UINT32 * pResp)
{
    (void) SdChanNo;
    (void) OpCode;
    (void) pResp;
    return 0;//AmbaRTSL_SdGetResponse(SdChanNo, OpCode, pResp);
}

//-----------------------------------------------------------------------------------------------------
/**
 *  HandleReadMisalign -
 *  @param[in] Misalign
 *  @param[out] pReadBuf
 *  @param[in] pTmpBuf
 *  @param[in] Sectors
 *  @param[in] BlkSize
 */

/**
 *  AmbaSD_ReadSector -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSecConfig
 *  @return error code
 */
UINT32 AmbaSD_ReadSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) pSecConfig;
    return RetVal;
}

/**
 *  AmbaSD_GetTotalSectors -
 *  @param[in] SdChanNo SD Channel Number
 *  @return total sectors
 */
UINT32 AmbaSD_GetTotalSectors(UINT32 SdChanNo)
{
    UINT32 TotalSectors = 1;
    (void) SdChanNo;
    return TotalSectors;
}

/**
 *  AmbaSD_EraseSector -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Sector
 *  @param[in] Sectors
 *  @return error code
 */
UINT32 AmbaSD_EraseSector(UINT32 SdChanNo, UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) Sector;
    (void) Sectors;
    return RetVal;
}

/**
 *  HandleWriteMisalign -
 *  @param[in] Misalign
 *  @param[in] pWriteBuf
 *  @param[in] pTmpBuf
 *  @param[in] Sectors
 *  @param[in] BlkSize
 */

/**
 *  AmbaSD_WriteSector -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSecConfig
 *  @return error code
 */
UINT32 AmbaSD_WriteSector(UINT32 SdChanNo, const AMBA_NVM_SECTOR_CONFIG_s * pSecConfig)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) pSecConfig;
    return RetVal;
}

/**
 *  AmbaSD0_EraseSector -
 *  @param[in] Sector
 *  @param[in] Sectors
 *  @return error code
 */
INT32 AmbaSD0_EraseSector(UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = 1;
    (void) Sector;
    (void) Sectors;
    return RetVal;
}

/**
 *  AmbaSD0_ReadSector -
 *  @param[out] pBuf
 *  @param[in] Sector
 *  @param[in] Sectors
 *  @return error code
 */
INT32 AmbaSD0_ReadSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = 1;
    (void) pBuf;
    (void) Sector;
    (void) Sectors;
    return RetVal;
}

/**
 *  AmbaSD0_WriteSector -
 *  @param[in] pBuf
 *  @param[in] Sector
 *  @param[in] Sectors
 *  @return error code
 */
INT32 AmbaSD0_WriteSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = 1;
    (void) pBuf;
    (void) Sector;
    (void) Sectors;
    return RetVal;
}

/**
 *  AmbaSD1_EraseSector -
 *  @param[in] Sector
 *  @param[in] Sectors
 *  @return error code
 */
INT32 AmbaSD1_EraseSector(UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = 1;
    (void) Sector;
    (void) Sectors;
    return RetVal;
}

/**
 *  AmbaSD1_ReadSector -
 *  @param[out] pBuf
 *  @param[in] Sector
 *  @param[in] Sectors
 *  @return error code
 */
INT32 AmbaSD1_ReadSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = 1;
    (void) pBuf;
    (void) Sector;
    (void) Sectors;
    return RetVal;
}

/**
 *  AmbaSD1_WriteSector -
 *  @param[in] pBuf
 *  @param[in] Sector
 *  @param[in] Sectors
 *  @return error code
 */
INT32 AmbaSD1_WriteSector(UINT8 * pBuf, UINT32 Sector, UINT32 Sectors)
{
    UINT32 RetVal = 1;
    (void) pBuf;
    (void) Sector;
    (void) Sectors;
    return RetVal;
}

/**
 *  AmbaSDIO_Reset -
 *  @param[in] SdChanNo SD Channel Number
 *  @return error code
 */
UINT32 AmbaSDIO_Reset(UINT32 SdChanNo)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    return RetVal;
}

/**
 *  AmbaSDIO_CardIrqEnable -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Enable
 *  @return error code
 */
UINT32 AmbaSDIO_CardIrqEnable(UINT32 SdChanNo, UINT32 Enable)
{
    (void) SdChanNo;
    (void) Enable;
    return OK;
}

/**
 *  AmbaSDIO_ReadByte -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] RegAddr
 *  @param[out] pRxDataBuf
 *  @return error code
 */
UINT32 AmbaSDIO_ReadByte(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, UINT8 * pRxDataBuf)
{
    UINT32   RetVal = 1;
    (void) SdChanNo;
    (void) FunctionNo;
    (void) RegAddr;
    (void) pRxDataBuf;
    return RetVal;

}

/**
 *  AmbaSDIO_WriteByte -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] RegAddr
 *  @param[in] pTxDataBuf
 *  @return error code
 */
UINT32 AmbaSDIO_WriteByte(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, UINT8 * pTxDataBuf)
{
    UINT32   RetVal = 1;
    (void) SdChanNo;
    (void) FunctionNo;
    (void) RegAddr;
    (void) pTxDataBuf;
    return RetVal;
}

/**
 *  AmbaSDIO_ReadAfterWrite -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] RegAddr
 *  @param[in] pTxDataBuf
 *  @return error code
 */
UINT32 AmbaSDIO_ReadAfterWrite(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 RegAddr, const UINT8 * pTxDataBuf)
{
    UINT32   RetVal = 1;
    (void) SdChanNo;
    (void) FunctionNo;
    (void) RegAddr;
    (void) pTxDataBuf;

    return RetVal;
}


/**
 *  AmbaSDIO_SetBlkSize -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] FunctionNo
 *  @param[in] BlkSize
 *  @return error code
 */
UINT32 AmbaSDIO_SetBlkSize(UINT32 SdChanNo, UINT8 FunctionNo, UINT32 BlkSize)
{

    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) FunctionNo;
    (void) BlkSize;

    return RetVal;
}

/**
 *  AmbaSDIO_Read -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pConfig
 *  @return error code
 */
UINT32 AmbaSDIO_Read(UINT32 SdChanNo, const AMBA_SDIO_RW_CONFIG_s * pConfig)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) pConfig;

    return RetVal;
}

/**
 *  AAmbaSDIO_Write -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pConfig
 *  @return error code
 */
UINT32 AmbaSDIO_Write(UINT32 SdChanNo, const AMBA_SDIO_RW_CONFIG_s * pConfig)
{
    UINT32 RetVal = 1;
    (void) SdChanNo;
    (void) pConfig;

    return RetVal;
}

