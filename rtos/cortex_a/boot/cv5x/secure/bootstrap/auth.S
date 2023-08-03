/**
 * @file auth.S
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 */

#include <AmbaBST.h>

/*
 * void auth_image(void *image, void *work_memory)
 * @param UST image pointer
 * @param work_memory for storing ECC public key
 */
#define auth_image	            0x20FFFF08E4 // please check OTP rom code

#define WORK_MEMORY_START       0x20E0030000
#define STACK_END_ADDRESS		0x20E0033000
#define OTP_CTRL1_ADDR          0x20E002F0A0

/*
 *	Image in DRAM
 *
 *  + --------------------- +
 *  |    UST header         |
 *  + --------------------- +
 *  |    data               |
 *  |    (UST body)         |
 *  + --------------------- +
 *  |    SIGN   -  64 byte  |
 *  |    (UST tail)         |
 *  + --------------------- +
 *
 */

.text

.global verify_signature

/*
 * x14 - Length of BLD/ATF binary
 * x20 - RCT base address
 * x22 - DRAM address of BLD/ATF binary
 */
verify_signature:
    ldr x24, =(AMBA_CORTEX_A76_SCRATCHPAD_S_BASE_ADDR)
    ldr w0,  [x24, #SCP_SEC_BOOT_STS_OFFSET]  /* Load AHB_SEC_BOOT_STS register from secure scratchpad */
    tbz w0, #0, verify_signature_end          /* branch out if not Secure Boot */

    # clean lower 16-bit of OTP_CTRL1_ADDR
    ldr x0, =OTP_CTRL1_ADDR
    ldr w1, [x0] 
    and	w1, w1, #0xffff0000
    str	w1, [x0]
    
	mov x0, x22                 // UST image pointer = BLD image pointer
	ldr x1, =WORK_MEMORY_START  // WORK_MEMORY_START

    # need to set X22 to 0 before calling auth_image()
    mov x22, #0

    ldr x20, =auth_image
    blr x20

    # auth_image won't return.
    # when success, it jumps to the image and execute
    # when failure, it loops and fill X7 with error code

verify_signature_end:

	ret