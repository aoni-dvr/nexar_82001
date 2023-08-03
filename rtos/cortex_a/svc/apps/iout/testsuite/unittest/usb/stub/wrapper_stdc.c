#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaWrap.h>

UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    memcpy(pDst, pSrc, num);
    return 0;
}

UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n)
{
    memset(ptr, v, n);
    return 0;
}