/**
 *  @file AmbaRTSL_CPU_A32.asm
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
 *  @details CPU RTSL APIs implemented by A32 instruction set
 *
 */

        .text
        .arm

/*
 *  Get core number in Cortex-A53 processor
 */
.globl AmbaRTSL_CpuGetCoreID
AmbaRTSL_CpuGetCoreID:
        mrc     p15, 0, r3, cr0, cr0, 5
        and     r0, r3, #0xf00
#ifdef CONFIG_SOC_CV2FS
#ifdef CONFIG_XEN_SUPPORT /* TODO */
#else
        sub     r0, r0, #0x100
#endif /* CONFIG_XEN_SUPPORT */
#endif
        uxtb    r3, r3
        add     r0, r3, r0, lsr #6
        bx      lr
/*
 *  Get Cortex-A53 cluster id
 */
.globl AmbaRTSL_CpuGetClusterID
AmbaRTSL_CpuGetClusterID:
        mrc     p15, 0, r0, cr0, cr0, 5
        ubfx    r0, r0, #8, #1
        bx      lr

/*
 *  Read system control register
 */
.globl AmbaRTSL_CpuReadSysCtrl
AmbaRTSL_CpuReadSysCtrl:
        mrc     p15, 0, r0, cr1, cr0, 0
        bx      lr

/*
 *  Write system control register
 */
.globl AmbaRTSL_CpuWriteSysCtrl
AmbaRTSL_CpuWriteSysCtrl:
        mcr     p15, 0, r0, cr1, cr0, 0
        bx      lr
