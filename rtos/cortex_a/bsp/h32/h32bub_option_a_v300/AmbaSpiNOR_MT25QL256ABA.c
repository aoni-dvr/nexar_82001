/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaNOR_MT25Q256ABA.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: NAND device Info
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaTypes.h"
#include "AmbaSpiNOR_Def.h"

extern const AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;
const AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo = {
    .DevName = "Micron MT25QL256ABA_256Mb",    /* NOR_NAME */

    /*--- DevInfo ---*/
    .ManufactuerID = 0x10,             /* Manufactuer ID  */
    .DeviceID = 0xBA,                  /* Devic ID */
    .TotalByteSize = 32 * 1024 * 1024, /* NOR_TOTAL_BYTE_SIZE */
    .PageSize = 256,                   /* NOR_PROGRAM_PAGE_SIZE */
    .EraseBlockSize = 64 * 1024,       /* NOR_BLOCKS_ERASE_SIZE */
    .EraseSectorSize = 4 * 1024,
    .SpiFrequncy = 60000000,           /* NOR SPI FREQUENCY_Mhz */
    .AddrByte = AMBA_NORSPI_4BYTE_ADDR,
    .LSBFirst = 0,                     /* NOR_LSBFirst */

    /*--- DevCMDInfo ---*/
    .Read = {
        0x6B,         /* NOR DDRIO READ CMD*/
        0x8,       /* NOR READ Dummy Cycles*/
        AMBA_NORSPI_QOR_READ,
        AMBA_NORSPI_DATA_4_LANE,
    },
    .Program = {
        0x32,      /* NOR Program Page CMD*/
        0x0,    /* NOR Write Dummy Cycles*/
        AMBA_NORSPI_QOR_WRITE,
        AMBA_NORSPI_DATA_4_LANE,
    },
    .WriteReg = 0x1,       /* NOR Write Regster CMD*/
    .ReadID = 0x9f,        /* NOR ReadID CMD*/
    .ResetEnable = 0x66,   /* NOR Enable Reset */
    .Reset = 0x99,         /* NOR Reset CMD*/
    .ReadStatus0 = 0x5,    /* NOR Read Status0 CMD */
    .ReadStatus1 = 0x7,    /* NOR Read Status1 CMD */
    .ReadFlagStatus = 0x70, /* NOR Read FlagStatus CMD */
    .ClearStatus = 0x50,   /* NOR Clear Status CMD */
    .WriteEnable = 0x6,    /* NOR Write Enable CMD */
    .WriteDisable = 0x4,   /* NOR Write Disable CMD*/
    .EraseBlk = 0xD8,      /* NOR Erase BLOCK CMD*/
    .EraseChip = 0xC7,     /* NOR Erase Chip CMD*/
    .EraseSector = 0x20,
    .Enable4ByteMode = 0xB7,    /* NOR Enter 4Byte Address CMD*/
    .Quad = {
        0x0, /* NOR Enter QUAD Protocol CMD */
        0x0,
        0x0,
        0x0,
        0x0,
        0x0
    },
    .EraseFailFlag = 0x20,
    .ProgramFailFlag = 0x10
};

extern const AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting;

const AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting = {
    .FlowControl = 0x1,       /* Flow Control Enable/Disable */
    .HoldTime = 0x0,          /* Hold Time for Flow Control */
    .TxThresholdLevel = 0x1f, /* Transmit FIFO Threshold Level */
    .RxThresholdLevel = 0x1f, /* Receive FIFO Threshold Level */
    .ChipSelect = 0x0,
    .RxSampleDelay = 0x1,     /* Shift time for RX Sampling phase */
};

