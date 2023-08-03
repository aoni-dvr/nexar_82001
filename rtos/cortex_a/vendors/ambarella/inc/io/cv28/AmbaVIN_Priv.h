/**
 *  @file AmbaVIN_Priv.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for Video Input APIs
 *
 */

#ifndef AMBA_VIN_PRIV_H
#define AMBA_VIN_PRIV_H

#define AMBA_VIN_CHANNEL0       0U
#define AMBA_VIN_CHANNEL1       1U
#define AMBA_VIN_CHANNEL2       2U
#define AMBA_NUM_VIN_CHANNEL    3U

#define AMBA_VIN_MSYNC0         0U
#define AMBA_VIN_MSYNC1         1U
#define AMBA_NUM_VIN_MSYNC      2U

#define AMBA_VIN_SENSOR_CLOCK0      0U
#define AMBA_VIN_SENSOR_CLOCK1      1U
#define AMBA_NUM_VIN_SENSOR_CLOCK   2U

#define AMBA_VIN_TERMINATION_VALUE_DEFAULT  6U

#define AMBA_VIN_DVP_SYNC_PIN_SPCLK_N_0     0U
#define AMBA_VIN_DVP_SYNC_PIN_SPCLK_N_1     1U
#define AMBA_VIN_DVP_SYNC_PIN_SPCLK_P_1     2U

#endif /* AMBA_VIN_PRIV_H */
