/**
 * @file AmbaFusa.h
 *
 * @copyright Copyright (C) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights.  This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors.  You may not use,
 * reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed
 * license agreement or nondisclosure agreement with Ambarella, Inc. or
 * its authorized affiliates.  In the absence of such an agreement, you
 * agree to promptly notify and return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_FUSA_H
#define AMBA_FUSA_H

#define FUSA_RUN_EXCEPTION  0x01U
#define FUSA_RUN_ISA        0x02U
#define FUSA_RUN_CREG       0x04U
#define FUSA_RUN_BP         0x08U
#define FUSA_RUN_L2         0x10U
#define FUSA_RUN_ECCFI      0x20U

/**
 * Function Safety Library initialization and start.
 *
 * @param mask The bitmask of self-diags to enable for execution.
 * @param dtti The diagnostic test time interval (in ticks or equivalently ms)
 */
extern void AmbaCortexA53FusaInit(UINT32 mask, UINT32 dtti);

/*
 * Enable diags with mask bitmap. The change takes effect on next
 * round of thread execution.
 *
 * @param mask The bitmask of self-diags to enable for execution
 */
extern void AmbaCortexA53FusaEnable(UINT32 mask);

/**
 * Disable diags with mask bitmap. The change takes effect on next
 * round of thread execution.
 *
 * @param mask The bitmask of self-diags to enable for execution
 */
extern void AmbaCortexA53FusaDisable(UINT32 mask);

/**
 * Fault inject into libfusa_ca53 with mask bitmap. The change takes
 * effect on next round of thread execution.
 *
 * @param mask The bitmask of to fault inject into
 */
extern void AmbaCortexA53FusaInject(UINT32 mask);

#endif /* AMBA_FUSA_H */
