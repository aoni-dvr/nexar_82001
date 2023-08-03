#ifndef AMBA_IOUTIL_CRC32_HW_H
#define AMBA_IOUTIL_CRC32_HW_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

void IO_UTIL_CRC32X(UINT32 *crc, UINT64 value);
void IO_UTIL_CRC32W(UINT32 *crc, UINT32 value);
void IO_UTIL_CRC32H(UINT32 *crc, UINT16 value);
void IO_UTIL_CRC32B(UINT32 *crc, UINT8 value);

UINT32 IO_UtilityCrc32Hw(const UINT8 *pBuffer, UINT32 Size);
UINT32 IO_UtilityCrc32AddHw(const UINT8 *pBuffer, UINT32 Size, UINT32 Crc);

#endif