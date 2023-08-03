#ifndef AMBA_OPENAMP_H
#define AMBA_OPENAMP_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_CC_USESTD

extern long unsigned int StdWrap_strlen(const char *s);
#define strlen		StdWrap_strlen

extern char *StdWrap_strncpy(char *dest, const char *src, long unsigned int  n);
#define strncpy		StdWrap_strncpy

extern unsigned long int StdWrap_strtoul(const char *nptr, char **endptr, int base);
#define strtoul		StdWrap_strtoul

extern int StdWrap_strcmp(const char *s1, const char *s2);
#define strcmp		StdWrap_strcmp

extern int StdWrap_strncmp(const char *s1, const char *s2i, long unsigned int n);
#define strncmp		StdWrap_strncmp

extern void *StdWrap_memset(void *s, int c, long unsigned int n);
#define memset		StdWrap_memset

extern int StdWrap_memcmp(const void *s1, const void *s2, long unsigned int n);
#define memcmp		StdWrap_memcmp

extern void *StdWrap_memcpy(const void *s1, const void *s2, long unsigned int n);
#define memcpy		StdWrap_memcpy

#endif // CONFIG_CC_USESTD

#ifdef __cplusplus
}
#endif
#endif // AMBA_OPENAMP_H

