/**
 *  @file AmbaIOUTDiag_CmdCEHU.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details CEHU diag Command for CV2FS
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaMisraFix.h>
#include <AmbaShell.h>
#include <AmbaShell_Utility.h>
#ifndef AMBA_UTILITY_H
#include "AmbaUtility.h"
#endif
#ifndef AMBA_FS_H
#include "AmbaFS.h"
#endif

#ifndef AMBA_CEHU_H
#include "AmbaCEHU.h"
#endif

#ifndef AMBA_IOUT_DIAG_H
#include <AmbaIOUTDiag.h>
#endif

static char string_output[255];
static UINT32 uint32_args[5];

static void shell_cehu_cmd_usage(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)ArgCount;
    (void)pArgVector;
    PrintFunc("Usage: cehu show <id>        : display CEHU information.\n");
    PrintFunc("Usage: cehu mask_set [id] [value] : set MASK (0/1) for the CEHU error ID.\n");
    PrintFunc("Usage: cehu mode_set [id] [value] : set Safety Mode (0/1/2/3) for the CEHU error ID.\n");
    PrintFunc("Usage: cehu err_clr [id]     : clear error for the CEHU error ID.\n");
    PrintFunc("Usage: cehu cb_reg           : register CEHU callback.\n");
    PrintFunc("Usage: cehu cb_status        : display CEHU callback status.\n");
}

static void cehu_info_show_one(UINT32 InstanceID, UINT32 ID, AMBA_SHELL_PRINT_f PrintFunc)
{
    static const char *id_names_array[] = {
        "dummy",
        "vrom_safety_error",
        "vp_safety_error",
        "vorc_safety_error",
        "vmem_safety_error",
        "tsfm_cehu_safety_error",
        "test_mode_cehu_monitor_safety_error",
        "sw_arm_safety_error",
        "smem_safety_error",
        "rtc_dbg_sm_safety_error",
        "rct_safety_error",
        "pm_errorb0",
        "pm_errorb1",
        "pm_errorb2",
        "pm_errorb3",
        "pm_errorb4",
        "pm_errorb5",
        "pm_errorb6",
        "pm_errorb7",
        "pm_lv_errorb1",
        "pm_lv_errorb0",
        "pm_errorb8",
        "pm_errorb9",
        "pm_errorb10",
        "pm_errorb11",
        "pm_errorb12",
        "pm_errorb13",
        "pm_errorb14",
        "otp_safety_error",
        "osc_monitor_safety_error",
        "ol2c_safety_error",
        "mipi_dsi_safety_error",
        "mipi_dsi_safety_error",
        "mipi_dsi_safety_error",
        "mipi_dsi_safety_error",
        "mipi_dsi_safety_error",
        "mipi_dsi_safety_error",
        "mipi_dphy_safety_error",
        "mdenc_cehu_safety_error",
        "mddec_cehu_safety_error",
        "lbist_safety_error",
        "ijtag_reset_cehu_monitor_safety_error",
        "idsp_safety_error",
        "fex_cehu_safety_error",
        "eorc_cehu_safety_error",
        "dram_controller_safety_error",
        "dorc_cehu_safety_error",
        "ddrhoste_dtte_safety_error",
        "ddrhoste_ddrc_safety_error",
        "ddrhost0_dtte_safety_error",
        "ddrhost0_ddrc_safety_error",
        "cpu_monitor_safety_error",
        "code_cehu_safety_error",
        "gclk_axi1",
        "n0_pclk",
        "n1_pclkdbg_clk",
        "CA53 cortex0",
        "CA53 cortex1",
        "gclk_ssi_dft",
        "gclk_ssi2_dft",
        "gclk_ssi3_dft",
        "gclk_uart0_dft",
        "gclk_uart1_dft",
        "gclk_uart2_dft",
        "gclk_uart3_dft",
        "gclk_uart_apb_dft",
        "gclk_core_ahb",
        "gclk_core_dft",
        "gclk_ahb_dft",
        "gclk_apb_dft",
        "gclk_dbg_dft",
        "gclk_sdio_dft",
        "gclk_sd48_dft",
        "gclk_nand_dft",
        "n0_gclk_sys_cnt_dft",
        "n1_gclk_sys_cnt_dft",
        "gclk_pwm_dft",
        "gclk_can_dft",
        "gclk_au_dft",
        "gclk_gtx_dft",
        "gclk_axi1_dft",
        "gclk_vo_a_dft",
        "gclk_vo_b_dft",
        "gclk_vision_dft",
        "gclk_dram_div2_dft",
        "combophy_clka",
        "combophy_clkb",
        "gclk_fex_dft",
        "amba1_gclk_dbg2dbg",
        "amba1_gclk_core_ahb",
        "amba1_gclk_core_dft",
        "amba1_gclk_ahb_dft",
        "amba1_gclk_axi1",
        "idsp0_cehu_dbg_ck_mon_error",
        "idsp0_cehu_core_ck_mon_error",
        "idsp0_cehu_idsp_ck_mon_error",
        "idsp1_cehu_dbg_ck_mon_error",
        "idsp1_cehu_core_ck_mon_error",
        "idsp1_cehu_idsp_ck_mon_error",
        "pll_out_video_b_send_err",
        "pll_out_video_a_send_err",
        "gclk_so_send_err",
        "gclk_so2_send_err",
        "pll_out_sd_send_err",
        "gclk_so_vin1_send_err",
        "gclk_so_vin0_send_err",
        "pll_out_enet_send_err",
        "pll_out_ddr_div4_send_err",
        "pll_out_cortex1_send_err",
        "pll_out_cortex0_send_err",
        "pll_out_core_send_err",
        "ddr_init_send_err",
        "pll_out_audio_send_err",
        "pll_out_vision_send_err",
        "pll_out_nand_send_err",
        "pll_out_idsp_send_err",
        "pll_out_fex_send_err",
        "gclk_dbg_out_send_err",
        "code_cehu_clk_core_ck_mon_error",
        "code_cehu_clk_dbg_ck_mon_error",
        "orc_cehu_clk_core_ck_mon_error",
        "orc_cehu_clk_dbg_ck_mon_error",
        "smem_cehu_core_ck_mon_error",
        "vdsp_cehu_clk_core_ck_mon_error",
        "vdsp_cehu_clk_dbg_ck_mon_error",
        "vmem_cehu_gclk_vision_ck_mon_error",
        "vpf_cehu_vision_ckmon_error",
        "vpd_cehu_vision_ckmon_error",
        "vpc_cehu_vision_ckmon_error",
        "vpb_cehu_vision_ckmon_error",
        "vpa_cehu_vision_ckmon_error",
        "vpa_cehu_dbg_ckmon_error",
        "clk_ref_err",
        "storc_l2c_ckmon_safety_error",
        "storc_vorc_ckmon_safety_error",
        "fex_cehu_fex_ck_mon_error",
        "ddrhoste_dram_div2_ckmon_error",
        "ddrhoste_dram_ckmon_error",
        "ddrhost0_dram_div2_ckmon_error",
        "ddrhost0_dram_ckmon_error",
        "pm_errorb15",
        "mipi_dphy_dbg_slave_par_fail",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "biasgen_safety_error",
        "amba_safety_error"
    };

    UINT32 mask = 0;
    UINT32 error_status = 0;
    UINT32 safety_mode = 0;

    if (AmbaCEHU_MaskGet(InstanceID, ID, &mask) != 0U) {
        // ignore error
    }
    if (AmbaCEHU_ErrorGet(InstanceID, ID, &error_status) != 0U) {
        // ignore error
    }
    if (AmbaCEHU_SafetyModeGet(InstanceID, ID, &safety_mode) != 0U) {
        // ignore error
    }

    uint32_args[0] = ID;
    uint32_args[1] = mask;
    uint32_args[2] = error_status;
    uint32_args[3] = safety_mode;

    if (AmbaUtility_StringPrintUInt32(string_output, 255, "%03d    %d   %d    0x%X(", 4, uint32_args) != 0U) {
        // ignore returned value
    }

    PrintFunc(string_output);

    if ((safety_mode & 0x01U) != 0U) {
        PrintFunc("GIC/");
    } else {
        PrintFunc("   /");
    }

    if ((safety_mode & 0x02U) != 0U) {
        PrintFunc("PIN");
    } else {
        PrintFunc("   ");
    }
    PrintFunc(")");
    PrintFunc(" ");
    PrintFunc(id_names_array[ID]);
    PrintFunc("\r\n");
}

static void cehu_info_show(UINT32 ID, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    PrintFunc("Instance 0:\r\n");
    PrintFunc("ID  MASK STS    MODE         Name\r\n");
    PrintFunc("---------------------------------------\r\n");
    if (ID != 0xFFFFFFFFU) {
        cehu_info_show_one(0, ID, PrintFunc);
    } else {
        for (i = 0; i < AMBA_NUM_CEHU_ERRORS; i++) {
            cehu_info_show_one(0, i, PrintFunc);
        }
    }

    PrintFunc("\r\n\r\n");
    PrintFunc("Instance 1:\r\n");
    PrintFunc("ID  MASK STS    MODE         Name\r\n");
    PrintFunc("---------------------------------------\r\n");
    if (ID != 0xFFFFFFFFU) {
        cehu_info_show_one(1, ID, PrintFunc);
    } else {
        for (i = 0; i < AMBA_NUM_CEHU_ERRORS; i++) {
            cehu_info_show_one(1, i, PrintFunc);
        }
    }
}

static void cehu_mask_set(UINT32 ID, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 uret;
    // instance 0
    if (Value == 0U) {
        uret = AmbaCEHU_MaskDisable(0, ID);
    } else {
        uret = AmbaCEHU_MaskEnable(0, ID);
    }
    if (uret != 0U) {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[0] Fail to set ID %d mask %d, code = %d\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    } else {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[0] set ID %d mask %d\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    }

    // instance 1
    if (Value == 0U) {
        uret = AmbaCEHU_MaskDisable(1, ID);
    } else {
        uret = AmbaCEHU_MaskEnable(1, ID);
    }
    if (uret != 0U) {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[1] Fail to set ID %d mask %d, code = %d\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    } else {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[1] set ID %d mask %d\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    }
}

static void cehu_mode_set(UINT32 ID, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 uret;

    // instance 0
    uret = AmbaCEHU_SafetyModeSet(0, ID, Value & 0x03U);
    if (uret != 0U) {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[0] Fail to set ID %d safety mode 0x%X, code = %d\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    } else {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[0] set ID %d safety mode 0x%X\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    }

    // instance 1
    uret = AmbaCEHU_SafetyModeSet(1, ID, Value & 0x03U);
    if (uret != 0U) {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[1] Fail to set ID %d safety mode 0x%X, code = %d\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    } else {
        uint32_args[0] = ID;
        uint32_args[1] = Value;
        uint32_args[2] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[1] set ID %d safety mode 0x%X\n", 3, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    }


}

static void cehu_error_clear(UINT32 ID, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 uret;

    // clear instance 0
    uret = AmbaCEHU_ErrorClear(0, ID);
    if (uret != 0U) {
        uint32_args[0] = ID;
        uint32_args[1] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[0] Fail to clear error for ID %d, code = %d\n", 2, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    } else {
        uint32_args[0] = ID;
        uint32_args[1] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[0] clear error for ID %d\n", 2, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    }
    // clear instance 1
    uret = AmbaCEHU_ErrorClear(1, ID);
    if (uret != 0U) {
        uint32_args[0] = ID;
        uint32_args[1] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[1] Fail to clear error for ID %d, code = %d\n", 2, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    } else {
        uint32_args[0] = ID;
        uint32_args[1] = uret;
        if (AmbaUtility_StringPrintUInt32(string_output, 255, "[1] clear error for ID %d\n", 2, uint32_args) != 0U) {
            // ignore returned value
        }
        PrintFunc(string_output);
    }
}

static UINT32 echu0_error_count = 0;
static UINT32 echu1_error_count = 0;

static void cehu_callback_func(UINT32 InstanceID)
{
    if (InstanceID == 0U) {
        echu0_error_count++;
    } else {
        echu1_error_count++;
    }

    if (echu0_error_count > 10U) {
        if (AmbaCEHU_InterruptDisable() != 0U) {
            // ignore returned value
        }
    }

    if (echu1_error_count > 10U) {
        if (AmbaCEHU_InterruptDisable() != 0U) {
            // ignore returned value
        }
    }
}

static void cehu_callback_register(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 uret = AmbaCEHU_IntCallbackRegister(cehu_callback_func);
    static UINT32 flag_init = 0;
    if (flag_init == 0U) {
        if (uret != 0U) {
            uint32_args[0] = uret;
            if (AmbaUtility_StringPrintUInt32(string_output, 255, "Fail to register callback, code = %d\n", 1, uint32_args) != 0U) {
                // ignore returned value
            }
            PrintFunc(string_output);
        } else {
            PrintFunc("callback registered.\n");
            flag_init = 1;
        }
    } else {
        PrintFunc("callback is already registered.\n");
    }
}

void AmbaIOUTDiag_CmdCEHU(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        shell_cehu_cmd_usage(ArgCount, pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "show", 4) == 0) {
            UINT32 id = 0xFFFFFFFFUL;
            if (ArgCount >= 3U) {
                if (AmbaUtility_StringToUInt32(pArgVector[2], &id) != 0U) {
                    // ignore returned value
                }
            }
            cehu_info_show(id, PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "mask_set", 8) == 0) {
            UINT32 id = 0xFFFFFFFFUL;
            UINT32 value = 0;
            if (ArgCount >= 4U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[2], &id);
                (void)AmbaUtility_StringToUInt32(pArgVector[3], &value);
                cehu_mask_set(id, value, PrintFunc);
            } else {
                shell_cehu_cmd_usage(ArgCount, pArgVector, PrintFunc);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "mode_set", 8) == 0) {
            UINT32 id = 0xFFFFFFFFUL;
            UINT32 value = 0;
            if (ArgCount >= 4U) {
                if (AmbaUtility_StringToUInt32(pArgVector[2], &id) != 0U) {
                    // ignore returned value
                }
                if (AmbaUtility_StringToUInt32(pArgVector[3], &value) != 0U) {
                    // ignore returned value
                }
                cehu_mode_set(id, value, PrintFunc);
            } else {
                shell_cehu_cmd_usage(ArgCount, pArgVector, PrintFunc);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "err_clr", 7) == 0) {
            UINT32 id = 0xFFFFFFFFUL;
            if (ArgCount >= 3U) {
                if (AmbaUtility_StringToUInt32(pArgVector[2], &id) != 0U) {
                    // ignore returned value
                }
                cehu_error_clear(id, PrintFunc);
            } else {
                shell_cehu_cmd_usage(ArgCount, pArgVector, PrintFunc);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "cb_reg", 6) == 0) {
            cehu_callback_register(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "cb_status", 9) == 0) {
            uint32_args[0] = echu0_error_count;
            uint32_args[1] = echu1_error_count;
            if (AmbaUtility_StringPrintUInt32(string_output, 255, "CEHU0 %d, CEHU2 %d\n", 2, uint32_args) != 0U) {
                // ignore returned value
            }
            PrintFunc(string_output);
        } else {
            shell_cehu_cmd_usage(ArgCount, pArgVector, PrintFunc);
        }
    }
    return;
}
