#ifndef IK_DATATYPE_H
#define IK_DATATYPE_H
#include <stddef.h>

typedef signed char int8;
typedef unsigned char uint8;
typedef short  int16;
typedef unsigned short uint16;
typedef int  int32;
typedef unsigned   uint32;
typedef long long  int64;
typedef unsigned long long   uint64;
typedef long intptr;
typedef unsigned long uintptr;
typedef float float32;
typedef double double64;
//typedef unsigned int size_t;

#if !(defined(CONFIG_CC_USESTD) && defined(VCAST_AMBA))
typedef unsigned char uint8_t;
typedef short  int16_t;
typedef unsigned short uint16_t;
typedef unsigned   uint32_t;
typedef int  int32_t;
#else
#include <stdint.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define REGULAR_EXECUTE 0U
#define STAGE_1_FOR_EIS_EXECUTE 1U
#define STAGE_2_FOR_EIS_EXECUTE 2U

#define AMBA_OK             0x00000000U /* General OK */
#define AMBA_SSP_ERR_BASE   0x01000000U
#define AMBA_IK_ERR_BASE    (AMBA_SSP_ERR_BASE + 0x00100000U) /* 0x01000000 to 0x01ffffff: Reserved for SSP */

// success
#ifndef IK_OK
#define IK_OK AMBA_OK
#endif
// error 0xFFFF0
#ifndef IK_ERR_0000
#define IK_ERR_0000 (AMBA_IK_ERR_BASE)                  // General error
#endif
#ifndef IK_ERR_0001
#define IK_ERR_0001 (IK_ERR_0000 | 0x00000001U) // IK Arch un-init error
#endif
#ifndef IK_ERR_0002
#define IK_ERR_0002 (IK_ERR_0000 | 0x00000002U) // Specified ContextId un-init error
#endif
#ifndef IK_ERR_0003
#define IK_ERR_0003 (IK_ERR_0000 | 0x00000003U) // Invalid ContextId error
#endif
#ifndef IK_ERR_0004
#define IK_ERR_0004 (IK_ERR_0000 | 0x00000004U) // Invalid api for specified context ability error
#endif
#ifndef IK_ERR_0005
#define IK_ERR_0005 (IK_ERR_0000 | 0x00000005U) // Null pointer error
#endif
#ifndef IK_ERR_0006
#define IK_ERR_0006 (IK_ERR_0000 | 0x00000006U) // Invalid buffer alignment error. Size or address are not aligned to design document specified value
#endif
#ifndef IK_ERR_0007
#define IK_ERR_0007 (IK_ERR_0000 | 0x00000007U) // Invalid buffer size too small error
#endif
#ifndef IK_ERR_0008
#define IK_ERR_0008 (IK_ERR_0000 | 0x00000008U) // One or more parameters are out of valid range error
#endif
#ifndef IK_ERR_0009
#define IK_ERR_0009 (IK_ERR_0000 | 0x00000009U) // Image kernel Default Binary un-init error
#endif

// error 0xFFFF1
#ifndef IK_ERR_0100
#define IK_ERR_0100 (IK_ERR_0000 | 0x00000100U) // One or more mandatory filters are not set
#endif
#ifndef IK_ERR_0101
#define IK_ERR_0101 (IK_ERR_0000 | 0x00000101U) // Invalid window geometry error
#endif
#ifndef IK_ERR_0102
#define IK_ERR_0102 (IK_ERR_0000 | 0x00000102U) // Invalid CC table version
#endif
#ifndef IK_ERR_0103
#define IK_ERR_0103 (IK_ERR_0000 | 0x00000103U) // Invalid AAA setting
#endif
#ifndef IK_ERR_0104
#define IK_ERR_0104 (IK_ERR_0000 | 0x00000104U) // Wrong calculation on warp
#endif
#ifndef IK_ERR_0105
#define IK_ERR_0105 (IK_ERR_0000 | 0x00000105U) // Wrong calculation on ca
#endif
#ifndef IK_ERR_0106
#define IK_ERR_0106 (IK_ERR_0000 | 0x00000106U) // Wrong calculation on sbp
#endif
#ifndef IK_ERR_0107
#define IK_ERR_0107 (IK_ERR_0000 | 0x00000107U) // Wrong calculation on vig
#endif
#ifndef IK_ERR_0108
#define IK_ERR_0108 (IK_ERR_0000 | 0x00000108U) // Invalid anti-aliasing setting
#endif
#ifndef IK_ERR_0109
#define IK_ERR_0109 (IK_ERR_0000 | 0x00000109U) // Invalid wb gain setting
#endif
#ifndef IK_ERR_010A
#define IK_ERR_010A (IK_ERR_0000 | 0x0000010AU) // Invalid ce setting
#endif


#endif //DATATYPE_H

