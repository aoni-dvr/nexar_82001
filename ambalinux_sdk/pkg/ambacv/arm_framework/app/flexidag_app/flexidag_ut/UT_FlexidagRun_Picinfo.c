#include <stdio.h>
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_svccvalgo_memio_interface.h"
#include "cvapi_visutil.h"
#include "cvapi_amba_od_interface.h"
#include "idsp_roi_msg.h"


#define MNETSSD_CVTASK_UUID         2
#define MNETSSD_HL_CVTASK_UUID      4
#define MAX_GOLDEN_NUM              4

#define AMBAOD_MAX_BBX_NUM  200

#define AmbaMisra_TypeCast(a, b) memcpy(a, b, sizeof(void *))

typedef struct {
    flexidag_memblk_t bin_buf;
    AMBA_CV_FLEXIDAG_INIT_s init;
    flexidag_memblk_t yuv_buf;
    AMBA_CV_FLEXIDAG_IO_s in_buf;
    AMBA_CV_FLEXIDAG_IO_s out_buf;
} flexidag_memblk_set_s;

typedef struct {
    amba_od_out_t Info;
    amba_od_candidate_t Bbx[AMBAOD_MAX_BBX_NUM];
} AMBA_OD_s;

typedef struct {
    uint32_t raw_w;
    uint32_t raw_h;

    uint32_t roi_w;
    uint32_t roi_h;

    uint32_t roi_start_col;
    uint32_t roi_start_row;

    uint32_t net_in_w;
    uint32_t net_in_h;
} win_ctx_t;

static VIS_UTIL_BBX_LIST_MSG_s golden[MAX_GOLDEN_NUM] = {0};

static uint32_t idsp_pyramid_scale = 0U;
static uint32_t roi_start_col = 810U;
static uint32_t roi_start_row = 390U;
static uint32_t source_vin = 0U;

static void RefCV_GoldenInit(void)
{
#if defined(CHIP_CV2A) || defined(CHIP_CV2FS) || defined(CHIP_CV22FS) || defined(CHIP_CV5) || defined(CHIP_CV52)
    golden[0].NumBbx = 4U;
    golden[0].Bbx[0].Cat = (uint16_t)255U;
    golden[0].Bbx[0].X = (uint16_t)810U;
    golden[0].Bbx[0].Y = (uint16_t)390U;
    golden[0].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[0].Bbx[0].H = (uint16_t)(690U-390U);
    golden[0].Bbx[1].Cat = (uint16_t)3U;
    golden[0].Bbx[1].X = (uint16_t)884U;
    golden[0].Bbx[1].Y = (uint16_t)475U;
    golden[0].Bbx[1].W = (uint16_t)(994U - 884U);
    golden[0].Bbx[1].H = (uint16_t)(579U - 475U);
    golden[0].Bbx[2].Cat = (uint16_t)3U;
    golden[0].Bbx[2].X = (uint16_t)1025U;
    golden[0].Bbx[2].Y = (uint16_t)482U;
    golden[0].Bbx[2].W = (uint16_t)(1103U - 1025U);
    golden[0].Bbx[2].H = (uint16_t)(535U - 482U);
    golden[0].Bbx[3].Cat = (uint16_t)3U;
    golden[0].Bbx[3].X = (uint16_t)813U;
    golden[0].Bbx[3].Y = (uint16_t)483U;
    golden[0].Bbx[3].W = (uint16_t)(853U - 813U);
    golden[0].Bbx[3].H = (uint16_t)(563U - 483U);

    golden[1].NumBbx = 3U;
    golden[1].Bbx[0].Cat = (uint16_t)255U;
    golden[1].Bbx[0].X = (uint16_t)810U;
    golden[1].Bbx[0].Y = (uint16_t)390U;
    golden[1].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[1].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[1].Bbx[1].Cat = (uint16_t)3U;
    golden[1].Bbx[1].X = (uint16_t)908U;
    golden[1].Bbx[1].Y = (uint16_t)513U;
    golden[1].Bbx[1].W = (uint16_t)(979U - 908U);
    golden[1].Bbx[1].H = (uint16_t)(574U - 513U);
    golden[1].Bbx[2].Cat = (uint16_t)3U;
    golden[1].Bbx[2].X = (uint16_t)860U;
    golden[1].Bbx[2].Y = (uint16_t)511U;
    golden[1].Bbx[2].W = (uint16_t)(907U - 860U);
    golden[1].Bbx[2].H = (uint16_t)(548U - 511U);

    golden[2].NumBbx = 2U;
    golden[2].Bbx[0].Cat = (uint16_t)255U;
    golden[2].Bbx[0].X = (uint16_t)810U;
    golden[2].Bbx[0].Y = (uint16_t)390U;
    golden[2].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[2].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[2].Bbx[1].Cat = (uint16_t)3U;
    golden[2].Bbx[1].X = (uint16_t)852U;
    golden[2].Bbx[1].Y = (uint16_t)504U;
    golden[2].Bbx[1].W = (uint16_t)(1009U - 852U);
    golden[2].Bbx[1].H = (uint16_t)(639U - 504U);

    golden[3].NumBbx = 4U;
    golden[3].Bbx[0].Cat = (uint16_t)255U;
    golden[3].Bbx[0].X = (uint16_t)810U;
    golden[3].Bbx[0].Y = (uint16_t)390U;
    golden[3].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[3].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[3].Bbx[1].Cat = (uint16_t)3U;
    golden[3].Bbx[1].X = (uint16_t)969U;
    golden[3].Bbx[1].Y = (uint16_t)512U;
    golden[3].Bbx[1].W = (uint16_t)(1103U - 969U);
    golden[3].Bbx[1].H = (uint16_t)(579U - 512U);
    golden[3].Bbx[2].Cat = (uint16_t)3U;
    golden[3].Bbx[2].X = (uint16_t)810U;
    golden[3].Bbx[2].Y = (uint16_t)516U;
    golden[3].Bbx[2].W = (uint16_t)(914U - 810U);
    golden[3].Bbx[2].H = (uint16_t)(604U - 516U);
    golden[3].Bbx[3].Cat = (uint16_t)3U;
    golden[3].Bbx[3].X = (uint16_t)905U;
    golden[3].Bbx[3].Y = (uint16_t)518U;
    golden[3].Bbx[3].W = (uint16_t)(980U - 905U);
    golden[3].Bbx[3].H = (uint16_t)(576U - 518U);
#elif defined(CHIP_CV28)
    golden[0].NumBbx = 4U;
    golden[0].Bbx[0].Cat = (uint16_t)255U;
    golden[0].Bbx[0].X = (uint16_t)810U;
    golden[0].Bbx[0].Y = (uint16_t)390U;
    golden[0].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[0].Bbx[0].H = (uint16_t)(690U-390U);
    golden[0].Bbx[1].Cat = (uint16_t)3U;
    golden[0].Bbx[1].X = (uint16_t)884U;
    golden[0].Bbx[1].Y = (uint16_t)473U;
    golden[0].Bbx[1].W = (uint16_t)(994U - 884U);
    golden[0].Bbx[1].H = (uint16_t)(580U - 473U);
    golden[0].Bbx[2].Cat = (uint16_t)3U;
    golden[0].Bbx[2].X = (uint16_t)1026U;
    golden[0].Bbx[2].Y = (uint16_t)482U;
    golden[0].Bbx[2].W = (uint16_t)(1104U - 1026U);
    golden[0].Bbx[2].H = (uint16_t)(535U - 482U);
    golden[0].Bbx[3].Cat = (uint16_t)3U;
    golden[0].Bbx[3].X = (uint16_t)812U;
    golden[0].Bbx[3].Y = (uint16_t)482U;
    golden[0].Bbx[3].W = (uint16_t)(854U - 812U);
    golden[0].Bbx[3].H = (uint16_t)(564U - 482U);

    golden[1].NumBbx = 3U;
    golden[1].Bbx[0].Cat = (uint16_t)255U;
    golden[1].Bbx[0].X = (uint16_t)810U;
    golden[1].Bbx[0].Y = (uint16_t)390U;
    golden[1].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[1].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[1].Bbx[1].Cat = (uint16_t)3U;
    golden[1].Bbx[1].X = (uint16_t)908U;
    golden[1].Bbx[1].Y = (uint16_t)515U;
    golden[1].Bbx[1].W = (uint16_t)(981U - 908U);
    golden[1].Bbx[1].H = (uint16_t)(575U - 515U);
    golden[1].Bbx[2].Cat = (uint16_t)3U;
    golden[1].Bbx[2].X = (uint16_t)859U;
    golden[1].Bbx[2].Y = (uint16_t)511U;
    golden[1].Bbx[2].W = (uint16_t)(909U - 859U);
    golden[1].Bbx[2].H = (uint16_t)(548U - 511U);

    golden[2].NumBbx = 2U;
    golden[2].Bbx[0].Cat = (uint16_t)255U;
    golden[2].Bbx[0].X = (uint16_t)810U;
    golden[2].Bbx[0].Y = (uint16_t)390U;
    golden[2].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[2].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[2].Bbx[1].Cat = (uint16_t)3U;
    golden[2].Bbx[1].X = (uint16_t)846U;
    golden[2].Bbx[1].Y = (uint16_t)504U;
    golden[2].Bbx[1].W = (uint16_t)(1011U - 846U);
    golden[2].Bbx[1].H = (uint16_t)(635U - 504U);

    golden[3].NumBbx = 3U;
    golden[3].Bbx[0].Cat = (uint16_t)255U;
    golden[3].Bbx[0].X = (uint16_t)810U;
    golden[3].Bbx[0].Y = (uint16_t)390U;
    golden[3].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[3].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[3].Bbx[1].Cat = (uint16_t)3U;
    golden[3].Bbx[1].X = (uint16_t)973U;
    golden[3].Bbx[1].Y = (uint16_t)512U;
    golden[3].Bbx[1].W = (uint16_t)(1104U - 973U);
    golden[3].Bbx[1].H = (uint16_t)(579U - 512U);
    golden[3].Bbx[2].Cat = (uint16_t)3U;
    golden[3].Bbx[2].X = (uint16_t)904U;
    golden[3].Bbx[2].Y = (uint16_t)518U;
    golden[3].Bbx[2].W = (uint16_t)(979U - 904U);
    golden[3].Bbx[2].H = (uint16_t)(576U - 518U);
#else
    golden[0].NumBbx = 4U;
    golden[0].Bbx[0].Cat = (uint16_t)255U;
    golden[0].Bbx[0].X = (uint16_t)810U;
    golden[0].Bbx[0].Y = (uint16_t)390U;
    golden[0].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[0].Bbx[0].H = (uint16_t)(690U-390U);
    golden[0].Bbx[1].Cat = (uint16_t)3U;
    golden[0].Bbx[1].X = (uint16_t)884U;
    golden[0].Bbx[1].Y = (uint16_t)472U;
    golden[0].Bbx[1].W = (uint16_t)(994U - 884U);
    golden[0].Bbx[1].H = (uint16_t)(579U - 472U);
    golden[0].Bbx[2].Cat = (uint16_t)3U;
    golden[0].Bbx[2].X = (uint16_t)1025U;
    golden[0].Bbx[2].Y = (uint16_t)482U;
    golden[0].Bbx[2].W = (uint16_t)(1103U - 1025U);
    golden[0].Bbx[2].H = (uint16_t)(535U - 482U);
    golden[0].Bbx[3].Cat = (uint16_t)3U;
    golden[0].Bbx[3].X = (uint16_t)812U;
    golden[0].Bbx[3].Y = (uint16_t)482U;
    golden[0].Bbx[3].W = (uint16_t)(854U - 812U);
    golden[0].Bbx[3].H = (uint16_t)(564U - 482U);

    golden[1].NumBbx = 3U;
    golden[1].Bbx[0].Cat = (uint16_t)255U;
    golden[1].Bbx[0].X = (uint16_t)810U;
    golden[1].Bbx[0].Y = (uint16_t)390U;
    golden[1].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[1].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[1].Bbx[1].Cat = (uint16_t)3U;
    golden[1].Bbx[1].X = (uint16_t)908U;
    golden[1].Bbx[1].Y = (uint16_t)515U;
    golden[1].Bbx[1].W = (uint16_t)(981U - 908U);
    golden[1].Bbx[1].H = (uint16_t)(575U - 515U);
    golden[1].Bbx[2].Cat = (uint16_t)3U;
    golden[1].Bbx[2].X = (uint16_t)859U;
    golden[1].Bbx[2].Y = (uint16_t)511U;
    golden[1].Bbx[2].W = (uint16_t)(909U - 859U);
    golden[1].Bbx[2].H = (uint16_t)(548U - 511U);

    golden[2].NumBbx = 2U;
    golden[2].Bbx[0].Cat = (uint16_t)255U;
    golden[2].Bbx[0].X = (uint16_t)810U;
    golden[2].Bbx[0].Y = (uint16_t)390U;
    golden[2].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[2].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[2].Bbx[1].Cat = (uint16_t)3U;
    golden[2].Bbx[1].X = (uint16_t)846U;
    golden[2].Bbx[1].Y = (uint16_t)504U;
    golden[2].Bbx[1].W = (uint16_t)(1011U - 846U);
    golden[2].Bbx[1].H = (uint16_t)(635U - 504U);

    golden[3].NumBbx = 3U;
    golden[3].Bbx[0].Cat = (uint16_t)255U;
    golden[3].Bbx[0].X = (uint16_t)810U;
    golden[3].Bbx[0].Y = (uint16_t)390U;
    golden[3].Bbx[0].W = (uint16_t)(1110U - 810U);
    golden[3].Bbx[0].H = (uint16_t)(690U - 390U);
    golden[3].Bbx[1].Cat = (uint16_t)3U;
    golden[3].Bbx[1].X = (uint16_t)973U;
    golden[3].Bbx[1].Y = (uint16_t)512U;
    golden[3].Bbx[1].W = (uint16_t)(1104U - 973U);
    golden[3].Bbx[1].H = (uint16_t)(579U - 512U);
    golden[3].Bbx[2].Cat = (uint16_t)3U;
    golden[3].Bbx[2].X = (uint16_t)904U;
    golden[3].Bbx[2].Y = (uint16_t)518U;
    golden[3].Bbx[2].W = (uint16_t)(979U - 904U);
    golden[3].Bbx[2].H = (uint16_t)(576U - 518U);
#endif
}

static uint32_t RefCV_BbxResult(uint32_t id, uint32_t frame, const flexidag_memblk_t *pBlk)
{
    uint32_t i, Rval = 0U, value = 0U, len = 0U;
    const AMBA_OD_s *pAmbaOD;
    VIS_UTIL_BBX_LIST_MSG_s BBX = {0};
    char print_buf[256];
    uint32_t index = id % MAX_GOLDEN_NUM ;

    AmbaMisra_TypeCast(&pAmbaOD, &pBlk->pBuffer);
    BBX.NumBbx = pAmbaOD->Info.num_objects;
    for (i = 0U ; i < pAmbaOD->Info.num_objects; i++) {
        BBX.Bbx[i].X = (uint16_t)pAmbaOD->Bbx[i].bb_start_col;
        BBX.Bbx[i].Y = (uint16_t)pAmbaOD->Bbx[i].bb_start_row;
        BBX.Bbx[i].W = (uint16_t)(pAmbaOD->Bbx[i].bb_width_m1 + 1U);
        BBX.Bbx[i].H = (uint16_t)(pAmbaOD->Bbx[i].bb_height_m1 + 1U);
        BBX.Bbx[i].Cat = (uint16_t)pAmbaOD->Bbx[i].clsId;
        BBX.Bbx[i].Score = pAmbaOD->Bbx[i].score;
    }

    if(golden[index].NumBbx != BBX.NumBbx) {
        Rval = 1U;
    }
    len = sprintf(&print_buf[value], "%d frame (%d) error : Result (%d) \n",id, frame,(uint32_t)BBX.NumBbx);
    value = value + len;
    for (i = 0U ; i<BBX.NumBbx ; i++) {
        len = sprintf(&print_buf[value], "(%d, %d)-(%d,%d) c:%d \n", \
                      ((uint32_t)BBX.Bbx[i].X)\
                      , ((uint32_t)BBX.Bbx[i].Y)\
                      , ((uint32_t)BBX.Bbx[i].X + (uint32_t)BBX.Bbx[i].W)\
                      , ((uint32_t)BBX.Bbx[i].Y + (uint32_t)BBX.Bbx[i].H)\
                      , ((uint32_t)BBX.Bbx[i].Cat));
        value = value + len;
        if( (golden[index].Bbx[i].X != BBX.Bbx[i].X) || (golden[index].Bbx[i].Y != BBX.Bbx[i].Y)
            ||(golden[index].Bbx[i].W != BBX.Bbx[i].W) || (golden[index].Bbx[i].H != BBX.Bbx[i].H)) {
            Rval = 1U;
        }
    }
    if(Rval == 1U) {
        printf("%s ",print_buf);
    }
    return Rval;
}

static uint32_t roi_handling(AMBA_CV_FLEXIDAG_HANDLE_s *fd_gen_handle, const cv_pic_info_t *idsp_pic_info)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t roi_w_m1, roi_h_m1;
    win_ctx_t win_ctx = {0};
    uint16_t flow_id;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    static uint32_t network_input_width = 300U;
    static uint32_t network_input_height = 300U;

    (void) fd_gen_handle;
    roi_w_m1 = idsp_pic_info->pyramid.half_octave[idsp_pyramid_scale].roi_width_m1;
    roi_h_m1 = idsp_pic_info->pyramid.half_octave[idsp_pyramid_scale].roi_height_m1;

    if (roi_start_col == 9999U) {
        roi_start_col = (roi_w_m1 + 1U - network_input_width) / 2U;
    } else if (roi_start_col > 0x7FFFFFFFU) {
        roi_start_col = 0U;
    } else if (roi_start_col > (roi_w_m1 + 1U - network_input_width)) {
        roi_start_col = (roi_w_m1 + 1U - network_input_width);
    } else {
        /* do nothing */
    }

    if (roi_start_row == 9999U) {
        roi_start_row = (roi_h_m1 + 1U - network_input_height) / 2U;
    } else if (roi_start_row > 0x7FFFFFFFU) {
        roi_start_row = 0U;
    } else if (roi_start_row > (roi_h_m1 + 1U - network_input_height)) {
        roi_start_row = (roi_h_m1 + 1U - network_input_height);
    } else {
        /* do nothing */
    }

    //printf("roi_handling: pyramid_scale = %d, start_col = %d, start_row= %d\n",idsp_pyramid_scale,roi_start_col,roi_start_row);
    //printf("roi_handling: roi_w_m1 = %d, roi_h_m1 = %d\n", roi_w_m1, roi_h_m1);

    win_ctx.raw_w = ((uint32_t)idsp_pic_info->pyramid.half_octave[0].roi_width_m1) + 1U;
    win_ctx.raw_h = ((uint32_t)idsp_pic_info->pyramid.half_octave[0].roi_height_m1) + 1U;
    win_ctx.roi_w = roi_w_m1 + 1U;
    win_ctx.roi_h = roi_h_m1 + 1U;

    win_ctx.roi_start_col    = roi_start_col;
    win_ctx.roi_start_row    = roi_start_row;
    win_ctx.net_in_w         = network_input_width;
    win_ctx.net_in_h         = network_input_height;

    (void) AmbaCV_FlexidagGetFlowIdByUUID(fd_gen_handle, MNETSSD_HL_CVTASK_UUID, &flow_id);
    msg.flow_id = flow_id;
    msg.vpMessage = &win_ctx;
    msg.length = sizeof(win_ctx);
    retcode = AmbaCV_FlexidagSendMsg(fd_gen_handle,&msg);
    return retcode;
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
    const char flexidag_path[] = "/tmp/SD0/flexidag_mnetssd_sensor/bin/flexibin/flexibin0.bin";
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

static uint32_t Do_Load_Input_File(memio_source_recv_picinfo_t *picinfo, flexidag_memblk_t *yuv_buf)
{
    const char yuv_path[] = "/tmp/SD0/flexidag_mnetssd_sensor/bin/golden/0.yuv";
    cv_pic_info_t *pic_info = &picinfo->pic_info;
    uint32_t size_align;
    uint32_t YuvWidth = 1920U;
    uint32_t YuvHeight = 1080U;
    uint32_t PaDataAddr;
    uint32_t Rval = 0U;

    // load input YUV file
    Rval = AmbaCV_UtilityFileSize(yuv_path, &size_align);
    if (Rval == 0U) {
        Rval = AmbaCV_UtilityCmaMemAlloc(size_align, 1, yuv_buf);
        if (Rval == 0U) {
            Rval = AmbaCV_UtilityFileLoad(yuv_path, yuv_buf);
        }
    }
    if (Rval != 0U) {
        printf("Do_Load_Input_File : load input YUV file fail (0x%x)\n", Rval);
    }

    if (Rval == 0U) {
        pic_info->capture_time = 0U;
        pic_info->channel_id = 0U;
        pic_info->frame_num = 0U;
        pic_info->pyramid.image_width_m1 = YuvWidth - 1U;
        pic_info->pyramid.image_height_m1 = YuvHeight - 1U;
        pic_info->pyramid.image_pitch_m1 = YuvWidth - 1U;
        pic_info->pyramid.half_octave[0].ctrl.roi_pitch = YuvWidth;
        pic_info->pyramid.half_octave[0].roi_start_col = 0;
        pic_info->pyramid.half_octave[0].roi_start_row = 0;
        pic_info->pyramid.half_octave[0].roi_width_m1 = YuvWidth - 1U;
        pic_info->pyramid.half_octave[0].roi_height_m1 = YuvHeight - 1U;

        pic_info->pyramid.half_octave[1].ctrl.disable = 1U;
        pic_info->pyramid.half_octave[2].ctrl.disable = 1U;
        pic_info->pyramid.half_octave[3].ctrl.disable = 1U;
        pic_info->pyramid.half_octave[4].ctrl.disable = 1U;
        pic_info->pyramid.half_octave[5].ctrl.disable = 1U;

        PaDataAddr = yuv_buf->buffer_daddr;
        pic_info->rpLumaLeft[0] = PaDataAddr;
        pic_info->rpChromaLeft[0] = (PaDataAddr + (YuvWidth * YuvHeight));
        pic_info->rpLumaRight[0] = pic_info->rpLumaLeft[0];
        pic_info->rpChromaRight[0] = pic_info->rpChromaLeft[0];
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

static uint32_t Do_Alloc_IO_Buffer(AMBA_CV_FLEXIDAG_HANDLE_s *handle, const cv_pic_info_t *pic_info, AMBA_CV_FLEXIDAG_IO_s *in_buf, AMBA_CV_FLEXIDAG_IO_s *out_buf)
{
    cv_pic_info_t *pDataIn;
    uint32_t U32DataIn;
    uint32_t i;
    uint32_t Rval = 0U;

    //input_buf
    in_buf->num_of_buf = 1U;
    Rval = AmbaCV_UtilityCmaMemAlloc(sizeof(memio_source_recv_picinfo_t), 1, &in_buf->buf[0]);
    if (Rval == 0U) {
        memcpy(&pDataIn, &in_buf->buf[0].pBuffer, sizeof(void *));
        memcpy(pDataIn, pic_info, sizeof(cv_pic_info_t));
        U32DataIn = in_buf->buf[0].buffer_daddr;
        for (i = 0U ; i < MAX_HALF_OCTAVES ; i++) {
            pDataIn->rpLumaLeft[i] = pic_info->rpLumaLeft[i] - U32DataIn;
            pDataIn->rpLumaRight[i] = pic_info->rpLumaRight[i] - U32DataIn;
            pDataIn->rpChromaLeft[i] = pic_info->rpChromaLeft[i] - U32DataIn;
            pDataIn->rpChromaRight[i] = pic_info->rpChromaRight[i] - U32DataIn;
        }
        Rval = AmbaCV_UtilityCmaMemClean(&in_buf->buf[0]);
    }
    if (Rval != 0U) {
        printf("Do_Alloc_IO_Buffer : in_buf->buf[0] fail (0x%x)\n", Rval);
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

static uint32_t Do_SendMsg(AMBA_CV_FLEXIDAG_HANDLE_s *handle)
{
    uint32_t Rval = 0U;
    uint16_t flow_id;
    AMBA_CV_FLEXIDAG_MSG_s msg;
    amba_roi_config_t roi_cfg;

    Rval = AmbaCV_FlexidagGetFlowIdByUUID(handle, MNETSSD_CVTASK_UUID, &flow_id);
    if (Rval == 0U) {
        roi_cfg.msg_type = AMBA_ROI_CONFIG_MSG;
        roi_cfg.image_pyramid_index = idsp_pyramid_scale;
        roi_cfg.source_vin = source_vin;
        roi_cfg.roi_start_col = roi_start_col;
        roi_cfg.roi_start_row = roi_start_row;

        msg.flow_id = flow_id;
        msg.vpMessage = &roi_cfg;
        msg.length = sizeof(roi_cfg);
        Rval = AmbaCV_FlexidagSendMsg(handle, &msg);
    }

    if (Rval != 0U) {
        printf("Do_SendMsg fail (0x%x)\n", Rval);
    }

    return Rval;
}

static uint32_t Do_Free_All_Buffer(flexidag_memblk_set_s *mem)
{
    uint32_t i;
    uint32_t Rval = 0U;

    Rval |= AmbaCV_UtilityCmaMemFree(&mem->bin_buf);
    Rval |= AmbaCV_UtilityCmaMemFree(&mem->init.state_buf);
    Rval |= AmbaCV_UtilityCmaMemFree(&mem->yuv_buf);
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
    uint32_t Rval = 0U;

    Rval = AmbaCV_UtilityCmaMemInvalid(&out_buf->buf[0]);
    if (Rval == 0U) {
        Rval = RefCV_BbxResult(0U, loop, &out_buf->buf[0]);
        if (Rval != 0U) {
            printf("%d fail\n", loop);
        } else {
            printf("%d ok\n", loop);
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

static uint32_t Do_Prepare_IO(AMBA_CV_FLEXIDAG_HANDLE_s *handle, flexidag_memblk_set_s *mem)
{
    memio_source_recv_picinfo_t picinfo;
    uint32_t Rval = 0U;

    Rval = Do_Load_Input_File(&picinfo, &mem->yuv_buf);
    if (Rval == 0U) {
        Rval = Do_Alloc_IO_Buffer(handle, &picinfo.pic_info, &mem->in_buf, &mem->out_buf);
    }

    if (Rval != 0U) {
        printf("Do_Prepare_IO fail (0x%x)\n", Rval);
    }

    return Rval;
}

uint32_t main(void)
{
    AMBA_CV_FLEXIDAG_HANDLE_s handle = {0};
    flexidag_memblk_set_s mem = {0};
    AMBA_CV_FLEXIDAG_RUN_INFO_s run_info = {0};
    uint32_t i;
    uint32_t Rval = 0U;

    RefCV_GoldenInit();

    Rval = Do_SchdrStart();
    if (Rval == 0U) {
        Rval = Do_FlexidagInit(&handle, &mem);
        if (Rval == 0U) {
            Rval = Do_Prepare_IO(&handle, &mem);
            if (Rval == 0U) {
                Rval = Do_SendMsg(&handle);
                if (Rval == 0U) {
                    for(i = 0; i < 5; i++) {
                        Rval = roi_handling(&handle, mem.in_buf.buf[0].pBuffer);
                        if (Rval == 0U) {
                            Rval = AmbaCV_FlexidagRun(&handle, &mem.in_buf, &mem.out_buf, &run_info);
                            if (Rval == 0U) {
                                Rval = Do_Show_Output(i, &mem.out_buf);
                            }
                        }
                        if (Rval != 0U) {
                            break;
                        }
                    }
                    if (Rval == 0U) {
                        Rval = AmbaCV_FlexidagClose(&handle);
                    }
                }
            }
        }
        Rval |= Do_Free_All_Buffer(&mem);
    }

    if (Rval != 0U) {
        printf("UT_FlexidagRun_Picinfo fail (0x%x)\n", Rval);
    } else {
        printf("UT_FlexidagRun_Picinfo success\n");
    }

    return 0;
}