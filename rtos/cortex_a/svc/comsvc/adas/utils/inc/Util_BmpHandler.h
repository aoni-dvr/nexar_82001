#ifndef UTIL_BMP_HANDLER_H
#define UTIL_BMP_HANDLER_H

#include "AmbaTypes.h"
#include "AmbaErrorCode.h"

#define UTIL_BMP_HDLR_ERR_NONE    ERR_NONE
#define UTIL_BMP_HDLR_ERR_ARG     ERR_ARG
#define UTIL_BMP_HDLR_ERR_IMPL    ERR_IMPL

typedef struct {
    UINT16 identifier;        // 0x0000 ("BM")
    UINT32 filesize;          // 0x0002 (bmp data + header data size)
    UINT32 reserved;          // 0x0006
    UINT32 bitmap_dataoffset; // 0x000A (the start position of bmp data)
    UINT32 bitmap_headersize; // 0x000E
    UINT32 width;             // 0x0012
    INT32 height;             // 0x0016 (if value is negative, data is from top to down)
    UINT16 planes;            // 0x001A (always = 1)
    UINT16 bits_perpixel;     // 0x001C (24: 24 bit per pixel)
    UINT32 compression;       // 0x001E (0: no compression)
    UINT32 bitmap_datasize;   // 0x0022
    UINT32 hresolution;       // 0x0026
    UINT32 vresolution;       // 0x002A
    UINT32 usedcolors;        // 0x002E
    UINT32 importantcolors;   // 0x0032
    //UINT32 palette;           // 0x0036 (no use in here)
} UTIL_BMP_HDLR_LBHEADER_s;

UINT32 UtilBmpHdlr_CreatBmpHeader(INT32 width, INT32 height, INT32 depth, UTIL_BMP_HDLR_LBHEADER_s *hbmp);
UINT32 UtilBmpHdlr_ReadBmpHeader(const char *fileName, UTIL_BMP_HDLR_LBHEADER_s *hbmp);
UINT32 UtilBmpHdlr_ReadBmpImg(const char *fileName, UTIL_BMP_HDLR_LBHEADER_s *hbmp, UINT8 *buffer);
UINT32 UtilBmpHdlr_WriteBmpImg(const char *fileName, UTIL_BMP_HDLR_LBHEADER_s *hbmp, UINT8 *buffer);
UINT32 UtilBmpHdlr_Bmp2Rgb(UINT8 *bmpBuf, INT32 width, INT32 height, UINT8 *rBuf, UINT8 *gBuf, UINT8 *bBuf);
UINT32 UtilBmpHdlr_Rgb2Bmp(UINT8 *rBuf, UINT8 *gBuf, UINT8 *bBuf, INT32 width, INT32 height, UINT8 *bmpBuf);
UINT32 UtilBmpHdlr_WriteRgb2BmpImg(const char *fileName, UINT8 *srcR, UINT8 *srcG, UINT8 *srcB, INT32 width, INT32 height);

#endif //UTIL_BMP_HANDLER_H
