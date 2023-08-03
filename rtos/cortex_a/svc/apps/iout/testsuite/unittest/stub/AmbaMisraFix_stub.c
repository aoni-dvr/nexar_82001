#include "AmbaTypes.h"
#include "AmbaDef.h"
#include <stdlib.h>
#include <string.h>

static UINT32 NoCopy_AmbaMisra_TypeCast = 0U;  /* 1U: directly return */
void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy)
{
    NoCopy_AmbaMisra_TypeCast = NoCopy;
}

static UINT64 *safe_buffer;

static void safe_buffer_cpy(void * pDst)
{
    UINT64 *pDes = pDst;
    UINT64 *pSrc;

    if(safe_buffer == NULL) {
        safe_buffer = malloc(sizeof(UINT64)*12);
    }
    pSrc = (UINT64)&safe_buffer;
    *pDes = *pSrc;
    return;
}

void AmbaMisra_TypeCast(void * pNewType, const void * pOldType)
{
    if (NoCopy_AmbaMisra_TypeCast == 2U) {
        /* Because accessing source addr causes segment fault,
         * allocating other safe memory for gcovr test.
         */
        safe_buffer_cpy(pNewType);
    } else if (NoCopy_AmbaMisra_TypeCast == 0U) {
        memcpy(pNewType, pOldType, sizeof(void*));
    }
}

void AmbaMisra_TypeCast32(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, 4);
}

void AmbaMisra_TypeCast64(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, 8);
}

void AmbaMisra_VolatileCast32(volatile void *pNewType, volatile const void *pOldType)
{

}

void AmbaMisra_TouchUnused(void * pArg)
{

}
