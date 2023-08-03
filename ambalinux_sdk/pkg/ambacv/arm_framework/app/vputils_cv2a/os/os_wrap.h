/**
 *  @file os_wrap.h
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
 *  @details Definitions & Constants for OS wrap APIs
 *
 */

#ifndef LINUX_OS_WRAP_H
#define LINUX_OS_WRAP_H

#if defined(CONFIG_BUILD_CV_THREADX)
#include "cvtask_ossrv.h"

#define OS_NAME             "ThreadX"

extern AMBA_SHELL_PRINT_f ThreadX_PrintFunc;
extern UINT32 __cv_schdr_start;

#elif defined(CONFIG_QNX)
#include "cvtask_ossrv.h"

#define OS_NAME             "QNX"

extern AMBA_SHELL_PRINT_f ThreadX_PrintFunc;

#elif defined(CONFIG_LINUX)
#include "cvtask_ossrv.h"

#define OS_NAME             "LINUX"

extern AMBA_SHELL_PRINT_f ThreadX_PrintFunc;

#else
#include <stdint.h>
#include "cvtask_ossrv.h"

#define OS_NAME             "LINUX"
#define LINUX_BUILD

#define FILE_WRAP   FILE
#define fopen_wrap  fopen
#define fclose_wrap fclose
#define fread_wrap  fread
#define fwrite_wrap fwrite
#endif


typedef struct {
    int32_t     amba_fd;
    int32_t     ambamal_fd;    
    uint32_t    *cvreg;
    uint32_t    *cvdram;
    uint32_t    cv_dram_base;
} reg_agent_t;


uint32_t   reg_agent_size(void);
void       open_reg_agent(reg_agent_t *r);
void       close_reg_agent(reg_agent_t *r);

uint32_t readl(const reg_agent_t *r, uint32_t addr);
void     writel(const reg_agent_t *r, uint32_t addr, uint32_t value);

void     read_buf(const reg_agent_t *r, uint32_t addr, uint32_t size, uint8_t* buf);

uint32_t cv_dram_v2c(const reg_agent_t *r, const void *buf);
uint8_t *cv_dram_c2v(const reg_agent_t *r, uint32_t base);
uint32_t cv_cache_clean(const reg_agent_t *r, const void *ptr, uint32_t size);
uint32_t cv_cache_invalidate(const reg_agent_t *r, const void *ptr, uint32_t size);

void type_cast(void *pNewType, const void *pOldType);
#endif //LINUX_OS_WRAP_H
