/**
 *  @file AmbaDDRC.h
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details Definitions & Constants for DDRC Middleware APIs
 *
 */

#ifndef AMBA_DDRC_H
#define AMBA_DDRC_H

#ifndef AMBA_DDRC_DEF_H
#include "AmbaDDRC_Def.h"
#endif

/* Use 'd' as magic number */
#define DDRC_IOC_MAGIC 'd'

#define DDRC_IOC_RESET          _IO(DDRC_IOC_MAGIC, 0)          // Reset records in this Linux devdrv
#define DDRC_IOC_CHG_REG        _IOW(DDRC_IOC_MAGIC, 1, int)    // Change target DDRC reg offset for R/W
#define DDRC_IOC_CHG_VAL        _IOW(DDRC_IOC_MAGIC, 2, int)    // Change value to be written to DDRC
#define DDRC_IOC_W_DDRC         _IO(DDRC_IOC_MAGIC, 3)          // Write to DDRC from Linux devdrv
#define DDRC_IOC_R_DDRC         _IOR(DDRC_IOC_MAGIC, 4, int)    // Read from DDRC to Linux devdrv
#define DDRC_IOC_R_REG_VALUE    _IOR(DDRC_IOC_MAGIC, 5, int)    // Read raw value of reg_value
#define DDRC_IOC_R_REG_OFFSET   _IOR(DDRC_IOC_MAGIC, 6, int)    // Read raw value of reg_offset

/*
 * Defined in AmbaDDRC.c
 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

UINT32 AmbaDRAMC_DrvEntry(void);
UINT32 AmbaDDRC_DrvEntry(void);

UINT32 AmbaDDRC_GetDramTypeFromVal(UINT32 Config0);
UINT32 AmbaDDRC_GetDramSizeFromVal(UINT32 Config0);
UINT32 AmbaDDRC_GetDuelDieEnFromVal(UINT32 Config0);
UINT32 AmbaDDRC_GetDramType(UINT32 DdrcId);
UINT32 AmbaDDRC_GetDramSize(UINT32 DdrcId);

UINT32 AmbaDRAMC_Get(UINT32 Offset);
UINT32 AmbaDRAMC_Set(UINT32 Offset, UINT32 Val);
UINT32 AmbaDDRC_Get(UINT32 DdrcId, UINT32 Offset);
UINT32 AmbaDDRC_Set(UINT32 DdrcId, UINT32 Offset, UINT32 Val);

UINT32 AmbaDDRC_GetDramTypeFromVal(UINT32 Config0);
UINT32 AmbaDDRC_GetDramSizeFromVal(UINT32 Config0);
UINT32 AmbaDDRC_GetDuelDieEnFromVal(UINT32 Config0);
UINT32 AmbaDDRC_GetDramType(UINT32 DdrcId);
UINT32 AmbaDDRC_GetDramSize(UINT32 DdrcId);
const char* AmbaDDRC_GetDramTypeStr(UINT32 DdrcId);
const char* AmbaDDRC_GetDramSizeStr(UINT32 DdrcId);
UINT32 AmbaDDRC_GetModeReg(UINT32 DdrcId);
UINT32 AmbaDDRC_SetModeReg(UINT32 DdrcId, UINT32 Val);
void AmbaDDRC_SetModeRegWait(UINT32 DdrcId, UINT32 BitMask, UINT32 Val);

#endif

#endif /* AMBA_DDRC_H */
