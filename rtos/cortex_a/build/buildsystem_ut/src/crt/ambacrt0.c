#include <stddef.h>

/* These magic symbols are provided by the linker.  */
//lint -e9067
extern void (*__preinit_array_start []) (void) __attribute__((weak));
//lint -e9067
extern void (*__preinit_array_end []) (void) __attribute__((weak));
//lint -e9067
extern void (*__init_array_start []) (void) __attribute__((weak));
//lint -e9067
extern void (*__init_array_end []) (void) __attribute__((weak));

//lint -e9067
extern void (*__fini_array_start []) (void) __attribute__((weak));
//lint -e9067
extern void (*__fini_array_end []) (void) __attribute__((weak));

#if defined(__aarch64__)
extern void *eh_frame_start __attribute__((weak));
extern void __register_frame (void *begin) __attribute__((weak));
extern void __deregister_frame (void *begin) __attribute__((weak));
#endif


void _init(void) __attribute__((weak));
void _init(void)
{
}


/* Iterate over all the init routines.  */
void libc_init_array (void) __attribute__((weak));
void libc_init_array (void)
{
    size_t count;
    size_t i;

#if defined(__aarch64__)
    /* cf. comments in gcc/libgcc/unwind-dw2-fde.c */
    /*
     * Set earlier, some constructur would have exception handler.
     */
    if (__register_frame != NULL) {
        __register_frame(&eh_frame_start);
    }
#endif // defined(__aarch64__)

    //lint -e732, -e946, -e947, -e9034
    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++) {
        __preinit_array_start[i] ();
    }

#ifdef HAVE_INIT_FINI
    _init ();
#endif

    //lint -e732, -e946, -e947, -e9034
    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++) {
        __init_array_start[i] ();
    }
}

void _fini(void) __attribute__((weak));
void _fini(void)
{
    size_t count;
    size_t i;

    count = __fini_array_end - __fini_array_start;
    for (i = 0; i < count; i++) {
        __fini_array_start[i] ();
    }

#if defined(__aarch64__)
    if (__deregister_frame != NULL) {
        __deregister_frame(&eh_frame_start);
    }
#endif // defined(__aarch64__)
}


#if 0
unsigned long __dso_handle __attribute__((weak));
unsigned long __dso_handle = 0;


/* Test argument */
int g_argc = 0;
char *g_argv[1] = { '\0' };

/*
 * Sample for section ".preinit_array"
 */
void test_preinit(void);
void test_preinit(void)
{
    static size_t preinit = 1;
    (void)preinit;
}
__attribute__((section(".preinit_array"))) __attribute__((unused)) static void (*y[])(void) = { &test_preinit };

/*
 * Sample for section ".init_array"
 */
void test_init(void) __attribute__((constructor));
void test_init(void)
{
    static size_t init = 2;
    (void)init;
}

/*
 * Sample for section ".fini_array"
 */
void test_fini(void) __attribute__((destructor));
void test_fini(void)
{
    static size_t fini = 3;
    (void)fini;
}
#endif

