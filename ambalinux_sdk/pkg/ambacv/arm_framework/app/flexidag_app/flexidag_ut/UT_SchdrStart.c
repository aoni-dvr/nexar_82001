#include "cvapi_ambacv_flexidag.h"


static void Do_Prepare_Config(AMBA_CV_STANDALONE_SCHDR_CFG_s *cfg)
{
    cfg->flexidag_slot_num = 8;
    cfg->cavalry_slot_num = 0;
    cfg->cpu_map = 0xD;
    cfg->log_level = LVL_DEBUG;
    cfg->boot_cfg = 1 + FLEXIDAG_ENABLE_BOOT_CFG;
}

uint32_t main(void)
{
    const char orcvp_path[] = "/lib/firmware/";
    AMBA_CV_STANDALONE_SCHDR_CFG_s cfg = {0};
    uint32_t Rval = 0U;

    Rval = AmbaCV_SchdrLoad(orcvp_path);
    if (Rval == 0U) {
        Do_Prepare_Config(&cfg);
        Rval = AmbaCV_StandAloneSchdrStart(&cfg);
    }

    if (Rval != 0U) {
        printf("UT_SchdrStart fail (0x%x)\n", Rval);
    } else {
        printf("UT_SchdrStart success\n", Rval);
    }

    return 0;
}