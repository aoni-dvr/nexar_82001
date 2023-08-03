/**
 *  @file reg_tables.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for register table
 *
 */

#ifndef REG_TABLES_H
#define REG_TABLES_H

#include "reg_common.h"

extern const reg_entry_t orc_regs[4];
extern const reg_entry_t vp_regs[78];

#ifdef REG_ENTRY_TABLES
const reg_entry_t orc_regs[4] = {
    //name[32],                        type,        addr,        offset, size, width,  height,  pitch
    {"pc_th0",                  REG_SCALAR,  0xed010034U,       0U,  32,     0,      0,     0 },
    {"pc_th1",                  REG_SCALAR,  0xed01003cU,       0U,  32,     0,      0,     0 },
    {"pc_th2",                  REG_SCALAR,  0xed010044U,       0U,  32,     0,      0,     0 },
    {"pc_th3",                  REG_SCALAR,  0xed01004cU,       0U,  32,     0,      0,     0 },
};


const reg_entry_t vp_regs[78] = {
    /* name[32],                            type,         addr, offset, size, width, height, pitch */
    /*---------------------------------------------------------------------------------------------*/
    { "perf_op_config_cycles",         REG_SCALAR,   0xed820068U,     0U,   32,     0,      0,     0 },
    { "perf_op_execute_cycles",        REG_SCALAR,   0xed82006cU,     0U,   32,     0,      0,     0 },
    { "perf_op_cooldown_cycles",       REG_SCALAR,   0xed820070U,     0U,   32,     0,      0,     0 },
    { "perf_op_unschedule_cycles",     REG_SCALAR,   0xed820074U,     0U,   32,     0,      0,     0 },
    { "perf_op_total_cycles",          REG_SCALAR,   0xed820078U,     0U,   32,     0,      0,     0 },
    { "perf_op_schedule_count",        REG_SCALAR,   0xed82007cU,     0U,   32,     0,      0,     0 },
    { "perf_dp_busy_level_0_cycles",   REG_SCALAR,   0xed820080U,     0U,   32,     0,      0,     0 },
    { "perf_dp_busy_level_1_cycles",   REG_SCALAR,   0xed820084U,     0U,   32,     0,      0,     0 },
    { "perf_dp_busy_level_2_cycles",   REG_SCALAR,   0xed820088U,     0U,   32,     0,      0,     0 },
    { "perf_dp_busy_level_3_cycles",   REG_SCALAR,   0xed82008cU,     0U,   32,     0,      0,     0 },
    { "perf_dp_schedule_count",        REG_SCALAR,   0xed820090U,     0U,   32,     0,      0,     0 },
    { "perf_dag_total_cycles",         REG_SCALAR,   0xed820094U,     0U,   32,     0,      0,     0 },

    { "dp_busy_level",                 REG_ARRAY_1D, 0xed82026fU,     3U,    2,   -17,      0,     0 },
    { "arithmetic1_dp_busy_level",     REG_SCALAR,   0xed820273U,     3U,    2,     0,      0,     0 },
    { "arithmetic2_dp_busy_level",     REG_SCALAR,   0xed820273U,     1U,    2,     0,      0,     0 },
    { "interp_dp_busy_level",          REG_SCALAR,   0xed820272U,     7U,    2,     0,      0,     0 },
    { "transcendental_dp_busy_level",  REG_SCALAR,   0xed820272U,     5U,    2,     0,      0,     0 },
    { "minmax_dp_busy_level",          REG_SCALAR,   0xed820272U,     3U,    2,     0,      0,     0 },
    { "classifier_dp_busy_level",      REG_SCALAR,   0xed820272U,     1U,    2,     0,      0,     0 },
    { "generator_dp_busy_level",       REG_SCALAR,   0xed820271U,     7U,    2,     0,      0,     0 },
    { "compare_dp_busy_level",         REG_SCALAR,   0xed820271U,     5U,    2,     0,      0,     0 },
    { "logical_dp_busy_level",         REG_SCALAR,   0xed820271U,     3U,    2,     0,      0,     0 },
    { "count_dp_busy_level",           REG_SCALAR,   0xed820271U,     1U,    2,     0,      0,     0 },
    { "warp_dp_busy_level",            REG_SCALAR,   0xed820270U,     7U,    2,     0,      0,     0 },
    { "segments_dp_busy_level",        REG_SCALAR,   0xed820270U,     5U,    2,     0,      0,     0 },
    { "statistics_dp_busy_level",      REG_SCALAR,   0xed820270U,     3U,    2,     0,      0,     0 },
    { "copy_dp_busy_level",            REG_SCALAR,   0xed820270U,     1U,    2,     0,      0,     0 },
    { "transfer1_dp_busy_level",       REG_SCALAR,   0xed82026fU,     7U,    2,     0,      0,     0 },
    { "transfer2_dp_busy_level",       REG_SCALAR,   0xed82026fU,     5U,    2,     0,      0,     0 },
    { "transfer3_dp_busy_level",       REG_SCALAR,   0xed82026fU,     3U,    2,     0,      0,     0 },

    { "dp_sched_count",                REG_ARRAY_1D, 0xed820160U,     4U,    2,   -17,      0,     0 },
    { "arithmetic1_dp_sched_count",    REG_SCALAR,   0xed820164U,     4U,    2,     0,      0,     0 },
    { "arithmetic2_dp_sched_count",    REG_SCALAR,   0xed820164U,     2U,    2,     0,      0,     0 },
    { "interp_dp_sched_count",         REG_SCALAR,   0xed820164U,     0U,    2,     0,      0,     0 },
    { "transcendental_dp_sched_count", REG_SCALAR,   0xed820163U,     6U,    2,     0,      0,     0 },
    { "minmax_dp_sched_count",         REG_SCALAR,   0xed820163U,     4U,    2,     0,      0,     0 },
    { "classifier_dp_sched_count",     REG_SCALAR,   0xed820163U,     2U,    2,     0,      0,     0 },
    { "generator_dp_sched_count",      REG_SCALAR,   0xed820163U,     0U,    2,     0,      0,     0 },
    { "compare_dp_sched_count",        REG_SCALAR,   0xed820162U,     6U,    2,     0,      0,     0 },
    { "logical_dp_sched_count",        REG_SCALAR,   0xed820162U,     4U,    2,     0,      0,     0 },
    { "count_dp_sched_count",          REG_SCALAR,   0xed820162U,     2U,    2,     0,      0,     0 },
    { "warp_dp_sched_count",           REG_SCALAR,   0xed820162U,     0U,    2,     0,      0,     0 },
    { "segments_dp_sched_count",       REG_SCALAR,   0xed820161U,     6U,    2,     0,      0,     0 },
    { "statistics_dp_sched_count",     REG_SCALAR,   0xed820161U,     4U,    2,     0,      0,     0 },
    { "copy_dp_sched_count",           REG_SCALAR,   0xed820161U,     2U,    2,     0,      0,     0 },
    { "transfer1_dp_sched_count",      REG_SCALAR,   0xed820161U,     0U,    2,     0,      0,     0 },
    { "transfer2_dp_sched_count",      REG_SCALAR,   0xed820160U,     6U,    2,     0,      0,     0 },
    { "transfer3_dp_sched_count",      REG_SCALAR,   0xed820160U,     4U,    2,     0,      0,     0 },

    { "dp_sched_op_id",                REG_ARRAY_2D, 0xed82012dU,     4U,    8,     3,    -17,     3 },
    { "arithmetic1_dp_sched_op_id",    REG_ARRAY_1D, 0xed82015dU,     4U,    8,     3,      0,     0 },
    { "arithmetic2_dp_sched_op_id",    REG_ARRAY_1D, 0xed82015aU,     4U,    8,     3,      0,     0 },
    { "interp_dp_sched_op_id",         REG_ARRAY_1D, 0xed820157U,     4U,    8,     3,      0,     0 },
    { "transcendental_dp_sched_op_id", REG_ARRAY_1D, 0xed820154U,     4U,    8,     3,      0,     0 },
    { "minmax_dp_sched_op_id",         REG_ARRAY_1D, 0xed820151U,     4U,    8,     3,      0,     0 },
    { "classifier_dp_sched_op_id",     REG_ARRAY_1D, 0xed82014eU,     4U,    8,     3,      0,     0 },
    { "generator_dp_sched_op_id",      REG_ARRAY_1D, 0xed82014bU,     4U,    8,     3,      0,     0 },
    { "compare_dp_sched_op_id",        REG_ARRAY_1D, 0xed820148U,     4U,    8,     3,      0,     0 },
    { "logical_dp_sched_op_id",        REG_ARRAY_1D, 0xed820145U,     4U,    8,     3,      0,     0 },
    { "count_dp_sched_op_id",          REG_ARRAY_1D, 0xed820142U,     4U,    8,     3,      0,     0 },
    { "warp_dp_sched_op_id",           REG_ARRAY_1D, 0xed82013fU,     4U,    8,     3,      0,     0 },
    { "segments_dp_sched_op_id",       REG_ARRAY_1D, 0xed82013cU,     4U,    8,     3,      0,     0 },
    { "statistics_dp_sched_op_id",     REG_ARRAY_1D, 0xed820139U,     4U,    8,     3,      0,     0 },
    { "copy_dp_sched_op_id",           REG_ARRAY_1D, 0xed820136U,     4U,    8,     3,      0,     0 },
    { "transfer1_dp_sched_op_id",      REG_ARRAY_1D, 0xed820133U,     4U,    8,     3,      0,     0 },
    { "transfer2_dp_sched_op_id",      REG_ARRAY_1D, 0xed820130U,     4U,    8,     3,      0,     0 },
    { "transfer3_dp_sched_op_id",      REG_ARRAY_1D, 0xed82012dU,     4U,    8,     3,      0,     0 },

    { "run_bank",                      REG_SCALAR,   0xed820188U,     5U,    1,     0,      0,     0 },
    { "parse_bank",                    REG_SCALAR,   0xed820188U,     6U,    1,     0,      0,     0 },

    { "operator_blocked",              REG_ARRAY_1D, 0xed8205f3U,     4U,    1,  -255,      0,     0 },
    { "operator_done",                 REG_ARRAY_1D, 0xed820613U,     3U,    1,  -255,      0,     0 },
    { "operator_waiting",              REG_ARRAY_1D, 0xed820633U,     2U,    1,  -255,      0,     0 },

    { "input_operand_ready",           REG_ARRAY_2D, 0xed8207baU,     5U,    1,    -3,   -255,     3 },
    { "output_operand_ready",          REG_ARRAY_2D, 0xed82077aU,     7U,    1,    -2,   -255,     2 },

    { "last_parsed_operator_id",       REG_SCALAR,   0xed82098bU,     3U,    8,     0,      0,     0 },
    { "next_vmem_word_addr_to_parse",  REG_SCALAR,   0xed820a76U,     7U,   19,     0,      0,     0 },

    { "vmemif_start_channel",          REG_SCALAR,   0xed82081bU,     2U,    8,     0,      0,     0 },
    { "vmemif_end_channel",            REG_SCALAR,   0xed82081aU,     2U,    8,     0,      0,     0 },

    { "vmb_coord",                     REG_ARRAY_2D, 0xed834000U,     0U,   32,     2,    256,    64 },
};
#endif

#endif //REG_TABLES_H

