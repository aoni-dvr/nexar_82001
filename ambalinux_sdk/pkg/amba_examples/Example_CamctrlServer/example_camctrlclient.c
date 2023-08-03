#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "CamCtrlMsgDef.h"
#include "CamCtrlOsdCmdDef.h"

#define SERVER_PORT 11009
#define NG (-1)
#define OK (0)

#define PrintFunc printf

int main(int Argc, char *Argv[])
{
    int sockfd;
    struct sockaddr_in dest;
    int ShowHelp = 1;
    CV_Request_s Request;
    char buf[10] = {0};

    bzero(&Request, sizeof(Request));
    if (Argc >= 2) {
        if (strcmp(Argv[1], "enable") == OK) {
            CV_OSD_Enable_s *pCmd = (CV_OSD_Enable_s *) &Request;

            Request.Msg = CAMCTRL_DISP_ENABLE;
            Request.ParamSize = 2 * sizeof(int);
            if (Argc > 3) {
                pCmd->Channel = atoi(Argv[2]);
                pCmd->Enable = atoi(Argv[3]);
            } else {
                pCmd->Channel = 0;
                pCmd->Enable = 1;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "wrt_pix") == OK) {
            CV_OSD_DrawPixel_s *Pixel = (CV_OSD_DrawPixel_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAW_PIXEL;
            Request.ParamSize = 4 * sizeof(int);
            if (Argc > 5) {
                Pixel->Channel = atoi(Argv[2]);
                Pixel->X = atoi(Argv[3]);
                Pixel->Y = atoi(Argv[4]);
                Pixel->Color = strtoul(Argv[5], NULL, 0);
            } else {
                Pixel->Channel = 0;
                Pixel->X = 1;
                Pixel->Y = 1;
                Pixel->Color = 0x80FF0001;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "line") == OK) {
            CV_OSD_DrawLine_s *Line = (CV_OSD_DrawLine_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAW_LINE;
            Request.ParamSize = 7 * sizeof(int);
            if (Argc > 8) {
                Line->Channel = atoi(Argv[2]);
                Line->X1 = atoi(Argv[3]);
                Line->Y1 = atoi(Argv[4]);
                Line->X2 = atoi(Argv[5]);
                Line->Y2 = atoi(Argv[6]);
                Line->Color = strtoul(Argv[7], NULL, 0);
                Line->Thickness = atoi(Argv[8]);
            } else {
                Line->Channel = 0;
                Line->X1 = 50;
                Line->Y1 = 50;
                Line->X2 = 100;
                Line->Y2 = 50;
                Line->Color = 0x80FF0001;
                Line->Thickness = 3;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "rect") == OK) {
            CV_OSD_DrawRect_s *Rect = (CV_OSD_DrawRect_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAW_RECT;
            Request.ParamSize = 7 * sizeof(int);
            if (Argc > 8) {
                Rect->Channel = atoi(Argv[2]);
                Rect->X1 = atoi(Argv[3]);
                Rect->Y1 = atoi(Argv[4]);
                Rect->X2 = atoi(Argv[5]);
                Rect->Y2 = atoi(Argv[6]);
                Rect->Color = strtoul(Argv[7], NULL, 0);
                Rect->Thickness = atoi(Argv[8]);
            } else {
                Rect->Channel = 0;
                Rect->X1 = 50;
                Rect->Y1 = 50;
                Rect->X2 = 100;
                Rect->Y2 = 100;
                Rect->Color = 0x80FF0001;
                Rect->Thickness = 3;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "flush") == OK) {
            CV_OSD_Flush_s *pCmd = (CV_OSD_Flush_s *) &Request;

            Request.Msg = CAMCTRL_DISP_FLUSH;
            Request.ParamSize = sizeof(int);
            if (Argc > 2) {
                pCmd->Channel = atoi(Argv[2]);
            } else {
                pCmd->Channel = 0;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "clear") == OK) {
            CV_OSD_Clear_s *pCmd = (CV_OSD_Clear_s *) &Request;

            Request.Msg = CAMCTRL_DISP_CLEAR;
            Request.ParamSize = sizeof(int);
            if (Argc > 2) {
                pCmd->Channel = atoi(Argv[2]);
            } else {
                pCmd->Channel = 0;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "charconf") == OK) {
            CV_OSD_SetCharConfig_s *pCmd = (CV_OSD_SetCharConfig_s *) &Request;

            Request.Msg = CAMCTRL_DISP_SETCHARCONFIG;
            Request.ParamSize = 6 * sizeof(int);
            if (Argc > 7) {
                pCmd->Channel = atoi(Argv[2]);
                pCmd->AttributeData = atoi(Argv[3]);
                pCmd->AttributeBits = strtoul(Argv[4], NULL, 0);
                pCmd->FontFaceIdx = atoi(Argv[5]);
                pCmd->FontPixelWidth = atoi(Argv[6]);
                pCmd->FontPixelHeight = atoi(Argv[7]);
            } else {
                pCmd->Channel = 1;
                pCmd->AttributeData = 0;
                pCmd->AttributeBits = 0;
                pCmd->FontFaceIdx = 0;
                pCmd->FontPixelWidth = 12;
                pCmd->FontPixelHeight = 12;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "string") == OK) {
            CV_OSD_DrawString_s *pCmd = (CV_OSD_DrawString_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAWSTRING;
            Request.ParamSize = (4 * sizeof(int)) + (12 * sizeof(char));
            if (Argc > 6) {
                pCmd->Channel = atoi(Argv[2]);
                pCmd->X = atoi(Argv[3]);
                pCmd->Y = atoi(Argv[4]);
                pCmd->Color = strtoul(Argv[5], NULL, 0);
                strncpy(pCmd->String, Argv[6], 11);
            } else {
                pCmd->Channel = 1;
                pCmd->X = 10;
                pCmd->Y = 10;
                pCmd->Color = 0xffff0001;
                strncpy(pCmd->String, "Test123456", 11);
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[1], "multi_cmd") == OK) {
            // This is an example to execute multi commands with CAMCTRL_DISP_MULTI_CMD
            unsigned int *pInt = (unsigned int *)&Request.Param[0];
            OSD_Command_s *pCmd;

            Request.Msg = CAMCTRL_DISP_MULTI_CMD;
            //Request.ParamSize = sizeof(CV_OSD_CommandSequence_s);
            memset(Request.Param, 0 , sizeof(Request.Param));
            pInt[0] = 0;
            pCmd = (OSD_Command_s *)&pInt[1];

            /* OSD enable */
            pCmd->Msg = CAMCTRL_DISP_ENABLE;
            pCmd->Channel = 1;
            pCmd->X1 = 1; //enable
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 50;
            pCmd->Y1 = 100;
            pCmd->X2 = 100;
            pCmd->Y2 = 150;
            pCmd->Color = 0xffff0001;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 100;
            pCmd->Y1 = 300;
            pCmd->X2 = 50;
            pCmd->Y2 = 200;
            pCmd->Color = 0xff8f8f8f;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 150;
            pCmd->Y1 = 100;
            pCmd->X2 = 200;
            pCmd->Y2 = 200;
            pCmd->Color = 0xff00ff02;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 250;
            pCmd->Y1 = 250;
            pCmd->X2 = 150;
            pCmd->Y2 = 350;
            pCmd->Color = 0xff0000ff;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 350;
            pCmd->Y1 = 100;
            pCmd->X2 = 400;
            pCmd->Y2 = 150;
            pCmd->Color = 0xffff0001;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 400;
            pCmd->Y1 = 300;
            pCmd->X2 = 350;
            pCmd->Y2 = 200;
            pCmd->Color = 0xffff0001;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 450;
            pCmd->Y1 = 100;
            pCmd->X2 = 500;
            pCmd->Y2 = 200;
            pCmd->Color = 0xffff0001;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 550;
            pCmd->Y1 = 250;
            pCmd->X2 = 450;
            pCmd->Y2 = 350;
            pCmd->Color = 0xff0000ff;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 650;
            pCmd->Y1 = 100;
            pCmd->X2 = 700;
            pCmd->Y2 = 200;
            pCmd->Color = 0xff0000ff;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 750;
            pCmd->Y1 = 250;
            pCmd->X2 = 850;
            pCmd->Y2 = 350;
            pCmd->Color = 0xff0000ff;
            pCmd->Thickness = 3;
            pCmd++;
            pInt[0]++;

            /* Set Char config */
            pCmd->Msg = CAMCTRL_DISP_SETCHARCONFIG;
            pCmd->Channel = 1;
            pCmd->X1 = 0; //AttributeData
            pCmd->Y1 = 0; //AttributeBits
            pCmd->X2 = 0; //FontFaceIdx
            pCmd->Y2 = 24; //FontPixelWidth
            pCmd->Color = 24; //FontPixelHeight
            pCmd++;
            pInt[0]++;

            /* Draw String */
            pCmd->Msg = CAMCTRL_DISP_DRAWSTRING;
            pCmd->Channel = 1;
            pCmd->X1 = 750; //X
            pCmd->Y1 = 300; //Y
            pCmd->X2 = 0xffff0001; //Color
            strncpy((char *)&(pCmd->Y2), "Test123456", 11); //string
            pCmd++;
            pInt[0]++;

            /* OSD flush */
            pCmd->Msg = CAMCTRL_DISP_FLUSH;
            pCmd->Channel = 1;
            pInt[0]++;

            Request.ParamSize = pInt[0] * sizeof(OSD_Command_s) + sizeof(pInt[0]);
            ShowHelp = 0;

        } else if (strcmp(Argv[1], "multi_cmd2") == OK) {
            CV_OSD_Enable_s *pCmd = (CV_OSD_Enable_s *) &Request;
            CV_OSD_DrawRect_s *Rect = (CV_OSD_DrawRect_s *) &Request;
            CV_OSD_Flush_s *pFlush = (CV_OSD_Flush_s *) &Request;

            /* create socket */
            sockfd = socket(PF_INET, SOCK_STREAM, 0);

            /* initialize value in dest */
            bzero(&dest, sizeof(dest));
            dest.sin_family = PF_INET;
            dest.sin_port = htons(SERVER_PORT);
            dest.sin_addr.s_addr = inet_addr("127.0.0.1");

            /* Connecting to server */
            connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));

            /* OSD enable */
            Request.Msg = CAMCTRL_DISP_ENABLE;
            Request.ParamSize = 2 * sizeof(int);
            pCmd->Channel = 1;
            pCmd->Enable = 1;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Request.Msg = CAMCTRL_DISP_DRAW_RECT;
            Request.ParamSize = 7 * sizeof(int);
            Rect->Channel = 1;
            Rect->X1 = 50;
            Rect->Y1 = 100;
            Rect->X2 = 100;
            Rect->Y2 = 150;
            Rect->Color = 0xffff0001;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 100;
            Rect->Y1 = 300;
            Rect->X2 = 50;
            Rect->Y2 = 200;
            Rect->Color = 0xff8f8f8f;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 150;
            Rect->Y1 = 100;
            Rect->X2 = 200;
            Rect->Y2 = 200;
            Rect->Color = 0xff00ff02;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 250;
            Rect->Y1 = 250;
            Rect->X2 = 150;
            Rect->Y2 = 350;
            Rect->Color = 0xff0000ff;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 350;
            Rect->Y1 = 100;
            Rect->X2 = 400;
            Rect->Y2 = 150;
            Rect->Color = 0xffff0001;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 400;
            Rect->Y1 = 300;
            Rect->X2 = 350;
            Rect->Y2 = 200;
            Rect->Color = 0xffff0001;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 450;
            Rect->Y1 = 100;
            Rect->X2 = 500;
            Rect->Y2 = 200;
            Rect->Color = 0xffff0001;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 550;
            Rect->Y1 = 250;
            Rect->X2 = 450;
            Rect->Y2 = 350;
            Rect->Color = 0xff0000ff;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 650;
            Rect->Y1 = 100;
            Rect->X2 = 700;
            Rect->Y2 = 200;
            Rect->Color = 0xff0000ff;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Draw Rectangle */
            Rect->Channel = 1;
            Rect->X1 = 750;
            Rect->Y1 = 250;
            Rect->X2 = 850;
            Rect->Y2 = 350;
            Rect->Color = 0xff0000ff;
            Rect->Thickness = 3;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* OSD flush */
            Request.Msg = CAMCTRL_DISP_FLUSH;
            Request.ParamSize = sizeof(int);
            pFlush->Channel = 1;
            send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
            recv(sockfd, buf, sizeof(buf), 0);

            /* Close connection */
            close(sockfd);

            ShowHelp = 1;
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s [operation]\n",Argv[0]);
        PrintFunc("operations:\n");
        PrintFunc("\t enable [channel] [0|1]. channel: Vout channel\n");
        PrintFunc("\t wrt_pix [channel] [X] [Y] [Color]. channel: Vout channel, X,Y: pixel position, Color: Color\n");
        PrintFunc("\t line [channel] [Xstart] [Ystart] [Xend] [Yend] [Color] [thickness].\n");
        PrintFunc("\t\t channel: Vout channel, Xstart,Ystart: start pixel position, Xend,Yend: end pixel position,\n");
        PrintFunc("\t\t Color: Color, Thickness: Thickness\n");
        PrintFunc("\t rect [channel] [Xstart] [Ystart] [Xend] [Yend] [Color] [thickness].\n");
        PrintFunc("\t\t channel: Vout channel, Xstart,Ystart: Top-left pixel position, Xend,Yend: Bottom-right pixel position,\n");
        PrintFunc("\t\t Color: Color, Thickness: Thickness\n");
        PrintFunc("\t flush [channel]. channel: Vout channel\n");
        PrintFunc("\t clear [channel]. channel: Vout channel\n");
        PrintFunc("\t charconf [channel] [AttributeData] [AttributeBits] [FontFaceIdx] [FontPixelWidth] [FontPixelHeight]\n");
        PrintFunc("\t string [channel] [X] [Y] [Color] [string]\n");
        PrintFunc("\t\t channel: Vout channel, X,Y: position, Color:Font color, string:string to draw (Maximal 11 chars)\n");
    } else {
        /* create socket */
        sockfd = socket(PF_INET, SOCK_STREAM, 0);

        /* initialize value in dest */
        bzero(&dest, sizeof(dest));
        dest.sin_family = PF_INET;
        dest.sin_port = htons(SERVER_PORT);
        dest.sin_addr.s_addr = inet_addr("127.0.0.1");

        /* Connecting to server */
        connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
        send(sockfd, &Request, Request.ParamSize + sizeof(Request.Msg) + sizeof(Request.ParamSize), 0);
        recv(sockfd, buf, sizeof(buf), 0);
        /* Close connection */
        close(sockfd);
    }
    return 0;
}
