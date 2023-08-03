/**
 *  @file AmbaDramAdjust.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Dram Shmoo diagnostic functions
 *
 */
#include "AmbaTypes.h"
#include "AmbaCortexA76.h"
#include "AmbaCSL_DDRC.h"
typedef UINT32      u32;
typedef UINT64      u64;
typedef UINT32      u32_t;
typedef UINT64      u64_t;
#include "uartp.h"

//#define DRAM_ADJUST_DEBUG

#define DDRC_BASE                       AMBA_CORTEX_A76_DRAM_CONFIG_VIRT_BASE_ADDR
#define DDRC_REG(x)                     (DDRC_BASE + (x))
#define mmio_write_32(addr,value)       (*(volatile UINT32*)addr = value)
#define mmio_read_32(addr)              (*(volatile UINT32*)addr)
#define get_ddr_freq_hz                 AmbaRTSL_PllGetDramClk
#define mdelay(x)                       rct_timer_delay(24000 * (x))

#define DRAMC_OFFSET                    0x0000
#define DDRCB_OFFSET                    0x4000
#define DDRC0_OFFSET                    0x5000
#define DDRC1_OFFSET                    0x6000
#define DDRCH_REG(x, y)                 (DDRC_BASE + DDRC0_OFFSET + (x) * 0x1000 + (y))

#define DRAM_CONTRL                     0x00000000
#define DRAM_CONFIG1                    0x00000004
#define DRAM_CONFIG2                    0x00000008
#define DRAM_TIMING1                    0x0000000c
#define DRAM_TIMING2                    0x00000010
#define DRAM_TIMING3                    0x00000014
#define DRAM_TIMING4                    0x00000018
#define DRAM_TIMING5                    0x0000001c
#define DRAM_DUAL_DIE_TIMING            0x00000020
#define DRAM_REFRESH_TIMING             0x00000024
#define DRAM_LP5_TIMING                 0x00000028
#define DRAM_INIT_CTL                   0x0000002c
#define DRAM_MODE_REG                   0x00000030
#define DRAM_SELF_REFRESH               0x00000034
#define DRAM_RSVD_SPACE                 0x00000038
#define DRAM_BYTE_MAP                   0x0000003c
#define DRAM_MPC_WDATA                  0x00000040
#define DRAM_MPC_WMASK                  0x00000060
#define DRAM_MPC_RDATA                  0x00000064
#define DRAM_MPC_RMASK                  0x000000a4
#define DRAM_UINST1                     0x000000ac
#define DRAM_UINST2                     0x000000b0
#define DRAM_UINST3                     0x000000b4
#define DRAM_UINST4                     0x000000b8
#define DRAM_UINST5                     0x000000bc
#define DRAM_UINST6                     0x000000c0
#define DRAM_SCRATCHPAD                 0x000000c4
#define DRAM_WDQS_TIMING                0x000000c8
#define DRAM_CLEAR_MPC_DATA             0x000000cc
#define DRAM_IO_CONTROL                 0x000000d0
#define DRAM_DDRC_MISC_1                0x000000d4
#define DRAM_DTTE_CONFIG                0x00000100
#define DRAM_DTTE_DELAY_REG(n)          (0x00000108 + ((n) * 4))
#define DRAM_DTTE_CMD_REG               0x00000130
#define DRAM_DTTE_DELAY_MAP             0x00000134
#define DRAM_DTTE_CP_INFO               0x00000138
#define DTTE_ALGO_ERROR                 0x00000148
#define DRAM_WRITE_VREF_0               0x00000164
#define DRAM_WRITE_VREF_1               0x00000168
#define DRAM_DTTE_TIMING                0x0000016c
#define DDRIO_DLL_CTRL_SBC(b)           (0x00000200 + (b) * 4)
#define DDRIO_DLL_CTRL_SEL(s, d)        (0x00000210 + (s) * 8 + (d) * 4)
#define DDRIO_BYTE_DLY0(b, d)           (0x00000228 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY1(b, d)           (0x0000022c + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY2(b, d)           (0x00000230 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY3(b, d)           (0x00000234 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY4(b, d)           (0x00000238 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY5(b, d)           (0x0000023c + (b) * 24 + (d) * 96)
#define DDRIO_CK_DELAY                  0x000002e8
#define DDRIO_CA_DELAY_COARSE           0x000002ec
#define DDRIO_CA_DELAY_FINE_0(c, d)     (0x000002f0 + (c) * 16 + (d) * 8)
#define DDRIO_CA_DELAY_FINE_1(c, d)     (0x000002f4 + (c) * 16 + (d) * 8)
#define DDRIO_CS_DELAY                  0x00000310
#define DDRIO_CKE_DELAY_COARSE          0x00000314
#define DDRIO_CKE_DELAY_FINE            0x00000318
#define DDRIO_DQS_TESTIRCV              0x0000035c
#define DDRIO_PAD_CTRL                  0x00000360
#define DDRIO_DQS_PUPD                  0x00000364
#define DDRIO_ZCTRL                     0x0000036c
#define DDRIO_CA_PADCTRL                0x00000370
#define DDRIO_DQ_PADCTRL                0x00000374
#define DDRIO_VREF_0                    0x00000378
#define DDRIO_VREF_1                    0x0000037c
#define DDRIO_IBIAS_CTRL                0x00000380
#define DDRIO_ZCTRL_STATUS              0x00000384
#define DDRIO_DLL_STATUS_0              0x00000388
#define DDRIO_DLL_STATUS_1              0x0000038c
#define DRAM_DDRC_STATUS                0x000003a4
#define DRAM_DDRC_MISC_2                0x000003c0
#define DRAM_READ_LINK_ECC_STATUS(b)    (0x000003c4 + (b) * 4)
#define DRAM_DDRC_MR46                  0x000003d4

/* DRAM_CTL register bit */
#define DRAM_CONTROL_AR_CNT_RELOAD      0x00000004
#define DRAM_CONTROL_AUTO_REF_EN        0x00000002
#define DRAM_CONTROL_ENABLE             0x00000001

/* DRAM_IO_CONTROL register bit */
#define DRAM_IO_CONTROL_CKE(d, c)       (1 << ((d) * 2 + (c)))
#define DRAM_IO_CONTROL_CKE_ALL         0x0000000f
#define DRAM_IO_CONTROL_DISABLE_RESET   0x00000010
#define DRAM_IO_CONTROL_DISABLE_CMD     0x00000020

/* DRAM_INIT_CTL register bit */
#define DRAM_INIT_CTL_RTT_DIE           0x00000400
#define DRAM_INIT_CTL_PAD_CLB_LONG_EN   0x00000200
#define DRAM_INIT_CTL_PAD_CLB_SHORT_EN  0x00000100
#define DRAM_INIT_CTL_DLL_RST_EN        0x00000008
#define DRAM_INIT_CTL_GET_RTT_EN        0x00000004

/* DDRIO_ZCTRL register bit */
#define DDRIO_ZCTRL_PAD_CLB_LONG        0x00000080
#define DDRIO_ZCTRL_PAD_CLB_SHORT       0x00000100
#define DDRIO_ZCTRL_PAD_RESET           0x00000200

/* DDRIO_ZCTRL_STATUS register bit */
#define DDRIO_ZCTRL_STATUS_ACK          0x00002000

static INT32 ddrcnum;
static INT32 ranknum;
static INT32 wck2dqi_last[2][2][2];
//static INT32 fine_fs[2][2][2];

extern void rct_timer_delay(u32 delay_cnt);

static UINT32 ddrc_mrr(UINT32 ddrc, UINT32 addr, UINT32 did, UINT32 cid)
{
    UINT32 data, temp;

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

static void ddrc_mrw(UINT32 ddrc, UINT32 addr, UINT32 data, UINT32 did, UINT32 cid)
{
    UINT32 temp;

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

static void ddrc_uinst(UINT32 ddrc, UINT32 uinst1, UINT32 uinst2, UINT32 uinst4)
{
    mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST1), uinst1);
    mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST2), uinst2);
    mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST4), uinst4);

    mmio_write_32(DDRCH_REG(ddrc, DRAM_UINST5), 0x1);
    while (mmio_read_32(DDRCH_REG(ddrc, DRAM_UINST5)) & 0x1);
}

static void bst_uart_wait(void)
{
    static int executed = 0;
    if (executed == 0) {
        rct_timer_delay(0x300000);  // Wait extra 130ms for UART Init done to avoid message disappearred
        executed = 1;
    }
}

INT32 AmbaDramAdjust(void)
{
    int c, d, ch;
    INT32 *wck2dqi_trained = (INT32 *) (DDRCT_RESULT_WCK2DQX_START);
    INT32 j = 0;
    INT32 wck2dqi[2][2][2];
    INT32 wck2dqi_diff, /*mr43,*/ step;
    UINT32 data_mr35, data_mr36, count;
    UINT64 freq;
    UINT32 dly0_val[2][2][4], dly1_val[2][2][4], dly2_val[2][2][4], dly3_val[2][2][4];
    INT32 fine[2][2][4][11], b, i, adjust_wck;
    INT32 fine_fs_pos = 3200, fine_fs_neg = 2500;
    //UINT32 Frequency;
    volatile UINT32 *pPllCtrlReg = (UINT32 *) 0x20ed180000UL;
    volatile UINT32 *pPllCtrl2Reg = (UINT32 *) (0x20ed180000UL + 8UL);
    UINT32 Sdiv, Sout, IntProg, Fsdiv, Fsout;
    UINT32 ClkDiv2 = 0;
    UINT32 Reg2Bit12 = 0;

    if (mmio_read_32(DDRCT_RESULT_WCK2DQX_VALID) != DDRCT_RESULT_VALID_MAGIC_CODE)
        return -1;

    if ((mmio_read_32(DDRC_REG(0)) & 0x06) == 0x06)
        ddrcnum = 2;
    else
        ddrcnum = 1;

    if (mmio_read_32(DDRCH_REG(0, DRAM_CONFIG1)) & (1 << 26))
        ranknum = 2;
    else
        ranknum = 1;


    freq = 24 / 4 / 2; //AmbaRTSL_PllGetClkRefFreq();
    Sdiv = (pPllCtrlReg[0U] >> 12U) & 0xfU;
    Sout = (pPllCtrlReg[0U] >> 16U) & 0xfU;
    IntProg = (pPllCtrlReg[0U] >> 24U) & 0x7fU;
    Fsdiv = (pPllCtrl2Reg[0] >> 9U) & 0x1U;
    Fsout= (pPllCtrl2Reg[0] >> 11U) & 0x1U;
    ClkDiv2 = (pPllCtrl2Reg[0] >> 8U) & 0x1U;
    Reg2Bit12 = (pPllCtrl2Reg[0] >> 12U) & 0x1U;

    if ( 0x0U != Reg2Bit12 ) {
        freq = freq * (ClkDiv2 + 1U) * (Fsdiv + 1U) * (Sdiv + 1U) * (IntProg + 1U);
    } else {
        freq = freq * (Fsdiv + 1U) * (Sdiv + 1U) * (IntProg + 1U);
        freq = freq / (Fsout + 1U) / (Sout + 1U);
    }
    /* fs(femtosecond): fs_wck2dqx = 8192 * 1000000000 / f / count_wck2dqx / 2 */
    //freq = Frequency / 4; /* CK rather than WCK */
//    freq = freq / 1000000;

    for (c = 0; c < ddrcnum; c++) {
        ddrc_mrw(c, 37, 0xc0, 0x3, 0x3);
        ddrc_uinst(c, 0xd, 0x170, 0x303);
        mdelay(5);
        for (d = 0; d < ranknum; d++) {
            data_mr35 = ddrc_mrr(c, 35, d, 0x3);
            data_mr36 = ddrc_mrr(c, 36, d, 0x3);
            /* Channel A */
            count = ((data_mr35 >> 0) & 0xff) | (((data_mr36 >> 0) & 0xff) << 8);
            wck2dqi[c][d][0] = 1000000000ULL * 8192 / freq / count / 2;

            /* Channel B */
            count = ((data_mr35 >> 8) & 0xff) | (((data_mr36 >> 8) & 0xff) << 8);
            wck2dqi[c][d][1] = 1000000000ULL * 8192 / freq / count / 2;

            for (ch = 0; ch < 2; ch++) {
                //wck2dqi_last[c][d][ch] = wck2dqi_fs[c][d][ch];
                j = (c*4) + (d*2) + ch;
                if (wck2dqi_trained[j] != 0) {
                    wck2dqi_last[c][d][ch] = wck2dqi_trained[j];
                } else {
                    continue;   // Go next if the save wck2dqi is 0 (some kind of invalid)
                }

#ifdef DRAM_ADJUST_DEBUG
                {
                    bst_uart_wait();
                	bst_uart_putstr("wck2dqi=");
                	bst_uart_puthex(wck2dqi[c][d][ch], 8);
                	bst_uart_putstr(",last=");
                	bst_uart_puthex(wck2dqi_last[c][d][ch], 8);
                	bst_uart_putstr("\n");
                }
#endif

                wck2dqi_diff = wck2dqi[c][d][ch] - wck2dqi_last[c][d][ch];
                if (wck2dqi_diff > 0 && wck2dqi_diff > fine_fs_pos)
                    step = (wck2dqi_diff + fine_fs_pos / 2) / fine_fs_pos;
                else if (wck2dqi_diff < 0 && wck2dqi_diff < -fine_fs_neg)
                    step = (wck2dqi_diff - fine_fs_neg / 2) / fine_fs_neg;
                else
                    step = 0;

                if (step != 0) {
                    //wck2dqi_last[c][d][ch] = wck2dqi[c][d][ch];

                    //ddrc_adjust_wdq(0x1 << c, 0x1 << d, 0x3 << (ch << 1), step);
                    for (b = (ch << 1); b < (ch << 1)+2; b++) {

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

                        adjust_wck = 0;
                        for (i = 0; i < 10; i++) {
                            fine[c][d][b][i] += step;
                            if (fine[c][d][b][i] < 0 || fine[c][d][b][i] > 31) {
                                fine[c][d][b][i] = fine[c][d][b][i] < 0 ? 0 : 31;
                                adjust_wck = 1;
                            }
                        }

                        if (adjust_wck) {
                            fine[c][d][b][10] -= step; /* reverse direction of WCK */
                            if (fine[c][d][b][10] < 0 || fine[c][d][b][10] > 31) {
                            	bst_uart_wait();
                                /*
                            	bst_uart_putchar('C');
                            	bst_uart_puthex(c, 1);
                            	bst_uart_putchar('D');
                            	bst_uart_puthex(d, 1);
                            	bst_uart_putchar('B');
                            	bst_uart_puthex(b, 1);
                            	*/
                            	bst_uart_putstr("WCK:");
                            	bst_uart_puthex(fine[c][d][b][10], 8);
                            	bst_uart_putstr("!\n");
                                break;
                            }

                            dly0_val[c][d][b] &= ~(0x1f << 0);
                            dly0_val[c][d][b] |= (fine[c][d][b][10] << 0);
                            mmio_write_32(DDRCH_REG(c, DDRIO_BYTE_DLY0(b, d)), dly0_val[c][d][b]);
                            /* Save back to ScratchPad */
                            wck2dqi_trained[j] = wck2dqi[c][d][ch];
                            continue;
                        }
                        /* Save back to ScratchPad */
                        wck2dqi_trained[j] = wck2dqi[c][d][ch];

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
        }
    }

    /* Perform ZQ calibration */
    for (c = 0; c < ddrcnum; c++)
        ddrc_uinst(c, 0xd, 0x670, 0x303);
    return 0;
}
