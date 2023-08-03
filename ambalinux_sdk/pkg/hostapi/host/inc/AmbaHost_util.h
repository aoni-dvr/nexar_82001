#ifndef __AMBAHOST_UTIL_H__
#define __AMBAHOST_UTIL_H__

#define DEBUG

#define EMSG(fmt,...)   AmbaHost_printf(fmt, ##__VA_ARGS__)
#ifdef  DEBUG
#define DMSG            EMSG
#else
#define DMSG(fmt,...)
#endif


int AmbaHost_printf(const char *format, ...);

#endif //__AMBAHOST_UTIL_H__
