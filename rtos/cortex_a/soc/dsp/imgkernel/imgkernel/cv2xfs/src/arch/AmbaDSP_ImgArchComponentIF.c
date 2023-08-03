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

#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgArchComponentIF.h"


static ik_buffer_info_t *p_ik_working_buffer_info = NULL;

uint32 img_arch_init_architecture(void *p_buffer, uint32 context_number, size_t mem_size, uint32 ik_init_mode)
{
    uint32 rval = IK_OK;

    (void)amba_ik_system_memcpy(&p_ik_working_buffer_info, &p_buffer, sizeof(ik_buffer_info_t*));

    p_ik_working_buffer_info->init_mode = ik_init_mode;

    if(ik_init_mode == AMBA_IK_ARCH_HARD_INIT) {
        (void)amba_ik_system_memset(p_ik_working_buffer_info, 0x0, IK_BUFFER_HEADER_SIZE);
        p_ik_working_buffer_info->init_flag = 1UL;
        p_ik_working_buffer_info->working_buffer_addr = (void*)p_buffer;
        p_ik_working_buffer_info->ik_version_major = IK_VERSION_MAJOR;
        p_ik_working_buffer_info->ik_version_minor = IK_VERSION_MINOR;
        p_ik_working_buffer_info->number_of_context = context_number;
        p_ik_working_buffer_info->buffer_size = mem_size;
        p_ik_working_buffer_info->p_bin_data_dram_addr = NULL;
#ifndef EARLYTEST_ENV
        p_ik_working_buffer_info->safety_enable = 0u;
#else
        p_ik_working_buffer_info->safety_enable = 1u;
#endif
        p_ik_working_buffer_info->safety_state = IK_SAFETY_STATE_OK;
    } else if (ik_init_mode == AMBA_IK_ARCH_SOFT_INIT) {
        if (p_ik_working_buffer_info->init_flag != 1UL) {
            amba_ik_system_print_str_5("[IK][ERROR] Incorrect AMBA_IK_ARCH_SOFT_INIT mode. IK havn't init yet.", DC_S, DC_S, DC_S, DC_S, DC_S);
            rval |= IK_ERR_0001;
        }
        if (p_ik_working_buffer_info->ik_version_major != IK_VERSION_MAJOR) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect IK_VERSION_MAJOR %d.", p_ik_working_buffer_info->ik_version_major, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0001;
        }
        if (p_ik_working_buffer_info->ik_version_minor != IK_VERSION_MINOR) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect IK_VERSION_MINOR %d.", p_ik_working_buffer_info->ik_version_minor, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0001;
        }
        if (p_ik_working_buffer_info->number_of_context != context_number) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect number_of_context %d.", p_ik_working_buffer_info->number_of_context, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0001;
        }
        if (p_ik_working_buffer_info->buffer_size != mem_size) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect buffer_size %d.", p_ik_working_buffer_info->buffer_size, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0001;
        }

        if(rval == IK_OK) {
            p_ik_working_buffer_info->working_buffer_addr = (void*)p_buffer;
        }
    } else {
        //TBD
    }

    amba_ik_system_print_uint32_5("[IK_version] V%d.%d, branch %d", IK_VERSION_MAJOR, IK_VERSION_MINOR, IK_BRANCH, DC_U, DC_U);

    return rval;
}

void img_arch_deinit_architecture(void)
{
    p_ik_working_buffer_info = NULL;
}

uint32 img_arch_get_context_number(void)
{
    uint32 context_number = 0;
    if(p_ik_working_buffer_info != NULL) {
        if(p_ik_working_buffer_info->init_flag == 1U) {
            context_number = p_ik_working_buffer_info->number_of_context;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] IK havn't init yet.", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        amba_ik_system_print_str_5("[IK][ERROR] IK havn't init yet, p_ik_working_buffer_info is NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return context_number;
}

uint32 img_arch_get_ik_working_buffer(ik_buffer_info_t **p_working_buffer)
{
    uint32 rval = IK_OK;

    if(p_ik_working_buffer_info != NULL) {
        *p_working_buffer = p_ik_working_buffer_info;
        rval = IK_OK;
    } else {
        amba_ik_system_print_str_5("[IK][ERROR] One or more pointer is NULL !!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    }

    return rval;
}

uint32 img_arch_check_ik_working_buffer(const uint32 ik_init_mode)
{
    uint32 rval;

    if((p_ik_working_buffer_info != NULL) && (ik_init_mode == AMBA_IK_ARCH_HARD_INIT)) {
        if(p_ik_working_buffer_info->safety_state != IK_SAFETY_STATE_OK) {
            rval = IK_ERR_0200;
        } else {
            rval = IK_ERR_0005;
        }
    } else {
        rval = IK_OK;
    }

    return rval;
}

