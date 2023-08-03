/**
 *  @file AmbaImg_AeAwb.h
 *
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
 *
 *  @Description    :: Definitions & Constants for Sample AE(Auto Exposure) and AWB(Auto White Balance) algorithms
 *
 */
#ifndef AMBA_IMG_AE_AWB_H
#define AMBA_IMG_AE_AWB_H

#define WB_UNIT_GAIN            4096U
#define MAX_DGAIN               (WB_UNIT_GAIN << 2)
#define DGAIN_STEP              96//64
#define SHIFT_BIT               2

#define WB_DGAIN_UNIT           4096
#define WB_ADJ_UNIT_RATIO       4096


#define WB_Y_VALUE              63
#define WB_Y256_VALUE           255
#define AE_Y_VALUE              16383
#define AE_MAX_TILES            (UINT16)384   //24U*16U
#define WB_MAX_TILES            (UINT16)4096  //64*64

#define USER_AE_MAX_EXPO_NUM     3U
#define USER_AE_MAX_TILES        128U
#define USER_HISTOGRAM_MAX_COUNT 256U
/** Global gain table size */
#define GAIN_TABLE_ENTRY        449
#define GAIN_DOUBLE             64
#define GAIN_0DB                448

/** Exposure time table size */
//#define EXP_TIME_TABLE_ENTRY    2048
//#define EXP_TIME_DOUBLE         128

//#define EXP_TIME_1_15_INDEX     884
//#define EXP_TIME_1_30_INDEX     1013 //1012
//#define EXP_TIME_1_60_INDEX     1141 //1140
//#define EXP_TIME_1_120_INDEX    1269 //1268

//#define EXP_TIME_1_12_INDEX     850
//#define EXP_TIME_1_25_INDEX     978
//#define EXP_TIME_1_50_INDEX     1106
//#define EXP_TIME_1_100_INDEX    1234

#define AMBA_IDLE               0U
#define AMBA_PROCESSING         1U
#define AMBA_LOCK               2U


#endif  /* _AMBA_AE_AWB_ */
