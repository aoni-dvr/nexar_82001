/* ARM NEON intrinsics include file. This file is generated automatically
   using neon-gen.ml.  Please do not edit manually.
   Copyright (C) 2006, 2007, 2009 Free Software Foundation, Inc.
   Contributed by CodeSourcery.
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
#ifndef GCC_ARM_NEON_H
#define GCC_ARM_NEON_H 
#ifndef __ARM_NEON__
#error You must enable NEON instructions (e.g. -mfloat-abi=softfp -mfpu=neon) to use arm_neon.h
#else
#ifdef __cplusplus
extern "C" {
#endif

typedef __builtin_neon_qi int8x8_t __attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_uqi uint8x8_t __attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_qi int8x16_t __attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_uqi uint8x16_t __attribute__ ((__vector_size__ (16)));

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

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vld1q_u8 (const uint8_t * __a)
{
  return (uint8x16_t)__builtin_neon_vld1v16qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcntq_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vcntv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint8x16_t __attribute__  ((__always_inline__, __gnu_inline__, __artificial__))
veorq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return __a ^ __b;
}

 __extension__ static __inline uint8x16_t __attribute__  ((__always_inline__, __gnu_inline__, __artificial__))
 vaddq_u8 (uint8x16_t __a, uint8x16_t __b)
 {
   return __a + __b;
 }

__extension__ static __inline uint8_t __attribute__  ((__always_inline__, __gnu_inline__, __artificial__))
vgetq_lane_u8 (uint8x16_t __a, const int32_t __b)
{
  return (uint8_t)__builtin_neon_vget_laneuv16qi ((int8x16_t) __a, __b);
}

#ifdef __cplusplus
}
#endif
#endif
#endif

