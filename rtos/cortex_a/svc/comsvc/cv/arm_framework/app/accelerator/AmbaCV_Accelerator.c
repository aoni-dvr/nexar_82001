/*
* Copyright (c) 2020 Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

//#include <stdint.h>
//#include <stdio.h>
//#include "string.h"
#include "AmbaKAL.h"
#include "ambint.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaSYS.h"
#include "AmbaNVM_Partition.h"
#include "AmbaMisraFix.h"
#include "AmbaCache.h"
#include "cvapi_ambacv_accelerator.h"
#include "cvapi_memio_interface.h"
#include "cvtask_ossrv.h"

//#if defined (ACC_20200904)
//#define HEADER_VER 1UL
//#endif
//#if defined (HEADER_VER)
#define ACC_LOAD_ON_CALL ((uint32_t)0)
#define ACC_LOAD_ON_INIT ((uint32_t)1)

#define ACC_ERRCODE_NONE ((uint32_t)0)
#define ACC_ERRCODE_MEMORY_NOT_ENOUGH ACC_ERR_0001
#define ACC_ERRCODE_INVALID_INPUTS ACC_ERR_0002
#define ACC_ERRCODE_MEMORY_NOT_ALIGN ACC_ERR_0003
#define ACC_ERRCODE_ACC_NOT_SUPPORT ACC_ERR_0004

#ifndef DC_U
#define DC_U    ((uint32_t)9999)
#define DC_S    NULL
#define ACC_AUTO_COM     ((uint32_t)0)
#endif

//extern uint64_t ambacv_p2c(uint64_t pa);
//#if defined(CONFIG_SOC_CV2)
//#define chip_cv2 1UL
//#endif

#if !defined(CONFIG_BUILD_CV_THREADX) && !defined(CONFIG_QNX)
static ULONG ACC_CacheDataClean(const char* VirtAddr, ULONG Size)
{
    //Do not thing
    return 0UL;
}
#else
static ULONG ACC_CacheDataClean(const char* VirtAddr, ULONG Size)
{
    ULONG tmp_addr;
    UINT32 ret = ACC_ERRCODE_NONE;
    ret = AmbaWrap_memcpy(&tmp_addr, &VirtAddr, sizeof(ULONG));
    if(ret == ACC_ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("[ACC][LOG] ACC_CacheDataClean success\n", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    return AmbaCache_DataClean(tmp_addr, Size);
}
#endif


typedef struct {
    uint32_t ID;
    char file_path[64UL];
    uint32_t BinSize;
    uint32_t StateBufferSize;
    uint32_t IsInit;
} ACC_FlexiBinInfo_s;

static ACC_FlexiBinInfo_s bin_table_info[] = {
    {Acc_Custom, "Custom.bin", ACC_AUTO_COM, 9216, 0},
    {ACC_RESIZE_Y_U8_640x360_1280x720, "resizeY_u8_420_640x360_1280x720.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_RESIZE_Y_U8_420_1280x720_640x360, "resizeY_u8_420_1280x720_640x360.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_RESIZE_UV_U8_420_640x360_1280x720, "resizeUV_u8_420_640x360_1280x720.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_RESIZE_UV_U8_420_1280x720_640x360, "resizeUV_u8_420_1280x720_640x360.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_MIN_MAX_LOC_U8_1280x720, "minmaxloc_u8_1280x720.bin", ACC_AUTO_COM, 6528, 0},
    {ACC_MIN_MAX_LOC_U16_1280x720, "minmaxloc_u16_1280x720.bin", ACC_AUTO_COM, 6528, 0},
    {ACC_AND_U8_TWO_POW_19, "and_u8_2pow19.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_AND_U8_TWO_POW_17, "and_u8_2pow17.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_AND_U8_TWO_POW_15, "and_u8_2pow15.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_19, "cmp_neq_u8_2pow19.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_17, "cmp_neq_u8_2pow17.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_NOT_EQUAL_U8_TWO_POW_15, "cmp_neq_u8_2pow15.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_GREATER_S8_TWO_POW_19, "cmp_grt_s8_2pow19.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_GREATER_S8_TWO_POW_17, "cmp_grt_s8_2pow17.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_GREATER_S8_TWO_POW_15, "cmp_grt_s8_2pow15.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_NEQ_U16_TWO_POW_18, "cmp_neq_u16_2pow18.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_NEQ_U16_TWO_POW_16, "cmp_neq_u16_2pow16.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CMP_NEQ_U16_TWO_POW_14, "cmp_neq_u16_2pow14.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DIVIDE_U8_TWO_POW_18, "divide_u8_2pow18.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DIVIDE_U8_TWO_POW_16, "divide_u8_2pow16.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DIVIDE_U8_TWO_POW_14, "divide_u8_2pow14.bin", ACC_AUTO_COM, 4480, 0},
    {ACC_DIVIDE_U16_TWO_POW_17, "divide_u16_2pow17.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DIVIDE_U16_TWO_POW_15, "divide_u16_2pow15.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DIVIDE_U16_TWO_POW_13, "divide_u16_2pow13.bin", ACC_AUTO_COM, 4480, 0},
    {ACC_DIVIDE_S16_TWO_POW_17, "divide_s16_2pow17.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DIVIDE_S16_TWO_POW_15, "divide_s16_2pow15.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DIVIDE_S16_TWO_POW_13, "divide_s16_2pow13.bin", ACC_AUTO_COM, 4480, 0},
    {ACC_THRES_BINARY_U8_TWO_POW18, "thres_binary_u8_2pow18.bin", ACC_AUTO_COM, 5504, 0},
    {ACC_THRES_BINARY_U8_TWO_POW14, "thres_binary_u8_2pow14.bin", ACC_AUTO_COM, 5504, 0},
    {ACC_THRES_BINARY_F32_TWO_POW17, "thres_binary_f32_2pow17.bin", ACC_AUTO_COM, 5504, 0},
    {ACC_THRES_BINARY_F32_TWO_POW13, "thres_binary_f32_2pow13.bin", ACC_AUTO_COM, 5504, 0},
    {ACC_THRES_TOZERO_F32_TWO_POW17, "thres_tozero_f32_2pow17.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_THRES_TOZERO_F32_TWO_POW13, "thres_tozero_f32_2pow13.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_REDUCE_AVG_H_U8_1280x720, "reduce_avg_h_u8_1280x720.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_REDUCE_SUM_H_U8_1280x720, "reduce_sum_h_u8_1280x720.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_REDUCE_MIN_W_U8_1280x720, "reduce_min_w_u8_1280x720.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_REDUCE_MAX_W_U8_1280x720, "reduce_max_w_u8_1280x720.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_SPLIT_U8_1280x720x3, "split_u8_1280x720x3.bin", ACC_AUTO_COM, 5504, 0},
    {ACC_ERODE_U8_640x480, "erode_u8_640x480.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_TRANSFORM_U8_640x480, "transform_u8_640x480.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_DILATE_U8_640x480, "dilate_u8_640x480.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_MERGE_U8_1280x720x3, "merge_u8_1280x720x3.bin", ACC_AUTO_COM, 5504, 0},
    {ACC_MIXCHANNELS_U8_640x480, "mix_channels_u8_640x480.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_DIV_F32_2496, "divide_f32_pad32_46x54.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_LOG_U8_TWO_POW19, "log8u_2paw19.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_LOG_U8_TWO_POW17, "log8u_2paw17.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_LOG_U8_TWO_POW15, "log8u_2paw15.bin", ACC_AUTO_COM, 3328, 0},
    {ACC_CONV_U8_VGA, "conv8u_640x480_k9x9.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_CONV_U8_HD, "conv8u_1280x720_k9x9.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_FINDNZ_U8_320x180, "find_nz_u8_320x180.bin", ACC_AUTO_COM, 4736, 0},
    {ACC_YUV2RGB_U8_HD, "yuv2rgb_u8_1280x720.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_GAUSSIAN_U8_VGA, "conv8u_640x480_k9x9.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_GAUSSIAN_U8_HD, "conv8u_1280x720_k9x9.bin", ACC_AUTO_COM, 4352, 0},
    {ACC_WARP_Y_640x480, "warp_rotate_y_640x480.bin", ACC_AUTO_COM, 5504, 0},
    {ACC_WARP_UV_640x480, "warp_rotate_uv_640x480.bin", ACC_AUTO_COM, 5504, 0},
#if !defined(CONFIG_SOC_CV28)
    {ACC_ScaleOSD, "scale_OSD.bin", ACC_AUTO_COM, 9216, 0},
#endif
    {ACC_ERODE_640x480_RECT3X3, "erode_640x480_rec3x3.bin", ACC_AUTO_COM, 9216, 0},
    {ACC_DILATE_640x480_RECT3X3, "dilate_640x480_rec3x3.bin", ACC_AUTO_COM, 9216, 0},
    {ACC_RGB2YUV_U8_HD, "rgb2yuv_u8_1280x720.bin", ACC_AUTO_COM, 9216, 0},
    {ACC_GEMM, "gemm.bin", ACC_AUTO_COM, 9216, 0},
};

//#define ACC_Temp_Buffer 468352UL
static flexidag_memblk_t g_temp_accbuf;
static flexidag_memblk_t global_buf;
static AMBA_CV_FLEXIDAG_HANDLE_s fd_gen_handle[ACC_DAG_ID_MAX];
static uint32_t LoadDagTiming = ACC_LOAD_ON_CALL;
//#define LoadDagTiming ACC_LOAD_ON_CALL
static ULONG acc_dbg_mode = 0UL;

static uint32_t ACC_REQUIRED_BUFFER_SIZE = ACC_Temp_Buffer + ACC_Debug_Mode_Buffer;
void ACC_SetDbgMode(UINT32 dbg_switch)
{
    if(dbg_switch > 0UL) {
        acc_dbg_mode = 1UL;
        AmbaPrint_PrintUInt5("[ACC][LOG] ACC_SetDbgMode dbgmode on!\n", DC_U, DC_U, DC_U, DC_U, DC_U);
    } else {
        acc_dbg_mode = 0UL;
        AmbaPrint_PrintUInt5("[ACC][LOG] ACC_SetDbgMode dbgmode off!\n", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
}
static uint32_t Get128_Aligned(uint32_t input)
{
    return input + (uint32_t)(0x80U-(input & 0x7fU));
}
void AmbaACC_RequireBufferSize_Custom(UINT32 bin_size, const char *file_path, UINT32 path_len, UINT32 *query_size)
{
    UINT32 ret = ACC_ERRCODE_NONE;
    *query_size = bin_size + ACC_REQUIRED_BUFFER_SIZE;
    bin_table_info[0].BinSize = bin_size;
    ret = AmbaWrap_memcpy(bin_table_info[0].file_path, file_path, path_len);
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] AmbaACC_RequireBufferSize_Custom get path fail\n",DC_S,DC_S,DC_S,DC_S,DC_S);
    }
    ACC_REQUIRED_BUFFER_SIZE += bin_size;
    AmbaPrint_PrintUInt5("[ACC][LOG] AmbaACC_RequireBufferSize_Custom size %d!\n", *query_size, DC_U, DC_U, DC_U, DC_U);
    AmbaPrint_PrintStr5("[ACC][LOG] AmbaACC_RequireBufferSize_Custom binpath %s!\n", bin_table_info[0].file_path, DC_S, DC_S, DC_S, DC_S);
}
uint32_t AmbaACC_RequireBufferSize_Query(UINT32 *query_size)
{
    UINT32 NvmID, FileSize = 0, Sum = 0, i;
    uint32_t ret = ACC_ERRCODE_NONE;

    ret |= AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_NVM_ROM_REGION_SYS_DATA);
    if (ret != 0U) {
        AmbaPrint_PrintUInt5("[ACC][LOG] AmbaACC_RequireBufferSize_Query: AmbaSvcWrap_GetNVMID failed!\n", 0U, 0U, 0U, 0U, 0U);
    } else {
        //except custom case
        for (i = 1U; i < (sizeof(bin_table_info) / sizeof(bin_table_info[0])); i++) {
            ret |= AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, bin_table_info[i].file_path, &FileSize);
            bin_table_info[i].BinSize = (UINT32)(Get128_Aligned(FileSize) & 0xFFFFFFFFU);
            Sum += bin_table_info[i].BinSize;
            Sum += bin_table_info[i].StateBufferSize;
        }
        *query_size = Sum + ACC_REQUIRED_BUFFER_SIZE;
        ACC_REQUIRED_BUFFER_SIZE += Sum;
        AmbaPrint_PrintUInt5("[ACC][LOG] AmbaACC_RequireBufferSize_Query %d and (rc=0x%x)!\n", *query_size, (UINT32)ret, DC_U, DC_U, DC_U);
    }

    return ret;
}

static UINT32 AmbaACC_selectIDLoc(uint32_t ID)
{
    uint32_t i;
    UINT32 ID_select = ACC_DAG_ID_MAX;
    for (i = 0U; i < (sizeof(bin_table_info) / sizeof(bin_table_info[0])); i++) {
        if(bin_table_info[i].ID == ID) {
            ID_select = i;
            break;
        }
    }
    return ID_select;
}
/*uint32_t AmbaACC_SetPitch(UINT32 ID, uint32_t pitch){

}
uint32_t AmbaACC_SetOffset(){

}*/
static uint32_t ACC_MemblkAlloc(uint32_t ReqSize, flexidag_memblk_t *pReqBuf)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    static uint32_t UsedBufferSize = 0;
    if ((ReqSize & 0x7fUL) != 0UL) {
        AmbaPrint_PrintUInt5("[ACC][ERROR] AmbaACC_MemblkAlloc(ReqSize %d) is not 128-aligned !\n", ReqSize, DC_U, DC_U, DC_U, DC_U);
        ret = ACC_ERRCODE_MEMORY_NOT_ALIGN;
    } else {
        pReqBuf->buffer_size = ReqSize;
        if ((pReqBuf->buffer_size + UsedBufferSize) >= global_buf.buffer_size) {
            AmbaPrint_PrintUInt5("AmbaACC_MemblkAlloc : Out of memory (size = %d, curr = %d, total_size = %d\n", pReqBuf->buffer_size, UsedBufferSize, global_buf.buffer_size, DC_U,DC_U);
            ret = ACC_ERRCODE_MEMORY_NOT_ENOUGH;
        } else {
            pReqBuf->buffer_daddr  = global_buf.buffer_daddr + UsedBufferSize;
            pReqBuf->buffer_cacheable = global_buf.buffer_cacheable;
            pReqBuf->pBuffer         = &global_buf.pBuffer[UsedBufferSize];
            UsedBufferSize += pReqBuf->buffer_size;
        }
    }

    return ret;
}
static uint32_t ACC_FlexidagSetParam(uint32_t ID)
{
    AMBA_CV_FLEXIDAG_LOG_MSG_s set;
    const AMBA_CV_FLEXIDAG_LOG_MSG_s *pSet;
    uint32_t ret;
    const void* pVoid = NULL;

    set.flexidag_msg_entry = 0U;       //no requirement to send msg from flexidag to cvtask
    set.cvtask_msg_entry = 0U;         //no requirement to send internal cvtask msg
    set.arm_cpu_map = 0xFU;
    set.arm_log_entry = 0U;
    set.orc_log_entry = 0U;
    pSet = &set;
    AmbaMisra_TypeCast(&pVoid, &pSet);
    ret = AmbaCV_FlexidagSetParamSet(&fd_gen_handle[ID], FLEXIDAG_PARAMSET_LOG_MSG, pVoid, (uint32_t)sizeof(AMBA_CV_FLEXIDAG_LOG_MSG_s));
    return ret;
}
static void ACC_FlexidagSetInput(const AMBA_CV_FLEXIDAG_IO_s *Input)
{
    uint32_t j, ret = 0;
    const memio_source_recv_raw_t *pSrc;
    uint64_t ul_addr, ul_size=0;
    ULONG addr;
    for(j = 0; j < Input->num_of_buf; j++) {
        AmbaMisra_TypeCast(&pSrc, &Input->buf[j].pBuffer);
        ul_addr = ambacv_p2c((uint64_t)(pSrc->addr));
        ul_size = (uint64_t)pSrc->size;
        ret |= AmbaCache_DataClean((ULONG)ul_addr, (ULONG)ul_size);
        AmbaMisra_TypeCast(&addr, &Input->buf[j].pBuffer);
        ret |= AmbaCache_DataClean(addr, Input->buf[j].buffer_size);
        ret |= AmbaCache_DataClean(Input->buf[j].buffer_daddr, Input->buf[j].buffer_size);
    }
    if(ret == 0U) {
        AmbaPrint_PrintUInt5("[ACC][LOG]: ambacv_p2c success.",DC_U,DC_U,DC_U,DC_U,DC_U);
    }
}
static uint32_t ACC_InitDag(uint32_t ID)
{
    uint32_t ret = ACC_ERRCODE_NONE, ID_select;
    AMBA_CV_FLEXIDAG_INIT_s init;
    UINT32 BootMode, NvmID;
    UINT8 *pU8;
    static flexidag_memblk_t g_bin_buf[ACC_DAG_ID_MAX];
    ID_select = AmbaACC_selectIDLoc(ID);
    if (ID_select != ACC_DAG_ID_MAX) {
        AmbaPrint_PrintUInt5("[ACC][LOG] Binsize %d ID_select %d",bin_table_info[ID_select].BinSize, ID_select, DC_U, DC_U, DC_U);
        if ((bin_table_info[ID_select].BinSize & 0x7fUL) != 0UL) {
            AmbaPrint_PrintUInt5("[ACC][ERROR] AmbaACC_InitDag(bin_table_info[%d].BinSize %d) is not 128-aligned !\n", ID_select, bin_table_info[ID_select].BinSize, DC_U, DC_U, DC_U);
            ret |= ACC_ERRCODE_MEMORY_NOT_ALIGN;
        }
        if ((bin_table_info[ID_select].StateBufferSize & 0x7fUL) != 0UL) {
            AmbaPrint_PrintUInt5("[ACC][ERROR] AmbaACC_InitDag(bin_table_info[%d].StateBufferSize %d) is not 128-aligned !\n", ID_select, bin_table_info[ID_select].StateBufferSize, DC_U, DC_U, DC_U);
            ret |= ACC_ERRCODE_MEMORY_NOT_ALIGN;
        }
        ret |= ACC_MemblkAlloc(Get128_Aligned(bin_table_info[ID_select].BinSize), &g_bin_buf[ID_select]);
        if (ret == ACC_ERRCODE_NONE) {
            ret |= ACC_MemblkAlloc(Get128_Aligned(bin_table_info[ID_select].StateBufferSize), &init.state_buf);
            if (ret == ACC_ERRCODE_NONE) {
                //ret = AmbaCV_UtilityFileLoad("c:\\flexibin01_save.bin", &g_bin_buf[ID]);
                ret |= AmbaSYS_GetBootMode(&BootMode);
                if (BootMode == AMBA_SYS_BOOT_FROM_NAND) {
                    NvmID = AMBA_NVM_NAND;
                } else if (BootMode == AMBA_SYS_BOOT_FROM_EMMC) {
                    NvmID = AMBA_NVM_eMMC;
                } else if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NAND) {
                    NvmID = AMBA_NVM_SPI_NAND;
                } else {
                    NvmID = AMBA_NVM_SPI_NOR;
                }
                if (ret == ACC_ERRCODE_NONE) {
                    UINT32 FileSize;
                    ret = AmbaWrap_memcpy(&pU8, &g_bin_buf[ID_select].pBuffer, sizeof(void*));
                    if(ID_select == 0UL) {
                        ret |= AmbaCV_UtilityFileLoad(bin_table_info[ID_select].file_path, &g_bin_buf[ID_select]);
                        ret |= (uint32_t)ACC_CacheDataClean(g_bin_buf[ID_select].pBuffer, g_bin_buf[ID_select].buffer_size);
                    } else {
                        ret |= AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, bin_table_info[ID_select].file_path, &FileSize);
                        if(ret == ACC_ERRCODE_NONE) {
                            ret |= AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, bin_table_info[ID_select].file_path, 0, FileSize, pU8, 5000U);
                        }
                    }
                }
                if (ret == ACC_ERRCODE_NONE) {
                    AmbaPrint_PrintStr5("[ACC] open >> %s <<\n", bin_table_info[ID_select].file_path, DC_S, DC_S, DC_S, DC_S);
                    ret |= AmbaCV_FlexidagOpen(&g_bin_buf[ID_select], &fd_gen_handle[ID_select]);
                    if (ret == ACC_ERRCODE_NONE) {
                        ret = AmbaWrap_memcpy(&init.temp_buf, &g_temp_accbuf, sizeof(flexidag_memblk_t));
                        ret |= AmbaCV_FlexidagInit(&fd_gen_handle[ID_select], &init);
                        bin_table_info[ID_select].IsInit = (uint32_t)1U;
                    } else {
                        AmbaPrint_PrintStr5("[ACC][ERROR] open >> %s << FAIL\n", bin_table_info[ID_select].file_path, DC_S, DC_S, DC_S, DC_S);
                    }
                } else {
                    AmbaPrint_PrintUInt5("[ACC][ERROR] ID %d load file fail \n", ID_select, DC_U, DC_U, DC_U, DC_U);
                }
            } else {
                AmbaPrint_PrintUInt5("[ACC][ERROR] ID %d init state buffer fail \n", ID_select, DC_U, DC_U, DC_U, DC_U);
            }
        } else {
            AmbaPrint_PrintUInt5("[ACC][ERROR] ID %d init bin buffer fail \n", ID_select, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return ret;
}

uint32_t AmbaACC_Init(const flexidag_memblk_t *pBuffer, uint32_t InitMode)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    uint32_t i;
    uint64_t tmp_addr;
    static uint32_t temp_init = 0U;
    ret = AmbaWrap_memcpy(&global_buf, pBuffer, sizeof(flexidag_memblk_t));
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("pBuffer memcpy fail\n", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    ret |= AmbaWrap_memcpy(&tmp_addr, &global_buf.pBuffer, sizeof(void*));
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintUInt5("global_buf memcpy fail\n", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    if((tmp_addr & 0x7fUL) != 0UL) {
        AmbaPrint_PrintUInt5("[ACC][ERROR] AmbaACC_Init fail ! pBuffer addr 0x%x is not aligned \n", (uint32_t)tmp_addr, DC_U, DC_U, DC_U, DC_U);
        ret = ACC_ERRCODE_MEMORY_NOT_ALIGN;
    } else {
        if (ACC_REQUIRED_BUFFER_SIZE == 0UL) {
            AmbaPrint_PrintUInt5("[ACC][ERROR] AmbaACC_Init query size not set, do AmbaACC_RequireBufferSize_Query first please\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
        if (ACC_REQUIRED_BUFFER_SIZE < pBuffer->buffer_size) {
            AmbaPrint_PrintUInt5("[ACC][ERROR] AmbaACC_Init required size is not enough, req %lu > size %d\n", pBuffer->buffer_size, ACC_REQUIRED_BUFFER_SIZE, DC_U, DC_U, DC_U);
        }
        if(temp_init == 0U) {
            ret = ACC_MemblkAlloc(ACC_Temp_Buffer, &g_temp_accbuf);
            temp_init = 1U;
        }
        if(InitMode <= 1UL) {
            LoadDagTiming = InitMode;
        }
        if(LoadDagTiming == ACC_LOAD_ON_INIT) {
            if(acc_dbg_mode == 1UL) {
                ret |= ACC_FlexidagSetParam(0);
                ret |= ACC_InitDag(0);
            } else {
                for (i = 1U; i < ACC_DAG_ID_MAX; i++) {
                    ret |= ACC_FlexidagSetParam(i);
                    ret |= ACC_InitDag(i);
                }
            }
        }
    }
    return ret;
}
uint32_t AmbaACC_Uninit(void)
{
    return 0;
}

static uint32_t ACC_CheckResizeYU8NHD2HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*360UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunResizeYU8NHD2HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_RESIZE_Y_U8_640x360_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckResizeYU8NHD2HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckResizeYU8HD2NHD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (640UL*360UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunResizeYU8HD2NHD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_RESIZE_Y_U8_420_1280x720_640x360);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckResizeYU8HD2NHD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckResizeUVU8NHD2HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*360UL/2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1280UL*720UL/2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunResizeUVU8NHD2HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_RESIZE_UV_U8_420_640x360_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckResizeUVU8NHD2HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckResizeUVU8HD2NHD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL/2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (640UL*360UL/2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunResizeUVU8HD2NHD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_RESIZE_UV_U8_420_1280x720_640x360);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckResizeUVU8HD2NHD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
static uint32_t ACC_CheckMinMaxLocU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret = AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < 64UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
        if(Output->buf[1].buffer_size < 32UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[1] buffer size %d \n",Output->buf[1].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[1].pBuffer, 0x0, Output->buf[1].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[1].pBuffer, Output->buf[1].buffer_size);
        }
        if(Output->buf[2].buffer_size < 64UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[2] buffer size %d \n",Output->buf[2].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[2].pBuffer, 0x0, Output->buf[2].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[2].pBuffer, Output->buf[2].buffer_size);
        }
        if(Output->buf[3].buffer_size < 32UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[3] buffer size %d \n",Output->buf[3].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[3].pBuffer, 0x0, Output->buf[3].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[3].pBuffer, Output->buf[3].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunMinMaxLocU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_MIN_MAX_LOC_U8_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckMinMaxLocU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckMinMaxLocU16HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret = AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL*2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < 64UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
        if(Output->buf[1].buffer_size < 32UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[1] buffer size %d \n",Output->buf[1].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[1].pBuffer, 0x0, Output->buf[1].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[1].pBuffer, Output->buf[1].buffer_size);
        }
        if(Output->buf[2].buffer_size < 64UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[2] buffer size %d \n",Output->buf[2].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[2].pBuffer, 0x0, Output->buf[2].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[2].pBuffer, Output->buf[2].buffer_size);
        }
        if(Output->buf[3].buffer_size < 32UL) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[3] buffer size %d \n",Output->buf[3].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[3].pBuffer, 0x0, Output->buf[3].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[3].pBuffer, Output->buf[3].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunMinMaxLocU16HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_MIN_MAX_LOC_U16_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckMinMaxLocU16HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckAndU8TwoPow19(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunAndU8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_AND_U8_TWO_POW_19);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckAndU8TwoPow19(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckAndU8TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunAndU8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_AND_U8_TWO_POW_17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckAndU8TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckAndU8TwoPow15(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunAndU8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_AND_U8_TWO_POW_15);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckAndU8TwoPow15(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpNotEqualU8TwoPow19(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunCmpNeqU8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_NOT_EQUAL_U8_TWO_POW_19);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpNotEqualU8TwoPow19(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpNeqU8TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunCmpNeqU8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_NOT_EQUAL_U8_TWO_POW_17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpNeqU8TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpNeqU8TwoPow15(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunCmpNeqU8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_NOT_EQUAL_U8_TWO_POW_15);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpNeqU8TwoPow15(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpGreaterS8TwoPow19(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunCmpGtrS8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_GREATER_S8_TWO_POW_19);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpGreaterS8TwoPow19(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpGtrS8TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunCmpGtrS8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_GREATER_S8_TWO_POW_17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpGtrS8TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpGtrS8TwoPow15(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunCmpGtrS8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_GREATER_S8_TWO_POW_15);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpGtrS8TwoPow15(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpNeqU16TwoPow18(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<19UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunCmpNeqU16TwoPow18(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_NEQ_U16_TWO_POW_18);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpNeqU16TwoPow18(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpNeqU16TwoPow16(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<17UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunCmpNeqU16TwoPow16(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_NEQ_U16_TWO_POW_16);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpNeqU16TwoPow16(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckCmpNeqU16TwoPow14(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<15UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunCmpNeqU16TwoPow14(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CMP_NEQ_U16_TWO_POW_14);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckCmpNeqU16TwoPow14(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideU8TwoPow18(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunDivideU8TwoPow18(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_U8_TWO_POW_18);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideU8TwoPow18(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideU8TwoPow16(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivideU8TwoPow16(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_U8_TWO_POW_16);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideU8TwoPow16(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideU8TwoPow14(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivideU8TwoPow14(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_U8_TWO_POW_14);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideU8TwoPow14(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideU16TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivideU16TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_U16_TWO_POW_17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideU16TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideU16TwoPow15(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivideU16TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_U16_TWO_POW_15);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideU16TwoPow15(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideU16TwoPow13(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivideU16TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_U16_TWO_POW_13);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideU16TwoPow13(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideS16TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivideS16TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_S16_TWO_POW_17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideS16TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideS16TwoPow15(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<16UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivideS16TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_S16_TWO_POW_15);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideS16TwoPow15(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDivideS16TwoPow13(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        if(Output->buf[0].buffer_size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunDivideS16TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIVIDE_S16_TWO_POW_13);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivideS16TwoPow13(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckThsBinU8TwoPow18(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        /*(void)AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < 1UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        (void)AmbaWrap_memcpy(&pSrc, &Input->buf[2].pBuffer, sizeof(void*));
        if(pSrc->size < 1UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[2] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }*/
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1UL<<18UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunThresBinU8TwoPow18(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_THRES_BINARY_U8_TWO_POW18);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckThsBinU8TwoPow18(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckThsBinU8TwoPow14(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        /*(void)AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < 1UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        (void)AmbaWrap_memcpy(&pSrc, &Input->buf[2].pBuffer, sizeof(void*));
        if(pSrc->size < 1UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[2] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }*/
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1UL<<14UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunThresBinU8TwoPow14(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_THRES_BINARY_U8_TWO_POW14);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckThsBinU8TwoPow14(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckThsBinF32TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<(17UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        /*(void)AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < 4UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        (void)AmbaWrap_memcpy(&pSrc, &Input->buf[2].pBuffer, sizeof(void*));
        if(pSrc->size < 4UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[2] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }*/
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1UL<<(17UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunThresBinF32TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_THRES_BINARY_F32_TWO_POW17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckThsBinF32TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckThsBinF32TwoPow13(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<(13UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        /*(void)AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < 4UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        (void)AmbaWrap_memcpy(&pSrc, &Input->buf[2].pBuffer, sizeof(void*));
        if(pSrc->size < 4UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[2] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }*/
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1UL<<(13UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunThresBinF32TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_THRES_BINARY_F32_TWO_POW13);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckThsBinF32TwoPow13(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckThsTo0F32TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<(17UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        /*(void)AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < 4UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }*/
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1UL<<(17UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunThresTo0F32TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_THRES_TOZERO_F32_TWO_POW17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckThsTo0F32TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckThsTo0F32TwoPow13(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1UL<<(13UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        /*(void)AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < 4UL){
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }*/
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1UL<<(13UL+2UL))) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunThresTo0F32TwoPow13(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_THRES_TOZERO_F32_TWO_POW13);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckThsTo0F32TwoPow13(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckReduceAvgHtU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1280UL*4UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunReduceAvgHeightU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_REDUCE_AVG_H_U8_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckReduceAvgHtU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckReduceSumHtU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1280UL*4UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunReduceSumHeightU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_REDUCE_SUM_H_U8_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckReduceSumHtU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckReduceMinWthU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunReduceMinWidthU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_REDUCE_MIN_W_U8_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckReduceMinWthU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckReduceMaxWthU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunReduceMaxWidthU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_REDUCE_MAX_W_U8_1280x720);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckReduceMaxWthU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckSplitU8HDC3(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret = AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL*3UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 3UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
        if(Output->buf[1].buffer_size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[1] buffer size %d \n",Output->buf[1].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[1].pBuffer, 0x0, Output->buf[1].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[1].pBuffer, Output->buf[1].buffer_size);
        }
        if(Output->buf[2].buffer_size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[2] buffer size %d \n",Output->buf[2].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[2].pBuffer, 0x0, Output->buf[2].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[2].pBuffer, Output->buf[2].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunSplitU8HDC3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_SPLIT_U8_1280x720x3);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckSplitU8HDC3(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckErodeU8VGA(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (32UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunErodeU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_ERODE_U8_640x480);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckErodeU8VGA(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckTransformU8VGA(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*480UL*3UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (64UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*480UL*3UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunTransformU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_TRANSFORM_U8_640x480);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckTransformU8VGA(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckDilateU8VGA(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (32UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDilateU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DILATE_U8_640x480);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDilateU8VGA(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckMergeU8HDC3(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 3UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[2].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[2] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1280UL*720UL*3UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunMergeU8HDC3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_MERGE_U8_1280x720x3);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckMergeU8HDC3(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckMixChannelsBgra2RgbU8VGA(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*480UL*4UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*480UL*3UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunMixChannelsBgra2RgbU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_MIXCHANNELS_U8_640x480);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckMixChannelsBgra2RgbU8VGA(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckDivF32PAD32_46x54(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (2496UL*4UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (2496UL*4UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (2496UL*4UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDivF32PAD32_46x54(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DIV_F32_2496);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDivF32PAD32_46x54(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckLogU8TwoPow19(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (524288UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1048576UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunLogU8TwoPow19(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_LOG_U8_TWO_POW19);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckLogU8TwoPow19(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckLogU8TwoPow17(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (131072UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (262144UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunLogU8TwoPow17(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_LOG_U8_TWO_POW17);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckLogU8TwoPow17(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckLogU8TwoPow15(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (32768UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (65536UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunLogU8TwoPow15(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_LOG_U8_TWO_POW15);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckLogU8TwoPow15(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckConvU8VGA(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (307200UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (256UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (307200UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunConvU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CONV_U8_VGA);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckConvU8VGA(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckConvU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (921600UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (256UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (921600UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunConvU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_CONV_U8_HD);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckConvU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckFindNZU8_320x180(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (320UL*180UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (32UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
        if(Output->buf[1].buffer_size < (320UL*180UL*4UL*2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[1].pBuffer, 0x0, Output->buf[1].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[1].pBuffer, Output->buf[1].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunFindNZU8_320x180(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_FINDNZ_U8_320x180);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckFindNZU8_320x180(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static inline uint32_t ACC_CheckYUV2RGBU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0](y) buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*360UL*2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1](uv) buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1280UL*720UL*3UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunYUV2RGBU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_YUV2RGB_U8_HD);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckYUV2RGBU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunGaussianBlurU8VGA(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    uint32_t misra_u32;
    if ((Input==Output)) {
        // misraC
    }
    misra_u32 = Input->num_of_buf;
    Input->num_of_buf = misra_u32;
    misra_u32 = Output->num_of_buf;
    Output->num_of_buf = misra_u32;
    AmbaPrint_PrintStr5("[ACC][ERROR] %s() not support \n",__func__, DC_S, DC_S, DC_S, DC_S);
    ret = ACC_ERRCODE_ACC_NOT_SUPPORT;
    return ret;
}

uint32_t AmbaACC_RunGaussianBlurU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    uint32_t misra_u32;
    if ((Input==Output)) {
        // misraC
    }
    misra_u32 = Input->num_of_buf;
    Input->num_of_buf = misra_u32;
    misra_u32 = Output->num_of_buf;
    Output->num_of_buf = misra_u32;
    AmbaPrint_PrintStr5("[ACC][ERROR] %s() not support \n",__func__, DC_S, DC_S, DC_S, DC_S);
    ret = ACC_ERRCODE_ACC_NOT_SUPPORT;
    return ret;
}

static inline uint32_t ACC_CheckWarpRotateY640x480(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0](y) buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (576UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1](uv) buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunWarpRotateY640x480(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_WARP_Y_640x480);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckWarpRotateY640x480(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            }
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return ret;
}

static inline uint32_t ACC_CheckWarpRotateUV640x480(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*240UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0](y) buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (576UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1](uv) buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*240UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunWarpRotateUV640x480(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_WARP_UV_640x480);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckWarpRotateUV640x480(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            }
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return ret;
}

static uint32_t ACC_CheckScaleOSD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*512UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1824UL*736UL*4UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunScaleOSD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_ScaleOSD);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckScaleOSD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckErode_rec3x3(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunErodeU8VGARect3x3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_ERODE_640x480_RECT3X3);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckErode_rec3x3(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckDilate_rec3x3(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (640UL*480UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunDilateU8VGARect3x3(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_DILATE_640x480_RECT3X3);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckDilate_rec3x3(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

static uint32_t ACC_CheckRGB2YUVU8HD(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (1280UL*720UL*3UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }

    if(Output->num_of_buf != 2UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1280UL*720UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
        if(Output->buf[1].buffer_size < (640UL*360UL*2UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[1] buffer size %d \n",Output->buf[1].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[1].pBuffer, 0x0, Output->buf[1].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[1].pBuffer, Output->buf[1].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunRGB2YUVU8HD(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_RGB2YUV_U8_HD);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckRGB2YUVU8HD(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
static uint32_t ACC_CheckGEMM(const AMBA_CV_FLEXIDAG_IO_s *Input, const AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret = ACC_ERRCODE_NONE;
    const memio_source_recv_raw_t *pSrc;
    if(Input->num_of_buf != 3UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input buffer num %d \n",Input->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[0].pBuffer, sizeof(void*));
        if(pSrc->size < (48UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[0] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[1].pBuffer, sizeof(void*));
        if(pSrc->size < (60UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[1] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
        ret |= AmbaWrap_memcpy(&pSrc, &Input->buf[2].pBuffer, sizeof(void*));
        if(pSrc->size < (80UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect input[2] buffer size %d \n",pSrc->size, DC_U, DC_U, DC_U, DC_U);
        }
    }
    if(Output->num_of_buf != 1UL) {
        ret = ACC_ERRCODE_INVALID_INPUTS;
        AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output buffer num %d \n",Output->num_of_buf, DC_U, DC_U, DC_U, DC_U);
    } else {
        if(Output->buf[0].buffer_size < (1024UL)) {
            ret = ACC_ERRCODE_INVALID_INPUTS;
            AmbaPrint_PrintUInt5("[ACC][ERROR] incorrect Output[0] buffer size %d \n",Output->buf[0].buffer_size, DC_U, DC_U, DC_U, DC_U);
        } else {
            ret |= AmbaWrap_memset(Output->buf[0].pBuffer, 0x0, Output->buf[0].buffer_size);
            (void)ACC_CacheDataClean(Output->buf[0].pBuffer, Output->buf[0].buffer_size);
        }
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s Fail \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}

uint32_t AmbaACC_RunGEMM(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = AmbaACC_selectIDLoc(ACC_GEMM);
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    ret = ACC_CheckGEMM(Input, Output);
    if(ret == ACC_ERRCODE_NONE) {
        (void)ACC_FlexidagSetInput(Input);
        if(bin_table_info[ID].IsInit == 0UL) {
            if(LoadDagTiming == ACC_LOAD_ON_CALL) {
                ret |= ACC_FlexidagSetParam(ID);
                ret |= ACC_InitDag(ID);
            } else {
                AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
            }
        } else {
            AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
        if(ret == ACC_ERRCODE_NONE) {
            ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
        }
        if(ret != ACC_ERRCODE_NONE) {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() check dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
uint32_t AmbaACC_RunCustom(AMBA_CV_FLEXIDAG_IO_s *Input, AMBA_CV_FLEXIDAG_IO_s *Output)
{
    uint32_t ret;
    uint32_t ID = Acc_Custom;
    AMBA_CV_FLEXIDAG_RUN_INFO_s RunInfo;
    //ret = ACC_CheckCustom(Input, Output);
    ret = ACC_FlexidagSetParam(ID);
    (void)ACC_FlexidagSetInput(Input);
    if(bin_table_info[ID].IsInit == 0UL) {
        if(LoadDagTiming == ACC_LOAD_ON_CALL) {
            ret |= ACC_InitDag(ID);
        } else {
            AmbaPrint_PrintStr5("[ACC][ERROR] %s() LoadDagTiming != ACC_LOAD_ON_CALL \n",__func__, DC_S, DC_S, DC_S, DC_S);
        }
    } else {
        AmbaPrint_PrintStr5("[ACC][LOG] %s() Is Init \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    if(ret == ACC_ERRCODE_NONE) {
        ret |= AmbaCV_FlexidagRun(&fd_gen_handle[ID], Input, Output, &RunInfo);
    }
    if(ret != ACC_ERRCODE_NONE) {
        AmbaPrint_PrintStr5("[ACC][ERROR] %s() run dag NG \n",__func__, DC_S, DC_S, DC_S, DC_S);
    }
    return ret;
}
//#endif
