/*
*  @file AmbaWU_WarpExecuteUtility.c
*
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
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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


#include "AmbaWU_WarpExecuteUtility.h"
#include "AmbaWU_SystemApi.h"

DOUBLE u_sin(DOUBLE x)
{
    DOUBLE value = 0.0;
    const DOUBLE *p_misra_d64 = &value;
    void *p_misra = NULL;

    if(wu_memcpy(&p_misra, &p_misra_d64, sizeof(void*)) == OK) {;} //Coverity 4.7
    if(AmbaWrap_sin(x, p_misra) == OK) {;} //Coverity 4.7

    return value;
}

DOUBLE u_cos(DOUBLE x)
{
    DOUBLE value = 0.0;
    const DOUBLE *p_misra_d64 = &value;
    void *p_misra = NULL;

    if(wu_memcpy(&p_misra, &p_misra_d64, sizeof(void*)) == OK) {;} //Coverity 4.7
    if(AmbaWrap_cos(x, p_misra) == OK) {;} //Coverity 4.7

    return value;
}

INT64 u_shift_int64_right(INT64 target, UINT64 offset)
{
    UINT64 misra_u64;
    misra_u64 = (((UINT64)target + (1ULL<<63ULL))>>offset) - (1ULL<<(63ULL-offset));
    return (INT64)misra_u64;
}

INT64 u_shift_int64_left(INT64 target, UINT64 offset)
{
    UINT64 misra_u64;
    misra_u64 = (UINT64)target << offset;
    return (INT64)misra_u64;
}

UINT64 u_shift_uint64_right(UINT64 target, UINT64 offset)
{
    return (target >> offset);
}

UINT64 u_shift_uint64_left(UINT64 target, UINT64 offset)
{
    return (target << offset);
}
