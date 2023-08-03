/*
 * Copyright (c) 2019-2019 Ambarella, Inc.
 * 2019/09/16 - [Zhikan Yang] created file
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
/* File source : "cavalry_core_dump.h"                                                            */
/* File size   : 1966 bytes                                                                       */
/* File md5sum : afd7b71916c35d407a3acfe197befc21                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing: None                                                             */
/*================================================================================================*/

#ifndef __CAVALRY_CORE_DUMP_H__
#define __CAVALRY_CORE_DUMP_H__

int cavalry_core_dump_init(struct ambarella_cavalry *cavalry);
int cavalry_core_dump_exit(struct ambarella_cavalry *cavalry);
int cavalry_vp_core_dump(struct ambarella_cavalry *cavalry,
	uint32_t finish_dags, uint16_t dag_loop_index);
int cavalry_query_vp_core_dump(struct ambarella_cavalry *cavalry, void __user *arg);
#endif //__CAVALRY_CORE_DUMP_H__
