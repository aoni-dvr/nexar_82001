#ifndef STD_XHCI_H
#define STD_XHCI_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#define XHCI_CONTEXT_ALIGNMENT          (64U)
#define XHCI_DCBAA_ALIGNMENT            (64U)
#define    XHCI_RING_ALIGNMENT             (64U)
#define    XHCI_RING_BOUNDARY              (65536U)
#define    XHCI_ERST_ALIGNMENT             (64U)
#define    XHCI_ERST_BOUNDARY              (0U)

#define XHCI_MAX_PORT_NUM               (255UL)

#define XHCI_PORTSC_PLS_RXDETECT        (5U)

//
// Definitions of standard TRB types
//
#define    XHCI_TRB_NORMAL                (1U)
#define    XHCI_TRB_SETUP_STAGE           (2U)
#define    XHCI_TRB_DATA_STAGE            (3U)
#define    XHCI_TRB_STATUS_STAGE          (4U)
#define    XHCI_TRB_ISOCH                 (5U)
#define    XHCI_TRB_LINK                  (6U)
#define    XHCI_TRB_EVENT_DATA            (7U)
#define    XHCI_TRB_NO_OP                 (8U)
#define    XHCI_TRB_ENABLE_SLOT_COMMAND   (9U)
#define    XHCI_TRB_DISABLE_SLOT_COMMAND  (10U)
#define    XHCI_TRB_ADDR_DEV_CMD          (11U)
#define    XHCI_TRB_CONF_EP_CMD           (12U)
#define    XHCI_TRB_EVALUATE_CXT_CMD      (13U)
#define    XHCI_TRB_RESET_EP_CMD          (14U)
#define    XHCI_TRB_STOP_EP_CMD           (15U)
#define    XHCI_TRB_SET_TR_DQ_PTR_CMD     (16U)
#define    XHCI_TRB_RESET_DEVICE_COMMAND  (17U)
#define    XHCI_TRB_FORCE_EVENT_COMMAND   (18U)
#define    XHCI_TRB_NEGOTIATE_BANDWIDTH   (19U)
#define    XHCI_TRB_SET_LAT_TOL_VAL_CMD   (20U)
#define    XHCI_TRB_GET_PORT_BNDWTH_CMD   (21U)
#define    XHCI_TRB_FORCE_HEADER_COMMAND  (22U)
#define    XHCI_TRB_NO_OP_COMMAND         (23U)
#define    XHCI_TRB_TRANSFER_EVENT        (32U)
#define    XHCI_TRB_CMD_CMPL_EVT          (33U)
#define    XHCI_TRB_PORT_ST_CHG_EVT       (34U)
#define    XHCI_TRB_BNDWTH_RQ_EVT         (35U)
#define    XHCI_TRB_DOORBELL_EVENT        (36U)
#define    XHCI_TRB_HOST_CTRL_EVT         (37U)
#define    XHCI_TRB_DEV_NOTIFCN_EVT       (38U)
#define    XHCI_TRB_MFINDEX_WRAP_EVENT    (39U)

//
// Definitions of TRB Completion Code
//
#define    XHCI_TRB_CMPL_INVALID            (0U)
#define    XHCI_TRB_CMPL_SUCCESS            (1U)
#define    XHCI_TRB_CMPL_DATA_BUFF_ER       (2U)
#define    XHCI_TRB_CMPL_BBL_DETECT_ER      (3U)
#define    XHCI_TRB_CMPL_USB_TRANSCN_ER     (4U)
#define    XHCI_TRB_CMPL_TRB_ERROR          (5U)
#define    XHCI_TRB_CMPL_STALL_ERROR        (6U)
#define    XHCI_TRB_CMPL_RSRC_ER            (7U)
#define    XHCI_TRB_CMPL_BDWTH_ER           (8U)
#define    XHCI_TRB_CMPL_NO_SLTS_AVL_ER     (9U)
#define    XHCI_TRB_CMPL_INVSTRM_TYP_ER     (10U)
#define    XHCI_TRB_CMPL_SLT_NOT_EN_ER      (11U)
#define    XHCI_TRB_CMPL_EP_NOT_EN_ER       (12U)
#define    XHCI_TRB_CMPL_SHORT_PKT          (13U)
#define    XHCI_TRB_CMPL_RING_UNDERRUN      (14U)
#define    XHCI_TRB_CMPL_RING_OVERRUN       (15U)
#define    XHCI_TRB_CMPL_VF_EVTRNGFL_ER     (16U)
#define    XHCI_TRB_CMPL_PARAMETER_ER       (17U)
#define    XHCI_TRB_CMPL_BDWTH_OVRRN_ER     (18U)
#define    XHCI_TRB_CMPL_CXT_ST_ER          (19U)
#define    XHCI_TRB_CMPL_NO_PNG_RSP_ER      (20U)
#define    XHCI_TRB_CMPL_EVT_RNG_FL_ER      (21U)
#define    XHCI_TRB_CMPL_INCMPT_DEV_ER      (22U)
#define    XHCI_TRB_CMPL_MISSED_SRV_ER      (23U)
#define    XHCI_TRB_CMPL_CMD_RNG_STOPPED    (24U)
#define    XHCI_TRB_CMPL_CMD_ABORTED        (25U)
#define    XHCI_TRB_CMPL_STOPPED            (26U)
#define    XHCI_TRB_CMPL_STOP_LEN_INV       (27U)
#define    XHCI_TRB_CMPL_STOP_SHORT_PKT     (28U)
#define    XHCI_TRB_CMPL_MAXEXTLT_LG_ER     (29U)
#define    XHCI_TRB_CMPL_ISO_BUFF_OVRUN     (31U)
#define    XHCI_TRB_CMPL_EVT_LOST_ER        (32U)
#define    XHCI_TRB_CMPL_UNDEFINED_ER       (33U)
#define    XHCI_TRB_CMPL_INV_STRM_ID_ER     (34U)
#define    XHCI_TRB_CMPL_SEC_BDWTH_ER       (35U)
#define    XHCI_TRB_CMPL_SPLT_TRNSCN_ER     (36U)

//
// XHCI endpoint state defined in Endpoint Context
//
#define XHCI_EPCTX_STATE_DISABLED              (0U)
#define XHCI_EPCTX_STATE_RUNNING               (1U)
#define XHCI_EPCTX_STATE_HALTED                (2U)
#define XHCI_EPCTX_STATE_STOPPED               (3U)
#define XHCI_EPCTX_STATE_ERROR                 (4U)

//
// XHCI endpoint type defined in Endpoint Context
//
#define XHCI_EPCXT_TYPE_ISO_OUT                (1U)
#define XHCI_EPCXT_TYPE_BULK_OUT               (2U)
#define XHCI_EPCXT_TYPE_INT_OUT                (3U)
#define XHCI_EPCXT_TYPE_CTRL_BI                (4U)
#define XHCI_EPCXT_TYPE_ISO_IN                 (5U)
#define XHCI_EPCXT_TYPE_BULK_IN                (6U)
#define XHCI_EPCXT_TYEP_INT_IN                 (7U)


typedef struct {
    UINT32             PORTSC;
    UINT32             PORTPMSC;
    UINT32             PORTLI;
    UINT32             PORTHLPMC;
} XHCI_PORT_s;

typedef struct {
    UINT32        IMAN;     //!< [00H] Interrupt Management
    UINT32        IMOD;     //!< [04H] Interrupt Moderation
    UINT32        ERSTSZ;   //!< [08H] Event Ring Segment Table Size
    UINT32        RSVD;     //!< [0CH]
    UINT64        ERSTBA;   //!< [10H-17H] Event Ring Segment Table Base Address
    UINT64        ERDP;     //!< [18H-1FH] Event Ring Dequeue Pointer
} XHCI_INTERRUPT_REG_s;

// Host Controller Capability Registers
typedef struct {
    UINT32   LENGTH_VER; //!< [RO][00H] Capability Register Length and Interface Version Number
    UINT32   HCSPARAMS1; //!< [RO][04H] Structual Parameters 1
    UINT32   HCSPARAMS2; //!< [RO][08H] Structual Parameters 2
    UINT32   HCSPARAMS3; //!< [RO][0CH] Structual Parameters 3
    UINT32   HCCPARAMS1; //!< [RO][10H] Capability Parameters 1
    UINT32   DBOFF;      //!< [RO][14H] Doorbell Offset
    UINT32   RTSOFF;     //!< [RO][18H] Runtime Register Space Offset
    UINT32   HCCPARAMS2; //!< [RO][1CH] Capability Parameters 2
    UINT32   VTIOSOFF;   //!< [RO][20H] Virtualization Based Trusted IO Register Space Offset
} XHCI_CAPABILITY_REG_s;

// Host Controller Operation Registers
typedef struct {
    UINT32          USBCMD;     //!< [00H] USB Command
    UINT32          USBSTS;     //!< [04H] USB Status
    UINT32          PAGESIZE;   //!< [08H] Page Size
    UINT32          RSVD0[2];   //!< [0C-13H] Reserved
    UINT32          DNCTL;      //!< [14H] Device Notification Control
    UINT64          CRCR;       //!< [18H-1FH] Command Ring Control
    UINT32          RSVD1[4];   //!< [20H-2FH]
    UINT64          DCBAAP;     //!< [30H-37H] Device Context Base Address Array Pointer
    UINT32          CONFIG;     //!< [38H] Configure
    UINT32          RSVD2[241]; //!< [3CH-400H]
    XHCI_PORT_s     PORTArray[XHCI_MAX_PORT_NUM]; //!< [400H-] Port Register Set
} XHCI_OPERATION_REG_s;

// Host Controller Runtime Registers
typedef struct {
    UINT32               MFINDEX; //!< [00H] Microframe Index
    UINT32               RSVD[7]; //!< [04H-1FH] Reserved
    XHCI_INTERRUPT_REG_s INTArray[XHCI_MAX_PORT_NUM]; //!< [20H-] Interrupt Register Set
} XHCI_RUNTIME_REG_s;

// Transfer request block structure, spec 4.11.1
typedef struct {
    UINT32 DWord0; //!< TRB parameter 0 */
    UINT32 DWord1; //!< TRB parameter 1 */
    UINT32 DWord2; //!< TRB status
    UINT32 DWord3; //!< TRB control
} XHCI_RING_ELEMENT_s;


#endif
