/*
* Copyright (c) 2017-2017 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CVAPI_METADATA_INTERFACE_INTERNAL_H_FILE
#define CVAPI_METADATA_INTERFACE_INTERNAL_H_FILE

#include <ambint.h>

#ifndef METADATA_TYPE_RAW
#define METADATA_TYPE_RAW             0xFEEDFEEDU       /* Type "RAW" metadata type, a collection of metadata_blocks */
#endif /* ?METADATA_TYPE_RAW */

/*-= Internal structure of the metadata block =-*/

typedef struct
{
  uint32_t  metadata_type;              /* Metadata type identifier */
  uint32_t  metadata_size;              /* Metadata actual size */
  uint32_t  metadata_block_size;        /* Metadata block size - total size of this block, including headers and cache line alignment */
  uint32_t  metadata_64bit_align;       /* Alignment for metadata */
} metadata_block_header_t;

/*-= Metadata (internal) block format =---------------------------------------+
| Each "block" is of type "metadata_type", of size "metadata_size"            |
| Each "block" must be a multiple of 64-bytes in size, including the header.  |
|   > This should be set in "metadata_block_size"                             |
+----------------------------------------------------------------------------*/

#endif /* !CVAPI_METADATA_INTERFACE_INTERNAL_H_FILE */

