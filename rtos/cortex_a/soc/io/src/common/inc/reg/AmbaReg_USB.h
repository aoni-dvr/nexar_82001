/**
 *  @file AmbaReg_USB.h
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
 *  @details Definitions & Constants for Universal Serial Bus (USB) Device Controller Control Registers
 *
 */

#ifndef AMBA_REG_USB_H
#define AMBA_REG_USB_H

/*
 * Define USB generic equivalences.
 */
#define USB_DEV_SPD_HI      0   // 00 (RW) - PHY CLK = 30 or 60 MHz
#define USB_DEV_SPD_FU      1   // 01 (RW) - PHY CLK = 30 or 60 MHz
#define USB_DEV_SPD_LO      2   // 10 (RW) - PHY CLK = 6 MHz
#define USB_DEV_SPD_FU48    3   // 11 (RW) - PHY CLK = 48 MHz
#define USB_IRQ_NO          4
#define USB_IRQ_PRIORITY    USB_IRQ_NO

/*
 * USB: In/Out Endpoint Control Register.
 */

#define EP_CONTROL_REG_STALL_SHIFT             ((UINT32)(0U))
#define EP_CONTROL_REG_FLUSH_TXFIFO_SHIFT      ((UINT32)(1U))
#define EP_CONTROL_REG_SNOOP_SHIFT             ((UINT32)(2U))
#define EP_CONTROL_REG_POLL_DEMAND_SHIFT       ((UINT32)(3U))
#define EP_CONTROL_REG_TYPE_SHIFT              ((UINT32)(4U))
#define EP_CONTROL_REG_NAK_SHIFT               ((UINT32)(6U))
#define EP_CONTROL_REG_SET_NAK_SHIFT           ((UINT32)(7U))
#define EP_CONTROL_REG_CLEAR_NAK_SHIFT         ((UINT32)(8U))
#define EP_CONTROL_REG_RX_READY_SHIFT          ((UINT32)(9U))
#define EP_CONTROL_REG_SEND_NULL_SHIFT         ((UINT32)(10U))
#define EP_CONTROL_REG_CLOSE_DESC_SHIFT        ((UINT32)(11U))

#define EP_CONTROL_REG_STALL_MASK              (((UINT32)1U) /*<< EP_CONTROL_REG_STALL_SHIFT*/)
#define EP_CONTROL_REG_FLUSH_TXFIFO_MASK       (((UINT32)1U) << EP_CONTROL_REG_FLUSH_TXFIFO_SHIFT)
#define EP_CONTROL_REG_SNOOP_MASK              (((UINT32)1U) << EP_CONTROL_REG_SNOOP_SHIFT)
#define EP_CONTROL_REG_POLL_DEMAND_MASK        (((UINT32)1U) << EP_CONTROL_REG_POLL_DEMAND_SHIFT)
#define EP_CONTROL_REG_TYPE_MASK               (((UINT32)3U) << EP_CONTROL_REG_TYPE_SHIFT)
#define EP_CONTROL_REG_NAK_MASK                (((UINT32)1U) << EP_CONTROL_REG_NAK_SHIFT)
#define EP_CONTROL_REG_SET_NAK_MASK            (((UINT32)1U) << EP_CONTROL_REG_SET_NAK_SHIFT)
#define EP_CONTROL_REG_CLEAR_NAK_MASK          (((UINT32)1U) << EP_CONTROL_REG_CLEAR_NAK_SHIFT)
#define EP_CONTROL_REG_RX_READY_MASK           (((UINT32)1U) << EP_CONTROL_REG_RX_READY_SHIFT)
#define EP_CONTROL_REG_SEND_NULL_MASK          (((UINT32)1U) << EP_CONTROL_REG_SEND_NULL_SHIFT)
#define EP_CONTROL_REG_CLOSE_DESC_MASK         (((UINT32)1U) << EP_CONTROL_REG_CLOSE_DESC_SHIFT)

/*
 * USB: In/Out Endpoint Status Register.
 */

#define EP_STATUS_REG_OUT_SHIFT             ((UINT32)(4U))
#define EP_STATUS_REG_IN_SHIFT              ((UINT32)(6U))
#define EP_STATUS_REG_BNA_SHIFT             ((UINT32)(7U))
#define EP_STATUS_REG_AHB_ERR_SHIFT         ((UINT32)(9U))
#define EP_STATUS_REG_TXDMA_COMPLETE_SHIFT  ((UINT32)(10U))
#define EP_STATUS_REG_RX_PKT_SIZE_SHIFT     ((UINT32)(11U))
#define EP_STATUS_REG_ISO_IN_DONE_SHIFT     ((UINT32)(23U))
#define EP_STATUS_REG_RX_CLR_STALL_SHIFT    ((UINT32)(25U))
#define EP_STATUS_REG_RX_SET_STALL_SHIFT    ((UINT32)(26U))
#define EP_STATUS_REG_TX_FIFO_EMPTY_SHIFT   ((UINT32)(27U))
#define EP_STATUS_REG_CLOSE_DESC_CLR_SHIFT  ((UINT32)(28U))

#define EP_STATUS_REG_OUT_MASK             (((UINT32)0x03U) << EP_STATUS_REG_OUT_SHIFT)
#define EP_STATUS_REG_IN_MASK              (((UINT32)0x01U) << EP_STATUS_REG_IN_SHIFT)
#define EP_STATUS_REG_BNA_MASK             (((UINT32)0x01U) << EP_STATUS_REG_BNA_SHIFT)
#define EP_STATUS_REG_AHB_ERR_MASK         (((UINT32)0x01U) << EP_STATUS_REG_AHB_ERR_SHIFT)
#define EP_STATUS_REG_TXDMA_COMPLETE_MASK  (((UINT32)0x01U) << EP_STATUS_REG_TXDMA_COMPLETE_SHIFT)
#define EP_STATUS_REG_RX_PKT_SIZE_MASK     (((UINT32)0x0FFFU) << EP_STATUS_REG_RX_PKT_SIZE_SHIFT)
#define EP_STATUS_REG_ISO_IN_DONE_MASK     (((UINT32)0x01U) << EP_STATUS_REG_ISO_IN_DONE_SHIFT)
#define EP_STATUS_REG_RX_CLR_STALL_MASK    (((UINT32)0x01U) << EP_STATUS_REG_RX_CLR_STALL_SHIFT)
#define EP_STATUS_REG_RX_SET_STALL_MASK    (((UINT32)0x01U) << EP_STATUS_REG_RX_SET_STALL_SHIFT)
#define EP_STATUS_REG_TX_FIFO_EMPTY_MASK   (((UINT32)0x01U) << EP_STATUS_REG_TX_FIFO_EMPTY_SHIFT)
#define EP_STATUS_REG_CLOSE_DESC_CLR_MASK  (((UINT32)0x01U) << EP_STATUS_REG_CLOSE_DESC_CLR_SHIFT)


/*
 * USB: In Endpoint Buffer Size Registers.
 */

#define EP_BUF_REG_SIZE_SHIFT        ((UINT32)(0U))
#define EP_BUF_REG_ISO_IN_PID_SHIFT  ((UINT32)(16U))

#define EP_BUF_REG_SIZE_MASK         (((UINT32)0x0FFFFU) /*<< EP_BUF_REG_SIZE_SHIFT*/)
#define EP_BUF_REG_ISO_IN_PID_MASK   (((UINT32)0x00003U) << EP_BUF_REG_ISO_IN_PID_SHIFT)

/*
 * USB: Out Endpoint Packet Frame Number Registers.
 */

#define EP_PFN_REG_FN_SHIFT                ((UINT32)(0U))
#define EP_PFN_REG_ISO_OUT_PID_SHIFT       ((UINT32)(16U))

#define EP_PFN_REG_FN_MASK                (((UINT32)0x0FFFFU) /*<< EP_PFN_REG_FN_SHIFT*/)
#define EP_PFN_REG_ISO_OUT_PID_MASK       (((UINT32)0x00003U) << EP_PFN_REG_ISO_OUT_PID_SHIFT)

/*
 * USB: In/Out Endpoint Maximum Packet Size Register .
 */

#define EP_MPS_REG_MPS_SHIFT ((UINT32)(0U))
#define EP_MPS_REG_MPS_MASK  (((UINT32)0x0FFFFU) << EP_MPS_REG_MPS_SHIFT)

/*
 * USB: In/Out Endpoint Descriptor Pointer Register.
 */

/*
 * USB: In Endpoint Write/Out Endpoint Read Confirmation (for Slave mode) Registers.
 */

#define EP_RWC_REG_CONFIRM_SHIFT ((UINT32)(0U))
#define EP_RWC_REG_CONFIRM_MASK  (((UINT32)0x1U) /*<< EP_RWC_REG_CONFIRM_SHIFT*/)

/*
 * USB: Device Configuration Register.
 */
#define USB_DEV_SELF_POWER              1
#define USB_DEV_REMOTE_WAKEUP           1
#define USB_DEV_PHY_8BIT                1
#define USB_DEV_UTMI_DIR_UNI            0
#define USB_DEV_UTMI_DIR_BI             1
#define USB_DEV_HALT_ACK                0
#define USB_DEV_HALT_STALL              1
#define USB_DEV_SET_DESC_STALL          0
#define USB_DEV_SET_DESC_ACK            1
#define USB_DEV_CSR_PRG                 1

#define CONFIG_REG_SPEED_SHIFT             ((UINT32)(0U))
#define CONFIG_REG_REMOTE_WAKEUP_SHIFT     ((UINT32)(2U))
#define CONFIG_REG_SELF_POWERED_SHIFT      ((UINT32)(3U))
#define CONFIG_REG_SYNC_FRAME_SHIFT        ((UINT32)(4U))
#define CONFIG_REG_PHY_TYPE_SHIFT          ((UINT32)(5U))
#define CONFIG_REG_RESERVED_SHIFT          ((UINT32)(6U))
#define CONFIG_REG_STATUS_SHIFT            ((UINT32)(7U))
#define CONFIG_REG_PHY_ERROR_SHIFT         ((UINT32)(9U))
#define CONFIG_REG_TIMEOUT_FS_SHIFT        ((UINT32)(10U))
#define CONFIG_REG_TIMEOUT_HS_SHIFT        ((UINT32)(13U))
#define CONFIG_REG_HALT_STATUS_SHIFT       ((UINT32)(16U))
#define CONFIG_REG_DYNAMIC_PROGRAM_SHIFT   ((UINT32)(17U))
#define CONFIG_REG_SET_DESC_SHIFT          ((UINT32)(18U))

#define CONFIG_REG_SPEED_MASK           (((UINT32)0x03U) /*<< CONFIG_REG_SPEED_SHIFT*/)
#define CONFIG_REG_REMOTE_WAKEUP_MASK   (((UINT32)0x01U) << CONFIG_REG_REMOTE_WAKEUP_SHIFT)
#define CONFIG_REG_SELF_POWERED_MASK    (((UINT32)0x01U) << CONFIG_REG_SELF_POWERED_SHIFT)
#define CONFIG_REG_SYNC_FRAME_MASK      (((UINT32)0x01U) << CONFIG_REG_SYNC_FRAME_SHIFT)
#define CONFIG_REG_PHY_TYPE_MASK        (((UINT32)0x01U) << CONFIG_REG_PHY_TYPE_SHIFT)
#define CONFIG_REG_RESERVED_MASK        (((UINT32)0x01U) << CONFIG_REG_RESERVED_SHIFT)
#define CONFIG_REG_STATUS_MASK          (((UINT32)0x03U) << CONFIG_REG_STATUS_SHIFT)
#define CONFIG_REG_PHY_ERROR_MASK       (((UINT32)0x01U) << CONFIG_REG_PHY_ERROR_SHIFT)
#define CONFIG_REG_TIMEOUT_FS_MASK      (((UINT32)0x07U) << CONFIG_REG_TIMEOUT_FS_SHIFT)
#define CONFIG_REG_TIMEOUT_HS_MASK      (((UINT32)0x07U) << CONFIG_REG_TIMEOUT_HS_SHIFT)
#define CONFIG_REG_HALT_STATUS_MASK     (((UINT32)0x01U) << CONFIG_REG_HALT_STATUS_SHIFT)
#define CONFIG_REG_DYNAMIC_PROGRAM_MASK (((UINT32)0x01U) << CONFIG_REG_DYNAMIC_PROGRAM_SHIFT)
#define CONFIG_REG_SET_DESC_MASK        (((UINT32)0x01U) << CONFIG_REG_SET_DESC_SHIFT)

/*
 * USB: Device Control Register.
 */
#define USB_DEV_RESUME_WAKEUP           1
#define USB_DEV_RX_DMA_ON               1
#define USB_DEV_TX_DMA_ON               1
#define USB_DEV_LITTLE_ENDN             0
#define USB_DEV_DESC_UPD_PYL            0
#define USB_DEV_DESC_UPD_PKT            1
#define USB_DEV_THRESH_ON               1
#define USB_DEV_BURST_ON                1
#define USB_DEV_DMA_MODE                1
#define USB_DEV_SET_SOFT_DISCON         1
#define USB_DEV_SET_FLUSH_RXFIFO        1
#define USB_DEV_SET_NAK                 1
#define USB_DEV_SET_CSR_DONE            1
#define USB_DEV_BURST_LEN               7

#define CONTROL_REG_RESUME_SIGNAL_SHIFT        ((UINT32)(0U))
#define CONTROL_REG_RXDMA_ENABLE_SHIFT         ((UINT32)(2U))
#define CONTROL_REG_TXDMA_ENABLE_SHIFT         ((UINT32)(3U))
#define CONTROL_REG_DESC_UPDATE_SHIFT          ((UINT32)(4U))
#define CONTROL_REG_BIG_ENDIAN_SHIFT           ((UINT32)(5U))
#define CONTROL_REG_BUFFER_FILL_SHIFT          ((UINT32)(6U))
#define CONTROL_REG_THRESHOLD_ENABLE_SHIFT     ((UINT32)(7U))
#define CONTROL_REG_BURST_ENABLE_SHIFT         ((UINT32)(8U))
#define CONTROL_REG_MODE_SHIFT                 ((UINT32)(9U))
#define CONTROL_REG_SOFT_DISCONNECT_SHIFT      ((UINT32)(10U))
#define CONTROL_REG_SCALE_DOWN_SHIFT           ((UINT32)(11U))
#define CONTROL_REG_DEVICE_NAK_SHIFT           ((UINT32)(12U))
#define CONTROL_REG_CSR_DONE_SHIFT             ((UINT32)(13U))
#define CONTROL_REG_RXFIFO_FLUSH_SHIFT         ((UINT32)(14U))
#define CONTROL_REG_BURST_LENGTH_SHIFT         ((UINT32)(16U))
#define CONTROL_REG_THRESHOLD_LENGTH_SHIFT     ((UINT32)(24U))

#define CONTROL_REG_RESUME_SIGNAL_MASK         (((UINT32)1U) /*<< CONTROL_REG_RESUME_SIGNAL_SHIFT*/)
#define CONTROL_REG_RXDMA_ENABLE_MASK          (((UINT32)1U) << CONTROL_REG_RXDMA_ENABLE_SHIFT)
#define CONTROL_REG_TXDMA_ENABLE_MASK          (((UINT32)1U) << CONTROL_REG_TXDMA_ENABLE_SHIFT)
#define CONTROL_REG_DESC_UPDATE_MASK           (((UINT32)1U) << CONTROL_REG_DESC_UPDATE_SHIFT)
#define CONTROL_REG_BIG_ENDIAN_MASK            (((UINT32)1U) << CONTROL_REG_BIG_ENDIAN_SHIFT)
#define CONTROL_REG_BUFFER_FILL_MASK           (((UINT32)1U) << CONTROL_REG_BUFFER_FILL_SHIFT)
#define CONTROL_REG_THRESHOLD_ENABLE_MASK      (((UINT32)1U) << CONTROL_REG_THRESHOLD_ENABLE_SHIFT)
#define CONTROL_REG_BURST_ENABLE_MASK          (((UINT32)1U) << CONTROL_REG_BURST_ENABLE_SHIFT)
#define CONTROL_REG_MODE_MASK                  (((UINT32)1U) << CONTROL_REG_MODE_SHIFT)
#define CONTROL_REG_SOFT_DISCONNECT_MASK       (((UINT32)1U) << CONTROL_REG_SOFT_DISCONNECT_SHIFT)
#define CONTROL_REG_SCALE_DOWN_MASK            (((UINT32)1U) << CONTROL_REG_SCALE_DOWN_SHIFT)
#define CONTROL_REG_DEVICE_NAK_MASK            (((UINT32)1U) << CONTROL_REG_DEVICE_NAK_SHIFT)
#define CONTROL_REG_CSR_DONE_MASK              (((UINT32)1U) << CONTROL_REG_CSR_DONE_SHIFT)
#define CONTROL_REG_RXFIFO_FLUSH_MASK          (((UINT32)1U) << CONTROL_REG_RXFIFO_FLUSH_SHIFT)
#define CONTROL_REG_BURST_LENGTH_MASK          (((UINT32)0x0FFU) << CONTROL_REG_BURST_LENGTH_SHIFT)
#define CONTROL_REG_THRESHOLD_LENGTH_MASK      (((UINT32)0x0FFU) << CONTROL_REG_THRESHOLD_LENGTH_SHIFT)

/*
 * USB: Device Status Register. (Read Only)
 */

#define DEV_STATUS_REG_CONFIG_SHIFT        ((UINT32)(0U))
#define DEV_STATUS_REG_INTERFACE_SHIFT     ((UINT32)(4U))
#define DEV_STATUS_REG_ALTERNATE_SHIFT     ((UINT32)(8U))
#define DEV_STATUS_REG_SUSPEND_SHIFT       ((UINT32)(12U))
#define DEV_STATUS_REG_ENUM_SPEED_SHIFT    ((UINT32)(13U))
#define DEV_STATUS_REG_RXFIFO_EMPTY_SHIFT  ((UINT32)(15U))
#define DEV_STATUS_REG_PHY_ERROR_SHIFT     ((UINT32)(16U))
#define DEV_STATUS_REG_REMOTE_WAKEUP_SHIFT ((UINT32)(17U))
#define DEV_STATUS_REG_TIME_STAMP_SHIFT    ((UINT32)(18U))

#define DEV_STATUS_REG_CONFIG_MASK         (((UINT32)0x0FU) /*<< DEV_STATUS_REG_CONFIG_SHIFT*/)
#define DEV_STATUS_REG_INTERFACE_MASK      (((UINT32)0x0FU) << DEV_STATUS_REG_INTERFACE_SHIFT)
#define DEV_STATUS_REG_ALTERNATE_MASK      (((UINT32)0x0FU) << DEV_STATUS_REG_ALTERNATE_SHIFT)
#define DEV_STATUS_REG_SUSPEND_MASK        (((UINT32)0x01U) << DEV_STATUS_REG_SUSPEND_SHIFT)
#define DEV_STATUS_REG_ENUM_SPEED_MASK     (((UINT32)0x03U) << DEV_STATUS_REG_ENUM_SPEED_SHIFT)
#define DEV_STATUS_REG_RXFIFO_EMPTY_MASK   (((UINT32)0x01U) << DEV_STATUS_REG_RXFIFO_EMPTY_SHIFT)
#define DEV_STATUS_REG_PHY_ERROR_MASK      (((UINT32)0x01U) << DEV_STATUS_REG_PHY_ERROR_SHIFT)
#define DEV_STATUS_REG_REMOTE_WAKEUP_MASK  (((UINT32)0x01U) << DEV_STATUS_REG_REMOTE_WAKEUP_SHIFT)
#define DEV_STATUS_REG_TIME_STAMP_MASK     (((UINT32)0x03FFFU) << DEV_STATUS_REG_TIME_STAMP_SHIFT)
/*
 * USB: Device Interrupt Status Register.
 */

#define DEV_INTSTS_REG_SET_CONFIG_SHIFT        ((UINT32)(0U))
#define DEV_INTSTS_REG_SET_INTERFACE_SHIFT     ((UINT32)(1U))
#define DEV_INTSTS_REG_IDLE_SHIFT              ((UINT32)(2U))
#define DEV_INTSTS_REG_RESET_SHIFT             ((UINT32)(3U))
#define DEV_INTSTS_REG_SUSPEND_SHIFT           ((UINT32)(4U))
#define DEV_INTSTS_REG_SOF_SHIFT               ((UINT32)(5U))
#define DEV_INTSTS_REG_ENUM_DONE_SHIFT         ((UINT32)(6U))
#define DEV_INTSTS_REG_REMOTE_WAKEUP_SHIFT     ((UINT32)(7U))

#define DEV_INTSTS_REG_SET_CONFIG_MASK         (((UINT32)0x01U) /*<< DEV_INTSTS_REG_SET_CONFIG_SHIFT*/)
#define DEV_INTSTS_REG_SET_INTERFACE_MASK      (((UINT32)0x01U) << DEV_INTSTS_REG_SET_INTERFACE_SHIFT)
#define DEV_INTSTS_REG_IDLE_MASK               (((UINT32)0x01U) << DEV_INTSTS_REG_IDLE_SHIFT)
#define DEV_INTSTS_REG_RESET_MASK              (((UINT32)0x01U) << DEV_INTSTS_REG_RESET_SHIFT)
#define DEV_INTSTS_REG_SUSPEND_MASK            (((UINT32)0x01U) << DEV_INTSTS_REG_SUSPEND_SHIFT)
#define DEV_INTSTS_REG_SOF_MASK                (((UINT32)0x01U) << DEV_INTSTS_REG_SOF_SHIFT)
#define DEV_INTSTS_REG_ENUM_DONE_MASK          (((UINT32)0x01U) << DEV_INTSTS_REG_ENUM_DONE_SHIFT)
#define DEV_INTSTS_REG_REMOTE_WAKEUP_MASK      (((UINT32)0x01U) << DEV_INTSTS_REG_REMOTE_WAKEUP_SHIFT)

/*
 * USB: Device Interrupt Mask Register.
 */

#define DEV_INTMASK_REG_SET_CONFIG_SHIFT        ((UINT32)(0U))
#define DEV_INTMASK_REG_SET_INTERFACE_SHIFT     ((UINT32)(1U))
#define DEV_INTMASK_REG_IDLE_SHIFT              ((UINT32)(2U))
#define DEV_INTMASK_REG_RESET_SHIFT             ((UINT32)(3U))
#define DEV_INTMASK_REG_SUSPEND_SHIFT           ((UINT32)(4U))
#define DEV_INTMASK_REG_SOF_SHIFT               ((UINT32)(5U))
#define DEV_INTMASK_REG_ENUM_DONE_SHIFT         ((UINT32)(6U))
#define DEV_INTMASK_REG_REMOTE_WAKEUP_SHIFT     ((UINT32)(7U))

#define DEV_INTMASK_REG_SET_CONFIG_MASK         (((UINT32)0x01U) /*<< DEV_INTMASK_REG_SET_CONFIG_SHIFT*/)
#define DEV_INTMASK_REG_SET_INTERFACE_MASK      (((UINT32)0x01U) << DEV_INTMASK_REG_SET_INTERFACE_SHIFT)
#define DEV_INTMASK_REG_IDLE_MASK               (((UINT32)0x01U) << DEV_INTMASK_REG_IDLE_SHIFT)
#define DEV_INTMASK_REG_RESET_MASK              (((UINT32)0x01U) << DEV_INTMASK_REG_RESET_SHIFT)
#define DEV_INTMASK_REG_SUSPEND_MASK            (((UINT32)0x01U) << DEV_INTMASK_REG_SUSPEND_SHIFT)
#define DEV_INTMASK_REG_SOF_MASK                (((UINT32)0x01U) << DEV_INTMASK_REG_SOF_SHIFT)
#define DEV_INTMASK_REG_ENUM_DONE_MASK          (((UINT32)0x01U) << DEV_INTMASK_REG_ENUM_DONE_SHIFT)
#define DEV_INTMASK_REG_REMOTE_WAKEUP_MASK      (((UINT32)0x01U) << DEV_INTMASK_REG_REMOTE_WAKEUP_SHIFT)

/*
 * USB: Endpoint Interrupt Status Register.
 */

/*
 * USB: Endpoint Interrupt Mask Register.
 */


/*
 * USB: Test Mode Register.
 */

#define DEV_TEST_REG_MODE_SHIFT        (0U)

#define DEV_TEST_REG_MODE_MASK         (1U << DEV_TEST_REG_MODE_SHIFT)

/*
 * USB: UDC-2O Endpoint Register.
 */

#define UDC20_EP_REG_LOGIC_ID_SHIFT   ((UINT32)(0U))
#define UDC20_EP_REG_DIRECTION_SHIFT  ((UINT32)(4U))
#define UDC20_EP_REG_TYPE_SHIFT       ((UINT32)(5U))
#define UDC20_EP_REG_CONFIG_ID_SHIFT  ((UINT32)(7U))
#define UDC20_EP_REG_INTF_ID_SHIFT    ((UINT32)(11U))
#define UDC20_EP_REG_ALT_ID_SHIFT     ((UINT32)(15U))
#define UDC20_EP_REG_MPS_SHIFT        ((UINT32)(19U))

#define UDC20_EP_REG_LOGIC_ID_MASK    (((UINT32)0x000FU) /*<< UDC20_EP_REG_LOGIC_ID_SHIFT*/)
#define UDC20_EP_REG_DIRECTION_MASK   (((UINT32)0X0001U) << UDC20_EP_REG_DIRECTION_SHIFT)
#define UDC20_EP_REG_TYPE_MASK        (((UINT32)0X0003U) << UDC20_EP_REG_TYPE_SHIFT)
#define UDC20_EP_REG_CONFIG_ID_MASK   (((UINT32)0X000FU) << UDC20_EP_REG_CONFIG_ID_SHIFT)
#define UDC20_EP_REG_INTF_ID_MASK     (((UINT32)0X000FU) << UDC20_EP_REG_INTF_ID_SHIFT)
#define UDC20_EP_REG_ALT_ID_MASK      (((UINT32)0X000FU) << UDC20_EP_REG_ALT_ID_SHIFT)
#define UDC20_EP_REG_MPS_MASK         (((UINT32)0X07FFU) << UDC20_EP_REG_MPS_SHIFT)

/*
 * USB: IN endpoint reg definition
 */
typedef struct {
    volatile UINT32                                 EndpointCtrl;           /* 0x000+0x20*N: Endpoint N Control Register */
    volatile UINT32                                 EndpointStatus;         /* 0x004+0x20*N: Endpoint N Status Register */
    volatile UINT32                                 EndpointBufSize;        /* 0x008+0x20*N: Endpoint N Buffer Size Register */
    volatile UINT32                                 EndpointMaxPktSize;     /* 0x00C+0x20*N: Endpoint N Maximum Packet Size Register */
    volatile UINT32                                 Reserved;
    volatile UINT32                                 EndpointDataDesc;       /* 0x014+0x20*N: Endpoint N Data Descriptor Pointer Register */
    volatile UINT32                                 Reserved1;
    volatile UINT32                                 EndpointWriteConfirm;   /* 0x01C+0x20*N: Endpoint N Write Confirmation (for Slave mode) Register */
} AMBA_USB_ENDPOINT_IN_REG_s;

/*
 * USB: OUT endpoint reg definition
 */
typedef struct {
    volatile UINT32                                 EndpointCtrl;           /* 0x200+0x20*N: Endpoint N Control Register */
    volatile UINT32                                 EndpointStatus;         /* 0x204+0x20*N: Endpoint N Status Register */
    volatile UINT32                                 EndpointPktFrameNum;    /* 0x208+0x20*N: Endpoint N Packet Frame Number Register */
    volatile UINT32                                 EndpointMaxPktSize;     /* 0x20C+0x20*N: Endpoint N Buffer Size/Maximum Packet Size Register */
    volatile UINT32                                 EndpointSetupDesc;      /* 0x210+0x20*N: Endpoint N Setup Buffer Pointer Register (only for control endpoint) */
    volatile UINT32                                 EndpointDataDesc;       /* 0x214+0x20*N: Endpoint N Data Descriptor Pointer Register */
    volatile UINT32                                 Reserved;
    volatile UINT32                                 EndpointReadConfirm;    /* 0x21C+0x20*N: Endpoint N Read Confirmation (for zero-length data) Register */
} AMBA_USB_ENDPOINT_OUT_REG_s;

/*
 * USB: All Registers
 */
typedef struct {
    AMBA_USB_ENDPOINT_IN_REG_s                      EndpointIn[6];      /* 0x000-0x0BC: IN Endpoint-Specific Registers */
    volatile UINT32                                 Reserved0[80];      /* 0x0C0-0x1FC */
    AMBA_USB_ENDPOINT_OUT_REG_s                     EndpointOut[6];     /* 0x200-0x2BC: OUT Endpoint-Specific Registers */
    volatile UINT32                                 Reserved1[80];      /* 0x2C0-0x3FC */
    volatile UINT32                                 DevConfig;          /* 0x400(RW): Device Configuration Register */
    volatile UINT32                                 DevCtrl;            /* 0x404(RW): Device Control Register */
    volatile UINT32                                 DevStatus;          /* 0x408(RO): Device Status Register */
    volatile UINT32                                 DevIntStatus;       /* 0x40c(RW): Device Interrupt Status Register */
    volatile UINT32                                 DevIntMask;         /* 0x410(RW): Device Interrupt Mask Register */
    volatile UINT32                                 EndpointIntStatus;  /* 0x414(RW): Endpoint Interrupt Status Register */
    volatile UINT32                                 EndpointIntMask;    /* 0x418(RW): Endpoint Interrupt Mask Register */
    volatile UINT32                                 TestMode;           /* 0x41C(RW): Test Mode */
    volatile UINT32                                 Reserved2[56];      /* 0x420-0x4FC */
    volatile UINT32                                 Reserved3;          /* 0x500: UDC20[0], reserved for EP0 */
    volatile UINT32                                 Udc20Endpoint[188]; /* 0x504-0x7FC UDC20[1~] */
} AMBA_USB_REG_s;

#endif /* _AMBA_REG_USB_H_ */
