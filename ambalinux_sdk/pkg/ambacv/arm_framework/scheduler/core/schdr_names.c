/**
 *  @file names.c
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
 *  @details Names APIs
 *
 */

#include "os_api.h"
#include "cvtask_api.h"
#include "dram_mmap.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "cvtask_errno.h"
#include "cvapi_metadata_interface.h"
#include "cvapi_metadata_interface_internal.h"
#include <cvsched_tbar_format.h>

static uint32_t sysflow_printed;
static uint32_t cvtable_printed;

static void print_sysflow(void)
{
    const sysflow_entry_t *pSysFlow = NULL;
    uint32_t  i;
    uint32_t  str_pos;
    uint32_t  sysflow_numentries = 0U;
    char str_line[512];

    if (schdr_sys_cfg.verbosity < LVL_VERBOSE) {
        sysflow_printed = 1U;
    } else {
        if (sysflow_printed == 0U) {
            sysflow_printed = 1U;

            pSysFlow            = schdr_sys_state.pSysFlow;
            sysflow_numentries  = schdr_sys_state.sysflow_numlines;

            if ((sysflow_numentries > 0U) && (pSysFlow != NULL)) {
                module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,"##################################################\n"
                               "                sysflow table\n"
                               "##################################################\n",
                               NULL, NULL, NULL, NULL, NULL);
            } /* if ((sysflow_numentries > 0) && (pCVName != NULL)) */
        }

        if(pSysFlow != NULL) {
            for (i = 0U; i < sysflow_numentries; i++) {
                str_pos = snprintf_uint5(&str_line[0], (uint32_t)sizeof(str_line), "    %3d: uuid=%-4d ",
                                         i,
                                         pSysFlow->uuid,
                                         0U, 0U, 0U);
                str_pos += snprintf_str5(&str_line[str_pos], (uint32_t)sizeof(str_line), "\t%s, %s, %s, %s\n",
                                         pSysFlow->instance_name,
                                         pSysFlow->algorithm_name,
                                         pSysFlow->step_name,
                                         pSysFlow->cvtask_name,
                                         NULL);
                (void) str_pos;
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,&str_line[0], 0U, 0U, 0U, 0U, 0U);
                pSysFlow++;
            }
            module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,"\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

} /* print_sysflow() */

void schdr_init_names(void)
{
    sysflow_printed = 0U;
    cvtable_printed = 0U;
}

uint32_t schdr_init_sysflow(uint64_t base, uint32_t numlines)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void *ptr;

    ptr = ambacv_c2v(base);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_SYSFLOW_INVALID;
    } else {
        schdr_sys_state.sysflow_addr = base;
        typecast(&schdr_sys_state.pSysFlow,&ptr);
        schdr_sys_state.sysflow_numlines = numlines;
        print_sysflow();
    }

    return retcode;
} /* schdr_init_sysflow() */

uint32_t schdr_reinit_sysflow(void)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void *ptr;

    ptr = ambacv_c2v(schdr_sys_state.sysflow_addr);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_SYSFLOW_INVALID;
    } else {
        typecast(&schdr_sys_state.pSysFlow,&ptr);
        print_sysflow();
    }

    return retcode;
} /* schdr_init_sysflow() */

static void print_cvtable(void)
{
    const cvtable_entry_t  *pTable = NULL;
    const cvtable_entry_t  *info;
    uint32_t  table_size = 0U;
    uint32_t  i;
    uint32_t  str_pos,tmp_pos;
    char str_line[512];
    const char *name;
    const void *ptr;

    if (schdr_sys_cfg.verbosity < LVL_VERBOSE) {
        cvtable_printed = 1U;
    } else {
        if (cvtable_printed == 0U) {
            cvtable_printed = 1U;

            pTable      = schdr_sys_state.pCvtable;
            table_size  = schdr_sys_state.cvtable_numlines;

            if ((table_size > 0U) && (pTable != NULL)) {
                module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,"##################################################\n"
                               "                 cvtable info\n"
                               "##################################################\n",
                               NULL, NULL, NULL, NULL, NULL);
            } /* if ((table_size > 0U) && (pTable != NULL)) */
        }

        if(pTable != NULL) {
            for (i = 0U; i < table_size; i++) {
                info = &pTable[i];
                str_pos = snprintf_uint5(&str_line[0], (uint32_t)sizeof(str_line), "    %3d: size=%7d, buff=0x%X",
                                         i,
                                         info->cvtable_size,
                                         (uint32_t)ambacv_c2p(info->CVTable_daddr),
                                         0U, 0U);
                if(info->CVTable_name_daddr != 0U) {
                    ptr = ambacv_c2v(info->CVTable_name_daddr);
                    if(ptr != NULL) {
                        typecast(&name,&ptr);
                        tmp_pos = snprintf_str5(&str_line[str_pos], (uint32_t)sizeof(str_line), " => name=%s\n",
                                                name,
                                                NULL, NULL, NULL, NULL);
                        str_pos += tmp_pos;
                        (void) str_pos;
                    }
                }
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, &str_line[0], 0U, 0U, 0U, 0U, 0U);
            }
        }
    }
} /* print_cvtable() */

uint32_t schdr_init_cvtable(uint64_t base, uint32_t numlines, uint32_t disable_log)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void *ptr;

    ptr = ambacv_c2v(base);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_CVTABLE_INVALID;
    } else {
        typecast(&schdr_sys_state.pCvtable,&ptr);
        schdr_sys_state.cvtable_numlines = numlines;
        if (disable_log == 0U) {
            print_cvtable();
        } /* if (!disable_log) */
    }

    return retcode;
} /* schdr_init_cvtable() */

uint32_t schdr_init_buildinfo(uint64_t cbase, uint64_t sbase, uint32_t numlines)
{
    uint32_t retcode = ERRCODE_NONE;
    component_build_info_t *entry;
    const char *name;
    const void *ptr;

    (void) sbase;
    (void) numlines;
    if (cbase != 0U) {
        ptr = ambacv_c2v(cbase);
        if(ptr == NULL) {
            retcode = ERR_DRV_SCHDR_BUILDINFO_INVALID;
        } else {
            typecast(&entry,&ptr);
            schdr_sys_state.pBuildinfo = entry;

            if (schdr_sys_cfg.verbosity >= LVL_DEBUG) {
                module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                               "#####################################################\n"
                               "              build version info\n"
                               "#####################################################\n",
                               NULL, NULL, NULL, NULL, NULL);
                while (entry->component_type != COMPONENT_TYPE_END_OF_LIST) {
                    if(entry->component_name_daddr != 0U) {
                        ptr = ambacv_c2v(entry->component_name_daddr);
                        if(ptr != NULL) {
                            typecast(&name,&ptr);
                            module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                                           "%s:\n",
                                           name,
                                           NULL, NULL, NULL, NULL);
                        }
                    }
                    module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                                   "    source: %s\n",
                                   entry->component_version,
                                   NULL, NULL, NULL, NULL);
                    module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                                   "      tool: %s\n",
                                   entry->tool_version,
                                   NULL, NULL, NULL, NULL);
                    entry++;
                }
            }
        }
    } else {
        retcode = ERR_DRV_SCHDR_BUILDINFO_INVALID;
    }

    return retcode;
}

uint32_t schdr_init_system_msg(uint64_t msgpool)
{
    uint32_t retcode = ERRCODE_NONE;
    const void *ptr;

    ptr = ambacv_c2v(msgpool);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_MSG_POOL_INVALID;
    } else {
        typecast(&schdr_sys_state.pMsgpool,&ptr);
        if (schdr_sys_cfg.verbosity >= LVL_VERBOSE) {
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "##################################################### \n", 0U, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "                system message pool \n", 0U, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "##################################################### \n", 0U, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "      write_index: %d \n",   schdr_sys_state.pMsgpool->hdr_variable.wr_index, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "       queue_size: %d \n",   schdr_sys_state.pMsgpool->hdr_variable.message_num, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "     message_size: %d \n",   schdr_sys_state.pMsgpool->hdr_variable.message_size, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "      buffer_size: %d \n",   schdr_sys_state.pMsgpool->hdr_variable.message_buffersize, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "        pool_base: 0x%X \n", ambacv_c2p(schdr_sys_state.pMsgpool->hdr_variable.messagepool_basedaddr), 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, " \n", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return retcode;
}

