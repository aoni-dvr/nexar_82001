
/* STD */
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

/* mbed TLS Buffer Allocator
 *   https://tls.mbed.org/kb/how-to/using-static-memory-instead-of-the-heap
 *     unsigned char memory_buf[100000];
 *     mbedtls_memory_buffer_alloc_init( memory_buf, sizeof(memory_buf) );
 */
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C

/* snprintf */
typedef __SIZE_TYPE__ size_t;
extern int snprintf(char *str, size_t size, const char *format, ...);
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf

/* Debug On/Off */
#if 1
#include <stdarg.h>
extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#else
#undef MBEDTLS_DEBUG_C
#endif

/* printf */
extern int mbedtls_sys_printf(const char *fmt, ...);
#define MBEDTLS_PLATFORM_PRINTF_MACRO mbedtls_sys_printf

/* exit */
extern void mbedtls_sys_exit(int status);
#define MBEDTLS_PLATFORM_EXIT_MACRO mbedtls_sys_exit

/* filesystem */
#undef MBEDTLS_FS_IO

/* Entropy: register mbedtls_entropy_add_source() at runtime */
#define MBEDTLS_NO_PLATFORM_ENTROPY

/* Networking: use mbedtls_ssl_set_bio() instead */
#undef MBEDTLS_NET_C

/* Timing: mbedtls_ssl_set_timer_cb() for DTLS */
#undef MBEDTLS_TIMING_C
#undef MBEDTLS_HAVE_TIME
#undef MBEDTLS_HAVE_TIME_DATE

/* Hardware Acceleration: ARMv8 Crypto Extensions for AES and GCM */
//#define MBEDTLS_ARMV8CE_AES_C

