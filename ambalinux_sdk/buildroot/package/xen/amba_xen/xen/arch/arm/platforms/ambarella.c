/*
 * xen/arch/arm/platforms/ambarella.c
 *
 * Copyright (C) 2020, Ambarella Inc.
 */

#include <asm/platform.h>
#include <asm/smccc.h>

static const char * const ambarella_dt_compat[] __initconst =
{
    "ambarella,cv2fs",
    NULL
};

static bool ambarella_smc(struct cpu_user_regs *regs)
{
    bool ret = false;
    struct arm_smccc_res res;
    uint32_t fid = get_user_reg(regs, 0);

    switch (fid) {
    case 0x8200ff81:
    case 0x8200ff82:
    case 0x8200ff83:
    case 0x8200ff84:
    case 0x8200ff86:
    case 0x8200ff87:
        arm_smccc_1_1_smc(get_user_reg(regs, 0),
                          get_user_reg(regs, 1),
                          get_user_reg(regs, 2),
                          get_user_reg(regs, 3),
                          get_user_reg(regs, 4),
                          get_user_reg(regs, 5),
                          get_user_reg(regs, 6),
                          get_user_reg(regs, 7),
                          &res);
        set_user_reg(regs, 0, res.a0);
        set_user_reg(regs, 1, res.a1);
        set_user_reg(regs, 2, res.a2);
        set_user_reg(regs, 3, res.a3);
        ret = true;
        break;
    default:
        gprintk(XENLOG_WARNING, "ambarella: unhandled smc call: 0x%x\n", fid);
        ret = false;
        break;
    }

    return ret;
}

PLATFORM_START(ambarella, "Ambarella")
    .compatible = ambarella_dt_compat,
    .smc = ambarella_smc,
PLATFORM_END

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
