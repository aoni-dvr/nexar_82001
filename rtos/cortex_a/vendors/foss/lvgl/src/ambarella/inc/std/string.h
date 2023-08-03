/*
 *  Dummy header when not using standard libraries from toolchain.
 */

//#ifndef CONFIG_CC_USESTD
#ifndef STRING_H
#define STRING_H

#define AMBA_ERROR_CODE_H  // Prevent to define conflicts, ERR_ARG
#include <AmbaWrap.h>
#include <AmbaUtility.h>

extern void *memset(void *s, int c, size_t n);
#define memcpy  AmbaWrap_memcpy
#define memmove AmbaWrap_memmove


#define strlen  AmbaUtility_StringLength

inline static char* strcat(char* destination, const char* source)
{
	// make ptr point to the end of destination string
	char* ptr = destination + strlen(destination);

	// Appends characters of source to the destination string
	while (*source != '\0')
		*ptr++ = *source++;

	// null terminate destination string
	*ptr = '\0';

	// destination is returned by standard strcat()
	return destination;
}


inline static char* strcpy(char* destination, const char* source)
{
	// return if no memory is allocated to the destination
	if (destination == NULL)
		return NULL;

	// take a pointer pointing to the beginning of destination string
	char *ptr = destination;

	// copy the C-string pointed by source into the array
	// pointed by destination
	while (*source != '\0')
	{
		*destination = *source;
		destination++;
		source++;
	}

	// include the terminating null character
	*destination = '\0';

	// destination is returned by standard strcpy()
	return ptr;
}



inline static char *strncpy(char *dest, const char *src, size_t n)
{
    UINT32 CopyLength;
    UINT32 i;

    if ((dest == NULL) || (src == NULL) || (n == 0U)) {
	// no action
    } else {
	CopyLength = AmbaUtility_StringLength(src);
	if (CopyLength > n) {
	    // overflow condition
	    CopyLength = n;
	}
	for (i = 0; i < CopyLength; i++) {
	    dest[i] = src[i];
	}

	if (CopyLength < n) {
	    dest[CopyLength] = '\0';
	}
    }

	return dest;
}

inline static int memcmp(const void *s1, const void *s2, size_t n)
{
    int v = 0;

    (void)AmbaWrap_memcmp(s1, s2, n, &v);

    return v;
}


#define strlen      AmbaUtility_StringLength

/*
 *  Reference:
 *      - bionic/libc/upstream-openbsd/lib/libc/string/strcmp.c
 *      - http://mirror.fsf.org/pmon2000/3.x/src/lib/libc/strcmp.c
 */
inline static int strcmp(const char *s1, const char *s2)
{
    if ((s1 == NULL) || (s2 == NULL) || (*s1 == '\0')) {
        return 0;
    }

    while ((*s1 != '\0') && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return ((*(unsigned char *) s1) - (*(unsigned char *) s2));
}

/*
 *  Reference:
 *      - bionic/libc/upstream-openbsd/lib/libc/string/strncmp.c
 *      - http://mirror.fsf.org/pmon2000/3.x/src/lib/libc/strncmp.c
 */
inline static int strncmp(const char *s1, const char *s2, size_t n)
{
    if ((s1 == NULL) || (s2 == NULL) || (*s1 == '\0') || (n <= 0)) {
        return 0;
    }

    while ((*s1 != '\0') && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
        if (n == 0) {
            break;
        }
    }

    if (n > 0) {
        return ((*(unsigned char *) s1) - (*(unsigned char *) s2));
    } else {
        return 0;
    }
}


/*
 *  Reference:
 *      - bionic/libc/upstream-openbsd/lib/libc/string/strstr.c
 *      - http://mirror.fsf.org/pmon2000/3.x/src/lib/libc/strstr.c
 */
inline static char *strstr(const char *s, const char *find)
{
    char c, sc;
    size_t len;

    if ((s == NULL) || (find == NULL)) {
        return 0;
    }

    if ((c = *find++) != 0) {
        len = strlen(find);
        do {
            do {
                if ((sc = *s++) == 0) {
                    return NULL;
                }
            } while (sc != c);
        } while (strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *)s);
}

/*
 *  Reference:
 *      - http://mirror.fsf.org/pmon2000/3.x/src/lib/libc/strchr.c
 */
inline static char *strchr(const char *s, int c)
{
    if (s == NULL) {
        return 0;
    }

    while (*s != '\0') {
        if (*s == c) {
            return (char *)s;
        }
        s++;
    }
    return 0;
}

#endif /* !STRING_H */
//#endif


