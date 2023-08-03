/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BDOOR_IO_H_
#define BDOOR_IO_H_

#ifndef MISRA_COMPLIANCE

#include <dsp_types.h>
#include <cvapi_idsp_interface.h>

/*! @file bdoor_io.h

@brief Prototypes for backdoor simulator functions that perform file I/O.

These functions have no effect except on amalgam, but on amalgam, they
can be used to load and store files during a run.
*/

/*!  @brief Load bytes from a file into DRAM with no interpretation or
  reformatting.

  @param filename File to load.
  @param dram_addr Address in DRAM to place bytes.
  @param file_offset Offset from the beginning of the file to start reading.
  @param size Number of bytes to transfer from the file (0 means the rest of the file)
*/
void
bdoor_loadbinary(const char* filename,
                 uint32_t dram_addr,
                 uint32_t file_offset,
                 uint32_t size);

/*! @brief Load bytes from a file into DRAM with no interpretation.  File name is computed.
 
This function is exactly like bdoor_loadbinary() except the file name is computed.  The file name should have one printf conversion from an integer (i.e., %d), which will be substitued from name_seq.  For instance,

@code
  bdoor_loadbinary_seq( "foo%d.y", 3, ... );
@endcode

is the same as:

@code
  bdoor_loadbinary( "foo3.y", ... );
@endcode

The advantage of this function is that string formatting is difficult
to do on the Orc, so this moves the formatting into the simulator.

  @param filename File to load.  Should contain %%d.
  @param name_seq Value to substitute for %%d in the filename.
  @param dram_addr Address in DRAM to place bytes.
  @param file_offset Offset from the beginning of the file to start reading.
  @param size Number of bytes to transfer from the file (0 means the rest of the file)
 
*/
void
bdoor_loadbinary_seq(const char* filename,
                     int32_t name_seq,
                     uint32_t dram_addr,
                     uint32_t file_offset,
                     uint32_t size);

/*!
  @brief Load a 2-D image from a file into DRAM, padding the rows as necessary.

 This function reads a 2-D image from a file and copies it into DRAM.
 Rows must be padded in DRAM, and this function will add the
 appropriate amount of padding.

 The file is assumed to have a 4-byte header at the beginning: 2 bytes
 that specify the width of a row (in bytes) and 2 bytes that specify
 the height of the frame.  This information is used to determine how
 the rows need to be padded.

 @param filename Name of the file to load.
 @param dram_addr Address in DRAM to place bytes
 @param file_offset Start reading at this point in the file.
 @param pitch Pitch to load rows in DRAM.  If 0, it will be computed by rounding the width up as necessary.

*/

void
bdoor_loadimage(const char* filename,
                uint32_t dram_addr,
                uint32_t file_offset,
                uint32_t pitch);


/*!
  @brief Load a 2-D image from a file into DRAM, padding the rows as necessary.

 This function reads a 2-D image from a file and copies it into DRAM.
 Rows must be padded in DRAM, and this function will add the
 appropriate amount of padding.

 This is just like bdoor_loadimage(), except that this functions does not assume that the file contains a 4-byte header describing the width and height of the image.  Those must be supplied as parameters.
 
 @param filename Name of the file to load.
 @param dram_addr Address in DRAM to place bytes
 @param file_offset Start reading at this point in the file.
 @param width Width of the image in bytes.
 @param height Height of the image.
 @param pitch Pitch to load rows in DRAM.  If 0, it will be computed by rounding the width up as necessary.
*/
void
bdoor_loadimage_no_header(const char* filename,
                          uint32_t dram_addr,
                          uint32_t file_offset,
                          uint32_t width,
                          uint32_t height,
                          uint32_t pitch);

/*!
  @brief Load a 2-D image from a file into DRAM, padding the rows as necessary, with sequence # support

 This function reads a 2-D image from a file and copies it into DRAM.
 Rows must be padded in DRAM, and this function will add the
 appropriate amount of padding.

 This is just like bdoor_loadimage(), except that this functions does not assume that the file contains a 4-byte header describing the width and height of the image.  Those must be supplied as parameters.
 
 @param filename Name of the file to load.
 @param name_seq Value to substitute for %%d in filename.
 @param dram_addr Address in DRAM to place bytes
 @param file_offset Start reading at this point in the file.
 @param width Width of the image in bytes.
 @param height Height of the image.
 @param pitch Pitch to load rows in DRAM.  If 0, it will be computed by rounding the width up as necessary.
*/
void
bdoor_loadimage_seq_no_header(const char* filename,
                              uint32_t name_seq,
                              uint32_t dram_addr,
                              uint32_t file_offset,
                              uint32_t width,
                              uint32_t height,
                              uint32_t pitch);

/*! @brief Component code for LUMA */
#define COMP_Y             0
/*! @brief Component code for CHROMA */
#define COMP_UV            1
/*! @brief Component code for ME1 */
#define COMP_ME1           2

/*! @brief Pictype code for FRAME */
#define PICTYPE_FRAME      0
/*! @brief Pictype code for FIELD:TopFieldFirst */
#define PICTYPE_FIELD_TFF  1
/*! @brief Pictype code for FIELD:BottomFieldFirst */
#define PICTYPE_FIELD_BFF  2
/*! @brief Pictype code for FRAME_INTERLACED (interlaced pictures in frame format) */
#define PICTYPE_FRAME_INTL 3

/*! @brief Subsample code for 4:4:4 */
#define COLOR_4_4_4        0
/*! @brief Subsample code for 4:2:2 */
#define COLOR_4_2_2        1
/*! @brief Subsample code for 4:1:1 */
#define COLOR_4_1_1        2
/*! @brief Subsample code for 4:2:0 */
#define COLOR_4_2_0        3
/*! @brief Subsample code for LUMA only */
#define COLOR_Y_ONLY       4
/*! @brief Subsample code for ME1 only */
#define COLOR_ME1_ONLY     5

/*!
@brief Loads one component of one picture of an APF file into DRAM.

This function will load a single component (Y, UV, or ME1) of a
single picture from a file into DRAM.  The bit-width of a pixel can
be adjusted, if necessary.

@param filename Name of APF file to use
@param dram_addr Address in DRAM to write the image
@param component_code Component to load: COMP_Y, COMP_UV, or COMP_ME1
@param picture Index of picture in the APF file to load: 0 for the first picture, 1 for the second, etc.
@param bits Number of bits per pixel.  If 0, the pixel size from the file is used.
@param pitch Pitch in DRAM.  If 0, it is computed automatically from the width.
*/
void
bdoor_loadapf_component(const char *filename,
                        uint32_t dram_addr,
                        int component_code,
                        uint32_t picture,
                        uint32_t bits,
                        uint32_t pitch);

/*!
@brief Store bytes from DRAM into a new file.

@param filename File to create.
@param dram_addr Address in DRAM to begin reading from.
@param length Number of bytes to read from DRAM and write to the file.
*/
void
bdoor_storebinary(const char* filename,
                  uint32_t dram_addr,
                  uint32_t length);

/*!
  @brief Store bytes from DRAM into a new file.  File name is computed.

  This is just like bdoor_storebinary except that the filename is computed from name_seq using a %d conversion.  See \link bdoor_loadbinary_seq() \endlink.
  
  @param filename File to create.  Should contain %%d.
  @param name_seq Value to substitute for %%d in filename.
  @param dram_addr Address in DRAM to begin reading from.
  @param length Number of bytes to read from DRAM and write to the file.
*/
void
bdoor_storebinary_seq(const char* filename,
                      int32_t name_seq,
                      uint32_t dram_addr,
                      uint32_t length);

/*!
  @brief Store a 2-D image from DRAM into a file.  Padding from DRAM is removed.

  The file created will have a 4-byte header: 2 bytes of width and 2 bytes of height.  Padding from DRAM is removed when the file is written.

  @param filename File to write.
  @param dram_addr DRAM address where image begins
  @param width Width of the image in bytes (not including padding).
  @param height Height of the image.
  @param pitch Distance between consecutive rows in DRAM (in bytes).  If 0, it is computed based on the width.
*/
void
bdoor_storeimage(const char* filename,
                 uint32_t dram_addr,
                 uint32_t width,
                 uint32_t height,
                 uint32_t pitch);

/*!
  @brief Store a 2-D image from DRAM into a file.  Padding from DRAM is removed.

  The file created will have a 4-byte header: 2 bytes of width and 2 bytes of height.  Padding from DRAM is removed when the file is written.

  @param filename File to write.  Should contain %%d.
  @param name_seq Value to substitute for %%d in filename.
  @param dram_addr DRAM address where image begins
  @param width Width of the image in bytes (not including padding).
  @param height Height of the image.
  @param pitch Distance between consecutive rows in DRAM (in bytes).  If 0, it is computed based on the width.
*/
void
bdoor_storeimage_seq(const char* filename,
                     uint32_t name_seq,
                     uint32_t dram_addr,
                     uint32_t width,
                     uint32_t height,
                     uint32_t pitch);


/*!
  @brief Store a 2-D image from DRAM into a file.  Padding from DRAM is removed.

  This is identical to \link bdoor_storeimage() \endlink except that
  no header is created in the file.
  
  @param filename File to write.
  @param dram_addr DRAM address where image begins
  @param width Width of the image in bytes (not including padding).
  @param height Height of the image.
  @param pitch Distance between consecutive rows in DRAM (in bytes).  If 0, it is computed based on the width.
*/
void
bdoor_storeimage_no_header(const char* filename,
                           uint32_t dram_addr,
                           uint32_t width,
                           uint32_t height,
                           uint32_t pitch);


/*!
  @brief Store a 2-D image from DRAM into a file.  Padding from DRAM is removed.

  This is identical to \link bdoor_storeimage() \endlink except that
  no header is created in the file.
  
  @param filename File to write.  Should contain %%d.
  @param name_seq Value to substitute for %%d in filename.
  @param dram_addr DRAM address where image begins
  @param width Width of the image in bytes (not including padding).
  @param height Height of the image.
  @param pitch Distance between consecutive rows in DRAM (in bytes).  If 0, it is computed based on the width.
*/
void
bdoor_storeimage_seq_no_header(const char* filename,
                               uint32_t name_seq,
                               uint32_t dram_addr,
                               uint32_t width,
                               uint32_t height,
                               uint32_t pitch);


/*!
  @brief Store a component of a picture to an APF file.

  This is the general form of storing a component to an APF file.
  Unfortunately, the general for takes a lot of arguments, which are
  expensive to pass since teh stack must be used.  There are many
  other "bdoor_storeapf" functions that hardcode values for some of
  the parameters, and those other functions are cheaper to call.

  @param filename Name of file to create, may have a %%d conversion.
  @param name_seq Value to substitute for %%d in filename
  @param dram_addr Address in DRAM of component.
  @param width Width of the image in bytes (the full image, not necessarily the same as the component!)
  @param height Height of the image (again, the full image).
  @param pitch Distance between consecutive rows in DRAM (in bytes).  If 0, it is computed based on the width.
  @param component_code Name of component to write: COMP_Y, COMP_UV, or COMP_ME1.
  @param subsample_code Subsampling scheme for this picture, for instance COLOR_4_2_0.
  @param bits Number of bits in a pixel.
  @param pictype_code Picture format, see PICTYPE_FRAME, PICTYPE_FIELD_TFF, etc.

*/
void
bdoor_storeapf(const char* filename,
               int name_seq,
               uint32_t dram_addr,
               uint32_t width,
               uint32_t height,
               uint32_t pitch,
               int component_code,
               int subsample_code,
               int bits,
               int pictype_code
               );


/*! @brief bdoor_storeapf() with component=Y, bits=8, subsample=4:2:0 */
void
bdoor_storeapf_y_8_420(const char* filename,
                       int name_seq,
                       uint32_t dram_addr,
                       uint32_t width,
                       uint32_t height,
                       uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=10, subsample=4:2:0 */
void
bdoor_storeapf_y_10_420(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=16, subsample=4:2:0 */
void
bdoor_storeapf_y_16_420(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=8, subsample=4:2:0 */
void
bdoor_storeapf_uv_8_420(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=10, subsample=4:2:0 */
void
bdoor_storeapf_uv_10_420(const char* filename,
                         int name_seq,
                         uint32_t dram_addr,
                         uint32_t width,
                         uint32_t height,
                         uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=16, subsample=4:2:0 */
void
bdoor_storeapf_uv_16_420(const char* filename,
                         int name_seq,
                         uint32_t dram_addr,
                         uint32_t width,
                         uint32_t height,
                         uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=8, subsample=4:2:2 */
void
bdoor_storeapf_y_8_422(const char* filename,
                       int name_seq,
                       uint32_t dram_addr,
                       uint32_t width,
                       uint32_t height,
                       uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=10, subsample=4:2:2 */
void
bdoor_storeapf_y_10_422(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=16, subsample=4:2:2 */
void
bdoor_storeapf_y_16_422(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=8, subsample=4:2:2 */
void
bdoor_storeapf_uv_8_422(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=10, subsample=4:2:2 */
void
bdoor_storeapf_uv_10_422(const char* filename,
                         int name_seq,
                         uint32_t dram_addr,
                         uint32_t width,
                         uint32_t height,
                         uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=16, subsample=4:2:2 */
void
bdoor_storeapf_uv_16_422(const char* filename,
                         int name_seq,
                         uint32_t dram_addr,
                         uint32_t width,
                         uint32_t height,
                         uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=8, subsample=4:4:4 */
void
bdoor_storeapf_y_8_444(const char* filename,
                       int name_seq,
                       uint32_t dram_addr,
                       uint32_t width,
                       uint32_t height,
                       uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=10, subsample=4:4:4 */
void
bdoor_storeapf_y_10_444(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=Y, bits=16, subsample=4:4:4 */
void
bdoor_storeapf_y_16_444(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=8, subsample=4:4:4 */
void
bdoor_storeapf_uv_8_444(const char* filename,
                        int name_seq,
                        uint32_t dram_addr,
                        uint32_t width,
                        uint32_t height,
                        uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=10, subsample=4:4:4 */
void
bdoor_storeapf_uv_10_444(const char* filename,
                         int name_seq,
                         uint32_t dram_addr,
                         uint32_t width,
                         uint32_t height,
                         uint32_t pitch);

/*! @brief bdoor_storeapf() with component=UV, bits=16, subsample=4:4:4 */
void
bdoor_storeapf_uv_16_444(const char* filename,
                         int name_seq,
                         uint32_t dram_addr,
                         uint32_t width,
                         uint32_t height,
                         uint32_t pitch);


void
bdoor_me_dumpfile(const char * filename, uint32_t tid, uint32_t frame );

void
bdoor_stack_limit( uint32_t limit );

struct timeval
{
	unsigned long long tv_sec;
	unsigned long tv_usec;
};

void bdoor_gettimeofday(struct timeval* tv);

int 
bdoor_get_file_size( const char* filename );

int 
bdoor_get_file_size_seq( const char* filename, int seq_num );

#if ((defined(CHIP_CV2A)) || (defined(CHIP_CV22A)) || (defined(CHIP_CV5)) || (defined(CHIP_CV6)))
#define BDOOR_MAX_HALF_OCTAVES  7
#else /* !CHIP_CV2A, !CHIP_CV22A, !CHIP_CV5, !CHIP_CV6 */
#define BDOOR_MAX_HALF_OCTAVES  6
#endif /* ?CHIP_CV2A, ?CHIP_CV22A */

typedef struct bdoor_cv_pic_info_addresses_s
{
  uint32_t  source_left_luma_daddr;
  uint32_t  source_right_luma_daddr;
  uint32_t  source_left_chroma_daddr;
  uint32_t  source_right_chroma_daddr;
} bdoor_cv_pic_info_addresses_t;

typedef struct /* bdoor_idsp_ld_info_s */
{
    uint16_t    ld_img_height_m1;
    uint16_t    ld_img_width_m1;
    int16_t     ld_roi_start_row;
    int16_t     ld_roi_start_col;
    int16_t     ld_roi_end_row;
    int16_t     ld_roi_end_col;
    uint16_t    ld_roi_height_m1;
    uint16_t    ld_roi_width_m1;
} bdoor_idsp_ld_info_t;

typedef struct /* bdoor_half_octave_ctrl_s */
{
    uint8_t     disable;           /* Scale disable flag, set to 1 to disable this */
    uint8_t     mode              :1; /* 0: mono, 1: stereo */
    uint8_t     octave_mode       :1; /* 0: 1/sqrt2, 1: half (CHIP_CV2) */
    uint8_t     bit_depth         :2; /* 00: 8-bit, 01: (reserved), 10: 12-bit, 11: reserved */
    uint8_t     mode_unused_flags :4; /* available_for_use */
    uint16_t    roi_pitch;            /* Pitch for the ROI */
} bdoor_half_octave_ctrl_t;

typedef struct /* bdoor_half_octave_info_s */
{
    bdoor_half_octave_ctrl_t  ctrl;
    int16_t             roi_start_row;
    int16_t             roi_start_col;
    uint16_t            roi_height_m1;
    uint16_t            roi_width_m1;
} bdoor_half_octave_info_t;

typedef struct
{
    uint16_t    image_height_m1;    /* in pixels  */
    uint16_t    image_width_m1;     /* in pixels  */
    uint32_t    image_pitch_m1;     /* in bytes   */
    bdoor_half_octave_info_t half_octave[BDOOR_MAX_HALF_OCTAVES];
} bdoor_idsp_pyramid_t;

typedef struct bdoor_cv_pic_info_s
{
  uint32_t frame_num;
  uint32_t token_id;
  bdoor_idsp_pyramid_t pyramid;

  /* DRAM locations of special lane-detection image */
  bdoor_idsp_ld_info_t lane_detect_info;
  uint32_t lane_detect_luma_daddr;
  uint32_t lane_detect_chroma_daddr;

  /* DRAM locations of left/right pyramids */
  uint32_t left_luma_daddr[BDOOR_MAX_HALF_OCTAVES];
  uint32_t right_luma_daddr[BDOOR_MAX_HALF_OCTAVES];
  uint32_t left_chroma_daddr[BDOOR_MAX_HALF_OCTAVES];
  uint32_t right_chroma_daddr[BDOOR_MAX_HALF_OCTAVES];

  void    *vpSidebandInfo;
} bdoor_cv_pic_info_t;


void
bdoor_run_idsp_section(uint32_t section, bdoor_cv_pic_info_addresses_t *pSrcAddr, cv_pic_info_t *pPicInfo );

uint32_t bdoor_get_dram_size(void);

#endif /* ?MISRA_COMPLIANCE */

#endif
