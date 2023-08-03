#include "test_ik_global.h"
#include "AmbaDSP_Img3astatistic.h"

#ifndef AMBA_IK_3A_SLICE_MAX_COUNT
#define AMBA_IK_3A_SLICE_MAX_COUNT             32U
#endif


FILE *pFile;
UINT32 id_cnt;

INT32 ik_ctest_aaa_main(void)
{
    UINT32 Rval = IK_OK;
    DSP_EVENT_3A_TRANSFER_s aaa_stat_data= {0};
    FILE *pFile_input1, *pFile_input2, *pFile_output;
    static UINT8 input_buffer[2][55042];
    static UINT8 output_buffer[55042];
    ik_3a_header_t src_aaa_header= {0};

    //vid8_4k_3a 2x1 stitching...
    aaa_stat_data.DstAddr = (uintptr)&output_buffer[0];
    aaa_stat_data.SrcAddr[0] = (uintptr)&input_buffer[0][0];
    aaa_stat_data.SrcAddr[1] = (uintptr)&input_buffer[1][0];
    aaa_stat_data.SrcSliceX = 2;
    aaa_stat_data.SrcSliceY = 1;

    id_cnt = 0;

    pFile = fopen("data/ik_ctest_aaa.txt", "w");

    //cfa aaa 2x1
    // load data
    pFile_input1 = fopen("../util/bin/vid8_4k_3a/cfa_1_x0_y0.bin", "rb");
    fread(&input_buffer[0][0], 1, 55042, pFile_input1);
    fclose(pFile_input1);
    pFile_input2 = fopen("../util/bin/vid8_4k_3a/cfa_1_x1_y0.bin", "rb");
    fread(&input_buffer[1][0], 1, 55042, pFile_input2);
    fclose(pFile_input2);

    // test
    Rval |= AmbaIK_TransferCFAAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferCFAAaaStatData", "OK case in ik_ctest_aaa.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_aaa_cfa.bin", "wb");
        fwrite(output_buffer, 1, 55042, pFile_output);
        fclose(pFile_output);
    }

    //pg aaa 2x1
    // load data
    pFile_input1 = fopen("../util/bin/vid8_4k_3a/pg_1_x0_y0.bin", "rb");
    fread(&input_buffer[0][0], 1, 5122, pFile_input1);
    fclose(pFile_input1);
    pFile_input2 = fopen("../util/bin/vid8_4k_3a/pg_1_x1_y0.bin", "rb");
    fread(&input_buffer[1][0], 1, 5122, pFile_input2);
    fclose(pFile_input2);

    // test
    Rval |= AmbaIK_TransferPGAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferPGAaaStatData", "OK case in ik_ctest_aaa.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_aaa_pg.bin", "wb");
        fwrite(output_buffer, 1, 5122, pFile_output);
        fclose(pFile_output);
    }

    //histogram aaa 2x1
    // load data
    pFile_input1 = fopen("../util/bin/vid8_4k_3a/histo_1_expo0_x0_y0.bin", "rb");
    fread(&input_buffer[0][0], 1, 2304, pFile_input1);
    fclose(pFile_input1);
    pFile_input2 = fopen("../util/bin/vid8_4k_3a/histo_1_expo0_x1_y0.bin", "rb");
    fread(&input_buffer[1][0], 1, 2304, pFile_input2);
    fclose(pFile_input2);

    // test
    aaa_stat_data.Type = 6;//for IR histogram coverage.
    Rval |= AmbaIK_TransCFAHistStatData(&aaa_stat_data);
    aaa_stat_data.Type = 0;
    Rval |= AmbaIK_TransCFAHistStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "OK case in ik_ctest_aaa.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_aaa_hist.bin", "wb");
        fwrite(output_buffer, 1, 2304, pFile_output);
        fclose(pFile_output);
    }


    // test dummy 1x2 stitching aaa ...
    memset(&input_buffer[0][0], 0x0, sizeof(input_buffer[0]));
    memset(&input_buffer[1][0], 0x0, sizeof(input_buffer[1]));

    aaa_stat_data.DstAddr = (uintptr)&output_buffer[0];
    aaa_stat_data.SrcAddr[0] = (uintptr)&input_buffer[0][0];
    aaa_stat_data.SrcAddr[1] = (uintptr)&input_buffer[1][0];
    aaa_stat_data.SrcSliceX = 1;
    aaa_stat_data.SrcSliceY = 2;

    src_aaa_header.stitch_tile_idx_x = 0;
    src_aaa_header.stitch_tile_idx_y = 0;

    memcpy(&input_buffer[0][0], &src_aaa_header, sizeof(ik_3a_header_t));

    src_aaa_header.stitch_tile_idx_x = 0;
    src_aaa_header.stitch_tile_idx_y = 1;
    memcpy(&input_buffer[1][0], &src_aaa_header, sizeof(ik_3a_header_t));

    // test
    Rval |= AmbaIK_TransferCFAAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferCFAAaaStatData dummy 1x2", "OK case in ik_ctest_aaa.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_aaa_cfa_1x2.bin", "wb");
        fwrite(output_buffer, 1, 55042, pFile_output);
        fclose(pFile_output);
    }

    // test
    Rval |= AmbaIK_TransferPGAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferPGAaaStatData dummy 1x2", "OK case in ik_ctest_aaa.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_aaa_pg_1x2.bin", "wb");
        fwrite(output_buffer, 1, 5122, pFile_output);
        fclose(pFile_output);
    }

    // test
    Rval |= AmbaIK_TransCFAHistStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData dummy 1x2", "OK case in ik_ctest_aaa.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_aaa_hist_1x2.bin", "wb");
        fwrite(output_buffer, 1, 2304, pFile_output);
        fclose(pFile_output);
    }

    // test failure cases.
    aaa_stat_data.DstAddr = (uintptr)NULL;
    aaa_stat_data.SrcAddr[0] = (uintptr)NULL;
    aaa_stat_data.SrcSliceX = 1;
    aaa_stat_data.SrcSliceY = 1;

    //test cfa failures
    Rval = AmbaIK_TransferCFAAaaStatData(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "null-ptr fail case in ik_ctest_aaa.c", id_cnt++);

    Rval = AmbaIK_TransferCFAAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "null-ptr fail case in ik_ctest_aaa.c", id_cnt++);

    aaa_stat_data.SrcSliceX = AMBA_IK_3A_SLICE_MAX_COUNT+1;
    Rval = AmbaIK_TransferCFAAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "parameter fail case in ik_ctest_aaa.c", id_cnt++);

    //test pg failures
    aaa_stat_data.SrcSliceX = 1;
    Rval = AmbaIK_TransferPGAaaStatData(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferPGAaaStatData", "null-ptr fail case in ik_ctest_aaa.c", id_cnt++);

    Rval = AmbaIK_TransferPGAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferPGAaaStatData", "null-ptr fail case in ik_ctest_aaa.c", id_cnt++);

    aaa_stat_data.SrcSliceX = AMBA_IK_3A_SLICE_MAX_COUNT+1;
    Rval = AmbaIK_TransferPGAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferPGAaaStatData", "parameter fail case in ik_ctest_aaa.c", id_cnt++);

    //test histogram failures
    aaa_stat_data.SrcSliceX = 1;
    Rval = AmbaIK_TransCFAHistStatData(NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "null-ptr fail case in ik_ctest_aaa.c", id_cnt++);

    Rval = AmbaIK_TransCFAHistStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "null-ptr fail case in ik_ctest_aaa.c", id_cnt++);

    aaa_stat_data.SrcSliceX = AMBA_IK_3A_SLICE_MAX_COUNT+1;
    Rval = AmbaIK_TransCFAHistStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "parameter fail case in ik_ctest_aaa.c", id_cnt++);

    //test header check failures
    aaa_stat_data.DstAddr = (uintptr)&output_buffer[0];
    aaa_stat_data.SrcAddr[0] = (uintptr)&input_buffer[0][0];
    aaa_stat_data.SrcAddr[1] = (uintptr)&input_buffer[1][0];
    aaa_stat_data.SrcSliceX = 2;
    aaa_stat_data.SrcSliceY = 1;

    src_aaa_header.stitch_tile_idx_x = 0;
    src_aaa_header.stitch_tile_idx_y = 0;
    src_aaa_header.awb.awb_tile_num_col = 63;
    src_aaa_header.cfa_ae.ae_tile_num_col = 23;
    src_aaa_header.cfa_af.af_tile_num_col = 23;
    src_aaa_header.pg_ae.ae_tile_num_col = 23;
    src_aaa_header.pg_af.af_tile_num_col = 31;
    memcpy(&input_buffer[0][0], &src_aaa_header, sizeof(ik_3a_header_t));

    src_aaa_header.stitch_tile_idx_x = 1;
    memcpy(&input_buffer[1][0], &src_aaa_header, sizeof(ik_3a_header_t));

    Rval = AmbaIK_TransferCFAAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "source header fail case in ik_ctest_aaa.c", id_cnt++);

    Rval = AmbaIK_TransferPGAaaStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransferPGAaaStatData", "source header fail case in ik_ctest_aaa.c", id_cnt++);

    Rval = AmbaIK_TransCFAHistStatData(&aaa_stat_data);
    LOG_RVAL(pFile, Rval, "AmbaDSP_Image3aStatistic.c", "AmbaIK_TransCFAHistStatData", "source header fail case in ik_ctest_aaa.c", id_cnt++);

    fclose(pFile);

    return Rval;
}

