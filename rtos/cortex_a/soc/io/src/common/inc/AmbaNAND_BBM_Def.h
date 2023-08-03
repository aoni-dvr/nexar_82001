/**
 *  @file AmbaNAND_BBM_Def.h
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
 *  @details Definitions & Constants for NAND Bad Block Management (BBM) APIs
 *
 */

#ifndef AMBA_NAND_BBM_DEF_H
#define AMBA_NAND_BBM_DEF_H

/* Use 2-bit Mark per block, 128KB(=2^17)/block, 1 Byte holds Marks of 4 Blocks */
#define AMBA_NAND_MAX_BBT_BYTE_SIZE     ((AMBA_NAND_MAX_BYTE_SIZE >> (17U + 2U)) + 5U)

#define AMBA_BBT_PRIMARY_SIGNATURE  0x30746242  /* {'B' 'b' 't' '0'} */
#define AMBA_BBT_MIRROR_SIGNATURE   0x42627431  /* {'1' 't' 'b' 'B'} */

#define AMBA_NAND_BLK_FACTORY_BAD (0U)  /* 0x00: Factory marked bad block */
#define AMBA_NAND_BLK_RUNTIME_BAD (1U)  /* 0x01: Run-time bad block: becaue of an erase/write failure */
#define AMBA_NAND_BLK_RESERVED    (2U)  /* 0x02: Reserved (don't use this block) */
#define AMBA_NAND_BLK_GOOD        (3U)  /* 0x03: Good block */

/* The maximum number of blocks to scan for a bbt */
#define NAND_BBT_SCAN_MAXBLOCKS (4U)

#endif  /* AMBA_NAND_BBM_DEF_H */
