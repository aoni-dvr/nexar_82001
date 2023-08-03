/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file dsp_types.h
 *
 * Some common types and macros
 *
 * History:
 *    2007/07/11 - [Qun Gu] created file.
 *
 * Copyright (C) 2006-2007, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef DSP_TYPES_H
#define DSP_TYPES_H
#if (defined(__linux__) || defined(ORCVIS) || defined(CONFIG_BUILD_CV_THREADX))
/* only do these for Linux or ORCVIS env. */
#ifndef _STDINT_H
/*!
  @file dsp_types.h

  @brief Contains typedefs (when called from C) and useful macros
 */


////////////////////////////////////////////////////////////////////////////////
#define DSP_TYPE_DEFINED 1

#ifndef __ASSEMBLER__

#ifndef AMBINT_H_FILE
#define AMBINT_H_FILE

#ifndef GENERAL_C
//<! 8-byte unsigned word
typedef unsigned long long uint64_t;
typedef unsigned long long ptr64_t;
//<! 8-byte signed word
typedef long long      int64_t;
//<! 1-byte signed word
typedef signed char    int8_t;
#endif

//<! 4-byte signed word
typedef int            int32_t;
//<! 2-byte signed word
typedef short          int16_t;
//<! 4-byte unsigned word
typedef unsigned int   uint32_t;
typedef unsigned int   ptr32_t;     /* 32-bit address pointer */
typedef unsigned int   rptr32_t;    /* Relative 32-bit pointer offset*/
//<! 2-byte unsigned word
typedef unsigned short uint16_t;
//<! 1-byte unsigned word
typedef unsigned char  uint8_t;

//<! Fake boolean type for use in C
typedef int BOOL;

typedef struct
{
  uint8_t segofs_msb8;  /* [39:32] of 40-bit offset */
  uint8_t rsvd0;
  uint8_t rsvd1;
  uint8_t g_seg_id; /* global segment id */
} u64_globaladdr_upper_t;

#endif
#endif //AMBINT_H_FILE

#ifndef NULL
#ifdef __ASSEMBLER__
# define NULL 0
#else
//<! NULL pointer
# define NULL ((void*)0)
#endif
#endif

#ifndef TRUE
//<! Boolean true value
#define TRUE 1
#endif

#ifndef FALSE
//<! Boolean false value
#define FALSE 0
#endif

#ifndef MISRA_COMPLIANCE  /* These cannot exist for MISRA compliant modes */

#ifndef MAX
#define MAX(a, b)      ((a) > (b) ? (a) : (b))  //!< Macro returning max value
#endif

#ifndef MIN
#define MIN(a, b)      ((a) < (b) ? (a) : (b))  //!< Macro returning min value
#endif


#ifndef INT_MIN
//<! Minimum represntable int
#define INT_MIN        (-2147483647 -1)
#endif

#ifndef INT_MAX
//<! Maximum represntable int
#define INT_MAX        (2147483647)
#endif

#ifndef UINT_MAX
//<! Minimum represntable unsigned int
#define UINT_MAX       (0xffffffff)
#endif

#endif /*?MISRA_COMPLIANCE */

////////////////////////////////////////////////////////////////////////////////
#endif // ?(__linux__ || ORCVIS)
#endif //?_STDINT_H
#endif //DSP_TYPES_H
