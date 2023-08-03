/*
     Copyright (C) 2004 By eSOL Co.,Ltd. Tokyo, Japan

     This software is protected by the law and the agreement concerning
     a Japanese country copyright method, an international agreement,
     and other intellectual property right and may be used and copied
     only in accordance with the terms of such license and with the inclusion
     of the above copyright notice.

     This software or any other copies thereof may not be provided
     or otherwise made available to any other person.  No title to
     and ownership of the software is hereby transferred.

     The information in this software is subject to change without
     notice and should not be construed as a commitment by eSOL Co.,Ltd.
 */
 /****************************************************************************
 [pf_code.h] - Character Code Module

    PrFILE2 uses three types of character sets as follows:

    - API Character Set (API-CS)

        A single-byte character set (SBCS) or a multi-byte character set
        (MBCS), which is used for arguments of API functions (ex. pathname)
        and for short file names.

        It is configurable what API-CS is actually used. (See 'pf_config.h')

    - API Wide Character Set (API-WCS)

        A 16-bit wide character set (WCS) corresponding to API Character
        Set. This API-WCS is used internally instead of API-CS.

    - Unicode

        A 16-bit wide character set (WCS) which is used for long file name.

    To convert each other, each character-code-specific module must provide
    the following macros:

    - PF_BOOL  IS_MB_LEAD(PF_U_CHAR c);

        This macro checks whether 'c', a byte of API-CS character, is a
        lead byte of MBCS or WCS characters.

        If the API-CSet is SBCS, this macro shall always return PF_FALSE.

    - PF_BOOL  IS_MB_TRAIL(PF_U_CHAR c);

        This macro checks whether 'c', a byte of API-CS character, is a
        trail byte of MBCS or WCS characters.

        If the API-CS is SBCS, this macro shall always return PF_FALSE.

    - PF_WCHAR  TO_WCS(PF_U_CHAR c_lead, PF_U_CHAR c_trail);

        This macro returns a API-WCS character corresponding to a API-CS
        character which consists of lead byte 'c_lead' and trail byte
        'c_trail'.

        If the API-CS is SBCS, this macro should be a dummy macro with
        no operation, since it is never called.

    - PF_WCHAR  WC_TO_UNICODE(PF_WCHAR wc)

        This macro returns a Unicode character corresponding to the given
        API-WCS character 'wc'.

    - PF_WCHAR  UNICODE_TO_WC(PF_WCHAR uc)

        This macro returns a API-WCS wide character corresponding to the
        given Unicode character 'uc'.

 ****************************************************************************/
#ifndef __PF_CODE_H__
#define __PF_CODE_H__

#include "prfile2/pf_locale.h"

/****************************************************************************
  Macro definitions
 ****************************************************************************/
/*---------------------------------------------------------------------------
  IS_ASCII
 ----------------------------------------------------------------------------*/
#define IS_ASCII(c)             ((c) < 0x80U)

#define IS_CAPITAL_LETTER(c)    (('A' <= (c)) && ((c) <= 'Z'))
#define IS_SMALL_LETTER(c)      (('a' <= (c)) && ((c) <= 'z'))

/*---------------------------------------------------------------------------
  TO_UPPER_WC

    If the argument 'wc', a wide character of API-WCS, is a lower-case
    letter, this macro returns a corresponding upper-case letter;
    Otherwise this function returns 'wc' without any conversion.
 ----------------------------------------------------------------------------*/
#define TO_UPPER_WC(wc)         ((PF_WCHAR)((('a' <= (wc)) && ((wc) <= 'z')) ? ((wc) + 'A' - 'a') : (wc)))


/*---------------------------------------------------------------------------
  IS_SEPARATOR
 ----------------------------------------------------------------------------*/
#define IS_SEPARATOR(c)         (((c) == '\\') || ((c) == '/'))


/*---------------------------------------------------------------------------
  IS_SFN_CHAR / IS_LFN_CHAR / IS_LFN_ONLY_CHAR / IS_PATH_CHAR
    0x01 (bit0) ... Set if the character is valid for Long Filename
    0x02 (bit1) ... Set if the character is valid for Short Filename
    0x04 (bit2) ... Set if the character is valid for Volume Label
    0x08 (bit3) ... Set if the character is valid for ICONV Codename
 ----------------------------------------------------------------------------*/
#define CC_TBL_CODE_MIN     (0x20U)
#define CC_TBL_CODE_MAX     (0x7fU)

#define CC_BIT_LONGNAME     (0x01U)
#define CC_BIT_SHORTNAME    (0x02U)
#define CC_BIT_VOLLABEL     (0x04U)
#define CC_BIT_ICONV        (0x08U)

/* Local */
#define IS_SKIP_CHAR_CHECK      (pf_vol_set.config & PF_CHAR_CHECK_DISABLE)

#define IS_SFN_CHAR(c)          (((c) > CC_TBL_CODE_MAX) \
                                 || (((c) >= CC_TBL_CODE_MIN) \
                                     && ((c) <= CC_TBL_CODE_MAX) \
                                     && ((pf_valid_fn_char[(c) - CC_TBL_CODE_MIN] & CC_BIT_SHORTNAME) == CC_BIT_SHORTNAME)))

#define IS_LFN_CHAR(c)          (((c) > CC_TBL_CODE_MAX) \
                                 || (((c) >= CC_TBL_CODE_MIN) \
                                     && ((c) <= CC_TBL_CODE_MAX) \
                                     && ((pf_valid_fn_char[(c) - CC_TBL_CODE_MIN] & CC_BIT_LONGNAME) == CC_BIT_LONGNAME)))

#define IS_VLABEL_CHAR_FAT(c)   (((c) > CC_TBL_CODE_MAX) \
                                 || (((c) >= CC_TBL_CODE_MIN) \
                                     && ((c) <= CC_TBL_CODE_MAX) \
                                     && ((pf_valid_fn_char[(c) - CC_TBL_CODE_MIN] & CC_BIT_VOLLABEL) == CC_BIT_VOLLABEL)))

#define IS_VLABEL_CHAR_EXFAT(c) IS_LFN_CHAR(c)

#if PF_EXFAT_SUPPORT
#define IS_VLABEL_CHAR(p_vol, c)    (IS_EXFAT(p_vol) ? IS_VLABEL_CHAR_EXFAT(c) : IS_VLABEL_CHAR_FAT(c))
#else /* PF_EXFAT_SUPPORT */
#define IS_VLABEL_CHAR(p_vol, c)    IS_VLABEL_CHAR_FAT(c)
#endif /* PF_EXFAT_SUPPORT */


#define IS_PATH_CHAR(c)         IS_LFN_CHAR(c)

#define IS_SFN_OR_SKIP_CHAR(c)  (IS_SKIP_CHAR_CHECK || IS_SFN_CHAR(c))

/* Unicode */
#define IS_SFN_WCHAR(wc)            IS_SFN_CHAR(wc)
#define IS_LFN_WCHAR(wc)            IS_LFN_CHAR(wc)
#define IS_VLABEL_WCHAR_FAT(wc)     IS_VLABEL_CHAR_FAT(wc)
#define IS_VLABEL_WCHAR_EXFAT(wc)   IS_LFN_WCHAR(wc)

#if PF_EXFAT_SUPPORT
#define IS_VLABEL_WCHAR(p_vol, c)   (IS_EXFAT(p_vol) ? IS_VLABEL_WCHAR_EXFAT(c) : IS_VLABEL_WCHAR_FAT(c))
#else
#define IS_VLABEL_WCHAR(p_vol, c)   IS_VLABEL_WCHAR_FAT(c)
#endif


#if PF_BIG_ENDIAN
#define IS_NOT_PATH_WCHAR(c1, c2)   (IS_LFN_WCHAR(TO_WCS(c1, c2)) == 0)
#define IS_NOT_VLABEL_WCHAR(p_vol, c1, c2) (IS_VLABEL_WCHAR(p_vol, TO_WCS(c1, c2)) == 0)
#else
#define IS_NOT_PATH_WCHAR(c1, c2)   (IS_LFN_WCHAR(TO_WCS(c2, c1)) == 0)
#define IS_NOT_VLABEL_WCHAR(p_vol, c1, c2) (IS_VLABEL_WCHAR(p_vol, TO_WCS(c2, c1)) == 0)
#endif

/*---------------------------------------------------------------------------
  UNICODE_WIDTH - width of UNICODE(UCS-2)
 ----------------------------------------------------------------------------*/
#define UNICODE_WIDTH           (2U)


#if PF_SUPPORT_FW_CONVERT
/*---------------------------------------------------------------------------
  UNICODE_character definitions
 ----------------------------------------------------------------------------*/
#define UNI_FW_SMALL_CHAR_A     (0xFF41)    /* Fullword small letter 'a' */
#define UNI_FW_SMALL_CHAR_Z     (0xFF5A)    /* Fullword small letter 'z' */

#define UNI_OFSET_FW_LARGE_CHAR (0x0020)    /* 0xFF41 - 0xFF21 = 0x0020 */
#endif


/****************************************************************************
  Variable declarations
 ****************************************************************************/
extern const PF_U_CHAR       pf_valid_fn_char[0x80-0x20];


/****************************************************************************
  Prototypes
 ****************************************************************************/
/* iconv */
#if PF_ICONV_SUPPORT
PF_ERROR  PFCODE_IconvInstall(PF_CONST PF_ICONV_CODESET *codeset, PF_CHAR codename[PF_ICONV_MAX_CODENAME+1U]);
PF_ERROR  PFCODE_IconvUninstall(PF_CONST PF_CHAR *codename);
PF_ERROR  PFCODE_IconvOpen(PF_ICONV *cp, PF_CONST PF_CHAR *to_codename, PF_CONST PF_CHAR *from_codename);
PF_ERROR  PFCODE_IconvClose(PF_ICONV *cp);
PF_ERROR  PFCODE_IconvConversion(PF_ICONV *cp, void **inbuf, PF_SIZE_T *inbytesleft, void **outbuf, PF_SIZE_T *outbytesleft, PF_SIZE_T *nonreversible_chars);
#endif /* PF_ICONV_SUPPORT*/

#endif  /* __PF_CODE_H__ */
