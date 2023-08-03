/**
 *  @file AmbaDrvEntry.h
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

#ifndef AMBA_DRV_ENTRY_H
#define AMBA_DRV_ENTRY_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

typedef UINT32 (*AMBA_DRV_ENTRY_f)(void);

UINT32 AmbaADC_DrvEntry(void);
UINT32 AmbaCVBS_DrvEntry(void);
UINT32 AmbaDMA_DrvEntry(void);
UINT32 AmbaDMIC_DrvEntry(void);
UINT32 AmbaGDMA_DrvEntry(void);
UINT32 AmbaGPIO_DrvEntry(void);
UINT32 AmbaHDMI_DrvEntry(void);
UINT32 AmbaI2C_DrvEntry(void);
UINT32 AmbaI2S_DrvEntry(void);
UINT32 AmbaINT_DrvEntry(void);
UINT32 AmbaIRIF_DrvEntry(void);
UINT32 AmbaRTC_DrvEntry(void);
UINT32 AmbaSD_DrvEntry(void);
UINT32 AmbaSPI_DrvEntry(void);
UINT32 AmbaSYS_DrvEntry(void);
UINT32 AmbaTMR_DrvEntry(void);
UINT32 AmbaUART_DrvEntry(void);
UINT32 AmbaPIO_DrvEntry(void);
UINT32 AmbaPWM_DrvEntry(void);
UINT32 AmbaVout_DrvEntry(void);
UINT32 AmbaWDT_DrvEntry(void);
UINT32 AmbaVIN_DrvEntry(void);
UINT32 AmbaDRAMC_DrvEntry(void);
#ifdef CONFIG_BUILD_SSP_USB_LIBRARY
UINT32 AmbaUSB_DrvEntry(void) __attribute__((weak));
#endif
/*
 * Defined in AmbaDrvEntry.c
 */
void AmbaDrvEntry(void);

#endif /* AMBA_DRV_ENTRY_H */
