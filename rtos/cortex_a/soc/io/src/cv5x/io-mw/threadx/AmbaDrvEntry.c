/**
 *  @file AmbaDrvEntry.c
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
 *  @details Ambarella Driver Entry Functions
 *
 */

#include "AmbaDef.h"
#include "AmbaDrvEntry.h"

/**
 *  AmbaDrvEntry - Initializing certain objects the driver uses, and setting up various per-driver system resources.
 */
void AmbaDrvEntry(void)
{
    static const AMBA_DRV_ENTRY_f DrvEntryList[] = {
        AmbaSYS_DrvEntry,
        AmbaINT_DrvEntry,
        AmbaDMA_DrvEntry,
        AmbaGDMA_DrvEntry,
        AmbaGPIO_DrvEntry,
        AmbaI2C_DrvEntry,
        AmbaI2S_DrvEntry,
        AmbaIRIF_DrvEntry,
        AmbaPWM_DrvEntry,
        AmbaRTC_DrvEntry,
        AmbaSD_DrvEntry,
        AmbaSPI_DrvEntry,
        AmbaTMR_DrvEntry,
        AmbaUART_DrvEntry,
        AmbaADC_DrvEntry,
        AmbaVIN_DrvEntry,
        AmbaVout_DrvEntry,
        AmbaHDMI_DrvEntry,
        AmbaCVBS_DrvEntry,
        AmbaWDT_DrvEntry,
        AmbaDMIC_DrvEntry,
        AmbaDRAMC_DrvEntry,
        AmbaTempSensor_DrvEntry,
#ifdef CONFIG_BUILD_SSP_USB_LIBRARY
        AmbaUSB_DrvEntry,
#endif
#ifdef CONFIG_BUILD_SSP_PCIE_LIB
        AmbaPCIE_DrvEntry,
#endif
    };

    SIZE_t i, entries;
    UINT32 RetVal = OK;

    entries = sizeof(DrvEntryList) / sizeof(DrvEntryList[0]);

    for (i = 0U; i < entries; i ++) {
        if (DrvEntryList[i] != NULL) {
            RetVal = DrvEntryList[i]();
            if (RetVal != OK) {
                AmbaAssert();
            }
        }
    }
}
