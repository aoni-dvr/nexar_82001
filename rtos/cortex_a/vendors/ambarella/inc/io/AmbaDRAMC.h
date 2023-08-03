/**
 *  @file AmbaDRAMC.h
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
 *  @details Definitions & Constants for DRAMC Middleware APIs
 *
 */

#ifndef AMBA_DRAMC_H
#define AMBA_DRAMC_H

#ifndef AMBA_DRAMC_DEF_H
#include "AmbaDRAMC_Def.h"
#endif

/* Use 'd' as magic number */
#define DRAMC_IOC_MAGIC 'd'

#define DRAMC_IOC_RESET          _IO(DRAMC_IOC_MAGIC,  0)         // Reset records in this Linux devdrv
#define DRAMC_IOC_CHG_REG        _IOW(DRAMC_IOC_MAGIC, 1, int)    // Change target DRAMC reg offset for R/W
#define DRAMC_IOC_CHG_VAL        _IOW(DRAMC_IOC_MAGIC, 2, int)    // Change value to be written to DRAMC
#define DRAMC_IOC_W_DRAMC        _IO(DRAMC_IOC_MAGIC,  3)         // Write to DRAMC from Linux devdrv
#define DRAMC_IOC_R_DRAMC        _IOR(DRAMC_IOC_MAGIC, 4, int)    // Read from DRAMC to Linux devdrv
#define DRAMC_IOC_R_REG_VALUE    _IOR(DRAMC_IOC_MAGIC, 5, int)    // Read raw value of reg_value
#define DRAMC_IOC_R_REG_OFFSET   _IOR(DRAMC_IOC_MAGIC, 6, int)    // Read raw value of reg_offset


/*
 * Defined in AmbaDRAMC.c
 */
UINT32 AmbaDRAMC_DrvEntry(void);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaDRAMC_GetHostNum(UINT32 *pHostNum);
UINT32 AmbaDRAMC_GetHostStartId(UINT32* pHostStartId);
#endif
UINT32 AmbaDRAMC_EnableStatis(void);
UINT32 AmbaDRAMC_GetStatisInfo(AMBA_DRAMC_STATIS_s *StatisData);

UINT32 AmbaDRAMC_Set(UINT32 Offset, UINT32 Val);
#endif /* AMBA_DRAMC_H */
