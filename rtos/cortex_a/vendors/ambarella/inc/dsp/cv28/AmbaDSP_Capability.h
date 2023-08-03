/**
 *  @file AmbaDSP_Capability.h
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
 *  @details Definitions & Constants for Ambarella DSP Driver HW capability
 *
 */
#ifndef AMBA_DSP_CAPABILITY_H
#define AMBA_DSP_CAPABILITY_H

/* Define Chip limitation */
#define AMBA_DSP_MAX_VIN_NUM                (3U)
#define AMBA_DSP_MAX_VIRT_VIN_NUM           (7U)
#define AMBA_DSP_MAX_VIN_SENSOR_NUM         (2U)  //define Max. Sensor number in one vin input
#define AMBA_DSP_MAX_VIRT_CHAN_NUM          (1U)
#define AMBA_DSP_MAX_VIEWZONE_NUM           (16U)
#define AMBA_DSP_MAX_STREAM_NUM             (16U)
#define AMBA_DSP_MAX_YUVSTRM_NUM            (32U)
#define AMBA_DSP_MAX_YUVSTRM_VIEW_NUM       (16U)
#define AMBA_DSP_MAX_HIER_NUM               (6U)
#define AMBA_DSP_MAX_VOUT_NUM               (1U)

#define AMBA_DSP_MAX_DEC_STREAM_NUM         (6U)

#define AMBA_DSP_MAX_DATACAP_NUM            (16U)

/* Time Division */
#define AMBA_DSP_MAX_VIN_TD_NUM             (8U)
#define AMBA_DSP_MAX_VIN_TD_FRM_NUM         (7U)

/* RC Group */
#define AMBA_DSP_MAX_ENC_GRP_NUM            (1U)
#define AMBA_DSP_MAX_ENC_GRP_STRM_NUM       (6U)

/* Internal used structure */
#define AMBA_DSP_MAX_TOKEN_ARRAY            (4U)

#endif  /* AMBA_DSP_CAPABILITY_H */
