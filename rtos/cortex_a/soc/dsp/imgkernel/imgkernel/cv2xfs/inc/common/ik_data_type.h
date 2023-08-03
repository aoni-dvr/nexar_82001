#ifndef IK_DATATYPE_H
#define IK_DATATYPE_H

#ifndef AMBINT_H_FILE
typedef signed char int8;
typedef unsigned char uint8;
typedef short  int16;
typedef unsigned short uint16;
typedef int  int32;
typedef unsigned   uint32;
typedef long long  int64;
typedef unsigned long long   uint64;
#endif

typedef long intptr;
typedef unsigned long uintptr;
typedef float float32;
typedef double double64;
//__QNX__ //IK QNX compiled symbol.
//EARLYTEST_ENV //IK early-test symbol.
#if (!defined(__QNX__) && (defined(__unix__) || defined(EARLYTEST_ENV)))
#include <stddef.h>
#endif

#if !(defined(CONFIG_CC_USESTD) && defined(VCAST_AMBA))

#else
#include <stdint.h>
#endif

#ifndef TRUE
#define TRUE ((uint8)0x1U)
#endif

#ifndef FALSE
#define FALSE ((uint8)0x0U)
#endif

#define AMBA_OK             0x00000000UL /* General OK */
#define AMBA_SSP_ERR_BASE   0x01000000UL
#define AMBA_IK_ERR_BASE    (AMBA_SSP_ERR_BASE + 0x00100000UL) /* 0x01000000 to 0x01ffffff: Reserved for SSP */

// success
#ifndef IK_OK
#define IK_OK AMBA_OK
#endif
// error 0xFFFF0
#ifndef IK_ERR_0000
#define IK_ERR_0000 (AMBA_IK_ERR_BASE)                  // General error
#endif
#ifndef IK_ERR_0001
#define IK_ERR_0001 (IK_ERR_0000 | 0x00000001UL) // IK Arch un-init error
#endif
#ifndef IK_ERR_0002
#define IK_ERR_0002 (IK_ERR_0000 | 0x00000002UL) // Specified ContextId un-init error
#endif
#ifndef IK_ERR_0003
#define IK_ERR_0003 (IK_ERR_0000 | 0x00000003UL) // Invalid ContextId error
#endif
#ifndef IK_ERR_0004
#define IK_ERR_0004 (IK_ERR_0000 | 0x00000004UL) // Invalid api for specified context ability error
#endif
#ifndef IK_ERR_0005
#define IK_ERR_0005 (IK_ERR_0000 | 0x00000005UL) // Null pointer error
#endif
#ifndef IK_ERR_0006
#define IK_ERR_0006 (IK_ERR_0000 | 0x00000006UL) // Invalid buffer alignment error. Size or address are not aligned to design document specified value
#endif
#ifndef IK_ERR_0007
#define IK_ERR_0007 (IK_ERR_0000 | 0x00000007UL) // Invalid buffer size too small error
#endif
#ifndef IK_ERR_0008
#define IK_ERR_0008 (IK_ERR_0000 | 0x00000008UL) // One or more parameters are out of valid range error
#endif
#ifndef IK_ERR_0009
#define IK_ERR_0009 (IK_ERR_0000 | 0x00000009UL) // Image kernel Default Binary un-init error
#endif

// error 0xFFFF1
#ifndef IK_ERR_0100
#define IK_ERR_0100 (IK_ERR_0000 | 0x00000100UL) // One or more mandatory filters are not set
#endif
#ifndef IK_ERR_0101
#define IK_ERR_0101 (IK_ERR_0000 | 0x00000101UL) // Invalid window geometry error
#endif
#ifndef IK_ERR_0102
#define IK_ERR_0102 (IK_ERR_0000 | 0x00000102UL) // Invalid CC table version
#endif
#ifndef IK_ERR_0103
#define IK_ERR_0103 (IK_ERR_0000 | 0x00000103UL) // Invalid AAA setting
#endif
#ifndef IK_ERR_0104
#define IK_ERR_0104 (IK_ERR_0000 | 0x00000104UL) // Wrong calculation on warp
#endif
#ifndef IK_ERR_0105
#define IK_ERR_0105 (IK_ERR_0000 | 0x00000105UL) // Wrong calculation on ca
#endif
#ifndef IK_ERR_0106
#define IK_ERR_0106 (IK_ERR_0000 | 0x00000106UL) // Wrong calculation on sbp
#endif
#ifndef IK_ERR_0107
#define IK_ERR_0107 (IK_ERR_0000 | 0x00000107UL) // Wrong calculation on vig
#endif
#ifndef IK_ERR_0108
#define IK_ERR_0108 (IK_ERR_0000 | 0x00000108UL) // Invalid anti-aliasing setting
#endif
#ifndef IK_ERR_0109
#define IK_ERR_0109 (IK_ERR_0000 | 0x00000109UL) // Invalid wb gain setting
#endif
#ifndef IK_ERR_010A
#define IK_ERR_010A (IK_ERR_0000 | 0x0000010AUL) // Invalid ce setting
#endif

// error 0xFFFF2
#ifndef IK_ERR_0200
#define IK_ERR_0200 (IK_ERR_0000 | 0x00000200UL) // General Safety error
#endif
#ifndef IK_ERR_0201
#define IK_ERR_0201 (IK_ERR_0000 | 0x00000201UL) // Safety error arch-init fail
#endif
#ifndef IK_ERR_0203
#define IK_ERR_0203 (IK_ERR_0000 | 0x00000203UL) // Safety error context memory fence error
#endif
#ifndef IK_ERR_0204
#define IK_ERR_0204 (IK_ERR_0000 | 0x00000204UL) // Safety error CR and flow_tbl memory fence error
#endif
#ifndef IK_ERR_0205
#define IK_ERR_0205 (IK_ERR_0000 | 0x00000205UL) // Safety error ring buffer error
#endif
#ifndef IK_ERR_0206
#define IK_ERR_0206 (IK_ERR_0000 | 0x00000206UL) // Safety error default binary error
#endif
#ifndef IK_ERR_0207
#define IK_ERR_0207 (IK_ERR_0000 | 0x00000207UL) // Safety error system API error
#endif


#endif //DATATYPE_H

