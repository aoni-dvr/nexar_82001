/**
 *  @file AmbaErrorCode.h
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
 *  @details Error Code Base number definitions
 *
 */

#ifndef AMBA_ERROR_CODE_H
#define AMBA_ERROR_CODE_H

/*-- Common error codes --*/
#define OK                  0x00000000U /* General OK */
#define ERR_NONE            0x00000000U /* No Error */
#define ERR_ARG             0x00000001U /* Invalid Argument */
#define ERR_IMPL            0x00000002U /* Not Yet Implemented */
#define ERR_NA              0x00000003U /* Not Applicable */


/*-- error code module catagory --*/
#define BST_ERR_BASE        0x00010000U /* 0x00010000 to 0x0001ffff: Reserved for Bootstrap */
#define BLD_ERR_BASE        0x00020000U /* 0x00020000 to 0x0002ffff: Reserved for Bootloader */
#define ATF_ERR_BASE        0x00030000U /* 0x00030000 to 0x0003ffff: Reserved for ARM Trusted Firmware */
#define PLAT_ERR_BASE       0x00100000U /* 0x00100000 to 0x00ffffff: Reserved for Platform */
#define SSP_ERR_BASE        0x01000000U /* 0x01000000 to 0x01ffffff: Reserved for SSP */
#define COMSVC_ERR_BASE     0x02000000U /* 0x02000000 to 0x02ffffff: Reserved for Common Service */
#define PERIPHERAL_ERR_BASE 0x03000000U /* 0x03000000 to 0x03ffffff: Reserved for Peripheral Driver */

/*-- Platform submodule error code range --*/
#define KAL_ERR_BASE        (PLAT_ERR_BASE)
#define FS_ERR_BASE         (PLAT_ERR_BASE + 0x00010000U)
#define USB_ERR_BASE        (PLAT_ERR_BASE + 0x00020000U)
#define ETH_ERR_BASE        (PLAT_ERR_BASE + 0x00030000U)
#define PIO_ERR_BASE        (PLAT_ERR_BASE + 0x00040000U)

#define ADC_ERR_BASE        (PLAT_ERR_BASE + 0x00100000U)
#define AIN_ERR_BASE        (PLAT_ERR_BASE + 0x00110000U)
#define AOUT_ERR_BASE       (PLAT_ERR_BASE + 0x00120000U)
#define CACHE_ERR_BASE      (PLAT_ERR_BASE + 0x00130000U)
#define CAN_ERR_BASE        (PLAT_ERR_BASE + 0x00140000U)
#define CVBS_ERR_BASE       (PLAT_ERR_BASE + 0x00150000U)
#define DMA_ERR_BASE        (PLAT_ERR_BASE + 0x00160000U)
#define GPIO_ERR_BASE       (PLAT_ERR_BASE + 0x00170000U)
#define HDMI_ERR_BASE       (PLAT_ERR_BASE + 0x00180000U)
#define I2C_ERR_BASE        (PLAT_ERR_BASE + 0x00190000U)
#define I2S_ERR_BASE        (PLAT_ERR_BASE + 0x001a0000U)
#define INT_ERR_BASE        (PLAT_ERR_BASE + 0x001b0000U)
#define IR_ERR_BASE         (PLAT_ERR_BASE + 0x001c0000U)
#define MMU_ERR_BASE        (PLAT_ERR_BASE + 0x001d0000U)
#define NAND_ERR_BASE       (PLAT_ERR_BASE + 0x001e0000U)
#define NVM_ERR_BASE        (PLAT_ERR_BASE + 0x001f0000U)
#define PLL_ERR_BASE        (PLAT_ERR_BASE + 0x00200000U)
#define PWM_ERR_BASE        (PLAT_ERR_BASE + 0x00210000U)
#define PWR_ERR_BASE        (PLAT_ERR_BASE + 0x00220000U)
#define RTC_ERR_BASE        (PLAT_ERR_BASE + 0x00230000U)
#define SD_ERR_BASE         (PLAT_ERR_BASE + 0x00240000U)
#define SPI_ERR_BASE        (PLAT_ERR_BASE + 0x00250000U)
#define SPINAND_ERR_BASE    (PLAT_ERR_BASE + 0x00260000U)
#define SPINOR_ERR_BASE     (PLAT_ERR_BASE + 0x00270000U)
#define SYS_ERR_BASE        (PLAT_ERR_BASE + 0x00280000U)
#define TMR_ERR_BASE        (PLAT_ERR_BASE + 0x00290000U)
#define UART_ERR_BASE       (PLAT_ERR_BASE + 0x002a0000U)
#define VIN_ERR_BASE        (PLAT_ERR_BASE + 0x002b0000U)
#define VOUT_ERR_BASE       (PLAT_ERR_BASE + 0x002c0000U)
#define WDT_ERR_BASE        (PLAT_ERR_BASE + 0x002d0000U)
#define GDMA_ERR_BASE       (PLAT_ERR_BASE + 0x002e0000U)
#define DMIC_ERR_BASE       (PLAT_ERR_BASE + 0x002f0000U)
#define DRAM_ERR_BASE       (PLAT_ERR_BASE + 0x00300000U)
#define RNG_ERR_BASE        (PLAT_ERR_BASE + 0x00310000U)
#define TEMPS_ERR_BASE      (PLAT_ERR_BASE + 0x00320000U)
#define PSM_ERR_BASE        (PLAT_ERR_BASE + 0x00320000U)

#define THREADX_ERR_BASE    (KAL_ERR_BASE + 0x800000U)
#define PRFILE2_ERR_BASE    (FS_ERR_BASE  + 0x800000U)
#define USBX_ERR_BASE       (USB_ERR_BASE + 0x800000U)
#define NETX_ERR_BASE       (ETH_ERR_BASE + 0x800000U)

/*-- Common Service submodule error code range --*/
#define LIBWRAP_ERR_BASE    (COMSVC_ERR_BASE + 0x00010000U)
#define CFS_ERR_BASE        (COMSVC_ERR_BASE + 0x00020000U)
#define FIFO_ERR_BASE       (COMSVC_ERR_BASE + 0x00030000U)
#define MUXER_ERR_BASE      (COMSVC_ERR_BASE + 0x00040000U)
#define DEMUXER_ERR_BASE    (COMSVC_ERR_BASE + 0x00050000U)
#define IMG_SCHDLR_ERR_BASE (COMSVC_ERR_BASE + 0x00060000U)
#define DRAW_ERR_BASE       (COMSVC_ERR_BASE + 0x00070000U)
#define STREAM_ERR_BASE     (COMSVC_ERR_BASE + 0x00090000U)
#define FTCM_ERR_BASE       (COMSVC_ERR_BASE + 0x000A0000U)
#define IMGPROC_ERR_BASE    (COMSVC_ERR_BASE + 0x000B0000U)
#define TUNE_ERR_BASE       (COMSVC_ERR_BASE + 0x000C0000U)
#define SYNCSVC_ERR_BASE    (COMSVC_ERR_BASE + 0x000D0000U)
#define TRANSFER_ERR_BASE   (COMSVC_ERR_BASE + 0x000E0000U)
#define WU_ERR_BASE         (COMSVC_ERR_BASE + 0x000F0000U)
#define CODEC_ERR_BASE      (COMSVC_ERR_BASE + 0x00100000U)
#define PLAYER_ERR_BASE     (COMSVC_ERR_BASE + 0x00110000U)
#define ADAS_ERR_BASE       (COMSVC_ERR_BASE + 0x00120000U)
#define DCF_ERR_BASE        (COMSVC_ERR_BASE + 0x00130000U)
#define RECODER_ERR_BASE    (COMSVC_ERR_BASE + 0x00140000U)
#define AUDIO_ERR_BASE      (COMSVC_ERR_BASE + 0x00150000U)
#define STU_ERR_BASE        (COMSVC_ERR_BASE + 0x00160000U)
#define IMG_IMUMGR_ERR_BASE (COMSVC_ERR_BASE + 0x00170000U)
#define EIS_ERR_BASE        (COMSVC_ERR_BASE + 0x00180000U)
#define IPMONADJ_ERR_BASE   (COMSVC_ERR_BASE + 0x00190000U)
#define IPMONAE_ERR_BASE    (COMSVC_ERR_BASE + 0x00200000U)
#define IPMONAWB_ERR_BASE   (COMSVC_ERR_BASE + 0x00210000U)
#define AF_ERR_BASE         (COMSVC_ERR_BASE + 0x00220000U)
/*-- Peripheral driver submodule error code range --*/
#define SENSOR_ERR_BASE     (PERIPHERAL_ERR_BASE + 0x00010000U)
#define BRIDGE_ERR_BASE     (PERIPHERAL_ERR_BASE + 0x00020000U)
#define YUV_ERR_BASE        (PERIPHERAL_ERR_BASE + 0x00030000U)
#define AUCODEC_ERR_BASE    (PERIPHERAL_ERR_BASE + 0x00040000U)
#define IMU_ERR_BASE        (PERIPHERAL_ERR_BASE + 0x00050000U)
#define LENS_ERR_BASE       (PERIPHERAL_ERR_BASE + 0x00060000U)

/*-- SSP submodule error code range --*/
#define AENC_ERR_BASE       (SSP_ERR_BASE + 0x00010000U)
#define ADEC_ERR_BASE       (SSP_ERR_BASE + 0x00020000U)
#define DSPMON_ERR_BASE     (SSP_ERR_BASE + 0x00030000U)
#define IK_ERR_BASE         (SSP_ERR_BASE + 0x00100000U)
#define CAL_ERR_BASE        (SSP_ERR_BASE + 0x00200000U)
#define SCHDR_ERR_BASE      (SSP_ERR_BASE + 0x00400000U)
#define CVTASK_ERR_BASE     (SSP_ERR_BASE + 0x00410000U)
#define CVAPP_ERR_BASE      (SSP_ERR_BASE + 0x00420000U)
#define CVACC_ERR_BASE      (SSP_ERR_BASE + 0x00430000U)
#define AMBALINK_ERR_BASE   (SSP_ERR_BASE + 0x00700000U)

#endif  /* AMBA_ERROR_CODE_H */
