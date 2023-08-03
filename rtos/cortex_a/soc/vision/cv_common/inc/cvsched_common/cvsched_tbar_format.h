/*
 * Copyright (c) 2017-2020 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVSCHED_TBAR_FORMAT_H_FILE
#define CVSCHED_TBAR_FORMAT_H_FILE

#ifdef ORCVIS
#include <dsp_types.h>
#endif

#define TBAR_FILE_VERSION       2U
#define TBAR_ENTRY_ALIGNMENT    128U    /* Alignment factor for entries in the TBAR file; must be a power of 2.   */

/*================================================================================================================*/
/* TBAR File Format                                                                                               */
/*                                                                                                                */
/* TBAR files are a collection of data tables, that are packaged into a single file for ease of use with the      */
/* CVScheduler.  The file is formatted as:                                                                        */
/*                                                                                                                */
/* +------------------------------------+                                                                         */
/* +  TBAR header (128 bytes)           +                                                                         */
/* +------------------------------------+                                                                         */
/* +  TBAR-TOC (table entries)          +  Table of Contents (TOC)                                                */
/* +  - version 1 : 40 bytes each       +  - Contains multiple TOC entries, determined by the tbar_version in the */
/* +  - version 2 : 128 bytes each      +    TBAR header.  Note that only one type of TOC entry can exist per     */
/* +  - version 3 : 128 bytes each      +    TBAR file, but different TBARs can have different TOC types.         */
/* +------------------------------------+                                                                         */
/* +  Table data                        +                                                                         */
/* +------------------------------------+                                                                         */
/* +     ...                            +                                                                         */
/* +------------------------------------+                                                                         */
/* +  Table data                        +                                                                         */
/* +------------------------------------+                                                                         */
/*                                                                                                                */
/* Each deliniated block above shall be aligned on a "TBAR_ENTRY_ALIGNMENT"-byte boundary.                        */
/*                                                                                                                */
/*================================================================================================================*/

typedef struct {                /* ==================================================================== */
    /*                                                                      */
    char      table_name[32];     /* +   0 : Name of the CVTable                                          */
    uint32_t  table_offset;       /* +  32 : Offset from the base of the file where the data is located   */
    uint32_t  table_size;         /* +  36 : Length of the data                                           */
} tbar_toc_entry_v1_t;          /* =  40 bytes ======================================================== */

typedef struct {                /* ==================================================================== */
    /*                                                                      */
    char      table_name[120];    /* +   0 : Name of the CVTable                                          */
    uint32_t  table_offset;       /* + 120 : Offset from the base of the file where the data is located   */
    uint32_t  table_size;         /* + 124 : Length of the data                                           */
} tbar_toc_entry_v2_t;          /* = 128 bytes ======================================================== */

typedef struct {                /* ==================================================================== */
    /*                                                                      */
    char      table_name[116];    /* +   0 : Name of the CVTable                                          */
    uint32_t  table_crc32;        /* + 116 : CRC32 value of the table entry                               */
    uint32_t  table_offset;       /* + 120 : Offset from the base of the file where the data is located   */
    uint32_t  table_size;         /* + 124 : Length of the data                                           */
} tbar_toc_entry_v3_t;          /* = 128 bytes ======================================================== */

typedef struct {                /* ==================================================================== */
    /*                                                                      */
    uint32_t  tbar_version;       /* +   0 : Version of TBAR file                                         */
    uint32_t  toc_num_entries;    /* +   4 : Number of tbar_file_entry_v*_t contained in TOC              */
    uint32_t  toc_entry_size;     /* +   8 : Size of the tbar_file_entry_v*_t used in the TOC             */
    uint32_t  toc_offset;         /* +  12 : Offset of the TOC from the base of the file                  */
    uint32_t  tbar_total_size;    /* +  16 : Total size of the TBAR file                                  */
    uint8_t   align_128b[108U];   /* +  20 : Alignment to 128 bytes for the header                        */
} tbar_file_header_t;           /* = 128 bytes ======================================================== */

#endif /* ?CVSCHED_TBAR_FORMAT_H_FILE */

