/*
 * Copyright (c) 2017, Ambarella Inc.
 * All rights reserved.
 *
 * Author: Zhi He <zhe@ambarella.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef AMBARELLA_TRNG_H
#define AMBARELLA_TRNG_H

#include <types_ext.h>

#if defined (ambarella_cv5) || defined(ambarella_cv52)
#define SECURE_SCRATCHPAD_BASE		0x20E002F000UL
#elif defined (ambarella_cv2fs) || defined(ambarella_cv22fs)
#define SECURE_SCRATCHPAD_BASE		0xe002f000
#else
#define SECURE_SCRATCHPAD_BASE		0xE8001000
#endif

#define SECURE_SCRATCHPAD_REG(x)	(SECURE_SCRATCHPAD_BASE + (x))

#ifdef ambarella_cv2
#define SECSP_RNG_CNT_BASE_OFFSET		0x40
#define SECSP_RNG_DATA4_OFFSET		0x10
#else
#define SECSP_RNG_CNT_BASE_OFFSET		0x00
#define SECSP_RNG_DATA4_OFFSET		0xB0
#endif

#define SECSP_RNG_CNT_OFFSET		0x00
#define SECSP_RNG_DATA0_OFFSET		0x04
#define SECSP_RNG_DATA1_OFFSET		0x08
#define SECSP_RNG_DATA2_OFFSET		0x0C
#define SECSP_RNG_DATA3_OFFSET		0x10

#define SECSP_RNG_CNT_REG		SECURE_SCRATCHPAD_REG(SECSP_RNG_CNT_BASE_OFFSET)

#define AMBARELLA_TRNG_REG_BASE SECSP_RNG_CNT_REG
#define AMBARELLA_TRNG_REG_SIZE 0x1000

#if defined (ambarella_cv5) || defined(ambarella_cv52)
#define RCT_BASE		0x20ED080000UL
#else
#define RCT_BASE		0xED080000
#endif

#define RCT_REG(x)		(RCT_BASE + (x))

#define RNG_CTRL_OFFSET		0x1A4
#define RNG_CTRL_REG			RCT_REG(RNG_CTRL_OFFSET)
#define RNG_CTRL_PD				(1 << 0)


void ambarella_trng_init(unsigned int amba_rng_samplerate);
int ambarella_trng_read(unsigned int *pout, unsigned int read_size);

#endif /* AMBARELLA_TRNG_H */
