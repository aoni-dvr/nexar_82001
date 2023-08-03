/*
 *  Dummy header when not using standard libraries from toolchain.
 */

#ifndef CONFIG_CC_USESTD
#ifndef STDIO_H
#define STDIO_H

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
#include <AmbaWrap.h>
#include <comsvc/misc/AmbaUtility.h>

// TODO: hacking http with non-std APIs.
extern int snprintf(char *str, size_t size, const char *format, ...);


#endif /* !STDIO_H */
#endif /* !CONFIG_CC_USESTD */

