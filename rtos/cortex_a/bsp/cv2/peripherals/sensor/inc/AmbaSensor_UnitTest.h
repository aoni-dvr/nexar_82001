/**
 *  @file AmbaSensor_UnitTest.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Header file Ambarella Sensor UnitTest APIs
 *
 */

#ifndef AMBA_SENSOR_UNIITEST_H
#define AMBA_SENSOR_UNIITEST_H

#include <AmbaShell.h>
#ifdef CONFIG_SENSOR_SONY_IMX290_MIPI
#include "AmbaSensor_IMX290_MIPI.h"
#endif
#ifdef CONFIG_SENSOR_SONY_IMX577
#include "AmbaSensor_IMX577.h"
#endif

#define UTIL_MAX_HEX_STR_LEN       11

typedef struct {
    const char* pName;
    UINT32  NameLength;
    AMBA_SENSOR_OBJ_s* pSensorObj;
} AMBA_SENSOR_UT_DEV_LIST_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_UnitTest.c
\*-----------------------------------------------------------------------------------------------*/
extern void AmbaSensorUT_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

#endif
