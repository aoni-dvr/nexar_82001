/**
 *  @file AmbaSTU_WarpTableMetadata.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details stereo utility
 *
 */
#ifndef AMBA_STU_WARPTABLE_METADATA_H
#define AMBA_STU_WARPTABLE_METADATA_H
#include "AmbaTypes.h"

typedef struct {
    UINT8 Reserved[8U];
    DOUBLE Ku;
    DOUBLE Kv;
    DOUBLE U0;
    DOUBLE V0;
} RESERVED_STRUCT_0_t;

typedef struct {
    UINT8 Reserved[8U];
    DOUBLE Angle[4U];
    DOUBLE Position[3U];
} RESERVED_STRUCT_1_t;

typedef struct {
    RESERVED_STRUCT_0_t affine;
    UINT8 Reserved[2096U];
} RESERVED_STRUCT_2_t;

typedef struct {
    UINT8 Reserved0[152U];
    RESERVED_STRUCT_0_t TargetParams;
    RESERVED_STRUCT_1_t Reserved1;
    RESERVED_STRUCT_2_t Reserved2;
    RESERVED_STRUCT_1_t SourcePose;
    UINT8 Reserved4[24U];
    DOUBLE Baseline;
} STEREO_CALIBRATION_METADATA_t;

#endif
