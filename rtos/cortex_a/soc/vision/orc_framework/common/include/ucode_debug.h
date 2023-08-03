/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
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

#ifndef UCODE_DEBUG_H_FILE
#define UCODE_DEBUG_H_FILE

#include <orc_counters.h>           /* needed for get_self_thread() */
#include <cvtask_errno.h>
#include <cvapi_logger_interface.h>

#define debug_level_t uint32_t

#define LVL_SILENT      0U
#define LVL_CRITICAL    1U
#define LVL_MINIMAL     2U
#define LVL_NORMAL      3U
#define LVL_VERBOSE     4U
#define LVL_DEBUG       5U
#define NUM_DBG_LEVELS  6U

#ifdef MISRA_COMPLIANCE

#define CVTASK_PRINTF cvtask_printf

#else /* !MISRA_COMPLIANCE */
#define CVTASK_PRINTF_0(lvl_, msg_)                     cvtask_printf(lvl_, msg_, 0, 0, 0, 0, 0);
#define CVTASK_PRINTF_1(lvl_, msg_, a)                  cvtask_printf(lvl_, msg_, (uint32_t)a, 0, 0, 0, 0);
#define CVTASK_PRINTF_2(lvl_, msg_, a, b)               cvtask_printf(lvl_, msg_, (uint32_t)a, (uint32_t)b, 0, 0, 0);
#define CVTASK_PRINTF_3(lvl_, msg_, a, b, c)            cvtask_printf(lvl_, msg_, (uint32_t)a, (uint32_t)b, (uint32_t)c, 0, 0);
#define CVTASK_PRINTF_4(lvl_, msg_, a, b, c, d)         cvtask_printf(lvl_, msg_, (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d, 0);
#define CVTASK_PRINTF_5(lvl_, msg_, a, b, c, d, e)      cvtask_printf(lvl_, msg_, (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d, (uint32_t)e);

#define GET_CVTASK_PRINTF_MACRO(_1, _2, _3, _4, _5, _6, _7, FNNAME, ...) FNNAME

#define CVTASK_PRINTF(...) GET_CVTASK_PRINTF_MACRO(__VA_ARGS__, CVTASK_PRINTF_5, CVTASK_PRINTF_4, CVTASK_PRINTF_3, CVTASK_PRINTF_2, CVTASK_PRINTF_1, CVTASK_PRINTF_0)(__VA_ARGS__)

#endif /* ?MISRA_COMPLIANCE */

#define SCHED_PRINTF  sched_printf
#define PERF_PRINTF   perf_printf
#define ERROR_PRINTF  error_printf

#ifdef CHIP_CV6

#define PERF_PRINTF_MGMT    perf_printf_mgmt
#define SCHED_PRINTF_MGR    sched_printf_mgr
#define SCHED_PRINTF_USR    sched_printf_usr
#define CVTASK_PRINTF_MGR   cvtask_printf_mgr
#define ERROR_PRINTF_MGR    error_printf_mgr
#define ERROR_PRINTF_MGMT   error_printf_mgmt

extern void perf_printf_mgmt(const cvlog_perf_entry_t *entry);
extern void sched_printf_usr(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
extern void sched_printf_mgr(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
extern void cvtask_printf_mgr(debug_level_t msg_debug_level, const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
extern void error_printf_mgr(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, errcode_enum_t error_retcode);
extern void error_printf_mgmt(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, errcode_enum_t error_retcode);

#else /* !CHIP_CV6 */

#define PERF_PRINTF_MGMT    perf_printf
#define SCHED_PRINTF_MGR    sched_printf
#define SCHED_PRINTF_USR    sched_printf
#define CVTASK_PRINTF_MGR   cvtask_printf
#define ERROR_PRINTF_MGR    error_printf
#define ERROR_PRINTF_MGMT   error_printf

#endif /* ?CHIP_CV6 */

extern void cvtask_printf(debug_level_t msg_debug_level, const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
extern void sched_printf(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
extern void perf_printf(const cvlog_perf_entry_t *entry);
extern void error_printf(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, errcode_enum_t error_retcode);
extern void cavalry_printf_raw(uint8_t log_level, uint32_t slot_id, uint32_t code_base, const char *string_addr, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);

// Specialized prints to remove recasters - these are just passthrough to the base function

#define SCHED_PRINTF_EPPPP  sched_printf_epppp
#define SCHED_PRINTF_EUPPP  sched_printf_euppp
#define SCHED_PRINTF_PPPPP  sched_printf_ppppp
#define SCHED_PRINTF_PUUUU  sched_printf_puuuu

extern void sched_printf_epppp(const char *fmtstring, errcode_enum_t arg1, const void *arg2, const void *arg3, const void *arg4, const void *arg5);
extern void sched_printf_euppp(const char *fmtstring, errcode_enum_t arg1, uint32_t arg2, const void *arg3, const void *arg4, const void *arg5);
extern void sched_printf_ppppp(const char *fmtstring, const void *arg1, const void *arg2, const void *arg3, const void *arg4, const void *arg5);
extern void sched_printf_puuuu(const char *fmtstring, const void *arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);

#define ERROR_PRINTF_EPPPP  error_printf_epppp
#define ERROR_PRINTF_EUPPP  error_printf_euppp

extern void error_printf_epppp(const char *fmtstring, errcode_enum_t arg1, const void *arg2, const void *arg3, const void *arg4, const void *arg5, errcode_enum_t error_retcode);
extern void error_printf_euppp(const char *fmtstring, errcode_enum_t arg1, uint32_t arg2, const void *arg3, const void *arg4, const void *arg5, errcode_enum_t error_retcode);

#ifdef MISRA_COMPLIANCE

void cavalry_printf_silent(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
void cavalry_printf_minimal(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
void cavalry_printf_normal(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
void cavalry_printf_verbose(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
void cavalry_printf_debug(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);
void cavalry_printf(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5);

#else

extern void codebase_start_addr(void);
extern void codebase_end_addr(void);

#define cavalry_printf_silent(fmtstring, arg1, arg2, arg3, arg4, arg5) \
	do { \
		cavalry_printf_raw(CAVALRY_LOG_LEVEL_SILENT, HOTLINK_SLOT_CAVALRY_FRAMEWORK, \
		(uint32_t)&codebase_start_addr, fmtstring, arg1, arg2, arg3, arg4, arg5); \
	} while(0)

#define cavalry_printf_minimal(fmtstring, arg1, arg2, arg3, arg4, arg5) \
	do { \
		cavalry_printf_raw(CAVALRY_LOG_LEVEL_MINIMAL, HOTLINK_SLOT_CAVALRY_FRAMEWORK, \
		(uint32_t)&codebase_start_addr, fmtstring, arg1, arg2, arg3, arg4, arg5); \
	} while(0)

#define cavalry_printf_normal(fmtstring, arg1, arg2, arg3, arg4, arg5) \
	do { \
		cavalry_printf_raw(CAVALRY_LOG_LEVEL_NORMAL, HOTLINK_SLOT_CAVALRY_FRAMEWORK, \
		(uint32_t)&codebase_start_addr, fmtstring, arg1, arg2, arg3, arg4, arg5); \
	} while(0)

#define cavalry_printf_verbose(fmtstring, arg1, arg2, arg3, arg4, arg5) \
	do { \
		cavalry_printf_raw(CAVALRY_LOG_LEVEL_VERBOSE, HOTLINK_SLOT_CAVALRY_FRAMEWORK, \
		(uint32_t)&codebase_start_addr, fmtstring, arg1, arg2, arg3, arg4, arg5); \
	} while(0)

#define cavalry_printf_debug(fmtstring, arg1, arg2, arg3, arg4, arg5) \
	do { \
		cavalry_printf_raw(CAVALRY_LOG_LEVEL_DEBUG, HOTLINK_SLOT_CAVALRY_FRAMEWORK, \
		(uint32_t)&codebase_start_addr, fmtstring, arg1, arg2, arg3, arg4, arg5); \
	} while(0)

#define cavalry_printf(fmtstring, arg1, arg2, arg3, arg4, arg5) \
	do { \
		cavalry_printf_raw(CAVALRY_LOG_LEVEL_NORMAL, HOTLINK_SLOT_CAVALRY_FRAMEWORK, \
		(uint32_t)&codebase_start_addr, fmtstring, arg1, arg2, arg3, arg4, arg5); \
	} while(0)

#endif /* ?MISRA_COMPLIANCE */

#endif /* ? UCODE_DEBUG_H_FILE */
