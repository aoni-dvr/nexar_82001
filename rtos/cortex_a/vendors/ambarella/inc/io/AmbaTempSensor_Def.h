/**
 *  @file AmbaTempSensor_Def.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for Temperature Sensor Controller
 *
 */

#ifndef AMBA_TEMPSENSOR_DEF_H
#define AMBA_TEMPSENSOR_DEF_H

#ifndef AMBA_TEMPSENSOR_PRIV_H
#include "AmbaTempSensor_Priv.h"
#endif
#define TEMPS_ERR_0000              (TEMPS_ERR_BASE)          /* Invalid argument */
#define TEMPS_ERR_0001              (TEMPS_ERR_BASE + 0x1U)   /* Unable to do concurrency protection */
#define TEMPS_ERR_0002              (TEMPS_ERR_BASE + 0X2U)   /* Invalid configuration */
#define TEMPS_ERR_000ff             (TEMPS_ERR_BASE + 0XffU)  /* Error from native driver */

#define TEMPSENSOR_ERR_NONE         (OK)
#define TEMPSENSOR_ERR_ARG          (TEMPS_ERR_0000)
#define TEMPSENSOR_ERR_MUTEX        (TEMPS_ERR_0001)
#define TEMPSENSOR_ERR_UNEXPECTED   (TEMPS_ERR_0002)
#define TEMPSENSOR_ERR_POSIX        (TEMPS_ERR_000ff)

#endif /* AMBA_TEMPSENSOR_DEF_H */

