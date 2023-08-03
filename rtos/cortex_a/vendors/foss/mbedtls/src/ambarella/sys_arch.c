
#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
#include <AmbaWrap.h>
#include <AmbaUART.h>
#include <AmbaUART_Def.h>
#include <AmbaPrint.h>
#include <config.h>

void mbedtls_sys_exit(int status)
{
#if defined(CONFIG_ARM64) || defined(CONFIG_CPU_CORTEX_R52)
    (void) status;
#else
    AmbaPrint_PrintInt5("mbedtls exit(%d)", status, 0, 0, 0, 0);
#endif
}

int mbedtls_sys_printf(const char *fmt, ...)
{
#if defined(CONFIG_CC_USESTD) && !defined(CONFIG_QNX)
    UINT8 Buf[512];
    va_list args;
    INT32 n;
    UINT32 SentSize;

    va_start(args, fmt);
    n = vsnprintf((char *)Buf, 512-3, (const char *)fmt, args);
    Buf[n] = (UINT8)'\r';
    Buf[n+1] = (UINT8)'\n';
    Buf[n+2] = (UINT8)'\0';
    va_end(args);
    (void) AmbaUART_Write(0U, 0U, n + 3, Buf, &SentSize, 1U);
    return n;
#else
#if defined(CONFIG_ARM64) || defined(CONFIG_CPU_CORTEX_R52)
    (void) fmt;
#else
    AmbaPrint_PrintStr5("mbedtls printf(%s)", fmt, NULL, NULL, NULL, NULL);
#endif
    return 0;
#endif
}

int mbedtls_sys_snprintf(char *str, size_t size, const char *format, ...)
{
    (void)str;
    (void)size;
    (void)format;
    return 0;
}
