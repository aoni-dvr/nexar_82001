/*
* Copyright (c) 2020 Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef IKC_SYSTEM_INFO_H
#define IKC_SYSTEM_INFO_H

#include "ik_data_type.h"
#include "AmbaDSP_ImgProcess.h"

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))
#define INTERNAL_EARLY_TEST_ENV  1
#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))
#include "AmbaTypes.h"
#include "AmbaPrint.h" // PP dirty mark
#include "AmbaWrap.h"
#define INTERNAL_EARLY_TEST_ENV  0
#else
#include "AmbaTypes.h"
#include "AmbaPrint.h" // PP dirty mark
#include "AmbaWrap.h"
#define INTERNAL_EARLY_TEST_ENV  0
#endif

#define DC_I    99999   /*PRINT DONT CARE FOR INT*/
#define DC_U    99999U  /*PRINT DONT CARE FOR UINT*/
#define DC_S    NULL    /*PRINT DONT CARE FOR STRING*/

typedef void* (*ikc_system_memcpy_t)(void *p_str1, const void *p_str2, size_t n);
typedef void* (*ikc_system_memset_t)(void *p_str, int32 c, size_t n);
typedef int32 (*ikc_system_memcmp_t)(const void *p_str1, const void *p_str2, size_t n);
typedef int32 (*ikc_system_print_t)(const char *p_format, ...);

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

#define ADVANCED_STITCH 0

extern ikc_system_memcpy_t ikc_system_memcpy;
extern ikc_system_memset_t ikc_system_memset;
extern ikc_system_memcmp_t ikc_system_memcmp;
extern ikc_system_print_t ikc_system_print;

#define amba_ikc_system_memcpy ikc_system_memcpy
#define amba_ikc_system_memset ikc_system_memset
#define amba_ikc_system_memcmp ikc_system_memcmp
#define amba_ikc_system_print_uint64_5(fmt, arg...) ikc_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ikc_system_print_uint32_5(fmt, arg...) ikc_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ikc_system_print_int32_5(fmt, arg...) ikc_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ikc_system_print_str_5(fmt, arg...) ikc_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ikc_system_print_flush(...)

void ikc_registe_system_memcpy(const ikc_system_memcpy_t *p_system_memcpy);
void ikc_registe_system_memset(const ikc_system_memset_t *p_system_memset);
void ikc_registe_system_memcmp(const ikc_system_memcmp_t *p_system_memcmp);
void ikc_registe_system_print(const ikc_system_print_t *p_system_print);
void ikc_unregiste_system_memcpy(void);
void ikc_unregiste_system_memset(void);
void ikc_unregiste_system_memcmp(void);
void ikc_unregiste_system_print(void);

typedef struct {
    ikc_system_memcpy_t system_memcpy;
    ikc_system_memset_t system_memset;
    ikc_system_memcmp_t system_memcmp;
    ikc_system_print_t system_print;
} ikc_system_api_t;

uint32 ikc_import_system_callback_func(const ikc_system_api_t *p_sys_api);

#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))

#define ADVANCED_STITCH 1

void ikc_system_print_uint64_5(const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5);
void ikc_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void ikc_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void ikc_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

//rtos
//#define amba_ikc_system_memcpy AmbaWrap_memcpy //memcpy
void amba_ikc_system_memcpy(void *pDst, const void *pSrc, SIZE_t num);
//#define amba_ikc_system_memset AmbaWrap_memset //memset
void amba_ikc_system_memset(void *ptr, INT32 v, SIZE_t n);
//#define amba_ikc_system_memcmp AmbaWrap_memcmp //memcmp
#define amba_ikc_system_print_uint64_5 ikc_system_print_uint64_5 // AmbaPrint_PrintUInt5
#define amba_ikc_system_print_uint32_5 ikc_system_print_uint32_5 // AmbaPrint_PrintUInt5
#define amba_ikc_system_print_int32_5 ikc_system_print_int32_5 // AmbaPrint_PrintInt5
#define amba_ikc_system_print_str_5 ikc_system_print_str_5 // AmbaPrint_PrintStr5
#define amba_ikc_system_print_flush AmbaPrint_Flush

#else

#define ADVANCED_STITCH 1

void ikc_system_print_uint64_5(const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5);
void ikc_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void ikc_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void ikc_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

//rtos
//#define amba_ikc_system_memcpy AmbaWrap_memcpy //memcpy
void amba_ikc_system_memcpy(void *pDst, const void *pSrc, SIZE_t num);
//#define amba_ikc_system_memset AmbaWrap_memset //memset
void amba_ikc_system_memset(void *ptr, INT32 v, SIZE_t n);
//#define amba_ikc_system_memcmp AmbaWrap_memcmp //memcmp
#define amba_ikc_system_print_uint64_5 ikc_system_print_uint64_5 // AmbaPrint_PrintUInt5
#define amba_ikc_system_print_uint32_5 ikc_system_print_uint32_5 // AmbaPrint_PrintUInt5
#define amba_ikc_system_print_int32_5 ikc_system_print_int32_5 // AmbaPrint_PrintInt5
#define amba_ikc_system_print_str_5 ikc_system_print_str_5 // AmbaPrint_PrintStr5
#define amba_ikc_system_print_flush AmbaPrint_Flush
#endif

#endif
