/**
 *  @file AmbaDramShmoo.h
 *
 * Copyright (c) [2021] Ambarella International LP
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header file for Ambarella Dram Shmoo functions.
 *
 */

#ifndef AMBA_DRAMSHMOO_H
#define AMBA_DRAMSHMOO_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * shmoo state format :
 * [0:3]: dir, 0: +, 1: -
 * [4:11]: item
 * [12]: 0: read, 1: write
 */
#define SHMOO_START             0x4F4D4853U  /* Amba */

#define SHMOO_DIR_MASK          0x1U
#define SHMOO_ITEM_MASK         0xF0U
#define SHMOO_ITEM_OFFSET       0x4U
#define READ_NUM                0x5
#define WRITE_NUM               0xb
#define SHMOO_READ              0x0000
#define SHMOO_PAD_TERM          0x0000
#define SHMOO_PAD_VREF          0x0010
#define SHMOO_RDDLY             0x0020
#define SHMOO_DLL0              0x0030
#define SHMOO_LPDDR4_PDDS       0x0040
#define SHMOO_READ_DONE         0x0ff0
#define SHMOO_WRITE             0x1000
#define SHMOO_PAD_DDS           0x1000
#define SHMOO_PAD_PDDS          0x1010
#define SHMOO_PAD_CA_DDS        0x1020
#define SHMOO_PAD_CA_PDDS       0x1030
#define SHMOO_WRDLY             0x1040
#define SHMOO_DLL2              0x1050
#define SHMOO_LPDDR4_CA_VREF    0x1060
#define SHMOO_LPDDR4_DQ_VREF    0x1070
#define SHMOO_LPDDR4_CA_ODT     0x1080
#define SHMOO_LPDDR4_DQ_ODT     0x1090
#define SHMOO_DLL1              0x10a0
#define SHMOO_DONE              0x2000
#define SHMOO_INIT              0xfff0U

#define SHMOO_OFFSET_FLAG           0U 
#define SHMOO_OFFSET_STATE          1U 
#define SHMOO_OFFSET_READ_DLL_1     2U 
#define SHMOO_OFFSET_READ_DLL_0     3U  
#define SHMOO_OFFSET_READ_VREF_1    4U 
#define SHMOO_OFFSET_READ_VREF_0    5U 
#define SHMOO_OFFSET_PAD_TERM_1     6U 
#define SHMOO_OFFSET_PAD_TERM_0     7U 
#define SHMOO_OFFSET_PAD_TERM3_1    8U 
#define SHMOO_OFFSET_PAD_TERM3_0    9U 
#define SHMOO_OFFSET_READ_DELAY_1   10U
#define SHMOO_OFFSET_READ_DELAY_0   11U
#define SHMOO_OFFSET_MR3_1          12U
#define SHMOO_OFFSET_MR3_0          13U
#define SHMOO_OFFSET_PAD_DDS_1      14U
#define SHMOO_OFFSET_PAD_DDS_0      15U
#define SHMOO_OFFSET_PAD3_DDS_1     16U
#define SHMOO_OFFSET_PAD3_DDS_0     17U
#define SHMOO_OFFSET_WRITE_DLY_1    18U
#define SHMOO_OFFSET_WRITE_DLY_0    19U
#define SHMOO_OFFSET_WRITE_DLL_1    20U
#define SHMOO_OFFSET_WRITE_DLL_0    21U
#define SHMOO_OFFSET_MR12_1         22U
#define SHMOO_OFFSET_MR12_0         23U
#define SHMOO_OFFSET_MR14_1         24U
#define SHMOO_OFFSET_MR14_0         25U
#define SHMOO_OFFSET_MR11_DQ_1      26U
#define SHMOO_OFFSET_MR11_DQ_0      27U
#define SHMOO_OFFSET_MR11_CA_1      28U
#define SHMOO_OFFSET_MR11_CA_0      29U
#define SHMOO_OFFSET_SYNC_DLL_1     30U
#define SHMOO_OFFSET_SYNC_DLL_0     31U
#define SHMOO_OFFSET_DLL_ORI        32U
#define SHMOO_OFFSET_PADTERM_ORI    33U
#define SHMOO_OFFSET_PADTERM3_ORI   34U
#define SHMOO_OFFSET_DQ_VREF_ORI    35U
#define SHMOO_OFFSET_DQ_READ_DLY_ORI    36U
#define SHMOO_OFFSET_DQ_WRTITE_DLY_ORI  37U
#define SHMOO_OFFSET_MR3_ORI            38U
#define SHMOO_OFFSET_MR11_ORI       39U
#define SHMOO_OFFSET_MR12_ORI       40U
#define SHMOO_OFFSET_MR14_ORI       41U
#define SHMOO_OFFSET_PADTERM2_ORI   42U
#define SHMOO_OFFSET_PAD2_PDDS_1    43U
#define SHMOO_OFFSET_PAD2_PDDS_0    44U
#define SHMOO_OFFSET_PAD3_PDDS_1    45U
#define SHMOO_OFFSET_PAD3_PDDS_0    46U
#define SHMOO_OFFSET_PAD2_CA_DDS_B_1    47U
#define SHMOO_OFFSET_PAD2_CA_DDS_B_0    48U
#define SHMOO_OFFSET_PAD2_CA_DDS_A_1    49U
#define SHMOO_OFFSET_PAD2_CA_DDS_A_0    50U
#define SHMOO_OFFSET_PAD2_CA_PDDS_B_1   51U
#define SHMOO_OFFSET_PAD2_CA_PDDS_B_0   52U
#define SHMOO_OFFSET_PAD2_CA_PDDS_A_1   53U
#define SHMOO_OFFSET_PAD2_CA_PDDS_A_0   54U

/* The shmoo item control structure */
/* Each item is independent to shmoo task */
/* 1 : do shmoo,  else : do not do shmoo */
typedef struct {
UINT8 pad_dds;
UINT8 pad_pdds;
UINT8 pad_ca_dds;
UINT8 pad_ca_pdds;
UINT8 pad_term;
UINT8 pad_vref;
UINT8 rddly;
UINT8 wrdly;
UINT8 dll0;
UINT8 dll1;
UINT8 dll2;
UINT8 ddr4_dds;
UINT8 ddr4_vref;
UINT8 ddr4_odt;
UINT8 lpddr4_pdds;
UINT8 lpddr4_ca_vref;
UINT8 lpddr4_dq_vref;
UINT8 lpddr4_ca_odt;
UINT8 lpddr4_dq_odt;
} AMBA_DRAM_SHMOO_CTRL_s;

typedef void (*AMBA_DRAM_SHMOO_CHECK_f)(UINT32 * pCheckData);
typedef void (*AMBA_DRAM_SHMOO_STOP_f)(UINT32 * pShemooData);
typedef struct {
    AMBA_DRAM_SHMOO_CHECK_f ShmooCheckCb;
    AMBA_DRAM_SHMOO_STOP_f SmooStopCb;
} AMBA_DRAM_SHMOO_CALLBACK_s;

typedef struct {
    UINT32 Duration;        /* The duration to check application result */ 
    UINT32 StoreDevice;     /* The storage device type to store shmoo result, 1:NAND, 2:SpiNOR, 3:eMMC */
    UINT32 CoreMask;        /* The core mask to shmoo task */
    UINT32 priority;        /* The shmoo task's priority */
    UINT32 PartitionID;
} AMBA_DRAM_SHMOO_MISC_s;

void AmbaDramShmoo_Start( AMBA_DRAM_SHMOO_CALLBACK_s* pCallBacks, 
                                AMBA_DRAM_SHMOO_CTRL_s* pShmooCtrl, 
                                AMBA_DRAM_SHMOO_MISC_s* pMiscSetting);
void AmbaDramShmoo_Stop(void);

#ifdef __cplusplus
}
#endif
#endif
