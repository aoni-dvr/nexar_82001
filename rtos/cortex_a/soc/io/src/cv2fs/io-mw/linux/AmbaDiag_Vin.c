/**
 *  @file AmbaDiag_Vin.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Diagnostic functions for video inputs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaIOUtility.h"

#include "AmbaKAL.h"
#include "AmbaDef.h"

#include "AmbaVIN.h"
//#include "AmbaCSL_VIN.h"
//#include "AmbaCSL_DebugPort.h"
//#include "AmbaRTSL_PLL.h"

#if defined(CONFIG_VIN_ASIL)
#include "AmbaSafety_VIN.h"
#endif


/**
 *  AmbaDiag_VinShowInfo - Show Vin 0/1 info
 *  @param[in] VinID Vin channel
 *  @return error code
 */
UINT32 AmbaDiag_VinShowInfo(UINT32 VinID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = VIN_ERR_NONE;
    (void) VinID;
    (void) LogFunc;

    return RetVal;
}

/**
 *  AmbaDiag_VinResetStatus - Reset status register
 *  @param[in] VinID Vin channel
 *  @return error code
 */
UINT32 AmbaDiag_VinResetStatus(UINT32 VinID, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = VIN_ERR_NONE;
    (void) VinID;
    if (LogFunc != NULL) {
        LogFunc("clear Status,\n SlvsStatus,\n MipiStatus0,\n MipiLongPacketCount,\n MipiShortPacketCount,\n MipiNumCrcErrPacket\n, MipiNumCorrEccErrPacket\n");
    }

    return RetVal;
}

/**
 *  AmbaDiag_VinConfigThrDbgPort - Configure Vin Main through debug port
 *  @param[in] VinID Vin channel
 *  @param[in] CaptureWidth Vin capture window width
 *  @param[in] CaptureHeight Vin Capture window height
 *  @return error code
 */
UINT32 AmbaDiag_VinConfigThrDbgPort(UINT32 VinID, UINT32 CaptureWidth, UINT32 CaptureHeight, AMBA_SYS_LOG_f LogFunc)
{
    UINT32 RetVal = VIN_ERR_NONE;
    (void) VinID;
    (void) CaptureWidth;
    (void) CaptureHeight;
    LogFunc("AmbaDiag_VinConfigThrDbgPort");

    return RetVal;
}

/**
 *  AmbaDiag_VinGetStatus - Configure Vin Main through debug port
 *  @param[in] VinID Vin channel
 *  @param[in] IODIAG_VIN_STATUS_s
 *  @return error code
 */
UINT32 AmbaDiag_VinGetStatus(UINT32 VinID, IODIAG_VIN_STATUS_s *pVinStatus)
{
    UINT32 RetVal = VIN_ERR_NONE;
    (void) VinID;
    (void) pVinStatus;
    return RetVal;
}

