/**
 *  @file AmbaIMU.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions for Ambarella IMU (Accelerometer and Gyroscope Devices) driver APIs
 *
 */

#include "AmbaIMU.h"
#include "AmbaPrint.h"

#if 0
#if defined(CONFIG_IMU_INVENSENSE_MPU6500)
#include "AmbaIMU_MPU6500.c"
#endif


AMBA_IMU_OBJ_s *pAmbaImuObj[AMBA_NUM_IMU_SENSOR] = {
#if defined(CONFIG_IMU_INVENSENSE_MPU6500)
    [AMBA_IMU_SENSOR0] = &AmbaIMU_MPU6500Obj,
#endif
};
#endif

AMBA_IMU_OBJ_s *pAmbaImuObj[AMBA_NUM_IMU_SENSOR];

void AmbaIMU_Hook(UINT8 Chan, AMBA_IMU_OBJ_s *pImuObj)
{
    pAmbaImuObj[Chan] = pImuObj;
}


