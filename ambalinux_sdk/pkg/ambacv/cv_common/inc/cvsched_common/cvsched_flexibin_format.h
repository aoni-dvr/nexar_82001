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

#ifndef CVSCHED_FLEXIBIN_FORMAT_H_FILE
#define CVSCHED_FLEXIBIN_FORMAT_H_FILE

#ifdef ORCVIS
#include <dsp_types.h>
#endif

/*================================================================================================================*/
/* FlexiBIN File Format                                                                                           */
/*                                                                                                                */
/* FlexiBin files are a collection of components, and additional supporting data if needed by that component.     */
/* This is similar to a TBAR file, except that each component is a fixed 64-byte in size, and has a 32-bit value  */
/* to identify the component type.  Each component has additional information within it's header as defined by    */
/* the specific structure for that component.                                                                     */
/*                                                                                                                */
/* +------------------------------------+                                                                         */
/* | Main component                     |                                                                         */
/* +------------------------------------+                                                                         */
/* | Additional components              |                                                                         */
/* +------------------------------------+                                                                         */
/* |   ...                              |                                                                         */
/* +------------------------------------+                                                                         */
/* | Additional components              |                                                                         */
/* +------------------------------------+                                                                         */
/* | Supporting data for component      |                                                                         */
/* +------------------------------------+                                                                         */
/* |   ...                              |                                                                         */
/* +------------------------------------+                                                                         */
/* | Supporting data for component      |                                                                         */
/* +------------------------------------+                                                                         */
/*                                                                                                                */
/* Each supporting data structure for the component must be aligned on a 64-byte offset                           */
/*----------------------------------------------------------------------------------------------------------------*/

#define FLEXIBIN_FILE_VERSION               1U
#define FLEXIBIN_ENTRY_ALIGNMENT            64U

#define FLEXIBIN_ID_HEADER                  0x6e696266U
#define FLEXIBIN_ID_BUILD_INFO              0x69646c62U
#define FLEXIBIN_ID_SYSFLOW_TABLE           0x62667300U
#define FLEXIBIN_ID_CVARCHIVE               0x72616274U
#define FLEXIBIN_ID_VISORC0_CVCODE          0x76633076U
#define FLEXIBIN_ID_VISORC1_CVCODE          0x76633176U
#define FLEXIBIN_ID_VISORC2_CVCODE          0x76633276U
#define FLEXIBIN_ID_VISORC3_CVCODE          0x76633376U
#define FLEXIBIN_ID_METADATA                0x7464746dU

typedef struct {
    uint32_t  identifier;
    uint32_t  offset;
    uint32_t  size;
    uint32_t  reserved_crc32;
} flexibin_component_hdr_t;

typedef struct {
    flexibin_component_hdr_t  hdr;    /* identifiers : FLEXIBIN_ID_SYSFLOW_TABLE, FLEXIBIN_ID_CVARCHIVE, one data table */

    uint32_t  reserved[12];
} flexibin_component_t;

typedef struct {
    flexibin_component_hdr_t  hdr;    /* identifier : FLEXIBIN_ID_HEADER, no data table */

    uint32_t  version;
    uint32_t  total_filesize;
    uint32_t  component_size;
    uint32_t  cvchip_id;
    char      flexidag_name[32];
} flexibin_main_header_t;

typedef struct {
    flexibin_component_hdr_t  hdr;    /* identifier : FLEXIBIN_ID_BUILD_INFO, one data table */

    uint32_t  build_name_offset;
    uint32_t  build_name_size;
    uint32_t  date_time_offset;
    uint32_t  date_time_size;
    uint32_t  reserved[8];
} flexibin_build_info_component_t;

typedef struct {
    flexibin_component_hdr_t  hdr;    /* identifier : FLEXIBIN_ID_VISORC[0..3]_CVCODE, has two data table */

    uint32_t  code_offset;
    uint32_t  code_size;
    uint32_t  patchup_offset;
    uint32_t  patchup_size;
    uint32_t  reserved[8];
} flexibin_visorc_code_component_t;

typedef struct {
    flexibin_component_hdr_t  hdr;    /* identifier : FLEXIBIN_ID_METADATA */
    uint32_t  reserved_mtdt[4];

    char      metadata_name[32];
} flexibin_metadata_component_t;

#define FLEXIPATCH_FINISHED               0U
#define FLEXIPATCH_16BIT_MOVLO            1U
#define FLEXIPATCH_16BIT_MOVHI            2U
#define FLEXIPATCH_32BIT_DWORD            3U

typedef struct {
    uint32_t  dword_offset;
    uint32_t  patchup_type;
} flexibin_patchup_entry_t;

#endif /* ?CVSCHED_FLEXIBIN_FORMAT_H_FILE */

