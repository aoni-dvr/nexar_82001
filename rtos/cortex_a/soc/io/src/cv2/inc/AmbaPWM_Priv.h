/*
 * Copyright 2020, Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
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
 */

#ifndef AMBA_PWM_PRIV_H
#define AMBA_PWM_PRIV_H

#define AMBA_PWM_CHANNEL0               0U    /* PWM Channel-0 */
#define AMBA_PWM_CHANNEL1               1U    /* PWM Channel-1 */
#define AMBA_PWM_CHANNEL2               2U    /* PWM Channel-2 */
#define AMBA_PWM_CHANNEL3               3U    /* PWM Channel-3 */
#define AMBA_PWM_CHANNEL4               4U    /* PWM Channel-4 */
#define AMBA_PWM_CHANNEL5               5U    /* PWM Channel-5 */
#define AMBA_PWM_CHANNEL6               6U    /* PWM Channel-6 */
#define AMBA_PWM_CHANNEL7               7U    /* PWM Channel-7 */
#define AMBA_PWM_CHANNEL8               8U    /* PWM Channel-8 */
#define AMBA_PWM_CHANNEL9               9U    /* PWM Channel-9 */
#define AMBA_PWM_CHANNEL10              10U   /* PWM Channel-10 */
#define AMBA_PWM_CHANNEL11              11U   /* PWM Channel-11 */
#define AMBA_NUM_PWM_CHANNEL            12U   /* Number of PWM Channels */

#define AMBA_PWM_STEPPER_CHANNEL_A      0U    /* Stepper Channel-A (A0-A3) */
#define AMBA_PWM_STEPPER_CHANNEL_B      1U    /* Stepper Channel-B (B0-B3)*/
#define AMBA_PWM_STEPPER_CHANNEL_C      2U    /* Stepper Channel-C (C0-C3)*/
#define AMBA_NUM_PWM_STEPPER_CHANNEL    3U    /* Number of Stepper Channels */

#define AMBA_PWM_OPMODE_INDIVIDUAL      0U    /* PWM signal is configured individually */
#define AMBA_PWM_OPMODE_COMPLEMENTARY   1U    /* One PWM signal is a complementary of the configured one */

#define AMBA_NUM_PWM_STEPPER_CHANNEL_PIN            (4U)   /* [SC_n0 to SC_n3] */
#define AMBA_NUM_PWM_STEPPER_PATTERN                (64U)  /* Maximum number of pwm wave patterns. (the shortest pwm wave takes 2 ticks) */

#endif /* AMBA_PWM_PRIV_H */