
#ifndef __local_mem_util_H_wPhsuUtC_lsAI_H9hy_sLrS_uhgJ9lgraZeN__
#define __local_mem_util_H_wPhsuUtC_lsAI_H9hy_sLrS_uhgJ9lgraZeN__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================
#if defined(SHOW_MEM_DATA)
static inline void
_show_mem_data(char *pInput, int length)
{
    int     i;
    printf("\n*** mem data (%d):\n", length);
    for(i = 0; i < length; i++)
    {
        if( i && (i & 0xF) == 0 )
            printf("\n");
        printf("%02x ", *(pInput+i));
    }
    printf("\n\n");
    usleep(20000);
}
#else
#define _show_mem_data(a, b)
#endif

#if defined(SHOW_CHAR_DATA)
static inline void
_show_char_data(char *pInput, int length)
{
    int     i;
    printf("\n*** char data (%d):\n", length);
    for(i = 0; i < length; i++)
    {
        printf("%c", *(pInput+i));
    }
    printf("\n\n");
    usleep(20000);
}
#else
#define _show_char_data(a, b)
#endif


#if defined(HAVE_PRVATE_MEMCPY) // workaround memcpy fail
static inline void*
_my_memcpy(void *dest, void *src, size_t count)
{
    char    *to = (char*)dest;
    char    *from = (char*)src;
    size_t  align_length = 0l;
    long    head_align_offset = 0l, tail_align_offset = 0l;

    head_align_offset = (((unsigned long)from + 0x7) & ~0x7) - (unsigned long)from;
    switch( head_align_offset ) {
        case 7: *to++ = *from++;
        case 6: *to++ = *from++;
        case 5: *to++ = *from++;
        case 4: *to++ = *from++;
        case 3: *to++ = *from++;
        case 2: *to++ = *from++;
        case 1: *to++ = *from++;
        default:    break;
    }

    align_length      = count - head_align_offset;
    tail_align_offset = align_length & 0x7;

    align_length &= (~0x7);
    memcpy(to, from, align_length);

    to   += align_length;
    from += align_length;

    switch( tail_align_offset ) {
        case 7: *to++ = *from++;
        case 6: *to++ = *from++;
        case 5: *to++ = *from++;
        case 4: *to++ = *from++;
        case 3: *to++ = *from++;
        case 2: *to++ = *from++;
        case 1: *to++ = *from++;
        default:    break;
    }
    return dest;
}

#undef memcpy
#define memcpy      _my_memcpy
#endif

#if defined(HAVE_PRVATE_MEMSET) // workaround memset fail
static inline void*
_my_memset(void *src, int c, size_t count)
{
    char    *to = (char*)src;
    size_t  align_length = 0l;
    int     value = 0;
    long    head_align_offset = 0l, tail_align_offset = 0l;

    // Truncate c to 8 bits
    value = c & 0xFF;

    head_align_offset = (((unsigned long)to + 0x7) & ~0x7) - (unsigned long)to;
    switch( head_align_offset ) {
        case 7: *to++ = value;
        case 6: *to++ = value;
        case 5: *to++ = value;
        case 4: *to++ = value;
        case 3: *to++ = value;
        case 2: *to++ = value;
        case 1: *to++ = value;
        default:    break;
    }

    align_length      = count - head_align_offset;
    tail_align_offset = align_length & 0x7;

    align_length &= (~0x7);

    if( value )
    {
        memset(to, value, align_length);
    }
    else
    {
        unsigned long   *pLongAddr = (unsigned long*)to;
        size_t          long_align_len = align_length >> 3;
        int             n = (long_align_len + 0x7) >> 3;

        switch( long_align_len & 0x3 ) {
            case 0: do { *pLongAddr++ = 0ul;
            case 7:      *pLongAddr++ = 0ul;
            case 6:      *pLongAddr++ = 0ul;
            case 5:      *pLongAddr++ = 0ul;
            case 4:      *pLongAddr++ = 0ul;
            case 3:      *pLongAddr++ = 0ul;
            case 2:      *pLongAddr++ = 0ul;
            case 1:      *pLongAddr++ = 0ul;
                    } while (--n > 0);
        }
    }

    to   += align_length;
    switch( tail_align_offset ) {
        case 7: *to++ = value;
        case 6: *to++ = value;
        case 5: *to++ = value;
        case 4: *to++ = value;
        case 3: *to++ = value;
        case 2: *to++ = value;
        case 1: *to++ = value;
        default:    break;
    }
    return src;
}

#undef memset
#define memset      _my_memset
#endif
//=============================================================================
//                Public Function Definition
//=============================================================================


#ifdef __cplusplus
}
#endif

#endif
