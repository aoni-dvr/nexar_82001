/**
 *  @file SvcRtcp.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 *  @details rtcp
 *
 */
#ifndef SVC_RTCP_H
#define SVC_RTCP_H

#define SVC_DV_NAME "Amba-DV"
#define SVC_DV_NAME_LENGTH 8
#define SVC_BYE_REASON "EOS"
#define SVC_BYE_REASON_LENGTH 4

#ifndef BIG_ENDIAN
#define BIG_ENDIAN
#endif

#define SR      200U
#define RR      201U
#define SDES    202U
#define BYE     203U
#define APP     204U


#define CNAME   1U
#define NAME    2U
#define EMAIL   3U
#define PHONE   4U
#define LOC     5U
#define TOOL    6U
#define NOTE    7U
#define PRIV    8U

typedef struct {
#ifdef BIG_ENDIAN
    UINT32 count:5;      //< SC or RC
    UINT32 padding:1;
    UINT32 version:2;
#else
    UINT32 version:2;
    UINT32 padding:1;
    UINT32 count:5;      //< SC or RC
#endif
    UINT8 pt;
    UINT16 Length;
} RTCP_HEADER_s;

typedef struct {
    UINT32 Ssrc;
    UINT32 NtpTimestampH;
    UINT32 NtpTimestampL;
    UINT32 RtpTimestamp;
    UINT32 PktCount;
    UINT32 OctetCount;
} RTCP_HEADER_SR_s;

typedef struct {
    UINT32 Ssrc;
    UINT8 FractLost;
    UINT8 PacketLost[3];
    UINT32 HSeqNo;
    UINT32 Jitter;
    UINT32 LastSr;
    UINT32 DelayLastSr;
} RTCP_REPORT_BLOCK_s;

typedef struct {
    UINT32 Ssrc;
    UINT8 AttrName;
    UINT8 Len;
    char Name[SVC_DV_NAME_LENGTH];
} RTCP_HEADER_SDES_s;

typedef struct {
    UINT32 Ssrc;
    UINT8 Length;
    char Reason[SVC_BYE_REASON_LENGTH];
} RTCP_HEADER_BYTE_s;

typedef struct {
    RTCP_HEADER_s SrHdr;        /* The header for the SR preamble */
    RTCP_HEADER_SR_s SrPkt;     /* The actual SR preamble */
    RTCP_HEADER_s PayloadHdr;   /* The header for the payload packet */
    union {
        RTCP_HEADER_SDES_s Sdes;    /* Payload for the source description packet */
        RTCP_HEADER_BYTE_s Bye;     /* Payload for the goodbye packet */
    } Payload;  /* Polymorphic payload */
} RTCP_SR_COMPOUND_s;

#endif /*SVC_RTCP_H*/
