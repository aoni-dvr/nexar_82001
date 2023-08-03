/**
 *  @file AmbaIMU.h
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
 *  @details Definitions for Ambarella IMU (Accelerometer and Gyroscope Devices) driver APIs
 *
 */

#ifndef AMBA_LENS_H
#define AMBA_LENS_H

#include "AmbaTypes.h"
#include "AmbaErrorCode.h"


/* ERROR CODE */
#define LENS_ERR_NONE            (0U)
#define LENS_ERR_INVALID_API    (LENS_ERR_BASE + 1U)
#define LENS_ERR_INIT_FAIL      (LENS_ERR_BASE + 2U)
#define LENS_ERR_OPER_FAIL        (LENS_ERR_BASE + 3U)
#define LENS_ERR_REGREAD_FAIL    (LENS_ERR_BASE + 4U)
#define LENS_ERR_REGWRITE_FAIL    (LENS_ERR_BASE + 5U)



typedef enum {
    AMBA_LENS_SENSOR0 = 0,
    AMBA_NUM_LENS_SENSOR = 1
} AMBA_LENS_NUM_e;


typedef struct {
    UINT32 (*Init)(UINT32 VinID);
    UINT32 (*Oper)(UINT32 VinID, UINT32 AfOpt);
    UINT32 (*RegRead)(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size);
    UINT32 (*RegWrite)(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size);
    UINT32 (*GetStatus)(UINT32 VinID);
} AMBA_LENS_OBJ_s;


/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaLENS.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_LENS_OBJ_s *pAmbaLensObj[AMBA_NUM_LENS_SENSOR];
extern void AmbaLENS_Hook(UINT8 Chan, AMBA_LENS_OBJ_s *pLensObj);

static inline UINT32 AmbaLENS_Init(UINT32 VinID)
{
    return ((pAmbaLensObj[VinID]->Init == NULL) ? LENS_ERR_INVALID_API : pAmbaLensObj[VinID]->Init(VinID));
}
static inline UINT32 AmbaLENS_Oper(UINT32 VinID, UINT32 AfOpt)
{
    return ((pAmbaLensObj[VinID]->Oper == NULL) ? LENS_ERR_INVALID_API : pAmbaLensObj[VinID]->Oper(VinID, AfOpt));
}

static inline UINT32 AmbaLENS_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData, UINT32 Size)
{
    return ((pAmbaLensObj[VinID]->RegRead == NULL) ? LENS_ERR_INVALID_API : pAmbaLensObj[VinID]->RegRead(VinID, Addr, pRxData, Size));
}
static inline UINT32 AmbaLENS_RegWrite(UINT32 VinID, UINT16 Addr, const UINT8 *pTxData, UINT32 Size)
{
    return ((pAmbaLensObj[VinID]->RegWrite == NULL) ? LENS_ERR_INVALID_API : pAmbaLensObj[VinID]->RegWrite(VinID, Addr, pTxData, Size));
}
static inline UINT32 AmbaLENS_GetStatus(UINT32 VinID)
{
    return ((pAmbaLensObj[VinID]->GetStatus == NULL) ? LENS_ERR_INVALID_API : pAmbaLensObj[VinID]->GetStatus(VinID));
}

#endif /* _AMBA_LENS_H_ */

