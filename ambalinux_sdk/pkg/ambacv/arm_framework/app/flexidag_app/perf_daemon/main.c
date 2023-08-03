/*******************************************************************************
 * main.c
 *
 * History:
 *    2019-05-28 - [Clifton Kayano] - Created initial version
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
#include <cvapi_flexidag.h>
#include <cvapi_cavalry.h>
#include <schdr_api.h>
#include <schdr_util.h>
#include <sys/ioctl.h>
#include <cvtask_ossrv.h>
#include <cvapi_logger_interface.h>
#include <cvapi_ambacv_flexidag.h>
#include <errno.h>

#define FILENAME_LENGTH     (256)

static char root_path[FILENAME_LENGTH];

static int  num_visperf_dump  = 1;
static char visperf_hdr_filename[SYSTEM_MAX_NUM_VISORC][FILENAME_LENGTH];
static char visperf_pld_filename[SYSTEM_MAX_NUM_VISORC][FILENAME_LENGTH];
FILE        *visperf_hdr_fd[SYSTEM_MAX_NUM_VISORC];
FILE        *visperf_pld_fd[SYSTEM_MAX_NUM_VISORC];
uint32_t    visperf_size[SYSTEM_MAX_NUM_VISORC];
uint32_t    visperf_rd_idx[SYSTEM_MAX_NUM_VISORC];


static int  num_armperf_dump  = 1;
static char armperf_hdr_filename[SYSTEM_MAX_NUM_CORTEX][FILENAME_LENGTH];
static char armperf_pld_filename[SYSTEM_MAX_NUM_CORTEX][FILENAME_LENGTH];
FILE        *armperf_hdr_fd[SYSTEM_MAX_NUM_CORTEX];
FILE        *armperf_pld_fd[SYSTEM_MAX_NUM_CORTEX];
uint32_t    armperf_size[SYSTEM_MAX_NUM_CORTEX];
uint32_t    armperf_rd_idx[SYSTEM_MAX_NUM_CORTEX];

static int  num_dspperf_dump  = 1;
static char dsp_perf_hdr_filename[FILENAME_LENGTH];
static char dsp_perf_pld_filename[FILENAME_LENGTH];
FILE        *dsp_perf_hdr_fd;
FILE        *dsp_perf_pld_fd;
uint32_t    dsp_perf_size;
uint32_t    dsp_perf_rd_idx;

uint8_t     *pFlexidag_Info[FLEXIDAG_MAX_SLOTS];
uint32_t    flexidag_info_size[FLEXIDAG_MAX_SLOTS];
uint32_t    flexidag_num_enabled = 0;
uint32_t    flexidag_num_active = 0;
uint32_t    cavalry_num_active = 0;
uint32_t    autorun_num_avail = 0;

uint32_t    sample_time = 250000;
uint32_t    debug_log = 0;

uint32_t    mix_arm_vis_perf = 1U;
uint32_t    mix_dsp_vis_perf = 1U;

cavalry_trace_t  *pCavalryTrace = NULL;
cavalry_trace_t  Local_CavTrace;
autorun_trace_t  *pAutoRunTrace = NULL;
autorun_trace_t  *pLocal_AutoRunTrace = NULL;
flexidag_trace_t *pFlexidagTrace = NULL;
struct tm start_timeinfo;

uint32_t schdr_start_flag = 0;

#define NO_ARG    0
#define HAS_ARG  1

extern uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size);

static struct option long_options[] = {
    {"visperf", HAS_ARG, 0, 'v'},
    {"armperf", HAS_ARG, 0, 'a'},
    {"output path", HAS_ARG, 0, 'p'},
    {"sample time (ms)", HAS_ARG, 0, 's'},
    {"debug log", HAS_ARG, 0, 'd'},
};

static const char *short_options = "v:a:p:s:d:";

struct hint_s {
    const char *arg;
    const char *str;
};

static const struct hint_s hint[] = {
    {"number",  "\t number visorc perf log to."},
    {"number",  "\t number arm perf log to."},
    {"output path",  "\t output path of logfile "},
    {"number",  "\t sample time (ms) "},
    {"number",  "\t debug log "},
};

void usage(void)
{
    int i;

    printf("pdc_daemon usage:\n");
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
        case 'p':
            value = strlen(optarg);
            if (value >= FILENAME_LENGTH) {
                printf("init_param() : Filename [%s] is too long [%d] (>%d); exiting \n", optarg, value, FILENAME_LENGTH);
                return -1;
            } /* if (value >= FILENAME_LENGTH) */

            memset(&root_path[0], 0x0, sizeof(root_path));
            strncpy(&root_path[0], optarg, value);
            break;

        case 'v':
            num_visperf_dump = atoi(optarg);
            break;
        case 'a':
            num_armperf_dump = atoi(optarg);
            break;
        case 's':
            sample_time = (atoi(optarg) * 1000);
            break;
        case 'd':
            debug_log = atoi(optarg);
            break;
        default:
            printf("unknown option found: %c\n", ch);
            return -1;
            break;
        } /* switch (ch) */
    } /* while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) */

    return 0;
} /* init_param() */

#define AMBADSP_GET_BUF_INFO                _IOWR ('s', 0x0AU, dsp_buf_info_t)

typedef struct {
    uint64_t  Addr;
    uint32_t Size;
    uint32_t IsCached;
} dsp_mem_t;

typedef struct {
    uint32_t Type;
    dsp_mem_t Buf;
} dsp_buf_info_t;

typedef struct {
    uint64_t  LogBufAddr;          /*!< [Valid address] base address of log buffer */
    uint32_t LogNum;              /*!< [Non zero] Maximum number of log entry.  */
    uint32_t WP;                  /*!< [LogNum-1] write pointer */
    uint32_t RP;                  /*!< [LogNum-1] read pointer */
} AMBA_DSP_PERFLOG_BUFF_CTRL_s;

AMBA_DSP_PERFLOG_BUFF_CTRL_s *dsp_perf_ctrl = NULL;
void *dsp_perf_ptr  = NULL;

void dsp_stream_init(void) 
{
    static int32_t dsp_fd = -1;
    dsp_buf_info_t DspBufInfo = {0};
    uint32_t Rval = 0;
    uint32_t addr_align, offset,size_align;
    void *ptr;

    if(dsp_fd < 0) {
        dsp_fd = open("/dev/ambadsp", O_RDWR);
    }

    if(dsp_fd >= 0) {
        if(dsp_perf_ptr == NULL) {
            DspBufInfo.Type = 6;
            Rval = ioctl(dsp_fd, AMBADSP_GET_BUF_INFO, &DspBufInfo);
            addr_align = (DspBufInfo.Buf.Addr & 0xFFFFF000U);
            offset = DspBufInfo.Buf.Addr - addr_align;
            ptr = (AMBA_DSP_PERFLOG_BUFF_CTRL_s *) mmap(NULL, 0x1000U, PROT_READ | PROT_WRITE, MAP_SHARED, dsp_fd, addr_align);
            if(ptr == MAP_FAILED) {
                printf("[Info]dsp_perf_ctrl map fail errno %d Addr 0x%lx size 0x%x addr_align 0x%x offset 0x%x \n",errno,DspBufInfo.Buf.Addr,DspBufInfo.Buf.Size,addr_align,offset);
                Rval = 1;
            } else {
                dsp_perf_ctrl = ptr + offset;
                addr_align = (dsp_perf_ctrl->LogBufAddr & 0xFFFFF000U);
                offset = dsp_perf_ctrl->LogBufAddr - addr_align;
                size_align = (((dsp_perf_ctrl->LogNum*sizeof(cvlog_perf_entry_t)) + offset + 0xFFF) & 0xFFFFF000U);
                ptr = mmap(NULL, size_align, PROT_READ | PROT_WRITE, MAP_SHARED, dsp_fd, addr_align);
                if( ptr == MAP_FAILED) {
                    printf("[Info]dsp_perf_ptr map fail errno %d Addr 0x%lx size 0x%x addr_align 0x%x offset 0x%x size_align 0x%x \n",errno,dsp_perf_ctrl->LogBufAddr,dsp_perf_ctrl->LogNum*sizeof(cvlog_perf_entry_t),addr_align,offset,size_align);
                    Rval = 1;
                } else {
                    dsp_perf_ptr = ptr + offset;
                }
            }
        }
    } else {
        printf("[Info] /dev/ambadsp not found errno %d \n",errno);
        Rval = 1;
    }

    if(Rval == 0) {
        num_dspperf_dump = 1;
        printf("[Info] dsp_perf_ctrl(0x%x) dsp_perf_daddr 0x%x  dsp_perf_size 0x%x dsp_perf_rd_idx 0x%x \n",DspBufInfo.Buf.Addr,dsp_perf_ctrl->LogBufAddr, dsp_perf_ctrl->LogNum*sizeof(cvlog_perf_entry_t),dsp_perf_rd_idx);
    } else {
        num_dspperf_dump = 0;
    }
}


uint32_t perf_stream_init(uint32_t error_enter)
{
    uint32_t  error_exit = error_enter;
    uint32_t  loop = 0;
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    memcpy(&start_timeinfo,timeinfo, sizeof(struct tm));

    /* Start scheduler */
    if(schdr_start_flag == 0) {
        error_exit = flexidag_schdr_start();
        schdr_start_flag = 1;
    }
    if(error_exit == 0) {
        pFlexidagTrace = schdr_get_pFlexidagTrace();
        if (pFlexidagTrace != NULL) {
            flexidag_num_enabled  = pFlexidagTrace->num_slots_enabled;
        } else {
            printf(" perf_stream_init() : schdr_get_pFlexidagTrace fail  \n");
            error_exit = 1;
        }
    } else {
        printf(" perf_stream_init() : flexidag_schdr_start fail ret 0x%x \n",error_exit);
    }

    /* Start DSP*/
    dsp_stream_init();

    if(error_exit == 0) {
        for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) {
            pFlexidag_Info[loop]      = NULL;
            flexidag_info_size[loop]  = 0;
        }

        flexidag_num_active = 0;
        cavalry_num_active = 0;
        autorun_num_avail = 0;

        pCavalryTrace = NULL;
        pAutoRunTrace = NULL;
        pLocal_AutoRunTrace = NULL;

        for(loop = 0; loop < num_visperf_dump; loop++) {
            if(error_exit == 0) {
                snprintf(&visperf_hdr_filename[loop][0], sizeof(visperf_hdr_filename[loop]), "%s/visperf[%02d]_%02d-%02d-%02d.prf",root_path,loop,start_timeinfo.tm_hour,start_timeinfo.tm_min,start_timeinfo.tm_sec);
                visperf_hdr_fd[loop] = fopen(visperf_hdr_filename[loop], "wb");
                if (visperf_hdr_fd[loop] == NULL) {
                    printf("[ERROR] : VISPERF[%d] : dump requested, but could not create file %s\n",loop, visperf_hdr_filename[loop]);
                    error_exit = 1;
                }
            }

            if(error_exit == 0) {
                snprintf(&visperf_pld_filename[loop][0], sizeof(visperf_pld_filename[loop]), "%s/visperf[%02d]_pld_%02d-%02d-%02d.prf",root_path,loop,start_timeinfo.tm_hour,start_timeinfo.tm_min,start_timeinfo.tm_sec);
                visperf_pld_fd[loop] = fopen(visperf_pld_filename[loop], "wb+");
                if (visperf_pld_fd[loop] == NULL) {
                    printf("[ERROR] : VISPERF[%d] : dump requested, but could not create file %s\n",loop, visperf_pld_filename[loop]);
                    error_exit = 1;
                }
            }
        }

        if(mix_arm_vis_perf == 0U) {
            for(loop = 0; loop < num_armperf_dump; loop++) {
                if(error_exit == 0) {
                    snprintf(&armperf_hdr_filename[loop][0], sizeof(armperf_hdr_filename[loop]), "%s/armperf[%02d]_%02d-%02d-%02d.prf",root_path,loop,start_timeinfo.tm_hour,start_timeinfo.tm_min,start_timeinfo.tm_sec);
                    armperf_hdr_fd[loop] = fopen(armperf_hdr_filename[loop], "wb");
                    if (armperf_hdr_fd[loop] == NULL) {
                        printf("[ERROR] : ARMPERF[%d] : dump requested, but could not create file %s\n",loop, armperf_hdr_filename[loop]);
                        error_exit = 1;
                    }
                }

                if(error_exit == 0) {
                    snprintf(&armperf_pld_filename[loop][0], sizeof(armperf_pld_filename[loop]), "%s/armperf[%02d]_pld_%02d-%02d-%02d.prf",root_path,loop,start_timeinfo.tm_hour,start_timeinfo.tm_min,start_timeinfo.tm_sec);
                    armperf_pld_fd[loop] = fopen(armperf_pld_filename[loop], "wb+");
                    if (armperf_pld_fd[loop] == NULL) {
                        printf("[ERROR] : ARMPERF[%d] : dump requested, but could not create file %s\n",loop, armperf_pld_filename[loop]);
                        error_exit = 1;
                    }
                }

            }
        }

        if(mix_dsp_vis_perf == 0U) {
            if(error_exit == 0) {
                snprintf(&dsp_perf_hdr_filename[0], sizeof(dsp_perf_hdr_filename), "%s/dsp_perf_%02d-%02d-%02d.prf",root_path,start_timeinfo.tm_hour,start_timeinfo.tm_min,start_timeinfo.tm_sec);
                dsp_perf_hdr_fd = fopen(dsp_perf_hdr_filename, "wb");
                if (dsp_perf_hdr_fd == NULL) {
                    printf("[ERROR] : DSP PERF : dump requested, but could not create file %s\n", dsp_perf_hdr_filename);
                    error_exit = 1;
                }
            }

            if(error_exit == 0) {
                snprintf(&dsp_perf_pld_filename[0], sizeof(dsp_perf_pld_filename), "%s/dsp_perf_pld_%02d-%02d-%02d.prf",root_path,start_timeinfo.tm_hour,start_timeinfo.tm_min,start_timeinfo.tm_sec);
                dsp_perf_pld_fd = fopen(dsp_perf_pld_filename, "wb+");
                if (dsp_perf_pld_fd == NULL) {
                    printf("[ERROR] : DSP PERF : dump requested, but could not create file %s\n", dsp_perf_pld_filename);
                    error_exit = 1;
                }
            }
        }
    }

    return error_exit;
}

void perf_stream_cleanup(void)
{
    uint32_t  loop = 0;

    /* Cleanup */
    for (loop = 0; loop < flexidag_num_enabled; loop++) {
        if (pFlexidag_Info[loop] != NULL) {
            free(pFlexidag_Info[loop]);
            pFlexidag_Info[loop] = NULL;
        } /* if (pFlexidag_Info[loop] != NULL) */
    } /* for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) */

    if (pLocal_AutoRunTrace != NULL) {
        free(pLocal_AutoRunTrace);
        pLocal_AutoRunTrace = NULL;
    }

    for(loop = 0; loop < num_visperf_dump; loop++) {
        if (visperf_hdr_fd[loop] != NULL) {
            fclose(visperf_hdr_fd[loop]);
        }

        if (visperf_pld_fd[loop] != NULL) {
            fclose(visperf_pld_fd[loop]);
            remove(visperf_pld_filename[loop]);
        }
    }

    if(mix_arm_vis_perf == 0U) {
        for(loop = 0; loop < num_armperf_dump; loop++) {
            if (armperf_hdr_fd[loop] != NULL) {
                fclose(armperf_hdr_fd[loop]);
            }

            if (armperf_pld_fd[loop] != NULL) {
                fclose(armperf_pld_fd[loop]);
                remove(armperf_pld_filename[loop]);
            }
        }
    }

    if(mix_dsp_vis_perf == 0U) {
        if (dsp_perf_hdr_fd != NULL) {
            fclose(dsp_perf_hdr_fd);
        }

        if (dsp_perf_pld_fd != NULL) {
            fclose(dsp_perf_pld_fd);
            remove(dsp_perf_pld_filename);
        }
    }

}

uint32_t perf_update_flexidag_info(uint32_t error_enter)
{
    uint32_t  error_exit = error_enter;
    uint32_t  loop = 0;

    if(error_exit == 0) {
        ambacv_cache_invalidate(pFlexidagTrace,sizeof(flexidag_trace_t));
        /* Retrieve flexidag information */
        while ((loop < flexidag_num_enabled) && (error_exit == 0)) {
            uint32_t  slot_state;
            uint32_t  num_sysflow_entries;
            uint32_t  flexidag_sysflow_daddr;
            uint32_t  flexidag_name_daddr;

            slot_state              = pFlexidagTrace->slottrace[loop].slot_state;
            num_sysflow_entries     = pFlexidagTrace->slottrace[loop].sysflow_name_table_numlines;
            flexidag_sysflow_daddr  = pFlexidagTrace->slottrace[loop].sysflow_name_table_daddr;
            flexidag_name_daddr     = pFlexidagTrace->slottrace[loop].flexidag_name_daddr;
            if ((error_exit == 0) && (slot_state == 100) && (flexidag_info_size[loop] == 0)) {
                uint32_t  allocsize;
                sysflow_name_entry_t *pFlexidagSysflowName;
                uint8_t  *pFlexidagName;
                uint8_t   flexidag_name_len;

                if (flexidag_sysflow_daddr != 0) {
                    pFlexidagSysflowName = ambacv_c2v(flexidag_sysflow_daddr);
                    if (pFlexidagSysflowName == NULL) {
                        printf("[ERROR] : perf_update_flexidag_info() : FD[%3d] : Trace could not map sysflow_name_table_daddr (0x%08x)\n", loop, flexidag_sysflow_daddr);
                        error_exit = 1;
                    } else {
                        ambacv_cache_invalidate(pFlexidagSysflowName, sizeof(sysflow_name_entry_t));
                    }
                } /* if (flexidag_sysflow_daddr != 0) */
                else { /* if (flexidag_sysflow_daddr == 0) */
                    printf("[ERROR] : perf_update_flexidag_info() : FD[%3d] : Trace has no sysflow_name_table_daddr\n", loop);
                    error_exit = 1;
                } /* if (flexidag_sysflow_daddr == 0) */

                if (flexidag_name_daddr != 0) {
                    pFlexidagName     = ambacv_c2v(flexidag_name_daddr);
                    if (pFlexidagName != NULL) {
                        ambacv_cache_invalidate(pFlexidagName, FLEXIDAG_NAME_MAX_LENGTH);
                        flexidag_name_len = strlen(pFlexidagName);
                        if (flexidag_name_len > FLEXIDAG_NAME_MAX_LENGTH) {
                            flexidag_name_len = FLEXIDAG_NAME_MAX_LENGTH;
                        } /* if (flexidag_name_len > FLEXIDAG_NAME_MAX_LENGTH) */
                    } else {
                        printf("[WARN!] : perf_update_flexidag_info() : FD[%3d] : Trace could not map flexidag_name_daddr\n", loop);
                        pFlexidagName     = NULL;
                        flexidag_name_len = 0;
                    }
                } /* if (flexidag_name_daddr != 0) */
                else { /* if (flexidag_name_daddr == 0) */
                    printf("[WARN!] : perf_update_flexidag_info() : FD[%3d] : Trace has no flexidag_name entry\n", loop);
                    pFlexidagName     = NULL;
                    flexidag_name_len = 0;
                } /* if (flexidag_name_daddr == 0) */

                allocsize = sizeof(ambaprof_flexidag_header_t) + (num_sysflow_entries * AMBAPROF_MAX_NAME_LEN);
                flexidag_info_size[loop]  = allocsize;
                pFlexidag_Info[loop]      = malloc(allocsize);
                if (pFlexidag_Info[loop] == NULL) {
                    error_exit = 1;
                    printf("[ERROR] : perf_update_flexidag_info() : Could not allocate flexidag_info for slot %d (size = %d)\n", loop, allocsize);
                } /* if (pFlexidag_Info[loop] == NULL) */
                else { /* if (pFlexidag_Info[loop] != NULL) */
                    ambaprof_flexidag_header_t *pPerfFDHeader;
                    uint8_t *pPerfFDSysflowName;
                    uint32_t  sysflow_loop;

                    memset(pFlexidag_Info[loop], 0, allocsize);

                    pPerfFDHeader       = (ambaprof_flexidag_header_t *)pFlexidag_Info[loop];
                    pPerfFDSysflowName  = (uint8_t *)&pPerfFDHeader[1];
                    pPerfFDHeader->flexidag_slot_id             = loop;
                    pPerfFDHeader->flexidag_num_sysflow_entries = num_sysflow_entries;
                    if (pFlexidagName != NULL) {
                        memcpy(&pPerfFDHeader->flexidag_name[0], pFlexidagName, flexidag_name_len);
                    }
                    for (sysflow_loop = 0; sysflow_loop < num_sysflow_entries; sysflow_loop++) {
                        sprintf(&pPerfFDSysflowName[0], "%s (%d)", pFlexidagSysflowName[sysflow_loop].cvtask_name, pFlexidagSysflowName[sysflow_loop].uuid);
                        pPerfFDSysflowName += AMBAPROF_MAX_NAME_LEN;
                    } /* for (sysflow_loop = 0; sysflow_loop < num_sysflow_entries; sysflow_loop++) */
                    flexidag_num_active++;
                    if(debug_log) {
                        printf("[INFO] : perf_update_flexidag_info() :  flexidag[%d] name : %s  info_size %d \n",loop,pFlexidagName,flexidag_info_size[loop]);
                    }
                } /* if (pFlexidag_Info[loop] != NULL) */
            } /* if ((slot_state > 0) && (slot_state < 255)) */
            loop++;
        } /* while ((loop < flexidag_num_enabled) && (error_exit == 0)) */
    }

    return error_exit;
}

uint32_t perf_update_cavalry_info(uint32_t error_enter)
{
    uint32_t  error_exit = error_enter;
    uint32_t  loop = 0;

    if(error_exit == 0) {
        /* Retrieve cavalry info */
        cavalry_num_active  = 0;
        pCavalryTrace   = schdr_get_pCavalryTrace();
        if (pCavalryTrace != NULL) {
            memcpy(&Local_CavTrace, pCavalryTrace, sizeof(cavalry_trace_t));
            for (loop = 0; loop < CAVALRY_TRACE_MAX_PROCESS; loop++) {
                if (Local_CavTrace.cavalry_info[loop].cavalry_pid != CAVALRY_INVALID_PID) {
                    cavalry_num_active++;
                } /* if (pLocal_CavTrace->cavalry_info[loop].cavalry_pid != CAVALRY_INVALID_PID) */
            } /* for (loop = 0; loop < CAVALRY_TRACE_MAX_PROCESS; loop++) */
        } /* if (pCavalryTrace != NULL) */
    }

    return error_exit;
}

uint32_t perf_update_autorun_info(uint32_t error_enter)
{
    uint32_t  error_exit = error_enter;

    if(error_exit == 0) {
        /* Retrieve autorun info */
        autorun_num_avail = 0;
        pAutoRunTrace     = schdr_get_pAutoRunTrace();
        if (pAutoRunTrace != NULL) {
            uint32_t  autorun_size;
            autorun_num_avail = pAutoRunTrace->autorun_cvtask_num;
            autorun_size      = autorun_num_avail * sizeof(autorun_trace_t);
            pLocal_AutoRunTrace = malloc(autorun_size);
            memcpy(pLocal_AutoRunTrace, pAutoRunTrace, autorun_size);
        } /* if (pAutoRunTrace != NULL) */
    }

    return error_exit;
}

uint32_t perf_output_vis_payload(uint32_t error_enter)
{
    uint32_t  loop = 0;
    uint32_t  error_exit = error_enter;

    if(error_exit == 0) {
        while ((loop < num_visperf_dump) && (error_exit == 0)) {
            uint32_t perf_daddr;
            uint32_t perf_size;
            uint32_t perf_output_size;
            uint32_t perf_rd_idx;
            uint32_t perf_wr_idx;
            uint32_t perf_total_idx;

            perf_daddr  = pFlexidagTrace->sysvis_printbuf_perf_daddr[loop];
            perf_size   = pFlexidagTrace->sysvis_printbuf_perf_size[loop];
            perf_rd_idx = visperf_rd_idx[loop];
            perf_total_idx = perf_size/((uint32_t)sizeof(cvlog_perf_flexidag_entry_t));
            error_exit = schdr_get_perf_last_wridx_daddr(loop,&perf_wr_idx);
            perf_wr_idx = perf_wr_idx%perf_total_idx;
            if((error_exit == 0U) && (perf_rd_idx != perf_wr_idx)) {
                if ((perf_daddr != 0) && (perf_size != 0)) {
                    cvlog_perf_flexidag_entry_t *pSrcBuf;
                    pSrcBuf = (cvlog_perf_flexidag_entry_t *)ambacv_c2v(perf_daddr);
                    if (pSrcBuf == NULL) {
                        error_exit = 1;
                        printf("[ERROR] : perf_output_vis_payload() : FD[%3d] : Trace could not map fdvis_printbuf_perf_daddr (0x%08x)\n", loop, perf_daddr);
                    } /* if (pSrcBuf == NULL) */
                    else { /* if (pSrcBuf != NULL) */
                        ambacv_cache_invalidate(pSrcBuf, perf_size);

                        if(perf_rd_idx < perf_wr_idx) {
                            perf_output_size = (perf_wr_idx - perf_rd_idx) * sizeof(cvlog_perf_flexidag_entry_t);
                            fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, visperf_pld_fd[loop]);
                            visperf_size[loop] += perf_output_size;

                            visperf_rd_idx[loop] = perf_wr_idx;
                        } else {
                            perf_output_size = (perf_total_idx - perf_rd_idx) * sizeof(cvlog_perf_flexidag_entry_t);
                            fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, visperf_pld_fd[loop]);
                            visperf_size[loop] += perf_output_size;

                            perf_output_size = (perf_wr_idx) * sizeof(cvlog_perf_flexidag_entry_t);
                            fwrite(&pSrcBuf[0], perf_output_size, 1, visperf_pld_fd[loop]);
                            visperf_size[loop] += perf_output_size;

                            visperf_rd_idx[loop] = perf_wr_idx;
                        }
                        if(debug_log) {
                            printf("[INFO] : perf_output_vis_payload() : visperf[%d] update idx [%d - %d] size 0x%x , totoal idx %d\n",loop,perf_rd_idx, visperf_rd_idx[loop],visperf_size[loop],perf_total_idx);
                        }
                    } /* if (pSrcBuf != NULL) */
                } /* if ((perf_daddr != 0) && (perf_size != 0)) */
                else { /* if ((perf_daddr == 0) || (perf_size == 0)) */
                    printf("[WARN!] : perf_output_vis_payload() : VISPERF[%d] : dump requested, but no perf buffer exists for this core\n", loop);
                } /* if ((perf_daddr == 0) || (perf_size == 0)) */
            }
            loop++;
        } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */
    }
    return error_exit;
}

uint32_t perf_output_arm_payload(uint32_t error_enter)
{
    uint32_t  loop = 0;
    uint32_t  error_exit = error_enter;

    if(error_exit == 0) {
        loop = 0;
        while ((loop < num_armperf_dump) && (error_exit == 0)) {
            uint32_t perf_daddr;
            uint32_t perf_size;
            uint32_t perf_output_size;
            uint32_t perf_rd_idx;
            uint32_t perf_wr_idx;
            uint32_t perf_total_idx;

            perf_daddr  = pFlexidagTrace->sysarm_printbuf_perf_daddr[loop];
            perf_size   = pFlexidagTrace->sysarm_printbuf_perf_size[loop];
            perf_rd_idx = armperf_rd_idx[loop];
            perf_total_idx = perf_size/((uint32_t)sizeof(cvlog_perf_flexidag_entry_t));
            error_exit = schdr_get_arm_perf_last_wridx_daddr(&perf_wr_idx);
            perf_wr_idx = perf_wr_idx%perf_total_idx;
            if((error_exit == 0U) && (perf_rd_idx != perf_wr_idx)) {
                if ((perf_daddr != 0) && (perf_size != 0)) {
                    cvlog_perf_flexidag_entry_t *pSrcBuf;
                    pSrcBuf = (cvlog_perf_flexidag_entry_t *)ambacv_c2v(perf_daddr);
                    if (pSrcBuf == NULL) {
                        error_exit = 1;
                        printf("[ERROR] : perf_output_arm_payload() : FD[%3d] : Trace could not map fdarm_printbuf_perf_daddr (0x%08x)\n", loop, perf_daddr);
                    } /* if (pSrcBuf == NULL) */
                    else { /* if (pSrcBuf != NULL) */
                        ambacv_cache_invalidate(pSrcBuf, perf_size);

                        if(perf_rd_idx < perf_wr_idx) {
                            perf_output_size = (perf_wr_idx - perf_rd_idx) * sizeof(cvlog_perf_flexidag_entry_t);
                            if(mix_arm_vis_perf == 0U) {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, armperf_pld_fd[loop]);
                            } else {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, visperf_pld_fd[0]);
                            }
                            armperf_size[loop] += perf_output_size;

                            armperf_rd_idx[loop] = perf_wr_idx;
                        } else {
                            perf_output_size = (perf_total_idx - perf_rd_idx) * sizeof(cvlog_perf_flexidag_entry_t);
                            if(mix_arm_vis_perf == 0U) {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, armperf_pld_fd[loop]);
                            } else {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, visperf_pld_fd[0]);
                            }
                            armperf_size[loop] += perf_output_size;

                            perf_output_size = (perf_wr_idx) * sizeof(cvlog_perf_flexidag_entry_t);
                            if(mix_arm_vis_perf == 0U) {
                                fwrite(&pSrcBuf[0], perf_output_size, 1, armperf_pld_fd[loop]);
                            } else {
                                fwrite(&pSrcBuf[0], perf_output_size, 1, visperf_pld_fd[0]);
                            }
                            armperf_size[loop] += perf_output_size;

                            armperf_rd_idx[loop] = perf_wr_idx;
                        }
                        if(debug_log) {
                            printf("[INFO] : perf_output_arm_payload() : armperf[%d] update idx [%d - %d] size 0x%x , totoal idx %d\n",loop,perf_rd_idx, armperf_rd_idx[loop],armperf_size[loop],perf_total_idx);
                        }
                    } /* if (pSrcBuf != NULL) */
                } /* if ((perf_daddr != 0) && (perf_size != 0)) */
                else { /* if ((perf_daddr == 0) || (perf_size == 0)) */
                    printf("[WARN!] : perf_output_arm_payload() : VISPERF[%d] : dump requested, but no perf buffer exists for this core\n", loop);
                } /* if ((perf_daddr == 0) || (perf_size == 0)) */
            }
            loop++;
        } /* while ((loop < num_armperf_dump) && (error_exit == 0)) */
    }
    return error_exit;
}

uint32_t perf_output_dsp_payload(uint32_t error_enter)
{
    uint32_t  error_exit = error_enter;

    if(error_exit == 0) {
        if (num_dspperf_dump == 1) {
            uint32_t perf_daddr;
            uint32_t perf_size;
            uint32_t perf_output_size;
            uint32_t perf_rd_idx;
            uint32_t perf_wr_idx;
            uint32_t perf_total_idx;

            perf_daddr  = dsp_perf_ctrl->LogBufAddr;
            perf_size   = dsp_perf_ctrl->LogNum*sizeof(cvlog_perf_entry_t);
            perf_rd_idx = dsp_perf_rd_idx;
            perf_total_idx = perf_size/((uint32_t)sizeof(cvlog_perf_entry_t));
            perf_wr_idx = dsp_perf_ctrl->WP;
            perf_wr_idx = perf_wr_idx%perf_total_idx;
            if((error_exit == 0U) && (perf_rd_idx != perf_wr_idx)) {
                if ((perf_daddr != 0) && (perf_size != 0)) {
                    cvlog_perf_entry_t *pSrcBuf;
                    pSrcBuf = (cvlog_perf_entry_t *)dsp_perf_ptr;
                    if (pSrcBuf == NULL) {
                        error_exit = 1;
                        printf("[ERROR] : perf_output_dsp_payload() : could not map (0x%08x)\n", perf_daddr);
                    } /* if (pSrcBuf == NULL) */
                    else { /* if (pSrcBuf != NULL) */
                        if(perf_rd_idx < perf_wr_idx) {
                            perf_output_size = (perf_wr_idx - perf_rd_idx) * sizeof(cvlog_perf_entry_t);
                            if(mix_dsp_vis_perf == 0U) {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, dsp_perf_pld_fd);
                            } else {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, visperf_pld_fd[0]);
                            }
                            dsp_perf_size += perf_output_size;

                            dsp_perf_rd_idx = perf_wr_idx;
                        } else {
                            perf_output_size = (perf_total_idx - perf_rd_idx) * sizeof(cvlog_perf_entry_t);
                            if(mix_dsp_vis_perf == 0U) {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, dsp_perf_pld_fd);
                            } else {
                                fwrite(&pSrcBuf[perf_rd_idx], perf_output_size, 1, visperf_pld_fd[0]);
                            }
                            dsp_perf_size += perf_output_size;

                            perf_output_size = (perf_wr_idx) * sizeof(cvlog_perf_entry_t);
                            if(mix_dsp_vis_perf == 0U) {
                                fwrite(&pSrcBuf[0], perf_output_size, 1, dsp_perf_pld_fd);
                            } else {
                                fwrite(&pSrcBuf[0], perf_output_size, 1, visperf_pld_fd[0]);
                            }
                            dsp_perf_size += perf_output_size;

                            dsp_perf_rd_idx = perf_wr_idx;
                        }
                        if(debug_log) {
                            printf("[INFO] : perf_output_dsp_payload() : dsp perf update idx [%d - %d] size 0x%x , totoal idx %d\n",perf_rd_idx, dsp_perf_rd_idx,dsp_perf_size,perf_total_idx);
                        }
                    } /* if (pSrcBuf != NULL) */
                } /* if ((perf_daddr != 0) && (perf_size != 0)) */
            }
        } /* while ((loop < num_armperf_dump) && (error_exit == 0)) */
    }
    return error_exit;
}

uint32_t perf_output_vis_header(uint32_t error_enter)
{
    uint32_t  loop = 0;
    uint32_t  error_exit = error_enter;

    if(error_exit == 0) {
        loop = 0;
        while ((loop < num_visperf_dump) && (error_exit == 0)) {
            if (visperf_size[loop] != 0) {
                ambaprof_header_t   prof_header;
                ambaprof_section_t  prof_section;
                uint32_t  slotloop;
                uint32_t  sectionpos;

                prof_header.version       = 2;
                prof_header.magic_num     = 0xBABEFACE;
                prof_header.section_count = 1 + flexidag_num_active;
                prof_header.padding       = 0;
                if (cavalry_num_active > 0) {
                    prof_header.section_count++;
                }
                if (autorun_num_avail > 0) {
                    prof_header.section_count++;
                }
                fwrite(&prof_header, sizeof(prof_header), 1, visperf_hdr_fd[loop]);
                if(debug_log) {
                    printf("[INFO] : perf_output_vis_header() : prof_header section_count %d \n",prof_header.section_count);
                }
                sectionpos = sizeof(prof_header) + (prof_header.section_count * sizeof(prof_section));
                for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                    if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                        memset(&prof_section, 0, sizeof(prof_section));
                        strcpy(&prof_section.name[0], "flexidag names");
                        prof_section.size = flexidag_info_size[slotloop];
                        prof_section.base = sectionpos;
                        sectionpos += prof_section.size;
                        fwrite(&prof_section, sizeof(prof_section), 1, visperf_hdr_fd[loop]);
                        if(debug_log) {
                            printf("[INFO] : perf_output_vis_header() : prof_section[%d] name (%s) base 0x%x size 0x%x \n",slotloop,prof_section.name,prof_section.base,prof_section.size);
                        }
                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                if (cavalry_num_active > 0) {
                    memset(&prof_section, 0, sizeof(prof_section));
                    strcpy(&prof_section.name[0], "cavalry names");
                    prof_section.size = cavalry_num_active * sizeof(ambaprof_cavalry_header_t);
                    prof_section.base = sectionpos;
                    sectionpos += prof_section.size;
                    fwrite(&prof_section, sizeof(prof_section), 1, visperf_hdr_fd[loop]);
                    if(debug_log) {
                        printf("[INFO] : perf_output_vis_header() : prof_section name (%s) base 0x%x size 0x%x \n",prof_section.name,prof_section.base,prof_section.size);
                    }
                } /* if (cavalry_num_active > 0) */

                if (autorun_num_avail > 0) {
                    memset(&prof_section, 0, sizeof(prof_section));
                    strcpy(&prof_section.name[0], "autorun names");
                    prof_section.size = autorun_num_avail * AMBAPROF_MAX_NAME_LEN;
                    prof_section.base = sectionpos;
                    sectionpos += prof_section.size;
                    fwrite(&prof_section, sizeof(prof_section), 1, visperf_hdr_fd[loop]);
                    if(debug_log) {
                        printf("[INFO] : perf_output_vis_header() :  prof_section name (%s) base 0x%x size 0x%x \n",prof_section.name,prof_section.base,prof_section.size);
                    }
                } /* if (autorun_num_avail > 0) */

                memset(&prof_section, 0, sizeof(prof_section));
                strcpy(&prof_section.name[0], "profile events");
                if(mix_arm_vis_perf == 0U) {
                    prof_section.size = visperf_size[loop];
                } else {
                    prof_section.size = visperf_size[loop] + armperf_size[0] + dsp_perf_size;
                }
                prof_section.base = sectionpos;
                fwrite(&prof_section, sizeof(prof_section), 1, visperf_hdr_fd[loop]);
                if(debug_log) {
                    printf("[INFO] : perf_output_vis_header() : prof_section name (%s) base 0x%x size 0x%x \n",prof_section.name,prof_section.base,prof_section.size);
                }
                for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                    if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                        fwrite(pFlexidag_Info[slotloop], flexidag_info_size[slotloop], 1, visperf_hdr_fd[loop]);
                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                if (cavalry_num_active > 0) {
                    uint32_t  subloop;
                    for (subloop = 0; subloop < CAVALRY_TRACE_MAX_PROCESS; subloop++) {
                        if (Local_CavTrace.cavalry_info[subloop].cavalry_pid != CAVALRY_INVALID_PID) {
                            ambaprof_cavalry_header_t header;
                            memset(&header, 0, sizeof(header));
                            header.cavalry_pid = Local_CavTrace.cavalry_info[subloop].cavalry_pid;
                            strncpy(&header.process_name[0], &Local_CavTrace.cavalry_info[subloop].process_name[0], sizeof(header.process_name));
                            fwrite(&header, sizeof(header), 1, visperf_hdr_fd[loop]);
                        } /* if (Local_CavTrace.cavalry_info[subloop].cavalry_pid != CAVALRY_INVALID_PID) */
                    } /* for (subloop = 0; subloop < CAVALRY_TRACE_MAX_PROCESS; subloop++) */
                } /* if (cavalry_num_active > 0) */

                if (autorun_num_avail > 0) {
                    uint32_t  subloop;
                    char      autorun_cvtask_name[AMBAPROF_MAX_NAME_LEN];
                    for (subloop = 0; subloop < autorun_num_avail; subloop++) {
                        memset(&autorun_cvtask_name[0], 0, sizeof(autorun_cvtask_name));
                        strncpy(&autorun_cvtask_name[0], &pLocal_AutoRunTrace[subloop].autorun_cvtask_name[0], sizeof(autorun_cvtask_name));
                        fwrite(&autorun_cvtask_name[0], 1, sizeof(autorun_cvtask_name), visperf_hdr_fd[loop]);
                    } /* for (subloop = 0; subloop < autorun_num_avail; subloop++) */
                } /* if (autorun_num_avail > 0) */
            } /* if (visperf_size[loop] != 0) */
            loop++;
        } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */
    }
    return error_exit;
}

uint32_t perf_output_arm_header(uint32_t error_enter)
{
    uint32_t  loop = 0;
    uint32_t  error_exit = error_enter;

    if(mix_arm_vis_perf == 0U) {
        if(error_exit == 0) {
            loop = 0;
            while ((loop < num_armperf_dump) && (error_exit == 0)) {
                if (armperf_size[loop] != 0) {
                    ambaprof_header_t   prof_header;
                    ambaprof_section_t  prof_section;
                    uint32_t  slotloop;
                    uint32_t  sectionpos;

                    prof_header.version       = 2;
                    prof_header.magic_num     = 0xBABEFACE;
                    prof_header.section_count = 1 + flexidag_num_active;
                    prof_header.padding       = 0;
                    fwrite(&prof_header, sizeof(prof_header), 1, armperf_hdr_fd[loop]);
                    if(debug_log) {
                        printf("[INFO] : perf_output_arm_header() : prof_header section_count %d \n",prof_header.section_count);
                    }
                    sectionpos = sizeof(prof_header) + (prof_header.section_count * sizeof(prof_section));
                    for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                        if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                            memset(&prof_section, 0, sizeof(prof_section));
                            strcpy(&prof_section.name[0], "flexidag names");
                            prof_section.size = flexidag_info_size[slotloop];
                            prof_section.base = sectionpos;
                            sectionpos += prof_section.size;
                            fwrite(&prof_section, sizeof(prof_section), 1, armperf_hdr_fd[loop]);
                            if(debug_log) {
                                printf("[INFO] : perf_output_arm_header() : prof_section[%d] name (%s) base 0x%x size 0x%x \n",slotloop,prof_section.name,prof_section.base,prof_section.size);
                            }
                        } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                    memset(&prof_section, 0, sizeof(prof_section));
                    strcpy(&prof_section.name[0], "profile events");
                    prof_section.size = armperf_size[loop];
                    prof_section.base = sectionpos;
                    fwrite(&prof_section, sizeof(prof_section), 1, armperf_hdr_fd[loop]);
                    if(debug_log) {
                        printf("[INFO] : perf_output_arm_header() : prof_section name (%s) base 0x%x size 0x%x \n",prof_section.name,prof_section.base,prof_section.size);
                    }
                    for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                        if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                            fwrite(pFlexidag_Info[slotloop], flexidag_info_size[slotloop], 1, armperf_hdr_fd[loop]);
                        } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                } /* if (visperf_size[loop] != 0) */
                loop++;
            }
        }
    }
    return error_exit;
}

uint32_t perf_output_dsp_header(uint32_t error_enter)
{
    uint32_t  error_exit = error_enter;

    if(mix_dsp_vis_perf == 0U) {
        if(error_exit == 0) {
            if (num_dspperf_dump == 1) {
                if (dsp_perf_size != 0) {
                    ambaprof_header_t   prof_header;
                    ambaprof_section_t  prof_section;
                    uint32_t  slotloop;
                    uint32_t  sectionpos;

                    prof_header.version       = 2;
                    prof_header.magic_num     = 0xBABEFACE;
                    prof_header.section_count = 1;
                    prof_header.padding       = 0;
                    fwrite(&prof_header, sizeof(prof_header), 1, dsp_perf_hdr_fd);
                    if(debug_log) {
                        printf("[INFO] : perf_output_dsp_header() : prof_header section_count %d \n",prof_header.section_count);
                    }
                    sectionpos = sizeof(prof_header) + (prof_header.section_count * sizeof(prof_section));
                    memset(&prof_section, 0, sizeof(prof_section));
                    strcpy(&prof_section.name[0], "profile events");
                    prof_section.size = dsp_perf_size;
                    prof_section.base = sectionpos;
                    fwrite(&prof_section, sizeof(prof_section), 1, dsp_perf_hdr_fd);
                    if(debug_log) {
                        printf("[INFO] : perf_output_dsp_header() : prof_section name (%s) base 0x%x size 0x%x \n",prof_section.name,prof_section.base,prof_section.size);
                    }
                } /* if (visperf_size[loop] != 0) */
            }
        }
    }
    return error_exit;
}

uint32_t perf_concat_vis(uint32_t error_enter)
{
    uint32_t  loop = 0;
    uint32_t  error_exit = error_enter;

    if(error_exit == 0) {
        while ((loop < num_visperf_dump) && (error_exit == 0)) {
            char c;
            uint32_t count = 0;
            uint32_t  concat_size;

            if(mix_arm_vis_perf == 0U) {
                concat_size = visperf_size[loop];
            } else {
                concat_size = visperf_size[loop] + armperf_size[0] + dsp_perf_size;
            }

            fseek( visperf_pld_fd[loop], 0, SEEK_SET);
            do {
                c = fgetc(visperf_pld_fd[loop]);
                fputc(c, visperf_hdr_fd[loop]);
                count++;
            } while ((c != EOF) && (count < concat_size));

            if(count != concat_size) {
                printf("[WARN!] : perf_concat_vis() : VISPERF[%d] : write 0x%x but total 0x%x\n", loop,count,concat_size);
            }
            loop++;
        }
    }
    return error_exit;
}

uint32_t perf_concat_arm(uint32_t error_enter)
{
    uint32_t  loop = 0;
    uint32_t  error_exit = error_enter;

    if(mix_arm_vis_perf == 0U) {
        if(error_exit == 0) {
            while ((loop < num_armperf_dump) && (error_exit == 0)) {
                char c;
                uint32_t count = 0;

                fseek( armperf_pld_fd[loop], 0, SEEK_SET);
                do {
                    c = fgetc(armperf_pld_fd[loop]);
                    fputc(c, armperf_hdr_fd[loop]);
                    count++;
                } while ((c != EOF) && (count <armperf_size[loop]));

                if(count != armperf_size[loop]) {
                    printf("[WARN!] : perf_concat_arm() : ARMPERF[%d] : write 0x%x but total 0x%x\n", loop,count,armperf_size[loop]);
                }
                loop++;
            }
        }
    }
    return error_exit;
}

uint32_t perf_concat_dsp(uint32_t error_enter)
{
    uint32_t  error_exit = error_enter;

    if(mix_dsp_vis_perf == 0U) {
        if(error_exit == 0) {
            if (num_dspperf_dump == 1) {
                char c;
                uint32_t count = 0;

                fseek( dsp_perf_pld_fd, 0, SEEK_SET);
                do {
                    c = fgetc(dsp_perf_pld_fd);
                    fputc(c, dsp_perf_hdr_fd);
                    count++;
                } while ((c != EOF) && (count <dsp_perf_size));

                if(count != dsp_perf_size) {
                    printf("[WARN!] : perf_concat_dsp() : DSP PERF : write 0x%x but total 0x%x\n",count,dsp_perf_size);
                }
            }
        }
    }
    return error_exit;
}

#define PERF_STATE_INIT           0
#define PERF_STATE_START          1
#define PERF_STATE_STOP           2
#define PERF_STATE_WAIT_START     3

uint32_t  perf_stream_state = PERF_STATE_INIT;
uint32_t  perf_stop = 0U;

static void signal_handler_start()
{
    uint32_t  error_exit = 0,loop;

    perf_stop = 0;
    if (perf_stream_state == PERF_STATE_INIT) {
        uint32_t state;

        schdr_get_ambacv_state(&state);
        if(state == 1) {
            printf(" pdc_dump start \n");
            /* Init */
            error_exit = perf_stream_init(error_exit);

            /* Rest size and rd_idx*/
            for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) {
            visperf_size[loop]        = 0;
            visperf_rd_idx[loop]      = 0;
            } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */      
            schdr_get_perf_last_wridx_daddr(0,&visperf_rd_idx[0]);

            for (loop = 0; loop < SYSTEM_MAX_NUM_CORTEX; loop++) {
            armperf_size[loop]        = 0;
            armperf_rd_idx[loop]      = 0;
            } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */
            schdr_get_arm_perf_last_wridx_daddr(&armperf_rd_idx[0]);


            dsp_perf_size = 0;
            dsp_perf_rd_idx = 0;
            if(num_dspperf_dump == 1){
            dsp_perf_rd_idx = dsp_perf_ctrl->WP;
            }
            error_exit = perf_stream_init(error_exit);

            /* Start */
            perf_stream_state = PERF_STATE_START;
        } else {
            perf_stream_state = PERF_STATE_WAIT_START;
        }
    }
}

static void signal_handler_stop()
{
    perf_stop = 1;
    if (perf_stream_state == PERF_STATE_START) {
        printf(" pdc_dump stop \n");
        perf_stream_state = PERF_STATE_STOP;
    }
}

int main(int argc, char **argv)
{
    uint32_t  error_exit = 0;
    uint32_t  runable = 1;

    signal(SIGUSR1, signal_handler_start);
    signal(SIGUSR2, signal_handler_stop);

    if (argc < 2) {
        usage();
        return 0;
    } /* if (argc < 2) */

    if (init_param(argc, argv) < 0) {
        return -1;
    }

    while((runable) && (error_exit == 0)) {
        if(perf_stream_state == PERF_STATE_INIT) {

        } else if(perf_stream_state == PERF_STATE_WAIT_START) {
            uint32_t state,loop;

            schdr_get_ambacv_state(&state);
            if(state == 1) {
                printf(" pdc_dump start \n");
                /* Init */
                error_exit = perf_stream_init(error_exit);

                /* Rest size and rd_idx*/
                for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) {
                    visperf_size[loop]        = 0;
                    visperf_rd_idx[loop]      = 0;
                } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */      
                schdr_get_perf_last_wridx_daddr(0,&visperf_rd_idx[0]);

                for (loop = 0; loop < SYSTEM_MAX_NUM_CORTEX; loop++) {
                    armperf_size[loop]        = 0;
                    armperf_rd_idx[loop]      = 0;
                } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */
                schdr_get_arm_perf_last_wridx_daddr(&armperf_rd_idx[0]);


                dsp_perf_size = 0;
                dsp_perf_rd_idx = 0;
                if(num_dspperf_dump == 1){
                    dsp_perf_rd_idx = dsp_perf_ctrl->WP;
                }

                /* Start */
                perf_stream_state = PERF_STATE_START;
            } else {
                usleep(sample_time);
            }
        } else if(perf_stream_state == PERF_STATE_START) {
            uint32_t state;

            schdr_get_ambacv_state(&state);
            if(state == 1) {
                error_exit = perf_update_flexidag_info(error_exit);
                error_exit = perf_output_vis_payload(error_exit);
                error_exit = perf_output_arm_payload(error_exit);
                error_exit = perf_output_dsp_payload(error_exit);
                if(error_exit != 0) {
                    //perf_update_cavalry_info(0);
                    perf_update_autorun_info(0);
                    perf_output_vis_header(0);
                    perf_output_arm_header(0);
                    perf_output_dsp_header(0);
                    perf_concat_vis(0);
                    perf_concat_arm(0);
                    perf_concat_dsp(0);
                    perf_stream_cleanup();
                    perf_stream_state = PERF_STATE_INIT;
                } else {
                    time_t rawtime;
                    struct tm * curr_timeinfo;
                    int32_t expire_min = 10;

                    time(&rawtime);
                    curr_timeinfo = localtime(&rawtime);
                    if(curr_timeinfo->tm_min == ((start_timeinfo.tm_min + expire_min) % 60)) {
                        perf_stream_state = PERF_STATE_STOP;
                    }
                }
            }else {
                perf_stream_state = PERF_STATE_STOP;
                perf_stop = 1U;
            }
        } else if(perf_stream_state == PERF_STATE_STOP) {
            //perf_update_cavalry_info(0);
            perf_update_autorun_info(0);
            perf_output_vis_header(0);
            perf_output_arm_header(0);
            perf_output_dsp_header(0);
            perf_concat_vis(0);
            perf_concat_arm(0);
            perf_concat_dsp(0);
            perf_stream_cleanup();
            perf_stream_state = PERF_STATE_INIT;
            if(perf_stop == 0) {
                error_exit = perf_stream_init(error_exit);
                perf_stream_state = PERF_STATE_START;
            }
        } else {
            runable = 0;
        }
        usleep(sample_time);
    }

    printf(" pdc_dump exit \n");
    return 0;

}

