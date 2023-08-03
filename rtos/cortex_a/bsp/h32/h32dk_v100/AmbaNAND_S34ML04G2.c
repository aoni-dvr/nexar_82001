/**
 *  @file AmbaNAND_S34ML04G2.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details NAND device Info of S34ML04G1
 *
 */

#include "AmbaTypes.h"
#include "AmbaNAND_Def.h"

extern AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo;
AMBA_NAND_DEV_INFO_s AmbaNAND_DevInfo = {
    .DevName  = "Spansion S34ML04G2_512MB_PG2K_SPARE2x",     /* pointer to NAND Device name */

    .ChipSize = AMBA_NAND_SIZE_4G_BITS, /* the NAND Flash Memory Chip size */
    .MainByteSize   = 2048,             /* Main area size in Byte */
    .SpareByteSize  = 128,              /* Spare area size in Byte */
    .BlockPageSize  = 64,               /* Block size in Page */
    .PlaneBlockSize = 2048,             /* Blocks per plane */
    .TotalPlanes    = 2,                /* Total number of planes */
    .ZoneBlockSize  = 2048,             /* Blocks per zone */
    .TotalZones     = 2,                /* Total number of zones */

    .ColumnCycles   = 2,                /* Column access cycles */
    .PageCycles     = 3,                /* Page access cycles */
    .IdCycles       = 5,                /* ID read cycles */
    .PlaneAddrMask  = 0x1,              /* Mask of Plane Address */

    /** Chip Timing Parameters **/
    .tCLS = 12,         /* CLE Setup Time (ns) */
    .tCLH = 5,          /* CLE Hold Time (ns) */
    .tCS  = 20,         /* CE setup Time (ns) */
    .tCH  = 5,          /* CE Hold Time (ns) */
    .tWP  = 12,         /* WE Pulse Width (ns) */
    .tALS = 12,         /* ALE Setup Time (ns) */
    .tALH = 58,         /* ALE Hold Time (ns) tALH + tWP >= tADL*/
    .tDS  = 12,         /* Data setup Time (ns) */
    .tDH  = 5,          /* Data Hold Time (ns) */
    .tWH  = 10,         /* WE High Hold Time (ns) */

    .tAR  = 10,         /* ALE to RE Delay (ns) */
    .tCLR = 10,         /* CLE to REDelay (ns) */
    .tRR  = 20,         /* Ready to RE Low (ns), original : 20 */
    .tRP  = 12,         /* RE Pulse Width (ns) original : 12 */
    .tWB  = 100,        /* WE High to Busy (ns) */
    .tREA = 20,         /* RE Access Time(tRDELAY) (ns) */
    .tRHZ = 100,        /* RE High to Output Hi-Z (ns) */
    .tREH = 10,         /* RE High Hold Time (ns) */
    .tIR  = 0,          /* Output Hi-Z to RE Low (ns) */
    .tWHR = 60,         /* nWE High to nRE Low (ns) */

    .tRB  = 100,        /* same as tWB */
    .tCEH = 70,         /* tRHZ - tCHZ */
    .tWW  = 100,        /* Write Protection time */

    .tCRL = 5,          /* CE Low to read pulse falling edge (ns) tCEA - tREA */
    .tADL = 70,         /* Address to data loading time (ns) */
    .tRHW = 100,        /* RE High to WE Low (ns) */

    .tRC  = 25,         /* Read cycle time (ns), tRP + tREH >= tRC */
};

