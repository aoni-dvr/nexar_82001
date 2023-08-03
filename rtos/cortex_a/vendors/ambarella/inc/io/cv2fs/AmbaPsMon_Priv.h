/**
 *  @file AmbaPsMon_Priv.h
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
 *  @details Definitions & Constants for Power Supply Monitor APIs
 *
 */

#ifndef AMBA_PS_MON_PRIV_H
#define AMBA_PS_MON_PRIV_H                            /*    pvlo                       pvhl1                  pvhl2                          */
#define AMBA_PSM_MON_0                        0U      /*    xx_p_ddre_vddq_x           xx_p_ddre_vddq_x       xx_p_ddre_vddq                 */
#define AMBA_PSM_MON_1                        1U      /*    VDDi(@chip upper left)     xx_p_mipi_avdd18_io    xx_p_mipi_avdd18               */
#define AMBA_PSM_MON_2                        2U      /*    VDDi(@chip upper left)     xx_p_gpio1_VDDP        xx_p_gpio1_VDDO                */
#define AMBA_PSM_MON_3                        3U      /*    xx_p_pll_center_avdd075    xx_p_nand_VDDP         xx_p_nand_VDDO                 */
#define AMBA_PSM_MON_4                        4U      /*    xx_p_osc_avdd075           xx_p_osc_DVDD18        xx_p_mipi_slav_psmon_avdd18    */
#define AMBA_PSM_MON_5                        5U      /*    xx_p_otp_avdd075           xx_p_otp_avdd18        xx_p_rtc_avdd18                */
#define AMBA_PSM_MON_6                        6U      /*    xx_p_pll_avdd075           xx_p_pll_avdd18        xx_p_pll_center_avdd18         */
#define AMBA_PSM_MON_7                        7U      /*    xx_p_dsi_avdd075           xx_p_dsi_avdd12        xx_p_dsi_avdd18                */
#define AMBA_PSM_MON_8                        8U      /*    xx_p_ddr0_vddq_x           xx_p_ddr0_vddq_x       xx_p_ddr0_vddq                 */
#define AMBA_PSM_MON_9                        9U      /*    VDDi(@chip lower right)    xx_p_ts0_avdd18        xx_p_ts1_avdd18                */
#define AMBA_PSM_MON_10                       10U     /*    VDDi(@chip lower right)    xx_p_gpio2_VDDP        xx_p_gpio2_VDDO                */
#define AMBA_PSM_MON_11                       11U     /*    VDDi(@chip bottom)         xx_p_sd_VDDP           xx_p_gpio3_psmon_avdd18        */
#define AMBA_PSM_MON_12                       12U     /*    xx_p_usb0_dvdd             xx_p_usb0_VDDh0        xx_p_usb0_vdd330               */
#define AMBA_PSM_MON_13                       13U     /*    VDDi(@chip bottom)         xx_p_sd_VDDO           xx_p_gpio2_psmon_avdd18        */
#define AMBA_PSM_MON_14                       14U     /*    VDDi(@chip bottom)         xx_p_sdio0_VDDP        xx_p_sdio0_VDDO                */
#define AMBA_PSM_MON_15                       15U     /*    VDDi(@chip lower left)     xx_p_gpio3_VDDP        xx_p_gpio3_VDDO                */
#define AMBA_PSM_MON_LV0                      16U     /*    VDDi(@R52 left edge)       VDDi(@chip upper right)                               */
#define AMBA_PSM_MON_LV1                      17U     /*    VDDi(@R52 right edge)      VDDi(@chip center)                                    */
#define AMBA_PSM_MON_NUM                      18U     /* Total number of Power monitor */

#endif  /* AMBA_PS_MON_PRIV_H */
