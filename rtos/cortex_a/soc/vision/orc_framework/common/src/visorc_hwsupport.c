/*
 * Copyright (c) 2017-2020 Ambarella, Inc.
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

#ifdef MISRA_COMPLIANCE

#include <orc_memory.h>
#include <ucode_debug.h>
#include <cvapi_cavalry.h>
#include <vis_coproc.h>

/*================================================================================================*/
/*=- DMA macro conversions -======================================================================*/
/*================================================================================================*/
/**
 * dma_cmem_2_dram
 *
 * @author ckayano ( 3/ 6/2020) : Imported from orc_memory.h
 *
 * @param daddr
 * @param caddr
 * @param byte_size
 */
void dma_cmem_2_dram(uint32_t daddr, uint32_t caddr, uint32_t byte_size)
{
  if ((caddr & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_cmem_2_dram(0x%08x, >0x%08x<, %d) : Invalid caddr (must be multiple of 4)\n", daddr, caddr, byte_size, 0, 0);
  } /* if ((caddr & 0x03U) != 0x00U) */
  else if ((byte_size & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_cmem_2_dram(0x%08x, 0x%08x, >%d<) : Invalid byte_size (must be multiple of 4)\n", daddr, caddr, byte_size, 0, 0);
  } /* if ((byte_size & 0x03U) != 0x00U) */
  else /* Input parameters valid */
  {
    uint32_t  words_left;
    uint32_t  curr_dram_addr;
    uint32_t  curr_cmem_addr;

    words_left      = byte_size >> 2;
    curr_dram_addr  = daddr;
    curr_cmem_addr  = caddr;
    while (words_left > 0U)
    {
      uint32_t  words_xfer;
      uint32_t  bytes_xfer;

      words_xfer = (words_left > 256U) ? 256U : words_left;
      bytes_xfer = words_xfer << 2;

      cmem_2_dram_nowait(curr_dram_addr, curr_cmem_addr, words_xfer);

      words_left -= words_xfer;
      curr_dram_addr += bytes_xfer;
      curr_cmem_addr += bytes_xfer;
    } /* while (words_left > 0U) */
    wait_vis_w_dram_dma();
  } /* Input parameters valid */
} /* dma_cmem_2_dram() */

/**
 * dma_dram_2_cmem()
 *
 * @author ckayano ( 3/ 6/2020) : Imported from orc_memory.h
 *
 * @param caddr
 * @param daddr
 * @param byte_size
 */
void dma_dram_2_cmem(uint32_t caddr, uint32_t daddr, uint32_t byte_size)
{
  if ((caddr & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_dram_2_cmem(>0x%08x<, 0x%08x, %d) : Invalid caddr (must be multiple of 4)\n", caddr, daddr, byte_size, 0, 0);
  } /* if ((caddr & 0x03U) != 0x00U) */
  else if ((byte_size & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_dram_2_cmem(0x%08x, 0x%08x, >%d<) : Invalid byte_size (must be multiple of 4)\n", caddr, daddr, byte_size, 0, 0);
  } /* if ((byte_size & 0x03U) != 0x00U) */
  else /* Input parameters valid */
  {
    uint32_t  words_left;
    uint32_t  curr_dram_addr;
    uint32_t  curr_cmem_addr;

    words_left      = byte_size >> 2;
    curr_dram_addr  = daddr;
    curr_cmem_addr  = caddr;
    while (words_left > 0U)
    {
      uint32_t  words_xfer;
      uint32_t  bytes_xfer;

      words_xfer = (words_left > 256U) ? 256U : words_left;
      bytes_xfer = words_xfer << 2;

      dram_2_cmem_nowait(curr_cmem_addr, curr_dram_addr, words_xfer);

      words_left -= words_xfer;
      curr_dram_addr += bytes_xfer;
      curr_cmem_addr += bytes_xfer;
    } /* while (words_left > 0U) */
    wait_vis_w_dram_dma();
  } /* Input parameters valid */
} /* dma_dram_2_cmem() */

/**
 * dma_cmem_2_coproc()
 *
 * @author ckayano ( 3/ 6/2020) : Imported from orc_memory.h
 *
 * @param coproc_addr
 * @param caddr
 * @param byte_size
 */
void dma_cmem_2_coproc(uint32_t coproc_addr, uint32_t caddr, uint32_t byte_size)
{
  if ((coproc_addr & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_cmem_2_coproc(>0x%08x<, 0x%08x, %d) : Invalid coproc_addr (must be multiple of 4)\n", coproc_addr, caddr, byte_size, 0, 0);
  } /* if ((coproc_addr & 0x03U) != 0x00U) */
  else if ((caddr & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_cmem_2_coproc(0x%08x, >0x%08x<, %d) : Invalid caddr (must be multiple of 4)\n", coproc_addr, caddr, byte_size, 0, 0);
  } /* if ((caddr & 0x03U) != 0x00U) */
  else if ((byte_size & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_cmem_2_coproc(0x%08x, 0x%08x, >%d<) : Invalid coproc_addr (must be multiple of 4)\n", coproc_addr, caddr, byte_size, 0, 0);
  } /* if ((byte_size & 0x03U) != 0x00U) */
  else /* Input parameters valid */
  {
    uint32_t  words_left;
    uint32_t  curr_cpro_addr;
    uint32_t  curr_cmem_addr;

    words_left      = byte_size >> 2;
    curr_cpro_addr  = coproc_addr;
    curr_cmem_addr  = caddr;
    while (words_left > 0U)
    {
      uint32_t  words_xfer;
      uint32_t  bytes_xfer;

#ifdef CHIP_CV6 /* BUG #21757 : Limit the transfer size for cmem->vmem to 31 words */
      words_xfer = (words_left > 31U) ? 31U : words_left;
#else /* !CHIP_CV6 */
      words_xfer = (words_left > 256U) ? 256U : words_left;
#endif /* ?CHIP_CV6 */
      bytes_xfer = words_xfer << 2;

      cmem_2_coproc_nowait(curr_cpro_addr, curr_cmem_addr, words_xfer);

      words_left -= words_xfer;
      curr_cpro_addr += bytes_xfer;
      curr_cmem_addr += bytes_xfer;
    } /* while (words_left > 0U) */
    wait_vis_w_dram_dma();
  } /* Input parameters valid */
} /* dma_cmem_2_coproc() */

/**
 * dma_coproc_2_cmem()
 *
 * @author ckayano ( 3/ 6/2020) : Imported from orc_memory.h
 *
 * @param caddr
 * @param coproc_addr
 * @param byte_size
 */
void dma_coproc_2_cmem(uint32_t caddr, uint32_t coproc_addr, uint32_t byte_size)
{
  if ((caddr & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_coproc_2_cmem(>0x%08x<, 0x%08x, %d) : Invalid caddr (must be multiple of 4)\n", caddr, coproc_addr, byte_size, 0, 0);
  } /* if ((caddr & 0x03U) != 0x00U) */
  else if ((coproc_addr & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_coproc_2_cmem(0x%08x, >0x%08x<, %d) : Invalid coproc_addr (must be multiple of 4)\n", caddr, coproc_addr, byte_size, 0, 0);
  } /* if ((coproc_addr & 0x03U) != 0x00U) */
  else if ((byte_size & 0x03U) != 0x00U)
  {
    SCHED_PRINTF("[WARN] : dma_coproc_2_cmem(0x%08x, 0x%08x, >%d<) : Invalid byte_size (must be multiple of 4)\n", caddr, coproc_addr, byte_size, 0, 0);
  } /* if ((byte_size & 0x03U) != 0x00U) */
  else /* Input parameters valid */
  {
    uint32_t  words_left;
    uint32_t  curr_cpro_addr;
    uint32_t  curr_cmem_addr;

    words_left      = byte_size >> 2;
    curr_cpro_addr  = coproc_addr;
    curr_cmem_addr  = caddr;
    while (words_left > 0U)
    {
      uint32_t  words_xfer;
      uint32_t  bytes_xfer;

      words_xfer = (words_left > 256U) ? 256U : words_left;
      bytes_xfer = words_xfer << 2;

      coproc_2_cmem_nowait(curr_cmem_addr, curr_cpro_addr, words_xfer);

      words_left -= words_xfer;
      curr_cpro_addr += bytes_xfer;
      curr_cmem_addr += bytes_xfer;
    } /* while (words_left > 0U) */
    wait_vis_w_dram_dma();
  } /* Input parameters valid */
} /* dma_coproc_2_cmem() */

/*================================================================================================*/
/*=- CAVALRY printf macro conversions -===========================================================*/
/*================================================================================================*/
void cavalry_printf_silent(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
  cavalry_printf_raw((uint8_t)CAVALRY_LOG_LEVEL_SILENT, (uint32_t)HOTLINK_SLOT_CAVALRY_FRAMEWORK, 0, fmtstring, arg1, arg2, arg3, arg4, arg5);
} /* cavalry_printf_silent() */

void cavalry_printf_minimal(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
  cavalry_printf_raw((uint8_t)CAVALRY_LOG_LEVEL_MINIMAL, (uint32_t)HOTLINK_SLOT_CAVALRY_FRAMEWORK, 0, fmtstring, arg1, arg2, arg3, arg4, arg5);
} /* cavalry_printf_minimal() */

void cavalry_printf_normal(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
  cavalry_printf_raw((uint8_t)CAVALRY_LOG_LEVEL_NORMAL, (uint32_t)HOTLINK_SLOT_CAVALRY_FRAMEWORK, 0, fmtstring, arg1, arg2, arg3, arg4, arg5);
} /* cavalry_printf_normal() */

void cavalry_printf_verbose(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
  cavalry_printf_raw((uint8_t)CAVALRY_LOG_LEVEL_VERBOSE, (uint32_t)HOTLINK_SLOT_CAVALRY_FRAMEWORK, 0, fmtstring, arg1, arg2, arg3, arg4, arg5);
} /* cavalry_printf_verbose() */

void cavalry_printf_debug(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
  cavalry_printf_raw((uint8_t)CAVALRY_LOG_LEVEL_DEBUG, (uint32_t)HOTLINK_SLOT_CAVALRY_FRAMEWORK, 0, fmtstring, arg1, arg2, arg3, arg4, arg5);
} /* cavalry_printf_debug() */

void cavalry_printf(const char *fmtstring, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5)
{
  cavalry_printf_raw((uint8_t)CAVALRY_LOG_LEVEL_NORMAL, (uint32_t)HOTLINK_SLOT_CAVALRY_FRAMEWORK, 0, fmtstring, arg1, arg2, arg3, arg4, arg5);
} /* cavalry_printf() */

/*================================================================================================*/
/*=- VIS_COPROC high level macro conversions -====================================================*/
/*================================================================================================*/

void coproc_poke_bits_word(uint32_t value, uint32_t address, uint32_t mask)
{
  if ((mask & 0xFFU) != 0U)
  {
    coproc_poke_bits((value      ) & 0xFFU, address     , (uint8_t)((mask      ) & 0xFFU));
  } /* if ((mask & 0xFFU) != 0U) */
  if (((mask >> 8) & 0xFFU) != 0U)
  {
    coproc_poke_bits((value >> 8 ) & 0xFFU, address + 1U, (uint8_t)((mask >> 8 ) & 0xFFU));
  } /* if (((mask >> 8) & 0xFFU) != 0U) */
  if (((mask >> 16) & 0xFFU) != 0U)
  {
    coproc_poke_bits((value >> 16) & 0xFFU, address + 2U, (uint8_t)((mask >> 16) & 0xFFU));
  } /* if (((mask >> 16) & 0xFFU) != 0U) */
  if (((mask >> 24) & 0xFFU) != 0U)
  {
    coproc_poke_bits((value >> 24) & 0xFFU, address + 3U, (uint8_t)((mask >> 24) & 0xFFU));
  } /* if (((mask >> 24) & 0xFFU) != 0U) */
} /* coproc_poke_bits_word() */


void coproc_poke_bits_word_ldq(uint32_t value, uint32_t address, uint32_t mask)
{
  if ((mask & 0xFFU) != 0U)
  {
    coproc_poke_bits_ldq((value      ) & 0xFFU, address     , (uint8_t)((mask      ) & 0xFFU));
  } /* if ((mask & 0xFFU) != 0U) */
  if (((mask >> 8) & 0xFFU) != 0U)
  {
    coproc_poke_bits_ldq((value >> 8 ) & 0xFFU, address + 1U, (uint8_t)((mask >> 8 ) & 0xFFU));
  } /* if (((mask >> 8) & 0xFFU) != 0U) */
  if (((mask >> 16) & 0xFFU) != 0U)
  {
    coproc_poke_bits_ldq((value >> 16) & 0xFFU, address + 2U, (uint8_t)((mask >> 16) & 0xFFU));
  } /* if (((mask >> 16) & 0xFFU) != 0U) */
  if (((mask >> 24) & 0xFFU) != 0U)
  {
    coproc_poke_bits_ldq((value >> 24) & 0xFFU, address + 3U, (uint8_t)((mask >> 24) & 0xFFU));
  } /* if (((mask >> 24) & 0xFFU) != 0U) */
} /* coproc_poke_bits_word() */


void coproc_poke_bits_word_stq(uint32_t value, uint32_t address, uint32_t mask)
{
  if ((mask & 0xFFU) != 0U)
  {
    coproc_poke_bits_stq((value      ) & 0xFFU, address     , (uint8_t)((mask      ) & 0xFFU));
  } /* if ((mask & 0xFFU) != 0U) */
  if (((mask >> 8) & 0xFFU) != 0U)
  {
    coproc_poke_bits_stq((value >> 8 ) & 0xFFU, address + 1U, (uint8_t)((mask >> 8 ) & 0xFFU));
  } /* if (((mask >> 8) & 0xFFU) != 0U) */
  if (((mask >> 16) & 0xFFU) != 0U)
  {
    coproc_poke_bits_stq((value >> 16) & 0xFFU, address + 2U, (uint8_t)((mask >> 16) & 0xFFU));
  } /* if (((mask >> 16) & 0xFFU) != 0U) */
  if (((mask >> 24) & 0xFFU) != 0U)
  {
    coproc_poke_bits_stq((value >> 24) & 0xFFU, address + 3U, (uint8_t)((mask >> 24) & 0xFFU));
  } /* if (((mask >> 24) & 0xFFU) != 0U) */
} /* coproc_poke_bits_word() */

void coproc_poke_field(uint32_t value, uint32_t address, uint32_t offset, uint32_t size)
{
  uint32_t  final_value;
  uint32_t  final_address;
  uint32_t  final_mask;

  final_value   = (value << (offset & 0x7U));
  final_address = address + (offset >> 3);
  if (size >= 32U)
  {
    final_mask = 0xFFFFFFFFU;
  } /* if (size >= 32U) */
  else /* if (size < 32U) */
  {
    final_mask = ((uint32_t)1U << size) - 1U;
    final_mask = final_mask << (offset & 7U);
  } /* if (size < 32U) */
  coproc_poke_bits_word(final_value, final_address, final_mask);
}

void coproc_poke_field_ldq(uint32_t value, uint32_t address, uint32_t offset, uint32_t size)
{
  uint32_t  final_value;
  uint32_t  final_address;
  uint32_t  final_mask;

  final_value   = (value << (offset & 0x7U));
  final_address = address + (offset >> 3);
  if (size >= 32U)
  {
    final_mask = 0xFFFFFFFFU;
  } /* if (size >= 32U) */
  else /* if (size < 32U) */
  {
    final_mask = ((uint32_t)1U << size) - 1U;
    final_mask = final_mask << (offset & 7U);
  } /* if (size < 32U) */
  coproc_poke_bits_word_ldq(final_value, final_address, final_mask);
}

void coproc_poke_field_stq(uint32_t value, uint32_t address, uint32_t offset, uint32_t size)
{
  uint32_t  final_value;
  uint32_t  final_address;
  uint32_t  final_mask;

  final_value   = (value << (offset & 0x7U));
  final_address = address + (offset >> 3);
  if (size >= 32U)
  {
    final_mask = 0xFFFFFFFFU;
  } /* if (size >= 32U) */
  else /* if (size < 32U) */
  {
    final_mask = ((uint32_t)1U << size) - 1U;
    final_mask = final_mask << (offset & 7U);
  } /* if (size < 32U) */
  coproc_poke_bits_word_stq(final_value, final_address, final_mask);
}


#endif /* MISRA_COMPLIANCE */

