/*
* Copyright (c) 2017-2020 Ambarella, Inc.
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

#ifndef CVAPI_CVFLOW_BUFFER_H
#define CVAPI_CVFLOW_BUFFER_H

typedef struct {
    uint8_t padding[24];
    uint32_t df[4];
    uint32_t dims[4];
    uint32_t num_bytes;
} cvflow_buffer_desc_t;

typedef struct {
    uint32_t magic_num;
    uint32_t num_packed;
    uint32_t buffer_offset[13];
    uint32_t version;
} cvflow_port_header_t;

// to access buffer/desc of Nth output of the port
// uint8_t *buf_n = (uint8_t*)outbuf + ((cvflow_port_header_t*)outbuf)->buffer_offset[n];
// cvflow_buffer_desc_t* desc_n = (uint8_t *)buf_n - sizeof(cvflow_buffer_desc_t);

#endif //CVAPI_CVFLOW_BUFFER_H
