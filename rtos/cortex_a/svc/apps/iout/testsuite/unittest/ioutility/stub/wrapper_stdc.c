#include <test_group.h>
#include <AmbaTypes.h>
#include <AmbaWrap.h>

UINT32 flag_memcpy_fail = 0;

UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if (flag_memcpy_fail == 0U) {
        memcpy(pDst, pSrc, num);
        return 0;
    } else {
        return -1;
    }
}

UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n)
{
    memset(ptr, v, n);
    return 0;
}