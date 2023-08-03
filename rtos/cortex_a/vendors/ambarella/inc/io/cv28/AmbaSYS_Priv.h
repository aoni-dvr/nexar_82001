/**
 *  @file AmbaSYS_Priv.h
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
 *  @details Constants and Definitions for System APIs
 *
 */

#ifndef AMBA_SYS_PRIV_H
#define AMBA_SYS_PRIV_H

#define AMBA_SYS_BOOT_FROM_NAND         0x0U
#define AMBA_SYS_BOOT_FROM_EMMC         0x1U
#define AMBA_SYS_BOOT_FROM_SPI_NOR      0x2U
#define AMBA_SYS_BOOT_FROM_USB          0x4U
#define AMBA_SYS_BOOT_FROM_SPI_NAND     0x5U

#define AMBA_SYS_POWER_MODE_STANDBY     0x0U
#define AMBA_SYS_POWER_MODE_SUSPEND     0x1U

#define AMBA_SYS_WAKEUP_NONE            0x0U    /* No power sequence invloved */
#define AMBA_SYS_WAKEUP_ALARM           0x1U    /* Wake-Up by Alarm */
#define AMBA_SYS_WAKEUP_PWC_WKUP        0x2U    /* Wake-Up by PWC_WKUP pin */
#define AMBA_SYS_WAKEUP_PWC_WKUP1       0x3U    /* Wake-Up by PWC_WKUP1 pin */
#define AMBA_SYS_WAKEUP_PWC_WKUP2       0x4U    /* Wake-Up by PWC_WKUP2 pin */
#define AMBA_SYS_WAKEUP_PWC_WKUP3       0x5U    /* Wake-Up by PWC_WKUP3 pin */

#define AMBA_SYS_EXT_CLK_AUDIO          0x0U    /* iopad_apb_clk_au_c */
#define AMBA_SYS_EXT_CLK_ETHERNET       0x1U    /* xx_enet_clk_rx */
#define AMBA_SYS_EXT_CLK_SENSOR_IN      0x2U    /* xx_clk_si */
#define AMBA_SYS_EXT_CLK_SENSOR_SPCLK0  0x3U    /* lvds_spclk_c[0] */
#define AMBA_SYS_EXT_CLK_SENSOR_SPCLK1  0x4U    /* lvds_spclk_c[1] */
#define AMBA_SYS_EXT_CLK_SENSOR_SPCLK2  0x5U    /* lvds_spclk_c[2] */

#define AMBA_SYS_CLK_CORE               0x0U
#define AMBA_SYS_CLK_CORTEX             0x1U
#define AMBA_SYS_CLK_DRAM               0x2U
#define AMBA_SYS_CLK_IDSP               0x3U
#define AMBA_SYS_CLK_VISION             0x4U
#define AMBA_SYS_CLK_DSP_SYS            0x6U

/* for A/V Sync */
#define AMBA_SYS_CLK_AUD_0              0x10U   /* Audio clock source 0 */
#define AMBA_SYS_CLK_VID_IN0            0x20U   /* Video input clock source 0 (master H/V sync 0) */
#define AMBA_SYS_CLK_VID_IN1            0x21U   /* Video input clock source 1 (master H/V sync 1) */
#define AMBA_SYS_CLK_VID_OUT0           0x30U   /* Video output clock source 0 (digital video interface) */
#define AMBA_SYS_CLK_VID_OUT1           0x31U   /* Video output clock source 1 (CVBS, HDMI) */
#define AMBA_SYS_CLK_REF_OUT0           0x50U   /* Clock output 0 (clk_si) */
#define AMBA_SYS_CLK_REF_OUT1           0x51U   /* Clock output 1 (clk_si2) */

#define AMBA_SYS_FEATURE_ALL            0x00U   /* All the DSP features */
#define AMBA_SYS_FEATURE_DSP            0x01U   /* DSP basic functions */
#define AMBA_SYS_FEATURE_MCTF           0x02U   /* DSP: Motion Compensated Temporal Filtering */
#define AMBA_SYS_FEATURE_CODEC          0x03U   /* DSP: Any encoder and decoder */
#define AMBA_SYS_FEATURE_HEVC           0x04U   /* DSP: HEVC encoder and decoder */
#define AMBA_SYS_FEATURE_CNN            0x11U   /* CV: Convolutional Neural Network */

#endif  /* AMBA_SYS_PRIV_H */
