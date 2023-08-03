/*
 * Copyright (c) 2017-2021 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef STATIC_COUNTERS_DEFS_H_
#define STATIC_COUNTERS_DEFS_H_

//================================================================================================
//================================================================================================
#if (defined(CHIP_CV2))

// Counters 0..111 are reserved for IDSP/Coding ORC

#define CTR_mutex_visorc_reserved0              112   // Reserved for future expansion
#define CTR_mutex_visorc_reserved1              113   // Reserved for future expansion
#define CTR_mutex_visorc_reserved2              114   // Reserved for future expansion

#define CTR_mutex_loadtrace                     115   // Mutex used for loadtrace system
#define CTR_mutex_cvtask_message                116   // Mutex used for cvtask messages
#define CTR_mutex_tasklist                      117   // Mutex used for tasklist management
#define CTR_mutex_message_queue                 118   // Mutex used for message management
#define CTR_mutex_printf                        119   // Mutex used for printf buffer management

#define CTR_doorbell_vporc_vp0                  120   // VP thread doorbell
#define CTR_doorbell_vporc_fex                  121   // FEX thread doorbell
#define CTR_doorbell_vporc_feature_matching     122   // Feature matching thread doorbell
#define CTR_doorbell_vporc_spare                123   // VP/Spare thread doorbell
#define CTR_doorbell_vporc_reserved0            124   // (Reserved)
#define CTR_doorbell_vporc_reserved1            125   // (Reserved)
#define CTR_cavalry_early_exit                  126   // Cavalry early exit signal
#define CTR_doorbell_scheduler                  127   // Scheduler thread doorbell

//================================================================================================
//================================================================================================
#elif ((defined(CHIP_CV22)) || (defined(CHIP_CV25)) || (defined(CHIP_CV28)))

// Counters 0..111 are reserved for IDSP/Coding ORC

#define CTR_mutex_visorc_reserved0              112   // Reserved for future expansion
#define CTR_mutex_visorc_reserved1              113   // Reserved for future expansion
#define CTR_mutex_visorc_reserved2              114   // Reserved for future expansion

#define CTR_mutex_loadtrace                     115   // Mutex used for loadtrace system
#define CTR_mutex_cvtask_message                116   // Mutex used for cvtask messages
#define CTR_mutex_tasklist                      117   // Mutex used for tasklist management
#define CTR_mutex_message_queue                 118   // Mutex used for message management
#define CTR_mutex_printf                        119   // Mutex used for printf buffer management

#define CTR_doorbell_vporc_vp0                  120   // VP thread doorbell
#define CTR_doorbell_vporc_spare                121   // VP/Spare thread doorbell
#define CTR_doorbell_vporc_reserved0            122   // (Reserved)
#define CTR_doorbell_vporc_reserved1            123   // (Reserved)
#define CTR_doorbell_vporc_reserved2            124   // (Reserved)
#define CTR_doorbell_vporc_reserved3            125   // (Reserved)
#define CTR_cavalry_early_exit                  126   // Cavalry early exit signal
#define CTR_doorbell_scheduler                  127   // Scheduler thread doorbell

//================================================================================================
//================================================================================================
#elif ((defined(CHIP_CV2A)) || (defined(CHIP_CV22A)))

// Counters 0..111 are reserved for IDSP/Coding ORC

#define CTR_mutex_visorc_reserved0              112   // Reserved for future expansion
#define CTR_mutex_visorc_reserved1              113   // Reserved for future expansion
#define CTR_mutex_visorc_reserved2              114   // Reserved for future expansion

#define CTR_mutex_loadtrace                     115   // Mutex used for loadtrace system
#define CTR_mutex_cvtask_message                116   // Mutex used for cvtask messages
#define CTR_mutex_tasklist                      117   // Mutex used for tasklist management
#define CTR_mutex_message_queue                 118   // Mutex used for message management
#define CTR_mutex_printf                        119   // Mutex used for printf buffer management

#define CTR_doorbell_vporc_vp0                  120   // VP thread doorbell
#define CTR_doorbell_vporc_fex                  121   // VP/FEX thread doorbell
#define CTR_doorbell_safety                     122   // VP/Safety thread doorbell
#define CTR_doorbell_vporc_spare                123   // VP/Spare thread doorbell
#define CTR_doorbell_vporc_reserved0            124   // (Reserved)
#define CTR_doorbell_vporc_reserved1            125   // (Reserved)
#define CTR_cavalry_early_exit                  126   // Cavalry early exit signal
#define CTR_doorbell_scheduler                  127   // Scheduler thread doorbell

//================================================================================================
//================================================================================================
#elif ((defined(CHIP_CV5)) || (defined(CHIP_CV52)))

// Counters 0..239 are reserved for IDSP/Coding ORC

#define CTR_mutex_visorc_reserved0              240   // Reserved for future expansion
#define CTR_mutex_visorc_reserved1              241   // Reserved for future expansion
#define CTR_mutex_visorc_reserved2              242   // Reserved for future expansion

#define CTR_mutex_loadtrace                     243   // Mutex used for loadtrace system
#define CTR_mutex_cvtask_message                244   // Mutex used for cvtask messages
#define CTR_mutex_tasklist                      245   // Mutex used for tasklist management
#define CTR_mutex_message_queue                 246   // Mutex used for message management
#define CTR_mutex_printf                        247   // Mutex used for printf buffer management

#define CTR_doorbell_vporc_vp0                  248   // VP thread doorbell
#define CTR_doorbell_vporc_spare                249   // VP/Spare thread doorbell
#define CTR_doorbell_vporc_reserved0            250   // (Reserved)
#define CTR_doorbell_vporc_reserved1            251   // (Reserved)
#define CTR_doorbell_vporc_reserved2            252   // (Reserved)
#define CTR_doorbell_vporc_reserved3            253   // (Reserved)
#define CTR_cavalry_early_exit                  254   // Cavalry early exit signal
#define CTR_doorbell_scheduler                  255   // Scheduler thread doorbell

#elif (defined(CHIP_CV6))

// No counters available for CV6, but keep the mutex defines for ease of compilation

#define CTR_mutex_visorc_reserved0              0     // Unused in CV6, just kept for ease of compilation
#define CTR_mutex_visorc_reserved1              0     // Unused in CV6, just kept for ease of compilation
#define CTR_mutex_visorc_reserved2              0     // Unused in CV6, just kept for ease of compilation

#define CTR_mutex_loadtrace                     0     // Unused in CV6, just kept for ease of compilation
#define CTR_mutex_cvtask_message                0     // Unused in CV6, just kept for ease of compilation
#define CTR_mutex_tasklist                      0     // Unused in CV6, just kept for ease of compilation
#define CTR_mutex_message_queue                 0     // Unused in CV6, just kept for ease of compilation
#define CTR_mutex_printf                        0     // Unused in CV6, just kept for ease of compilation

#define CTR_doorbell_scheduler                  0     // Unused in CV6, just kept for ease of compilation
#define CTR_doorbell_vporc_fex                  0     // Unused in CV6, just kept for ease of compilation
#define CTR_doorbell_sub_sched_0                0     // Unused in CV6, just kept for ease of compilation
#define CTR_doorbell_vporc_spare                0     // Unused in CV6, just kept for ease of compilation

#else
#error "Unknown Chip Type defined - cannot compile"
#endif

#endif /* ?STATIC_COUNTERS_DEFS_H_ */

