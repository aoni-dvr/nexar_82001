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

#ifndef IMG_PRINT_AAA_STATISTIC_H
#define IMG_PRINT_AAA_STATISTIC_H

#include "AmbaDSP_Img3astatistic.h"


void img_3a_statistic_source_cfa_print(ik_cfa_3a_data_t * p_src_cfa_aaa_stat);
void img_3a_statistic_source_pg_print(ik_pg_3a_data_t * p_src_pg_aaa_stat);
void img_3a_statistic_source_hist_print(ik_cfa_histogram_stat_t * p_src_hist_aaa_stat);
void img_3a_statistic_cfa_print(ik_cfa_3a_data_t * p_cfa_aaa_stat_t);
void img_3a_statistic_pg_print(ik_pg_3a_data_t * p_pg_aaa_stat);
void img_3a_statistic_hist_print(ik_cfa_histogram_stat_t * p_hist_aaa_stat);
void img_set_3a_statisitc_debug_flag(UINT8 cfa_pg, UINT32 aaa_set_debug_flag, UINT32 interval);

#endif

