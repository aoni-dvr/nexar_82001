
#include <test_group.h>
#include <AmbaRTSL_GIC.h>

typedef struct {
    AMBA_INT_ISR_f IntFunc;
    UINT32         FlagEnable;
} ISR_CONFIG_s;

static ISR_CONFIG_s isr_config_array[300];

UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg)
{
    if (IntID < 300) {
        isr_config_array[IntID].IntFunc = IntFunc;
    }
    return 0;
}

UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID)
{
    if (IntID < 300) {
        isr_config_array[IntID].FlagEnable = 1;
    }
    return 0;
}

UINT32 AmbaRTSL_GicIntDisable(UINT32 IntID)
{
    if (IntID < 300) {
        isr_config_array[IntID].FlagEnable = 0;
    }
    return 0;
}

UINT32 AmbaRTSL_GicGetIntInfo(UINT32 IntID, AMBA_INT_INFO_s *pIntInfo)
{
    if (IntID < 300) {
        return isr_config_array[IntID].FlagEnable;
    } else {
        return 0;
    }
}