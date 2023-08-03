/**
 *  @file SvcH264Subsession.c
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
 *  @details H264 subsession module
 *
 */

#include "AmbaTypes.h"
#include <AmbaKAL.h>
#include "AmbaPrint.h"
#include "AmbaWrap.h"

#include "SvcMediaSubsession.h"
#include "SvcStreamMgr.h"
#include "SvcBase64.h"
#include "AmbaUtility.h"
#include "AmbaSvcWrap.h"

#define RTSP_H264_MAX_FRAME_SIZE 0x00F00000U

#define H264_SPS_BASE64ENCODE_MAX_LENGTH ((SVC_RTSP_MAX_SPS_LENGTH * 4U) / 3U)
#define H264_PPS_BASE64ENCODE_MAX_LENGTH ((SVC_RTSP_MAX_PPS_LENGTH * 4U) / 3U)

typedef struct {
    UINT32 type : 5U;
} NALU_TYPE_s;

//Bits manipulation for generating SEI NALU
#define BITS_BYTE   8U

/* ---------------------------------------
 *  Begin of SEI recovery point insertion
 * --------------------------------------- */
static inline UINT32 OmitSei(MEDIA_SUBSESSION_s const * sub)
{
    UINT32 RetVal;

    if (sub->PayloadCtx.H264.SeiRecoveryLen > 0U) {
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }
    return RetVal;
}

static inline UINT32 Mod8(UINT32 x)
{
    return x & 0x07U;
}

static inline void AddBit(UINT8* pBitsBuffer, UINT32 *pLastBitsInBuffer, UINT32 Val)
{
    UINT32 BitPosInByte, BytePos;
    UINT32 ValTmp;

    // index the current byte
    BytePos = *pLastBitsInBuffer >> 3U;
    // first bit to add is on the left
    BitPosInByte = 7U - Mod8(*pLastBitsInBuffer);

    // get the byte value from buffer
    ValTmp = pBitsBuffer[BytePos];

    // change the bit
    if(Val > 0U){
        ValTmp = (ValTmp |  ((UINT32)0x01U << BitPosInByte));
    } else {
        ValTmp = (ValTmp & ~((UINT32)0x01U << BitPosInByte));
    }

    // save byte to buffer
    pBitsBuffer[BytePos] = (UINT8)ValTmp;

    (*pLastBitsInBuffer)++;
}


static inline UINT32 GetBitNum(UINT32 Val, UINT32 Index)
{
    UINT32 Lrc = 0U;

    UINT32 lmask = ((UINT32)0x01U << Index);
    if ((Val & lmask) > 0U) {
        Lrc = 1U;
    }

    return Lrc;
}

static inline UINT32 AddBits(UINT8* pBitsBuffer, UINT32 *pLastBitsInBuffer, UINT32 Val, UINT32 NumBits)
{
    UINT32 Bit = 0U;
    UINT32 Index = NumBits;
    UINT32 RetVal = OK;

    if((NumBits == 0U) || (NumBits > 32U)){
        AmbaPrint_PrintStr5("Error: number of bits must be [1:32]\n", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        while(Index != 0U){
            Index--;
            Bit = GetBitNum(Val, Index);
            AddBit(pBitsBuffer, pLastBitsInBuffer, Bit);
        }
    }

    return RetVal;
}

static UINT32 GenerateRecoveryPoint(char* pBuffer, UINT32 RecoveryFrameCnt)
{
    const UINT32 SeiHeaderSize = 3U;
    const UINT32 RbspTrailingBits = 1U; /*1 byte*/
    UINT32 PayloadSize;
    UINT32 NumOfBits, LeadingZeroBits;
    UINT32 PktSize;
    UINT8* pSeiRecovery;
    UINT32 LastBitsInBuffer = 0U;
    DOUBLE LeadingZeroBitsDouble = 0.0;
    UINT32 RetVal;
    UINT32 AddBitsRetSum = 0;

    // generate SEI pkt
    if (AmbaWrap_log2((DOUBLE)RecoveryFrameCnt + 1.0, &LeadingZeroBitsDouble) != OK) {
        AmbaPrint_PrintStr5("fail to call AmbaWrap_log2", NULL, NULL, NULL, NULL, NULL);
    }
    LeadingZeroBits = (UINT32)LeadingZeroBitsDouble;

    NumOfBits  = 2U * LeadingZeroBits;
    NumOfBits += 1U; /*recovery_frame_cnt       */
    NumOfBits += 1U; /*exact match flag         */
    NumOfBits += 1U; /*broken_link_flag         */
    NumOfBits += 2U; /*changing_slice_group_idc */

    PayloadSize = NumOfBits >> 3U; // divide by 8
    PayloadSize = (Mod8(NumOfBits) > 0U) ? (PayloadSize + 1U) : PayloadSize; // if mod(8) > 0, plus one

    PktSize = (SeiHeaderSize + PayloadSize) + RbspTrailingBits;
    if (PktSize > MAX_SEI_RECOVERY_PKT_LENGTH) {
        AmbaPrint_PrintStr5("[SvcH264Subsession]<GenerateRecoveryPoint>: pkt_size too big", NULL, NULL, NULL, NULL, NULL);
        RetVal = 0U;
    } else {
        pSeiRecovery = (UINT8*) pBuffer;

        if (AmbaWrap_memset(pBuffer, 0 , sizeof(PktSize))!= 0U) { }
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, 0x06U, BITS_BYTE); // nal = SEI
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, 0x06U, BITS_BYTE); // sei type = recovery point
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, PayloadSize, BITS_BYTE);
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, 0x00U, LeadingZeroBits);
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, RecoveryFrameCnt + 1U, LeadingZeroBits + 1U);
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, 0x00U, 1U); // exact match flag
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, 0x00U, 1U); // broken_link_flag
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, 0x00U, 2U); // changing_slice_group_idc
        //rbsp stop bit
        AddBit(pSeiRecovery, &LastBitsInBuffer, 1U);
        NumOfBits++;
        while(Mod8(NumOfBits) != 0U){
            AddBit(pSeiRecovery, &LastBitsInBuffer, 0U);
            NumOfBits++;
        }
        // rbsp trailing bits
        AddBitsRetSum += AddBits(pSeiRecovery, &LastBitsInBuffer, 0x80U, 8U);
        RetVal = PktSize;

        if(AddBitsRetSum != OK) {
            AmbaPrint_PrintUInt5("[SvcH264Subsession]<GenerateRecoveryPoint> AddBits Failed!",
                0U, 0U, 0U, 0U, 0U);
            RetVal = 0U;
        }
    }
    return RetVal;
}

static void InsertRecoveryPoint(MEDIA_SUBSESSION_s *pSub)
{
    RTP_PAYLOAD_INFO_s PayloadInfo;

    PayloadInfo.Buf = (UINT8*) pSub->PayloadCtx.H264.SeiRecoveryPkt;
    PayloadInfo.Len = pSub->PayloadCtx.H264.SeiRecoveryLen;
    PayloadInfo.TimeStamp = pSub->CurTimestamp;
    PayloadInfo.Maker = 0;
    if(SvcRtpSink_SendPacket(&pSub->Rtp, &PayloadInfo) != OK) {
        AmbaPrint_PrintUInt5("[SvcH264Subsession]<InsertRecoveryPoint> SvcRtpSink_SendPacket Failed!", 0U, 0U, 0U, 0U, 0U);
    }
}

/* ---------------------------------------
 *  End of SEI recovery point insertion
 * --------------------------------------- */

static const char* GetAuxSdp(MEDIA_SUBSESSION_s* thiz)
{
    UINT32 i;
    UINT32 FmtpFmtSize;
    char Rsps[H264_SPS_BASE64ENCODE_MAX_LENGTH];
    char Rpps[H264_PPS_BASE64ENCODE_MAX_LENGTH];
    char* pSpsBase64 = NULL;
    char* pPpsBase64 = NULL;
    UINT32 Ret;
    char Sps[SVC_RTSP_MAX_SPS_LENGTH];
    UINT32 SpsLen = 0;
    char Pps[SVC_RTSP_MAX_PPS_LENGTH];
    UINT32 PpsLen = 0;
    char const *pSps;
    char const *pPps;
    UINT32 ProfileId = 0;
    AMBA_STREAM_s *pSvcStream;
    char const *pRetString;
    char const * pFmtpFmt = "a=fmtp:%d packetization-mode=1"
                    ";profile-level-id=%06X"
                    ";sprop-parameter-sets=%s,%s\r\n";
    void const *pTmpPointer;

    if (AmbaWrap_memset(Sps, 0, SVC_RTSP_MAX_SPS_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(Pps, 0, SVC_RTSP_MAX_PPS_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(Rsps, 0, H264_SPS_BASE64ENCODE_MAX_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(Rpps, 0, H264_PPS_BASE64ENCODE_MAX_LENGTH)!= 0U) { }

    if (thiz->AuxSdpLine != NULL) {
        pRetString = thiz->AuxSdpLine;
    } else {
        AmbaMisra_TypeCast(&pSvcStream, &thiz->Streamer);
        Ret = SvcRtspStrmGetSpsPps(pSvcStream, Sps, &SpsLen,
                                Pps, &PpsLen,
                                &ProfileId);
        if ((Ret != OK)
            || (SpsLen > SVC_RTSP_MAX_SPS_LENGTH)
            || (PpsLen > SVC_RTSP_MAX_PPS_LENGTH)) { // our source isn't ready
            pRetString = NULL;
        } else {
            // remove padding byte before base64_encode
            for (i = 0U; i < 3U; i++) {
                if ('\0' == Sps[SpsLen - 1U]) {
                    SpsLen--;
                } else {
                    break;
                }
            }
            for (i = 0U; i < 3U; i++) {
                if ('\0' == Pps[PpsLen - 1U]) {
                    PpsLen--;
                } else {
                    break;
                }
            }

            // Set up the "a=fmtp:" SDP line for this stream:
            if (('\0' == Sps[0U]) && ('\0' == Sps[1U]) && ('\0' == Sps[2U]) && ('\1' == Sps[3U]) &&
                ('\0' == Pps[0U]) && ('\0' == Pps[1U]) && ('\0' == Pps[2U]) && ('\1' == Pps[3U])) {//Skip NAL Header (00 00 00 01)
                pSps = &Sps[4U];
                pPps = &Pps[4U];
                SpsLen = SpsLen - 4U;
                PpsLen = PpsLen - 4U;
            } else {
                pSps = Sps;
                pPps = Pps;
            }

            Ret  = SvcBase64_Encode(pSps, SpsLen, Rsps, sizeof(Rsps));
            Ret += SvcBase64_Encode(pPps, PpsLen, Rpps, sizeof(Rpps));

            if(OK != Ret) {
                pRetString = NULL;
            } else {
                pSpsBase64 = Rsps;
                pPpsBase64 = Rpps;

                FmtpFmtSize  = AmbaUtility_StringLength(pFmtpFmt);
                FmtpFmtSize += 3U; /* max char len */
                FmtpFmtSize += 6U; /* 3 bytes in hex */
                FmtpFmtSize += AmbaUtility_StringLength(pSpsBase64);
                FmtpFmtSize += AmbaUtility_StringLength(pPpsBase64);

                pTmpPointer = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, FmtpFmtSize + 1U);
                AmbaMisra_TypeCast(&(thiz->AuxSdpLine), &pTmpPointer);

                if (thiz->AuxSdpLine == NULL) {
                    AmbaPrint_PrintUInt5("GetAuxSdp: malloc sdp_len(%u) fail", \
                                    FmtpFmtSize, 0U, 0U, 0U, 0U);
                    pRetString = NULL;
                } else {
                    UINT32 RetVal;

                    if (AmbaWrap_memset(thiz->AuxSdpLine, 0, FmtpFmtSize)!= 0U) { }

                    // RetVal = AmbaAdvSnPrint(thiz->AuxSdpLine, FmtpFmtSize, pFmtpFmt, 96U + thiz->TrackNum - 1U, ProfileId, pSpsBase64, pPpsBase64);

                    {
                        char    *StrBuf = thiz->AuxSdpLine;
                        UINT32  BufSize = FmtpFmtSize;
                        UINT32  CurStrLen;

                        AmbaUtility_StringAppend(StrBuf, BufSize, "a=fmtp:");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(96ULL + (UINT64)thiz->TrackNum - 1ULL), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, " packetization-mode=1;profile-level-id=");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            char    NumBuf[16];
                            UINT32  NumLen;

                            if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                            (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(ProfileId), 16U);
                            NumLen = AmbaUtility_StringLength(NumBuf);

                            while (NumLen < 6UL) {
                                AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                                NumLen++;
                            }

                            AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, ";sprop-parameter-sets=");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (pSpsBase64));
                        AmbaUtility_StringAppend(StrBuf, BufSize, ",");
                        AmbaUtility_StringAppend(StrBuf, BufSize, (pPpsBase64));
                        AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");

                        RetVal = AmbaUtility_StringLength(StrBuf);


                    }

                    AmbaPrint_PrintStr5("%s, aux sdp %s", __func__, thiz->AuxSdpLine, NULL, NULL, NULL);
                    AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);

                    pRetString = thiz->AuxSdpLine;
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pSpsBase64);
    AmbaMisra_TouchUnused(pPpsBase64);

    return pRetString;
}

static const char* GetSdpLines(void const * thiz)
{
    MEDIA_SUBSESSION_s *pThiz;
    const char *pRetString;
    void const *pTmpPointer;

    AmbaMisra_TypeCast(&pThiz, &thiz);

    if(NULL == pThiz) {
        AmbaPrint_PrintStr5("GetSdpLines: thiz == NULL", NULL, NULL, NULL, NULL, NULL);
        pRetString = NULL;
    } else if (NULL == pThiz->SdpLines) {
        char const* pMediaType = "video";
        char const* pSdpServer = "0.0.0.0";
        UINT32 RtpPayloadType = 96U + pThiz->TrackNum - 1U;
        char const* const pSdpFmt =
            "m=%s %u RTP/AVP %d\r\n"
            "c=IN IP4 %s\r\n"
            "b=AS:%u\r\n"
            "a=rtpmap:%u H264/90000\r\n"
            "%s"
            "a=control:%s\r\n";
        UINT32 SdpFmtSize;
        char const* pAuxSDPLine = GetAuxSdp(pThiz);

        if (NULL == pAuxSDPLine) {
            AmbaPrint_PrintStr5("GetSdpLines: auxSDPLine == NULL", NULL, NULL, NULL, NULL, NULL);
        }

        SdpFmtSize  = AmbaUtility_StringLength(pSdpFmt);
        SdpFmtSize += AmbaUtility_StringLength(pMediaType);
        SdpFmtSize += 5U; /* max short len */
        SdpFmtSize += 3U; /* max char len */
        SdpFmtSize += AmbaUtility_StringLength(pSdpServer);
        SdpFmtSize += 20U; /* max int len */
        SdpFmtSize += 3U;
        SdpFmtSize += ((NULL == pAuxSDPLine)? 0U:AmbaUtility_StringLength(pAuxSDPLine));
        SdpFmtSize += AmbaUtility_StringLength(SvcMediaSubsession_GetTrackID(pThiz));

        pTmpPointer = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, SdpFmtSize + 1U);
        AmbaMisra_TypeCast(&pThiz->SdpLines, &pTmpPointer);

        if (NULL == pThiz->SdpLines) {
            AmbaPrint_PrintUInt5("GetSdpLines: malloc sdp_len(%u) fail", \
                            SdpFmtSize, 0U, 0U, 0U, 0U);
            pRetString = NULL;
        } else {
            UINT32 RetVal;

            if (AmbaWrap_memset(pThiz->SdpLines, 0, SdpFmtSize)!= 0U) { }

            {
                char    *StrBuf = pThiz->SdpLines;
                UINT32  BufSize = SdpFmtSize;
                UINT32  CurStrLen;

                AmbaUtility_StringAppend(StrBuf, BufSize, "m=");
                AmbaUtility_StringAppend(StrBuf, BufSize, pMediaType);
                AmbaUtility_StringAppend(StrBuf, BufSize, " ");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)0U, 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, " RTP/AVP ");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(UINT8)RtpPayloadType, 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\nc=IN IP4 ");
                AmbaUtility_StringAppend(StrBuf, BufSize, pSdpServer);
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\nb=AS:");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)10000U, 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\na=rtpmap:");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(UINT8)RtpPayloadType, 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, " H264/90000\r\n");
                AmbaUtility_StringAppend(StrBuf, BufSize, (pAuxSDPLine==NULL)? "":pAuxSDPLine);
                AmbaUtility_StringAppend(StrBuf, BufSize, "a=control:");
                AmbaUtility_StringAppend(StrBuf, BufSize, SvcMediaSubsession_GetTrackID(pThiz));
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");

                RetVal = AmbaUtility_StringLength(StrBuf);
            }

            AmbaPrint_PrintStr5("%s, sdp %s", __func__, pThiz->SdpLines, NULL, NULL, NULL);
            AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);

            SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_SDP_LINE_BUF, pThiz->AuxSdpLine);
            pThiz->AuxSdpLine = NULL;
            pRetString = pThiz->SdpLines;
        }
    } else {
        pRetString = pThiz->SdpLines;
    }

    return pRetString;
}

// when start-code round across ring-buff
static UINT8* GetH264NextNALAddress(UINT8 const * pBuf, UINT8 const * pBase, UINT8 const* pLimit)
{
    UINT32 const * pPtr;
    UINT8 StartCode[4U];
    UINT8 const *pStartCode = StartCode;
    UINT32 IndexI, IndexJ, RoundIdx;
    ULONG LimitAddr, BufAddr, BaseAddr;
    UINT8 *pRetPtr;

    if (AmbaWrap_memset(StartCode, 0, sizeof(StartCode))!= 0U) { }
    AmbaMisra_TypeCast(&LimitAddr, &pLimit);
    AmbaMisra_TypeCast(&BufAddr, &pBuf);
    RoundIdx = (LimitAddr - BufAddr) / sizeof(pLimit[0]);

    AmbaPrint_PrintUInt5("=========Warning========= first NAL start-code rounding, roundIdx(%u)", RoundIdx, 0U, 0U, 0U, 0U);
    //AmbaPrint_PrintUInt5("base=%x, limit=%x", base, limit, 0U, 0U, 0U);
    for (IndexI = 0U; IndexI < RoundIdx; IndexI++) {
        StartCode[IndexI] = pBuf[IndexI];
    }
    for (IndexJ = 0U; IndexJ < (4U - RoundIdx); IndexJ++) {
        StartCode[IndexI] = pBase[IndexJ];
        IndexI++;
    }
    AmbaMisra_TypeCast(&pPtr, &pStartCode);
    if (*pPtr != 0x01000000U) {
        AmbaPrint_PrintStr5("cannot find start code", NULL, NULL, NULL, NULL, NULL);
        pRetPtr = NULL;
    } else {
        AmbaMisra_TypeCast(&BaseAddr, &pBase);
        BaseAddr = BaseAddr + (4U - RoundIdx);
        AmbaMisra_TypeCast(&pRetPtr, &BaseAddr);
    }
    return pRetPtr;
}

static void H264SubsessionFUA(MEDIA_SUBSESSION_s* pSub, UINT8 *pBuf,
                            UINT32 Len, UINT32 Mtu, UINT32 Marker)
{
    UINT32 FragLen;
    UINT32 Remain = Len;
    UINT8 const *pNalu = pBuf;
    UINT8 *pPayload = &pBuf[1U];// skip the NAL byte
    UINT8 FuaIndicator = (pNalu[0U] & 0xe0U) | 28U;
    UINT8 FuaHeader = pNalu[0U] & 0x1fU;
    UINT8 Start = 1U;
    UINT8 Fua[2U];   // fua_indicator + fua_header
    UINT8 *pTmp;
    RTP_PAYLOAD_INFO_s PayloadInfo;
    RTP_IO_VEC_s Vec[3U];
    ULONG BufferLimitAddr, PayloadInfoAddr;

    Remain--;
    while(Remain > 0U) {
        Fua[0U] = FuaIndicator;
        if(1U == Start) {
            Fua[1U] = FuaHeader | (1U << 7U);
            Start = 0U;
        } else {
            Fua[1U] = FuaHeader;
        }
        if(Remain > (Mtu - 2U)) {
            FragLen = Mtu - 2U;
        } else {
            FragLen = Remain;
            Fua[1U] = FuaHeader | (1U << 6U);     //End
        }
        //take care ring buf case
        pTmp = pSub->pBufferLimit;
        AmbaMisra_TypeCast(&BufferLimitAddr, &pTmp);
        AmbaMisra_TypeCast(&PayloadInfoAddr, &pPayload);
        if ((PayloadInfoAddr + FragLen) >= BufferLimitAddr){ //truncated
            PayloadInfo.Len = FragLen;
            PayloadInfo.TimeStamp = pSub->CurTimestamp;// - priv->ts_offset;
            PayloadInfo.Maker = ((Remain <= FragLen) && (Marker == 1U)) ? 1U : 0U;
            Vec[0U].base = Fua;
            Vec[0U].Len = 2U;
            Vec[1U].base = pPayload;
            Vec[1U].Len = BufferLimitAddr - PayloadInfoAddr;
            Vec[2U].base = pSub->pBufferBase;
            Vec[2U].Len = PayloadInfo.Len - Vec[1U].Len;
            if(ERR_NA == SvcRtpSink_SendPacketVector(&pSub->Rtp, &PayloadInfo, &Vec[0U], 3U)) {
                AmbaPrint_PrintUInt5("[SvcH264Subsession]<H264SubsessionFUA>SendPacketVector Failed!",
                    0U, 0U, 0U, 0U, 0U);
            }
            pPayload = &(pSub->pBufferBase[Vec[2U].Len]);
            Remain -= FragLen;
        } else {
            PayloadInfo.Buf = pPayload;
            PayloadInfo.Len = FragLen + 2U;
            PayloadInfo.TimeStamp = pSub->CurTimestamp;// - priv->ts_offset;
            PayloadInfo.Maker = ((Remain <= FragLen) && (1U == Marker)) ? 1U : 0U;
            Vec[0U].base = Fua;
            Vec[0U].Len = 2U;
            Vec[1U].base = pPayload;
            Vec[1U].Len = FragLen;
            if(ERR_NA == SvcRtpSink_SendPacketVector(&pSub->Rtp, &PayloadInfo, &Vec[0U], 2U)) {
                AmbaPrint_PrintUInt5("[SvcH264Subsession]<H264SubsessionFUA>SendPacketVector Failed!!",
                    0U, 0U, 0U, 0U, 0U);
            }

            pPayload = &(pPayload[FragLen]);
            Remain -= FragLen;
        }
    }
    return;
}

static void SendPacket(MEDIA_SUBSESSION_s* pSub, UINT8* pStartAddr, UINT32 Length, UINT32 Mark)
{
    UINT8               *pTmp;
    ULONG               StartAddr, BufferLimitAddr;
    RTP_PAYLOAD_INFO_s  PayloadInfo;
    RTP_IO_VEC_s        Vec[2U];

    AmbaMisra_TypeCast(&StartAddr, &pStartAddr);
    pTmp = pSub->pBufferLimit;
    AmbaMisra_TypeCast(&BufferLimitAddr, &pTmp);

    if (Length > RTP_OVER_UDP_MAX) {
        H264SubsessionFUA(pSub, pStartAddr, Length, RTP_OVER_UDP_MAX, Mark);
    } else if ((StartAddr + Length) > BufferLimitAddr){
        Vec[0U].base = pStartAddr;
        Vec[0U].Len = BufferLimitAddr - StartAddr;
        Vec[1U].base = pSub->pBufferBase;
        Vec[1U].Len = Length - Vec[0U].Len;
        PayloadInfo.Len = Length;
        PayloadInfo.TimeStamp = pSub->CurTimestamp;// need to add - priv->ts_offset;
        PayloadInfo.Maker = (UINT8)Mark;
        if(ERR_NA == SvcRtpSink_SendPacketVector(&pSub->Rtp, &PayloadInfo, &Vec[0U], 2U)) {
            AmbaPrint_PrintUInt5("[SvcH264Subsession]<SendPacket>SendPacket Failed!",
                    0U, 0U, 0U, 0U, 0U);
        }
    } else {
        PayloadInfo.Buf = pStartAddr;
        PayloadInfo.Len = Length;
        PayloadInfo.TimeStamp = pSub->CurTimestamp; // need to add - thiz->ts_offset;
        PayloadInfo.Maker = (UINT8)Mark;
        if(ERR_NA == SvcRtpSink_SendPacket(&pSub->Rtp, &PayloadInfo)) {
            AmbaPrint_PrintUInt5("[SvcH264Subsession]<SendPacket>SendPacket Failed!!",
                    0U, 0U, 0U, 0U, 0U);
        }
    }

    // manually insert SEI recovery point after sps/pps
    // workaround for uCode doesn't generate correct recovery_frame_cnt
    if (((pStartAddr[0] & 0x1FU) == 0x01U) && (pSub->PayloadCtx.H264.SeiRecoveryLen > 0U)) {
        InsertRecoveryPoint(pSub);
    }
}

static inline UINT32 Uint8ArrayToUint32(UINT8 H2L0, UINT8 H2L1, UINT8 H2L2, UINT8 H2L3)
{
    UINT32 RetVal = H2L0;
    RetVal <<= 8U;
    RetVal |= H2L1;
    RetVal <<= 8U;
    RetVal |= H2L2;
    RetVal <<= 8U;
    RetVal |= H2L3;

    return RetVal;
}

static void RingBufPacketize(MEDIA_SUBSESSION_s* pSub, UINT8 *pBuf, UINT32 Len, UINT32 Mark)
{
    UINT32 const* pPtr;
    UINT8* pLastStart;
    UINT8  *p, *pTmp;
    UINT32 w;
    UINT32 NALsize;
    NALU_TYPE_s    Nalu;
    UINT8* pEndAddr;
    ULONG BufAddr, BufferLimitAddr, BufferBaseAddr, EndAddr, pAddr, LastStartAddr;

    AmbaMisra_TypeCast(&BufAddr, &pBuf);
    pTmp = pSub->pBufferBase;
    AmbaMisra_TypeCast(&BufferBaseAddr, &pTmp);
    pTmp = pSub->pBufferLimit;
    AmbaMisra_TypeCast(&BufferLimitAddr, &pTmp);

    if ((BufAddr + 4U) >= BufferLimitAddr) {                //first NAL start-code rounding
        pLastStart = GetH264NextNALAddress(pBuf, pSub->pBufferBase, pSub->pBufferLimit);
    } else {
        // find start code
        AmbaMisra_TypeCast(&pPtr, &pBuf);
        if (0x01000000U != *pPtr) {
            AmbaPrint_PrintStr5("cannot find start code", NULL, NULL, NULL, NULL, NULL);
            pLastStart = NULL;
        } else {
            // skip the start code
            pLastStart = &pBuf[4U];
        }
    }
    p = pLastStart;

    if (p != NULL) {
        // this AU lies on ring-buf boundary, endAddr must accross BufferLimit
        EndAddr = BufferBaseAddr + (Len - (BufferLimitAddr - BufAddr));
        AmbaMisra_TypeCast(&pEndAddr, &EndAddr);

        // every frame only have one slice nalu, the slice nalu alway be the last nalu.
        if (AmbaWrap_memcpy(&Nalu, pLastStart, 1U)!= 0U) { }
        if ((Nalu.type > 0U) && (Nalu.type < 6U)) { //it's slice nalu
            EndAddr -= 4U;
            AmbaMisra_TypeCast(&p, &EndAddr);
        }

        AmbaMisra_TypeCast(&pAddr, &p);
        AmbaMisra_TypeCast(&LastStartAddr, &pLastStart);
        AmbaMisra_TypeCast(&EndAddr, &pEndAddr);
        AmbaMisra_TypeCast(&BufAddr, &pBuf);
        pTmp = pSub->pBufferLimit;
        AmbaMisra_TypeCast(&BufferLimitAddr, &pTmp);

        while (((pAddr < LastStartAddr) && (pAddr < (EndAddr - 4U)))
            || ((LastStartAddr > BufAddr) && (pAddr >= LastStartAddr))
            || ((LastStartAddr < BufAddr) && (pAddr < (EndAddr - 4U)))) { // searching for next start-code
            if ((pAddr + 3U) < BufferLimitAddr) {
                // combine p[0]<<24 | p[1]<<16 | p[2]<<8 | p[3] to w.
                w = Uint8ArrayToUint32(p[0U], p[1U], p[2U], p[3U]);

                if ((w == 0x00000001U) || ((w & 0xffffff00U) == 0x00000100U)) {//got next nalu, p point to next start-code
                    if( ((0x06U == Nalu.type) && (1U == OmitSei(pSub))) //omit SEI in Intra Refresh mode
                      || (0x1aU == Nalu.type)                           //omit AMBA proprietary GOP Header
                      || (0x09U == Nalu.type)) {                        //omit AU-delimiter
                        // Do Nothing.
                    } else {
                        if(pAddr >= LastStartAddr) {
                            SendPacket(pSub, pLastStart, pAddr - LastStartAddr, 0U);
                        } else {
                            NALsize = (BufferLimitAddr - LastStartAddr) + (pAddr - BufferBaseAddr);
                            SendPacket(pSub, pLastStart, NALsize, 0U);
                        }
                    }

                    if(0x01U == p[2U]) {
                        pAddr += 3U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else {
                        // skip 4, for next search
                        pAddr = ((pAddr + 4U) == BufferLimitAddr) ? BufferBaseAddr : (pAddr + 4U);
                        AmbaMisra_TypeCast(&p, &pAddr);
                    }
                    pLastStart = p;
                    AmbaMisra_TypeCast(&LastStartAddr, &pLastStart);

                    if (AmbaWrap_memcpy(&Nalu, &pLastStart[0U], 1U)!= 0U) { }
                    if((Nalu.type > 0U) && (Nalu.type < 6U)){
                        pAddr = EndAddr - 4U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    }
                } else if(p[3U] > 1U) {
                    pAddr = ((pAddr + 4U) == BufferLimitAddr) ? BufferBaseAddr : (pAddr + 4U);
                    AmbaMisra_TypeCast(&p, &pAddr);
                } else if (p[2U] > 1U){
                    pAddr += 3U;
                    AmbaMisra_TypeCast(&p, &pAddr);
                } else if (p[1U] > 1U){
                    pAddr += 2U;
                    AmbaMisra_TypeCast(&p, &pAddr);
                } else {
                    pAddr ++;
                    AmbaMisra_TypeCast(&p, &pAddr);
                }
            } else {// start-code might at boundary
                if ((pAddr + 3U) == BufferLimitAddr) {
                    //w = (p[0U] << 24U) | (p[1U] << 16U) | (p[2U] << 8U) | pSub->pBufferBase[0U];
                    w = Uint8ArrayToUint32(p[0U], p[1U], p[2U], pSub->pBufferBase[0U]);

                    if ((0x00000001U == w) || (0x00000100U == (w & 0xffffff00U))) {
                        //next NAL start-code accross boundary, so this NAL won't
                        if(((0x06U == Nalu.type) && (1U == OmitSei(pSub)))  //omit SEI in Intra Refresh mode
                        || (0x1aU == Nalu.type)                     //omit AMBA proprietary GOP Header
                        || (0x09U == Nalu.type)) {                  //omit AU-delimiter
                            //Do nothing.
                        } else {
                            SendPacket(pSub, pLastStart, pAddr - LastStartAddr, 0);
                        }

                        if(0x01U == p[2U]) {
                            pAddr = BufferBaseAddr;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        } else {
                            pAddr = BufferBaseAddr + 1U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        }
                        pLastStart = p;
                        LastStartAddr = pAddr;

                        if (AmbaWrap_memcpy(&Nalu, &pLastStart[0U], 1U)!= 0U) { }
                        if((Nalu.type > 0U) && (Nalu.type < 6U)){
                            pAddr = EndAddr - 4U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        }
                    } else if (pSub->pBufferBase[0U] > 0U) {
                        pAddr = BufferBaseAddr + 1U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else if (p[2U] > 1U) {
                        pAddr = BufferBaseAddr;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else if (p[1U] > 1U) {
                        pAddr += 2U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else {
                        pAddr ++;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    }
                } else if ((pAddr + 2U) == BufferLimitAddr) {
                    w = Uint8ArrayToUint32(p[0U], p[1U], pSub->pBufferBase[0U], pSub->pBufferBase[1U]);
                    if ((0x00000001U == w) || (0x00000100U == (w & 0xffffff00U))) {

                        //next NAL start-code accross boundary, so this NAL won't
                        if(((0x06U == Nalu.type) && (1U == OmitSei(pSub)))    //omit SEI in Intra Refresh mode
                        || (0x1aU == Nalu.type)                             //omit AMBA proprietary GOP Header
                        || (0x09U == Nalu.type)) {                          //omit AU-delimiter
                            // Do nothing.
                        } else {
                            SendPacket(pSub, pLastStart, pAddr - LastStartAddr, 0);
                        }


                        if(pSub->pBufferBase[0U] == 0x01U) {
                            pAddr = BufferBaseAddr + 1U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        } else {
                            pAddr = BufferBaseAddr + 2U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        }
                        pLastStart = p;
                        LastStartAddr = pAddr;

                        if (AmbaWrap_memcpy(&Nalu, &pLastStart[0U], 1U)!= 0U) { }
                        if((Nalu.type > 0U) && (Nalu.type < 6U)){
                            pAddr = EndAddr - 4U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        }
                    } else if (pSub->pBufferBase[1U] > 0U) {
                        pAddr = BufferBaseAddr + 2U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else if (pSub->pBufferBase[0U] > 1U) {
                        pAddr = BufferBaseAddr + 1U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else if (p[1U] > 1U) {
                        pAddr = BufferBaseAddr;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else {
                        pAddr++;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    }
                } else if ((pAddr + 1U) == BufferLimitAddr) {
                    w = Uint8ArrayToUint32(p[0U], pSub->pBufferBase[0U], pSub->pBufferBase[1U], pSub->pBufferBase[2U]);
                    if ((0x00000001U == w) || (0x00000100U == (w & 0xffffff00U))) {
                        if(((0x06U == Nalu.type) && (1U == OmitSei(pSub)))
                        || (0x1aU == Nalu.type)
                        || (0x09U == Nalu.type)) {
                            // Do Nothing.
                        } else {
                            SendPacket(pSub, pLastStart, pAddr - LastStartAddr, 0U);
                        }

                        if(0x01U == pSub->pBufferBase[1U]) {
                            pAddr = BufferBaseAddr + 2U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        } else {
                            pAddr = BufferBaseAddr + 3U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        }
                        pLastStart = p;
                        LastStartAddr = pAddr;

                        if (AmbaWrap_memcpy(&Nalu, &pLastStart[0U], 1U)!= 0U) { }
                        if((Nalu.type > 0U) && (Nalu.type < 6U)){
                            pAddr = EndAddr - 4U;
                            AmbaMisra_TypeCast(&p, &pAddr);
                        }
                    } else if (pSub->pBufferBase[2U] > 0U) {
                        pAddr = BufferBaseAddr + 3U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else if (pSub->pBufferBase[1U] > 1U) {
                        pAddr = BufferBaseAddr + 2U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else if (pSub->pBufferBase[0U] > 1U) {
                        pAddr = BufferBaseAddr + 1U;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    } else {
                        pAddr = BufferBaseAddr;
                        AmbaMisra_TypeCast(&p, &pAddr);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[SvcH264Subsession]<RingBufPacketize> exception happened.",
                        0U, 0U, 0U, 0U, 0U);
                }
            }
        }
        //last nalu
        if (pAddr >= LastStartAddr) {
            SendPacket(pSub, pLastStart, EndAddr - LastStartAddr, Mark);
            AmbaPrint_PrintUInt5("RingBufPacketize,(last): sent NAL=%u with size=%u,  pts=%u", \
                                Nalu.type, pAddr - LastStartAddr, pSub->CurTimestamp, 0U, 0U);
        } else { // this NAL accros boundary
            NALsize = (BufferLimitAddr - LastStartAddr) + (EndAddr - BufferBaseAddr);
            SendPacket(pSub, pLastStart, NALsize, Mark);
        }
    }
    return;
}

static void Packetize(MEDIA_SUBSESSION_s* pSub, UINT8 *pBuf, UINT32 Len, UINT32 Mark)
{
    UINT8* pLastStart;
    UINT8* pPtr, *pTmp;
    UINT32 Word;
    NALU_TYPE_s Nalu = {0};
    ULONG BufAddr, BufferLimitAddr, LastStartAddr, PtrAddr;

    AmbaMisra_TypeCast(&BufAddr, &pBuf);
    pTmp = pSub->pBufferLimit;
    AmbaMisra_TypeCast(&BufferLimitAddr, &pTmp);

    if ((BufAddr + Len) > BufferLimitAddr) {
        // complicated ring-buf rounding case
        RingBufPacketize(pSub, pBuf, Len, Mark);
    } else {
        UINT32 const * Int32Ptr;
        // normal case, find start code
        AmbaMisra_TypeCast(&Int32Ptr, &pBuf);
        if (0x01000000U != *Int32Ptr) {
            AmbaPrint_PrintStr5("cannot find start code", NULL, NULL, NULL, NULL, NULL);
        } else {
            // skip the start code
            LastStartAddr = BufAddr + 4U;
            AmbaMisra_TypeCast(&pLastStart, &LastStartAddr);
            pPtr = pLastStart;

            // it's a hack for amba solution, in bitstream from amba,
            // every frame only have one slice nalu, the slice nalu alway be the last nalu.
            if (AmbaWrap_memcpy(&Nalu, &pLastStart[0U], 1U)!= 0U) { }
            if( (Nalu.type > 0U) && (Nalu.type < 6U) ){ //it's slice nalu
                PtrAddr = BufAddr + Len - 4U;
                AmbaMisra_TypeCast(&pPtr, &PtrAddr);
            }
            AmbaMisra_TypeCast(&PtrAddr, &pPtr);

            while (PtrAddr < ((BufAddr + Len) - 4U)) {
                Word = Uint8ArrayToUint32(pPtr[0U], pPtr[1U], pPtr[2U], pPtr[3U]);
                if ((0x00000001U == Word) || (0x00000100U == (Word & 0xffffff00U))) {//got next nalu, p point to next start-code
                    if( ((0x06U == Nalu.type) && (1U == OmitSei(pSub))) //omit SEI in Intra Refresh mode
                    ||  (0x1aU == Nalu.type)                            //omit AMBA proprietary GOP Header
                    ||  (0x09U == Nalu.type)) {                         //omit AU-delimiter
                        // Do nothing.
                    } else {
                        SendPacket(pSub, pLastStart, PtrAddr - LastStartAddr, 0U);
                    }

                    if(0x01U == pPtr[3U]) {
                        // skip 4, for next search
                        PtrAddr += 4U;
                        AmbaMisra_TypeCast(&pPtr, &PtrAddr);
                    } else {
                        PtrAddr += 3U;
                        AmbaMisra_TypeCast(&pPtr, &PtrAddr);
                    }
                    pLastStart = pPtr;
                    AmbaMisra_TypeCast(&LastStartAddr, &pLastStart);

                    if (AmbaWrap_memcpy(&Nalu, &pLastStart[0U], 1U)!= 0U) { }
                    if((Nalu.type > 0U) && (Nalu.type < 6U)){
                        PtrAddr = BufAddr + Len - 4U;
                        AmbaMisra_TypeCast(&pPtr, &PtrAddr);
                    }
                } else if(pPtr[3U] > 1U) {
                    PtrAddr += 4U;
                    AmbaMisra_TypeCast(&pPtr, &PtrAddr);
                } else if (pPtr[2U] > 1U){
                    PtrAddr += 3U;
                    AmbaMisra_TypeCast(&pPtr, &PtrAddr);
                } else if (pPtr[1U] > 1U){
                    PtrAddr += 2U;
                    AmbaMisra_TypeCast(&pPtr, &PtrAddr);
                } else {
                    PtrAddr++;
                    AmbaMisra_TypeCast(&pPtr, &PtrAddr);
                }
            }
            //last nalu
            SendPacket(pSub, pLastStart, BufAddr + Len - LastStartAddr, Mark);
        }
    }
}

static void H264UpdatePts(const SVC_RTSP_FRAME_DESC_s* pDesc,
                      MEDIA_SUBSESSION_s* pSub,
                      UINT32 *pTmpTs,
                      UINT32 *pFramePts,
                      UINT8 StreamRestart) {

    AMBA_STREAM_s *pStreamer = NULL;
    DOUBLE TempDouble64 = (DOUBLE)pDesc->Pts;
    UINT64 FramePtsU64;

    AmbaMisra_TypeCast(&pStreamer, &(pSub->Streamer));

    TempDouble64 = TempDouble64 * pSub->VectorClk;
    FramePtsU64 = (UINT64)TempDouble64;
    FramePtsU64 = FramePtsU64 & 0x00000000FFFFFFFFLU;
    *pFramePts = (UINT32)FramePtsU64;

    if (1U == StreamRestart) {
        pSub->CurTimestamp = SvcRtpSink_GetCurTimestamp(&pSub->Rtp, 0U);
        pSub->ChangeTick = 0U;
    } else {
        *pTmpTs = *pFramePts;
        if (*pTmpTs < pSub->LastPts) {
            *pTmpTs = *pTmpTs + (0xFFFFFFFFU - pSub->LastPts) + 1U;
        } else {
            *pTmpTs = *pTmpTs - pSub->LastPts;
        }

        if ((1U == pSub->ChangeTick) && (0U != *pTmpTs)) {
            if(OK != SvcRtspStrmChangeTickTimer(pStreamer, pSub->TicksPerFrame, *pTmpTs)) {
                AmbaPrint_PrintUInt5("[SvcAacSubsession]<UpdatePts>: AmbaStreamer_ChangeTickTimer failed.", 0U, 0U, 0U, 0U, 0U);
            }
            pSub->TicksPerFrame = *pTmpTs;
            pSub->ChangeTick = 0U;
            AmbaPrint_PrintUInt5("[SvcH264Subsession]<H264UpdatePts>: change TickPerFrame to %lu",
                pSub->TicksPerFrame, 0U, 0U, 0U, 0U);
        }

        if (0U != *pTmpTs) {    /* tmp_ts == 0 is multi-slice, don't update curTimestamp */
            /* Threadx UINT32 overflow will keep value 0xFFFFFFFF, handle it */
            FramePtsU64 = ((UINT64)pSub->CurTimestamp + (UINT64)pSub->TicksPerFrame);
            if (FramePtsU64 > 0x00000000FFFFFFFFLU) {
                FramePtsU64 -= 0x00000000FFFFFFFFLU;
            }
            pSub->CurTimestamp = (UINT32)FramePtsU64;
        }
    }

    if (((*pFramePts - pSub->LastPts) != pSub->TicksPerFrame)
        && ((0U != *pFramePts) || (0U != pSub->LastPts))) {
        pSub->ChangeTick = 1U;
    }
    pSub->LastPts = *pFramePts;
}


static void StreamerFunc(SVC_RTSP_FRAME_DESC_s const * pDesc, void const * pCtx)
{
    MEDIA_SUBSESSION_s* pSub;
    UINT8 StreamRestart = 0U;
    UINT32 TmpTs, FramePts = 0U;
    UINT8 *pSrcU8;
    UINT32 NeedToSendPkt = 1U;
    ULONG DescStartAddr;

    if ((NULL == pDesc) || (NULL == pCtx)) {
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaMisra_TypeCast(&pSub, &pCtx);

        if (STREAM_MARK_DROP_FRAME == pDesc->Size) {
            pSub->PayloadCtx.H264.GotFirstIdr = 0U;
            H264UpdatePts(pDesc, pSub, &TmpTs, &FramePts, StreamRestart);
        } else if ((pDesc->Type != SVC_RTSP_FRAME_TYPE_IDR_FRAME) &&
            (pDesc->Type != SVC_RTSP_FRAME_TYPE_I_FRAME) &&
            (pDesc->Type != SVC_RTSP_FRAME_TYPE_P_FRAME) &&
            (pDesc->Type != SVC_RTSP_FRAME_TYPE_B_FRAME) &&
            (pDesc->Type != SVC_RTSP_FRAME_TYPE_EOS)) {
            // Do nothing.
        } else {
            if ((pDesc->Type != SVC_RTSP_FRAME_TYPE_EOS) && ((pDesc->Size < RTSP_H264_MAX_FRAME_SIZE) || (pDesc->Size != 0U))) {
                if (0U == pSub->TotalReadFrames) {
                    StreamRestart = 1U;
                    pSub->PayloadCtx.H264.GotFirstIdr = 0U;
                } else if (((pSub->CurTimestamp >= pSub->NextSrPts) && ((pSub->CurTimestamp - pSub->NextSrPts) < 0xF0000000U))) {
                    pSub->Rtp.NeedToSendSrSdes = 1U;
                } else {
                    // Do nothing here.
                }

                // drop until IDR, we need to consider intra refresh case
                if (0U == pSub->PayloadCtx.H264.GotFirstIdr) {
                    if ((pDesc->Type == SVC_RTSP_FRAME_TYPE_IDR_FRAME) || (pSub->PayloadCtx.H264.SeiRecoveryLen > 0U)) {
                        pSub->PayloadCtx.H264.GotFirstIdr = 1U;
                    } else {
                        H264UpdatePts(pDesc, pSub, &TmpTs, &FramePts, StreamRestart);
                        NeedToSendPkt = 0U;
                    }
                }

                if(0U != NeedToSendPkt) {
                    H264UpdatePts(pDesc, pSub, &TmpTs, &FramePts, StreamRestart);

                    //if need to send sr_sdes, set next sr pts
                    if ((pSub->Rtp.NeedToSendSrSdes == 1U) || (StreamRestart == 1U)) {
                        pSub->NextSrPts = pSub->CurTimestamp + (pSub->RefClock * 5U);//min_sr_period
                    }

                    DescStartAddr = pDesc->StartAddr;
                    AmbaMisra_TypeCast(&pSrcU8, &DescStartAddr);

                    Packetize(pSub, pSrcU8, pDesc->Size, pDesc->Completed);
                    pSub->TotalReadFrames++;
                }
            } else {
                AmbaPrint_PrintStr5("Good-Bye", NULL, NULL, NULL, NULL, NULL);
                if(ERR_NA == SvcRtcp_SendGoodBye(&pSub->Rtp)) {
                    AmbaPrint_PrintStr5("Good-Bye Failed!!!", NULL, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
}

/**
* create H264 Subsession.
* @param [in]  SVC_RTSP_SERVER_s object pointer.
* @param [in]  SVC_STREAM_LIVE_MEDIA_s or SVC_RTSP_MEDIA_TRACK_CFG_s object pointer.
*              AAC track info.
* @param [in]  Indicate whether AAC track is live.
* @return MEDIA_SUBSESSION_s object created.NULL when failed.
*/
MEDIA_SUBSESSION_s* SvcH264Subsession_Create(void const *pSrv, void const * pInfo, UINT32 IsLive)
{
    MEDIA_SUBSESSION_s* pSub;
    SVC_RTSP_MEDIA_TRACK_CFG_s Track = {0};
    SVC_RTSP_MEDIA_TRACK_CFG_s* pTrk;
    SVC_STREAM_LIVE_MEDIA_s const * pLiveMedia;
    AMBA_STREAM_s *pStm;
    const SVC_RTSP_SERVER_s *pRtspSvr;
    void const *pTmpPtr;
    DOUBLE TicksPerFrame;
    MEDIA_SUBSESSION_s *pRetSubsession;

    AmbaMisra_TypeCast(&pRtspSvr, &pSrv);

    if ((NULL == pRtspSvr) || ((0U == IsLive) && (NULL == pInfo))) {
        AmbaPrint_PrintStr5("Invalid parameter.", NULL, NULL, NULL, NULL, NULL);
        pRetSubsession = NULL;
    } else {
        pTmpPtr = SvcRtspServer_GetSubsessionSlot(pRtspSvr);
        AmbaMisra_TypeCast(&pSub, &pTmpPtr);

        if (NULL == pSub) {
            AmbaPrint_PrintStr5("no available subsession", NULL, NULL, NULL, NULL, NULL);
            pRetSubsession = NULL;
        } else {
            if (0U == IsLive) {
                AmbaMisra_TypeCast(&pTrk, &pInfo);
                pStm = SvcRtspStrmCreate();
            } else {
                AmbaMisra_TypeCast(&pLiveMedia, &pInfo);
                pStm = SvcRtspStrmBind(&Track, pLiveMedia->Stm, StreamerFunc, pSub);
                pTrk = &Track;
                if ((0xffffU == pTrk->Info.Video.nN) && (pTrk->Info.Video.nIRCycle != 0U)) {
                    UINT16 recovery_frame_cnt = (pTrk->Info.Video.nRecoveryFrameCnt == 0U) ?
                        (pTrk->Info.Video.nIRCycle - 1U) : pTrk->Info.Video.nRecoveryFrameCnt;
                    pSub->PayloadCtx.H264.SeiRecoveryLen =
                        GenerateRecoveryPoint(pSub->PayloadCtx.H264.SeiRecoveryPkt, (UINT32)recovery_frame_cnt);
                } else {
                    pSub->PayloadCtx.H264.SeiRecoveryLen = 0U;
                }
            }

            if (pStm == NULL) {
                AmbaPrint_PrintStr5("Streamer create fail", NULL, NULL, NULL, NULL, NULL);
                pRetSubsession = NULL;
            } else {
                pSub->GetSdpLines = GetSdpLines;
                pSub->CodecID = pTrk->nMediaId;
                pSub->RefClock = 90000U;

                pSub->Streamer = pStm;
                pSub->IsLive = (UINT8)IsLive;
                pSub->MaxTransport = (0U == pSub->IsLive) ? 1U : MAX_SUBSESSION_TRANSPORT; //prevent from multi-playback on same file

                // streamerFunc need these information
                pSub->VectorClk = (DOUBLE)pSub->RefClock /(DOUBLE)pTrk->nTimeScale;
                TicksPerFrame = (DOUBLE)pTrk->nTimePerFrame * pSub->VectorClk;
                pSub->TicksPerFrame = (UINT32)TicksPerFrame;
                pSub->pBufferBase = pTrk->pBufferBase;
                pSub->pBufferLimit = pTrk->pBufferLimit;

                pRetSubsession = pSub;
            }
        }
    }
    return pRetSubsession;
}
