#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "aipc_user.h"
#include "AmbaIPC_RpcProg_RT_CamCtrl.h"

#include "CamCtrlMsgDef.h"
#include "CamCtrlOsdCmdDef.h"

#define NG (-1)
#define OK (0)

#define PrintFunc printf

int CamCtrl_ProcessRequet(unsigned int Msg, unsigned char *Param, unsigned int ParamSize)
{
	AMBA_IPC_REPLY_STATUS_e status;
	RT_CV_Request_s Arg = {0};
	int Result = OK;
	CLIENT_ID_t Clnt;

	if (ParamSize > CV_MAX_PARAM_SIZE) {
		PrintFunc("%s: ParamSize(%d) reach limit(%d)!!\n",__FUNCTION__, ParamSize, CV_MAX_PARAM_SIZE);
		return NG;
	}

	Clnt = ambaipc_clnt_create(1, RT_CAMCTRL_PROG_ID, RT_CAMCTRL_VER);
	if (!Clnt) {
		PrintFunc("ambaipc_clnt_create failed\n");
		return NG;
	}

	Arg.Msg = Msg;
	Arg.ParamSize = ParamSize;
	memcpy(Arg.Param, Param, ParamSize);

	status = ambaipc_clnt_call(Clnt, RT_CAMCTRL_FUNC_REQUEST,
							   &Arg, sizeof(RT_CV_Request_s),
							   &Result, sizeof(int),
							   RT_CAMCTRL_DEFULT_TIMEOUT);

	if(status != AMBA_IPC_REPLY_SUCCESS) {
		Result = NG;
	}

	ambaipc_clnt_destroy(Clnt);

	return Result;
}

static int CamCtrl_TestCmd_Sys(int Argc, char **Argv)
{
    int Rval = NG;
    int ShowHelp = 1;
    CV_Request_s Request;

    if (Argc >= 3) {
        if (strcmp(Argv[2], "init_dsp") == OK) {
            CV_Dsp_Init_Data_s *DspInitData = (CV_Dsp_Init_Data_s *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_IDSP_INIT_CONFIG;
            Rval = OK;
            if (Argc > 17) {
                //pInt[Idx++] = atoi(Argv[3]); //AsyncQueueAddress
                //pInt[Idx++] = atoi(Argv[4]);
                //pInt[Idx++] = atoi(Argv[5]); //VPQueueAddress
                //pInt[Idx++] = atoi(Argv[6]);
                //pInt[Idx++] = atoi(Argv[7]);
                PrintFunc("NOT READY YET!!\n");
                Rval = NG;
            } else {
                unsigned int *pInt;
                uintptr_t Addr;

                DspInitData->async_cmd_queue_addr = 0x60000000;
                DspInitData->async_cmd_queue_size = 4096;
                DspInitData->vp_msg_queue_cb_addr = 0x62000000;
                Addr = DspInitData->vp_msg_queue_cb_addr;
                pInt = (unsigned int *)Addr;
                pInt[0] = 0x61000000; /* base address of Message */
                pInt[1] = 32; /* maximum number of messages */
                pInt[2] = 0;  /* read pointer */
                pInt[3] = 0;  /* write pointer */

                DspInitData->num_frame_list_descriptor = 1;
                DspInitData->frame_list_descriptor[0].fov_id = 0;
                DspInitData->frame_list_descriptor[0].frame_list_table_addr = 0x63000000;
                DspInitData->frame_list_descriptor[0].frame_list_table_size = 5;
                DspInitData->frame_list_descriptor[0].frame_buffer_length = 0x1000000;
                Addr = DspInitData->frame_list_descriptor[0].frame_list_table_addr;
                pInt = (unsigned int *)Addr;
                pInt[0] = 0x64000000;
                pInt[1] = 0x65000000;
                pInt[2] = 0x66000000;
                pInt[3] = 0x67000000;
                pInt[4] = 0x68000000;

                DspInitData->frame_list_descriptor[0].lanedection_frame_list_table_addr = 0x63100000;
                DspInitData->frame_list_descriptor[0].lanedection_frame_list_table_size = 5;
                Addr = DspInitData->frame_list_descriptor[0].lanedection_frame_list_table_addr;
                pInt = (unsigned int *) Addr;
                DspInitData->frame_list_descriptor[0].lanedect_frame_buffer_length = 0x400000;
                pInt[0] = 0x64c00000;
                pInt[1] = 0x65c00000;
                pInt[2] = 0x66c00000;
                pInt[3] = 0x67c00000;
                pInt[4] = 0x68c00000;
            }

            if(Rval == OK) {
                Request.ParamSize = sizeof(CV_Dsp_Init_Data_s);

                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "en_lv") == OK) {
            unsigned int *pInt = (unsigned int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_ENABLE_LIVEVIEW;
            Request.ParamSize = sizeof(unsigned int);
            if (Argc > 3) {
                *pInt = strtoul(Argv[3], NULL, 0);
            } else {
                *pInt = 1; //Enable
            }
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "en_hdr") == OK) {
            unsigned int *pInt = (unsigned int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_ENABLE_HDR;
            Request.ParamSize = sizeof(unsigned int);
            if (Argc > 3) {
                *pInt = strtoul(Argv[3], NULL, 0);
            } else {
                *pInt = 1; //Enable
            }
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "conf_lv") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_LIVEVIEW_CONFIG;
            Request.ParamSize = 5*sizeof(int);
            if (Argc > 7) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
                pInt[2] = atoi(Argv[5]);
                pInt[3] = atoi(Argv[6]);
                pInt[4] = strtoul(Argv[7], NULL, 0);
            } else {
                pInt[0] = 0; //InputMode
                pInt[1] = 0; //ResolutionID
                pInt[2] = 0; //Raw_FileAmount
                pInt[3] = 0; //Raw_Period
                pInt[4] = CAMCTRLFLAG_LIVEVIEW_NOCHANGE; //use default CtrlFlags setting
            }
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "feed_next") == OK) {
            Request.Msg = CAMCTRL_SYS_FEED_NEXT_RAW;
            Request.ParamSize = 0;
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "stop_feed") == OK) {
            Request.Msg = CAMCTRL_SYS_STOP_RAW_FEED;
            Request.ParamSize = 0;
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "init_frame") == OK) {
            CV_Frame_Init_Data_s *DspInitData = (CV_Frame_Init_Data_s *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_FRAME_INIT_CONFIG;
            Rval = OK;
            if (Argc > 3) {
                DspInitData->fovid = atoi(Argv[3]);

                DspInitData->pyramid_config.HierBit = 0x1fff;
                DspInitData->pyramid_config.IsPolySqrt2 = 1;
                DspInitData->pyramid_config.HierPolyWidth = 1360;
                DspInitData->pyramid_config.HierPolyHeight = 764;
                DspInitData->pyramid_config.HierTag = 0;
                memset(&DspInitData->pyramid_config.HierCropWindow, 0, sizeof(DSP_WINDOW_s)*AMBA_DSP_MAX_HIER);

                DspInitData->lanedection_config.HierSource = 1;
                DspInitData->lanedection_config.Width = 1360;
                DspInitData->lanedection_config.Height = 764;
                DspInitData->lanedection_config.CropWindow.OffsetX = 0;
                DspInitData->lanedection_config.CropWindow.OffsetY = 0;
                DspInitData->lanedection_config.CropWindow.Width = 0;
                DspInitData->lanedection_config.CropWindow.Height = 0;

                DspInitData->pyramid_frame_setting.DataFmt = 0;
                DspInitData->pyramid_frame_setting.Pitch = 1952;
                DspInitData->pyramid_frame_setting.Window.OffsetX = 0;
                DspInitData->pyramid_frame_setting.Window.OffsetY = 0;
                DspInitData->pyramid_frame_setting.Window.Width = 1952;
                DspInitData->pyramid_frame_setting.Window.Height = 2970;

                DspInitData->lanedection_frame_setting.DataFmt = 0;
                DspInitData->lanedection_frame_setting.Pitch = 1920;
                DspInitData->lanedection_frame_setting.Window.OffsetX = 0;
                DspInitData->lanedection_frame_setting.Window.OffsetY = 0;
                DspInitData->lanedection_frame_setting.Window.Width = 1920;
                DspInitData->lanedection_frame_setting.Window.Height = 1080;
            } else {
                Rval = NG;
            }

            if(Rval == OK) {
                Request.ParamSize = sizeof(CV_Frame_Init_Data_s);
                //AmbaPrint("%s[%d] Param_Size = %d",__func__,__LINE__,Request.ParamSize);
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "pyramid") == OK) {
            int *Fovid = (int *)&Request.Param[0];
            PYRAMID_CONFIG_s *PyramidConfig = (PYRAMID_CONFIG_s *)&Fovid[1];

            Request.Msg = CAMCTRL_SYS_SET_PYRAMID_INIT_CONFIG;
            Rval = OK;
            if (Argc > 8) {
                Fovid[0] = atoi(Argv[3]);

                PyramidConfig->HierBit = strtoul(Argv[4], NULL, 0);
                PyramidConfig->IsPolySqrt2 = atoi(Argv[5]);
                PyramidConfig->HierPolyWidth = atoi(Argv[6]);
                PyramidConfig->HierPolyHeight = atoi(Argv[7]);
                PyramidConfig->HierTag = strtoul(Argv[8], NULL, 0);
                memset(&PyramidConfig->HierCropWindow, 0, sizeof(DSP_WINDOW_s)*AMBA_DSP_MAX_HIER);

                /*
                PrintFunc("Fovid=%d\n",*Fovid);
                PrintFunc("HierBit=0x%08x\n",PyramidConfig->HierBit);
                PrintFunc("IsPolySqrt2=%u\n",PyramidConfig->IsPolySqrt2);
                PrintFunc("HierPolyWidth=%u\n",PyramidConfig->HierPolyWidth);
                PrintFunc("HierPolyHeight=%u\n",PyramidConfig->HierPolyHeight);
                PrintFunc("HierTag=%u\n",PyramidConfig->HierTag);
                */
            } else {
                Rval = NG;
            }

            if(Rval == OK) {
                Request.ParamSize = sizeof(int) + sizeof(PYRAMID_CONFIG_s);
                //AmbaPrint("%s[%d] Param_Size = %d",__func__,__LINE__,Request.ParamSize);
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "lndt") == OK) {
            int *Fovid = (int *)&Request.Param[0];
            LANE_DETECT_CONFIG_s *LndtConfig = (LANE_DETECT_CONFIG_s *)&Fovid[1];

            Request.Msg = CAMCTRL_SYS_SET_LANEDETECTION_INIT_CONFIG;
            Rval = OK;
            if (Argc > 10) {
                Fovid[0] = atoi(Argv[3]);

                LndtConfig->HierSource = atoi(Argv[4]);
                LndtConfig->CropWindow.OffsetX = atoi(Argv[5]);
                LndtConfig->CropWindow.OffsetY = atoi(Argv[6]);
                LndtConfig->CropWindow.Width = atoi(Argv[7]);
                LndtConfig->CropWindow.Height = atoi(Argv[8]);
                LndtConfig->Width = atoi(Argv[9]);
                LndtConfig->Height = atoi(Argv[10]);

                /*
                PrintFunc("Fovid=%d\n",*Fovid);
                PrintFunc("HierSource=%u\n",LndtConfig->HierSource);
                PrintFunc("OffsetX=%u\n",LndtConfig->CropWindow.OffsetX);
                PrintFunc("OffsetY=%u\n",LndtConfig->CropWindow.OffsetY);
                PrintFunc("CropWidth=%u\n",LndtConfig->CropWindow.Width);
                PrintFunc("CropHeight=%u\n",LndtConfig->CropWindow.Height);
                PrintFunc("Width=%u\n",LndtConfig->Width);
                PrintFunc("Height=%u\n",LndtConfig->Height);
                */
            } else {
                Rval = NG;
            }

            if(Rval == OK) {
                Request.ParamSize = sizeof(int) + sizeof(LANE_DETECT_CONFIG_s);
                //AmbaPrint("%s[%d] Param_Size = %d",__func__,__LINE__,Request.ParamSize);
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "pyramidbuf") == OK) {
            int *Fovid = (int *)&Request.Param[0];
            YUV_IMG_BUF_SETTING_s *PyramidBufSetting = (YUV_IMG_BUF_SETTING_s *)&Fovid[1];

            Request.Msg = CAMCTRL_SYS_SET_PYRAMID_BUFFER_INFO;
            Rval = OK;
            if (Argc > 9) {
                Fovid[0] = atoi(Argv[3]);

                PyramidBufSetting->DataFmt = atoi(Argv[4]);
                PyramidBufSetting->Pitch = atoi(Argv[5]);
                PyramidBufSetting->Window.OffsetX = atoi(Argv[6]);
                PyramidBufSetting->Window.OffsetY = atoi(Argv[7]);
                PyramidBufSetting->Window.Width = atoi(Argv[8]);
                PyramidBufSetting->Window.Height = atoi(Argv[9]);

                /*
                PrintFunc("Fovid=%d\n",*Fovid);
                PrintFunc("DataFmt=%u\n",PyramidBufSetting->DataFmt);
                PrintFunc("Pitch=%u\n",PyramidBufSetting->Pitch);
                PrintFunc("OffsetX=%u\n",PyramidBufSetting->Window.OffsetX);
                PrintFunc("OffsetY=%u\n",PyramidBufSetting->Window.OffsetY);
                PrintFunc("Width=%u\n",PyramidBufSetting->Window.Width);
                PrintFunc("Height=%u\n",PyramidBufSetting->Window.Height);
                */
            } else {
                Rval = NG;
            }

            if(Rval == OK) {
                Request.ParamSize = sizeof(int) + sizeof(YUV_IMG_BUF_SETTING_s);
                //AmbaPrint("%s[%d] Param_Size = %d",__func__,__LINE__,Request.ParamSize);
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "lndtbuf") == OK) {
            int *Fovid = (int *)&Request.Param[0];
            YUV_IMG_BUF_SETTING_s *LndtBufSetting = (YUV_IMG_BUF_SETTING_s *)&Fovid[1];

            Request.Msg = CAMCTRL_SYS_SET_LANEDETECTION_BUFFER_INFO;
            Rval = OK;
            if (Argc > 9) {
                Fovid[0] = atoi(Argv[3]);

                LndtBufSetting->DataFmt = atoi(Argv[4]);
                LndtBufSetting->Pitch = atoi(Argv[5]);
                LndtBufSetting->Window.OffsetX = atoi(Argv[6]);
                LndtBufSetting->Window.OffsetY = atoi(Argv[7]);
                LndtBufSetting->Window.Width = atoi(Argv[8]);
                LndtBufSetting->Window.Height = atoi(Argv[9]);

                /*
                PrintFunc("Fovid=%d\n",*Fovid);
                PrintFunc("DataFmt=%u\n",LndtBufSetting->DataFmt);
                PrintFunc("Pitch=%u\n",LndtBufSetting->Pitch);
                PrintFunc("OffsetX=%u\n",LndtBufSetting->Window.OffsetX);
                PrintFunc("OffsetY=%u\n",LndtBufSetting->Window.OffsetY);
                PrintFunc("Width=%u\n",LndtBufSetting->Window.Width);
                PrintFunc("Height=%u\n",LndtBufSetting->Window.Height);
                */
            } else {
                Rval = NG;
            }

            if(Rval == OK) {
                Request.ParamSize = sizeof(int) + sizeof(YUV_IMG_BUF_SETTING_s);
                //AmbaPrint("%s[%d] Param_Size = %d",__func__,__LINE__,Request.ParamSize);
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "dly_vsync") == OK) {
            int *pInt = (int *)&Request.Param[0];

            pInt[0] = atoi(Argv[3]);
            pInt[1] = atoi(Argv[4]);
            pInt[2] = atoi(Argv[5]);
            pInt[3] = atoi(Argv[6]);
            pInt[4] = atoi(Argv[7]);

            Request.Msg = CAMCTRL_SYS_SET_DELAY_VSYNC;
            Request.ParamSize = 5 * sizeof(int);
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "start_vin") == OK) {
            Request.Msg = CAMCTRL_SYS_START_VIN_CAPTURE;
            Request.ParamSize = 0;
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "dspworkbuf") == OK) {
            int *pInt = (int *)&Request.Param[0];

            if (Argc > 3) {
                pInt[0] = atoi(Argv[3]);
                if (Argc > 4) {
                    pInt[1] = atoi(Argv[4]);
                } else {
                    pInt[1] = 0;
                }

                Request.Msg = CAMCTRL_SYS_SET_DSPWORKBUF;
                Request.ParamSize = 2 * sizeof(int);
                ShowHelp = 0;
            } else {
                ShowHelp = 1;
            }
        } else if (strcmp(Argv[2], "dispsel") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_DISPOUTPUT;
            Request.ParamSize = sizeof(int)*2;
            if (Argc > 4) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
            } else {
                pInt[0] = 1; //Ch: TV
                pInt[1] = 0; //FovId: Fov-0
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "inputpath") == OK) {
            char *Path;

            Request.Msg = CAMCTRL_SYS_SET_INPUTFILEPATH;
            if (Argc > 5) {
                Request.Param[0] = atoi(Argv[3]);
                Request.Param[1] = atoi(Argv[4]);
                Path = (char *)&Request.Param[2];
                strcpy(Path, Argv[5]);
            } else {
                Request.Param[0] = 0; //TYPE_RAW
                Request.Param[1] = 0; //CH_0
                Path = (char *)&Request.Param[2];
                strcpy(Path, "c:\\rawfiles");
            }
            Request.ParamSize = strlen(Path) + 1 + 2; //include EOS
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "startid") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_INPUTSTARTID;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                pInt[0] = atoi(Argv[3]);
            } else {
                pInt[0] = 0; //startid
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "inputoffset") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_INPUTOFFSET;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                pInt[0] = atoi(Argv[3]);
            } else {
                pInt[0] = 1; //Offset
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "taskamount") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_CAMCTRL_TASK_AMOUNT;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                pInt[0] = atoi(Argv[3]);
            } else {
                pInt[0] = 1; //Camctrl Task amount
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "conf_mipicsi") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_SYS_SET_MIPICSI_MODE;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                pInt[0] = atoi(Argv[3]);
            } else {
                pInt[0] = 0; //AR16_9_1920_1080_P30_1X_1X_P30
            }

            ShowHelp = 0;
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s %s [operation]\n",Argv[0],Argv[1]);
        PrintFunc("operations:\n");
        PrintFunc("\t init_dsp\n");
        PrintFunc("\t en_lv [0|1]\n");
        PrintFunc("\t en_hdr [0|1]\n");
        PrintFunc("\t conf_lv [InputMode] [ResolutionID] [RawFileAmount] [RawPeriod] [CtrlFlags]\n");
        PrintFunc("\t\t InputMode: 0-Sensor 1-Raw 2-HDMI.\n");
        PrintFunc("\t\t ResolutionID: resolution id for sensor mode. (for Raw/YUV: 0-1920x1080, 13-3840x2160)\n");
        PrintFunc("\t\t RawFileAmount: Total Raw file amount. (Raw mode only)\n");
        PrintFunc("\t\t RawPeriod: Time to feed next Raw file (in ms). (Raw mode only)\n");
        PrintFunc("\t\t CtrlFlag: bit-wise flags for liveview behavior.\n");
        PrintFunc("\t\t\t bit-0: Multiple Vin on(1)/off(0).\n");
        PrintFunc("\t\t\t bit-1: File input format, YUV(1)/RAW(0).\n");
        PrintFunc("\t\t\t bit-2: Vout type, TV(1)/LCD(0).\n");
        PrintFunc("\t\t\t bit-3: TV output mode, MIPICSI(1)/HDMI(0).\n");
        PrintFunc("\t\t\t bit-4: 3A mode, Enable(1)/Disable(0).\n");
        PrintFunc("\t init_frame [FovId]\n");
        PrintFunc("\t start_vin\n");
        PrintFunc("\t dspworkbuf [Size] [Addr]\n");
        PrintFunc("\t dispsel [Ch] [FovId]. Show [FovId] on [Ch].\n");
        PrintFunc("\t\t [Ch]: 0-LCD, 1-TV.\n");
        PrintFunc("\t inputpath [Type] [Ch] [Path]. Set Path for Raw/YUV file input.\n");
        PrintFunc("\t\t [Type]: 0-RAW, 1-YUV, 2-Ituner Input File.\n");
        PrintFunc("\t\t [Ch]: Channel (0/1).\n");
        PrintFunc("\t startid [Id]. Set start id for Raw/YUV file input.\n");
        PrintFunc("\t taskamount [Amount]. Set amount of Camctrl CVTask enabled.\n");
        PrintFunc("\t inputoffset [Offset]. Set offset for Raw/YUV file input.(Next_ID = current_ID + Offset)\n");
    } else {
        Rval = CamCtrl_ProcessRequet(Request.Msg, Request.Param, Request.ParamSize);
    }

    return Rval;
}

static int CamCtrl_TestCmd_Img(int Argc, char **Argv)
{
    int Rval = NG;
    int ShowHelp = 1;
    CV_Request_s Request;

    if (Argc >= 3) {
        if (strcmp(Argv[2], "set_evbias") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_IMG_SET_EVBIAS;
            Request.ParamSize = 2*sizeof(int);
            if (Argc > 4) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
            } else {
                pInt[0] = 0; //Ch0
                pInt[1] = 0; //0EV
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_de") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_IMG_SET_DECOLOR;
            Request.ParamSize = 2*sizeof(int);
            if (Argc > 4) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
            } else {
                pInt[0] = 0; //Ch0
                pInt[1] = 0; //default
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_scene") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_IMG_SET_SCENE;
            Request.ParamSize = 3*sizeof(int);
            if (Argc > 5) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
                pInt[2] = atoi(Argv[5]);
            } else {
                pInt[0] = 0; //Ch0
                pInt[1] = 0; //VinNum
                pInt[2] = 0; //SceneMode
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_slowshutter") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_IMG_SET_SLOWSHUTTER;
            Request.ParamSize = 3*sizeof(int);
            if (Argc > 5) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
                pInt[2] = atoi(Argv[5]);
            } else {
                pInt[0] = 0; //Ch0
                pInt[1] = 0; //Mode
                pInt[2] = 0; //SlowshutterEnable
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_sharpness") == OK) {
            unsigned int *pInt = (unsigned int *)&Request.Param[0];

            Request.Msg = CAMCTRL_IMG_SET_SHARPNESS;
            Request.ParamSize = sizeof(unsigned int);
            if (Argc > 3) {
                *pInt = strtoul(Argv[3], NULL, 0);
            } else {
                *pInt = 0; //Default value
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_colortemp") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_IMG_SET_COLORTEMP;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                *pInt = strtoul(Argv[3], NULL, 0);
            } else {
                *pInt = 0; //Default value
            }

            ShowHelp = 0;
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s %s [operation]\n",Argv[0],Argv[1]);
        PrintFunc("operations:\n");
        PrintFunc("\t set_evbias [ch][value]. ch: ChNo. value: 32:+1EV, 0:0EV, -32:-1EV\n");
        PrintFunc("\t set_de [ch] [value]. ch: ChNo. value: 0~6\n");
        PrintFunc("\t set_scene [ch] [VinNum] [value]. ch: ChNo. VinNum:VinNum. value: 0~38\n");
        PrintFunc("\t set_slowshutter [ch] [Mode] [value]. ch: ChNo. Mode:0-video 1-photo. value: 0~2\n");
        PrintFunc("\t set_sharpness [value]. value: unknown\n");
        PrintFunc("\t set_colortemp [value]. value: unknown\n");
    } else {
        Rval = CamCtrl_ProcessRequet(Request.Msg, Request.Param, Request.ParamSize);
    }

    return Rval;
}

static int CamCtrl_TestCmd_Enc(int Argc, char **Argv)
{
    int Rval = NG;
    int ShowHelp = 1;
    CV_Request_s Request;

    if (Argc >= 3) {
        if (strcmp(Argv[2], "set_resolution") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_ENC_SET_RESOLUTION;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                *pInt = atoi(Argv[3]);
            } else {
                *pInt = 0; //fist one
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_bitrate") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_ENC_SET_BITRATE;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                *pInt = atoi(Argv[3]);
            } else {
                *pInt = 2000000; //2Mbits
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_framerate") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_ENC_SET_FRAMERATE;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                *pInt = atoi(Argv[3]);
            } else {
                *pInt = 30; //30fps
            }

            ShowHelp = 0;
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s %s [operation]\n",Argv[0],Argv[1]);
        PrintFunc("operations:\n");
        PrintFunc("\t set_resolution [value]. value:unknow\n");
        PrintFunc("\t set_bitrate [value]. value: bitrate in bps\n");
        PrintFunc("\t set_framerate [value]. value: frame rate in fps\n");
    } else {
        Rval = CamCtrl_ProcessRequet(Request.Msg, Request.Param, Request.ParamSize);
    }

    return Rval;
}

static int CamCtrl_TestCmd_Roi(int Argc, char **Argv)
{
    int Rval = NG;
    int ShowHelp = 1;
    CV_Request_s Request;

    if (Argc >= 3) {
        if (strcmp(Argv[2], "set_zoomstep") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_ROI_SET_ZOOMSTEP;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                *pInt = atoi(Argv[3]);
            } else {
                *pInt = 0; //step 0
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_pan") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_ROI_SET_PAN;
            Request.ParamSize = sizeof(int)*2;
            if (Argc > 4) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
            } else {
                pInt[0] = 1; //X+1
                pInt[1] = 0; //Y+0
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "set_weighting") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_ROI_SET_WEIGHTING;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                *pInt = atoi(Argv[3]);
            } else {
                *pInt = 0; //unknown
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "update_pyramid") == OK) {
            int Idx = 0;
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_ROI_UPDATE_PYRAMID;
            if (Argc > 4) {
                int HierAmount = 0;
                pInt[Idx++] = atoi(Argv[3]);
                pInt[Idx++] = atoi(Argv[4]); //HierTag
                pInt[Idx++] = atoi(Argv[5]); //NumHier
                HierAmount = pInt[Idx-1];

                PrintFunc("Argc=%d, expected len=%d\n",Argc, ((HierAmount * 6) + 6));
                if (Argc != ((HierAmount * 6) + 6)) {
                    PrintFunc("%s: Invalid parameter length. Please make sure you follow the API definition!\n",Argv[2]);
                    ShowHelp = 1;
                    Rval = NG;
                } else {
                    int i, k;

                    for (i=0, k=5; i<HierAmount; i++){
                        pInt[Idx++] = atoi(Argv[k++]); //HierIdx_N
                        pInt[Idx++] = atoi(Argv[k++]); //HierEnable_N
                        pInt[Idx++] = atoi(Argv[k++]); //OffsetX_N
                        pInt[Idx++] = atoi(Argv[k++]); //OffsetY_N
                        pInt[Idx++] = atoi(Argv[k++]); //Width_N
                        pInt[Idx++] = atoi(Argv[k++]); //Height_N
                    }

                    Rval = OK;
                }
            } else {
                pInt[Idx++] = 4; //ViewZoneID
                pInt[Idx++] = 8; //HierTag
                pInt[Idx++] = 2; //NumHier
                pInt[Idx++] = 3; //HierIdx_1
                pInt[Idx++] = 1; //HierEnable_1
                pInt[Idx++] = 10; //OffsetX_1
                pInt[Idx++] = 10; //OffsetY_1
                pInt[Idx++] = 848; //Width_1
                pInt[Idx++] = 480; //Height_1
                pInt[Idx++] = 5; //HierIdx_2
                pInt[Idx++] = 1; //HierEnable_2
                pInt[Idx++] = 100; //OffsetX_1
                pInt[Idx++] = 100; //OffsetY_1
                pInt[Idx++] = 320; //Width_1
                pInt[Idx++] = 240; //Height_1

                Rval = OK;
            }

            if(Rval == OK) {
                PrintFunc("%s[%d]: Idx=%d\n", __FUNCTION__,__LINE__,Idx);
                Request.ParamSize = Idx * sizeof(int);
#if 0
                do {
                    int i;
                    for(i=0;i<Idx;i++) {
                        PrintFunc("pInt[%d] = %u\n",i,pInt[i]);
                    }
                } while(0);
#endif
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "update_aeroi") == OK) {
            int Idx;
            AEROI_SETTING_s *AeRoiSetting;
            CV_Roi_AeroiSetting_s *AeRoiInfo = (CV_Roi_AeroiSetting_s *)&Request.Param[0];

            Request.Msg = CAMCTRL_ROI_UPDATE_AEROI;
            if (Argc > 9) {
                AeRoiInfo->NumChanges = atoi(Argv[3]);

                for(Idx = 0; Idx < AeRoiInfo->NumChanges; Idx++) {
                    AeRoiSetting = &(AeRoiInfo->AeroiSetting[Idx]);

                    AeRoiSetting->FovId = atoi(Argv[4]) + Idx;
                    AeRoiSetting->Token = atoi(Argv[5]);
                    AeRoiSetting->X = atoi(Argv[6]);
                    AeRoiSetting->Y = atoi(Argv[7]);
                    AeRoiSetting->Width = atoi(Argv[8]);
                    AeRoiSetting->Height = atoi(Argv[9]);
                }

                Request.ParamSize = sizeof(CV_Roi_AeroiSetting_s);
                ShowHelp = 0;
            }
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s %s [operation]\n",Argv[0],Argv[1]);
        PrintFunc("operations:\n");
        PrintFunc("\t set_zoomstep [value]. value:zoomstep\n");
        PrintFunc("\t set_pan [X offset] [Y offset].\n");
        PrintFunc("\t set_weighting [value]. value: unknown\n");
        PrintFunc("\t update_pyramid [VinID] [Amount of change] [...change detail...] [HierTag]\n");
        PrintFunc("\t update_aeroi [Amount of change] [start Fov] [Token] [X] [Y] [Width] [Height]\n");
    } else {
        Rval = CamCtrl_ProcessRequet(Request.Msg, Request.Param, Request.ParamSize);
    }

    return Rval;
}

static int CamCtrl_TestCmd_Warp(int Argc, char **Argv)
{
    int Rval = NG;
    int ShowHelp = 1;
    CV_Request_s Request;

    if (Argc >= 3) {
        if (strcmp(Argv[2], "en_warp") == OK) {
            unsigned int *pInt = (unsigned int *)&Request.Param[0];

            Request.Msg = CAMCTRL_WARP_ENABLE;
            Request.ParamSize = sizeof(unsigned int);
            if (Argc > 3) {
                *pInt = strtoul(Argv[3], NULL, 0);
            } else {
                *pInt = 1; //Enable
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "update_warp") == OK) {
            int Idx;
            WARP_SETTING_s *WarpSetting;
            CV_Warp_WarpSetting_s *WarpInfo = (CV_Warp_WarpSetting_s *)&Request.Param[0];

            Request.Msg = CAMCTRL_WARP_UPDATE;
            if (Argc > 6) {
                WarpInfo->NumChanges = atoi(Argv[3]); //FovNum

                for(Idx = 0; Idx < WarpInfo->NumChanges; Idx++) {
                    WarpSetting = &(WarpInfo->WarpSetting[Idx]);
                    WarpSetting->FovId = atoi(Argv[4]) + Idx;
                    WarpSetting->Token = atoi(Argv[5]);
                    WarpSetting->WarpInfoAddr = strtoul(Argv[6], NULL, 0);
                }

                Request.ParamSize = sizeof(CV_Warp_WarpSetting_s);
                ShowHelp = 0;
            }
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s %s [operation]\n",Argv[0],Argv[1]);
        PrintFunc("operations:\n");
        PrintFunc("\t en_warp [0|1]\n");
        PrintFunc("\t update_warp [Amount of change] [start Fov] [Token] [WarpInfoAddr]\n");
    } else {
        Rval = CamCtrl_ProcessRequet(Request.Msg, Request.Param, Request.ParamSize);
    }

    return Rval;
}

static int CamCtrl_TestCmd_Disp(int Argc, char **Argv)
{
    int Rval = NG;
    int ShowHelp = 1;
    CV_Request_s Request;

    if (Argc >= 3) {
        if (strcmp(Argv[2], "enable") == OK) {
            CV_OSD_Enable_s *pCmd = (CV_OSD_Enable_s *) &Request;

            Request.Msg = CAMCTRL_DISP_ENABLE;
            Request.ParamSize = 2 * sizeof(int);
            if (Argc > 4) {
                pCmd->Channel = atoi(Argv[3]);
                pCmd->Enable = atoi(Argv[4]);
            } else {
                pCmd->Channel = 0;
                pCmd->Enable = 1;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "wrt_pix") == OK) {
            CV_OSD_DrawPixel_s *Pixel = (CV_OSD_DrawPixel_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAW_PIXEL;
            Request.ParamSize = 4 * sizeof(int);
            if (Argc > 6) {
                Pixel->Channel = atoi(Argv[3]);
                Pixel->X = atoi(Argv[4]);
                Pixel->Y = atoi(Argv[5]);
                Pixel->Color = strtoul(Argv[6], NULL, 0);

            } else {
                Pixel->Channel = 0;
                Pixel->X = 1;
                Pixel->Y = 1;
                Pixel->Color = 0x80FF0000;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "line") == OK) {
            CV_OSD_DrawLine_s *Line = (CV_OSD_DrawLine_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAW_LINE;
            Request.ParamSize = 7 * sizeof(int);
            if (Argc > 9) {
                Line->Channel = atoi(Argv[3]);
                Line->X1 = atoi(Argv[4]);
                Line->Y1 = atoi(Argv[5]);
                Line->X2 = atoi(Argv[6]);
                Line->Y2 = atoi(Argv[7]);
                Line->Color = strtoul(Argv[8], NULL, 0);
                Line->Thickness = atoi(Argv[9]);
            } else {
                Line->Channel = 0;
                Line->X1 = 50;
                Line->Y1 = 50;
                Line->X2 = 100;
                Line->Y2 = 50;
                Line->Color = 0x80FF0000;
                Line->Thickness = 3;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "rect") == OK) {
            CV_OSD_DrawRect_s *Rect = (CV_OSD_DrawRect_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAW_RECT;
            Request.ParamSize = 7 * sizeof(int);
            if (Argc > 9) {
                Rect->Channel = atoi(Argv[3]);
                Rect->X1 = atoi(Argv[4]);
                Rect->Y1 = atoi(Argv[5]);
                Rect->X2 = atoi(Argv[6]);
                Rect->Y2 = atoi(Argv[7]);
                Rect->Color = strtoul(Argv[8], NULL, 0);
                Rect->Thickness = atoi(Argv[9]);
            } else {
                Rect->Channel = 0;
                Rect->X1 = 50;
                Rect->Y1 = 50;
                Rect->X2 = 100;
                Rect->Y2 = 100;
                Rect->Color = 0x80FF0000;
                Rect->Thickness = 3;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "flush") == OK) {
            CV_OSD_Flush_s *pCmd = (CV_OSD_Flush_s *) &Request;

            Request.Msg = CAMCTRL_DISP_FLUSH;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                pCmd->Channel = atoi(Argv[3]);
            } else {
                pCmd->Channel = 0;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "clear") == OK) {
            CV_OSD_Clear_s *pCmd = (CV_OSD_Clear_s *) &Request;

            Request.Msg = CAMCTRL_DISP_CLEAR;
            Request.ParamSize = sizeof(int);
            if (Argc > 3) {
                pCmd->Channel = atoi(Argv[3]);
            } else {
                pCmd->Channel = 0;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "flushcs") == OK) {
            CV_OSD_FlushCapSeq_s *pCmd = (CV_OSD_FlushCapSeq_s *) &Request;

            Request.Msg = CAMCTRL_DISP_FLUSH_CAPSEQ;
            Request.ParamSize = 4 * sizeof(int); //Due to uint64_t, it needs to do 64 bits alignment
            //PrintFunc("CV_OSD_FlushCapSeq_s=%u, uint32=%u, uint64=%u ull=%u\n",sizeof(CV_OSD_FlushCapSeq_s), sizeof(uint32_t), sizeof(uint64_t), sizeof(unsigned long long));
            if (Argc > 4) {
                pCmd->Channel = atoi(Argv[3]);
                pCmd->CapSeq = atoi(Argv[4]);
            } else {
                pCmd->Channel = 1;
                pCmd->CapSeq = 2;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "multi_cmd") == OK) {
            // This is an example to execute multi commands with CAMCTRL_DISP_MULTI_CMD
            unsigned int *pInt = (unsigned int *)&Request.Param[0];
            OSD_Command_s *pCmd;

            Request.Msg = CAMCTRL_DISP_MULTI_CMD;
            //Request.ParamSize = sizeof(CV_OSD_CommandSequence_s);
            memset(Request.Param, 0 , sizeof(Request.Param));
            pInt[0] = 5; // There are 5 commands to execute.
            pCmd = (OSD_Command_s *)&pInt[1];

            /* OSD enable */
            pCmd->Msg = CAMCTRL_DISP_ENABLE;
            pCmd->Channel = 1;
            pCmd->X1 = 1; //enable
            pCmd++;

            /* Draw Pixel */
            pCmd->Msg = CAMCTRL_DISP_DRAW_PIXEL;
            pCmd->Channel = 1;
            pCmd->X1 = 50;
            pCmd->Y1 = 50;
            pCmd->Color = 0x80FF0000;
            pCmd++;

            /* Draw Line */
            pCmd->Msg = CAMCTRL_DISP_DRAW_LINE;
            pCmd->Channel = 1;
            pCmd->X1 = 200;
            pCmd->Y1 = 50;
            pCmd->X2 = 300;
            pCmd->Y2 = 150;
            pCmd->Color = 0x80FF0000;
            pCmd->Thickness = 3;
            pCmd++;

            /* Draw Rectangle */
            pCmd->Msg = CAMCTRL_DISP_DRAW_RECT;
            pCmd->Channel = 1;
            pCmd->X1 = 250;
            pCmd->Y1 = 250;
            pCmd->X2 = 400;
            pCmd->Y2 = 400;
            pCmd->Color = 0x80FF0000;
            pCmd->Thickness = 6;
            pCmd++;

            /* OSD flush */
            pCmd->Msg = CAMCTRL_DISP_FLUSH;
            pCmd->Channel = 1;
            Request.ParamSize = pInt[0] * sizeof(OSD_Command_s) + sizeof(pInt[0]);
            ShowHelp = 0;
        } else if (strcmp(Argv[2], "roiwin") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_DISP_SET_ROIWINDOW;
            Request.ParamSize = 5 * sizeof(int);
            if (Argc > 7) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = atoi(Argv[4]);
                pInt[2] = atoi(Argv[5]);
                pInt[3] = atoi(Argv[6]);
                pInt[4] = atoi(Argv[7]);
            } else {
                pInt[0] = 1; //Channel
                pInt[1] = 800; //X
                pInt[2] = 540; //Y
                pInt[3] = 320; //width
                pInt[4] = 480; //height
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "wrapinfo") == OK) {
            int *pInt = (int *)&Request.Param[0];

            Request.Msg = CAMCTRL_DISP_SET_WARPINFO;
            Request.ParamSize = 7 * sizeof(int);
            if (Argc > 9) {
                pInt[0] = atoi(Argv[3]);
                pInt[1] = strtoul(Argv[4], NULL, 0);
                pInt[2] = strtoul(Argv[5], NULL, 0);
                pInt[3] = atoi(Argv[6]);
                pInt[4] = atoi(Argv[7]);
                pInt[5] = atoi(Argv[8]);
                pInt[6] = atoi(Argv[9]);
            } else {
                pInt[0] = 1; //Channel
                pInt[1] = 0; //warptable addrss
                pInt[2] = 0; //wraptable size
                pInt[3] = 16; //grid_w
                pInt[4] = 16; //grid_h
                pInt[5] = 21; //grid_num_x
                pInt[6] = 31; //grid_num_y
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "ldresult") == OK) {
            int *pInt = (int *)&Request.Param[0];
            float *pFloat = (float*)(&Request.Param[4]);

            Request.Msg = CAMCTRL_DISP_DRAW_LDRESULT;
            Request.ParamSize = sizeof(int) + 5 * sizeof(float);
            if (Argc > 8) {
                pInt[0] = atoi(Argv[3]);
                pFloat[0] = atof(Argv[4]);
                pFloat[1] = atof(Argv[5]);
                pFloat[2] = atof(Argv[6]);
                pFloat[3] = atof(Argv[7]);
                pFloat[4] = atof(Argv[8]);
            } else {
                pInt[0] = 1; //Channel
                pFloat[0] = 0.0; //a
                pFloat[1] = 0.0; //b
                pFloat[2] = 0.0; //c
                pFloat[3] = 0.5; //Near width
                pFloat[4] = 0.2; //Far width
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "charconf") == OK) {
            CV_OSD_SetCharConfig_s *pCmd = (CV_OSD_SetCharConfig_s *) &Request;

            Request.Msg = CAMCTRL_DISP_SETCHARCONFIG;
            Request.ParamSize = 6 * sizeof(int);
            if (Argc > 8) {
                pCmd->Channel = atoi(Argv[3]);
                pCmd->AttributeData = atoi(Argv[4]);
                pCmd->AttributeBits = strtoul(Argv[5], NULL, 0);
                pCmd->FontFaceIdx = atoi(Argv[6]);
                pCmd->FontPixelWidth = atoi(Argv[7]);
                pCmd->FontPixelHeight = atoi(Argv[8]);
            } else {
                pCmd->Channel = 1;
                pCmd->AttributeData = 0;
                pCmd->AttributeBits = 0;
                pCmd->FontFaceIdx = 0;
                pCmd->FontPixelWidth = 12;
                pCmd->FontPixelHeight = 12;
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "string") == OK) {
            CV_OSD_DrawString_s *pCmd = (CV_OSD_DrawString_s *) &Request;

            Request.Msg = CAMCTRL_DISP_DRAWSTRING;
            Request.ParamSize = (4 * sizeof(int)) + (12 * sizeof(char));
            if (Argc > 7) {
                pCmd->Channel = atoi(Argv[3]);
                pCmd->X = atoi(Argv[4]);
                pCmd->Y = atoi(Argv[5]);
                pCmd->Color = strtoul(Argv[6], NULL, 0);
                strncpy(pCmd->String, Argv[7], 11);
            } else {
                pCmd->Channel = 1;
                pCmd->X = 10;
                pCmd->Y = 10;
                pCmd->Color = 0xffff0000;
                strncpy(pCmd->String, "Test123456", 11);
            }

            ShowHelp = 0;
        } else if (strcmp(Argv[2], "extclut") == OK) {
            CV_OSD_SetExtCLUT_s *pCmd = (CV_OSD_SetExtCLUT_s *) &Request;

            Request.Msg = CAMCTRL_DISP_SET_EXTOSDCLUT;
            Request.ParamSize = 2 * sizeof(unsigned int);
            if (Argc > 4) {
                pCmd->Channel = atoi(Argv[3]);
                pCmd->CLUTAddr = strtoul(Argv[4], NULL, 0);
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "extbufinfo") == OK) {
            CV_OSD_SetExtBufInfo_s *pCmd = (CV_OSD_SetExtBufInfo_s *) &Request;

            Request.Msg = CAMCTRL_DISP_SET_EXTOSDBUFINFO;
            Request.ParamSize = 9 * sizeof(unsigned int);
            if (Argc > 11) {
                pCmd->Channel = atoi(Argv[3]);
                pCmd->PixelFormat = atoi(Argv[4]);
                pCmd->BufferPitch = strtoul(Argv[5], NULL, 0);
                pCmd->BufferWidth = strtoul(Argv[6], NULL, 0);
                pCmd->BufferHeight = strtoul(Argv[7], NULL, 0);
                pCmd->BufferSize = strtoul(Argv[8], NULL, 0);
                pCmd->WindowWidth = strtoul(Argv[9], NULL, 0);
                pCmd->WindowHeight = strtoul(Argv[10], NULL, 0);
                pCmd->Interlace = atoi(Argv[11]);
                ShowHelp = 0;
            }
        } else if (strcmp(Argv[2], "extupdate") == OK) {
            CV_OSD_UpdateExtBuf_s *pCmd = (CV_OSD_UpdateExtBuf_s *) &Request;

            Request.Msg = CAMCTRL_DISP_UPDATE_EXTBUF;
            Request.ParamSize = 2 * sizeof(unsigned int);
            if (Argc > 4) {
                pCmd->Channel = atoi(Argv[3]);
                pCmd->OSDBufAddr = strtoul(Argv[4], NULL, 0);
                ShowHelp = 0;
            }
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s %s [operation]\n",Argv[0],Argv[1]);
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
        PrintFunc("\t flushcs [channel] [CapSeq]. channel: Vout channel, CapSeq: Capture Sequence of the Frame to match.\n");
        PrintFunc("\t roiwin [channel] [X] [Y] [Width] [Height]\n");
        PrintFunc("\t\t channel: Vout channel, X,Y: Top-left pixel position, Width,Height: Roi window's width/height\n");
        PrintFunc("\t wrapinfo [channel] [addr] [size] [grid_w] [grid_h] [grid_num_x] [grid_num_y]\n");
        PrintFunc("\t\t channel: Vout channel, addr,size: wrap table address/size, grid_w,grid_h: grid block width/height\n");
        PrintFunc("\t\t grid_num_x,grid_num_y: total grid point on X-axis/Y-axis\n");
        PrintFunc("\t ldresult [channel] [a] [b] [c] [NearWidth] [FarWidth]\n");
        PrintFunc("\t\t channel: Vout channel, a,b,c: coefficient of Y=aX^2+bX+c, NearWidth,FarWidth: lane width of Near/Far side\n");
        PrintFunc("\t charconf [channel] [AttributeData] [AttributeBits] [FontFaceIdx] [FontPixelWidth] [FontPixelHeight]\n");
        PrintFunc("\t string [channel] [X] [Y] [Color] [string]\n");
        PrintFunc("\t\t channel: Vout channel, X,Y: position, Color:Font color, string:string to draw (Maximal 11 chars)\n");
        PrintFunc("\t extclut [channel] [CLUT addr]\n");
        PrintFunc("\t extbufinfo [channel] [PixelFormat] [BufPitch] [BufWidth] [BufHeight] [BufSize] [WinWidth] [WinHeight] [Interlace]\n");
        PrintFunc("\t extupdate [channel] [OSD buffer addr]\n");
    } else {
        Rval = CamCtrl_ProcessRequet(Request.Msg, Request.Param, Request.ParamSize);
    }

    return Rval;
}

static int CamCtrl_TestCmd_Platform(int Argc, char **Argv)
{
    int Rval = NG;
    int ShowHelp = 1;
    CV_Request_s Request;

    if (strcmp(Argv[2], "reboot") == OK) {
        Request.Msg = CAMCTRL_PLATFORM_REBOOT;
        Request.ParamSize = 0;
        ShowHelp = 0;
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s %s [operation]\n",Argv[0],Argv[1]);
        PrintFunc("operations:\n");
        PrintFunc("\t reboot\n");
    } else {
        Rval = CamCtrl_ProcessRequet(Request.Msg, Request.Param, Request.ParamSize);
    }

    return Rval;
}

int main(int Argc, char *Argv[])
{
    int Rval = NG;
    int ShowHelp = 1;

    if (Argc >= 2) {
        if (strcmp(Argv[1], "sys") == OK) {
            Rval = CamCtrl_TestCmd_Sys(Argc, Argv);
            ShowHelp = 0;
        } else if (strcmp(Argv[1], "img") == OK) {
            Rval = CamCtrl_TestCmd_Img(Argc, Argv);
            ShowHelp = 0;
        } else if (strcmp(Argv[1], "enc") == OK) {
            Rval = CamCtrl_TestCmd_Enc(Argc, Argv);
            ShowHelp = 0;
        } else if (strcmp(Argv[1], "roi") == OK) {
            Rval = CamCtrl_TestCmd_Roi(Argc, Argv);
            ShowHelp = 0;
        } else if (strcmp(Argv[1], "warp") == OK) {
            Rval = CamCtrl_TestCmd_Warp(Argc, Argv);
            ShowHelp = 0;
        } else if (strcmp(Argv[1], "disp") == OK) {
            Rval = CamCtrl_TestCmd_Disp(Argc, Argv);
            ShowHelp = 0;
        } else if (strcmp(Argv[1], "platform") == OK) {
            Rval = CamCtrl_TestCmd_Platform(Argc, Argv);
            ShowHelp = 0;
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s [FUNCTION]\n",Argv[0]);
        PrintFunc("Function:\n");
        PrintFunc("\t sys\n");
        PrintFunc("\t img\n");
        PrintFunc("\t enc\n");
        PrintFunc("\t roi\n");
        PrintFunc("\t warp\n");
        PrintFunc("\t disp\n");
        PrintFunc("\t platform\n");
    }

    return Rval;
}