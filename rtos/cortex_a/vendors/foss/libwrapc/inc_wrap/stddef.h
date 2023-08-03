#ifdef CONFIG_CC_USESTD
#error "This is pseudo header when -nostdinc"
#else

#ifndef AMBA_STDWRAP_STDDEF_H
#define AMBA_STDWRAP_STDDEF_H

/* Suppose it's defined at some file. */
#undef NULL
#ifndef __cplusplus
#define NULL ((void *)0)
#else   /* Would be C++ */
#define NULL 0
#endif  /* C/C++ */

#ifndef offsetof
#define offsetof(DataType, Member) ((unsigned int) &(((DataType *) 0)->Member))
#endif

#endif // AMBA_STDWRAP_STDDEF_H

#endif // CONFIG_CC_USESTD

