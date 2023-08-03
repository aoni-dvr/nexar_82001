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

#ifndef VIS_COPROC_H_
#define VIS_COPROC_H_

#ifdef ORCVIS

#include <orc_spr.h>

#ifndef __ASSEMBLER__

#ifdef MISRA_COMPLIANCE

//////////////////////////////////////////////////////////////////////////////
// POKE
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

// Word

/*! @brief Poke the specified 4-byte value to the coproc address
  base_address+offset using the run queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_word_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 4-byte value to the specified coproc
  address using the run queue. */
void coproc_poke_word(uint32_t value, uint32_t base_address);

/*! @brief Poke the specified 4-byte value to the coproc address
  base_address+offset using the load queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_ldq_word_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 4-byte value to the specified coproc
  address using the load queue. */
void coproc_poke_ldq_word(uint32_t value, uint32_t base_address);

/*! @brief Poke the specified 4-byte value to the coproc address
  base_address+offset using the store queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_stq_word_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 4-byte value to the specified coproc
  address using the store queue. */
void coproc_poke_stq_word(uint32_t value, uint32_t base_address);

// Half-Word

/*! @brief Poke the specified 2-byte value to the coproc address
  base_address+offset using the run queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_half_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 2-byte value to the specified coproc
  address using the run queue. */
void coproc_poke_half(uint32_t value, uint32_t base_address);

/*! @brief Poke the specified 2-byte value to the coproc address
  base_address+offset using the load queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_ldq_half_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 2-byte value to the specified coproc
  address using the load queue. */
void coproc_poke_ldq_half(uint32_t value, uint32_t base_address);

/*! @brief Poke the specified 2-byte value to the coproc address
  base_address+offset using the store queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_stq_half_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 2-byte value to the specified coproc
  address using the store queue. */
void coproc_poke_stq_half(uint32_t value, uint32_t base_address);

// Byte

/*! @brief Poke the specified 1-byte value to the coproc address
  base_address+offset using the run queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_byte_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 1-byte value to the specified coproc
  address using the run queue. */
void coproc_poke_byte(uint32_t value, uint32_t base_address);

/*! @brief Poke the specified 1-byte value to the coproc address
  base_address+offset using the load queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_ldq_byte_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 1-byte value to the specified coproc
  address using the load queue. */
void coproc_poke_ldq_byte(uint32_t value, uint32_t base_address);

/*! @brief Poke the specified 1-byte value to the coproc address
  base_address+offset using the store queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
void coproc_poke_stq_byte_offset(uint32_t value, uint32_t base_address, uint32_t offset);

/*! @brief Poke the specified 1-byte value to the specified coproc
  address using the store queue. */
void coproc_poke_stq_byte(uint32_t value, uint32_t base_address);

//////////////////////////////////////////////////////////////////////////////

// Poke Bits

/*! @brief Poke the specified 1-byte value to the specified address
  using a mask on the run queue.  The mask must be a constant
  expression. */
void coproc_poke_bits(uint32_t value, uint32_t address, uint8_t mask);

/*! @brief Poke the specified 1-byte value to the specified address
  using a mask on the load queue.  The mask must be a constant
  expression. */
void coproc_poke_bits_ldq(uint32_t value, uint32_t address, uint8_t mask);

/*! @brief Poke the specified 1-byte value to the specified address
  using a mask on the store queue.  The mask must be a constant
  expression. */
void coproc_poke_bits_stq(uint32_t value, uint32_t address, uint8_t mask);

//////////////////////////////////////////////////////////////////////////////

void coproc_poke_bits_word(uint32_t value, uint32_t address, uint32_t mask);
void coproc_poke_bits_word_ldq(uint32_t value, uint32_t address, uint32_t mask);
void coproc_poke_bits_word_stq(uint32_t value, uint32_t address, uint32_t mask);

//////////////////////////////////////////////////////////////////////////////

void coproc_poke_field(uint32_t value, uint32_t address, uint32_t offset, uint32_t size);
void coproc_poke_field_ldq(uint32_t value, uint32_t address, uint32_t offset, uint32_t size);
void coproc_poke_field_stq(uint32_t value, uint32_t address, uint32_t offset, uint32_t size);

//////////////////////////////////////////////////////////////////////////////

// Run

/*! @brief Run a command-block using the specified base address */
void coproc_run(uint32_t base);

//////////////////////////////////////////////////////////////////////////////

// Dmald

/*! @brief Enqueue a coprocessor DMA load using the specified
  descriptor address. */
void coproc_dmald(uint32_t descr_address);

//////////////////////////////////////////////////////////////////////////////

// Dmast

/*! @brief Enqueue a coprocessor DMA store using the specified
  descriptor address. */
void coproc_dmast(uint32_t descr_address);

/*! @brief Enqueue a coprocessor DMA store using the specified
  descriptor address.  The flush bit is set. */
void coproc_dmast_flush(uint32_t descr_address);

//////////////////////////////////////////////////////////////////////////////

/*! @brief Same as coproc_peek, but the effective src address is
  base_address+offset_.

  Offset_ must be a constant expression.  This can potentially save a
  cycle or two if base_address_ is already register-allocated. */
void coproc_peek_word_offset(uint32_t index, uint32_t base_address, uint32_t offset);

/*! @brief Peek a 4-byte word from the specified source address in the
  coproc to the specified peek index.

  The "index" must a constant expression from 0-15. */
void coproc_peek_word(uint32_t index, uint32_t base_address);


/*! @brief Same as coproc_peek_half, but the effective src address is
  base_address+offset_.

  Offset_ must be a constant expression.  This can potentially save a
  cycle or two if base_address_ is already register-allocated. */
void coproc_peek_half_offset(uint32_t index, uint32_t base_address, uint32_t offset);


/*! @brief Peek a 2-byte half from the specified source address in the
  coproc to the specified index.  The index must a constant
  expression from 0-15. */
void coproc_peek_half(uint32_t index, uint32_t base_address);


/*! @brief Same as coproc_peek_byte, but the effective src address is
  base_address+offset_.

  Offset_ must be a constant expression.  This can potentially save a
  cycle or two if base_address_ is already register-allocated. */
void coproc_peek_byte_offset(uint32_t index, uint32_t base_address, uint32_t offset);

/*! @brief Peek a byte from the specified source address in the
  coproc to the specified index.

  The index must a constant expression from 0-15. */
void coproc_peek_byte(uint32_t index, uint32_t base_address);

//////////////////////////////////////////////////////////////////////////////

/*! @brief Read the specified peek register (0-15).

   The index must be a constant expression, but its a few cycles
   faster than get_peek_value(), which can take a variable. */
uint32_t get_peek_value_imm(uint32_t index);

/*! @brief Read the specified peek register (0-15).

   The index can be a variable, but this takes a few cycles longer
   than get_peek_value_imm(). */
uint32_t get_peek_value(uint32_t index);

//////////////////////////////////////////////////////////////////////////////


#else /* !MISRA_COMPLIANCE */

//////////////////////////////////////////////////////////////////////////////
// POKE
//////////////////////////////////////////////////////////////////////////////


/* not for clients */
#define coproc_poke_( size_str_, q_str_, value_, base_addr_, offset_ )  \
asm volatile("lv.poke." size_str_ q_str_ " %0(%1), %2" ::               \
             "g"(offset_), "r"(base_addr_), "r"(value_))

//////////////////////////////////////////////////////////////////////////////

// Word

/*! @brief Poke the specified 4-byte value to the coproc address
  base_address+offset using the run queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_word_offset( value_, base_address_, offset_ )       \
  coproc_poke_("word", "", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 4-byte value to the specified coproc
  address using the run queue. */
#define coproc_poke_word( value_, address_ )            \
  coproc_poke_word_offset( (value_), (address_), 0 )

/*! @brief Poke the specified 4-byte value to the coproc address
  base_address+offset using the load queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_ldq_word_offset( value_, base_address_, offset_ )   \
  coproc_poke_("word", ".ldq", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 4-byte value to the specified coproc
  address using the load queue. */
#define coproc_poke_ldq_word( value_, address_ )                        \
  coproc_poke_ldq_word_offset( (value_), (address_), 0 )

/*! @brief Poke the specified 4-byte value to the coproc address
  base_address+offset using the store queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_stq_word_offset( value_, base_address_, offset_ )   \
  coproc_poke_("word", ".stq", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 4-byte value to the coproc address
  base_address+offset using the load queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
/*! @brief Poke the specified 4-byte value to the specified coproc
  address using the store queue. */
#define coproc_poke_stq_word( value_, address_ )                        \
  coproc_poke_stq_word_offset( (value_), (address_), 0 )



// Half-Word
  
/*! @brief Poke the specified 2-byte value to the coproc address
  base_address+offset using the run queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_half_offset( value_, base_address_, offset_ )       \
  coproc_poke_("half", "", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 2-byte value to the specified coproc
  address using the run queue. */
#define coproc_poke_half( value_, address_ )                            \
  coproc_poke_half_offset( (value_), (address_), 0 )

/*! @brief Poke the specified 2-byte value to the coproc address
  base_address+offset using the load queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_ldq_half_offset( value_, base_address_, offset_ )   \
  coproc_poke_("half", ".ldq", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 2-byte value to the specified coproc
  address using the load queue. */
#define coproc_poke_ldq_half( value_, address_ )                        \
  coproc_poke_ldq_half_offset( (value_), (address_), 0 )

/*! @brief Poke the specified 2-byte value to the coproc address
  base_address+offset using the store queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_stq_half_offset( value_, base_address_, offset_ )   \
  coproc_poke_("half", ".stq", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 2-byte value to the specified coproc
  address using the store queue. */
#define coproc_poke_stq_half( value_, address_ )                        \
  coproc_poke_stq_half_offset( (value_), (address_), 0 )

// Byte
  
/*! @brief Poke the specified 1-byte value to the coproc address
  base_address+offset using the run queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_byte_offset( value_, base_address_, offset_ )       \
  coproc_poke_("byte", "", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 1-byte value to the specified coproc
  address using the run queue. */
#define coproc_poke_byte( value_, address_ )                            \
  coproc_poke_byte_offset( (value_), (address_), 0 )

/*! @brief Poke the specified 1-byte value to the coproc address
  base_address+offset using the load queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_ldq_byte_offset( value_, base_address_, offset_ )   \
  coproc_poke_("byte", ".ldq", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 1-byte value to the specified coproc
  address using the load queue. */
#define coproc_poke_ldq_byte( value_, address_ )                        \
  coproc_poke_ldq_byte_offset( (value_), (address_), 0 )

/*! @brief Poke the specified 1-byte value to the coproc address
  base_address+offset using the store queue.

  The offset must be a constant expression. This macro can potentially
  save one or two Orc cycles over the non-offset version if you want
  to poke to constant offset from an already-register-allocated base
  address. */
#define coproc_poke_stq_byte_offset( value_, base_address_, offset_ )   \
  coproc_poke_("byte", ".stq", (value_), (base_address_), (offset_))

/*! @brief Poke the specified 1-byte value to the specified coproc
  address using the store queue. */
#define coproc_poke_stq_byte( value_, address_ )                        \
  coproc_poke_stq_byte_offset( (value_), (address_), 0 )

//////////////////////////////////////////////////////////////////////////////

// Poke Bits

// Not for clients
#define coproc_poke_bits_( q_str_, value_, address_, mask_ )    \
  asm volatile("lv.poke.bits" q_str_ " %0, %1, %2" ::           \
               "r"(address_), "r"(value_), "g"(mask_))

/*! @brief Poke the specified 1-byte value to the specified address
  using a mask on the run queue.  The mask must be a constant
  expression. */
#define coproc_poke_bits( value_, address_, mask_ )     \
  coproc_poke_bits_("", (value_), (address_), (mask_))

/*! @brief Poke the specified 1-byte value to the specified address
  using a mask on the load queue.  The mask must be a constant
  expression. */
#define coproc_poke_bits_ldq( value_, address_, mask_ ) \
  coproc_poke_bits_(".ldq", (value_), (address_), (mask_))

/*! @brief Poke the specified 1-byte value to the specified address
  using a mask on the store queue.  The mask must be a constant
  expression. */
#define coproc_poke_bits_stq( value_, address_, mask_ ) \
  coproc_poke_bits_(".stq", (value_), (address_), (mask_))

//////////////////////////////////////////////////////////////////////////////

// Not for clients
#define coproc_poke_bits_word_( q_str_, value_, address_, mask_ )       \
do {                                                                    \
  if( (mask_) & 0xff ) {                                                \
    coproc_poke_bits_( q_str_, ((value_) >> 0) & 0xff, (address_),      \
                       ((mask_) >> 0) & 0xff );                         \
  }                                                                     \
  if( ((mask_) >> 8) & 0xff ) {                                         \
    coproc_poke_bits_( q_str_, ((value_) >> 8) & 0xff, (address_)+1,    \
                       ((mask_) >> 8) & 0xff);                          \
  }                                                                     \
  if( ((mask_) >> 16) & 0xff ) {                                        \
    coproc_poke_bits_( q_str_, ((value_) >> 16) & 0xff, (address_)+2,   \
                       ((mask_) >> 16) & 0xff);                         \
  }                                                                     \
  if( ((mask_) >> 24) & 0xff ) {                                        \
    coproc_poke_bits_( q_str_, ((value_) >> 24) & 0xff, (address_)+3,   \
                       ((mask_) >> 24) & 0xff);                         \
  }                                                                     \
                                                                        \
} while(0)

// Mask must be a constant expression
#define coproc_poke_bits_word( value_, address_, mask_ )        \
  coproc_poke_bits_word_( "", (value_), (address_), (mask_) )

#define coproc_poke_bits_word_ldq( value_, address_, mask_ )        \
  coproc_poke_bits_word_( ".ldq", (value_), (address_), (mask_) )

#define coproc_poke_bits_word_stq( value_, address_, mask_ )        \
  coproc_poke_bits_word_( ".stq", (value_), (address_), (mask_) )

//////////////////////////////////////////////////////////////////////////////

// Not for clients
#define coproc_poke_field_( q_str_, value_, address_, offset_, size_ )  \
coproc_poke_bits_word_( q_str_,                                         \
                        (value_) << ((offset_) & 0x7),                  \
                        (address_) + ((offset_) >> 3),                  \
                        (((size_)==32)?0xffffffffU:((1 << (size_)) - 1))  \
                        << ((offset_) & 0x7) )

// offset and size must be constant expressions
// Field must not span a word boundary
#define coproc_poke_field( value_, address_, offset_, size_ )           \
  coproc_poke_field_( "", (value_), (address_), (offset_), (size_) )

#define coproc_poke_field_ldq( value_, address_, offset_, size_ )       \
  coproc_poke_field_( ".ldq", (value_), (address_), (offset_), (size_) )

#define coproc_poke_field_stq( value_, address_, offset_, size_ )       \
  coproc_poke_field_( ".stq", (value_), (address_), (offset_), (size_) )

//////////////////////////////////////////////////////////////////////////////

// Run

/*! @brief Run a command-block using the specified base address */
#define coproc_run( base_ )                      \
  asm volatile("lv.run %0" :: "r"( base_ ) )

//////////////////////////////////////////////////////////////////////////////

// Dmald

/*! @brief Enqueue a coprocessor DMA load using the specified
  descriptor address. */
#define coproc_dmald( descr_address_ )                          \
  asm volatile("lv.dmald %0" :: "r"( descr_address_ ) )

//////////////////////////////////////////////////////////////////////////////

// Dmast

/*! @brief Enqueue a coprocessor DMA store using the specified
  descriptor address. */
#define coproc_dmast( descr_address_ )                          \
  asm volatile("lv.dmast %0" :: "r"( descr_address_ ) )

/*! @brief Enqueue a coprocessor DMA store using the specified
  descriptor address.  The flush bit is set. */
#define coproc_dmast_flush( descr_address_ )                          \
  asm volatile("lv.dmast.flush %0" :: "r"( descr_address_ ) )


//////////////////////////////////////////////////////////////////////////////

/* Not for clients */
#define coproc_peek_( size_str_, index_, base_address_, offset_ )       \
  asm volatile("lv.peek." size_str_ " %0, %1(%2)" ::                     \
               "g"(index_), "g"(offset_), "r"(base_address_))


/*! @brief Same as coproc_peek, but the effective src address is
  base_address+offset_.

  Offset_ must be a constant expression.  This can potentially save a
  cycle or two if base_address_ is already register-allocated. */
#define coproc_peek_word_offset(index_, base_address_, offset_)         \
  coproc_peek_("word", (index_), (base_address_), (offset_))

/*! @brief Peek a 4-byte word from the specified source address in the
  coproc to the specified peek index.

  The "index" must a constant expression from 0-15. */
#define coproc_peek_word(index_, src_)                  \
  coproc_peek_word_offset( (index_), (src_), 0 )


/*! @brief Same as coproc_peek_half, but the effective src address is
  base_address+offset_.

  Offset_ must be a constant expression.  This can potentially save a
  cycle or two if base_address_ is already register-allocated. */
#define coproc_peek_half_offset(index_, base_address_, offset_)         \
  coproc_peek_("half", (index_), (base_address_), (offset_))


/*! @brief Peek a 2-byte half from the specified source address in the
  coproc to the specified index.  The index must a constant
  expression from 0-15. */
#define coproc_peek_half(index_, src_)                  \
  coproc_peek_half_offset( (index_), (src_), 0 )


/*! @brief Same as coproc_peek_byte, but the effective src address is
  base_address+offset_.

  Offset_ must be a constant expression.  This can potentially save a
  cycle or two if base_address_ is already register-allocated. */
#define coproc_peek_byte_offset(index_, base_address_, offset_)         \
  coproc_peek_("byte", (index_), (base_address_), (offset_))

/*! @brief Peek a byte from the specified source address in the
  coproc to the specified index.

  The index must a constant expression from 0-15. */
#define coproc_peek_byte(index_, src_)                  \
  coproc_peek_byte_offset( (index_), (src_), 0 )

//////////////////////////////////////////////////////////////////////////////

/*! @brief Read the specified peek register (0-15).
 
   The index must be a constant expression, but its a few cycles
   faster than get_peek_value(), which can take a variable. */
#define get_peek_value_imm( index_ )            \
  read_spr_imm( (index_) + SPR_PEEK_BASE )

/*! @brief Read the specified peek register (0-15).
 
   The index can be a variable, but this takes a few cycles longer
   than get_peek_value_imm(). */
#define get_peek_value( index_ )                \
  read_spr( (index_) + SPR_PEEK_BASE )
  
//////////////////////////////////////////////////////////////////////////////

#endif /* ?MISRA_COMPLIANCE */
#endif /* !__ASSEMBLER__ */
#endif /* ORCVIS */
#endif /* !VIS_COPROC_H_ */
