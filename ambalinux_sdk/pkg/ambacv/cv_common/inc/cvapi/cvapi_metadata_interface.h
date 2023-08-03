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
#ifndef CVAPI_METADATA_INTERFACE_H_FILE
#define CVAPI_METADATA_INTERFACE_H_FILE

#ifndef CVAPI_COMMON_H_FILE
#include <cvapi_common.h>
#endif

/*-= Gyro specific metadata =-*/
#define MAX_GYRO_SAMPLES 40 /*  temporary fix */

typedef struct /* gyro data */
{
 int16_t g_x[MAX_GYRO_SAMPLES];
 int16_t g_y[MAX_GYRO_SAMPLES];
 int16_t g_z[MAX_GYRO_SAMPLES];
 int16_t a_x[MAX_GYRO_SAMPLES];
 int16_t a_y[MAX_GYRO_SAMPLES];
 int16_t a_z[MAX_GYRO_SAMPLES];
 int32_t  samples;
} idsp_gyro_data_t;


/*-= Metadata structures and types =-*/

#define METADATA_TYPE_START           0xFEEDB10CU       /* Start of meta data header set  */
#define METADATA_TYPE_END             0xDEADB10CU       /* End of meta data header set    */

#ifndef METADATA_TYPE_RAW
#define METADATA_TYPE_RAW             0xFEEDFEEDU       /* Type "RAW" metadata type - assumed to be of metadata format */
#endif /* ?METADATA_TYPE_RAW */

#define METADATA_TYPE_GYRO_DATA       1                 /* uses idsp_gyro_data_t structure above */

/*-= Generic start metadata block =-*/
typedef struct
{
  uint32_t  metadata_capture_time;
  uint32_t  frame_num;
} metadata_start_header_t;

/**
 *  Output buffer from IDSP sourcing CVTasks - the metadata is
 *  packaged into a buffer with the name IONAME_PIC_METADATA. To
 *  find metadata provided by the system, please use
 *  cvtask_metadata_find() - documented in cvtask_api.h.
 */
#define IONAME_PIC_METADATA     "IDSP_METADATA"

/*
 * META_RX is the task designed to connect to non-IDSP data sources with
 * pure metadata associated with it.
 *
 * META_RX currently has one output:
 *
 * IONAME_PIC_METADATA
 *
 * config[0] : Cvtask_instance_identifier, also acts as (fov_id / 2)
 * config[5] : Additional data for meta-buffer
 */
#define TASKNAME_META_RX      "META_RX"

#endif /* !CVAPI_METADATA_INTERFACE_H_FILE */

