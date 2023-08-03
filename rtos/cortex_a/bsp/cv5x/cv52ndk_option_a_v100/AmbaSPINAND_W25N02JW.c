/**
 *  @file AmbaNAND_MT29F4G01ADAGDWB.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details NAND device Info
 *
 */
#include "AmbaTypes.h"
#include "AmbaNAND_Def.h"

const AMBA_SPINAND_DEV_INFO_s AmbaSpiNAND_DevInfo = {
    .DevName = "W25N02JW",    /* pointer to NAND Device name */

    .ChipSize = AMBA_NAND_SIZE_2G_BITS,//AMBA_NAND_SIZE_2G_BITS, /* the NAND Flash Memory Chip size */
    .MainByteSize   = 2048,             /* Main area size in Byte */
    .SpareByteSize  = 64,               /* Spare area size in Byte */
    .BlockPageSize  = 64,               /* Block size in Page */
    .PlaneBlockSize = 1024,             /* Blocks per plane */
    .TotalPlanes    = 2,                /* Total number of planes */
    .ZoneBlockSize  = 1024,             /* Blocks per zone */
    .TotalZones     = 2,                /* Total number of zones */

    .ColumnCycles   = 2,                /* Column access cycles */
    .PageCycles     = 3,                /* Page access cycles */
    .InternalECC = AMBA_SPINAND_ENABLE_ONDEVICE_ECC,
    .EraseAddrType  = AMBA_SPINAND_ERASE_ADDRBY_PAGE,
    .PlaneSelect    = AMBA_SPINAND_NOPLANE_SELECT,

    /* Read Operator CmdSet */
    .Read = {
        .ReadArray_AddrDummy = 0,            /* The Dummy Cycles for Read Operator */
        .ReadCacheIndex = 0x6B,
        .AddrDummy = 0,                      /* The Dummy Cycles for Read Operator */
        .DataDummy = 1,                      /* The Dummy Cycles for Read Operator */
        .CmdType = AMBA_SPINAND_READCACHE_4LANE,

        //.DataDummyCR = 6U,                  /* The Dummy Cycles for Continues Read Operator */
    },
    /* Program Operator CmdSet*/
    .Program = {
        .LoadCmdIndex = 0x32,                 /* The index for ProgramLoad Array */
        .LoadAddrDummy = 0x0,                 /* The Dummy Cycles for Program Operator */
        .LoadDataDummy = 0x0,                 /* The Dummy Cycles for Program Operator */
        .LoadCmdType = AMBA_SPINAND_PROGRAM_4LANE
    },
    /** Chip Timing Parameters **/
    .tCLQV = 6,                         /* CLock Low to Read Data valid (ns) */
    .tCS = 50,                          /* Command deselect time (ns) */
    .tCLL = 3,                          /* Clock low time (ns), default: 2.7 */

    .tCLH = 3,                          /* Clock High time (ns), default: 2.7 */
    .tSHC = 3,                          /* CS Rising edge to SCK rising edge (ns) */
    .tCHS = 3,                          /* SCK Rising edge to CS rising edge (ns),tCHSH */
    .tSLCH = 5,                         /* CS Rising edge to SCK rising edge (ns) */
    .tCHSL = 5,                         /* SCK Rising edge to CS falling edge (ns) */
    .tWPH = 100,                        /* CS Rising Edge to WP falling edge (ns) */
    .tWPS = 20,                         /* WP Rising edge to CS falling edge (ns) */
    .tHHQX = 7                         /* HOLD Rising edge to Read Data valid (ns) */
};
