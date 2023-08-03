#include <stdio.h>
#include <string.h>
#include "AmbaVOUT.h"
#include "AmbaVOUT_Ctrl.h"
#include "AmbaRTSL_VOUT.h"
#include "AmbaReg_VOUT.h"
#include "AmbaReg_RCT.h"
#include "AmbaDrvEntry.h"
#include "AmbaKAL.h"

AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;

AMBA_VOUT_DISPLAY_CONFIG_REG_s AmbaVoutDisplay0_Reg = {0};
AMBA_VOUT_DISPLAY_CONFIG_REG_s *    const pAmbaVoutDisplay0_Reg = &AmbaVoutDisplay0_Reg;

AMBA_VOUT_DISPLAY_CONFIG_REG_s AmbaVoutDisplay1_Reg = {0};
AMBA_VOUT_DISPLAY_CONFIG_REG_s *    const pAmbaVoutDisplay1_Reg = &AmbaVoutDisplay1_Reg;

AMBA_VOUT_MIPI_DSI_COMMAND_REG_s AmbaVoutMipiDsiCmd0_Reg = {0};
AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  const pAmbaVoutMipiDsiCmd0_Reg = &AmbaVoutMipiDsiCmd0_Reg;

AMBA_VOUT_MIPI_DSI_COMMAND_REG_s AmbaVoutMipiDsiCmd1_Reg = {0};
AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  const pAmbaVoutMipiDsiCmd1_Reg = &AmbaVoutMipiDsiCmd1_Reg;

AMBA_VOUT_TOP_REG_s AmbaVoutTop_Reg = {0};
AMBA_VOUT_TOP_REG_s *               const pAmbaVoutTop_Reg = &AmbaVoutTop_Reg;

extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);
extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexGive(UINT32 RetVal);
extern void Set_RetVal_pow(UINT32 RetVal);
extern void Set_RetVal_ceil(UINT32 RetVal);
extern void Set_RetVal_floor(UINT32 RetVal);
extern void Set_RetVal_memset(UINT32 RetVal);

void TEST_AmbaVout_ChannelSelect(UINT32 VoutChannel)
{
    AmbaVout_ChannelSelect(VoutChannel);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_ChannelSelect(VoutChannel);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_ChannelSelect(VoutChannel);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_ChannelSelect\n");
}

void TEST_AmbaVout_GetDisp0ConfigAddr(void)
{
    AmbaVout_GetDisp0ConfigAddr(NULL, NULL);

    ULONG VirtAddr = 0U;
    ULONG *pVirtAddr = &VirtAddr;
    UINT32 Size = 0U;
    UINT32 *pSize = &Size;
    AmbaVout_GetDisp0ConfigAddr(pVirtAddr, pSize);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetDisp0ConfigAddr(pVirtAddr, pSize);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetDisp0ConfigAddr(pVirtAddr, pSize);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_GetDisp0ConfigAddr\n");
}

void TEST_AmbaVout_GetDisp1ConfigAddr(void)
{
    AmbaVout_GetDisp1ConfigAddr(NULL, NULL);

    ULONG VirtAddr = 0U;
    ULONG *pVirtAddr = &VirtAddr;
    UINT32 Size = 0U;
    UINT32 *pSize = &Size;
    AmbaVout_GetDisp1ConfigAddr(pVirtAddr, pSize);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetDisp1ConfigAddr(pVirtAddr, pSize);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetDisp1ConfigAddr(pVirtAddr, pSize);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_GetDisp1ConfigAddr\n");
}

void TEST_AmbaVout_GetDisp0DigiCscAddr(void)
{
    AmbaVout_GetDisp0DigiCscAddr(NULL, NULL);

    ULONG VirtAddr = 0U;
    ULONG *pVirtAddr = &VirtAddr;
    UINT32 Size = 0U;
    UINT32 *pSize = &Size;
    AmbaVout_GetDisp0DigiCscAddr(pVirtAddr, pSize);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetDisp0DigiCscAddr(pVirtAddr, pSize);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetDisp0DigiCscAddr(pVirtAddr, pSize);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_GetDisp0DigiCscAddr\n");
}

void TEST_AmbaVout_GetDisp1AnalogCscAddr(void)
{
    AmbaVout_GetDisp1AnalogCscAddr(NULL, NULL);

    ULONG VirtAddr = 0U;
    ULONG *pVirtAddr = &VirtAddr;
    UINT32 Size = 0U;
    UINT32 *pSize = &Size;
    AmbaVout_GetDisp1AnalogCscAddr(pVirtAddr, pSize);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetDisp1AnalogCscAddr(pVirtAddr, pSize);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetDisp1AnalogCscAddr(pVirtAddr, pSize);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_GetDisp1AnalogCscAddr\n");
}

void TEST_AmbaVout_GetDisp1DigiCscAddr(void)
{
    AmbaVout_GetDisp1DigiCscAddr(NULL, NULL);

    ULONG VirtAddr = 0U;
    ULONG *pVirtAddr = &VirtAddr;
    UINT32 Size = 0U;
    UINT32 *pSize = &Size;
    AmbaVout_GetDisp1DigiCscAddr(pVirtAddr, pSize);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetDisp1DigiCscAddr(pVirtAddr, pSize);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetDisp1DigiCscAddr(pVirtAddr, pSize);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_GetDisp1DigiCscAddr\n");
}

void TEST_AmbaVout_GetDisp1HdmiCscAddr(void)
{
    ULONG VirtAddr = 0U;
    UINT32 pSize = 0U;
    AmbaVout_GetDisp1HdmiCscAddr(&VirtAddr, &pSize);

    printf("TEST_AmbaVout_GetDisp1HdmiCscAddr\n");
}

void TEST_AmbaVout_SetDisp0VinVoutSync(void)
{
    UINT32 EnableFlag = 2U;
    AmbaVout_SetDisp0VinVoutSync(EnableFlag);

    EnableFlag = 1U;
    AmbaVout_SetDisp0VinVoutSync(EnableFlag);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_SetDisp0VinVoutSync(EnableFlag);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_SetDisp0VinVoutSync(EnableFlag);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_SetDisp0VinVoutSync\n");
}


void TEST_AmbaVout_SetDisp1VinVoutSync(void)
{
    UINT32 EnableFlag = 2U;
    AmbaVout_SetDisp1VinVoutSync(EnableFlag);

    EnableFlag = 1U;
    AmbaVout_SetDisp1VinVoutSync(EnableFlag);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_SetDisp1VinVoutSync(EnableFlag);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_SetDisp1VinVoutSync(EnableFlag);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_SetDisp1VinVoutSync\n");
}

void TEST_AmbaVout_DigiYccEnable(void)
{
    UINT32 YccMode = 0U;
    UINT32 ColorOrder = 0U;
    AMBA_VOUT_DATA_LATCH_CONFIG_s DataLatch;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AmbaVout_DigiYccEnable(YccMode, ColorOrder, &DataLatch, &DisplayTiming);

    printf("TEST_AmbaVout_DigiYccEnable\n");
}

void TEST_AmbaVout_DigiYccSetPhyCtrl(void)
{
    UINT32 ParamID = 0U;
    UINT32 ParamVal = 0U;
    AmbaVout_DigiYccSetPhyCtrl(ParamID, ParamVal);

    printf("TEST_AmbaVout_DigiYccSetPhyCtrl\n");
}

void TEST_AmbaVout_DigiYccGetStatus(void)
{
    AMBA_VOUT_DIGITAL_YCC_CONFIG_s DigiYccConfig;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AmbaVout_DigiYccGetStatus(&DigiYccConfig, &DisplayTiming);

    printf("TEST_AmbaVout_DigiYccGetStatus\n");
}

void TEST_AmbaVout_DigiYccSetCscMatrix(void)
{
    AMBA_VOUT_CSC_MATRIX_s CscMatrix;
    UINT32 YccMode = 0U;
    AmbaVout_DigiYccSetCscMatrix(&CscMatrix, YccMode);

    printf("TEST_AmbaVout_DigiYccSetCscMatrix\n");
}

void TEST_AmbaVout_DigiRgbEnable(void)
{
    UINT32 RgbMode = 0U;
    UINT32 ColorOrder = 0U;
    AMBA_VOUT_DATA_LATCH_CONFIG_s DataLatch;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AmbaVout_DigiRgbEnable(RgbMode, ColorOrder, &DataLatch, &DisplayTiming);

    printf("TEST_AmbaVout_DigiRgbEnable\n");
}

void TEST_AmbaVout_DigiRgbSetPhyCtrl(void)
{
    UINT32 ParamID = 0U;
    UINT32 ParamVal = 0U;
    AmbaVout_DigiRgbSetPhyCtrl(ParamID, ParamVal);

    printf("TEST_AmbaVout_DigiRgbSetPhyCtrl\n");
}

void TEST_AmbaVout_DigiRgbGetStatus(void)
{
    AMBA_VOUT_DIGITAL_RGB_CONFIG_s DigiRgbConfig;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AmbaVout_DigiRgbGetStatus(&DigiRgbConfig, &DisplayTiming);

    printf("TEST_AmbaVout_DigiRgbGetStatus\n");
}

void TEST_AmbaVout_DigiRgbSetCscMatrix(void)
{
    AMBA_VOUT_CSC_MATRIX_s CscMatrix;
    UINT32 RgbMode;
    AmbaVout_DigiRgbSetCscMatrix(&CscMatrix, RgbMode);

    printf("TEST_AmbaVout_DigiRgbSetCscMatrix\n");
}

void TEST_AmbaVout_FpdLinkEnable(void)
{
    UINT32 RgbMode = 0U;
    UINT32 ColorOrder = 0U;
    UINT32 DataEnablePolarity = 0U;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AmbaVout_FpdLinkEnable(RgbMode, ColorOrder, DataEnablePolarity, &DisplayTiming);

    printf("TEST_AmbaVout_FpdLinkEnable\n");
}

void TEST_AmbaVout_FpdLinkSetPhyCtrl(void)
{
    UINT32 ParamID = 0U;
    UINT32 ParamVal = 0U;
    AmbaVout_FpdLinkSetPhyCtrl(ParamID, ParamVal);

    printf("TEST_AmbaVout_FpdLinkSetPhyCtrl\n");
}

void TEST_AmbaVout_FpdLinkGetStatus(void)
{
    AMBA_VOUT_FPD_LINK_CONFIG_s FpdLinkConfig;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AmbaVout_FpdLinkGetStatus(&FpdLinkConfig, &DisplayTiming);

    printf("TEST_AmbaVout_FpdLinkGetStatus\n");
}

void TEST_AmbaVout_FpdLinkSetCscMatrix(void)
{
    AMBA_VOUT_CSC_MATRIX_s CscMatrix;
    UINT32 FpdMode = 0U;
    AmbaVout_FpdLinkSetCscMatrix(&CscMatrix, FpdMode);

    printf("TEST_AmbaVout_FpdLinkSetCscMatrix\n");
}

typedef struct {
    UINT32 MipiCsiMode;
    UINT32 ColorOrder;
    UINT32 PixelClkFreq;
    UINT16 HsyncFrontPorch;
    UINT16 HsyncPulseWidth;
    UINT16 HsyncBackPorch;
    UINT16 VsyncFrontPorch;
    UINT16 VsyncPulseWidth;
    UINT16 VsyncBackPorch;
    UINT16 ActivePixels;
    UINT16 ActiveLines;
} MIPI_CSI_ENABLE_TC_s;

#define VOUT_REG_MAX_VALUE   (UINT16)0x3fff  /* 14 bits for vout timing paremeters */

/* test case for input param, [0]: golden parameter  */
MIPI_CSI_ENABLE_TC_s MipiCsiEnableTC[] = {
    {.MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,
     .HsyncFrontPorch = 30U, .HsyncPulseWidth = 2U, .HsyncBackPorch = 2U, .ActivePixels = 1U,
     .VsyncFrontPorch = 30U, .VsyncPulseWidth = 2U, .VsyncBackPorch = 2U, .ActiveLines = 1U,},
    {.MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,
     .HsyncFrontPorch = 30U, .HsyncPulseWidth = 2U, .HsyncBackPorch = VOUT_REG_MAX_VALUE + 1U, .ActivePixels = 1U,
     .VsyncFrontPorch = 30U, .VsyncPulseWidth = 2U, .VsyncBackPorch = VOUT_REG_MAX_VALUE + 1U, .ActiveLines = 1U,},
    {.MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = 0U,},
    {.MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK,},
    {.MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER,      .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE,       .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiCsiMode = VOUT_MIPI_CSI_MODE_422_1LANE, .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiCsiMode = VOUT_MIPI_CSI_MODE_422_2LANE, .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,},
    /* For AmbaVout_MipiCsiGetBypass in AmbaRTSL_VOUT_LCD.c */
    {.MipiCsiMode = 0x1U << 29U,                  .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,},
    /* For AmbaVout_MipiGetDphyClkMode in AmbaRTSL_VOUT_LCD.c */
    {.MipiCsiMode = 0x1U << 31U,                  .ColorOrder = VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_CSI_MAX_PIXEL_CLOCK - 1U,},
};

void MipiCsiEnable_DisplayTiming_Get(UINT32 Index, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    memset(pDisplayTiming, 0, sizeof(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s));
    pDisplayTiming->PixelClkFreq    = MipiCsiEnableTC[Index].PixelClkFreq;
    pDisplayTiming->HsyncFrontPorch = MipiCsiEnableTC[Index].HsyncFrontPorch;
    pDisplayTiming->HsyncPulseWidth = MipiCsiEnableTC[Index].HsyncPulseWidth;
    pDisplayTiming->HsyncBackPorch  = MipiCsiEnableTC[Index].HsyncBackPorch;
    pDisplayTiming->ActivePixels    = MipiCsiEnableTC[Index].ActivePixels;
    pDisplayTiming->VsyncFrontPorch = MipiCsiEnableTC[Index].VsyncFrontPorch;
    pDisplayTiming->VsyncPulseWidth = MipiCsiEnableTC[Index].VsyncPulseWidth;
    pDisplayTiming->VsyncBackPorch  = MipiCsiEnableTC[Index].VsyncBackPorch;
    pDisplayTiming->ActiveLines     = MipiCsiEnableTC[Index].ActiveLines;
}

void TEST_AmbaVout_MipiCsiEnable(void)
{
    UINT32 i;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming = {0};

    AmbaVout_MipiCsiEnable(MipiCsiEnableTC[0].MipiCsiMode, MipiCsiEnableTC[0].ColorOrder, NULL);

    for (i = 0U; i < (sizeof(MipiCsiEnableTC) / (UINT32) sizeof(MIPI_CSI_ENABLE_TC_s)); i ++ ) {
        MipiCsiEnable_DisplayTiming_Get(i, &DisplayTiming);
        AmbaVout_MipiCsiEnable(MipiCsiEnableTC[i].MipiCsiMode, MipiCsiEnableTC[i].ColorOrder, &DisplayTiming);
    }

    MipiCsiEnable_DisplayTiming_Get(0U, &DisplayTiming);
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiCsiEnable(MipiCsiEnableTC[0].MipiCsiMode, MipiCsiEnableTC[0].ColorOrder, &DisplayTiming);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    MipiCsiEnable_DisplayTiming_Get(0U, &DisplayTiming);
    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiCsiEnable(MipiCsiEnableTC[0].MipiCsiMode, MipiCsiEnableTC[0].ColorOrder, &DisplayTiming);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiCsiEnable\n");
}

void TEST_AmbaVout_MipiCsiSetPhyCtrl(void)
{
    UINT32 ParamID = VOUT_NUM_PHY_PARAM;
    UINT32 ParamVal = 0U;
    AmbaVout_MipiCsiSetPhyCtrl(ParamID, ParamVal);

    ParamID = VOUT_NUM_PHY_PARAM - 1U;
    AmbaVout_MipiCsiSetPhyCtrl(ParamID, ParamVal);

    AmbaVout_MipiCsiSetPhyCtrl(ParamID, ParamVal);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiCsiSetPhyCtrl(ParamID, ParamVal);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiCsiSetPhyCtrl(ParamID, ParamVal);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiCsiSetPhyCtrl\n");
}


void TEST_AmbaVout_MipiCsiGetStatus(void)
{
    UINT32 NumMipiLane = 0U;
    AmbaVout_MipiCsiGetStatus(NULL, NULL);

    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming = &DisplayTiming;
    AmbaVout_MipiCsiGetStatus(NULL, pDisplayTiming);

    AMBA_VOUT_MIPI_CSI_CONFIG_s MipiCsiConfig;
    AMBA_VOUT_MIPI_CSI_CONFIG_s *pMipiCsiConfig = &MipiCsiConfig;
    AmbaVout_MipiCsiGetStatus(pMipiCsiConfig, pDisplayTiming);

    /* For NULL pointer check in AmbaRTSL_VOUT_LCD.c */
    AmbaVout_MipiCsiGetStatus(pMipiCsiConfig, NULL);

    pAmbaVoutDisplay0_Reg->DigitalOutputMode.OutputMode = 11U;
    for (NumMipiLane = 0U; NumMipiLane <= 2U; NumMipiLane ++) {
        pAmbaVoutDisplay0_Reg->DispCtrl.NumMipiLane = NumMipiLane;
        pAmbaVoutDisplay1_Reg->DispCtrl.NumMipiLane = NumMipiLane;
        AmbaVout_MipiCsiGetStatus(pMipiCsiConfig, pDisplayTiming);
    }

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiCsiGetStatus(pMipiCsiConfig, pDisplayTiming);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiCsiGetStatus(pMipiCsiConfig, pDisplayTiming);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiCsiGetStatus\n");
}


void TEST_AmbaVout_MipiCsiSetCscMatrix(void)
{
    UINT32 MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE;
    AmbaVout_MipiCsiSetCscMatrix(NULL, MipiCsiMode);

    AMBA_VOUT_CSC_MATRIX_s CscMatrix;
    AMBA_VOUT_CSC_MATRIX_s * pCscMatrix = &CscMatrix;
    AmbaVout_MipiCsiSetCscMatrix(pCscMatrix, MipiCsiMode);

    MipiCsiMode = VOUT_NUM_MIPI_CSI_MODE - 1U;
    AmbaVout_MipiCsiSetCscMatrix(pCscMatrix, MipiCsiMode);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiCsiSetCscMatrix(pCscMatrix, MipiCsiMode);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiCsiSetCscMatrix(pCscMatrix, MipiCsiMode);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiCsiSetCscMatrix\n");
}


typedef struct {
    UINT32 MipiDsiMode;
    UINT32 ColorOrder;
    UINT32 PixelClkFreq;
    UINT16 HsyncFrontPorch;
    UINT16 HsyncPulseWidth;
    UINT16 HsyncBackPorch;
    UINT16 VsyncFrontPorch;
    UINT16 VsyncPulseWidth;
    UINT16 VsyncBackPorch;
    UINT16 ActivePixels;
    UINT16 ActiveLines;
} MIPI_DSI_ENABLE_TC_s;

/* test case for input param, [0]: golden parameter  */
MIPI_DSI_ENABLE_TC_s MipiDsiEnableTC[] = {
    {.MipiDsiMode = VOUT_NUM_MIPI_DSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK / 100U,
     .HsyncFrontPorch = 30U, .HsyncPulseWidth = 2U, .HsyncBackPorch = 2U, .ActivePixels = 1U,
     .VsyncFrontPorch = 30U, .VsyncPulseWidth = 2U, .VsyncBackPorch = 2U, .ActiveLines = 1U,},
    {.MipiDsiMode = VOUT_NUM_MIPI_DSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = 0U,},
    {.MipiDsiMode = VOUT_NUM_MIPI_DSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK,},
    {.MipiDsiMode = VOUT_NUM_MIPI_DSI_MODE - 1U,  .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER,      .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_NUM_MIPI_DSI_MODE,       .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_422_1LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_422_2LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_422_4LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_565_1LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_565_2LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_565_4LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_666_1LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_666_2LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_666_4LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_888_1LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_888_2LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    {.MipiDsiMode = VOUT_MIPI_DSI_MODE_888_4LANE, .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK - 1U,},
    /* For AmbaVout_MipiGetEotpMode in AmbaRTSL_VOUT_LCD.c */
    {.MipiDsiMode = 0x1U << 30U,                  .ColorOrder = VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, .PixelClkFreq = VOUT_MIPI_DSI_MAX_PIXEL_CLOCK / 100U,},
};

void MipiDsiEnable_DisplayTiming_Get(UINT32 Index, AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming)
{
    memset(pDisplayTiming, 0, sizeof(AMBA_VOUT_DISPLAY_TIMING_CONFIG_s));
    pDisplayTiming->PixelClkFreq    = MipiDsiEnableTC[Index].PixelClkFreq;
    pDisplayTiming->HsyncFrontPorch = MipiDsiEnableTC[Index].HsyncFrontPorch;
    pDisplayTiming->HsyncPulseWidth = MipiDsiEnableTC[Index].HsyncPulseWidth;
    pDisplayTiming->HsyncBackPorch  = MipiDsiEnableTC[Index].HsyncBackPorch;
    pDisplayTiming->ActivePixels    = MipiDsiEnableTC[Index].ActivePixels;
    pDisplayTiming->VsyncFrontPorch = MipiDsiEnableTC[Index].VsyncFrontPorch;
    pDisplayTiming->VsyncPulseWidth = MipiDsiEnableTC[Index].VsyncPulseWidth;
    pDisplayTiming->VsyncBackPorch  = MipiDsiEnableTC[Index].VsyncBackPorch;
    pDisplayTiming->ActiveLines    = MipiDsiEnableTC[Index].ActiveLines;
}

void TEST_AmbaVout_MipiDsiEnable(void)
{
    UINT32 i;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming = {0};

    AmbaVout_MipiDsiEnable(MipiDsiEnableTC[0].MipiDsiMode, MipiDsiEnableTC[0].ColorOrder, NULL);

    for (i = 0; i < (sizeof(MipiDsiEnableTC) / (UINT32) sizeof(MIPI_DSI_ENABLE_TC_s)); i ++ ) {
        MipiDsiEnable_DisplayTiming_Get(i, &DisplayTiming);
        AmbaVout_MipiDsiEnable(MipiDsiEnableTC[i].MipiDsiMode, MipiDsiEnableTC[i].ColorOrder, &DisplayTiming);
    }

    MipiDsiEnable_DisplayTiming_Get(0U, &DisplayTiming);
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiEnable(MipiDsiEnableTC[0].MipiDsiMode, MipiDsiEnableTC[0].ColorOrder, &DisplayTiming);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    MipiDsiEnable_DisplayTiming_Get(0U, &DisplayTiming);
    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiEnable(MipiDsiEnableTC[0].MipiDsiMode, MipiDsiEnableTC[0].ColorOrder, &DisplayTiming);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    /* For BoundaryCheck in AmbaRTSL_VOUT_LCD.c */
    Set_RetVal_pow(1U);
    AmbaVout_MipiDsiEnable(MipiDsiEnableTC[0].MipiDsiMode, MipiDsiEnableTC[0].ColorOrder, &DisplayTiming);
    Set_RetVal_pow(0U);

    /* For VOUT_CalculateMphyConfig in AmbaRTSL_VOUT_LCD.c */
    Set_RetVal_ceil(1U);
    AmbaVout_MipiDsiEnable(MipiDsiEnableTC[0].MipiDsiMode, MipiDsiEnableTC[0].ColorOrder, &DisplayTiming);
    Set_RetVal_ceil(0U);

    printf("TEST_AmbaVout_MipiDsiEnable\n");
}

void TEST_AmbaVout_MipiDsiSetPhyCtrl(void)
{
    UINT32 ParamID = VOUT_NUM_PHY_PARAM;
    UINT32 ParamVal = 0U;
    AmbaVout_MipiDsiSetPhyCtrl(ParamID, ParamVal);

    ParamID = VOUT_NUM_PHY_PARAM - 1U;
    AmbaVout_MipiDsiSetPhyCtrl(ParamID, ParamVal);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiSetPhyCtrl(ParamID, ParamVal);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiSetPhyCtrl(ParamID, ParamVal);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiDsiSetPhyCtrl\n");
}


void TEST_AmbaVout_MipiDsiGetStatus(void)
{
    UINT8 OutputMode = 0U;
    AmbaVout_MipiDsiGetStatus(NULL, NULL);

    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s *pDisplayTiming = &DisplayTiming;
    AmbaVout_MipiDsiGetStatus(NULL, pDisplayTiming);

    AMBA_VOUT_MIPI_DSI_CONFIG_s MipiDsiConfig;
    AMBA_VOUT_MIPI_DSI_CONFIG_s *pMipiDsiConfig = &MipiDsiConfig;
    AmbaVout_MipiDsiGetStatus(pMipiDsiConfig, pDisplayTiming);

    /* For NULL pointer check in AmbaRTSL_VOUT_LCD.c */
    AmbaVout_MipiDsiGetStatus(pMipiDsiConfig, NULL);

    for (OutputMode = 11U; OutputMode <= 14U; OutputMode ++) {
        pAmbaVoutDisplay0_Reg->DigitalOutputMode.OutputMode = OutputMode;
        pAmbaVoutDisplay1_Reg->DigitalOutputMode.OutputMode = OutputMode;
        AmbaVout_MipiDsiGetStatus(pMipiDsiConfig, pDisplayTiming);
    }

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiGetStatus(pMipiDsiConfig, pDisplayTiming);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiGetStatus(pMipiDsiConfig, pDisplayTiming);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiDsiGetStatus\n");
}


void TEST_AmbaVout_MipiDsiSetCscMatrix(void)
{
    UINT32 MipiDsiMode = VOUT_NUM_MIPI_DSI_MODE;
    AmbaVout_MipiDsiSetCscMatrix(NULL, MipiDsiMode);

    AMBA_VOUT_CSC_MATRIX_s CscMatrix;
    AMBA_VOUT_CSC_MATRIX_s * pCscMatrix = &CscMatrix;
    AmbaVout_MipiDsiSetCscMatrix(pCscMatrix, MipiDsiMode);

    MipiDsiMode = VOUT_NUM_MIPI_DSI_MODE - 1U;
    AmbaVout_MipiDsiSetCscMatrix(pCscMatrix, MipiDsiMode);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiSetCscMatrix(pCscMatrix, MipiDsiMode);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiSetCscMatrix(pCscMatrix, MipiDsiMode);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiDsiSetCscMatrix\n");
}

void TEST_AmbaVout_MipiDsiSendDcsWrite(void)
{
    UINT32 DcsCmd = 0U;
    UINT32 NumParam = VOUT_DSI_CMD_MAX_PARAM_NUM + 1U;
    AmbaVout_MipiDsiSendDcsWrite(DcsCmd, NumParam, NULL);

    NumParam = VOUT_DSI_CMD_MAX_PARAM_NUM;
    AmbaVout_MipiDsiSendDcsWrite(DcsCmd, NumParam, NULL);

    UINT8 Param = 0U;
    UINT8 * pParam = &Param;
    AmbaVout_MipiDsiSendDcsWrite(DcsCmd, NumParam, pParam);

    NumParam = 0U;
    AmbaVout_MipiDsiSendDcsWrite(DcsCmd, NumParam, pParam);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiSendDcsWrite(DcsCmd, NumParam, pParam);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiSendDcsWrite(DcsCmd, NumParam, pParam);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_MipiDsiSendDcsWrite\n");
}

void TEST_AmbaVout_MipiDsiSendNormalWrite(void)
{
    UINT32 NumParam = VOUT_DSI_CMD_MAX_PARAM_NUM + 1U;
    UINT8 Param = 0U;
    UINT8 * pParam = &Param;
    AmbaVout_MipiDsiSendNormalWrite(NumParam, pParam);

    NumParam = VOUT_DSI_CMD_MAX_PARAM_NUM;
    AmbaVout_MipiDsiSendNormalWrite(NumParam, pParam);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiSendNormalWrite(NumParam, pParam);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiSendNormalWrite(NumParam, pParam);
    Set_RetVal_MutexGive(KAL_ERR_NONE);


    printf("TEST_AmbaVout_MipiDsiSendNormalWrite\n");
}

/* test case for input param, [0]: golden parameter  */
AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s MipiDsiSetBlankPktTC[] = {
    {.LenHBP = 1U, .LenHSA = 0U, .LenHFP = 0U, .LenBLLP = 0U,},
    {.LenHBP = 0U, .LenHSA = 1U, .LenHFP = 0U, .LenBLLP = 0U,},
    {.LenHBP = 0U, .LenHSA = 0U, .LenHFP = 1U, .LenBLLP = 0U,},
    {.LenHBP = 0U, .LenHSA = 0U, .LenHFP = 0U, .LenBLLP = 1U,},
};

void MipiDsiSetBlankPkt_Get(UINT32 Index, AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s *pMipiDsiBlankPktCtrl)
{
    memset(pMipiDsiBlankPktCtrl, 0, sizeof(AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s));
    pMipiDsiBlankPktCtrl->LenHBP = MipiDsiSetBlankPktTC[Index].LenHBP;
    pMipiDsiBlankPktCtrl->LenHSA = MipiDsiSetBlankPktTC[Index].LenHSA;
    pMipiDsiBlankPktCtrl->LenHFP = MipiDsiSetBlankPktTC[Index].LenHFP;
    pMipiDsiBlankPktCtrl->LenBLLP = MipiDsiSetBlankPktTC[Index].LenBLLP;
}

void TEST_AmbaVout_MipiDsiSetBlankPkt(void)
{
    UINT32 i;
    AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s MipiDsiBlankPktCtrl = {0};

    AmbaVout_MipiDsiSetBlankPkt(NULL);

    for (i = 0U; i < (sizeof(MipiDsiSetBlankPktTC) / (UINT32) sizeof(AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s)); i ++ ) {
        MipiDsiSetBlankPkt_Get(i, &MipiDsiBlankPktCtrl);
        AmbaVout_MipiDsiSetBlankPkt(&MipiDsiBlankPktCtrl);
    }

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiSetBlankPkt(&MipiDsiBlankPktCtrl);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiSetBlankPkt(&MipiDsiBlankPktCtrl);
    Set_RetVal_MutexGive(KAL_ERR_NONE);


    printf("TEST_AmbaVout_MipiDsiSetBlankPkt\n");
}

void TEST_AmbaVout_GetAckData(void)
{
    UINT32 AckData = {0};
    AmbaVout_MipiDsiGetAckData(NULL);

    /* For test cases in AmbaRTSL_VoutMipiDsiGetAckData */
    pAmbaRCT_Reg->MipiDsi0Obsv[0] = 0x0U;
    pAmbaRCT_Reg->MipiDsi1Obsv[0] = 0x0U;
    AmbaVout_MipiDsiGetAckData(&AckData);

    pAmbaRCT_Reg->MipiDsi0Obsv[0] = 0x84U;
    pAmbaRCT_Reg->MipiDsi1Obsv[0] = 0x84U;
    AmbaVout_MipiDsiGetAckData(&AckData);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_MipiDsiGetAckData(&AckData);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_MipiDsiGetAckData(&AckData);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_GetAckData\n");
}

void TEST_AmbaVout_GetDisp0Status(void)
{
    AmbaVout_GetDisp0Status(NULL, NULL);

    UINT32 Status = 0U;
    UINT32 *pStatus = &Status;
    UINT32 FreezeDetect = 0U;
    UINT32 *pFreezeDetect = &FreezeDetect;
    AmbaVout_GetDisp0Status(pStatus, pFreezeDetect);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetDisp0Status(pStatus, pFreezeDetect);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetDisp0Status(pStatus, pFreezeDetect);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    pAmbaVoutDisplay0_Reg->ChromaChecksum = 1U;
    AmbaVout_GetDisp0Status(pStatus, pFreezeDetect);
    pAmbaVoutDisplay0_Reg->ChromaChecksum = 0U;
    pAmbaVoutDisplay0_Reg->LumaChecksum = 1U;
    AmbaVout_GetDisp0Status(pStatus, pFreezeDetect);

    printf("TEST_AmbaVout_GetDisp0Status\n");
}

void TEST_AmbaVout_GetDisp1Status(void)
{
    AmbaVout_GetDisp1Status(NULL, NULL);

    UINT32 Status = 0U;
    UINT32 *pStatus = &Status;
    UINT32 FreezeDetect = 0U;
    UINT32 *pFreezeDetect = &FreezeDetect;
    AmbaVout_GetDisp1Status(pStatus, pFreezeDetect);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetDisp1Status(pStatus, pFreezeDetect);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetDisp1Status(pStatus, pFreezeDetect);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    pAmbaVoutDisplay1_Reg->ChromaChecksum = 1U;
    AmbaVout_GetDisp1Status(pStatus, pFreezeDetect);
    pAmbaVoutDisplay1_Reg->ChromaChecksum = 0U;
    pAmbaVoutDisplay1_Reg->LumaChecksum = 1U;
    AmbaVout_GetDisp1Status(pStatus, pFreezeDetect);

    printf("TEST_AmbaVout_GetDisp1Status\n");
}

void TEST_AmbaVout_SetDisp0Timeout(void)
{
    UINT32 Timeout = 0U;
    AmbaVout_SetDisp0Timeout(Timeout);

    Timeout = 1U;
    AmbaVout_SetDisp0Timeout(Timeout);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_SetDisp0Timeout(Timeout);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_SetDisp0Timeout(Timeout);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_SetDisp0Timeout\n");
}

void TEST_AmbaVout_SetDisp1Timeout(void)
{
    UINT32 Timeout = 0U;
    AmbaVout_SetDisp1Timeout(Timeout);

    Timeout = 1U;
    AmbaVout_SetDisp1Timeout(Timeout);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_SetDisp1Timeout(Timeout);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_SetDisp1Timeout(Timeout);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_SetDisp1Timeout\n");
}

void TEST_AmbaVout_GetTopLevelStatus(void)
{
    AmbaVout_GetTopLevelStatus(NULL);

    UINT32 Status = 0U;
    UINT32 *pStatus = &Status;
    AmbaVout_GetTopLevelStatus(pStatus);

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVout_GetTopLevelStatus(pStatus);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    Set_RetVal_MutexGive(KAL_ERR_0000);
    AmbaVout_GetTopLevelStatus(pStatus);
    Set_RetVal_MutexGive(KAL_ERR_NONE);

    printf("TEST_AmbaVout_GetTopLevelStatus\n");
}

void TEST_AmbaVout_SafetyStart(void)
{
    UINT32 VoutID = 0U;
    AmbaVout_SafetyStart(VoutID);

    printf("TEST_AmbaVout_SafetyStart\n");
}

void TEST_AmbaVout_SafetyStop(void)
{
    UINT32 VoutID;
    AmbaVout_SafetyStop(VoutID);

    printf("TEST_AmbaVout_SafetyStop\n");
}

/* Test APIs in AmbaRTSL_VOUT_LCD.c */
void TEST_AmbaRTSL_Vout(void)
{
    UINT32 MaxFrameCount = 0U;
    UINT32 Mask = 0U;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DisplayTiming = {0};
    UINT32 DcsCmd = 0U;
    UINT8 Param = 0U;
    UINT32 AckData = 0U;
    UINT32 EnableFlag = 2U;

    AmbaRTSL_VoutMipiCsiSetMaxFrame(MaxFrameCount);
    AmbaRTSL_VoutSetTopLevelMask(Mask);

    /* NULL pointer check*/
    AmbaRTSL_Vout0GetDispConfigAddr(NULL);
    AmbaRTSL_Vout1GetDispConfigAddr(NULL);
    AmbaRTSL_Vout0GetDigitalCscAddr(NULL);
    AmbaRTSL_Vout1GetDigitalCscAddr(NULL);
    AmbaRTSL_VoutMipiDsiEnable(VOUT_MIPI_DSI_MODE_422_1LANE, VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, NULL);
    AmbaRTSL_VoutMipiCsiEnable(VOUT_MIPI_CSI_MODE_422_1LANE, VOUT_NUM_MIPI_CSI_MODE_ORDER - 1U, NULL);

    /* (TxByteClkHS == 0.0) in VOUT_CalculateMphyConfig */
    MipiDsiEnable_DisplayTiming_Get(1U, &DisplayTiming);
    AmbaRTSL_VoutMipiDsiEnable(VOUT_MIPI_DSI_MODE_422_1LANE, VOUT_NUM_MIPI_DSI_MODE_ORDER - 1U, &DisplayTiming);

    /* ((NumParam != 0U) && (pParam != NULL))*/
    AmbaRTSL_VoutMipiDsiDcsWrite(DcsCmd, 1U, NULL);
    AmbaRTSL_VoutMipiDsiDcsWrite(DcsCmd, 1U, &Param);

    /* (i >= NumParam) in VOUT_MipiDsiCommandSetup */
    AmbaRTSL_VoutMipiDsiDcsWrite(DcsCmd, 3U, &Param);

    /* (BtaEnSrc >= NUM_MIPI_DSI_BTA_ENABLE) */
    AmbaRTSL_VoutMipiDsiSetBtaSrc(NUM_MIPI_DSI_BTA_ENABLE);

    /* (BtaEnable != MIPI_DSI_BTA_ENABLE_FROM_RCT) */
    AmbaRTSL_VoutMipiDsiSetBtaSrc(MIPI_DSI_BTA_ENABLE_FROM_VOUT);
    AmbaRTSL_VoutMipiDsiGetAckData(&AckData);

    /* (EnableFlag > 1U) */
    AmbaRTSL_Vout0SetVinVoutSync(EnableFlag);
    AmbaRTSL_Vout1SetVinVoutSync(EnableFlag);

    /* default case of ParamID */
    AmbaRTSL_VoutMipiDsiSetPhyCtrl(VOUT_NUM_PHY_PARAM, 0U);
    AmbaRTSL_VoutMipiCsiSetPhyCtrl(VOUT_NUM_PHY_PARAM, 0U);

    printf("TEST_AmbaRTSL_VoutMipiCsiSetMaxFrame\n");
    printf("TEST_AmbaRTSL_VoutSetTopLevelMask\n");
}

typedef struct {
    UINT32 CscType;
    UINT32 CscIndex;
} GET_CSC_MATRIX_TC_s;

GET_CSC_MATRIX_TC_s GetCscMatrixTC[] = {
    {.CscType = CSC_TYPE_BT601, .CscIndex = NUM_CSC_OPTION},
    {.CscType = CSC_TYPE_BT709, .CscIndex = NUM_CSC_OPTION},
    {.CscType = CSC_TYPE_BT709, .CscIndex = 1U},
    {.CscType = NUM_CSC_TYPE,   .CscIndex = 1U},
};

void TEST_AmbaVout_GetCscMatrix(void)
{
    UINT32 i;
    AMBA_VOUT_CSC_MATRIX_s CscMatrix;
    AMBA_VOUT_CSC_MATRIX_s *pCscMatrix = &CscMatrix;

    AmbaVout_GetCscMatrix(CSC_TYPE_BT709, 1U, NULL);

    for (i = 0U; i < (sizeof(GetCscMatrixTC) / (UINT32) sizeof(GET_CSC_MATRIX_TC_s)); i ++ ) {
        AmbaVout_GetCscMatrix(GetCscMatrixTC[i].CscType, GetCscMatrixTC[i].CscIndex, &pCscMatrix);
    };

    printf("TEST_AmbaVout_GetCscMatrix\n");
}

int main(void)
{
    TEST_AmbaVout_ChannelSelect(2U);
    TEST_AmbaVout_ChannelSelect(0U);

    TEST_AmbaVout_GetDisp0ConfigAddr();
    TEST_AmbaVout_GetDisp1ConfigAddr();
    TEST_AmbaVout_GetDisp0DigiCscAddr();
    TEST_AmbaVout_GetDisp1AnalogCscAddr();
    TEST_AmbaVout_GetDisp1DigiCscAddr();
    TEST_AmbaVout_GetDisp1HdmiCscAddr();
    TEST_AmbaVout_SetDisp0VinVoutSync();
    TEST_AmbaVout_SetDisp1VinVoutSync();

    TEST_AmbaVout_DigiYccEnable();
    TEST_AmbaVout_DigiYccSetPhyCtrl();
    TEST_AmbaVout_DigiYccGetStatus();
    TEST_AmbaVout_DigiYccSetCscMatrix();

    TEST_AmbaVout_DigiRgbEnable();
    TEST_AmbaVout_DigiRgbSetPhyCtrl();
    TEST_AmbaVout_DigiRgbGetStatus();
    TEST_AmbaVout_DigiRgbSetCscMatrix();

    TEST_AmbaVout_FpdLinkEnable();
    TEST_AmbaVout_FpdLinkSetPhyCtrl();
    TEST_AmbaVout_FpdLinkGetStatus();
    TEST_AmbaVout_FpdLinkSetCscMatrix();

    TEST_AmbaVout_MipiCsiEnable();
    TEST_AmbaVout_MipiCsiSetPhyCtrl();
    TEST_AmbaVout_MipiCsiGetStatus();
    TEST_AmbaVout_MipiCsiSetCscMatrix();

    TEST_AmbaVout_MipiDsiEnable();
    TEST_AmbaVout_MipiDsiSetPhyCtrl();
    TEST_AmbaVout_MipiDsiGetStatus();
    TEST_AmbaVout_MipiDsiSetCscMatrix();
    TEST_AmbaVout_MipiDsiSendDcsWrite();
    TEST_AmbaVout_MipiDsiSendNormalWrite();
    TEST_AmbaVout_MipiDsiSetBlankPkt();

    TEST_AmbaVout_GetAckData();

    TEST_AmbaVout_GetDisp0Status();
    TEST_AmbaVout_GetDisp1Status();

    TEST_AmbaVout_SetDisp0Timeout();
    TEST_AmbaVout_SetDisp1Timeout();

    TEST_AmbaVout_GetTopLevelStatus();

    TEST_AmbaVout_GetCscMatrix();

    /* avoid AmbaVout_DrvEntry to update register address */
    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaVout_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    Set_NoCopy_AmbaMisra_TypeCast(1U);
    AmbaVout_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    Set_RetVal_memset(1U);
    AmbaVout_DrvEntry();
    Set_RetVal_memset(ERR_NONE);

    /* For VOUT_DrvInited Test Case in AmbaVOUT.c */
    TEST_AmbaVout_MipiCsiSetCscMatrix();
    TEST_AmbaVout_MipiDsiSetCscMatrix();
    TEST_AmbaVout_MipiDsiSetBlankPkt();

    TEST_AmbaVout_GetDisp0Status();
    TEST_AmbaVout_GetDisp1Status();

    TEST_AmbaVout_SetDisp0Timeout();
    TEST_AmbaVout_SetDisp1Timeout();

    TEST_AmbaVout_GetTopLevelStatus();

    TEST_AmbaVout_SafetyStart();
    TEST_AmbaVout_SafetyStop();

    /* For VoutChan Test Case in AmbaRTSL_VOUT_LCD.c */
    for (UINT8 i = 0U; i <= 1U; i ++){
        TEST_AmbaVout_ChannelSelect(i);
        TEST_AmbaVout_MipiCsiEnable();
        TEST_AmbaVout_MipiCsiSetPhyCtrl();
        TEST_AmbaVout_MipiCsiGetStatus();

        TEST_AmbaVout_MipiDsiEnable();
        TEST_AmbaVout_MipiDsiSetPhyCtrl();
        TEST_AmbaVout_MipiDsiGetStatus();
        TEST_AmbaVout_MipiDsiSendDcsWrite();
        TEST_AmbaVout_MipiDsiSendNormalWrite();

        TEST_AmbaVout_GetAckData();

        TEST_AmbaRTSL_Vout();
    }

    return 0;
}

