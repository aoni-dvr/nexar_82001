/*
 * ambarella_otp.h
 *
 * History:
 *	2018/05/24 - [Cao Rongrong] created file
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __AMBARELLA_OTP_H__
#define __AMBARELLA_OTP_H__

/* ==========================================================================*/

/* OTP_CTRL1_REG bit define */
#define READ_FSM_ENABLE			BIT(22)
#define READ_ENABLE			BIT(21)
#define DBG_READ_MODE			BIT(20)
#define FSM_WRITE_MODE			BIT(18)
#define PROG_ENABLE			BIT(17)
#define PROG_FSM_ENABLE			BIT(16)

/* OTP_OBSV_REG bit define */
#define WRITE_PROG_DONE			BIT(4)
#define WRITE_PROG_FAIL			BIT(3)
#define WRITE_PROG_RDY			BIT(2)
#define READ_OBSV_RDY			BIT(1)
#define READ_OBSV_DONE			BIT(0)

/* ==========================================================================*/

#define SECSP_RNG_CNT_OFFSET		0x00
#define SECSP_RNG_DATA0_OFFSET		0x04
#define SECSP_RNG_DATA1_OFFSET		0x08
#define SECSP_RNG_DATA2_OFFSET		0x0C
#define SECSP_RNG_DATA3_OFFSET		0x10
#define SECSP_RNG_DATA4_OFFSET		0xB0

#define SECSP_RNG_CNT_REG		SECURE_SCRATCHPAD_REG(SECSP_RNG_CNT_OFFSET)
#define SECSP_RNG_DATA0_REG		SECURE_SCRATCHPAD_REG(SECSP_RNG_DATA0_OFFSET)
#define SECSP_RNG_DATA1_REG		SECURE_SCRATCHPAD_REG(SECSP_RNG_DATA1_OFFSET)
#define SECSP_RNG_DATA2_REG		SECURE_SCRATCHPAD_REG(SECSP_RNG_DATA2_OFFSET)
#define SECSP_RNG_DATA3_REG		SECURE_SCRATCHPAD_REG(SECSP_RNG_DATA3_OFFSET)
#define SECSP_RNG_DATA4_REG		SECURE_SCRATCHPAD_REG(SECSP_RNG_DATA4_OFFSET)

#if defined(AMBARELLA_CV2)
#define OTP_CTRL1_OFFSET		0x760
#define OTP_CTRL2_OFFSET		0x764
#define OTP_OBSV_OFFSET			0x768
#define OTP_READ_DOUT_OFFSET		0x76C
#define OTP_CTRL1_REG			RCT_REG(OTP_CTRL1_OFFSET)
#define OTP_OBSV_REG			RCT_REG(OTP_OBSV_OFFSET)
#define OTP_READ_DOUT_REG		RCT_REG(OTP_READ_DOUT_OFFSET)
#else
#define OTP_CTRL1_OFFSET		0xA0
#define OTP_OBSV_OFFSET			0xA4
#define OTP_READ_DOUT_OFFSET		0xA8
#define OTP_CTRL1_REG			SECURE_SCRATCHPAD_REG(OTP_CTRL1_OFFSET)
#define OTP_OBSV_REG			SECURE_SCRATCHPAD_REG(OTP_OBSV_OFFSET)
#define OTP_READ_DOUT_REG		SECURE_SCRATCHPAD_REG(OTP_READ_DOUT_OFFSET)
#endif

#if defined(AMBARELLA_CV5)
#define OTP_BIT_SIZE 0x10000
#else
#define OTP_BIT_SIZE 0x8000
#endif

/* ==========================================================================*/

#endif

