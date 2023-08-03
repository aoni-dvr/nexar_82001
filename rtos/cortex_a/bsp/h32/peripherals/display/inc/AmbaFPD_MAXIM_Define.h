/**
 *  @file AmbaFPD_MAXIM_Define.h
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
 *  @details Definitions & Constants for DZXtech FPD panel MAXIM DEFINE APIs.
 *
 */

#ifndef AMBA_FPD_MAXIM_DEFINE_H
#define AMBA_FPD_MAXIM_DEFINE_H

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the view mode.
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_FPD_MAXIM_SINGLEVIEW_60HZ         0U    /* 59.94Hz */
#define AMBA_FPD_MAXIM_MULTIVIEW_60HZ          1U    /* 59.94Hz */
#define AMBA_FPD_MAXIM_SINGLEVIEW_A60HZ        2U    /* 60.00Hz */
#define AMBA_FPD_MAXIM_MULTIVIEW_A60HZ         3U    /* 60.00Hz */
#define AMBA_FPD_MAXIM_DUAL_SERDES_NUM_MODE    4U

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the GPIO forward mode.
\*-----------------------------------------------------------------------------------------------*/
#define MAXIM_DUAL_GPIO_FORWARD_DISABLE        0U
#define MAXIM_DUAL_GPIO_FORWARD_PWM            1U
#define MAXIM_DUAL_GPIO_FORWARD_NUM_MODE       2U

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the view Link State.
\*-----------------------------------------------------------------------------------------------*/
#define MAXIM_NO_LINK                          1U
#define MAXIM_MULTIVIEW_A_60Hz                 2U    /* 59.94Hz */
#define MAXIM_MULTIVIEW_B_60Hz                 3U    /* 59.94Hz */
#define MAXIM_MULTIVIEW_SPLITTER_60HZ          4U    /* 59.94Hz */
#define MAXIM_SINGLEVIEW_A_60HZ                5U    /* 59.94Hz */
#define MAXIM_SINGLEVIEW_B_60HZ                6U    /* 59.94Hz */
#define MAXIM_SINGLEVIEW_AUTO_60HZ             7U    /* 59.94Hz */
#define MAXIM_MULTIVIEW_A_A60HZ                8U    /* 60.00Hz */
#define MAXIM_MULTIVIEW_B_A60HZ                9U    /* 60.00Hz */
#define MAXIM_MULTIVIEW_SPLITTER_A60HZ         10U   /* 60.00Hz */
#define MAXIM_SINGLEVIEW_A_A60HZ               11U   /* 60.00Hz */
#define MAXIM_SINGLEVIEW_B_A60HZ               12U   /* 60.00Hz */
#define MAXIM_SINGLEVIEW_AUTO_A60HZ            13U   /* 60.00Hz */
#define MAXIM_STATE_VIEW_NUM                   14U

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for all the serdes Link state.
\*-----------------------------------------------------------------------------------------------*/
#define SERDES_LINK_OK                         0x00000000U
#define SERDES_LINK_WRONG                      0x00000001U
#define SERDES_LINK_ERROR                      0x00000002U

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for MAXIM serdes Link status.
\*-----------------------------------------------------------------------------------------------*/
#define LINK_STATUS_VOU1_MAX96751_A            0x00000001U
#define LINK_STATUS_VOU1_MAX96752_A            0x00000100U
#define LINK_STATUS_VOU1_MAX96752_B            0x00001000U

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for MAXIM serdes register.
\*-----------------------------------------------------------------------------------------------*/
#define DEV_ID_MAX96751                        0x83
#define DEV_ID_MAX96752                        0x82

#define MAXIM_REG_DEV_ID                       0x000D

#define MAXIM_REG_GPIO_01                      0x0203
#define MAXIM_REG_GPIO_02                      0x0206
#define MAXIM_REG_GPIO_04                      0x020C
#define MAXIM_REG_GPIO_06                      0x0212
#define MAXIM_REG_GPIO_07                      0x0215
#define MAXIM_REG_GPIO_09                      0x021B
#define MAXIM_REG_GPIO_13                      0x0227

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for MAXIM Delay.
\*-----------------------------------------------------------------------------------------------*/
#define MAXIM_I2C_DELAY_RESET                  100U
#define MAXIM_I2C_DELAY_RESET_MAX96751         100U
#define MAXIM_I2C_DELAY_RESET_MAX96752         50U
#define MAXIM_I2C_DELAY_COMMAND                0U

/*-----------------------------------------------------------------------------------------------*\
 * Defintions for MAXIM GPIO.
\*-----------------------------------------------------------------------------------------------*/
#define MAXIM_GPIO_IN                          0x08U

#if defined(CONFIG_BSP_CV22RDB) || defined(CONFIG_BSP_CV22DK)
#define MAXIM_REG_IOKEY_ENTER                  MAXIM_REG_GPIO_09
#define MAXIM_REG_IOKEY_UP                     MAXIM_REG_GPIO_04
#define MAXIM_REG_IOKEY_DOWN                   MAXIM_REG_GPIO_01
#define MAXIM_REG_IOKEY_MENU                   MAXIM_REG_GPIO_13
/*------------------------------------------------------------*/
#define MAXIM_A_REG_IOKEY_ENTER                MAXIM_REG_GPIO_09
#define MAXIM_A_REG_IOKEY_UP                   MAXIM_REG_GPIO_04
#define MAXIM_A_REG_IOKEY_DOWN                 MAXIM_REG_GPIO_01
#define MAXIM_A_REG_IOKEY_MENU                 MAXIM_REG_GPIO_13
#define MAXIM_B_REG_IOKEY_ENTER                MAXIM_REG_GPIO_09
#define MAXIM_B_REG_IOKEY_UP                   MAXIM_REG_GPIO_04
#define MAXIM_B_REG_IOKEY_DOWN                 MAXIM_REG_GPIO_01
#define MAXIM_B_REG_IOKEY_MENU                 MAXIM_REG_GPIO_13
#else
#define MAXIM_REG_IOKEY_ENTER                  MAXIM_REG_GPIO_01
#define MAXIM_REG_IOKEY_UP                     MAXIM_REG_GPIO_04
#define MAXIM_REG_IOKEY_DOWN                   MAXIM_REG_GPIO_09
#define MAXIM_REG_IOKEY_MENU                   MAXIM_REG_GPIO_13
/*------------------------------------------------------------*/
#define MAXIM_A_REG_IOKEY_ENTER                MAXIM_REG_GPIO_01
#define MAXIM_A_REG_IOKEY_UP                   MAXIM_REG_GPIO_04
#define MAXIM_A_REG_IOKEY_DOWN                 MAXIM_REG_GPIO_09
#define MAXIM_A_REG_IOKEY_MENU                 MAXIM_REG_GPIO_13
#define MAXIM_B_REG_IOKEY_ENTER                MAXIM_REG_GPIO_01
#define MAXIM_B_REG_IOKEY_UP                   MAXIM_REG_GPIO_04
#define MAXIM_B_REG_IOKEY_DOWN                 MAXIM_REG_GPIO_09
#define MAXIM_B_REG_IOKEY_MENU                 MAXIM_REG_GPIO_13
#endif
/*------------------------------------------------------------*/
#define MAXIM_GPIO_LEVEL_LOW                   0U
#define MAXIM_GPIO_LEVEL_HIGH                  1U
/*------------------------------------------------------------*/
#define MAXIM_IOKEY_ENTER                      0U
#define MAXIM_IOKEY_UP                         1U
#define MAXIM_IOKEY_DOWN                       2U
#define MAXIM_IOKEY_MENU                       3U
#define MAXIM_IOKEY_NUM_MODE                   4U
/*------------------------------------------------------------*/
#define MAXIM_A_IOKEY_ENTER                    0U
#define MAXIM_A_IOKEY_UP                       1U
#define MAXIM_A_IOKEY_DOWN                     2U
#define MAXIM_A_IOKEY_MENU                     3U
#define MAXIM_B_IOKEY_ENTER                    4U
#define MAXIM_B_IOKEY_UP                       5U
#define MAXIM_B_IOKEY_DOWN                     6U
#define MAXIM_B_IOKEY_MENU                     7U
#define MAXIM_DUAL_IOKEY_NUM_MODE              8U

/*-----------------------------------------------------------------------------------------------*\
 * MAXIM Serdes Register Structure
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32 I2cChanNo;
    UINT32 SlaveAddr;
    UINT16 RegisterAddr;
    UINT8 pRxData;
    UINT32 Delay;
} MAXIM_I2C_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * MAXIM DISPLAY CONFIG
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32                                  OutputMode;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s       DisplayTiming;
    AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s  BlankPacket;
} AMBA_FPD_MAXIM_CONFIG_s;


#endif /* AMBA_FPD_MAXIM_DEFINE_H */
