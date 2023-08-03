
#include <stdio.h>
#include <stdint.h>

#include <sys/mman.h>

#include <AmbaTypes.h>
#include <AmbaWrap.h>
#include <AmbaKAL.h>

#include <AmbaLinkPrivate.h>


#if 0
#define IpcDebug printf
#else
#define IpcDebug(fmt, ...)
#endif

/*
 * mmap information for RPMSG buffers and descriptors
 */
static void* pVAddrStart = NULL;    /* start address of mapping zone */
static UINT64 PAddrStart = 0;       /* start address of mapping zone, need 128B alignment */
static INT64 Offset = 0;            /* offset between real PAddrStart and request one */

UINT32 IpcMmap(UINT64 PAddr, UINT64 MMapSize)
{
    if (pVAddrStart != NULL) {
        fprintf(stderr, "%s %d already mapped\n", __func__, __LINE__);
        return 0;
    }
    /* start addr for mmap_device_memory should be multiple of 128 */
    PAddrStart = (PAddr / 128U) * 128U;
    Offset = (PAddr - PAddrStart);
    MMapSize += Offset;

    pVAddrStart = mmap_device_memory(NULL, MMapSize,
            PROT_READ | PROT_WRITE | PROT_NOCACHE,
            0,
            PAddrStart);

    if (pVAddrStart == MAP_FAILED) {
        fprintf(stderr, "%s %d mmap_device_memory() failed\n", __func__, __LINE__);
        return (UINT32)-1;
    }

    return 0;
}

void *IpcVirtToPhys(void *pVAddr)
{
    void *pPAddr;

    /* TODO: arithmetic overflow AND input check */
    pPAddr = (void *)((uintptr_t)pVAddr - (uintptr_t)pVAddrStart - Offset + PAddrStart);

    return pPAddr;
}

void *IpcPhysToVirt(void *pPAddr)
{
    void *pVAddr;

    /* TODO: arithmetic overflow AND input check */
    pVAddr = (void *)((uintptr_t)pVAddrStart + (((uintptr_t)pPAddr - PAddrStart) + Offset));

    return pVAddr;
}


static UINT64 *IpcSPPhysToVirt(UINT64 Addr)
{
    static UINT32 mmaped = 0;
    static void* pVAddrStart_SP = NULL;    /* start address of mapping zone */
    static UINT64 PAddrStart_SP = 0;       /* start address of mapping zone, need 128B alignment */
    static INT64 Offset_SP = 0;            /* offset between real PAddrStart and request one */

    if (mmaped == 0) {
        UINT64 PAddr = AHB_SCRATCHPAD_BASE;
        UINT64 MMapSize = 0x300;

        PAddrStart_SP = (PAddr / 128U) * 128U;
        Offset_SP = (PAddr - PAddrStart_SP);
        MMapSize += Offset_SP;

        pVAddrStart_SP = mmap_device_memory(NULL, MMapSize,
                PROT_READ | PROT_WRITE | PROT_NOCACHE,
                0,
                PAddrStart_SP);

        if (pVAddrStart == MAP_FAILED) {
            fprintf(stderr, "%s %d mmap_device_memory() failed\n", __func__, __LINE__);
            return 0;
        } else {
            mmaped = 1;
        }
    }

    return (UINT64 *)((uintptr_t)pVAddrStart_SP + ((Addr - PAddrStart_SP) + Offset_SP));
}


void IpcSPWriteBit(const ULONG Addr, const UINT32 bit)
{
    const UINT32 Value = (UINT32)((UINT32)1U << (UINT32)bit);
    UINT64 *pAddr;

    pAddr = IpcSPPhysToVirt(Addr);
    out32((uintptr_t)pAddr, Value);
    IpcDebug("IPC %s(%d) 0x%08x 0x%08x\n", __func__, __LINE__, Addr, Value);
}

