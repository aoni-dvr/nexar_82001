#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>
#include <execinfo.h> /* for backtrace() */

#include "cvtask_ossrv.h"
#include "cvapi_ambacv_flexidag.h"
#include "cam_util.h"
#include "mem_util.h"
#include "dag_util.h"
#include "dag_impl.h"
#include "postprocess_impl.h"
#include "osdutil.h"
#include "preference.h"

#include "NMS.h"


typedef struct {
    /* save input frame */
    uint32_t save_flag;
    uint32_t frame_cnt;

    /* memory area for mmap */
    uint8_t enable_cache;
} PROGRAM_PARAMS_s;
static PROGRAM_PARAMS_s g_params = {0};

/*
 * Command line arguments.
 */
#define OPTSTRING "f:c:o:s:l:r:t:i:"
static struct option longopts[] =
{
    { "fin_0",     required_argument,  NULL, 'f', },
    { "cache",     required_argument,  NULL, 'c', },
    { "osdtype",   required_argument,  NULL, 'o', },
    { "osdwin",    required_argument,  NULL, 'w', },
    { "saveinput", required_argument,  NULL, 's', },
    { "layer",     required_argument,  NULL, 'l', },
    { "dagroi",    required_argument,  NULL, 'r', },
    { "dagtype",   required_argument,  NULL, 't', },
    { "inputwin",  required_argument,  NULL, 'i', },
    { NULL, 0, NULL, 0, },
};

/**
 * disply usage of parameters
 **/
static void ShowHelp(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t--saveinput [input frame N0.]\t Save No.# input frame.\n");
    printf("\t--cache [enable]\t\t Set map as cached(1)/noncached(0) area in mmap operation.\n");
    printf("\t--osdtype [id] [type]\t\t Set osd type. type : 8-bit(8)/32-bit(32)\n");
    printf("\t--osdwin [id] [width] [height]\t Set osd window.\n");
    printf("\t--layer [id] [layer]\t\t Set layer of input pyramid for FlexiDAG.\n");
    printf("\t--dagtype [id] [type]\t\t Set dag input type. type : PicInfo(0)/RawBuffer(1)\n");
    printf("\t--dagroi [id] [start_col] [start_row] [width] [height]\t Set reference roi for FlexiDAG.\n");
    printf("\t--inputwin [id] [width] [height] [pitch]\t Set input window size.\n");
    printf("\t--fin_0 [frameId] [file name] [output type]\t Enable file input mode. output type : OSD(0)/FILE_DAG(1)/FILE_NMS(1)\n");
    printf("Default is to enable flexidag_io ID0 handle flow only! (single vin)\n");
    printf("It will be forced as file input mode if --fn_0 specified.\n");
}

/**
 * set setting for saving input frame
 **/
static void SetSaveFrame(int frame_cnt)
{
    g_params.save_flag = 1;
    g_params.frame_cnt = frame_cnt;
}

/**
 * parse program parameters
 **/
static void ParseOpt(int argc, char **argv)
{
    int i;
    static int argcnt = 1;
    while ((i = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (i) {
        case 'f':
        {
            argcnt += 4;
            if (argcnt <= argc) {
                uint32_t SrcInputType, DstOutputType;
                FILE_INPUT_INFO_s FileInfo = {0};
                //default is to enable flexidag_io ID0 handle flow only
                FileInfo.FrameId = strtoul(optarg, NULL, 0);
                FileInfo.FileName = argv[optind];
                Preference_SetFileInputInfo(0, &FileInfo);
                SrcInputType = SRC_INPUT_TYPE_FILE;
                Preference_SetSrcInputType(0, SrcInputType);
                DstOutputType = strtoul(argv[optind+1], NULL, 0);
                if (DstOutputType < DST_OUTPUT_TYPE_NUM) {
                    Preference_SetDstOutputType(0, DstOutputType);
                    printf("Enable file-in mode, output type = %u \n", DstOutputType);
                } else {
                    printf("Invalid output type %u!\n", DstOutputType);
                    exit(1);
                }
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 'o':
        {
            argcnt += 3;
            if (argcnt <= argc) {
                int Id = strtoul(optarg, NULL, 0);
                int OsdType = strtoul(argv[optind], NULL, 0);
                if ((OsdType != OSD_TYPE_8_BIT) && (OsdType != OSD_TYPE_32_BIT)) {
                    printf("Only support 8-bit/32-bit OSD type! \n", OsdType);
                    exit(1);
                } else {
                    OSD_INFO_s OsdInfo = {0};
                    Preference_GetOSDInfo(Id, &OsdInfo);
                    OsdInfo.OsdType = OsdType;
                    Preference_SetOSDInfo(Id, &OsdInfo);
                    printf("OSD Type : %d-bit \n", OsdType);
                }
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 'w':
        {
            argcnt += 4;
            if (argcnt <= argc) {
                int Id = strtoul(optarg, NULL, 0);
                OSD_INFO_s OsdInfo = {0};
                Preference_GetOSDInfo(Id, &OsdInfo);
                OsdInfo.OsdWin.Width = strtoul(argv[optind], NULL, 0);
                OsdInfo.OsdWin.Height = strtoul(argv[optind+1], NULL, 0);
                Preference_SetOSDInfo(Id, &OsdInfo);
                printf("OSD Window (W/H) : ( %d / %d ) \n", OsdInfo.OsdWin.Width, OsdInfo.OsdWin.Height);
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 'c':
        {
            argcnt += 2;
            if (argcnt <= argc) {
                g_params.enable_cache = atoi(optarg);
                printf("Cache operation is %s\n", g_params.enable_cache? "ON":"OFF");
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 's':
        {
            argcnt += 2;
            if (argcnt <= argc) {
                int FrameCount = strtoul(optarg, NULL, 0);
                if (FrameCount <= 0) {
                    printf("Invalid frame count = %d", FrameCount);
                    exit(1);
                } else {
                    SetSaveFrame(FrameCount);
                    printf("Save input frames %d\n", FrameCount);
                }
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 'l':
        {
            argcnt += 3;
            if (argcnt <= argc) {
                int Id = strtoul(optarg, NULL, 0);
                int Layer = strtoul(argv[optind], NULL, 0);
                if ((Layer < 0) && (Layer > 5)) {
                    printf("Layer should be between 0 to 5!\n");
                    exit(1);
                } else {
                    amba_roi_config_t RoiCfg = {0};
                    Preference_GetDAGRoiCfg(Id, &RoiCfg);
                    RoiCfg.image_pyramid_index = Layer;
                    Preference_SetDAGRoiCfg(Id, &RoiCfg);
                    printf("Set pyramid layer as %d for id %d\n", Layer, Id);
                }
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 'r':
        {
            argcnt += 6;
            if (argcnt <= argc) {
                int Id = strtoul(optarg, NULL, 0);
                amba_roi_config_t RoiCfg = {0};
                Preference_GetDAGRoiCfg(Id, &RoiCfg);
                RoiCfg.roi_start_col = strtoul(argv[optind], NULL, 0);
                RoiCfg.roi_start_row = strtoul(argv[optind+1], NULL, 0);
                RoiCfg.roi_width = strtoul(argv[optind+2], NULL, 0);
                RoiCfg.roi_height = strtoul(argv[optind+3], NULL, 0);
                Preference_SetDAGRoiCfg(Id, &RoiCfg);
                printf("Set roi of input pyramid (start_col/start_row/width/height) = (%d/%d/%d/%d) for id %d\n", RoiCfg.roi_start_col, RoiCfg.roi_start_row, RoiCfg.roi_width, RoiCfg.roi_height , Id);
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 't':
        {
            argcnt += 3;
            if (argcnt <= argc) {
                int Id = strtoul(optarg, NULL, 0);
                uint32_t DagInputType = strtoul(argv[optind], NULL, 0);
                if (DagInputType < DAG_INPUT_TYPE_NUM) {
                    Preference_SetDagInputType(Id, DagInputType);
                    printf("Set Dag input type as %s for id %d\n", DagInputType? "Raw buffer mode" : "Pic info mode", Id);
                } else {
                    printf("invalid dag input type %u!\n", DagInputType);
                    exit(1);
                }
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        case 'i':
        {
            argcnt += 5;
            if (argcnt <= argc) {
                int Id = strtoul(optarg, NULL, 0);
                WINDOW_INFO_s InputInfo = {0};
                InputInfo.Width = strtoul(argv[optind], NULL, 0);
                InputInfo.Height = strtoul(argv[optind+1], NULL, 0);
                InputInfo.Pitch = strtoul(argv[optind+2], NULL, 0);
                Preference_SetInputInfo(Id, &InputInfo);
                printf("Set input window size (Width/Height/Pitch) = (%d/%d/%d) for id %d\n", InputInfo.Width, InputInfo.Height, InputInfo.Pitch, Id);
                break;
            } else {
                printf("Error!! Please check the paramenters!\n");
                exit(1);
            }
        }
        default:
            ShowHelp(argv[0]);
            exit(1);
        }
    }
}

/**
 * Relase program resource
 **/
static void ReleaseAPP(void)
{
    int id;
    for (id = 0; id < MAX_FLOW_AMOUNT; id++) {
        CamUtil_Release(id);
        DagFlow_Release(id);
        PostProcess_Release(id);
        if (g_params.save_flag == 1) {
            MemUtil_Release();
        }
    }
}

/**
 * aexit() callback
 * This function will be invoked when program exits.
 **/
static void CleanUp(void)
{
    printf("program exit!\n");
    ReleaseAPP();
    printf("Program built at %s - %s\n", __DATE__, __TIME__);
}

/**
 * signal() callback
 * This function will be invoked when got related signal.
 **/
static void SignalHandler(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(1);
}

static void SEGVhandler(int signo)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "%s: Got segmentation fault (%d)\n", __FILE__, signo);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

/**
 * init program resource.
 * This function will hook system callback, parse command-line parameters and init resource.
 **/
static uint32_t InitAPP(int argc, char **argv)
{
    uint32_t ret = 0;

    ParseOpt(argc,argv);

    atexit(CleanUp);

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, SignalHandler);
    signal(SIGHUP, SignalHandler);
    signal(SIGUSR1, SignalHandler);
    signal(SIGQUIT, SignalHandler);
    signal(SIGINT, SignalHandler);
    signal(SIGKILL, SignalHandler);
    signal(SIGSEGV, SEGVhandler);

    return ret;
}

/**
 * init CV framework.
 * This function will init CV framework.
 **/
static uint32_t InitCVFlow(void)
{
    uint32_t ret = 0;
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

    /* init scheduler */
    cfg.cpu_map = 0x9;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    return ret;
}

/**
 * Save input frame
 **/
static uint32_t SaveFrame(uint32_t id, memio_source_recv_picinfo_t *PicInfo)
{
    uint32_t ret = 0;
    static uint32_t cnt[MAX_FLOW_AMOUNT] = {0};

    if (cnt[id] != 0XFFFFFFFF) {
        cnt[id]++;
        if (cnt[id] == g_params.frame_cnt) {
            FILE *fptr;
            char fn_out[128];
            uint32_t width, pitch, height, length, addr;
            uint8_t *data;
            amba_roi_config_t RoiCfg = {0};
            uint32_t layer;

            Preference_GetDAGRoiCfg(id, &RoiCfg);
            layer = RoiCfg.image_pyramid_index;
            if (PicInfo->pic_info.pyramid.half_octave[layer].ctrl.disable == 0) {
                pitch = PicInfo->pic_info.pyramid.half_octave[layer].ctrl.roi_pitch;
                width = PicInfo->pic_info.pyramid.half_octave[layer].roi_width_m1 + 1;
                height = PicInfo->pic_info.pyramid.half_octave[layer].roi_height_m1 + 1;
                length = pitch * height;

                addr = PicInfo->pic_info.rpLumaLeft[layer];
                if (addr == 0) {
                    addr = PicInfo->pic_info.rpLumaRight[layer];
                    if (addr == 0) {
                        ret = 1;
                        printf("[Error] There is no valid Luma in pyramid!\n");
                    }
                }

                if (ret == 0) {
                    printf("SAVE(%u):Y[%u] (%ux%u, p:%u) addr: 0x%08x\n", cnt[id], layer, width, height, pitch, addr);
                    MemUtil_CacheInvalidPhy(addr,length);
                    data = (uint8_t *)MemUtil_P2V(addr);

                    sprintf(fn_out, "./frame%u_%ux%u_L%u.y", cnt[id], pitch, height, layer);
                    fptr = fopen(fn_out, "wb");
                    if (fptr != NULL) {
                        fwrite(data, 1, length, fptr);
                        fclose(fptr);
                    } else {
                        ret = 1;
                        printf("[Error] write failed! fptr is NULL!\n");
                    }
                }

                addr = PicInfo->pic_info.rpChromaLeft[layer];
                if (addr == 0) {
                    addr = PicInfo->pic_info.rpChromaRight[layer];
                    if (addr == 0) {
                        ret = 1;
                        printf("[Error] There is no valid Chroma in pyramid!\n");
                    }
                }

                if (ret == 0) {
                    printf("SAVE(%u):UV[%u] (%ux%u, p:%u) addr: 0x%08x\n", cnt[id], layer, width, height, pitch, addr);
                    MemUtil_CacheInvalidPhy(addr,length);
                    data = (uint8_t *)MemUtil_P2V(addr);

                    sprintf(fn_out, "./frame%u_%ux%u_L%u.uv", cnt[id], pitch, height, layer);
                    fptr = fopen(fn_out, "wb");
                    if (fptr != NULL) {
                        fwrite(data, 1, length/2, fptr);
                        fclose(fptr);
                    } else {
                        ret = 1;
                        printf("[Error] write failed! fptr is NULL!\n");
                    }
                }
                cnt[id] = 0XFFFFFFFF;
            } else {
                ret = 1;
                printf("[Error] LAYER %d of pyramid is disable!\n", layer);
            }
        }
    }
    return ret;
}

void *Thread_0(void *data)
{
    uint32_t retcode, id = 0;
    char *path;
    AMBA_CV_FLEXIDAG_IO_s *Resultl;
    memio_source_recv_picinfo_t RecvData;
    if (data != NULL) {
        path = (char *)data;
    } else {
        Preference_GetFlexidagPath(id, &path);
    }


    /* init DagFlow. This will init your flow implementation. */
    retcode = DagFlow_Init(id, path);
    if (retcode != 0) {
        printf("[ID%u] DagFlow_Init fail!\n", id);
        return NULL;
    }

    /* init PostProcess. This will init and create the resouce for post-process */
    retcode = PostProcess_Init(id, path);
    if (retcode != 0) {
        printf("[ID%u] PostProcess_Init fail!\n", id);
        return NULL;
    }

    /* Init CamUtil. RTOS will be signaled for frame feeding during this operation */
    retcode = CamUtil_Init(id);
    if (retcode != 0) {
        printf("[ID%u] CamUtil_Init fail!\n", id);
        return NULL;
    }
    if (g_params.save_flag == 1) {
        /* Init mem region */
        retcode = MemUtil_Init(g_params.enable_cache);
        if(retcode != 0) {
            printf("[ID%u] MemUtil_Init fail!(%u)\n", id, retcode);
            return NULL;
        }
    }

    for(;;) {
        /* retrieve Frame */
        retcode = CamUtil_GetFrame(id, &RecvData);
        if (retcode != 0) {
            fprintf(stderr, "[ID%u] CamUtil_GetFrame fail!\n", id);
            break;
        }


        /* save input frame */
        if (g_params.save_flag == 1) {
            retcode = SaveFrame(id, &RecvData);
            if (retcode != 0) {
                fprintf(stderr, "[ID%u] SaveFrame fail!\n", id);
                break;
            }
        }

        /* Feed into flexidag(s) and get result */
        retcode = DagFlow_Process(id, &RecvData, &Resultl);
        if (retcode != 0) {
            printf("[ID%u] DagFlow_Process fail!\n",id);
            break;
        }

        /* do post-process */
        retcode = PostProcess_Impl(id, &RecvData, Resultl);
        if (retcode != 0) {
            printf("[ID%u] PostProcess_Impl fail!\n",id);
            break;
        }
    }

    fprintf(stderr, "[ID%u] Thread exit!(%u)\n", id, retcode);
    pthread_exit(NULL);
}

static uint32_t PackPicInfo(uint32_t id, memio_source_recv_picinfo_t *outPicInfo)
{
    cv_pic_info_t *pic_info;
    idsp_pyramid_t *pyramid;
    half_octave_info_t *half_octave;
    uint32_t i;
    FILE_INPUT_INFO_s file_input = {0};
    WINDOW_INFO_s InputInfo = {0};
    Preference_GetFileInputInfo(id, &file_input);
    Preference_GetInputInfo(id, &InputInfo);

    pic_info = &(outPicInfo->pic_info);
    memset(pic_info, 0, sizeof(pic_info));
    pic_info->frame_num = file_input.FrameId;
    pic_info->capture_time = file_input.FrameId;

    /* fake all pyramid layer as the same setting */
    pyramid = &(pic_info->pyramid);
    pyramid->image_width_m1 = InputInfo.Width - 1;
    pyramid->image_height_m1 = InputInfo.Height - 1;
    pyramid->image_pitch_m1 = InputInfo.Pitch - 1;
    for (i=0; i<MAX_HALF_OCTAVES; i++) {
        half_octave = &(pyramid->half_octave[i]);
        half_octave->ctrl.roi_pitch = InputInfo.Pitch;
        half_octave->roi_start_col = 0;
        half_octave->roi_start_row = 0;
        half_octave->roi_width_m1 = InputInfo.Width - 1;
        half_octave->roi_height_m1 = InputInfo.Height - 1;

        pic_info->rpLumaLeft[i] = file_input.Buffer.buffer_daddr; //physical address
        pic_info->rpChromaLeft[i] = pic_info->rpLumaLeft[i] + (InputInfo.Pitch * InputInfo.Height);
        pic_info->rpLumaRight[i] = pic_info->rpLumaLeft[i];
        pic_info->rpChromaRight[i] = pic_info->rpChromaLeft[i];
    }

#if 0
    if (g_params.save_flag == 1) { //save input content for checking
        char fn[128];
        FILE *out;
        uint8_t *data;

        sprintf(fn,"./input_%u_%ux%u.y", id, pyramid->image_pitch_m1+1, pyramid->image_height_m1+1);
        out = fopen(fn, "wb");
        if (out != NULL) {
            data = (uint8_t *)ambacv_p2v(pic_info->rpLumaLeft[0]);
            fwrite(data, 1, ((pyramid->image_pitch_m1+1) * (pyramid->image_height_m1+1)), out);
            fclose(out);
        }

        sprintf(fn,"./input_%u_%ux%u.uv", id, pyramid->image_pitch_m1+1, pyramid->image_height_m1+1);
        out = fopen(fn, "wb");
        if (out != NULL) {
            data = (uint8_t *)ambacv_p2v(pic_info->rpChromaLeft[0]);
            fwrite(data, 1, ((pyramid->image_pitch_m1+1) * (pyramid->image_height_m1+1))/2, out);
            fclose(out);
        }
    }
#endif

    return 0;
}

static uint32_t GetFramebyFile(uint32_t id, memio_source_recv_picinfo_t *outPicInfo)
{
    uint32_t retcode;
    FILE *fptr;
    uint32_t file_size, frame_size;
    uint8_t *pframebuf = NULL;
    FILE_INPUT_INFO_s file_input = {0};
    WINDOW_INFO_s InputInfo = {0};
    Preference_GetFileInputInfo(id, &file_input);
    Preference_GetInputInfo(id, &InputInfo);
    if (file_input.FileName == NULL) {
        printf("GetFramebyFile[%u]: invalid Filename!\n",id);
        return 1;
    }
    if (outPicInfo == NULL) {
        printf("GetFramebyFile[%u]: invalid outPicInfo!\n",id);
        return 1;
    }

    /* create frame buffer for yuv420 frame */
    frame_size = (InputInfo.Pitch * InputInfo.Height) * 3 / 2;
    retcode = MemUtil_MemblkAlloc(CACHE_ALIGN(frame_size), &file_input.Buffer);
    if(retcode != 0) {
        printf("Fail to create framebuf[%u] buffer!! ret=%u\n", id, retcode);
        return 2;
    }
    pframebuf = file_input.Buffer.pBuffer;

    /* read input file */
    do {
        fptr = fopen(file_input.FileName, "rb");
        if (fptr == NULL) {
            printf("GetFramebyFile[%u]: fail to open file[%s]!\n", id, file_input.FileName);
            retcode = 3;
            break;
        }
        //get file size
        fseek(fptr, 0, SEEK_END);
        file_size = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);

        //file has to be in yuv420 format
        if (file_size != frame_size) {
            printf("GetFramebyFile[%u]: file_size(%u) is not as expected(%u).\n", id, file_size, frame_size);
            printf("Please check your input file. It should be %ux%u in yuv420(nv12) format.\n", InputInfo.Pitch, InputInfo.Height);
            printf("(width is %u with extra 64 padding pixels.)\n", InputInfo.Width);
            retcode = 3;
            break;
        }

        //read
        retcode = fread(pframebuf, 1, file_size, fptr);
        if (retcode != file_size) {
            printf("GetFramebyFile[%u]: fail to read file! got %u, expect %u.\n", id, retcode, file_size);
            retcode = 3;
            break;
        }

    //cache_clean
    MemUtil_MemblkClean(&file_input.Buffer);

        retcode = 0;
    } while(0);
    if (fptr != NULL) {
        fclose(fptr);
    }

    /* wrap input as memio_source_recv_picinfo_t */
    if (retcode == 0) {
        Preference_SetFileInputInfo(0,&file_input);
        retcode = PackPicInfo(id, outPicInfo);
    }

    return retcode;
}

void *Thread_Fin_0(void *data)
{
    uint32_t retcode = 0, id = 0;
    char *path;
    AMBA_CV_FLEXIDAG_IO_s *Resultl;
    memio_source_recv_picinfo_t RecvData;
    uint32_t DagInputType, DstOutputType;
    FILE_INPUT_INFO_s file_input = {0};
    Preference_GetFileInputInfo(id, &file_input);

    if (file_input.FileName == NULL) {
        printf("invalid input filename[%u]! please specify it in command parameter\n", id);
        return NULL;
    }

    if (data != NULL) {
        path = (char *)data;
    } else {
        Preference_GetFlexidagPath(id, &path);
    }

    /* init DagFlow. This will init your flow implementation. */
    retcode = DagFlow_Init(id, path);
    if (retcode != 0) {
        printf("[ID%u] DagFlow_Init fail!\n", id);
        return NULL;
    }

    /* init PostProcess. This will init and create the resouce for post-process */
    retcode = PostProcess_Init(id, path);
    if (retcode != 0) {
        printf("[ID%u] PostProcess_Init fail!\n", id);
        return NULL;
    }
    do {
        /* retrieve Frame */
        retcode = GetFramebyFile(id, &RecvData);
        if (retcode != 0) {
            printf("[ID%u] GetFramebyFile fail!\n",id);
            break;
        }

        /* Feed into flexidag(s) and get result */
        retcode = DagFlow_Process(id, &RecvData, &Resultl);
        if (retcode != 0) {
            printf("[ID%u] DagFlow_Process fail!\n",id);
            break;
        }

        /* do post-process */
        retcode = PostProcess_Impl(id, &RecvData, Resultl);
        if (retcode != 0) {
            printf("[ID%u] PostProcess_Impl fail!\n",id);
            break;
        }
    } while(0);

    fprintf(stderr, "[ID%u] Thread exit!(%u)\n", id, retcode);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    uint32_t retcode;
    pthread_t t_id0;
    void *ret;
    uint32_t SrcInputType;
    retcode = InitAPP(argc, argv);
    if (retcode != 0) {
        printf("main: InitAPP fail!\n");
        return retcode;
    }

    retcode = InitCVFlow();
    if (retcode != 0) {
        printf("main: InitCV fail!\n");
        return retcode;
    }
    /* single channel for this case */
    Preference_GetSrcInputType(0, &SrcInputType);
    if (SrcInputType == SRC_INPUT_TYPE_FILE) {
        pthread_create(&t_id0, NULL, Thread_Fin_0, NULL);
    } else {
        pthread_create(&t_id0, NULL, Thread_0, NULL);
    }

    pthread_join(t_id0, &ret);

    /* Program exit */
    printf("main process finish.\n");
    return 0;
}

