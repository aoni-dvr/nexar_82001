/**
 *  @file AmbaSD_DelayCtrl.c
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
 *  @details SD Card Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"

//#include "AmbaRTSL_GPIO.h"

#include "AmbaSD.h"
#include "AmbaSD_Ctrl.h"

//#include "AmbaRTSL_SD.h"

/**
 *  AmbaSD_SetDriveStrength - Set SD driving strength
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] pSdConfig pointer to the SD controller configurations
 *  @return error code
 */
UINT32 AmbaSD_SetDriveStrength(UINT32 SdChanNo, const AMBA_SD_SETTING_s * pSdSetting)
{
    UINT32 RetVal = 0;
    (void) pSdSetting;
    //UINT32 DataDriveStrength;

    switch (SdChanNo) {
    case AMBA_SD_CHANNEL0:      /* for SD0 */
        /* Drive Strength for Clock */
        //(void) AmbaRTSL_GpioSetDriveStrength(GPIO_PIN_061_SD0_CLK, pSdSetting->ClockDrive);

        /* Drive Strength for Data */
        //DataDriveStrength = pSdSetting->DataDrive;
        /* Drive Strength for CMD */

        /* Drive Strength for CD */

        /* Drive Strength for WP */
        break;

    case AMBA_SD_CHANNEL1:      /* for SD1 */
        /* Drive Strength for Clock */

        /* Drive Strength for Data */
        //DataDriveStrength = pSdSetting->DataDrive;

        /* Drive Strength for CMD */

        /* Drive Strength for CD */

        /* Drive Strength for WP */
        break;

    default:
        RetVal = 1;
        break;
    }

    return RetVal;
}

/**
 *  AmbaSD_DelayCtrlSet -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] ClkBypass
 *  @param[in] RxClkPol
 *  @param[in] SbcCore
 *  @param[in] SelValue
 *  @param[in] DinClkPol
 *  @param[in] CmdBypass
 */
void AmbaSD_DelayCtrlSet(UINT32 SdChanNo, UINT32 ClkBypass, UINT32 RxClkPol, UINT32 SbcCore, UINT32 SelValue, UINT32 DinClkPol, UINT32 CmdBypass)
{
    (void) SdChanNo;
    (void) ClkBypass;
    (void) RxClkPol;
    (void) SbcCore;
    (void) SelValue;
    (void) DinClkPol;
    (void) CmdBypass;
    AmbaMisra_TouchUnused(&DinClkPol);
    AmbaSD_DelayCtrlReset(SdChanNo);
    (void) AmbaKAL_TaskSleep(1U); //DLL reset time
    (void) AmbaKAL_TaskSleep(1U); //DLL lock time
}


/**
 *  AmbaSD_DelayCtrlGet -
 *  @param[in] SdChanNo SD Channel Number
 *  @return delay value
 */
UINT32 AmbaSD_DelayCtrlGet(UINT32 SdChanNo)
{
    //AMBA_SD_DETAIL_DELAY_s DetailDelay = {0};
    UINT32 DetailDelayVal = 1;
    (void) SdChanNo;


    return DetailDelayVal;
}

/**
 *  AmbaSD_DelayCtrlReset -
 *  @param[in] SdChanNo SD Channel Number
 */
void AmbaSD_DelayCtrlReset(UINT32 SdChanNo)
{
    (void) SdChanNo;
}

