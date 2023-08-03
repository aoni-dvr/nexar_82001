/**
 * @file IsoBoxDef.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
#ifndef CONTAINER_ISO_BOX_DEF__H
#define CONTAINER_ISO_BOX_DEF__H

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include "format/SvcFormatDef.h"

/**
 * Iso box type
 */
#define TAG_ALIS 0x616c6973U     /**< 'ALIS' */
#define TAG_AMBA 0x414D4241U     /**< 'AMBA' */
#define TAG_AVC1 0x61766331U     /**< 'avc1' */
#define TAG_HVC1 0x68766331U     /**< 'hvc1' */
#define TAG_AVCC 0x61766343U     /**< 'avcc' */
#define TAG_HVCC 0x68766343U     /**< 'hvcc' */
#define TAG_CTTS 0x63747473U     /**< 'ctts' */
#define TAG_DINF 0x64696E66U     /**< 'dinf' */
#define TAG_DREF 0x64726566U     /**< 'dref' */
#define TAG_EDTS 0x65647473U     /**< 'edts' */
#define TAG_ESDS 0x65736473U     /**< 'esds' */
#define TAG_ELST 0x656C7374U     /**< 'elst' */
#define TAG_FTYP 0x66747970U     /**< 'ftyp' */
#define TAG_FREE 0x66726565U     /**< 'free' */
#define TAG_GMHD 0x676D6864U     /**< 'gmhd' */
#define TAG_GMIN 0x676d696eU     /**< 'gmin' */
#define TAG_HDLR 0x68646C72U     /**< 'hdlr' */
#define TAG_MINF 0x6D696E66U     /**< 'minf' */
#define TAG_MDAT 0x6D646174U     /**< 'mdat' */
#define TAG_MDHD 0x6D646864U     /**< 'mdhd' */
#define TAG_UUID 0x75756964U     /**< 'uuid' */
#define TAG_MDIA 0x6D646961U     /**< 'mdia' */
#define TAG_MOOV 0x6D6F6F76U     /**< 'moov' */
#define TAG_MP4A 0x6D703461U     /**< 'mp4a' */
#define TAG_IN24 0x696e3234U     /**< 'in24' */
#define TAG_LPCM 0x6C70636DU     /**< 'lpcm' */
#define TAG_SOWT 0x736f7774U     /**< 'sowt' */
#define TAG_TWOS 0x74776f73U     /**< 'twos' */
#define TAG_MS0011 0x6d730011U   /**< 'ms0011' */
#define TAG_WAVE 0x77617665U     /**< 'wave' */
#define TAG_FRMA 0x66726d61U     /**< 'frma' */
#define TAG_ENDA 0x656e6461U     /**< 'enda' */
#define TAG_MP4S 0x6D703473U     /**< 'mp4s' */
#define TAG_MVHD 0x6D766864U     /**< 'mvhd' */
#define TAG_NMHD 0x6E6D6864U     /**< 'nmhd' */
#define TAG_QT__ 0x71742020U     /**< 'qt'   */
#define TAG_SMHD 0x736D6864U     /**< 'smhd' */
#define TAG_SOUN 0x736F756EU     /**< 'soun' */
#define TAG_SOWT 0x736f7774U     /**< 'sowt' */
#define TAG_STBL 0x7374626CU     /**< 'stbl' */
#define TAG_STSD 0x73747364U     /**< 'stsd' */
#define TAG_STTS 0x73747473U     /**< 'stts' */
#define TAG_STSC 0x73747363U     /**< 'stsc' */
#define TAG_STSZ 0x7374737AU     /**< 'stsz' */
#define TAG_STZ2 0x73747A32U     /**< 'stz2' */
#define TAG_STCO 0x7374636FU     /**< 'stco' */
#define TAG_CO64 0x636f3634U     /**< 'co64' */
#define TAG_STSS 0x73747373U     /**< 'stss' */
#define TAG_TEXT 0x74657874U     /**< 'text' */
#define TAG_SDSM 0x7364736DU     /**< 'sdsm' */
#define TAG_TKHD 0x746b6864U     /**< 'tkhd' */
#define TAG_TRAK 0x7472616bU     /**< 'trak' */
#define TAG_TWOS 0x74776f73U     /**< 'twos' */
#define TAG_UDTA 0x75647461U     /**< 'udta' */
#define TAG_VMHD 0x766D6864U     /**< 'vmhd' */
#define TAG_VIDE 0x76696465U     /**< 'vide' */
#define TAG_URL_ 0x75726C20U     /**< 'url' */
#define TAG_PASP 0x70617370U     /**< 'pasp' */

#define TAG_MVEX 0x6d766578U     /**< 'mvex' */
#define TAG_MEHD 0x6d656864U     /**< 'mehd' */
#define TAG_TREX 0x74726578U     /**< 'trex' */
#define TAG_MOOF 0x6d6f6f66U     /**< 'moof' */
#define TAG_MFHD 0x6d666864U     /**< 'mfhd' */
#define TAG_TRAF 0x74726166U     /**< 'traf' */
#define TAG_TFHD 0x74666864U     /**< 'tfhd' */
#define TAG_TRUN 0x7472756eU     /**< 'trun' */
#define TAG_TFDT 0x74666474U     /**< 'tfdt' */

#define TAG_isom 0x69736F6DU     /**< 'isom' */
#define TAG_iso4 0x69736F34U     /**< 'iso4' */

#define ISO_HDLR_MAX_COM_NAME_LEN   14U  /**< Max hdlr Component name length */
#define ISO_STSD_MAX_ENC_NAME_LEN   32U  /**< Max stsd encode name length */
#define ISO_STSD_MAX_TEXT_NAME_LEN  6U   /**< Max stsd text name length */
#define ISO_TIMECODE_FONT_NAME_LEN  10U  /**< Max timecode font name length */

/* Box types, structure
 *
 *                      free
 *                      mdat
 * stsd
 * stsc
 * stts
 * stco
 * stsz
 * stss
 * ctts
 *      stbl
 * dref
 *      dinf
 *      vmhd / gmhd /smhd
 *          minf
 *          hdlr
 *          mdhd
 *              mdia
 *          elst
 *              edts
 *              tref
 *              tkhd
 *                  trak
 *                  udta
 *                  mvhd
 *                      moov
 *                      ftyp
 *
 */

/**
 *  ISO Box
 */
typedef struct{
    UINT32 Size;    /**< Box size */
    UINT32 Type;    /**< Box type */
}ISO_BOX_s;

/**
 *  ISO Full Box
 */
typedef struct{
    ISO_BOX_s Box;      /**< ISO_BOX_s */
    UINT8 Version;      /**< Box version */
    UINT8 Flags[3];     /**< Box flag */
} ISO_FULL_BOX_s;

/**
 *  HVCC('hvcc')
 *  video sample description for HEVC video Extension. Codec type is 'hvc1'.(ISO/IEC 14496-15)
 */
typedef struct {
    ISO_BOX_s Box;      /**< ISO_BOX_s */
    UINT8 ConfigVersion;            /**< Configure version */
    UINT8 Generalconfiguration[SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH]; /**< VPS general config */
    UINT8 ChromaFormat;             /**< chroma format in sps */
    UINT8 BitDepthLumaMinus8;       /**< The bit depth of the samples of the luma in sps */
    UINT8 BitDepthChromaMinus8;     /**< The bit depth of the samples of the chroma in sps */
    UINT8 NumTemporalLayers;        /**< Profile information */
    UINT8 TemporalIdNested;         /**< Nested level */
    UINT8 NumofArray;       /**< The number of parameter set */
    UINT8 NALULength;       /**< NALU length */
    UINT8 PicSizeInCtbsY;/**< PicSizeInCtbsY */
    UINT16 VPSNum;       /**< VPS count */
    UINT16 SPSNum;       /**< SPS count */
    UINT16 PPSNum;       /**< PPS count */
    UINT16 VPSLength;   /**< VPS length */
    UINT16 SPSLength;   /**< SPS length */
    UINT16 PPSLength;   /**< PPS length */
    UINT8 VPSCtx[SVC_FORMAT_MAX_VPS_LENGTH];    /**< VPS context */
    UINT8 SPSCtx[SVC_FORMAT_MAX_SPS_LENGTH];    /**< SPS context */
    UINT8 PPSCtx[SVC_FORMAT_MAX_PPS_LENGTH];    /**< PPS context */
} HVCC_ENTRY_s;

/**
 *  AVCC('avcc')
 *  video sample description for H.264 video Extension. Codec type is 'avc1'.(ISO/IEC 14496-15)
 */
typedef struct {
    ISO_BOX_s Box;      /**< ISO_BOX_s */
    UINT16 SPSLength;   /**< SPS length */
    UINT16 PPSLength;   /**< PPS length */
    UINT8 SPSCtx[SVC_FORMAT_MAX_SPS_LENGTH];    /**< SPS context */
    UINT8 PPSCtx[SVC_FORMAT_MAX_PPS_LENGTH];    /**< PPS context */
    UINT8 PPSNum;       /**< PPS count */
    UINT8 ConfigVersion;            /**< Configure version */
    UINT8 AVCProfileIndication;     /**< AVC profile indication */
    UINT8 ProfileCompatibility;     /**< Profile compatibility */
    UINT8 AVCLevelIndication;       /**< AVC level indication */
    UINT8 SPSNum;       /**< SPS count */
    UINT8 NALULength;   /**< NALU length */
} AVCC_ENTRY_s;

/**
 *  Pixel Aspect Ratio('pasp')
 *  The height-to-width ratio of pixels.
 */
typedef struct {
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    UINT32 HSpacing;        /**< Horizontal spacing of pixels */
    UINT32 VSpacing;        /**< Vertical spacing of pixels */
} PASP_ENTRY_s;

/**
 *  Video Description
 */
typedef struct{
    HVCC_ENTRY_s Hvcc;  /**< HVCC_ENTRY_s */
    AVCC_ENTRY_s Avcc;  /**< AVCC_ENTRY_s */
} VIDEO_DESCRIPTION_s;
/**
 *  Video Sample Entry('stsd')
 */
typedef struct{
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    VIDEO_DESCRIPTION_s DecCfg; /**< Video description */
    PASP_ENTRY_s PaspEntry; /**< Pixel Aspect Ratio */
    UINT32 Predefined[3];   /**< Pre define */
    UINT32 HorRes;          /**< Hor res */
    UINT32 VerRes;          /**< Ver res */
    UINT32 Reserv4;         /**< Reserve */
    UINT16 DataRefIdx;      /**< Data reference index */
    UINT16 Version;         /**< Version */
    UINT16 Revison;         /**< Revison */
    UINT16 Width;           /**< Width */
    UINT16 Height;          /**< Height */
    UINT16 FrameCount;      /**< Count of frame */
    UINT16 Depth;           /**< Depth */
    UINT16 Color;           /**< Color */
    char Name[ISO_STSD_MAX_ENC_NAME_LEN];   /**< additional 1 byte for NULL termination */
    UINT8 Reserv6[6];       /**< Reserve */
} STSD_VIDEO_ENTRY_s;


/**
 *  BOX_ESDS Entry('esds')
 */
typedef struct {
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    UINT32 MaxBrate;        /**< Max bitrate */
    UINT32 AvgBrate;        /**< Average bitrate */
    UINT16 AACinfo;         /**< AAC info */
} ESDS_ENTRY_s;

/**
 *  Audio Sample Entry('stsd')
 */
typedef struct{
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    ESDS_ENTRY_s Esds;      /**< ESDS_ENTRY_s */
    UINT32 Vendor;          /**< Vendor */
    UINT32 SamplesPerChunk; /**< Sample per chunk */
    UINT32 BytesPerFrame;   /**< Byte per frame */
    UINT8* Extension;       /**< esds : sound sample description for MPEG-4 audio Extension. (ISO/IEC FDIS 14496) TODO: */
    UINT16 DataRefIdx;      /**< Data reference index */
    UINT16 Version;         /**< Sample count */
    UINT16 Revison;         /**< Sample offset */
    UINT16 Channels;        /**< Channel number */
    UINT16 BitsPerSample;   /**< Bits per sample */
    UINT16 CompressedID;    /**< Compressed Id */
    UINT16 PacketSize;      /**< Packer size */
    UINT32 SampleRate;      /**< Sample rate */
    UINT8 Reserv6[6];       /**< Reserve */
} STSD_AUDIO_ENTRY_s;

/**
 *  Text Sample Entry('stsd')
 */
typedef struct{
    ISO_BOX_s Box;      /**< ISO_BOX_s */
    UINT32 DisFlags;    /**< Display flag */
    UINT32 TextJust;    /**< Text justification */
    UINT64 TextBox;     /**< Text box */
    UINT16 BgColor[3];  /**< Background color */
    UINT16 FgColor[3];  /**< Frontground color */
    UINT16 FontNumber;  /**< Font number */
    UINT16 FontFace;    /**< Font face */
    UINT16 DataRefIdx;  /**< Data reference index */
    UINT8 Reserv8[8];   /**< Reserve */
    UINT8 Reserv3[3];   /**< Reserve */
    UINT8 Reserv6[6];   /**< Reserve */
    char TextName[ISO_STSD_MAX_TEXT_NAME_LEN];      /**< Default, Using 9 size for alignment */
} STSD_TEXT_ENTRY_s;

/**
 *  Video Sample Description Box('stsd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;         /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;             /**< Video Stsd entry count */
    STSD_VIDEO_ENTRY_s VideoEntry;  /**< STSD_VIDEO_ENTRY_s */
} ISO_STSD_VIDEO_BOX_s;

/**
 *  Video Sample Description Box('stsd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;         /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;             /**< Audio Stsd entry count */
    STSD_AUDIO_ENTRY_s AudioEntry;  /**< STSD_AUDIO_ENTRY_s */
} ISO_STSD_AUDIO_BOX_s;

/**
 *  Video Sample Description Box('stsd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;         /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;              /**< Text Stsd entry count */
    STSD_TEXT_ENTRY_s TextEntry;    /**< STSD_TEXT_ENTRY_s */
} ISO_STSD_TEXT_BOX_s;

/**
 *  Sample Entry('stsc')
 */
typedef struct{
    UINT32 FirstChunk;      /**< First_chunk, run length coding */
    UINT32 SamPerChunk;     /**< Samples per chunk */
    UINT32 SamDescIdx;      /**< Sample description index */
} STSC_ENTRY_s;

/**
 *  Sample To Chunk Box('stsc')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;          /**< Stsc entry count */
    STSC_ENTRY_s StscTable;     /**< STSC_ENTRY_s */
} ISO_STSC_BOX_s;

/**
 *  Sample Entry('stts')
 */
typedef struct {
    UINT32 SampleCount;     /**< Sample count */
    UINT32 SampleDelta;     /**< Time-scale of the media */
} STTS_ENTRY_s;

/**
 *  Decoding Time to Sample Box('stts')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;      /**< Stts entry count */
    UINT32 TmpOff;          /**< Stts offset in index file */
    UINT32 EntryUnit;       /**< Stts entry size */
    STTS_ENTRY_s SttsTable; /**< Stts table */
    UINT8 ItemId;           /**< Item id */
} ISO_STTS_BOX_s;

/**
 *  Chunk Offset Box('stco')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;      /**< Stco entry count */
    UINT32 TmpOff;          /**< Stco offset in index file */
    UINT32 EntryUnit;       /**< Stco entry size */
    UINT8 EnableCO64;       /**< EnableCO64 */
    UINT8 ItemId;           /**< Item id */
//  UINT32 *pChunkOffset;
} ISO_STCO_BOX_s;

/**
 *  Sample Size Box('stsz')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT32 SampleSize;      /**< Sample size */
    UINT32 SampleCount;     /**< Sample count */
    UINT32 TmpOff;          /**< Stsz offset in index file */
    UINT32 EntryUnit;       /**< Stsz entry size */
    UINT8 ItemId;           /**< Item id */
//  UINT32 *pEntrySize;
} ISO_STSZ_BOX_s;

/**
 *  Sample Entry('ctts')
 */
typedef struct {
    UINT32 SampleCount;     /**< Count, always 1 */
    UINT32 SampleOffset;    /**< Ctts */
} CTTS_ENTRY_s;

/**
 *  Composition Time to Sample Box('ctts')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;      /**< Ctts entry count */
    UINT32 TmpOff;          /**< Ctts offset in index file */
    UINT32 EntryUnit;       /**< Ctts entry size */
    UINT8 ItemId;           /**< Item id */
//  CTTS_ENTRY_S* pCttsTable;
} ISO_CTTS_BOX_s;

/**
 *  Sync Sample Box('stss')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;      /**< Stss entry count */
    UINT32 TmpOff;          /**< Stss offset in index file */
    UINT32 EntryUnit;       /**< Stss entry size */
    UINT8 ItemId;           /**< Item id */
//  UINT32 *pSampleNumber;
} ISO_STSS_BOX_s;

/**
 *  Video Sample Table Box('stbl')
 *  (no B frame)
 */
typedef struct{
    ISO_BOX_s Box;  /**< ISO_BOX_s */
    ISO_STSD_VIDEO_BOX_s StsdAtom;  /**< ISO_STSD_VIDEO_BOX_s */
    ISO_STSC_BOX_s StscAtom;    /**< ISO_STSC_BOX_s */
    ISO_STTS_BOX_s SttsAtom;    /**< ISO_STTS_BOX_s */
    ISO_STCO_BOX_s StcoAtom;    /**< ISO_STCO_BOX_s */
    ISO_STSZ_BOX_s StszAtom;    /**< ISO_STSZ_BOX_s */
} ISO_STBL_VIDEO_NOCTTS_BOX_s;

/**
 *  Video Sample Table Box('stbl')
 *  (Within B frame)
 */
typedef struct{
    ISO_BOX_s Box;   /**< ISO_BOX_s */
    ISO_STSD_VIDEO_BOX_s StsdAtom;  /**< ISO_STSD_VIDEO_BOX_s */
    ISO_STSC_BOX_s StscAtom;    /**< ISO_STSC_BOX_s */
    ISO_STTS_BOX_s SttsAtom;    /**< ISO_STTS_BOX_s */
    ISO_STCO_BOX_s StcoAtom;    /**< ISO_STCO_BOX_s */
    ISO_STSZ_BOX_s StszAtom;    /**< ISO_STSZ_BOX_s */
    ISO_CTTS_BOX_s CttsAtom;    /**< ISO_CTTS_BOX_s */
    ISO_STSS_BOX_s StssAtom;    /**< ISO_STSS_BOX_s */
} ISO_STBL_VIDEO_BOX_s;

/**
 *  Audio Sample Table Box('stbl')
 */
typedef struct{
    ISO_BOX_s Box;   /**< ISO_BOX_s */
    ISO_STSD_AUDIO_BOX_s StsdAtom;  /**< ISO_STSD_AUDIO_BOX_s */
    ISO_STSC_BOX_s StscAtom;    /**< ISO_STSC_BOX_s */
    ISO_STTS_BOX_s SttsAtom;    /**< ISO_STTS_BOX_s */
    ISO_STCO_BOX_s StcoAtom;    /**< ISO_STCO_BOX_s */
    ISO_STSZ_BOX_s StszAtom;    /**< ISO_STSZ_BOX_s */
    ISO_STSS_BOX_s StssAtom;    /**< ISO_STSS_BOX_s */
} ISO_STBL_AUDIO_BOX_s;

/**
 *  Text Sample Table Box('stbl')
 */
typedef struct{
    ISO_BOX_s Box;   /**< ISO_BOX_s */
    ISO_STSD_TEXT_BOX_s StsdAtom;   /**< ISO_STSC_BOX_s */
    ISO_STSC_BOX_s StscAtom;    /**< ISO_STSC_BOX_s */
    ISO_STTS_BOX_s SttsAtom;    /**< ISO_STTS_BOX_s */
    ISO_STCO_BOX_s StcoAtom;    /**< ISO_STCO_BOX_s */
    ISO_STSZ_BOX_s StszAtom;    /**< ISO_STSZ_BOX_s */
    ISO_STSS_BOX_s StssAtom;    /**< ISO_STSS_BOX_s */
} ISO_STBL_TEXT_BOX_s;

/**
 *  Data Entry Box('dref')
 */
typedef struct {
    ISO_BOX_s Box;      /**< ISO_BOX_s */
    UINT8 Version;      /**< Version */
    UINT8 Flags[3];     /**< Flag */
} DREF_ENTRY_s;

/**
 *  Data Reference Box('dref')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;          /**< Dref entry count */
    DREF_ENTRY_s DrefTable;     /**< DREF_ENTRY_s */
} ISO_DREF_BOX_s;

/**
 *  Data Information Box('dinf')
 */
typedef struct{
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_DREF_BOX_s DrefAtom;    /**< ISO_DREF_BOX_s */
} ISO_DINF_BOX_s;

/**
 *  Timecode media information('tcmi')
 */
typedef struct {
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT16 TextColor[3];    /**< Text Color */
    UINT16 BgColor[3];      /**< Background color */
    UINT16 TextFont;        /**< Text font */
    UINT16 TextFace;        /**< Text face */
    UINT16 TextSize;        /**< Text size */
    char FontName[ISO_TIMECODE_FONT_NAME_LEN];     /**< Font name */
} TIMECODE_INFO_ENTRY_s;

/**
 *  Timecode media('tmcd')
 */
typedef struct {
    ISO_BOX_s Box;                  /**< ISO_BOX_s */
    TIMECODE_INFO_ENTRY_s TcmiAtom; /**< TIMECODE_INFO_ENTRY_s */
} TIMECODE_ENTRY_S;

/**
 *  Base media info('gmin')
 */
typedef struct {
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT16 GraphicMode;     /**< Graphic mode */
    UINT16 Opcolor[3];      /**< Opcolor */
    UINT16 Balanced;        /**< Balance */
    UINT16 Reserv2;         /**< Reserve */
} ISO_GMIN_BOX_s;

/**
 *  Video Media Header Box('vmhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT16 GraphicMode;     /**< Video graphic mode */
    UINT16 Opcolor[3];      /**< Video opcolor */
} ISO_VMHD_BOX_s;

/**
 *  Sound Media Header Box('smhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT16 Balanced;        /**< Sound balance */
    UINT16 Reserv2;         /**< Reserve */
} ISO_SMHD_BOX_s;

/**
 *  Text Entry('gmhd')
 */
typedef struct {    //TODO : Cannot find the data in Spec.
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    UINT16 PreDefined[18];  /**< PreDefined */
} TEXT_ENTRY_S;

/**
 *  GMHD handler atom ('gmhd')
 */
typedef struct{
    TEXT_ENTRY_S TextHdlr;          /**< TEXT_ENTRY_S */
    TIMECODE_ENTRY_S TimecodeHdlr;  /**< TIMECODE_ENTRY_S */
} ISO_GMHD_HDLR_ATOM_s;

/**
 *  Base Media Information Header Box('gmhd')
 */
typedef struct{
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_GMIN_BOX_s GminAtom;    /**< ISO_GMIN_BOX_s */
    UINT8 HdlrType;             /**< Handler type */
    ISO_GMHD_HDLR_ATOM_s HdlrAtom;  /**< ISO_GMHD_HDLR_ATOM_s */
} ISO_GMHD_BOX_s;

/**
 *  Null Media Header Box('nmhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT16 Balanced;        /**< Balanced */
    UINT16 Reserv2;         /**< Reserved */
} ISO_NMHD_BOX_s;

/**
 *  Media atom ('minf')
 */
typedef struct{
    ISO_VMHD_BOX_s VmhdAtom;    /**< ISO_VMHD_BOX_s */
    ISO_SMHD_BOX_s SmhdAtom;    /**< ISO_SMHD_BOX_s */
    ISO_GMHD_BOX_s GmhdAtom;    /**< ISO_GMHD_BOX_s */
    ISO_NMHD_BOX_s NmhdAtom;    /**< ISO_NMHD_BOX_s */
} ISO_MEDIA_ATOM_s;

/**
 *  Stbl atom ('minf')
 */
typedef struct{
    ISO_STBL_VIDEO_BOX_s VideoStbl1;        /**< ISO_STBL_VIDEO_BOX_s */
    ISO_STBL_VIDEO_NOCTTS_BOX_s VideoStbl2; /**< ISO_STBL_VIDEO_NOCTTS_BOX_s */
    ISO_STBL_AUDIO_BOX_s AudioStbl;         /**< ISO_STBL_AUDIO_BOX_s */
    ISO_STBL_TEXT_BOX_s TextStbl;           /**< ISO_STBL_TEXT_BOX_s */
} ISO_STBL_ATOM_s;

/**
 *  Media information Box('minf')
 */
typedef struct{
    UINT8 MediaType;    /**< Media type */
    ISO_BOX_s Box;      /**< ISO_BOX_s */
    ISO_MEDIA_ATOM_s MediaAtom; /**< ISO_STBL_ATOM_s */
    ISO_DINF_BOX_s DinfAtom;    /**< ISO_DINF_BOX_s */
    ISO_STBL_ATOM_s StblAtom;   /**< ISO_STBL_ATOM_s */
} ISO_MINF_BOX_s;

/**
 *  Handler Reference Box('hdlr')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 ComType;             /**< MP4 - PreDefined */
    UINT32 ComSubType;          /**< MP4 - HandlerType */
    UINT32 ComManufacturer;     /**< MP4 - Resv[0] */
    UINT32 ComFlags;            /**< MP4 - Resv[1] */
    UINT32 ComFlagMask;         /**< MP4 - Resv[2] */
    char ComName[ISO_HDLR_MAX_COM_NAME_LEN];            /**< MP4 - Name */
} ISO_HDLR_BOX_s;

/**
 *  Media header Box('mdhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT32 CreateTime;      /**< Media create time */
    UINT32 ModifyTime;      /**< Media modification time */
    UINT32 TimeScale;       /**< Time scale */
    UINT32 Duration;        /**< Duration */
    UINT16 Language;        /**< Language */
    UINT16 Quality;         /**< Quality */
} ISO_MDHD_BOX_s;

/**
 *  Media Box('mdia')
 */
typedef struct{
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_MDHD_BOX_s MdhdAtom;    /**< ISO_MDHD_BOX_s */
    ISO_HDLR_BOX_s HdlrAtom;    /**< ISO_HDLR_BOX_s */
    ISO_MINF_BOX_s MinfAtom;    /**< ISO_MINF_BOX_s */
} ISO_MDIA_BOX_s;

/**
 *  Edit List Box entry
 */
typedef struct{
    UINT32 SegmentDuration;     /**< Segment duration */
    UINT32 MediaTime;           /**< Media time */
    UINT32 MediaRate;           /**< Media rate */
} ISO_ELST_ENTRY_S;

/**
 *  Edit List Box('elts')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;         /**< ISO_FULL_BOX_s */
    UINT32 EntryCount;              /**< Count of ELST table */
    ISO_ELST_ENTRY_S ElstTable1;    /**< ISO_ELST_ENTRY_S */
    ISO_ELST_ENTRY_S ElstTable2;    /**< ISO_ELST_ENTRY_S */
} ISO_ELST_BOX_s;

/**
 *  Edit Box('edts')
 */
typedef struct {
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_ELST_BOX_s ElstAtom;    /**< ISO_ELST_BOX_s */
} ISO_EDTS_BOX_s;


/**
 *  Uuid Box('uuid')
 */
typedef struct {
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    UINT32 Identifiers[4];  /**< Identifiers */
} ISO_UUID_BOX_s;

/**
 *  Track Reference type Box
 */
typedef struct{
    ISO_BOX_s Box;      /**< ISO_BOX_s */
    UINT32 TrackId;     /**< Track Id */
} TREF_ENTRY_s;

/**
 *  Track Reference Box('tref')
 */
typedef struct{
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    TREF_ENTRY_s TrefEntry; /**< TREF_ENTRY_s */
} ISO_TREF_BOX_s;

/**
 *  Track Header Box('tkhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox; /**< ISO_FULL_BOX_s */
    UINT32 CreateTime;      /**< Media create time */
    UINT32 ModifyTime;      /**< Media modification time */
    UINT32 TrackID;         /**< Track id of the track */
    UINT32 Reserv4;         /**< Reserve */
    UINT32 Duration;        /**< Track Duration */
    UINT32 Reserv8[2];      /**< Reserve */
    UINT32 Rotation;        /**< Track rotation parameter */
    UINT32 Width;           /**< Track width */
    UINT32 Height;          /**< Track height */
    UINT16 Layer;           /**< Track layer */
    UINT16 Group;           /**< Track group */
    UINT16 Volume;          /**< Track volume */
    UINT16 Reserv2;         /**< Reserve */
} ISO_TKHD_BOX_s;

/**
 *  Video Track Box('trak')
 */
typedef struct{
    ISO_TKHD_BOX_s TkhdAtom;   /**< ISO_TKHD_BOX_s */
    ISO_EDTS_BOX_s EdtsAtom;   /**< ISO_EDTS_BOX_s */
    ISO_MDIA_BOX_s MdiaAtom;   /**< ISO_MDIA_BOX_s */
    ISO_UUID_BOX_s UuidAtom;   /**< ISO_UUID_BOX_s */
} ISO_VIDEO_TRAK_BOX_s;

/**
 *  Audio Track Box('trak')
 */
typedef struct{
    ISO_TKHD_BOX_s TkhdAtom;   /**< ISO_TKHD_BOX_s */
    ISO_EDTS_BOX_s EdtsAtom;   /**< ISO_EDTS_BOX_s */
    ISO_MDIA_BOX_s MdiaAtom;   /**< ISO_MDIA_BOX_s */
} ISO_AUDIO_TRAK_BOX_s;

/**
 *  Text Track Box('trak')
 */
typedef struct{
    ISO_TKHD_BOX_s TkhdAtom;   /**< ISO_TKHD_BOX_s */
    ISO_EDTS_BOX_s EdtsAtom;   /**< ISO_EDTS_BOX_s */
    ISO_MDIA_BOX_s MdiaAtom;   /**< ISO_MDIA_BOX_s */
} ISO_TEXT_TRAK_BOX_s;

/**
 *  Iso track atom
 */
typedef struct{
    ISO_VIDEO_TRAK_BOX_s Video;     /**< ISO_VIDEO_TRAK_BOX_s */
    ISO_AUDIO_TRAK_BOX_s Audio;     /**< ISO_AUDIO_TRAK_BOX_s */
    ISO_TEXT_TRAK_BOX_s Text;       /**< ISO_TEXT_TRAK_BOX_s */
} ISO_TRACK_ATOM_s;

/**
 *  Iso Track Box
 */
typedef struct {
    ISO_BOX_s Box;          /**< ISO_BOX_s */
    ISO_TRACK_ATOM_s Info;  /**< ISO_TRACK_ATOM_s */
    UINT8 TrackType;        /**< Track type of the track */
} ISO_TRACK_BOX_s;

/**
 *  User Data Box('udta')
 */
typedef struct{
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    INT64 UserDataPos;          /**< User data position */
    UINT32 UserDataBufferSize;  /**< User data buffer size */
    UINT32 UserDataSize;        /**< User data size */
    UINT8 *UserData;            /**< User data */
} ISO_UDTA_BOX_s;

/**
 *  Track fragment run sample ('trun')
 */
typedef struct{
    UINT32 SampleDuration;  /**< Sample duration */
    UINT32 SampleSize;      /**< Sample size */
    UINT32 SampleFlags;     /**< Sample flags */
    UINT32 SampleCompositionTimeOffset; /**< Sample composition time offset */
} ISO_TRUN_SAMPLE_s;

/**
 *  Track Fragment Run Box('trun')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 SampleCount;         /**< Sample count */
    /* Optional*/
    UINT32 DataOffset;          /**< Data offset */
    UINT32 FirstSampleFlags;    /**< First sample flags */
    ISO_TRUN_SAMPLE_s Samples[SVC_ISO_MAX_FRAME_PER_FRAG];  /**< ISO_TRUN_SAMPLE_s */
} ISO_TRUN_BOX_s;

/**
 *  Track Fragment Header Box('tfhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 TrackId;             /**< Track id */
    /* Optional*/
    UINT64 BaseDataOffset;                  /**< Base data offset */
    UINT32 DefaultSampleDescriptionIndex;   /**< Default sample description index */
    UINT32 DefaultSampleDuration;           /**< Default sample duration */
    UINT32 DefaultSampleSize;               /**< Default sample size */
    UINT32 DefaultSampleFlags;              /**< Default sample flags */
} ISO_TFHD_BOX_s;


/**
 *  Track Fragment Decode Time Box('tfdt')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 BaseMediaDecodeTime; /**< Base media decode time */
} ISO_TFDT_BOX_s;

/**
 *  Track Fragment Box('traf')
 */
typedef struct{
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_TFHD_BOX_s TfhdAtom;    /**< ISO_TFHD_BOX_s */
    ISO_TFDT_BOX_s TfdtAtom;    /**< ISO_TFDT_BOX_s */
    ISO_TRUN_BOX_s TrunAtom;    /**< ISO_TRUN_BOX_s */
    UINT32 SampleCount;         /**< Sample count */
} ISO_TRAF_BOX_s;

/**
 *  Movie Fragment Header Box('mfhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 SequenceNum;         /**< Sequence number */
} ISO_MFHD_BOX_s;

/**
 *  Movie Fragment Box('moof')
 */
typedef struct{
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_MFHD_BOX_s MfhdAtom;    /**< ISO_MFHD_BOX_s */
    ISO_TRAF_BOX_s TrafAtom[SVC_FORMAT_MAX_TRACK_PER_MEDIA];    /**< ISO_TRAF_BOX_s */
    UINT32 TrackCount;          /**< Track count */
} ISO_MOOF_BOX_s;

/**
 *  Track Extends Box('trex')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 TrackId;             /**< Track id */
    UINT32 DefaultSampleDescriptionIndex;   /**< Default sample description index */
    UINT32 DefaultSampleDuration;           /**< Default sample duration */
    UINT32 DefaultSampleSize;               /**< Default sample size */
    UINT32 DefaultSampleFlags;              /**< Default sample flags */
} ISO_TREX_BOX_s;

/**
 *  Movie Extends Header Box('mehd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 FragmentDuration;    /**<Fragment duration */
} ISO_MEHD_BOX_s;

/**
 *  Movie Extends Box('mvex')
 */
typedef struct{
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_MEHD_BOX_s MehdAtom;    /**< ISO_MEHD_BOX_s */
    ISO_TREX_BOX_s TrexAtom[SVC_FORMAT_MAX_TRACK_PER_MEDIA];    /**< ISO_TREX_BOX_s */
    UINT8          EnableMehd;
} ISO_MVEX_BOX_s;

/**
 *  Movie Header Box('mvhd')
 */
typedef struct{
    ISO_FULL_BOX_s FullBox;     /**< ISO_FULL_BOX_s */
    UINT32 CreateTime;          /**< Movie create time */
    UINT32 ModifyTime;          /**< Movie modify time */
    UINT32 TimeScale;           /**< Movie time scale */
    UINT32 Duration;            /**< Movie duration */
    UINT32 PreferRate;          /**< Movie perfer rate */
    UINT32 Reserv4[2];          /**< Reserve */
    UINT32 Matrix[9];           /**< Matrix - Rotation parameter */
    UINT32 Width;               /**< Movie width */
    UINT32 Height;              /**< Movie high */
    UINT32 Rotation;            /**< Rotation */
    UINT32 PreviewTime;         /**< MP4 - PreDefined[0] */
    UINT32 PreviewDuration;     /**< MP4 - PreDefined[1] */
    UINT32 PosterTime;          /**< MP4 - PreDefined[2] */
    UINT32 SelectionTime;       /**< MP4 - PreDefined[3] */
    UINT32 SelectionDuration;   /**< MP4 - PreDefined[4] */
    UINT32 CurrentTime;         /**< MP4 - PreDefined[5] */
    UINT32 NextTrackID;         /**< Next track id, indicate the next track id */
    UINT16 Volume;              /**< Audio volume, if have audio track */
    UINT16 Reserv2;             /**< Reserve */
} ISO_MVHD_BOX_s;

/**
 *  Movie Box ('moov')
 */
typedef struct {
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    ISO_MVHD_BOX_s MvhdAtom;    /**< ISO_MVHD_BOX_s */
    ISO_MVEX_BOX_s MvexAtom;    /**< ISO_MVEX_BOX_s */
    ISO_UDTA_BOX_s UdtaAtom;    /**< ISO_UDTA_BOX_s */
    ISO_TRACK_BOX_s TrackAtom[SVC_FORMAT_MAX_TRACK_PER_MEDIA];  /**< ISO_TRACK_BOX_s */
    UINT8 TrackCount;   /**< Track count of the moov box */
    UINT64 CinsertOff;  /**< CinsertOff */
} ISO_MOOV_BOX_s;

/**
 *  ISO Ftyp box
 */
typedef struct {
    ISO_BOX_s Box;              /**< ISO_BOX_s */
    UINT32 MediaId;             /**< Media Type*/
    UINT32 MajorBrand;          /**< Major brand */
    UINT32 MinorVersion;        /**< Minor brand */
    UINT32 CompatibleBrands[4]; /**< Compatible brand */
} ISO_FTYP_s;

#endif /* __ISO_BOX_DEF__H__ */

