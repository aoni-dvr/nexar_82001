#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaMisraFix.h>

void AmbaMisra_TypeCast32(void * pNewType, const void * pOldType)
{
    *(UINT32 *)pNewType = *(UINT32 *)pOldType;
}

void AmbaMisra_TypeCast(void * pNewType, const void * pOldType)
{
    AmbaMisra_TypeCast32(pNewType, pOldType);
}

void AmbaMisra_TouchUnused(void * pArg)
{
    return;
}

void AmbaAssert(void)
{
    return;
}

UINT32 AmbaUSB_IsrTrace_Push(UINT32 Tag, UINT32 Code, UINT32 Data0, UINT32 Data1, UINT32 Data2, UINT32 Data3, void * Parser)
{
    return 0;
}