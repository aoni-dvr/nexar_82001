#include <stdlib.h>
#include <stdio.h>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "preference.h"

#define SAVEFILE (1)
#define NUM_COLOR_TBL     (8U)


using namespace cv;

/***
 * Note:
 *  - DO NOT use the CV_AA in LineType since opencv will change color for smoothness.
 *  - For multi line string, you have to draw it line by line manually.
 ***/

extern "C" unsigned int OSD_Init(int Channel, int **Hndlr, int Width, int Height, unsigned char *OSDBuffer);
extern "C" unsigned int OSD_DrawLine(int *Hndlr, int X_ST, int Y_ST, int X_END, int Y_END, int Thickness, int ColorIdx);
extern "C" unsigned int OSD_DrawRect(int *Hndlr, int X_TL, int Y_TL, int X_BR, int Y_BR, int Thickness, int ColorIdx);
extern "C" unsigned int OSD_DrawCircle(int *Hndlr, int X_C, int Y_C, int Radius, int Thickness, int ColorIdx);
extern "C" unsigned int OSD_DrawString(int *Hndlr, int X_BL, int Y_BL, char *Text, int Thickness, int ColorIdx);
extern "C" unsigned int OSD_GetBuf(int *Hndlr, unsigned char **Buf, unsigned int *Size);
extern "C" unsigned int OSD_Release(int *Hndlr);


const int color_tbl_8[NUM_COLOR_TBL] = {
    0x1U, //index
    0x2U,
    0x3U,
    0x4U,
    0x5U,
    0x6U,
    0x7U,
    0xffU,
};

const unsigned int color_tbl_32[NUM_COLOR_TBL] = {
    0x80800000U, //0xAARRGGBB
    0x80008000U,
    0x80808000U,
    0x80000080U,
    0x80800080U,
    0x80008080U,
    0x80c0c0c0U,
    0xffffffffU,
};

typedef struct {
    unsigned char *Buffer;
    int Width;
    int Height;
    int BufferMode;
    int OsdType;
    int OpenCVType;
} OSDInfo_s;

static Scalar GetColor(int OsdType, int ColorIdx)
{
    if (ColorIdx < NUM_COLOR_TBL) {
        if (OsdType == OSD_TYPE_8_BIT) {
            int Color8 = color_tbl_8[ColorIdx];
            return Scalar(Color8,0,0);
        } else if (OsdType == OSD_TYPE_32_BIT) {
            unsigned char R,G,B,A;
            unsigned int Color32 = color_tbl_32[ColorIdx];
            B = (Color32 & 0xff);
            G = ((Color32 >> 8) & 0xff);
            R = ((Color32 >> 16) & 0xff);
            A = ((Color32 >> 24) & 0xff);
            return Scalar(B,G,R,A);
        } else {
            printf("Invalid osd type : %d-bit!!\n", OsdType);
            return Scalar(0);
        }
    } else {
        printf("Invalid ColorIdx : %d!!\n", ColorIdx);
        return Scalar(0);
    }
}

/* Caller have to take care error case. (Should not do rest call if init returns fail.) */
unsigned int OSD_Init(int Channel, int **Hndlr, int Width, int Height, unsigned char *OSDBuffer)
{
    unsigned int Ret = 0;
    OSDInfo_s *OSDInfo = NULL;

    OSDInfo = (OSDInfo_s *)malloc(sizeof(OSDInfo_s));
    if (OSDInfo == NULL) {
        Ret = 0xffffffff;
    } else {
        int OsdBufSize;
        OSD_INFO_s OsdInfo;
        Preference_GetOSDInfo(Channel, &OsdInfo);

        if (OsdInfo.OsdType == OSD_TYPE_8_BIT) {
            OSDInfo->OpenCVType = CV_8UC1;
            OsdBufSize = (Width*Height);
        } else if (OsdInfo.OsdType == OSD_TYPE_32_BIT) {
            OSDInfo->OpenCVType = CV_8UC4;
            OsdBufSize = (Width*Height*4);
        } else {
            OsdBufSize = 0;
        }

        if (OSDBuffer == NULL) {
            OSDInfo->Buffer = (unsigned char *)malloc(OsdBufSize);
            OSDInfo->BufferMode = 0;
        } else {
            OSDInfo->Buffer = OSDBuffer;
            OSDInfo->BufferMode = 1;
        }

        if (OSDInfo->Buffer != NULL) {
            memset(OSDInfo->Buffer, 0, OsdBufSize);
            OSDInfo->Width = Width;
            OSDInfo->Height = Height;
            OSDInfo->OsdType = OsdInfo.OsdType;

            //The 1st creation will take more time
            //Mat Img(Height, Width, OSDInfo->OsdType, OSDInfo->Buffer);
        } else {
            Ret = 0xffffffff;
        }
    }

    *Hndlr = (int *)OSDInfo;

    return Ret;
}

unsigned int OSD_DrawLine(int *Hndlr, int X_ST, int Y_ST, int X_END, int Y_END, int Thickness, int ColorIdx)
{
    Point Start, End;
    int Shift;
    int LineType;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, OSDInfo->OpenCVType, OSDInfo->Buffer);

    Start = Point(X_ST,Y_ST);
    End = Point(X_END,Y_END);
    Color_s = GetColor(OSDInfo->OsdType, ColorIdx);
    Shift = 0;
    LineType = 8;
    line(Image1, Start, End, Color_s, Thickness, LineType, Shift);

    return 0;
}

unsigned int OSD_DrawRect(int *Hndlr, int X_TL, int Y_TL, int X_BR, int Y_BR, int Thickness, int ColorIdx)
{
    Point TopLeft, ButtonRight;
    int LineType;
    int Shift;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, OSDInfo->OpenCVType, OSDInfo->Buffer);

    TopLeft = Point(X_TL,Y_TL);
    ButtonRight = Point(X_BR,Y_BR);
    Color_s = GetColor(OSDInfo->OsdType, ColorIdx);
    LineType = 8; //cannot use CV_AA since it will change the Color
    Shift = 0;
    rectangle(Image1,TopLeft,ButtonRight,Color_s,Thickness,LineType,Shift);

    return 0;
}

unsigned int OSD_DrawCircle(int *Hndlr, int X_C, int Y_C, int Radius, int Thickness, int ColorIdx)
{
    Point Center;
    int Shift;
    int LineType;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, OSDInfo->OpenCVType, OSDInfo->Buffer);

    Center = Point(X_C,Y_C);
    Color_s = GetColor(OSDInfo->OsdType, ColorIdx);
    Shift = 0;
    LineType = 8;

    circle(Image1, Center, Radius, Color_s, Thickness, LineType, Shift);

    return 0;
}

unsigned int OSD_DrawString(int *Hndlr, int X_BL, int Y_BL, char *Text, int Thickness, int ColorIdx)
{
    double FontScale;
    int LineType;
    int FontFace;
    Point ButtonLeft;
    bool ImgOrg;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, OSDInfo->OpenCVType, OSDInfo->Buffer);

    FontScale = 1.0;
    FontFace = 0; //CV_FONT_HERSHEY_SIMPLEX
    LineType = 8;
    ImgOrg = false;

    ButtonLeft = Point(X_BL,Y_BL);
    Color_s = GetColor(OSDInfo->OsdType, ColorIdx);
    putText(Image1, Text, ButtonLeft, FontFace, FontScale, Color_s, Thickness, LineType, ImgOrg);

    return 0;
}

unsigned int OSD_GetBuf(int *Hndlr, unsigned  char **Buf, unsigned int *Size)
{
    int MatSize;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;

    if (SAVEFILE) {
        Mat Image1(OSDInfo->Height, OSDInfo->Width, OSDInfo->OpenCVType, OSDInfo->Buffer);

        /* Save into PNG */
        imwrite("/tmp/SD0/foo.png",Image1);
    }

    /* Get OSD buffer */
    //MatSize = Img.cols * Img.rows * Img.elemSize();
    //printf("MatType=%u, MatSize = %u (%u x %u x %u)\n", Img.type(), MatSize, Img.cols, Img.rows, Img.elemSize());
    MatSize = OSDInfo->Height * OSDInfo->Width;


    //*Buf = (unsigned char *)Img.data;
    *Buf = OSDInfo->Buffer;
    *Size = MatSize;

    return 0;
}

unsigned int OSD_Release(int *Hndlr)
{
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;

    if (OSDInfo->BufferMode == 0) {
        free(OSDInfo->Buffer);
    }

    free(OSDInfo);

    return 0;
}

