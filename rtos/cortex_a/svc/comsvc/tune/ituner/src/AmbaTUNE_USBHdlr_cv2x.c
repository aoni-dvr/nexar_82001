/*
*  @file AmbaTUNEUSBHdlr_cv2x.c
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
*/

#include "AmbaDSP_ImageUtility.h"
#include "AmbaTUNE_SystemApi_cv2x.h"
#include "AmbaTUNE_USBHdlr_cv2x.h"
#include "AmbaTUNE_Parser_cv2x.h"
#include "AmbaTUNE_Rule_cv2x.h"
#include "AmbaMisraFix.h"

#ifndef READ_BUF_SIZE
#define READ_BUF_SIZE 1024U
#endif
/************************ structure define ******************************/


typedef struct {
    AMBA_ITN_TUNING_MODE_EXT_e TuningModeExt;
    TUNE_Parser_Object_t * ParserObject;
    UINT8 Is_Idsp_Load_Finish;
} USBHdlr_ITUNER_OBJ_s;

// typedef struct {
//     char* text_addr;
//     UINT32 size;
//     INT32 ptr;
// }USBHdlr_TEXT_STREAM;

typedef struct {
    INT32 (*pFunc)(const Ituner_Ext_File_Param_s * Ext_File_Param);
    Ituner_Ext_File_Type_e Ext_File_Type;
} USBHdlr_SaveDataApiList_s;

typedef struct {
    INT32 (*pFunc)(const Ituner_Ext_File_Param_s * Ext_File_Param);
    Ituner_Ext_File_Type_e Ext_File_Type;
} USBHdlr_LoadDataApiList_s;

//static USBHdlr_TEST_IS2_s ImgConfig;

static USBHdlr_ITUNER_OBJ_s ItunerObj;
// static USBHdlr_TEXT_STREAM ItunerText;


static INT32 USBHdlr_Save_Vignette_Mult(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Save Vig", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}


static INT32 USBHdlr_Save_Warp_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Save Warp", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}


static INT32 USBHdlr_Save_Cawarp_Red_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Save Ca Red", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}


static INT32 USBHdlr_Save_Cawarp_Blue_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Save Ca Blue", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}

static INT32 USBHdlr_Save_and_Load_Dummy(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Not Yet Implement", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}


static INT32 USBHdlr_Load_Warp(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Load Warp", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}
static INT32 USBHdlr_Load_Ca_Red(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Load Ca Red", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}
static INT32 USBHdlr_Load_Ca_Blue(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Load Ca Blue", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}


static INT32 USBHdlr_Load_Vig(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][WARNING] Load Vig", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}






static INT32 USBHdlr_Load_CC_Reg(const Ituner_Ext_File_Param_s *Ext_File_Param)
{

    UINT32 MisraU32;
    INT32 Rval;
    ITUNER_COLOR_CORRECTION_s Color_Correction;
    AmbaItuner_Get_ColorCorrection(&Color_Correction);

    if ((Ext_File_Param->CC_Reg_Load_Param.Address == NULL) || (Ext_File_Param->CC_Reg_Load_Param.Size == 0U)) {
        AmbaMisra_TypeCast(&MisraU32, &Ext_File_Param->CC_Reg_Load_Param.Address);
        ituner_print_str_5("[USB HDLR][ERROR] CC 3D Path = %s", Color_Correction.RegPath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[USB HDLR][ERROR] CC 3D Address = %d, Size = %d", MisraU32, Ext_File_Param->CC_ThreeD_Load_Param.Size, DC_U, DC_U, DC_U);
        Rval = USBHDLR_ERR_LOADCC;
    } else {
        Rval = USBHDLR_OK;
    }

    return Rval;
}
static INT32 USBHdlr_Save_CC_Reg(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][Debug] Save CC Reg", DC_S, DC_S, DC_S, DC_S, DC_S);
    return USBHDLR_OK;
}

static INT32 USBHdlr_Save_CC_ThreeD(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[USB HDLR][Debug] Save CC ThreeD", DC_S, DC_S, DC_S, DC_S, DC_S);
    return USBHDLR_OK;
}

static INT32 USBHdlr_Load_CC_ThreeD(const Ituner_Ext_File_Param_s *Ext_File_Param)
{

    UINT32 MisraU32;
    INT32 Rval;
    ITUNER_COLOR_CORRECTION_s Color_Correction;
    AmbaItuner_Get_ColorCorrection(&Color_Correction);

    if ((Ext_File_Param->CC_ThreeD_Load_Param.Address == NULL) || (Ext_File_Param->CC_ThreeD_Load_Param.Size == 0U)) {
        AmbaMisra_TypeCast(&MisraU32, &Ext_File_Param->CC_ThreeD_Load_Param.Address);
        ituner_print_str_5("[USB HDLR][ERROR] CC 3D Path = %s", Color_Correction.ThreeDPath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[USB HDLR][ERROR] CC 3D Address = %d, Size = %d", MisraU32, Ext_File_Param->CC_ThreeD_Load_Param.Size, DC_U, DC_U, DC_U);
        Rval = USBHDLR_ERR_LOADCC;
    } else {
        Rval = USBHDLR_OK;
    }

    return Rval;
}

INT32 USBHdlr_Init(const Ituner_Initial_Config_t *pItunerInit)
{

    INT32 ret = USBHDLR_OK;
    AmbaItuner_Config_t Ituner_Config;
    TUNE_Rule_Info_t Rule_Info;

    AmbaPrint_PrintStr5("[USBHDLR][Debug] USBHdlr_Init()",NULL,NULL,NULL,NULL,NULL);
    ituner_Opmode_Set(ITUNER_DEC);
    (void)TUNE_Parser_Create(&ItunerObj.ParserObject);
    ItunerObj.Is_Idsp_Load_Finish = 0U; //#Note: FALSE
    (void)TUNE_Rule_Get_Info(&Rule_Info);
    for (UINT32 i = 0; i < Rule_Info.RegCount; i++) {
        (void)TUNE_Parser_Add_Reg(&Rule_Info.RegList[i], ItunerObj.ParserObject);
        // if(ret != 0){
        //      AmbaPrint_PrintStr5("[USBHDLR][ERROR] Call TUNE_Parser_Add_Reg() Fail",NULL,NULL,NULL,NULL,NULL);
        //      ret = USBHDLR_ERR_ITUNERINIT;
        // }
    }

    (void)TUNE_Parser_Set_Reglist_Valid((INT64)0, ItunerObj.ParserObject);
    Ituner_Config.Hook_Func.Load_Data = USBHdlr_Load_Data;
    Ituner_Config.Hook_Func.Save_Data = USBHdlr_Save_Data;
    Ituner_Config.pWorkingBuffer = pItunerInit->pItunerBuffer;


    if(ret != AmbaItuner_Init(&Ituner_Config)) {
        AmbaPrint_PrintStr5("[USBHDLR][ERROR] Call AmbaTUNE_Init() Fail",NULL,NULL,NULL,NULL,NULL);
        ret = USBHDLR_ERR_ITUNERINIT;
    }

    return ret;
}


static INT32 USBHdlr_Parse_File(UINT8 *pDataBuffer, UINT32 ItunerSize, char* LineBuffer)
{

    UINT32 Idx = 0;
    UINT32 StrLen = 0;
    UINT32 StrIdx = 0;
    UINT32 RemainSize = ItunerSize;
    INT32 MisraI32;
    UINT32 MisraU32;
    // (void)LineBuffer;
    char* pDstStr = LineBuffer;

    //static char TempBuffer[READ_BUF_SIZE + 32U];

    while(Idx < ItunerSize) {

        if(((char)pDataBuffer[Idx]=='\n')||((char)pDataBuffer[Idx]=='\r')) {
            pDataBuffer[Idx] = (UINT8)'\0';
        }
        Idx++;
    }

    while(RemainSize > 0U) {
        StrLen = ituner_strlen((char*)&pDataBuffer[StrIdx]);

        if(StrLen > 0U) {
            UINT32 CopyRemainSize = StrLen;

            if(StrLen > READ_BUF_SIZE) {
                // ituner_print_int32_5("[XXX] StrLen %d",StrLen ,0,0,0,0);
                while(CopyRemainSize > 0U) {
                    UINT32 CopySize = 0U;

                    if(CopyRemainSize >= READ_BUF_SIZE) {
                        CopySize = READ_BUF_SIZE;
                    } else {
                        CopySize = CopyRemainSize;
                    }

                    MisraU32 = CopySize + 1U;
                    AmbaMisra_TypeCast(&MisraI32, &MisraU32);
                    ituner_strncpy(&pDstStr[StrLen - CopyRemainSize], (char*)&pDataBuffer[StrIdx + (StrLen - CopyRemainSize)], MisraI32);
                    //ituner_strncpy(&TempBuffer[0], &pDataBuffer[StrIdx + (StrLen - CopyRemainSize)], (CopySize+1));
                    //ituner_print_str_5("%s", &pDstStr[0 + (StrLen - CopyRemainSize)], DC_S, DC_S, DC_S, DC_S);
                    //ituner_print_str_5("%s", TempBuffer, DC_S, DC_S, DC_S, DC_S);
                    //ituner_print_int32_5("[AAA] Idx %d, lenth %d",(StrIdx + (StrLen - CopyRemainSize)),(CopySize),0,0,0);
                    CopyRemainSize -= CopySize;
                }
            } else {
                //ituner_print_int32_5("[USB HDLR][DEBUG] StrLen %d",StrLen,0,0,0,0);
                //ituner_strncpy(&TempBuffer[0], &pDataBuffer[StrIdx], StrLen + 1);

                MisraU32 = StrLen + 1U;
                AmbaMisra_TypeCast(&MisraI32, &MisraU32);
                ituner_strncpy(&pDstStr[0], (char*)&pDataBuffer[StrIdx], MisraI32);
                //ituner_print_str_5("%s", pDstStr, DC_S, DC_S, DC_S, DC_S);
            }

            // ituner_print_str_5("[USB HDLR][DEBUG]%s", LineBuffer, DC_S, DC_S, DC_S, DC_S);
            // AmbaPrint_Flush();
            // AmbaKAL_TaskSleep(10);

            // (void)TUNE_Parser_Parse_Line(ItunerObj.ParserObject);


            if (LineBuffer[0] != '\0') {
                (void)TUNE_Parser_Parse_Line(ItunerObj.ParserObject);
            }
        }



        StrIdx = StrIdx + StrLen + 1U;

        RemainSize = RemainSize - (StrLen+1U);
        pDstStr = LineBuffer;
    }


    return USBHDLR_OK;

}


// static UINT8 USB_READ_TEXT(void* destAddr, int size, int count, TEXT_STREAM* srcAddr)
// {
//     if( (srcAddr->ptr + size*count) >= srcAddr->size){
//         return -1;
//     }
//     else {
//         int idx = srcAddr->ptr;
//         ituner_memcpy(destAddr, &(srcAddr->text_addr[idx]), size*count);
//          // AmbaPrintColor(YELLOW, "%c", srcAddr->text_addr[idx]);
//         srcAddr->ptr = srcAddr->ptr + size*count;
//     }
//     return OK;
// }

// static int _read_line(TEXT_STREAM* text, char * Line_Buf, int Buf_Size, char **Ptr)
// {
//     int Ret = 0;
//    *Ptr = Line_Buf;
//     while(USB_READ_TEXT(*Ptr, 1, 1, text) == OK) {
//         if ((**Ptr == '\r') || (**Ptr == '\n')) {
//             **Ptr = 0;
//           //  AmbaPrintColor(YELLOW, "%s", Line_Buf);
//             Ret = 1;
//             break;
//         }
//         (*Ptr)++;
//         if ((*Ptr - Line_Buf) >= Buf_Size) {
//             //HDLR_WARF("%s() %d, Input Text String is too long", __func__, __LINE__);
//             Ret = 1;
//             break;
//         }
//     }
//     return Ret;
// }



static INT32 USBHdlr_Update_System_Info(const AMBA_IK_MODE_CFG_s *pMode)
{
    AMBA_ITN_SYSTEM_s System;
    char const *Tuning_Mode_Str = NULL;
    AMBA_IK_ABILITY_s ability= {0,0,0};
    INT32 MisraI32;
    UINT32 MisraU32;
    (void)AmbaIK_GetContextAbility(pMode, &ability);
    AmbaItuner_Get_SystemInfo(&System);
    if (ability.Pipe == AMBA_IK_PIPE_VIDEO) {
        Tuning_Mode_Str = TUNE_Rule_LU_Tuning_Mode_Str(IMG_MODE_VIDEO);

    } else {
        ituner_print_uint32_5("[TEXT HDLR][ERROR] ability->pipe: %d is invalid", ability.Pipe, DC_U, DC_U, DC_U, DC_U);
    }
    MisraU32 = sizeof(System.TuningMode);
    AmbaMisra_TypeCast(&MisraI32, &MisraU32);
    ituner_strncpy(System.TuningMode, Tuning_Mode_Str, MisraI32);
    AmbaItuner_Set_SystemInfo(&System);
    return 0;
}

static INT32 USBHdlr_Get_Param_Status(INT32 RegIdx, INT32 ParamIdx, const TUNE_REG_s *Reg)
{
    INT32 Rval = 0;
    INT32 AbsoluteParamIdx = (Reg->ParamCount * RegIdx) + ParamIdx;
    if ((Reg->Attribute==1U) || (Reg->ParamList[ParamIdx].Attribute==1U)) {
        Rval = 1;
    } else {
        if ((Reg->ParamStatus[(UINT32)AbsoluteParamIdx >> 6U] & (0x1ULL << ((UINT32)AbsoluteParamIdx & 0x3FU))) != 0U) {
            Rval = 1;
        }
    }
    return Rval;
}

static INT32 USBHdlr_Check_Param_Completeness(void)
{
    INT32 Filter_Idx;
    INT32 Param_Idx;
    INT32 Ret = 0;
    const TUNE_REG_s* Reg = NULL;
    const TUNE_PARAM_s* Param = NULL;
    AMBA_ITUNER_VALID_FILTER_t FilterStatus;
    AmbaItuner_Get_FilterStatus(&FilterStatus);
    for (Filter_Idx = 0; Filter_Idx < ItunerObj.ParserObject->RegCount; Filter_Idx++) {
        Reg = TUNE_Parser_Get_Reg(Filter_Idx, ItunerObj.ParserObject);
        if (Reg == NULL) {
            ituner_print_str_5("[USB HDLR][WARNING] call TUNE_Param_Get_Reg Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
            continue;
        }
        if (FilterStatus[Reg->Index] == 1U) {
            if (Reg->RegNum == 1) {
                for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                    if (USBHdlr_Get_Param_Status(0, Param_Idx, Reg) == 0) {
                        Param = TUNE_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
//                        if ((Reg->Index == ITUNER_FRONT_END_WB_GAIN_EXP0) && (Param_Idx == 3)) {
//                            //ignored shutter ratio under linear mode
//                            AMBA_ITN_SYSTEM_s System;
//                            TextHdlr_Get_SystemInfo(&System);
//                            if(System.NumberOfExposures == 1U){
//                                Ret = 0;
//                            } else {
//                                ituner_print_str_5("[TEXT HDLR][WARNING] Lost Param: %s.%s", Reg->Name, Param->Name, DC_S, DC_S, DC_S);
//                                Ret = -1;
//                            }
//                        } else {
                        if (Param != NULL) {
                            ituner_print_str_5("[TEXT HDLR][WARNING] Lost Param: %s.%s", Reg->Name, Param->Name, DC_S, DC_S, DC_S);
                        }
                        Ret = -1;
//                        }
                    }
                }
            } else {
                INT32 i;
                for (i = 0; i < Reg->RegNum; i++) {
                    INT32 LoseParamNum = 0;
                    for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                        if (USBHdlr_Get_Param_Status(i, Param_Idx, Reg) == 0) {
                            LoseParamNum++;
                        }
                    }
                    if (LoseParamNum != Reg->ParamCount) {
                        for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                            if (USBHdlr_Get_Param_Status(i, Param_Idx, Reg) == 0) {
                                Param = TUNE_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                                if (Param != NULL) {
                                    ituner_print_str_5("[USB HDLR][WARNING] Lost Param: %s.%s", Reg->Name, Param->Name, DC_S, DC_S, DC_S);
                                    ituner_print_int32_5("[USB HDLR][WARNING] Lost Param: %d", i, DC_I, DC_I, DC_I, DC_I);
                                }
                                Ret = -1;
                            }
                        }
                    }
                }
            }
        }
    }
    return Ret;
}

static INT32 USBHdlr_Load_Text(UINT8 *pDataBuffer, UINT32 ItunerDataSize)
{


    INT32 BufSize = 0;
    INT32 Rval = 0;
    char  *Buf = NULL;


    // int Line = 0;
    // char *Ptr;
    // UINT32 MisraI32 = 0;


    // ItunerText.text_addr = pDataBuffer;
    // ItunerText.size = ItunerDataSize ;
    // ItunerText.ptr = 0;

    ituner_Opmode_Set(ITUNER_DEC);


    if (0 != TUNE_Parser_Set_Reglist_Valid((INT64)0, ItunerObj.ParserObject)) {
        ituner_print_str_5("[USB HDLR][WARNING] call TUNE_PArser_Set_Reglist_Valid() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }


    if (0 != TUNE_Parser_Get_LineBuf(ItunerObj.ParserObject, &Buf, &BufSize)) {
        ituner_print_str_5("[USB HDLR][WARNING] call TUNE_Parser_Get_LineBuf() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }


    if (0 != USBHdlr_Parse_File(pDataBuffer, ItunerDataSize, Buf)) {
        ituner_print_str_5("[USB HDLR][WARNING] call TUNE_Parser_Get_LineBuf() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }


    // while(_read_line(&ItunerText, Buf, BufSize, &Ptr)) {
    // if (Buf[0] != '\0') {
    //         TUNE_Parser_Parse_Line(ItunerObj.ParserObject);
    //         Line++;
    //     }
    // }

    if ((0 != USBHdlr_Check_Param_Completeness())) {
        ituner_print_str_5("[USB HDLR][WARNING] call USBHdlr_Check_Param_Completeness() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }


    return Rval;
}

INT32 USBHdlr_Load_IDSP(UINT8 *pDataBufferAddr,UINT32 ItunerDataSize)
{

    INT32 Rval = USBHDLR_OK;

    ItunerObj.Is_Idsp_Load_Finish = 1U; //#Note: TRUE;


    if (0 !=USBHdlr_Load_Text(pDataBufferAddr,ItunerDataSize)) {
        ituner_print_str_5("[USB HDLR][WARNING] call USBHdlr_Load_Text Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        //(void)USBHdlr_Update_ItunerInfo();
    }


    return Rval;
}



INT32 USBHdlr_Save_Reg_Params(UINT8 *pDataBuffer)
{
    //POSIX_FILE *Fd;
    INT32 RegIndex;
    INT32 ParamIndex;
    INT32 TextDataSize = 0;
    INT32 SizeOfChar = (INT32)sizeof(char);
    char EndReturn ='\r';
    char EndLineFeed ='\n';
    char EndTab ='\t';
    UINT8 Buf[200];
    const TUNE_REG_s *Reg = NULL;
    ituner_print_str_5("[USB HDLR][WARNING] %s()", "USBHdlr_Save_Reg_Params", DC_S, DC_S, DC_S, DC_S);

    for (RegIndex = 0; RegIndex < ItunerObj.ParserObject->RegCount; RegIndex++) {

        Reg = TUNE_Parser_Get_Reg(RegIndex, ItunerObj.ParserObject);

        if (Reg == NULL) {
            continue;
        }


        for (ParamIndex = 0; ParamIndex < Reg->ParamCount; ParamIndex++) {
            INT32 Length=0;
            INT32 StrLen = 0;
            UINT8 CharValCnt[16] = {0};
            UINT8 CharValType[16] = {0};
            if(0U != usbhdlr_memcpy(Buf,Reg->Name,ituner_strlen(Reg->Name))) {
                ituner_print_str_5("[USB HDLR][ERROR] %s() Reg->Name usbhdlr_memcpy error", __func__, DC_S, DC_S, DC_S, DC_S);
            }

            StrLen = (INT32)ituner_strlen(Reg->Name);
            Length = Length + StrLen;
            Buf[Length] = (UINT8)'.';
            Length +=1;




            if(0U != usbhdlr_memcpy(&Buf[Length],Reg->ParamList[ParamIndex].Name,ituner_strlen(Reg->ParamList[ParamIndex].Name))) {
                ituner_print_str_5("[USB HDLR][ERROR] %s() Reg->ParamList[ParamIndex].Name usbhdlr_memcpy error", __func__, DC_S, DC_S, DC_S, DC_S);
            }
            StrLen = (INT32)ituner_strlen(Reg->ParamList[ParamIndex].Name);
            Length = Length + StrLen;
            Buf[Length] = (UINT8)' ';
            Length += 1;




            (void)AmbaUtility_UInt32ToStr((char*)CharValCnt, 16, (UINT32)Reg->ParamList[ParamIndex].ValCount, 10U);
            if(0U != usbhdlr_memcpy(&Buf[Length],CharValCnt,ituner_strlen((char*)CharValCnt))) {
                ituner_print_str_5("[USB HDLR][ERROR] %s() CharValCnt usbhdlr_memcpy error", __func__, DC_S, DC_S, DC_S, DC_S);
            }
            StrLen = (INT32)ituner_strlen((char*)CharValCnt);
            Length = Length + StrLen;
            Buf[Length] = (UINT8)' ';
            Length+=1;


            (void)AmbaUtility_UInt32ToStr((char*)CharValType, 16, (UINT32)Reg->ParamList[ParamIndex].ValType, 10U);
            if(0U != usbhdlr_memcpy(&Buf[Length],CharValType,ituner_strlen((char*)CharValType))) {
                ituner_print_str_5("[USB HDLR][ERROR] %s() CharValType usbhdlr_memcpy error", __func__, DC_S, DC_S, DC_S, DC_S);
            }
            StrLen = (INT32)ituner_strlen((char*)CharValType);
            Length = Length + StrLen;


            //usbhdlr_print_str_5("%s.%s %s %s",Reg->Name,Reg->ParamList[ParamIndex].Name, CharValCnt, CharValType, NULL);

            if(0U != usbhdlr_memcpy(&pDataBuffer[TextDataSize],Buf,ituner_strlen((char*)Buf))) {
                ituner_print_str_5("[USB HDLR][ERROR] %s() Buf usbhdlr_memcpy error", __func__, DC_S, DC_S, DC_S, DC_S);
            }
            TextDataSize = TextDataSize + Length;
            //*(pDataBuffer + TextDataSize)= (UINT8)EndReturn;
            pDataBuffer[TextDataSize] = (UINT8)EndLineFeed;
            TextDataSize = TextDataSize + SizeOfChar;
            if(0U != usbhdlr_memset(Buf,0,200)) {
                ituner_print_str_5("[USB HDLR][ERROR] %s() usbhdlr_memset error", __func__, DC_S, DC_S, DC_S, DC_S);
            }

        }

    }



    //****endmark of ituner file**************//
    pDataBuffer[TextDataSize] = (UINT8) EndTab;
    pDataBuffer[TextDataSize + SizeOfChar]   = (UINT8)EndTab;
    pDataBuffer[TextDataSize + (2*SizeOfChar)] = (UINT8)EndTab;
    pDataBuffer[TextDataSize + (3*SizeOfChar)] = (UINT8)EndReturn;
    pDataBuffer[TextDataSize + (4*SizeOfChar)] = (UINT8)EndLineFeed;

    return USBHDLR_OK;

}



static INT32 USBHdlr_Save_Ituner(const AMBA_IK_MODE_CFG_s *pMode, UINT8 *pDataBuffer)
{
    INT32 Rval = USBHDLR_OK;

    INT32 RegIndex;
    INT32 RegNumIdx;
    INT32 ParamIndex;

    INT32 BufSize = 0;
    INT32 TextDataSize = 0;
    INT32 SizeOfChar = (INT32)sizeof(char);

    char EndReturn='\r';
    char EndLineFeed='\n';
    char EndTab='\t';
    char *Buf = NULL;

    const TUNE_REG_s *Reg = NULL;
    // UINT8 EndReturn='\r';
    // UINT8 EndLineFeed='\n';
    // UINT8 EndTab='\t';
    // UINT8 Buf[200];


    AMBA_ITUNER_VALID_FILTER_t Filter_Status;
    (void)pDataBuffer;


    if (0 != USBHdlr_Update_System_Info(pMode)) {
        ituner_print_str_5("[USB HDLR][WARNING] %s()  USBHdlr_Update_System_Info() Fail", "USBHdlr_Save_Ituner", DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }


    if (0 != AmbaItuner_Refresh(pMode)) {
        ituner_print_str_5("[USB HDLR][WARNING] %s() call AmbaItuner_Refresh() Fail", "USBHdlr_Save_Ituner", DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }


    if(0!=TUNE_Parser_Set_Reglist_Valid((INT64)SKIP_VALID_CHECK, ItunerObj.ParserObject)) {
        ituner_print_str_5("[USB HDLR][WARNING] %s() call  TUNE_Parser_Set_Reglist_Valid() Fail", "USBHdlr_Save_Ituner", DC_S, DC_S, DC_S, DC_S);
        Rval = -1;

    }


    if( 0 !=TUNE_Parser_Get_LineBuf(ItunerObj.ParserObject, &Buf, &BufSize)) {
        ituner_print_str_5("[USB HDLR][WARNING] %s() call  TUNE_Parser_Get_LineBuf() Fail", "USBHdlr_Save_Ituner", DC_S, DC_S, DC_S, DC_S);
        Rval = -1;

    }

    ituner_Opmode_Set(ITUNER_ENC);


    AmbaItuner_Get_FilterStatus(&Filter_Status);

    for (RegIndex = 0; RegIndex < ItunerObj.ParserObject->RegCount; RegIndex++) {
        Reg = TUNE_Parser_Get_Reg(RegIndex, ItunerObj.ParserObject);
        if (Reg == NULL) {
            ituner_print_str_5("[USB HDLR][ERROR] %s()  Reg is NULL", "USBHdlr_Save_Ituner", DC_S, DC_S, DC_S, DC_S);
            continue;
        }
        if (Filter_Status[Reg->Index] == 0U) {
            continue;
        }

        for (RegNumIdx = 0; RegNumIdx < Reg->RegNum; RegNumIdx++) { // e.g. amp[0] amp[1] .. amp[3]
            for (ParamIndex = 0; ParamIndex < Reg->ParamCount; ParamIndex++) {
                if (0 != TUNE_Parser_Generate_Line(RegIndex,RegNumIdx, ParamIndex, ItunerObj.ParserObject)) {
                    continue;
                }

                //ituner_print_str_5("%s", Buf, NULL, NULL, NULL, NULL);
                if(0U != usbhdlr_memcpy(&pDataBuffer[TextDataSize],Buf,ituner_strlen(Buf))) {
                    ituner_print_str_5("[USB HDLR][ERROR] %s() Buf usbhdlr_memcpy error", __func__, DC_S, DC_S, DC_S, DC_S);
                }
                TextDataSize = TextDataSize + (INT32)ituner_strlen(Buf);
                pDataBuffer[TextDataSize - SizeOfChar] = (UINT8)EndReturn;
                pDataBuffer[TextDataSize] = (UINT8)EndLineFeed;
                TextDataSize = TextDataSize + SizeOfChar;

            }

        }
    }
    //****endmark of ituner file**************//


    pDataBuffer[TextDataSize] = (UINT8)EndTab;
    pDataBuffer[TextDataSize + (SizeOfChar)]     = (UINT8)EndTab;
    pDataBuffer[TextDataSize + (2*SizeOfChar)]   = (UINT8)EndTab;
    pDataBuffer[TextDataSize + (3*SizeOfChar)]   = (UINT8)EndReturn;
    pDataBuffer[TextDataSize + (4*SizeOfChar)]   = (UINT8)EndLineFeed;


    return Rval;

}

INT32 USBHdlr_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, UINT8 *pDataBufferAddr)
{

    INT32 Rval = 0;
    //ituner_print_str_5("[USB HDLR][WARNING] call USBHdlr_Save_IDSP", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (0 != USBHdlr_Save_Ituner(pMode, pDataBufferAddr)) {
        ituner_print_str_5("[USB HDLR][WARNING] call USBHdlr_Save_Ituner Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = USBHDLR_ERR_SAVETEXT;
    } else {

        Rval = USBHDLR_OK;
    }

    return Rval;
}




INT32 USBHdlr_Execute_IDSP(const AMBA_IK_MODE_CFG_s *pMode)
{
    INT32 Rval;
    Rval = AmbaItuner_Execute(pMode);
    return Rval;
}


INT32 USBHdlr_Get_SystemInfo(AMBA_ITN_SYSTEM_s *System)
{
    AmbaItuner_Get_SystemInfo(System);
    return 0;
}

INT32 USBHdlr_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *System)
{
    AmbaItuner_Set_SystemInfo(System);
    return 0;
}

#if 0
INT32 USBHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo)
{
    AmbaItuner_Get_AeInfo(AeInfo);
    return 0;
}

INT32 USBHdlr_Set_AeInfo(const ITUNER_AE_INFO_s *AeInfo)
{
    AmbaItuner_Set_AeInfo(AeInfo);
    return 0;
}

INT32 USBHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    AmbaItuner_Get_WbSimInfo(WbSimInfo);
    return 0;
}

INT32 USBHdlr_Set_WbSimInfo(const ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    AmbaItuner_Set_WbSimInfo(WbSimInfo);
    return 0;
}
#endif

#define LoadDataApiList_Count 25
INT32 USBHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s* Ext_File_Param)
{
    INT32 i, Rval = -1;
    const USBHdlr_LoadDataApiList_s LoadDataApiList[LoadDataApiList_Count] = {
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_FPN_MAP},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_FPN_MAP},
        {USBHdlr_Load_Vig, FILE_EXT_VIGNETTE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_VIGNETTE_MULT},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_VIGNETTE_SHIFT},
        {USBHdlr_Load_Warp, FILE_EXT_WARP_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_WARP_HORIZONTAL_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_WARP_VERTICAL_TABLE},
        {USBHdlr_Load_Ca_Red, FILE_EXT_CA_RED_TABLE},
        {USBHdlr_Load_Ca_Blue, FILE_EXT_CA_BLUE_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_CA_RED_HORIZONTAL_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_CA_RED_VERTICAL_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_CA_BLUE_HORIZONTAL_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_CA_BLUE_VERTICAL_TABLE},
        {USBHdlr_Load_CC_Reg, FILE_EXT_CC_REG},
        {USBHdlr_Load_CC_ThreeD, FILE_EXT_CC_THREED},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_ASF_THREE_D_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_FIRST_SHARPEN},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_ASF},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_RAW},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_JPG},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_YUV},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_HDR_ALPHA_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_HDR_LINEAR_TABLE},
        {USBHdlr_Save_and_Load_Dummy, FILE_EXT_HDR_CONTRAST_ENHANCE_TABLE},
    };
    for (i = 0; i < LoadDataApiList_Count; i++) {
        if (LoadDataApiList[i].Ext_File_Type == Ext_File_Type) {
            Rval = LoadDataApiList[i].pFunc(Ext_File_Param);
            break;
        }
    }
    if (Rval == -1) {
        ituner_print_uint32_5("[USB HDLR][ERROR] Invalid Ext_File_Type %d", (UINT32)Ext_File_Type, DC_U, DC_U, DC_U, DC_U);
        Rval = -1;
    }
    return Rval;
}


#define SaveDataApiList_Count 23
INT32 USBHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s* Ext_File_Param)
{
    INT32 i, Rval = -1;
    USBHdlr_SaveDataApiList_s SaveDataApiList[SaveDataApiList_Count];
    SaveDataApiList[0].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[0].Ext_File_Type  = FILE_EXT_FPN_MAP;
    SaveDataApiList[1].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[1].Ext_File_Type  = FILE_EXT_INTERNAL_FPN_MAP;
    SaveDataApiList[2].pFunc = USBHdlr_Save_Vignette_Mult;
    SaveDataApiList[2].Ext_File_Type  = FILE_EXT_VIGNETTE;
    SaveDataApiList[3].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[3].Ext_File_Type  = FILE_EXT_INTERNAL_VIGNETTE_MULT;
    SaveDataApiList[4].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[4].Ext_File_Type  = FILE_EXT_INTERNAL_VIGNETTE_SHIFT;
    SaveDataApiList[5].pFunc = USBHdlr_Save_Warp_Table;
    SaveDataApiList[5].Ext_File_Type  = FILE_EXT_WARP_TABLE;
    SaveDataApiList[6].pFunc = USBHdlr_Save_Cawarp_Red_Table;
    SaveDataApiList[6].Ext_File_Type  = FILE_EXT_CA_RED_TABLE;
    SaveDataApiList[7].pFunc = USBHdlr_Save_Cawarp_Blue_Table;
    SaveDataApiList[7].Ext_File_Type  = FILE_EXT_CA_BLUE_TABLE;
    SaveDataApiList[8].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[8].Ext_File_Type  = FILE_EXT_INTERNAL_WARP_HORIZONTAL_TABLE;
    SaveDataApiList[9].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[9].Ext_File_Type  = FILE_EXT_INTERNAL_WARP_VERTICAL_TABLE;
    SaveDataApiList[10].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[10].Ext_File_Type  = FILE_EXT_INTERNAL_CA_RED_HORIZONTAL_TABLE;
    SaveDataApiList[11].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[11].Ext_File_Type  = FILE_EXT_INTERNAL_CA_RED_VERTICAL_TABLE;
    SaveDataApiList[12].pFunc = USBHdlr_Save_CC_Reg;
    SaveDataApiList[12].Ext_File_Type  = FILE_EXT_CC_REG;
    SaveDataApiList[13].pFunc = USBHdlr_Save_CC_ThreeD;
    SaveDataApiList[13].Ext_File_Type  = FILE_EXT_CC_THREED;
    SaveDataApiList[14].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[14].Ext_File_Type  = FILE_EXT_FIRST_SHARPEN;
    SaveDataApiList[15].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[15].Ext_File_Type  = FILE_EXT_ASF;
    SaveDataApiList[16].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[16].Ext_File_Type  = FILE_EXT_RAW;
    SaveDataApiList[17].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[17].Ext_File_Type  = FILE_EXT_YUV;
    SaveDataApiList[18].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[18].Ext_File_Type  = FILE_EXT_JPG;
    SaveDataApiList[19].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[19].Ext_File_Type  = FILE_EXT_HDR_ALPHA_TABLE;
    SaveDataApiList[20].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[20].Ext_File_Type  = FILE_EXT_HDR_LINEAR_TABLE;
    SaveDataApiList[21].pFunc = USBHdlr_Save_and_Load_Dummy;
    SaveDataApiList[21].Ext_File_Type  = FILE_EXT_HDR_CONTRAST_ENHANCE_TABLE;

    for (i = 0; i < SaveDataApiList_Count; i++) {
        if (SaveDataApiList[i].Ext_File_Type == Ext_File_Type) {
            Rval = SaveDataApiList[i].pFunc(Ext_File_Param);
            break;
        }
    }
    if (Rval == -1) {
        ituner_print_uint32_5("[USB HDLR][ERROR] Invalid Ext_File_Type %d", (UINT32)Ext_File_Type, DC_U, DC_U, DC_U, DC_U);
        Rval = -1;
    }
    return Rval;
}



// INT32 USBHdlr_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, TUNE_USB_Save_Param_s *USBbuffer)
// {

//     //USBHdlr_AmageSaveIDSP(USBbuffer->Buffer, USBbuffer->Offset, USBbuffer->LengthRequested, USBbuffer->ActualLength);
//     return 0;
// }

// INT32 USBHdlr_Load_IDSP(TUNE_USB_Load_Param_s *USBbuffer)
// {
//     USBHdlr_AmageLoadIDSP(USBbuffer->Buffer, USBbuffer->Offset, USBbuffer->Length);
//     return 0;
// }
