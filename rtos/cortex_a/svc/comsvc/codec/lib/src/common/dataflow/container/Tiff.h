/**
 * @file Tiff.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */
#ifndef TIFF_H
#define TIFF_H

#include "format/SvcExif.h"
#include "MemByteOp.h"

#define SVC_FORMAT_EXIF_BIGENDIAN       0x00U    /**< big endian order */
#define SVC_FORMAT_EXIF_LITTLEENDIAN    0x01U    /**< little endian order */

#define TIFF_EXIF_AMBA_0_1_SIZE  64U /**< Amba box size (version 0_1) */
#define TIFF_EXIF_AMBA_1_0_SIZE  64U /**< Amba box size (version 1_0) */

#define TIFF_PIMTAGS 3U              /**< PIM tags*/

/**
 * Tag attribute
 */
#define TIFF_EXIF_OPTIONAL           ((UINT8)0x01U)  /**< Optional */
#define TIFF_EXIF_MANDATORY          ((UINT8)0x02U)  /**< Mandatory */
#define TIFF_EXIF_NONCONFIGURABLE    ((UINT8)0x04U)  /**< Nonconfigurable*/
#define TIFF_EXIF_COUNT_NONCONFIG    ((UINT8)0x08U)  /**< Count nonconfigurable */

/**
 *   Tiff tag type
 *
 *   TIFF_BYTE = 1,      1 byte
 *   TIFF_ASCII = 2,     1 byte
 *   TIFF_SHORT = 3,     2 bytes
 *   TIFF_LONG = 4,      4 bytes
 *   TIFF_RATIONAL = 5,  8 bytes
 *   TIFF_UNDEFINED = 7, 1 byte
 *   TIFF_SLONG = 9,     4 bytes
 *   TIFF_SRATIONAL = 10 8 bytes
 */
#define TIFF_BYTE       1U  /**< Byte */
#define TIFF_ASCII      2U  /**< ASCII */
#define TIFF_SHORT      3U  /**< Short */
#define TIFF_LONG       4U  /**< Long */
#define TIFF_RATIONAL   5U  /**< Rational */
#define TIFF_UNDEFINED  7U  /**< Undefined */
#define TIFF_SLONG      9U  /**< SLong */
#define TIFF_SRATIONAL  10U /**< SRational */
#define TIFF_TOTAL      11U /**< Total */
#define TIFF_POINTER    13U /**< Pointer */

/**
 * The structure of exif tag
 */
typedef struct {
    UINT8 Set;          /**< 0:disabled, 1:enabled, 2:configured */
    UINT8 Attribute;    /**< tag attribute*/
    UINT16 Type;        /**< tag type */
    UINT16 Tag;         /**< tag id */
    UINT32 Count;       /**< tag count */
    UINT32 Value;       /**< tag value */
    UINT8 *Data;        /**< tag data */
} SVC_TIFF_TAG_s;

/**
 * The structure of exif tag buffer
 */
typedef struct {
    UINT32 HeadOffset;  /**< The buffer offset of exif header*/
    UINT8 *HeadBase;    /**< The buffer base of exif header*/
    UINT32 HeadSize;    /**< The buffer size of exif header*/
    UINT32 InfoOffset;  /**< The buffer offset of exif data*/
    UINT8 *InfoBase;    /**< The buffer base of exif data*/
    UINT32 InfoSize;    /**< The buffer size of exif data*/
} SVC_TIFF_TAG_BUFFER_s;

/**
 * The structure of exif tag information
 */
typedef struct {
    UINT16 Ifd0Tags;        /**< IFD0 usage Tag number */
    UINT16 Ifd1Tags;        /**< IFD0 usage Tag number */
    UINT16 ExifIfdTags;     /**< EXIFIFD usage Tag number */
    UINT16 IntIfdTags;      /**< INITIFD usage Tag number */
    UINT16 GpsIfdTags;      /**< GPSIFD usage Tag number */
    UINT32 Ifd0HeaderSz;    /**< EXIF IFD0 header buffer size */
    UINT32 Ifd0InfoSz;      /**< EXIF IFD0 info buffer size */
    UINT32 Ifd1HeaderSz;    /**< EXIF IFD1 header buffer size */
    UINT32 Ifd1InfoSz;      /**< EXIF IFD1 info buffer size */
    UINT32 HeaderSz;        /**< EXIF total header buffer size */
    UINT32 InfoSz;          /**< EXIF total info buffer size */
    UINT32 AmbaBoxSz;       /**< The size of amba box */
    SVC_TIFF_TAG_s Ifd0[SVC_IFD0_TOTAL_TAGS];     /**< Tag status of IFD0 */
    SVC_TIFF_TAG_s ExifIfd[SVC_EXIF_TOTAL_TAGS];  /**< Tag status of EXIFIFD */
    SVC_TIFF_TAG_s IntIfd[SVC_IntIFD_TOTAL_TAGS]; /**< Tag status of INITIFD */
    SVC_TIFF_TAG_s Ifd1[SVC_IFD1_TOTAL_TAGS];     /**< Tag status of IFD1 */
    SVC_TIFF_TAG_s GpsIfd[SVC_GPS_TOTAL_TAGS];    /**< Tag status of GPSIFD */
} SVC_TIFF_TAG_INFO_s;

/**
 *  JPEG Marker
 */
#define Marker_SOI  0xFFD8U         /**< SOI */
#define Marker_APP1 0xFFE1U         /**< APP1 */
#define Marker_Exif 0x45786966U     /**< EXIF */
#define Marker_APP2 0xFFE2U         /**< APP2 */
#define Marker_FPXR 0x46505852U     /**< FPXR */
#define Marker_DQT  0xFFDBU         /**< DQT */
#define Marker_DHT  0xFFC4U         /**< DHT */
#define Marker_DRI  0xFFDDU         /**< DRI */
#define Marker_SOS  0xFFDAU         /**< SOS */
#define Marker_SOF  0xFFD9U         /**< SOF */
#define Marker_MM   0x4D4DU         /**< MM */
#define Marker_II   0X4949U         /**< II */

/**
 *  The dafault setting for exif tags
 */
extern SVC_TIFF_TAG_s Ifd0Tag[SVC_IFD0_TOTAL_TAGS];         /**< Default Ifd0 tags */
extern SVC_TIFF_TAG_s Ifd1Tag[SVC_IFD1_TOTAL_TAGS];         /**< Default Ifd1 tags */
extern SVC_TIFF_TAG_s ExifIfdTag[SVC_EXIF_TOTAL_TAGS];      /**< Default ExifIfd tags */
extern SVC_TIFF_TAG_s IntIfdTag[SVC_IntIFD_TOTAL_TAGS];     /**< Default IntIfd tags */
extern SVC_TIFF_TAG_s GpsIfdTag[SVC_GPS_TOTAL_TAGS];        /**< Default GpsIfd tags */
#if 0
extern SVC_TIFF_TAG_s IptcTag[SVC_IPTC_TOTAL_TAGS];         /**< Default Iptc tag */
extern SVC_TIFF_TAG_s MpidxTag[SVC_MPIDX_TOTAL_TAGS];       /**< Default Mpidx tag */
extern SVC_TIFF_TAG_s MpattrTag[SVC_MPATTR_TOTAL_TAGS];     /**< Default Mpattr tag */
#endif

/**
 * Function pointer of data operation for different endian order
 */
extern UINT32 (*SvcFormat_PutMem16)(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Code); /**< The function pointer to put 2 bytes data */
extern UINT32 (*SvcFormat_PutMem32)(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT32 Code); /**< The function pointer to put 4 bytes data */
extern UINT32 (*SvcFormat_GetMem16)(SVC_MEM_OP_s *Ctx, UINT16 *Buffer); /**< The function pointer to get 2 bytes data */
extern UINT32 (*SvcFormat_GetMem32)(SVC_MEM_OP_s *Ctx, UINT32 *Buffer);  /**< The function pointer to get 4 bytes data */
extern void (*SvcFormat_Convert64)(UINT8 *Buffer, UINT32 Num, UINT32 Deno); /**< The function pointer to convert 8 bytes data */

/**
 * Get the exif TIFF header
 *
 * @param [in] Ctx The context of memory operation
 * @param [out] Endian 0 - Big endian, 1 - Little Endian
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetTiffHeader(SVC_MEM_OP_s *Ctx, UINT8 *Endian);

/**
 * Set the endia order of exif tags
 *
 * @param [in] Endian The endian order to be set
 */
void SvcFormat_SetTiffHeader(UINT8 Endian);

/**
 * Put the exif tag entris to memory
 *
 * @param [in] Offset The current offset of memory buffer pointer
 * @param [in] Base The start address of memory buffer pointer
 * @param [in] Size The size of memory buffer
 * @param [in] Tag The id of the tag
 * @param [in] Type The type of the tag
 * @param [in] Count The count of the tag
 * @param [in] Param The parameter to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_PutIfdEntry(UINT32 *Offset, UINT8 *Base, UINT32 Size, UINT16 Tag, UINT16 Type, UINT32 Count, UINT32 Param);

/**
 * Put the exif tag information to memory, including entries and data. (Type : ASCII / BYTE / UNDEFINED)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put1Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data);

/**
 * Put the exif tag information to memory, including entries and data. (Type : SHORT)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put2Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data);

/**
 * Put the exif tag information to memory, including entries and data. (Type : LONG)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put4Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data);

/**
 * Put the exif tag information to memory, including entries and data. (Type : RATIONAL / SRATIONAL)
 *
 * @param [in] Tag The target tag
 * @param [in] Buffer The tag buffer status
 * @param [in] HeadSize The header size
 * @param [in] Data The data to be put
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_Put8Byte(const SVC_TIFF_TAG_s *Tag, SVC_TIFF_TAG_BUFFER_s *Buffer, UINT32 HeadSize, const UINT8 *Data);

/**
 * Get the exif tag entries from memory
 *
 * @param [in] Ctx The context of memory operation
 * @param [in,out] Tag The id of the tag
 * @param [in,out] Type The type of the tag
 * @param [in,out] Count The count of the tag
 * @param [in,out] Param The parameter of the tag
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFormat_GetIfdEntry(SVC_MEM_OP_s *Ctx, UINT16 *Tag, UINT16 *Type, UINT32 *Count, UINT32 *Param);

#endif /* TIFF_H */
