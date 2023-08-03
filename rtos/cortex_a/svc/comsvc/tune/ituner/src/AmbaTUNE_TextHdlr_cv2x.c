/*
*  @file AmbaTUNE_TextHdlr_cv2x.c
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
#include "AmbaTUNE_TextHdlr_cv2x.h"
#include "AmbaTUNE_Parser_cv2x.h"
#include "AmbaTUNE_Rule_cv2x.h"
#include "AmbaCache.h"

typedef struct {
    AMBA_ITN_TUNING_MODE_EXT_e TuningModeExt;
    TUNE_Parser_Object_t * ParserObject;
    UINT8 Is_Idsp_Load_Finish;
} ITUNER_OBJ_s;


//static INT32 TextHdlr_Add_Reg(TUNE_REG_s *pReg);

static INT32 TextHdlr_Check_Param_Completeness(void);
static INT32 TextHdlr_Save_Ext_File(const char *FileName, UINT32 Size, void *Buf);
static INT32 TextHdlr_load_Ext_file(const char *FileName, UINT32 Size, void *Buf);

#if 0
char TextHdlr_Get_Driver_Letter(void);
char TextHdlr_Get_Driver_Letter(void)
{
    ituner_print_str_5("[TEXT HDLR][DEBUG] Not Yet Ready", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 'c';
}
#endif
static ITUNER_OBJ_s Ituner;

static char Ituner_File_Path[128];

static UINT32 save_line(AMBA_FS_FILE *Fid, char * LineBuf)
{
    return (ituner_fwrite(LineBuf, 1, ituner_strlen(LineBuf), Fid));
}
#define READ_BUF_SIZE 1024
static INT32 TextHdlr_Parse_File(const char *FileName, char* LineBuffer)
{
    static char ReadBuffer[(UINT32)READ_BUF_SIZE + 32U] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    INT32 Line = 0, Rval = 0;
//    static struct stat FileStat;
    AMBA_FS_FILE_INFO_s FileInfo;
    AMBA_FS_FILE *Fid;
    char* pReadBuf;
    char* pStartOfStr;
    char* pDstStr = LineBuffer;
    INT32 ReadSize;
    UINT32 len = 0;
    INT32 i;
    UINT32 MisraU32_1, MisraU32_2;
    UINTPTR MisraUPtr;
    const char *pMisraChar_0, *pMisraChar_1;
    INT32 MisraI32_BreakYes;
    UINTPTR MisraUPtr_0, MisraUPtr_1, MisraUPtr_2, MisraUPtr_3, MisraUPtr_4;
    FileInfo.Size = 0u;//misra 9.1
    (void)ituner_memcpy(&MisraUPtr, &ReadBuffer, sizeof(UINTPTR));
    pReadBuf = &ReadBuffer[32U - (MisraUPtr & 0x1FU)];
    pStartOfStr = pReadBuf;

    (void)ituner_memset(ReadBuffer, 0x0, READ_BUF_SIZE + 32);

    Fid = ituner_fopen(FileName, "rb");
    if (Fid == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_fopen(%s) Fail", FileName, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        (void)AmbaFS_GetFileInfo(FileName, &FileInfo);
        (void)ituner_memset(ReadBuffer, 0x0, sizeof(ReadBuffer));
        ReadSize = (INT32)FileInfo.Size;

        if (ReadSize > READ_BUF_SIZE) {
            MisraU32_2 = (UINT32)READ_BUF_SIZE;
        } else {
            MisraU32_2 = (UINT32)ReadSize;
        }
        MisraU32_1 = ituner_fread(pReadBuf, MisraU32_2, 1U, Fid);
        while ((ReadSize > 0) && (MisraU32_1 > 0U)) {
            for (i = 0; i < ((ReadSize > READ_BUF_SIZE) ? READ_BUF_SIZE : ReadSize); i++) {
                if ((pReadBuf[i] == '\n') || (pReadBuf[i] == '\r')) {
                    pReadBuf[i] = '\0';
                }
            }
            MisraI32_BreakYes = 0;
            while (MisraI32_BreakYes == 0) {
                len = ituner_strlen(pStartOfStr);
                (void)ituner_memcpy(&MisraUPtr_0, &pStartOfStr, sizeof(UINTPTR));
                (void)ituner_memcpy(&MisraUPtr_1, &pReadBuf, sizeof(UINTPTR));
                pMisraChar_0 = &pStartOfStr[len];
                (void)ituner_memcpy(&MisraUPtr_2, &pMisraChar_0, sizeof(UINTPTR));
                pMisraChar_1 = &pReadBuf[READ_BUF_SIZE];
                (void)ituner_memcpy(&MisraUPtr_3, &pMisraChar_1, sizeof(UINTPTR));
                if (MisraUPtr_2 >= MisraUPtr_3) {
                    MisraUPtr_4 = ((UINTPTR)READ_BUF_SIZE - (MisraUPtr_0 - MisraUPtr_1))+1UL;
                    ituner_strncpy(pDstStr, pStartOfStr, (INT32)(UINT32)MisraUPtr_4);
                    pDstStr = &pDstStr[(UINTPTR)READ_BUF_SIZE - (MisraUPtr_0 - MisraUPtr_1)];
                    pStartOfStr = pReadBuf;
                    // Feed ReadBuf Again
                    MisraI32_BreakYes = 1;
                } else {
                    MisraUPtr_4 = ((UINTPTR)READ_BUF_SIZE - (MisraUPtr_0 - MisraUPtr_1))+1UL;
                    ituner_strncpy(pDstStr, pStartOfStr, (INT32)(UINT32)MisraUPtr_4);
                    pStartOfStr = &pStartOfStr[len + 1U];
                    //ituner_print_str_5("[TEXT HDLR][DEBUG] LineBuffer:%s", LineBuffer, DC_S, DC_S, DC_S, DC_S);
                    //                AmbaKAL_TaskSleep(10);
                    if (LineBuffer[0] != '\0') {
                        (void)TUNE_Parser_Parse_Line(Ituner.ParserObject);
                        Line++;
                    }
                    //(void)ituner_memcpy(&MisraUPtr_0, &pStartOfStr, sizeof(UINTPTR));
                    if ((MisraUPtr_0 - MisraUPtr_1) >= (UINTPTR)(UINT32)ReadSize) {
                        MisraI32_BreakYes = 1;
                    }
                    pDstStr = LineBuffer;
                }
            }
            ReadSize -= READ_BUF_SIZE;

            if (ReadSize > READ_BUF_SIZE) {
                MisraU32_2 = (UINT32)READ_BUF_SIZE;
            } else {
                MisraU32_2 = (UINT32)ReadSize;
            }
            MisraU32_1 = ituner_fread(pReadBuf, MisraU32_2, 1U, Fid);
        }
        (void)ituner_fclose(Fid);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Text(const char *FileName)
{
    char *Buf = NULL;
    INT32 BufSize = 0, Rval = 0, MisraI32;
    char AscFname[128];
    UINT32 MisraU32;

    if (FileName==NULL) {
        // FIXME
    }

    (void)ituner_memset(AscFname, 0x0, sizeof(AscFname));
    MisraU32 = sizeof(AscFname);
    (void)ituner_memcpy(&MisraI32, &MisraU32, sizeof(INT32));
    ituner_strncpy(AscFname, FileName, MisraI32);
    ituner_print_str_5("[TEXT HDLR][DEBUG] ---------------------", DC_S, DC_S, DC_S, DC_S, DC_S);
    ituner_print_int32_5("[TEXT HDLR][DEBUG] iTuner ver. %d.%d", ITUNER_VER_MAJOR, ITUNER_VER_MINOR, DC_I, DC_I, DC_I);
    ituner_print_str_5("[TEXT HDLR][DEBUG] ---------------------", DC_S, DC_S, DC_S, DC_S, DC_S);
    ituner_print_str_5("[TEXT HDLR][DEBUG] file to load:%s", AscFname, DC_S, DC_S, DC_S, DC_S);
    ituner_Opmode_Set(ITUNER_DEC);
    if (0 != TUNE_Parser_Set_Reglist_Valid(0, Ituner.ParserObject)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TUNE_Parser_Set_Reglist_Valid() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    MisraI32 = TUNE_Parser_Get_LineBuf(Ituner.ParserObject, &Buf, &BufSize);
    if ((Rval == 0) && (0 != MisraI32)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TUNE_Parser_Get_LineBuf() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    MisraI32 = TextHdlr_Parse_File(AscFname, Buf);
    if ((Rval == 0) && (0 != MisraI32)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Parse_File(%s) Fail", AscFname, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    MisraI32 = TextHdlr_Check_Param_Completeness();
    if ((Rval == 0) && (0 != MisraI32)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Check_Param_Completeness() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    return Rval;
}

INT32 TextHdlr_Init(const Ituner_Initial_Config_t *pItunerInit)
{
    UINT32 i;
    AmbaItuner_Config_t Ituner_Config;
    TUNE_Rule_Info_t Rule_Info;
    ituner_Opmode_Set(ITUNER_DEC);
    (void)TUNE_Parser_Create(&Ituner.ParserObject);
    Ituner.Is_Idsp_Load_Finish = 0U; //#Note: FALSE
    (void)TUNE_Rule_Get_Info(&Rule_Info);
    for (i = 0; i < Rule_Info.RegCount; i++) {
        (void)TUNE_Parser_Add_Reg(&Rule_Info.RegList[i], Ituner.ParserObject);
    }

    (void)TUNE_Parser_Set_Reglist_Valid(0, Ituner.ParserObject);
    Ituner_Config.Hook_Func.Load_Data = TextHdlr_Load_Data;
    Ituner_Config.Hook_Func.Save_Data = TextHdlr_Save_Data;
    Ituner_Config.pWorkingBuffer = pItunerInit->pItunerBuffer;
    return AmbaItuner_Init(&Ituner_Config);
}

INT32 TextHdlr_Load_IDSP(const char *Filepath)
{
    INT32 Rval = 0;
    Ituner.Is_Idsp_Load_Finish = 1U; //#Note: TRUE
    if (0 != TextHdlr_Load_Text(Filepath)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Load_Text(%s) Fail", Filepath, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        //(void)TextHdlr_Update_ItunerInfo();
    }
    return Rval;
}

static INT32 TextHdlr_Get_Param_Status(INT32 RegIdx, INT32 ParamIdx, const TUNE_REG_s *Reg)
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

static INT32 TextHdlr_Check_Param_Completeness(void)
{
    INT32 Filter_Idx;
    INT32 Param_Idx;
    INT32 Ret = 0;
    const TUNE_REG_s* Reg = NULL;
    const TUNE_PARAM_s* Param = NULL;
    AMBA_ITUNER_VALID_FILTER_t FilterStatus;
    AmbaItuner_Get_FilterStatus(&FilterStatus);
    for (Filter_Idx = 0; Filter_Idx < Ituner.ParserObject->RegCount; Filter_Idx++) {
        Reg = TUNE_Parser_Get_Reg(Filter_Idx, Ituner.ParserObject);
        if (Reg == NULL) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TUNE_Param_Get_Reg Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
            continue;
        }
        if (FilterStatus[Reg->Index] == 1U) {
            if (Reg->RegNum == 1) {
                for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                    if (TextHdlr_Get_Param_Status(0, Param_Idx, Reg) == 0) {
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
                        if (TextHdlr_Get_Param_Status(i, Param_Idx, Reg) == 0) {
                            LoseParamNum++;
                        }
                    }
                    if (LoseParamNum != Reg->ParamCount) {
                        for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                            if (TextHdlr_Get_Param_Status(i, Param_Idx, Reg) == 0) {
                                Param = TUNE_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                                if (Param != NULL) {
                                    ituner_print_str_5("[TEXT HDLR][WARNING] Lost Param: %s.%s", Reg->Name, Param->Name, DC_S, DC_S, DC_S);
                                    ituner_print_int32_5("[TEXT HDLR][WARNING] Lost Param: %d", i, DC_I, DC_I, DC_I, DC_I);
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

INT32 TextHdlr_Execute_IDSP(const AMBA_IK_MODE_CFG_s *pMode)
{
    INT32 Rval;
    Rval = AmbaItuner_Execute(pMode);
    return Rval;
}

INT32 TextHdlr_Get_SystemInfo(AMBA_ITN_SYSTEM_s *System)
{
    AmbaItuner_Get_SystemInfo(System);
    return 0;
}

INT32 TextHdlr_Set_SystemInfo(const AMBA_ITN_SYSTEM_s *System)
{
    AmbaItuner_Set_SystemInfo(System);
    return 0;
}

INT32 TextHdlr_Get_AeInfo(ITUNER_AE_INFO_s *AeInfo)
{
    AmbaItuner_Get_AeInfo(AeInfo);
    return 0;
}

INT32 TextHdlr_Set_AeInfo(const ITUNER_AE_INFO_s *AeInfo)
{
    AmbaItuner_Set_AeInfo(AeInfo);
    return 0;
}

INT32 TextHdlr_Get_WbSimInfo(ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    AmbaItuner_Get_WbSimInfo(WbSimInfo);
    return 0;
}

INT32 TextHdlr_Set_WbSimInfo(const ITUNER_WB_SIM_INFO_s *WbSimInfo)
{
    AmbaItuner_Set_WbSimInfo(WbSimInfo);
    return 0;
}

static INT32 TextHdlr_Update_System_Info(const AMBA_IK_MODE_CFG_s *pMode)
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
    } else if (ability.Pipe== AMBA_IK_PIPE_STILL) {
        if (ability.StillPipe == AMBA_IK_STILL_HISO) {
            Tuning_Mode_Str = TUNE_Rule_LU_Tuning_Mode_Str(IMG_MODE_HIGH_ISO_STILL);
        } else {
            /*misrac 15.7*/;
        }
    } else {
        /*misrac 15.7*/;
    }
    MisraU32 = sizeof(System.TuningMode);
    (void)ituner_memcpy(&MisraI32, &MisraU32, sizeof(INT32));
    ituner_strncpy(System.TuningMode, Tuning_Mode_Str, MisraI32);
    AmbaItuner_Set_SystemInfo(&System);
    return 0;
}


static INT32 TextHdlr_Get_Ext_File_Path(Ituner_Ext_File_Type_e Ext_File_Type, char *Ext_File_path)
{
    INT32 Rval = 0, MisraI32 = 0;
    char *Ext_pos = NULL, MisraChar;
    if (ituner_strlen(Ituner_File_Path) < 7U) {
        ituner_print_str_5("[TEXT HDLR][ERROR] Ituner_File_Path = %s, it should not happened!", Ext_File_path, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        ituner_strncpy(Ext_File_path, Ituner_File_Path, 128);
        MisraChar = '.';
        (void)ituner_memcpy(&MisraI32, &MisraChar, sizeof(char));
        Ext_pos = ituner_strrchr(Ext_File_path, MisraI32);
    }
    if (Ext_pos == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] Ituner_File_Path = %s, it should not happened!", Ext_File_path, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        *Ext_pos = '\0';
        switch (Ext_File_Type) {
        case FILE_EXT_FPN_MAP:
            (void)ituner_strncat(Ext_File_path, "_Fpn_Map.bin", APP_MAX_FN_SIZE);
            break;
        case FILE_EXT_VIGNETTE:
            (void)ituner_strncat(Ext_File_path, "_Vignette.bin", APP_MAX_FN_SIZE);
            break;
        case FILE_EXT_WARP_TABLE:
            (void)ituner_strncat(Ext_File_path, "_Warp_Table.bin", APP_MAX_FN_SIZE);
            break;
        case FILE_EXT_CC_REG:
            (void)ituner_strncat(Ext_File_path, "_CC_Reg.bin", APP_MAX_FN_SIZE);
            break;
        case FILE_EXT_CC_THREED:
            (void)ituner_strncat(Ext_File_path, "_CC_3d.bin", APP_MAX_FN_SIZE);
            break;
        case FILE_EXT_CA_RED_TABLE:
            (void)ituner_strncat(Ext_File_path, "_Cawarp_red_Table.bin", APP_MAX_FN_SIZE);
            break;
        case FILE_EXT_CA_BLUE_TABLE:
            (void)ituner_strncat(Ext_File_path, "_Cawarp_blue_Table.bin", APP_MAX_FN_SIZE);
            break;
        default:
            ituner_print_uint32_5("[TEXT HDLR][ERROR] Invalid Ext_File_Type = %d", (UINT32)Ext_File_Type, DC_U, DC_U, DC_U, DC_U);
            Rval = -1;
            break;
        }
    }
    return Rval;
}

static INT32 TextHdlr_Save_Raw(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0;
    AMBA_ITN_SYSTEM_s System;
    AMBA_FS_FILE *Fd = NULL;
    UINTPTR MisraUptr;

    AmbaItuner_Get_SystemInfo(&System);
    if ((ituner_strlen(System.RawPath) < 8U) || (Ext_File_Param->Raw_Save_Param.Address == NULL) || (Ext_File_Param->Raw_Save_Param.Size == 0U)) {
        (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Raw_Save_Param.Address, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][WARNING] %s(), RawPath: %s",
                           __func__, System.RawPath, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING] %d, Addr: %d, Size: %d",
                              __LINE__, MisraUptr, Ext_File_Param->Raw_Save_Param.Size, DC_U, DC_U);
        Rval = -1;
    } else {
        Fd = ituner_fopen(System.RawPath, "w");
    }
    if (Fd == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call ituner_fopen(%s) Fail", System.RawPath, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        (void)ituner_fwrite(Ext_File_Param->Raw_Save_Param.Address, Ext_File_Param->Raw_Save_Param.Size, 0, Fd);
        (void)ituner_fclose(Fd);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Raw(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Ret = 0;
    AMBA_ITN_SYSTEM_s System;
    AMBA_FS_FILE *Fd = NULL;
    UINT32 InputPitch;
    INT32 i;
    UINT32 Offset = 0;
    UINT8 *pMisraU8;
    UINTPTR MisraUptr;

    AmbaItuner_Get_SystemInfo(&System);
    if ((ituner_strlen(System.RawPath) < 8U) || (Ext_File_Param->Raw_Load_Param.Address == NULL) || (Ext_File_Param->Raw_Load_Param.Max_Size == 0U)) {
        (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Raw_Load_Param.Address, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][WARNING] %s(), RawPath: %s",
                           __func__, System.RawPath, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][WARNING] %d, Addr: %d, Max_Size: %d",
                              __LINE__, MisraUptr, Ext_File_Param->Raw_Load_Param.Max_Size, DC_U, DC_U);
        Ret = -1;
    } else {
        Fd = ituner_fopen(System.RawPath, "r");
    }
    if (Fd == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_fopen(%s) Fail", System.RawPath, DC_S, DC_S, DC_S, DC_S);
        Ret = -1;
    } else {
        InputPitch = ((System.RawPitch + 31U) >> 5) << 5;

        for (i = 0; i < (INT32)System.RawHeight; i++) {
            if ((Offset + System.RawPitch) > Ext_File_Param->Raw_Load_Param.Max_Size) {
                ituner_print_uint32_5("[TEXT HDLR][WARNING] %d, Load Size Buffer Full, Max_Size = %d, i = %d, InputPitch = %d",
                                      __LINE__, Ext_File_Param->Raw_Load_Param.Max_Size, (UINT32)i, InputPitch, DC_U);
                Ret = -1;
            }
            (void)ituner_memcpy(&pMisraU8, &Ext_File_Param->Raw_Load_Param.Address, sizeof(char*));
            (void)ituner_fread(&(pMisraU8)[Offset], System.RawPitch, 1, Fd);

            Offset += InputPitch;
        }
        (void)ituner_fclose(Fd);
    }
    return Ret;
}

static INT32 TextHldr_Load_Vignette(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0;
    ITUNER_VIGNETTE_s VignetteCompensation;
    UINTPTR MisraUptr;

    AmbaItuner_Get_VignetteCompensation(&VignetteCompensation);
    if (VignetteCompensation.Enable>0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Vignette Gain", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            if ((ituner_strlen(VignetteCompensation.VigGainTblPath) < 4U) || (Ext_File_Param->Vignette_Gain_Load_Param.Address == NULL) || (Ext_File_Param->Vignette_Gain_Load_Param.Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Vignette_Gain_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][DEBUG] %s(), VigGainTblMultPath = %s", DC_S, DC_S, DC_S,
                                   __func__, VignetteCompensation.VigGainTblPath);
                ituner_print_uint32_5("[TEXT HDLR][DEBUG] %d, Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Vignette_Gain_Load_Param.Size, DC_U, DC_U);
                Rval = 0;
            } else {
                if (TextHdlr_load_Ext_file(VignetteCompensation.VigGainTblPath, Ext_File_Param->Vignette_Gain_Load_Param.Size, Ext_File_Param->Vignette_Gain_Load_Param.Address) < 0) {
                    ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail", DC_S, DC_S, DC_S,
                                       __func__, VignetteCompensation.VigGainTblPath);
                    Rval = -1;
                }
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Vignette Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Warp_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_WARP_s WarpCompensation;
    UINTPTR MisraUptr;

    AmbaItuner_GetWarpInfo(&WarpCompensation);
    if (WarpCompensation.Enable>0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Warp Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(WarpCompensation.WarpGridTablePath);
            if ((MisraU32 < 4U) || (Ext_File_Param->Warp_Table_Load_Param.Address == NULL) || (Ext_File_Param->Warp_Table_Load_Param.Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Warp_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Warp_Table_Path = %s",
                                   __func__, WarpCompensation.WarpGridTablePath, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Warp_Table Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Warp_Table_Load_Param.Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(WarpCompensation.WarpGridTablePath, Ext_File_Param->Warp_Table_Load_Param.Size, Ext_File_Param->Warp_Table_Load_Param.Address);
            if ((Rval == 0) && ((MisraI32 < 0))) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, WarpCompensation.WarpGridTablePath, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Warp Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Bypass_Horizontal_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_WARP_DZOOM_INTERNAL_s WarpCompensationDzoomByPass;
    UINTPTR MisraUptr;

    AmbaItuner_Get_WarpCompensationDzoomInternal(&WarpCompensationDzoomByPass);
    if (WarpCompensationDzoomByPass.Enable>0U) {
        AMBA_FS_FILE_INFO_s FileInfo;
        FileInfo.Size = 0u;//misra 9.1
        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Warp Horizontal Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(WarpCompensationDzoomByPass.WarpHorizontalTablePath);
            if ((MisraU32 < 4U) || (Ext_File_Param->Internal_Warp_Horizontal_Table_Load_Param.Address == NULL) || (Ext_File_Param->Internal_Warp_Horizontal_Table_Load_Param.Max_Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Internal_Warp_Horizontal_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s() Warp_Horizontal_Table_Path = %s",
                                   __func__, WarpCompensationDzoomByPass.WarpHorizontalTablePath, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Warp_Horizontal_Table_ Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Internal_Warp_Horizontal_Table_Load_Param.Max_Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraU32 = AmbaFS_GetFileInfo(WarpCompensationDzoomByPass.WarpHorizontalTablePath, &FileInfo);
            if ((Rval == 0) &&
                (MisraU32 != 0UL)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_Stat(%s) Fail", WarpCompensationDzoomByPass.WarpHorizontalTablePath, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }

            if ((Rval == 0) &&
                ((UINT32)FileInfo.Size > Ext_File_Param->Internal_Warp_Horizontal_Table_Load_Param.Max_Size)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call Invalid File Size", DC_S, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(WarpCompensationDzoomByPass.WarpHorizontalTablePath, (UINT32)FileInfo.Size, Ext_File_Param->Internal_Warp_Horizontal_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, WarpCompensationDzoomByPass.WarpHorizontalTablePath, DC_S, DC_S, DC_S);
                Rval = -1;
            }

        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Bypass Horizontal Warp Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Bypass_Vertical_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_WARP_DZOOM_INTERNAL_s WarpCompensationDzoomByPass;
    UINTPTR MisraUptr;

    AmbaItuner_Get_WarpCompensationDzoomInternal(&WarpCompensationDzoomByPass);
    if (WarpCompensationDzoomByPass.Enable>0U) {
        AMBA_FS_FILE_INFO_s FileInfo;
        ituner_memset(&FileInfo, 0, sizeof(FileInfo));

        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Warp Vertical Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(WarpCompensationDzoomByPass.WarpVerticalTablePath);
            if ((MisraU32 < 4U) || (Ext_File_Param->Internal_Warp_Vertical_Table_Load_Param.Address == NULL) || (Ext_File_Param->Internal_Warp_Vertical_Table_Load_Param.Max_Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Internal_Warp_Vertical_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Warp_Vertical_Table_Path = %s",
                                   __func__, WarpCompensationDzoomByPass.WarpVerticalTablePath, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Warp_Vertical_Table Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Internal_Warp_Vertical_Table_Load_Param.Max_Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraU32 = AmbaFS_GetFileInfo(WarpCompensationDzoomByPass.WarpVerticalTablePath, &FileInfo);
            if ((Rval == 0) && (MisraU32 != 0UL)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_Stat(%s) Fail", WarpCompensationDzoomByPass.WarpVerticalTablePath, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            if ((Rval == 0) &&
                (FileInfo.Size > Ext_File_Param->Internal_Warp_Vertical_Table_Load_Param.Max_Size)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call Invalid File Size", DC_S, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(WarpCompensationDzoomByPass.WarpVerticalTablePath, (UINT32)FileInfo.Size, Ext_File_Param->Internal_Warp_Vertical_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s() call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, WarpCompensationDzoomByPass.WarpVerticalTablePath, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Bypass Vertical Warp Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Ca_Red_Table(const Ituner_Ext_File_Param_s *Ext_File_Param) __attribute__((unused));
static INT32 TextHdlr_Load_Ca_Red_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_CHROMA_ABERRATION_s ChromaAberrationInfo;
    UINTPTR MisraUptr;

    AmbaItuner_GetCawarpInfo(&ChromaAberrationInfo);
    if (ChromaAberrationInfo.Enable>0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Ca Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(ChromaAberrationInfo.RedCaGridTablePath);
            if ((MisraU32 < 4U) || (Ext_File_Param->Ca_Table_Load_Param.Address == NULL) || (Ext_File_Param->Ca_Table_Load_Param.Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Ca_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Ca_Table_Path = %s",
                                   __func__, ChromaAberrationInfo.RedCaGridTablePath, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Ca_Table Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Ca_Table_Load_Param.Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(ChromaAberrationInfo.RedCaGridTablePath, Ext_File_Param->Ca_Table_Load_Param.Size, Ext_File_Param->Ca_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, ChromaAberrationInfo.RedCaGridTablePath, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Ca Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}
static INT32 TextHdlr_Load_Ca_Blue_Table(const Ituner_Ext_File_Param_s *Ext_File_Param) __attribute__((unused));
static INT32 TextHdlr_Load_Ca_Blue_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_CHROMA_ABERRATION_s ChromaAberrationInfo;
    UINTPTR MisraUptr;

    AmbaItuner_GetCawarpInfo(&ChromaAberrationInfo);
    if (ChromaAberrationInfo.Enable>0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Ca Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(ChromaAberrationInfo.BlueCaGridTablePath);
            if ((MisraU32 < 4U) || (Ext_File_Param->Ca_Table_Load_Param.Address == NULL) || (Ext_File_Param->Ca_Table_Load_Param.Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Ca_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Ca_Table_Path = %s",
                                   __func__, ChromaAberrationInfo.BlueCaGridTablePath, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Ca_Table Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Ca_Table_Load_Param.Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(ChromaAberrationInfo.BlueCaGridTablePath, Ext_File_Param->Ca_Table_Load_Param.Size, Ext_File_Param->Ca_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, ChromaAberrationInfo.BlueCaGridTablePath, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Ca Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}


static INT32 TextHldr_Load_Bypass_Ca_Hor_Red_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_CHROMA_ABERRATION_INTERNAL_s ChromaAberrationInfoByPass;
    UINTPTR MisraUptr;

    AmbaItuner_GetCaWarpInternal(&ChromaAberrationInfoByPass);

    if (ChromaAberrationInfoByPass.HorzWarpEnable>0U) {
        AMBA_FS_FILE_INFO_s FileInfo;
        ituner_memset(&FileInfo, 0, sizeof(FileInfo));

        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Bypass Horizontal Ca Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(ChromaAberrationInfoByPass.WarpHorzTablePathRed);
            if ((MisraU32 < 4U) || (Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Address == NULL) || (Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Max_Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Bypass Horizontal Ca_Table_Path = %s",
                                   __func__, ChromaAberrationInfoByPass.WarpHorzTablePathRed, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Bypass Horizontal Address = %d, Max_Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Max_Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraU32 = AmbaFS_GetFileInfo(ChromaAberrationInfoByPass.WarpHorzTablePathRed, &FileInfo);
            if ((Rval == 0) && (MisraU32 != 0UL)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_Stat(%s) Fail", ChromaAberrationInfoByPass.WarpHorzTablePathRed, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            if ((Rval == 0) &&
                (FileInfo.Size > Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Max_Size)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call Invalid File Size", DC_S, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(ChromaAberrationInfoByPass.WarpHorzTablePathRed, (UINT32)FileInfo.Size, Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, ChromaAberrationInfoByPass.WarpHorzTablePathRed, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Bypass Horizontal Ca Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Bypass_Ca_Vert_Red_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_CHROMA_ABERRATION_INTERNAL_s ChromaAberrationInfoByPass;
    UINTPTR MisraUptr;

    AmbaItuner_GetCaWarpInternal(&ChromaAberrationInfoByPass);
    if (ChromaAberrationInfoByPass.VertWarpEnable>0U) {
        AMBA_FS_FILE_INFO_s FileInfo;
        ituner_memset(&FileInfo, 0, sizeof(FileInfo));

        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Bypass Vertical Ca Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(ChromaAberrationInfoByPass.WarpVertTablePathRed);
            if ((MisraU32 < 4U) || (Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Address == NULL) || (Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Max_Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Bypass Vertical Ca_Table_Path = %s",
                                   __func__, ChromaAberrationInfoByPass.WarpVertTablePathRed, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Bypass Vertical Address = %p, Max_Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Max_Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraU32 = AmbaFS_GetFileInfo(ChromaAberrationInfoByPass.WarpVertTablePathRed, &FileInfo);
            if ((Rval == 0) && (MisraU32 != 0UL)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_Stat(%s) Fail", ChromaAberrationInfoByPass.WarpVertTablePathRed, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            if ((Rval == 0) &&
                (FileInfo.Size > Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Max_Size)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call Invalid File Size", DC_S, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(ChromaAberrationInfoByPass.WarpVertTablePathRed, (UINT32)FileInfo.Size, Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, ChromaAberrationInfoByPass.WarpVertTablePathRed, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Bypass Vertical Ca Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHldr_Load_Bypass_Ca_Hor_Blue_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_CHROMA_ABERRATION_INTERNAL_s ChromaAberrationInfoByPass;
    UINTPTR MisraUptr;

    AmbaItuner_GetCaWarpInternal(&ChromaAberrationInfoByPass);

    if (ChromaAberrationInfoByPass.HorzWarpEnable>0U) {
        AMBA_FS_FILE_INFO_s FileInfo;
        ituner_memset(&FileInfo, 0, sizeof(FileInfo));

        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Bypass Horizontal Ca Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(ChromaAberrationInfoByPass.WarpHorzTablePathBlue);
            if ((MisraU32 < 4U) || (Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Address == NULL) || (Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Max_Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Bypass Horizontal Ca_Table_Path = %s",
                                   __func__, ChromaAberrationInfoByPass.WarpHorzTablePathBlue, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Bypass Horizontal Address = %d, Max_Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Max_Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraU32 = AmbaFS_GetFileInfo(ChromaAberrationInfoByPass.WarpHorzTablePathBlue, &FileInfo);
            if ((Rval == 0) && (MisraU32 != 0UL)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_Stat(%s) Fail", ChromaAberrationInfoByPass.WarpHorzTablePathBlue, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            if ((Rval == 0) &&
                (FileInfo.Size > Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Max_Size)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call Invalid File Size", DC_S, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(ChromaAberrationInfoByPass.WarpHorzTablePathBlue, (UINT32)FileInfo.Size, Ext_File_Param->Internal_Ca_Horizontal_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, ChromaAberrationInfoByPass.WarpHorzTablePathBlue, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Bypass Horizontal Ca Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Bypass_Ca_Vert_Blue_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_CHROMA_ABERRATION_INTERNAL_s ChromaAberrationInfoByPass;
    UINTPTR MisraUptr;

    AmbaItuner_GetCaWarpInternal(&ChromaAberrationInfoByPass);
    if (ChromaAberrationInfoByPass.VertWarpEnable>0U) {
        AMBA_FS_FILE_INFO_s FileInfo;
        ituner_memset(&FileInfo, 0, sizeof(FileInfo));

        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Bypass Vertical Ca Table", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(ChromaAberrationInfoByPass.WarpVertTablePathBlue);
            if ((MisraU32 < 4U) || (Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Address == NULL) || (Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Max_Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Bypass Vertical Ca_Table_Path = %s",
                                   __func__, ChromaAberrationInfoByPass.WarpVertTablePathBlue, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Bypass Vertical Ca_Table Address = %p, Max_Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Max_Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraU32 = AmbaFS_GetFileInfo(ChromaAberrationInfoByPass.WarpVertTablePathBlue, &FileInfo);
            if ((Rval == 0) && (MisraU32 != 0UL)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_Stat(%s) Fail", ChromaAberrationInfoByPass.WarpVertTablePathBlue, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            if ((Rval == 0) &&
                (FileInfo.Size > Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Max_Size)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] call Invalid File Size", DC_S, DC_S, DC_S, DC_S, DC_S);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(ChromaAberrationInfoByPass.WarpVertTablePathBlue, (UINT32)FileInfo.Size, Ext_File_Param->Internal_Ca_Vertical_Table_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, ChromaAberrationInfoByPass.WarpVertTablePathBlue, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Bypass Vertical Ca Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_FPN_Map(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_FPN_s StaticBadPixelCorrection;
    UINTPTR MisraUptr;

    AmbaItuner_Get_StaticBadPixelCorrection(&StaticBadPixelCorrection);
    if (StaticBadPixelCorrection.Enable>0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Load FPN Map", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(StaticBadPixelCorrection.MapPath);
            if ((MisraU32 < 4U) || (Ext_File_Param->FPN_MAP_Ext_Load_Param.Address == NULL) || (Ext_File_Param->FPN_MAP_Ext_Load_Param.Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->FPN_MAP_Ext_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), FPN_Map_Path = %s",
                                   __func__, StaticBadPixelCorrection.MapPath, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, FPN_Map Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->FPN_MAP_Ext_Load_Param.Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(StaticBadPixelCorrection.MapPath, Ext_File_Param->FPN_MAP_Ext_Load_Param.Size, Ext_File_Param->FPN_MAP_Ext_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, StaticBadPixelCorrection.MapPath, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] FPN Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_Bypass_FPN_Map(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    ITUNER_FPN_INTERNAL_s StaticBadPixelCorrectionByPass;
    UINTPTR MisraUptr;

    AmbaItuner_Get_SBP_CorrectionInternal(&StaticBadPixelCorrectionByPass);
    if (StaticBadPixelCorrectionByPass.Enable>0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Load Bypass FPN Map", DC_S, DC_S, DC_S, DC_S, DC_S);
        if (Ext_File_Param == NULL) {
            ituner_print_str_5("[TEXT HDLR][ERROR] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            Rval = -1;
        } else {
            MisraU32 = ituner_strlen(StaticBadPixelCorrectionByPass.MapPath);
            if ((MisraU32 < 4U) || (Ext_File_Param->Internal_FPN_MAP_Ext_Load_Param.Address == NULL) || (Ext_File_Param->Internal_FPN_MAP_Ext_Load_Param.Size == 0U)) {
                (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->Internal_FPN_MAP_Ext_Load_Param.Address, sizeof(UINTPTR));
                ituner_print_str_5("[TEXT HDLR][ERROR] %s(), FPN_Map_Path = %s",
                                   __func__, StaticBadPixelCorrectionByPass.MapPath, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, FPN_Map Address = %d, Size = %d",
                                      __LINE__, MisraUptr, Ext_File_Param->Internal_FPN_MAP_Ext_Load_Param.Size, DC_U, DC_U);
                Rval = -1;
            }
            MisraI32 = TextHdlr_load_Ext_file(StaticBadPixelCorrectionByPass.MapPath, Ext_File_Param->Internal_FPN_MAP_Ext_Load_Param.Size, Ext_File_Param->Internal_FPN_MAP_Ext_Load_Param.Address);
            if ((Rval == 0) && (MisraI32 < 0)) {
                ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                                   __func__, StaticBadPixelCorrectionByPass.MapPath, DC_S, DC_S, DC_S);
                Rval = -1;
            }
        }
    } else {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Bypass FPN Disable, Skip Load Process", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

static INT32 TextHdlr_Load_CC_Reg(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    ITUNER_COLOR_CORRECTION_s ColorCorrection;
    UINTPTR MisraUptr;

    AmbaItuner_Get_ColorCorrection(&ColorCorrection);
    if ((ituner_strlen(ColorCorrection.RegPath) < 4U) || (Ext_File_Param->CC_Reg_Load_Param.Address == NULL) || (Ext_File_Param->CC_Reg_Load_Param.Size == 0U)) {
        (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->CC_Reg_Load_Param.Address, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][ERROR] CC Reg Path = %s", ColorCorrection.RegPath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][ERROR] CC Reg Address = %d, Size = %d", MisraUptr, Ext_File_Param->CC_Reg_Load_Param.Size, DC_U, DC_U, DC_U);
        Rval = -1;
    }
    MisraI32 = TextHdlr_load_Ext_file(ColorCorrection.RegPath, Ext_File_Param->CC_Reg_Load_Param.Size, Ext_File_Param->CC_Reg_Load_Param.Address);
    if ((Rval == 0) && (MisraI32 < 0)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %s() call TextHdlr_load_ext_file(%s) Fail", __func__, ColorCorrection.RegPath, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    return Rval;
}

static INT32 TextHdlr_Load_CC_ThreeD(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    ITUNER_COLOR_CORRECTION_s ColorCorrection;
    UINTPTR MisraUptr;

    AmbaItuner_Get_ColorCorrection(&ColorCorrection);
    if ((ituner_strlen(ColorCorrection.ThreeDPath) < 4U) || (Ext_File_Param->CC_ThreeD_Load_Param.Address == NULL) || (Ext_File_Param->CC_ThreeD_Load_Param.Size == 0U)) {
        (void)ituner_memcpy(&MisraUptr, &Ext_File_Param->CC_ThreeD_Load_Param.Address, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][ERROR] CC 3D Path = %s", ColorCorrection.ThreeDPath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][ERROR] CC 3D Address = %d, Size = %d", MisraUptr, Ext_File_Param->CC_ThreeD_Load_Param.Size, DC_U, DC_U, DC_U);
        Rval = -1;
    }
    MisraI32 = TextHdlr_load_Ext_file(ColorCorrection.ThreeDPath, Ext_File_Param->CC_ThreeD_Load_Param.Size, Ext_File_Param->CC_ThreeD_Load_Param.Address);
    if ((Rval == 0) && (MisraI32 < 0)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %s() call TextHdlr_load_ext_file(%s) Fail", __func__, ColorCorrection.ThreeDPath, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    return Rval;
}

static INT32 TextHdlr_Load_HdrAlphaTable(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0;
#ifdef TODO_HDR
    ituner_video_hdr_alpha_calc_alpha_s AlphaTable;
    AmbaItuner_Get_VideoHdrAlphaCalcAlpha(Ext_File_Param->HDR_AlphaTable_Load_Param.Index, &AlphaTable);
    if (ituner_strlen(AlphaTable.AlphaTablePath) < 4 || Ext_File_Param->HDR_AlphaTable_Load_Param.Address == NULL || Ext_File_Param->HDR_AlphaTable_Load_Param.Size == 0) {
        ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Hdr Alpha Path = %s",
                           __func__, AlphaTable.AlphaTablePath, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Hdr Alpha Address = %d, Size = %d",
                              __LINE__, (UINT32)Ext_File_Param->HDR_AlphaTable_Load_Param.Address, Ext_File_Param->HDR_AlphaTable_Load_Param.Size, DC_U, DC_U);
        Rval = -1;
    }
    if ((Rval == 0) &&
        (TextHdlr_load_Ext_file(AlphaTable.AlphaTablePath, Ext_File_Param->HDR_AlphaTable_Load_Param.Size, Ext_File_Param->HDR_AlphaTable_Load_Param.Address) < 0)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s) Fail",
                           __func__, AlphaTable.AlphaTablePath,, DC_S, DC_S, DC_S);
        Rval = -1;
    }
#endif
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    return Rval;
}

static INT32 TextHdlr_Load_HdrLinearTable(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
#ifdef TODO_HDR
    ituner_video_hdr_amp_linear_s LinearTable;
    AmbaItuner_Get_VideoHdrAmplifierLinearization(Ext_File_Param->HDR_Linear_Load_Param.Index, &LinearTable);
    if (ituner_strlen(LinearTable.LinearLookupTablePath) < 4 || Ext_File_Param->HDR_Linear_Load_Param.Address == NULL || Ext_File_Param->HDR_Linear_Load_Param.Size == 0) {
        ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Hdr Linear Path = %s",
                           __func__,
                           LinearTable.LinearLookupTablePath, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Hdr Linear Address = %d, Size = %d",
                              __LINE__,
                              (UINT32)Ext_File_Param->HDR_Linear_Load_Param.Address,
                              Ext_File_Param->HDR_Linear_Load_Param.Size, DC_U, DC_U);
    } else if (TextHdlr_load_Ext_file(LinearTable.LinearLookupTablePath, Ext_File_Param->HDR_Linear_Load_Param.Size, Ext_File_Param->HDR_Linear_Load_Param.Address) < 0) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %s(),call TextHdlr_load_ext_file(%s) Fail", __func__, LinearTable.LinearLookupTablePath, DC_S, DC_S, DC_S);
    }
#endif
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    return 0;
}

static INT32 TextHdlr_Load_HdrContrastEnhanceTable(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
#ifdef TODO_HDR
    ITUNER_VIDEO_CONTRAST_ENHC_s Contrast_Enhance;
    AmbaItuner_Get_VideoHdrContrastEnhance(&Contrast_Enhance);
    if (ituner_strlen(Contrast_Enhance.OutputTablePath) < 4 || Ext_File_Param->HDR_Contrast_Enhance_Load_Param.Address == NULL || Ext_File_Param->HDR_Contrast_Enhance_Load_Param.Size == 0) {
        ituner_print_str_5("[TEXT HDLR][ERROR] %s(), Hdr Contrast Enhance output table Path = %s",
                           __func__,
                           Contrast_Enhance.OutputTablePath, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][ERROR] %d, Hdr Contrast Enhance output table Address = %d, Size = %d",
                              __LINE__,
                              (UINT32)Ext_File_Param->HDR_Contrast_Enhance_Load_Param.Address,
                              Ext_File_Param->HDR_Contrast_Enhance_Load_Param.Size, DC_U, DC_U);
        return -1;
    } else if (TextHdlr_load_Ext_file(Contrast_Enhance.OutputTablePath, Ext_File_Param->HDR_Contrast_Enhance_Load_Param.Size, Ext_File_Param->HDR_Contrast_Enhance_Load_Param.Address) < 0 ) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_load_ext_file(%s, Addr : %p) Fail", __func__, Contrast_Enhance.OutputTablePath, DC_S, DC_S, DC_S);
        return -1;
    }
#endif
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    return 0;
}

static INT32 TextHdlr_Save_Warp_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    const AMBA_IK_GRID_POINT_s *pMisraGridPoint;
    char FilePath[APP_MAX_FN_SIZE];
    ITUNER_WARP_s Warp;
    AMBA_IK_WARP_INFO_s WarpInfo;
    void *pMisraVoid;
    UINTPTR MisraUptr;
    FilePath[0] = (char)0; //misra9.1
    if (Ext_File_Param==NULL) {
        // FIXME
    }

    AmbaItuner_GetWarpInfoRetrive(&WarpInfo);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Warp Reg", DC_S, DC_S, DC_S, DC_S, DC_S);
    // Note: Update Warp Path
    (void)TextHdlr_Get_Ext_File_Path(FILE_EXT_WARP_TABLE, FilePath);
    AmbaItuner_GetWarpInfo(&Warp);
    if (Warp.Enable==0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Warp.enable=0, skip to dump it.", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        MisraU32 = sizeof(Warp.WarpGridTablePath);
        MisraI32 = (INT32)MisraU32;
        ituner_strncpy(Warp.WarpGridTablePath, FilePath, MisraI32);
        AmbaItuner_SetWarpInfo(&Warp);
        pMisraGridPoint = &WarpInfo.pWarp[0];
        (void)ituner_memcpy(&pMisraVoid, &pMisraGridPoint, sizeof(void*));
        (void)ituner_memcpy(&MisraUptr, &pMisraGridPoint, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Warp Reg to %s", Warp.WarpGridTablePath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][DEBUG] Dump Warp Reg Address %p", MisraUptr, DC_U, DC_U, DC_U, DC_U);
        if (TextHdlr_Save_Ext_File(Warp.WarpGridTablePath, Warp.HorizontalGridNumber*Warp.VerticalGridNumber*4UL, pMisraVoid) < 0) {
            ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_Save_Ext_File(%s) Fail", __func__, Warp.WarpGridTablePath, DC_S, DC_S, DC_S);
            Rval = -1;
        }
    }
    return Rval;
}

static INT32 TextHdlr_Save_Cawarp_Red_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    const AMBA_IK_GRID_POINT_s *pMisraGridPoint;
    char FilePath[128];
    ITUNER_CHROMA_ABERRATION_s Ca;
    AMBA_IK_CAWARP_INFO_s CAwarpInfo;
    void *pMisraVoid;
    UINTPTR MisraUptr;
    FilePath[0] = (char)0;//misra 9.1
    if (Ext_File_Param==NULL) {
        // FIXME
    }

    AmbaItuner_GetCawarpInfoRetrive(&CAwarpInfo);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump CA red Reg", DC_S, DC_S, DC_S, DC_S, DC_S);
    // Note: Update CA red Path
    (void)TextHdlr_Get_Ext_File_Path(FILE_EXT_CA_RED_TABLE, FilePath);
    AmbaItuner_GetCawarpInfo(&Ca);
    if (Ca.Enable==0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Ca.enable=0, skip to dump it.", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        MisraU32 = sizeof(Ca.RedCaGridTablePath);
        MisraI32 = (INT32)MisraU32;
        ituner_strncpy(Ca.RedCaGridTablePath, FilePath, MisraI32);
        AmbaItuner_SetCawarpInfo(&Ca);
        pMisraGridPoint = CAwarpInfo.pCawarpRed;
        (void)ituner_memcpy(&pMisraVoid, &pMisraGridPoint, sizeof(void*));
        (void)ituner_memcpy(&MisraUptr, &pMisraGridPoint, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Ca red Reg to %s", Ca.RedCaGridTablePath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][DEBUG] Dump Ca red Reg Address %p", MisraUptr, DC_U, DC_U, DC_U, DC_U);
        if (TextHdlr_Save_Ext_File(Ca.RedCaGridTablePath, Ca.HorizontalGridNumber*Ca.VerticalGridNumber*4UL, pMisraVoid) < 0) {
            ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_Save_Ext_File(%s) Fail", __func__, Ca.RedCaGridTablePath, DC_S, DC_S, DC_S);
            Rval = -1;
        }
    }
    return Rval;
}

static INT32 TextHdlr_Save_Cawarp_Blue_Table(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    const AMBA_IK_GRID_POINT_s *pMisraGridPoint;
    char FilePath[128];
    ITUNER_CHROMA_ABERRATION_s Ca;
    AMBA_IK_CAWARP_INFO_s CAwarpInfo;
    void *pMisraVoid;
    UINTPTR MisraUptr;
    FilePath[0] = (char)0;//misra 9.1
    if (Ext_File_Param==NULL) {
        // FIXME
    }

    AmbaItuner_GetCawarpInfoRetrive(&CAwarpInfo);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump CA blue Reg", DC_S, DC_S, DC_S, DC_S, DC_S);
    // Note: Update CA blue Path
    (void)TextHdlr_Get_Ext_File_Path(FILE_EXT_CA_BLUE_TABLE, FilePath);
    AmbaItuner_GetCawarpInfo(&Ca);
    if (Ca.Enable==0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Ca.enable=0, skip to dump it.", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        MisraU32 = sizeof(Ca.BlueCaGridTablePath);
        MisraI32 = (INT32)MisraU32;
        ituner_strncpy(Ca.BlueCaGridTablePath, FilePath, MisraI32);
        AmbaItuner_SetCawarpInfo(&Ca);
        pMisraGridPoint = CAwarpInfo.pCawarpBlue;
        (void)ituner_memcpy(&pMisraVoid, &pMisraGridPoint, sizeof(void*));
        (void)ituner_memcpy(&MisraUptr, &pMisraGridPoint, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Ca blue Reg to %s", Ca.BlueCaGridTablePath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][DEBUG] Dump Ca blue Reg Address %p", MisraUptr, DC_U, DC_U, DC_U, DC_U);
        if (TextHdlr_Save_Ext_File(Ca.BlueCaGridTablePath, Ca.HorizontalGridNumber*Ca.VerticalGridNumber*4UL, pMisraVoid) < 0) {
            ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_Save_Ext_File(%s) Fail", __func__, Ca.BlueCaGridTablePath, DC_S, DC_S, DC_S);
            Rval = -1;
        }
    }
    return Rval;
}

static INT32 TextHdlr_Save_Vignette_Mult(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    const UINT32 *pMisraU32;
    char FilePath[128];
    ITUNER_VIGNETTE_s Vignette;
    void *pMisraVoid;
    UINTPTR MisraUptr;
    FilePath[0] = (char)0;//misra 9.1
    if (Ext_File_Param==NULL) {
        // FIXME
    }

    AmbaItuner_Get_VignetteCompensation(&Vignette);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump vig Reg", DC_S, DC_S, DC_S, DC_S, DC_S);
    // Note: Update Warp Path
    (void)TextHdlr_Get_Ext_File_Path(FILE_EXT_VIGNETTE, FilePath);
    if (Vignette.Enable==0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] vig.enable=0, skip to dump it.", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        MisraU32 = sizeof(Vignette.VigGainTblPath);
        MisraI32 = (INT32)MisraU32;
        ituner_strncpy(Vignette.VigGainTblPath, FilePath, MisraI32);
        AmbaItuner_Set_VignetteCompensation(&Vignette);
        pMisraU32 = &Vignette.VignetteInfo.CalibVignetteInfo.VigGainTblR[0][0];
        (void)ituner_memcpy(&pMisraVoid, &pMisraU32, sizeof(void*));
        (void)ituner_memcpy(&MisraUptr, &pMisraU32, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][DEBUG] Dump vig Reg to %s", Vignette.VigGainTblPath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][DEBUG] Dump vig Reg Address %p", MisraUptr, DC_U, DC_U, DC_U, DC_U);
        if (TextHdlr_Save_Ext_File(Vignette.VigGainTblPath, 4*128*4*4, pMisraVoid) < 0) {
            ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_Save_Ext_File(%s) Fail", __func__, Vignette.VigGainTblPath, DC_S, DC_S, DC_S);
            Rval = -1;
        }
    }
    return Rval;
}

static INT32 TextHdlr_Save_FPN_Map(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    const UINT8 *pMisraU8;
    char FilePath[128];
    ITUNER_FPN_s Fpn;
    AMBA_IK_STATIC_BAD_PXL_COR_s SbpInfo;
    void *pMisraVoid;
    UINTPTR MisraUptr;
    FilePath[0] = (char)0;//misra 9.1
    if (Ext_File_Param==NULL) {
        // FIXME
    }

    AmbaItuner_GetSbpInfoRetrive(&SbpInfo);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Warp Reg", DC_S, DC_S, DC_S, DC_S, DC_S);
    // Note: Update FPN Path
    (void)TextHdlr_Get_Ext_File_Path(FILE_EXT_FPN_MAP, FilePath);
    AmbaItuner_Get_StaticBadPixelCorrection(&Fpn);
    if (Fpn.Enable==0U) {
        ituner_print_str_5("[TEXT HDLR][DEBUG] Fpn.enable=0, skip to dump it.", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        MisraU32 = sizeof(Fpn.MapPath);
        MisraI32 = (INT32)MisraU32;
        ituner_strncpy(Fpn.MapPath, FilePath, MisraI32);
        AmbaItuner_Set_StaticBadPixelCorrection(&Fpn);
        pMisraU8 = &SbpInfo.CalibSbpInfo.pSbpBuffer[0];
        (void)ituner_memcpy(&pMisraVoid, &pMisraU8, sizeof(void*));
        (void)ituner_memcpy(&MisraUptr, &pMisraU8, sizeof(UINTPTR));
        ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Fpn Reg to %s", Fpn.MapPath, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[TEXT HDLR][DEBUG] Dump Fpn Reg Address %p", MisraUptr, DC_U, DC_U, DC_U, DC_U);
        if (TextHdlr_Save_Ext_File(Fpn.MapPath, (Fpn.CalibVinWidth*Fpn.CalibVinHeight)>>3UL, pMisraVoid) < 0) {
            ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_Save_Ext_File(%s) Fail", __func__, Fpn.MapPath, DC_S, DC_S, DC_S);
            Rval = -1;
        }
    }
    return Rval;
}

static INT32 TextHdlr_Save_CC_Reg(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    const UINT32 *pMisraU32;
    char FilePath[128];
    ITUNER_COLOR_CORRECTION_s ColorCorrection;
    AMBA_IK_COLOR_CORRECTION_REG_s *pCcReg;
    void *pMisraVoid;
    UINTPTR MisraUptr;
    FilePath[0] = (char)0;//misra 9.1
    if (Ext_File_Param==NULL) {
        // FIXME
    }

    AmbaItuner_Get_CcReg(&pCcReg);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump CC Reg", DC_S, DC_S, DC_S, DC_S, DC_S);
    // Note: Update ColorCorrection Path
    (void)TextHdlr_Get_Ext_File_Path(FILE_EXT_CC_REG, FilePath);
    AmbaItuner_Get_ColorCorrection(&ColorCorrection);
    MisraU32 = sizeof(ColorCorrection.RegPath);
    MisraI32 = (INT32)MisraU32;
    ituner_strncpy(ColorCorrection.RegPath, FilePath, MisraI32);
    AmbaItuner_Set_ColorCorrection(&ColorCorrection);
    pMisraU32 = &(pCcReg->RegSettingTable[0]);
    (void)ituner_memcpy(&pMisraVoid, &pMisraU32, sizeof(void*));
    (void)ituner_memcpy(&MisraUptr, &pMisraU32, sizeof(UINTPTR));
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump CC Reg to %s", ColorCorrection.RegPath, DC_S, DC_S, DC_S, DC_S);
    ituner_print_uint32_5("[TEXT HDLR][DEBUG] Dump CC Reg Address %p", MisraUptr, DC_U, DC_U, DC_U, DC_U);
    if (TextHdlr_Save_Ext_File(ColorCorrection.RegPath, IK_CC_REG_SIZE, pMisraVoid) < 0) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_Save_Ext_File(%s) Fail", __func__, ColorCorrection.RegPath, DC_S, DC_S, DC_S);
        Rval = -1;
    }

    return Rval;
}

static INT32 TextHdlr_Save_CC_ThreeD(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    const UINT32 *pMisraU32;
    char FilePath[128];
    ITUNER_COLOR_CORRECTION_s ColorCorrection;
    AMBA_IK_COLOR_CORRECTION_s *pCcThreed;
    void *pMisraVoid;
    UINTPTR MisraUptr;
    FilePath[0] = (char)0;//misra 9.1
    if (Ext_File_Param==NULL) {
        // FIXME
    }

    AmbaItuner_Get_CcThreeD(&pCcThreed);
    (void)TextHdlr_Get_Ext_File_Path(FILE_EXT_CC_THREED, FilePath);
    AmbaItuner_Get_ColorCorrection(&ColorCorrection);
    MisraU32 = sizeof(ColorCorrection.ThreeDPath);
    MisraI32 = (INT32)MisraU32;
    ituner_strncpy(ColorCorrection.ThreeDPath, FilePath, MisraI32);
    AmbaItuner_Set_ColorCorrection(&ColorCorrection);
    pMisraU32 = &(pCcThreed->MatrixThreeDTable[0]);
    (void)ituner_memcpy(&pMisraVoid, &pMisraU32, sizeof(void*));
    (void)ituner_memcpy(&MisraUptr, &pMisraU32, sizeof(UINTPTR));
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump CC ThreeD to %s", ColorCorrection.ThreeDPath, DC_S, DC_S, DC_S, DC_S);
    ituner_print_uint32_5("[TEXT HDLR][DEBUG] Dump CC ThreeD Address %p", MisraUptr, DC_U, DC_U, DC_U, DC_U);
    if (TextHdlr_Save_Ext_File(ColorCorrection.ThreeDPath, IK_CC_3D_SIZE, pMisraVoid) < 0) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %s(), call TextHdlr_Save_Ext_File(%s, Addr : %p) Fail", __func__, ColorCorrection.ThreeDPath, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    return Rval;
}
static INT32 TextHdlr_Save_CFA_STAT(const Ituner_Ext_File_Param_s *Ext_File_Param) __attribute__((unused));
static INT32 TextHdlr_Save_CFA_STAT(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
#ifdef TODO_STAT
    AMBA_ITN_SYSTEM_s System;
    AMBA_DSP_EVENT_CFA_3A_DATA_s *Cfa_Stat;
    AMBA_IK_3A_HEADER_s *headInfo;
    AMBA_IK_CFA_AE_s *silceAeStat = NULL;
    AMBA_IK_CFA_AWB_s *silceAwbStat = NULL;
    UINT8 w = 0, h = 0;
    UINT8 Output_Filename_Len;
    char Token[16];
    INT32 i, j, idx;
    char lineBuf[640];
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    AMBA_FS_FILE *Fd;
    if (Ext_File_Param == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    if ( Ext_File_Param->Stat_Save_Param.Address == NULL || Ext_File_Param->Stat_Save_Param.Size < sizeof(AMBA_DSP_EVENT_CFA_3A_DATA_s)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] %d, Ext_File_Param->Stat_Save_Param.Address = %d, Size = %d(Need size :%d)",
                           __LINE__, (UINT32)Ext_File_Param->Stat_Save_Param.Address, Ext_File_Param->Stat_Save_Param.Size);
        return -1;
    }

    AmbaItuner_Get_SystemInfo(&System);
    (void)ituner_memset(FilePath, 0x0, sizeof(FilePath));
    ituner_strncpy(FilePath, Ext_File_Param->Stat_Save_Param.Target_File_Path, sizeof(FilePath));
    if (0 != TextHdlr_Change_System_Drive(FilePath)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Change_System_Drive(%s)", FilePath, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    FileName_Pos = ituner_strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] Invalid FileName_Pos: %s", FileName_Pos, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    Output_Filename_Len = ituner_strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        (void)ituner_strncat(FileName_Pos + 1, System.OutputFilename, MAX_NAME_LENS);
        //sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + ituner_strlen(System.OutputFilename);
        (void)ituner_strncat(Ext_Name_Pos, "_CFA_STAT.txt", MAX_NAME_LENS);
        //sprintf(Ext_Name_Pos, "_CFA_STAT.txt");
    } else {
        Ext_Name_Pos = ituner_strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            ituner_print_str_5("[TEXT HDLR][WARNING] Invalid FilePath : %s", FilePath, DC_S, DC_S, DC_S, DC_S);
            return -1;
        }

    }


    Cfa_Stat = (AMBA_DSP_EVENT_CFA_3A_DATA_s*)Ext_File_Param->Stat_Save_Param.Address;
    ituner_print_str_5("[TEXT HDLR][DEBUG] FilePath = %s", FilePath, DC_S, DC_S, DC_S, DC_S);



    headInfo = &Cfa_Stat->Header;
    w = headInfo->AeTileNumCol;
    h = headInfo->AeTileNumRow;
    Fd = ituner_fopen(FilePath, "w");
    if (Fd < 0) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call ituner_fopen(%s) Fail", FilePath, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    (void)ituner_strncat(lineBuf, "CFA AE statistics ", sizeof(lineBuf));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)w, 10);
    (void)ituner_strncat(lineBuf, Token, sizeof(lineBuf));
    (void)ituner_strncat(lineBuf, "x", sizeof(lineBuf));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)h, 10);
    (void)ituner_strncat(lineBuf, Token, sizeof(lineBuf));
    (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
    //sprintf(lineBuf, "CFA AE statistics %dx%d\n", w, h);
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    silceAeStat = &Cfa_Stat->Ae[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * w + i;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)silceAeStat[idx].LinY, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            //sprintf(num, "%5d ", silceAeStat[idx].LinY);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, 1, ituner_strlen(lineBuf), Fd);
    }

    w = headInfo->AwbTileNumCol;
    h = headInfo->AwbTileNumRow;
    silceAwbStat = &Cfa_Stat->Awb[0];
    (void)ituner_strncat(lineBuf, "CFA AWB statistics\n", sizeof(lineBuf));
    //sprintf(lineBuf, "CFA AWB statistics\n");
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    (void)ituner_strncat(lineBuf, "---  Red statistics   ---\n", sizeof(lineBuf));
    //sprintf(lineBuf, "---  Red statistics   ---\n");
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            uint16 temp;
            idx = j * w + i;
            temp = silceAwbStat[idx].SumR;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)temp, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%5d ", temp);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    }
    (void)ituner_strncat(lineBuf, "---  Green statistics   ---\n", sizeof(lineBuf));
    //sprintf(lineBuf, "---  Green statistics   ---\n");
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            uint16 temp;
            idx = j * w + i;
            temp = silceAwbStat[idx].SumG;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)temp, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%5d ", temp);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    }
    (void)ituner_strncat(lineBuf, "---  Blue statistics   ---\n");
    //sprintf(lineBuf, "---  Blue statistics   ---\n");
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    for (j=0; j<h; j++) {
        lineBuf[0]='\0';
        for (i=0; i<w; i++) {
            char num[10]="\0";
            uint16 temp;
            idx = j*w + i;
            temp = silceAwbStat[idx].SumB;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)temp, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num,"%5d ", temp);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    }

    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)headInfo->AwbTileActiveWidth, 10);
    (void)ituner_strncat(num, "CFA AWB tile act_width: ", sizeof(num));
    (void)ituner_strncat(num, Token, sizeof(num));
    (void)ituner_strncat(num, "\n", sizeof(num));
    //sprintf(lineBuf, "CFA AWB tile act_width: %d\n" , headInfo->AwbTileActiveWidth);
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)headInfo->AwbTileActiveHeight, 10);
    (void)ituner_strncat(num, "CFA AWB tile act_height: ", sizeof(num));
    (void)ituner_strncat(num, Token, sizeof(num));
    (void)ituner_strncat(num, "\n", sizeof(num));
    //sprintf(lineBuf, "CFA AWB tile act_height: %d\n", headInfo->AwbTileActiveHeight);
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)headInfo->AwbRgbShift, 10);
    (void)ituner_strncat(num, "CFA AWB RGB shift: ", sizeof(num));
    (void)ituner_strncat(num, Token, sizeof(num));
    (void)ituner_strncat(num, "\n", sizeof(num));
    //sprintf(lineBuf, "CFA AWB RGB shift: %d\n" , headInfo->AwbRgbShift);
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    AmbaFS_FileSync(Fd);
    ituner_fclose(Fd);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Cfa Stat to %s Done", FilePath, DC_S, DC_S, DC_S, DC_S);
#endif
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    return 0;
}
static INT32 TextHdlr_Save_RGB_STAT(const Ituner_Ext_File_Param_s *Ext_File_Param) __attribute__((unused));
static INT32 TextHdlr_Save_RGB_STAT(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
#ifdef TODO_STAT
    AMBA_ITN_SYSTEM_s System;
    AMBA_DSP_EVENT_RGB_3A_DATA_s *rgbStat;
    AMBA_IK_3A_HEADER_s *headInfo;
    AMBA_IK_RGB_AE_s *silceAeStat = NULL;
    AMBA_IK_RGB_AF_s *silceAfStat = NULL;
    UINT8 w = 0, h = 0;
    UINT8 Output_Filename_Len;
    char Token[16];
    INT32 i, j, idx;
    char lineBuf[640];
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    AMBA_FS_FILE *Fd;
    if (Ext_File_Param == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    if (Ext_File_Param->Stat_Save_Param.Address == NULL || Ext_File_Param->Stat_Save_Param.Size < sizeof(AMBA_DSP_EVENT_RGB_3A_DATA_s)) {
        ituner_print_uint32_5("[TEXT HDLR][WARNING] Ext_File_Param->Stat_Save_Param.Address = %d, Size = %d(Need size :%d)", (UINT32)Ext_File_Param->Stat_Save_Param.Address, Ext_File_Param->Stat_Save_Param.Size);
        return -1;
    }

    AmbaItuner_Get_SystemInfo(&System);
    (void)ituner_memset(FilePath, 0x0, sizeof(FilePath));
    ituner_strncpy(FilePath, Ext_File_Param->Stat_Save_Param.Target_File_Path, sizeof(FilePath));
    if (0 != TextHdlr_Change_System_Drive(FilePath)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Change_System_Drive(%s)", FilePath, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    FileName_Pos = ituner_strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] Invalid FileName_Pos: %s", FileName_Pos, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    Output_Filename_Len = ituner_strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        (void)ituner_strncat(FileName_Pos + 1, System.OutputFilename, MAX_NAME_LENS);
        //sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + ituner_strlen(System.OutputFilename);
        (void)ituner_strncat(Ext_Name_Pos, "_RGB_STAT.txt", MAX_NAME_LENS);
        //sprintf(Ext_Name_Pos, "_RGB_STAT.txt");
    } else {
        Ext_Name_Pos = ituner_strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            ituner_print_str_5("[TEXT HDLR][WARNING] Invalid FilePath : %s", FilePath, DC_S, DC_S, DC_S, DC_S);
            return -1;
        }

    }


    rgbStat = (AMBA_DSP_EVENT_RGB_3A_DATA_s*) Ext_File_Param->Stat_Save_Param.Address;
    ituner_print_str_5("[TEXT HDLR][DEBUG] FilePath = %s", FilePath, DC_S, DC_S, DC_S, DC_S);

    headInfo = &rgbStat->Header;
    w = headInfo->AeTileNumCol;
    h = headInfo->AeTileNumRow;
    Fd = ituner_fopen(FilePath, "w");
    if (Fd < 0) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call ituner_fopen(%s) Fail", FilePath, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    (void)ituner_strncat(lineBuf, "RGB AE statistics ", sizeof(lineBuf));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)w, 10);
    (void)ituner_strncat(lineBuf, Token, sizeof(lineBuf));
    (void)ituner_strncat(lineBuf, "x", sizeof(lineBuf));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)h, 10);
    (void)ituner_strncat(lineBuf, Token, sizeof(lineBuf));
    (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
    //sprintf(lineBuf, "RGB AE statistics %dx%d\n", w, h);
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    silceAeStat = &rgbStat->Ae[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * w + i;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)silceAeStat[idx].SumY, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%5d ", silceAeStat[idx].SumY);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, 1, ituner_strlen(lineBuf), Fd);
    }

    w = headInfo->AfTileNumCol;
    h = headInfo->AfTileNumRow;
    (void)ituner_strncat(lineBuf, "RGB AF statistics ", sizeof(lineBuf));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)w, 10);
    (void)ituner_strncat(lineBuf, Token, sizeof(lineBuf));
    (void)ituner_strncat(lineBuf, "x", sizeof(lineBuf));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
    AmbaUtility_UInt32ToStr(Token, 16, (UINT32)h, 10);
    (void)ituner_strncat(lineBuf, Token, sizeof(lineBuf));
    (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
    //sprintf(lineBuf, "RGB AF statistics %dx%d\n", w, h);
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    (void)ituner_strncat(lineBuf, "---  SumFY  ---\n", sizeof(lineBuf));
    //sprintf(lineBuf, "---  SumFY  ---\n");
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    silceAfStat = &rgbStat->Af[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * w + i;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)silceAfStat[idx].SumFY, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%5d ", silceAfStat[idx].SumFY);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, 1, ituner_strlen(lineBuf), Fd);
    }
    (void)ituner_strncat(lineBuf, "---  SumFV1  ---\n", sizeof(lineBuf));
    //sprintf(lineBuf, "---  SumFV1  ---\n");
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    silceAfStat = &rgbStat->Af[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * w + i;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)silceAfStat[idx].SumFV1, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%5d ", silceAfStat[idx].SumFV1);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, 1, ituner_strlen(lineBuf), Fd);
    }

    (void)ituner_strncat(lineBuf, "---  SumFV2  ---\n", sizeof(lineBuf));
    //sprintf(lineBuf, "---  SumFV2  ---\n");
    ituner_fwrite(lineBuf, ituner_strlen(lineBuf), 1, Fd);
    silceAfStat = &rgbStat->Af[0];
    for (j = 0; j < h; j++) {
        lineBuf[0] = '\0';
        for (i = 0; i < w; i++) {
            char num[10] = "\0";
            idx = j * w + i;
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)silceAfStat[idx].SumFV2, 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%5d ", silceAfStat[idx].SumFV2);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, 1, ituner_strlen(lineBuf), Fd);
    }

    AmbaFS_FileSync(Fd);
    ituner_fclose(Fd);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Cfa Stat to %s Done", FilePath, DC_S, DC_S, DC_S, DC_S);
#endif
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    return 0;
}
static INT32 TextHdlr_Save_HDR_CFA_Histogram(const Ituner_Ext_File_Param_s *Ext_File_Param) __attribute__((unused));
static INT32 TextHdlr_Save_HDR_CFA_Histogram(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
#ifdef TODO_STAT
    AMBA_ITN_SYSTEM_s System;
    UINT8 Output_Filename_Len;
    INT32 j;
    char lineBuf[640];
    char FilePath[MAX_NAME_LENS];
    char *FileName_Pos;
    char *Ext_Name_Pos;
    AMBA_FS_FILE *Fd;
    char Token[16];
    AMBA_DSP_HDR_HIST_STAT_s *pHdrAaaHist;
    if (Ext_File_Param == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] Ext_File_Param = NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    if ( Ext_File_Param->Stat_Save_Param.Address == NULL || Ext_File_Param->Stat_Save_Param.Size < sizeof(AMBA_DSP_HDR_HIST_STAT_s)) {
        ituner_print_uint32_5("[TEXT HDLR][WARNING] %d, Ext_File_Param->Stat_Save_Param.Address = %d, Size = %d(Need size :%d)",
                              __LINE__, (UINT32)Ext_File_Param->Stat_Save_Param.Address, Ext_File_Param->Stat_Save_Param.Size);
        return -1;
    }

    AmbaItuner_Get_SystemInfo(&System);
    (void)ituner_memset(FilePath, 0x0, sizeof(FilePath));
    ituner_strncpy(FilePath, Ext_File_Param->Stat_Save_Param.Target_File_Path, sizeof(FilePath));
    if (0 != TextHdlr_Change_System_Drive(FilePath)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Change_System_Drive(%s)", FilePath, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    FileName_Pos = ituner_strrchr(FilePath, '\\');
    if (FileName_Pos == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] Invalid FileName_Pos: %s", FileName_Pos, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    Output_Filename_Len = ituner_strlen(System.OutputFilename);
    if (Output_Filename_Len) {
        (void)ituner_strncat(FileName_Pos + 1, System.OutputFilename, MAX_NAME_LENS);
        //sprintf(FileName_Pos + 1, "%s", System.OutputFilename);
        Ext_Name_Pos = FileName_Pos + 1 + ituner_strlen(System.OutputFilename);
        (void)ituner_strncat(Ext_Name_Pos, "_HDR_CFA_HIST.txt", MAX_NAME_LENS);
        //sprintf(Ext_Name_Pos, "_HDR_CFA_HIST.txt");
    } else {
        Ext_Name_Pos = ituner_strrchr(FilePath, '.');
        if (Ext_Name_Pos == NULL) {
            ituner_print_str_5("[TEXT HDLR][WARNING] Invalid FilePath : %s", FilePath, DC_S, DC_S, DC_S, DC_S);
            return -1;
        }

    }

    ituner_print_str_5("[TEXT HDLR][DEBUG] FilePath = %s", FilePath, DC_S, DC_S, DC_S, DC_S);

    Fd = ituner_fopen(FilePath, "w");
    if (Fd < 0) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call ituner_fopen(%s) Fail", FilePath, DC_S, DC_S, DC_S, DC_S);
        return -1;
    }
    pHdrAaaHist = (AMBA_DSP_HDR_HIST_STAT_s*) Ext_File_Param->Stat_Save_Param.Address;
    for (j = 0; j < 128; j++) {
        lineBuf[0] = '\0';
        {
            char num[10] = "\0";
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)pHdrAaaHist->HistoBinR[j], 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%7d ", pHdrAaaHist->HistoBinR[j]);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)pHdrAaaHist->HistoBinG[j], 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%7d ", pHdrAaaHist->HistoBinG[j]);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
            (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16);
            AmbaUtility_UInt32ToStr(Token, 16, (UINT32)pHdrAaaHist->HistoBinB[j], 10);
            (void)ituner_strncat(num, Token, sizeof(num));
            (void)ituner_strncat(num, " ", sizeof(num));
            //sprintf(num, "%7d ", pHdrAaaHist->HistoBinB[j]);
            (void)ituner_strncat(lineBuf, num, sizeof(lineBuf));
        }
        (void)ituner_strncat(lineBuf, "\n", sizeof(lineBuf));
        ituner_fwrite(lineBuf, 1, ituner_strlen(lineBuf), Fd);
    }

    AmbaFS_FileSync(Fd);
    ituner_fclose(Fd);
    ituner_print_str_5("[TEXT HDLR][DEBUG] Dump Cfa Stat to %s Done", FilePath, DC_S, DC_S, DC_S, DC_S);
#endif
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    return 0;
}
static INT32 TextHdlr_Save_and_Load_Dummy(const Ituner_Ext_File_Param_s *Ext_File_Param)
{
    if (Ext_File_Param==NULL) {
        // FIXME
    }
    ituner_print_str_5("[TEXT HDLR][WARNING] Not Yet Implement", DC_S, DC_S, DC_S, DC_S, DC_S);
    return 0;
}

typedef struct {
    INT32 (*pFunc)(const Ituner_Ext_File_Param_s * Ext_File_Param);
    Ituner_Ext_File_Type_e Ext_File_Type;
} LoadDataApiList_s;

#define LoadDataApiList_Count 25
INT32 TextHdlr_Load_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s* Ext_File_Param)
{
    INT32 i, Rval = -1;
    const LoadDataApiList_s LoadDataApiList[LoadDataApiList_Count] = {
        {TextHdlr_Load_FPN_Map, FILE_EXT_FPN_MAP},
        {TextHdlr_Load_Bypass_FPN_Map, FILE_EXT_INTERNAL_FPN_MAP},
        {TextHldr_Load_Vignette, FILE_EXT_VIGNETTE},
        {TextHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_VIGNETTE_MULT},
        {TextHdlr_Save_and_Load_Dummy, FILE_EXT_INTERNAL_VIGNETTE_SHIFT},
        {TextHdlr_Load_Warp_Table, FILE_EXT_WARP_TABLE},
        {TextHdlr_Load_Bypass_Horizontal_Table, FILE_EXT_INTERNAL_WARP_HORIZONTAL_TABLE},
        {TextHdlr_Load_Bypass_Vertical_Table, FILE_EXT_INTERNAL_WARP_VERTICAL_TABLE},
        {TextHdlr_Load_Ca_Red_Table, FILE_EXT_CA_RED_TABLE},
        {TextHdlr_Load_Ca_Blue_Table, FILE_EXT_CA_BLUE_TABLE},
        {TextHldr_Load_Bypass_Ca_Hor_Red_Table, FILE_EXT_INTERNAL_CA_RED_HORIZONTAL_TABLE},
        {TextHdlr_Load_Bypass_Ca_Vert_Red_Table, FILE_EXT_INTERNAL_CA_RED_VERTICAL_TABLE},
        {TextHldr_Load_Bypass_Ca_Hor_Blue_Table, FILE_EXT_INTERNAL_CA_BLUE_HORIZONTAL_TABLE},
        {TextHdlr_Load_Bypass_Ca_Vert_Blue_Table, FILE_EXT_INTERNAL_CA_BLUE_VERTICAL_TABLE},
        {TextHdlr_Load_CC_Reg, FILE_EXT_CC_REG},
        {TextHdlr_Load_CC_ThreeD, FILE_EXT_CC_THREED},
        {TextHdlr_Save_and_Load_Dummy, FILE_EXT_ASF_THREE_D_TABLE},
        {TextHdlr_Save_and_Load_Dummy, FILE_EXT_FIRST_SHARPEN},
        {TextHdlr_Save_and_Load_Dummy, FILE_EXT_ASF},
        {TextHdlr_Load_Raw, FILE_EXT_RAW},
        {TextHdlr_Save_and_Load_Dummy, FILE_EXT_JPG},
        {TextHdlr_Save_and_Load_Dummy, FILE_EXT_YUV},
        {TextHdlr_Load_HdrAlphaTable, FILE_EXT_HDR_ALPHA_TABLE},
        {TextHdlr_Load_HdrLinearTable, FILE_EXT_HDR_LINEAR_TABLE},
        {TextHdlr_Load_HdrContrastEnhanceTable, FILE_EXT_HDR_CONTRAST_ENHANCE_TABLE},
    };
    for (i = 0; i < LoadDataApiList_Count; i++) {
        if (LoadDataApiList[i].Ext_File_Type == Ext_File_Type) {
            Rval = LoadDataApiList[i].pFunc(Ext_File_Param);
            break;
        }
    }
    if (Rval == -1) {
        ituner_print_uint32_5("[TEXT HDLR][ERROR] Invalid Ext_File_Type %d", (UINT32)Ext_File_Type, DC_U, DC_U, DC_U, DC_U);
    }
    return Rval;
}

typedef struct {
    INT32 (*pFunc)(const Ituner_Ext_File_Param_s * Ext_File_Param);
    Ituner_Ext_File_Type_e Ext_File_Type;
} SaveDataApiList_s;

#define SaveDataApiList_Count 23
INT32 TextHdlr_Save_Data(Ituner_Ext_File_Type_e Ext_File_Type, const Ituner_Ext_File_Param_s* Ext_File_Param)
{
    INT32 i, Rval = -1;
    SaveDataApiList_s SaveDataApiList[SaveDataApiList_Count];

    SaveDataApiList[0].pFunc = TextHdlr_Save_FPN_Map;
    SaveDataApiList[0].Ext_File_Type  = FILE_EXT_FPN_MAP;
    SaveDataApiList[1].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[1].Ext_File_Type  = FILE_EXT_INTERNAL_FPN_MAP;
    SaveDataApiList[2].pFunc = TextHdlr_Save_Vignette_Mult;
    SaveDataApiList[2].Ext_File_Type  = FILE_EXT_VIGNETTE;
    SaveDataApiList[3].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[3].Ext_File_Type  = FILE_EXT_INTERNAL_VIGNETTE_MULT;
    SaveDataApiList[4].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[4].Ext_File_Type  = FILE_EXT_INTERNAL_VIGNETTE_SHIFT;
    SaveDataApiList[5].pFunc = TextHdlr_Save_Warp_Table;
    SaveDataApiList[5].Ext_File_Type  = FILE_EXT_WARP_TABLE;
    SaveDataApiList[6].pFunc = TextHdlr_Save_Cawarp_Red_Table;
    SaveDataApiList[6].Ext_File_Type  = FILE_EXT_CA_RED_TABLE;
    SaveDataApiList[7].pFunc = TextHdlr_Save_Cawarp_Blue_Table;
    SaveDataApiList[7].Ext_File_Type  = FILE_EXT_CA_BLUE_TABLE;
    SaveDataApiList[8].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[8].Ext_File_Type  = FILE_EXT_INTERNAL_WARP_HORIZONTAL_TABLE;
    SaveDataApiList[9].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[9].Ext_File_Type  = FILE_EXT_INTERNAL_WARP_VERTICAL_TABLE;
    SaveDataApiList[10].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[10].Ext_File_Type  = FILE_EXT_INTERNAL_CA_RED_HORIZONTAL_TABLE;
    SaveDataApiList[11].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[11].Ext_File_Type  = FILE_EXT_INTERNAL_CA_RED_VERTICAL_TABLE;
    SaveDataApiList[12].pFunc = TextHdlr_Save_CC_Reg;
    SaveDataApiList[12].Ext_File_Type  = FILE_EXT_CC_REG;
    SaveDataApiList[13].pFunc = TextHdlr_Save_CC_ThreeD;
    SaveDataApiList[13].Ext_File_Type  = FILE_EXT_CC_THREED;
    SaveDataApiList[14].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[14].Ext_File_Type  = FILE_EXT_FIRST_SHARPEN;
    SaveDataApiList[15].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[15].Ext_File_Type  = FILE_EXT_ASF;
    SaveDataApiList[16].pFunc = TextHdlr_Save_Raw;
    SaveDataApiList[16].Ext_File_Type  = FILE_EXT_RAW;
    SaveDataApiList[17].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[17].Ext_File_Type  = FILE_EXT_YUV;
    SaveDataApiList[18].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[18].Ext_File_Type  = FILE_EXT_JPG;
    SaveDataApiList[19].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[19].Ext_File_Type  = FILE_EXT_HDR_ALPHA_TABLE;
    SaveDataApiList[20].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[20].Ext_File_Type  = FILE_EXT_HDR_LINEAR_TABLE;
    SaveDataApiList[21].pFunc = TextHdlr_Save_and_Load_Dummy;
    SaveDataApiList[21].Ext_File_Type  = FILE_EXT_HDR_CONTRAST_ENHANCE_TABLE;

    for (i = 0; i < SaveDataApiList_Count; i++) {
        if (SaveDataApiList[i].Ext_File_Type == Ext_File_Type) {
            Rval = SaveDataApiList[i].pFunc(Ext_File_Param);
            break;
        }
    }
    if (Rval == -1) {
        ituner_print_uint32_5("[TEXT HDLR][ERROR] Invalid Ext_File_Type %d", (UINT32)Ext_File_Type, DC_U, DC_U, DC_U, DC_U);
    }
    return Rval;
}


static INT32 TextHdlr_load_Ext_file(const char *FileName, UINT32 Size, void *Buf)
{
    AMBA_FS_FILE *Fid;
    INT32 Ret = 0;
    UINTPTR MisraUptr;

    (void)ituner_memcpy(&MisraUptr, &Buf, sizeof(UINTPTR));
    ituner_print_str_5("[TEXT HDLR][DEBUG] FileName: %s", FileName, DC_S, DC_S, DC_S, DC_S);
    ituner_print_uint32_5("[TEXT HDLR][DEBUG] buf: %d, size: %d", MisraUptr, Size, DC_U, DC_U, DC_U);
    Fid = ituner_fopen(FileName,"r");
    (void)ituner_fseek(Fid, 0, 0);
    ituner_print_str_5("[TEXT HDLR][DEBUG] fseek to front", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (Fid == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_fopen(%s) Fail", FileName, DC_S, DC_S, DC_S, DC_S);
        Ret = -1;
    } else {
        if (ituner_fread(Buf, Size, 1, Fid)>0U) {
        } else {
            ituner_print_uint32_5("[TEXT HDLR][WARNING] call AmbaFS_fread() Fail, Buf: %d, Size: %d", MisraUptr, Size, DC_U, DC_U, DC_U);
            Ret = -2;
        }

        (void)ituner_fclose(Fid);
    }
    return Ret;
}

static INT32 TextHdlr_Save_Ext_File(const char *FileName, UINT32 Size, void *Buf)
{
    AMBA_FS_FILE *Fid;
    INT32 Ret = 0;
    UINTPTR MisraUptr;

    Fid = ituner_fopen(FileName, "w");
    if (Fid == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaFS_fopen(%s) Fail", FileName, DC_S, DC_S, DC_S, DC_S);
        Ret = -1;
    } else {
        if (ituner_fwrite(Buf, Size, 1, Fid)>0U) {

        } else {
            (void)ituner_memcpy(&MisraUptr, &Buf, sizeof(UINTPTR));
            ituner_print_uint32_5("[TEXT HDLR][WARNING] call AmbaFS_fwrite() Fail, Buf: %d, Size: %d", MisraUptr, Size, DC_U, DC_U, DC_U);
            Ret = -2;
        }

        (void)ituner_fclose(Fid);
    }

    return Ret;
}

static INT32 TextHdlr_Fill_Ext_File_Path(void)
{
    AMBA_ITUNER_VALID_FILTER_t Ituner_Valid;
    AmbaItuner_Get_FilterStatus(&Ituner_Valid);
    // Note: Without Return Fail, Because it should be happen.
    if (Ituner_Valid[ITUNER_COLOR_CORRECTION] == 1U) {
        if (0 != TextHdlr_Save_Data(FILE_EXT_CC_REG, NULL)) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Save_Data(FILE_EXT_CC_REG) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
        if (0 != TextHdlr_Save_Data(FILE_EXT_CC_THREED, NULL)) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Save_Data(FILE_EXT_CC_THREED) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if (Ituner_Valid[ITUNER_STATIC_BAD_PIXEL_CORRECTION] == 1U) {
        if (0 != TextHdlr_Save_Data(FILE_EXT_FPN_MAP, NULL)) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Save_Data(FILE_EXT_FPN_MAP) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if (Ituner_Valid[ITUNER_VIGNETTE_COMPENSATION] == 1U) {
        if (0 != TextHdlr_Save_Data(FILE_EXT_VIGNETTE, NULL)) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Save_Data(FILE_EXT_VIGNETTE_MULT) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if (Ituner_Valid[ITUNER_WARP_COMPENSATION] == 1U) {
        if (0 != TextHdlr_Save_Data(FILE_EXT_WARP_TABLE, NULL)) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Save_Data(FILE_EXT_WARP_TABLE) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    if (Ituner_Valid[ITUNER_CHROMA_ABERRATION_INFO] == 1U) {
        if (0 != TextHdlr_Save_Data(FILE_EXT_CA_RED_TABLE, NULL)) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Save_Data(FILE_EXT_CA_RED_TABLE) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
        if (0 != TextHdlr_Save_Data(FILE_EXT_CA_BLUE_TABLE, NULL)) {
            ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Save_Data(FILE_EXT_CA_BLUE_TABLE) Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return 0;
}

INT32 TextHdlr_Save_IDSP(const AMBA_IK_MODE_CFG_s *pMode, const char *Filepath)
{
    INT32 Rval = 0, MisraI32;
    UINT32 MisraU32;
    char *Buf = NULL;
    INT32 BufSize = 0;
    AMBA_FS_FILE *Fd = NULL;
    INT32 RegIndex;
    INT32 ParamIndex;
    INT32 RegNumIdx;
    const TUNE_REG_s *Reg;
    AMBA_ITUNER_VALID_FILTER_t Filter_Status;

    MisraU32 = sizeof(Ituner_File_Path);
    MisraI32 = (INT32)MisraU32;

    ituner_strncpy(Ituner_File_Path, Filepath, MisraI32);
    // Note: Refresh GData, System...
    if (0 != TextHdlr_Update_System_Info(pMode)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Update_System_Info() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    MisraI32 = AmbaItuner_Refresh(pMode);
    if ((Rval == 0) && (0 != MisraI32)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call AmbaItuner_Refresh() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    }
    MisraI32 = TextHdlr_Fill_Ext_File_Path();
    if ((Rval == 0) && (0 != MisraI32)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TextHdlr_Fill_Ext_File_Path() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        (void)TUNE_Parser_Set_Reglist_Valid((INT64)SKIP_VALID_CHECK, Ituner.ParserObject);
    }

    MisraI32 = TUNE_Parser_Get_LineBuf(Ituner.ParserObject, &Buf, &BufSize);
    if ((Rval == 0) && (0 != MisraI32)) {
        ituner_print_str_5("[TEXT HDLR][WARNING] call TUNE_Parser_Get_LineBuf() Fail", DC_S, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        ituner_print_str_5("[TEXT HDLR] Open ituner file: %s", Filepath, DC_S, DC_S, DC_S, DC_S);
        Fd = ituner_fopen(Filepath, "wb");
    }
    if (Fd == NULL) {
        ituner_print_str_5("[TEXT HDLR][WARNING] File %s open fail!", Filepath, DC_S, DC_S, DC_S, DC_S);
        Rval = -1;
    } else {
        ituner_Opmode_Set(ITUNER_ENC);
        AmbaItuner_Get_FilterStatus(&Filter_Status);
        // TODO : Remove _Ituner.ParserObject->RegCount, use api to get count
        for (RegIndex = 0; RegIndex < Ituner.ParserObject->RegCount; RegIndex++) {
            Reg = TUNE_Parser_Get_Reg(RegIndex, Ituner.ParserObject);
            if (Reg == NULL) {
                ituner_print_int32_5("[TEXT HDLR][WARNING] Reg %d is NULL", RegIndex, DC_I, DC_I, DC_I, DC_I);
                continue;
            }

            if (Filter_Status[Reg->Index] == 0U) {
                continue;
            }
            for (RegNumIdx = 0; RegNumIdx < Reg->RegNum; RegNumIdx++) { // e.g. amp[0] amp[1] .. amp[3]
                for (ParamIndex = 0; ParamIndex < Reg->ParamCount; ParamIndex++) { // enable, lutTable, path
                    if (0 != TUNE_Parser_Generate_Line(RegIndex, RegNumIdx, ParamIndex, Ituner.ParserObject)) {
                        continue;
                    }
                    (void)save_line(Fd, Buf);
                }
            }
        }
        (void)ituner_fclose(Fd);
        ituner_print_str_5("[TEXT HDLR] close ituner file: %s", Filepath, DC_S, DC_S, DC_S, DC_S);
    }
    return Rval;
}

