#include <stdlib.h>
#include <stdio.h>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#define SAVEFILE (0)

using namespace cv;

/***
 * Note:
 *  - DO NOT use the CV_AA in LineType since opencv will change color for smoothness.
 *  - For multi line string, you have to draw it line by line manually.
 ***/

extern "C" unsigned int OSD_Init(int **Hndlr, int Width, int Height);
extern "C" unsigned int OSD_DrawLine(int *Hndlr, int X_ST, int Y_ST, int X_END, int Y_END, int Thickness, int Color);
extern "C" unsigned int OSD_DrawRect(int *Hndlr, int X_TL, int Y_TL, int X_BR, int Y_BR, int Thickness, int Color);
extern "C" unsigned int OSD_DrawCircle(int *Hndlr, int X_C, int Y_C, int Radius, int Thickness, int Color);
extern "C" unsigned int OSD_DrawString(int *Hndlr, int X_BL, int Y_BL, char *Text, int Thickness, int Color);
extern "C" unsigned int OSD_GetBuf(int *Hndlr, unsigned char **Buf, unsigned int *Size);
extern "C" unsigned int OSD_Release(int *Hndlr);

typedef struct {
    unsigned char *Buffer;
    int Width;
    int Height;
} OSDInfo_s;

/* Caller have to take care error case. (Should not do rest call if init returns fail.) */
unsigned int OSD_Init(int **Hndlr, int Width, int Height)
{
    unsigned int Ret = 0;
    OSDInfo_s *OSDInfo = NULL;

    OSDInfo = (OSDInfo_s *)malloc(sizeof(OSDInfo_s));
    if (OSDInfo == NULL) {
        Ret = 0xffffffff;
    } else {
        OSDInfo->Buffer = (unsigned char *)malloc(Width*Height);
        if (OSDInfo->Buffer != NULL) {
            memset(OSDInfo->Buffer, 0, Width*Height);
            OSDInfo->Width = Width;
            OSDInfo->Height = Height;
        } else {
            Ret = 0xffffffff;
        }
    }

    *Hndlr = (int *)OSDInfo;

    return Ret;
}

unsigned int OSD_DrawLine(int *Hndlr, int X_ST, int Y_ST, int X_END, int Y_END, int Thickness, int Color)
{
    Point Start, End;
    int Shift;
    int LineType;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, CV_8UC1, OSDInfo->Buffer);

    Start = Point(X_ST,Y_ST);
    End = Point(X_END,Y_END);
    Color_s = Scalar(Color,0,0);
    Shift = 0;
    LineType = 8;
    line(Image1, Start, End, Color_s, Thickness, LineType, Shift);

    return 0;
}

unsigned int OSD_DrawRect(int *Hndlr, int X_TL, int Y_TL, int X_BR, int Y_BR, int Thickness, int Color)
{
    Point TopLeft, ButtonRight;
    int LineType;
    int Shift;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, CV_8UC1, OSDInfo->Buffer);

    TopLeft = Point(X_TL,Y_TL);
    ButtonRight = Point(X_BR,Y_BR);
    Color_s = Scalar(Color,0,0);
    LineType = 8; //cannot use CV_AA since it will change the color
    Shift = 0;
    rectangle(Image1,TopLeft,ButtonRight,Color_s,Thickness,LineType,Shift);

    return 0;
}

unsigned int OSD_DrawCircle(int *Hndlr, int X_C, int Y_C, int Radius, int Thickness, int Color)
{
    Point Center;
    int Shift;
    int LineType;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, CV_8UC1, OSDInfo->Buffer);

    Center = Point(X_C,Y_C);
    Color_s = Scalar(Color,0,0);
    Shift = 0;
    LineType = 8;

    circle(Image1, Center, Radius, Color_s, Thickness, LineType, Shift);

    return 0;
}

unsigned int OSD_DrawString(int *Hndlr, int X_BL, int Y_BL, char *Text, int Thickness, int Color)
{
    double FontScale;
    int LineType;
    int FontFace;
    Point ButtonLeft;
    bool ImgOrg;
    Scalar Color_s;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;
    Mat Image1(OSDInfo->Height, OSDInfo->Width, CV_8UC1, OSDInfo->Buffer);

    FontScale = 1.0;
    FontFace = 0; //CV_FONT_HERSHEY_SIMPLEX
    LineType = 8;
    ImgOrg = false;

    ButtonLeft = Point(X_BL,Y_BL);
    Color_s=Scalar(Color,0,0);
    putText(Image1, Text, ButtonLeft, FontFace, FontScale, Color_s, Thickness, LineType, ImgOrg);

    return 0;
}

unsigned int OSD_GetBuf(int *Hndlr, unsigned char **Buf, unsigned int *Size)
{
    int MatSize;
    OSDInfo_s *OSDInfo = (OSDInfo_s *)Hndlr;

    if (SAVEFILE) {
        Mat Image1(OSDInfo->Height, OSDInfo->Width, CV_8UC1, OSDInfo->Buffer);

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

    free(OSDInfo->Buffer);
    free(OSDInfo);

    return 0;
}

