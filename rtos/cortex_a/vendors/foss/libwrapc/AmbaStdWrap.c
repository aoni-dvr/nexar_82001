
#if 1 // ndef CONFIG_CC_USESTD

#include <AmbaTypes.h>
#include <AmbaWrap.h>
#include <AmbaUtility.h>

long unsigned int StdWrap_strlen(const char *s)
{
    return AmbaUtility_StringLength(s);
}

char *StdWrap_strncpy(char *dest, const char *src, long unsigned int n)
{
    if ((dest != NULL) && (src != NULL)) {
        AmbaUtility_StringCopy(dest, n, src);
        return dest;
    }

    return NULL;
}

unsigned long int StdWrap_strtoul(const char *nptr, char **endptr, int base)
{
    UINT32 v = 0;
    UINT32 err;

    if ((base != 0) && (base != 10) && (base != 16)) {
        /* ULONG_MAX */
        return (unsigned long int)(-1);
    }

    if (nptr == NULL) {
        /* ULONG_MAX */
        return (unsigned long int)(-1);
    }

    (void)endptr;
    err = AmbaUtility_StringToUInt32(nptr, &v);
    if (err) {
        /* ULONG_MAX */
        return (unsigned long int)(-1);
    } else {
        return v;
    }
}

int StdWrap_strcmp(const char *s1, const char *s2)
{
    return AmbaUtility_StringCompare(s1, s2, AmbaUtility_StringLength(s1));
}

int StdWrap_strncmp(const char *s1, const char *s2, long unsigned int n)
{
    UINT32 l = AmbaUtility_StringLength(s1);

    if (l > n) {
        l = n;
    }
    return AmbaUtility_StringCompare(s1, s2, l);
}

void *StdWrap_memset(void *s, int c, long unsigned int n)
{
    (void)AmbaWrap_memset(s, c, n);
    return s;
}

int StdWrap_memcmp(const void *s1, const void *s2, long unsigned int n)
{
    INT32 v;

    (void)AmbaWrap_memcmp(s1, s2, n, &v);
    return v;
}

void *StdWrap_memcpy(const void *s1, const void *s2, long unsigned int n)
{
    void *s = (void *)s1;
    (void)AmbaWrap_memcpy((void *)s1, (void *)s2, n);

    return s;
}

#endif // CONFIG_CC_USESTD

