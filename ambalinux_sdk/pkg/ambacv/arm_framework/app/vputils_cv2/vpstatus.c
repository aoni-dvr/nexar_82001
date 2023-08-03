/**
 *  @file vpstatus.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaCV VP status APIs
 *
 */

#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <malloc.h>
#endif

#include "os_wrap.h"

#define REG_ENTRY_TABLES
#include "reg_tables.h"

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#define VP_NUM 2
#endif
#if defined(CONFIG_BUILD_CV_THREADX)
#define ASM_NOP __asm__ __volatile__ ("nop")
#endif
#include "orcvp_dma_vmem_bin.h"

void print_summary(const reg_agent_t *ra, uint32_t vp,    uint32_t id);
void print_reg(const reg_agent_t *ra, const char *reg_name, uint32_t id);
void dump_vp_regs (const reg_agent_t *ra, const char *file_name);

typedef enum {
    STAT_VOID,
    STAT_SUMMARY,
    STAT_QUERY,
    STAT_DUMP
} STAT_ACTION;

typedef struct {
    uint8_t     vpinst;
    STAT_ACTION action;
    uint8_t     is_vp;
    char        reg_name[32];
    char        debug_file[256];
} cli_args_t;

#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
static void usage(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t-v arg[0|1]: vp instance [vp0|vp1]\n");
    printf("\t-s arg[0|1]: summary of  [vorc|vp]\n");
    printf("\t-n arg: name of the register to read\n");
    printf("\t-d arg: file name of the debug registers\n");
}


#define OPTIONS "v:s:n:d:h"

static int parse_cli(int argc, char** argv, cli_args_t *cli_arg)
{
    int c;

    if(argc < 2) {
        usage(argv[0]);
        return 1;
    }

    cli_arg->vpinst    = 0;
    cli_arg->action   = STAT_VOID;

    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'v':
            cli_arg->vpinst = !!strtoul(optarg, NULL, 0);
            break;

        case 's':
            cli_arg->action = STAT_SUMMARY;
            cli_arg->is_vp   = strtoul(optarg, NULL, 0);
            break;

        case 'n':
            cli_arg->action = STAT_QUERY;
            cvtask_strcpy(cli_arg->reg_name, optarg);
            break;

        case 'd':
            cli_arg->action = STAT_DUMP;
            cvtask_strcpy(cli_arg->debug_file, optarg);
            break;

        case 'h':
            usage(argv[0]);
            return 2;

        default:
            usage(argv[0]);
            break;
        }
    }

    return 0;

}
#else /* ThreadX build */

static int32_t parse_cli(int32_t argc, char** argv, cli_args_t *cli_arg, AMBA_SHELL_PRINT_f PrintFunc)
{
    char** argv_new = argv;
    int32_t ret = 0;
    uint32_t val = 0U;
    const void *dest;

    cli_arg->vpinst    = 0U;
    cli_arg->action   = STAT_VOID;

    if (argc < 3) {
        PrintFunc("usage: t vputils [OPTIONS]\n");
        PrintFunc("\t-v arg[0|1]: vp instance [vp0|vp1]\n");
        PrintFunc("\t-s arg[0|1]: summary of  [vorc|vp]\n");
        PrintFunc("\t-n arg: name of the register to read\n");
        PrintFunc("\t-d arg: file name of the debug registers\n");
        ret = 1;
    }

    if(ret == 0) {
        if (0 == cvtask_strcmp(argv_new[1], "-v")) {
            (void)AmbaUtility_StringToUInt32(argv_new[2], &val);
            cli_arg->vpinst = (uint8_t)val;
            if ( cli_arg->vpinst >= (uint8_t)VP_NUM ) {
                ret = 1;
            }
        }

        if(ret == 0) {
            if (argc >= 5) {
                argv_new = &argv_new[2];
            }
            if (0 == cvtask_strcmp(argv_new[1], "-s")) {
                cli_arg->action = STAT_SUMMARY;
                (void)AmbaUtility_StringToUInt32(argv_new[2], &val);
                cli_arg->is_vp = (uint8_t)val;
            }
            if (argc >= 5) {
                argv_new = &argv_new[2];
            }
            if (0 == cvtask_strcmp(argv_new[1], "-n")) {
                cli_arg->action = STAT_QUERY;
                dest = cvtask_strcpy(cli_arg->reg_name, argv_new[2]);
                if(dest == NULL) {
                    PrintFunc("parse_cli(): cvtask_strcpy reg_name fail\n");
                }
            }
            if (argc >= 5) {
                argv_new = &argv_new[2];
            }
            if (0 == cvtask_strcmp(argv_new[1], "-d")) {
                cli_arg->action = STAT_DUMP;
                dest = cvtask_strcpy(cli_arg->debug_file, argv_new[2]);
                if(dest == NULL) {
                    PrintFunc("parse_cli(): cvtask_strcpy debug_file fail\n");
                }
            }
        }
    }

    return ret;
}
#endif

#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(CONFIG_QNX) && !defined(CONFIG_LINUX)
int main(int argc, char **argv)
{
    cli_args_t cli;
    reg_agent_t ra;

    if ( parse_cli(argc, argv, &cli) || cli.action == STAT_VOID)
        return 1;

    system("modprobe -r ambacv");
    system("modprobe ambacv enable_arm_cache=0");

    open_reg_agent(&ra );

    switch(cli.action) {

    case STAT_SUMMARY:
        print_summary(&ra, cli.is_vp, cli.vpinst);
        break;

    case STAT_QUERY:
        print_reg (&ra, cli.reg_name, cli.vpinst);
        break;

    case STAT_DUMP:
        dump_vp_regs(&ra, cli.debug_file);
        break;

    default:
        usage(argv[0]);
        break;
    }

    close_reg_agent(&ra );

    system("modprobe -r ambacv");
    system("modprobe ambacv");

    return 0;
}
#else /* ThreadX build */
INT32 VPutilsTestEntry(INT32 argc, char **argv, AMBA_SHELL_PRINT_f PrintFunc);
INT32 VPutilsTestEntry(INT32 argc, char **argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    static cli_args_t cli;
    reg_agent_t ra = {0};
    INT32 ret = 0;
    char strbuf[256];

    ThreadX_PrintFunc = PrintFunc;

    if ((parse_cli(argc, argv, &cli, PrintFunc) != 0) || (cli.action == STAT_VOID)) {
        ret = 1;
    } else {
        open_reg_agent(&ra);

        switch(cli.action) {

        case STAT_SUMMARY:
            print_summary(&ra, cli.is_vp, cli.vpinst);
            break;

        case STAT_QUERY:
            print_reg (&ra, cli.reg_name, cli.vpinst);
            break;

        case STAT_DUMP:
            dump_vp_regs(&ra, cli.debug_file);
            break;

        default:
            (void) cvtask_snprintf_str1(strbuf, sizeof(strbuf), "%s : cli.action not support",
                                        __func__);
            ThreadX_PrintFunc(strbuf);
            break;
        }

        close_reg_agent(&ra);
    }

    return ret;
}
#endif

static uint32_t read_bits(const reg_agent_t *p, uint32_t addr, uint32_t bpos, uint32_t bsize)
{
    uint32_t m;
    uint32_t v;
    uint32_t bsize_new = bsize;

    if((bpos + bsize_new) > 32U) {
        uint32_t t;
        uint32_t r = 32U-bpos;

        t = readl (p, addr);
        v = t >> bpos;

        bsize_new -= r;
        m = ((uint32_t)1U<< bsize_new) - 1U;

        t = readl (p, addr+1U);
        v |= (t & m) << r;

    } else {
        m = ((uint32_t)1U << bsize_new) - 1U;
        v = readl (p, addr);
        v = (v>>bpos) & m;
    }

    return v;
}

static void next_bit_pos (uint32_t *wa, uint32_t *bpos, int32_t offset)
{
    int32_t b, b_wa;
    b = (int32_t)*bpos + offset;
    b_wa = b / 32;
    if (b < 0) {
        *wa = *wa + (uint32_t)b_wa - 1U;
        *bpos = (uint32_t)b & 0x1fU;
    } else {
        *wa = *wa + (uint32_t)b_wa;
        *bpos = (uint32_t)b & 0x1fU;
    }
}


static void output_one_reg(const reg_agent_t *ra, reg_entry_t const *r, uint32_t id)
{
    uint32_t base;
    uint32_t value;
    int32_t i, j;
    int32_t w, h, s;
    uint32_t p, a;
    int32_t pitch;
    uint32_t row_addr;
    uint32_t row_bpos;
#if defined(CONFIG_BUILD_CV_THREADX)
    char strbuf[256];
#endif

    if(id == 0U) {
        base = 0U;
    } else {
        base = 0x100000U;
    }

    switch(r->type) {
    case REG_SCALAR:
        value = readl(ra, r->addr + base);
#if !defined(CONFIG_BUILD_CV_THREADX)
        printf("%-32s = 0x%x\n", r->name, value);
#else
        (void) cvtask_snprintf_str1(strbuf, (uint32_t)sizeof(strbuf), "%s = 0x", r->name);
        AmbaUtility_StringAppendUInt32(strbuf, (uint32_t)sizeof(strbuf), value, 16U);
        AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "\n");
        ThreadX_PrintFunc(strbuf);
#endif
        break;

    case REG_ARRAY_1D:
        w = r->width;
        s = r->size_in_bits;
        p = r->offset_in_bits;
        a = r->addr;

        if(w < 0) {
            w = -w;
            s = -s;
        }

        for(i = 0; i < w; i++) {
            value = read_bits(ra, a, p, (uint32_t)r->size_in_bits);
#if !defined(CONFIG_BUILD_CV_THREADX)
            printf("\t%s[%d] = 0x%x\n", r->name, i, value);
#else
            (void) cvtask_snprintf_str1(strbuf, (uint32_t)sizeof(strbuf), "\t%s[", r->name);
            AmbaUtility_StringAppendUInt32(strbuf, (uint32_t)sizeof(strbuf), (uint32_t)i, 10U);
            AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "] = 0x");
            AmbaUtility_StringAppendUInt32(strbuf, (uint32_t)sizeof(strbuf), value, 16U);
            AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "\n");
            ThreadX_PrintFunc(strbuf);
#endif
            next_bit_pos(&a, &p, s);
        }

        break;

    case REG_ARRAY_2D:
        w = r->width;
        h = r->height;
        s = r->size_in_bits;
        pitch = r->pitch;


        if(w < 0) {
            w = -w;
            s = -s;
        }

        if(h < 0) {
            h = -h;
            pitch = -pitch;
        }

        row_addr = r->addr;
        row_bpos = r->offset_in_bits;
        for(j = 0; j < h; j++) {
            for(i = 0; i < w; i++) {
                a = row_addr;
                p = row_bpos;

                value = read_bits(ra, a, p, (uint32_t)r->size_in_bits);
#if !defined(CONFIG_BUILD_CV_THREADX)
                printf("\t\t%s[%d][%d] = 0x%x\n", r->name, j, i, value);
#else
                (void) cvtask_snprintf_str1(strbuf, (uint32_t)sizeof(strbuf), "\t\t%s[", r->name);
                AmbaUtility_StringAppendUInt32(strbuf, (uint32_t)sizeof(strbuf), (uint32_t)j, 10U);
                AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "][");
                AmbaUtility_StringAppendUInt32(strbuf, (uint32_t)sizeof(strbuf), (uint32_t)i, 10U);
                AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "] = 0x");
                AmbaUtility_StringAppendUInt32(strbuf, (uint32_t)sizeof(strbuf), value, 16U);
                AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "\n");
                ThreadX_PrintFunc(strbuf);
#endif
                next_bit_pos(&a, &p, s);
            }
            next_bit_pos(&row_addr,  &row_bpos, pitch);
        }
        break;
    default:
#if defined(CONFIG_BUILD_CV_THREADX)
        (void) cvtask_snprintf_str1(strbuf, (uint32_t)sizeof(strbuf), "%s : r->type not support",
                                    __func__);
        ThreadX_PrintFunc(strbuf);
#endif
        break;
    }
}


void print_summary(const reg_agent_t *ra, uint32_t vp, uint32_t id)
{
    reg_entry_t const *r;
    uint32_t     tbl_size,i, new_id = id;
    const char *name;
#if defined(CONFIG_BUILD_CV_THREADX)
    char strbuf[256];
#endif

    if(vp != 0U) {
        r = &vp_regs[0];
        tbl_size =  (uint32_t)(sizeof(vp_regs)/sizeof(vp_regs[0]));
        if(id == 0U) {
            name = "VP0";
        } else {
            name = "VP1";
        }
    } else {
        r = &orc_regs[0];
        tbl_size = (uint32_t)(sizeof(orc_regs)/sizeof(orc_regs[0]));
        name = "VORC";
        new_id = 0U;
    }
#if !defined(CONFIG_BUILD_CV_THREADX)
    printf("\n\n -------------------------     Summy of %-5s  Status"
           "     -------------------------\n\n",
           name);
#else
    (void) cvtask_snprintf_str1(strbuf, (uint32_t)sizeof(strbuf), "\n\n -------------------------     Summy of %-5s  Status"
                                "     -------------------------\n\n",
                                name);
    ThreadX_PrintFunc(strbuf);
#endif

    for (i = 0U; i < tbl_size; i++) {
        output_one_reg(ra, r, new_id);
        r = &r[1];
    }
}


void print_reg(const reg_agent_t *ra, const char *reg_name, uint32_t id)
{
    const reg_entry_t *tbl[2];
    uint32_t     tbl_size[2];
    uint32_t     found = 0U,i,j;
#if defined(CONFIG_BUILD_CV_THREADX)
    char strbuf[256];
#endif

    tbl[0] = &vp_regs[0];
    tbl[1] = &orc_regs[0];

    tbl_size[0] = (uint32_t)(sizeof(vp_regs)/sizeof(vp_regs[0]));
    tbl_size[1] = (uint32_t)(sizeof(orc_regs)/sizeof(orc_regs[0]));
    for (j = 0U; j < 2U ; j++) {
        if(found == 0U) {
            reg_entry_t const  *r = tbl[j];
            for (i = 0U; i < tbl_size[j]; i++) {
                if( cvtask_strcmp(reg_name, &r->name[0]) == 0) {
                    output_one_reg(ra, r, id);
                    found = 1U;
                    break;
                }
                r = &r[1];
            }
        }
    }

    if(found == 0U) {
#if !defined(CONFIG_BUILD_CV_THREADX)
        printf("cannot find register %s\n", reg_name);
#else
        const void *dest;

        (void) cvtask_snprintf_str1(strbuf, (uint32_t)sizeof(strbuf), "cannot find register %s\n", reg_name);
        ThreadX_PrintFunc(strbuf);
        // For coverity misra-c
        dest = cvtask_memcpy(strbuf, orcvp_dma_vmem, 1U);
        if(dest != NULL) {
            ThreadX_PrintFunc(strbuf);
        }
#endif
    }
}

#define  VMEM_SIZE        0x200000U
#define  DBGREG_SIZE      0x50000U

void dump_vp_regs(const reg_agent_t *ra, const char* file_name)
{
    typedef struct {
        uint32_t   addr;
        uint32_t   size;
        uint32_t   org_size;
    }
    region_t;
    CVTASK_FILE *dbg_file;
    uint32_t cycles;
#if defined(CONFIG_BUILD_CV_THREADX)
    static uint8_t reg_dbase[DBGREG_SIZE] GNU_SECTION_NOZEROINIT;
#else
    static uint8_t reg_dbase[DBGREG_SIZE];
#endif
    uint8_t *dbase = reg_dbase;
    uint32_t i;
    region_t const *r;
    uint8_t *dram_vbase;
    uint32_t t;
    uint32_t dump_dbase = ra->cv_dram_base;
    uint32_t vmem_dbase = dump_dbase;
    uint32_t vpexe_dbase = vmem_dbase + VMEM_SIZE;
#if defined(CONFIG_BUILD_CV_THREADX)
    char strbuf[256];
#endif
    const void *dest;

    static const region_t regions[] = {
        {0xed800000U,  0x17000U, 0x17000U}, //vp0_dbgreg_dp.bin
        {0xed820000U,  0x2c000U, 0x2c000U}, //vp0_dbgreg_main.bin
        {0xed84c000U,  0x6000U, 0x6000U },  //vp0_dbgreg_vmemif.bin
        {0xed860000U,  0xfa0U, 0xf98U  },   //vp0_dbgreg_xmem.bin
        {0xed861000U,  0x2400U, 0x2400U },  //vp0_dbgreg_dcb.bin
    };

    dest = cvtask_memset(reg_dbase, 0, sizeof(reg_dbase));
    if(dest == NULL) {
#if !defined(CONFIG_BUILD_CV_THREADX)
        printf("dump_vp_regs(): cvtask_memset fail\n");
#else
        ThreadX_PrintFunc("dump_vp_regs(): cvtask_memset fail\n");
#endif
    }
    //static const uint32_t block_size = 0x1000;
    r = &regions[0];

    //XMEM chicken bits, is it still necessary ????
    //writel 0xed860008 0x2f
    //writel(ra, 0xed860008, 0x2f);

    //latch
    writel(ra, 0xed82006cU, 0x200000U);
    //throttle DCB
    writel(ra, 0xed860008U, 0x3fU);

    cycles = readl(ra, 0xed8200a0U);
#if !defined(CONFIG_BUILD_CV_THREADX)
    printf("cycles = 0x%x\n", cycles);
#else
    (void) cvtask_snprintf_str1(strbuf, (uint32_t)sizeof(strbuf), "[%s]", __func__);
    AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "cycles = 0x");
    AmbaUtility_StringAppendUInt32(strbuf, (uint32_t)sizeof(strbuf), cycles, 16);
    AmbaUtility_StringAppend(strbuf, (uint32_t)sizeof(strbuf), "\n");
    ThreadX_PrintFunc(strbuf);
#endif

    for(i = 0U; i < (sizeof(regions) / sizeof(*regions)); i++) {
        //printf("reading debug register addr=0x%x size=0x%x vaddr=0x%llx\n", r->addr, r->size, dbase);
        read_buf(ra, r->addr, r->size, dbase);
        dbase = &dbase[r->size];
        r = &r[1];
    }

    dram_vbase = cv_dram_c2v(ra, vpexe_dbase );

    //read vmem using ORC

    //1. read vp.exe/vp.bin and load into a non-cachable dram adress vpexe_dbase
    dest = cvtask_memcpy (dram_vbase, orcvp_dma_vmem, sizeof(orcvp_dma_vmem));
    if(dest == NULL) {
#if !defined(CONFIG_BUILD_CV_THREADX)
        printf("dump_vp_regs(): cvtask_memcpy fail\n");
#else
        ThreadX_PrintFunc("dump_vp_regs(): cvtask_memcpy fail\n");
#endif
    }
    (void)cv_cache_clean(ra, dram_vbase, sizeof(orcvp_dma_vmem));

    //2. suspend all ORCVP threads
    writel(ra, 0xed010000U, 0x1f1fU);

    t = readl (ra, 0xed010030U);
    if((t & 0x10U) != 0U) {
#if !defined(CONFIG_BUILD_CV_THREADX)
        printf("VP hang detected!\n");
#else
        ThreadX_PrintFunc("VP hang detected!\n");
#endif

        //reset VP/VMEM???
        writel(ra, 0xed080228U, 0x00000000U );
        writel(ra, 0xed080228U, (uint32_t)0x3U << 9U );
        writel(ra, 0xed080228U, 0x00000000U );

    }

    //4. program the starting PC to match with vp.exe, then program the offset in L2.
    //printf("vpexe dbase=0x%x\n", VPEXE_DBASE);
    writel(ra, 0xed010008U, vpexe_dbase);
    writel(ra, 0xed0a0048U, 0U); /* Explicitly disable VISORC remap registers */
    writel(ra, 0xed0a0058U, 0U); /* Explicitly disable VISORC remap registers */
    writel(ra, 0xed0a0068U, 0U); /* Explicitly disable VISORC remap registers */
    writel(ra, 0xed0a0078U, 0U); /* Explicitly disable VISORC remap registers */

    writel(ra, 0xed018000U, 0xc0ffeeU);
    writel(ra, 0xed018004U, vmem_dbase);
    writel(ra, 0xed018008U, 0U);


    writel(ra, 0xed010004U, 0x3U);
    writel(ra, 0xed0a0084U, 0x0U);


    //5. kick off orcvp to linear DMA the vmem into dram address vmem_dbase
    writel(ra, 0xed010000U, 0x1fU);
    writel(ra, 0xed010030U, 0U);   /* Explicitly start VISORC on Thread 0 */

    //6. wait for orccode to finish
    do {
        t = readl(ra, 0xed018008U);
        for(i = 0U; i < 16U; i++) {
#if !defined(CONFIG_BUILD_CV_THREADX)
            asm("nop");
#else
            ASM_NOP;
#endif
        }
    } while (t != 0xcafefaceU);

    writel(ra, 0xed010000U, 0xf00U);

    writel(ra, 0xed05f010U, 0x1020U);
    writel(ra, 0xed05f010U, 0x0U);


    dbg_file = cvtask_fopen(file_name, "wb");

    if(dbg_file == NULL) {
#if !defined(CONFIG_BUILD_CV_THREADX)
        printf("cannot open file %s for writing!\n", file_name);
        exit(-1);
#else
        (void) cvtask_snprintf_str1(strbuf, sizeof(strbuf), "cannot open file %s for writing!\n", file_name);
        ThreadX_PrintFunc(strbuf);
#endif
    } else {
        dbase = (uint8_t *)cv_dram_c2v(ra,  vmem_dbase);
        (void) cv_cache_invalidate(ra, dbase, VMEM_SIZE);
        (void) cvtask_fwrite(dbase, sizeof(uint8_t), VMEM_SIZE, dbg_file);

        r = &regions[0];
        dbase = reg_dbase;

        for(i = 0U; i < (sizeof(regions) / sizeof(*regions)); i++) {
            (void) cvtask_fwrite(dbase, sizeof(uint8_t), r->org_size, dbg_file);
            dbase = &dbase[r->size];
            r = &r[1];
        }

        (void) cvtask_fclose(dbg_file);

        writel(ra, 0xed860008U, 0U);
    }
}

