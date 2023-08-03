/* ARM NEON intrinsics include file.
   Copyright (C) 2011-2013 Free Software Foundation, Inc.
   Contributed by ARM Ltd.
   This file is part of GCC.
   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.
   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.
   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */
#ifndef _AARCH64_NEON_H_
#define _AARCH64_NEON_H_
//#include <stdint.h>

#pragma GCC push_options
#pragma GCC target ("+nothing+simd")

typedef __builtin_aarch64_simd_qi int8x8_t
  __attribute__ ((__vector_size__ (8)));
typedef __builtin_aarch64_simd_hi int16x4_t
  __attribute__ ((__vector_size__ (8)));
typedef __builtin_aarch64_simd_si int32x2_t
  __attribute__ ((__vector_size__ (8)));

typedef __builtin_aarch64_simd_sf float32x2_t
  __attribute__ ((__vector_size__ (8)));
typedef __builtin_aarch64_simd_poly8 poly8x8_t
  __attribute__ ((__vector_size__ (8)));
typedef __builtin_aarch64_simd_poly16 poly16x4_t
  __attribute__ ((__vector_size__ (8)));
typedef __builtin_aarch64_simd_uqi uint8x8_t
  __attribute__ ((__vector_size__ (8)));
typedef __builtin_aarch64_simd_uhi uint16x4_t
  __attribute__ ((__vector_size__ (8)));
typedef __builtin_aarch64_simd_usi uint32x2_t
  __attribute__ ((__vector_size__ (8)));

typedef __builtin_aarch64_simd_qi int8x16_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_hi int16x8_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_si int32x4_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_di int64x2_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_sf float32x4_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_df float64x2_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_poly8 poly8x16_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_poly16 poly16x8_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_uqi uint8x16_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_uhi uint16x8_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_usi uint32x4_t
  __attribute__ ((__vector_size__ (16)));
typedef __builtin_aarch64_simd_udi uint64x2_t
  __attribute__ ((__vector_size__ (16)));

typedef signed char         int8_t;         /* 8 bits, [-128, 127] */
typedef short               int16_t;        /* 16 bits */
typedef int                 int32_t;        /* 32 bits */
typedef long long           int64_t;        /* 64 bits */
typedef unsigned char       uint8_t;        /* 8 bits, [0, 255] */
typedef unsigned short      uint16_t;       /* 16 bits */
typedef unsigned int        uint32_t;       /* 32 bits */
typedef unsigned long long  uint64_t;       /* 64 bits */
typedef unsigned short      wchar;          /* 16 bits */
typedef int                intptr_t;
typedef unsigned int       uintptr_t;
typedef int64_t int64x1_t;
typedef int32_t int32x1_t;
typedef int16_t int16x1_t;
typedef int8_t int8x1_t;
typedef double float64x1_t;
typedef uint64_t uint64x1_t;
typedef uint32_t uint32x1_t;
typedef uint16_t uint16x1_t;
typedef uint8_t uint8x1_t;
typedef float float32_t;
typedef double float64_t;
typedef __builtin_aarch64_simd_poly8 poly8_t;
typedef __builtin_aarch64_simd_poly16 poly16_t;

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vld1q_u8 (const uint8_t *a)
{
  return (uint8x16_t)
    __builtin_aarch64_ld1v16qi ((const __builtin_aarch64_simd_qi *) a);
}

__extension__ static __inline uint8_t __attribute__ ((__always_inline__))
vaddvq_u8 (uint8x16_t __a)
{
  return (uint8_t) __builtin_aarch64_reduc_plus_scal_v16qi ((int8x16_t) __a);
}


__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
veorq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a ^ __b;
}

__extension__ extern __inline uint8x16_t __attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
vcntq_u8 (uint8x16_t __a)
{
  return (uint8x16_t) __builtin_aarch64_popcountv16qi ((int8x16_t) __a);
}

#if 0

#define __DEFINTERLEAVE(op, rettype, intype, funcsuffix, Q) 		\
  __extension__ static __inline rettype					\
  __attribute__ ((__always_inline__))					\
  v ## op ## Q ## _ ## funcsuffix (intype a, intype b)			\
  {									\
    return (rettype) {v ## op ## 1 ## Q ## _ ## funcsuffix (a, b),	\
		      v ## op ## 2 ## Q ## _ ## funcsuffix (a, b)};	\
  }
#define __INTERLEAVE_LIST(op)					\
  __DEFINTERLEAVE (op, float32x2x2_t, float32x2_t, f32,)	\
  __DEFINTERLEAVE (op, poly8x8x2_t, poly8x8_t, p8,)		\
  __DEFINTERLEAVE (op, poly16x4x2_t, poly16x4_t, p16,)		\
  __DEFINTERLEAVE (op, int8x8x2_t, int8x8_t, s8,)		\
  __DEFINTERLEAVE (op, int16x4x2_t, int16x4_t, s16,)		\
  __DEFINTERLEAVE (op, int32x2x2_t, int32x2_t, s32,)		\
  __DEFINTERLEAVE (op, uint8x8x2_t, uint8x8_t, u8,)		\
  __DEFINTERLEAVE (op, uint16x4x2_t, uint16x4_t, u16,)		\
  __DEFINTERLEAVE (op, uint32x2x2_t, uint32x2_t, u32,)		\
  __DEFINTERLEAVE (op, float32x4x2_t, float32x4_t, f32, q)	\
  __DEFINTERLEAVE (op, poly8x16x2_t, poly8x16_t, p8, q)		\
  __DEFINTERLEAVE (op, poly16x8x2_t, poly16x8_t, p16, q)	\
  __DEFINTERLEAVE (op, int8x16x2_t, int8x16_t, s8, q)		\
  __DEFINTERLEAVE (op, int16x8x2_t, int16x8_t, s16, q)		\
  __DEFINTERLEAVE (op, int32x4x2_t, int32x4_t, s32, q)		\
  __DEFINTERLEAVE (op, uint8x16x2_t, uint8x16_t, u8, q)		\
  __DEFINTERLEAVE (op, uint16x8x2_t, uint16x8_t, u16, q)	\
  __DEFINTERLEAVE (op, uint32x4x2_t, uint32x4_t, u32, q)
/* vuzp */
__INTERLEAVE_LIST (uzp)
/* vzip */
__INTERLEAVE_LIST (zip)
#undef __INTERLEAVE_LIST
#undef __DEFINTERLEAVE
#endif
/* End of optimal implementations in approved order.  */
#endif

#pragma GCC pop_options



