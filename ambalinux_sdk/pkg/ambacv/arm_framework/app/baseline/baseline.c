#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include "schdr_api.h"
#include "schdr_util.h"
#include "cvapi_sink_interface.h"
#include <semaphore.h>

#define DEFAULT_KEY     0xaabbccddeeff
#define DEFAULT_INET    "127.0.0.1"
#define DEFAULT_PORT    8699
#define OPTIONS         "k:s:t:f:b:d:o:i:j:a:e:p:mh"

static int boot_flag = 1;
static int boot_cfg = 0;

static void print_help(const char *exe)
{
	printf("\nusage: %s [args]\n", exe);
	printf("\t-b arg: set memort mode 0[malloc]/1[mmap]/2[sharedmem]\n");
	printf("\t-s arg: pairing server inet address\n");
	printf("\t-k arg: 64bit pairing key\n");
	printf("\t-d arg: set dram size\n");
	printf("\t-t arg: cvtable file name\n");
	printf("\t-f arg: system flow file name\n");
	printf("\t-o arg: number of sub-schedulers, default 1\n");
        printf("\t-i arg: idspfeeder input filename\n");
        printf("\t-j arg: sink output top directory\n");
        printf("\t-a arg: start frame number for idspfeeder\n");
        printf("\t-e arg: end frame number for idspfeeder\n");
        printf("\t-p key:value: define a cvtable entry\n");
        printf("\t-m:     minimalist mode\n");
	printf("\nfor example: %s ", exe);
	printf("-s 10.1.11.16 -k 0x1234567890\n\n");
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

static void add_cvtable(char *path)
{
	static int cvtable_cnt;
	if (++cvtable_cnt > MAX_CVTABLE_COUNT) {
		printf("Too many cvtables! limit is %d\n", MAX_CVTABLE_COUNT);
		exit(-1);
	}
	schdr_add_cvtable((void*)path, load_binary);
}

static void add_sysflow(char *path)
{
	static int sysflow_cnt;
	if (++sysflow_cnt > MAX_SYSFLOW_COUNT) {
		printf("Too many sysflow! limit is %d\n", MAX_SYSFLOW_COUNT);
		exit(-1);
	}
	schdr_add_sysflow((void*)path, load_binary);
}

static void parse_amalgam_opt(int argc, char**argv)
{
	int c, port = DEFAULT_PORT;
	char *inet = DEFAULT_INET;
	long long unsigned int key = DEFAULT_KEY;
	unsigned int dram_size = 0x20000000;

	optind = 1;
	while ((c = getopt(argc, argv, OPTIONS)) != -1) {
		switch (c) {
		case 'b':
			schdr_set_dram_mode(SCHDR_DRAM_MODE_SHM, optarg);
			break;
		case 'k':
			key = strtoull(optarg, NULL, 0);
			break;
		case 's':
			inet = optarg;
			break;
		case 'd':
			dram_size = strtoul(optarg, NULL, 0);
			break;
		case 'o':
			// This is not an amalgam option.
			// However, boot_flag is needed for schdr_init.
			boot_flag = strtol(optarg, NULL, 0);
			break;
                case 'm':
                        boot_cfg = SCHDR_DISABLE_CVTABLE_LOG | SCHDR_DISABLE_SS_LOG;
                        break;
		default:
			break;
		}
	}
	amalgam_config(inet, port, key, dram_size);
}

static void parse_schdr_opt(int argc, char **argv)
{
	int c;

	optind = 1;
	while ((c = getopt(argc, argv, OPTIONS)) != -1) {
		switch (c) {
		case 'b':
		case 'k':
		case 's':
		case 'd':
		case 'o':
                case 'm':
			break;
		case 't':
			add_cvtable(optarg);
			break;
		case 'f':
			add_sysflow(optarg);
			break;
		case 'i':
			schdr_insert_cvtable_entry(
				"IdspFeeder0_input",
				(const void*)optarg,
				strlen(optarg)+1);
			break;
		case 'j':
			schdr_insert_cvtable_entry(
				SINK_LABEL_TOPDIR,
				(const void*)optarg,
				strlen(optarg)+1);
			break;
		case 'a':
			schdr_insert_cvtable_entry(
				"IdspFeeder0_frame_start",
				(const void*)optarg,
				strlen(optarg)+1);
			schdr_insert_cvtable_entry(
				SINK_LABEL_FRAME_BASE,
				(const void*)optarg,
				strlen(optarg)+1);
			break;
		case 'e':
			schdr_insert_cvtable_entry(
				"IdspFeeder0_frame_end",
				(const void*)optarg,
				strlen(optarg)+1);
			break;
                case 'p':
                {
                        char key[128], *value;
                        strcpy(key, optarg);
                        printf("%s\n", key);
                        value = strchr(key, ':');
                        if (value == NULL) {
                                printf("please use key:value format for -p\n");
                                exit(1);
                        }
                        *value++ = 0;
                        schdr_insert_cvtable_entry(
                                key,
                                (const void*)value,
                                strlen(value)+1);
                }
                break;
		default:
			print_help(argv[0]);
			exit(-1);
		}
	}
}

static sem_t done_semaphore;
static sem_t *pDoneSemaphore = NULL;

static int app_callback(int type, void *arg)
{
	int i, ret = 0;

	switch(type) {
	case SCHDR_CB_START_REGISTRATION:
		printf("====BASELINE starts cvtask registration\n");
		break;
	case SCHDR_CB_START_QUERY:
		printf("====BASELINE starts cvtask query\n");
		break;
	case SCHDR_CB_START_INIT:
		printf("====BASELINE is about to init cvtasks\n");
		break;
	case SCHDR_CB_START_RUN:
		printf("====BASELINE is about to run cvtasks\n");
		break;
	case SCHDR_CB_START_SHUTDOWN:
		printf("====SUPERDAG is shutting down\n");
		if (pDoneSemaphore != NULL)
		{
			sem_post(pDoneSemaphore);
		}
		break;
	default:
		printf("Warning: skip unknow callback type %d\n", type);
		ret = -1;
	}
	return ret;
}

int main(int argc, char **argv)
{
	int      spt_port, hlt_port;
	extern void fusion_init(int *, int*);

	schdr_prefetch(0);

	sem_init(&done_semaphore, 0, 0);
	pDoneSemaphore = &done_semaphore;

	/* parse opt to config amalgam before we init scheduler */
	parse_amalgam_opt(argc, argv);

	/* init scheduler */
	schdr_set_callback(app_callback);
	schdr_init(0xD, boot_cfg, boot_flag);
	schdr_insert_cvtable_entry("TEST_LABLE0", "hello0", 7);
	schdr_insert_cvtable_entry("TEST_LABLE1", "hello1", 7);
	schdr_insert_cvtable_entry("TEST_LABLE", "hello", 6);

	/* parse the reset of scheduler to add tbar and sysflow */
	parse_schdr_opt(argc, argv);

	/* start the scheduler */
	schdr_start();

	/* main thread got nothing to do , just put it to sleep */
	sem_wait(&done_semaphore);

	return 0;
}
