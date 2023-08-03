/*******************************************************************************
 * flexidag_cvcehu.c
 *
 * Copyright (c) 2018-2019 Ambarella, Inc.
 *
 * This file and its contents ( "Software" ) are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>


#define NO_ARG	  0
#define HAS_ARG	 1

#define AMBA_NUM_CEHU_NUM      (2U)
#define AMBA_NUM_CEHU_ERRORS  (155U)

#define CEHU_CV_ERROR                       0x4000001CU
#define CEHU_REGISTER_SIZE                  0x00001000U
#define CEHU_REGISTER_BASE                  0xE0025000U
#define CV_SAFETY_REGISTER_SIZE             0x01000000U
#define CV_SAFETY_REGISTER_BASE             0xED000000U
#define CV_ARITH1_SAFETY_REGISTER           0xED800FFCU
#define CV_INTERP_SAFETY_REGISTER           0xED801FFCU
#define CV_INTEG_SAFETY_REGISTER            0xED802FFCU
#define CV_MINMAX_SAFETY_REGISTER           0xED803FFCU
#define CV_GEN_SAFETY_REGISTER              0xED804FFCU
#define CV_COMPARE_SAFETY_REGISTER          0xED805FFCU
#define CV_LOGIC1_SAFETY_REGISTER           0xED806FFCU
#define CV_COUNT_SAFETY_REGISTER            0xED807FFCU
#define CV_WARP_SAFETY_REGISTER             0xED808FFCU
#define CV_SEGMENTS_SAFETY_REGISTER         0xED8097F0U
#define CV_STATISTICS_SAFETY_REGISTER       0xED80AFFCU
#define CV_COPY_SAFETY_REGISTER             0xED80BFFCU
#define CV_TRANS1_SAFETY_REGISTER           0xED80CFFCU
#define CV_TRANS2_SAFETY_REGISTER           0xED80DFFCU
#define CV_RESAMP_SAFETY_REGISTER           0xED80EFFCU
#define CV_ICE1_SAFETY_REGISTER             0xED80FFFCU
#define CV_ICE2_SAFETY_REGISTER             0xED810FFCU
#define CV_VPMAIN_SAFETY_REGISTER           0xED820FFCU
#define CV_XMEM_SAFETY_REGISTER             0xED860FFCU
#define CV_VMEM_SAFETY_REGISTER             0xED13FFF4U
#define CV_OL2C_SAFETY_REGISTER             0xED0A00C8U

#define CVCEHU_ERR_00001                    0x00000001U


typedef struct {
    unsigned int sm_failure:1;
    unsigned int sm_rsvd:4;
    unsigned int sm_masked_failure:1;
    unsigned int sm_failure_point:10;
    unsigned int sm_ecc_count:8;
    unsigned int reset_cycles_m1:5;
    unsigned int reset_rsvd:3;
} CV_VP_SAFETY_REGISTER_s;

typedef struct {
    unsigned int ID:        8;  /* [7:0] Error ID */
    unsigned int Reserved: 24; /* [31:8] Reserved */
} AMBA_CEHU_ERROR_ID_s;

typedef struct {
    unsigned int CheckDone:       1;  /* [0]  Safety Check Done 0: Not yet, 1: Done*/
    unsigned int Reserved:       31;  /* [31:1] Reserved */
} AMBA_CEHU_SAFETY_CHECK_s;

typedef struct {
    volatile unsigned int                   ModeArray[10];             /* 0x00-0x24(RW): Safety Mode 0-9 */
    volatile unsigned int                   ErrorInjectionArray[5];    /* 0x28-0x38(RW): Error Injection 0-4 */
    volatile unsigned int                   ErrorBitMaskArray[5];      /* 0x3C-0x4C(RW): Error Bit Mask 0-4 */
    volatile unsigned int                   ErrorBitPolarityArray[5];  /* 0x50-0x60(RW): Error Bit Polarity 0-4 */
    volatile unsigned int                   ErrorBitVectorArray[5];    /* 0x64-0x74(RW1C): Error Bit Vector 0-4 */
    volatile AMBA_CEHU_ERROR_ID_s           ErrorID;                   /* 0x78(RW): Error ID */
    volatile AMBA_CEHU_SAFETY_CHECK_s       SafetyCheck;               /* 0x7C(RW): Safety Check */
} AMBA_CEHU_REG_s;

typedef struct {
    int amba_fd;
    unsigned int *pSafety_Reg;
    AMBA_CEHU_REG_s *pAmbaCEHU_Reg[AMBA_NUM_CEHU_NUM];
} CV_CEHU_REG_s;

static int cv_cehu_disable  = 0;
static int cv_cehu_enable = 0;
static int cv_cehu_clear  = 0;
static int cv_cehu_show  = 0;
static CV_CEHU_REG_s cv_cehu_reg;

static struct option long_options[] = {
    {"disable", NO_ARG, 0, 'd'},
    {"enable", NO_ARG, 0, 'e'},
    {"clear", NO_ARG, 0, 'c'},
    {"show", NO_ARG, 0, 's'},
    {0, 0, 0, 0},
};

static const char *short_options = "decs";

struct hint_s {
    const char *arg;
    const char *str;
};

static const struct hint_s hint[] = {
    {"",          "\t\tDisable cv cehu."},
    {"",          "\t\tEnable cv cehu."},
    {"",          "\t\tClear cv cehu error."},
    {"",          "\t\tShow cv cehu error status."},
};

void usage(void)
{
    int i;

    printf("flexidag_cvcehu usage:\n");
    for (i = 0; i < sizeof(long_options) / sizeof(long_options[0]) - 1; i++) {
        if (isalpha(long_options[i].val))
            printf("-%c ", long_options[i].val);
        else
            printf("   ");
        printf("--%s", long_options[i].name);
        if (hint[i].arg[0] != 0)
            printf(" [%s]", hint[i].arg);
        printf("\t%s\n", hint[i].str);
    } /* for (i = 0; i < sizeof(long_options) / sizeof(long_options[0]) - 1; i++) */
} /* usage() */

int init_param(int argc, char **argv)
{
    int ch;
    int option_index = 0;
    int value;

    opterr = 0;
    while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (ch) {
        case 'd':
            cv_cehu_disable = 1;
            break;

        case 'e':
            cv_cehu_enable = 1;
            break;

        case 'c':
            cv_cehu_clear = 1;
            break;

        case 's':
            cv_cehu_show = 1;
            break;

        default:
            printf("unknown option found: %c\n", ch);
            return -1;
            break;
        } /* switch (ch) */
    } /* while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) */

    return 0;

} /* init_param() */

unsigned int open_reg_agent(void)
{
    unsigned int ret = 0;
    unsigned int i;

    cv_cehu_reg.amba_fd = open("/dev/ppm", O_RDWR | O_SYNC);
    if (cv_cehu_reg.amba_fd < 0) {
        printf("Can't open device file /dev/ppm !!!\n");
        ret = CVCEHU_ERR_00001;
    } else {
        cv_cehu_reg.pSafety_Reg = mmap(NULL, CV_SAFETY_REGISTER_SIZE, PROT_READ | PROT_WRITE,
                                       MAP_SHARED, cv_cehu_reg.amba_fd, CV_SAFETY_REGISTER_BASE);
        if (cv_cehu_reg.pSafety_Reg == MAP_FAILED) {
            printf("Can't mmap cv safety register !!!\n");
            ret = CVCEHU_ERR_00001;
        }

        for (i = 0U; i<AMBA_NUM_CEHU_NUM; i++) {
            cv_cehu_reg.pAmbaCEHU_Reg[i] = mmap(NULL, CEHU_REGISTER_SIZE, PROT_READ | PROT_WRITE,
                                                MAP_SHARED, cv_cehu_reg.amba_fd, (CEHU_REGISTER_BASE + i*CEHU_REGISTER_SIZE));
            if (cv_cehu_reg.pAmbaCEHU_Reg == MAP_FAILED) {
                printf("Can't mmap cv cehu register[%d] !!!\n", i);
                ret = CVCEHU_ERR_00001;
            }
        }
    }

    return ret;
}

void close_reg_agent(void)
{
    unsigned int i;

    munmap(cv_cehu_reg.pSafety_Reg, CV_SAFETY_REGISTER_SIZE);
    for (i = 0U; i<AMBA_NUM_CEHU_NUM; i++) {
        munmap(cv_cehu_reg.pAmbaCEHU_Reg[i], CEHU_REGISTER_SIZE);
    }

    close( cv_cehu_reg.amba_fd);
}

unsigned int safety_readl(unsigned int addr)
{
    unsigned int addr_align;

    if ((addr >= CV_SAFETY_REGISTER_BASE) && (addr <= (CV_SAFETY_REGISTER_BASE + CV_SAFETY_REGISTER_SIZE))) {
        addr_align = addr - CV_SAFETY_REGISTER_BASE;
        addr_align = addr_align >> 2U;

        return cv_cehu_reg.pSafety_Reg[addr_align];
    } else {
        printf("safety_readl() : unknown addr=0x%x !!!\n", addr);
        return 0xFFFFFFFF;
    }
}

unsigned int safety_writel(unsigned int addr, unsigned int value)
{
    unsigned int ret = 0;
    unsigned int addr_align;

    if ((addr >= CV_SAFETY_REGISTER_BASE) && (addr <= (CV_SAFETY_REGISTER_BASE + CV_SAFETY_REGISTER_SIZE))) {
        addr_align = addr - CV_SAFETY_REGISTER_BASE;
        addr_align = addr_align >> 2U;

        cv_cehu_reg.pSafety_Reg[addr_align] = value;
    } else {
        printf("safety_writel() : unknown addr=0x%x !!!\n", addr);
        ret = CVCEHU_ERR_00001;
    }

    return ret;
}

static void cehu_print_safety_register(unsigned int Addr)
{
    unsigned int reg_value;
    static CV_VP_SAFETY_REGISTER_s reg;

    reg_value = safety_readl(Addr);
    if (reg_value != 0U) {
        memcpy(&reg, &reg_value, sizeof(void *));
        printf("         0x%x :\n", Addr);
        printf("                      Failure signal : 0x%x\n", reg.sm_failure);
        printf("                      Failure Masked : 0x%x\n", reg.sm_masked_failure);
        printf("                      Failure Point  : 0x%x\n", reg.sm_failure_point);
        printf("                      Ecc Count      : %d\n", reg.sm_ecc_count);
        printf("                      Reset Cycles   : %d\n", (reg.reset_cycles_m1 + 1U));
    }
}

static unsigned int cehu_get_error(unsigned int InstanceID, unsigned int ErrorID, unsigned int *Value)
{
    unsigned int ret = 0;

    if ((InstanceID < AMBA_NUM_CEHU_NUM) && (ErrorID < AMBA_NUM_CEHU_ERRORS) && (Value != NULL)) {
        unsigned int group = ErrorID / 32U;
        unsigned int idx   = ErrorID % 32U;
        unsigned int error_vector;

        error_vector = cv_cehu_reg.pAmbaCEHU_Reg[InstanceID]->ErrorBitVectorArray[group];

        if ((error_vector & (0x1UL << idx)) == 0U) {
            *Value = 0;
        } else {
            *Value = 1;
        }
    } else {
        ret = CVCEHU_ERR_00001;
    }
    return ret;
}


static unsigned int cehu_clear_error(unsigned int InstanceID, unsigned int ErrorID)
{
    unsigned int ret = 0;

    if ((InstanceID < AMBA_NUM_CEHU_NUM) && (ErrorID < AMBA_NUM_CEHU_ERRORS)) {
        unsigned int group_id = ErrorID / 32U;
        unsigned int error_vector;

        error_vector = cv_cehu_reg.pAmbaCEHU_Reg[InstanceID]->ErrorBitVectorArray[group_id];
        cv_cehu_reg.pAmbaCEHU_Reg[InstanceID]->ErrorBitVectorArray[group_id] = error_vector;
    } else {
        ret = CVCEHU_ERR_00001;
    }
    return ret;
}

static unsigned int cehu_disable(unsigned int InstanceID, unsigned int ErrorID)
{
    unsigned int ret = 0;

    if ((InstanceID < AMBA_NUM_CEHU_NUM) && (ErrorID < AMBA_NUM_CEHU_ERRORS)) {
        unsigned int group = ErrorID / 32U;
        unsigned int idx   = ErrorID % 32U;
        unsigned int mask;

        mask = cv_cehu_reg.pAmbaCEHU_Reg[InstanceID]->ErrorBitMaskArray[group];
        mask = mask | (1UL << idx);

        cv_cehu_reg.pAmbaCEHU_Reg[InstanceID]->ErrorBitMaskArray[group] = mask;
    } else {
        ret = CVCEHU_ERR_00001;
    }
    return ret;
}

static unsigned int cehu_enable(unsigned int InstanceID, unsigned int ErrorID)
{
    unsigned int ret = 0;

    if ((InstanceID < AMBA_NUM_CEHU_NUM) && (ErrorID < AMBA_NUM_CEHU_ERRORS)) {
        unsigned int group = ErrorID / 32U;
        unsigned int idx   = ErrorID % 32U;
        unsigned int mask;

        mask = cv_cehu_reg.pAmbaCEHU_Reg[InstanceID]->ErrorBitMaskArray[group];
        mask = mask & (~(1UL << idx));

        cv_cehu_reg.pAmbaCEHU_Reg[InstanceID]->ErrorBitMaskArray[group] = mask;
    } else {
        ret = CVCEHU_ERR_00001;
    }
    return ret;
}

static unsigned int cvcehu_show_status(void)
{
    unsigned int error_status;
    unsigned int i, j;
    unsigned int group, idx;
    unsigned int CehuUnMask[8U] = {0};
    unsigned int ret = 0U;

    CehuUnMask[0] = CEHU_CV_ERROR;
    for (i = 0U; i<AMBA_NUM_CEHU_NUM; i++) {
        for (j = 0U; j < AMBA_NUM_CEHU_ERRORS; j++) {
            group = j / 32U;
            idx = j % 32U;
            if (((CehuUnMask[group] >> idx) & 0x1U) == 1U) {
                ret = cehu_get_error(i, j, &error_status);
                if (ret != 0U) {
                    printf("cvcehu_show_status() : cehu_get_error(%d, %d) fail(%d)\n", i, j, ret);
                } else {
                    if (error_status != 0U) {
                        printf("cvcehu_show_status() : cehu get error!! Instance=%d ErrorID=%3d\n", i, j, 0U, 0U, 0U);
                        if (j == 2U) {
                            printf("cvcehu_show_status() : cehu get vp error!!\n");
                            cehu_print_safety_register(CV_ARITH1_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_INTERP_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_INTEG_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_MINMAX_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_GEN_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_COMPARE_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_LOGIC1_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_COUNT_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_WARP_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_SEGMENTS_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_STATISTICS_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_COPY_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_TRANS1_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_TRANS2_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_RESAMP_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_ICE1_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_ICE2_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_VPMAIN_SAFETY_REGISTER);
                            cehu_print_safety_register(CV_XMEM_SAFETY_REGISTER);
                        }
                        if (j == 3U) {
                            printf("cvcehu_show_status() : cehu get vmem error!!\n");
                        }
                        if (j == 4U) {
                            printf("cvcehu_show_status() : cehu get vorc error!!\n");
                        }
                        if (j == 30U) {
                            printf("cvcehu_show_status() : cehu get ol2c error!!\n");
                        }
                        ret = CVCEHU_ERR_00001;
                    }
                }
            }
        }
    }

    return ret;
}

static unsigned int cvcehu_clear_error(void)
{
    unsigned int i, j;
    unsigned int group, idx;
    unsigned int CehuUnMask[8U] = {0};
    unsigned int ret = 0U;

    safety_writel(CV_ARITH1_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_INTERP_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_INTEG_SAFETY_REGISTER,        0x00000011U);
    safety_writel(CV_MINMAX_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_GEN_SAFETY_REGISTER,          0x00000011U);
    safety_writel(CV_COMPARE_SAFETY_REGISTER,      0x00000011U);
    safety_writel(CV_LOGIC1_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_COUNT_SAFETY_REGISTER,        0x00000011U);
    safety_writel(CV_WARP_SAFETY_REGISTER,         0x00000011U);
    safety_writel(CV_SEGMENTS_SAFETY_REGISTER,     0x00000011U);
    safety_writel(CV_STATISTICS_SAFETY_REGISTER,   0x00000011U);
    safety_writel(CV_COPY_SAFETY_REGISTER,         0x00000011U);
    safety_writel(CV_TRANS1_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_TRANS2_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_RESAMP_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_ICE1_SAFETY_REGISTER,         0x00000011U);
    safety_writel(CV_ICE2_SAFETY_REGISTER,         0x00000011U);
    safety_writel(CV_VPMAIN_SAFETY_REGISTER,       0x00000011U);
    safety_writel(CV_XMEM_SAFETY_REGISTER,         0x00000011U);
    safety_writel(CV_VMEM_SAFETY_REGISTER,         0x00000001U);
    safety_writel(CV_OL2C_SAFETY_REGISTER,         0x00000000U);

    CehuUnMask[0] = CEHU_CV_ERROR;
    for (i = 0U; i<AMBA_NUM_CEHU_NUM; i++) {
        for (j = 0U; j < AMBA_NUM_CEHU_ERRORS; j++) {
            group = j / 32U;
            idx = j % 32U;
            if (((CehuUnMask[group] >> idx) & 0x1U) == 1U) {
                ret = cehu_clear_error(i, j);
                if (ret != 0U) {
                    printf("cvcehu_clear_error() : cehu_clear_error(%d, %d) fail(%d)\n", i, j, ret);
                }
            }
        }
    }

    return ret;
}

static unsigned int cvcehu_disable(void)
{
    unsigned int i, j;
    unsigned int group, idx;
    unsigned int CehuUnMask[8U] = {0};
    unsigned int ret = 0U;

    CehuUnMask[0] = CEHU_CV_ERROR;
    for (i = 0U; i<AMBA_NUM_CEHU_NUM; i++) {
        for (j = 0U; j < AMBA_NUM_CEHU_ERRORS; j++) {
            group = j / 32U;
            idx = j % 32U;
            if (((CehuUnMask[group] >> idx) & 0x1U) == 1U) {
                ret = cehu_disable(i, j);
                if (ret != 0U) {
                    printf("cvcehu_disable() : cehu_disable(%d, %d) fail(%d)\n", i, j, ret);
                }
            }
        }
    }

    return ret;
}

static unsigned int cvcehu_enable(void)
{
    unsigned int i, j;
    unsigned int group, idx;
    unsigned int CehuUnMask[8U] = {0};
    unsigned int ret = 0U;

    CehuUnMask[0] = CEHU_CV_ERROR;
    for (i = 0U; i<AMBA_NUM_CEHU_NUM; i++) {
        for (j = 0U; j < AMBA_NUM_CEHU_ERRORS; j++) {
            group = j / 32U;
            idx = j % 32U;
            if (((CehuUnMask[group] >> idx) & 0x1U) == 1U) {
                ret = cehu_enable(i, j);
                if (ret != 0U) {
                    printf("cvcehu_enable() : cehu_enable(%d, %d) fail(%d)\n", i, j, ret);
                }
            }
        }
    }

    return ret;
}

unsigned int main(int argc, char **argv)
{
    unsigned int ret = 0U;

    if (argc < 2) {
        usage();
    } else if (init_param(argc, argv) < 0) {
        usage();
    } else {
        ret = (unsigned int)open_reg_agent();
        if (ret == 0) {
            if (cv_cehu_clear == 1) {
                ret = cvcehu_clear_error();
            }

            if (ret == 0) {
                if (cv_cehu_disable == 1) {
                    ret = cvcehu_disable();
                } else if (cv_cehu_enable == 1) {
                    ret = cvcehu_enable();
                }
            }

            if (ret == 0) {
                if (cv_cehu_show == 1) {
                    ret = cvcehu_show_status();
                }
            }
            close_reg_agent();
        }
    }

    return ret;
}

