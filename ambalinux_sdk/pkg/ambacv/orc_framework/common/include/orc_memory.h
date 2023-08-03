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

#ifndef ORC_MEMORY2_H_
#define ORC_MEMORY2_H_

#include <dsp_types.h>
// for wait bits
#include <cv1-def.h>
// for wait macro
#include <orc_wait.h>
// for assert_msg
#include <ucode_base.h>

/*!
 @file orc_memory.h

 @brief Defines some constants and some macros relating to the memory
 layout of this orc.
*/

#ifdef ORCVIS

#define CMEM_SIZE       0x8000U
#define CMEM_SIZE_ASM   0x8000      /* Keep this the same as CMEM_SIZE, except the assembler does not understand the "U" suffix in CMEM_SIZE */

#ifdef CHIP_CV6
#define SHMEM_SIZE      0x20000U    // 128kb
#define SHMEM_SIZE_ASM  0x20000     // Keep this the same as SHMEM_SIZE, as assembler does not understand the "U" suffix
#endif /* ?CHIP_CV6 */

#ifndef __ASSEMBLER__

// defined in linker script unless you defined it yourself
extern volatile uint8_t cmem[CMEM_SIZE];

#ifdef CHIP_CV6
extern volatile uint32_t  shmem[SHMEM_SIZE / sizeof(uint32_t)];
#endif /* ?CHIP_CV6 */

#ifdef MISRA_COMPLIANCE
void visdma_cmem_2_dram(void *vpDramDst, void *vpCmemSrc, uint32_t size_in_u32);
void visdma_cmem_2_dram_nowait(void *vpDramDst, void *vpCmemSrc, uint32_t size_in_u32);
void visdma_cmem_2_dram_imm(void *vpDramDst, void *vpCmemSrc, uint32_t size_in_u32);
void visdma_cmem_2_dram_imm_nowait(void *vpDramDst, void *vpCmemSrc, uint32_t size_in_u32);

void visdma_dram_2_cmem(void *vpCmemDst, void *vpDramSrc, uint32_t size_in_u32);
void visdma_dram_2_cmem_nowait(void *vpCmemDst, void *vpDramSrc, uint32_t size_in_u32);
void visdma_dram_2_cmem_imm(void *vpCmemDst, void *vpDramSrc, uint32_t size_in_u32);
void visdma_dram_2_cmem_imm_nowait(void *vpCmemDst, void *vpDramSrc, uint32_t size_in_u32);

void visdma_cmem_2_coproc(void *vpVmemDst, void *vpCmemSrc, uint32_t size_in_u32);
void visdma_cmem_2_coproc_nowait(void *vpVmemDst, void *vpCmemSrc, uint32_t size_in_u32);
void visdma_cmem_2_coproc_imm(void *vpVmemDst, void *vpCmemSrc, uint32_t size_in_u32);
void visdma_cmem_2_coproc_imm_nowait(void *vpVmemDst, void *vpCmemSrc, uint32_t size_in_u32);

void visdma_coproc_2_cmem(void *vpCmemDst, void *vpVmemSrc, uint32_t size_in_u32);
void visdma_coproc_2_cmem_nowait(void *vpCmemDst, void *vpVmemSrc, uint32_t size_in_u32);
void visdma_coproc_2_cmem_imm(void *vpCmemDst, void *vpVmemSrc, uint32_t size_in_u32);
void visdma_coproc_2_cmem_imm_nowait(void *vpCmemDst, void *vpVmemSrc, uint32_t size_in_u32);

void cmem_2_dram(uint32_t dram_addr, uint32_t cmem_addr, uint32_t size_in_u32);
void cmem_2_dram_nowait(uint32_t dram_addr, uint32_t cmem_addr, uint32_t size_in_u32);
void cmem_2_dram_imm(uint32_t dram_addr, uint32_t cmem_addr, uint32_t size_in_u32);
void cmem_2_dram_imm_nowait(uint32_t dram_addr, uint32_t cmem_addr, uint32_t size_in_u32);

void dram_2_cmem(uint32_t cmem_addr, uint32_t dram_addr, uint32_t size_in_u32);
void dram_2_cmem_nowait(uint32_t cmem_addr, uint32_t dram_addr, uint32_t size_in_u32);
void dram_2_cmem_imm(uint32_t cmem_addr, uint32_t dram_addr, uint32_t size_in_u32);
void dram_2_cmem_imm_nowait(uint32_t cmem_addr, uint32_t dram_addr, uint32_t size_in_u32);

void cmem_2_coproc(uint32_t coproc_addr, uint32_t cmem_addr, uint32_t size_in_u32);
void cmem_2_coproc_nowait(uint32_t coproc_addr, uint32_t cmem_addr, uint32_t size_in_u32);
void cmem_2_coproc_imm(uint32_t coproc_addr, uint32_t cmem_addr, uint32_t size_in_u32);
void cmem_2_coproc_imm_nowait(uint32_t coproc_addr, uint32_t cmem_addr, uint32_t size_in_u32);

void coproc_2_cmem(uint32_t cmem_addr, uint32_t coproc_addr, uint32_t size_in_u32);
void coproc_2_cmem_nowait(uint32_t cmem_addr, uint32_t coproc_addr, uint32_t size_in_u32);
void coproc_2_cmem_imm(uint32_t cmem_addr, uint32_t coproc_addr, uint32_t size_in_u32);
void coproc_2_cmem_imm_nowait(uint32_t cmem_addr, uint32_t coproc_addr, uint32_t size_in_u32);


#else /* !MISRA_COMPLIANCE */

// Memory mapping for DMA space

#define DADDR_2_DMA_ADDR(daddr_) (daddr_)
#define CADDR_2_DMA_ADDR(caddr_) (((uint32_t)(caddr_ & (CMEM_SIZE - 1))) >> 2)
#define COPROCADDR_2_DMA_ADDR(coproc_addr_) (((uint32_t)(coproc_addr_)) >> 2)

// size is a constant expression, transfer size in words
#define dram_2_cmem_imm_nowait( _caddr, _daddr, _size_in_u32 ) \
visasm_membar();                                        \
asm volatile("lv.dram2cmemi %0, %1, %2" ::              \
             "r"(CADDR_2_DMA_ADDR(_caddr)),             \
             "r"(DADDR_2_DMA_ADDR(_daddr)),             \
             "g"(_size_in_u32 - 1) : "memory")

// size can be a variable, transfer size in words
#define dram_2_cmem_nowait( _caddr, _daddr, _size_in_u32 ) \
visasm_membar();                                    \
asm volatile("lv.dram2cmem %0, %1, %2" ::           \
             "r"(CADDR_2_DMA_ADDR(_caddr)),         \
             "r"(DADDR_2_DMA_ADDR(_daddr)),         \
             "r"(_size_in_u32 - 1) : "memory")

// size is a constant expression, transfer size in words
#define coproc_2_cmem_imm_nowait( _caddr, _coproc_addr, _size_in_u32 ) \
asm volatile("lv.coproc2cmemi %0, %1, %2" ::                    \
             "r"(CADDR_2_DMA_ADDR(_caddr)),                     \
             "r"(COPROCADDR_2_DMA_ADDR(_coproc_addr)),          \
             "g"(_size_in_u32 - 1) : "memory")

// size can be a variable, transfer size in words
#define coproc_2_cmem_nowait( _caddr, _coproc_addr, _size_in_u32 )      \
asm volatile("lv.coproc2cmem %0, %1, %2" ::                     \
             "r"(CADDR_2_DMA_ADDR(_caddr)),                     \
             "r"(COPROCADDR_2_DMA_ADDR(_coproc_addr)),          \
             "r"(_size_in_u32 - 1) : "memory")

#define dram_2_cmem_imm( _caddr, _daddr, _size )                \
do {                                                            \
  dram_2_cmem_imm_nowait( (_caddr), (_daddr), (_size) );        \
  wait( VIS_W_DRAM_DMA );                                       \
} while(0)

#define dram_2_cmem( _caddr, _daddr, _size )            \
do {                                                    \
  dram_2_cmem_nowait( (_caddr), (_daddr), (_size) );    \
  wait( VIS_W_DRAM_DMA );                               \
} while(0)

#define coproc_2_cmem_imm( _caddr, _coproc_addr, _size )                \
do {                                                                    \
  coproc_2_cmem_imm_nowait( (_caddr), (_coproc_addr), (_size) );        \
  wait( VIS_W_COPROC_DMA );                                             \
} while(0)

#define coproc_2_cmem( _caddr, _coproc_addr, _size )            \
do {                                                            \
  coproc_2_cmem_nowait( (_caddr), (_coproc_addr), (_size) );    \
  wait( VIS_W_COPROC_DMA );                                     \
} while(0)

#define cmem_2_dram_imm_nowait( _daddr, _caddr, _size_in_u32 ) \
visasm_membar();                                        \
asm volatile("lv.cmem2drami %0, %1, %2" ::              \
             "r"(DADDR_2_DMA_ADDR(_daddr)),             \
             "r"(CADDR_2_DMA_ADDR(_caddr)),             \
             "g"(_size_in_u32 - 1) : "memory")

#define cmem_2_dram_nowait( _daddr, _caddr, _size_in_u32 )     \
visasm_membar();                                        \
asm volatile("lv.cmem2dram %0, %1, %2" ::               \
             "r"(DADDR_2_DMA_ADDR(_daddr)),             \
             "r"(CADDR_2_DMA_ADDR(_caddr)),             \
             "r"(_size_in_u32 - 1) : "memory")

#define cmem_2_coproc_imm_nowait( _coproc_addr, _caddr, _size_in_u32 ) \
visasm_membar();                                                \
asm volatile("lv.cmem2coproci %0, %1, %2" ::                    \
             "r"(COPROCADDR_2_DMA_ADDR(_coproc_addr)),          \
             "r"(CADDR_2_DMA_ADDR(_caddr)),                     \
             "g"(_size_in_u32 - 1) : "memory")

#define cmem_2_coproc_nowait( _coproc_addr, _caddr, _size_in_u32 )     \
visasm_membar();                                                \
asm volatile("lv.cmem2coproc %0, %1, %2" ::                     \
             "r"(COPROCADDR_2_DMA_ADDR(_coproc_addr)),          \
             "r"(CADDR_2_DMA_ADDR(_caddr)),                     \
             "r"(_size_in_u32 - 1) : "memory")

#define cmem_2_dram_imm( _daddr, _caddr, _size )                \
do {                                                            \
  cmem_2_dram_imm_nowait( (_daddr), (_caddr), (_size) );        \
  wait( VIS_W_DRAM_DMA );                                       \
} while(0)

#define cmem_2_dram( _daddr, _caddr, _size )            \
do {                                                    \
  cmem_2_dram_nowait( (_daddr), (_caddr), (_size) );    \
  wait( VIS_W_DRAM_DMA );                               \
} while(0)

#define cmem_2_coproc_imm( _coproc_addr, _caddr, _size )                \
do {                                                                    \
  cmem_2_coproc_imm_nowait( (_coproc_addr), (_caddr), (_size) );        \
  wait( VIS_W_COPROC_DMA );                                             \
} while(0)

#define cmem_2_coproc( _coproc_addr, _caddr, _size )            \
do {                                                            \
  cmem_2_coproc_nowait( (_coproc_addr), (_caddr), (_size) );    \
  wait( VIS_W_COPROC_DMA );                                     \
} while(0)

#define visdma_cmem_2_dram(x,y,z)               cmem_2_dram((uint32_t)x, (uint32_t)y, z)
#define visdma_cmem_2_dram_nowait(x,y,z)        cmem_2_dram_nowait((uint32_t)x, (uint32_t)y, z)
#define visdma_cmem_2_dram_imm(x,y,z)           cmem_2_dram_imm((uint32_t)x, (uint32_t)y, z)
#define visdma_cmem_2_dram_imm_nowait(x,y,z)    cmem_2_dram_imm_nowait((uint32_t)x, (uint32_t)y, z)

#define visdma_dram_2_cmem(x,y,z)               dram_2_cmem((uint32_t)x, (uint32_t)y, z)
#define visdma_dram_2_cmem_nowait(x,y,z)        dram_2_cmem_nowait((uint32_t)x, (uint32_t)y, z)
#define visdma_dram_2_cmem_imm(x,y,z)           dram_2_cmem_imm((uint32_t)x, (uint32_t)y, z)
#define visdma_dram_2_cmem_imm_nowait(x,y,z)    dram_2_cmem_imm_nowait((uint32_t)x, (uint32_t)y, z)

#define visdma_cmem_2_coproc(x,y,z)             cmem_2_coproc((uint32_t)x, (uint32_t)y, z)
#define visdma_cmem_2_coproc_nowait(x,y,z)      cmem_2_coproc_nowait((uint32_t)x, (uint32_t)y, z)
#define visdma_cmem_2_coproc_imm(x,y,z)         cmem_2_coproc_imm((uint32_t)x, (uint32_t)y, z)
#define visdma_cmem_2_coproc_imm_nowait(x,y,z)  cmem_2_coproc_imm_nowait((uint32_t)x, (uint32_t)y, z)

#define visdma_coproc_2_cmem(x,y,z)             coproc_2_cmem((uint32_t)x, (uint32_t)y, z)
#define visdma_coproc_2_cmem_nowait(x,y,z)      coproc_2_cmem_nowait((uint32_t)x, (uint32_t)y, z)
#define visdma_coproc_2_cmem_imm(x,y,z)         coproc_2_cmem_imm((uint32_t)x, (uint32_t)y, z)
#define visdma_coproc_2_cmem_imm_nowait(x,y,z)  coproc_2_cmem_imm_nowait((uint32_t)x, (uint32_t)y, z)

#endif /* ?MISRA_COMPLIANCE */

// This takes daddr and caddr as byte addresses (caddr must be word
// aligned), and byte_size is the size in bytes to transfer, which
// must be a multiple of 4.  There is no limit to the DMA size (other
// than the sizes of the memories, of course).

#ifdef MISRA_COMPLIANCE

void dma_cmem_2_dram(uint32_t daddr, uint32_t caddr, uint32_t byte_size);
void dma_dram_2_cmem(uint32_t caddr, uint32_t daddr, uint32_t byte_size);
void dma_cmem_2_coproc(uint32_t coproc_addr, uint32_t caddr, uint32_t byte_size);
void dma_coproc_2_cmem(uint32_t caddr, uint32_t coproc_addr, uint32_t byte_size);

#else /* !MISRA_COMPLIANCE : function-like macros are okay */

#define dma_cmem_2_dram( _daddr, _caddr, _byte_size )                   \
({                                                                      \
                                                                        \
  uint32_t error_code =                                                 \
    !(                                                                  \
      (((_byte_size) & 0x3) == 0) &&                                    \
      (((_caddr) & 0x3) == 0)                                           \
      );                                                                \
                                                                        \
  if( !error_code ) {                                                   \
                                                                        \
    unsigned words = (_byte_size)>>2;                                   \
    unsigned dram_addr = (_daddr);                                      \
    unsigned cmem_addr = (_caddr);                                      \
                                                                        \
    while( words ) {                                                    \
      unsigned words_this_transfer = (words > 256) ? 256 : words;       \
      unsigned bytes_this_transfer = words_this_transfer << 2;          \
                                                                        \
      cmem_2_dram_nowait( dram_addr, cmem_addr, words_this_transfer );  \
                                                                        \
      dram_addr += bytes_this_transfer;                                 \
      cmem_addr += bytes_this_transfer;                                 \
      words -= words_this_transfer;                                     \
    }                                                                   \
    wait( VIS_W_DRAM_DMA );                                             \
  }                                                                     \
  error_code ? 0xffffffffU : 0;                                         \
})

#define dma_dram_2_cmem( _caddr, _daddr, _byte_size )                   \
({                                                                      \
  uint32_t error_code =                                                 \
    !(                                                                  \
      (((_byte_size) & 0x3) == 0) &&                                    \
      (((_caddr) & 0x3) == 0)                                           \
      );                                                                \
                                                                        \
  if( !error_code ) {                                                   \
                                                                        \
    unsigned words = (_byte_size)>>2;                                   \
    unsigned dram_addr = (_daddr);                                      \
    unsigned cmem_addr = (_caddr);                                      \
                                                                        \
    while( words ) {                                                    \
      unsigned words_this_transfer = (words > 256) ? 256 : words;       \
      unsigned bytes_this_transfer = words_this_transfer << 2;          \
                                                                        \
      dram_2_cmem_nowait( cmem_addr, dram_addr, words_this_transfer );  \
                                                                        \
      dram_addr += bytes_this_transfer;                                 \
      cmem_addr += bytes_this_transfer;                                 \
      words -= words_this_transfer;                                     \
    }                                                                   \
    wait( VIS_W_DRAM_DMA );                                             \
  }                                                                     \
                                                                        \
  error_code ? 0xffffffffU : 0;                                         \
})

#ifdef CHIP_CV6 /* BUG #21757 : Limit the transfer size for cmem->vmem to 31 words */
#define MAX_C2V_XFER_SIZE_BUG21757    31U
#else /* !CHIP_CV6 */
#define MAX_C2V_XFER_SIZE_BUG21757    256U
#endif /* ?CHIP_CV6 */

#define dma_cmem_2_coproc( _coproc_addr, _caddr, _byte_size )           \
({                                                                      \
  uint32_t error_code =                                                 \
    !(                                                                  \
      (((_byte_size) & 0x3) == 0) &&                                    \
      (((_caddr) & 0x3) == 0) &&                                        \
      (((_coproc_addr) & 0x3 ) == 0)                                    \
      );                                                                \
                                                                        \
  if( !error_code ) {                                                   \
    unsigned words = (_byte_size)>>2;                                   \
    unsigned coproc_addr = (_coproc_addr);                              \
    unsigned cmem_addr = (_caddr);                                      \
                                                                        \
    while( words ) {                                                    \
      unsigned words_this_transfer = (words > MAX_C2V_XFER_SIZE_BUG21757) ? MAX_C2V_XFER_SIZE_BUG21757 : words;       \
      unsigned bytes_this_transfer = words_this_transfer << 2;          \
                                                                        \
      cmem_2_coproc_nowait( coproc_addr, cmem_addr, words_this_transfer ); \
                                                                        \
      coproc_addr += bytes_this_transfer;                               \
      cmem_addr += bytes_this_transfer;                                 \
      words -= words_this_transfer;                                     \
    }                                                                   \
    wait( VIS_W_COPROC_DMA );                                           \
  }                                                                     \
                                                                        \
  error_code ? 0xffffffffU : 0;                                         \
})

#define dma_coproc_2_cmem( _caddr, _coproc_addr, _byte_size )           \
({                                                                      \
  uint32_t error_code =                                                 \
    !(                                                                  \
      (((_byte_size) & 0x3) == 0) &&                                    \
      (((_caddr) & 0x3) == 0) &&                                        \
      (((_coproc_addr) & 0x3 ) == 0)                                    \
      );                                                                \
                                                                        \
  if ( !error_code ) {                                                  \
    unsigned words = (_byte_size)>>2;                                   \
    unsigned coproc_addr = (_coproc_addr);                              \
    unsigned cmem_addr = (_caddr);                                      \
                                                                        \
    while( words ) {                                                    \
      unsigned words_this_transfer = (words > 256) ? 256 : words;       \
      unsigned bytes_this_transfer = words_this_transfer << 2;          \
                                                                        \
      coproc_2_cmem_nowait( cmem_addr, coproc_addr, words_this_transfer ); \
                                                                        \
      coproc_addr += bytes_this_transfer;                               \
      cmem_addr += bytes_this_transfer;                                 \
      words -= words_this_transfer;                                     \
    }                                                                   \
    wait( VIS_W_COPROC_DMA );                                           \
  }                                                                     \
                                                                        \
  error_code ? 0xffffffffU : 0;                                         \
})

#endif


// #ifndef __ASSEMBLER__
#endif

// #if defined(ORCVIS)
#endif


//////////////////////////////////////////////////////////////////////////////
// END Orc-Specific section

#ifdef CMEM_SIZE

/// Same as CMEM_SIZE - for compatibility with old code.
#define DATA_MEM_SIZE CMEM_SIZE

#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef MISRA_COMPLIANCE

// The funny-business with bound is to make sure we don't evaluate
// mem+size, whcih might overflow;

#define memset(mem_, value_, size_)             \
  do {                                          \
    int i;                                      \
    uint8_t *p = (uint8_t*)(mem_);              \
    int bound = (size_)-1;                      \
    for ( i = 0; i <= bound; i++) {             \
      *p++ = (value_);                          \
    }                                           \
  } while (0)
  
#endif /* !MISRA_COMPLIANCE */

//////////////////////////////////////////////////////////////////////////////

#endif
