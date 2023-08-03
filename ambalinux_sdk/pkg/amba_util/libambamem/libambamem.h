/**
 * Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella Corporation.
 */
#ifndef __LIBAMBAMEM_H__
#define __LIBAMBAMEM_H__

#ifdef  __cplusplus
extern "C" {
#endif

#define DEV_MEM "/dev/ppm"

enum __AMBAMEM_TYPE_e__ {
	AMBAMEM_TYPE_HEAP = 0,
	AMBAMEM_TYPE_DSP,
	AMBAMEM_TYPE_AMOUNT
};

/**
 * writel memory addr "phyaddr" with "value"
 * [in] phyaddr - memory addr
 * [in] value   - 32bits value to write
 *
 * return 0 - ok
 *       -1 - NG
 **/
int writel(const unsigned long phyaddr, const unsigned long value);

/**
 * load file "src_file" to memory addr "phyaddr"
 * [in] src_file - file path
 * [in] phyaddr  - memory addr to write
 *
 * return 0 - ok
 *       -1 - NG
 **/
int loadbin(const char *src_file, const unsigned long phyaddr);

/**
 * read memory addr "phyaddr" length "op_size" bytes
 * [in] phyaddr - memory addr
 * [in] op_size - length to read
    result will be printed as
     0x03585000: 0x00000044
     0x03585004: 0x732f6e69
 *
 * return the first 32bits value read from phyaddr
 **/
unsigned int readl(const unsigned long phyaddr, const unsigned long op_size);

/**
 * save memory addr "phyaddr" length "length" to file "dst_file"
 * [in] dst_file - file path to write
 * [in] phyaddr  - memory addr to read
 * [in] length   - length to read & save into file
 *
 * return 0 - ok
 *       -1 - NG
 **/
int savebin(const char *dst_file, const unsigned long phyaddr, const unsigned long length);

#ifdef  __cplusplus
}
#endif
#endif /* __LIBAMBAMEM_H__ */
