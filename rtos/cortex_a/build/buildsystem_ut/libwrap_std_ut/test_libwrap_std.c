/**
 *  @file test_libwrap_std.c
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
 *  @details Test case for libwrap_std.a
 *
 */

#include <AmbaWrap.h>


#pragma GCC push_options
#pragma GCC optimize ("O0")
#define TEST_ERR (1U)
UINT32 test_libwrap_std(void);


/* Local implementation */
static UINT32 test_AmbaWrap_memcpy(void)
{
    const char a[10] = { 'A', 'm', 'b', 'a', 'r', 'e', 'l', 'l', 'a', '.' };
    char b[10];
    UINT32 err;
    INT32 i;

    err = AmbaWrap_memcpy(b, a, 10);
    if (err == 0U) {
        for (i = 0; i < 10; i++) {
            if (a[i] != b[i]) {
                err = TEST_ERR;
                break;
            }
        }
    }

    return err;
}


/* libc.a */
static UINT32 test_AmbaWrap_abs(void)
{
    UINT32 err;
    INT32 v = 0;

    err = AmbaWrap_abs(17, &v);
    if (err == 0U) {
        if (v != 17) {
            err = TEST_ERR;
        } else {
            err = AmbaWrap_abs(-2000000000, &v);
            if (err == 0U) {
                if (v != 2000000000) {
                    err = TEST_ERR;
                }
            }
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_memcmp(void)
{
    UINT32 err;
    INT32 v         = 0;
    const char b1[] = "1234";
    const char b2[] = "1256";

    err = AmbaWrap_memcmp(b1, b2, 2, &v);
    if (err == 0U) {
        if (v != 0) {
            err = TEST_ERR;
        } else {
            err = AmbaWrap_memcmp(b1, b2, 4, &v);
            if (err == 0U) {
                if (v >= 0) {
                    err = TEST_ERR;
                } else {
                    err = AmbaWrap_memcmp(b2, b1, 4, &v);
                    if (err == 0U) {
                        if (v <= 0) {
                            err = TEST_ERR;
                        }
                    }
                }
            }
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_memmove(void)
{
    UINT32 err;
    char mem[] = "Do memmove() with overlap.                                  ";
    INT32 v    = 0;

    err = AmbaWrap_memmove(&(mem[30]), &(mem[3]), 9);
    if (err == 0U) {
        err = AmbaWrap_memcmp(&(mem[30]), &(mem[3]), 9, &v);
        if (err == 0U) {
            if (v != 0) {
                err = TEST_ERR;
            }
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_memset(void)
{
    UINT32 err;
    UINT32 b[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35 }; // 012345
    INT32 i;
    INT32 v  = 0x41;                                     // 'A'
    UINT32 r = 0x41414141;

    err = AmbaWrap_memset(b, v, sizeof(b));
    if (err == 0U) {
        for (i = 0; i < 6; i++) {
            if (b[i] != r) {
                err = TEST_ERR;
                break;
            }
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_rand(void)
{
    UINT32 err;

    err = AmbaWrap_srand(0xA5);
    if (err == 0U) {
        INT32 v = 0;

        err = AmbaWrap_rand(&v);
        if (err == 0U) {
            err = AmbaWrap_srand((UINT32)v);
        }
    }

    return err;
}


/*
 * libm.a
 */

#define PI 3.14159265

static UINT32 test_AmbaWrap_acos(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 0.5;

    err = AmbaWrap_acos(x, &v);
    if (err == 0U) {
        v = v * 180.0 / PI;
        // round
        (void) AmbaWrap_floor(v, &v);
        if (v != 60.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_asin(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 0.5;

    err = AmbaWrap_asin(x, &v);
    if (err == 0U) {
        v = v * 180.0 / PI;
        // round
        (void) AmbaWrap_floor(v, &v);
        if (v != 30.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_atan(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 1.0;

    err = AmbaWrap_atan(x, &v);
    if (err == 0U) {
        v = v * 180.0 / PI;
        // round
        (void) AmbaWrap_floor(v, &v);
        if (v != 45.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_ceil(void)
{
    UINT32 err;
    DOUBLE v = 0.0;

    err = AmbaWrap_ceil(3.2, &v);
    if (err == 0U) {
        if (v != 4.0) {
            err = TEST_ERR;
        } else {
            err = AmbaWrap_ceil(7.8, &v);
            if (err == 0U) {
                if (v != 8.0) {
                    err = TEST_ERR;
                } else {
                    err = AmbaWrap_ceil(-3.2, &v);
                    if (err == 0U) {
                        if (v != -3.0) {
                            err = TEST_ERR;
                        } else {
                            err = AmbaWrap_ceil(-7.8, &v);
                            if (err == 0U) {
                                if (v != -7.0) {
                                    err = TEST_ERR;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_cos(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 60.0 * PI / 180.0;

    err = AmbaWrap_cos(x, &v);
    if (err == 0U) {
        // round
        v *= 10.0;
        (void) AmbaWrap_floor(v, &v);
        if (v != 5.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_fabs(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE r;

    err = AmbaWrap_fabs(3.14, &v);
    if (err == 0U) {
        r = v - 3.14;
        if (r != 0.0) {
            err = TEST_ERR;
        } else {
            err = AmbaWrap_fabs(-512.6, &v);
            if (err == 0U) {
                r = v - 512.6;
                if (r != 0.0) {
                    err = TEST_ERR;
                }
            }
        }
    }
    return err;
}

static UINT32 test_AmbaWrap_floor(void)
{
    UINT32 err;
    DOUBLE v = 0.0;

    err = AmbaWrap_floor(3.2, &v);
    if (err == 0U) {
        if (v != 3.0) {
            err = TEST_ERR;
        } else {
            err = AmbaWrap_floor(7.8, &v);
            if (err == 0U) {
                if (v != 7.0) {
                    err = TEST_ERR;
                } else {
                    err = AmbaWrap_floor(-3.2, &v);
                    if (err == 0U) {
                        if (v != -4.0) {
                            err = TEST_ERR;
                        } else {
                            err = AmbaWrap_floor(-7.8, &v);
                            if (err == 0U) {
                                if (v != -8.0) {
                                    err = TEST_ERR;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_log10(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 100.0;

    err = AmbaWrap_log10(x, &v);
    if (err == 0U) {
        if (v != 2.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_log2(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 512.0;

    err = AmbaWrap_log2(x, &v);
    if (err == 0U) {
        if (v != 9.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_pow(void)
{
    UINT32 err;
    DOUBLE v = 0.0;

    err = AmbaWrap_pow(8.9, 1.2, &v);
    if (err == 0U) {
        // round
        (void) AmbaWrap_floor(v, &v);
        if (v != 13.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_sin(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 31.0 * PI / 180.0;

    err = AmbaWrap_sin(x, &v);
    if (err == 0U) {
        // round
        v *= 10.0;
        (void) AmbaWrap_floor(v, &v);
        if (v != 5.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_sqrt(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 272.25;

    err = AmbaWrap_sqrt(x, &v);
    if (err == 0U) {
        if (v != 16.5) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_tan(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 45.0 * PI / 180.0;

    err = AmbaWrap_tan(x, &v);
    if (err == 0U) {
        // round
        v *= 10.0;
        (void) AmbaWrap_floor(v, &v);
        if (v != 9.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_exp(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 5.0;

    err = AmbaWrap_exp(x, &v);
    if (err == 0U) {
        (void) AmbaWrap_floor(v, &v);
        if (v != 148.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_log(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 5.5;

    err = AmbaWrap_log(x, &v);
    if (err == 0U) {
        (void) AmbaWrap_floor(v, &v);
        if (v != 1.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_atan2(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = -10.0;
    DOUBLE y = 10.0;
    DOUBLE result;

    err = AmbaWrap_atan2(y, x, &v);
    if (err == 0U) {
        // result would be 135.00000015425999, floor it
        result = v * 180 / PI;
        (void) AmbaWrap_floor(result, &result);
        if (result != 135.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

static UINT32 test_AmbaWrap_fmod(void)
{
    UINT32 err;
    DOUBLE v = 0.0;
    DOUBLE x = 18.5;
    DOUBLE y = 4.2;

    err = AmbaWrap_fmod(x, y, &v);
    if (err == 0U) {
        // v would be 1.6999999999999993
        (void) AmbaWrap_floor(v, &v);
        if (v != 1.0) {
            err = TEST_ERR;
        }
    }

    return err;
}

/* ------------------------------------------------------------------------- */

UINT32 test_libwrap_std(void)
{
    UINT32 err;

    err = test_AmbaWrap_memcpy();

    /* libc.a */
    if (err == 0U) {
        err = test_AmbaWrap_abs();
    }

    if (err == 0U) {
        err = test_AmbaWrap_memcmp();
    }
    if (err == 0U) {
        err = test_AmbaWrap_memmove();
    }
    if (err == 0U) {
        err = test_AmbaWrap_memset();
    }
    if (err == 0U) {
        /* Testing srand() and rand() */
        err = test_AmbaWrap_rand();
    }

    /* libm.a */
    if (err == 0U) {
        err = test_AmbaWrap_acos();
    }
    if (err == 0U) {
        err = test_AmbaWrap_asin();
    }
    if (err == 0U) {
        err = test_AmbaWrap_atan();
    }
    if (err == 0U) {
        err = test_AmbaWrap_ceil();
    }
    if (err == 0U) {
        err = test_AmbaWrap_cos();
    }
    if (err == 0U) {
        err = test_AmbaWrap_fabs();
    }
    if (err == 0U) {
        err = test_AmbaWrap_floor();
    }
    if (err == 0U) {
        err = test_AmbaWrap_log10();
    }
    if (err == 0U) {
        err = test_AmbaWrap_log2();
    }
    if (err == 0U) {
        err = test_AmbaWrap_pow();
    }
    if (err == 0U) {
        err = test_AmbaWrap_sin();
    }
    if (err == 0U) {
        err = test_AmbaWrap_sqrt();
    }
    if (err == 0U) {
        err = test_AmbaWrap_tan();
    }
    if (err == 0U) {
        err = test_AmbaWrap_exp();
    }
    if (err == 0U) {
        err = test_AmbaWrap_log();
    }
    if (err == 0U) {
        err = test_AmbaWrap_atan2();
    }
    if (err == 0U) {
        err = test_AmbaWrap_fmod();
    }

    return err;
}

#pragma GCC pop_options
