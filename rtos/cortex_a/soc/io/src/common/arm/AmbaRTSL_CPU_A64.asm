/**
 *  @file AmbaRTSL_CPU_A64.asm
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
 *  @details CPU RTSL APIs implemented by A64 instruction set
 *
 */

        .text

/*
 *  Get core number in Cortex-A53 processor
 */
.globl AmbaRTSL_CpuGetCoreID
AmbaRTSL_CpuGetCoreID:
        mrs     x1, mpidr_el1
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        lsr     x1, x1, #8
        and     x0, x1, #0xf
#else
        and     w0, w1, #0xf00
#ifdef CONFIG_SOC_CV2FS
        sub     w0, w0, #0x100
#endif
        and     w1, w1, #0xff
        add     w0, w1, w0, lsr #6
#endif
        ret

/*
 *  Get Cortex-A53 cluster id
 */
.globl AmbaRTSL_CpuGetClusterID
AmbaRTSL_CpuGetClusterID:
        mrs     x0, mpidr_el1
        ubfx    x0, x0, #8, #1
        ret

/*
 *  Read system control register
 */
.globl AmbaRTSL_CpuReadSysCtrl
AmbaRTSL_CpuReadSysCtrl:
        mrs     x0, currentel
        ubfx    x0, x0, #2, #2
        cmp     w0, #0x1
        b.eq    ReadSysCtrlEL1
        cmp     w0, #0x2
        b.eq    ReadSysCtrlEL2
        cmp     w0, #0x3
        b.eq    ReadSysCtrlEL3
        ret
ReadSysCtrlEL3:
        mrs     x0, sctlr_el3
        ret
ReadSysCtrlEL2:
        mrs     x0, sctlr_el2
        ret
ReadSysCtrlEL1:
        mrs     x0, sctlr_el1
        ret

/*
 *  Write system control register
 */
.globl AmbaRTSL_CpuWriteSysCtrl
AmbaRTSL_CpuWriteSysCtrl:
        mrs     x1, currentel
        ubfx    x1, x1, #2, #2
        cmp     w1, #0x1
        b.eq    WriteSysCtrlEL1
        cmp     w1, #0x2
        b.eq    WriteSysCtrlEL2
        cmp     w1, #0x3
        b.eq    WriteSysCtrlEL3
        mov     x0, #0x0
        ret
WriteSysCtrlEL3:
        msr     sctlr_el3, x0
        dsb     sy
        isb
        ret
WriteSysCtrlEL2:
        msr     sctlr_el2, x0
        dsb     sy
        isb
        ret
WriteSysCtrlEL1:
        msr     sctlr_el1, x0
        dsb     sy
        isb
        ret
