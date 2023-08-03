/*
* Copyright (c) 2020 Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ARCH_COMPONENT_IF_H
#define ARCH_COMPONENT_IF_H
#include "ik_data_type.h"
#include "AmbaDSP_ImgArchSpec.h"

#define IK_BUFFER_HEADER_SIZE 512U

typedef struct {
    uint32 ik_version_major;
    uint32 ik_version_minor;
    uint32 init_flag;
    void *working_buffer_addr;
    uint32 number_of_context;
    uintptr context_entity[MAX_CONTEXT_NUM];
    uint32 buffer_size;
    uint32 init_mode;
    uint8 reserved_ik_buffer_info_t[(IK_BUFFER_HEADER_SIZE)-(((sizeof(uintptr)*MAX_CONTEXT_NUM) + (sizeof(uint32)*6U) + sizeof(void*))%(IK_BUFFER_HEADER_SIZE))];
} ik_buffer_info_t;

uint32 img_arch_init_architecture(void *p_buffer, uint32 context_number, size_t mem_size, uint32 ik_init_mode);
void img_arch_deinit_architecture(void);
uint32 img_arch_get_context_number(void);
uint32 img_arch_get_ik_working_buffer(ik_buffer_info_t **p_working_buffer);
uint32 img_arch_check_ik_working_buffer(void);

#endif
