/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright © 2020, Ambarella International LP
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

#include <proto.h>

#define IO_R_D {AMBA_PLL_AUDIO, AMBA_PLL_AUDIO, AMBA_PLL_AUDIO}

/* Database with information about reference clock registers. */
ctrl_db_t clock_db[] = {
    { AMBA_CLK_ADC, NULL, NULL, NULL},
    { AMBA_CLK_AHB, NULL, AmbaRTSL_PllGetAhbClk, NULL},
    { AMBA_CLK_APB, NULL, AmbaRTSL_PllGetApbClk, NULL},
    { AMBA_CLK_AXI, NULL, AmbaRTSL_PllGetAxiClk, NULL},
    { AMBA_CLK_AUDIO, AmbaRTSL_PllSetAudioClk, AmbaRTSL_PllGetAudioClk, NULL},
    { AMBA_CLK_CAN, AmbaRTSL_PllSetCanClk, AmbaRTSL_PllGetCanClk, AmbaRTSL_PllSetCanClkConfig},
    { AMBA_CLK_CORE, AmbaRTSL_PllSetCoreClk, AmbaRTSL_PllGetCoreClk, NULL},
    { AMBA_CLK_CORTEX, NULL, AmbaRTSL_PllGetCortex0Clk, NULL},
    { AMBA_CLK_DEBUGPORT, NULL, AmbaRTSL_PllGetDbgClk, NULL},
    { AMBA_CLK_DEBOUNCE, AmbaRTSL_PllSetDebounceClk, AmbaRTSL_PllGetDebounceClk, NULL},
    { AMBA_CLK_DRAM, NULL, AmbaRTSL_PllGetDramClk, NULL},
    { AMBA_CLK_ENET, AmbaRTSL_PllSetEthernetClk, AmbaRTSL_PllGetEthernetClk, NULL},
    { AMBA_CLK_FEX, AmbaRTSL_PllSetFexClk, AmbaRTSL_PllGetFexClk, NULL},
    { AMBA_CLK_IDSP, AmbaRTSL_PllSetIdspClk, AmbaRTSL_PllGetIdspClk, NULL},
    { AMBA_CLK_MIPIDSI, NULL, NULL, NULL},
    { AMBA_CLK_NAND, AmbaRTSL_PllSetNandClk, AmbaRTSL_PllGetNandClk, NULL},
    { AMBA_CLK_PWM, AmbaRTSL_PllSetPwmClk, AmbaRTSL_PllGetPwmClk, AmbaRTSL_PllSetPwmClkConfig},
    { AMBA_CLK_SD0, AmbaRTSL_PllSetSd0Clk, AmbaRTSL_PllGetSd0Clk, AmbaRTSL_PllSetSd0ScalerClkSrc},
    { AMBA_CLK_SD1, AmbaRTSL_PllSetSd1Clk, AmbaRTSL_PllGetSd1Clk, AmbaRTSL_PllSetSd1ScalerClkSrc},
    { AMBA_CLK_SENSOR0, AmbaRTSL_PllSetSensor0Clk, AmbaRTSL_PllGetSensor0Clk, AmbaRTSL_PllSetSensor0ClkDir},
    { AMBA_CLK_SENSOR1, AmbaRTSL_PllSetSensor1Clk, AmbaRTSL_PllGetSensor1Clk, AmbaRTSL_PllSetVin1ClkConfig},
    { AMBA_CLK_SENSOR2, AmbaRTSL_PllSetSensor2Clk, AmbaRTSL_PllGetSensor2Clk, NULL},
    { AMBA_CLK_SPIMASTER, AmbaRTSL_PllSetSpiMasterClk, AmbaRTSL_PllGetSpiMasterClk, NULL},
    { AMBA_CLK_SPISLAVE, AmbaRTSL_PllSetSpiSlaveClk, AmbaRTSL_PllGetSpiSlaveClk, NULL},
    { AMBA_CLK_SPINOR, AmbaRTSL_PllSetSpiNorClk, AmbaRTSL_PllGetSpiNorClk, NULL},
    { AMBA_CLK_UARTAPB, AmbaRTSL_PllSetUartApbClk, AmbaRTSL_PllGetUartApbClk, AmbaRTSL_PllSetUartApbClkConfig},
    { AMBA_CLK_UART0, AmbaRTSL_PllSetUart0Clk, AmbaRTSL_PllGetUart0Clk, AmbaRTSL_PllSetUart0ClkConfig},
    { AMBA_CLK_UART1, AmbaRTSL_PllSetUart1Clk, AmbaRTSL_PllGetUart1Clk, AmbaRTSL_PllSetUart1ClkConfig},
    { AMBA_CLK_UART2, AmbaRTSL_PllSetUart2Clk, AmbaRTSL_PllGetUart2Clk, AmbaRTSL_PllSetUart2ClkConfig},
    { AMBA_CLK_UART3, AmbaRTSL_PllSetUart3Clk, AmbaRTSL_PllGetUart3Clk, AmbaRTSL_PllSetUart3ClkConfig},
    { AMBA_CLK_USBPHY, NULL, AmbaRTSL_PllGetUsbPhyClk, NULL},
    { AMBA_CLK_VIN0, AmbaRTSL_PllFineAdjSensor0Clk, AmbaRTSL_PllGetVin0Clk, NULL},
    { AMBA_CLK_VIN1, AmbaRTSL_PllFineAdjEthernetClk, AmbaRTSL_PllGetVin1Clk, NULL},
    { AMBA_CLK_VISION, AmbaRTSL_PllSetVisionClk, AmbaRTSL_PllGetVisionClk, NULL},
    { AMBA_CLK_VOUTA, AmbaRTSL_PllSetVout0Clk, AmbaRTSL_PllGetVout0Clk, AmbaRTSL_PllSetVout0ClkConfig},
    { AMBA_CLK_VOUTB, AmbaRTSL_PllSetVout1Clk, AmbaRTSL_PllGetVout1Clk, AmbaRTSL_PllSetVout1ClkConfig},
};

const int clock_db_size = (int)AMBA_CLK_NUM;

const char* pll_id_to_str(pll_id_t id)
{
    switch (id) {
    case AMBA_PLL_AUDIO:
        return "AUDIO";
    case AMBA_PLL_CORE:
        return "CORE";
    case AMBA_PLL_CORTEX1:
        return "CORTEX_A53";
    case AMBA_PLL_DDR:
        return "DDR";
    case AMBA_PLL_ENET:
        return "ENET";
    case AMBA_PLL_IDSP:
        return "IDSP";
    case AMBA_PLL_NAND:
        return "NAND";
    case AMBA_PLL_SD:
        return "SD";
    case AMBA_PLL_SENSOR0:
        return "SENSOR0";
    case AMBA_PLL_SENSOR2:
        return "SENSOR2";
    case AMBA_PLL_VIDEO_A:
        return "VIDEO_A";
    case AMBA_PLL_VIDEO_B:
        return "VIDEO_B";
    case AMBA_PLL_FEX:
        return "FEX";
    case AMBA_PLL_VISION:
        return "VISION";
    default:
        return "unknown";
    }
}

int clk_dev_init(clk_dev_t* cinfo)
{
    uintptr_t virt_base;

    if ((void*)(cinfo->crl_base = (uintptr_t)mmap_device_memory( NULL, 0x1000,
                                  PROT_READ | PROT_WRITE | PROT_NOCACHE, 0,
                                  AMBA_RCT_BASE_ADDR)) == MAP_FAILED) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot access to clock region",
              __FUNCTION__);
        return -1;
    } else {
        cinfo->RegBase = (AMBA_RCT_REG_s *)cinfo->crl_base;
        pAmbaRCT_Reg = cinfo->RegBase;
        AmbaRTSL_PllInit(24000000);
    }

    virt_base = mmap_device_io(0x1000, AMBA_DBG_PORT_CORC_BASE_ADDR);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaORC_Reg[0] = (AMBA_DBG_PORT_ORC_REG_s *)virt_base;

    virt_base = mmap_device_io(0x1000, AMBA_DBG_PORT_EORC_BASE_ADDR);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaORC_Reg[1] = (AMBA_DBG_PORT_ORC_REG_s *)virt_base;

    virt_base = mmap_device_io(0x1000, AMBA_DBG_PORT_DORC_BASE_ADDR);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaORC_Reg[2] = (AMBA_DBG_PORT_ORC_REG_s *)virt_base;

    virt_base = mmap_device_io(0x1000, AMBA_DBG_PORT_VORC_BASE_ADDR);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaORC_Reg[3] = (AMBA_DBG_PORT_ORC_REG_s *)virt_base;

    virt_base = mmap_device_io(0x1000, AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaScratchpadNS_Reg = (AMBA_SCRATCHPAD_NS_REG_s *)virt_base;

    AmbaRTSL_PWC_Init();

    return 0;
}

int clk_dev_dinit(clk_dev_t* cinfo)
{
    if (munmap_device_memory((void*)cinfo->crl_base, 0x1000) < 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): cannot free memory",
              __FUNCTION__);
        return -1;
    }

    munmap_device_io((void*)pAmbaORC_Reg[0], 0x1000);
    munmap_device_io((void*)pAmbaORC_Reg[1], 0x1000);
    munmap_device_io((void*)pAmbaORC_Reg[2], 0x1000);
    munmap_device_io((void*)pAmbaORC_Reg[3], 0x1000);
    munmap_device_io((void*)pAmbaScratchpadNS_Reg, 0x1000);

    return 0;
}

int Amba_Clk_Set_Pll_Freq(pll_id_t srcsel, UINT32 freq)
{
    switch (srcsel) {
    case AMBA_PLL_CORE:
        AmbaRTSL_PllSetCoreClk(freq);
        break;
    case AMBA_PLL_ENET:
        AmbaRTSL_PllSetEthernetClk(freq);
        break;
    case AMBA_PLL_IDSP:
        AmbaRTSL_PllSetIdspClk(freq);
        break;
    case AMBA_PLL_NAND:
        AmbaRTSL_PllSetNandClk(freq);
        break;
    case AMBA_PLL_SD:
        AmbaRTSL_PllSetSdPllOutClk(freq);
        break;
    case AMBA_PLL_SENSOR0:
        AmbaRTSL_PllSetSensor0Clk(freq);
        break;
    case AMBA_PLL_SENSOR2:
        AmbaRTSL_PllSetSensor1Clk(freq);
        break;
    case AMBA_PLL_VIDEO_A:
        AmbaRTSL_PllSetVout0ClkConfig(freq);
        break;
    case AMBA_PLL_VIDEO_B:
        AmbaRTSL_PllSetVout1ClkConfig(freq);
        break;
    case AMBA_PLL_VISION:
        AmbaRTSL_PllSetVisionClk(freq);
        break;
    case AMBA_PLL_FEX:
        AmbaRTSL_PllSetFexClk(freq);
        break;
    default:
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): unknown type of source clock",
              __FUNCTION__);
        return -1;
    }
    return 0;
}

unsigned int Amba_Clk_Get_Pll_Freq(pll_id_t srcsel)
{
    double clock;

    switch (srcsel) {
    case AMBA_PLL_AUDIO:
        clock = AmbaRTSL_PllGetAudioClk();
        break;
    case AMBA_PLL_CORE:
        clock = AmbaRTSL_PllGetCoreClk();
        break;
    case AMBA_PLL_CORTEX1:
        clock = AmbaRTSL_PllGetCortex1Clk();
        break;
    case AMBA_PLL_DDR:
        clock = AmbaRTSL_PllGetDramClk();
        break;
    case AMBA_PLL_ENET:
        clock = AmbaRTSL_PllGetEthernetClk();
        break;
    case AMBA_PLL_FEX:
        clock = AmbaRTSL_PllGetFexClk();
        break;
    case AMBA_PLL_IDSP:
        clock = AmbaRTSL_PllGetIdspClk();
        break;
    case AMBA_PLL_NAND:
        clock = AmbaRTSL_PllGetNandClk();
        break;
    case AMBA_PLL_SD:
        clock = AmbaRTSL_PllGetSd0Clk();
        break;
    case AMBA_PLL_SENSOR0:
        clock = AmbaRTSL_PllGetSensor0Clk();
        break;
    case AMBA_PLL_SENSOR2:
        clock = AmbaRTSL_PllGetSensor1Clk();
        break;
    case AMBA_PLL_VIDEO_A:
        clock = AmbaRTSL_PllGetVout0Clk();
        break;
    case AMBA_PLL_VISION:
        clock = AmbaRTSL_PllGetVisionClk();
        break;
    default:
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s(): unknown type of source clock",
              __FUNCTION__);
        return 0;
    }
    return (unsigned int)clock;
}

ctrl_db_t* get_clock_descriptor(ctrl_id_t id)
{
    int i = id;
    int i_start = i;
    if ( i > clock_db_size) {
        slogerr("%s() : bad clock id(%d. But max is %d)",__FUNCTION__,  i, clock_db_size);
        return NULL;
    }

    do {
        if (clock_db[i].id == id) {
            return &clock_db[i];
        }

        i++;
        if (i >= clock_db_size) {
            i = 0;
        }

    } while (i != i_start);

    return NULL;
}

int Amba_Clk_SetConfig(ctrl_id_t id, int action)
{
    ctrl_db_t* clk = get_clock_descriptor(id);

    if (clk == NULL) {
        slogerr("%s() : Unknown clock required(%d)", __FUNCTION__, id);
        return -1;
    } else {
        if (clk->ConfigFunc != NULL) {
            clk->ConfigFunc(action);
        }
    }

    return 0;
}

int Amba_Clk_Set_Freq(ctrl_id_t id, unsigned int freq)
{
    ctrl_db_t* clk = get_clock_descriptor(id);

    if (clk == NULL) {
        slogerr("%s() : Unknown clock required(%d)", __FUNCTION__, id);
        return -1;
    } else {
        if (clk->SetFunc != NULL) {
            clk->SetFunc(freq);
        }
    }

    return 0;
}


unsigned int Amba_Clk_Get_Freq(ctrl_id_t id)
{
    ctrl_db_t* clk = get_clock_descriptor(id);

    if (clk == NULL) {
        slogerr("%s() : Unknown clock required(%d)", __FUNCTION__, id);
        return 0;
    } else {
        if (clk->GetFunc != NULL) {
            return clk->GetFunc();
        } else {
            return 0;
        }
    }
}

int Amba_Get_Info(ctrl_id_t id, clk_info_t* info)
{
    ctrl_db_t* clk_desc = get_clock_descriptor(id);
    if (clk_desc == NULL) {
        slogerr("%s() : unknown clock id(%d)", __FUNCTION__, id);
        return 0;
    }

    info->freq = Amba_Clk_Get_Freq(id);

    return 0;
}

void Amba_Get_BootMode(uint32_t *pBootMode)
{
    if (pBootMode == NULL) {
        slogerr("%s() : invalid parameter", __FUNCTION__);
    } else {
        *pBootMode = AmbaRTSL_PwcGetBootDevice();
    }
}

void Amba_Clk_EnableFeature(uint32_t Feature)
{
    AmbaRTSL_PllClkEnable(Feature);
}

void Amba_Clk_DisableFeature(uint32_t Feature)
{
    AmbaRTSL_PllClkDisable(Feature);
}

void Amba_Clk_CheckFeature(uint32_t Feature, uint32_t *pEnable)
{
    UINT32 Enable = 0;

    AmbaRTSL_PllClkCheck(Feature, &Enable);
    *pEnable = Enable;
}

void Amba_System_Reboot(void)
{
    //UINT32 BootMode;

    //Amba_Get_BootMode(&BootMode);
    //if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
    //    AmbaSPINOR_RestoreRebootClkSetting();
    //}
#ifdef CONFIG_ENABLE_SPINOR_BOOT
    extern UINT32 AmbaSPINOR_SWReset(void);
    (void) AmbaSPINOR_SWReset();
#endif
    /* Trigger soc soft-reset */

    AmbaRTSL_PwcReboot();
}

