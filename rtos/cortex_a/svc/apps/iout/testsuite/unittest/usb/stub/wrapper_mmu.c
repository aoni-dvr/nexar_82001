#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaMMU.h>

UINT32 AmbaMMU_Virt32ToPhys32(UINT32 VirtAddr, UINT32 *pPhysAddr)
{
    *pPhysAddr = VirtAddr;
    return 0;
}
UINT32 AmbaMMU_Phys32ToVirt32(UINT32 PhysAddr, UINT32 *pVirtAddr)
{
    *pVirtAddr = PhysAddr;
    return 0;
}