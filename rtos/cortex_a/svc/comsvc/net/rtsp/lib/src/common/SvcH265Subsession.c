/**
 *  @file SvcH265Subsession.c
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
 *  @details H265 subsession module
 *
 */

#include "AmbaTypes.h"
//#include <common/common.h>
#include <AmbaKAL.h>
#include "AmbaPrint.h"

#include "SvcMediaSubsession.h"
#include "SvcStreamMgr.h"
#include "SvcBase64.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaSvcWrap.h"

#define RTSP_H265_MAX_FRAME_SIZE 0x00F00000

#define H265_VPS_BASE64ENCODE_MAX_LENGTH (SVC_RTSP_MAX_SPS_LENGTH*4/3)
#define H265_SPS_BASE64ENCODE_MAX_LENGTH (SVC_RTSP_MAX_SPS_LENGTH*4/3)
#define H265_PPS_BASE64ENCODE_MAX_LENGTH (SVC_RTSP_MAX_PPS_LENGTH*4/3)

#define REMOVE_PADDING(p, Len) do{  \
    UINT32 i;                       \
    for (i = 0; i < 3; i++) {       \
        if (p[Len-1] == 0x00) {     \
            Len--;                  \
        } else {                    \
            break;                  \
        }                           \
    }                               \
}while(0)                           \

typedef struct _NALU_TYPE_s_{
    UINT16 tid:3;
    UINT16 layerID:6;
    UINT16 type:6;
    UINT16 forbidden_zero:1;
} NALU_TYPE_s;



static UINT32 RemoveEmulationBytes(char* to, UINT32 toMaxSize, char* from, UINT32 fromMaxSize)
{
    UINT32 index = 0, fromIndex = 0;
    UINT32 w;
    while(index + 1 < toMaxSize && fromIndex < fromMaxSize) {
        w = (from[fromIndex] << 16) | (from[fromIndex+1] << 8) | from[fromIndex+2];
        if (fromIndex+2 < fromMaxSize && (w & 0x00ffffff)==0x00000003) {
            to[index++] = from[fromIndex++];
            to[index++] = from[fromIndex++];
            fromIndex++;
        } else {
            to[index++] = from[fromIndex++];
        }
    }
    return index;
}

static const char* GetAuxSdp(MEDIA_SUBSESSION_s *thiz)
{
    char* fmtpFmt;
    UINT32 fmtpFmtSize;
    UINT32 VpsLen, SpsLen, PpsLen;
    INT32 ret;
    char profileTierLevelHeaderBytes[12] = {0};
    INT32 ProfileSpace=0, ProfileId=0, TierFlag=0, LevelId=0;
    char interop_strbuf[32] = {0};
    char *vps_noEmulationBytes = NULL;
    INT32 vps_noEmulationBytes_len = 0;
    UINT8* interop_constraints;
    char vps_base64[H265_VPS_BASE64ENCODE_MAX_LENGTH];
    char sps_base64[H265_SPS_BASE64ENCODE_MAX_LENGTH];
    char pps_base64[H265_PPS_BASE64ENCODE_MAX_LENGTH];
    char Vps[SVC_RTSP_MAX_SPS_LENGTH];
    char Sps[SVC_RTSP_MAX_SPS_LENGTH];
    char Pps[SVC_RTSP_MAX_PPS_LENGTH];

    if (thiz->AuxSdpLine != NULL) return thiz->AuxSdpLine;

    if (AmbaWrap_memset(vps_base64, 0, H265_VPS_BASE64ENCODE_MAX_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(sps_base64, 0, H265_SPS_BASE64ENCODE_MAX_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(pps_base64, 0, H265_PPS_BASE64ENCODE_MAX_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(Vps, 0, SVC_RTSP_MAX_SPS_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(Sps, 0, SVC_RTSP_MAX_SPS_LENGTH)!= 0U) { }
    if (AmbaWrap_memset(Pps, 0, SVC_RTSP_MAX_PPS_LENGTH)!= 0U) { }
    ret = SvcRtspStrmGetVpsSpsPps(thiz->Streamer,
                            Vps, &VpsLen,
                            Sps, &SpsLen,
                            Pps, &PpsLen);
    if (ret != OK) {
        return NULL; // our source isn't ready
    }

    // sanity check
    if (VpsLen > SVC_RTSP_MAX_SPS_LENGTH
        || SpsLen > SVC_RTSP_MAX_SPS_LENGTH
        || PpsLen > SVC_RTSP_MAX_PPS_LENGTH) {
        return NULL;
    }

    // remove padding byte before base64_encode
    REMOVE_PADDING(Vps, VpsLen);
    REMOVE_PADDING(Sps, SpsLen);
    REMOVE_PADDING(Pps, PpsLen);

    // analyze profile level header, need a copy of vps data without emulation bytes
    if (VpsLen >= 6/*'profile_tier_level' offset*/ + 12/*num 'profile_tier_level' bytes*/) {
        vps_noEmulationBytes = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_VPS_SPS_PPS_BUF, VpsLen);
        if (vps_noEmulationBytes != NULL) {
            vps_noEmulationBytes_len = RemoveEmulationBytes(vps_noEmulationBytes, VpsLen, Vps, VpsLen);
        }
        if (vps_noEmulationBytes_len >= 6 + 12) {
            AmbaWrap_memcpy(profileTierLevelHeaderBytes, &vps_noEmulationBytes[6], 12);
        } else {
        AmbaWrap_memcpy(profileTierLevelHeaderBytes, &Vps[6], 12);
        }
        ProfileSpace  = profileTierLevelHeaderBytes[0]>>6; // general_profile_space
        ProfileId = profileTierLevelHeaderBytes[0]&0x1F; // general_profile_idc
        TierFlag = (profileTierLevelHeaderBytes[0]>>5)&0x1; // general_tier_flag
        LevelId = profileTierLevelHeaderBytes[11]; // general_level_idc
        interop_constraints = (UINT8*) &profileTierLevelHeaderBytes[5];
    {
        UINT32 RetVal;

       if (AmbaWrap_memset(interop_strbuf, 0U, 32U)!= 0U) { }

    //    RetVal = AmbaAdvSnPrint(interop_strbuf, 32U, "%02X%02X%02X%02X%02X%02X", interop_constraints[0], interop_constraints[1]
    //                            interop_constraints[2], interop_constraints[3], interop_constraints[4], interop_constraints[5]);

        {
            char    *StrBuf = interop_strbuf;
            UINT32  BufSize = 32U;
            UINT32  CurStrLen;

            AmbaUtility_StringAppend(StrBuf, BufSize, "");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(interop_constraints[0]), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(interop_constraints[1]), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(interop_constraints[2]), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(interop_constraints[3]), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(interop_constraints[4]), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                char    NumBuf[16];
                UINT32  NumLen;

                if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)(interop_constraints[5]), 16U);
                NumLen = AmbaUtility_StringLength(NumBuf);

                while (NumLen < 2UL) {
                    AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                    NumLen++;
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, "");

            RetVal = AmbaUtility_StringLength(StrBuf);
        }

       AmbaPrint_PrintStr5("%s, Buf %s", __func__, interop_strbuf, NULL, NULL, NULL);
       AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);
    }

        SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_VPS_SPS_PPS_BUF, vps_noEmulationBytes);
    }

    if (Sps[0]==0 && Sps[1]==0 && Sps[2]==0 && Sps[3]==1 &&
        Pps[0]==0 && Pps[1]==0 && Pps[2]==0 && Pps[3]==1){//Skip start code (00 00 00 01)
        if( SvcBase64_Encode((char*)Vps+4, VpsLen-4, vps_base64, H265_VPS_BASE64ENCODE_MAX_LENGTH) != OK ||
            SvcBase64_Encode((char*)Sps+4, SpsLen-4, sps_base64, H265_SPS_BASE64ENCODE_MAX_LENGTH) != OK ||
            SvcBase64_Encode((char*)Pps+4, PpsLen-4, pps_base64, H265_PPS_BASE64ENCODE_MAX_LENGTH) != OK ) {
            return NULL;
        }
    } else if (Sps[0]==0 && Sps[1]==0 && Sps[2]==1 &&
               Pps[0]==0 && Pps[1]==0 && Pps[2]==1){     //Skip start code (00 00 01)
        if( SvcBase64_Encode((char*)Vps+3, VpsLen-3, vps_base64, H265_VPS_BASE64ENCODE_MAX_LENGTH) != OK ||
            SvcBase64_Encode((char*)Sps+3, SpsLen-3, sps_base64, H265_SPS_BASE64ENCODE_MAX_LENGTH) != OK ||
            SvcBase64_Encode((char*)Pps+3, PpsLen-3, pps_base64, H265_PPS_BASE64ENCODE_MAX_LENGTH) != OK ) {
            return NULL;
        }
    } else {
        if( SvcBase64_Encode((char*)Vps, VpsLen, vps_base64, H265_VPS_BASE64ENCODE_MAX_LENGTH) != OK ||
            SvcBase64_Encode((char*)Sps, SpsLen, sps_base64, H265_SPS_BASE64ENCODE_MAX_LENGTH) != OK ||
            SvcBase64_Encode((char*)Pps, PpsLen, pps_base64, H265_PPS_BASE64ENCODE_MAX_LENGTH) != OK ) {
            return NULL;
        }
    }

    // Set up the "a=fmtp:" SDP line for this stream:
    fmtpFmt =
        "a=fmtp:%d profile-space=%d;profile-id=%d;tier-flag=%d"
        ";level-id=%d"
        ";interop-constraints=%s"
        ";sprop-Vps=%s;sprop-Sps=%s;sprop-Pps=%s\r\n";
    fmtpFmtSize = AmbaUtility_StringLength(fmtpFmt)
                + 3 /* max char len: payload type*/
                + 20/* max char len: profile-space*/
                + 20/* max char len: profile-Id*/
                + 20/* max char len: tier-flag*/
                + 20/* max char len: level-id*/
                + AmbaUtility_StringLength(interop_strbuf)
                + AmbaUtility_StringLength(vps_base64) + AmbaUtility_StringLength(sps_base64) + AmbaUtility_StringLength(pps_base64);

    thiz->AuxSdpLine = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, fmtpFmtSize + 1);

    if (thiz->AuxSdpLine == NULL) {
        AmbaPrint_PrintUInt5("GetAuxSdp: malloc sdp_len(%u) fail", fmtpFmtSize, 0U, 0U, 0U, 0U);
        return NULL;
    }

    {
        UINT32 RetVal;

        if (AmbaWrap_memset(thiz->AuxSdpLine, 0, fmtpFmtSize)!= 0U) { }

        {
            char    *StrBuf = thiz->AuxSdpLine;
            UINT32  BufSize = fmtpFmtSize;
            UINT32  CurStrLen;

            AmbaUtility_StringAppend(StrBuf, BufSize, "a=fmtp:");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(96 + thiz->TrackNum - 1), 10U);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, " profile-space=");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(ProfileSpace), 10U);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, ";profile-id=");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(ProfileId), 10U);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, ";tier-flag=");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(TierFlag), 10U);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, ";level-id=");

            CurStrLen = AmbaUtility_StringLength(StrBuf);
            if (CurStrLen < BufSize) {
                (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(LevelId), 10U);
            }
            AmbaUtility_StringAppend(StrBuf, BufSize, ";interop-constraints=");
            AmbaUtility_StringAppend(StrBuf, BufSize, (interop_strbuf));
            AmbaUtility_StringAppend(StrBuf, BufSize, ";sprop-Vps=");
            AmbaUtility_StringAppend(StrBuf, BufSize, (vps_base64));
            AmbaUtility_StringAppend(StrBuf, BufSize, ";sprop-Sps=");
            AmbaUtility_StringAppend(StrBuf, BufSize, (sps_base64));
            AmbaUtility_StringAppend(StrBuf, BufSize, ";sprop-Pps=");
            AmbaUtility_StringAppend(StrBuf, BufSize, (pps_base64));
            AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");

            RetVal = AmbaUtility_StringLength(StrBuf);
        }

        AmbaPrint_PrintStr5("%s, aux sdp %s", __func__, thiz->AuxSdpLine, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);
    }

    return thiz->AuxSdpLine;
}

static const char* GetSdpLines(void const * pthiz)
{
    MEDIA_SUBSESSION_s *thiz = (MEDIA_SUBSESSION_s *)pthiz;

    if(thiz == NULL) {
        AmbaPrint_PrintStr5("GetSdpLines: thiz == NULL", NULL, NULL, NULL, NULL, NULL);
        return NULL;
    }

    if (thiz->SdpLines == NULL){
        char const* mediaType = "video";
        char const* sdp_server = "0.0.0.0";
        UINT8 rtpPayloadType = 96 + thiz->TrackNum - 1;
        char const* const sdpFmt =
            "m=%s %u RTP/AVP %d\r\n"
            "c=IN IP4 %s\r\n"
            "b=AS:%u\r\n"
            "a=rtpmap:%u H265/90000\r\n"
            "%s"
            "a=control:%s\r\n";
        UINT32 sdpFmtSize;
        char const* auxSDPLine = GetAuxSdp(thiz);

        if (auxSDPLine==NULL) {
            AmbaPrint_PrintStr5("GetSdpLines: auxSDPLine == NULL", NULL, NULL, NULL, NULL, NULL);
        }

        sdpFmtSize = AmbaUtility_StringLength(sdpFmt)
            + AmbaUtility_StringLength(mediaType) + 5 /* max short len */ + 3 /* max char len */
            + AmbaUtility_StringLength(sdp_server)
            + 20 /* max int len */
            + 3
            + ((auxSDPLine==NULL)? 0:AmbaUtility_StringLength(auxSDPLine))
            + AmbaUtility_StringLength(SvcMediaSubsession_GetTrackID(thiz));

        thiz->SdpLines = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, sdpFmtSize + 1);

        if (thiz->SdpLines == NULL) {
            AmbaPrint_PrintUInt5("GetSdpLines: malloc sdp_len(%u) fail", \
                            sdpFmtSize, 0U, 0U, 0U, 0U);
            return NULL;
        }

        {
            UINT32 RetVal;

            if (AmbaWrap_memset(thiz->SdpLines, 0, sdpFmtSize)!= 0U) { }

            // RetVal = AmbaAdvSnPrint(thiz->AuxSdpLine, sdpFmtSize, sdpFmt, mediaType, 0U, rtpPayloadType, sdp_server, 10000U,
            //                         rtpPayloadType, (auxSDPLine==NULL)? "":auxSDPLine, SvcMediaSubsession_GetTrackID(thiz));
            {
                char    *StrBuf = thiz->SdpLines;
                UINT32  BufSize = sdpFmtSize;
                UINT32  CurStrLen;


                AmbaUtility_StringAppend(StrBuf, BufSize, "m=");
                AmbaUtility_StringAppend(StrBuf, BufSize, (mediaType));
                AmbaUtility_StringAppend(StrBuf, BufSize, " ");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(0U), 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, " RTP/AVP ");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(rtpPayloadType), 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\nc=IN IP4 ");
                AmbaUtility_StringAppend(StrBuf, BufSize, (sdp_server));
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\nb=AS:");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(10000U), 10U);
                }
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\na=rtpmap:");

                CurStrLen = AmbaUtility_StringLength(StrBuf);
                if (CurStrLen < BufSize) {
                    (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(rtpPayloadType), 10U);
                }

                AmbaUtility_StringAppend(StrBuf, BufSize, " H265/90000\r\n");
                AmbaUtility_StringAppend(StrBuf, BufSize, ((auxSDPLine==NULL)? "":auxSDPLine));
                AmbaUtility_StringAppend(StrBuf, BufSize, "a=control:");
                AmbaUtility_StringAppend(StrBuf, BufSize, (SvcMediaSubsession_GetTrackID(thiz)));
                AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");

                RetVal = AmbaUtility_StringLength(StrBuf);
            }

            AmbaPrint_PrintStr5("%s, aux sdp %s", __func__, thiz->SdpLines, NULL, NULL, NULL);
            AmbaPrint_PrintUInt5("Len %d", RetVal, 0U, 0U, 0U, 0U);
        }

        SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_SDP_LINE_BUF, thiz->AuxSdpLine);
        thiz->AuxSdpLine = NULL;
    }
    return thiz->SdpLines;
}

static UINT8* GetH265NextNALAddress(UINT8* Buf, UINT8* base, UINT8* limit)
{
    UINT32* i_ptr;
    char start_code[4] = {0};
    UINT32 i, j, roundIdx = limit - Buf;
    for (i=0; i < roundIdx; i++) {
        start_code[i] = Buf[i];
    }
    for (j=0; j < 4 - roundIdx; i++, j++) {
        start_code[i] = base[j];
    }
    i_ptr = (UINT32*) start_code;
    if (*i_ptr != 0x01000000) {
        AmbaPrint_PrintStr5("cannot find start code", NULL, NULL, NULL, NULL, NULL);
        return NULL;
    }
    return base + (4 - roundIdx);
}


static void H265SubsessionFU(MEDIA_SUBSESSION_s* sub, UINT8 *Buf,
                            UINT32 Len, UINT32 mtu, UINT32 marker)
{
    UINT32 frag_len;
    UINT32 remain = Len;
    UINT8 nalu[2] = {0};
    UINT8 *Payload;
    UINT8 fu_indicatorH, fu_indicatorL, fu_header;
    UINT8 start = 1;
    UINT8 fu[3];   // fu_indicator + fu_header
    RTP_PAYLOAD_INFO_s payloadInfo;
    RTP_IO_VEC_s vec[3];

    if (Buf + 1 == sub->pBufferLimit) { // NAL header is truncated
        AmbaWrap_memcpy(&nalu[0], Buf, 1);
        AmbaWrap_memcpy(&nalu[1], sub->pBufferBase, 1);
        Payload = sub->pBufferBase + 1;
    } else {
        AmbaWrap_memcpy(&nalu[0], Buf, 2);
        Payload = Buf +2;   // skip NAL bytes
    }

    fu_indicatorH = (nalu[0]&0x81) | (49 << 1);
    fu_indicatorL = (nalu[1]&0xff);
    fu_header = (nalu[0] & 0x7e) >> 1;

    remain-=2;
    while(remain > 0) {
        fu[0] = fu_indicatorH;
        fu[1] = fu_indicatorL;
        if(start == 1) {
            fu[2] = fu_header | (1 << 7);
            start = 0;
        } else {
            fu[2] = fu_header;
        }
        if(remain > mtu - 3) {
            frag_len = mtu - 3;
        } else {
            frag_len = remain;
            fu[2] = fu_header | (1 << 6);     //End
        }
        //take care ring buf case
        if (Payload + frag_len>= sub->pBufferLimit){ //truncated
            payloadInfo.Len = frag_len;
            payloadInfo.TimeStamp = sub->CurTimestamp;// - priv->ts_offset;
            payloadInfo.Maker = (remain <= frag_len && marker == 1) ? 1 : 0;
            vec[0].base = fu;
            vec[0].Len = 3;
            vec[1].base = Payload;
            vec[1].Len = sub->pBufferLimit - Payload;
            vec[2].base = sub->pBufferBase;
            vec[2].Len = payloadInfo.Len - vec[1].Len;
            SvcRtpSink_SendPacketVector(&sub->Rtp, &payloadInfo, &vec[0], 3);
            Payload = sub->pBufferBase + vec[2].Len ;
            remain -= frag_len;
        } else {
            payloadInfo.Buf = Payload;
            payloadInfo.Len = frag_len + 3;
            payloadInfo.TimeStamp = sub->CurTimestamp;// - priv->ts_offset;
            payloadInfo.Maker = (remain <= frag_len && marker == 1) ? 1 : 0;
            vec[0].base = fu;
            vec[0].Len = 3;
            vec[1].base = Payload;
            vec[1].Len = frag_len;
            SvcRtpSink_SendPacketVector(&sub->Rtp, &payloadInfo, &vec[0], 2);

            Payload += frag_len;
            remain -= frag_len;
            //nalu += frag_len;
        }
    }
    return;
}

static void SendPacket(MEDIA_SUBSESSION_s* sub, UINT8* start_addr, UINT32 Length, UINT32 mark)
{
    RTP_PAYLOAD_INFO_s payloadInfo;
    RTP_IO_VEC_s vec[2];

    if (Length > RTP_OVER_UDP_MAX) {
        H265SubsessionFU(sub, start_addr, Length, RTP_OVER_UDP_MAX, mark);
    } else if (start_addr + Length > sub->pBufferLimit){
        vec[0].base = start_addr;
        vec[0].Len = sub->pBufferLimit - start_addr;
        vec[1].base = sub->pBufferBase;
        vec[1].Len = Length - vec[0].Len;
        payloadInfo.Len = Length;
        payloadInfo.TimeStamp = sub->CurTimestamp;// need to add - priv->ts_offset;
        payloadInfo.Maker = mark;
        SvcRtpSink_SendPacketVector(&sub->Rtp, &payloadInfo, &vec[0], 2);
    } else {
        payloadInfo.Buf = start_addr;
        payloadInfo.Len = Length;
        payloadInfo.TimeStamp = sub->CurTimestamp; // need to add - thiz->ts_offset;
        payloadInfo.Maker = mark;
        SvcRtpSink_SendPacket(&sub->Rtp, &payloadInfo);
    }
}

#define READ_H265_NAL(nalu, last_start, base, limit) do{        \
    if (last_start + 1 == limit) {                              \
        AmbaWrap_memcpy( (char*)&nalu + 1, last_start, 1);               \
        AmbaWrap_memcpy(&nalu, base, 1);                                 \
    } else {                                                    \
        AmbaWrap_memcpy( (char*)&nalu + 1, last_start, 1);               \
        AmbaWrap_memcpy(&nalu, last_start + 1, 1);                       \
    }                                                           \
}while(0)                                                       \

static void RingBufPacketize(MEDIA_SUBSESSION_s* sub, UINT8 *Buf, UINT32 Len, UINT32 mark)
{
    UINT32* i_ptr;
    UINT8 *p, *last_start, *endAddr;
    NALU_TYPE_s    nalu;
    UINT32 w, NALsize;

    if (Buf + 4 >= sub->pBufferLimit) { //first NAL start-code rounding
        last_start = GetH265NextNALAddress(Buf, sub->pBufferBase, sub->pBufferLimit);
        if (last_start == NULL) return;
    } else {
        // find start code
        i_ptr = (UINT32*) Buf;
        if (*i_ptr != 0x01000000 && ((*i_ptr) & 0x00FFFFFF) != 0x010000) {
            AmbaPrint_PrintStr5("cannot find start code", NULL, NULL, NULL, NULL, NULL);
            return;
        }
        // skip the start code
        last_start = ((*i_ptr) == 0x01000000)? Buf + 4 : Buf + 3;
    }
    p = last_start;

    // this AU lies on ring-buf boundary, endAddr must accross BufferLimit
    endAddr = sub->pBufferBase + (Len - (sub->pBufferLimit - Buf));

    // read nal header
    READ_H265_NAL(nalu, last_start, sub->pBufferBase, sub->pBufferLimit);

    while ((p < last_start && p < endAddr - 4)
        || (last_start > Buf && p >= last_start)
        || (last_start < Buf && p < endAddr - 4)) { // searching for next start-code
        if (p+3 < sub->pBufferLimit) {
            w = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
            if (w==0x00000001 || (w & 0xffffff00)==0x00000100) {//got next nalu, p point to next start-code
                if (p >= last_start) {
                    SendPacket(sub, last_start, p - last_start, 0);
                } else { // this NAL accros boundary
                    NALsize = (sub->pBufferLimit - last_start) + (p - sub->pBufferBase);
                    SendPacket(sub, last_start, NALsize, 0);
                }
                if(p[2] == 0x01) {
                    p += 3;
                } else {
                    // skip 4, for next search
                    p = (p+4 == sub->pBufferLimit)? sub->pBufferBase: p+4;
                }
                last_start = p;
                READ_H265_NAL(nalu, last_start, sub->pBufferBase, sub->pBufferLimit);
            } else if(p[3] > 1) {
                p = (p+4 == sub->pBufferLimit)? sub->pBufferBase: p+4;
            } else if (p[2] > 1){
                p += 3;
            } else if (p[1] > 1){
                p += 2;
            } else {
                p++;
            }
        } else { // start-code might be truncated
            if (p + 3 == sub->pBufferLimit) {
                w = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | sub->pBufferBase[0];
                if (w==0x00000001 || (w & 0xffffff00)==0x00000100) {
                    SendPacket(sub, last_start, p - last_start, 0);
                    if(p[2] == 0x01) {
                        p = sub->pBufferBase;
                    } else {
                        p = sub->pBufferBase + 1;
                    }
                    last_start = p;
                    READ_H265_NAL(nalu, last_start, sub->pBufferBase, sub->pBufferLimit);
                } else if (sub->pBufferBase[0] > 0) {
                    p = sub->pBufferBase + 1;
                } else if (p[2] > 1) {
                    p = sub->pBufferBase;
                } else if (p[1] > 1) {
                    p += 2;
                } else {
                    p++;
                }
            } else if (p+2 == sub->pBufferLimit) {
                w = (p[0] << 24) | (p[1] << 16) | (sub->pBufferBase[0] << 8) | sub->pBufferBase[1];
                if (w==0x00000001 || (w & 0xffffff00)==0x00000100) {
                    SendPacket(sub, last_start, p - last_start, 0);
                    if(sub->pBufferBase[0] == 0x01) {
                        p = sub->pBufferBase + 1;
                    } else {
                        p = sub->pBufferBase + 2;
                    }
                    last_start = p;
                    READ_H265_NAL(nalu, last_start, sub->pBufferBase, sub->pBufferLimit);
                } else if (sub->pBufferBase[1] > 0) {
                    p = sub->pBufferBase + 2;
                } else if (sub->pBufferBase[0] > 1) {
                    p = sub->pBufferBase + 1;
                } else if (p[1] > 1) {
                    p = sub->pBufferBase;
                } else {
                    p++;
                }
            } else if (p+1 == sub->pBufferLimit) {
                w = (p[0] << 24) | (sub->pBufferBase[0] << 16) | (sub->pBufferBase[1] << 8) | sub->pBufferBase[2];
                if (w==0x00000001 || (w & 0xffffff00)==0x00000100) {
                    SendPacket(sub, last_start, p - last_start, 0);
                    if(sub->pBufferBase[1] == 0x01) {
                        p = sub->pBufferBase + 2;
                    } else {
                        p = sub->pBufferBase + 3;
                    }
                    last_start = p;
                    READ_H265_NAL(nalu, last_start, sub->pBufferBase, sub->pBufferLimit);
                } else if (sub->pBufferBase[2] > 0) {
                    p = sub->pBufferBase + 3;
                } else if (sub->pBufferBase[1] > 1) {
                    p = sub->pBufferBase + 2;
                } else if (sub->pBufferBase[0] > 1) {
                    p = sub->pBufferBase + 1;
                } else {
                    p = sub->pBufferBase;
                }
            }
        }
    }
    //last nalu
    if (p >= last_start) {
        SendPacket(sub, last_start, endAddr - last_start, mark);
    } else { // this NAL accros boundary
        NALsize = (sub->pBufferLimit - last_start) + (endAddr - sub->pBufferBase);
        SendPacket(sub, last_start, NALsize, mark);
    }

}

static void Packetize(MEDIA_SUBSESSION_s* sub, UINT8 *Buf, UINT32 Len, UINT32 mark)
{
    UINT8* last_start;
    UINT8* p;
    UINT32 w;
    NALU_TYPE_s    nalu;

    // if start code truncated at boundary
    if (Buf + Len > sub->pBufferLimit) {
        // complicated ring-buf rounding case
        RingBufPacketize(sub, Buf, Len, mark);
        return;
    } else {
        UINT32* i_ptr;
        // normal case, find start code
        i_ptr = (UINT32*) Buf;
        if (*i_ptr != 0x01000000 && ((*i_ptr) & 0x00FFFFFF) != 0x010000) {
            AmbaPrint_PrintStr5("cannot find start code", NULL, NULL, NULL, NULL, NULL);
            return;
        }
        // skip the start code
        last_start = ((*i_ptr) == 0x01000000)? Buf + 4 : Buf + 3;
    }
    p = last_start;

    // read nal header
    AmbaWrap_memcpy( (char*)&nalu + 1, last_start, 1);
    AmbaWrap_memcpy(&nalu, last_start + 1, 1);

    while (p < Buf + Len - 4 ) {
        w = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        if (w==0x00000001 || (w & 0xffffff00)==0x00000100) {//got next nalu, p point to next start-code
            SendPacket(sub, last_start, p - last_start, 0);
            if(p[3] == 0x01) {
                // skip 4, for next search
                p += 4;
            } else {
                p += 3;
            }
            last_start = p;
            AmbaWrap_memcpy( (char*)&nalu + 1, last_start, 1);
            AmbaWrap_memcpy(&nalu, last_start + 1, 1);
        } else if(p[3] > 1) {
            p += 4;
        } else if (p[2] > 1){
            p += 3;
        } else if (p[1] > 1){
            p += 2;
        } else {
            p++;
        }
    }
    SendPacket(sub, last_start, Buf + Len - last_start, mark);
}

#define H265_UPDATE_PTS(Desc, sub, tmp_ts, framePts, stream_restart) do{                            \
    framePts = ((UINT64)(Desc->Pts*sub->VectorClk)) & 0xffffffff;                                   \
    if (stream_restart == 1) {                                                                      \
        sub->CurTimestamp = SvcRtpSink_GetCurTimestamp(&sub->Rtp, 0);                                  \
        sub->ChangeTick = 0;                                                                        \
    } else {                                                                                        \
        tmp_ts = framePts;                                                                          \
        if (tmp_ts < sub->LastPts) {                                                                \
            tmp_ts = tmp_ts + (0xffffffff - sub->LastPts)+1;                                        \
        } else {                                                                                    \
            tmp_ts = tmp_ts - sub->LastPts;                                                         \
        }                                                                                           \
        if (sub->ChangeTick == 1 && tmp_ts ) {                                                      \
            SvcRtspStrmChangeTickTimer(sub->Streamer, sub->TicksPerFrame, tmp_ts);                 \
            sub->TicksPerFrame = tmp_ts;                                                            \
            sub->ChangeTick = 0;                                                                    \
        }                                                                                           \
        /* Threadx UINT32 overflow will keep value 0xffffffff, handle it.                           \
           tmp_ts == 0 is multi-slice, don't update curTimestamp */                                 \
        if( tmp_ts ) {                                                                              \
            if ((UINT64)sub->CurTimestamp + (UINT64)sub->TicksPerFrame > 0x00000000ffffffffllu) {   \
                sub->CurTimestamp = ((UINT64)sub->CurTimestamp + (UINT64)sub->TicksPerFrame)        \
                                    - 0x00000000ffffffffllu;                                        \
            } else {                                                                                \
                sub->CurTimestamp += sub->TicksPerFrame;                                            \
            }                                                                                       \
        }                                                                                           \
    }                                                                                               \
    if (framePts - sub->LastPts != sub->TicksPerFrame &&                                            \
        (framePts != 0 || sub->LastPts !=0)) {                                                      \
        sub->ChangeTick = 1;                                                                        \
    }                                                                                               \
    sub->LastPts = framePts;                                                                        \
}while(0)

static void StreamerFunc(SVC_RTSP_FRAME_DESC_s const * Desc, void const * Ctx)
{
    MEDIA_SUBSESSION_s* sub;
    UINT8 stream_restart = 0;
    UINT32 tmp_ts = 0, framePts = 0;
    UINT8 *pSrcU8;
    ULONG DescStartAddr;

    if ( Desc == NULL || Ctx == NULL ){
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
        return;
    }

    sub = (MEDIA_SUBSESSION_s*) Ctx;

    if (Desc->Size == STREAM_MARK_DROP_FRAME) {
        sub->PayloadCtx.H264.GotFirstIdr = 0;
        H265_UPDATE_PTS(Desc, sub, tmp_ts, framePts, stream_restart);
        return;
    }

    if (Desc->Type != SVC_RTSP_FRAME_TYPE_IDR_FRAME &&
        Desc->Type != SVC_RTSP_FRAME_TYPE_I_FRAME &&
        Desc->Type != SVC_RTSP_FRAME_TYPE_P_FRAME &&
        Desc->Type != SVC_RTSP_FRAME_TYPE_B_FRAME &&
        Desc->Type != SVC_RTSP_FRAME_TYPE_EOS) {
        return;
    }

    //if (desc->Type != AMP_FIFO_TYPE_EOS && (desc->Size < RTSP_H265_MAX_FRAME_SIZE || desc->Size != 0)) {
    if (Desc->Type != SVC_RTSP_FRAME_TYPE_EOS && (Desc->Size < RTSP_H265_MAX_FRAME_SIZE || Desc->Size != 0)) {
        if (sub->TotalReadFrames == 0) {
            stream_restart = 1;
            sub->PayloadCtx.H264.GotFirstIdr = 0;
        } else if (((sub->CurTimestamp >= sub->NextSrPts) && (sub->CurTimestamp - sub->NextSrPts) < 0xf0000000)) {
            sub->Rtp.NeedToSendSrSdes = 1;
        }

        // drop until IDR, we need to consider intra refresh case
        if (sub->PayloadCtx.H264.GotFirstIdr== 0) {
            //if (desc->Type == AMP_FIFO_TYPE_IDR_FRAME) {
            if (Desc->Type == SVC_RTSP_FRAME_TYPE_IDR_FRAME) {
                sub->PayloadCtx.H264.GotFirstIdr = 1;
            } else {
                H265_UPDATE_PTS(Desc, sub, tmp_ts, framePts, stream_restart);
                return;
            }
        }
        H265_UPDATE_PTS(Desc, sub, tmp_ts, framePts, stream_restart);

        //if need to send sr_sdes, set next sr pts
        if (sub->Rtp.NeedToSendSrSdes == 1 || stream_restart == 1) {
            sub->NextSrPts = sub->CurTimestamp + (sub->RefClock*5);//min_sr_period
        }

        DescStartAddr = (Desc->StartAddr);
        AmbaMisra_TypeCast(&pSrcU8, &DescStartAddr);
        Packetize(sub, pSrcU8, Desc->Size, Desc->Completed);
        sub->TotalReadFrames++;
    } else {
        SvcRtcp_SendGoodBye(&sub->Rtp);
    }
}

/**
* create H265 Subsession.
* @param [in]  SVC_RTSP_SERVER_s object pointer.
* @param [in]  SVC_STREAM_LIVE_MEDIA_s or SVC_RTSP_MEDIA_TRACK_CFG_s object pointer.
*              AAC track info.
* @param [in]  Indicate whether AAC track is live.
* @return MEDIA_SUBSESSION_s object created.NULL when failed.
*/
MEDIA_SUBSESSION_s* SvcH265Subsession_Create(void *pSrv, const void* Info, UINT32 IsLive)
{
    MEDIA_SUBSESSION_s* sub;
    SVC_RTSP_MEDIA_TRACK_CFG_s track;
    SVC_RTSP_MEDIA_TRACK_CFG_s* trk;
    SVC_STREAM_LIVE_MEDIA_s* liveMedia;
    AMBA_STREAM_s *Stm;
    SVC_RTSP_SERVER_s *Srv = (SVC_RTSP_SERVER_s*)pSrv;
    if (Srv == NULL || (IsLive == 0 && Info == NULL)) {
        AmbaPrint_PrintStr5("Invalid parameter.", NULL, NULL, NULL, NULL, NULL);
        return NULL;
    }
    sub = SvcRtspServer_GetSubsessionSlot(Srv);
    if (sub == NULL) {
        AmbaPrint_PrintStr5("no available subsession", NULL, NULL, NULL, NULL, NULL);
        return NULL;
    }

    if (IsLive == 0) {
        trk = (SVC_RTSP_MEDIA_TRACK_CFG_s*) Info;
        Stm = SvcRtspStrmCreate();
    } else if (IsLive == 1) {
        liveMedia = (SVC_STREAM_LIVE_MEDIA_s*) Info;
        Stm = SvcRtspStrmBind(&track, liveMedia->Stm, StreamerFunc, sub);
        trk = &track;
    } else {
        return NULL;
    }
    if (Stm == NULL) {
        return NULL;
    }

    sub->GetSdpLines = GetSdpLines;
    sub->CodecID = trk->nMediaId;
    sub->RefClock = 90000;

    sub->Streamer = Stm;
    sub->IsLive = IsLive;
    sub->MaxTransport = (sub->IsLive==0) ? 1 : MAX_SUBSESSION_TRANSPORT; //prevent from multi-playback on same file

    // streamerFunc need these information
    sub->VectorClk = (double) (double)sub->RefClock/(double)trk->nTimeScale;
    sub->TicksPerFrame = trk->nTimePerFrame * sub->VectorClk;
    sub->pBufferBase = trk->pBufferBase;
    sub->pBufferLimit = trk->pBufferLimit;

    return sub;
}

