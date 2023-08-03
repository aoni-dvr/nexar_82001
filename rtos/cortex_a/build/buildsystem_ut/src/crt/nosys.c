/*
 * Need to run Qemu with "-semihosting"
 *
 * Semihosting SVCs
 * cf. https://developer.arm.com/docs/100863/latest/semihosting-operations
 *
 * Simple implementation of libnosys.a APIs by using semihosting.
 * cf. newlib/libc/include/reent.h, libgloss/libnosys/ of newlib
 */

//#include <stddef.h>
#ifndef NULL
#ifndef __cplusplus
#define NULL ((void *)0)
#else   /* C++ */
#define NULL 0
#endif  /* C++ */
#endif

//#include <fcntl.h>
#define O_RDONLY        (0U)
#define O_WRONLY        (1U)
#define O_RDWR          (2U)
#define O_APPEND        (0x00008U) // _FAPPEND
#define O_BINARY        (0x10000U) //_FBINARY

#include <AmbaWrap.h>
#include <RefBuild.h>


#ifndef INT
#define INT int
#endif
#ifndef UINT
#define UINT unsigned int
#endif

void _exit(INT status);
INT _fstat(INT fd, void *stat);
INT _open(const char *file, INT flags, INT mode);
INT _close(INT fd);
INT _isatty(INT file);
INT _lseek(INT file, INT ptr, INT dir);
INT _write(INT file, char *ptr, UINT len);
INT _read(INT file, char *ptr, UINT len);
void * _sbrk(UINT incr);


//#include <sys/time.h>
//#define time_t      ULONG
#define suseconds_t ULONG

struct timeval {
    ULONG        tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
};

struct timezone {
    LONG tz_minuteswest;     /* minutes west of Greenwich */
    LONG tz_dsttime;         /* type of DST correction */
};

INT _gettimeofday(struct timeval *tp, const void *tzvp);
INT _link(char *existing, char *new);
INT _unlink(char *name);
INT _getpid(void);
INT _kill(INT pid, INT sig);

/* clock_t _time (struct tms *buf) */
LONG _times(void *buf);


void _exit(INT status)
{
    (void)status;
    {
        void _fini(void);

        _fini();
    }
    (void)semi_AppExit();

    /* noreturn API */
//    while (1) { ; }
}


/* int _fstat(int fd, struct stat *st) */
INT _fstat(INT fd, void *stat)
{
    (void)fd;
    (void)stat;

    return 0;
}


INT _open(const char *file, INT flags, INT mode)
{
    ULONG f = 0U;
    // ISO C fopen mode, default as O_RDONLY
    ULONG imode = O_RDONLY;

    // TODO: mode
    (void)mode;
    (void) AmbaWrap_memcpy(&f, &flags, sizeof(LONG));
    /* Check newlib/newlib/libc/stdio/flags.c */
    if ((f & O_APPEND) != 0U) {
        imode |= 0x08U;
    } else if ((f & O_WRONLY) != 0U) {
        imode |= 0x04U;
    } else {
        /* MISRA */
        ;
    }
#ifdef O_BINARY
    if ((f & O_BINARY) != 0U) {
        imode |= 0x01U;
    }
#endif
    if ((f & O_RDWR) != 0U) {
        imode |= 0x02U;
    }

    return semi_open(file, imode);
}


INT _close(INT fd)
{
    LONG file_handle = fd;

    return semi_close(file_handle);
}


INT _isatty(INT file)
{
    LONG f = file;

    return semi_isatty(f);
}

/* cf. sys/unistd.h, stdio.h */
#define MY_SEEK_SET    (0)
#define MY_SEEK_CUR    (1)
#define MY_SEEK_END    (2)


INT _lseek(INT file, INT ptr, INT dir)
{
    INT err = 0;
    long file_l = file;

    switch (dir) {
    case MY_SEEK_CUR:
        // TODO: Need FS to record
        break;
    case MY_SEEK_END:
        err = semi_flen(file_l);
        if (err >= 0) {
            ptr += err;
        } else {
            err = -1;
        }
        break;
    case MY_SEEK_SET:
        // TODO: Need FS to record
        break;
    default:
        err = -1;
        break;
    }

    if (err == 0) {
        err = semi_seek(file_l, ptr);
    }

    return err;
}


INT _write(INT file, char *ptr, UINT len)
{
    INT err = -1;
    ULONG length = len;

    if ((ptr == NULL) || (length == 0U)) {
        err = -1;
    } else {
        /*
         * sys/unistd.h, STDIN_FILENO(1), STDERR_FILENO(2), using WRITE0
         * But WRITE0 checks only '\0', it would be issue of buffer-overflow.
         * And '\n' is not treated as '\r\n'.
         */
        if ((file == 1) || (file == 2)) {
            UINT i;
            for (i = 0; i < len; i++) {
                semi_putchar(ptr[i]);
                if (ptr[i] == '\n') {
                    semi_putchar('\r');
                }
            }
            err = len;
        } else {
            err = semi_write(file, ptr, length);
#if 1 // QEMU bug?! cf. qemu/semihosting/arm-compat-semi.c, common_semi_cb()
            if (err < 0) {
                err = -1;
            }
#else // by ARM Semihosting spec.
            if (err == 0) {
                /* success */
                err = len;
            } else {
                /* The number of bytes that are not written, if there is an error. */
                err = len - err;
            }
#endif
        }
    }

    return err;
}


INT _read(INT file, char *ptr, UINT len)
{
    INT err = 0;
    ULONG length = len;

    if ((ptr == NULL) || (length == 0U)) {
        err = -1;
    } else {
        err = semi_read(file, ptr, length);
        if (err < 0) {
            // TODO: set errno
            err = 0;
        }
    }
    return err;
}


/* TODO: for 64b, to fix sizeof(); but now only use 32-bit address */
void * _sbrk(UINT incr)
{
    extern char __sbrk_start;  /* Set by linker.  */
    extern char __sbrk_end;    /* Set by linker.  */

    static ULONG heap_end = 0U;
    static ULONG sbrk_end = 0U;
    void * prev_heap_end = NULL;
    ULONG v;

    if (heap_end == 0U) {
        const void *p = &__sbrk_start;
        (void) AmbaWrap_memcpy(&heap_end, &p, sizeof(LONG));
        p = &__sbrk_end;
        (void) AmbaWrap_memcpy(&sbrk_end, &p, sizeof(LONG));
    }
    (void) AmbaWrap_memcpy(&prev_heap_end, &heap_end, sizeof(LONG));

    v = heap_end + incr;
    if (v < sbrk_end) {
        heap_end = v;
    } else {
        while (heap_end != 0U) { ; }
    }

    return prev_heap_end;
}



INT _gettimeofday(struct timeval *tp, const void *tzvp)
{
    struct timezone *tzp;

    (void) AmbaWrap_memcpy(&tzp, &tzvp, sizeof(void *));

    if (tp != NULL) {
        tp->tv_sec = semi_time();
        tp->tv_usec = 0;
    }

    /* Return fixed data for the time zone. */
    if (tzp != NULL)
    {
        tzp->tz_minuteswest = 0;
        tzp->tz_dsttime = 0;
    }

    return 0;
}


INT _link(char *existing, char *new)
{
    (void)existing;
    (void)new;

    return -1;
}

INT _unlink(char *name)
{
    (void)name;

    return -1;
}

INT _getpid(void)
{
    return -1;
}

INT _kill(INT pid, INT sig)
{
    (void)pid;
    (void)sig;

    return -1;
}


/* 'void' should be 'struct tms' */
/* clock_t _time (struct tms *buf) */
LONG _times(void *buf)
{
    (void)buf;

    return -1;
}

