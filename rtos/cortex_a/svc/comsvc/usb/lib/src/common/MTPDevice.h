/**
 *  @file MTPDevice.h
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
 *  @details Header file includes MTP device functions and variables.
 */
#ifndef MTP_DEVICE_H
#define MTP_DEVICE_H

#define MTP_STORAGE_ID                       0x10001U
#define MTP_OC_CUSTOM_COMMAND                0x9999U

/* Define PIMA object and thumb transfer phase.  */
#define MTP_OBJ_TRANS_PHASE_ACTIVE           0U
#define MTP_OBJ_TRANS_PHASE_COMPL            1U
#define MTP_OBJ_TRANS_PHASE_COMPL_ERROR      2U

/* Define PIMA Dataset equivalences.  */
#define MTP_OBJ_PROP_VALUE_GET               0x00U
#define MTP_OBJ_PROP_VALUE_GETSET            0x01U

/* Define local Audio codecs.  */
#define MTP_WAVE_FORMAT_MPEGLAYER3           0x00000055U
#define MTP_WAVE_FORMAT_MPEG                 0x00000050U
#define MTP_WAVE_FORMAT_RAW_AAC1             0x000000FFU

#define MTP_DATASET_VALUE_OFFSET             6U
#define MTP_MAX_HANDLES                      9999U

#define MTP_OBJECT_PROP_FORMAT_ALL           0x00000000U

#define MTP_OBJECT_PROP_ZERO_DEPTH           0x00000000U
#define MTP_OBJECT_PROP_ALL_DEPTH            0xFFFFFFFFU
#define MTP_OBJECT_PROP_ROOT_OBJECTS         0x00000000U
#define MTP_OBJECT_PROP_SPECIFIC_ROOT_OBJECT 0x00000001U
#define MTP_OBJECT_PROP_ALL_OBJECTS          0xFFFFFFFFU
#define MTP_DEVICE_PROP_ALL_OBJECTS          0xFFFFFFFFU

#define FLAG_PREPARSE_START                  0x01U
#define FLAG_PREPARSE_DONE                   0x02U

#define ExtractObjectInfoMode                0    // 0: sequential; 1: partially pre-parse;
#define PartialPreParseStartIndex            701U
#define MTP_MAX_FILENAME_LENGTH              64U

/* Define PIMA object info structure.  */
typedef struct {
    UINT8  FileName[USBD_MTP_MAX_FILENAME_LEN];
    UINT32 format;
    UINT32 size;
    UINT32 BitDepth;
    UINT32 height;
    UINT32 width;
    UINT32 WriteProtect;
} MTP_OBJECT_INFO_s;

/* Structure of the Object property dataset.  This is specific to the local device. */
typedef struct {
    UINT32 StorageId;
    UINT32 ObjectFormat;
    UINT32 ProtectionStatus;
    UINT32 ObjectSizeLow;
    UINT32 ObjectSizeHigh;
    UINT8  ObjectFileName[128];
    UINT32 ParentObject;
    UINT32 PersistentUniqueObjectIdentifier[4];
    UINT8  name[128];
    UINT8  NonConsumable;
    UINT8  artist[128];
    UINT32 track;
    UINT32 UseCount;
    UINT8  authored[16];
    UINT8  genre[128];
    UINT8  AlbumName[128];
    UINT8  AlbumArtist[128];
    UINT32 SampleRate;
    UINT32 NumOfChannels;
    UINT32 AudioWaveCodec;
    UINT32 AudioBitrate;
    UINT32 duration;
    UINT32 width;
    UINT32 height;
    UINT32 ScanType;
    UINT32 FourccCodec;
    UINT32 VideoBitrate;
    UINT32 FramePerThousandSeconds;
    UINT32 KeyframeDistance;
    UINT8  EncodingProfile[128];
} MTP_OBJECT_PROP_DATASET_s;

typedef struct {
    //AMBA_MEM_CTRL_s Desc;
    UINT32          Size;
    UINT8 *         Buffer;
} APP_MTPD_MEM_CTRL_s;

#define DEVICE_PROP_DATASET_STRING_LENGTH (256U)

typedef struct {
    // SyncPartner
    WCHAR SyncPartnerDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR SyncPartnerCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
    // DateTime
    WCHAR DateTimeDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR DateTimeCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
    // DeviceFriendlyName
    WCHAR DeviceFriendlyNameDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR DeviceFriendlyNameCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
    // Session Initiator Version Info
    WCHAR SessionInitVerInfoDefault[DEVICE_PROP_DATASET_STRING_LENGTH];
    WCHAR SessionInitVerInfoCurrent[DEVICE_PROP_DATASET_STRING_LENGTH];
} APP_MTPD_DEVICE_PROPS_s;

typedef struct {
    UINT32                  HandleNumber;
    USBD_MTP_DEVICE_INFO_s  DeviceInfo;
    AMBA_FS_FILE *          FpArray;
    ULONG                   OjectHandleOpened;
    APP_MTPD_DEVICE_PROPS_s DeviceProps;
} MTP_CLASS_INFO_s;

typedef struct {
    UINT32 ObjectHeight;
    UINT32 ObjectWidth;
    UINT64 ObjectLength;
    UINT32 ThumbHeight;
    UINT32 ThumbWidth;
    UINT32 ThumbSize;
} MTP_PREPARSE_INFO_s;

#endif /* MTP_DEVICE_H */
