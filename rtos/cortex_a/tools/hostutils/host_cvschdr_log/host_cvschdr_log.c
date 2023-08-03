/*******************************************************************************
 * host_cvschdr_log.c
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
#include <sys/ioctl.h>
#include "host_cvschdr_log.h"


#define FILENAME_LENGTH		(256)

static char orcvp_filename[FILENAME_LENGTH];
static char schdrlog_filename[FILENAME_LENGTH];
static char vissched_filename[FILENAME_LENGTH];

static struct option long_options[] =
{
    {"core",	1, 0, 'c'},
    {"binary",	1, 0, 'b'},
    {"output",	1, 0, 'o'},
    {0, 0, 0, 0},
};

static const char *short_options = "c:b:o:";

struct hint_s {
    const char *arg;
    const char *str;
};

static const struct hint_s hint[] = {
    {"filename",  "Input orcvp.bin to dump full visorc scheduler log."},
    {"filename",  "Input scheduler log buffer to dump full visorc scheduler log."},
    {"filename",  "Dump full visorc scheduler log to output."},
};

void usage(void)
{
    int i;

    printf("host_cvschdr_log usage:\n");
    for(i = 0; i < sizeof(long_options) / sizeof(long_options[0]) - 1; i++){
        if (isalpha(long_options[i].val))
            printf("-%c ", long_options[i].val);
        else
            printf("   ");
        printf("--%s", long_options[i].name);
        if (hint[i].arg[0] != 0)
            printf(" [%s]", hint[i].arg);
        printf("\t%s\n", hint[i].str);
    }
}

int init_param(int argc, char **argv)
{
    int ch;
    int option_index = 0;
    int value;

    opterr = 0;
    while((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1){
        switch(ch){
        case 'c':
            value = strlen(optarg);
            if(value >= FILENAME_LENGTH){
                printf("[ERROR] core filename[%s] is too long [%d] (>%d)\n", optarg, value, FILENAME_LENGTH);
                return -1;
            } else {
                strncpy(&orcvp_filename[0], optarg, value);
            }
            break;

        case 'b':
            value = strlen(optarg);
            if(value >= FILENAME_LENGTH){
                printf("[ERROR] binary filename[%s] is too long [%d] (>%d)\n", optarg, value, FILENAME_LENGTH);
                return -1;
            } else {
                strncpy(&schdrlog_filename[0], optarg, value);
            }
            break;

        case 'o':
            value = strlen(optarg);
            if(value >= FILENAME_LENGTH){
                printf("[ERROR] output filename[%s] is too long [%d] (>%d)\n", optarg, value, FILENAME_LENGTH);
                return -1;
            } else {
                strncpy(&vissched_filename[0], optarg, value);
            }
            break;

        default:
            printf("unknown option found: %c\n", ch);
            return -1;
            break;
        }
    }

    return 0;
}

static void sigstop()
{
    exit(0);
}

uint32_t main(int argc, char **argv)
{
    uint32_t rval = 0;
    uint8_t  *pVISSchedCopy = NULL;
    uint32_t  vissched_size;
    uint8_t  *pVISSchedStrings = NULL;
    uint32_t  visschedcode_size;
    FILE *orcvpfile, *schdrlogfile, *outfile;

    signal(SIGINT, sigstop);
    signal(SIGQUIT, sigstop);
    signal(SIGTERM, sigstop);

    if(argc < 2){
        rval = 1;
     } else if(init_param(argc, argv) < 0){
        rval = 1;
    }

    /*==================================================================================================*/
    /*=- Dump Scheduler Logs              -=============================================================*/
    /*==================================================================================================*/

    if(rval == 0){
        orcvpfile = fopen(orcvp_filename, "r+");
        if(orcvpfile != NULL){
            fseek(orcvpfile, 0, SEEK_END);
            visschedcode_size = ftell(orcvpfile);
            fseek(orcvpfile, 0, SEEK_SET);
            pVISSchedStrings = malloc(visschedcode_size);
            fread(pVISSchedStrings, visschedcode_size, 1U, orcvpfile);
            fclose(orcvpfile);
            if((visschedcode_size == 0) || (pVISSchedStrings == NULL)){
                printf("[ERROR] VISSCHED: VISORC codebase not properly set(%p, %9d)\n", pVISSchedStrings, visschedcode_size);
                rval = 2;
            }
        } else {
            printf("[ERROR] VISSCHED: Could not open core file[%s]\n", orcvp_filename);
            rval = 1;
        }
    }

    /* Shadow copy visorc scheduler buffer */
    if(rval == 0){
        schdrlogfile = fopen(schdrlog_filename, "r+");
        if(schdrlogfile != NULL){
            fseek(schdrlogfile, 0, SEEK_END);
            vissched_size = ftell(schdrlogfile);
            fseek(schdrlogfile, 0, SEEK_SET);
            pVISSchedCopy = malloc(vissched_size);
            fread(pVISSchedCopy, vissched_size, 1U, schdrlogfile);
            fclose(schdrlogfile);
            if((vissched_size == 0) || (pVISSchedCopy == NULL)){
                printf("[ERROR] VISSCHED: VISORC log buffer not properly set(%p, %9d)\n", pVISSchedCopy, vissched_size);
                rval = 2;
            }
        } else {
            printf("[ERROR] VISSCHED: Could not open binary file[%s]\n", schdrlog_filename);
            rval = 1;
        }
    }

    if(rval == 0){
        outfile = fopen(vissched_filename, "wb");
        if(outfile != NULL){
            uint32_t  entry_loop;
            uint32_t  num_entries;
            char      tempstring[256];
            cvlog_sched_entry_t *pEntry;

            num_entries = vissched_size / sizeof(cvlog_sched_entry_t);
            pEntry = (cvlog_sched_entry_t *)pVISSchedCopy;

            for(entry_loop = 0; entry_loop < num_entries; entry_loop++){
                uint8_t *pEntryString;
                uint32_t  offset;
                uint32_t  bank;

                if(pEntry->entry_string_offset != 0){
                    offset  = pEntry->entry_string_offset & 0x1FFFFFFFU;
                    bank    = pEntry->entry_string_offset >> 29;

                    if(bank == 0){
                        if(offset < visschedcode_size){
                            pEntryString = &pVISSchedStrings[offset];
                        } else {
                            pEntryString = NULL;
                        }
                    } else {
                        pEntryString = NULL;
                    }

                    fprintf(outfile, "[%10u][SCH:VP:TH%d] ", pEntry->entry_time, pEntry->entry_flags.src_thread);
                    if(pEntryString != NULL){
                        fprintf(outfile, pEntryString, pEntry->entry_arg1, pEntry->entry_arg2, pEntry->entry_arg3, pEntry->entry_arg4, pEntry->entry_arg5);
                    } else {
                        fprintf(outfile, "<string inaccessible> : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", pEntry->entry_string_offset, pEntry->entry_arg1, pEntry->entry_arg2, pEntry->entry_arg3, pEntry->entry_arg4, pEntry->entry_arg5);
                    }
                }
                pEntry++;
            }
            fclose(outfile);
        } else {
            printf("[ERROR] VISSCHED: Could not create output file[%s]\n", vissched_filename);
            rval = 1;
        }
    }

    /* Cleanup */
    if(pVISSchedStrings != NULL){
        free(pVISSchedStrings);
    }
    if(pVISSchedCopy != NULL){
        free(pVISSchedCopy);
    }

    if(rval == 1){
        usage();
    }

    return rval;
}

