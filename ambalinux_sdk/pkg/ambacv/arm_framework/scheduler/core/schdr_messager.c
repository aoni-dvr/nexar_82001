/**
 *  @file schdr_api.c
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
 *  @details Scheduler APIs
 *
 */

#include "os_api.h"
#include "schdrmsg_def.h"
#include "schdr_api.h"
#include "dram_mmap.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "cavalry.h"
#include "thpool.h"
#include "cvtask_errno.h"
#include "cvapi_logger_interface.h"
#if !defined(__QNXNTO__)
#include "build_version.h"
#endif

static void default_error_printer_part1(const schedmsg_superdag_error_request_t *pErrorMsg, uint32_t  *phandled, uint32_t *hit)
{
    const void *ptr;
    const char *name,*name1,*name2,*name3;
    char line[256];
    uint32_t  pos = 0U;

    (void) pErrorMsg;
    *hit = 1U;
    switch (pErrorMsg->retcode) {
    case ERR_TASKLIST_HAS_NONUNIQUE_NAME:
        if (pErrorMsg->arg2 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg2);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_TASKLIST_HAS_NONUNIQUE_NAME / 0x%08x) : scheduler_cvtasktable_validate() : FD[%3d] : VISORC Tasklist contains a non-unique cvtask ",(uint32_t)pErrorMsg->retcode, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg2 != 0) */
        break;

    case ERR_ARM_TASKLIST_HAS_NONUNIQUE_NAME:
        if (pErrorMsg->arg3 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg3);
            typecast(&name,&ptr);
            pos += snprintf_uint3(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_ARM_TASKLIST_HAS_NONUNIQUE_NAME / 0x%08x) : scheduler_add_arm_cvtask() : FD[%3d] : ARM[%d] has more than one entry for cvtask ", pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg3 != 0) */
        break;

    case ERR_TASKLIST_HAS_INVALID_ORCANY:
        if (pErrorMsg->arg3 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg3);
            typecast(&name,&ptr);
            pos += snprintf_uint3(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_TASKLIST_HAS_INVALID_ORCANY / 0x%08x) : scheduler_sysflowtable_validate() : FD[%3d] : (UUID=%10u) : Unknown ORCANY CVTask reported; cvtask ", pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg3 != 0) */
        break;

    case ERR_INVALID_CVTASK_ON_VISORC:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint1(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_INVALID_CVTASK_ON_VISORC / 0x%08x) : cvtask ", pErrorMsg->arg0);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);
        } /* if (pErrorMsg->arg4 != 0) */
        *phandled = 1U;
        break;

    case ERR_SYSFLOW_NONUNIQUE_NAME:
        if ((pErrorMsg->arg1 != 0U) && (pErrorMsg->arg2 != 0U) && (pErrorMsg->arg3 != 0U) && (pErrorMsg->arg4 != 0U)) {
            ptr = ambacv_c2v(pErrorMsg->arg1);
            typecast(&name,&ptr);
            ptr = ambacv_c2v(pErrorMsg->arg2);
            typecast(&name1,&ptr);
            ptr = ambacv_c2v(pErrorMsg->arg3);
            typecast(&name2,&ptr);
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name3,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_SYSFLOW_NONUNIQUE_NAME / 0x%08x) : scheduler_sysflowtable_validate() : FD[%3d] : Sysflow table has duplicated name ", ERR_SYSFLOW_NONUNIQUE_NAME, pErrorMsg->arg0);
            pos += snprintf_str4(&line[pos], (uint32_t)sizeof(line), "(%s %s %s %s)\n",name, name1, name2, name3);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg1 != 0) */
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void default_error_printer_part2(const schedmsg_superdag_error_request_t *pErrorMsg, uint32_t  *phandled, uint32_t *hit)
{
    const void *ptr;
    const char *name;
    char line[256];
    uint32_t  pos = 0U;

    (void) pErrorMsg;
    *hit = 1U;
    switch (pErrorMsg->retcode) {
    case ERR_CVTASK_HAS_NO_QUERY_FUNCTION:
        if (pErrorMsg->arg2 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg2);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_HAS_NO_QUERY_FUNCTION / 0x%08x) : scheduler_validate_cvtask_register() : FD[%3d] : query function not provided for cvtask ", pErrorMsg->arg0, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg2 != 0) */
        break;

    case ERR_CVTASK_HAS_NO_INIT_FUNCTION:
        if (pErrorMsg->arg2 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg2);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_HAS_NO_INIT_FUNCTION / 0x%08x) : scheduler_validate_cvtask_register() : FD[%3d] : init function not provided for cvtask ", pErrorMsg->arg0, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg2 != 0) */
        break;

    case ERR_CVTASK_HAS_NO_GET_INFO_FUNCTION:
        if (pErrorMsg->arg2 != 0U) {
            if (pErrorMsg->arg4 == 0U) {
                ptr = ambacv_c2v(pErrorMsg->arg2);
                typecast(&name,&ptr);
                pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_HAS_NO_GET_INFO_FUNCTION / 0x%08x) : scheduler_validate_cvtask_register() : FD[%3d] : get_info function not provided for cvtask ", pErrorMsg->arg0, pErrorMsg->arg1);
                pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
                (void) pos;
                console_printS5("%s",line,NULL,NULL,NULL,NULL);
            } /* if (pErrorMsg->arg4 == 0) */
            else { /* if (pErrorMsg->arg4 != 0) */
                ptr = ambacv_c2v(pErrorMsg->arg2);
                typecast(&name,&ptr);
                pos += snprintf_uint1(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_HAS_NO_GET_INFO_FUNCTION / 0x%08x) : scheduler_validate_cvtask_register() : FD[SYS] : get_info function not provided for internal cvtask ", pErrorMsg->arg0);
                pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
                (void) pos;
                console_printS5("%s",line,NULL,NULL,NULL,NULL);
            } /* if (pErrorMsg->arg4 != 0) */
            *phandled = 1U;
        } /* if (pErrorMsg->arg2 != 0) */
        break;

    case ERR_CVTASK_HAS_NO_MESSAGE_FUNCTION:
        if (pErrorMsg->arg2 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg2);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_HAS_NO_MESSAGE_FUNCTION / 0x%08x) : scheduler_validate_cvtask_register() : FD[%3d] : process_messages function not provided for ", pErrorMsg->arg0, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "cvtask (%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg2 != 0) */
        break;

    case ERR_CVTASK_HAS_NO_RUN_FUNCTION:
        if (pErrorMsg->arg1 != 0U) {
            if (pErrorMsg->arg4 == 0U) {
                ptr = ambacv_c2v(pErrorMsg->arg2);
                typecast(&name,&ptr);
                pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_HAS_NO_RUN_FUNCTION / 0x%08x) : scheduler_validate_cvtask_register() : FD[%3d] : run function not provided for cvtask ", pErrorMsg->arg0, pErrorMsg->arg1);
                pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
                (void) pos;
                console_printS5("%s",line,NULL,NULL,NULL,NULL);
            } /* if (pErrorMsg->arg4 == 0) */
            else { /* if (pErrorMsg->arg4 != 0) */
                ptr = ambacv_c2v(pErrorMsg->arg2);
                typecast(&name,&ptr);
                pos += snprintf_uint1(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_HAS_NO_RUN_FUNCTION / 0x%08x) : scheduler_validate_cvtask_register() : FD[SYS] : run function not provided for internal cvtask ", pErrorMsg->arg0);
                pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
                (void) pos;
                console_printS5("%s",line,NULL,NULL,NULL,NULL);
            } /* if (pErrorMsg->arg4 != 0) */
            *phandled = 1U;
        } /* if (pErrorMsg->arg1 != 0) */
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void default_error_printer_part3(const schedmsg_superdag_error_request_t *pErrorMsg, uint32_t  *phandled, uint32_t *hit)
{
    const void *ptr;
    const char *name;
    char line[256];
    uint32_t  pos = 0U;

    (void) pErrorMsg;
    *hit = 1U;
    switch (pErrorMsg->retcode) {
    case ERR_CVTASK_MISMATCHED_CVAPI:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_MISMATCHED_CVAPI / 0x%08x) : scheduler_validate_cvtask_register() : FD[%3d] :", pErrorMsg->arg0, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "CVTask (%s) ",name);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "reports API version %d, scheduler requires %d\n", pErrorMsg->arg2, pErrorMsg->arg3);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg4 == 0) */
        break;

    case ERR_CVTASK_MISMATCHED_CVCHIP:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_MISMATCHED_CVCHIP / 0x%08x) : scheduler_validate_cvtask_register() : FD[%3d] :", pErrorMsg->arg0, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "CVTask (%s) ",name);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "reports CVCHIP of 0x%02x, should be 0x%02x\n", pErrorMsg->arg2, pErrorMsg->arg3);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg4 != 0) */
        break;

    case ERR_CVTASK_NONUNIQUE_OUTPUT_IONAME:
        if (pErrorMsg->arg3 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg3);
            typecast(&name,&ptr);
            pos += snprintf_uint3(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_NONUNIQUE_OUTPUT_IONAME / 0x%08x) : visorc_process_query_response() : FD[%3d] : (UUID=%10u) : Nonunique CVTask Hash found, ", pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "io_name (%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg3 != 0) */
        break;

    case ERR_CVTASK_REQUIRES_TOO_MUCH_CMEM:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint1(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_REQUIRES_TOO_MUCH_CMEM / 0x%08x) : visorc_process_cvtask_queries() :", pErrorMsg->arg0);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "cvtask (%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);
        } /* if (pErrorMsg->arg4 != 0) */
        *phandled = 1U;
        break;

    case ERR_CVTASK_REQUIRES_UNAVAILABLE_UNIT:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint1(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_CVTASK_REQUIRES_UNAVAILABLE_UNIT / 0x%08x) : visorc_process_cvtask_queries() : ", pErrorMsg->arg0);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "cvtask (%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);
        } /* if (pErrorMsg->arg4 != 0) */
        *phandled = 1U;
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void default_error_printer_part4(const schedmsg_superdag_error_request_t *pErrorMsg, uint32_t  *phandled, uint32_t *hit)
{
    const void *ptr;
    const char *name;
    char line[256];
    uint32_t  pos = 0U;

    (void) pErrorMsg;
    *hit = 1U;
    switch (pErrorMsg->retcode) {
    case ERR_SYSFLOW_CANNOT_FIND_CVTASK:
        if (pErrorMsg->arg3 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg3);
            typecast(&name,&ptr);
            pos += snprintf_uint3(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_SYSFLOW_CANNOT_FIND_CVTASK / 0x%08x) : scheduler_sysflowtable_validate() : FD[%3d] : Cannot find CVTask requested by (UUID=%10u). CVTask ", pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg3 != 0) */
        break;

    case ERR_UNKNOWN_INTERNAL_FUNCTION:
        if (pErrorMsg->arg2 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg2);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_UNKNOWN_INTERNAL_FUNCTION / 0x%08x) : scheduler_tasktable_patch_internal_functions() : Unknown internal function (%3d) found in registered cvtasks; CVTask ", pErrorMsg->arg0, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg2 != 0) */
        break;

    case ERR_REPEATED_INTERNAL_FUNCTION:
        if (pErrorMsg->arg2 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg2);
            typecast(&name,&ptr);
            pos += snprintf_uint2(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_REPEATED_INTERNAL_FUNCTION / 0x%08x) : scheduler_tasktable_patch_internal_functions() : Repeated internal function (%3d) found in registered cvtasks; CVTask ", pErrorMsg->arg0, pErrorMsg->arg1);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg2 != 0) */
        break;

    case ERR_SYSFLOWLINK_INPUT_IONAME:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint4(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_SYSFLOWLINK_INPUT_IONAME / 0x%08x) : scheduler_sysflowtable_link() : FD[%3d] : Cannot link (UUID=%10u) to input from (UUID=%10u) ", pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2, pErrorMsg->arg3);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "ioname=%s\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg4 != 0) */
        break;

    case ERR_SYSFLOWLINK_FEEDBACK_IONAME:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint4(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_SYSFLOWLINK_FEEDBACK_IONAME / 0x%08x) : scheduler_sysflowtable_link() : FD[%3d] : Cannot link (UUID=%10u) to feedback from (UUID=%10u) ", pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2, pErrorMsg->arg3);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "ioname=%s\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg4 != 0) */
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void default_error_printer_part5(const schedmsg_superdag_error_request_t *pErrorMsg, uint32_t  *phandled, uint32_t *hit)
{
    const void *ptr;
    const char *name;
    char line[256];
    uint32_t  pos = 0U;

    (void) pErrorMsg;
    *hit = 1U;
    switch (pErrorMsg->retcode) {
    case ERR_FLEXIDAG_INVALID_SYSFLOW_ENTRY:
        if (pErrorMsg->arg4 != 0U) {
            ptr = ambacv_c2v(pErrorMsg->arg4);
            typecast(&name,&ptr);
            pos += snprintf_uint3(&line[pos], (uint32_t)sizeof(line), "[ERROR] : (ERR_FLEXIDAG_INVALID_SYSFLOW_ENTRY / 0x%08x) : scheduler_sysflowtable_validate() : FD[%3d] : Invalid sysflow entry (%d) for $##FLEXIDAG control cvtask; instance_name ", pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2);
            pos += snprintf_str1(&line[pos], (uint32_t)sizeof(line), "(%s)\n",name);
            (void) pos;
            console_printS5("%s",line,NULL,NULL,NULL,NULL);

            *phandled = 1U;
        } /* if (pErrorMsg->arg4 != 0) */
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void default_error_printer(const char *pString, const schedmsg_superdag_error_request_t *pErrorMsg)
{
    uint32_t  handled = 0U, hit = 0U;

    (void) pString;
    (void) pErrorMsg;
    /* Note: These error code prints are still evolving.  Once they have been finalized, I will remove this comment and then the prints can be ported over for RTOS mode */
    default_error_printer_part1(pErrorMsg, &handled, &hit);
    if(hit == 0U) {
        default_error_printer_part2(pErrorMsg, &handled, &hit);
        if(hit == 0U) {
            default_error_printer_part3(pErrorMsg, &handled, &hit);
            if(hit == 0U) {
                default_error_printer_part4(pErrorMsg, &handled, &hit);
                if(hit == 0U) {
                    default_error_printer_part5(pErrorMsg, &handled, &hit);
                }
            }
        }
    }

    if (handled == 0U) {
        console_printU5(pString, pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2, pErrorMsg->arg3, pErrorMsg->arg4);
    } /* if (handled == 0) */
}

static uint32_t schdr_handle_set_phase(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t  size;

    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_set_phase() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        // wait until all jobs are done for the current phase
        retcode = thpool_wait_for_finish();
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_set_phase() : boot_phase (0x%x) thpool_wait_for_finish fail retcode (0x%x)", msg->msg.set_bootup_phase.bootup_phase, retcode, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            // send cb event to application
            if (schdr_sys_state.app_callback != NULL) {
                if( schdr_sys_state.app_callback((int32_t)msg->msg.set_bootup_phase.bootup_phase, NULL) != 0) {
                    console_printU5("[ERROR] schdr_handle_set_phase() : schdr_sys_state.app_callback fail ", 0U, 0U, 0U, 0U, 0U);
                }
            }

            switch (msg->msg.set_bootup_phase.bootup_phase) {
            case REGISTRATION_PHASE:
                retcode = schdr_set_env(&schdr_sys_state.thenv, thread_self());
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_handle_set_phase() : schdr_set_env fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
                }

                if(retcode == ERRCODE_NONE) {
                    retcode = thpool_create_thenv();
                    if(retcode != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_handle_set_phase() : boot_phase (0x%x) thpool_create_thenv fail retcode (0x%x)", msg->msg.set_bootup_phase.bootup_phase, retcode, 0U, 0U, 0U);
                    }
                }
                break;
            case INITIALIZATION_PHASE:
                size = cvtask_get_scratchpad_size();
                retcode = thpool_create_thenv_scratchpads(size);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_handle_set_phase() : boot_phase (0x%x) thpool_create_thenv_scratchpads fail retcode (0x%x)", msg->msg.set_bootup_phase.bootup_phase, retcode, 0U, 0U, 0U);
                }
                break;
            case ACTIVE_PHASE:
                msg->hdr.message_type     = (uint16_t)SCHEDMSG_CVSCHEDULER_RUN;
                msg->hdr.message_retcode  = 0U;
                retcode = schdrmsg_send_msg(msg, 0U);
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_handle_set_phase() : boot_phase (0x%x) schdrmsg_send_msg fail retcode (0x%x)", msg->msg.set_bootup_phase.bootup_phase, retcode, 0U, 0U, 0U);
                }
                break;
            case QUERY_PHASE:
            case DISCOVERY_PHASE:
                break;
            default :
                console_printU5("[ERROR] schdr_handle_set_phase() : boot_phase (0x%x) not support ", msg->msg.set_bootup_phase.bootup_phase, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_BOOTUP_PHASE_UNKNOW;
                break;
            }
        }
    }

    return retcode;
}

static uint32_t schdr_handle_init_setup(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    schedmsg_initial_setup_reply_t *rpl;

    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_init_setup() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        rpl = &msg->msg.reply_initial_setup;

        msg->hdr.message_retcode    = ERRCODE_NONE;
        rpl->arm_major_version      = CVSCHEDULER_INTERFACE_VERSION;
        rpl->arm_minor_version      = CVTASK_API_VERSION;
        rpl->arm_num_cvtask         = cvtask_get_total_class();
        retcode = schdrmsg_send_msg(msg, (uint32_t)sizeof(*rpl));
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_init_setup() : schdrmsg_send_msg fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }
    }

    return retcode;
}

static uint32_t schdr_handle_boot_setup1(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_boot_setup1_t *req;
    schedmsg_boot_setup1_reply_t *rpl;
#ifndef DISABLE_ARM_CVTASK
    uint64_t addr1 = 0UL,addr2 = 0UL;
    const char *paddr1,*paddr2;
#endif

    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_boot_setup1() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.boot_setup1;
        rpl = &msg->msg.reply_boot_setup1;

        if ((req->scheduler_version != CVSCHEDULER_INTERFACE_VERSION) ||
            (req->cvtask_version != CVTASK_API_VERSION)) {
            console_printU5(
                "[ERROR]  schdr_handle_boot_setup1() : Scheduler Version Mismatch: "
                "ARM=(0x%X, 0x%x), ORC=(0x%X, 0x%x)!!!!!!\n",
                CVSCHEDULER_INTERFACE_VERSION, CVTASK_API_VERSION,
                req->scheduler_version, req->cvtask_version, 0U);
            retcode = ERR_DRV_SCHDR_VERSION_MISMATCHED;
        }

        if(retcode == ERRCODE_NONE) {
            drv_schdr_set_pSchedulerTrace(req->scheduler_trace_daddr);
            drv_schdr_set_pFlexidagTrace(req->flexidag_trace_daddr);
#ifndef DISABLE_CAVALRY
            drv_schdr_set_pCavalryTrace(req->cavalry_trace_daddr);
#endif
            drv_schdr_set_pAutoRunTrace(req->autorun_trace_daddr);

            msg->hdr.message_retcode  = CVTASK_ERR_OK;
            rpl->scheduler_version    = CVSCHEDULER_INTERFACE_VERSION;
            rpl->cvtask_version       = CVTASK_API_VERSION;
            rpl->cvtask_total_num     = cvtask_get_total_class();

            // request size for each items
            rpl->memsize_arm_sysflow_block        = schdr_sysflow_get_size() + SCHDR_PRIVATE_MEM_SIZE;
            rpl->memsize_arm_cvtable_block        = schdr_cvtable_get_size();

#ifndef DISABLE_ARM_CVTASK
            paddr1 = &__cvtask_format_end;
            paddr2 = &__cvtask_format_start;
            typecast(&addr1,&paddr1);
            typecast(&addr2,&paddr2);
            rpl->memsize_printf_string_block      = (uint32_t)(addr1 - addr2);
#else
            rpl->memsize_printf_string_block      = 0U;
#endif
            rpl->memsize_arm_cvtask_debug_buffer  = SCHDR_LOG_ENTRIES_ARMCVTASK * ((uint32_t)sizeof(cvlog_cvtask_entry_t));
            rpl->memsize_arm_sched_debug_buffer   = SCHDR_LOG_ENTRIES_ARMSCHDR * ((uint32_t)sizeof(cvlog_sched_entry_t));
            rpl->memsize_arm_perf_debug_buffer    = SCHDR_LOG_ENTRIES_ARMPERF * ((uint32_t)sizeof(cvlog_perf_entry_t));

            retcode = schdrmsg_send_msg(msg, (uint32_t)sizeof(*rpl));
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_boot_setup1() : schdrmsg_send_msg fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}

#ifndef DISABLE_ARM_CVTASK
static uint32_t schdr_handle_boot_setup2_formats(uint64_t formats_addr)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t len = 0UL;
    const char *src;
    uint64_t addr1 = 0UL,addr2 = 0UL;
    const char *paddr1,*paddr2;
    void *ptr;

    src = &__cvtask_format_start;
    if(formats_addr != 0U) {
        ptr = ambacv_c2v(formats_addr);
        if(ptr == NULL) {
            console_printU5("[ERROR] schdr_handle_boot_setup2_formats() : dst = ambacv_c2v(formats_addr) fail", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_FORMAT_INVALID;
        } else {
            paddr1 = &__cvtask_format_end;
            paddr2 = &__cvtask_format_start;
            typecast(&addr1,&paddr1);
            typecast(&addr2,&paddr2);
            len = addr1 - addr2;
            if(len != 0U) {
                retcode = thread_memcpy(ptr, src, (size_t)len);
            }
        }
    }

    return retcode;
}
#endif

static uint32_t schdr_handle_boot_setup2_init(const schedmsg_boot_setup2_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t start = 0UL, end = 0UL;
    uint64_t c_start = 0UL, c_end = 0UL;

#ifdef CHIP_CV6
    retcode = schdrmsg_config_idsp(req->arm_extra_picinfo_queue_daddr, req->arm_idsp_async_queue_daddr);
#else /* !CHIP_CV6 */
    retcode = schdrmsg_config_idsp(req->arm_idsp_picinfo_queue_daddr, req->arm_idsp_async_queue_daddr);
#endif /* ?CHIP_CV6 */
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdr_handle_boot_setup2_init() : schdrmsg_config_idsp fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
    } else {
        retcode = schdrmsg_config_vin(req->arm_armvin_queue_daddr);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_boot_setup2_init() : schdrmsg_config_vin fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        /* Init mmap, need to have a little more at the bottom to pass query */
        if(req->arm_printf_buffer_info_daddr != 0U) {
            start = (((uint64_t)req->arm_printf_buffer_info_daddr - (1024UL * 1024UL)) & (~0xFFFUL));
        } else {
            start = schdr_sys_state.visorc_init.sysinit_dram_block_baseaddr;
        }
        end   = (uint64_t)schdr_sys_state.visorc_init.sysinit_dram_block_baseaddr + (uint64_t)schdr_sys_state.visorc_init.sysinit_dram_block_size;
        end  += schdr_sys_state.visorc_init.hotlink_region_size;
        end  += ((uint64_t)schdr_sys_state.visorc_init.cavalry_num_slots * CAVALRY_SLOT_SIZE);
        c_start = ambacv_c2p(start);
        c_end = ambacv_c2p(end);
        retcode = cvtask_mmap_init(c_start, c_end);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_boot_setup2_init() : cvtask_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        if(req->sysflow_block_daddr != 0U) {
            retcode = thread_memset(ambacv_c2v(req->sysflow_block_daddr), 0, SCHDR_PRIVATE_MEM_SIZE);
        }

        if(retcode == ERRCODE_NONE) {
#ifndef DISABLE_CAVALRY
            if (schdr_sys_state.visorc_init.cavalry_base_daddr != 0U) {
                if( drv_cavalry_associate_buf(schdr_sys_state.visorc_init.cavalry_base_daddr) != 0) {
                    console_printU5("[ERROR] schdr_handle_boot_setup2_init() : drv_cavalry_associate_buf fail ", 0U, 0U, 0U, 0U, 0U);
                }
            } /* if (schdr_sys_state.visorc_init.cavalry_base_daddr != 0) */
#endif

            /* Init log info */
            retcode = schdr_log_init(req->sysflow_block_daddr, req,
                                     SCHDR_LOG_ENTRIES_ARMCVTASK, SCHDR_LOG_ENTRIES_ARMSCHDR, SCHDR_LOG_ENTRIES_ARMPERF);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_boot_setup2_init() : schdr_log_init fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}

static uint32_t schdr_handle_boot_setup2(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_boot_setup2_t *req;
    schedmsg_boot_setup2_reply_t *rpl, rplreal;
    uint64_t sysflow_addr = 0UL, cvtable_addr = 0UL;
#ifndef DISABLE_ARM_CVTASK
    uint64_t formats_addr = 0UL;
#endif

    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_boot_setup2() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.boot_setup2;
        rpl = &rplreal;

        retcode = schdr_handle_boot_setup2_init(req);
        if(retcode == ERRCODE_NONE) {
            sysflow_addr = (uint64_t)req->sysflow_block_daddr + (uint64_t)SCHDR_PRIVATE_MEM_SIZE;
            retcode = schdr_handle_sysflow(rpl, sysflow_addr);
        }

        if(retcode == ERRCODE_NONE) {
            cvtable_addr = req->cvtable_block_daddr;
            retcode = schdr_handle_cvtable(rpl, cvtable_addr);
        }

#ifndef DISABLE_ARM_CVTASK
        if(retcode == ERRCODE_NONE) {
            formats_addr = req->printf_string_block_daddr;
            retcode = schdr_handle_boot_setup2_formats(formats_addr);
        }
#endif

        if(retcode == ERRCODE_NONE) {
            retcode = thread_memcpy(&msg->msg.reply_boot_setup2, rpl, sizeof(*rpl));
            if(retcode == ERRCODE_NONE) {
                retcode = schdrmsg_send_msg(msg, (uint32_t)sizeof(*rpl));
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_handle_boot_setup2() : schdrmsg_send_msg fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    return retcode;
}

static uint32_t schdr_handle_boot_setup3(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_boot_setup3_t *req;
    schedmsg_boot_setup3_reply_t *rpl;

    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_boot_setup3() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        req = &msg->msg.boot_setup3;
        rpl = &msg->msg.reply_boot_setup3;

        retcode = schdr_init_sysflow(req->sysflow_table_daddr, req->sysflow_numlines);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_boot_setup3() : schdr_init_sysflow fail retcode (0x%x) sysflow (0x%x)", retcode, req->sysflow_table_daddr, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_init_cvtable(req->cvtable_base_daddr, req->cvtable_numlines,
                                         schdr_sys_cfg.flags & SCHDR_DISABLE_CVTABLE_LOG);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_boot_setup3() : schdr_init_cvtable fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            rpl->dummy_ack = 0U;
            retcode = schdrmsg_send_msg(msg, (uint32_t)sizeof(*rpl));
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_boot_setup3() : schdrmsg_send_msg fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}

static uint32_t schdr_handle_report_memory_mmap(void)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t start,end;

    if (schdr_cma_region.va_base == NULL) { // this may be allocated by drv_cavalry_set_memory_block()
        start = ambacv_c2p(schdr_sys_state.visorc_init.cma_region_daddr);
        end = start + (uint64_t)schdr_sys_state.visorc_init.cma_region_size;
        retcode = schdr_cma_mmap_init(start, end);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_report_memory_mmap() : schdr_cma_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
    } /* if (schdr_cma_region.va_base == NULL) */

    if(retcode == ERRCODE_NONE) {
        if(schdr_sys_state.visorc_init.rtos_region_size != 0U) {
            start = ambacv_c2p(schdr_sys_state.visorc_init.rtos_region_daddr);
            end = start + (uint64_t)schdr_sys_state.visorc_init.rtos_region_size;
        } else {
            start = 0UL;
            end = 0UL;
        }
        retcode = schdr_rtos_mmap_init(start, end);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_report_memory_mmap() : schdr_rtos_mmap_init fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }
    }

    return retcode;
}

static uint32_t schdr_handle_report_memory(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvscheduler_memory_report_t *r;
    uint64_t start,end;

    (void) msg;
    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_report_memory() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        r = &msg->msg.memory_report;

        schdr_cma_region.va_base = NULL;  // Initially reset this for safety

        if(schdr_sys_state.visorc_init.cma_region_size != 0U) {
            retcode = cvtask_mmap_remap(ambacv_c2p((uint64_t)r->orc_uncached_baseaddr+(uint64_t)r->orc_uncached_size));
        } else {
            retcode = cvtask_mmap_remap(ambacv_c2p(r->armblock_baseaddr));
        }

        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_report_memory_mmap() : cvtask_mmap_remap fail ret 0x%x ", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            /* re-init sysflow/cvtable/log since mmap has changed */
            retcode = schdr_reinit_sysflow();
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_report_memory() : schdr_reinit_sysflow fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_init_buildinfo(r->CVTaskInfoTable_daddr, r->SchedInfoTable_daddr, r->sysflow_numlines);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_report_memory() : schdr_init_buildinfo fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_init_cvtable(r->cvtable_master_daddr, r->cvtable_master_num_entries,
                                         schdr_sys_cfg.flags & SCHDR_DISABLE_CVTABLE_LOG);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_report_memory() : schdr_init_cvtable fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_log_reset();
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_report_memory() : schdr_log_reset fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "##################################################### \n", 0U, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "                   memory partition \n", 0U, 0U, 0U, 0U, 0U);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "##################################################### \n", 0U, 0U, 0U, 0U, 0U);
            start = ambacv_c2p(r->orc_cached_baseaddr);
            end = ambacv_c2p((uint64_t)r->orc_cached_baseaddr + (uint64_t)r->orc_cached_size);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "       ORC cached: [0x%llX, 0x%llX] \n", start, end, 0U, 0U, 0U);
            start = ambacv_c2p(r->orc_uncached_baseaddr);
            end = ambacv_c2p((uint64_t)r->orc_uncached_baseaddr + (uint64_t)r->orc_uncached_size);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "     ORC uncached: [0x%llX, 0x%llX] \n", start,end, 0U, 0U, 0U);
            start = ambacv_c2p(r->armblock_baseaddr);
            end = ambacv_c2p((uint64_t)r->armblock_baseaddr + (uint64_t)r->armblock_size);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "              ARM: [0x%llX, 0x%llX]\t \n", start, end, 0U, 0U, 0U);
            start = ambacv_c2p(schdr_sys_state.sysflow_addr);
            end = (uint64_t)r->sysflow_numlines * (uint64_t)sizeof(sysflow_entry_t);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "     Sysflow name: [0x%llX++0x%llX]\t \n", start, end, 0U, 0U, 0U);
            start = ambacv_c2p(r->cvtable_master_daddr);
            end = (uint64_t)r->cvtable_master_num_entries * (uint64_t)sizeof(cvtable_entry_t);
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "   cvtable header: [0x%llX++0x%llX]\t \n", start, end, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
#ifndef DISABLE_CAVALRY
            retcode = (uint32_t)drv_cavalry_set_memory_block(msg);
#else
            armvis_msg_t msgbase;
            schedmsg_cavalry_setup_t *pSetupMsg;

            msgbase.hdr.message_type    = (uint16_t)SCHEDMSG_CAVALRY_SETUP;
            msgbase.hdr.message_type    |= MSGTYPEBIT_ARM_TO_VISORC;
            pSetupMsg                   = &msgbase.msg.cavalry_setup;
            pSetupMsg->log_base_daddr   = 0U;
            pSetupMsg->log_size         = 0U;
            retcode = schdrmsg_send_msg(&msgbase, (uint32_t)sizeof(schedmsg_cavalry_setup_t));
#endif
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_init_system_msg(r->CVTaskMsgPool_daddr);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_report_memory() : schdr_init_system_msg fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = schdr_handle_report_memory_mmap();
        }
    }

    return retcode;
}

static uint32_t schdr_handle_startup_log(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t tmp_boot_os = schdr_sys_state.visorc_init.coprocessor_cfg;

    (void) msg;
    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_startup_log() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        if ((schdr_sys_cfg.flags & (SCHDR_DISABLE_SS_LOG | SCHDR_FASTBOOT)) == 0U) {
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"Receive startup_log update message \n", 0U, 0U, 0U, 0U, 0U);
            retcode = schdr_startup_log_flush(msg->msg.shutdown.CVStaticLogInfo_daddr, tmp_boot_os);
        }
    }
    return retcode;
}

static uint32_t schdr_handle_shutdown(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_shutdown() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        console_printU5("Receive shutdown message", 0U, 0U, 0U, 0U, 0U);
        retcode = schdr_process_shutdown(SCHDR_ARM_ORC_RESET, msg->msg.shutdown.CVStaticLogInfo_daddr);
        console_printU5("Receive shutdown message done ret 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if (pStartupSemaphore != NULL) {
        if(thread_sem_post(pStartupSemaphore) != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_shutdown() : thread_sem_post fail ", 0U, 0U, 0U, 0U, 0U);
        }
    } /* if (pStartupSemaphore != NULL) */
    return retcode;
}

static uint32_t schdr_handle_activate(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    schdr_sys_state.state = SCHDR_STATE_ACTIVE;
    if (pStartupSemaphore != NULL) {
        retcode = thread_sem_post(pStartupSemaphore);
    } /* if (pStartupSemaphore != NULL) */
    return retcode;
}

static uint32_t schdr_handle_get_buildinfo(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_getbuildinfo_t *req;
    const char *self_info, *tool_info;
    const char un_name[32] = "un-versioned";
    const char schdr_name[32] = "ARM_SCHEDULER";
    char *info;
    const void* ptr;
    char *name;
    uint32_t  is_valid = 0U;
    const char *dest_ptr = NULL;

    if(msg == NULL) {
        console_printU5("[ERROR] schdr_handle_get_buildinfo() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else {
        if ((msg->hdr.message_id < 0x100U) || (msg->hdr.message_id >= (0x0100U + FLEXIDAG_MAX_SLOTS))) {
            is_valid = 1U;
        } /* if ((pMsg->hdr.message_id < 0x100) || (pMsg->hdr.message_id >= (0x0100 + FLEXIDAG_MAX_SLOTS))) */
        else { /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100 + FLEXIDAG_MAX_SLOTS))) */
            is_valid = is_associated_flexidag(msg);
        } /* if ((pMsg->hdr.message_id >= 0x100) && (pMsg->hdr.message_id < (0x0100 + FLEXIDAG_MAX_SLOTS))) */

        if (is_valid != 0U) {
            req = &msg->msg.cvtask_getbuildinfo;

            // for scheduler itself
            ptr = ambacv_c2v(req->daddr_component_location);
            if(ptr == NULL) {
                retcode = ERR_DRV_SCHDR_BUILDINFO_INVALID;
                console_printU5("[ERROR] schdr_handle_get_buildinfo() : name == NULL!", 0U, 0U, 0U, 0U, 0U);
            } else {
                typecast(&name,&ptr);
                dest_ptr = thread_strcpy(name, schdr_name);
            }
            self_info = LIBRARY_VERSION;
            tool_info = un_name;

            if(retcode == ERRCODE_NONE) {
                ptr = ambacv_c2v(req->daddr_versionstring_location);
                if(ptr == NULL) {
                    retcode = ERR_DRV_SCHDR_BUILDINFO_INVALID;
                    console_printU5("[ERROR] schdr_handle_get_buildinfo() : daddr_versionstring_location == NULL!", 0U, 0U, 0U, 0U, 0U);
                } else {
                    typecast(&info,&ptr);
                    dest_ptr = thread_strncpy(info, self_info, req->versionstring_maxsize);
                }
            }

            if(retcode == ERRCODE_NONE) {
                ptr = ambacv_c2v(req->daddr_toolstring_location);
                if(ptr == NULL) {
                    retcode = ERR_DRV_SCHDR_BUILDINFO_INVALID;
                    console_printU5("[ERROR] schdr_handle_get_buildinfo() : daddr_toolstring_location == NULL!", 0U, 0U, 0U, 0U, 0U);
                } else {
                    typecast(&info,&ptr);
                    dest_ptr = thread_strncpy(info, tool_info, req->versionstring_maxsize);
                }
            }

            msg->hdr.message_retcode  = CVTASK_ERR_OK;
            if (retcode == ERRCODE_NONE) {
                retcode = schdrmsg_send_msg(msg, (uint32_t)sizeof(schedmsg_cvtask_getbuildinfo_reply_t));
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_handle_get_buildinfo() : schdrmsg_send_msg fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
                }
            } else {
                console_printU5("[ERROR] schdr_handle_get_buildinfo() : type (0x%x) retcode (0x%x)\n",msg->hdr.message_type, retcode, 0U, 0U, 0U);
            }
        }
    }

    (void) dest_ptr;
    return retcode;
}

static uint32_t schdr_handle_error_message(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_superdag_error_request_t *pErrorMsg = &msg->msg.superdag_error_message;
    char *pString = NULL;
    const void *ptr;
    uint32_t i;

    if(pErrorMsg->format_daddr != 0U) {
        ptr = ambacv_c2v(pErrorMsg->format_daddr);
        if(ptr != NULL) {
            typecast(&pString,&ptr);
        }
    }

    if(pString != NULL) {
        retcode = errmgr_report_error(CV_MODULE_ID_UCODE, pErrorMsg->retcode);
        if (pErrorMsg->retcode == (uint32_t)ERR_VISORC_COREDUMP_REQUEST) {
            if ((schdr_sys_state.vpcoredump_callback != NULL) && ((pErrorMsg->arg2 == (uint32_t)CVCORE_VP0) || (pErrorMsg->arg2 == (uint32_t)CVCORE_VP1))) {
                superdag_error_struct_t errorinfo;
                errorinfo.flexidag_slot_id    = FLEXIDAG_INVALID_SLOT_ID;
                errorinfo.sysflow_index       = pErrorMsg->sysflow_index;
                errorinfo.cvtask_frameset_id  = pErrorMsg->cvtask_frameset_id;
                errorinfo.cvtask_uuid         = pErrorMsg->cvtask_uuid;
                errorinfo.retcode             = pErrorMsg->retcode;
                errorinfo.pFormatString       = pString;
                errorinfo.msg_time            = pErrorMsg->msg_time;
                errorinfo.arg0                = pErrorMsg->arg0;
                errorinfo.arg1                = pErrorMsg->arg1;
                errorinfo.arg2                = pErrorMsg->arg2;
                errorinfo.arg3                = pErrorMsg->arg3;
                errorinfo.arg4                = pErrorMsg->arg4;
                schdr_sys_state.vpcoredump_callback(schdr_sys_state.pVPCoredumpCallbackHandle, &errorinfo, schdr_sys_state.pVPCoredumpCallbackParam);
            } /* if ((schdr_sys_state.vpcoredump_callback != NULL) && ((pErrorMsg->arg2 == CVCORE_VP0) || (pErrorMsg->arg2 == CVCORE_VP1))) */
            else {
                console_printU5(pString, pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2, pErrorMsg->arg3, pErrorMsg->arg4);
            }
        } /* if (pErrorMsg->retcode == ERR_VISORC_COREDUMP_REQUEST) */
        else if (schdr_sys_state.error_callback != NULL) {
            superdag_error_struct_t errorinfo;
            errorinfo.flexidag_slot_id    = FLEXIDAG_INVALID_SLOT_ID;
            errorinfo.sysflow_index       = pErrorMsg->sysflow_index;
            errorinfo.cvtask_frameset_id  = pErrorMsg->cvtask_frameset_id;
            errorinfo.cvtask_uuid         = pErrorMsg->cvtask_uuid;
            errorinfo.retcode             = pErrorMsg->retcode;
            errorinfo.pFormatString       = pString;
            errorinfo.msg_time            = pErrorMsg->msg_time;
            errorinfo.arg0                = pErrorMsg->arg0;
            errorinfo.arg1                = pErrorMsg->arg1;
            errorinfo.arg2                = pErrorMsg->arg2;
            errorinfo.arg3                = pErrorMsg->arg3;
            errorinfo.arg4                = pErrorMsg->arg4;
            schdr_sys_state.error_callback(schdr_sys_state.pVPCoredumpCallbackHandle, &errorinfo, schdr_sys_state.pVPCoredumpCallbackParam);
        } /* if (schdr_sys_state.error_callback != NULL) */
        else { /* if (schdr_sys_state.error_callback == NULL) */
            default_error_printer(pString, pErrorMsg);
        } /* if (schdr_sys_state.error_callback == NULL) */
    }

    for(i = 0U ; i < FLEXIDAG_MAX_SLOTS; i++) {
        if ((flexidag_sys_state[i].pHandle != NULL) && (flexidag_sys_state[i].error_callback != NULL)) {
            flexidag_error_struct_t errorinfo;

            errorinfo.flexidag_slot_id    = i;
            errorinfo.sysflow_index       = pErrorMsg->sysflow_index;
            errorinfo.cvtask_frameset_id  = pErrorMsg->cvtask_frameset_id;
            errorinfo.cvtask_uuid         = pErrorMsg->cvtask_uuid;
            errorinfo.retcode             = pErrorMsg->retcode;
            errorinfo.pFormatString       = pString;
            errorinfo.msg_time            = pErrorMsg->msg_time;
            errorinfo.arg0                = pErrorMsg->arg0;
            errorinfo.arg1                = pErrorMsg->arg1;
            errorinfo.arg2                = pErrorMsg->arg2;
            errorinfo.arg3                = pErrorMsg->arg3;
            errorinfo.arg4                = pErrorMsg->arg4;
            flexidag_sys_state[i].error_callback(flexidag_sys_state[i].pHandle, &errorinfo, flexidag_sys_state[i].pErrorCallbackParam);
        }
    }
    return retcode;
}

static uint32_t schdr_handle_flexidag_open_parse(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;

    if (is_associated_flexidag(msg) != 0U) {
        const schedmsg_flexidag_open_parsed_t *pOpenParsed;
        uint32_t  flexidag_slot_id;

        flexidag_slot_id = ((uint32_t)msg->hdr.message_id - (uint32_t)0x0100U);
        pOpenParsed       = &msg->msg.flexidag_open_parsed;
        retcode = flexidag_init_sysflow(flexidag_slot_id, pOpenParsed->sysflow_daddr, pOpenParsed->sysflow_num_entries);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_flexidag_open_parse() : cvtask_init_flexidag_sysflow_name fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = flexidag_init_cvtable(flexidag_slot_id, pOpenParsed->cvtable_daddr, pOpenParsed->cvtable_num_entries, 0U);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_flexidag_open_parse() : cvtable_init_flexidag_info fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = flexidag_handle_open_parsed(flexidag_slot_id, pOpenParsed);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_flexidag_open_parse() : flexidag_handle_open_parsed fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }
    return retcode;
}

static uint32_t schdr_handle_flexidag_init_partition(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;

    if (is_associated_flexidag(msg) != 0U) {
        const schedmsg_flexidag_init_partitioned_t *pInitPart;
        uint32_t  flexidag_slot_id;
        uint32_t  core_id;

        flexidag_slot_id = ((uint32_t)msg->hdr.message_id - (uint32_t)0x0100U);
        core_id           = 0U; /* TODO: link to core_id */
        pInitPart = &msg->msg.flexidag_init_partitioned;
        retcode = flexidag_init_system_msg(flexidag_slot_id, pInitPart->arm_cvmsg_ctrl_daddr[core_id]);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_flexidag_init_partition() : flexidag_init_system_msg fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            retcode = flexidag_log_system_init(msg);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_flexidag_init_partition() : flexidag_log_system_init fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }

        if(retcode == ERRCODE_NONE) {
            retcode = flexidag_log_init(msg);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_handle_flexidag_init_partition() : flexidag_log_init fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            }
        }
    }

    return retcode;
}

static uint32_t schdr_handle_flexidag_output_done(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;

    if (is_associated_flexidag(msg) != 0U) {
        uint32_t  flexidag_slot_id;
        uint32_t  output_num;
        void *vpHandle;
        flexidag_memblk_t *pblk;

        flexidag_slot_id = ((uint32_t)msg->hdr.message_id - (uint32_t)0x0100U);
        output_num        = msg->msg.flexidag_output_reply.flexidag_output_num;
        vpHandle          = flexidag_sys_state[flexidag_slot_id].pHandle;

        retcode = drv_flexidag_process_kernel_reply(msg);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_flexidag_output_done() : drv_flexidag_process_kernel_reply fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }

        if(retcode == ERRCODE_NONE) {
            if (output_num < FLEXIDAG_MAX_OUTPUTS) {
                if (flexidag_sys_state[flexidag_slot_id].callback_array[output_num] != NULL) {
                    typecast(&pblk,&msg->msg.flexidag_output_reply.vpOutputBlk);
                    flexidag_sys_state[flexidag_slot_id].callback_array[output_num](vpHandle, output_num, pblk,
                            flexidag_sys_state[flexidag_slot_id].pCallbackParam[output_num]);
                } /* if (flexidag_sys_state[flexidag_slot_id].callback_array[output_num] != NULL) */
            } /* if (output_num < FLEXIDAG_MAX_OUTPUTS) */
        }
    } /* if (is_associated_flexidag(msg)) */
    return retcode;
}

static uint32_t schdr_handle_flexidag_get_version(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_getbuildinfo_t *req;
    uint32_t priority = 0U;

    req = &msg->msg.cvtask_getbuildinfo;
    if (req->arm_cvtask_index == ARM_CVTASK_INDEX_SCHEDULER) {
        retcode = schdr_handle_get_buildinfo(msg);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_flexidag_get_version() : schdr_handle_get_buildinfo fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_BUILDINFO_FAIL;
        }
    } else {
        retcode = thpool_add_task(priority, cvtask_execute_request, msg);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_handle_flexidag_get_version() : thpool_add_task fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_BUILDINFO_FAIL;
        }
    }

    return retcode;
}

static uint32_t schdr_handle_flexidag_error_message(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    const void *ptr;

    (void)msg;
    if (is_associated_flexidag(msg) != 0U) {
        const schedmsg_flexidag_error_request_t *pErrorMsg = &msg->msg.flexidag_error_message;
        char *pString = NULL;
        uint32_t  flexidag_slot_id;
        void *vpHandle;

        flexidag_slot_id = ((uint32_t)msg->hdr.message_id - (uint32_t)0x0100U);
        vpHandle          = flexidag_sys_state[flexidag_slot_id].pHandle;

        if(pErrorMsg->format_daddr != 0U) {
            ptr = ambacv_c2v(pErrorMsg->format_daddr);
            if(ptr != NULL) {
                typecast(&pString,&ptr);
            }
        }

        if(pString != NULL) {
            if (pErrorMsg->retcode == (uint32_t)ERR_VISORC_COREDUMP_REQUEST) {
                if ((flexidag_sys_state[flexidag_slot_id].vpcoredump_callback != NULL) && ((pErrorMsg->arg2 == (uint32_t)CVCORE_VP0) || (pErrorMsg->arg2 == (uint32_t)CVCORE_VP1))) {
                    flexidag_error_struct_t errorinfo;

                    errorinfo.flexidag_slot_id    = flexidag_slot_id;
                    errorinfo.sysflow_index       = pErrorMsg->sysflow_index;
                    errorinfo.cvtask_frameset_id  = pErrorMsg->cvtask_frameset_id;
                    errorinfo.cvtask_uuid         = pErrorMsg->cvtask_uuid;
                    errorinfo.retcode             = pErrorMsg->retcode;
                    errorinfo.pFormatString       = pString;
                    errorinfo.msg_time            = pErrorMsg->msg_time;
                    errorinfo.arg0                = pErrorMsg->arg0;
                    errorinfo.arg1                = pErrorMsg->arg1;
                    errorinfo.arg2                = pErrorMsg->arg2;
                    errorinfo.arg3                = pErrorMsg->arg3;
                    errorinfo.arg4                = pErrorMsg->arg4;
                    flexidag_sys_state[flexidag_slot_id].vpcoredump_callback(vpHandle, &errorinfo, flexidag_sys_state[flexidag_slot_id].pVPCoredumpCallbackParam);
                } /* if ((flexidag_sys_state[flexidag_slot_id].vpcoredump_callback != NULL) && ((pErrorMsg->arg2 == CVCORE_VP0) || (pErrorMsg->arg2 == CVCORE_VP1))) */
                else {
                    console_printU5(pString, pErrorMsg->arg0, pErrorMsg->arg1, pErrorMsg->arg2, pErrorMsg->arg3, pErrorMsg->arg4);
                }
            } /* if (pErrorMsg->retcode == ERR_VISORC_COREDUMP_REQUEST) */
            else if (flexidag_sys_state[flexidag_slot_id].error_callback != NULL) {
                flexidag_error_struct_t errorinfo;

                errorinfo.flexidag_slot_id    = flexidag_slot_id;
                errorinfo.sysflow_index       = pErrorMsg->sysflow_index;
                errorinfo.cvtask_frameset_id  = pErrorMsg->cvtask_frameset_id;
                errorinfo.cvtask_uuid         = pErrorMsg->cvtask_uuid;
                errorinfo.retcode             = pErrorMsg->retcode;
                errorinfo.pFormatString       = pString;
                errorinfo.msg_time            = pErrorMsg->msg_time;
                errorinfo.arg0                = pErrorMsg->arg0;
                errorinfo.arg1                = pErrorMsg->arg1;
                errorinfo.arg2                = pErrorMsg->arg2;
                errorinfo.arg3                = pErrorMsg->arg3;
                errorinfo.arg4                = pErrorMsg->arg4;
                flexidag_sys_state[flexidag_slot_id].error_callback(vpHandle, &errorinfo, flexidag_sys_state[flexidag_slot_id].pErrorCallbackParam);
            } else {
                const schedmsg_superdag_error_request_t *pError_sdg;

                typecast(&pError_sdg,&pErrorMsg);
                default_error_printer(pString, pError_sdg);

            }
        }
    }

    return retcode;
}

static uint32_t schdr_process_request_schedmsg_boot(armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    *hit = 1U;
    switch (type) {
    case SCHEDMSG_SET_BOOTUP_PHASE:
        retcode = schdr_handle_set_phase(msg);
        break;
    case SCHEDMSG_INITIAL_SETUP:
        retcode = schdr_handle_init_setup(msg);
        break;
    case SCHEDMSG_BOOT_SETUP1:
        retcode = schdr_handle_boot_setup1(msg);
        break;
    case SCHEDMSG_BOOT_SETUP2:
        retcode = schdr_handle_boot_setup2(msg);
        break;
    case SCHEDMSG_BOOT_SETUP3:
        retcode = schdr_handle_boot_setup3(msg);
        break;
    case SCHEDMSG_CVSCHEDULER_REPORT_MEMORY:
        retcode = schdr_handle_report_memory(msg);
        break;
    case SCHEDMSG_VISORC_PRINTF_UPDATE:
        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"Receive log_update message \n", 0U, 0U, 0U, 0U, 0U);
        break;
    case SCHEDMSG_VISORC_STARTUP_UPDATE:
        retcode = schdr_handle_startup_log(msg);
        break;
    case SCHEDMSG_SCHEDULER_SHUTDOWN:
        retcode = schdr_handle_shutdown(msg);
        break;
    case SCHEDMSG_HWUNIT_RESET:
        drv_handle_softreset(msg);
        break;
    case SCHEDMSG_CVSCHEDULER_ACTIVE:
        retcode = schdr_handle_activate(msg);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t schdr_process_request_schedmsg(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    *hit = 1U;
    switch (type) {
    case SCHEDMSG_CONSOLE_ECHO_MESSAGE:
        console_printU5(&msg->msg.console_message.fmtstring[0],
                        msg->msg.console_message.arg0,
                        msg->msg.console_message.arg1,
                        msg->msg.console_message.arg2,
                        msg->msg.console_message.arg3,
                        msg->msg.console_message.arg4);
        break;

    case SCHEDMSG_SUPERDAG_ERROR_MESSAGE:
        retcode = schdr_handle_error_message(msg);
        break;

    case SCHEDMSG_CONFIGURE_IDSP:
    case SCHEDMSG_CONFIGURE_VIN:
    case SCHEDMSG_INVALIDATE_CACHE:
    case SCHEDMSG_SET_REWAKE_TIME:
    case SCHEDMSG_FLUSH_VISPRINTF_REPLY:
        break;
    case SCHEDMSG_AUTORUN_STATUS_UPDATE:
        break;  // TODO: Implement this
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t schdr_process_request_cvtask(armvis_msg_t *msg, uint32_t type)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t priority = 0U;

    switch (type) {
    case SCHEDMSG_CVTASK_RUN_REQUEST: {
        const schedmsg_cvtask_run_t *req;

        req = &msg->msg.cvtask_run;
        priority = req->task_deadline;
        if (is_blockable_cvtask(req->arm_cvtask_index) != 0U) {
            retcode = thpool_add_extra_task(priority, cvtask_execute_request, msg);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_process_request_cvtask() : type (0x%x) thpool_add_extra_task fail retcode (0x%x)", type, retcode, 0U, 0U, 0U);
            }
        } /* if (is_blockable_cvtask(req->arm_cvtask_index) != 0) */
        else { /* if (is_blockable_cvtask(req->arm_cvtask_index) == 0) */
            retcode = thpool_add_task(priority, cvtask_execute_request, msg);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_process_request_cvtask() : type (0x%x) thpool_add_task fail retcode (0x%x)", type, retcode, 0U, 0U, 0U);
            }
        } /* if (is_blockable_cvtask(req->arm_cvtask_index) == 0) */
    }
    break;
    case SCHEDMSG_CVTASK_GET_BUILDINFO: {
        const schedmsg_cvtask_getbuildinfo_t *req;

        req = &msg->msg.cvtask_getbuildinfo;
        if (req->arm_cvtask_index == ARM_CVTASK_INDEX_SCHEDULER) {
            retcode = schdr_handle_get_buildinfo(msg);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_process_request_cvtask() : type (0x%x) schdr_handle_get_buildinfo fail retcode (0x%x)", type, retcode, 0U, 0U, 0U);
            }
        } else {
            retcode = thpool_add_task(priority, cvtask_execute_request, msg);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_process_request_cvtask() : type (0x%x) thpool_add_task fail retcode (0x%x)", type, retcode, 0U, 0U, 0U);
            }
        }
    }
    break;
    default:
        retcode = thpool_add_task(priority, cvtask_execute_request, msg);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_process_request_cvtask() : type (0x%x) thpool_add_task fail retcode (0x%x)", type, retcode, 0U, 0U, 0U);
        }
        break;
    }
    return retcode;
}

static uint32_t schdr_process_request_cavalry(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    (void) type;
    (void) hit;
#ifndef DISABLE_CAVALRY
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_CAVALRY_VP_RUN_REPLY:
    case SCHEDMSG_CAVALRY_HL_RUN_REPLY:
    case SCHEDMSG_CAVALRY_FEX_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FEX_RUN_REPLY:
    case SCHEDMSG_CAVALRY_FMA_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FMA_RUN_REPLY:
        retcode = (uint32_t)drv_cavalry_process_reply(msg);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_process_request_cavalry() : type (0x%x) drv_cavalry_process_reply fail retcode (0x%x)", type, retcode, 0U, 0U, 0U);
        }
        break;
    default:
        *hit = 0U;
        break;
    }
#endif
    return retcode;
}

static uint32_t schdr_process_request_flexidag(armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    *hit = 1U;
    switch (type) {
    case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
        retcode = drv_flexidag_process_kernel_reply(msg); /* This has no associated flexidag */
        break;

    case SCHEDMSG_FLEXIDAG_SETUP_REQUEST:
        if (is_associated_flexidag(msg) != 0U) {
            retcode = flexidag_handle_reg(msg);
        }
        break;
    case SCHEDMSG_FLEXIDAG_OPEN_REPLY:
    case SCHEDMSG_FLEXIDAG_INIT_REPLY:
        if (is_associated_flexidag(msg) != 0U) {
            uint32_t  flexidag_slot_id;

            flexidag_slot_id = ((uint32_t)msg->hdr.message_id - (uint32_t)0x0100U);
            retcode = drv_flexidag_process_kernel_reply(msg);
            if(retcode == ERRCODE_NONE) {
                retcode = thread_sem_post(&flexidag_sys_sem[flexidag_slot_id]);
            }
        }
        break;
    case SCHEDMSG_FLEXIDAG_RUN_REPLY:
    case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
        if (is_associated_flexidag(msg) != 0U) {
            uint32_t  flexidag_slot_id;

            flexidag_slot_id = ((uint32_t)msg->hdr.message_id - (uint32_t)0x0100U);
            fd_prof_unit_meta(EVENT_FLEXIDAG_METADATA_START, EVENT_ST_FLEXIDAG_RUN, flexidag_slot_id,flexidag_sys_state[flexidag_slot_id].finish_frameset_id);
            flexidag_sys_state[flexidag_slot_id].finish_frameset_id++;
            retcode = drv_flexidag_process_kernel_reply(msg);
        }
        break;
    case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
        retcode = schdr_handle_flexidag_open_parse(msg);
        break;
    case SCHEDMSG_FLEXIDAG_INIT_PARTITIONED:
        retcode = schdr_handle_flexidag_init_partition(msg);
        break;
    case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
        retcode = schdr_handle_flexidag_output_done(msg);
        break;

    case SCHEDMSG_FLEXIDAG_ERROR_MESSAGE:
        retcode = schdr_handle_flexidag_error_message(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST:
        retcode = schdr_handle_flexidag_get_version(msg);
        break;
    default:
        *hit = 0U;
        break;
    }
    return retcode;
}

static uint32_t schdr_check_schdrmsg(const armvis_msg_t *msg, uint32_t seqno)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) msg;
    if(msg->hdr.message_seqno != seqno) {
        if ((msg->hdr.message_id >= (uint16_t)0x100U) && (msg->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
            retcode = ERR_DRV_FLEXIDAG_VISMSG_SEQNO_MISMATCHED;
        } else {
            retcode = ERR_DRV_SCHDR_VISMSG_SEQNO_MISMATCHED;
        }
#ifdef ASIL_COMPLIANCE
    } else {
        uint32_t hash = FNV1A_32_INIT_HASH;

        if(msg->hdr.message_checksum != 0U) {
            hash = schdrmsg_calc_hash(&msg->hdr, 12, hash);
            hash = schdrmsg_calc_hash(&msg->msg, sizeof(msg->msg), hash);
            if(hash != msg->hdr.message_checksum) {
                if ((msg->hdr.message_id >= (uint16_t)0x100U) && (msg->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
                    retcode = ERR_DRV_FLEXIDAG_VISMSG_CHECKSUM_MISMATCHED;
                } else {
                    retcode = ERR_DRV_SCHDR_VISMSG_CHECKSUM_MISMATCHED;
                }
            }
        }
#endif /* ?ASIL_COMPLIANCE */
    }

    return retcode;
}

/**
 * Process an armvis_msg_t received by message receiver.
 */
uint32_t schdr_process_request(const armvis_msg_t *msg, uint32_t seqno)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t type, hit;

    (void) msg;
    if(msg == NULL) {
        console_printU5("[ERROR] schdr_process_request() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_VISMSG_INVALID;
    } else if (msg->hdr.message_retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdr_process_request() : type (0x%x) invalid fail message_retcode (0x%x)", (uint32_t)msg->hdr.message_type, msg->hdr.message_retcode, 0U, 0U, 0U);
        retcode = msg->hdr.message_retcode; /* TO DO */
    } else {
        armvis_msg_t shadow = {0};

        retcode = thread_memcpy(&shadow, msg, sizeof(shadow));
        if(retcode == ERRCODE_NONE) {
            retcode = schdr_check_schdrmsg(&shadow, seqno);
            if(retcode == ERRCODE_NONE) {
                type = (uint32_t)shadow.hdr.message_type;
                retcode = schdr_process_request_schedmsg_boot(&shadow, type, &hit);
                if(hit == 0U) {
                    retcode = schdr_process_request_schedmsg(&shadow, type, &hit);
                    if(hit == 0U) {
                        retcode = schdr_process_request_flexidag(&shadow, type, &hit);
                        if(hit == 0U) {
                            retcode = schdr_process_request_cavalry(&shadow, type, &hit);
                            if(hit == 0U) {
                                retcode = schdr_process_request_cvtask(&shadow, type);
                            }
                        }
                    }
                }
            }
        }
    }

    return retcode;
}
