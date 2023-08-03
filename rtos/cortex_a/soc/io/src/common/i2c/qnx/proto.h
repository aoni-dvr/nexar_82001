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

#ifndef __PROTO_H_INCLUDED
#define __PROTO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <sys/hwinfo.h>
#include <drvr/hwinfo.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <arm/ambarella.h>
#include <fcntl.h>

#include "AmbaCortexA53.h"
#include "AmbaINT_Def.h"

#ifndef AMBA_I2C_DEF_H
#include "AmbaI2C_Def.h"
#endif

#include "AmbaRTSL_I2C.h"
#include "AmbaCSL_I2C.h"

typedef struct _amba_i2c_dev_t {
    unsigned reglen;
    uintptr_t regbase;
    unsigned physbase;
    unsigned ChannelId;

    unsigned slave_addr;

    int intr;
    int iid;
    struct sigevent intrevent;
    i2c_status_t status;
    int stop;

    unsigned RxStatus;
    uint8_t *Rxbuf;
    uint8_t *Txbuf;
    unsigned size_to_send;
    unsigned size_to_receive;

    unsigned speed;
    unsigned verbose;
} amba_i2c_dev_t;

#define MASTER_SEND       0x00000001
#define MASTER_RECEIVE    0x00000002

#define AMBA_I2C_MASTER_FIFO_COUNT   64U

#define AMBA_I2C_REG_SIZE 0x1000
/************************** Constant Definitions *****************************/
/** @name Register Map
 *
 * Register offsets for the I2C.
 */
#define AMBA_I2C_ENABLE_OFFSET              0x00  /**< Enable Register */
#define AMBA_I2C_CTRL_OFFSET                0x04  /**< Control Register */
#define AMBA_I2C_DATA_OFFSET                0x08  /**< Data Registe */
#define AMBA_I2C_STATUS_OFFSET              0x0C  /**< Status Register: 0 = Tx, 1 = Rx */
#define AMBA_I2C_PRESCALER_LOW_OFFSET       0x10  /**< Prescaler Register (Low byte) */
#define AMBA_I2C_PRESCALER_HIGH_OFFSET      0x14  /**< Prescaler Register (High byte) */
#define AMBA_I2C_FIFO_MODE_CTRL_OFFSET      0x18  /**< FIFO Turbo Mode Control Register */
#define AMBA_I2C_FIFO_MODE_DATA_OFFSET      0x1C  /**< FIFO Turbo Mode Data Register (LSB 8-bit) */
#define AMBA_I2C_PRESCALER_TURBO_OFFSET     0x20  /**< Prescaler Register for Turbo mode */
#define AMBA_I2C_DUTY_CYCLE_OFFSET          0x24  /**< Standard mode (non-Turbo) Duty cycle Register */
#define AMBA_I2C_DELAY_OFFSET               0x28  /**< Delay Register */

/** @name Enable Register
 */
#define AMBA_I2C_ENR_ENABLE_MASK        0x00000001  /* [0]: 1 - I2C Enabled; 0 - Disable */
#define AMBA_I2C_ENR_SCDSEN_MASK        0x00000002  /* [1]: 1 - Scdc Enabled; 0 - Disable */

/** @name Control Register
 *
 * This register contains various control bits that
 * affects the operation of the I2C controller. Read/Write.
 * @{
 */
#define AMBA_I2C_CR_ACK_MASK            0x00000001  /* [0]: Write: 0 - ACK, 1 - NACK */
#define AMBA_I2C_CR_IRQ_FLAG_MASK       0x00000002  /* [1]: 0 = No interrupt, 1= Interrupt pending */
#define AMBA_I2C_CR_START_MASK          0x00000004  /* [2]: 1 = The next process generates a START condition */
#define AMBA_I2C_CR_STOP_MASK           0x00000008  /* [3]: 1 = The next process generates a STOP condition */
#define AMBA_I2C_CR_HS_MODE_MASK        0x00000010  /* [4]: 1 = Enables High-Speed Mode */
#define AMBA_I2C_CR_CLEAN_FIFO_MASK     0x00000020  /* [5]: 1 = Clear 63-entry FIFO used during turbo mode */
#define AMBA_I2C_CR_IRQSCDS_MASK        0x00000040  /* [6]: 1 = Clear */
#define AMBA_I2C_ACK                    0x0
#define AMBA_I2C_NACK                   0x1
#define AMBA_I2C_STOP                   0x8
/** @name Duty cycle Register
 */
#define AMBA_I2C_DCR_MODE_MASK          0x00000003  /* [1:0]: Duty Cycle setting for standard mode 0-1:1, 1-2:3, 2-1:2 */
#define AMBA_I2C_DCR_SCLCUR_EN_MASK     0x00000004  /* [2]: 1 - SCL (clock line) pin current source enable */
#define AMBA_I2C_DCR_SDACUR_EN_MASK     0x00000008  /* [3]: 1 - SDA (Data line) pin current source enable */

/** @name Data Register
 */
#define AMBA_I2C_DATA_MASK              0x000000FF  /* [7:0]: Data Value */

/** @name Status Register
 */
#define AMBA_I2C_SR_OPMODE_MASK        0x00000001  /* [0]: 0 - Master Tx; 1 - Master Rx */
#define AMBA_I2C_SR_FIFOFULL_MASK      0x00000002  /* [1]: 1 = Fifo Full */
#define AMBA_I2C_SR_FIFOEMPTY_MASK     0x00000004  /* [2]: 1 = Fifo empty */
#define AMBA_I2C_SR_STATUS_MASK        0x000000F0  /* [7:4]: IDC Finite State Machine (FSM) status */
#define AMBA_I2C_SR_FIFO_COUNT_MASK    0x00003F00  /* [13:8]: FIFO count of empty entries */

/** @name Fifo Mode Control Register
 */
#define AMBA_I2C_FIFO_MODE_CR_IRQ_MASK          0x00000002  /* [1]: 1 - Assert interrupt at transmission completed */
#define AMBA_I2C_FIFO_MODE_CR_START_MASK        0x00000004  /* [2]: 1 - The next process generates a START condition */
#define AMBA_I2C_FIFO_MODE_CR_STOP_MASK         0x00000008  /* [3]: 1 = The next process generates a STOP condition */
#define AMBA_I2C_FIFO_MODE_CR_HS_MODE_MASK      0x00000010  /* [4]: 1 = Enables High-Speed Mode */

void *Amba_I2c_init(int argc, char *argv[]);
void Amba_I2c_Abort(void *hdl, int rcvid);
void Amba_I2c_fini(void *hdl);
int AMBA_I2c_options(amba_i2c_dev_t *dev, int argc, char *argv[]);

int Amba_I2c_SetSlaveAddr(void *hdl, unsigned int addr, i2c_addrfmt_t fmt);
int Amba_I2c_SetSpeed(void *hdl, unsigned int speed, unsigned int *ospeed);
int Amba_I2C_VerInfo(i2c_libversion_t *version);
int Amba_I2C_DriverInfo(void *hdl, i2c_driver_info_t *info);
i2c_status_t Amba_I2c_recv(void *hdl, void *buf, unsigned int len, unsigned int stop);
i2c_status_t Amba_I2c_send(void *hdl, void *buf, unsigned int len, unsigned int stop);
const struct sigevent *Amba_I2c_intr(void *area, int id);
void Amba_I2c_WaitComplete(amba_i2c_dev_t *dev);
void Amba_I2c_reset(amba_i2c_dev_t* dev);
uint32_t Amba_wait_bus_not_busy(amba_i2c_dev_t *dev);
int i2c_master_getfuncs(i2c_master_funcs_t *funcs, int tabsize);

#endif

