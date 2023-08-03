/**
 *  @file AmbaCRT0.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Initialization codes for C/C++ (constructor).
 *           cf. comments in newlib/libc/stdlib/__atexit.c of newlib
 */
/*
 * Copyright (C) 2004 CodeSourcery, LLC
 *
 * Permission to use, copy, modify, and distribute this file
 * for any purpose is hereby granted without fee, provided that
 * the above copyright notice and this notice appears in all
 * copies.
 *
 * This file is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <stdio.h>


#define HAVE_CRT0  (0)


void AmbaCRT0(void);
void _fini(void);

void AmbaCRT0(void)
{
#if HAVE_CRT0
    /* Using APIs from toolchain. */
    extern void __libc_init_array(void);
    __libc_init_array();
#else
    /* These magic symbols are provided by the linker.  */
    extern void (*__preinit_array_start []) (void) __attribute__((weak));
    extern void (*__preinit_array_end []) (void) __attribute__((weak));
    extern void (*__init_array_start []) (void) __attribute__((weak));
    extern void (*__init_array_end []) (void) __attribute__((weak));

//    extern void _init (void);

    /* Iterate over all the init routines.  */
    //void __libc_init_array (void)
    {
        size_t count;
        size_t i;

        count = __preinit_array_end - __preinit_array_start;
        for (i = 0; i < count; i++) {
            __preinit_array_start[i] ();
        }

//      _init ();

        count = __init_array_end - __init_array_start;
        for (i = 0; i < count; i++) {
            __init_array_start[i] ();
        }
    }
#endif
}

/* Replace weak symbol in libcrt.a */
void _fini(void)
{
    extern void (*__finit_array_start []) (void) __attribute__((weak));
    extern void (*__finit_array_end []) (void) __attribute__((weak));

    size_t count;
    size_t i;

    count = __finit_array_end - __finit_array_start;
    for (i = 0; i < count; i++) {
        __finit_array_start[i] ();
    }
}

