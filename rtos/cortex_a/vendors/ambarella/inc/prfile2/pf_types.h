/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan

     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.

     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.

     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
 /****************************************************************************
 [pf_types.h] - Basic type definitions depending on user's environment.

  NOTES:
   - These definitions are system-dependent; User can configure them for
     user's specific environment.
 ****************************************************************************/
#ifndef PF_TYPES_H
#define PF_TYPES_H

#include "e_types.h"

#ifndef PF_CONFIG_H
#include "prfile2/pf_config.h"
#endif

#if defined AMBA_FS_KAL
#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#endif

#define OFF_T_DEFINED

/****************************************************************************
  Basic type definitions depending on user's environment.
 ****************************************************************************/
/*===========================================================================
  Basic integer types commonly used in PrFILE2
 ============================================================================*/
#define PF_CHAR                     e_char_t    /* 8-bit integer            */
#define PF_WCHAR                    uint16_t    /* 16-bit unsigned integer  */
#define PF_U_CHAR                   uint8_t     /* 8-bit unsigned integer   */
#define PF_S_CHAR                   int8_t      /* 8-bit signed integer     */
#define PF_U_SHORT                  uint16_t    /* 16-bit unsigned integer  */
#define PF_S_SHORT                  int16_t     /* 16-bit signed integer    */
#define PF_U_LONG                   uint32_t    /* 32-bit unsigned integer  */
#define PF_S_LONG                   int32_t     /* 32-bit signed integer    */
#if PF_EXFAT_SUPPORT
#define PF_INT64                    int64_t     /* 64-bit signed integer    */
#define PF_UINT64                   uint64_t    /* 64-bit signed integer    */
#endif /* PF_EXFAT_SUPPORT */
#define PF_UTF8                     uint8_t     /* 8-bit unsigned integer   */
#define PF_UTF16                    uint16_t    /* 16-bit unsigned integer  */
#define PF_UTF32                    uint32_t    /* 32-bit unsigned integer  */

#if defined AMBA_FS_KAL
/* For MisraC checking. Move following define to 'pdm_mbr.c'. */
#else
/*---------------------------------------------------------------------------
  64bit Value
 ----------------------------------------------------------------------------*/
#if defined (_MSC_VER) && (_MSC_VER <= 1300)
#define PF_INT64_C(val)             (0I64  + (val))
#define PF_UINT64_C(val)            (0UI64 + (val))
#else
#define PF_INT64_C(val)             (0LL  + (val##LL))
#define PF_UINT64_C(val)            (0ULL + (val##ULL))
#endif

#define PF_ULL(val)                 ((PF_UINT64)PF_UINT64_C(val))
#endif

/*---------------------------------------------------------------------------
  Internal Error Number
 ----------------------------------------------------------------------------*/
typedef PF_S_LONG                   PF_ERROR;


/*---------------------------------------------------------------------------
  const definition for API
    This is for compatibility with PrFILE.
 ----------------------------------------------------------------------------*/
#define PF_CONST                    const


/*---------------------------------------------------------------------------
  inline
 ----------------------------------------------------------------------------*/
#define PF_INLINE                   ESOL_INLINE


/*---------------------------------------------------------------------------
  size_t
 ----------------------------------------------------------------------------*/
/* AMBA_FS_KAL */
#if (PF_STDLIB_ALIAS_SUPPORT == 0) /* !PF_STDLIB_ALIAS_SUPPORT */

#ifndef AMBA_FS_KAL
  #include <stdlib.h>
#endif

#else /* PF_STDLIB_ALIAS_SUPPORT */

  #define size_t        uint32_t

#endif /* !PF_STDLIB_ALIAS_SUPPORT */

#ifdef AMBA_FS_KAL
#define PF_SIZE_T       SIZE_t              /* size_t definition        */
#else
#define PF_SIZE_T       size_t              /* size_t definition        */
#endif

#if PF_EXFAT_SUPPORT
typedef uint64_t        PF_FSIZE_T;
#else /* PF_EXFAT_SUPPORT */
typedef uint32_t        PF_FSIZE_T;
#endif /* PF_EXFAT_SUPPORT */


/*---------------------------------------------------------------------------
  off_t
 ----------------------------------------------------------------------------*/
#if !defined(off_t) && !defined(OFF_T_DEFINED)
#if !PF_STDLIB_ALIAS_SUPPORT && USE_EBINDER

  #include <sys/types.h>

#else /* PF_STDLIB_ALIAS_SUPPORT */

#if PF_EXFAT_SUPPORT
  typedef int64_t      off_t;
#else /* !PF_EXFAT_SUPPORT */
  typedef int32_t      off_t;
#endif /* !PF_EXFAT_SUPPORT */

#endif /* !PF_STDLIB_ALIAS_SUPPORT */
#endif /* #if !defined(off_t) && !defined(OFF_T_DEFINED) */

#if PF_EXFAT_SUPPORT
typedef int64_t        PF_OFF_T;
#else /* PF_EXFAT_SUPPORT */
typedef int32_t        PF_OFF_T;
#endif /* PF_EXFAT_SUPPORT */


/*---------------------------------------------------------------------------
  fpos_t
 ----------------------------------------------------------------------------*/
/* AMBA_FS_KAL */
#if (PF_STDLIB_ALIAS_SUPPORT == 0) /* !PF_STDLIB_ALIAS_SUPPORT */

#if defined AMBA_FS_KAL
#else
  #include <stdlib.h>
#endif

#else /* PF_STDLIB_ALIAS_SUPPORT */

#if PF_EXFAT_SUPPORT
  typedef uint64_t      fpos_t;
#else /* !PF_EXFAT_SUPPORT */
  typedef uint32_t      fpos_t;
#endif /* !PF_EXFAT_SUPPORT */

#endif /* !PF_STDLIB_ALIAS_SUPPORT */

#if PF_EXFAT_SUPPORT
typedef uint64_t        PF_FPOS_T;          /* fpos_t definition */
#else /* !PF_EXFAT_SUPPORT */
typedef uint32_t        PF_FPOS_T;          /* fpos_t definition */
#endif /* !PF_EXFAT_SUPPORT */


/*---------------------------------------------------------------------------
  Boolean
 ----------------------------------------------------------------------------*/
#ifndef PF_BOOL
#define PF_BOOL         e_bool_t
#endif

#ifndef PF_FALSE
#define PF_FALSE        e_false
#endif

#ifndef PF_TRUE
#define PF_TRUE         e_true
#endif


/*---------------------------------------------------------------------------
  Drive letter
 ----------------------------------------------------------------------------*/
#define PF_DRV_CHAR     PF_CHAR                 /* 8-bit integer            */


/*---------------------------------------------------------------------------
  Sector, Cluster
 ----------------------------------------------------------------------------*/
/* Sector number, Number of sector */
#define PF_SECT         PF_U_LONG

/* Cluster number, Number of cluster */
#define PF_CLST         PF_U_LONG


/*---------------------------------------------------------------------------
  NULL pointer
 ----------------------------------------------------------------------------*/
#ifndef NULL
  #define NULL                  ((void*)0)
#endif


#endif  /* PF_TYPES_H */
