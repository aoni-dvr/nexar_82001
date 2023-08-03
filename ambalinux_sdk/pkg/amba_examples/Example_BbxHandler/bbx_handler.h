/**
 * Copyright (C) 2017 Ambarella Corporation. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella Corporation.
 */
#ifndef __BBXHANDLER_H__
#define __BBXHANDLER_H__

#ifdef  __cplusplus
extern "C" {
#endif

#define BBX_MAX_ROI_AMOUNT (4)

typedef struct Bbx_Buffer_Setting_s_ {
    unsigned int MemType;
    unsigned int Size;
    unsigned int Align;
} Bbx_Buffer_Setting_s;

typedef struct Bbx_Buffer_Info_s_ {
    void *RawAddr;
    void *AlignedAddr;
    void *MmapedBase;
    unsigned long long MmapedSize;
    void *MmapedAddr;
} Bbx_Buffer_Info_s;

typedef struct Bbx_Info_s_ {
    unsigned int Flags;
    unsigned int Size;
    void *Addr;
} Bbx_Info_s;

typedef struct Bbx_Header_s_ {
    unsigned int Ver;
    unsigned int Roi[BBX_MAX_ROI_AMOUNT][4];
} Bbx_Header_s;

/**
 * create BBX buffer.
 * Setting  [in]    setting of BBX buffer
 * Buffer   [out]   the created buffer
 *
 * Return value:
 *       0: success
 *      -1: fail
 **/
int Bbx_Create_Buffer(Bbx_Buffer_Setting_s *Setting, Bbx_Buffer_Info_s *Buffer);

/**
 * free BBX buffer.
 * Buffer   [in]    the BBX buffer to free
 *
 * Return value:
 *       0: success
 *      -1: fail
 **/
int Bbx_Free_Buffer(Bbx_Buffer_Info_s *Buffer);

/**
 * Set BBX Header.
 * Header   [in]    the BBX Log Header
 *
 * Return value:
 *       0: success
 *      -1: fail
 **/
int Bbx_SetLogHeader(Bbx_Header_s *Header);

/**
 * update BBX data.
 * Info [in]    the BBX info (all the BBX) of a frame
 *
 * Return value:
 *       0: success
 *      -1: fail
 **/
int Bbx_Update(Bbx_Info_s *Info);

/**
 * Init BBX Handler.
 *
 * Return value:
 *       0: success
 *      -1: fail
 **/
int Bbx_Init(void);

/**
 * Release BBX Handler.
 *
 * Return value:
 *       0: success
 *      -1: fail
 **/
int Bbx_Release(void);

#ifdef  __cplusplus
}
#endif
#endif /* __BBXHANDLER_H__ */

