/**
 *  @file AmbaEEPROM_UnitTest.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 *  @details EEPROM unit test implementation
 *
 */

#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaEEPROM_UnitTest.h"

#if defined(CONFIG_EEPROM_MAX9295_96712_MICRO_24AA256)
#include "AmbaSbrg_Max9295_96712.h"
#include "AmbaEEPROM_MAX9295_96712_MICRO_24AA256.h"
#endif

#include "AmbaSvcWrap.h"

#include "SvcWrap.h"

static void AmbaEEPRomUT_PrintHexUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char HexString[MAX_HEX_STR_LEN];
    HexString[0] = '0';
    HexString[1] = 'x';
    HexString[2] = '\0';
    (void)AmbaUtility_UInt32ToStr(&HexString[2], MAX_HEX_STR_LEN-2U, Value, 16);
    PrintFunc(HexString);
}

static void AmbaEEPRomUT_PrintUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char IntString[MAX_INT_STR_LEN];
    (void)AmbaUtility_UInt32ToStr(IntString, MAX_INT_STR_LEN, Value, 10);
    PrintFunc(IntString);
}

static INT32 AmbaEEPRomUT_StringCompare(const char *pString1, const char *pString2)
{
    INT32 RetVal = 0;
    SIZE_t StringLength1, StringLength2;

    if ((pString1 == NULL) || (pString2 == NULL)) {
        RetVal = -1;
    } else {
        SIZE_t i;
        StringLength1 = AmbaUtility_StringLength(pString1);
        StringLength2 = AmbaUtility_StringLength(pString2);

        /* Compare string length first */
        if (StringLength1 != StringLength2) {
            RetVal = -1;
        } else {
            /* Compare string */
            for(i = 0U; i < StringLength1; i++) {
                if (pString1[i] != pString2[i]) {
                    if (pString1[i] < pString2[i]) {
                        RetVal = -1;
                    } else {
                        RetVal = 1;
                    }
                    break;
                }
            }
        }
    }
    return RetVal;
}

static void AmbaEEPRomUT_CmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage:\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" attach <VinId> <ChipId> - start eeprom\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" read <VinId> <ChipId> <RegAddr> <Count> - read eeprom\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" write <VinId> <ChipId> <RegAddr> <Value> <Count> - write eeprom\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" detach <VinId> - stop eeprom\n");
}

/**
 * EEProm test command entry
 *
 * @param [in] ArgCount Input arguments counter
 * @param [in] pArgVector Input arguments data
 * @param [in] PrintFunc print function entry
 *
 */
void AmbaEEPRomUT_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    #define MAX_BUF_SIZE    (128U)
    UINT8 DataBuf[MAX_BUF_SIZE];

    if (ArgCount < 2U) {
        AmbaEEPRomUT_CmdUsage(pArgVector, PrintFunc);
    } else {
        AMBA_EEPROM_CHANNEL_s Chan;

        AmbaSvcWrap_MisraMemset(&Chan, 0, sizeof(Chan));
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &(Chan.VinId));

        if ((AmbaEEPRomUT_StringCompare(pArgVector[1], "attach") == 0) && (ArgCount == 4U)) {
            /* init eeprom */
            AMBA_EEPROM_OBJ_s *pObj;
#if defined(CONFIG_EEPROM_MAX9295_96712_MICRO_24AA256)
            pObj = &AmbaEEPROM_MAX9295_96712_MICRO_24AA256Obj;
#else
            pObj = NULL;
#endif

            (void)AmbaUtility_StringToUInt32(pArgVector[3], &(Chan.VinId));

            (void)AmbaEEPROM_Hook(&Chan, pObj);
            (void)AmbaEEPROM_Init(&Chan);

        } else if ((AmbaEEPRomUT_StringCompare(pArgVector[1], "read") == 0) && (ArgCount == 6U)) {
            AMBA_EEPROM_CMD_CFG_s CmdCfg;
            UINT32 ChipId, RegAddr, Cnt, i;

            (void)AmbaUtility_StringToUInt32(pArgVector[3], &ChipId);
            (void)AmbaUtility_StringToUInt32(pArgVector[4], &RegAddr);
            (void)AmbaUtility_StringToUInt32(pArgVector[5], &Cnt);

            Chan.ChanId = ChipId;

            Cnt = (Cnt > MAX_BUF_SIZE) ? (MAX_BUF_SIZE) : (Cnt);
            AmbaSvcWrap_MisraMemset(DataBuf, 0, Cnt);

            CmdCfg.Offset = (UINT16)RegAddr;
            CmdCfg.pDataBuf = DataBuf;
            CmdCfg.DataSize = Cnt;

            if (OK == AmbaEEPROM_Read(&Chan, &CmdCfg)) {
                PrintFunc("[EEPROM] Read Chip ");
                AmbaEEPRomUT_PrintUInt32(ChipId, PrintFunc);
                PrintFunc("\n\r");

                for (i = 0; i < Cnt; i++) {
                    PrintFunc("\tAddr ");
                    AmbaEEPRomUT_PrintHexUInt32(RegAddr + i, PrintFunc);
                    PrintFunc(" Value ");
                    AmbaEEPRomUT_PrintHexUInt32(DataBuf[i], PrintFunc);
                    PrintFunc("\n\r");
                }
            }

        } else if ((AmbaEEPRomUT_StringCompare(pArgVector[1], "write") == 0) && (ArgCount == 7U)) {
            AMBA_EEPROM_CMD_CFG_s CmdCfg;
            UINT32 ChipId, RegAddr, Value, Cnt, i;

            (void)AmbaUtility_StringToUInt32(pArgVector[3], &ChipId);
            (void)AmbaUtility_StringToUInt32(pArgVector[4], &RegAddr);
            (void)AmbaUtility_StringToUInt32(pArgVector[5], &Value);
            (void)AmbaUtility_StringToUInt32(pArgVector[6], &Cnt);

            Chan.ChanId = ChipId;

            Cnt = (Cnt > MAX_BUF_SIZE) ? (MAX_BUF_SIZE) : (Cnt);
            AmbaSvcWrap_MisraMemset(DataBuf, (INT32)Value, Cnt);

            CmdCfg.Offset   = (UINT16)RegAddr;
            CmdCfg.pDataBuf = DataBuf;
            CmdCfg.DataSize = Cnt;

            if (OK == AmbaEEPROM_Write(&Chan, &CmdCfg)) {
                PrintFunc("[EEPROM] Write Chip ");
                AmbaEEPRomUT_PrintUInt32(ChipId, PrintFunc);
                PrintFunc("\n\r");

                for (i = 0; i < Cnt; i++) {
                    PrintFunc("\tAddr ");
                    AmbaEEPRomUT_PrintHexUInt32(RegAddr + i, PrintFunc);
                    PrintFunc(" Value ");
                    AmbaEEPRomUT_PrintHexUInt32(Value, PrintFunc);
                    PrintFunc("\n\r");
                }
            }

        } else {
            AmbaEEPRomUT_CmdUsage(pArgVector, PrintFunc);
        }
    }
}
