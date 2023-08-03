#include <stdio.h>
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_svccvalgo_memio_interface.h"


#define MAX_GOLDEN_NUM              4

typedef struct {
    flexidag_memblk_t bin_buf;
    AMBA_CV_FLEXIDAG_INIT_s init;
    flexidag_memblk_t y_buf;
    flexidag_memblk_t uv_buf;
    AMBA_CV_FLEXIDAG_IO_s in_buf;
    AMBA_CV_FLEXIDAG_IO_s out_buf;
} flexidag_memblk_set_s;

static uint32_t Crc32Table[] = {
    0x00000000U, 0x77073096U, 0xee0e612cU, 0x990951baU, 0x076dc419U, 0x706af48fU,
    0xe963a535U, 0x9e6495a3U, 0x0edb8832U, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U,
    0x09b64c2bU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U, 0x1db71064U, 0x6ab020f2U,
    0xf3b97148U, 0x84be41deU, 0x1adad47dU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
    0x136c9856U, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU, 0x14015c4fU, 0x63066cd9U,
    0xfa0f3d63U, 0x8d080df5U, 0x3b6e20c8U, 0x4c69105eU, 0xd56041e4U, 0xa2677172U,
    0x3c03e4d1U, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU, 0x35b5a8faU, 0x42b2986cU,
    0xdbbbc9d6U, 0xacbcf940U, 0x32d86ce3U, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
    0x26d930acU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U, 0x21b4f4b5U, 0x56b3c423U,
    0xcfba9599U, 0xb8bda50fU, 0x2802b89eU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U,
    0x2f6f7c87U, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU, 0x76dc4190U, 0x01db7106U,
    0x98d220bcU, 0xefd5102aU, 0x71b18589U, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
    0x7807c9a2U, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U, 0x7f6a0dbbU, 0x086d3d2dU,
    0x91646c97U, 0xe6635c01U, 0x6b6b51f4U, 0x1c6c6162U, 0x856530d8U, 0xf262004eU,
    0x6c0695edU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U, 0x65b0d9c6U, 0x12b7e950U,
    0x8bbeb8eaU, 0xfcb9887cU, 0x62dd1ddfU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
    0x4db26158U, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U, 0x4adfa541U, 0x3dd895d7U,
    0xa4d1c46dU, 0xd3d6f4fbU, 0x4369e96aU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U,
    0x44042d73U, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U, 0x5005713cU, 0x270241aaU,
    0xbe0b1010U, 0xc90c2086U, 0x5768b525U, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
    0x5edef90eU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U, 0x59b33d17U, 0x2eb40d81U,
    0xb7bd5c3bU, 0xc0ba6cadU, 0xedb88320U, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU,
    0xead54739U, 0x9dd277afU, 0x04db2615U, 0x73dc1683U, 0xe3630b12U, 0x94643b84U,
    0x0d6d6a3eU, 0x7a6a5aa8U, 0xe40ecf0bU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
    0xf00f9344U, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU, 0xf762575dU, 0x806567cbU,
    0x196c3671U, 0x6e6b06e7U, 0xfed41b76U, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU,
    0xf9b9df6fU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U, 0xd6d6a3e8U, 0xa1d1937eU,
    0x38d8c2c4U, 0x4fdff252U, 0xd1bb67f1U, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
    0xd80d2bdaU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U, 0xdf60efc3U, 0xa867df55U,
    0x316e8eefU, 0x4669be79U, 0xcb61b38cU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U,
    0xcc0c7795U, 0xbb0b4703U, 0x220216b9U, 0x5505262fU, 0xc5ba3bbeU, 0xb2bd0b28U,
    0x2bb45a92U, 0x5cb36a04U, 0xc2d7ffa7U, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
    0x9b64c2b0U, 0xec63f226U, 0x756aa39cU, 0x026d930aU, 0x9c0906a9U, 0xeb0e363fU,
    0x72076785U, 0x05005713U, 0x95bf4a82U, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U,
    0x92d28e9bU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U, 0x86d3d2d4U, 0xf1d4e242U,
    0x68ddb3f8U, 0x1fda836eU, 0x81be16cdU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
    0x88085ae6U, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU, 0x8f659effU, 0xf862ae69U,
    0x616bffd3U, 0x166ccf45U, 0xa00ae278U, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U,
    0xa7672661U, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU, 0xaed16a4aU, 0xd9d65adcU,
    0x40df0b66U, 0x37d83bf0U, 0xa9bcae53U, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
    0xbdbdf21cU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U, 0xbad03605U, 0xcdd70693U,
    0x54de5729U, 0x23d967bfU, 0xb3667a2eU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U,
    0xb40bbe37U, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU
};

static uint32_t bisenet_golden[MAX_GOLDEN_NUM] = {
#if defined(CHIP_CV2A) || defined(CHIP_CV2FS) || defined(CHIP_CV22FS) || defined(CHIP_CV5) || defined(CHIP_CV52)
    0xf3647648U,
    0xc806f96dU,
    0x3f943256U,
    0x1e66fd8dU,
#elif defined(CHIP_CV28)
    0x8707bb19U,
    0x2f8c2e71U,
    0x9f1fb42bU,
    0x5452d506U,
#elif defined(CHIP_CV6)
    0x113fa428U,
    0xbbe5f46dU,
    0x1ac2faa8U,
    0x6d26d411U,
#else
    0xbd29bf68U,
    0x553ad91fU,
    0xdca92e6cU,
    0x329d7a64U
#endif
};

static uint32_t check_crc32(const uint8_t *pBuffer, uint32_t Size)
{
    const uint8_t *p;
    uint32_t Crc;
    uint32_t idx;

    p = pBuffer;
    Crc = ~0U;

    while (Size > 0U) {
        idx = (Crc ^ (uint32_t)*p) & 0x0FFU;
        Crc = Crc32Table[idx] ^ (Crc >> 8U);
        p++;
        Size--;
    }

    return Crc ^ ~0U;
}

static void Do_Prepare_Config(AMBA_CV_STANDALONE_SCHDR_CFG_s *cfg)
{
    cfg->flexidag_slot_num = 8;
    cfg->cavalry_slot_num = 0;
    cfg->cpu_map = 0xD;
    cfg->log_level = LVL_DEBUG;
    cfg->boot_cfg = 1 + FLEXIDAG_ENABLE_BOOT_CFG;
}

static uint32_t Do_SchdrStart(void)
{
    const char orcvp_path[] = "/lib/firmware/";
    uint32_t schdr_state;
    uint32_t Rval = 0U;

    Rval = AmbaCV_SchdrState(&schdr_state);
    if (Rval == 0U) {
        if (schdr_state == FLEXIDAG_SCHDR_OFF) {
            AMBA_CV_STANDALONE_SCHDR_CFG_s cfg = {0};

            Rval = AmbaCV_SchdrLoad(orcvp_path);
            if (Rval == 0U) {
                Do_Prepare_Config(&cfg);
                Rval = AmbaCV_StandAloneSchdrStart(&cfg);
            }
        } else {
            AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

            cfg.cpu_map = 0xD;
            cfg.log_level = LVL_DEBUG;
            Rval = AmbaCV_FlexidagSchdrStart(&cfg);
        }
    }

    if (Rval != 0U) {
        printf("Do_SchdrStart fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_Load_FlexidBin(flexidag_memblk_t *bin_buf)
{
    const char flexidag_path[] = "/tmp/SD0/flexidag_bisenet_mnv2_raw/flexibin/flexibin0.bin";
    uint32_t size_align;
    uint32_t Rval = 0U;

    Rval = AmbaCV_UtilityFileSize(flexidag_path, &size_align);
    if (Rval == 0U) {
        Rval = AmbaCV_UtilityCmaMemAlloc(size_align, 1, bin_buf);
        if (Rval == 0U) {
            Rval = AmbaCV_UtilityFileLoad(flexidag_path, bin_buf);
        }
    }

    if (Rval != 0U) {
        printf("Do_Load_FlexidBin fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_Load_Input_File(memio_source_recv_multi_raw_t *raw, flexidag_memblk_t *y_buf, flexidag_memblk_t *uv_buf)
{
    const char y_path[] = "/tmp/SD0/flexidag_bisenet_mnv2_raw/golden/0_y.bin";
    const char uv_path[] = "/tmp/SD0/flexidag_bisenet_mnv2_raw/golden/0_uv.bin";
    uint32_t size_align;
    uint32_t Rval = 0U;

    // load input Y file
    Rval = AmbaCV_UtilityFileSize(y_path, &size_align);
    if (Rval == 0U) {
        Rval = AmbaCV_UtilityCmaMemAlloc(size_align, 1, y_buf);
        if (Rval == 0U) {
            Rval = AmbaCV_UtilityFileLoad(y_path, y_buf);
        }
    }
    if (Rval != 0U) {
        printf("Do_Load_Input_File : load input Y file fail (0x%x)\n", Rval);
    }

    // load input UV file
    if (Rval == 0U) {
        Rval = AmbaCV_UtilityFileSize(uv_path, &size_align);
        if (Rval == 0U) {
            Rval = AmbaCV_UtilityCmaMemAlloc(size_align, 1, uv_buf);
            if (Rval == 0U) {
                Rval = AmbaCV_UtilityFileLoad(uv_path, uv_buf);
            }
        }
        if (Rval != 0U) {
            printf("Do_Load_Input_File : load input UV file fail (0x%x)\n", Rval);
        }
    }

    if (Rval == 0U) {
        raw->num_io = 2;
        raw->io[0].addr = y_buf->buffer_caddr;
        raw->io[0].size = y_buf->buffer_size;
        raw->io[0].pitch = 0U;

        raw->io[1].addr = uv_buf->buffer_caddr;
        raw->io[1].size = uv_buf->buffer_size;
        raw->io[1].pitch = 0U;
    }

    return Rval;
}

static uint32_t DO_Alloc_State_Buffer(AMBA_CV_FLEXIDAG_HANDLE_s *handle, AMBA_CV_FLEXIDAG_INIT_s *init)
{
    uint32_t Rval = AmbaCV_UtilityCmaMemAlloc(handle->mem_req.flexidag_state_buffer_size, 1U, &init->state_buf);

    if (Rval != 0U) {
        printf("DO_Alloc_State_Buffer fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_Alloc_IO_Buffer(AMBA_CV_FLEXIDAG_HANDLE_s *handle, memio_source_recv_multi_raw_t *raw, AMBA_CV_FLEXIDAG_IO_s *in_buf, AMBA_CV_FLEXIDAG_IO_s *out_buf)
{
    memio_source_recv_raw_t *pDataIn;
    uint32_t i;
    uint32_t Rval = 0U;

    //input_buf
    in_buf->num_of_buf = raw->num_io;
    for(i = 0; i < in_buf->num_of_buf; i++) {
        Rval = AmbaCV_UtilityCmaMemAlloc(sizeof(memio_source_recv_raw_t), 1, &in_buf->buf[i]);
        if (Rval == 0U) {
            memcpy(&pDataIn, &in_buf->buf[i].pBuffer, sizeof(void *));
            memcpy(pDataIn, &raw->io[i], sizeof(memio_source_recv_raw_t));
            Rval = AmbaCV_UtilityCmaMemClean(&in_buf->buf[i]);
        }
        if (Rval != 0U) {
            printf("Do_Alloc_IO_Buffer : in_buf->buf[%d] fail (0x%x)\n", i, Rval);
            break;
        }
    }

    //output buffer
    if (Rval == 0U) {
        out_buf->num_of_buf = handle->mem_req.flexidag_num_outputs;
        for(i = 0; i < out_buf->num_of_buf; i++) {
            Rval = AmbaCV_UtilityCmaMemAlloc(handle->mem_req.flexidag_output_buffer_size[i], 1, &out_buf->buf[i]);
            if (Rval != 0U) {
                printf("Do_Alloc_IO_Buffer : out_buf->buf[%d] fail (0x%x)\n", i, Rval);
                break;
            }
        }
    }

    return Rval;
}

static uint32_t Do_Free_All_Buffer(flexidag_memblk_set_s *mem)
{
    uint32_t i;
    uint32_t Rval = 0U;

    Rval |= AmbaCV_UtilityCmaMemFree(&mem->bin_buf);
    Rval |= AmbaCV_UtilityCmaMemFree(&mem->init.state_buf);
    Rval |= AmbaCV_UtilityCmaMemFree(&mem->y_buf);
    Rval |= AmbaCV_UtilityCmaMemFree(&mem->uv_buf);
    for(i = 0; i < mem->in_buf.num_of_buf; i++) {
        Rval |= AmbaCV_UtilityCmaMemFree(&mem->in_buf.buf[i]);
    }
    for(i = 0; i < mem->out_buf.num_of_buf; i++) {
        Rval |= AmbaCV_UtilityCmaMemFree(&mem->out_buf.buf[i]);
    }
    if (Rval != 0U) {
        printf("Do_Free_All_Buffer fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_Show_Output(uint32_t loop, AMBA_CV_FLEXIDAG_IO_s *out_buf)
{
    uint32_t Crc;
    uint32_t Rval = 0U;

    Rval = AmbaCV_UtilityCmaMemInvalid(&out_buf->buf[0]);
    if (Rval == 0U) {
        Crc = check_crc32((const uint8_t *)out_buf->buf[0].pBuffer, out_buf->buf[0].buffer_size);
        if (Crc != bisenet_golden[0]) {
            printf("%d fail Crc32 = 0x%x\n", loop, Crc);
        } else {
            printf("%d ok Crc32 = 0x%x\n", loop, Crc);
        }
        memset(out_buf->buf[0].pBuffer, 0x0, out_buf->buf[0].buffer_size);
        Rval = AmbaCV_UtilityCmaMemClean(&out_buf->buf[0]);
    }

    if (Rval != 0U) {
        printf("Do_Show_Output fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_FlexidagInit(AMBA_CV_FLEXIDAG_HANDLE_s *handle, flexidag_memblk_set_s *mem)
{
    uint32_t Rval = 0U;

    Rval = Do_Load_FlexidBin(&mem->bin_buf);
    if (Rval == 0U) {
        Rval = AmbaCV_FlexidagOpen(&mem->bin_buf, handle);
        if (Rval == 0U) {
            Rval |= DO_Alloc_State_Buffer(handle, &mem->init);
            Rval |= AmbaCV_FlexidagInit(handle, &mem->init);
        }
    }

    if (Rval != 0U) {
        printf("Do_FlexidagInit fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_FlexidagRun(AMBA_CV_FLEXIDAG_HANDLE_s *handle, flexidag_memblk_set_s *mem)
{
    memio_source_recv_multi_raw_t raw;
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info = {0};
    uint32_t i;
    uint32_t Rval = 0U;

    Rval = Do_Load_Input_File(&raw, &mem->y_buf, &mem->uv_buf);
    if (Rval == 0U) {
        Rval = Do_Alloc_IO_Buffer(handle, &raw, &mem->in_buf, &mem->out_buf);
        for(i = 0; i < 5; i++) {
            Rval = AmbaCV_FlexidagRun(handle, &mem->in_buf, &mem->out_buf, &run_info);
            if (Rval == 0U) {
                Rval = Do_Show_Output(i, &mem->out_buf);
            }
            if (Rval != 0U) {
                printf("Do_FlexidagRun : loop %d fail (0x%x)\n", i, Rval);
                break;
            }
        }
    }
    if (Rval != 0U) {
        printf("Do_FlexidagRun fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_FlexidagClose(AMBA_CV_FLEXIDAG_HANDLE_s *handle, flexidag_memblk_set_s *mem)
{
    uint32_t Rval = 0U;

    Rval |= AmbaCV_FlexidagClose(handle);
    Rval |= Do_Free_All_Buffer(mem);
    if (Rval != 0U) {
        printf("Do_FlexidagClose fail (0x%x)\n", Rval);
    }

    return Rval;
}

uint32_t main(void)
{
    AMBA_CV_FLEXIDAG_HANDLE_s handle = {0};
    flexidag_memblk_set_s mem = {0};
    AMBA_CV_FLEXIDAG_PERF_s flow_id_perf[4];
    uint32_t Rval = 0U;

    Rval = Do_SchdrStart();
    if (Rval == 0U) {
        Rval = Do_FlexidagInit(&handle, &mem);
        if (Rval == 0U) {
            Rval = AmbaCV_FlexidagPerf(&handle, FLEXIDAG_PERF_START, (uint32_t)CVCORE_VP0, flow_id_perf, 4);
            if (Rval == 0U) {
                Rval |= Do_FlexidagRun(&handle, &mem);
                Rval |= AmbaCV_FlexidagPerf(&handle, FLEXIDAG_PERF_STOP, (uint32_t)CVCORE_VP0, flow_id_perf, 4);
            }
        }
        Rval |= Do_FlexidagClose(&handle, &mem);
    }

    if (Rval != 0U) {
        printf("UT_FlexidagPerf fail (0x%x)\n", Rval);
    } else {
        printf("UT_FlexidagPerf success\n");
    }

    return 0;
}