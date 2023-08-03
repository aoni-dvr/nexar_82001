/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef __PROTO_CLOCK_H_INCLUDED
#define __PROTO_CLOCK_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <errno.h>
#include <sys/procmgr.h>
#include <drvr/hwinfo.h>
#include <string.h>
#include <hw/ambarella_clk.h>

#include "AmbaSYS.h"
#include "AmbaReg_RCT.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaRTSL_PWC.h"
#include "AmbaCSL_DebugPort.h"

#define AMBA_CORC_BASE_ADDR             0xEd030000
#define AMBA_RCT_BASE_ADDR              0xEd080000

typedef struct {
    int fbdiv;
    int cp;
    int res;
    int lfhf;
    int lock_dly;
    int lock_cnt;
} pll_helper_data_rec_t;
pll_helper_data_rec_t* get_pll_helper_data(int fbdiv);

typedef struct  {
    uintptr_t crl_base;
    AMBA_RCT_REG_s *RegBase;
} clk_dev_t;

extern AMBA_RCT_REG_s * pAmbaRCT_Reg;
extern AMBA_DBG_PORT_ORC_REG_s * pAmbaORC_Reg[4];
extern const int clock_db_size;

int clk_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
int clk_dev_init(clk_dev_t* cinfo);
int clk_dev_dinit(clk_dev_t* cinfo);
const char* pll_id_to_str(pll_id_t id);

unsigned int Amba_Clk_Get_Pll_Freq(pll_id_t srcsel);
int Amba_Clk_SetConfig(ctrl_id_t id, int action);
int Amba_Clk_Set_Freq(ctrl_id_t id, unsigned int freq);
unsigned int Amba_Clk_Get_Freq(ctrl_id_t id);
int Amba_Get_Info(ctrl_id_t id, clk_info_t* info);
int Amba_Clk_Set_Pll_Freq(pll_id_t srcsel, UINT32 freq);
void Amba_Get_BootMode(uint32_t *pBootMode);
void Amba_Clk_EnableFeature(uint32_t Feature);
void Amba_Clk_DisableFeature(uint32_t Feature);
void Amba_Clk_CheckFeature(uint32_t Feature, uint32_t *pEnable);
void Amba_System_Reboot(void);

#define slogerr(...)  slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, __VA_ARGS__)
#define sloginfo(...)  slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, __VA_ARGS__)

typedef struct {
    ctrl_id_t id;
    unsigned int (*SetFunc)(unsigned int); /* pointer to the AmbaRTSL_PllSetXXX */
    unsigned int (*GetFunc)(void); /* pointer to the AmbaRTSL_PllGetXXX */
    unsigned int (*ConfigFunc)(unsigned int); /* pointer to the AmbaRTSL_PllSetXXXClkConfig */
} ctrl_db_t;

extern ctrl_db_t clock_db[];
extern const int clock_db_size;

ctrl_db_t* get_clock_descriptor(ctrl_id_t id);

uint8_t set_pll_src(ctrl_db_t* clk, pll_id_t id);
#endif

