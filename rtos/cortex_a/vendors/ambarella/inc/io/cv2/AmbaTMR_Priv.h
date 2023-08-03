/**
 *  @file AmbaTMR_Priv.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Common Definitions & Constants for Inverval Timer APIs
 *
 */

#ifndef AMBA_TMR_PRIV_H
#define AMBA_TMR_PRIV_H

/*
 * Interval Timers
 */
#define AMBA_TIMER0             0U
#define AMBA_TIMER1             1U
#define AMBA_TIMER2             2U
#define AMBA_TIMER3             3U
#define AMBA_TIMER4             4U
#define AMBA_TIMER5             5U
#define AMBA_TIMER6             6U
#define AMBA_TIMER7             7U
#define AMBA_TIMER8             8U
#define AMBA_TIMER9             9U
#define AMBA_TIMER10            10U
#define AMBA_TIMER11            11U
#define AMBA_TIMER12            12U
#define AMBA_TIMER13            13U
#define AMBA_TIMER14            14U
#define AMBA_TIMER15            15U
#define AMBA_TIMER16            16U
#define AMBA_TIMER17            17U
#define AMBA_TIMER18            18U /* Reserved the timer for System Profiling */
#define AMBA_TIMER19            19U /* Reserved the Last Timer for OS tick */

#define AMBA_NUM_TIMER          20U

#define AMBA_TIMER_SYS_TICK     (AMBA_NUM_TIMER - 1U)
#define AMBA_TIMER_SYS_PROFILE  (AMBA_NUM_TIMER - 2U)

#endif /* AMBA_TMR_PRIV_H */
