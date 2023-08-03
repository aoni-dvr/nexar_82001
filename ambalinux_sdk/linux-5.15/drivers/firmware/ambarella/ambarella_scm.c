/*
 * Ambarella SCM driver
 *
 * Copyright (C) 2017 Ambarella Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/arm-smccc.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/freezer.h>
#include "ambarella_scm.h"

#if defined(CONFIG_ARCH_AMBA_CAMERA)
typedef struct {
    uint32_t  ClientRequestStatis[32];    /* Statistics for each client's number of requests */
    uint32_t  ClientBurstStatis[32];      /* Statistics for each client's number of bursts */
    uint32_t  ClientMaskWriteStatis[32];  /* Statistics for each client's number of masked write bursts */
} AMBA_DRAMC_STATIS_s;
#endif

static int ambarella_scm_query(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SCM_SVC_QUERY, AMBA_SCM_QUERY_VERSION);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}

int ambarella_aarch64_cntfrq_update(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SCM_SVC_FREQ, AMBA_SCM_CNTFRQ_SETUP_CMD);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_aarch64_cntfrq_update);

/*
 *  Ambarella memory monitor
 */
int ambarella_scm_monitor_config(size_t addr, uint32_t length, uint32_t mode)
{
	u32 fn, cmd;
	struct arm_smccc_res res;
#ifdef FREEZE_SYSTEM
	int error;
	error = freeze_processes();
	if (error)
		return -EBUSY;
	error = freeze_kernel_threads();
	if (error) {
		thaw_processes();
		return -EBUSY;
	}
#endif

	fn = SVC_SCM_FN(AMBA_SIP_MEMORY_MONITOR, AMBA_SIP_MONITOR_CONFIG);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, addr, length, mode, 0, 0, 0, 0, &res);

#ifdef FREEZE_SYSTEM
	thaw_processes();
	thaw_kernel_threads();
#endif

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_monitor_config);

int ambarella_scm_monitor_enable(size_t addr, uint32_t length, uint32_t mode)
{
	u32 fn, cmd;
	struct arm_smccc_res res;
#ifdef FREEZE_SYSTEM
	int error;
	error = freeze_processes();
	if (error)
		return -EBUSY;
	error = freeze_kernel_threads();
	if (error) {
		thaw_processes();
		return -EBUSY;
	}
#endif

	fn = SVC_SCM_FN(AMBA_SIP_MEMORY_MONITOR, AMBA_SIP_MONITOR_ENABLE);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, addr, length, mode, 0, 0, 0, 0, &res);

#ifdef FREEZE_SYSTEM
	thaw_processes();
	thaw_kernel_threads();
#endif

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_monitor_enable);

int ambarella_scm_monitor_disable(size_t addr, uint32_t length, uint32_t mode)
{
	u32 fn, cmd;
	struct arm_smccc_res res;
#ifdef FREEZE_SYSTEM
	int error;
	error = freeze_processes();
	if (error)
		return -EBUSY;
	error = freeze_kernel_threads();
	if (error) {
		thaw_processes();
		return -EBUSY;
	}
#endif

	fn = SVC_SCM_FN(AMBA_SIP_MEMORY_MONITOR, AMBA_SIP_MONITOR_DISABLE);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, addr, length, mode, 0, 0, 0, 0, &res);

#ifdef FREEZE_SYSTEM
	thaw_processes();
	thaw_kernel_threads();
#endif

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_monitor_disable);

/* Software Reset VP cluster */
int ambarella_scm_soft_reset_vp(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_VP_CONFIG, AMBA_SIP_VP_CONFIG_RESET);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);
	if (res.a0)
		return -EINVAL;

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_soft_reset_vp);

/* uuidbuf space at least 128bits */
int ambarella_otp_get_uuid(u32 *uuidbuf)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	if (!uuidbuf) {
		return -EINVAL;
	}

	fn = SVC_SCM_FN(AMBA_SIP_ACCESS_OTP, AMBA_SIP_GET_AMBA_UNIQUE_ID);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	if (res.a0) {
		return -EINVAL;
	}

	uuidbuf[0] = res.a1 & 0xFFFFFFFF;
	uuidbuf[1] = (res.a1 >> 32) & 0xFFFFFFFF;
	uuidbuf[2] = res.a2 & 0xFFFFFFFF;
	uuidbuf[3] = (res.a2 >> 32) & 0xFFFFFFFF;

	return res.a0;
}
EXPORT_SYMBOL(ambarella_otp_get_uuid);

int ambarella_scm_lp5_adjust_islp5(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_ISLP5);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_islp5);

int ambarella_scm_lp5_adjust_init(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_INIT);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_init);

int ambarella_scm_lp5_adjust_run(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_RUN);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_run);

int ambarella_scm_lp5_adjust_set_wck2dqi_timer(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_SET_WCK2DQI_TIMER);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_set_wck2dqi_timer);

int ambarella_scm_lp5_adjust_show_switch(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_SHOW_SWITCH);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_show_switch);

int ambarella_scm_lp5_adjust_set_pvalue(uint32_t pval)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_SET_PVAL);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, pval, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_set_pvalue);

int ambarella_scm_lp5_adjust_get_pvalue(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_GET_PVAL);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_get_pvalue);

int ambarella_scm_lp5_adjust_set_nvalue(uint32_t nval)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_SET_NVAL);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, nval, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_set_nvalue);

int ambarella_scm_lp5_adjust_get_nvalue(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_LP5_ADJUST, AMBA_SIP_LP5_ADJUST_GET_NVAL);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_lp5_adjust_get_nvalue);

#if defined(CONFIG_ARCH_AMBA_CAMERA)
int ambarella_scm_dramc_enable_statis_ctrl(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_DRAMC_STATS, AMBA_SIP_DRAMC_STATS_ENABLE);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_dramc_enable_statis_ctrl);

int ambarella_scm_dramc_disable_statis_ctrl(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_DRAMC_STATS, AMBA_SIP_DRAMC_STATS_DISABLE);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_dramc_disable_statis_ctrl);

AMBA_DRAMC_STATIS_s* ambarella_scm_dramc_get_statis_info(void)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_DRAMC_STATS, AMBA_SIP_DRAMC_STATS_GET_INFO);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, 0, 0, 0, 0, 0, 0, 0, &res);

	return (AMBA_DRAMC_STATIS_s*)res.a0;
}
EXPORT_SYMBOL(ambarella_scm_dramc_get_statis_info);

uint32_t ambarella_scm_dramc_read(uint32_t offset)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_DRAMC_OPS, AMBA_SIP_DRAMC_OPS_READ);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, offset, 0, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_dramc_read);

void ambarella_scm_dramc_write(uint32_t offset, uint32_t value)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_DRAMC_OPS, AMBA_SIP_DRAMC_OPS_WRITE);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, offset, value, 0, 0, 0, 0, 0, &res);
}
EXPORT_SYMBOL(ambarella_scm_dramc_write);

uint32_t ambarella_scm_ddrc_read(uint32_t ddrc, uint32_t offset)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_DDRC_OPS, AMBA_SIP_DDRC_OPS_READ);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, ddrc, offset, 0, 0, 0, 0, 0, &res);

	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_ddrc_read);

void ambarella_scm_ddrc_write(uint32_t ddrc, uint32_t offset, uint32_t value)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_DDRC_OPS, AMBA_SIP_DDRC_OPS_WRITE);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, ddrc, offset, value, 0, 0, 0, 0, &res);
}
EXPORT_SYMBOL(ambarella_scm_ddrc_write);
#endif

int ambarella_scm_hsm_init_queue(void *arg)
{
	u32 fn, cmd;
	struct arm_smccc_res res;

	fn = SVC_SCM_FN(AMBA_SIP_HSM_CALL, AMBA_SIP_HSM_INIT_QUEUE);
	cmd = ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL, ARM_SMCCC_SMC_64,
			ARM_SMCCC_OWNER_SIP, fn);

	arm_smccc_smc(cmd, (uint64_t)arg, 0, 0, 0, 0, 0, 0, &res);
	return res.a0;
}
EXPORT_SYMBOL(ambarella_scm_hsm_init_queue);
/* ---------------------------------------------------------------------------- */
int __init ambarella_scm_init(void)
{
	int rval, len;
	const char *method;
	struct device_node *node;

	node = of_find_node_by_name(NULL, "psci");
	if (node == NULL)
		return 0;

	method = of_get_property(node, "method", &len);
	if (method == NULL) {
		pr_err("'method' property is not found.\n");
		return 0;
	}

	/* if psci method is set as spin table, return to not access smc */
	if (strncmp(method, "smc", 3))
		return 0;

	rval = ambarella_scm_query();
	if ( rval != ARM_SMCCC_SMC_64)
		pr_warn("Ambarella SCM is not implemented, skip ...\n");

	return 0;
}
arch_initcall(ambarella_scm_init);
