/*
 * Copyright (C) 2017 Ambarella Inc.
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

#include <assert.h>
#include <drivers/ambarella_trng.h>
#include <io.h>
#include <keep.h>
#include <util.h>

#include <mm/core_memprot.h>
#include <mm/core_mmu.h>

#include <kernel/delay.h>

/* print for debug */
#if 0
#define DDBG_PRINT MSG_RAW
#else
#define DDBG_PRINT(format, ...) (void)0
#endif

#define DAMBA_RNG_SAMPLE_RATE_32K  0x0
#define DAMBA_RNG_SAMPLE_RATE_64K  0x1
#define DAMBA_RNG_SAMPLE_RATE_128K  0x2
#define DAMBA_RNG_SAMPLE_RATE_256K  0x3
#define DAMBA_RNG_SAMPLE_RATE_BIT_SHIFT  4
#define DAMBA_RNG_SAMPLE_RATE_BIT_MASK  0x00000030

#define DAMBA_RNG_BUSY_BIT_SHIFT  1
#define DAMBA_RNG_CNT_BIT_SHIFT  0

typedef struct {
	paddr_t rng_reg_pa_base;
	vaddr_t rng_reg_va_base;

	vaddr_t rng_ctrl_reg_va;

	vaddr_t rng_data_reg_va[5];

	paddr_t rng_powerup_reg_pa_base;
	vaddr_t rng_powerup_reg_va_base;
} amba_trng_driver_t;

static amba_trng_driver_t amba_trng_driver;
static int trng_driver_inited = 0;

static vaddr_t __map_pa_to_va(paddr_t pa)
{
	if (cpu_mmu_enabled()) {
		return (vaddr_t) phys_to_virt_io(pa, SIZE_64K);
	} else {
		return pa;
	}
}

void ambarella_trng_init(unsigned int amba_rng_samplerate)
{
	if (!trng_driver_inited) {
		unsigned int val = 0;

		DDBG_PRINT("ambarella_trng_init begin");

		amba_trng_driver.rng_reg_pa_base = (paddr_t) SECURE_SCRATCHPAD_BASE;
		amba_trng_driver.rng_reg_va_base = __map_pa_to_va(amba_trng_driver.rng_reg_pa_base);

		amba_trng_driver.rng_powerup_reg_pa_base = (paddr_t) RNG_CTRL_REG;
		amba_trng_driver.rng_powerup_reg_va_base = __map_pa_to_va(amba_trng_driver.rng_powerup_reg_pa_base);

		amba_trng_driver.rng_ctrl_reg_va = amba_trng_driver.rng_reg_va_base + SECSP_RNG_CNT_OFFSET;

		amba_trng_driver.rng_data_reg_va[0] = amba_trng_driver.rng_reg_va_base + SECSP_RNG_DATA0_OFFSET;
		amba_trng_driver.rng_data_reg_va[1] = amba_trng_driver.rng_reg_va_base + SECSP_RNG_DATA1_OFFSET;
		amba_trng_driver.rng_data_reg_va[2] = amba_trng_driver.rng_reg_va_base + SECSP_RNG_DATA2_OFFSET;
		amba_trng_driver.rng_data_reg_va[3] = amba_trng_driver.rng_reg_va_base + SECSP_RNG_DATA3_OFFSET;
		amba_trng_driver.rng_data_reg_va[4] = amba_trng_driver.rng_reg_va_base + SECSP_RNG_DATA4_OFFSET;

		DDBG_PRINT("ambarella_trng_init before set samplerate, base pa %p va %p",
			(void *) amba_trng_driver.rng_reg_pa_base,
			(void *) amba_trng_driver.rng_reg_va_base);

		val = io_read32(amba_trng_driver.rng_ctrl_reg_va);
		val &= ~((unsigned int) DAMBA_RNG_SAMPLE_RATE_BIT_MASK);
		val |= amba_rng_samplerate << DAMBA_RNG_SAMPLE_RATE_BIT_SHIFT;
		io_write32(amba_trng_driver.rng_ctrl_reg_va, val);

		DDBG_PRINT("ambarella_trng_init power up");
		val = io_read32(amba_trng_driver.rng_powerup_reg_va_base);
		//DDBG_PRINT("val 0x%08x", val);
		val &= ~((unsigned int) RNG_CTRL_PD);
		//DDBG_PRINT("write val 0x%08x", val);
		io_write32(amba_trng_driver.rng_powerup_reg_va_base, val);

		DDBG_PRINT("ambarella_trng_init done");

		trng_driver_inited = 1;
	}
}

static void ambarella_trng_start_sample(void)
{
	io_write32(amba_trng_driver.rng_ctrl_reg_va,
			io_read32(amba_trng_driver.rng_ctrl_reg_va) | (1 << 1));
}

static void ambarella_trng_wait_sample_complete(void)
{

	while(io_read32(amba_trng_driver.rng_ctrl_reg_va) & (1 << 1))
		;
}

int ambarella_trng_read(unsigned int *pout, unsigned int read_size)
{
	ambarella_trng_init(0x00);

	read_size &= 0xfffffffc;

	while (read_size >= 16) {

		DDBG_PRINT("ambarella_trng_read begin, rng_reg_va_base %p, rng_ctrl_reg_va %p",
			(void *) amba_trng_driver.rng_reg_va_base,
			(void *) amba_trng_driver.rng_ctrl_reg_va);

		ambarella_trng_start_sample();
		ambarella_trng_wait_sample_complete();

		*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[0]);
		*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[1]);
		*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[2]);
		*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[3]);
		DDBG_PRINT("trng read 0x%08x 0x%08x 0x%08x 0x%08x", pout[-4], pout[-3], pout[-2], pout[-1]);
		read_size -= 16;
	}

	if (read_size) {

		DDBG_PRINT("ambarella_trng_read read remain %d", read_size);

		ambarella_trng_start_sample();
		ambarella_trng_wait_sample_complete();

		if (12 == read_size) {
			*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[0]);
			*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[1]);
			*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[2]);
			DDBG_PRINT("trng read 0x%08x 0x%08x 0x%08x", pout[-3], pout[-2], pout[-1]);
		} else if (8 == read_size) {
			*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[0]);
			*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[1]);
			DDBG_PRINT("trng read 0x%08x 0x%08x", pout[-2], pout[-1]);
		} else if (4 == read_size) {
			*pout++ = io_read32(amba_trng_driver.rng_data_reg_va[0]);
			DDBG_PRINT("trng read 0x%08x", pout[-1]);
		}
	}

	DDBG_PRINT("ambarella_trng_read end");

	return 0;
}


