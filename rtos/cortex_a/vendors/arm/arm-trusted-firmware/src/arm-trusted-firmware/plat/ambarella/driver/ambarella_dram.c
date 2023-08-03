/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>
#include <lib/mmio.h>
#include <common/runtime_svc.h>
#include <plat_private.h>
#include <ambarella_smc.h>
#include <drivers/delay_timer.h>
#if defined(CONFIG_ATF_AMBALINK)
#include "ambarella_dram.h"
#endif

#define DRAMC_OFFSET 				0x0000
#define DDRCB_OFFSET  				0x4000
#define DDRC0_OFFSET  				0x5000
#define DDRC1_OFFSET  				0x6000
#define DDRCH_REG(x, y) 			(DDRC_BASE + DDRC0_OFFSET + (x) * 0x1000 + (y))

#define DRAM_CONTRL 				0x00000000
#define DRAM_CONFIG1				0x00000004
#define DRAM_CONFIG2				0x00000008
#define DRAM_TIMING1				0x0000000c
#define DRAM_TIMING2				0x00000010
#define DRAM_TIMING3				0x00000014
#define DRAM_TIMING4				0x00000018
#define DRAM_TIMING5				0x0000001c
#define DRAM_DUAL_DIE_TIMING			0x00000020
#define DRAM_REFRESH_TIMING 			0x00000024
#define DRAM_LP5_TIMING 			0x00000028
#define DRAM_INIT_CTL				0x0000002c
#define DRAM_MODE_REG				0x00000030
#define DRAM_SELF_REFRESH			0x00000034
#define DRAM_RSVD_SPACE 			0x00000038
#define DRAM_BYTE_MAP				0x0000003c
#define DRAM_MPC_WDATA				0x00000040
#define DRAM_MPC_WMASK				0x00000060
#define DRAM_MPC_RDATA				0x00000064
#define DRAM_MPC_RMASK				0x000000a4
#define DRAM_UINST1				0x000000ac
#define DRAM_UINST2				0x000000b0
#define DRAM_UINST3				0x000000b4
#define DRAM_UINST4				0x000000b8
#define DRAM_UINST5				0x000000bc
#define DRAM_UINST6				0x000000c0
#define DRAM_SCRATCHPAD 			0x000000c4
#define DRAM_WDQS_TIMING			0x000000c8
#define DRAM_CLEAR_MPC_DATA			0x000000cc
#define DRAM_IO_CONTROL 			0x000000d0
#define DRAM_DDRC_MISC_1			0x000000d4
#define DRAM_DTTE_CONFIG			0x00000100
#define DRAM_DTTE_DELAY_REG(n)			(0x00000108 + ((n) * 4))
#define DRAM_DTTE_CMD_REG 			0x00000130
#define DRAM_DTTE_DELAY_MAP			0x00000134
#define DRAM_DTTE_CP_INFO			0x00000138
#define DTTE_ALGO_ERROR 			0x00000148
#define DRAM_WRITE_VREF_0			0x00000164
#define DRAM_WRITE_VREF_1			0x00000168
#define DRAM_DTTE_TIMING			0x0000016c
#define DDRIO_DLL_CTRL_SBC(b)			(0x00000200 + (b) * 4)
#define DDRIO_DLL_CTRL_SEL(s, d)		(0x00000210 + (s) * 8 + (d) * 4)
#define DDRIO_BYTE_DLY0(b, d)			(0x00000228 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY1(b, d)			(0x0000022c + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY2(b, d)			(0x00000230 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY3(b, d)			(0x00000234 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY4(b, d)			(0x00000238 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY5(b, d)			(0x0000023c + (b) * 24 + (d) * 96)
#define DDRIO_CK_DELAY				0x000002e8
#define DDRIO_CA_DELAY_COARSE			0x000002ec
#define DDRIO_CA_DELAY_FINE_0(c, d)		(0x000002f0 + (c) * 16 + (d) * 8)
#define DDRIO_CA_DELAY_FINE_1(c, d)		(0x000002f4 + (c) * 16 + (d) * 8)
#define DDRIO_CS_DELAY				0x00000310
#define DDRIO_CKE_DELAY_COARSE			0x00000314
#define DDRIO_CKE_DELAY_FINE			0x00000318
#define DDRIO_DQS_TESTIRCV			0x0000035c
#define DDRIO_PAD_CTRL				0x00000360
#define DDRIO_DQS_PUPD				0x00000364
#define DDRIO_ZCTRL				0x0000036c
#define DDRIO_CA_PADCTRL			0x00000370
#define DDRIO_DQ_PADCTRL			0x00000374
#define DDRIO_VREF_0				0x00000378
#define DDRIO_VREF_1				0x0000037c
#define DDRIO_IBIAS_CTRL			0x00000380
#define DDRIO_ZCTRL_STATUS			0x00000384
#define DDRIO_DLL_STATUS_0			0x00000388
#define DDRIO_DLL_STATUS_1			0x0000038c
#define DRAM_DDRC_STATUS			0x000003a4
#define DRAM_DDRC_MISC_2			0x000003c0
#define DRAM_READ_LINK_ECC_STATUS(b)		(0x000003c4 + (b) * 4)
#define DRAM_DDRC_MR46				0x000003d4

/* DRAM_CTL register bit */
#define DRAM_CONTROL_AR_CNT_RELOAD		0x00000004
#define DRAM_CONTROL_AUTO_REF_EN		0x00000002
#define DRAM_CONTROL_ENABLE			0x00000001

/* DRAM_IO_CONTROL register bit */
#define	DRAM_IO_CONTROL_CKE(d, c)		(1 << ((d) * 2 + (c)))
#define DRAM_IO_CONTROL_CKE_ALL			0x0000000f
#define	DRAM_IO_CONTROL_DISABLE_RESET		0x00000010
#define	DRAM_IO_CONTROL_DISABLE_CMD		0x00000020

/* DRAM_INIT_CTL register bit */
#define DRAM_INIT_CTL_RTT_DIE			0x00000400
#define DRAM_INIT_CTL_PAD_CLB_LONG_EN		0x00000200
#define DRAM_INIT_CTL_PAD_CLB_SHORT_EN		0x00000100
#define DRAM_INIT_CTL_DLL_RST_EN		0x00000008
#define DRAM_INIT_CTL_GET_RTT_EN		0x00000004

/* DDRIO_ZCTRL register bit */
#define DDRIO_ZCTRL_PAD_CLB_LONG		0x00000080
#define DDRIO_ZCTRL_PAD_CLB_SHORT		0x00000100
#define DDRIO_ZCTRL_PAD_RESET			0x00000200

/* DDRIO_ZCTRL_STATUS register bit */
#define DDRIO_ZCTRL_STATUS_ACK			0x00002000

#if defined(CONFIG_ATF_AMBALINK)
/* DRAMC base address and reg offset */
#define DRAMC_REG(x)                            (DDRC_BASE + DRAMC_OFFSET + x)
#define DRAMC_OFFSET_DRAMMODE                   0x0000
#define DRAMC_OFFSET_CLIENTREQUESTSTATIS(i)     (0x1500 + (i) * 4)
#define DRAMC_OFFSET_CLIENTBURSTSTATIS(i)       (0x1580 + (i) * 4)
#define DRAMC_OFFSET_CLIENTMASKWRITESTATIS(i)   (0x1600 + (i) * 4)
#define DRAMC_OFFSET_DRAMSTATISCTRL             0x1808
#endif

static uint32_t ddrcnum;
static uint32_t ranknum;
static int32_t wck2dqi_last[2][2][2];
static int32_t fine_fs[2][2][2];
static int32_t step_record[2][2][2];
static int32_t debug;
static int32_t negative_adjust_value = 2500;
static int32_t positive_adjust_value = 3200;
#if defined(CONFIG_ATF_AMBALINK)
/**
 * Mapping copied from AmbaDramTrain.h in BLD to share the same spaces
 */
#define DDRCT_LOG_SPACE     0x20e0030000ULL
#define DDRCT_LOG_SIZE      12*1024
#define DDRCT_STACK_SPACE   (DDRCT_LOG_SPACE + DDRCT_LOG_SIZE)
#define DDRCT_STACK_SIZE    (4*1024 - 128)
#define DDRCT_RESULT_SPACE  (DDRCT_STACK_SPACE + DDRCT_STACK_SIZE)
#define DDRCT_RESULT_SIZE   128
#define DRAM_STATS_SPACE    DDRCT_LOG_SPACE     // reuse log space
static AMBA_DRAMC_STATIS_s* dram_stats = (AMBA_DRAMC_STATIS_s*)DRAM_STATS_SPACE;
#endif

static uint32_t ddrc_mrr(uint32_t ddrc, uint32_t addr, uint32_t did, uint32_t cid)
{
	uint32_t data, temp;

	did = (did == 0x3) ? 0x3 : 1 << did;
	cid = (cid == 0x3) ? 0x3 : 1 << cid;

	temp = (1U << 31) | (cid << 27) | (did << 25) | (addr << 16);

	mmio_write_32(DDRCH_REG(ddrc, DRAM_MODE_REG), temp);

	while (1) {
		temp = mmio_read_32(DDRCH_REG(ddrc, DRAM_MODE_REG));
		if ((temp & (1U << 31)) == 0)
			break;
	}

	if (cid == 1)
		data = temp & 0xff;
	else if (cid == 2)
		data = (temp & 0xff00) >> 8;
	else
		data = temp & 0xffff;

	return data;
}

static void ddrc_mrw(uint32_t ddrc, uint32_t addr, uint32_t data, uint32_t did, uint32_t cid)
{
	uint32_t temp;

	did = (did == 0x3) ? 0x3 : 1 << did;
	cid = (cid == 0x3) ? 0x3 : 1 << cid;

	temp = (1U << 31) | (cid << 27) | (did << 25) | (1 << 24) | (addr << 16) | data;

	mmio_write_32(DDRCH_REG(ddrc, DRAM_MODE_REG), temp);

	while (1) {
		temp = mmio_read_32(DDRCH_REG(ddrc, DRAM_MODE_REG));
		if ((temp & (1U << 31)) == 0)
			break;
	}
}

static void ddrc_uinst(uint32_t ddrc, uint32_t uinst1, uint32_t uinst2, uint32_t uinst4)
{
	mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST1), uinst1);
	mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST2), uinst2);
	mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST4), uinst4);

	mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST5), 0x1);
	while (mmio_read_32(DDRCH_REG(ddrc, DRAM_UINST5)) & 0x1);
}

static void ddrc_calc_wck2dqx(int32_t count_wck2dqi[2][2][2], int32_t fs_wck2dqi[2][2][2])
{
	uint32_t c, d, data_mr35, data_mr36, count;
	uint64_t freq;

	/* fs(femtosecond): fs_wck2dqx = 8192 * 1000000000 / f / count_wck2dqx / 2 */
	freq = get_ddr_freq_hz() / 4; /* CK rather than WCK */
	freq = freq / 1000000;

	for (c = 0; c < ddrcnum; c++) {
		if (!count_wck2dqi && !fs_wck2dqi)
			break;

		ddrc_mrw(c, 37, 0xc0, 0x3, 0x3);
		ddrc_uinst(c, 0xd, 0x170, 0x303);
		udelay(100);
		for (d = 0; d < ranknum; d++) {
			data_mr35 = ddrc_mrr(c, 35, d, 0x3);
			data_mr36 = ddrc_mrr(c, 36, d, 0x3);

			/* Channel A */
			count = ((data_mr35 >> 0) & 0xff) | (((data_mr36 >> 0) & 0xff) << 8);
			if (count_wck2dqi)
				count_wck2dqi[c][d][0] = count;
			if (fs_wck2dqi)
				fs_wck2dqi[c][d][0] = 1000000000ULL * 8192 / freq / count / 2;

			/* Channel B */
			count = ((data_mr35 >> 8) & 0xff) | (((data_mr36 >> 8) & 0xff) << 8);
			if (count_wck2dqi)
				count_wck2dqi[c][d][1] = count;
			if (fs_wck2dqi)
				fs_wck2dqi[c][d][1] = 1000000000ULL * 8192 / freq / count / 2;
		}
	}
}

static int ddrc_adjust_wdq(uint32_t c_mask, uint32_t d_mask, uint32_t b_mask, int32_t offset)
{
	uint32_t dly0_val[2][2][4], dly1_val[2][2][4], dly2_val[2][2][4], dly3_val[2][2][4];
	int32_t fine[2][2][4][11], c, d, b, i, adjust_wck;

	for (c = 0; c < ddrcnum; c++) {
		if (!(c_mask & (1 << c)))
			continue;

		for (d = 0; d < ranknum; d++) {
			if (!(d_mask & (1 << d)))
				continue;

			for (b = 0; b < 4; b++) {
				if (!(b_mask & (1 << b)))
					continue;

				dly0_val[c][d][b] = mmio_read_32(DDRCH_REG(c, DDRIO_BYTE_DLY0(b, d)));
				dly1_val[c][d][b] = mmio_read_32(DDRCH_REG(c, DDRIO_BYTE_DLY1(b, d)));
				dly2_val[c][d][b] = mmio_read_32(DDRCH_REG(c, DDRIO_BYTE_DLY2(b, d)));
				dly3_val[c][d][b] = mmio_read_32(DDRCH_REG(c, DDRIO_BYTE_DLY3(b, d)));

				for (i = 0; i < 4; i++)
					fine[c][d][b][i] = (dly2_val[c][d][b] >> (i * 8)) & 0x1f;
				for (; i < 8; i++)
					fine[c][d][b][i] = (dly3_val[c][d][b] >> ((i - 4) * 8)) & 0x1f;
				fine[c][d][b][8] = (dly1_val[c][d][b] >> 20) & 0x1f;
				fine[c][d][b][9] = (dly0_val[c][d][b] >> 10) & 0x1f;
				fine[c][d][b][10] = (dly0_val[c][d][b] >> 0) & 0x1f;
			}
		}
	}

	for (c = 0; c < ddrcnum; c++) {
		if (!(c_mask & (1 << c)))
			continue;

		for (d = 0; d < ranknum; d++) {
			if (!(d_mask & (1 << d)))
				continue;

			for (b = 0; b < 4; b++) {
				if (!(b_mask & (1 << b)))
					continue;

				adjust_wck = 0;
				for (i = 0; i < 10; i++) {
					fine[c][d][b][i] += offset;
					if (fine[c][d][b][i] < 0 || fine[c][d][b][i] > 31) {
						fine[c][d][b][i] = fine[c][d][b][i] < 0 ? 0 : 31;
						adjust_wck = 1;
					}
				}

				if (adjust_wck) {
					fine[c][d][b][10] -= offset; /* reverse direction of WCK */
					if (fine[c][d][b][10] < 0 || fine[c][d][b][10] > 31) {
						NOTICE("C%dD%dB%d WCK overflow/underflow: %d!\n",
							c, d, b, fine[c][d][b][10]);
						return -1;
					}

					dly0_val[c][d][b] &= ~(0x1f << 0);
					dly0_val[c][d][b] |= (fine[c][d][b][10] << 0);
					mmio_write_32(DDRCH_REG(c, DDRIO_BYTE_DLY0(b, d)), dly0_val[c][d][b]);
					continue;
				}

				for (i = 0; i < 4; i++) {
					dly2_val[c][d][b] &= ~(0x1f << (i * 8));
					dly2_val[c][d][b] |= fine[c][d][b][i] << (i * 8);
					mmio_write_32(DDRCH_REG(c, DDRIO_BYTE_DLY2(b, d)), dly2_val[c][d][b]);
				}

				for (; i < 8; i++) {
					dly3_val[c][d][b] &= ~(0x1f << ((i - 4) * 8));
					dly3_val[c][d][b] |= fine[c][d][b][i] << ((i - 4) * 8);
					mmio_write_32(DDRCH_REG(c, DDRIO_BYTE_DLY3(b, d)), dly3_val[c][d][b]);
				}

				dly1_val[c][d][b] &= ~(0x1f << 20);
				dly1_val[c][d][b] |= fine[c][d][b][8] << 20;
				mmio_write_32(DDRCH_REG(c, DDRIO_BYTE_DLY1(b, d)), dly1_val[c][d][b]);

				dly0_val[c][d][b] &= ~(0x1f << 10);
				dly0_val[c][d][b] |= (fine[c][d][b][9] << 10);
				mmio_write_32(DDRCH_REG(c, DDRIO_BYTE_DLY0(b, d)), dly0_val[c][d][b]);
			}
		}
	}

	return 0;
}

void ambarella_lp5_adjust_run(void)
{
	int32_t wck2dqi[2][2][2];
	int32_t wck2dqi_diff, c, d, ch, mr43, step;

	ddrc_calc_wck2dqx(NULL, wck2dqi);

	for (c = 0; c < ddrcnum; c++) {
		for (d = 0; d < ranknum; d++) {
			for (ch = 0; ch < 2; ch++) {
				mr43 = ddrc_mrr(c, 43, d, ch) & 0xbf;

				wck2dqi_diff = wck2dqi[c][d][ch] - wck2dqi_last[c][d][ch];

				if (wck2dqi_diff > 0 && wck2dqi_diff > fine_fs[c][d][ch])
					step = (wck2dqi_diff + fine_fs[c][d][ch] / 2) / fine_fs[c][d][ch];
				else if (wck2dqi_diff < 0 && wck2dqi_diff < -fine_fs[c][d][ch])
					step = (wck2dqi_diff - fine_fs[c][d][ch] / 2) / fine_fs[c][d][ch];
				else
					step = 0;
				if (debug)
					NOTICE("C%dD%dch%s: wck2dqi = %06dfs, wck2dqi_last = %06dfs, "
						"wck2dqi_diff = %06dfs, fine = %04dfs, mr43 = 0x%02x, step = %d\n",
						c, d, ch ? "B" : "A",
						wck2dqi[c][d][ch],
						wck2dqi_last[c][d][ch],
						wck2dqi_diff,
						fine_fs[c][d][ch],
						mr43, step_record[c][d][ch]);

				if (step != 0) {
					if (debug)
						NOTICE("C%dD%dch%s: adjust by %d, mr43 = 0x%02x\n",
							c, d, ch ? "B" : "A", step, mr43);

					if((step_record[c][d][ch] >= 0) && (step_record[c][d][ch] + step >= 0))
					        wck2dqi_last[c][d][ch] += positive_adjust_value * step;
					else if((step_record[c][d][ch] <= 0) && (step_record[c][d][ch] + step <= 0))
					        wck2dqi_last[c][d][ch] += negative_adjust_value * step;
					else if((step_record[c][d][ch] < 0) && (step_record[c][d][ch] + step > 0))
					        wck2dqi_last[c][d][ch] += (step_record[c][d][ch] + step) * positive_adjust_value - step_record[c][d][ch] * negative_adjust_value;
					else if ((step_record[c][d][ch] > 0) && (step_record[c][d][ch] + step < 0))
					        wck2dqi_last[c][d][ch] += (step_record[c][d][ch] + step) * negative_adjust_value - step_record[c][d][ch] * positive_adjust_value;

					ddrc_adjust_wdq(0x1 << c, 0x1 << d, 0x3 << (ch << 1), step);

					step_record[c][d][ch] += step;

					if (step_record[c][d][ch] < 0)
						fine_fs[c][d][ch] = negative_adjust_value;
					else
						fine_fs[c][d][ch] = positive_adjust_value;
				}
			}
		}
	}

	if (debug)
		NOTICE("\n");

	/* Perform ZQ calibration */
	for (c = 0; c < ddrcnum; c++)
		ddrc_uinst(c, 0xd, 0x670, 0x303);
}

void ambarella_lp5_adjust_set_pval(uint32_t pval)
{
	positive_adjust_value = pval;
	NOTICE("postive adjust value is %d fs\n", positive_adjust_value);
}

uint32_t ambarella_lp5_adjust_get_pval(void)
{
	return positive_adjust_value;
}

void ambarella_lp5_adjust_set_nval(uint32_t nval)
{
	negative_adjust_value = nval;
	NOTICE("negtive adjust value is %d fs\n", negative_adjust_value);
}

uint32_t ambarella_lp5_adjust_get_nval(void)
{
	return negative_adjust_value;
}

void ambarella_lp5_adjust_init(void)
{
	int c, d, ch;
#if defined(CONFIG_ATF_AMBALINK)
    int32_t *wck2dqi_trained = (int32_t*)(DDRCT_RESULT_SPACE + 32);
    int32_t i = 0;
#endif

	if((mmio_read_32(DDRC_REG(0)) & 0x06) == 0x06)
		ddrcnum = 2;
	else
		ddrcnum = 1;

	if(mmio_read_32(DDRCH_REG(0, DRAM_CONFIG1)) & (1 << 26))
		ranknum = 2;
	else
		ranknum = 1;

	ddrc_calc_wck2dqx(NULL, wck2dqi_last);

	for (c = 0; c < ddrcnum; c++) {
		for (d = 0; d < ranknum; d++) {
			for (ch = 0; ch < 2; ch++) {
#if defined(CONFIG_ATF_AMBALINK)
                if (wck2dqi_trained[i] != 0) {
                    wck2dqi_last[c][d][ch] = wck2dqi_trained[i];
                    if (debug) {
                        NOTICE("C%dD%dch%d: wck2dqi = %06dfs, wck2dqi_last = %06dfs\n",
                               c, d, ch,
                               0,
                               wck2dqi_last[c][d][ch]);
                    }
                }
                i++;
#endif
				step_record[c][d][ch] = 0;
				fine_fs[c][d][ch] = positive_adjust_value;
			}
		}
	}
}

uint32_t ambarella_lp5_adjust_islp5(void)
{
	return (mmio_read_32(DDRCH_REG(0, DRAM_CONFIG1)) & 0x3) ? 1 : 0;
}

void ambarella_lp5_adjust_show_switch(void)
{
	debug = !debug;
}

#if defined(CONFIG_ATF_AMBALINK)
void ambarella_dramc_reset_statis_ctrl(void)
{
    uint32_t count = 0U;
    uint32_t index;

    //Clean structure to 0
    for (index = 0U; index < 32U; index++) {
        dram_stats->ClientRequestStatis[index] = 0;
        dram_stats->ClientBurstStatis[index] = 0;
        dram_stats->ClientMaskWriteStatis[index] = 0;
    }

    //Reset, When reset is done, the read value will be changed to 0(OFF)
    mmio_write_32(DRAMC_REG(DRAMC_OFFSET_DRAMSTATISCTRL), 0x2U);
    while ((mmio_read_32(DRAMC_REG(DRAMC_OFFSET_DRAMSTATISCTRL)) != 0x0U) && (count < 2000000U)) {
        count ++;
    }
}

void ambarella_dramc_enable_statis_ctrl(void)
{
    ambarella_dramc_reset_statis_ctrl();
    mmio_write_32(DRAMC_REG(DRAMC_OFFSET_DRAMSTATISCTRL), 0x1U); //Enable
}

void ambarella_dramc_disable_statis_ctrl(void)
{
    mmio_write_32(DRAMC_REG(DRAMC_OFFSET_DRAMSTATISCTRL), 0x0U); //Disable
}

AMBA_DRAMC_STATIS_s* ambarella_dramc_get_statis_info(void)
{
    uint32_t index;
    uint32_t div = 1U;

    ambarella_dramc_disable_statis_ctrl();
    if (0x6 == (mmio_read_32(DRAMC_REG(DRAMC_OFFSET_DRAMMODE)) & 0x6)) {
        div = 2U;
    }

    for (index = 0U; index < 32U; index++) {
        dram_stats->ClientRequestStatis[index] = mmio_read_32(DRAMC_REG(DRAMC_OFFSET_CLIENTREQUESTSTATIS(index)));
        dram_stats->ClientBurstStatis[index] = mmio_read_32(DRAMC_REG(DRAMC_OFFSET_CLIENTBURSTSTATIS(index)));
        dram_stats->ClientMaskWriteStatis[index] = mmio_read_32(DRAMC_REG(DRAMC_OFFSET_CLIENTMASKWRITESTATIS(index))) / div;
    }

    return dram_stats;
}

// dramc_write()/dramc_read() are ATF path for accessing DRAMC regs
void ambarella_dramc_write(uint32_t offset, uint32_t val)
{
    mmio_write_32(DRAMC_REG(offset), val);
}

uint32_t ambarella_dramc_read(uint32_t offset)
{
    return mmio_read_32(DRAMC_REG(offset));
}

// ddrc_write()/ddrc_read() are ATF path for accessing DDRC regs
void ambarella_ddrc_write(uint32_t ddrc, uint32_t offset, uint32_t val)
{
    mmio_write_32(DDRCH_REG(ddrc, offset), val);
}

uint32_t ambarella_ddrc_read(uint32_t ddrc, uint32_t offset)
{
    return mmio_read_32(DDRCH_REG(ddrc, offset));
}

#endif

