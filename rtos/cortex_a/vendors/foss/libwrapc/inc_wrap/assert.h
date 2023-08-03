#ifdef CONFIG_CC_USESTD
#error "This is pseudo header when -nostdinc"
#else

#ifndef AMBA_STDWRAP_ASSERT_H
#define AMBA_STDWRAP_ASSERT_H

#undef assert
#ifdef NDEBUG
#define assert(s)   ((void)0)
#else
#define assert(s)   { if ((s)) { while (1) {;} } }
#endif

#endif // AMBA_STDWRAP_ASSERT_H

#endif // CONFIG_CC_USESTD

