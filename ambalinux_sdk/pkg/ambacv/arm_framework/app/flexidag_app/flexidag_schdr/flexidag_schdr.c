/*******************************************************************************
 * flexidag_schdr.c
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

#define FILENAME_LENGTH     (256)

static int fd_ambacv          = -1;
static int startup_requested  = 0;
static int shutdown_requested = 0;
static int cavalry_num_slots  = 0;
static int flexidag_num_slots = 8;
static int schdr_verbosity    = 0;
static int flexidag_boot_cfg  = 1;
static int autorun_interval = 0;
static int autorun_fex_cfg = 0;
static int flexidag_perf_time = 0;
static char visorc_path[FILENAME_LENGTH] = "/lib/firmware/";

static int  dump_requested    = 0;

static int  sched_dump        = 0;
static int  num_vissched_dump = 0;
static char vissched_filename[SYSTEM_MAX_NUM_VISORC][FILENAME_LENGTH];

static int  perf_dump         = 0;
static int  num_visperf_dump  = 0;
static int  num_armperf_dump  = 0;
static char visperf_filename[SYSTEM_MAX_NUM_VISORC][FILENAME_LENGTH];
static char armperf_filename[SYSTEM_MAX_NUM_CORTEX][FILENAME_LENGTH];

static int external_msg_thread = 0;

#define NO_ARG    0
#define HAS_ARG  1

static struct option long_options[] = {
    {"start", NO_ARG, 0, 's'},
    {"stop", NO_ARG, 0, 'x'},
    {"slots", HAS_ARG, 0, 'n'},
    {"vispath", HAS_ARG, 0, 'v'},
    {"verbose", NO_ARG, 0, 'V'},
    {"cavslots", HAS_ARG, 0, 'c'},
    {"boot_cfg", HAS_ARG, 0, 'b'},
    {"autorun_intval", HAS_ARG, 0, 'i'},
    {"autorun_fex_cfg", HAS_ARG, 0, 'f'},
    {"visperf", HAS_ARG, 0, 'p'},
    {"armperf", HAS_ARG, 0, 'P'},
    {"vissched", HAS_ARG, 0, 'l'},
    {"visperfshow", HAS_ARG, 0, 't'},
    {"extern msg thread", HAS_ARG, 0, 'e'},
    {0, 0, 0, 0},
};

static const char *short_options = "sxn:v:Vc:b:p:P:l:t:i:f:e";

struct hint_s {
    const char *arg;
    const char *str;
};

static const struct hint_s hint[] = {
    {"",          "\t\tRequests flexidag scheduler to start, loads visorc binary."},
    {"",          "\t\tRequests flexidag scheduler to stop."},
    {"numslots",  "\tConfigures number of flexidag slots to use."},
    {"path",      "\tConfigures location of default visorc scheduler binaries."},
    {"",          "\t\tAdds verbosity to startup/shutdown sequence prints."},
    {"numslots",  "Configures unmber of cavalry slots to use."},
    {"config",    "\tFlexidag boot cfg."},
    {"interval in ms",  "\tAutorun cvtask interval in ms(Optional for ASIL CHIP), defaults to 90ms if zero or greater than 10000.."},
    {"fex config",  "\tAutorun fex cvtask config(Optional for ASIL CHIP), disable if set 1."},
    {"filename",  "\tlogfile to dump full visorc perf log to."},
    {"filename",  "\tlogfile to dump full arm perf log to."},
    {"filename",  "logfile to dump full visorc scheduler log to."},
    {"range",     "Show visorc perf result in certain time range(ms)."},
    {"",     "Enable external msg thread loop."},
};

void usage(void)
{
    int i;

    printf("flexidag_schdr usage:\n");
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
        case 'v':
            value = strlen(optarg);
            if (value >= FILENAME_LENGTH) {
                printf("Filename [%s] is too long [%d] (>%d).\n", optarg, value, FILENAME_LENGTH);
                return -1;
            }
            memset(visorc_path, 0x0, sizeof(visorc_path));
            strncpy(visorc_path, optarg, value);
            break;

        case 's':
            if (shutdown_requested != 0) {
                printf("Shutdown previously requested, switching request to startup\n");
                shutdown_requested = 0;
            }
            startup_requested = 1;
            break;

        case 'x':
            if (startup_requested != 0) {
                printf("Startup previously requested, switching request to shutdown\n");
                startup_requested = 0;
            }
            shutdown_requested = 1;
            break;

        case 'n':
            flexidag_num_slots = atoi(optarg);
            break;

        case 'c':
            cavalry_num_slots = atoi(optarg);
            break;

        case 'i':
            autorun_interval = atoi(optarg);
            break;
        case 'f':
            autorun_fex_cfg = atoi(optarg);
            break;

        case 'V':
            schdr_verbosity = 1;
            break;

        case 'b':
            flexidag_boot_cfg = atoi(optarg);
            break;

        case 'p':
            value = strlen(optarg);
            if (value >= FILENAME_LENGTH) {
                printf("flexidag_schdr() : --visperf : Filename [%s] is too long [%d] (>%d); exiting \n", optarg, value, FILENAME_LENGTH);
                return -1;
            } /* if (value >= FILENAME_LENGTH) */
            if (num_visperf_dump >= SYSTEM_MAX_NUM_VISORC) {
                printf("flexidag_schdr() : --visperf : Too many requests made (max %d); exiting\n", SYSTEM_MAX_NUM_VISORC);
                return -1;
            } /* if (num_visperf_dump >= SYSTEM_MAX_NUM_VISORC) */
            else { /* if (num_visperf_dump < SYSTEM_MAX_NUM_VISORC) */
                memset(&visperf_filename[num_visperf_dump][0], 0x0, sizeof(visperf_filename[num_visperf_dump]));
                strncpy(&visperf_filename[num_visperf_dump][0], optarg, value);
                num_visperf_dump++;
                perf_dump       = 1;
                dump_requested  = 1;
            } /* if (num_visperf_dump < SYSTEM_MAX_NUM_VISORC) */
            break;

        case 'P':
            value = strlen(optarg);
            if (value >= FILENAME_LENGTH) {
                printf("flexidag_schdr() : --armperf : Filename [%s] is too long [%d] (>%d); exiting\n", optarg, value, FILENAME_LENGTH);
                return -1;
            } /* if (value >= FILENAME_LENGTH) */
            if (num_armperf_dump >= SYSTEM_MAX_NUM_CORTEX) {
                printf("flexidag_schdr() : --armperf : Too many requests made (max %d); exiting\n", SYSTEM_MAX_NUM_CORTEX);
                return -1;
            } /* if (num_armperf_dump >= SYSTEM_MAX_NUM_CORTEX) */
            else { /* if (num_armperf_dump < SYSTEM_MAX_NUM_CORTEX) */
                memset(&armperf_filename[num_armperf_dump][0], 0x0, sizeof(armperf_filename[num_armperf_dump]));
                strncpy(&armperf_filename[num_armperf_dump][0], optarg, value);
                num_armperf_dump++;
                perf_dump       = 1;
                dump_requested  = 1;
            } /* if (num_armperf_dump < SYSTEM_MAX_NUM_CORTEX) */
            break;

        case 'l':
            value = strlen(optarg);
            if (value >= FILENAME_LENGTH) {
                printf("flexidag_schdr() : --vissched : Filename [%s] is too long [%d] (>%d); exiting\n", optarg, value, FILENAME_LENGTH);
                return -1;
            } /* if (value >= FILENAME_LENGTH) */
            if (num_vissched_dump >= SYSTEM_MAX_NUM_VISORC) {
                printf("flexidag_schdr() : --vissched : Too many requests made (max %d); exiting\n", SYSTEM_MAX_NUM_VISORC);
                return -1;
            } /* if (num_vissched_dump >= SYSTEM_MAX_NUM_VISORC) */
            else { /* if (num_vissched_dump < SYSTEM_MAX_NUM_VISORC) */
                memset(&vissched_filename[num_vissched_dump][0], 0x0, sizeof(vissched_filename[num_vissched_dump]));
                strncpy(&vissched_filename[num_vissched_dump][0], optarg, value);
                num_vissched_dump++;
                sched_dump      = 1;
                dump_requested  = 1;
            } /* if (num_vissched_dump < SYSTEM_MAX_NUM_VISORC) */
            break;

        case 't':
            flexidag_perf_time = atoi(optarg);
            break;
        case 'e':
            external_msg_thread = 1;
            break;
        default:
            printf("unknown option found: %c\n", ch);
            return -1;
            break;
        } /* switch (ch) */
    } /* while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) */

    return 0;

} /* init_param() */

static void show_perf(const AMBA_CV_FLEXIDAG_PERF_s *flexidag_perf_array, uint32_t num_of_flexidag, uint32_t cvcore)
{
    uint32_t i,totoal_flexidag_run = 0U, percent,total_time = 0U;
    float fltmp,value;
    uint32_t hw_unit = 1U;

    printf("label                               runs      vp total_run(us)     total(us)     vp min(us)     vp avg(us)     vp max(us)     frame min(us)     frame avg(us)     frame max(us)\n");
    printf("=================================================================================================================================================================================\n");
    for(i = 0U; i < num_of_flexidag; i++) {
        value = (float) flexidag_perf_array[i].total_time;
        fltmp = (float) (flexidag_perf_array[i].total_run_time);
        fltmp = (float) ((fltmp*100.0) / value);
        percent = (uint32_t)fltmp;

        printf("%-32s  %08d  %13d ( %02d )  %10d  %13d  %13d  %13d  %16d  %16d  %16d\n",
               flexidag_perf_array[i].label,
               flexidag_perf_array[i].runs,
               flexidag_perf_array[i].total_run_time,
               percent,
               flexidag_perf_array[i].total_time,
               flexidag_perf_array[i].min_time,
               flexidag_perf_array[i].avg_time,
               flexidag_perf_array[i].max_time,
               flexidag_perf_array[i].min_frame_time,
               flexidag_perf_array[i].avg_frame_time,
               flexidag_perf_array[i].max_frame_time);
        totoal_flexidag_run += flexidag_perf_array[i].total_run_time;
        total_time = flexidag_perf_array[i].total_time;
    }
    printf("=================================================================================================================================================================================\n");

#if defined(CHIP_CV6)
    if(cvcore == CVCORE_VP0) {
        hw_unit = 6U; // 6NVP
    } else if(cvcore == CVCORE_FEX) {
        hw_unit = 2U;
    } else if(cvcore == CVCORE_FMA) {
        hw_unit = 2U;
    } else {
        hw_unit = 1U;
    }
#else
    hw_unit = 1U;
#endif

    if(num_of_flexidag == 0U) {
        percent = 0U;
    } else {
        value = (float) (flexidag_perf_array[num_of_flexidag - 1U].total_time * hw_unit);
        fltmp = (float) (totoal_flexidag_run);
        fltmp = (float) ((fltmp*100.0) / value);
        percent = (uint32_t)fltmp;
    }

    printf("                                            %13d ( %02d )  %10d  with %02d hw unit\n",
           totoal_flexidag_run,
           percent,
           total_time,
           hw_unit);

}

static int load_binary(char *buff, int max_size, void *token)
{
    const char *name = (const char *)token;
    FILE *ifp;
    int size;

    ifp = fopen(name, "rb");
    if (ifp == NULL) {
        printf("Can't open input file %s!\n", name);
        exit(-1);
    }
    fseek(ifp, 0, SEEK_END);
    size = ftell(ifp);

    if (buff != NULL) {
        if (size > max_size) {
            printf("The provided buff too small for %s!\n", name);
            exit(-1);
        }
        fseek(ifp, 0, SEEK_SET);
        assert(size == fread(buff, 1, size, ifp));
    }

    fclose(ifp);
    return size;
}

static pthread_t cv_msg_thread;
static int cv_msg_loop = 1;

static void* cv_msg_tsk(void* arg)
{
    unsigned int msg_num, i;
    unsigned int retcode;
    unsigned int message_type, message_retcode;
    AMBA_CV_FLEXIDAG_HANDLE_s *phandle;

    while(cv_msg_loop == 1U) {
        retcode = AmbaCV_SchdrWaitMsg(&msg_num);
        if(retcode == ERRCODE_NONE) {
            for(i = 0U; i < msg_num; i++) {
                retcode = AmbaCV_SchdrProcessMsg(&phandle, &message_type, &message_retcode);
            }
        } else {
            printf("[ERROR] cv_msg_tsk() : AmbaCV_SchdrWaitMsg fail ret (0x%x)", retcode, 0U, 0U, 0U, 0U);
            break;
        }
    }

    return NULL;
}

static void sigstop()
{
    if (external_msg_thread != 0) {
        cv_msg_loop = 0;
    }

    exit(0);
} /* sigstop() */

int main(int argc, char **argv)
{
    uint32_t size;
    int32_t ret;
    char tbar_name[256];

    signal(SIGINT, sigstop);
    signal(SIGQUIT, sigstop);
    signal(SIGTERM, sigstop);

    if (argc < 2) {
        usage();
        return 0;
    } /* if (argc < 2) */

    if (init_param(argc, argv) < 0) {
        return -1;
    }

    /*-= Parameter validation =-------------------------------------------------*/
    if ((flexidag_num_slots < 0) || (flexidag_num_slots > FLEXIDAG_MAX_SLOTS)) {
        printf("Invalid number of flexidag slots : %d (max %d)\n", flexidag_num_slots, FLEXIDAG_MAX_SLOTS);
        return -1;
    } /* if ((flexidag_num_slots < 1) || (flexidag_num_slots > FLEXIDAG_MAX_SLOTS)) */

    /*-= Proceed with initialization =------------------------------------------*/
    fd_ambacv = open("/dev/ambacv", O_SYNC | O_RDWR, 0);
    if (fd_ambacv < 0) {
        perror("/dev/ambacv");
        return -1;
    }

    if (external_msg_thread != 0) {
        printf("create schdr cmd thread \n");
        pthread_create(&cv_msg_thread, NULL, cv_msg_tsk, NULL);
        //pthread_setschedprio(cv_msg_thread, 90U);
    }

    if (startup_requested != 0) {
        /* load visload */
        if( flexidag_boot_cfg != 0 ) {
            ret = schdr_load_binary(visorc_path);
            if(ret != 0) {
                printf("schdr_load_binary fail ret (0x%x)\n", ret);
            }
            ret = (int32_t)AmbaCV_SchdrCheckVisorc(SCHDR_VP_BIN);
            if(ret != 0) {
                printf("AmbaCV_SchdrCheckVisorc fail ret (0x%x)\n", ret);
            }
        }
        ret = schdrcfg_set_flexidag_num_slots(flexidag_num_slots);
        if(ret != 0) {
            printf("schdrcfg_set_flexidag_num_slots fail ret (0x%x)\n", ret);
        }
        ret = schdrcfg_set_cavalry_num_slots(cavalry_num_slots);
        if(ret != 0) {
            printf("schdrcfg_set_cavalry_num_slots fail ret (0x%x)\n", ret);
        }
        ret = schdrcfg_set_cpu_map(0xD);
        if(ret != 0) {
            printf("schdrcfg_set_cpu_map fail ret (0x%x)\n", ret);
        }
        ret = schdrcfg_set_autorun_interval(autorun_interval);
        if(ret != 0) {
            printf("schdrcfg_set_autorun_interval fail ret (0x%x)\n", ret);
        }
        ret = schdrcfg_set_autorun_fex_cfg(autorun_fex_cfg);
        if(ret != 0) {
            printf("schdrcfg_set_autorun_fex_cfg fail ret (0x%x)\n", ret);
        }

        if (external_msg_thread != 0) {
            ret = schdrcfg_set_flags(SCHDR_NO_INTERNAL_THREAD);
            if(ret != 0) {
                printf("schdrcfg_set_flags fail ret (0x%x)\n", ret);
            }
        }
        ret = schdrcfg_set_boot_cfg(flexidag_boot_cfg);
        if(ret != 0) {
            printf("schdrcfg_set_boot_cfg fail ret (0x%x)\n", ret);
        }

        {
            FILE *tempfile;
            sprintf(&tbar_name[0], "%s/visorc_cvtable.tbar", visorc_path);
            tempfile = fopen(tbar_name, "rb");
            if (tempfile != NULL) {
                uint32_t size;
                fseek(tempfile, 0, SEEK_END);
                size = ftell(tempfile);
                fclose(tempfile);
                if (size != 0) {
                    ret = schdr_add_cvtable((void *)&tbar_name[0], load_binary);
                    if (ret != 0) {
                        printf("schdr_add_cvtable fail ret (0x%x)\n", ret);
                    }
                } /* if (size != 0) */
            } /* if (tempfile != NULL) */
        } /* auto-load visorc_cvtable.tbar file, if available */

        if (schdr_verbosity != 0) {
            ret = schdrcfg_set_verbosity(LVL_DEBUG);
            if(ret != 0) {
                printf("schdrcfg_set_boot_cfg fail ret (0x%x)\n", ret);
            }
        } /* if (schdr_verbosity == 0) */
        ret = schdr_standalone_start();
        if(ret != 0) {
            printf("schdr_standalone_start fail ret (0x%x)\n", ret);
        }
    } /* if (startup_requested != 0) */
    else if (shutdown_requested != 0) {
        if (schdr_verbosity != 0) {
            ret = schdrcfg_set_verbosity(LVL_DEBUG);
            if(ret != 0) {
                printf("schdrcfg_set_verbosity fail ret (0x%x)\n", ret);
            }
        } /* if (schdr_verbosity == 0) */
        ret = schdr_standalone_start(); /* Needed to connect to scheduler to perform shutdown */
        if(ret != 0) {
            printf("schdr_standalone_start fail ret (0x%x)\n", ret);
        }
        ret = schdr_shutdown(0);
        if(ret != 0) {
            printf("schdr_standalone_start fail ret (0x%x)\n", ret);
        }
    } /* if (shutdown_requested != 0) */
    else if (flexidag_perf_time != 0) {
        AMBA_CV_FLEXIDAG_PERF_s flexidag_perf_array[32];
        uint32_t num_of_flexidag;

        printf("flexidag_perf_time\n");
        flexidag_schdr_start();
        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_START, CVCORE_VP0, flexidag_perf_array, 32, &num_of_flexidag);
        usleep(flexidag_perf_time*1000);
        printf("runs            : how many runs in this perf\n");
        printf("vp total_run    : total vp run time in this perf (us)\n");
        printf("total           : total time in this perf (us)\n");
        printf("vp min          : min time of vp run in one frame (us) - without yeild\n");
        printf("vp avg          : avg time of vp run in one frame (us) - without yeild\n");
        printf("vp max          : max time of vp run in one frame (us) - without yeild\n");
        printf("frame min       : min time in one frame (us) - with yeild\n");
        printf("frame avg       : avg time in one frame (us) - with yeild\n");
        printf("frame max       : max time in one frame (us) - with yeild\n");

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP, CVCORE_VP0, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_VP0 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_VP0);

#if defined(CHIP_CV6)
        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_NVP0, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_NVP0 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_NVP0);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_NVP1, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_NVP1 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_NVP1);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_NVP2, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_NVP2 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_NVP2);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_NVP3, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_NVP3 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_NVP3);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_NVP4, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_NVP4 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_NVP4);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_NVP5, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_NVP5 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_NVP5);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_GVP0, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_GVP0 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_GVP0);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_GVP1, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_GVP1 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_GVP1);
#endif

#if defined(CHIP_CV2)
        // SP prefiling
        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_SP, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_SP :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_SP);
#endif


#if defined(CHIP_CV2) || defined(CHIP_CV6)
        // FMA prefiling
        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_FMA, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_FMA :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_FMA);

#if defined(CHIP_CV6)
        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_FMA0, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_FMA0 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_FMA0);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_FMA1, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_FMA1 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_FMA1);

#endif
#endif

#if defined(CHIP_CV2FS) || defined(CHIP_CV6)
        // FEX prefiling
        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_FEX, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_FEX :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_FEX);

#if defined(CHIP_CV6)
        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_FEX0, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_FEX0 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_FEX0);

        AmbaCV_FlexidagPerfAll(FLEXIDAG_PERF_STOP_CONTI, CVCORE_FEX1, flexidag_perf_array, 32, &num_of_flexidag);
        printf("CVCORE_FEX1 :\n");
        show_perf(flexidag_perf_array, num_of_flexidag, CVCORE_FEX1);
#endif
#endif
    } /* if (flexidag_perf_time != 0) */
    else if (dump_requested != 0) {
        flexidag_schdr_start(); /* Connect via standard flexidag_schdr_start() */

        {
            flexidag_trace_t *pFlexidagTrace;

            pFlexidagTrace = schdr_get_pFlexidagTrace();

            if (pFlexidagTrace != NULL) {
                uint32_t  error_exit;

                error_exit              = 0;

                /*==================================================================================================*/
                /*=- Dump Perf Logs if requested -==================================================================*/
                /*==================================================================================================*/
                if ((perf_dump != 0) && (error_exit == 0)) {
                    uint8_t  *pVISPerfCopy[SYSTEM_MAX_NUM_VISORC];
                    uint8_t  *pARMPerfCopy[SYSTEM_MAX_NUM_CORTEX];
                    uint32_t  visperf_size[SYSTEM_MAX_NUM_VISORC];
                    uint32_t  armperf_size[SYSTEM_MAX_NUM_CORTEX];
                    uint8_t  *pFlexidag_Info[FLEXIDAG_MAX_SLOTS];
                    uint32_t  flexidag_info_size[FLEXIDAG_MAX_SLOTS];
                    uint32_t  flexidag_hdr_total_size;
                    uint32_t  flexidag_num_enabled;
                    uint32_t  flexidag_num_active;
                    uint32_t  cavalry_num_active;
                    uint32_t  loop;
                    cavalry_trace_t  *pCavalryTrace;
                    cavalry_trace_t  *pLocal_CavTrace;

                    uint32_t  autorun_num_avail;
                    autorun_trace_t  *pAutoRunTrace;
                    autorun_trace_t  *pLocal_AutoRunTrace;

                    flexidag_num_enabled  = pFlexidagTrace->num_slots_enabled;  /* May change during run */
                    flexidag_num_active   = 0;
                    cavalry_num_active    = 0;

                    for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) {
                        pVISPerfCopy[loop]        = NULL;
                        visperf_size[loop]        = 0;
                    } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */
                    for (loop = 0; loop < SYSTEM_MAX_NUM_CORTEX; loop++) {
                        pARMPerfCopy[loop]        = NULL;
                        armperf_size[loop]        = 0;
                    } /* for (loop = 0; loop < SYSTEM_MAX_NUM_CORTEX; loop++) */
                    for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) {
                        pFlexidag_Info[loop]      = NULL;
                        flexidag_info_size[loop]  = 0;
                    }
                    flexidag_num_active     = 0;
                    pLocal_CavTrace         = malloc(sizeof(cavalry_trace_t));
                    pLocal_AutoRunTrace     = NULL;

                    /* Retrieve flexidag information */
                    loop = 0;
                    while ((loop < flexidag_num_enabled) && (error_exit == 0)) {
                        uint32_t  slot_state;
                        uint32_t  num_sysflow_entries;
                        uint32_t  flexidag_sysflow_daddr;
                        uint32_t  flexidag_name_daddr;

                        slot_state              = pFlexidagTrace->slottrace[loop].slot_state;
                        num_sysflow_entries     = pFlexidagTrace->slottrace[loop].sysflow_name_table_numlines;
                        flexidag_sysflow_daddr  = pFlexidagTrace->slottrace[loop].sysflow_name_table_daddr;
                        flexidag_name_daddr     = pFlexidagTrace->slottrace[loop].flexidag_name_daddr;
                        if ((error_exit == 0) && (slot_state > 0) && (slot_state < 255)) {
                            uint32_t  allocsize;
                            sysflow_name_entry_t *pFlexidagSysflowName;
                            uint8_t  *pFlexidagName;
                            uint8_t   flexidag_name_len;

                            if (flexidag_sysflow_daddr != 0) {
                                pFlexidagSysflowName = ambacv_c2v(flexidag_sysflow_daddr);
                                if (pFlexidagSysflowName == NULL) {
                                    printf("[ERROR] : flexidag_schdr() : FD[%3d] : Trace could not map sysflow_name_table_daddr (0x%08x)\n", loop, flexidag_sysflow_daddr);
                                    error_exit = 1;
                                } /* if (pFlexidagSysflowName == NULL) */
                            } /* if (flexidag_sysflow_daddr != 0) */
                            else { /* if (flexidag_sysflow_daddr == 0) */
                                printf("[ERROR] : flexidag_schdr() : FD[%3d] : Trace has no sysflow_name_table_daddr\n", loop);
                                error_exit = 1;
                            } /* if (flexidag_sysflow_daddr == 0) */

                            if (flexidag_name_daddr != 0) {
                                pFlexidagName     = ambacv_c2v(flexidag_name_daddr);
                                if (pFlexidagName != NULL) {
                                    flexidag_name_len = strlen(pFlexidagName);
                                    if (flexidag_name_len > FLEXIDAG_NAME_MAX_LENGTH) {
                                        flexidag_name_len = FLEXIDAG_NAME_MAX_LENGTH;
                                    } /* if (flexidag_name_len > FLEXIDAG_NAME_MAX_LENGTH) */
                                } else {
                                    printf("[WARN!] : flexidag_schdr() : FD[%3d] : Trace could not map flexidag_name_daddr\n", loop);
                                    pFlexidagName     = NULL;
                                    flexidag_name_len = 0;
                                }
                            } /* if (flexidag_name_daddr != 0) */
                            else { /* if (flexidag_name_daddr == 0) */
                                printf("[WARN!] : flexidag_schdr() : FD[%3d] : Trace has no flexidag_name entry\n", loop);
                                pFlexidagName     = NULL;
                                flexidag_name_len = 0;
                            } /* if (flexidag_name_daddr == 0) */

                            allocsize = sizeof(ambaprof_flexidag_header_t) + (num_sysflow_entries * AMBAPROF_MAX_NAME_LEN);
                            flexidag_info_size[loop]  = allocsize;
                            pFlexidag_Info[loop]      = malloc(allocsize);
                            if (pFlexidag_Info[loop] == NULL) {
                                error_exit = 1;
                                printf("[ERROR] : flexidag_schdr() : Could not allocate flexidag_info for slot %d (size = %d)\n", loop, allocsize);
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
                            } /* if (pFlexidag_Info[loop] != NULL) */
                        } /* if ((slot_state > 0) && (slot_state < 255)) */
                        loop++;
                    } /* while ((loop < flexidag_num_enabled) && (error_exit == 0)) */

                    /* Retrieve cavalry info */
                    cavalry_num_active  = 0;
                    pCavalryTrace   = schdr_get_pCavalryTrace();
                    if (pCavalryTrace != NULL) {
                        memcpy(pLocal_CavTrace, pCavalryTrace, sizeof(cavalry_trace_t));
                        for (loop = 0; loop < CAVALRY_TRACE_MAX_PROCESS; loop++) {
                            if (pLocal_CavTrace->cavalry_info[loop].cavalry_pid != CAVALRY_INVALID_PID) {
                                cavalry_num_active++;
                            } /* if (pLocal_CavTrace->cavalry_info[loop].cavalry_pid != CAVALRY_INVALID_PID) */
                        } /* for (loop = 0; loop < CAVALRY_TRACE_MAX_PROCESS; loop++) */
                    } /* if (pCavalryTrace != NULL) */

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

                    loop = 0;
                    while ((loop < num_visperf_dump) && (error_exit == 0)) {
                        uint32_t perf_daddr;
                        uint32_t perf_size;

                        perf_daddr  = pFlexidagTrace->sysvis_printbuf_perf_daddr[loop];
                        perf_size   = pFlexidagTrace->sysvis_printbuf_perf_size[loop];
                        if ((perf_daddr != 0) && (perf_size != 0)) {
                            uint8_t *pSrcBuf;
                            pSrcBuf = ambacv_c2v(perf_daddr);
                            if (pSrcBuf == NULL) {
                                error_exit = 1;
                                printf("[ERROR] : flexidag_schdr() : FD[%3d] : Trace could not map fdvis_printbuf_perf_daddr (0x%08x)\n", loop, perf_daddr);
                            } /* if (pSrcBuf == NULL) */
                            else { /* if (pSrcBuf != NULL) */
                                pVISPerfCopy[loop] = malloc(perf_size);
                                if (pVISPerfCopy[loop] == NULL) {
                                    error_exit = 1;
                                    printf("[ERROR] : flexidag_schdr() : Could not allocate buffer for VISPERF[%d] (size = %d)\n", loop, perf_size);
                                } /* if (pVISPerfCopy[loop] == NULL) */
                                else { /* if (pVISPerfCopy[loop] != NULL) */
                                    memcpy(pVISPerfCopy[loop], pSrcBuf, perf_size);
                                    visperf_size[loop] = perf_size;
                                } /* if (pVISPerfCopy[loop] != NULL) */
                            } /* if (pSrcBuf != NULL) */
                        } /* if ((perf_daddr != 0) && (perf_size != 0)) */
                        else { /* if ((perf_daddr == 0) || (perf_size == 0)) */
                            printf("[WARN!] : flexidag_schdr() : VISPERF[%d] : dump requested, but no perf buffer exists for this core\n", loop);
                        } /* if ((perf_daddr == 0) || (perf_size == 0)) */
                        loop++;
                    } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */

                    loop = 0;
                    while ((loop < num_armperf_dump) && (error_exit == 0)) {
                        uint32_t perf_daddr;
                        uint32_t perf_size;

                        perf_daddr  = pFlexidagTrace->sysarm_printbuf_perf_daddr[loop];
                        perf_size   = pFlexidagTrace->sysarm_printbuf_perf_size[loop];

                        if ((perf_daddr != 0) && (perf_size != 0)) {
                            uint8_t *pSrcBuf;
                            pSrcBuf = ambacv_c2v(perf_daddr);
                            if (pSrcBuf == NULL) {
                                error_exit = 1;
                                printf("[ERROR] : flexidag_schdr() : FD[%3d] : Trace could not map fdarm_printbuf_perf_daddr (0x%08x)\n", loop, perf_daddr);
                            } /* if (pSrcBuf == NULL) */
                            else { /* if (pSrcBuf != NULL) */
                                pARMPerfCopy[loop] = malloc(perf_size);
                                if (pARMPerfCopy[loop] == NULL) {
                                    error_exit = 1;
                                    printf("[ERROR] : flexidag_schdr() : Could not allocate buffer for ARMPERF[%d] (size = %d)\n", loop, perf_size);
                                } /* if (pARMPerfCopy[loop] == NULL) */
                                else { /* if (pARMPerfCopy[loop] != NULL) */
                                    memcpy(pARMPerfCopy[loop], pSrcBuf, perf_size);
                                    armperf_size[loop] = perf_size;
                                } /* if (pARMPerfCopy[loop] != NULL) */
                            } /* if (pSrcBuf != NULL) */
                        } /* if ((perf_daddr != 0) && (perf_size != 0)) */
                        else { /* if ((perf_daddr == 0) || (perf_size == 0)) */
                            printf("[WARN!] : flexidag_schdr() : ARMPERF[%d] : dump requested, but no perf buffer exists for this core\n", loop);
                        } /* if ((perf_daddr == 0) || (perf_size == 0)) */
                        loop++;
                    } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */

                    if (error_exit == 0) {
                        FILE *outfile;

                        loop = 0;
                        while ((loop < num_visperf_dump) && (error_exit == 0)) {
                            if ((pVISPerfCopy[loop] != NULL) && (visperf_size[loop] != 0)) {
                                outfile = fopen(visperf_filename[loop], "wb");
                                if (outfile != NULL) {
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
                                    fwrite(&prof_header, sizeof(prof_header), 1, outfile);
                                    sectionpos = sizeof(prof_header) + (prof_header.section_count * sizeof(prof_section));
                                    for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                                        if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                                            memset(&prof_section, 0, sizeof(prof_section));
                                            strcpy(&prof_section.name[0], "flexidag names");
                                            prof_section.size = flexidag_info_size[slotloop];
                                            prof_section.base = sectionpos;
                                            sectionpos += prof_section.size;
                                            fwrite(&prof_section, sizeof(prof_section), 1, outfile);
                                        } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    if (cavalry_num_active > 0) {
                                        memset(&prof_section, 0, sizeof(prof_section));
                                        strcpy(&prof_section.name[0], "cavalry names");
                                        prof_section.size = cavalry_num_active * sizeof(ambaprof_cavalry_header_t);
                                        prof_section.base = sectionpos;
                                        sectionpos += prof_section.size;
                                        fwrite(&prof_section, sizeof(prof_section), 1, outfile);
                                    } /* if (cavalry_num_active > 0) */

                                    if (autorun_num_avail > 0) {
                                        memset(&prof_section, 0, sizeof(prof_section));
                                        strcpy(&prof_section.name[0], "autorun names");
                                        prof_section.size = autorun_num_avail * AMBAPROF_MAX_NAME_LEN;
                                        prof_section.base = sectionpos;
                                        sectionpos += prof_section.size;
                                        fwrite(&prof_section, sizeof(prof_section), 1, outfile);
                                    } /* if (autorun_num_avail > 0) */

                                    memset(&prof_section, 0, sizeof(prof_section));
                                    strcpy(&prof_section.name[0], "profile events");
                                    prof_section.size = visperf_size[loop];
                                    prof_section.base = sectionpos;
                                    fwrite(&prof_section, sizeof(prof_section), 1, outfile);
                                    for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                                        if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                                            fwrite(pFlexidag_Info[slotloop], flexidag_info_size[slotloop], 1, outfile);
                                        } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    if (cavalry_num_active > 0) {
                                        uint32_t  subloop;
                                        for (subloop = 0; subloop < CAVALRY_TRACE_MAX_PROCESS; subloop++) {
                                            if (pLocal_CavTrace->cavalry_info[subloop].cavalry_pid != CAVALRY_INVALID_PID) {
                                                ambaprof_cavalry_header_t header;
                                                memset(&header, 0, sizeof(header));
                                                header.cavalry_pid = pLocal_CavTrace->cavalry_info[subloop].cavalry_pid;
                                                strncpy(&header.process_name[0], &pLocal_CavTrace->cavalry_info[subloop].process_name[0], sizeof(header.process_name));
                                                fwrite(&header, sizeof(header), 1, outfile);
                                            } /* if (pLocal_CavTrace->cavalry_info[subloop].cavalry_pid != CAVALRY_INVALID_PID) */
                                        } /* for (subloop = 0; subloop < CAVALRY_TRACE_MAX_PROCESS; subloop++) */
                                    } /* if (cavalry_num_active > 0) */

                                    if (autorun_num_avail > 0) {
                                        uint32_t  subloop;
                                        char      autorun_cvtask_name[AMBAPROF_MAX_NAME_LEN];
                                        for (subloop = 0; subloop < autorun_num_avail; subloop++) {
                                            memset(&autorun_cvtask_name[0], 0, sizeof(autorun_cvtask_name));
                                            strncpy(&autorun_cvtask_name[0], &pLocal_AutoRunTrace[subloop].autorun_cvtask_name[0], sizeof(autorun_cvtask_name));
                                            fwrite(&autorun_cvtask_name[0], 1, sizeof(autorun_cvtask_name), outfile);
                                        } /* for (subloop = 0; subloop < autorun_num_avail; subloop++) */
                                    } /* if (autorun_num_avail > 0) */

                                    fwrite(pVISPerfCopy[loop], visperf_size[loop], 1, outfile);
                                    fclose(outfile);
                                } /* if (outfile != NULL) */
                                else { /* if (outfile == NULL) */
                                    printf("[ERROR] : flexidag_schdr() : VISPERF[%d] : dump requested, but could not create file %s\n", visperf_filename[loop]);
                                } /* if (outfile == NULL) */
                            } /* if ((pVISPerfCopy[loop] != NULL) && (visperf_size[loop] != 0)) */
                            loop++;
                        } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */
                    } /* if (error_exit == 0) */

                    if (error_exit == 0) {
                        FILE *outfile;

                        loop = 0;
                        while ((loop < num_armperf_dump) && (error_exit == 0)) {
                            if ((pARMPerfCopy[loop] != NULL) && (armperf_size[loop] != 0)) {
                                outfile = fopen(armperf_filename[loop], "wb");
                                if (outfile != NULL) {
                                    ambaprof_header_t   prof_header;
                                    ambaprof_section_t  prof_section;
                                    uint32_t  slotloop;
                                    uint32_t  sectionpos;

                                    prof_header.version       = 2;
                                    prof_header.magic_num     = 0xBABEFACE;
                                    prof_header.section_count = 1 + flexidag_num_active;
                                    prof_header.padding       = 0;
                                    fwrite(&prof_header, sizeof(prof_header), 1, outfile);
                                    sectionpos = sizeof(prof_header) + (prof_header.section_count * sizeof(prof_section));
                                    for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                                        if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                                            memset(&prof_section, 0, sizeof(prof_section));
                                            strcpy(&prof_section.name[0], "flexidag names");
                                            prof_section.size = flexidag_info_size[slotloop];
                                            prof_section.base = sectionpos;
                                            sectionpos += prof_section.size;
                                            fwrite(&prof_section, sizeof(prof_section), 1, outfile);
                                        } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    memset(&prof_section, 0, sizeof(prof_section));
                                    strcpy(&prof_section.name[0], "profile events");
                                    prof_section.size = armperf_size[loop];
                                    prof_section.base = sectionpos;
                                    fwrite(&prof_section, sizeof(prof_section), 1, outfile);
                                    for (slotloop = 0; slotloop < flexidag_num_enabled; slotloop++) {
                                        if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) {
                                            fwrite(pFlexidag_Info[slotloop], flexidag_info_size[slotloop], 1, outfile);
                                        } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    } /* if ((pFlexidag_Info[slotloop] != NULL) && (flexidag_info_size[slotloop] != 0)) */
                                    fwrite(pARMPerfCopy[loop], armperf_size[loop], 1, outfile);
                                    fclose(outfile);
                                } /* if (outfile != NULL) */
                                else { /* if (outfile == NULL) */
                                    printf("[ERROR] : flexidag_schdr() : VISPERF[%d] : dump requested, but could not create file %s\n", visperf_filename[loop]);
                                } /* if (outfile == NULL) */
                            } /* if ((pVISPerfCopy[loop] != NULL) && (visperf_size[loop] != 0)) */
                            loop++;
                        } /* while ((loop < num_visperf_dump) && (error_exit == 0)) */
                    } /* if (error_exit == 0) */

                    /* Cleanup */
                    for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) {
                        if (pVISPerfCopy[loop] != NULL) {
                            free(pVISPerfCopy[loop]);
                            pVISPerfCopy[loop] = NULL;
                        } /* if (pVISPerfCopy[loop] != NULL) */
                    } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */
                    for (loop = 0; loop < SYSTEM_MAX_NUM_CORTEX; loop++) {
                        if (pARMPerfCopy[loop] != NULL) {
                            free(pARMPerfCopy[loop]);
                            pARMPerfCopy[loop] = NULL;
                        } /* if (pARMPerfCopy[loop] != NULL) */
                    } /* for (loop = 0; loop < SYSTEM_MAX_NUM_CORTEX; loop++) */
                    for (loop = 0; loop < flexidag_num_enabled; loop++) {
                        if (pFlexidag_Info[loop] != NULL) {
                            free(pFlexidag_Info[loop]);
                            pFlexidag_Info[loop] = NULL;
                        } /* if (pFlexidag_Info[loop] != NULL) */
                    } /* for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) */
                    free(pLocal_CavTrace);
                    if (pLocal_AutoRunTrace != NULL) {
                        free(pLocal_AutoRunTrace);
                    }
                } /* if ((perf_dump != 0) && (error_exit == 0) */

                /*==================================================================================================*/
                /*=- Dump Scheduler Logs if requested -=============================================================*/
                /*==================================================================================================*/
                if ((sched_dump != 0) && (error_exit == 0)) {
                    uint8_t  *pVISSchedCopy[SYSTEM_MAX_NUM_VISORC];
                    uint32_t  vissched_size[SYSTEM_MAX_NUM_VISORC];
                    uint8_t  *pVISSchedStrings;
                    uint32_t  visschedcode_size;
                    uint32_t  loop;

                    for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) {
                        pVISSchedCopy[loop] = NULL;
                        vissched_size[loop] = 0;
                    } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */

                    visschedcode_size = pFlexidagTrace->sysvis_scheduler_code_size;
                    pVISSchedStrings  = ambacv_c2v(pFlexidagTrace->sysvis_scheduler_code_base);
                    if ((visschedcode_size == 0) || (pVISSchedStrings == NULL)) {
                        printf("[ERROR] : flexidag_schdr() : VISSCHED_DUMP : VISORC codebase values not properly set (0x%08x/%p, %9d)\n",
                               pFlexidagTrace->sysvis_scheduler_code_base, pVISSchedStrings, pFlexidagTrace->sysvis_scheduler_code_size);
                        error_exit = 1;
                    } /* if ((visschedcode_size == 0) || (pVISSchedStrings == NULL)) */

                    /* Shadow copy visorc scheduler buffer */
                    loop = 0;
                    while ((loop < num_vissched_dump) && (error_exit == 0)) {
                        uint32_t sched_daddr;
                        uint32_t sched_size;

                        sched_daddr = pFlexidagTrace->sysvis_printbuf_sched_daddr[loop];
                        sched_size  = pFlexidagTrace->sysvis_printbuf_sched_size[loop];

                        if ((sched_daddr != 0) && (sched_size != 0)) {
                            uint8_t *pSrcBuf;
                            pSrcBuf = ambacv_c2v(sched_daddr);
                            if (pSrcBuf == NULL) {
                                printf("[ERROR] : flexidag_schdr() : FD[%3d] : Trace could not map sysvis_printbuf_sched_daddr (0x%08x)\n", loop, sched_daddr);
                                error_exit = 1;
                            } /* if (pSrcBuf == NULL) */
                            else {
                                pVISSchedCopy[loop] = malloc(sched_size);
                                if (pVISSchedCopy[loop] == NULL) {
                                    printf("[ERROR] : flexidag_schdr() : Could not allocate buffer for VISSCHED[%d] (size = %d)\n", loop, sched_size);
                                    error_exit = 1;
                                } /* if (pVISSchedCopy[loop] == NULL) */
                                else { /* if (pVISSchedCopy[loop] != NULL) */
                                    memcpy(pVISSchedCopy[loop], pSrcBuf, sched_size);
                                    vissched_size[loop] = sched_size;
                                } /* if (pVISSchedCopy[loop] != NULL) */
                            } /* if ((sched_daddr != 0) && (sched_size != 0)) */
                            loop++;
                        } else { /* if ((perf_daddr == 0) || (perf_size == 0)) */
                            printf("[WARN!] : flexidag_schdr() : VISSCHED[%d] : dump requested, but no sched buffer exists for this core\n", loop);
                        } /* if ((perf_daddr == 0) || (perf_size == 0)) */
                        loop++;
                    } /* while ((loop < num_vissched_dump) && (error_exit == 0)) */

                    loop = 0;

                    while ((loop < num_vissched_dump) && (error_exit == 0)) {
                        if ((pVISSchedCopy[loop] != NULL) && (vissched_size[loop] != 0)) {
                            FILE *outfile;
                            outfile = fopen(vissched_filename[loop], "wb");
                            if (outfile != NULL) {
                                uint32_t  entry_loop;
                                uint32_t  num_entries;
                                char      tempstring[256];
                                cvlog_sched_entry_t *pEntry;

                                fprintf(outfile, "===================================================================================================\n");
                                fprintf(outfile, "===========================================- VISCHED[%d] -==========================================\n", loop);
                                fprintf(outfile, "===================================================================================================\n");

                                num_entries = vissched_size[loop] / sizeof(cvlog_sched_entry_t);
                                pEntry = (cvlog_sched_entry_t *)pVISSchedCopy[loop];

                                for (entry_loop = 0; entry_loop < num_entries; entry_loop++) {
                                    uint8_t *pEntryString;
                                    uint32_t  offset;
                                    uint32_t  bank;

                                    if (pEntry->entry_string_offset != 0) {
                                        offset  = pEntry->entry_string_offset & 0x1FFFFFFFU;
                                        bank    = pEntry->entry_string_offset >> 29;

                                        if (bank == 0) {
                                            if (offset < visschedcode_size) {
                                                pEntryString = &pVISSchedStrings[offset];
                                            } /* if (offset < visschedcode_size) */
                                            else { /* if (offset >= visschedcode_size) */
                                                pEntryString = NULL;
                                            } /* if (offset >= visschedcode_size) */
                                        } /* if (bank == 0) */
                                        else { /* if (bank != 0)*/
                                            pEntryString = NULL;
                                        } /* if (bank != 0)*/

                                        fprintf(outfile, "VISSCH[%d] : [%10u] : ", loop, pEntry->entry_time);
                                        if (pEntryString != NULL) {
                                            fprintf(outfile, pEntryString, pEntry->entry_arg1, pEntry->entry_arg2, pEntry->entry_arg3, pEntry->entry_arg4, pEntry->entry_arg5);
                                        } /* if (pEntryString != NULL) */
                                        else { /* if (pEntryString == NULL) */
                                            fprintf(outfile, "<string inaccessible> : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", pEntry->entry_string_offset, pEntry->entry_arg1, pEntry->entry_arg2, pEntry->entry_arg3, pEntry->entry_arg4, pEntry->entry_arg5);
                                        } /* if (pEntryString == NULL) */
                                    } /* if (pEntry->entry_string_offset != 0) */
                                    pEntry++;
                                } /* for (entry_loop = 0; entry_loop < num_entries; entry_loop++) */
                                fclose(outfile);
                            } /* if (outfile != NULL) */
                            else { /* if (outfile == NULL) */
                                printf("[ERROR] : flexidag_schdr() : VISSCHED[%d] : dump requested, but could not create file %s\n", loop, vissched_filename[loop]);
                            } /* if (outfile == NULL) */
                        } /* if ((pVISSchedCopy[loop] != NULL) && (vissched_size[loop] != 0)) */
                        loop++;
                    } /* while ((loop < num_vissched_dump) && (error_exit == 0)) */

                    /* Cleanup */
                    for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) {
                        if (pVISSchedCopy[loop] != NULL) {
                            free(pVISSchedCopy[loop]);
                        }
                        pVISSchedCopy[loop] = NULL;
                        vissched_size[loop] = 0;
                    } /* for (loop = 0; loop < SYSTEM_MAX_NUM_VISORC; loop++) */

                } /* if ((sched_dump != 0) && (error_exit == 0)) */
            } /* if (pFlexidagTrace != NULL) */
            else { /* if (pFlexidagTrace == NULL) */
                printf("[flexidag_schdr] : Unable to map flexidag trace address\n");
            } /* if (pFlexidagTrace == NULL) */
        }

        flexidag_schdr_stop();

    } /* if (dump_requested != 0) */

    if (external_msg_thread != 0) {
        cv_msg_loop = 0;
    }
    return 0;

}

