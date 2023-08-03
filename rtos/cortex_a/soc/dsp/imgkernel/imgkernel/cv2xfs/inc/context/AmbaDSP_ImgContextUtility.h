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

#ifndef IMG_CONTEXT_UTILITY_H
#define IMG_CONTEXT_UTILITY_H

void ctx_raise_update_flag(uint8 *iso_filter_update_flag);
//uint32 ctx_check_uint8_parameter_valid_range(const char *parameter_name, uint8 parameter_value, uint8 min_value, uint8 max_value);
//uint32 ctx_check_int8_parameter_valid_range(const char *parameter_name, int8 parameter_value, int8 min_value, int8 max_value);
//uint32 ctx_check_uint16_parameter_valid_range(const char *parameter_name, uint16 parameter_value, uint16 min_value, uint16 max_value);
//uint32 ctx_check_int16_parameter_valid_range(const char *parameter_name, int16 parameter_value, int16 min_value, int16 max_value);
uint32 ctx_check_uint32_parameter_valid_range(const char *parameter_name, uint32 parameter_value, uint32 min_value, uint32 max_value);
uint32 ctx_check_int32_parameter_valid_range(const char *parameter_name, int32 parameter_value, int32 min_value, int32 max_value);
uint32 ctx_check_level_control_uint32_parameter_valid_range(char const *prefix_parameter_name, char const *postfix_parameter_name, uint32 parameter_value, uint32 min_value, uint32 max_value);

#endif
