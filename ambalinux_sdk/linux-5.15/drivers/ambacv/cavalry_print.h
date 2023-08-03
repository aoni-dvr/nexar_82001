/*
 * Copyright (c) 2018-2018 Ambarella, Inc.
 * 2018/09/06 - [Tao Wu] created file
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
/* File source : "cavalry_print.h"                                                                */
/* File size   : 2268 bytes                                                                       */
/* File md5sum : 2a91cf65e42617af14f7be928c9ce5b5                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing: None                                                             */
/*================================================================================================*/

#ifndef __CAVALRY_PRINT_H__
#define __CAVALRY_PRINT_H__

enum {
	LOG_ERR = 0,
	LOG_INFO = 1,
	LOG_DEBUG = 2,
};

extern uint32_t debug_level;

#define prt_err(str, arg...)	do { \
	if (LOG_ERR <= debug_level ) {\
		printk(KERN_ERR "%s(%d): "str, __func__, __LINE__, ##arg); \
	} \
} while (0)

#define prt_info(str, arg...)	do { \
	if (LOG_INFO <= debug_level ) { \
		printk(KERN_INFO "%s(%d): "str, __func__, __LINE__, ##arg); \
	} \
} while (0)

#define prt_debug(str, arg...)	do { \
	if (LOG_DEBUG <= debug_level ) {\
		printk(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg); \
	} \
} while (0)

#define prt_trace(str, arg...)	do { \
	printk(KERN_DEBUG "%s(%d): "str, __func__, __LINE__, ##arg); \
} while (0)

#endif
