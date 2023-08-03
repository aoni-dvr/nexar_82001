/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright 2018 AMBARELLA
 */

#include <asm.S>

FUNC get_hw_unique_key , :
	smc #0
	ret
END_FUNC get_hw_unique_key
