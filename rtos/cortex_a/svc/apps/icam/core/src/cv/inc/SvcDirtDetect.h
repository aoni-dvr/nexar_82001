/**
 *  @file SvcDirtDetect.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 *
 *  @details svc dirt detection task header
 *
 */


#ifndef SVC_DIRT_DETECT_H
#define SVC_DIRT_DETECT_H

#define SVC_DIRT_DETECT_QUEUE_SIZE  (2U)

#define SVC_DIRT_DETECT_PYRAMID_CP_DONE         ((UINT32)1U << 16U)
#define SVC_DIRT_DETECT_ALGO_PROCESSING         ((UINT32)1U << 17U)
#define SVC_DIRT_DETECT_PYRAMID_GDMA_Y_DONE     ((UINT32)1U << 18U)
#define SVC_DIRT_DETECT_PYRAMID_GDMA_UV_DONE    ((UINT32)1U << 19U)

#define SVC_DIRT_DETECT_DAY_SCENE               (0U)
#define SVC_DIRT_DETECT_NIGHT_SCENE             (1U)

#define SVC_DIRT_DETECT_REPEAT_MODE             (0U)
#define SVC_DIRT_DETECT_ONE_TIME                (1U)

#define SVC_DIRT_DETECT_STATUS_IDLE             (0U)
#define SVC_DIRT_DETECT_STATUS_EXEC             (1U)
#define SVC_DIRT_DETECT_STATUS_FORCE_STOP       (2U)

#define GET                                 0U
#define SET                                 1U

typedef void (* SVC_DIRT_DETECT_DONE_FUNC)(void);

typedef struct {
    UINT32 AlgoInterval;
    UINT32 PyramidHier;
    UINT8  *pOutputBuf;
    UINT8  *pOsdOutputBuf;
    UINT32 OsdOutputWidth;
    UINT32 OsdOutputHeight;
    UINT32 OsdOutputBlockX;
    UINT32 OsdOutputBlockY;
    UINT32 HOZ_x;
    UINT32 HOZ_y;
    UINT32 ROILT_x;
    UINT32 ROILT_y;
    UINT32 ROIRB_x;
    UINT32 ROIRB_y;
    SVC_DIRT_DETECT_DONE_FUNC pDetectDoneFunc;
    UINT32 FovIdx;
} SVC_DIRT_DETECT_CONFIG_s;

typedef struct {
    UINT32 WrIdx;
    UINT32 RdIdx;
    AMBA_DSP_PYMD_DATA_RDY_s PyramidInfo[SVC_DIRT_DETECT_QUEUE_SIZE];
} SVC_DIRT_DETECT_QUEUE_s;

typedef struct {
    UINT32 FovIdx;
    UINT32 PyramidHier;
    UINT64 CapSequence;
    UINT32 Pitch;
    UINT32 Width;
    UINT32 Height;
    UINT32 DataFmt;
    UINT8  *pBufferY;
    UINT8  *pBufferUV;
} SVC_DIRT_DETECT_PYRAMID_INFO_s ;

UINT32 SvcDirtDetect_Init(void);
UINT32 SvcDirtDetect_Config(const SVC_DIRT_DETECT_CONFIG_s *pConfig);
UINT32 SvcDirtDetect_ConfigGet(SVC_DIRT_DETECT_CONFIG_s *pConfig);
UINT32 SvcDirtDetect_Start(UINT32 Priority, UINT32 CpuBits);
UINT32 SvcDirtDetect_Stop(void);
UINT32 SvcDirtDetect_Debug(char * const *pArgVector);

#if defined(CONFIG_BUILD_AMBA_ADAS)
typedef void (*SVC_DIRTDT_GET_SPEED)(DOUBLE *TransferSpeed);
void SvcDirtDetect_GetCanSpeed(DOUBLE *TransferSpeed) GNU_WEAK_SYMBOL;
#endif

void SvcDirtDetect_GetStatus(UINT32 *Status);
void SvcDirtDetect_GetScene(UINT32 *Scene);
void SvcDirtDetect_GetOpMode(UINT32 *OpMode);
void SvcDirtDetect_SetOpMode(UINT32 OpMode);
UINT32 SvcDirtDetect_InitAddress(UINT32 Addr);

void SvcDirtDetect_MVDataCB(const void *pMVData);

#endif /* SVC_DIRT_DETECT_H */