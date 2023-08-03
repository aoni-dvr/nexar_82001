#include "cvapi_ambacv_flexidag.h"


#define SCHDR_ARM_ORC_RESET             0x0000U  //ARM ORC reset

static void Do_Prepare_Config(AMBA_CV_STANDALONE_SCHDR_CFG_s *cfg)
{
    cfg->flexidag_slot_num = 8;
    cfg->cavalry_slot_num = 0;
    cfg->cpu_map = 0xD;
    cfg->log_level = LVL_DEBUG;
    cfg->boot_cfg = 1 + FLEXIDAG_ENABLE_BOOT_CFG;
}

static uint32_t Do_SchdrStart(void)
{
    const char orcvp_path[] = "/lib/firmware/";
    uint32_t schdr_state;
    uint32_t Rval = 0U;

    Rval = AmbaCV_SchdrState(&schdr_state);
    if (Rval == 0U) {
        if (schdr_state == FLEXIDAG_SCHDR_OFF) {
            AMBA_CV_STANDALONE_SCHDR_CFG_s cfg = {0};

            Rval = AmbaCV_SchdrLoad(orcvp_path);
            if (Rval == 0U) {
                Do_Prepare_Config(&cfg);
                Rval = AmbaCV_StandAloneSchdrStart(&cfg);
            }
        } else {
            AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

            cfg.cpu_map = 0xD;
            cfg.log_level = LVL_DEBUG;
            Rval = AmbaCV_FlexidagSchdrStart(&cfg);
        }
    }

    if (Rval != 0U) {
        printf("Do_SchdrStart fail (0x%x)\n", Rval);
    }

    return Rval;
}

uint32_t main(void)
{
    uint32_t Rval = 0U;

    Rval = Do_SchdrStart();
    if (Rval == 0U) {
        Rval = AmbaCV_SchdrShutdown(SCHDR_ARM_ORC_RESET);
    }

    if (Rval != 0U) {
        printf("UT_SchdrShutdown fail (0x%x)\n", Rval);
    } else {
        printf("UT_SchdrShutdown success\n", Rval);
    }

    return 0;
}