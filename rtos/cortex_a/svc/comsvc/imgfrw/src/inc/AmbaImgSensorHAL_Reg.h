/**
 *  @file AmbaImgSensorHAL_Reg.h
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants and Definitions for Amba Image Sensor HAL REG
 *
 */

#ifndef AMBA_IMG_SENSOR_HAL_REG_H
#define AMBA_IMG_SENSOR_HAL_REG_H

typedef enum /*_AMBA_IMG_SENSOR_HAL_AGC_HDR_e_*/ {
    AGC_HDR_OFF = 0,
    AGC_HDR_ON
} AMBA_IMG_SENSOR_HAL_AGC_HDR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_AGC_ACK_e_*/ {
    AGC_ACK_NONE = 0,
    AGC_ACK_ON
} AMBA_IMG_SENSOR_HAL_AGC_ACK_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_AGC_SHR_e_*/ {
    AGC_SHR_NON_ADV = 0,
    AGC_SHR_ADV,
    AGC_SHR_CHK
} AMBA_IMG_SENSOR_HAL_AGC_SHR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_AGC_SIG_e_*/ {
    AGC_SIG_AGC = 0,
    AGC_SIG_SKIP
} AMBA_IMG_SENSOR_HAL_AGC_SIG_e;

typedef struct /*_AMBA_IMG_SENSOR_HAL_AGC_DATA_u__*/ {
    UINT64  Agc;
} AMBA_IMG_SENSOR_HAL_AGC_DATA_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_AGC_DATA_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_AGC_DATA_u_    Bits;
} AMBA_IMG_SENSOR_HAL_AGC_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_AGC_DATA_s_*/ {
    AMBA_IMG_SENSOR_HAL_AGC_DATA_u    Ctx;
} AMBA_IMG_SENSOR_HAL_AGC_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_AGC_CTRL_u__*/ {
    UINT32  Reserved_L:    32;
    UINT32  Reserved_H:    27;
    UINT32  Hdr:            1;
    UINT32  Ack:            1;
    UINT32  Shr:            2;
    UINT32  Sig:            1;
} AMBA_IMG_SENSOR_HAL_AGC_CTRL_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_AGC_CTRL_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_AGC_CTRL_u_    Bits;
} AMBA_IMG_SENSOR_HAL_AGC_CTRL_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_AGC_CTRL_s_*/ {
    AMBA_IMG_SENSOR_HAL_AGC_CTRL_u    Ctx;
} AMBA_IMG_SENSOR_HAL_AGC_CTRL_s;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_ACK_e_*/ {
    DGC_ACK_NONE = 0,
    DGC_ACK_ON
} AMBA_IMG_SENSOR_HAL_DGC_ACK_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_SHR_e_*/ {
    DGC_SHR_NON_ADV = 0,
    DGC_SHR_ADV,
    DGC_SHR_CHK
} AMBA_IMG_SENSOR_HAL_DGC_SHR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_SVR_e_*/ {
    DGC_SVR_NON_ADV = 0,
    DGC_SVR_ADV
} AMBA_IMG_SENSOR_HAL_DGC_SVR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_SSI_e_*/ {
    DGC_SSI_NON_ADV = 0,
    DGC_SSI_ADV
} AMBA_IMG_SENSOR_HAL_DGC_SSI_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_HDR_e_*/ {
    DGC_HDR_OFF = 0,
    DGC_HDR_ON
} AMBA_IMG_SENSOR_HAL_DGC_HDR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_IMG_e_*/ {
    DGC_IMG_VIDEO = 0,
    DGC_IMG_STILL
} AMBA_IMG_SENSOR_HAL_DGC_IMG_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_PUT_e_*/ {
    DGC_PUT_NONE = 0,
    DGC_PUT_SOF,
    DGC_PUT_ADV
} AMBA_IMG_SENSOR_HAL_DGC_PUT_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_DGC_SIG_e_*/ {
    DGC_SIG_DGC = 0,
    DGC_SIG_SKIP
} AMBA_IMG_SENSOR_HAL_DGC_SIG_e;

typedef struct /*_AMBA_IMG_SENSOR_HAL_WGC_DATA_u__*/ {
    UINT64 Wgc;
} AMBA_IMG_SENSOR_HAL_WGC_DATA_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_WGC_DATA_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_WGC_DATA_u_    Bits;
} AMBA_IMG_SENSOR_HAL_WGC_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_WGC_DATA_s_*/ {
    AMBA_IMG_SENSOR_HAL_WGC_DATA_u    Ctx;
} AMBA_IMG_SENSOR_HAL_WGC_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_DGC_DATA_u__*/ {
    UINT64 Dgc;
} AMBA_IMG_SENSOR_HAL_DGC_DATA_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_DGC_DATA_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_DGC_DATA_u_    Bits;
} AMBA_IMG_SENSOR_HAL_DGC_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_DGC_DATA_s_*/ {
    AMBA_IMG_SENSOR_HAL_DGC_DATA_u    Ctx;
} AMBA_IMG_SENSOR_HAL_DGC_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_DGC_CTRL_u__*/ {
    UINT32  Reserved_L:    32;
    UINT32  Reserved_H:    19;
    UINT32  WbId:           3;
    UINT32  Ack:            1;
    UINT32  Shr:            2;
    UINT32  Svr:            1;
    UINT32  Ssi:            1;
    UINT32  Hdr:            1;
    UINT32  Img:            1;
    UINT32  Put:            2;
    UINT32  Sig:            1;
} AMBA_IMG_SENSOR_HAL_DGC_CTRL_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_DGC_CTRL_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_u_    Bits;
} AMBA_IMG_SENSOR_HAL_DGC_CTRL_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_DGC_CTRL_s_*/ {
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_u    Ctx;
} AMBA_IMG_SENSOR_HAL_DGC_CTRL_s;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_MSC_e_*/ {
    SHR_MSC_NONE = 0,
    SHR_MSC_UPD,
    SHR_MSC_UPD_SOF
} AMBA_IMG_SENSOR_HAL_SHR_MSC_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_SLS_e_*/ {
    SHR_SLS_NONE = 0,
    SHR_SLS_ADV
} AMBA_IMG_SENSOR_HAL_SHR_SLS_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_HDR_e_*/ {
    SHR_HDR_OFF = 0,
    SHR_HDR_ON
} AMBA_IMG_SENSOR_HAL_SHR_HDR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_AIK_e_*/ {
    SHR_AIK_NONE = 0,
    SHR_AIK_SOF,
    SHR_AIK_ADV,
    SHR_AIK_ADV_SOF
} AMBA_IMG_SENSOR_HAL_SHR_AIK_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_ACK_e_*/ {
    SHR_ACK_NONE = 0,
    SHR_ACK_SHR,
    SHR_ACK_SSI_SVR
} AMBA_IMG_SENSOR_HAL_SHR_ACK_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_RGB_e_*/ {
    SHR_RGB_NON_ADV = 0,
    SHR_RGB_ADV
} AMBA_IMG_SENSOR_HAL_SHR_RGB_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_SSI_e_*/ {
    SHR_SSI_NONE = 0,
    SHR_SSI_SOF,
    SHR_SSI_ADV,
    SHR_SSI_CHK
} AMBA_IMG_SENSOR_HAL_SHR_SSI_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_SVR_e_*/ {
    SHR_SVR_NON_ADV = 0,
    SHR_SVR_ADV,
    SHR_SVR_CHK
} AMBA_IMG_SENSOR_HAL_SHR_SVR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_SHR_SIG_e_*/ {
    SHR_SIG_BOTH = 0,
    SHR_SIG_SHR,
    SHR_SIG_SVR,
    SHR_SIG_SSR,
    SHR_SIG_SSG
} AMBA_IMG_SENSOR_HAL_SHR_SIG_e;

typedef struct /*_AMBA_IMG_SENSOR_HAL_SHR_DATA_u__*/ {
    UINT64 Shr;
} AMBA_IMG_SENSOR_HAL_SHR_DATA_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_SHR_DATA_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_SHR_DATA_u_    Bits;
} AMBA_IMG_SENSOR_HAL_SHR_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_SHR_DATA_s_*/ {
    AMBA_IMG_SENSOR_HAL_SHR_DATA_u    Ctx;
} AMBA_IMG_SENSOR_HAL_SHR_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_SHR_CTRL_u__*/ {
    UINT32  Svr:           16;
    UINT32  Reserved_M:    16;
    UINT32  Reserved_H:    14;
    UINT32  Msc:            2;
    UINT32  Sls:            1;
    UINT32  Hdr:            1;
    UINT32  Aik:            3;
    UINT32  Ack:            2;
    UINT32  Rgb:            1;
    UINT32  Ssi:            3;
    UINT32  Fll:            2;
    UINT32  Sig:            3;
} AMBA_IMG_SENSOR_HAL_SHR_CTRL_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_SHR_CTRL_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_SHR_CTRL_u_    Bits;
} AMBA_IMG_SENSOR_HAL_SHR_CTRL_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_SHR_CTRL_s_*/ {
    AMBA_IMG_SENSOR_HAL_SHR_CTRL_u    Ctx;
} AMBA_IMG_SENSOR_HAL_SHR_CTRL_s;

typedef enum /*_AMBA_IMG_SENSOR_HAL_USER_SHR_e_*/ {
    USER_SHR_NON_ADV = 0,
    USER_SHR_ADV
} AMBA_IMG_SENSOR_HAL_USER_SHR_e;

typedef enum /*_AMBA_IMG_SENSOR_HAL_USER_SIG_e_*/ {
    USER_SIG_NONE = 0,
    USER_SIG_USER,
    USER_SIG_SKIP
} AMBA_IMG_SENSOR_HAL_USER_SIG_e;

typedef struct /*_AMBA_IMG_SENSOR_HAL_USR_DATA_u__*/ {
    UINT64 User;
} AMBA_IMG_SENSOR_HAL_USR_DATA_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_USR_DATA_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_USR_DATA_u_    Bits;
} AMBA_IMG_SENSOR_HAL_USR_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_USR_DATA_s_*/ {
    AMBA_IMG_SENSOR_HAL_USR_DATA_u Ctx;
} AMBA_IMG_SENSOR_HAL_USR_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_USR_CTRL_u__*/ {
    UINT32  Reserved_L:    32;
    UINT32  Reserved_H:    29;
    UINT32  Shr:            1;
    UINT32  Sig:            2;
} AMBA_IMG_SENSOR_HAL_USR_CTRL_u_;

typedef union /*_AMBA_IMG_SENSOR_HAL_USR_CTRL_u_*/ {
    UINT64                             Reg;
    AMBA_IMG_SENSOR_HAL_USR_CTRL_u_    Bits;
} AMBA_IMG_SENSOR_HAL_USR_CTRL_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_USR_CTRL_s_*/ {
    AMBA_IMG_SENSOR_HAL_USR_CTRL_u    Ctx;
} AMBA_IMG_SENSOR_HAL_USR_CTRL_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_DATA_s_*/ {
    AMBA_IMG_SENSOR_HAL_AGC_DATA_s    Agc;
    AMBA_IMG_SENSOR_HAL_DGC_DATA_s    Dgc;
    AMBA_IMG_SENSOR_HAL_WGC_DATA_s    Wgc;
    AMBA_IMG_SENSOR_HAL_SHR_DATA_s    Shr;
    AMBA_IMG_SENSOR_HAL_DGC_DATA_s    DDgc;
    AMBA_IMG_SENSOR_HAL_USR_DATA_s    User;
} AMBA_IMG_SENSOR_HAL_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_CTRL_s_*/ {
    AMBA_IMG_SENSOR_HAL_AGC_CTRL_s    Agc;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_s    Dgc;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_s    Wgc;
    AMBA_IMG_SENSOR_HAL_SHR_CTRL_s    Shr;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_s    DDgc;
    AMBA_IMG_SENSOR_HAL_USR_CTRL_s    User;
} AMBA_IMG_SENSOR_HAL_CTRL_s;

typedef union /*_AMBA_IMG_SENSOR_HAL_DATA_MEM_u_*/ {
    UINT64                             *pUint64;
    AMBA_IMG_SENSOR_HAL_AGC_DATA_u_    *pAgc;
    AMBA_IMG_SENSOR_HAL_DGC_DATA_u_    *pDgc;
    AMBA_IMG_SENSOR_HAL_WGC_DATA_u_    *pWgc;
    AMBA_IMG_SENSOR_HAL_SHR_DATA_u_    *pShr;
    AMBA_IMG_SENSOR_HAL_DGC_DATA_u_    *pDDgc;
    AMBA_IMG_SENSOR_HAL_USR_DATA_u_    *pUser;
} AMBA_IMG_SENSOR_HAL_DATA_MEM_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_DATA_MEM_s_*/ {
    AMBA_IMG_SENSOR_HAL_DATA_MEM_u    Ctx;
} AMBA_IMG_SENSOR_HAL_DATA_MEM_s;

typedef union /*_AMBA_IMG_SENSOR_HAL_CTRL_MEM_u_*/ {
    UINT64                             *pUint64;
    AMBA_IMG_SENSOR_HAL_AGC_CTRL_u_    *pAgc;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_u_    *pDgc;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_u_    *pWgc;
    AMBA_IMG_SENSOR_HAL_SHR_CTRL_u_    *pShr;
    AMBA_IMG_SENSOR_HAL_DGC_CTRL_u_    *pDDgc;
    AMBA_IMG_SENSOR_HAL_USR_CTRL_u_    *pUser;
} AMBA_IMG_SENSOR_HAL_CTRL_MEM_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_CTRL_MEM_s_*/ {
    AMBA_IMG_SENSOR_HAL_CTRL_MEM_u    Ctx;
} AMBA_IMG_SENSOR_HAL_CTRL_MEM_s;

#endif  /* AMBA_IMG_SENSOR_HAL_REG_H */
