
#ifndef UTIL_SOCKET_H
#define UTIL_SOCKET_H

#include "AmbaTypes.h"
#include "RefFlow_Common.h"
#include "Util_Common.h"

#define UTIL_SOCKET_CLINET_BUB (0xBABEBEEFU)
#define UTIL_SOCKET_CLINET_CARLA (314159U)
#define UTIL_SOCKET_CLINET_DBG (0x0F1E2D3CU)

UINT32 UtilSocket_ClientConnect(void);
UINT32 UtilSocket_ClientSend(const RF_LOG_HEADER_s *pLogHeader);
UINT32 UtilSocket_ClientClose(void);


#endif
