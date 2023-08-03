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

#ifndef __AMBARELLA_SPI_H_INCLUDED
#define __AMBARELLA_SPI_H_INCLUDED

#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/spi-master.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include "AmbaSPI_Def.h"


#define AMBA_SPI_PRIORITY                    21
#define AMBA_SPI_EVENT                        1
#define AMBA_SPI_REGLEN                  0x1000
#define AMBA_SPI_BAUDRATE_DEFAULT      10000000

#define AMBA_SPI_SLAVE_DEVICE                 1

#define AMBA_SPI_MASTER_FIFO_COUNT          64U
#define NUM_OF_SPI_DEVS                       4


typedef enum {
    AMBA_SPI_STATUS_DONE =  0x1,   /* transaction completed (with or without error) */
    AMBA_SPI_STATUS_ARBL =  0x2,   /* lost arbitration */
} amba_spi_status_t;


/* The structure which maintains the various parameters
 * of the SPI module.
 */
typedef struct {
    SPIDEV      spi;        /* This has to be the first element */

    uint64_t    pbase;
    uintptr_t   vbase;

    int         irq_spi;
    int         chid, coid;
    int         iid_spi;

    uint32_t    BaudRate;
    uint32_t    ClkMode;
    int         FrameBitOrder;
    int         FrameSize;
    int         CsPolarity; /* Force chip select */
    int         Channel;
    unsigned int SourceClk;

    struct sigevent spievent;

    uint8_t     *pbuf;
    int         xlen, tlen, rlen;
    int         dlen;
    int         dtime;      /* usec per data, for time out use */

    int         ctrl[NUM_OF_SPI_DEVS];
    amba_spi_status_t status;
} amba_spi_t;


/* ======== DCMD_SPI_MST_XFER_D8 ========= */
typedef struct _mst_xfer_d8_in {
    UINT32 SlaveMask;
    AMBA_SPI_CONFIG_s Config;
    UINT32 BufSize;
    UINT8 TxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 TimeOut;
} mst_xfer_d8_in_t;

typedef struct _mst_xfer_d8_out {
    UINT8 RxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 ActualSize;
} mst_xfer_d8_out_t;

typedef union _mst_xfer_d8_msg {
    mst_xfer_d8_in_t In;
    mst_xfer_d8_out_t Out;
} mst_xfer_d8_msg_t;

/* ======== DCMD_SPI_MST_XFER_D16 ========= */
typedef struct _mst_xfer_d16_in {
    UINT32 SlaveMask;
    AMBA_SPI_CONFIG_s Config;
    UINT32 BufSize;
    UINT16 TxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 TimeOut;
} mst_xfer_d16_in_t;

typedef struct _mst_xfer_d16_out {
    UINT16 RxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 ActualSize;
} mst_xfer_d16_out_t;

typedef union _mst_xfer_d16_msg {
    mst_xfer_d16_in_t In;
    mst_xfer_d16_out_t Out;
} mst_xfer_d16_msg_t;

/* ======== DCMD_SPI_MST_INFO_GET ========= */
typedef union _mst_info_get_msg {
    UINT32 NumSlaves;
    UINT32 Status;
} mst_info_get_msg_t;

/* ======== DCMD_SPI_SLV_XFER_D8 ========= */
typedef struct _slv_xfer_d8_in {
    UINT32 BufSize;
    UINT8 TxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 TimeOut;
} slv_xfer_d8_in_t;

typedef struct _slv_xfer_d8_out {
    UINT8 RxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 ActualSize;
} slv_xfer_d8_out_t;

typedef union _slv_xfer_d8_msg {
    slv_xfer_d8_in_t In;
    slv_xfer_d8_out_t Out;
} slv_xfer_d8_msg_t;

/* ======== DCMD_SPI_SLV_XFER_D16 ========= */
typedef struct _slv_xfer_d16_in {
    UINT32 BufSize;
    UINT16 TxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 TimeOut;
} slv_xfer_d16_in_t;

typedef struct _slv_xfer_d16_out {
    UINT16 RxBuf[AMBA_SPI_MASTER_FIFO_COUNT];
    UINT32 ActualSize;
} slv_xfer_d16_out_t;

typedef union _slv_xfer_d16_msg {
    slv_xfer_d16_in_t In;
    slv_xfer_d16_out_t Out;
} slv_xfer_d16_msg_t;

/* ======== DCMD_SPI_SLV_INFO_GET ========= */
typedef union _slv_info_get_msg {
    UINT32 Status;
} slv_info_get_msg_t;

/* ======== DCMD_SPI_SLV_CFG ========= */
typedef struct _slv_cfg_msg {
    UINT32 SlaveID;
    AMBA_SPI_CONFIG_s Config;
} slv_cfg_msg_t;


/*
 * The following devctls are used by a client application
 * to control the SPI interface.
 */
#include <devctl.h>

#define _DCMD_SPI   _DCMD_MISC

#define DCMD_SPI_MST_XFER_D8            __DIOTF(_DCMD_SPI, 0, mst_xfer_d8_msg_t)
#define DCMD_SPI_MST_XFER_D16           __DIOTF(_DCMD_SPI, 1, mst_xfer_d16_msg_t)
#define DCMD_SPI_MST_INFO_GET           __DIOTF(_DCMD_SPI, 2, mst_info_get_msg_t)
#define DCMD_SPI_SLV_XFER_D8            __DIOTF(_DCMD_SPI, 3, slv_xfer_d8_msg_t)
#define DCMD_SPI_SLV_XFER_D16           __DIOTF(_DCMD_SPI, 4, slv_xfer_d16_msg_t)
#define DCMD_SPI_SLV_INFO_GET           __DIOTF(_DCMD_SPI, 5, slv_info_get_msg_t)
#define DCMD_SPI_SLV_CFG                __DIOT(_DCMD_SPI, 6, slv_cfg_msg_t)

#endif /* __AMBARELLA_SPI_H_INCLUDED */
