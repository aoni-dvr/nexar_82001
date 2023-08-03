/*******************************************************************************
 * AmbaBST_OTP.asm
 *
 * Copyright (c) 2020 Ambarella, Inc.
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
 ******************************************************************************/

#include <AmbaBST.h>


.text

.global __AmbaSecureBootCheck

#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)

/* CV22/CV25/CV28/H32 */
__AmbaSecureBootCheck:

    ldr     x24, =(AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR - AMBA_CORTEX_A53_SECURE_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_AHB_PHYS_BASE_ADDR)
    ldr     w0,  [x24, #SCP_SEC_BOOT_STS_OFFSET]  /* Load AHB_SEC_BOOT_STS register from secure scratchpad */

    // overwrite POC[6] if secure boot flag = 1
    tbz     w0, #0, Done          /* branch out if not Secure Boot (w0[0] = 0) */
    ldr     x20, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR)
    ldr     w1, [x20, #RCT_POC_REG] // read RCT SYS_CONFIG_REG into W1
    orr     w1, w1, #0x40           // set W1[6] = POC[6] = 1
    str     w1, [x20, #RCT_POC_REG] // write W1 to RCT SYS_CONFIG_REG

Done:
    
    ret

#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

/* CV5/CV52 */
__AmbaSecureBootCheck:

    ldr     x24, =(AMBA_CORTEX_A76_SCRATCHPAD_S_BASE_ADDR)
    ldr     w0,  [x24, #SCP_SEC_BOOT_STS_OFFSET]  /* Load AHB_SEC_BOOT_STS register from secure scratchpad */

    // overwrite POC[6] if secure boot flag = 1
    tbz     w0, #0, Done          /* branch out if not Secure Boot (w0[0] = 0) */
    ldr     x20, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A76_APB_PHYS_BASE_ADDR)
    ldr     w1, [x20, #RCT_POC_REG] // read RCT SYS_CONFIG_REG into W1
    orr     w1, w1, #0x40           // set W1[6] = POC[6] = 1
    str     w1, [x20, #RCT_POC_REG] // write W1 to RCT SYS_CONFIG_REG

Done:
    
    ret   
   
#else
#error "AmbaBST_OTP.asm: undefined SOC"
#endif



