#include <AmbaWrap.h>
#include <AmbaVer.h>

#include <RefBuild.h>


typedef int INT;
extern INT main(void);


static void putstr(const char *s)
{
    const char nullstr[] = "<null>";
    const char *p;

    if (s == NULL) {
        p = nullstr;
    } else {
        p = s;
    }

    while (*p != '\0') {
        if (*p == '\n') {
            semi_putchar('\r');
            semi_putchar('\n');
        } else {
            semi_putchar(*p);
        }
        p++;
    }
}


INT main(void)
{
    char a[6] = { 'T', 'e', 's', 't', '\n', '\0' };
    char b[6] = { 'F', 'i', 'n', 'e', '\n', '\0' };
    LONG c;
    char *pC;   // For MISRA C.

    putstr("Orig:\t");
    putstr(a);
    c =  0x78; // 'x'
    (void) AmbaWrap_memset(a, c, 4);
    putstr("memset:\t");
    putstr(a);
    (void) AmbaWrap_memcpy(a, b, 4);
    putstr("memcpy:\t");
    putstr(a);

    putstr("In a char: ");
    c = semi_getchar();
    putstr("\n");
    if (c != -1) {
        pC = (char *)&c;
        pC[1] = '\n';
        pC[2] = '\0';
        putstr("get:\t");
        putstr(pC);
    } else {
        putstr("Get failed\n");
    }

    {
        extern void AmbaLibWrap_Std_GetVerInfo(AMBA_VerInfo_s *pVerInfo);

        AMBA_VerInfo_s v;

        AmbaLibWrap_Std_GetVerInfo(&v);

        putstr("Ver: ");
        putstr(v.Ver);
        putstr("\n");
    }
    {

        extern UINT32 test_libwrap_std(void);

        UINT32 err;

        err = test_libwrap_std();
        if (err == 0U) {
            putstr("test_libwrap_std OK\n");
        } else {
            putstr("test_libwrap_std NG\n");
        }
    }

#ifdef CONFIG_CC_CXX_SUPPORT
    {
        extern void AmbaCRT0(void);
        extern void cpp_test(void);

#if 0
        // crt/ambacrt0.c has done during booting.
        AmbaCRT0();
#endif
        cpp_test();
    }
#endif /* CONFIG_CC_CXX_SUPPORT */

    return 0;
}

#ifndef CONFIG_CC_USESTD
extern struct _reent *_impure_ptr;
extern struct _reent *__getreent(void);

struct _reent *__getreent(void)
{
    return _impure_ptr;
}
#endif // CONFIG_CC_USESTD

