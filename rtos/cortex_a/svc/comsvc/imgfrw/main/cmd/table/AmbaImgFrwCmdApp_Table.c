/**
 *  @file AmbaImgFrwCmdApp_Table.c
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaShell Image framework Command App Table
 *
 */

static void ambsh_imgfrw_app(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_prt_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_dbg_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_ae_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_ae_lock_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_ae_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_awb_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_awb_lock_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_awb_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_adj_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_adj_lock_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_adj_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_stat_mask(UINT32 Argc, char *const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_stat_inter(UINT32 Argc, char *const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_sync_en(UINT32 Argc, char *const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_sync_margin(UINT32 Argc, char *const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
#ifdef CONFIG_BUILD_IMGFRW_SMC
static void ambsh_imgfrw_app_ae_smc_on(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_ae_smc_off(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif
static void ambsh_imgfrw_app_ae_stable_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
#ifndef CONFIG_BUILD_IMGFRW_EIS
static void ambsh_imgfrw_app_eis_dmy_param(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_app_eis_dmy_ms(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif
static AMBA_IMG_FRW_CMD_s AmbaImgFrwCmdApp_List[] = {
    { AMBSH_MAGIC, "load",           ambsh_imgfrw_app_load,           NULL },
    { AMBSH_MAGIC, "prt_en",         ambsh_imgfrw_app_prt_en,         NULL },
    { AMBSH_MAGIC, "dbg_en",         ambsh_imgfrw_app_dbg_en,         NULL },
    { AMBSH_MAGIC, "ae_lock",        ambsh_imgfrw_app_ae_lock,        NULL },
    { AMBSH_MAGIC, "ae_lock_wait",   ambsh_imgfrw_app_ae_lock_wait,   NULL },
    { AMBSH_MAGIC, "ae_unlock",      ambsh_imgfrw_app_ae_unlock,      NULL },
    { AMBSH_MAGIC, "awb_lock",       ambsh_imgfrw_app_awb_lock,       NULL },
    { AMBSH_MAGIC, "awb_lock_wait",  ambsh_imgfrw_app_awb_lock_wait,  NULL },
    { AMBSH_MAGIC, "awb_unlock",     ambsh_imgfrw_app_awb_unlock,     NULL },
    { AMBSH_MAGIC, "adj_lock",       ambsh_imgfrw_app_adj_lock,       NULL },
    { AMBSH_MAGIC, "adj_lock_wait",  ambsh_imgfrw_app_adj_lock_wait,  NULL },
    { AMBSH_MAGIC, "adj_unlock",     ambsh_imgfrw_app_adj_unlock,     NULL },
#ifdef CONFIG_BUILD_IMGFRW_SMC
    { AMBSH_MAGIC, "ae_smc_on",      ambsh_imgfrw_app_ae_smc_on,      NULL },
    { AMBSH_MAGIC, "ae_smc_off",     ambsh_imgfrw_app_ae_smc_off,     NULL },
#endif
    { AMBSH_MAGIC, "ae_stable_wait", ambsh_imgfrw_app_ae_stable_wait, NULL },
#ifndef CONFIG_BUILD_IMGFRW_EIS
    { AMBSH_MAGIC, "eis_dmy_param",  ambsh_imgfrw_app_eis_dmy_param,  NULL },
    { AMBSH_MAGIC, "eis_dmy_ms",     ambsh_imgfrw_app_eis_dmy_ms,     NULL },
#endif
    { AMBSH_MAGIC, "stat_mask",      ambsh_imgfrw_app_stat_mask,      NULL },
    { AMBSH_MAGIC, "stat_inter",     ambsh_imgfrw_app_stat_inter,     NULL },
    { AMBSH_MAGIC, "sync_en",        ambsh_imgfrw_app_sync_en,        NULL },
    { AMBSH_MAGIC, "sync_margin",    ambsh_imgfrw_app_sync_margin,    NULL }
};
