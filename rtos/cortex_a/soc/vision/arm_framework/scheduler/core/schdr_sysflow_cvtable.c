/**
 *  @file schdr_cvtable.c
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
 *  @details Scheduler cvtable APIs
 *
 */

#include "os_api.h"
#include "dram_mmap.h"
#include "schdrmsg_def.h"
#include "schdr_api.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "cvtask_errno.h"
#include <cvsched_tbar_format.h>
#include <cvsched_tbar_format.h>      /* for tbar file formats */
#include <cvsched_sysflow_format.h>   /* for sysflow_entry_t */

#define INTERNAL_TBAR_VERSION       2U
#define MAX_ENTRY_NUM               32U
#define MAX_TABLE_LENGTH            (16U*1024U)

typedef struct {
    uint32_t        sysflow_sets;
    uint32_t        sysflow_len;
    uint32_t        sysflow_line[MAX_SYSFLOW_COUNT];
    char*           sysflow_addr[MAX_SYSFLOW_COUNT];
    schdr_fill_cb   sysflow_fill[MAX_SYSFLOW_COUNT];
    void*           sysflow_token[MAX_SYSFLOW_COUNT];

    uint32_t        cvtable_sets;
    uint32_t        cvtable_len;
    uint32_t        cvtable_size[SUPERDAG_MAX_CVTABLE_PER_ARM];
    schdr_fill_cb   cvtable_fill[SUPERDAG_MAX_CVTABLE_PER_ARM];
    void*           cvtable_token[SUPERDAG_MAX_CVTABLE_PER_ARM];
} user_init_data_t;

static char stable_buf[MAX_TABLE_LENGTH] GNU_SECTION_NOZEROINIT;
static tbar_file_header_t *stable;
static user_init_data_t user_data;

static uint32_t cvtable_align(uint32_t size)
{
    return ((size) + TBAR_ENTRY_ALIGNMENT - 1U) & ~(TBAR_ENTRY_ALIGNMENT - 1U);
}

uint32_t schdr_internal_cvtable_reset(void)
{
    const char *ptr = &stable_buf[0];
    uint32_t retcode = ERRCODE_NONE;

    retcode = thread_memset(stable_buf, 0, sizeof(stable_buf));
    if(retcode == ERRCODE_NONE) {
        typecast(&stable,&ptr);
        stable->tbar_version    = INTERNAL_TBAR_VERSION;
        stable->toc_num_entries = 0;
        stable->toc_entry_size  = (uint32_t)sizeof(tbar_toc_entry_v2_t);
        stable->toc_offset      = (uint32_t)sizeof(tbar_file_header_t);
        stable->tbar_total_size = (uint32_t)sizeof(tbar_file_header_t) + (uint32_t)(MAX_ENTRY_NUM * stable->toc_entry_size);
    }
    return retcode;
}

uint32_t schdr_internal_cvtable_get_size(void)
{
    uint32_t ret;
    if(stable->toc_num_entries == 0U) {
        ret = 0U;
    } else {
        ret = stable->tbar_total_size;
    }
    return ret;
}

uint32_t schdr_internal_cvtable_copy(char *dst)
{
    uint32_t size = 0U;

    if(dst == NULL) {
        console_printU5("[ERROR] schdr_internal_cvtable_copy() : dst == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if(thread_memcpy(dst, stable_buf, stable->tbar_total_size) != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_internal_cvtable_copy() : thread_memcpy fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            if(ambacv_cache_clean(dst, stable->tbar_total_size) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_internal_cvtable_copy() : ambacv_cache_clean fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                size = stable->tbar_total_size;
            }
        }
    }
    return size;
}

uint32_t schdr_cvtable_get_size(void)
{
    return user_data.cvtable_len;
}

uint32_t schdr_sysflow_get_size(void)
{
    return user_data.sysflow_len;
}

uint32_t schdr_check_cvtable_size(void)
{
    uint32_t retcode = ERRCODE_NONE;

    /* handle internal cvtable */
    uint32_t sc_size = schdr_internal_cvtable_get_size();

    if ((user_data.cvtable_sets == SUPERDAG_MAX_CVTABLE_PER_ARM) && (sc_size != 0U)) {
        console_printU5("[ERROR] schdr_start() : Too many cvtables!\n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_CVTABLE_OUT_OF_RANGE;
    } else {
        user_data.cvtable_len += cvtable_align(sc_size);
    }

    return retcode;
}

int32_t schdr_insert_cvtable_entry(const char *name, const void *data, int32_t length)
{
    uint32_t len = (uint32_t)length;
    uint32_t retcode = ERRCODE_NONE;

    if(name == NULL) {
        console_printU5("[ERROR] schdr_insert_cvtable_entry() : name == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else if(data == NULL) {
        console_printU5("[ERROR] schdr_insert_cvtable_entry() : data == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        if ((schdr_sys_state.module_init & SCHDR_MODULE) == 0U) {
            console_printU5(
                "[ERROR] schdr_insert_cvtable_entry() : schdr not inited ",
                0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_NOT_ACTIVATE;
        } else if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {
            console_printU5(
                "[ERROR] schdr_insert_cvtable_entry() : should be called between schdr_init() and schdr_start() ",
                0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
        } else if (stable->toc_num_entries >= MAX_ENTRY_NUM) {
            console_printU5(
                "[ERROR] schdr_insert_cvtable_entry() : internal cvtable has no more entry ",
                0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CVTABLE_OUT_OF_RANGE;
        } else if ((stable->tbar_total_size + len) > MAX_TABLE_LENGTH) {
            console_printU5(
                "[ERROR] schdr_insert_cvtable_entry() : internal cvtable has no more space ",
                0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CVTABLE_SIZE_OUT_OF_RANGE;
        } else if ((thread_strlen(name)+1U) >= NAME_MAX_LENGTH) {
            console_printU5(
                "[ERROR] schdr_insert_cvtable_entry() : name too long ",
                0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CVTABLE_SIZE_OUT_OF_RANGE;
        } else {
            uint32_t offset;
            tbar_toc_entry_v2_t *entry;
            const char *ptr;
            const char *dst_ptr;

            offset = (uint32_t)sizeof(tbar_file_header_t) + (uint32_t)(stable->toc_num_entries * stable->toc_entry_size);
            ptr = &stable_buf[offset];
            typecast(&entry,&ptr);

            dst_ptr = thread_strcpy(entry->table_name, name);
            entry->table_size = len;

            retcode = thread_memcpy(&stable_buf[stable->tbar_total_size], data, len);
            if(retcode == ERRCODE_NONE) {
                entry->table_offset    = stable->tbar_total_size;
                len = (((len) + TBAR_ENTRY_ALIGNMENT - 1U) & ~(TBAR_ENTRY_ALIGNMENT - 1U));
                stable->tbar_total_size += len;
                stable->toc_num_entries++;
            }
            (void) dst_ptr;
        }
    }

    return (int32_t)retcode;
}

uint32_t schdr_cvtable_patch_addr(uint64_t base, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;
    tbar_file_header_t *hdr;
    uint32_t i, num_entries;
    const void *ptr;

    ptr = ambacv_c2v(base);
    if(ptr == NULL) {
        retcode = ERR_DRV_SCHDR_CVTABLE_INVALID;
    } else {
        typecast(&hdr,&ptr);
        hdr->toc_offset      += (uint32_t)base;
        hdr->tbar_total_size  = (uint32_t)size;

        num_entries = hdr->toc_num_entries;
        if (hdr->tbar_version == 2U) {
            tbar_toc_entry_v2_t *entry;

            ptr = ambacv_c2v(hdr->toc_offset);
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_CVTABLE_TOC_INVALID;
            } else {
                typecast(&entry,&ptr);
                //printf("patch tbar, en=%d\n", hdr->num_entries);
                for (i = 0U; i < num_entries; i++) {
                    entry->table_offset += (uint32_t)base;
                    entry++;
                }
            }
        } else {
            tbar_toc_entry_v1_t *entry;

            ptr = ambacv_c2v(hdr->toc_offset);
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_CVTABLE_TOC_INVALID;
            } else {
                typecast(&entry,&ptr);
                //printf("patch tbar, en=%d\n", hdr->num_entries);
                for (i = 0U; i < num_entries; i++) {
                    entry->table_offset += (uint32_t)base;
                    entry++;
                }
            }
        }
    }

    return retcode;
}

int32_t schdr_add_cvtable(void *token, schdr_fill_cb cb)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index = user_data.cvtable_sets;
    uint32_t size;

    if(token == NULL) {
        console_printU5("[ERROR] schdr_add_cvtable() token == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else if (cb == NULL) {
        console_printU5("[ERROR] schdr_add_cvtable() cb == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        size = (uint32_t)cb(NULL, 0, token);
        /* fail if we've reached max sets */
        if (index >= SUPERDAG_MAX_CVTABLE_PER_ARM) {
            retcode = ERR_DRV_SCHDR_CVTABLE_OUT_OF_RANGE;
            console_printU5("[ERROR] schdr_add_cvtable() : index (%d) >= SUPERDAG_MAX_CVTABLE_PER_ARM (%d) \n", index, SUPERDAG_MAX_CVTABLE_PER_ARM, 0U, 0U, 0U);
        } else {
            if (size <= 32U) {
                console_printU5("[ERROR] schdr_add_cvtable() : Ignoring possibly invalid table (size = %d)\n", size, 0U, 0U, 0U, 0U);
            } else {
                /* copy cvtable binary into memory */
                user_data.cvtable_token[index] = token;
                user_data.cvtable_fill[index] = cb;
                user_data.cvtable_size[index] = size;
                user_data.cvtable_sets++;
                user_data.cvtable_len += cvtable_align(size);
            }
        }
    }
    return (int32_t)retcode;
}

int32_t schdr_add_sysflow(void *token, schdr_fill_cb cb)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t nlines, index = user_data.sysflow_sets;
    uint32_t size;

    if(token == NULL) {
        console_printU5("[ERROR] schdr_add_sysflow() : token == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else if (cb == NULL) {
        console_printU5("[ERROR] schdr_add_sysflow() cb == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        size = (uint32_t)cb(NULL, 0, token);
        /* update sysflow_lines value */
        nlines = size / ((uint32_t)sizeof(sysflow_entry_t));
        if((uint32_t)size != (nlines * sizeof(sysflow_entry_t))) {
            console_printU5("[ERROR] schdr_add_sysflow() : size (%d) != (nlines * sizeof(sysflow_entry_t)) (%d)\n", size, ((uint64_t)nlines * (uint64_t)sizeof(sysflow_entry_t)), 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_SYSFLOW_SIZE_MISMATCHED;
        } else {
            /* fail if we've reached max sets */
            if (index >= MAX_SYSFLOW_COUNT) {
                console_printU5("[ERROR] schdr_add_sysflow() : index (%d) >= MAX_SYSFLOW_COUNT (%d)\n", index, MAX_SYSFLOW_COUNT, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_SYSFLOW_OUT_OF_RANGE;
            } else {
                user_data.sysflow_token[index] = token;
                user_data.sysflow_fill[index] = cb;
                user_data.sysflow_line[index] = nlines;
                user_data.sysflow_sets++;
                user_data.sysflow_len += size;
            }
        }
    }
    return (int32_t)retcode;
}

uint32_t schdr_handle_sysflow(schedmsg_boot_setup2_reply_t *rpl, uint64_t sysflow_addr)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index, len;
    char *dst;
    uint64_t addr1,addr2;
    const void *ptr;

    if (user_data.sysflow_sets != 0U) {
        /* concat all sysflow into a single one */
        rpl->sysflow_table_num_sets = 1U;
        rpl->sysflow_table_set_daddr[0] = (uint32_t)sysflow_addr;
        rpl->sysflow_table_set_numlines[0] =
            user_data.sysflow_len / ((uint32_t)sizeof(sysflow_entry_t));
        ptr = ambacv_c2v(sysflow_addr);
        if(ptr == NULL) {
            console_printU5("[ERROR] schdr_handle_boot_setup2_sysflow() : dst = ambacv_c2v(sysflow_addr) fail", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_SYSFLOW_INVALID;
        } else {
            typecast(&dst,&ptr);
            for(index = 0U; index < user_data.sysflow_sets; index++) {
                len = user_data.sysflow_line[index];
                len *= (uint32_t)sizeof(sysflow_entry_t);
                if( (uint32_t)user_data.sysflow_fill[index](dst, (int32_t)len,
                        user_data.sysflow_token[index]) != len) {
                    retcode = ERR_DRV_SCHDR_SYSFLOW_FILL_FAIL;
                    break;
                }
                user_data.sysflow_addr[index] = dst;
                dst = &dst[len];
            }

            /* output aggregated sfb if requested */
            if ((schdr_sys_cfg.flags & SCHDR_AGGREGATE_SYSFLOW) != 0U) {
                THREAD_FILE *fp = thread_fopen("aggregated.sfb", "wb");
                const char *base;

                if(fp != NULL) {
                    ptr = ambacv_c2v(sysflow_addr);
                    if(ptr == NULL) {
                        console_printU5("[ERROR] schdr_handle_boot_setup2_sysflow() : base = ambacv_c2v(sysflow_addr) fail", 0U, 0U, 0U, 0U, 0U);
                        retcode = ERR_DRV_SCHDR_SYSFLOW_INVALID;
                    } else {
                        typecast(&base,&ptr);
                        typecast(&addr1,&dst);
                        typecast(&addr2,&base);
                        if( thread_fwrite(base, 1U, (uint32_t)(addr1-addr2), fp) != (uint32_t)(addr1-addr2)) {
                            retcode = ERR_DRV_SCHDR_FILE_WRITE_FAIL;
                        }
                    }
                    if(thread_fclose(fp) != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_handle_boot_setup2_sysflow() : thread_fclose fail", 0U, 0U, 0U, 0U, 0U);
                    }
                } else {
                    retcode = ERR_DRV_SCHDR_FILE_DESC_INVALID;
                }
            }
        }
    } else {
        rpl->sysflow_table_num_sets = 0U;
    }

    return retcode;
}

uint32_t schdr_handle_cvtable(schedmsg_boot_setup2_reply_t *rpl, uint64_t cvtable_addr)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index, len;
    uint32_t *cvtable_ptr;
    char *dst;
    const void *ptr;

    rpl->cvtable_num_sets = user_data.cvtable_sets;
    cvtable_ptr = rpl->cvtable_daddr;
    if(cvtable_addr != 0U) {
        ptr = ambacv_c2v(cvtable_addr);
        if(ptr == NULL) {
            console_printU5("[ERROR] schdr_handle_boot_setup2_cvtable() : dst = ambacv_c2v(cvtable_addr) fail", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CVTABLE_INVALID;
        } else {
            typecast(&dst,&ptr);
            /* insert internal cvtable first */
            if (schdr_internal_cvtable_get_size() != 0U) {
                rpl->cvtable_num_sets++;
                len = schdr_internal_cvtable_copy(dst);
                retcode = schdr_cvtable_patch_addr(ambacv_v2c(dst), len);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_handle_boot_setup2_cvtable() : schdr_cvtable_patch_addr fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
                } else {
                    retcode = ambacv_cache_clean(dst, len);         /* Add explicit cache clean here */
                    if(retcode != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_handle_boot_setup2_cvtable() : ambacv_cache_clean fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
                    }
                    *cvtable_ptr = (uint32_t)ambacv_v2c(dst);
                    cvtable_ptr = &cvtable_ptr[1];
                    dst = &dst[cvtable_align(len)];
                }
            }

            /* insert user-provided cvtabls */
            for(index = 0U; index < user_data.cvtable_sets; index++) {
                len = user_data.cvtable_size[index];
                if( (uint32_t)user_data.cvtable_fill[index](dst, (int32_t)len,
                        user_data.cvtable_token[index]) != len) {
                    retcode = ERR_DRV_SCHDR_CVTABLE_FILL_FAIL;
                    break;
                }
                retcode = schdr_cvtable_patch_addr(ambacv_v2c(dst), len);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_handle_boot_setup2_cvtable() : schdr_cvtable_patch_addr fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
                } else {
                    retcode = ambacv_cache_clean(dst, len);         /* Add explicit cache clean here */
                    if(retcode != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_handle_boot_setup2_cvtable() : ambacv_cache_clean fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
                    }
                    *cvtable_ptr = (uint32_t)ambacv_v2c(dst);
                    cvtable_ptr = &cvtable_ptr[1];
                    dst = &dst[cvtable_align(len)];
                }
            }
        }
    }

    return retcode;
}

void schdr_sysflow_cvtable_reset(void)
{
    /* reset static data */
    if(thread_memset(&user_data, 0, sizeof(user_data)) != ERRCODE_NONE) {
        console_printU5("[ERROR] schdr_sysflow_cvtable_reset() : thread_memset fail ", 0U, 0U, 0U, 0U, 0U);
    }
}

int32_t schdr_wake_feeders(int32_t wake_all_feeders, uint32_t target_uuid)
{
    armvis_msg_t msg = {0};
    uint32_t retcode = ERRCODE_NONE;

    /* if schdr is not started yet, return immediately */
    if (schdr_sys_state.state == SCHDR_STATE_ACTIVE) {

        /* request master scheduder to shutdown */
        msg.hdr.message_type  = (uint16_t)SCHEDMSG_WAKE_FEEDERS;
        if (wake_all_feeders != 0) {
            msg.msg.wake_feeders.targeted_wake = 0U;
            msg.msg.wake_feeders.target_uuid   = 0U;
        } /* if (wake_all_feeders != 0) */
        else { /* if (wake_all_feeders == 0) */
            msg.msg.wake_feeders.targeted_wake = 1U;
            msg.msg.wake_feeders.target_uuid   = target_uuid;
        } /* if (wake_all_feeders == 0) */
        retcode = schdrmsg_send_msg(&msg, (uint32_t)sizeof(msg));
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_wake_feeders() : schdrmsg_send_msg fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
        }
    }

    return (int32_t)retcode;
}
