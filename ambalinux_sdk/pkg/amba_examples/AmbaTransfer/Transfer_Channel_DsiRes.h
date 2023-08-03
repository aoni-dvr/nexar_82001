/**
 * Copyright (C) 2017 Ambarella Corporation. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella Corporation.
 */
#ifndef TRANSFER_CHANNEL_DSIRES_H
#define TRANSFER_CHANNEL_DSIRES_H

#ifdef  __cplusplus
extern "C" {
#endif

#define TRANSFER_CHANNEL_DSIRES_NAME "TRANSDSIRES"

#define TRANSFER_DSIRES_TYPE_HEADER      (0xA3BA0001U)
#define TRANSFER_DSIRES_TYPE_INFO        (0xA3BA0002U)

#define TRANSFER_DSIRES_DATA_10BITS (0U)
#define TRANSFER_DSIRES_DATA_10BITS_COMPRESSED (1U)

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef int INT32;

typedef struct {
    UINT32 Type;          //Pkg Type
    UINT32 Version;       //Pkg version
    UINT32 PayloadSize;   //Pkg Payload size, not include Pkg Header.
} TRANSFER_DSIRES_PKG_HEADER_s;

typedef struct {
    TRANSFER_DSIRES_PKG_HEADER_s Header;
    UINT8 Payload[1];   //Payload
} TRANSFER_DSIRES_PKG_s;

typedef struct {
    UINT32 Version;
    UINT32 DataFormat;
    UINT32 DataSize;
    UINT32 CapTs;
    UINT32 CapSeq;
    UINT32 CapScale;
    UINT32 Pitch;
    UINT32 Width;
    UINT32 Height;
} TRANSFER_DSIMAP_LISTHEADER_s;

typedef struct {
    TRANSFER_DSIMAP_LISTHEADER_s Header;
    //UINT8 Map[1];   //Payload
    void *Map;   //Payload
} TRANSFER_DSIMAP_LIST_s;


#ifdef  __cplusplus
}
#endif
#endif /* TRANSFER_CHANNEL_DSIRES_H */

