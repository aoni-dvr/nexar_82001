/**
 *  @file RefBuild.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header file for semihost APIs
 *
 */

#ifndef REFBUILD_H
#define REFBUILD_H

#ifndef AMBA_TYPES_H
/* For AArch32, data type is 32b. For AArch64, data type is 64b */
typedef long LONG;
typedef unsigned long ULONG;
#endif

#ifdef  __cplusplus
extern  "C" {
#endif

extern void semi_putchar(char c);
extern LONG semi_getchar(void);
extern ULONG semi_AppExit(void);

extern LONG semi_open(const char *name, ULONG mode);
extern LONG semi_close(LONG hdl);
extern LONG semi_write(LONG file, char *ptr, ULONG len);
extern LONG semi_read(LONG file, char *ptr, ULONG len);
extern LONG semi_isatty(LONG file);
extern LONG semi_seek(LONG file, LONG offset);
extern LONG semi_flen(LONG file);
extern ULONG semi_time(void);

#ifdef  __cplusplus
}
#endif

#ifdef CONFIG_BUT_VCAST
#include "vcast.h"
#endif

#endif /* REFBUILD_H */

