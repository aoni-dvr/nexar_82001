
/*
 * Need to run Qemu with "-semihosting"
 *
 * Semihosting SVCs
 * cf. http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0205g/Bacdagge.html
 * cf. https://developer.arm.com/docs/100863/latest/semihosting-operations
 */

/* Need AmbaWrap_memcpy() for MISRA C */
#include <AmbaWrap.h>

#include <RefBuild.h>



#define SM_SYS_OPEN         (0x01)
#define SM_SYS_CLOSE        (0x02)
#define SM_SYS_WRITEC       (0x03) /* Writes a character byte, pointed to by r1, to the debug channel. */
#if 0
#define SM_SYS_WRITE0       (0x04) /* Writes a null-terminated string to the debug channel. r1 contains a pointer to the first byte of the string. */
#endif
#define SM_SYS_WRITE        (0x05)
#define SM_SYS_READ         (0x06)
#define SM_SYS_READC        (0x07) /* Not implemented by qemu */

#define SM_SYS_ISTTY        (0x09)
#define SM_SYS_SEEK         (0x0A)

#define SM_SYS_TIME         (0x11)

#define SM_SYS_FLEN         (0x0C)

#define SM_SYS_EXIT             (0x18)
#if defined(EXIT_E) && (EXIT_E == 1) /* Using SM_SYS_EXIT_EXTENDED */
#define SM_SYS_EXIT_EXTENDED    (0x20)
#endif

/* Used by SM_SYS_EXIT */
#define ADP_Stopped_ApplicationExit     (0x20026)

typedef struct {
    ULONG hdl;  /* file handle */
    char *buf; /* Pointer of buffer */
    ULONG len;  /* bytes to read */
    ULONG pad;
} sm_sys_read_param;


#ifdef __aarch64__
#define svc_swi __asm__("hlt 0xF000")
#else // aarch32
/*
 * SVC 0x123456: In ARM state for all architectures.
 * SVC 0xAB: In ARM state and Thumb state, excluding ARMv6-M and ARMv7-M.
 *           This behavior is not guaranteed on all debug targets from ARM or from third parties.
 * BKPT 0xAB: For ARMv6-M and ARMv7-M, Thumb state only.
 */
#define svc_swi __asm__("svc 0x00123456")
#endif // __aarchXX__

static ULONG svc_op(ULONG op, ULONG param)
{
#ifdef __aarch64__
    register ULONG reg0 __asm__("x0") __attribute__ ((unused));
    register ULONG reg1 __asm__("x1") __attribute__ ((unused));
#else
    register ULONG reg0 __asm__("r0") __attribute__ ((unused));
    register ULONG reg1 __asm__("r1") __attribute__ ((unused));
#endif
    register ULONG regV = 0;

    reg0 = op;
    reg1 = param;

    switch (op) {
    case SM_SYS_WRITEC:
    case SM_SYS_EXIT:
        reg0 = op;
        reg1 = param;
        svc_swi;
        break;

    case SM_SYS_READC:
#if 0
        /* SM_SYS_READC is not implemented */
        reg0 = SM_SYS_READ;
        reg1 = param;
#else
        reg0 = op;
        reg1 = 0;
#endif
        svc_swi;
        regV = reg0;
        break;

    case SM_SYS_OPEN:
    case SM_SYS_CLOSE:
    case SM_SYS_WRITE:
    case SM_SYS_READ:
    case SM_SYS_ISTTY:
    case SM_SYS_SEEK:
    case SM_SYS_TIME:
        reg0 = op;
        reg1 = param;
        svc_swi;
        regV = reg0;
        break;

    default:
        regV = 0xFFFFFFFFU;
        break;
    }

    /* MISRA C */
    (void)reg0;
    (void)reg1;

    return regV;
}

/* putchar() via semihosting, qemu would need "-semihosting-config target=gdb" */
void semi_putchar(char c)
{
    const void *p = &c;
    ULONG v;

    /* For MISRA C */
    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));

    (void) svc_op(SM_SYS_WRITEC, v);
}

/* getchar from semihosting */
LONG semi_getchar(void)
{
    /* -1 as error */
    ULONG c;
    char buf[8];
    sm_sys_read_param param;
    const void *p = &param;
    ULONG v;
    LONG  r = -1;

    param.hdl = 0;
    param.buf = buf;
    param.len = 1;
    param.pad = 0;

    /* For MISRA C */
    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));

    c = svc_op(SM_SYS_READC, v);
#if 1
    (void) AmbaWrap_memcpy(&r, &c, sizeof(ULONG));
#else
    if (c == 0U) {
        /* MISRA C */
        char *pR = (char *)&r;
        *pR = buf[0];
        (void)pR;
    } else if (c == 1U) {
        /* No bytes were read */
        /* TODO: error */
        r = -1;
    } else {
        /* Should no else */
        r = -1;
    }
#endif

    return r;
}


static ULONG semi_strlen(const char *p)
{
    ULONG size = 0;

    if (p == NULL) {
        size = 0;
    } else {
        while (p[size] != '\0') {
            size++;
        }
    }

    return size;
}


typedef struct {
    const char *name;
    ULONG        mode;
    ULONG        len_name;
} sm_sys_open_param;

LONG semi_open(const char *name, ULONG mode)
{
    sm_sys_open_param param;
    const void *p = &param;
    ULONG v;
    ULONG err;
    LONG  r = 0;

    param.name = name;
    param.mode = mode;
    param.len_name = semi_strlen(name);

    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));
    err = svc_op(SM_SYS_OPEN, v);
    (void) AmbaWrap_memcpy(&r, &err, sizeof(LONG));
    return r;
}


LONG semi_close(LONG hdl)
{
    const void *p = &hdl;
    ULONG v;
    ULONG err;
    LONG  r = 0;

    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));
    err = svc_op(SM_SYS_CLOSE, v);
    (void) AmbaWrap_memcpy(&r, &err, sizeof(LONG));
    return r;
}


typedef struct {
    LONG     file;
    char    *ptr; // Buffer
    ULONG    len;
} sm_sys_rdwt_param;

LONG semi_write(LONG file, char *ptr, ULONG len)
{
    sm_sys_rdwt_param param;
    const void *p;
    ULONG v;
    ULONG err;
    LONG  r = 0;

    /*
     * sys/unistd.h, STDIN_FILENO(1), STDERR_FILENO(2), using WRITE0
     * But WRITE0 checks only '\0', it would be issue of buffer-overflow.
     * And '\n' is not treated as '\r\n'.
     * Caller, i.e. _wite(), covers checling of 'file'.
     */
    if ((file == 1) || (file == 2)) {
        return -1;
    } else {
        p = &param;

        param.file = file;
        param.ptr = ptr;
        param.len =  len;

        (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));
        err = svc_op(SM_SYS_WRITE, v);
        if (err == 0U) {
            (void) AmbaWrap_memcpy(&r, &len, sizeof(LONG));
        } else {
            if (err < len) {
                v = len - err;
                (void) AmbaWrap_memcpy(&r, &v, sizeof(LONG));
            } else {
                r = -1;
            }
        }
    }
    return r;
}

LONG semi_read(LONG file, char *ptr, ULONG len)
{
    sm_sys_rdwt_param param;
    const void *p = &param;
    ULONG v;
    ULONG err;
    LONG  r = 0;


    param.file = file;
    param.ptr = ptr;
    param.len =  len;

    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));
    err = svc_op(SM_SYS_READ, v);
    if (err == 0U) {
        (void) AmbaWrap_memcpy(&r, &len, sizeof(LONG));
    } else if (err == len) {
        /* No bytes were read */
        /* TODO: error */
        r = -1;
    } else {
        /* few data, less than len, err = bytes NOT filled */
        v = len - err;
        (void) AmbaWrap_memcpy(&r, &v, sizeof(LONG));
    }
    return r;
}


LONG semi_isatty(LONG file)
{
    const void *p = &file;
    ULONG v;
    ULONG err;
    LONG  r = 0;

    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));
    err = svc_op(SM_SYS_ISTTY, v);
    (void) AmbaWrap_memcpy(&r, &err, sizeof(LONG));
    return r;
}


typedef struct {
    LONG     file;
    LONG     offset;
} sm_sys_seek_param;

LONG semi_seek(LONG file, LONG offset)
{
    sm_sys_seek_param param;
    const void *p = &param;
    ULONG v;
    ULONG err;
    LONG  r = 0;


    param.file = file;
    param.offset = offset;

    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));
    err = svc_op(SM_SYS_SEEK, v);
    (void) AmbaWrap_memcpy(&r, &err, sizeof(LONG));
    return r;
}

LONG semi_flen(LONG file)
{
    const void *p = &file;
    ULONG v;
    ULONG err;
    LONG  r = 0;

    (void) AmbaWrap_memcpy(&v, &p, sizeof(LONG));
    err = svc_op(SM_SYS_FLEN, v);
    (void) AmbaWrap_memcpy(&r, &err, sizeof(LONG));
    return r;
}

ULONG semi_time(void)
{
    /* Return the number of seconds since 00:00 Jan 1, 1970 */
    return svc_op(SM_SYS_TIME, 0U);
}


/* Need to run qemu with "-no-reboot" */
ULONG semi_AppExit(void)
{
    return svc_op(SM_SYS_EXIT, ADP_Stopped_ApplicationExit);
}

#if (defined(EXIT_E) && (EXIT_E == 1)) || defined(__aarch64__)
struct param_appexit {
    LONG code;
    LONG status;
};
#endif

void semi_exit(LONG status);

/* Need to run qemu with "-no-reboot" */
void semi_exit(LONG status)
{
// Will return status
#if defined(EXIT_E) && (EXIT_E == 1) /* Using SM_SYS_EXIT_EXTENDED */
    struct param_appexit p = { ADP_Stopped_ApplicationExit, status };

    (void)svc_op(SM_SYS_EXIT_EXTENDED, (ULONG)&p);
#elif defined(__aarch64__)
    struct param_appexit p = { ADP_Stopped_ApplicationExit, status };

    (void)svc_op(SM_SYS_EXIT, (ULONG)&p);
#else // Will always return 0
    ULONG code = ADP_Stopped_ApplicationExit;

    (void)status;
    (void)svc_op(SM_SYS_EXIT, code);
#endif

    {
        static char x[] = ">>> Must not see this <<<\r\n";
        ULONG i;

        for (i = 0u; i < sizeof(x); i++) {
            semi_putchar(x[i]);
        }
    }

    /* Should not return */
    for (;;) { ; }
}

