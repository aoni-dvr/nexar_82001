/**
 *  @file AmbaImgFrwCmdFlow_Table.c
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
 *  @details AmbaShell Image framework Command Flow Table
 *
 */

static void ambsh_imgfrw_flow(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);

static void ambsh_imgfrw_flow_vin_cap(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_vin_sensor(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_pipe_mode(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_pipe_out(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_pipe_hdr(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);

static void ambsh_imgfrw_flow_chan_select(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_sensor(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_algo(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_zone(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_zone_msb(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_inter(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_vr(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_iq(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_fov(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_fov_iq(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_avm(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_avm_iq(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_eis(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_post(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_post_add(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_chan_aeb(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);

static void ambsh_imgfrw_flow_liv_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_liv_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_liv_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_liv_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_fov_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_fov_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_avm_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_avm_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_eis_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_eis_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_sync_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_sync_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_stat_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_stat_inter(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
static void ambsh_imgfrw_flow_stat_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);

static AMBA_IMG_FRW_CMD_s AmbaImgFrwCmdFlow_List[] = {
    { AMBSH_MAGIC, "load",          ambsh_imgfrw_flow_load,          NULL },
    { AMBSH_MAGIC, "vin_cap",       ambsh_imgfrw_flow_vin_cap,       NULL },
    { AMBSH_MAGIC, "vin_sensor",    ambsh_imgfrw_flow_vin_sensor,    NULL },
    { AMBSH_MAGIC, "pipe_mode",     ambsh_imgfrw_flow_pipe_mode,     NULL },
    { AMBSH_MAGIC, "pipe_out",      ambsh_imgfrw_flow_pipe_out,      NULL },
    { AMBSH_MAGIC, "pipe_hdr",      ambsh_imgfrw_flow_pipe_hdr,      NULL },
    { AMBSH_MAGIC, "chan_select",   ambsh_imgfrw_flow_chan_select,   NULL },
    { AMBSH_MAGIC, "chan_sensor",   ambsh_imgfrw_flow_chan_sensor,   NULL },
    { AMBSH_MAGIC, "chan_algo",     ambsh_imgfrw_flow_chan_algo,     NULL },
    { AMBSH_MAGIC, "chan_zone",     ambsh_imgfrw_flow_chan_zone,     NULL },
    { AMBSH_MAGIC, "chan_zone_msb", ambsh_imgfrw_flow_chan_zone_msb, NULL },
    { AMBSH_MAGIC, "chan_inter",    ambsh_imgfrw_flow_chan_inter,    NULL },
    { AMBSH_MAGIC, "chan_vr",       ambsh_imgfrw_flow_chan_vr,       NULL },
    { AMBSH_MAGIC, "chan_iq",       ambsh_imgfrw_flow_chan_iq,       NULL },
    { AMBSH_MAGIC, "chan_fov",      ambsh_imgfrw_flow_chan_fov,      NULL },
    { AMBSH_MAGIC, "chan_fov_iq",   ambsh_imgfrw_flow_chan_fov_iq,   NULL },
    { AMBSH_MAGIC, "chan_avm",      ambsh_imgfrw_flow_chan_avm,      NULL },
    { AMBSH_MAGIC, "chan_avm_iq",   ambsh_imgfrw_flow_chan_avm_iq,   NULL },
    { AMBSH_MAGIC, "chan_eis",      ambsh_imgfrw_flow_chan_eis,      NULL },
    { AMBSH_MAGIC, "chan_post",     ambsh_imgfrw_flow_chan_post,     NULL },
    { AMBSH_MAGIC, "chan_post_add", ambsh_imgfrw_flow_chan_post_add, NULL },
    { AMBSH_MAGIC, "chan_aeb",      ambsh_imgfrw_flow_chan_aeb,      NULL },
    { AMBSH_MAGIC, "liv_start",     ambsh_imgfrw_flow_liv_start,     NULL },
    { AMBSH_MAGIC, "liv_stop",      ambsh_imgfrw_flow_liv_stop,      NULL },
    { AMBSH_MAGIC, "liv_lock",      ambsh_imgfrw_flow_liv_lock,      NULL },
    { AMBSH_MAGIC, "liv_unlock",    ambsh_imgfrw_flow_liv_unlock,    NULL },
    { AMBSH_MAGIC, "fov_start",     ambsh_imgfrw_flow_fov_start,     NULL },
    { AMBSH_MAGIC, "fov_stop",      ambsh_imgfrw_flow_fov_stop,      NULL },
    { AMBSH_MAGIC, "avm_start",     ambsh_imgfrw_flow_avm_start,     NULL },
    { AMBSH_MAGIC, "avm_stop",      ambsh_imgfrw_flow_avm_stop,      NULL },
    { AMBSH_MAGIC, "eis_start",     ambsh_imgfrw_flow_eis_start,     NULL },
    { AMBSH_MAGIC, "eis_stop",      ambsh_imgfrw_flow_eis_stop,      NULL },
    { AMBSH_MAGIC, "sync_start",    ambsh_imgfrw_flow_sync_start,    NULL },
    { AMBSH_MAGIC, "sync_stop",     ambsh_imgfrw_flow_sync_stop,     NULL },
    { AMBSH_MAGIC, "stat_start",    ambsh_imgfrw_flow_stat_start,    NULL },
    { AMBSH_MAGIC, "stat_inter",    ambsh_imgfrw_flow_stat_inter,    NULL },
    { AMBSH_MAGIC, "stat_stop",     ambsh_imgfrw_flow_stat_stop,     NULL }
};
