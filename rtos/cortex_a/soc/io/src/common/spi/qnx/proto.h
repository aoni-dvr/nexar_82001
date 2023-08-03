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
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/procmgr.h>
#include <drvr/hwinfo.h>

#include "AmbaSPI_Def.h"
#include "AmbaReg_SPI.h"
#include "AmbaReg_RCT.h"

#include "hw/ambarella_spi.h"

#define AMBA_SPI_CLOCK_VALUE                120000000U
#define AMBA_RCT_BASE_ADDR                  0xed080000
#define AMBA_RCT_MMAP_SIZE                      0x1000

/* Defined in lib.c */
int spi_dev_init(int chan, int slave);
int spi_dev_dinit(void);

/* SPI master channels and SPI slave */
int spi_io_devctl0(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
int spi_io_devctl1(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
int spi_io_devctl2(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
int spi_io_devctl3(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
int spi_io_devctl4(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
int spi_io_devctl5(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
#endif
int spi_io_devctl_slave(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);


int spi_MasterTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                         UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
int spi_MasterTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                          UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
int spi_MasterGetInfo(UINT32 MasterID, UINT32 *pNumSlaves, UINT32 *pStatus);

int spi_SlaveTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
int spi_SlaveTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
int spi_SlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus);
int spi_SlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s * pConfig);

/* Defined in Intr.c */
int Amba_Attach_Intr(amba_spi_t *dev);

/* Defined in Wait.c */
int Amba_Wait(amba_spi_t *dev, int len);

#endif

