/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <ambarella_def.h>
#include <platform_def.h>

	.global ambarella_rotpk
	.global ambarella_rotpk_raw
	.global ambarella_rotpk_end
	.global ambarella_embeded_rotpk_der
	.global ambarella_embeded_rotpk_der_end

	.section .data.ambarella_rotpk
ambarella_rotpk:
	/* DER header */
	.byte 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09
	.byte 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01
	.byte 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00
	.byte 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00
ambarella_rotpk_raw:
	/* Raw Public Key, filled with N(Public Key) stord in in OTP */
.rept 256
	.byte 0x00
.endr
	.byte 0x02, 0x03, 0x01, 0x00, 0x01
ambarella_rotpk_end:

.align 3
ambarella_embeded_rotpk_der:
#if defined(PLAT_CFG_ATF_EMBED_PUB_COT_ROOT)
#if defined(PLAT_CFG_PUB_COT_ROOT_DER)
	.incbin PLAT_CFG_PUB_COT_ROOT_DER
#else
	.int	0xdeadbeef, 0xdeadbeef
#endif
#endif
ambarella_embeded_rotpk_der_end:
