#include <stdio.h>
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaRTSL_ClkMon.h"
#include "AmbaCSL_ClkMon.h"
#include "AmbaClkMon_Def.h"
#include "AmbaReg_ScratchpadNS.h"

AMBA_SCRATCHPAD_NS_REG_s AmbaScratchpadNS_Reg = {0};
AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg = &AmbaScratchpadNS_Reg;
AMBA_RCT_REG_s AmbaRCT_Reg = {0};
AMBA_RCT_REG_s *const pAmbaRCT_Reg = &AmbaRCT_Reg;

extern void Set_NoCopy_AmbaMisra_TypeCast(UINT32 NoCopy);

void TEST_AmbaRTSL_ClkMonGetCount(void)
{
    UINT32 ClkMonID = 0U;
    UINT32 ClkCountsVal = 0U;
    UINT32 *pClkCountsVal = &ClkCountsVal;

    for (ClkMonID = 0U; ClkMonID < AMBA_NUM_PLL; ClkMonID ++)
    {
        AmbaRTSL_ClkMonGetCount(ClkMonID, pClkCountsVal);
    }

    for (ClkMonID = AMBA_NUM_PLL; ClkMonID < AMBA_CKM_IDX_MAX; ClkMonID ++)
    {
        AmbaCSL_ClkMonGetCount(ClkMonID);
    }

    printf("TEST_AmbaRTSL_ClkMonGetCount\n");
}

void TEST_AmbaRTSL_ClkMonSetCtrlManual(void)
{
    UINT32 ClkMonID = 0U;
    UINT8 Enable = 0U;
    UINT16 UpperBound = 0U;
    UINT16 LowerBound = 0U;
    AmbaRTSL_ClkMonSetCtrlManual(ClkMonID, Enable, UpperBound, LowerBound);

    for (ClkMonID = 7U; ClkMonID < AMBA_CKM_IDX_MAX; ClkMonID ++)
    {
        if (((ClkMonID < 36U) || (ClkMonID > 52U)) && (ClkMonID != 54U)) {
            AmbaRTSL_ClkMonSetCtrlManual(ClkMonID, Enable, UpperBound, LowerBound);
        }
    }

    ClkMonID = AMBA_CKM_IDX_MAX;
    AmbaRTSL_ClkMonSetCtrlManual(ClkMonID, Enable, UpperBound, LowerBound);

    printf("TEST_AmbaRTSL_ClkMonSetCtrlManual\n");
}

void TEST_AmbaRTSL_ClkMonGetCtrl(void)
{
    UINT32 ClkMonID = 0U;
    UINT8 Enable = 0U;
    UINT8 *pEnable = &Enable;
    UINT16 UpperBound = 0U;
    UINT16 *pUpperBound = &UpperBound;
    UINT16 LowerBound = 0U;
    UINT16 *pLowerBound = &LowerBound;
    AmbaRTSL_ClkMonGetCtrl(ClkMonID, pEnable, pUpperBound, pLowerBound);

    for (ClkMonID = 7U; ClkMonID < AMBA_CKM_IDX_MAX; ClkMonID ++)
    {
        if (((ClkMonID < 36U) || (ClkMonID > 52U)) && (ClkMonID != 54U)) {
            AmbaRTSL_ClkMonGetCtrl(ClkMonID, pEnable, pUpperBound, pLowerBound);
        }
    }

    ClkMonID = AMBA_CKM_IDX_MAX;
    AmbaRTSL_ClkMonGetCtrl(ClkMonID, pEnable, pUpperBound, pLowerBound);

    printf("TEST_AmbaRTSL_ClkMonGetCtrl\n");
}


int main(void)
{
    TEST_AmbaRTSL_ClkMonGetCount();

    /* avoid AmbaCSL_ClkMon.c to update register address */
    Set_NoCopy_AmbaMisra_TypeCast(1U);
    TEST_AmbaRTSL_ClkMonSetCtrlManual();
    TEST_AmbaRTSL_ClkMonGetCtrl();
    Set_NoCopy_AmbaMisra_TypeCast(0U);

    return 0;
}

