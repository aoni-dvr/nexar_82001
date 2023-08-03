/**
 *  @file AmbaPCIE_Struct.h
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details PCIe structure header file.
 */
#ifndef AMBAPCIE_STRUCT_H
#define AMBAPCIE_STRUCT_H


/** @defgroup ConfigInfo  Configuration and Hardware Operation Information
 *  The following definitions specify the driver operation environment that
 *  is defined by hardware configuration or client code. These defines are
 *  located in the header file of the core driver.
 *  @{
 */

/**********************************************************************
* Defines
**********************************************************************/
/** Number of AXI regions (excluding Region-0) */
#define PCIE_AXI_NB_REGIONS 8U

/**
 * The maximum number of regions available
 * when using the AXI Interface
 */
#define PCIE_MAX_AXI_WRAPPER_REGION_NUM 8U

/** Max ECAM bus number */
#define PCIE_ECAM_MAX_BUS 255U

/** Max ECAM device number */
#define PCIE_ECAM_MAX_DEV 31U

/** Max ECAM function number */
#define PCIE_ECAM_MAX_FUNC 7U

/** Max ECAM offset */
#define PCIE_ECAM_MAX_OFFSET 4095U

/** Max ECAM function number for ARI supported devices. */
#define PCIE_ARI_ECAM_MAX_FUNC 255U

/**
 * Defines the minimum number of address bits passed down from the
 * AXI Interface
 */
#define PCIE_MIN_AXI_ADDR_BITS_PASSED 8U

/**
 * Defines the default local vendor and device id for PFO. If the Device ID is
 * changed in RTL during hardware configuration, this define should be updated
 * to match, or the probe() API will fail.  The Device ID may also be set at
 * driver intialization time for each Function, in which case this define should
 * be left unchanged.
 */
#define PCIE_DEFAULT_VENDOR_DEVICE_ID 0x10017cdU

/**
 * Defines the maximum number of BARS available in functions (both Physical
 * and Virtual).
 */
#define PCIE_MAX_BARS_IN_FUNC 6U

/** Defines the maximum number of BARs in the RP */
#define PCIE_MAX_BARS_IN_RP 3U

/** Number of uDMA channels configured in the core */
#define PCIE_NUM_UDMA_CHANNELS 4U

/** Number of uDMA partitions configured in the core */
#define PCIE_NUM_UDMA_PARTITIONS 4U

/** uDMA buffer size, maximum size of scatter/gather transfers */
#define PCIE_UDMA_BUFFER_DEPTH 4096U

/** Number of uDMA max descriptor slots */
#define PCIE_MAX_DESC_SLOTS 5U

/** Number of uDMA max dars */
#define PCIE_MAX_DARS 5U

/** Number of PCI physical function supported. */
#define PCIE_NUM_PCI_FUNCTIONS 1U

/** Total number of Traffic class supported. */
#define PCIE_MAX_TRAFFIC_CLASS_NUM 8U

/** The maximum number of Lanes available */
#define PCIE_LANE_COUNT 4U


/**********************************************************************
* Enumerations
**********************************************************************/
/** Enumeration of the LTSSM (link training and status state machine) States */
typedef enum {
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Transmitter is in an Electrical Idle state */
    PCIE_DETECT_QUIET = 0U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Transmitter performs Receive Detection of unconfigured lanes */
    PCIE_DETECT_ACTIVE = 1U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Transmitter sends TS1 Ordered Sets on all lanes that Detected a Receiver */
    PCIE_POLLING_ACTIVE = 2U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Transmitter sends Compliance patterns at certain data-rates and de-emphasis levels */
    PCIE_POLLING_COMPLIANCE = 3U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Transmitter sends Configuration information in TS2 Ordered Sets */
    PCIE_POLLING_CONFIGURATION = 4U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_CONFIGURATION_LINKWIDTH_START = 5U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_CONFIGURATION_LINKWIDTH_ACCEPT = 6U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_CONFIGURATION_LANENUM_ACCEPT = 7U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_CONFIGURATION_LANENUM_WAIT = 8U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_CONFIGURATION_COMPLETE = 9U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Link fully configured, bit-lock and symbol-lock achieved */
    PCIE_CONFIGURATION_IDLE = 10U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_RCVRLOCK = 11U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_SPEED = 12U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_RCVRCFG = 13U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_IDLE = 14U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 Normal operational state - full ON state */
    PCIE_L0 = 16U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 Electrical idle state */
    PCIE_RX_L0S_ENTRY = 17U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RX_L0S_IDLE = 18U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RX_L0S_FTS = 19U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_TX_L0S_ENTRY = 20U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_TX_L0S_IDLE = 21U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_TX_L0S_FTS = 22U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Transmitter sends an Electrical Idle Ordered Set. Links enters Electrical Idle */
    PCIE_L1_ENTRY = 23U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_L1_IDLE = 24U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Receiver in Low impedance state.Transmitter in Electrical Idle for a min of 50UI */
    PCIE_L2_IDLE = 25U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Transmit Beacon signal in direction of Root Port */
    PCIE_L2_TRANSMITWAKE = 26U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Link is off and no DC common mode voltage driven */
    PCIE_DISABLED = 32U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details Loopback master continuously transmits TS1 ordered sets */
    PCIE_LOOPBACK_ENTRY_MASTER = 33U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_LOOPBACK_ACTIVE_MASTER = 34U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_LOOPBACK_EXIT_MASTER = 35U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_LOOPBACK_ENTRY_SLAVE = 36U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_LOOPBACK_ACTIVE_SLAVE = 37U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_LOOPBACK_EXIT_SLAVE = 38U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 @details TS1s with Hot Reset bit set are exchanged */
    PCIE_HOT_RESET = 39U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_EQ_PHASE_0 = 40U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_EQ_PHASE_1 = 41U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_EQ_PHASE_2 = 42U,
    /** @details Refer to PCI EXPRESS BASE SPECIFICATION, REV. 3.0 */
    PCIE_RECOVERY_EQ_PHASE_3 = 43U
} PCIE_LtssmState;

/** Debug Mux Module Select */
typedef enum {
    /** @brief Select Mux for debug of Physical Layer 0 LTSSM */
    PCIE_DEBUG_PHY_LAYER_LTSSM_0 = 0U,
    /** @brief Select Mux for debug of Physical Layer 1 LTSSM */
    PCIE_DEBUG_PHY_LAYER_LTSSM_1 = 1U,
    /** @brief Select Mux for debug of TX DLL */
    PCIE_DEBUG_DATA_LINK_LAYER_TX_SIDE = 4U,
    /** @brief Select Mux for debug of RX DLL */
    PCIE_DEBUG_DATA_LINK_LAYER_RX_SIDE = 5U,
    /** @brief Select Mux for debug of TX TL */
    PCIE_DEBUG_TCTN_LAYER_TX_SIDE = 8U,
    /** @brief Select Mux for debug of RX 0 TL */
    PCIE_DEBUG_TCTN_LAYER_RX_SIDE_0 = 9U,
    /** @brief Select Mux for debug of RX 1 TL */
    PCIE_DEBUG_TCTN_LAYER_RX_SIDE_1 = 10U
} PCIE_DebugMuxModuleSelect;

/** Enable or Disable */
typedef enum {
    /** Disable the associated parameter */
    PCIE_DISABLE_PARAM = 0U,
    /** Enable the associated parameter */
    PCIE_ENABLE_PARAM = 1U
} PCIE_EnableOrDisable;

/** Mask or Unmask */
typedef enum {
    /** Mask the associated parameter */
    PCIE_MASK_PARAM = 0U,
    /** Unmask the associated parameter */
    PCIE_UNMASK_PARAM = 1U
} PCIE_MaskOrUnmask;

/** Receive or Transmit */
typedef enum {
    /** Receive information */
    PCIE_RECEIVE = 0U,
    /** Transmit information */
    PCIE_TRANSMIT = 1U
} PCIE_TransmitOrReceive;

/** Read or Write */
typedef enum {
    /** Read information */
    PCIE_DO_READ = 0U,
    /** Write information */
    PCIE_DO_WRITE = 1U
} PCIE_ReadOrWrite;

/** RP or EP - For a dual mode core the strapping options below indicate how the core has been strapped, or for a single mode core how it is configured. */
typedef enum {
    /** The core is strapped as an EP */
    PCIE_CORE_EP_STRAPPED = 0U,
    /** The core is strapped as an RP */
    PCIE_CORE_RP_STRAPPED = 1U
} PCIE_CoreEpOrRp;

/** Boolean true or false */
typedef enum {
    /** Boolean false */
    PCIE_FALSE = 0U,
    /** Boolean true */
    PCIE_TRUE = 1U
} PCIE_Bool;

/** Receive or Transmit Side */
typedef enum {
    /** RX side for transfers */
    PCIE_RECEIVE_SIDE = 0U,
    /** TX side for transfers */
    PCIE_TRANSMIT_SIDE = 1U
} PCIE_TransmitOrReceiveSide;

/** Upstream or Downstream */
typedef enum {
    /** Upstream information flow */
    PCIE_UPSTREAM = 0U,
    /** Downstream information flow */
    PCIE_DOWNSTREAM = 1U
} PCIE_UpstreamOrDownstream;

/** Wait For Action */
typedef enum {
    /** Wait for some event to occur */
    PCIE_WAIT = 0U,
    /** Do not wait for some event to occur */
    PCIE_DO_NOT_WAIT = 1U
} PCIE_WaitOrNot;

/** Snoop */
typedef enum {
    /** Snoop */
    PCIE_SNOOP = 0U,
    /** No Snoop */
    PCIE_NO_SNOOP = 1U
} PCIE_SnoopOrNoSnoop;

/** Wait For Completion */
typedef enum {
    /** Wait for completion of operation */
    PCIE_WAIT_TILL_COMPLETE = 0U,
    /** Don't Wait for completion of operation */
    PCIE_NO_WAIT_TILL_COMPLETE = 1U
} PCIE_WaitOrNoWaitForComplete;

/** Vendor or Subsystem Vendor IDs */
typedef enum {
    /** Id type is Vendor ID */
    PCIE_VENDOR_ID = 0U,
    /** Id type is Subsystem Vendor ID */
    PCIE_SUBSYSTEM_VENDOR_ID = 1U
} PCIE_AdvertisedIdType;

/** Message Types */
typedef enum {
    /**
     * @details LTR messages can be sent only when the core is configured in the EndPoint mode. All the other
     * messages can be sent only when the core is configured as a Root Port.
     */
    PCIE_LTR_MESSAGE = 0U
} PCIE_MessageType;

/** Auto Message Types */
typedef enum {
    /**
     * @details Transmit message on Function Power State change.
     * When the sending of this type of message is enabled, the core will automatically transmit an LTR message when all the Functions in the core have transitioned to a non-D0 power state, provided that the following conditions are both true:
     * 1. The core sent at least one LTR message since the Data Link layer last transitioned from down to up state.
     * 2. The most recent LTR message transmitted by the core had as least one of the Requirement bits set.
     * The core will set the Requirement bits in this LTR message to 0.
     * When the sending of this type of message is disabled, the core will not, by itself, send any
     * LTR messages in response to Function Power State changes. (Client logic may monitor the
     * FUNCTION_POWER_STATE outputs of the core and transmit LTR messages through the HAL master interface, in response to changes in their states - Refer to the Cadence PCIe Core User Guide)
     */
    PCIE_LTR_MESSAGE_ON_FUNCTION_POWER_STATE_CHANGE = 0U,
    /**
     * @details When the sending of this type of message is enabled, the core will also transmit an LTR
     * message whenever the LTR Mechanism Enable bit is cleared, if the following conditions are both true:
     * 1. The core sent at least one LTR message since the LTR Mechanism Enable bit was last set.
     * 2. The most recent LTR message transmitted by the core had as least one of the
     * Requirement bits set. The core will set the Requirement bits in this LTR message to 0. When the sending of this type of message is disabled, the core will not, by itself, send any LTR messages in response to state changes of the LTR Mechanism Enable bit. (Client logic may monitor the state of the LTR_MECHANISM_ENABLE output of the core and transmit LTR messages through the HAL master interface, in response to its state changes - Refer to the Cadence PCIe Core User Guide)
     */
    PCIE_LTR_MESSAGE_ON_LTR_MECHANISM_ENABLE = 1U
} PCIE_AutoMessageType;

/** PCIe Lane Count */
typedef enum {
    /** 1 Lane */
    PCIE_LANECNT_X1 = 0U,
    /** 2 Lanes */
    PCIE_LANECNT_X2 = 1U,
    /** 4 Lanes */
    PCIE_LANECNT_X4 = 2U
} PCIE_LaneCount;

/** Link Speed in terms of GT */
typedef enum {
    /** Speed is 2.5GT */
    PCIE_SPEED_2P5_GB = 1U,
    /** Speed is 5GT */
    PCIE_SPEED_5_GB = 2U,
    /** Speed is 8GT */
    PCIE_SPEED_8_GB = 3U
} PCIE_LinkSpeed;

/** BAR Numbers */
typedef enum {
    /**
     * @details Base Address Register 0. Used to define the
     * Function memory and I/O address ranges.
     */
    PCIE_BAR_0 = 0U,
    /**
     * @details Base Address Register 1. Used to define the
     * Function memory and I/O address ranges.
     */
    PCIE_BAR_1 = 1U,
    /**
     * @details Base Address Register 2. Used to define the
     * Function memory and I/O address ranges.
     */
    PCIE_BAR_2 = 2U,
    /**
     * @details Base Address Register 3. Used to define the
     * Function memory and I/O address ranges.
     */
    PCIE_BAR_3 = 3U,
    /**
     * @details Base Address Register 4. Used to define the
     * Function memory and I/O address ranges.
     */
    PCIE_BAR_4 = 4U,
    /**
     * @details Base Address Register 5. Used to define the
     * Function memory and I/O address ranges.
     */
    PCIE_BAR_5 = 5U
} PCIE_BarNumber;

/** Debug Controllable Parameters */
typedef enum {
    /**
     * @details When this flag is enabled, the AXI bridge places a write request on the HAL Master interface
     * in preference over a read request if both AXI write and AXI read requests are available to be
     * asserted on the same clock cycle.
     */
    PCIE_AXI_BRIDGE_WRITE_PRIORITY = 0U,
    /**
     * @details Disabling the Sync Header Framing Error flag disables flagging of framing error and subsequent
     * retraining of the link in the case where core receives a illegal value on the sync header. Enabling
     * the flag will enable flagging of framing errors and subsequent retraining of the link.
     * This flag is ENABLED by default.
     */
    PCIE_SYNC_HEADER_FRAMING_ERROR = 1U,
    /**
     * @details When Link Re-training on Framing Error is disabled, the core will not transition its LTSSM
     * into the Recovery state when it detects a Framing Error at 8 GT/s speed (as defined in Section 4.2.2.3.3
     * of the PCIe Base Specification 3.0. This flag must normally be enabled so that a Framing Error will
     * cause the LTSSM to enter Recovery.  The setting of this flag has no effect on the operation of the core at
     * 2.5 and 5 GT/s speeds.
     * This flag is ENABLED by default.
     */
    PCIE_LINK_RETRAINING_ON_FRAMING_ERROR = 2U,
    /**
     * @details When this field is enabled, it indicates that the remote node advertised Linkwidth Upconfigure
     * Capability in the training sequences in the Configuration Complete state when the link came
     * up. When disabled, it indicates that the remote node did not set the Link Upconfigure bit.
     * The user may set Disable or Enable Link Upconfigure Capability  to turn on or off the link upconfigure
     * capability of the core. Disabling this flag prevents the core from advertising the link
     * upconfigure capability in training sequences transmitted in the Configuration.
     * This flag is ENABLED by default.
     */
    PCIE_LINK_UPCONFIGURE_CAPABILITY = 3U,
    /**
     * @details Enabling Fast Link Training is provided to shorten the link training time to facilitate fast
     * simulation of the design, especially at the gate level.  Enabling this flag has the following effects:
     * 1. The 1 ms, 2 ms, 12 ms, 24 ms, 32 ms and 48 ms timeout intervals in the LTSSM are shortened by a factor of 500.
     * 2. In the Polling.Active state of the LTSSM, only 16 training sequences are required to be
     * transmitted (Instead of 1024) to make the transition to the Configuration state.
     * This flag should not be enabled during normal operation of the core.
     * This flag is DISABLED by default.
     */
    PCIE_FAST_LINK_TRAINING = 4U,
    /**
     * @details When Slot Power Capture is enabled, the core will capture the Slot Power Limit Value and
     * Slot Power Limit Scale parameters from a Set_Slot_Power_Limit message received in the Device
     * Capabilities Register. When this flag is disabled, the capture is disabled.  This bit is valid only when
     * the core is configured as an EndPoint. It has not effect when the core is a Root Port.
     * This flag is DISABLED by default.
     */
    PCIE_SLOT_POWER_CAPTURE = 5U,
    /**
     * @details Disabling Gen3 LFSR Update from SKP disables the update of the LFSRs in the Gen3 descramblers of the
     * core, from the values received in SKP sequences.  This flag should be enabled during normal operation, but
     * may be disabled for testing.
     * This flag is ENABLED by default.
     */
    PCIE_GEN3_LFSR_UPDATE_FROM_SKP = 7U,
    /**
     * @details Disabling Electrical Idle Infer in L0 State disables the inferring of electrical idle in the L0 state.
     * Electrical idle is inferred when no flow control updates and no SKP sequences are received
     * within an interval of 128 us.  This flag should be enabled during normal operation, but may be
     * disabled for testing.
     * This flag is ENABLED by default.
     */
    PCIE_ELECTRICAL_IDLE_INFER_IN_L0_STATE = 8U,
    /**
     * @details Disabling Flow Control Update Timeout will allow the core to time out and re-train the link when no
     * Flow Control Update DLLPs are received from the link within an interval of 128 us. Disabling this flag
     * disables this timeout. When the advertised receive credit of the link partner is infinity for the header
     * and payload of all credit types, this timeout is always suppressed. The setting of this flag has no
     * effect in this case.  This flag should be enabled during normal operation, but may be disabled
     * for testing.
     * This flag is ENABLED by default.
     */
    PCIE_FLOW_CONTROL_UPDATE_TIMEOUT = 9U,
    /**
     * @details Disabling Ordering Checks disables the ordering check in the core between Completions and Posted requests
     * received from the link
     * This flag is ENABLED by default.
     */
    PCIE_ORDERING_CHECKS = 10U,
    /**
     * @details Setting this flag to enable causes all the enabled Functions to report an error when a Type-1
     * configuration access is received by the core, targeted at any Function. Disabling the flag limits the
     * error reporting to the type-0 Function whose number matches with the Function number specified in the
     * request.  If the Function number in the request refers to an unimplemented or disabled Function,
     * all enabled Functions report the error regardless of the setting of this bit.
     * This flag is DISABLED by default.
     */
    PCIE_FUNC_SPECIFIC_RPRT_OF_TYPE1_CONF_ACC = 11U,
    /** @details Disable ID Based ordering check */
    PCIE_ID_BASED_ORDERING_CHECKS = 13U
} PCIE_DebugParams;

/** Maskable Interrupt Condition Types */
typedef enum {
    /** @details Uncorrectable parity error detected while reading from the PNP Receive FIFO RAM */
    PCIE_PNP_RX_FIFO_PARITY_ERROR = 0U,
    /** @details Overflow occured in the PNP Receive FIFO */
    PCIE_PNP_RX_FIFO_OVERFLOW_CONDITION = 2U,
    /** @details Uncorrectable parity error detected while reading from the Completion Receive FIFO RAM */
    PCIE_COMPLETION_RX_FIFO_PARITY_ERROR = 4U,
    /** @details Uncorrectable parity error detected while reading from Replay Buffer RAM */
    PCIE_REPLAY_RAM_PARITY_ERROR = 5U,
    /** @details Overflow occured in the Receive FIFO */
    PCIE_COMPLETION_RX_FIFO_OVERFLOW_CONDITION = 6U,
    /** @details Replay timer timed out */
    PCIE_REPLAY_TIMEOUT_CONDITION = 7U,
    /** @details Replay timer rolled over after 4 transmissions of the same TLP */
    PCIE_REPLAY_TIMEOUT_ROLLOVER_CONDITION = 8U,
    /** @details Phy error detected on receive side */
    PCIE_PHY_ERROR = 9U,
    /** @details Malformed TLP received from the link */
    PCIE_MALFORMED_TLP_RECEIVED = 10U,
    /** @details Unexpected completion received from the link */
    PCIE_UNEXPECTED_COMPLETION_RECEIVED = 11U,
    /** @details An error was observed in the flow control advertisements from the other side */
    PCIE_FLOW_CONTROL_ERROR = 12U,
    /** @details A request timed out waiting for completion */
    PCIE_COMPLETION_TIMEOUT_CONDITION = 13U,
    /** @details DMA parity error on outbound buffer RAM  */
    PCIE_DMA_OB_ERROR = 16U,
    /** @details DMA parity error on inbound buffer RAM  */
    PCIE_DMA_IB_ERROR = 17U,
    /** @details MSI Mask value change  */
    PCIE_MSI_CHG_CONDITION = 18U,
    /** @details MSIX Function Mask value change */
    PCIE_MSIX_CHG_CONDITION = 19U,
    /** @details Hardware Autonomous Width Change Disable Toggle */
    PCIE_HAWCD_CONDITION = 20U,
    /** @details AXI master read  FIFO RAM ECC uncorrectable error */
    PCIE_AXIM_ECC_READ_ERROR = 21U,
    /** @details AXI slave write FIFO RAM ECC uncorrectable error */
    PCIE_AXIS_ECC_WRITE_ERROR = 22U,
    /** @details AXI slave reorder FIFO RAM ECC uncorrectable error */
    PCIE_REORDER_ERROR = 23U
} PCIE_MaskableLocalInterruptConditions;

/** Count Registers that save count values */
typedef enum {
    /**
     * @details The value referred to by this type contains the number of Transaction-Layer Packets received by
     * the core from the link since the value was last reset. This counter saturates on reaching a count
     * of all 1s.
     */
    PCIE_RECEIVE_TLP_COUNT = 0U,
    /**
     * @details The value referred to by this type contains the aggregate number of payload double-words received
     * in Transaction-Layer Packets by the core from the link since the value was last reset. This
     * counter saturates on reaching a count of all 1s.
     */
    PCIE_RECEIVE_TLP_PAYLOAD_DWORD_COUNT = 1U,
    /**
     * @details The value referred to by this type contains the number of Transaction-Layer Packets transmitted
     * by the core on the link since the value was last reset. This counter saturates on reaching a
     * count of all 1s.
     */
    PCIE_TRANSMIT_TLP_COUNT = 2U,
    /**
     * @details The value referred to by this type contains the aggregate number of payload double-words
     * transmitted in Transaction-Layer Packets by the core on the link since the value was last reset.
     * This counter saturates on reaching a count of all 1s.
     */
    PCIE_TRANSMIT_TLP_PAYLOAD_DWORD_COUNT = 3U,
    /**
     * @details Number of TLPs received with LCRC errors. This value referred to by this type contains the count
     * of the number of TLPs received by the core with LCRC errors in them. This is a 16-bit saturating
     * counter.
     */
    PCIE_TLP_LCRC_ERROR_COUNT = 4U,
    /**
     * @details Number of correctable errors detected while reading from the PNP FIFO RAM.  This is an 8-bit saturating
     * counter that can be cleared by writing all 1s into it.
     */
    PCIE_ECC_CORR_ERR_COUNT_PNP_FIFO_RAM = 5U,
    /**
     * @details Number of correctable errors detected while reading from the SC FIFO RAM.  This is an 8-bit saturating
     * counter that can be cleared by writing all 1s into it.
     */
    PCIE_ECC_CORR_ERR_COUNT_SC_FIFO_RAM = 6U,
    /**
     * @details Number of correctable errors detected while reading from the Replay Buffer RAM. This is an 8-bit
     * saturating counter that can be cleared by writing all 1s into it.
     */
    PCIE_ECC_CORR_ERR_COUNT_REPLAY_RAM = 7U,
    /**
     * @details FTS count received from the other side during link training for use at the 2.5 GT/s link speed.
     * The core transmits this many FTS sequences while exiting the L0S state, when operating at the
     * 2.5 GT/s speed.  These values determine the number of Fast Training Sequences transmitted by the
     * core when it exits the L0s link power state.
     */
    PCIE_RECEIVED_FTS_COUNT_FOR_2_5_GT_SPEED = 9U,
    /**
     * @details FTS count received from the other side during link training for use at the 5 GT/s link speed.
     * The core transmits this many FTS sequences while exiting the L0S state, when operating at the
     * 5 GT/s speed.  These values determine the number of Fast Training Sequences transmitted by the
     * core when it exits the L0s link power state.
     */
    PCIE_RECEIVED_FTS_COUNT_FOR_5_GT_SPEED = 10U,
    /**
     * @details FTS count received from the other side during link training for use at the 8 GT/s link speed.
     * The core transmits this many FTS sequences while exiting the L0S state, when operating at the
     * 8 GT/s speed.  These values determine the number of Fast Training Sequences transmitted by the
     * core when it exits the L0s link power state.
     */
    PCIE_RECEIVED_FTS_COUNT_FOR_8_GT_SPEED = 11U,
    /**
     * @details FTS count transmitted by the core in TS1/TS2 sequences during link training at the 2.5 GT/s link speed
     * This value must be set based on the time needed by the receiver to acquire sync while
     * exiting from L0S state.
     */
    PCIE_TRN_FTS_COUNT_FOR_2_5_GT_SPEED = 14U,
    /**
     * @details FTS count transmitted by the core in TS1/TS2 sequences during link training at the 5 GT/s link speed
     * This value must be set based on the time needed by the receiver to acquire sync while
     * exiting from L0S state.
     */
    PCIE_TRN_FTS_COUNT_FOR_5_GT_SPEED = 15U,
    /**
     * @details FTS count transmitted by the core in TS1/TS2 sequences during link training at the 8 GT/s link speed
     * This value must be set based on the time needed by the receiver to acquire sync while
     * exiting from L0S state.
     */
    PCIE_TRN_FTS_COUNT_FOR_8_GT_SPEED = 16U
} PCIE_SavedCountParams;

/** Timeout and delay values */
typedef enum {
    /**
     * @details The value referred to by this type defines the timeout value for transitioning to the
     * power state. If the transmit side has been idle for this interval, the core will transmit the
     * idle sequence on the link and transition the state of the link to L0S.
     * The value referred to by this type Contains the timeout value (in units of 4 ns) for
     * transitioning to the L0S power state. (Setting it to 0 permanently disables the transition
     * to the L0S power state - refer to the Cadence PCIe Core User Guide)
     */
    PCIE_L0S_TIMEOUT = 0U,
    /**
     * @details The value referred to by this type contains the timeout value used to detect a completion
     * timeout event for a request originated by the core from its HAL master interface,
     * (measured in 4 ns cycles).
     */
    PCIE_COMPLETION_TIMEOUT_LIMIT_0 = 1U,
    /**
     * @details The value referred to by this type contains the timeout value used to detect a completion timeout
     * event for a request originated  by the core from its HAL master interface,
     * (measured in 4 ns cycles).
     */
    PCIE_COMPLETION_TIMEOUT_LIMIT_1 = 2U,
    /**
     * @details The value referred to by this type specifies the time the core will wait before it re-enters
     * the L1 state if its link partner transitions the link to L0 while all the Functions of the
     * core are in D3 power state. The core will change the power state of the link from L0 to L1 if
     * no activity is detected both on the transmit and receive sides before this interval, while all
     * Function are in D3 state and the link is in L0. Setting this value to 0 disables re-rentry to
     * L1 state if the link partner returns the link to L0 from L1 when all the Functions of
     * the core are in D3 state. The value referred to by this type controls only the re-entry to L1.
     * The initial transition to L1 always occurs when the all the Functions of the core are set to
     * the D3 state. Delay to re-enter L1 after no activity is specified in units of 4 ns cycles.
     */
    PCIE_L1_STATE_RETRY_DELAY = 3U,
    /**
     * @details The value referred to by this type defines the timeout value for transitioning to the L1 power
     * state under Active State Power management. If the transmit side has been idle for this
     * interval, the core will initiate a transition of its link to the L1 power state. Contains the
     * timeout value (in units of 4 ns) for transitioning to the L1 power state. Setting it to 0
     * permanently disables the transition to the L1 power state.
     */
    PCIE_ASPM_L1_ENTRY_TIMEOUT_DELAY = 4U,
    /**
     * @details The value referred to by this type defines the delay between the core receiving a
     * PME_Turn_Off message from the link and generating a PME_TO_Ack message in response to it.
     * The core generates the ack only when all its Functions are in low-power states (D1 or D3).
     * Time in microseconds between the core receiving a PME_Turn_Off message TLP and sending a PME_TO_Ack
     * response to it. This field must be set to a non-zero value to enable the core to send the response.
     * Setting this field to 0 suppresses the cores response to the PME_Turn_Off message, so that
     * the client may transmit the PME_TO_Ack message through the HAL master interface.
     */
    PCIE_PME_TURNOFF_ACK_DELAY = 5U,
    /**
     * @details Additional transmit-side replay timer timeout interval. This 9-bit value is added as a signed 2s
     * complement number to the internal replay timer timeout value computed by the core based on the
     * PCI Express Specifications. This enables the user to make minor adjustments to the spec-defined
     * replay timer settings. Its value is in multiples of 4 ns (maximum = +1020 ns, minimum = 1024 ns).
     */
    PCIE_TRANSMIT_SIDE_REPLAY_TIMEOUT = 6U,
    /**
     * @details Additional receive side ACK-NAK timer timeout interval. This 9-bit value is added as a
     * signed 2s complement number to the internal ACK-NAK timer timeout value computed by the core
     * based on the PCI Express Specifications.
     */
    PCIE_RECEIVE_SIDE_ACK_NACK_REPLAY_TIMEOUT = 7U,
    /**
     * @details Timeout delay parameter for the service timeout mechanism associated with the generation of
     * PM_PME messages. In EP mode, the core will transmit a PM_PME message after the expiration of
     * this delay, if the RP did not clear the PME status bit in the Power Management Control and
     * Status register. This delay parameter is only applicable to EP and not to RP.
     */
    PCIE_PME_SERVICE_TIMEOUT_DELAY = 8U,
    /**
     * @details No-Snoop Latency Value should be programmed with the value to be sent in the No-Snoop
     * Latency Value field of the LTR message.
     */
    PCIE_NO_SNOOP_LATENCY_VALUE = 9U,
    /**
     * No-Snoop Latency Scale should be programmed with the value to be sent in the No-Snoop
     * Latency Scale field of the LTR message.
     */
    PCIE_NO_SNOOP_LATENCY_SCALE = 10U,
    /**
     * @details Snoop Latency Value should be programmed with the value to be sent in the Snoop
     * Latency Value field of the LTR message.
     */
    PCIE_SNOOP_LATENCY_VALUE = 11U,
    /**
     * @details Snoop Latency Scale should be programmed with the value to be sent in the Snoop
     * Latency Scale field of the LTR message.
     */
    PCIE_SNOOP_LATENCY_SCALE = 12U,
    /** @details Link Down Indication Auto Reset timer Control Value should be programmed with the timeout value to be used for LDTIMER. */
    PCIE_LDTIMER_TIMEOUT = 13U
} PCIE_TimeoutAndDelayValues;

/** Credit Limit Registers */
typedef enum {
    /** @details Posted payload credit limit advertised by the core for VC 0. (in units of 4 Dwords). */
    PCIE_POSTED_PAYLOAD_CREDIT = 0U,
    /** @details Posted payload credit limit advertised by the core for VC 0 (in number of packets). */
    PCIE_POSTED_HEADER_CREDIT = 1U,
    /** @details Non-Posted payload credit limit advertised by the core for VC 0 (in units of 4 Dwords). */
    PCIE_NONPOSTED_PAYLOAD_CREDIT = 2U,
    /** @details Non-Posted Header Credit limit advertised by the core for VC 0 (in number of packets). */
    PCIE_NONPOSTED_HEADER_CREDIT = 3U,
    /** @details Completion payload credit limit advertised by the core for VC 0 (in units of 4 Dwords). */
    PCIE_COMPLETION_PAYLOAD_CREDIT = 4U,
    /** @details Completion Header Credit limit advertised by the core for VC 0 (in number of packets). */
    PCIE_COMPLETION_HEADER_CREDIT = 5U
} PCIE_CreditLimitTypes;

/** Credit Update Intervals */
typedef enum {
    /**
     * @details Minimum Posted credit update interval for posted transactions.
     * The core follows this minimum interval between issuing posted credit updates on the link.
     * This is to limit the bandwidth use of credit updates. If new credit becomes available in
     * the receive FIFO since the last update was sent, the core will issue a new update only
     * after this interval has elapsed since the last update. The value is in units of 4 ns.
     * This field is re-written by the internal logic when the negotiated link width or link
     * speed changes, to correspond to the default values. The user may override this default
     * value by writing into the value referred to by this type. The value written will be lost
     * on a change in the negotiated link width/speed.
     */
    PCIE_MIN_POSTED_CREDIT_UPDATE_INTERVAL = 0U,
    /**
     * @details Minimum credit update interval for non-posted transactions. The core follows this minimum
     * interval between issuing posted credit updates on the link. This is to limit the bandwidth
     * use of credit updates. If new credit becomes available in the receive FIFO since the last
     * update was sent, the core will issue a new update only after this interval has elapsed
     * since the last update. The value is in units of 4ns.
     */
    PCIE_MIN_NONPOSTED_CREDIT_UPDATE_INTERVAL = 1U,
    /**
     * @details Minimum credit update interval for Completion packets.
     * The core follows this minimum interval between issuing posted credit updates on the link.
     * This is to limit the bandwidth use of credit updates. If new credit becomes available in
     * the receive FIFO since the last update was sent, the core will issue a new update only
     * after this interval has elapsed since the last update. The value is in units of 4 ns.
     * This parameter is not used when the Completion credit is infinity.
     */
    PCIE_MIN_COMPLETION_UPDATE_INTERVAL = 2U,
    /**
     * Maximum credit update interval for all transactions. If no new credit has become
     * available since the last update, the core will repeat the last update after this interval.
     * This is to recover from any losses of credit update packets. The value is in units of 4 ns.
     * This field could be re-written by the internal logic when the negotiated link width or
     * link speed changes, to correspond to the default values for all link widths and speeds.
     * The user may override this default value by writing into value referred to by this
     * type field. The value written will be lost on a change in the negotiated link width/speed.
     */
    PCIE_MAX_UPDATE_INTERVAL_FOR_ALL = 3U
} PCIE_CreditUpdateIntervals;

/** BAR Aperture Coding */
typedef enum {
    /** Not available size */
    PCIE_APERTURE_SIZE_NA = 0U,
    /** BAR Aperture size of 4K */
    PCIE_APERTURE_SIZE_4K = 5U,
    /** BAR Aperture size of 8K */
    PCIE_APERTURE_SIZE_8K = 6U,
    /** BAR Aperture size of 16K */
    PCIE_APERTURE_SIZE_16K = 7U,
    /** BAR Aperture size of 32K */
    PCIE_APERTURE_SIZE_32K = 8U,
    /** BAR Aperture size of 64K */
    PCIE_APERTURE_SIZE_64K = 9U,
    /** BAR Aperture size of 128K */
    PCIE_APERTURE_SIZE_128K = 10U,
    /** BAR Aperture size of 256K */
    PCIE_APERTURE_SIZE_256K = 11U,
    /** BAR Aperture size of 512K */
    PCIE_APERTURE_SIZE_512K = 12U,
    /** BAR Aperture size of 1M */
    PCIE_APERTURE_SIZE_1M = 13U,
    /** BAR Aperture size of 2M */
    PCIE_APERTURE_SIZE_2M = 14U,
    /** BAR Aperture size of 4M */
    PCIE_APERTURE_SIZE_4M = 15U,
    /** BAR Aperture size of 8M */
    PCIE_APERTURE_SIZE_8M = 16U,
    /** BAR Aperture size of 16M */
    PCIE_APERTURE_SIZE_16M = 17U,
    /** BAR Aperture size of 32M */
    PCIE_APERTURE_SIZE_32M = 18U,
    /** BAR Aperture size of 64M */
    PCIE_APERTURE_SIZE_64M = 19U,
    /** BAR Aperture size of 128M */
    PCIE_APERTURE_SIZE_128M = 20U,
    /** BAR Aperture size of 256M */
    PCIE_APERTURE_SIZE_256M = 21U,
    /** BAR Aperture size of 512M */
    PCIE_APERTURE_SIZE_512M = 22U,
    /** BAR Aperture size of 1G */
    PCIE_APERTURE_SIZE_1G = 23U,
    /** BAR Aperture size of 2G */
    PCIE_APERTURE_SIZE_2G = 24U,
    /** BAR Aperture size of 4G */
    PCIE_APERTURE_SIZE_4G = 25U,
    /** BAR Aperture size of 8G */
    PCIE_APERTURE_SIZE_8G = 26U,
    /** BAR Aperture size of 16G */
    PCIE_APERTURE_SIZE_16G = 27U,
    /** BAR Aperture size of 32G */
    PCIE_APERTURE_SIZE_32G = 28U,
    /** BAR Aperture size of 64G */
    PCIE_APERTURE_SIZE_64G = 29U,
    /** BAR Aperture size of 128G */
    PCIE_APERTURE_SIZE_128G = 30U,
    /** BAR Aperture size of 256G */
    PCIE_APERTURE_SIZE_256G = 31U
} PCIE_BarApertureSize;

/** BAR Control Coding */
typedef enum {
    /** Disabled */
    PCIE_DISABLED_BAR = 0U,
    /** 32 bit IO BAR */
    PCIE_IO_32_BIT_BAR = 1U,
    /** 32 bit memory BAR, non-prefetchable */
    PCIE_NON_PREFETCHABLE_32_BIT_MEM_BAR = 4U,
    /** 32 bit memory BAR, prefetchable */
    PCIE_PREFETCHABLE_32_BIT_MEM_BAR = 5U,
    /** 64 bit memory BAR, non-prefetchable */
    PCIE_NON_PREFETCHABLE_64_BIT_MEM_BAR = 6U,
    /** 64 bit memory BAR, prefetchable */
    PCIE_PREFETCHABLE_64_BIT_MEM_BAR = 7U
} PCIE_BarControl;

/** RP BAR Numbers */
typedef enum {
    /**
     * @details Base Address Register 0. Used to define the
     * RP memory and I/O address ranges.
     */
    PCIE_RP_BAR_0 = 0U,
    /**
     * @details Base Address Register 1. Used to define the
     * RP memory and I/O address ranges.
     */
    PCIE_RP_BAR_1 = 1U
} PCIE_RpBarNumber;

/** RP BAR Aperture Coding */
typedef enum {
    /** Not available size */
    PCIE_RP_APERTURE_SIZE_NA = 0U,
    /** BAR Aperture size of 4K */
    PCIE_RP_APERTURE_SIZE_4K = 10U,
    /** BAR Aperture size of 8K */
    PCIE_RP_APERTURE_SIZE_8K = 11U,
    /** BAR Aperture size of 16K */
    PCIE_RP_APERTURE_SIZE_16K = 12U,
    /** BAR Aperture size of 32K */
    PCIE_RP_APERTURE_SIZE_32K = 13U,
    /** BAR Aperture size of 64K */
    PCIE_RP_APERTURE_SIZE_64K = 14U,
    /** BAR Aperture size of 128K */
    PCIE_RP_APERTURE_SIZE_128K = 15U,
    /** BAR Aperture size of 256K */
    PCIE_RP_APERTURE_SIZE_256K = 16U,
    /** BAR Aperture size of 512K */
    PCIE_RP_APERTURE_SIZE_512K = 17U,
    /** BAR Aperture size of 1M */
    PCIE_RP_APERTURE_SIZE_1M = 18U,
    /** BAR Aperture size of 2M */
    PCIE_RP_APERTURE_SIZE_2M = 19U,
    /** BAR Aperture size of 3M */
    PCIE_RP_APERTURE_SIZE_4M = 20U,
    /** BAR Aperture size of 4M */
    PCIE_RP_APERTURE_SIZE_8M = 21U,
    /** BAR Aperture size of 16M */
    PCIE_RP_APERTURE_SIZE_16M = 22U,
    /** BAR Aperture size of 32M */
    PCIE_RP_APERTURE_SIZE_32M = 23U,
    /** BAR Aperture size of 64M */
    PCIE_RP_APERTURE_SIZE_64M = 24U,
    /** BAR Aperture size of 128M */
    PCIE_RP_APERTURE_SIZE_128M = 25U,
    /** BAR Aperture size of 256M */
    PCIE_RP_APERTURE_SIZE_256M = 26U,
    /** BAR Aperture size of 512M */
    PCIE_RP_APERTURE_SIZE_512M = 27U,
    /** BAR Aperture size of 1G */
    PCIE_RP_APERTURE_SIZE_1G = 28U,
    /** BAR Aperture size of 2G */
    PCIE_RP_APERTURE_SIZE_2G = 29U,
    /** BAR Aperture size of 4G */
    PCIE_RP_APERTURE_SIZE_4G = 30U,
    /** BAR Aperture size of 8G */
    PCIE_RP_APERTURE_SIZE_8G = 31U,
    /** BAR Aperture size of 16G */
    PCIE_RP_APERTURE_SIZE_16G = 32U,
    /** BAR Aperture size of 32G */
    PCIE_RP_APERTURE_SIZE_32G = 33U,
    /** BAR Aperture size of 64G */
    PCIE_RP_APERTURE_SIZE_64G = 34U,
    /** BAR Aperture size of 128G */
    PCIE_RP_APERTURE_SIZE_128G = 35U,
    /** BAR Aperture size of 256G */
    PCIE_RP_APERTURE_SIZE_256G = 36U
} PCIE_RpBarApertureSize;

/** Root Port BAR Control Coding */
typedef enum {
    /** Disabled */
    PCIE_RP_DISABLED_BAR = 0U,
    /** 32 bit memory BAR */
    PCIE_RP_32_BIT_IO_BAR = 1U,
    /** 32 bit memory BAR - type 0 accesses */
    PCIE_RP_TYPE_0_32_BIT_MEM_BAR = 4U,
    /** 32 bit memory BAR - type 1 accesses */
    PCIE_RP_TYPE_1_32_BIT_MEM_BAR = 5U,
    /** 64 bit memory BAR - type 0 accesses */
    PCIE_RP_TYPE_0_64_BIT_MEM_BAR = 6U,
    /** 64 bit memory BAR - type 1 accesses */
    PCIE_RP_TYPE_1_64_BIT_MEM_BAR = 7U
} PCIE_RpBarControl;

/** Type 1 access for RP */
typedef enum {
    /** Disabled */
    PCIE_RP_T1_DIS_ALL = 0U,
    /** 32 bit prefetchable, io disabled */
    PCIE_RP_T1_PFETCH_32_BIT_IO_DIS = 1U,
    /** 64 bit prefetchable, io disabled */
    PCIE_RP_T1_PFETCH_64_BIT_IO_DIS = 2U,
    /** disabled prefetchable, 16 bit io */
    PCIE_RP_T1_PFETCH_DIS_IO_16_BIT = 3U,
    /** 32 bit prefetchable, 16 bit io */
    PCIE_RP_T1_PFETCH_32_BIT_IO_16_BIT = 4U,
    /** 64 bit prefetchable, 16 bit io */
    PCIE_RP_T1_PFETCH_64_BIT_IO_16_BIT = 5U,
    /** disabled prefetchable, 32 bit io */
    PCIE_RP_T1_PFETCH_DIS_IO_32_BIT = 6U,
    /** 32 bit prefetchable, 32 bit io */
    PCIE_RP_T1_PFETCH_32_BIT_IO_32_BIT = 7U,
    /** 64 bit prefetchable, 32 bit io */
    PCIE_RP_T1_PFETCH_64_BIT_IO_32_BIT = 8U
} PCIE_RpType1ConfigControl;

/** Data size in bytes. */
typedef enum {
    /** 1 byte (8 bits) */
    PCIE_1_BYTE = 1U,
    /** 2 byte (16 bits) */
    PCIE_2_BYTE = 2U,
    /** 4 byte (32 bits) */
    PCIE_4_BYTE = 4U
} PCIE_SizeInBytes;

/** UDMA continuity modes */
typedef enum {
    /** Read the data and write the data */
    PCIE_READ_WRITE = 0U,
    /** Only read data do not write it */
    PCIE_PREFETCH = 1U,
    /** Write previously read data */
    PCIE_POSTWRITE = 2U
} PCIE_UdmaContinuity;

/** UDMA buffer error types */
typedef enum {
    /** IB buffer, uncorrected ECC errors */
    PCIE_UNCORRECTED_ECC_IB = 0U,
    /** IB buffer, corrected ECC errors */
    PCIE_CORRECTED_ECC_IB = 1U,
    /** OB buffer, uncorrected ECC errors */
    PCIE_UNCORRECTED_ECC_OB = 2U,
    /** OB buffer, corrected ECC errors */
    PCIE_CORRECTED_ECC_OB = 3U
} PCIE_UdmaBufferErrorType;

/** state of uDMA channel */
typedef enum {
    /** Channel is free */
    PCIE_CHANNEL_FREE = 0U,
    /** Channel is busy */
    PCIE_CHANNEL_BUSY = 1U
} PCIE_channel_state;

/** state of uDMA dar */
typedef enum {
    /** Dar is free */
    PCIE_DAR_FREE = 0U,
    /** Dar is busy */
    PCIE_DAR_BUSY = 1U,
    /** Dar is attached */
    PCIE_DAR_ATTACHED = 2U
} PCIE_td_dar_state;

/** state of uDMA TD descriptor */
typedef enum {
    /** Not defined */
    PCIE_TD_UNDEFINED = 0U,
    /** TD is free */
    PCIE_TD_FREE = 1U,
    /** TD is pending */
    PCIE_TD_STATUS_PEND = 2U,
    /** TD is waiting to be programmed to HW */
    PCIE_TD_BUSY_WAITING = 3U,
    /** HW has been requested to perform transfer */
    PCIE_TD_BUSY_IN_HW = 4U,
    /** HW has completed the transfer successfully */
    PCIE_TD_COMPLETE = 5U,
    /** HW has failed to completed the transfer successfully */
    PCIE_TD_FAILURE = 6U
} PCIE_td_desc_state;

/** uDMA transfer type */
typedef enum {
    /** Undefined */
    PCIE_UNDEFINED_OP = 0U,
    /** Scatter operation, copy contiguous memory region to many */
    PCIE_SCT_OP = 1U,
    /** Gather operation, copy many memory regions  to contiguous */
    PCIE_GTH_OP = 2U,
    /** Block operation, copy one contiguous block to another */
    PCIE_BLK_OP = 3U
} PCIE_td_op_type;

/** uDMA transfer direction */
typedef enum {
    /** Undefined */
    PCIE_UNDEFINED_DIR = 0U,
    /** Inbound transfer, to system memory */
    PCIE_INB_DIR = 1U,
    /** Outbound transfer, to external memory */
    PCIE_OUTB_DIR = 2U
} PCIE_td_direction;

/**********************************************************************
* Callbacks
**********************************************************************/
/** Error Callback */
typedef void (*PCIE_ErrorCallback)(UINT32 re);

/**
 * uDMA Callback.
 * udmaChannel is the number of the uDMA channel for which the interrupt was received
 * isDone will be true for a uDMA Done interrupt, false for a uDMA Error interrupt
 */
typedef void (*PCIE_UdmaCallback)(UINT32 udmaChannel, PCIE_Bool isDone);

/** @defgroup DataStructure Dynamic Data Structures
 *  This section defines the data structures used by the driver to provide
 *  hardware information, modification and dynamic operation of the driver.
 *  These data structures are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
* Structures and unions
**********************************************************************/
/** UDMA configuration.  This struct is used to read configuration from the UDMA core */
typedef struct {
    /** Number of UDMA channels supported by the core */
    UINT32 numChannels;
    /** Number of partitions in each RAM */
    UINT32 numPartitions;
    /**
     * Size of RAM partition is (128 * (2 to the power partitionSize)).
     * Note that scatter/gather transfers are limited to RAM partition size / numPartitions
     */
    UINT32 partitionSize;
    /** TRUE if system addresses are 64bit wide */
    PCIE_Bool wideSysAddr;
    /** TRUE if attributes for system bus are 64bit wide */
    PCIE_Bool wideSysAttr;
    /** TRUE if external addresses are 64bit wide */
    PCIE_Bool wideExtAddr;
    /** TRUE if attributes for external bus are 64bit wide */
    PCIE_Bool wideExtAttr;
} PCIE_UdmaConfiguration;

/** The structure that contains addresses of BAR config registers */
typedef struct {
    /** @details Config register 0 */
    volatile UINT32* config_reg_0;
    /** @details Config register 1 */
    volatile UINT32* config_reg_1;
} PCIE_BarConfigRegisters;

/** The structure that contains the Read or Write indicator and pointer to read/write value */
typedef struct {
    /** @details Read or Write indicator */
    PCIE_ReadOrWrite rdOrWr;
    /** @details Value read or written */
    UINT32* pRdWrVal;
} PCIE_ReadOrWrite_Config;

/** Chip Initialization Info  - Used ONLY for initializing the chip at init */
typedef struct {
    /** Flag to indicate a change in default value in register */
    UINT32 changeDefault;
    /** The new value to be programmed into a register */
    UINT32 newValue;
} PCIE_InitValue;

/** Address Initialization Info  - Used ONLY for initializing the register addresses at init */
typedef struct {
    /** Flag to indicate a change in default address */
    UINT32 changeDefault;
    /** The new value to be used for the address */
    UINT64 newValue;
} PCIE_InitAddressValue;

/** Chip Initialization - Credit Related Parameters */
typedef struct {
    /**
     * @brief Initialization value for posted paymoad credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue postedPayloadCredit;
    /**
     * @brief Initialization value for posted header credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue postedHeaderCredit;
    /**
     * @brief Initialization value for non-posted payload credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue nonPostedPayloadCredit;
    /**
     * @brief Initialization value for non-posted header credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue nonPostedHeaderCredit;
    /**
     * @brief Initialization value for completion payload credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue completionPayloadCredit;
    /**
     * @brief Initialization value for completion header credit
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue completionHeaderCredit;
    /**
     * @brief Initialization value for min posted update interval
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue minPostedUpdateInterval;
    /**
     * @brief Initialization value for min non-posted update interval
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue minNonPostedUpdateInterval;
    /**
     * @brief Initialization value for credit update interval for Completion packets
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue completionUpdateInterval;
    /**
     * @brief Initialization value for maximum update interval for all transactions
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maxUpdateInterval;
} PCIE_InitCreditRelatedParams;

/** Chip Initialization - Count Related Parameters */
typedef struct {
    /**
     * @brief Initialization value for transmitted FTS count at 2.5GT
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue transmittedFtsCount2_5Gbs;
    /**
     * @brief Initialization value for transmitted FTS count at 5GT
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue transmittedFtsCount5Gbs;
    /**
     * @brief Initialization value for transmitted FTS count at 8GT
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue transmittedFtsCount8Gbs;
} PCIE_InitCountRelatedParams;

/** Chip Initialization - Timing Related Parameters */
typedef struct {
    /**
     * @brief Initialization value for  L0S_timeout
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue L0sTimeout;
    /**
     * @brief Initialization value for completion_timeout_limit_0
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue completionTimeoutLimit0;
    /**
     * @brief Initialization value for completion_timeout_limit_1
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue completionTimeoutLimit1;
    /**
     * @brief Initialization value for l1_retry_delay
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue l1RetryDelay;
    /**
     * @brief Initialization value for tx_replay_timeout_adjustment
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue txReplayTimeoutAdjustment;
    /**
     * @brief Initialization value for rx_replay_timeout_adjustment
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue rxReplayTimeoutAdjustment;
    /**
     * @brief Initialization value for l1_timeout
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue l1Timeout;
    /**
     * @brief Initialization value for pme_turnoff_ack_delay
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue pmeTurnoffAckDelay;
    /**
     * @brief Initialization value for pme_service_timeout_delay
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue pmeServiceTimeoutDelay;
    /**
     * @brief Initialization value for link-down reset timer delay
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue linkDownResetTim;
} PCIE_InitTimingRelatedParams;

/** Chip Initialization - Identifier Related Parameters */
typedef struct {
    /**
     * @brief Initialization value for VendorId
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue vendorId;
    /**
     * @brief Initialization value for SubsystemVendorId
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue subsystemVendorId;
} PCIE_InitIdRelatedParams;

/** Chip Initialization - Target Lane Map Related Parameters */
typedef struct {
    /**
     * @brief Initialization value for TargetLaneMap
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue targetLaneMap;
} PCIE_TargetLaneMapRelatedParams;

/** Chip Initialization - Debug Related Parameters */
typedef struct {
    /**
     * @brief Initialization value for MuxSelect
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue muxSelect;
    /**
     * @brief Initialization value for SyncHeaderFramingError
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue syncHeaderFramingError;
    /**
     * @brief Initialization value for LinkReTrainingFramingError
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue linkReTrainingFramingError;
    /**
     * @brief Initialization value for LinkUpconfigureCapability
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue linkUpconfigureCapability;
    /**
     * @brief Initialization value for FastLinkTraining
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue fastLinkTraining;
    /**
     * @brief Initialization value for SlotPowerCapture
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue slotPowerCapture;
    /**
     * @brief Initialization value for Gen3LfsrUpdateFromSkp
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue gen3LfsrUpdateFromSkp;
    /**
     * @brief Initialization value for ElectricalIdleInferInL0State
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue electricalIdleInferInL0State;
    /**
     * @brief Initialization value for FlowControlUpdateTimeout
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue flowControlUpdateTimeout;
    /**
     * @brief Initialization value for OrderingChecks
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue orderingChecks;
    /**
     * @brief Initialization value for FuncSpecificRprtType1CfgAccess
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue funcSpecificRprtType1CfgAccess;
    /**
     * @details When this value is enabled (set to a non-0 value), the core will report all
     * phy errors detected at the PIPE interface, regardless of whether it caused
     * packet corruption. If disabled, the core will report only those
     * errors that caused a TLP or DLLP to be dropped because of the phy error.
     */
    PCIE_InitValue allPhyErrorReporting;
    /**
     * @details This value when enabled (set to a non-0 value), will drive the PIPE_TX_SWING
     * output of the core
     */
    PCIE_InitValue txSwing;
    /**
     * @details When the core is operating as a Root Port, setting this to 1 causes the LTSSM to
     * initiate a loopback and become the loopback master.
     * This bit is not used in the EndPoint Mode.
     */
    PCIE_InitValue masterLoopback;
} PCIE_InitDebugRelatedParams;

/** Chip Initialization - Maskable Conditions */
typedef struct {
    /**
     * @brief Initialization value for PNP RX FIFO Parity Error interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskPnpRxFifoParityError;
    /**
     * @brief Initialization value for PnpRxFifoOverflow interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskPnpRxFifoOverflow;
    /**
     * @brief Initialization value for CompletionRxFifoParity Error interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskCompletionRxFifoParityError;
    /**
     * @brief Initialization value for ReplayRamParity Error interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskReplayRamParityError;
    /**
     * @brief Initialization value for CompletionRxFifoOverflow interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskCompletionRxFifoOverflow;
    /**
     * @brief Initialization value for ReplayTimeout interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskReplayTimeout;
    /**
     * @brief Initialization value for ReplayTimerRollover interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskReplayTimerRollover;
    /**
     * @brief Initialization value for PhyError interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskPhyError;
    /**
     * @brief Initialization value for MalformedTlpReceived interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskMalformedTlpReceived;
    /**
     * @brief Initialization value for UnexpectedComplReceived interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskUnexpectedComplReceived;
    /**
     * @brief Initialization value for FlowControlError interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskFlowControlError;
    /**
     * @brief Initialization value for CompletionTimeout interrupt mask
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue maskCompletionTimeout;
} PCIE_InitMaskableConditions;

/** Chip Initialization - Init Physical Function BAR Parameters */
typedef struct {
    /**
     * @brief Initialization value for Function Bar Aperture
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue funcBarAperture[PCIE_MAX_BARS_IN_FUNC];
    /**
     * @brief Initialization value for Function Bar Control
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue funcBarControl[PCIE_MAX_BARS_IN_FUNC];
} PCIE_InitBarParams;

/** Chip Initialization - Init RP BAR Parameters */
typedef struct {
    /**
     * @brief Initialization value for Root Port Bar Aperture
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue rpBarAperture[PCIE_MAX_BARS_IN_RP];
    /**
     * @brief Initialization value for Root Port Bar Control
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue rpBarControl[PCIE_MAX_BARS_IN_RP];
    /**
     * @brief Initialization value for Enable Root Port Bar control
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue enableRpBarCheck;
} PCIE_InitRpBarParams;

/** Chip Initialization - Link Equalization Parameters */
typedef struct {
    /**
     * @brief Initialization value for 8GT Link Equalization downstream port Transmitter preset
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue downstreamTxPs8GT[PCIE_LANE_COUNT];
    /**
     * @brief Initialization value for 8GT Link Equalization downstream port Receiver preset
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue downstreamRxPs8GT[PCIE_LANE_COUNT];
    /**
     * @brief Initialization value for 8GT Link Equalization upstream port Transmitter preset
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue upstreamTxPs8GT[PCIE_LANE_COUNT];
    /**
     * @brief Initialization value for 8GT Link Equalization upstream port Receiver preset
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitValue upstreamRxPs8GT[PCIE_LANE_COUNT];
} PCIE_InitLinkEquParams;

/** Address Initialization Info  - Used ONLY for initializing the register addresses at init */
typedef struct {
    /** The new value to be used for the address */
    struct PCIE_IClientLm_s* newLmAddrValue;
    /** Flag to indicate a change in default address */
    UINT32 changeDefault;
} PCIE_InitLmBaseAddress;

/** Address Initialization Info  - Used ONLY for initializing the register addresses at init */
typedef struct {
    /** The new value to be used for the address */
    struct PCIE_IClientRc_s* newRcAddrValue;
    /** Flag to indicate a change in default address */
    UINT32 changeDefault;
} PCIE_InitRcBaseAddress;

/** Address Initialization Info  - Used ONLY for initializing the register addresses at init */
typedef struct {
    /** The new value to be used for the address */
    struct PCIE_IClientAtu_s* newAtuAddrValue;
    /** Flag to indicate a change in default address */
    UINT32 changeDefault;
} PCIE_InitAtuBaseAddress;

/** The structure that contains attributes data used by the UDMA channel when fetching and returning the link list descriptors (lower and upper 32 bits) */
typedef struct {
    /** @details Lower 32 bits of attribute */
    UINT32 lower;
    /** @details Upper 32 bits of attribute */
    UINT32 upper;
} PCIE_ListAttribute;

/** Address Initialization Info  - Used ONLY for initializing the register addresses at init */
typedef struct {
    /** The new value to be used for the address */
    struct PCIE_IClientUdma_s* newUdmaAddrValue;
    /** Flag to indicate a change in default address */
    UINT32 changeDefault;
} PCIE_InitUdmaBaseAddress;

/** Chip Base Addresses */
typedef struct {
    /** @details Base address for Local Management registers */
    PCIE_InitLmBaseAddress lmBase;
    /** @details Base address for AXI Wrapper registers */
    PCIE_InitAtuBaseAddress awBase;
    /** @details Base address for AXI Interface, to be used for ECAM config access */
    PCIE_InitAddressValue axBase;
    /** @details Base address for UDMA registers */
    PCIE_InitUdmaBaseAddress udBase;
} PCIE_InitBaseAddresses;

/** Chip Initialization - Init structure to intialize or leave at default */
typedef struct {
    /**
     * @brief Initialization Values for Base
     * addresses
     */
    PCIE_InitBaseAddresses initBaseAddresses;
    /**
     * @brief Initialization Values For Credit Related Params
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitCreditRelatedParams initCreditParams;
    /**
     * Initialization Values For Count Registers that save count values
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitCountRelatedParams initCountParams;
    /**
     * @brief Initialization Values For Timeouts, Delays and Latency Settings
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitTimingRelatedParams initTimingParams;
    /**
     * @brief Initialization Values For Identifiers
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitIdRelatedParams initIdParams;
    /**
     * @brief Initialization Values For Target Lane Map
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_TargetLaneMapRelatedParams initTargetLaneMap;
    /**
     * @brief Initialization Values For Debug Controllable Parameters
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitDebugRelatedParams initDebugParams;
    /**
     * @brief Initialization Values For Maskable Interrupt Condition Types
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitMaskableConditions initMaskableConditions;
    /**
     * @brief Initialization Values For physical functions
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitBarParams initBarParams[1];
    /**
     * @brief Initialization Values For Root Port Aperture and BARS
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitRpBarParams initRpBarParams;
    /**
     * @brief Initialization Values For Link Equalization preset values
     * @details Refer to the Cadence PCIe Core User Guide for further information
     */
    PCIE_InitLinkEquParams initLinkEquParams;
    /**
     * @details Error Callback which will be called by the Core driver in the case
     * it encounters one or multiple errors
     */
    PCIE_ErrorCallback initErrorCallback;
    /**
     * @details uDMA Callback which will be called by the Core driver in the case
     * it encounters a uDMA Done or Error interrupt.
     */
    PCIE_UdmaCallback initUdmaCallback;
} PCIE_InitParam;

/** This structure is used to read Link equalization debug informations */
typedef struct {
    /** @details TX Preset agreed upon for this lane */
    UINT8 leqTxPr;
    /** @details TX Preset Valid Indicator. */
    PCIE_Bool leqTxPrValid;
    /** @details TX Coefficients agreed upon for this lane. */
    UINT32 leqTxCo;
} PCIE_LinkEquaDebugInfo;

/** uDMA TD channel */
typedef struct {
    /** Index of dar head */
    UINT32 headDarIdx;
    /** uDMA channel state */
    PCIE_channel_state state;
    /** uDMA channel state */
    PCIE_channel_state hwState;
} PCIE_td_channel;

/** uDMA transfer address & size */
typedef struct {
    /** a "x" value */
    UINT32 x;
    /** sz of transfer of "x" value */
    UINT32 n;
} PCIE_td_xn_pair;

/** uDMA transfer list of addresses & sizes */
typedef struct {
    /** the "a" value */
    UINT32 a;
    /** number of "x" values */
    UINT32 num_x_vals;
    /** "x/n" value pairs */
    PCIE_td_xn_pair* xn;
} PCIE_td_addr;

/** uDMA TD descriptor used by upper layer */
typedef struct PCIE_td_desc_s {
    /** Type of operation (scatter, gather, bulk) */
    PCIE_td_op_type op_typ;
    /** Direction of transfer (inbound, outbound) */
    PCIE_td_direction direction;
    /** TD state */
    PCIE_td_desc_state state;
    /** uDMA channel to be used */
    UINT32 channel;
    /** Pointer to next TD in linked list */
    struct PCIE_td_desc_s* next;
    /** Reserved */
    UINT32* drv_reserved;
    /** Reserved */
    UINT32 ref_count;
    /** pointers to source and destination, and transfer sizes */
    PCIE_td_addr xfer;
} PCIE_td_desc;

/** uDMA control byte bitfields */
typedef struct {
    /** Control bits */
    UINT8 control_bits;
} PCIE_cbits;

/** uDMA transfer size and control byte */
typedef struct {
    /** Number of bytes to be transferred.  For max bulk transfer size, set to zero */
    UINT32 size : 24;
    /** Control byte */
    PCIE_cbits ctrl_bits;
} PCIE_sz_ctrl;

/** uDMA status bytes */
typedef struct {
    /** System (local) bus status */
    UINT8 sys_status;
    /** External (remote) bus status */
    UINT8 ext_status;
    /** uDMA channel status */
    UINT8 chnl_status;
    /** Reserved */
    UINT8 reserved_0;
} PCIE_sbytes;

/** uDMA Descriptor structure */
typedef struct {
    /** Low 32 bits of system address */
    UINT32 sys_lo_addr;
    /** High 32 bits of system address */
    UINT32 sys_hi_addr;
    /** Access attributes for system bus */
    UINT32 sys_attr;
    /** Low 32 bits of external address */
    UINT32 ext_lo_addr;
    /** High 32 bits of external address */
    UINT32 ext_hi_addr;
    /** Access attributes for external bus */
    UINT32 ext_attr;
    /** Transfer size and control byte */
    PCIE_sz_ctrl size_and_ctrl;
    /** Transfer status.  This word is written by uDMA engine, and can be read to determine status. */
    PCIE_sbytes status;
    /** Low 32bits of pointer to next descriptor in linked list */
    UINT32 next;
    /** High 32bits of pointer to next descriptor in linked list */
    UINT32 next_hi_addr;

    UINT32 reserved[6];
} PCIE_xd_desc;

/** uDMA TD descriptor */
typedef struct {
    /** XD descriptor */
    PCIE_xd_desc xds[PCIE_MAX_DESC_SLOTS];
    /** Value of next free slot */
    UINT32 next_free_slot;
    /** Value of next element */
    UINT32 next;
    /** TD dar state */
    PCIE_td_dar_state state;
    /** uDMA number of used slots */
    UINT32 num_slots_used;
} PCIE_td_dar;

/** uDMA TD configuration */
typedef struct {
    /** Array of dars */
    PCIE_td_dar dars[PCIE_MAX_DARS];
    /** uDMA td channel */
    PCIE_td_channel channels[PCIE_NUM_UDMA_CHANNELS];
    /** uDMA channel state */
    PCIE_td_desc* altd_head;
    /** Base address of descriptor */
    UINT32 desc_mem_phys_base_addr;
} PCIE_td_cfg;

/** The structure that contains the resource requirements for driver operation */
typedef struct {
    /** @details The number of bytes required for driver operation */
    UINT32 pdataSize;
} PCIE_SysReq;

/** Driver private data */
typedef struct {
    /** Base address for PCIe registers. */
    UINT64 p_pcie_base;
    /** Base address for PCIe LocalManagement registers. */
    struct PCIE_IClientLm_s* p_lm_base;
    /** Base address for PCIe RootPort registers. */
    struct PCIE_IClientRc_s* p_rp_base;
    /** Base address for PCIe AXI Interface registers. */
    struct PCIE_IClientAtu_s* p_aw_base;
    /** Base address for PCIe uDMA registers. */
    struct PCIE_IClientUdma_s* p_ud_base;
    /** Base address for AXI regions */
    UINT64 p_ax_base;
    /** Transfer descriptors */
    PCIE_td_cfg* p_td_cfg;
    /** Pointer to PCIe error callback */
    PCIE_UdmaCallback p_td_func;
    /** Pointer to PCIe error callback */
    PCIE_ErrorCallback p_ec_func;
} PCIE_PrivateData;

/**
 *  @}
 */


/****************************************************************************/
/*   Defines for uDMA                                                       */
/****************************************************************************/
#define    MAX_DESC_SLOTS    5U
#define    MAX_DARS          5U
#define    EXTEND_BULK_TRANSFER_FLAG 0x20U

/****************************************************************************/
/*   State of xd descriptor                                                 */
/****************************************************************************/
typedef enum {

    XD_DESC_UNDEFINED,
    XD_DESC_COMPLETE,
    XD_DESC_FAILURE

} xd_desc_state;

/****************************************************************************/
/* The structure that contains transfer configuration data                  */
/****************************************************************************/
typedef struct {
    UINT16 bus;
    UINT16 dev;
    UINT16 func;
    UINT32 offset;

} TransferConfigData;

/****************************************************************************/
/* Settings of the credit limit, includes name, direction and read/wite info*/
/****************************************************************************/
typedef enum {

    CPD_PPC_RCV_READ       = 0x000,
    CPD_PPC_RCV_WRITE      = 0x001,
    CPD_PPC_TRANSM_READ    = 0x010,
    CPD_PPC_TRANSM_WRITE   = 0x011,
    CPD_PHC_RCV_READ       = 0x100,
    CPD_PHC_RCV_WRITE      = 0x101,
    CPD_PHC_TRANSM_READ    = 0x110,
    CPD_PHC_TRANSM_WRITE   = 0x111,
    CPD_NPPC_RCV_READ      = 0x200,
    CPD_NPPC_RCV_WRITE     = 0x201,
    CPD_NPPC_TRANSM_READ   = 0x210,
    CPD_NPPC_TRANSM_WRITE  = 0x211,
    CPD_NPHC_RCV_READ      = 0x300,
    CPD_NPHC_RCV_WRITE     = 0x301,
    CPD_NPHC_TRANSM_READ   = 0x310,
    CPD_NPHC_TRANSM_WRITE  = 0x311,
    CPD_CPC_RCV_READ       = 0x400,
    CPD_CPC_RCV_WRITE      = 0x401,
    CPD_CPC_TRANSM_READ    = 0x410,
    CPD_CPC_TRANSM_WRITE   = 0x411,
    CPD_CHC_RCV_READ       = 0x500,
    CPD_CHC_RCV_WRITE      = 0x501,
    CPD_CHC_TRANSM_READ    = 0x510,
    CPD_CHC_TRANSM_WRITE   = 0x511

} cred_lim_setting;

/****************************************************************************/
/* Type of array of pointers to local functions                             */
/****************************************************************************/
typedef UINT32 (*AccessCreditLimitSettingsReadFunctions)(const PCIE_PrivateData *pD, UINT8 vcNum, UINT32 *pRdWrVal);
typedef UINT32 (*AccessCreditLimitSettingsWriteFunctions)(const PCIE_PrivateData *pD, UINT8 vcNum, const UINT32 *pRdWrVal);

typedef UINT32 (*TimeoutAndDelayFunctions)(const PCIE_PrivateData * pD, PCIE_ReadOrWrite rdOrWr, UINT32 * pRdWrVal);

typedef UINT32 (*ControlDebugParamsFunctions)(const PCIE_PrivateData * pD, PCIE_EnableOrDisable enableOrDisable);
typedef UINT32 (*SetUdmaRegsFunctions)(UINT32 * pUdmaIntEna, UINT32 * pUdmaIntDis, UINT8 enableSetting, UINT8 disableSetting);

typedef void (*UdmaInterruptRegsStatusFunctions)(UINT32 udmaReg, struct PCIE_IClientUdma_s * pcieAddr, PCIE_Bool * pIntActive);
typedef void (*SetUdmaChannelAddressFunctions)(struct PCIE_IClientUdma_s * chanregAddr, UINT64 address);
typedef void (*SetUdmaChannelAttributesFunctions)(struct PCIE_IClientUdma_s * chanregAddr, const PCIE_ListAttribute * pListAttr);

/** @defgroup DriverObject Driver API Object
 *  API listing for the driver. The API is contained in the object as
 *  function pointers in the object structure. As the actual functions
 *  resides in the Driver Object, the client software must first use the
 *  global GetInstance function to obtain the Driver Object Pointer.
 *  The actual APIs then can be invoked using obj->(api_name)() syntax.
 *  These functions are defined in the header file of the core driver
 *  and utilized by the API.
 *  @{
 */

/**********************************************************************
* API methods
**********************************************************************/

/* parasoft-begin-suppress MISRA2012-DIR-4_8-4 "Consider hiding the implementation of the structure, DRV-4932" */
typedef struct {

    UINT32 (*probe)(UINT64 cfg, PCIE_SysReq* sysReq);

    UINT32 (*init)(PCIE_PrivateData* pD, UINT64 cfg, const PCIE_InitParam* pInitParam);

    void (*destroy)(void);

    UINT32 (*start)(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam);

    UINT32 (*stop)(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam);

    UINT32 (*isr)(const PCIE_PrivateData* pD);

    UINT32 (*isLinkTrainingComplete)(const PCIE_PrivateData* pD, PCIE_Bool* pTrainingComplete);

    UINT32 (*getLinkTrainingState)(const PCIE_PrivateData* pD, PCIE_LtssmState* pLtssmState);

    UINT32 (*getLinkTrainingDirection)(const PCIE_PrivateData* pD, PCIE_UpstreamOrDownstream* pUpOrDown);

    UINT32 (*isCoreStrappedAsEpOrRp)(const PCIE_PrivateData* pD, PCIE_CoreEpOrRp* pEpOrRp);

    UINT32 (*getNegotiatedLaneCount)(const PCIE_PrivateData* pD, PCIE_LaneCount* nlc);

    UINT32 (*getNegotiatedLinkSpeed)(const PCIE_PrivateData* pD, PCIE_LinkSpeed* ns);

    UINT32 (*getNegotiatedLaneBitMap)(const PCIE_PrivateData* pD, UINT32* pLaneMap);

    UINT32 (*areLaneNumbersReversed)(const PCIE_PrivateData* pD, PCIE_Bool* pReversed);

    UINT32 (*getReceivedLinkId)(const PCIE_PrivateData* pD, UINT32* pLinkId);

    UINT32 (*isRemoteLinkwidthUpconfig)(const PCIE_PrivateData* pD, PCIE_Bool* upconfig);

    UINT32 (*getTransmittedLinkId)(const PCIE_PrivateData* pD, UINT8* pLinkId);

    UINT32 (*setTransmittedLinkId)(const PCIE_PrivateData* pD, UINT8 pLinkId);

    UINT32 (*getVendorIdSubsysVendId)(const PCIE_PrivateData* pD, PCIE_AdvertisedIdType id, UINT16* idValue);

    UINT32 (*setVendorIdSubsysVendId)(const PCIE_PrivateData* pD, PCIE_AdvertisedIdType id, UINT16 idValue);

    UINT32 (*setTimingParams)(const PCIE_PrivateData* pD, PCIE_TimeoutAndDelayValues timingVal, UINT32 timeDelay);

    UINT32 (*getTimingParams)(const PCIE_PrivateData* pD, PCIE_TimeoutAndDelayValues timingVal, UINT32* timeDelay);

    UINT32 (*setL0sTimeout)(const PCIE_PrivateData* pD, UINT32 *timeout);

    UINT32 (*getL0sTimeout)(const PCIE_PrivateData* pD, UINT32* timeout);

    UINT32 (*disableRpTransitToL0s)(const PCIE_PrivateData* pD);

    UINT32 (*getTargetLanemap)(const PCIE_PrivateData* pD, UINT32* lanemap);

    UINT32 (*setTargetLanemap)(const PCIE_PrivateData* pD, UINT32 lanemap);

    UINT32 (*retrainLink)(const PCIE_PrivateData* pD, PCIE_WaitOrNot waitForCompletion);

    UINT32 (*setCreditLimitSettings)(const PCIE_PrivateData* pD, PCIE_CreditLimitTypes creditLimitParam, PCIE_TransmitOrReceive transmitOrReceive, UINT8 vcNum, UINT32 limit);

    UINT32 (*getCreditLimitSettings)(const PCIE_PrivateData* pD, PCIE_CreditLimitTypes creditLimitParam, PCIE_TransmitOrReceive transmitOrReceive, UINT8 vcNum, UINT32* limit);

    UINT32 (*setTransCreditUpdInterval)(const PCIE_PrivateData* pD, PCIE_CreditUpdateIntervals creditUpdateIntervalParam, UINT32 *interval);

    UINT32 (*getTransCreditUpdInterval)(const PCIE_PrivateData* pD, PCIE_CreditUpdateIntervals creditUpdateIntervalParam, UINT32* interval);

    UINT32 (*getFuncBarApertureSetting)(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarApertureSize* apertureSize);

    UINT32 (*setFuncBarApertureSetting)(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarApertureSize apertureSize);

    UINT32 (*setFuncBarControlSetting)(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarControl barControl);

    UINT32 (*getFuncBarControlSetting)(const PCIE_PrivateData* pD, UINT32 funcNumber, PCIE_BarNumber barNumber, PCIE_BarControl* barControl);

    UINT32 (*setRootPortBarApertSetting)(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarApertureSize apertureSize);

    UINT32 (*getRootPortBarApertSetting)(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarApertureSize* apertureSize);

    UINT32 (*getRootPortBarCtrlSetting)(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarControl* rpBarControl);

    UINT32 (*setRootPortBarCtrlSetting)(const PCIE_PrivateData* pD, PCIE_RpBarNumber barNumber, PCIE_RpBarControl rpBarControl);

    UINT32 (*getRootPortType1ConfSet)(const PCIE_PrivateData* pD, PCIE_RpType1ConfigControl* rpType1Config);

    UINT32 (*setRootPortType1ConfSet)(const PCIE_PrivateData* pD, PCIE_RpType1ConfigControl rpType1Config);

    UINT32 (*controlRootPortBarCheck)(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*controlRpMasterLoopback)(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*getDebugMux)(const PCIE_PrivateData* pD, PCIE_DebugMuxModuleSelect* muxSelect);

    UINT32 (*setDebugMux)(const PCIE_PrivateData* pD, PCIE_DebugMuxModuleSelect muxSelect);

    UINT32 (*controlDebugParams)(const PCIE_PrivateData* pD, PCIE_DebugParams debugParam, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*setSavedCountValues)(const PCIE_PrivateData* pD, PCIE_SavedCountParams countParam, PCIE_Bool resetAfterRd, UINT32 *pCountVal);

    UINT32 (*getSavedCountValues)(const PCIE_PrivateData* pD, PCIE_SavedCountParams countParam, PCIE_Bool resetAfterRd, UINT32* pCountVal);

    UINT32 (*ctrlReportingOfAllPhyErr)(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*controlTxSwing)(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*ctrlMaskingOfLocInterrupt)(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam, PCIE_MaskOrUnmask maskOrUnmask);

    UINT32 (*areThereLocalErrors)(const PCIE_PrivateData* pD, PCIE_Bool* pLocalErrorsPresent, UINT32* pErrorType);

    UINT32 (*isLocalError)(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions errorCondition, PCIE_Bool* pLocalError);

    UINT32 (*resetLocalErrStatusCondit)(const PCIE_PrivateData* pD, PCIE_MaskableLocalInterruptConditions maskableIntrParam);

    UINT32 (*getRootPortRequestorId)(const PCIE_PrivateData* pD, UINT16* requestorId);

    UINT32 (*setRootPortRequestorId)(const PCIE_PrivateData* pD, UINT16 requestorId);

    UINT32 (*ctrlSnpNoSnpLatReqInLtrMsg)(const PCIE_PrivateData* pD, PCIE_SnoopOrNoSnoop snoopOrNoSnoop, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*sendMessage)(const PCIE_PrivateData* pD, PCIE_MessageType msgType, PCIE_WaitOrNoWaitForComplete waitFlag);

    UINT32 (*enableAutomaticMessage)(const PCIE_PrivateData* pD, PCIE_AutoMessageType msgType);

    UINT32 (*clearLinkDownIndicator)(const PCIE_PrivateData* pD);

    UINT32 (*controlLinkDownReset)(const PCIE_PrivateData* pD, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*updObWrapperTrafficClass)(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 trafficClass);

    UINT32 (*setupObWrapperMemIoAccess)(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT64 address);

    UINT32 (*setupObWrapperMsgAccess)(const PCIE_PrivateData* pD, UINT8 regionNo, UINT64 address);

    UINT32 (*setupObWrapperConfigAccess)(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT32 busDevFuncAddr);

    UINT32 (*setupIbRootPortAddrTransl)(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT64 address);

    UINT32 (*setupIbEPAddrTranslation)(const PCIE_PrivateData* pD, UINT32 functionNo, PCIE_BarNumber barNo, UINT64 address);

    UINT32 (*setupObWrapperRegions)(const PCIE_PrivateData* pD, UINT8 regionNo, UINT8 numPassedBits, UINT32 upperAddr, UINT32 lowerAddr);

    UINT32 (*doConfigRead)(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32* data);

    UINT32 (*doConfigWrite)(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32 data);

    UINT32 (*doAriConfigRead)(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32* data);

    UINT32 (*doAriConfigWrite)(const PCIE_PrivateData* pD, UINT16 bdf, UINT32 offset, PCIE_SizeInBytes size, UINT32 data);

    UINT32 (*getRootPortBAR)(const PCIE_PrivateData* pD, UINT32 bar, UINT32* barVal);

    UINT32 (*getRp8GTsLinkEquPreset)(const PCIE_PrivateData* pD, PCIE_TransmitOrReceiveSide txRxSide, PCIE_UpstreamOrDownstream upDownStrm, UINT32 reqLane, UINT32* presetValPtr);

    UINT32 (*setRp8GTsLinkEquPreset)(const PCIE_PrivateData* pD, PCIE_TransmitOrReceiveSide txRxSide, PCIE_UpstreamOrDownstream upDownStrm, UINT32 reqLane, UINT32 presetVal);

    UINT32 (*getLinkEquGen3DebugInfo)(const PCIE_PrivateData* pD, UINT32 reqLane, PCIE_LinkEquaDebugInfo* lEquaInfo);

    UINT32 (*enableRpMemBarAccess)(const PCIE_PrivateData* pD, PCIE_RpBarNumber bar, UINT32 addr_hi, UINT32 addr_lo);

    UINT32 (*UDMA_GetVersion)(const PCIE_PrivateData* pD, UINT8* maj_ver, UINT8* min_ver);

    UINT32 (*UDMA_GetConfiguration)(const PCIE_PrivateData* pD, PCIE_UdmaConfiguration* pConf);

    UINT32 (*UDMA_ControlDoneInterrupts)(const PCIE_PrivateData* pD, UINT32 channel, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*UDMA_ControlErrInterrupts)(const PCIE_PrivateData* pD, UINT32 channel, PCIE_EnableOrDisable enableOrDisable);

    UINT32 (*UDMA_GetDoneInterrupt)(const PCIE_PrivateData* pD, UINT32 channel, PCIE_Bool* pIntActive);

    UINT32 (*UDMA_GetErrorInterrupt)(const PCIE_PrivateData* pD, UINT32 channel, PCIE_Bool* pIntActive);

    UINT32 (*UDMA_DoTransfer)(const PCIE_PrivateData* pD, UINT32 channel, PCIE_Bool isOutbound, const UINT32* pListAddr, const PCIE_ListAttribute* pListAttr);

    UINT32 (*UDMA_PrepareDescriptor)(const PCIE_PrivateData* pD, const UINT32* pDesc, UINT32* pDescAddr, UINT32 count);

    UINT32 (*UDMA_ExtendBulkDescriptor)(const PCIE_PrivateData* pD, const UINT32* pDesc, const UINT32* pDescAddr);

    UINT32 (*UDMA_GetBufferErrorCount)(const PCIE_PrivateData* pD, PCIE_UdmaBufferErrorType errorType, UINT32* pErrorCount);

    UINT32 (*UDMA_Init)(PCIE_PrivateData* pD, PCIE_td_cfg* pDescMem, UINT32 phys_base);

    UINT32 (*UDMA_RequestLtdAttach)(const PCIE_PrivateData* pD, PCIE_td_desc* p_ltd);

    UINT32 (*UDMA_RequestLtdExtend)(const PCIE_PrivateData* pD, PCIE_td_desc* p_ltd);

    UINT32 (*UDMA_ClearState)(const PCIE_PrivateData* pD);

    UINT32 (*UDMA_RequestLtdDetach)(const PCIE_PrivateData* pD);

    UINT32 (*UDMA_CheckLtdDetach)(const PCIE_PrivateData* pD);

    UINT32 (*UDMA_Isr)(const PCIE_PrivateData* pD);

    void (*DmaSysCb)(UINT32 status);

} PCIE_OBJ;

#endif  /* AMBAPCIE_STRUCT_H */
