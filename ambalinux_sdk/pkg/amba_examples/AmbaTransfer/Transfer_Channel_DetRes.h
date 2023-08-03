/**
 * Copyright (C) 2017 Ambarella Corporation. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella Corporation.
 */
#ifndef __AMBATRANSFER_CHANNEL_DECRES_H__
#define __AMBATRANSFER_CHANNEL_DECRES_H__

#ifdef  __cplusplus
extern "C" {
#endif

#define AMBATRANSFER_CHANNEL_DECRES_NAME "TRANSDETRES"

#define AMBADECRES_MAX_ROI_AMOUNT   (4)

#define AMBADECRES_PKG_MAGIC        (0xA3BA0000)
#define AMBADECRES_PKG_MAGIC_MASK   (0xFFFF0000)
#define IS_AMBADETRES_PKG(x) ((x & AMBADECRES_PKG_MAGIC_MASK) == AMBADECRES_PKG_MAGIC)

#define AMBADETRES_TYPE_HEADER      (0xA3BA0001)
#define AMBADETRES_TYPE_INFO        (0xA3BA0002)

typedef struct AmbaDetRes_PkgHeader_s_ {
    unsigned int Type;          //Pkg Type
    unsigned int Version;       //Pkg version
    unsigned int PayloadSize;   //Pkg Payload size, not include Pkg Header.
} AmbaDetRes_PkgHeader_s;

typedef struct AmbaDetRes_Pkg_s_ {
    AmbaDetRes_PkgHeader_s Header;
    unsigned char Payload[1];   //Payload
} AmbaDetRes_Pkg_s;

typedef struct AmbaBbxHeader_s_ {
    unsigned int Ver;
    unsigned int MsgCode;
    unsigned int Roi[AMBADECRES_MAX_ROI_AMOUNT][4];
    unsigned int DispRoi[AMBADECRES_MAX_ROI_AMOUNT];
} AmbaBbxHeader_s;

typedef struct AmbaBbxWindow_s_{
    unsigned short Cat;
    unsigned short Fid;
    unsigned int Score;
    unsigned short X;
    unsigned short Y;
    unsigned short W;
    unsigned short H;
} AmbaBbxWindow_s;

typedef struct AmbaBbxList_s_{
    unsigned int MsgCode;
    unsigned int Source;
    unsigned int CaptureTime;
    unsigned int FrameNum;
    unsigned int BbxAmount;       //How much AmbaBbxWindow_s in this list.
    AmbaBbxWindow_s Bbx[1];
} AmbaBbxList_s;

#ifdef  __cplusplus
}
#endif
#endif /* __AMBATRANSFER_CHANNEL_DECRES_H__ */

