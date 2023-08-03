/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaSpiNOR_MT35XU01GBBA1G.c
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: SPINOR device Info
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaTypes.h"
#include "AmbaSpiNOR_Def.h"

extern AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo;
AMBA_NORSPI_DEV_INFO_s AmbaNORSPI_DevInfo = {
    .DevName = "Micron MT35XUL01GBBA_128MB",    /* NOR_NAME */

    /*--- DevInfo ---*/
    .ManufactuerID   = 0x2C,             /* Manufactuer ID  */
    .DeviceID        = 0xBA,             /* Devic ID */
    .TotalByteSize   = 128 * 1024 * 1024, /* NOR_TOTAL_BYTE_SIZE */
    .PageSize        = 256,              /* NOR_PROGRAM_PAGE_SIZE */
    .EraseBlockSize  = 128 * 1024,       /* NOR_BLOCKS_ERASE_SIZE */
    .EraseSectorSize = 4 * 1024,
    .SpiFrequncy     = 100000000,        /* NOR SPI FREQUENCY_Mhz */
    .AddrByte        = AMBA_NORSPI_4BYTE_ADDR,
    .LSBFirst        = 0,                /* NOR_LSBFirst */

    /*--- DevCMDInfo ---*/
    .Read = {
        0x7c,                     /* CmdIndex */
#ifdef CONFIG_ENABLE_AMBALINK
        8,                        /* DummyCycles*/
        AMBA_NORSPI_READ,         /* CmdType */
#else
        16,                       /* DummyCycles*/
        AMBA_NORSPI_OCT_READ,     /* CmdType */
#endif
        AMBA_NORSPI_DATA_8_LANE,  /* DataLane */
    },
    .Program = {
        0x84,                     /* CmdIndex */
        0x0,                      /* DummyCycles*/
#ifdef CONFIG_ENABLE_AMBALINK
        AMBA_NORSPI_WRITE,        /* CmdType */
#else
        AMBA_NORSPI_OCT_WRITE,    /* CmdType */
#endif
        AMBA_NORSPI_DATA_8_LANE,  /* DataLane */
    },

    .ResetEnable  = 0x66,    /* NOR Enable Reset */
    .Reset        = 0x99,    /* NOR Reset CMD */
    .WriteEnable  = 0x6,     /* NOR Write Enable CMD */
    .WriteDisable = 0x4,     /* NOR Write Disable CMD */

    .WriteReg       = 0x1,   /* NOR Write Regster CMD */
    .ReadID         = 0x9e,  /* NOR ReadID CMD */
    .ReadStatus0    = 0x5,   /* NOR Read Status0 CMD */
    .ReadStatus1    = 0x7,   /* NOR Read Status1 CMD */
    .ReadFlagStatus = 0x70,
    .ClearStatus    = 0x50,  /* NOR Clear Status CMD */

    .EraseBlk        = 0xDC, /* NOR Erase BLOCK CMD */
    .EraseSector     = 0x21,
    .EraseChip       = 0x0,  /* NOR Erase Chip CMD */
    .Enable4ByteMode = 0xB7, /* NOR Enter 4Byte Address CMD */

    .EraseFailFlag   = 0x20, /* The Flag for Erase Fail */
    .ProgramFailFlag = 0x10, /* The Flag for Erase Fail */

    /* NOR Enter QUAD Protocol CMD */
    .Quad = {
        0x0,         /* EnableQuadMode */
        0x0,         /* ReadQuadMode */
        0x0,         /* ExitQuadMode */
        0x0,         /* QuadFlag */
        0x0,         /* ExitQuadFlag */
        0x0          /* QuadCmdArgLen */
    }
};

extern AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting;
AMBA_SERIAL_SPI_CONFIG_s AmbaNOR_SPISetting = {
    .FlowControl   = 0x1,     /* Flow Control Enable/Disable */
    .HoldTime      = 0x0,     /* Hold mode for Flow Control. 0:Hold by clock. */
    .ChipSelect    = 0x0,
    /*.RxSampleDelay = 0x4,*/ /* Shift time for RX Sampling phase */
    .RxSampleDelay = 0x2,     /* Shift time for RX Sampling phase */
    .TxThresholdLevel = 0x1f, /* Transmit FIFO Threshold Level */
    .RxThresholdLevel = 0x1f, /* Receive FIFO Threshold Level */
};

