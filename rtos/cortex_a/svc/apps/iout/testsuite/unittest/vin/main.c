#include <stdio.h>
#include <string.h>
#include "AmbaVIN.h"
#include "AmbaDebugPort_IDSP.h"
#include "AmbaReg_ScratchpadNS.h"
#include "AmbaReg_RCT.h"
#include "AmbaDrvEntry.h"
#include "AmbaKAL.h"

AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s AmbaIDSP_CtrlReg = {0};
AMBA_SCRATCHPAD_NS_REG_s AmbaScratchpadNS_Reg = {0};
AMBA_MIPI_REG_s AmbaMIPI_Reg = {0};
AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s AmbaIDSP_VinGlobalReg = {0};
AMBA_DBG_PORT_IDSP_VIN_MAIN_SM_DBG_REG_s AmbaIDSP_VinMainReg = {0};
AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s AmbaIDSP_VinMasterSyncReg = {0};

AMBA_DBG_PORT_IDSP_CONTROLLER_REG_s * pAmbaIDSP_CtrlReg = &AmbaIDSP_CtrlReg;
AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg = &AmbaScratchpadNS_Reg;
AMBA_MIPI_REG_s * pAmbaMIPI_Reg = &AmbaMIPI_Reg;
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;
AMBA_DBG_PORT_IDSP_VIN_GLOBAL_REG_s * pAmbaIDSP_VinGlobalReg = &AmbaIDSP_VinGlobalReg;
AMBA_DBG_PORT_IDSP_VIN_MAIN_SM_DBG_REG_s * pAmbaIDSP_VinMainReg = &AmbaIDSP_VinMainReg;
AMBA_DBG_PORT_IDSP_VIN_MASTER_SYNC_REG_s * pAmbaIDSP_VinMasterSyncReg = &AmbaIDSP_VinMasterSyncReg;

extern void Set_RetVal_MutexTake(UINT32 RetVal);
extern void Set_RetVal_MutexCreate(UINT32 RetVal);
extern void Set_RetVal_PllGetVin0Clk(UINT32 RetVal);
extern void Set_RetVal_PllGetVin1Clk(UINT32 RetVal);
extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

typedef struct {
    UINT32 VinID;
    UINT32 EnabledPin;
} SLVS_RESET_TC_s;

/* test case for input param, [0]: golden parameter  */
SLVS_RESET_TC_s SlvsResetTC[] = {
    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0xfU, },

    /* VIN_ClearSafetyError */
    {.VinID =      AMBA_VIN_CHANNEL1, .EnabledPin = 0xf00U, },
    {.VinID =      AMBA_VIN_CHANNEL2, .EnabledPin = 0xfU, },
    {.VinID =      AMBA_VIN_CHANNEL3, .EnabledPin = 0xfU, },
    {.VinID =      AMBA_VIN_CHANNEL4, .EnabledPin = 0xfU, },
    {.VinID =      AMBA_VIN_CHANNEL8, .EnabledPin = 0xf0U, },

    /* incorrect pin */
    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0x0U, },
    {.VinID =      AMBA_VIN_CHANNEL1, .EnabledPin = 0xfffU, },
    {.VinID =      AMBA_VIN_CHANNEL1, .EnabledPin = 0xfU, },
    {.VinID =      AMBA_VIN_CHANNEL8, .EnabledPin = 0xfffU, },
    {.VinID =      AMBA_VIN_CHANNEL8, .EnabledPin = 0xfU, },

    {.VinID =   AMBA_NUM_VIN_CHANNEL, .EnabledPin = 0xfU, },
};
void SlvsReset_PadConfig_Get(UINT32 Index, AMBA_VIN_SLVS_PAD_CONFIG_s * pPadConfig)
{
    memset(pPadConfig, 0, sizeof(AMBA_VIN_SLVS_PAD_CONFIG_s));
    pPadConfig->EnabledPin = SlvsResetTC[Index].EnabledPin;
}

void TEST_AmbaVIN_SlvsReset(void)
{
    AMBA_VIN_SLVS_PAD_CONFIG_s PadConfig = {0};
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    SlvsReset_PadConfig_Get(0U, &PadConfig);
    AmbaVIN_SlvsReset(SlvsResetTC[0].VinID, &PadConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_SlvsReset(SlvsResetTC[0].VinID, NULL);

    for (i = 0; i < (sizeof(SlvsResetTC) / (UINT32) sizeof(SLVS_RESET_TC_s)); i ++ ) {
        SlvsReset_PadConfig_Get(i, &PadConfig);
        AmbaVIN_SlvsReset(SlvsResetTC[i].VinID, &PadConfig);
    }

    printf("TEST_AmbaVIN_SlvsReset\n");
}

typedef struct {
    UINT32 VinID;
    UINT32 EnabledPin;
    UINT32  ClkMode;
    UINT64  DateRate;
} MIPI_RESET_TC_s;

/* test case for input param, [0]: golden parameter  */
MIPI_RESET_TC_s MipiResetTC[] = {
    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL1, .EnabledPin = 0xf00U, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL2, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL3, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL4, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL8, .EnabledPin = 0xf0U, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =   AMBA_NUM_VIN_CHANNEL, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},

    /* incorrect pin */
    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0xfffU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0xf00U, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL2, .EnabledPin = 0x0U, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL3, .EnabledPin = 0x0U, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL4, .EnabledPin = 0x0U, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL2, .EnabledPin = 0xfffU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL1, .EnabledPin = 0xfffU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL1, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL8, .EnabledPin = 0xfffU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL8, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 500000000U},

    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0xfU, .ClkMode = 0xFF, .DateRate = 500000000U},
    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 0},
    {.VinID =      AMBA_VIN_CHANNEL0, .EnabledPin = 0xfU, .ClkMode = AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS, .DateRate = 0xFFFFFFFF},

};

void MipiReset_PadConfig_Get(UINT32 Index, AMBA_VIN_MIPI_PAD_CONFIG_s * pPadConfig)
{
    memset(pPadConfig, 0, sizeof(AMBA_VIN_MIPI_PAD_CONFIG_s));

    pPadConfig->ClkMode = MipiResetTC[Index].ClkMode;
    pPadConfig->DateRate = MipiResetTC[Index].DateRate;
    pPadConfig->EnabledPin = MipiResetTC[Index].EnabledPin;
}

void TEST_AmbaVIN_MipiReset(void)
{
    AMBA_VIN_MIPI_PAD_CONFIG_s PadConfig = {0};
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    MipiReset_PadConfig_Get(0, &PadConfig);
    AmbaVIN_MipiReset(MipiResetTC[0].VinID, &PadConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_MipiReset(MipiResetTC[0].VinID, NULL);

    for (i = 0; i < (sizeof(MipiResetTC)/sizeof(MIPI_RESET_TC_s)); i ++ ) {
        MipiReset_PadConfig_Get(i, &PadConfig);
        AmbaVIN_MipiReset(MipiResetTC[i].VinID, &PadConfig);
    }

    printf("TEST_AmbaVIN_MipiReset\n");
}

typedef struct {
    UINT32  VinID;
    UINT8   PadType;
} DVP_RESET_TC_s;

/* test case for input param */
DVP_RESET_TC_s DvpResetTC[] = {
    {.VinID =      AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, },
    {.VinID =      AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVDS, },
    {.VinID =      AMBA_VIN_CHANNEL0, .PadType = 0xFF, },
    {.VinID =   AMBA_NUM_VIN_CHANNEL, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, },
};

void DvpReset_PadConfig_Get(UINT32 Index, AMBA_VIN_DVP_PAD_CONFIG_s * pPadConfig)
{
    memset(pPadConfig, 0, sizeof(AMBA_VIN_DVP_PAD_CONFIG_s));

    pPadConfig->PadType = DvpResetTC[Index].PadType;
}

void TEST_AmbaVIN_DvpReset(void)
{
    AMBA_VIN_DVP_PAD_CONFIG_s PadConfig = {0};
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    DvpReset_PadConfig_Get(0, &PadConfig);
    AmbaVIN_DvpReset(DvpResetTC[0].VinID, &PadConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_DvpReset(DvpResetTC[0].VinID, NULL);

    for (i = 0; i < (sizeof(DvpResetTC)/sizeof(DVP_RESET_TC_s)); i ++ ) {
        DvpReset_PadConfig_Get(i, &PadConfig);
        AmbaVIN_DvpReset(DvpResetTC[i].VinID, &PadConfig);
    }

    printf("TEST_AmbaVIN_DvpReset\n");
}

typedef struct {
    UINT32 VinID;
    UINT8  Interlace;
    UINT8  ColorSpace;
    UINT8  BayerPattern;
    UINT8  YuvOrder;
    UINT8  NumDataBits;
    UINT32 NumActivePixels;
    UINT32 NumActiveLines;
    UINT32 NumTotalPixels;
    UINT32 NumTotalLines;

    UINT8  ITU656Type;
    UINT8  PatternAlign;
    UINT8  SolDetectEnable;
    UINT8  EolDetectEnable;
    UINT8  SofDetectEnable;
    UINT8  EofDetectEnable;
    UINT8  SovDetectEnable;
    UINT8  EovDetectEnable;

    UINT8  *pLaneMapping;
    UINT8  NumActiveLanes;
} SLVS_CONFIG_TC_s;

static UINT8 LaneMapping[8] = {0, 1, 2, 3, 4, 5, 6, 7};
static UINT8 FalseLaneMapping[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* test case for input param */
SLVS_CONFIG_TC_s SlvsConfigTC[] = {
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4},
    {.VinID =      AMBA_VIN_CHANNEL1, .Interlace = 1, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },

    /* Test MainConfig parameter */
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0xf, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0xff, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = AMBA_VIN_BAYER_PATTERN_GB+1, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = AMBA_VIN_BAYER_PATTERN_GIBG+1, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = AMBA_VIN_BAYER_PATTERN_CCRC+1, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0xff,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 0, .NumActivePixels = 0, .NumActiveLines = 0, .NumTotalPixels = 0, .NumTotalLines = 0,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 0, .NumActiveLines = 0, .NumTotalPixels = 0, .NumTotalLines = 0,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 0, .NumTotalPixels = 0, .NumTotalLines = 0,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 0, .NumTotalLines = 0,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 0, .NumTotalLines = 0,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 0,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },

    /* Test SyncDetectCtrl */
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0xFF, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0xFF, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0xff, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0xff, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0xff, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0xff,
     .SovDetectEnable = 0, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0xff, .EovDetectEnable = 0, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0xff, .pLaneMapping = LaneMapping, .NumActiveLanes = 4 },


    /* test VIN_ParamCheckLaneMapping */
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0xff, .pLaneMapping = FalseLaneMapping, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0xff, .pLaneMapping = NULL, .NumActiveLanes = 4 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0xff, .pLaneMapping = LaneMapping, .NumActiveLanes = 0 },
    {.VinID =      AMBA_VIN_CHANNEL0, .Interlace = 0, .ColorSpace = 0, .BayerPattern = 0, .YuvOrder = 0,
     .NumDataBits = 8, .NumActivePixels = 1920, .NumActiveLines = 1080, .NumTotalPixels = 2250, .NumTotalLines = 1125,
     .ITU656Type = 0, .PatternAlign = 0, .SolDetectEnable = 0, .EolDetectEnable = 0, .SofDetectEnable = 0, .EofDetectEnable = 0,
     .SovDetectEnable = 0, .EovDetectEnable = 0xff, .pLaneMapping = LaneMapping, .NumActiveLanes = 0xff },

};

void SlvsConfig_VinCfg_Get(UINT32 Index, AMBA_VIN_SLVS_CONFIG_s * pVinCfg)
{
    memset(pVinCfg, 0, sizeof(AMBA_VIN_SLVS_CONFIG_s));

    pVinCfg->pLaneMapping = SlvsConfigTC[Index].pLaneMapping;
    pVinCfg->NumActiveLanes = SlvsConfigTC[Index].NumActiveLanes;
    pVinCfg->Config.FrameRate.Interlace = SlvsConfigTC[Index].Interlace;
    pVinCfg->Config.ColorSpace = SlvsConfigTC[Index].ColorSpace;
    pVinCfg->Config.BayerPattern = SlvsConfigTC[Index].BayerPattern;
    pVinCfg->Config.YuvOrder = SlvsConfigTC[Index].YuvOrder;
    pVinCfg->Config.NumDataBits = SlvsConfigTC[Index].NumDataBits;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels = SlvsConfigTC[Index].NumActivePixels;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines = SlvsConfigTC[Index].NumActiveLines;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels = SlvsConfigTC[Index].NumTotalPixels;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines = SlvsConfigTC[Index].NumTotalLines;
    pVinCfg->SyncDetectCtrl.ITU656Type = SlvsConfigTC[Index].ITU656Type;
    pVinCfg->SyncDetectCtrl.CustomSyncCode.PatternAlign     = SlvsConfigTC[Index].PatternAlign;
    pVinCfg->SyncDetectCtrl.CustomSyncCode.SolDetectEnable  = SlvsConfigTC[Index].SolDetectEnable;
    pVinCfg->SyncDetectCtrl.CustomSyncCode.EolDetectEnable  = SlvsConfigTC[Index].EolDetectEnable;
    pVinCfg->SyncDetectCtrl.CustomSyncCode.SofDetectEnable  = SlvsConfigTC[Index].SofDetectEnable;
    pVinCfg->SyncDetectCtrl.CustomSyncCode.EofDetectEnable  = SlvsConfigTC[Index].EofDetectEnable;
    pVinCfg->SyncDetectCtrl.CustomSyncCode.SovDetectEnable  = SlvsConfigTC[Index].SovDetectEnable;
    pVinCfg->SyncDetectCtrl.CustomSyncCode.EovDetectEnable  = SlvsConfigTC[Index].EovDetectEnable;
}

void TEST_AmbaVIN_SlvsConfig(void)
{
    AMBA_VIN_SLVS_CONFIG_s VinCfg = {0};
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    SlvsConfig_VinCfg_Get(0, &VinCfg);
    AmbaVIN_SlvsConfig(SlvsConfigTC[0].VinID, &VinCfg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(SlvsConfigTC)/sizeof(SLVS_CONFIG_TC_s)); i ++ ) {
        SlvsConfig_VinCfg_Get(i, &VinCfg);
        AmbaVIN_SlvsConfig(SlvsConfigTC[i].VinID, &VinCfg);
    }


    printf("TEST_AmbaVIN_SlvsConfig\n");
}

typedef struct {
    UINT32  VinID;
    UINT8   ColorSpace;
} MIPI_CONFIG_TC_s;

/* test case for input param */
MIPI_CONFIG_TC_s MipiConfigTC[] = {
    {.VinID =   AMBA_VIN_CHANNEL0,   .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB},
    {.VinID =   AMBA_VIN_CHANNEL0,   .ColorSpace = 0xff},
    {.VinID =   AMBA_VIN_CHANNEL1,   .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB},
    {.VinID =   AMBA_VIN_CHANNEL8,   .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB},
    {.VinID =  AMBA_NUM_VIN_CHANNEL, .ColorSpace = AMBA_VIN_COLOR_SPACE_RGB },
};

void MipiConfig_VinCfg_Get(UINT32 Index, AMBA_VIN_MIPI_CONFIG_s * pVinCfg)
{
    memset(pVinCfg, 0, sizeof(AMBA_VIN_MIPI_CONFIG_s));

    pVinCfg->NumActiveLanes = 4U;
    pVinCfg->Config.NumDataBits = 8U;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels = 1920;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines = 1080;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels = 2250;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines = 1125;
    pVinCfg->Config.ColorSpace = MipiConfigTC[Index].ColorSpace;
}

void TEST_AmbaVIN_MipiConfig(void)
{
    AMBA_VIN_MIPI_CONFIG_s VinCfg = {0};
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    MipiConfig_VinCfg_Get(0, &VinCfg);
    AmbaVIN_MipiConfig(MipiConfigTC[0].VinID, &VinCfg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(MipiConfigTC)/sizeof(MIPI_CONFIG_TC_s)); i ++ ) {
        MipiConfig_VinCfg_Get(i, &VinCfg);
        AmbaVIN_MipiConfig(MipiConfigTC[i].VinID, &VinCfg);
    }

    printf("TEST_AmbaVIN_MipiConfig\n");
}

typedef struct {
    UINT32 VinID;
    UINT8  PadType;
    UINT8  DvpWide;
    UINT8  DataLatchEdge;
    UINT8  SyncType;
    UINT8  HsyncPinSelect;
    UINT8  VsyncPinSelect;
    UINT8  FieldPinSelect;
    UINT8  HsyncPolarity;
    UINT8  VsyncPolarity;
    UINT8  FieldPolarity;
    UINT8  SyncLocate;
} DVP_CONFIG_TC_s;

/* test case for input param */
DVP_CONFIG_TC_s DvpConfigTC[] = {
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_NUM_VIN_CHANNEL, .PadType = 0, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },

    /* test VIN_ParamCheckDvpConfig */
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVDS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = 0xF, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0xF, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0xF, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0xF, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0xF, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0xF, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0xF,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0xF, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0xF, .FieldPolarity = 0, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0xF, .SyncLocate = 0, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 0, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0xF, },
    {.VinID =   AMBA_VIN_CHANNEL0, .PadType = AMBA_VIN_DVP_PAD_TYPE_LVCMOS, .DvpWide = 0, .DataLatchEdge = 0, .SyncType = 1, .HsyncPinSelect = 0, .VsyncPinSelect = 0, .FieldPinSelect = 0,
     .HsyncPolarity = 0, .VsyncPolarity = 0, .FieldPolarity = 0, .SyncLocate = 0, },

};

void DvpConfig_VinCfg_Get(UINT32 Index, AMBA_VIN_DVP_CONFIG_s * pVinCfg)
{
    memset(pVinCfg, 0, sizeof(AMBA_VIN_DVP_CONFIG_s));

    pVinCfg->Config.NumDataBits = 8U;
    pVinCfg->Config.RxHvSyncCtrl.NumActivePixels = 1920;
    pVinCfg->Config.RxHvSyncCtrl.NumActiveLines = 1080;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalPixels = 2250;
    pVinCfg->Config.RxHvSyncCtrl.NumTotalLines = 1125;

    pVinCfg->PadType = DvpConfigTC[Index].PadType;
    pVinCfg->DvpWide = DvpConfigTC[Index].DvpWide;
    pVinCfg->DataLatchEdge = DvpConfigTC[Index].DataLatchEdge;
    pVinCfg->SyncDetectCtrl.SyncType = DvpConfigTC[Index].SyncType;

    pVinCfg->SyncDetectCtrl.SyncPinConfig.HsyncPinSelect = DvpConfigTC[Index].HsyncPinSelect;
    pVinCfg->SyncDetectCtrl.SyncPinConfig.VsyncPinSelect = DvpConfigTC[Index].VsyncPinSelect;
    pVinCfg->SyncDetectCtrl.SyncPinConfig.FieldPinSelect = DvpConfigTC[Index].FieldPinSelect;
    pVinCfg->SyncDetectCtrl.SyncPinConfig.HsyncPolarity = DvpConfigTC[Index].HsyncPolarity;
    pVinCfg->SyncDetectCtrl.SyncPinConfig.VsyncPolarity = DvpConfigTC[Index].VsyncPolarity;
    pVinCfg->SyncDetectCtrl.SyncPinConfig.FieldPolarity = DvpConfigTC[Index].FieldPolarity;
    pVinCfg->SyncDetectCtrl.EmbSyncConfig.SyncLocate = DvpConfigTC[Index].SyncLocate;

}

void TEST_AmbaVIN_DvpConfig(void)
{
    AMBA_VIN_DVP_CONFIG_s VinCfg = {0};
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    DvpConfig_VinCfg_Get(0, &VinCfg);
    AmbaVIN_DvpConfig(DvpConfigTC[0].VinID, &VinCfg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(DvpConfigTC)/sizeof(DVP_CONFIG_TC_s)); i ++ ) {
        DvpConfig_VinCfg_Get(i, &VinCfg);
        AmbaVIN_DvpConfig(DvpConfigTC[i].VinID, &VinCfg);
    }

    printf("TEST_AmbaVIN_DvpConfig\n");
}

typedef struct {
    UINT32 MSyncID;
    UINT32 RefClk;
    UINT32 HPeriod;
    UINT32 HPulseWidth;
    UINT32 HPolarity;
    UINT32 VPeriod;
    UINT32 VPulseWidth;
    UINT32 VPolarity;
    UINT8  ToggleHsyncInVblank;

} MSYNC_CONFIG_TC_s;

/* test case for input param */
MSYNC_CONFIG_TC_s MsyncConfigTC[] = {
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },

    {.MSyncID =   AMBA_VIN_MSYNC1, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_NUM_VIN_MSYNC, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_NUM_VIN_MSYNC, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },

    /* test error for VIN_ParamCheckMasterSync */
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 0, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 0, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 0xFFFF, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0xFF,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 0, .VPulseWidth = 8, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 0xFFFFFFFF, .VPolarity = 0, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity = 0xFF, .ToggleHsyncInVblank = 0 },
    {.MSyncID =   AMBA_VIN_MSYNC0, .RefClk = 24000000, .HPeriod = 1000, .HPulseWidth = 8, .HPolarity = 0,
     .VPeriod = 1000, .VPulseWidth = 8, .VPolarity =  0, .ToggleHsyncInVblank = 0xFF },

};

void MasterSyncEnable_VMSyncCfg_Get(UINT32 Index, AMBA_VIN_MASTER_SYNC_CONFIG_s * pMSyncCfg)
{
    memset(pMSyncCfg, 0, sizeof(AMBA_VIN_MASTER_SYNC_CONFIG_s));

    pMSyncCfg->RefClk = MsyncConfigTC[Index].RefClk;
    pMSyncCfg->HSync.Period = MsyncConfigTC[Index].HPeriod;
    pMSyncCfg->HSync.PulseWidth = MsyncConfigTC[Index].HPulseWidth;
    pMSyncCfg->HSync.Polarity = MsyncConfigTC[Index].HPolarity;
    pMSyncCfg->VSync.Period = MsyncConfigTC[Index].VPeriod;
    pMSyncCfg->VSync.PulseWidth = MsyncConfigTC[Index].VPulseWidth;
    pMSyncCfg->VSync.Polarity = MsyncConfigTC[Index].VPolarity;
    pMSyncCfg->ToggleHsyncInVblank = MsyncConfigTC[Index].ToggleHsyncInVblank;

}

void TEST_AmbaVIN_MasterSyncEnable(void)
{
    AMBA_VIN_MASTER_SYNC_CONFIG_s MSyncCfg = {0};
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    MasterSyncEnable_VMSyncCfg_Get(0, &MSyncCfg);
    AmbaVIN_MasterSyncEnable(MsyncConfigTC[0].MSyncID, &MSyncCfg);
    Set_RetVal_MutexTake(KAL_ERR_NONE);


    /* RTSL_PLL test */
    Set_RetVal_PllGetVin0Clk(MSyncCfg.RefClk);
    MasterSyncEnable_VMSyncCfg_Get(0, &MSyncCfg);
    AmbaVIN_MasterSyncEnable(AMBA_VIN_MSYNC0, &MSyncCfg);
    Set_RetVal_PllGetVin0Clk(0);

    Set_RetVal_PllGetVin1Clk(MSyncCfg.RefClk);
    MasterSyncEnable_VMSyncCfg_Get(0, &MSyncCfg);
    AmbaVIN_MasterSyncEnable(AMBA_VIN_MSYNC1, &MSyncCfg);
    Set_RetVal_PllGetVin1Clk(0);

    /* parameter test */
    for (i = 0; i < (sizeof(MsyncConfigTC)/sizeof(MSYNC_CONFIG_TC_s)); i ++ ) {
        MasterSyncEnable_VMSyncCfg_Get(i, &MSyncCfg);
        AmbaVIN_MasterSyncEnable(MsyncConfigTC[i].MSyncID, &MSyncCfg);
    }

    printf("TEST_AmbaVIN_MasterSyncEnable\n");
}


void TEST_AmbaVIN_MasterSyncDisable(void)
{
    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_MasterSyncDisable(AMBA_VIN_MSYNC0);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_MasterSyncDisable(AMBA_VIN_MSYNC0);
    AmbaVIN_MasterSyncDisable(AMBA_NUM_VIN_MSYNC);

    printf("TEST_AmbaVIN_MasterSyncDisable\n");
}

typedef struct {
    UINT32 VinID;
    UINT32 Value;
} LVDS_TERM_TC_s;

/* test case for input param */
LVDS_TERM_TC_s LvdsTermTC[] = {
    {.VinID =   AMBA_VIN_CHANNEL0,   .Value = AMBA_VIN_TERMINATION_VALUE_MIN},
    {.VinID =   AMBA_VIN_CHANNEL0,   .Value = AMBA_VIN_TERMINATION_VALUE_MAX},
    {.VinID =   AMBA_VIN_CHANNEL0,   .Value = AMBA_VIN_TERMINATION_VALUE_MAX + 1},
};

void TEST_AmbaVIN_SetLvdsTermination(void)
{
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_SetLvdsTermination(LvdsTermTC[0].VinID, LvdsTermTC[0].Value);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(LvdsTermTC)/sizeof(LVDS_TERM_TC_s)); i ++ ) {
        AmbaVIN_SetLvdsTermination(LvdsTermTC[i].VinID, LvdsTermTC[i].Value);
    }

    printf("TEST_AmbaVIN_SetLvdsTermination\n");
}

typedef struct {
    UINT32 ClkID;
    UINT32 Frequency;
} SENSOR_CLK_TC_s;

/* test case for input param */
SENSOR_CLK_TC_s SensorClkTC[] = {
    {.ClkID =   AMBA_VIN_SENSOR_CLOCK0,     .Frequency = 24000000},
    {.ClkID =   AMBA_VIN_SENSOR_CLOCK1,     .Frequency = 24000000},
    {.ClkID =   AMBA_VIN_SENSOR_CLOCK2,     .Frequency = 24000000},
    {.ClkID =   AMBA_NUM_VIN_SENSOR_CLOCK,  .Frequency = 24000000},
    {.ClkID =   AMBA_VIN_SENSOR_CLOCK0,     .Frequency = 0},
};

void TEST_AmbaVIN_SensorClkEnable(void)
{
    UINT32 i;

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_SensorClkEnable(SensorClkTC[0].ClkID, SensorClkTC[0].Frequency);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    /* parameter test */
    for (i = 0; i < (sizeof(SensorClkTC)/sizeof(SENSOR_CLK_TC_s)); i ++ ) {
        AmbaVIN_SensorClkEnable(SensorClkTC[i].ClkID, SensorClkTC[i].Frequency);
    }

    printf("TEST_AmbaVIN_SensorClkEnable\n");
}


void TEST_AmbaVIN_SensorClkDisable(void)
{

    /* KAL ERR */
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_SensorClkDisable(AMBA_VIN_SENSOR_CLOCK0);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_SensorClkDisable(AMBA_VIN_SENSOR_CLOCK0);
    AmbaVIN_SensorClkDisable(AMBA_VIN_SENSOR_CLOCK1);
    AmbaVIN_SensorClkDisable(AMBA_VIN_SENSOR_CLOCK2);
    AmbaVIN_SensorClkDisable(AMBA_NUM_VIN_SENSOR_CLOCK);

    printf("TEST_AmbaVIN_SensorClkDisable\n");
}


void TEST_AmbaVIN_SensorClkSetDrvStrength(void)
{
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_SensorClkSetDrvStrength(AMBA_VIN_DRIVE_STRENGTH_12MA);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_SensorClkSetDrvStrength(AMBA_VIN_DRIVE_STRENGTH_12MA);

    AmbaVIN_SensorClkSetDrvStrength(AMBA_VIN_DRIVE_STRENGTH_12MA+1);

    printf("TEST_AmbaVIN_SensorClkSetDrvStrength\n");
}

typedef struct {
    UINT32 VinID;
    UINT32 NumActiveLanes;
} LANE_REMAP_TC_s;

/* test case for input param */
LANE_REMAP_TC_s LaneRemapTC[] =
{
    {.VinID =   AMBA_VIN_CHANNEL0,    .NumActiveLanes = 8},
    {.VinID =   AMBA_NUM_VIN_CHANNEL, .NumActiveLanes = 8},
};
static UINT8 PhyLaneMapping[8] = {0, 1, 2, 3, 4, 5, 6, 7};

void DataLaneRemap_Config_Get(UINT32 Index, AMBA_VIN_LANE_REMAP_CONFIG_s* pConfig)
{
    memset(pConfig, 0, sizeof(AMBA_VIN_LANE_REMAP_CONFIG_s));

    pConfig->NumActiveLanes = LaneRemapTC[Index].NumActiveLanes;
    pConfig->pPhyLaneMapping = PhyLaneMapping;
}

void TEST_AmbaVIN_DataLaneRemap(void)
{
    AMBA_VIN_LANE_REMAP_CONFIG_s LaneRemapConfig = {0};
    UINT32 i;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    DataLaneRemap_Config_Get(0, &LaneRemapConfig);
    AmbaVIN_DataLaneRemap(LaneRemapTC[0].VinID, &LaneRemapConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(LaneRemapTC)/sizeof(LANE_REMAP_TC_s)); i ++ ) {
        DataLaneRemap_Config_Get(i, &LaneRemapConfig);
        AmbaVIN_DataLaneRemap(LaneRemapTC[i].VinID, &LaneRemapConfig);
    }

    printf("TEST_AmbaVIN_DataLaneRemap\n");
}

typedef struct {
    UINT32  VinID;
    UINT8   VirtChan;
} MIPI_VC_TC_s;

/* test case for input param */
MIPI_VC_TC_s MipiVcTC[] =
{
    {.VinID =   AMBA_VIN_CHANNEL0,    .VirtChan = 0},
    {.VinID =   AMBA_NUM_VIN_CHANNEL, .VirtChan = 0},
};

void MipiVirtChanConfig_Config_Get(UINT32 Index, AMBA_VIN_MIPI_VC_CONFIG_s* pConfig)
{
    memset(pConfig, 0, sizeof(AMBA_VIN_MIPI_VC_CONFIG_s));

    pConfig->VirtChan = MipiVcTC[Index].VirtChan;
}

void TEST_AmbaVIN_MipiVirtChanConfig(void)
{
    AMBA_VIN_MIPI_VC_CONFIG_s MipiVirtChanConfig = {0};
    UINT32 i;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    MipiVirtChanConfig_Config_Get(0, &MipiVirtChanConfig);
    AmbaVIN_MipiVirtChanConfig(MipiVcTC[0].VinID, &MipiVirtChanConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(MipiVcTC)/sizeof(MIPI_VC_TC_s)); i ++ ) {
        MipiVirtChanConfig_Config_Get(i, &MipiVirtChanConfig);
        AmbaVIN_MipiVirtChanConfig(MipiVcTC[i].VinID, &MipiVirtChanConfig);
    }

    printf("TEST_AmbaVIN_MipiVirtChanConfig\n");
}


void TEST_AmbaVIN_GetMainCfgBufInfo(void)
{
    ULONG Addr;
    UINT32 Size;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_GetMainCfgBufInfo(AMBA_VIN_CHANNEL0, &Addr, &Size);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_GetMainCfgBufInfo(AMBA_VIN_CHANNEL0, &Addr, &Size);
    AmbaVIN_GetMainCfgBufInfo(AMBA_NUM_VIN_CHANNEL, &Addr, &Size);

    printf("TEST_AmbaVIN_GetMainCfgBufInfo\n");
}


typedef struct {
    UINT32  VinID;
    UINT16  Width;
    UINT16  Height;
} CAPTURE_CONFIG_TC_s;

/* test case for input param */
CAPTURE_CONFIG_TC_s CaptureConfigTC[] = {
    {.VinID =   AMBA_VIN_CHANNEL0,       .Width = 1920, .Height = 1080},
    {.VinID =   AMBA_NUM_VIN_CHANNEL,    .Width = 1920, .Height = 1080},
};

void CaptureConfig_CapWin_Get(UINT32 Index, AMBA_VIN_WINDOW_s* pCapWin)
{
    memset(pCapWin, 0, sizeof(AMBA_VIN_WINDOW_s));

    pCapWin->Width = CaptureConfigTC[0].Width;
    pCapWin->Height = CaptureConfigTC[0].Height;

}

void TEST_AmbaVIN_CaptureConfig(void)
{
    AMBA_VIN_WINDOW_s CaptureWindow = {0};
    UINT32 i;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    CaptureConfig_CapWin_Get(0, &CaptureWindow);
    AmbaVIN_CaptureConfig(AMBA_VIN_CHANNEL0, &CaptureWindow);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(CaptureConfigTC)/sizeof(CAPTURE_CONFIG_TC_s)); i ++ ) {
        CaptureConfig_CapWin_Get(i, &CaptureWindow);
        AmbaVIN_CaptureConfig(CaptureConfigTC[i].VinID, &CaptureWindow);
    }

    printf("TEST_AmbaVIN_CaptureConfig\n");
}

void TEST_AmbaVIN_GetInfo(void)
{
    AMBA_VIN_INFO_s VinInfo;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_GetInfo(AMBA_VIN_CHANNEL0, &VinInfo);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_GetInfo(AMBA_VIN_CHANNEL0, &VinInfo);
    AmbaVIN_GetInfo(AMBA_NUM_VIN_CHANNEL, &VinInfo);

    printf("TEST_AmbaVIN_GetInfo\n");
}

void TEST_AmbaVIN_IsEnabled(void)
{

    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_IsEnabled(AMBA_VIN_CHANNEL0);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_IsEnabled(AMBA_VIN_CHANNEL0);
    AmbaVIN_IsEnabled(AMBA_NUM_VIN_CHANNEL);

    printf("TEST_AmbaVIN_IsEnabled\n");
}


typedef struct {
    UINT32  VinID;
    UINT8   DataType;
} MIPI_EMB_TC_s;

/* test case for input param */
MIPI_EMB_TC_s MipiEmbTC[] =
{
    {.VinID =   AMBA_VIN_CHANNEL0,    .DataType = 0x2C},
    {.VinID =   AMBA_NUM_VIN_CHANNEL, .DataType = 0x2C},
    {.VinID =   AMBA_VIN_CHANNEL0,    .DataType = 0xff},
};

void MipiEmbDataConfig_Config_Get(UINT32 Index, AMBA_VIN_MIPI_EMB_DATA_CONFIG_s* pConfig)
{
    memset(pConfig, 0, sizeof(AMBA_VIN_MIPI_EMB_DATA_CONFIG_s));

    pConfig->DataType = MipiEmbTC[Index].DataType;
}

void TEST_AmbaVIN_MipiEmbDataConfig(void)
{
    AMBA_VIN_MIPI_EMB_DATA_CONFIG_s MipiEmbDataConfig = {0};
    UINT32 i;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    MipiEmbDataConfig_Config_Get( 0, &MipiEmbDataConfig);
    AmbaVIN_MipiEmbDataConfig(MipiEmbTC[0].VinID, &MipiEmbDataConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(MipiEmbTC)/sizeof(MIPI_EMB_TC_s)); i ++ ) {
        MipiEmbDataConfig_Config_Get( i, &MipiEmbDataConfig);
        AmbaVIN_MipiEmbDataConfig(MipiEmbTC[i].VinID, &MipiEmbDataConfig);
    }

    printf("TEST_AmbaVIN_MipiEmbDataConfig\n");

}

typedef struct {
    UINT32  RefClk;
    UINT32  VsdelaySrc;
    UINT32  VsDlyPeriod; //
    UINT32  HsDlyClk; //
    UINT32  VsDlyClk; //
    UINT32  HsPulseWidth; //
    UINT32  VsPulseWidth; //
    UINT32  HsPolInput;
    UINT32  VsPolInput;
    UINT32  HsPolOutput;
    UINT32  VsPolOutput;
} DLY_VSYNC_TC_s;

/* test case for input param */
DLY_VSYNC_TC_s DlyVsyncTC[] =
{
    {.RefClk = 24000000, .VsdelaySrc = AMBA_VIN_VSDLY_SRC_HV_MSYNC0, .VsDlyPeriod = 0, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0, .VsPolInput = 0, .HsPolOutput = 0, .VsPolOutput = 0},
    {.RefClk = 24000000, .VsdelaySrc = AMBA_VIN_VSDLY_SRC_HV_MSYNC1, .VsDlyPeriod = 0, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0, .VsPolInput = 0, .HsPolOutput = 0, .VsPolOutput = 0},
    {.RefClk =        0, .VsdelaySrc = AMBA_VIN_VSDLY_SRC_HV_MSYNC0, .VsDlyPeriod = AMBA_VIN_VSDLY_TIME_MAX+1, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0, .VsPolInput = 0, .HsPolOutput = 0, .VsPolOutput = 0},

    /* test error for VIN_ParamCheckDelayedVSync */
    {.RefClk = 24000000, .VsdelaySrc = 0xF, .VsDlyPeriod = 0, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0, .VsPolInput = 0, .HsPolOutput = 0, .VsPolOutput = 0},
    {.RefClk = 24000000, .VsdelaySrc = AMBA_VIN_VSDLY_SRC_HV_MSYNC0, .VsDlyPeriod = 0, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0xFF, .VsPolInput = 0, .HsPolOutput = 0, .VsPolOutput = 0},
    {.RefClk = 24000000, .VsdelaySrc = AMBA_VIN_VSDLY_SRC_HV_MSYNC0, .VsDlyPeriod = 0, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0, .VsPolInput = 0xFF, .HsPolOutput = 0, .VsPolOutput = 0},
    {.RefClk = 24000000, .VsdelaySrc = AMBA_VIN_VSDLY_SRC_HV_MSYNC0, .VsDlyPeriod = 0, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0, .VsPolInput = 0, .HsPolOutput = 0xFF, .VsPolOutput = 0},
    {.RefClk = 24000000, .VsdelaySrc = AMBA_VIN_VSDLY_SRC_HV_MSYNC0, .VsDlyPeriod = 0, .HsDlyClk = 0, .VsDlyClk = 0, .HsPulseWidth = 0, .VsPulseWidth = 0,
     .HsPolInput = 0, .VsPolInput = 0, .HsPolOutput = 0, .VsPolOutput = 0xFF},

};

void DelayedVSyncEnable_Config_Get(UINT32 Index, AMBA_VIN_DELAYED_VSYNC_CONFIG_s* pConfig)
{
    memset(pConfig, 0, sizeof(AMBA_VIN_DELAYED_VSYNC_CONFIG_s));


    pConfig->FineAdjust.RefClk = DlyVsyncTC[Index].RefClk;
    pConfig->DelayedHVsyncSource = DlyVsyncTC[Index].VsdelaySrc;

    pConfig->VsDlyPeriod[0] = DlyVsyncTC[Index].VsDlyPeriod;
    pConfig->Polarity.HsPolInput = DlyVsyncTC[Index].HsPolInput;
    pConfig->Polarity.VsPolInput = DlyVsyncTC[Index].VsPolInput;
    pConfig->Polarity.HsPolOutput = DlyVsyncTC[Index].HsPolOutput;
    pConfig->Polarity.VsPolOutput[0] = DlyVsyncTC[Index].VsPolOutput;
}

void TEST_AmbaVIN_DelayedVSyncEnable(void)
{
    AMBA_VIN_DELAYED_VSYNC_CONFIG_s DelayedVSyncConfig = {0};
    UINT32 i;

    Set_RetVal_MutexTake(KAL_ERR_0000);
    DelayedVSyncEnable_Config_Get(0, &DelayedVSyncConfig);
    AmbaVIN_DelayedVSyncEnable(&DelayedVSyncConfig);
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    for (i = 0; i < (sizeof(DlyVsyncTC)/sizeof(DLY_VSYNC_TC_s)); i ++ ) {
        DelayedVSyncEnable_Config_Get(i, &DelayedVSyncConfig);
        AmbaVIN_DelayedVSyncEnable(&DelayedVSyncConfig);
    }

    AmbaVIN_DelayedVSyncEnable(&DelayedVSyncConfig);


    printf("TEST_AmbaVIN_DelayedVSyncEnable\n");
}


void TEST_AmbaVIN_DelayedVSyncDisable(void)
{
    Set_RetVal_MutexTake(KAL_ERR_0000);
    AmbaVIN_DelayedVSyncDisable();
    Set_RetVal_MutexTake(KAL_ERR_NONE);

    AmbaVIN_DelayedVSyncDisable();

    printf("TEST_AmbaVIN_DelayedVSyncDisable\n");

}

void TEST_AmbaVIN_BeforeInit(void)
{
    AMBA_VIN_SLVS_PAD_CONFIG_s SlvsPadConfig = {0};
    AMBA_VIN_MIPI_PAD_CONFIG_s MipiPadConfig = {0};
    AMBA_VIN_DVP_PAD_CONFIG_s DvpPadConfig = {0};
    AMBA_VIN_SLVS_CONFIG_s SlvsCfg = {0};
    AMBA_VIN_MIPI_CONFIG_s MipiCfg = {0};
    AMBA_VIN_DVP_CONFIG_s DvpCfg = {0};
    AMBA_VIN_MASTER_SYNC_CONFIG_s MSyncCfg = {0};
    AMBA_VIN_LANE_REMAP_CONFIG_s LaneRemapConfig = {0};
    AMBA_VIN_MIPI_VC_CONFIG_s MipiVirtChanConfig = {0};
    ULONG Addr;
    UINT32 Size;
    AMBA_VIN_WINDOW_s CaptureWindow = {0};
    AMBA_VIN_INFO_s VinInfo;
    AMBA_VIN_MIPI_EMB_DATA_CONFIG_s MipiEmbDataConfig = {0};
    AMBA_VIN_DELAYED_VSYNC_CONFIG_s DelayedVSyncConfig = {0};

    SlvsPadConfig.EnabledPin = SlvsResetTC[0].EnabledPin;
    SlvsReset_PadConfig_Get(0, &SlvsPadConfig);
    AmbaVIN_SlvsReset(SlvsResetTC[0].VinID, &SlvsPadConfig);

    MipiReset_PadConfig_Get(0, &MipiPadConfig);
    AmbaVIN_MipiReset(MipiResetTC[0].VinID, &MipiPadConfig);

    DvpReset_PadConfig_Get(0, &DvpPadConfig);
    AmbaVIN_DvpReset(DvpResetTC[0].VinID, &DvpPadConfig);

    SlvsConfig_VinCfg_Get(0, &SlvsCfg);
    AmbaVIN_SlvsConfig(SlvsConfigTC[0].VinID, &SlvsCfg);

    MipiConfig_VinCfg_Get(0, &MipiCfg);
    AmbaVIN_MipiConfig(MipiConfigTC[0].VinID, &MipiCfg);

    DvpConfig_VinCfg_Get(0, &DvpCfg);
    AmbaVIN_DvpConfig(DvpConfigTC[0].VinID, &DvpCfg);

    MasterSyncEnable_VMSyncCfg_Get(0, &MSyncCfg);
    AmbaVIN_MasterSyncEnable(MsyncConfigTC[0].MSyncID, &MSyncCfg);

    AmbaVIN_MasterSyncDisable(AMBA_VIN_MSYNC0);

    AmbaVIN_SetLvdsTermination(LvdsTermTC[0].VinID, LvdsTermTC[0].Value);

    AmbaVIN_SensorClkEnable(SensorClkTC[0].ClkID, SensorClkTC[0].Frequency);

    AmbaVIN_SensorClkDisable(AMBA_VIN_SENSOR_CLOCK0);

    AmbaVIN_SensorClkSetDrvStrength(AMBA_VIN_DRIVE_STRENGTH_12MA);

    DataLaneRemap_Config_Get(0, &LaneRemapConfig);
    AmbaVIN_DataLaneRemap(LaneRemapTC[0].VinID, &LaneRemapConfig);

    MipiVirtChanConfig_Config_Get(0, &MipiVirtChanConfig);
    AmbaVIN_MipiVirtChanConfig(MipiVcTC[0].VinID, &MipiVirtChanConfig);

    AmbaVIN_GetMainCfgBufInfo(AMBA_VIN_CHANNEL0, &Addr, &Size);

    CaptureConfig_CapWin_Get(0, &CaptureWindow);
    AmbaVIN_CaptureConfig(AMBA_VIN_CHANNEL0, &CaptureWindow);

    AmbaVIN_GetInfo(AMBA_VIN_CHANNEL0, &VinInfo);

    AmbaVIN_IsEnabled(AMBA_VIN_CHANNEL0);

    MipiEmbDataConfig_Config_Get( 0, &MipiEmbDataConfig);
    AmbaVIN_MipiEmbDataConfig(MipiEmbTC[0].VinID, &MipiEmbDataConfig);

    DelayedVSyncEnable_Config_Get(0, &DelayedVSyncConfig);
    AmbaVIN_DelayedVSyncEnable(&DelayedVSyncConfig);

    AmbaVIN_DelayedVSyncDisable();

}

int main(void)
{
    TEST_AmbaVIN_BeforeInit();

    /* avoid AmbaVIN_DrvEntry to update register address */
    Set_RetVal_MutexCreate(KAL_ERR_0000);
    AmbaVIN_DrvEntry();
    Set_RetVal_MutexCreate(KAL_ERR_NONE);

    Set_NoCopy_AmbaMisra_TypeCast(1U);
    AmbaVIN_DrvEntry();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    TEST_AmbaVIN_SlvsReset();
    TEST_AmbaVIN_MipiReset();
    TEST_AmbaVIN_DvpReset();
    TEST_AmbaVIN_SlvsConfig();
    TEST_AmbaVIN_MipiConfig();
    TEST_AmbaVIN_DvpConfig();

    TEST_AmbaVIN_MasterSyncEnable();

    TEST_AmbaVIN_MasterSyncDisable();

    TEST_AmbaVIN_SetLvdsTermination();

    TEST_AmbaVIN_SensorClkEnable();
    TEST_AmbaVIN_SensorClkDisable();
    TEST_AmbaVIN_SensorClkSetDrvStrength();

    TEST_AmbaVIN_DataLaneRemap();
    TEST_AmbaVIN_MipiVirtChanConfig();

    TEST_AmbaVIN_GetMainCfgBufInfo();
    TEST_AmbaVIN_CaptureConfig();
    TEST_AmbaVIN_GetInfo();
    TEST_AmbaVIN_IsEnabled();

    TEST_AmbaVIN_MipiEmbDataConfig();

    TEST_AmbaVIN_DelayedVSyncEnable();

    TEST_AmbaVIN_DelayedVSyncDisable();

    return 0;
}
