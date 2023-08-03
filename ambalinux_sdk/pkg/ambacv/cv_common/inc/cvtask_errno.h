/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
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

#ifndef CVTASK_ERRNO_H_FILE
#define CVTASK_ERRNO_H_FILE

#include "cvsched_errno.h"
#include "cvsched_drv_errno.h"
/*-= ARM CVTask errors - these can be replaced with VISORC errors */

#define CVTASK_ERR_OK              0x0U   // request fulfilled OK
#define CVTASK_ERR_GENERAL         0xaa000001U   // a catch-all error code
#define CVTASK_ERR_UNIMPLEMENTED   0xaa000002U   // request function is not implemented
#define CVTASK_ERR_MSG_TYPE        0xaa000003U   // invalid message type
#define CVTASK_ERR_CVTASK_ID       0xaa000004U   // invalid cvtask id
#define CVTASK_ERR_INSTANCE_ID     0xaa000005U   // invalid instance id
#define CVTASK_ERR_NOT_FOUND       0xaa000006U   // target is not found
#define CVTASK_ERR_MSG_FULL        0xaa000007U   // the target message is full
#define CVTASK_ERR_INDEX_TOO_LARGE 0xaa000008U
#define CVTASK_ERR_NULL_POINTER    0xaa000009U

/* errcode_enum_t must be kept as a uint32_t */

#define errcode_enum_t                uint32_t

/*------------------------------------------------------------------------------------------------*/
/* Non error cases                                                                                */
/*------------------------------------------------------------------------------------------------*/
#define ERRCODE_NONE                                0x00000000U /* No error                                                                         */

#define RETCODE_CVTASK_YIELDED                      0x20000000U /* Specialized return code for CVTASK_YIELD                                         */

/*------------------------------------------------------------------------------------------------*/
/* Generic errors;                                                                                */
/*------------------------------------------------------------------------------------------------*/
#define ERRCODE_GENERIC                             0x80000000U /* Generic error code for an error, unspecified                                     */
#define ERRCODE_BAD_PARAMETER                       0x80000001U /* Generic bad parameter for a call, unspecified                                    */
#define ERRCODE_BAD_ADDRESS                         0x80000002U /* Generic bad address for a call, unspecified                                    */
#define ERRCODE_NO_DEV                              0x80000003U /* Generic no such device for a call, unspecified                                    */
#define ERRCODE_TIMEDOUT                            0x80000004U /* Generic timeout for a call, unspecified                           */

static inline uint32_t is_err(errcode_enum_t x)
{
    uint32_t ret_val;
    if ((((uint32_t)x) & (uint32_t)0x80000000U) != (uint32_t)0) {
        ret_val = 1;
    } /* if ((((uint32_t)x) & (uint32_t)0x80000000U) != (uint32_t)0) */
    else { /* if ((((uint32_t)x) & (uint32_t)0x80000000U) == (uint32_t)0) */
        ret_val = 0;
    } /* if ((((uint32_t)x) & (uint32_t)0x80000000U) == (uint32_t)0) */
    return ret_val;
} /* is_err() */

static inline uint32_t is_not_err(errcode_enum_t x)
{
    uint32_t ret_val;
    if ((((uint32_t)x) & (uint32_t)0x80000000U) == (uint32_t)0) {
        ret_val = 1;
    } /* if ((((uint32_t)x) & (uint32_t)0x80000000U) == (uint32_t)0) */
    else { /* if ((((uint32_t)x) & (uint32_t)0x80000000U) != (uint32_t)0) */
        ret_val = 0;
    } /* if ((((uint32_t)x) & (uint32_t)0x80000000U) != (uint32_t)0) */
    return ret_val;
} /* is_not_err() */

static inline uint32_t is_warn(errcode_enum_t x)
{
    uint32_t ret_val;
    if ((((uint32_t)x) & (uint32_t)0x40000000U) != (uint32_t)0) {
        ret_val = 1;
    } /* if ((((uint32_t)x) & (uint32_t)0x40000000U) != (uint32_t)0) */
    else { /* if ((((uint32_t)x) & (uint32_t)0x40000000U) == (uint32_t)0) */
        ret_val = 0;
    } /* if ((((uint32_t)x) & (uint32_t)0x40000000U) == (uint32_t)0) */
    return ret_val;
} /* is_warn() */

static inline uint32_t is_not_warn(errcode_enum_t x)
{
    uint32_t ret_val;
    if ((((uint32_t)x) & (uint32_t)0x40000000U) == (uint32_t)0) {
        ret_val = 1;
    } /* if ((((uint32_t)x) & (uint32_t)0x40000000U) == (uint32_t)0) */
    else { /* if ((((uint32_t)x) & (uint32_t)0x40000000U) != (uint32_t)0) */
        ret_val = 0;
    } /* if ((((uint32_t)x) & (uint32_t)0x40000000U) != (uint32_t)0) */
    return ret_val;
} /* is_not_warn() */

static inline uint32_t is_okay(errcode_enum_t x)
{
    uint32_t ret_val;
    if ((((uint32_t)x) & (uint32_t)0xC0000000U) == (uint32_t)0) {
        ret_val = 1;
    } /* if ((((uint32_t)x) & (uint32_t)0xC0000000U) == (uint32_t)0) */
    else { /* if ((((uint32_t)x) & (uint32_t)0xC0000000U) != (uint32_t)0) */
        ret_val = 0;
    } /* if ((((uint32_t)x) & (uint32_t)0xC0000000U) != (uint32_t)0) */
    return ret_val;
} /* is_okay() */

static inline uint32_t is_cvtask_err(errcode_enum_t x)
{
    uint32_t ret_val;
    if ((((uint32_t)x) & (uint32_t)0xF0000000U) == (uint32_t)0xC0000000U) {
        ret_val = 1;
    } /* if ((((uint32_t)x) & (uint32_t)0xF0000000U) == (uint32_t)0xC0000000U) */
    else { /* if ((((uint32_t)x) & (uint32_t)0xF0000000U) != (uint32_t)0xC0000000U) */
        ret_val = 0;
    } /* if ((((uint32_t)x) & (uint32_t)0xF0000000U) != (uint32_t)0xC0000000U) */
    return ret_val;
} /* is_cvtask_err() */

#endif /* !CVTASK_ERRNO_H_FILE */

