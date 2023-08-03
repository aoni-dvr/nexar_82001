#include "test_ik_global.h"

#define ALIGN_N(x_, n_)   ((((x_)+((n_)-1)) & ~((n_)-1)))

FILE *pFile;
UINT32 id_cnt;


static INT32 fake_create_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_destroy_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_lock_mutex(AMBA_KAL_MUTEX_t *mutex, UINT32 timeout)
{
    return 0;
}

static INT32 fake_unlock_mutex(AMBA_KAL_MUTEX_t *mutex)
{
    return 0;
}

static INT32 fake_clean_cache(void *addr, size_t size)
{
    return 0;
}

static void* fake_convert_to_physical_address(void* virtual)
{
    return virtual;
}

static void* fake_convert_to_virtual_address(void* phys)
{
    return phys;
}

static void _Init_Image_Kernel(void)
{
    ik_system_api_t sys_api;

    sys_api.system_memcpy = memcpy;
    sys_api.system_memset = memset;
    sys_api.system_memcmp = memcmp;
    sys_api.system_create_mutex = (ik_system_create_mutex_t)fake_create_mutex;
    sys_api.system_destroy_mutex = (ik_system_destroy_mutex_t)fake_destroy_mutex;
    sys_api.system_lock_mutex = (ik_system_lock_mutex_t)fake_lock_mutex;
    sys_api.system_unlock_mutex = (ik_system_unlock_mutex_t)fake_unlock_mutex;
    sys_api.system_print = (ik_system_print_t)printf;
    sys_api.system_clean_cache = (ik_system_clean_cache_t)fake_clean_cache;
    sys_api.system_virtual_to_physical_address = (ik_system_convert_virtual_to_physical_t)fake_convert_to_physical_address;
    sys_api.system_physical_to_virtual_address = (ik_system_convert_physical_to_virtual_t)fake_convert_to_virtual_address;
    ik_import_system_callback_func(&sys_api);

}

static void _Init_Image_Kernel_Core(void)
{
    ikc_system_api_t sys_api;

    sys_api.system_memcpy = memcpy;
    sys_api.system_memset = memset;
    sys_api.system_memcmp = memcmp;
    sys_api.system_print = (ikc_system_print_t)printf;
    ikc_import_system_callback_func(&sys_api);
}

INT32 ik_ctest_remap_main(void)
{
    UINT32 Rval = IK_OK;
    AMBA_IK_IN_CAWARP_PRE_PROC_s CaPreProcIn= {0};
    AMBA_IK_OUT_CAWARP_PRE_PROC_s CaPreProcOut= {0};
    AMBA_IK_IN_WARP_PRE_PROC_s WarpPreProcIn= {0};
    AMBA_IK_OUT_WARP_PRE_PROC_s WarpPreProcOut= {0};
    AMBA_IK_CAWARP_INFO_s CaInpuIinfo= {0};
    AMBA_IK_CAWARP_INFO_s CaResultInfo= {0};
    AMBA_IK_WARP_INFO_s WarpInputInfo= {0};
    AMBA_IK_WARP_INFO_s WarpResultInfo= {0};
    static UINT8 working_buffer[393984];
    static AMBA_IK_GRID_POINT_s input_table_ca_blue[32*48];
    static AMBA_IK_GRID_POINT_s input_table_ca_red[32*48];
    static AMBA_IK_GRID_POINT_s out_table_ca_blue[32*48];
    static AMBA_IK_GRID_POINT_s out_table_ca_red[32*48];
    static AMBA_IK_GRID_POINT_s input_table_warp[128*96];
    static AMBA_IK_GRID_POINT_s output_table_warp[128*96];
    FILE *pFile_input, *pFile_output;

    id_cnt = 0;

    pFile = fopen("data/ik_ctest_remap.txt", "w");

    // init IK
    _Init_Image_Kernel();
    _Init_Image_Kernel_Core();

    //hook Ca input info.
    CaPreProcIn.pWorkingBuffer = working_buffer;
    CaPreProcIn.WorkingBufferSize = sizeof(working_buffer);
    CaPreProcIn.pInpuIinfo = &CaInpuIinfo;
    CaInpuIinfo.pCawarpBlue = input_table_ca_blue;
    CaInpuIinfo.pCawarpRed = input_table_ca_red;
    //hook Ca output info.
    CaPreProcOut.pResultInfo = &CaResultInfo;
    CaResultInfo.pCawarpBlue = out_table_ca_blue;
    CaResultInfo.pCawarpRed = out_table_ca_red;

    //hook Warp input info.
    WarpPreProcIn.pWorkingBuffer = working_buffer;
    WarpPreProcIn.WorkingBufferSize = sizeof(working_buffer);
    WarpPreProcIn.pInputInfo = &WarpInputInfo;
    WarpInputInfo.pWarp = input_table_warp;
    //hook Warp output info.
    WarpPreProcOut.pResultInfo = &WarpResultInfo;
    WarpResultInfo.pWarp = output_table_warp;

    //load Ca tables
    CaInpuIinfo.HorGridNum = 32;
    CaInpuIinfo.VerGridNum = 19;
    CaInpuIinfo.TileWidthExp = 5;
    CaInpuIinfo.TileHeightExp = 5;
    CaInpuIinfo.VinSensorGeo.HSubSample.FactorNum = 1;
    CaInpuIinfo.VinSensorGeo.HSubSample.FactorDen = 1;
    CaInpuIinfo.VinSensorGeo.VSubSample.FactorNum = 1;
    CaInpuIinfo.VinSensorGeo.VSubSample.FactorDen = 1;
    CaInpuIinfo.VinSensorGeo.Width = (CaInpuIinfo.HorGridNum-1) << CaInpuIinfo.TileWidthExp;
    CaInpuIinfo.VinSensorGeo.Height = (CaInpuIinfo.VerGridNum-1) << CaInpuIinfo.TileHeightExp;

    pFile_input = fopen("../util/bin/rotate_32x19_1920x1080_designed_for_coverage.bin", "rb");
    fread(input_table_ca_blue, sizeof(AMBA_IK_GRID_POINT_s), CaInpuIinfo.HorGridNum*CaInpuIinfo.VerGridNum, pFile_input);
    fclose(pFile_input);
    pFile_input = fopen("../util/bin/rotate_32x19_1920x1080_designed_for_coverage.bin", "rb");
    fread(input_table_ca_red, sizeof(AMBA_IK_GRID_POINT_s), CaInpuIinfo.HorGridNum*CaInpuIinfo.VerGridNum, pFile_input);
    fclose(pFile_input);

    //test
    Rval |= AmbaIK_PreProcCAWarpTbl(&CaPreProcIn, &CaPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcCAWarpTbl", "OK case in ik_ctest_remap.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_remap_cawarp_blue.bin", "wb");
        fwrite(out_table_ca_blue, sizeof(AMBA_IK_GRID_POINT_s), CaInpuIinfo.HorGridNum*CaInpuIinfo.VerGridNum, pFile_output);
        fclose(pFile_output);

        pFile_output = fopen("data/ik_ctest_remap_cawarp_red.bin", "wb");
        fwrite(out_table_ca_red, sizeof(AMBA_IK_GRID_POINT_s), CaInpuIinfo.HorGridNum*CaInpuIinfo.VerGridNum, pFile_output);
        fclose(pFile_output);
    }

    //load warp table
    WarpInputInfo.HorGridNum = 32;
    WarpInputInfo.VerGridNum = 19;
    WarpInputInfo.TileWidthExp = 6;
    WarpInputInfo.TileHeightExp = 6;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.Width = (WarpInputInfo.HorGridNum-1) << WarpInputInfo.TileWidthExp;
    WarpInputInfo.VinSensorGeo.Height = (WarpInputInfo.VerGridNum-1) << WarpInputInfo.TileHeightExp;

    //pFile_input = fopen("../util/bin/rotate_45degree_32x19_1920x1080_right_top_hack.bin", "rb");
    pFile_input = fopen("../util/bin/rotate_32x19_1920x1080_designed_for_coverage.bin", "rb");
    fread(input_table_warp, sizeof(AMBA_IK_GRID_POINT_s), WarpInputInfo.HorGridNum*WarpInputInfo.VerGridNum, pFile_input);
    fclose(pFile_input);

    //test
    Rval |= AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "OK case in ik_ctest_remap.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_remap_warp1.bin", "wb");
        fwrite(output_table_warp, sizeof(AMBA_IK_GRID_POINT_s), WarpInputInfo.HorGridNum*WarpInputInfo.VerGridNum, pFile_output);
        fclose(pFile_output);
    }

    //load warp table
    WarpInputInfo.HorGridNum = 41;
    WarpInputInfo.VerGridNum = 43;
    WarpInputInfo.TileWidthExp = 4;
    WarpInputInfo.TileHeightExp = 4;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.Width = (WarpInputInfo.HorGridNum-1) << WarpInputInfo.TileWidthExp;
    WarpInputInfo.VinSensorGeo.Height = (WarpInputInfo.VerGridNum-1) << WarpInputInfo.TileHeightExp;

    pFile_input = fopen("../util/bin/3D_t0_L_main_L_RawEncode.bin", "rb");
    fread(input_table_warp, sizeof(AMBA_IK_GRID_POINT_s), WarpInputInfo.HorGridNum*WarpInputInfo.VerGridNum, pFile_input);
    fclose(pFile_input);

    //test
    Rval |= AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "OK case in ik_ctest_remap.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_remap_warp2.bin", "wb");
        fwrite(output_table_warp, sizeof(AMBA_IK_GRID_POINT_s), WarpInputInfo.HorGridNum*WarpInputInfo.VerGridNum, pFile_output);
        fclose(pFile_output);
    }

    //load warp table
    WarpInputInfo.HorGridNum = 2;
    WarpInputInfo.VerGridNum = 2;
    WarpInputInfo.TileWidthExp = 4;
    WarpInputInfo.TileHeightExp = 4;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.Width = (WarpInputInfo.HorGridNum-1) << WarpInputInfo.TileWidthExp;
    WarpInputInfo.VinSensorGeo.Height = (WarpInputInfo.VerGridNum-1) << WarpInputInfo.TileHeightExp;

    memset(input_table_warp, 0x0, sizeof(input_table_warp));
    input_table_warp[0].Y = 0;
    input_table_warp[1].Y = 0;
    input_table_warp[2].Y = 2<<(WarpInputInfo.TileHeightExp+4);
    input_table_warp[3].Y = 2<<(WarpInputInfo.TileHeightExp+4);

    //test
    Rval |= AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "OK case in ik_ctest_remap.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_remap_warp3.bin", "wb");
        fwrite(output_table_warp, sizeof(AMBA_IK_GRID_POINT_s), WarpInputInfo.HorGridNum*WarpInputInfo.VerGridNum, pFile_output);
        fclose(pFile_output);
    }

    //load warp table
    WarpInputInfo.HorGridNum = 256;
    WarpInputInfo.VerGridNum = 2;
    WarpInputInfo.TileWidthExp = 4;
    WarpInputInfo.TileHeightExp = 4;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.HSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorNum = 1;
    WarpInputInfo.VinSensorGeo.VSubSample.FactorDen = 1;
    WarpInputInfo.VinSensorGeo.Width = (WarpInputInfo.HorGridNum-1) << WarpInputInfo.TileWidthExp;
    WarpInputInfo.VinSensorGeo.Height = (WarpInputInfo.VerGridNum-1) << WarpInputInfo.TileHeightExp;

    memset(input_table_warp, 0x0, sizeof(input_table_warp));

    //test
    Rval |= AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "OK case in ik_ctest_remap.c", id_cnt++);
    {
        //write result
        pFile_output = fopen("data/ik_ctest_remap_warp4.bin", "wb");
        fwrite(output_table_warp, sizeof(AMBA_IK_GRID_POINT_s), WarpInputInfo.HorGridNum*WarpInputInfo.VerGridNum, pFile_output);
        fclose(pFile_output);
    }

    //test failures
    Rval = AmbaIK_PreProcCAWarpTbl(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcCAWarpTbl", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

    Rval = AmbaIK_PreProcWarpTbl(NULL, NULL);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

    //assign Ca overflow grid number
    CaInpuIinfo.HorGridNum = 64+1;

    Rval = AmbaIK_PreProcCAWarpTbl(&CaPreProcIn, &CaPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcCAWarpTbl", "parameter fail case in ik_ctest_remap.c", id_cnt++);

    //assign Warp overflow grid number
    WarpInputInfo.HorGridNum = 256+1;

    Rval = AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "parameter fail case in ik_ctest_remap.c", id_cnt++);

    //remove Ca working buffer.
    CaPreProcIn.WorkingBufferSize = 0;

    Rval = AmbaIK_PreProcCAWarpTbl(&CaPreProcIn, &CaPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcCAWarpTbl", "Invalid buffer size too small fail case in ik_ctest_remap.c", id_cnt++);

    //remove Warp working buffer.
    WarpPreProcIn.WorkingBufferSize = 0;

    Rval = AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "Invalid buffer size too small fail case in ik_ctest_remap.c", id_cnt++);

    //de-hook Ca input/output tbls.
    CaPreProcIn.pWorkingBuffer = NULL;
    CaInpuIinfo.pCawarpBlue = NULL;
    CaInpuIinfo.pCawarpRed = NULL;
    CaResultInfo.pCawarpBlue = NULL;
    CaResultInfo.pCawarpRed = NULL;

    Rval = AmbaIK_PreProcCAWarpTbl(&CaPreProcIn, &CaPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcCAWarpTbl", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

    //de-hook Warp input/output tbls.
    WarpPreProcIn.pWorkingBuffer = NULL;
    WarpInputInfo.pWarp = NULL;
    WarpResultInfo.pWarp = NULL;

    Rval = AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

    //de-hook Ca input/output info.
    CaPreProcIn.pInpuIinfo = NULL;
    CaPreProcOut.pResultInfo = NULL;

    Rval = AmbaIK_PreProcCAWarpTbl(&CaPreProcIn, &CaPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcCAWarpTbl", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

    //de-hook Warp input/output info.
    WarpPreProcIn.pInputInfo = NULL;
    WarpPreProcOut.pResultInfo = NULL;

    Rval = AmbaIK_PreProcWarpTbl(&WarpPreProcIn, &WarpPreProcOut);
    LOG_RVAL(pFile, Rval, "AmbaDSP_ImageUtility.c", "AmbaIK_PreProcWarpTbl", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);


    {
        // test ikc AmbaDSP_IkcUnitRemap.c
        ikc_in_warp_remap_t warp_remap_in = {0};
        ikc_out_warp_remap_t warp_remap_out = {0};
        ik_warp_info_t input_info = {0};
        ik_warp_info_t result_info = {0};

        warp_remap_in.p_input_info = &input_info;
        warp_remap_in.p_working_buffer = working_buffer;
        warp_remap_in.working_buffer_size = sizeof(working_buffer);
        warp_remap_out.p_result_info = &result_info;

        result_info.pwarp = (ik_grid_point_t*)output_table_warp;
        input_info.pwarp = (ik_grid_point_t*)input_table_warp;


        //load warp table
        input_info.hor_grid_num = 32;
        input_info.ver_grid_num = 19;

        input_info.tile_width_exp = 6;
        input_info.tile_height_exp = 6;

        input_info.vin_sensor_geo.h_sub_sample.factor_num = 1;
        input_info.vin_sensor_geo.h_sub_sample.factor_den = 1;
        input_info.vin_sensor_geo.v_sub_sample.factor_num = 1;
        input_info.vin_sensor_geo.v_sub_sample.factor_den = 1;
        input_info.vin_sensor_geo.width = (input_info.hor_grid_num-1)<<input_info.tile_width_exp;
        input_info.vin_sensor_geo.height = (input_info.ver_grid_num-1)<<input_info.tile_height_exp;
        warp_remap_in.vin_sensor_geo = input_info.vin_sensor_geo;

        //zoom-in 1/4 FOV optimize
        warp_remap_in.vin_sensor_geo.start_x = input_info.vin_sensor_geo.width>>2;
        warp_remap_in.vin_sensor_geo.start_y = input_info.vin_sensor_geo.height>>2;
        warp_remap_in.vin_sensor_geo.width = input_info.vin_sensor_geo.width>>2;
        warp_remap_in.vin_sensor_geo.height = input_info.vin_sensor_geo.height>>2;

        pFile_input = fopen("../util/bin/rotate_32x19_1920x1080_designed_for_coverage.bin", "rb");
        fread(input_table_warp, sizeof(AMBA_IK_GRID_POINT_s), input_info.hor_grid_num*input_info.ver_grid_num, pFile_input);
        fclose(pFile_input);

        Rval = ikc_warp_remap(&warp_remap_in, &warp_remap_out);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRemap.c", "ikc_warp_remap", "OK case in ik_ctest_remap.c", id_cnt++);
        {
            //write result
            pFile_output = fopen("data/ik_ctest_remap_warp5.bin", "wb");
            fwrite(output_table_warp, sizeof(AMBA_IK_GRID_POINT_s), input_info.hor_grid_num*input_info.ver_grid_num, pFile_output);
            fclose(pFile_output);
        }

        //assign FOV mismatched with Warp
        input_info.vin_sensor_geo.start_x = warp_remap_in.vin_sensor_geo.start_x+1;

        Rval = ikc_warp_remap(&warp_remap_in, &warp_remap_out);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRemap.c", "ikc_warp_remap", "parameter fail case in ik_ctest_remap.c", id_cnt++);

        //assign FOV binning mode = 0
        warp_remap_in.vin_sensor_geo.h_sub_sample.factor_num = 0;

        Rval = ikc_warp_remap(&warp_remap_in, &warp_remap_out);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRemap.c", "ikc_warp_remap", "parameter fail case in ik_ctest_remap.c", id_cnt++);

        //assign Warp binning mode = 0
        input_info.vin_sensor_geo.h_sub_sample.factor_num = 0;

        Rval = ikc_warp_remap(&warp_remap_in, &warp_remap_out);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRemap.c", "ikc_warp_remap", "parameter fail case in ik_ctest_remap.c", id_cnt++);

        //null input
        Rval = ikc_warp_remap(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRemap.c", "ikc_warp_remap", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

        //null input
        warp_remap_in.p_input_info = NULL;
        warp_remap_out.p_result_info = NULL;

        Rval = ikc_warp_remap(&warp_remap_in, &warp_remap_out);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRemap.c", "ikc_warp_remap", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

        //null input
        Rval = ikc_cawarp_remap(NULL, NULL);
        LOG_RVAL(pFile, Rval, "AmbaDSP_IkcUnitRemap.c", "ikc_cawarp_remap", "null-ptr fail case in ik_ctest_remap.c", id_cnt++);

    }

    fclose(pFile);

    fake_create_mutex(NULL);
    fake_destroy_mutex(NULL);
    fake_lock_mutex(NULL, 0);
    fake_unlock_mutex(NULL);
    fake_clean_cache(NULL, 0);
    fake_convert_to_physical_address(NULL);
    fake_convert_to_virtual_address(NULL);

    return Rval;
}

