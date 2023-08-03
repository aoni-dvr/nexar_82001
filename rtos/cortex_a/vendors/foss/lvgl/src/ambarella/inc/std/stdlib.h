/*
 *  Dummy header when not using standard libraries from toolchain.
 */

//#ifndef CONFIG_CC_USESTD
#ifndef STDLIB_H
#define STDLIB_H

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
#include <AmbaWrap.h>
#include <AmbaUtility.h>

inline static int rand(void)
{
    int v;

    (void)AmbaWrap_rand(&v);

    return v;
}

inline static int atoi(const char *nptr)
{
    UINT32 v = 0;

    (void)AmbaUtility_StringToUInt32(nptr, &v);

    return (int)v;
}

#endif /* !STDLIB_H */
//#endif

