/**
 * include/macro.S
 *
 * Author: Anthony Ginger <hfjiang@ambarella.com>
 *
 * History:
 *    2015/11/26 - [Cao Rongrong] Add ARMv8 supported
 *
 * Copyright (c) 2015 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
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
 *
 */

.macro _mov_imm16 _reg, _val, _shift
	.if (\_val >> \_shift) & 0xffff
		.if (\_val & (1 << \_shift - 1))
			movk	\_reg, (\_val >> \_shift) & 0xffff, LSL \_shift
		.else
			mov	\_reg, \_val & (0xffff << \_shift)
		.endif
	.endif
.endm

.macro mov_imm _reg, _val
	.if (\_val) == 0
		mov	\_reg, #0
	.else
		_mov_imm16	\_reg, (\_val), 0
		_mov_imm16	\_reg, (\_val), 16
		_mov_imm16	\_reg, (\_val), 32
		_mov_imm16	\_reg, (\_val), 48
	.endif
.endm

