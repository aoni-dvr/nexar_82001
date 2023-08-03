/*
 * Copyright (c) 2018-2018 Ambarella, Inc.
 * 2018/06/01 - [Zhikan Yang] created file
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

/*================================================================================================*/
/* Directly imported from ambarella/private/cavalry_drv git repository                            */
/*                                                                                                */
/* File source : "cavalry_log.h"                                                                  */
/* File size   : 2106 bytes                                                                       */
/* File md5sum : 9641ae55df43a123fddaa4bca36fcfa9                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing: None                                                             */
/*================================================================================================*/

#ifndef __CAVALRY_LOG_H__
#define __CAVALRY_LOG_H__

#define CAVALRY_LOG_TIMER_PERIOD	(msecs_to_jiffies(10))

int cavalry_log_init(struct ambarella_cavalry *cavalry);
void cavalry_log_exit(struct ambarella_cavalry *cavalry);

int cavalry_log_start(struct ambarella_cavalry *cavalry, void __user *arg);
int cavalry_log_stop(struct ambarella_cavalry *cavalry, void __user *arg);
int cavalry_log_reset(struct ambarella_cavalry *cavalry);
ssize_t cavalry_log_read(struct file *filp, char __user *buffer, size_t count, loff_t *offp);


#endif //__CAVALRY_LOG_H__
