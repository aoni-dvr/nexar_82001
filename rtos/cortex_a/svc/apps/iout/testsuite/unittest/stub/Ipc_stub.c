#include "AmbaTypes.h"

static UINT32 NoDummy_IpcPhysToVirt  = 0U;
static UINT32 NoDummy_IpcVirtToPhys  = 0U;
void Set_NoDummy_IpcPhysToVirt(UINT32 NoDummy)
{
    NoDummy_IpcPhysToVirt = NoDummy;
}
void Set_NoDummy_IpcVirtToPhys(UINT32 NoDummy)
{
    NoDummy_IpcVirtToPhys = NoDummy;
}

UINT32 ipc_buf[1];

void IpcDebug(const char *fmt, ...)
{

}

void IpcCacheClean(const void *pAddr, UINT32 Size)
{

}

void *IpcPhysToVirt(void *pPAddr)
{
    if (NoDummy_IpcPhysToVirt != 0U) {
        return ipc_buf;
    }
}

void *IpcVirtToPhys(void *pVAddr)
{

}

void IpcSPWriteBit(const ULONG Addr, const UINT32 bit)
{

}