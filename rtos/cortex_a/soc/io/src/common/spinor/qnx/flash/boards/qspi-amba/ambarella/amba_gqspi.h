/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef __XZYNQ_GQSPI_H_INCLUDED
#define __XZYNQ_GQSPI_H_INCLUDED

#include <stdint.h>
#include <sys/cache.h>
#include <sys/slog.h>

/* Registers base address */
//#define XZYNQ_GQSPI_REG_ADDRESS      0xFF0F0100
#define XZYNQ_GQSPI_REG_ADDRESS      0xE0001000
#define XZYNQ_GQSPI_REG_SIZE         0x1000

/* QSPI modes */
#define XZYNQ_GQSPI_DMA_MODE         2
#define XZYNQ_GQSPI_IO_MODE          0

/* Controller clock frequency */
#define XZYNQ_GQSPI_MAX_DRATE        (150 * 1000 * 1000)

/* Interrupt number */
#define XZYNQ_GQSPI_IRQ              105

/* Messages variables */
#define XZYNQ_GQSPI_EVENT            1
#define XZYNQ_GQSPI_ERROR            2
#define XZYNQ_GQSPI_PRIORITY         21

/* Minimal transfer sizes */
#define XZYNQ_GQSPI_MIN_EXP          7
#define XZYNQ_GQSPI_MIN_EXP_SIZE     255
#define XZYNQ_GQSPI_MIN_DMA_SIZE     4
#define XZYNQ_GQSPI_MAX_DMA_SIZE     (1024*1024)

/* Registers offset */
#define XZYNQ_GQSPI_CR_OFFSET               0x00 /**< 32-bit Control */
#define XZYNQ_GQSPI_ISR_OFFSET              0x04 /**< Interrupt Status */
#define XZYNQ_GQSPI_IER_OFFSET              0x08 /**< Interrupt Enable */
#define XZYNQ_GQSPI_IDR_OFFSET              0x0c /**< Interrupt Disable */
#define XZYNQ_GQSPI_IMR_OFFSET              0x10 /**< Interrupt Enabled Mask */
#define XZYNQ_GQSPI_ER_OFFSET               0x14 /**< Enable/Disable Register */
#define XZYNQ_GQSPI_TXD_00_OFFSET           0x1C /**< Transmit 4-byte inst/data */
#define XZYNQ_GQSPI_RXD_OFFSET              0x20 /**< Data Receive Register */
#define XZYNQ_GQSPI_TXWR_OFFSET             0x28 /**< Transmit FIFO Watermark */
#define XZYNQ_GQSPI_RXWR_OFFSET             0x2C /**< Receive FIFO Watermark */
#define XZYNQ_GQSPI_PIO_OFFSET              0x30 /**< General purpose register */
#define XZYNQ_GQSPI_LBKDADJ_OFFSET          0x38 /**< Loopback clock delay adjustment Register */
#define XZYNQ_GQSPI_FIFO_OFFSET             0x40 /**< Generic FIFO Data Register. Keyhole addresses for the Generic FIFO.*/
#define XZYNQ_GQSPI_SEL_OFFSET              0x44 /**< GQSPI Select Register.*/
#define XZYNQ_GQSPI_GCTRL_OFFSET            0x4C /**< FIFO control register.*/
#define XZYNQ_GQSPI_GTHRESH_OFFSET          0x50 /**< FIFO threshold register.*/
#define XZYNQ_GQSPI_POLLCFG_OFFSET          0x54 /**< Poll Configuration Register*/
#define XZYNQ_GQSPI_PTIMEOUT_OFFSET         0x58 /**< Poll Time out Register*/
#define XZYNQ_GQSPI_DATADLYADJ_OFFSET       0xF8 /**< Data delay adjust*/
#define XZYNQ_GQSPI_MODID_OFFSET            0x1FC /**< GQSPI Module Identification register*/
#define XZYNQ_GQSPIDMA_ADDR_OFFSET          0x700 /**< Destination memory address for DMA stream->memory data transfer */
#define XZYNQ_GQSPIDMA_SIZE_OFFSET          0x704 /**< DMA transfer payload for DMA stream-> memory data transfer */
#define XZYNQ_GQSPIDMA_STS_OFFSET           0x708 /**< General DST DMA Status */
#define XZYNQ_GQSPIDMA_CTRL_OFFSET          0x70C /**< General DST DMA Control */
#define XZYNQ_GQSPIDMA_ISR_OFFSET           0x714 /**< DST DMA Interrupt Status Register */
#define XZYNQ_GQSPIDMA_IER_OFFSET           0x718 /**< DST DMA Interrupt Enable */
#define XZYNQ_GQSPIDMA_IDR_OFFSET           0x71C /**<    DST DMA Interrupt Disable */
#define XZYNQ_GQSPIDMA_IMR_OFFSET           0x720 /**<    DST DMA Interrupt Mask */
#define XZYNQ_GQSPIDMA_CTRL2_OFFSET         0x724 /**< General DST DMA Control Register 2 */
#define XZYNQ_GQSPIDMA_ADDR_MSB_OFFSET      0x728 /**< Destination memory address (msbs) for DMA stream->memory data transfer */

/* Control register */
#define XZYNQ_GQSPI_CR_MODE_MASK            0xC0000000   /**< Flash mode */
#define XZYNQ_GQSPI_CR_MODE_SHIFT           30
#define XZYNQ_GQSPI_CR_MODE(val)            ((val & 0x3) << XZYNQ_GQSPI_CR_MODE_SHIFT)
#define XZYNQ_GQSPI_CR_START_MODE_MASK      0x20000000   /**< Flash start mode. 1 - manual, 0 - auto */
#define XZYNQ_GQSPI_CR_GEN_FIFO_MASK        0x10000000   /**< Enable/disable generic FIFO */
#define XZYNQ_GQSPI_CR_ENDIAN_MASK          0x04000000   /**< 0 for LE, 1 for BE */
#define XZYNQ_GQSPI_CR_EN_POLL_TO_MASK      0x00100000   /**< Enable/disable poll timeout */
#define XZYNQ_GQSPI_CR_WPHOLD_MASK          0x00080000   /**< If set, WP pins driven by QSPI module */
#define XZYNQ_GQSPI_CR_BRDIV_MASK           0x00000038   /**< Baudrate divisor */
#define XZYNQ_GQSPI_CR_BRDIV_SHIFT          3         /**< Prescaler shift */
#define XZYNQ_GQSPI_CR_BRDIV_MAXIMUM        0x07      /**< Prescaler maximum value */
#define XZYNQ_GQSPI_CR_CPHA_MASK            0x00000004   /**< Phase Configuration */
#define XZYNQ_GQSPI_CR_CPOL_MASK            0x00000002   /**< Polarity Configuration */
#define XZYNQ_GQSPI_DFLT_BRDIV              (0x2<<3)
#define XZYNQ_GQSPI_CR_BRDIV(div)           ((div & 0xF) << XZYNQ_GQSPI_CR_BRDIV_SHIFT)
#define XZYNQ_GQSPI_BRX8                    0x2

/* Initial control register state */
#define XZYNQ_GQSPI_CR_RESET_STATE   (XZYNQ_GQSPI_CR_MODE(XZYNQ_GQSPI_IO_MODE) |\
                            XZYNQ_GQSPI_CR_GEN_FIFO_MASK  |\
                            XZYNQ_GQSPI_CR_WPHOLD_MASK |\
                            XZYNQ_GQSPI_DFLT_BRDIV)

#define XZYNQ_GQSPI_DMA_CTRL_RESET_STATE       0x803FFA00
#define XZYNQ_GQSPI_DMA_CTRL2_RESET_STATE      0x081BFFF8

/* Interrupt Registers */
#define XZYNQ_GQSPI_IXR_RXEMPTY_MASK      0x00000800  /**< GQSPI RX FIFO empty */
#define XZYNQ_GQSPI_IXR_GFIFOFULL_MASK    0x00000400  /**< GQSPI Generic FIFO is full */
#define XZYNQ_GQSPI_IXR_GFIFONFULL_MASK   0x00000200  /**< GQSPI Generic FIFO is not full */
#define XZYNQ_GQSPI_IXR_TXEMPTY_MASK      0x00000100  /**< GQSPI TX FIFO is empty */
#define XZYNQ_GQSPI_IXR_GFIFOEMPTY_MASK   0x00000080  /**< GQSPI Generic FIFO empty interrupt */
#define XZYNQ_GQSPI_IXR_RXFULL_MASK       0x00000020  /**< GQSPI Rx FIFO Full */
#define XZYNQ_GQSPI_IXR_RXNEMPTY_MASK     0x00000010  /**< GQSPI Rx FIFO Not Empty */
#define XZYNQ_GQSPI_IXR_TXFULL_MASK       0x00000008  /**< GQSPI Tx FIFO Full */
#define XZYNQ_GQSPI_IXR_TXNFULL_MASK      0x00000004  /**< GQSPI Tx FIFO Overwater */
#define XZYNQ_GQSPI_IXR_PTO_MASK          0x00000002  /**< GQSPI poll timeout */
#define XZYNQ_GQSPI_IXR_DFLT_MASK         (XZYNQ_GQSPI_IXR_TXNFULL_MASK | XZYNQ_GQSPI_IXR_RXNEMPTY_MASK | XZYNQ_GQSPI_IXR_GFIFONFULL_MASK)
#define XZYNQ_GQSPI_ISR_ALL_MASK          0x00000FBF
#define XZYNQ_GQSPI_IXR_ALL_FIFO_EMPTY    (XZYNQ_GQSPI_IXR_RXEMPTY_MASK | XZYNQ_GQSPI_IXR_TXEMPTY_MASK | XZYNQ_GQSPI_IXR_GFIFOEMPTY_MASK)
/* Enable Register */
#define XZYNQ_GQSPI_ER_ENABLE_MASK        0x00000001 /**< QSPI Enable Bit Mask */

/* Transmit FIFO Watermark Register */
#define XZYNQ_QSPI_TXWR_MASK              0x0000003F   /**< Transmit Watermark Mask */

/* Generic QSPI selection register fields */
#define XZYNQ_GQSPI_SEL_GEN               0x1


/* Delay registers fields for loopback clock */
#define XZYNQ_GQSPI_LBKDADJ_EN             0x00000020
#define XZYNQ_GQSPI_LBKDADJ_DLY1_MASK      0x00000018
#define XZYNQ_GQSPI_LBKDADJ_DLY1_OFFSET    3
#define XZYNQ_GQSPI_LBKDADJ_DLY1(val)      ((val & 0x3) << XZYNQ_GQSPI_LBKDADJ_DLY1_OFFSET)
#define XZYNQ_GQSPI_LBKDADJ_DLY1_MAX       3
#define XZYNQ_GQSPI_LBKDADJ_DLY0_MASK      0x00000007
#define XZYNQ_GQSPI_LBKDADJ_DLY0(val)      (val & 0x7)
#define XZYNQ_GQSPI_LBKDADJ_DLY0_MAX       0x7

#define XZYNQ_GQSPI_DATADLYADJ_EN             0x80000000
#define XZYNQ_GQSPI_DATADLYADJ_DLY_MASK       0x70000000
#define XZYNQ_GQSPI_DATADLYADJ_DLY_OFFSET     28
#define XZYNQ_GQSPI_DATADLYADJ_DLY(val)       ((val & 7) << XZYNQ_GQSPI_DATADLYADJ_DLY_OFFSET)
#define XZYNQ_GQSPI_DATADLYADJ_DLY_MAX        7
#define XZYNQ_GQSPI_DATADLYADJ_DLY_MAX_RATE   3


/* Generic QSPI POLL configuration register field */
#define XZYNQ_GQSPI_POLLCFG_UPPER_MASK      0x80000000
#define XZYNQ_GQSPI_POLLCFG_LOWER_MASK      0x40000000
#define XZYNQ_GQSPI_POLLCFG_MASK_EN_MASK    0x0000FF00
#define XZYNQ_GQSPI_POLLCFG_MASK_EN_OFFSET  8
#define XZYNQ_GQSPI_POLLCFG_MASK_EN(val)    (val << XZYNQ_GQSPI_POLLCFG_MASK_EN_OFFSET)
#define XZYNQ_GQSPI_POLLCFG_MASK_VAL_MASK   0x000000FF

#define XZYNQ_GQSPI_POLL_DEFAULT_TIMEOUT    0xFFFFF


/* FIFO control register fields */
#define XZYNQ_GQSPI_GCTRL_RST_GFIFO         0x1
#define XZYNQ_GQSPI_GCTRL_RST_TXFIFO        (0x1<<1)
#define XZYNQ_GQSPI_GCTRL_RST_RXFIFO        (0x1<<2)

/* Generic FIFO fields */
#define XZYNQ_GQSPI_FIFO_POLL           0x00080000
#define XZYNQ_GQSPI_FIFO_STRIPE         0x00040000
#define XZYNQ_GQSPI_FIFO_RECEIVE        0x00020000
#define XZYNQ_GQSPI_FIFO_TRANSMIT       0x00010000
#define XZYNQ_GQSPI_FIFO_DBSEL_MASK     0x0000C000
#define XZYNQ_GQSPI_FIFO_DBSEL_SHIFT    14
#define XZYNQ_GQSPI_FIFO_CSUPPER        0x00002000
#define XZYNQ_GQSPI_FIFO_CSLOWER        0x00001000
#define XZYNQ_GQSPI_FIFO_SPIMODE_MASK   0x00000C00
#define XZYNQ_GQSPI_FIFO_SPIMODE_SHIFT  10
#define XZYNQ_GQSPI_FIFO_EXPONENT       0x00000200
#define XZYNQ_GQSPI_FIFO_DATAXFER       0x00000100
#define XZYNQ_GQSPI_FIFO_IMMDATA_MASK   0xFF


/* DMA interrupt register fields */
#define XZYNQ_GQSPI_DMA_IXR_DONE            0x00000002
#define XZYNQ_GQSPI_DMA_IXR_AXI_BRESP_ERR   0x00000004
#define XZYNQ_GQSPI_DMA_IXR_TIMEOUT_STRM    0x00000008
#define XZYNQ_GQSPI_DMA_IXR_TIMEOUT_MEM     0x00000010
#define XZYNQ_GQSPI_DMA_IXR_THRESH_HIT      0x00000020
#define XZYNQ_GQSPI_DMA_IXR_INVALID_APB     0x00000040
#define XZYNQ_GQSPI_DMA_IXR_FIFO_OVERFLOW   0x00000080
#define XZYNQ_GQSPI_DMA_IXR_ALL_MASK        0xFE
#define XZYNQ_GQSPI_DMA_IXR_DFLT_MASK       XZYNQ_GQSPI_DMA_IXR_DONE

/* DMA control register fields */
#define XZYNQ_GQSPIDMA_CTRL_PAUSE_MEM_MASK       0x00000001
#define XZYNQ_GQSPIDMA_CTRL_PAUSE_STRM_MASK      0x00000002
#define XZYNQ_GQSPIDMA_CTRL_FIFO_THRESH_MASK     0x000003FC
#define XZYNQ_GQSPIDMA_CTRL_FIFO_THRESH_SHIFT    2
#define XZYNQ_GQSPIDMA_CTRL_FIFO_THRESH(val)     ((val & 0xFF) << XZYNQ_GQSPIDMA_CTRL_FIFO_THRESH_SHIFT)
#define XZYNQ_GQSPIDMA_CTRL_TIMEOUT_VAL          0x003FFC00
#define XZYNQ_GQSPIDMA_CTRL_TIMEOUT_SHIFT        10
#define XZYNQ_GQSPIDMA_CTRL_TIMEOUT(val)         ((val & 0xFFF) << XZYNQ_GQSPIDMA_CTRL_TIMEOUT_SHIFT)
#define XZYNQ_GQSPIDMA_CTRL_AXI_BRST_TYPE_MASK   0x00400000
#define XZYNQ_GQSPIDMA_CTRL_ENDIANESS_MASK       0x00800000
#define XZYNQ_GQSPIDMA_CTRL_APB_ERR_RESP_MASK    0x01000000
#define XZYNQ_GQSPIDMA_CTRL_FIFO_LVL_HIT_MASK    0xFE000000
#define XZYNQ_GQSPIDMA_CTRL_FIFO_LVL_HIT_SHIFT   25
#define XZYNQ_GQSPIDMA_CTRL_FIFO_LVL_HIT(val)    ((val & 0x7F) << XZYNQ_GQSPIDMA_CTRL_FIFO_LVL_HIT_SHIFT)

/* DMA control register2 fields */
#define XZYNQ_GQSPIDMA_CTRL2_AWCACHE_MASK        0x07000000
#define XZYNQ_GQSPIDMA_CTRL2_AWCACHE_SHIFT       24
#define XZYNQ_GQSPIDMA_CTRL2_AWCACHE(val)        ((val << XZYNQ_GQSPIDMA_CTRL2_AWCACHE_SHIFT) & XZYNQ_GQSPIDMA_CTRL2_AWCACHE_MASK)
#define XZYNQ_GQSPIDMA_CTRL2_TIMEOUTEN           0x00400000
#define XZYNQ_GQSPIDMA_CTRL2_TIMEOUT_PRE_MASK    0x0000FFF0
#define XZYNQ_GQSPIDMA_CTRL2_TIMEOUT_PRE_SHIFT   4
#define XZYNQ_GQSPIDMA_CTRL2_TIMEOUT_PRE(val)    ((val << XZYNQ_GQSPIDMA_CTRL2_TIMEOUT_PRE_SHIFT) & XZYNQ_GQSPIDMA_CTRL2_TIMEOUT_PRE_MASK)
#define XZYNQ_GQSPIDMA_CTRL2_MAX_OUTS_CMD_MASK   0x0000000F
#define XZYNQ_GQSPIDMA_CTRL2_MAX_OUTS_CMD(val)   (val & XZYNQ_GQSPIDMA_CTRL2_MAX_OUTS_CMD_MASK)

/* DMA status register */
#define XZYNQ_GQSPIDMA_STS_DONE_CNT_MASK       0x0000E000
#define XZYNQ_GQSPIDMA_STS_DONE_CNT_SHIFT      13
#define XZYNQ_GQSPIDMA_STS_DONE_CNT_GET(v)     ((v >> XZYNQ_GQSPIDMA_STS_DONE_CNT_SHIFT) & 0x7)
#define XZYNQ_GQSPIDMA_STS_DONE_CNT(v)         ((v << XZYNQ_GQSPIDMA_STS_DONE_CNT_SHIFT) & XZYNQ_GQSPIDMA_STS_DONE_CNT_MASK)
#define XZYNQ_GQSPIDMA_STS_BUSY                1

/* Calculatable fields in FIFO commands*/
#define XZYNQ_GQSPI_FIFO_DBSEL(mode)    ((mode & 0x3) << XZYNQ_GQSPI_FIFO_DBSEL_SHIFT)
#define XZYNQ_GQSPI_FIFO_DBMODE_LOWER   1
#define XZYNQ_GQSPI_FIFO_DBMODE_UPPER   2
#define XZYNQ_GQSPI_FIFO_DBMODE_BOTH    3

#define XZYNQ_GQSPI_FIFO_SPIMODE(mode)   ((mode & 0x3) << XZYNQ_GQSPI_FIFO_SPIMODE_SHIFT)
#define XZYNQ_GQSPI_FIFO_SPIMODE_SPI     1
#define XZYNQ_GQSPI_FIFO_SPIMODE_DSPI    2
#define XZYNQ_GQSPI_FIFO_SPIMODE_QSPI    3


#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

/* Default controller settings */
#define XZYNQ_GQSPI_DEFAULT_BUS           XZYNQ_GQSPI_FIFO_DBMODE_BOTH
#define XZYNQ_GQSPI_DEFAULT_SPI_MODE      XZYNQ_GQSPI_FIFO_SPIMODE_SPI
#define XZYNQ_GQSPI_DEFAULT_BOARD_CLOCK   166666666
#define XZYNQ_GQSPI_DEFAULT_SPEED         0
#define XZYNQ_GQSPI_DEFAULT_TRESHOLD      1
#define XZYNQ_GQSPI_DEFAULT_CMD_BUF_SIZE  255

/* Flags to transfer function */
#define TRANSFER_FLAG_LOW_DB          (1) /*< Using lower device(override other settings) */
#define TRANSFER_FLAG_UP_DB           (1<<1)/*< Using upper device(override other settings) */
#define TRANSFER_FLAG_DB_MASK         (0x3)
#define TRANSFER_FLAG_DEBUG           (1<<2)
#define TRANSFER_FLAG_DEBUG2          ((1<<9) | (1<<2))
#define TRANSFER_FLAG_STRIPE          (1<<3)
#define TRANSFER_FLAG_MODE_SHIFT      4
#define TRANSFER_FLAG_MODE_MASK       (0x3 << TRANSFER_FLAG_MODE_SHIFT)
#define TRANSFER_FLAG_MODE(v)         (v << TRANSFER_FLAG_MODE_SHIFT)
#define TRANSFER_FLAG_MODE_QSPI       1
#define TRANSFER_FLAG_MODE_DSPI       2
#define TRANSFER_FLAG_MODE_SPI        3
#define TRANSFER_FLAG_CS_SHIFT        6
#define TRANSFER_FLAG_CS_LOW          1
#define TRANSFER_FLAG_CS_UP           2
#define TRANSFER_FLAG_CS_BOTH         3
#define TRANSFER_FLAG_CS(v)           (v << TRANSFER_FLAG_CS_SHIFT)
#define TRANSFER_FLAG_CS_MASK         TRANSFER_FLAG_CS(3)
#define TRANSFER_FLAG_WAIT_WP_BIT     (1<<10)

#define xzynq_cache_flush(dev,buf)   CACHE_FLUSH(&(dev->cinfo), (buf)->offset, (buf)->phys_base, (buf)->len);
#define xzynq_cache_inval(dev,buf)   CACHE_INVAL(&(dev->cinfo), (buf)->offset, (buf)->phys_base, (buf)->len);

typedef struct {
    uint8_t* offset;
    int len;
    int dummy_bytes;
    off64_t phys_base;
} qspi_buf;

typedef struct {
    unsigned    pbase;
    uintptr_t   vbase;
    int         irq;
    int         iid;
    int         chid, coid;
    int         board_clock;
    int         speed;
    int         busy;
    uint8_t     mode;
    uint8_t     treshold;
    uint32_t    flags;
#define XZYNQ_FLAG_CS_LOWER     (1 << 0)
#define XZYNQ_FLAG_CS_UPPER     (1 << 1)
#define XZYNQ_FLAG_CS_STRIPE    (1 << 2)
#define XZYNQ_FLAG_CS_MASK      (XZYNQ_FLAG_CS_LOWER | XZYNQ_FLAG_CS_UPPER | XZYNQ_FLAG_CS_STRIPE)
#define XZYNQ_FLAG_DB_LOWER     (1 << 4)
#define XZYNQ_FLAG_DB_UPPER     (1 << 5)
#define XZYNQ_FLAG_DB_STRIPE    (1 << 6)
#define XZYNQ_FLAG_DB_MASK      (XZYNQ_FLAG_DB_LOWER | XZYNQ_FLAG_DB_UPPER | XZYNQ_FLAG_DB_STRIPE)
//   uint32_t    ier;
//   uint32_t    dma_ier;

    qspi_buf dma_buf;
    qspi_buf send_buf;
    qspi_buf recv_buf;

    uint32_t cmd_buf[XZYNQ_GQSPI_DEFAULT_CMD_BUF_SIZE];
    int cmd_idx;
    int remaining_cmds;

    uint32_t transfer_flags;

    struct sigevent qspievent;
    struct cache_ctrl cinfo;
    int clk_dev;

    int page_size;
} xzynq_qspi_t;

int xzynq_qspi_transfer(xzynq_qspi_t*, qspi_buf* send_buf, qspi_buf* recv_buf, qspi_buf* cmd_buf, uint32_t flags);
int xzynq_qspi_setcfg(xzynq_qspi_t*, int mode, int drate);
int xzynq_qspi_cmd_read(const xzynq_qspi_t*, uint8_t cmd[6], uint8_t* buf, int len, int dummy_read, uint32_t flags);
int xzynq_qspi_cmd_write(const xzynq_qspi_t*, uint8_t cmd[5], uint8_t const* buf, int len, int dummy_write, uint32_t flags);
int xzynq_qspi_word_exchange(const xzynq_qspi_t*, uint32_t* buf, uint32_t flags);
int xzynq_qspi_write_1byte(const xzynq_qspi_t*, uint8_t cmd, uint32_t flags);
xzynq_qspi_t *xzynq_qspi_open(void);
int xzynq_qspi_close(void* fd);


#define devdbg(...)   if(dev->transfer_flags & TRANSFER_FLAG_DEBUG)    fprintf(stderr,__VA_ARGS__);

#define UINT32 uint32_t

#endif /* __XZYNQ_QSPI_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/boards/qspi-xzynq/aarch64/le.zcu102/xzynq_gqspi.h $ $Rev: 864713 $");
#endif
