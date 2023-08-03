/**
 *  @file dsp_wrapper_osal.c
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
 *  @details DSP OS abstract layer APIs
 *
 */

#include "dsp_wrapper_osal.h"
#include "AmbaDSP_Def.h"

static inline UINT32 wrapper_get_u64msb(UINT64 U64Val)
{
    return (UINT32)((U64Val >> 32U) & 0xFFFFFFFFU);
}

static inline UINT32 wrapper_get_u64lsb(UINT64 U64Val)
{
    return (UINT32)(U64Val & 0xFFFFFFFFU);
}

/******************************************************************************/
/*                                  print                                 */
/******************************************************************************/
void dsp_wrapper_osal_print_flush(void)
{
    AmbaPrint_Flush();
}

void dsp_wrapper_osal_print_stop_and_flush(void)
{
    AmbaPrint_StopAndFlush();
}

void dsp_wrapper_osal_printU5(const char *fmt,
                      uint32_t argv0,
                      uint32_t argv1,
                      uint32_t argv2,
                      uint32_t argv3,
                      uint32_t argv4)
{
    AmbaPrint_PrintUInt5(fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_wrapper_osal_printS5(const char *fmt,
                      const char *argv0,
                      const char *argv1,
                      const char *argv2,
                      const char *argv3,
                      const char *argv4)
{
    AmbaPrint_PrintStr5(fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_wrapper_osal_printI5(const char *fmt,
                      int32_t argv0,
                      int32_t argv1,
                      int32_t argv2,
                      int32_t argv3,
                      int32_t argv4)
{
    AmbaPrint_PrintInt5(fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_wrapper_osal_module_printU5(uint32_t module_id, const char *fmt,
                             uint32_t argv0, uint32_t argv1,
                             uint32_t argv2, uint32_t argv3,
                             uint32_t argv4)
{
    AmbaPrint_ModulePrintUInt5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_wrapper_osal_module_printS5(uint32_t module_id, const char *fmt,
                             const char *argv0, const char *argv1,
                             const char *argv2, const char *argv3,
                             const char *argv4)
{
    AmbaPrint_ModulePrintStr5((uint16_t)module_id, fmt, argv0, argv1, argv2, argv3, argv4);
}

void dsp_wrapper_osal_print_err(uint32_t rval, const char *fn, uint32_t line_num)
{
    if (rval != 0U) {
        dsp_wrapper_osal_printS5("%s got NG result", fn, NULL, NULL, NULL, NULL);
        dsp_wrapper_osal_printU5("    Rval = 0x%x @ %d", rval, line_num, 0U, 0U, 0U);
    }
}

/******************************************************************************/
/*                                  OS relative                               */
/******************************************************************************/
uint32_t dsp_wrapper_osal_mutex_init(wrap_osal_mutex_t *mutex, char *pName)
{
    return AmbaKAL_MutexCreate(mutex, pName);
}

uint32_t dsp_wrapper_osal_mutex_deinit(wrap_osal_mutex_t *mutex)
{
    return AmbaKAL_MutexDelete(mutex);
}

uint32_t dsp_wrapper_osal_mutex_lock(wrap_osal_mutex_t *mutex, uint32_t timeout)
{
    return AmbaKAL_MutexTake(mutex, timeout);
}

uint32_t dsp_wrapper_osal_mutex_unlock(wrap_osal_mutex_t *mutex)
{
    return AmbaKAL_MutexGive(mutex);
}

uint32_t dsp_wrapper_osal_sleep(uint32_t msec)
{
    return AmbaKAL_TaskSleep(msec);
}

typedef struct {
    ULONG       Base;       /* Virtual Address */
    ULONG    PhysBase;   /* Physical Address, u32 only */
    uint32_t    size;
    uint32_t    is_cached;
} osal_wrap_buf_pool_t;

static osal_wrap_buf_pool_t dsp_wrapper_buf_pool[NUM_DSP_WRAP_POOL] = {0};
uint32_t dsp_wrapper_osal_add_buf_pool(UINT32 Type, ULONG VirtAddr, ULONG PhysAddr, UINT32 Size, UINT32 IsCached)
{
    uint32_t Rval = DSP_ERR_NONE;

    if (Type >= NUM_DSP_WRAP_POOL) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_add_buf_pool : Invalid type", 0U, 0U, 0U, 0U, 0U);
    } else if ((VirtAddr == 0U) ||
               (PhysAddr == 0U) ||
               (Size == 0U)) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_add_buf_pool : Invalid setting [0x%X%X 0x%X %d]"
            , wrapper_get_u64msb((UINT64)VirtAddr), wrapper_get_u64lsb((UINT64)VirtAddr), PhysAddr, Size, 0U);
    } else {
        dsp_wrapper_buf_pool[Type].Base = VirtAddr;
        dsp_wrapper_buf_pool[Type].PhysBase = PhysAddr;
        dsp_wrapper_buf_pool[Type].size = Size;
        dsp_wrapper_buf_pool[Type].is_cached = IsCached;
        //dsp_wrapper_osal_printU5("dsp_wrapper_osal_add_buf_pool : pool[%d] update [0x%X 0x%X %d %d]", Type, VirtAddr, PhysAddr, Size, IsCached);
    }

    return Rval;
}

uint32_t dsp_wrapper_osal_get_buf_pool_Info(UINT32 Type, ULONG *pVirtAddr, ULONG *pPhysAddr, UINT32 *pSize, UINT32 *pIsCached)
{
    uint32_t Rval = DSP_ERR_NONE;

    if (Type >= NUM_DSP_WRAP_POOL) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_get_buf_pool_Info : Invalid type", 0U, 0U, 0U, 0U, 0U);
    } else if ((pVirtAddr == NULL) ||
               (pPhysAddr == NULL) ||
               (pSize == NULL) ||
               (pIsCached == NULL)) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_get_buf_pool_Info : Null Input", 0U, 0U, 0U, 0U, 0U);
    } else {
        *pVirtAddr = dsp_wrapper_buf_pool[Type].Base;
        *pPhysAddr = dsp_wrapper_buf_pool[Type].PhysBase;
        *pSize = dsp_wrapper_buf_pool[Type].size;
        *pIsCached = dsp_wrapper_buf_pool[Type].is_cached;
    }

    return Rval;
}

uint32_t dsp_wrapper_osal_virt2phys(const ULONG Virt, ULONG *pPhys)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i;
    uint32_t exit_i_loop;
    ULONG ul_pool_end_addr, len;

    if (pPhys == NULL) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_virt2phys() : Null Addr", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else if (Virt == 0UL) {
        *pPhys = 0U;
    } else {
        /* Check map table */
        exit_i_loop = 0U;
        for (i = 0U; i < NUM_DSP_WRAP_POOL; i++) {
            if (dsp_wrapper_buf_pool[i].size > 0U) {
                ul_pool_end_addr = (dsp_wrapper_buf_pool[i].Base + dsp_wrapper_buf_pool[i].size) - 1U;

                if ((Virt >= dsp_wrapper_buf_pool[i].Base) &&
                    (Virt <= ul_pool_end_addr)) {
                    len = (Virt - dsp_wrapper_buf_pool[i].Base);
                    *pPhys = dsp_wrapper_buf_pool[i].PhysBase + len;
                    exit_i_loop = 1U;
                    break;
                }
            }
        }

        if (exit_i_loop == 0U) {
            dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_virt2phys : fail for 0x%X%X"
                  , wrapper_get_u64msb((UINT64)Virt), wrapper_get_u64lsb((UINT64)Virt), 0U, 0U, 0U);
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
}

uint32_t dsp_wrapper_osal_phys2virt(const ULONG Phys, ULONG *pVirt)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i;
    uint32_t exit_i_loop;
    ULONG offset;
    ULONG pool_end_addr;

    if (pVirt == NULL) {
        dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_phys2virt() : Null Addr", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else if (Phys == 0UL) {
        *pVirt = 0U;
    } else {
        /* Check map table */
        exit_i_loop = 0U;
        for (i = 0U; i < NUM_DSP_WRAP_POOL; i++) {
            if (dsp_wrapper_buf_pool[i].size > 0U) {
                pool_end_addr = (dsp_wrapper_buf_pool[i].PhysBase + dsp_wrapper_buf_pool[i].size) - 1U;

                if ((Phys >= dsp_wrapper_buf_pool[i].PhysBase) &&
                    (Phys <= pool_end_addr)) {
                    offset = (Phys - dsp_wrapper_buf_pool[i].PhysBase);
                    *pVirt = dsp_wrapper_buf_pool[i].Base + offset;
                    exit_i_loop = 1U;
                    break;
                }
            }
        }

        if (exit_i_loop == 0U) {
            dsp_wrapper_osal_printU5("[ERR] dsp_wrapper_osal_phys2virt : fail for 0x%X \n", Phys, 0U, 0U, 0U, 0U);
            *pVirt = Phys;
            Rval = DSP_ERR_0003;
        }
    }

    return Rval;
}

/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/
uint32_t dsp_wrapper_osal_str_to_u32(const char *pString, uint32_t *pVal)
{
    return AmbaUtility_StringToUInt32(pString, pVal);
}

uint32_t dsp_wrapper_osal_u32_to_str(char *pString, uint32_t str_len, uint32_t val, uint32_t radix)
{
    return AmbaUtility_UInt32ToStr(pString, str_len, val, radix);
}

uint32_t dsp_wrapper_osal_str_print_u32(char *pBuffer, uint32_t BufferSize, const char *pFmtString, uint32_t Count, const uint32_t *pArgs)
{
    return AmbaUtility_StringPrintUInt32(pBuffer, BufferSize, pFmtString, Count, pArgs);
}

uint32_t dsp_wrapper_osal_str_append(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
    uint32_t retcode = DSP_ERR_NONE;
    AmbaUtility_StringAppend(pBuffer, BufferSize, pSource);
    return retcode;
}

uint32_t dsp_wrapper_osal_str_append_u32(char *pBuffer, uint32_t BufferSize, uint32_t Value, uint32_t Radix)
{
    uint32_t retcode = DSP_ERR_NONE;
    AmbaUtility_StringAppendUInt32(pBuffer, BufferSize, Value, Radix);
    return retcode;
}

uint32_t dsp_wrapper_osal_str_copy(char *pBuffer, uint32_t BufferSize, const char *pSource)
{
    uint32_t retcode = DSP_ERR_NONE;
    AmbaUtility_StringCopy(pBuffer, BufferSize, pSource);
    return retcode;
}

