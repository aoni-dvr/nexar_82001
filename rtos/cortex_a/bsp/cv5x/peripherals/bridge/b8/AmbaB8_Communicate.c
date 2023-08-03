/**
 *  @file AmbaB8_Communicate.c
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
 *  @details B8 Communication APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_PwmEnc.h"
#include "AmbaB8_PwmDec.h"
#include "AmbaB8_IoMap.h"

#ifdef BUILT_IN_SERDES
#include "AmbaSERDES.h"
#include "AmbaCSL_SERDES.h"
#endif

static AmbaB8_KAL_MUTEX_t B8Comm_Mutex;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8F_RegWrite
 *
 *  @Description:: Write B8F registers
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *      RegAddr:       Register address
 *      AddrIncrement: Increment address for continuous register read/write
 *      DataWidth:     Data unit is 8/16/32-bit
 *      DataSize:      Number of data to be transmitted
 *      pTxDataBuf:    Pointer to data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8_ERR_NONE(0)/B8 ERROR CODE
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8F_RegWrite(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf)
{
#ifdef BUILT_IN_SERDES
    UINT32 SubChipID = (ChipID & B8_SUB_CHIP_ID_MASK);
    UINT32 RetVal;
    ULONG RegAddr;

    if (((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) && (SubChipID != 0U)) {
        /* Use SoC PWM encoder */
        if (DataWidth <= B8_DATA_WIDTH_32BIT) {
            AmbaB8_Misra_TypeCast(&RegAddr, &pReg);
            if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
                RetVal = AmbaSERDES_RegWrite((0xf0U | SubChipID), RegAddr, AddrIncrement, (8U << DataWidth), DataSize, pTxDataBuf);
            } else {
                RetVal = AmbaSERDES_RegWrite((0xd0U | (SubChipID - 1U)), RegAddr, AddrIncrement, (8U << DataWidth), DataSize, pTxDataBuf);
            }

            if (RetVal != 0U) {
                RetVal = B8_ERR_UNEXPECTED;
            }
        } else {
            RetVal = B8_ERR_ARG;
        }

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;

#else
    AmbaB8_Misra_TouchUnused(&ChipID);
    AmbaB8_Misra_TouchUnused(&pReg);
    AmbaB8_Misra_TouchUnused(&AddrIncrement);
    AmbaB8_Misra_TouchUnused(&DataWidth);
    AmbaB8_Misra_TouchUnused(&DataSize);
    AmbaB8_Misra_TouchUnused(&pTxDataBuf);

    AmbaB8_PrintUInt5(" Not support built-in SerDes", 0U, 0U, 0U, 0U, 0U);
    return B8_ERR_INVALID_API;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8F_RegRead
 *
 *  @Description:: Read B8F registers
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *      RegAddr:       Register address
 *      AddrIncrement: Increment address for continuous register read/write
 *      DataWidth:     Data unit is 8/16/32-bit
 *      DataSize:      Number of data to be received
 *      pRxDataBuf:    Pointer to data buffer
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          UINT32 : B8_ERR_NONE(0)/B8 ERROR CODE
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8F_RegRead(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pRxDataBuf)
{
#ifdef BUILT_IN_SERDES
    UINT32 SubChipID = (ChipID & B8_SUB_CHIP_ID_MASK);
    UINT32 RetVal = B8_ERR_NONE;
    ULONG RegAddr;
    UINT8 *pRData8[4] = {(UINT8 *)pRxDataBuf, (UINT8 *)pRxDataBuf, (UINT8 *)pRxDataBuf, (UINT8 *)pRxDataBuf};
    UINT16 *pRData16[4] = {(UINT16 *)pRxDataBuf, (UINT16 *)pRxDataBuf, (UINT16 *)pRxDataBuf, (UINT16 *)pRxDataBuf};
    UINT32 *pRData32[4] = {(UINT32 *)pRxDataBuf, (UINT32 *)pRxDataBuf, (UINT32 *)pRxDataBuf, (UINT32 *)pRxDataBuf};
    void **pRData;

    if (((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) && (SubChipID != 0U)) {
        /* Use SoC PWM encoder */
        if (DataWidth <= B8_DATA_WIDTH_32BIT) {

            AmbaB8_Misra_TypeCast(&RegAddr, &pReg);

            if (DataWidth == B8_DATA_WIDTH_8BIT) {
                pRData = (void **)pRData8;
            } else if (DataWidth == B8_DATA_WIDTH_16BIT) {
                pRData = (void **)pRData16;
            } else {    /* DataWidth == B8_DATA_WIDTH_32BIT */
                pRData = (void **)pRData32;
            }

            if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
                RetVal = AmbaSERDES_RegRead((0xf0U | SubChipID), RegAddr, AddrIncrement, (8U << DataWidth), DataSize, pRData);
            } else {
                RetVal = AmbaSERDES_RegRead((0xd0U | (SubChipID - 1U)), RegAddr, AddrIncrement, (8U << DataWidth), DataSize, pRData);
            }

            if (RetVal != 0U) {
                RetVal = B8_ERR_UNEXPECTED;
            }
        } else {
            RetVal = B8_ERR_ARG;
        }

    } else {
        RetVal = B8_ERR_ARG;
    }

    return RetVal;

#else
    AmbaB8_Misra_TouchUnused(&ChipID);
    AmbaB8_Misra_TouchUnused(&pReg);
    AmbaB8_Misra_TouchUnused(&AddrIncrement);
    AmbaB8_Misra_TouchUnused(&DataWidth);
    AmbaB8_Misra_TouchUnused(&DataSize);
    AmbaB8_Misra_TouchUnused(&pRxDataBuf);

    AmbaB8_PrintUInt5(" Not support built-in SerDes", 0U, 0U, 0U, 0U, 0U);
    return B8_ERR_INVALID_API;
#endif
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_CommInit
 *
 *  @Description:: Initialize B8 Communication
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_CommInit(void)
{
    UINT32 RetVal = B8_ERR_NONE;
    static UINT8 MutexInitDone = 0U;

    if (MutexInitDone == 0U) {
        /* Create a mutex */
        if (AmbaB8_KAL_MutexCreate(&B8Comm_Mutex, NULL) != B8_ERR_NONE) {
            AmbaB8_PrintStr5("Unable to create mutex for B8 Comm", NULL, NULL, NULL, NULL, NULL);
            RetVal = B8_ERR_MUTEX;
        }
        MutexInitDone = 1U;
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_CommReset
 *
 *  @Description:: Reset B8 PwmEnc Module
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     Channel:          B8 PWM Encoder channel
 *
 *  @Output     :: none
 *
 *  @Return     :: B8_ERR_NONE(0)/ERROR CODE
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_CommReset(UINT32 ChipID)
{
    UINT32 RetVal = B8_ERR_NONE;

#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
        if (AmbaSERDES_Reset(0xf0 | (ChipID & B8_SUB_CHIP_ID_MASK)) != 0U) {
            RetVal = B8_ERR_MUTEX;
        }

    } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8ND) != 0U) {
        if (AmbaSERDES_Reset(0xd0 | (ChipID & B8_SUB_CHIP_ID_MASK)) != 0U) {
            RetVal = B8_ERR_MUTEX;
        }
    } else
#endif
    {
        if (AmbaB8_KAL_MutexTake(&B8Comm_Mutex, AMBAB8_KAL_WAIT_FOREVER) == B8_ERR_NONE) {

            /* B6N */
            if ((ChipID & B8_SELECT_CHIP_ID_B6N) != 0U) {

                /* B8N */
            } else {
                /* disable error status interrupt */
                RetVal |= AmbaB8_PwmEncMaskErrStatus((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 1U);

                /* reset error status */
                RetVal |= AmbaB8_PwmEncClearErrStatus(ChipID & B8_MAIN_CHIP_ID_B8N_MASK);

                /* reset tx & rport_fifo */
                RetVal |= AmbaB8_PwmEncTxReset(ChipID & B8_MAIN_CHIP_ID_B8N_MASK);

                /* reset rx */
                RetVal |= AmbaB8_PwmEncSendResetPkt(ChipID & B8_MAIN_CHIP_ID_B8N_MASK);

                /* reset error status */
                RetVal |= AmbaB8_PwmEncClearErrStatus(ChipID & B8_MAIN_CHIP_ID_B8N_MASK);

                /* enable error status interrupt */
                RetVal |= AmbaB8_PwmEncMaskErrStatus((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 0U);

                /* reset config */
                RetVal |= AmbaB8_PwmEncSetChunkSize((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 0U);
                RetVal |= AmbaB8_PwmDecSetCreditPwr(ChipID, 4U);
                RetVal |= AmbaB8_PwmEncSetCreditPwr((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 4U);
                RetVal |= AmbaB8_PwmEncEnable((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 0U);
            }
            (void) AmbaB8_KAL_MutexGive(&B8Comm_Mutex);
        } else {
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_CommConfig
 *
 *  @Description:: Configure B8 PWM Encoder
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     Channel:          B8 PWM Encoder channel
 *     pConfig:          pointer to B8 PWM Encoder config
 *
 *  @Output     :: none
 *
 *  @Return     :: B8_ERR_NONE(0)/ERROR CODE
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_CommConfig(UINT32 ChipID, const B8_COMM_CONFIG_s *pConfig)
{
    UINT32 RetVal = B8_ERR_NONE;

    /* configure pwm_enc/dec */
#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
        AmbaSERDES_Config((0xf0 | (ChipID & B8_SUB_CHIP_ID_MASK)), pConfig->ChunkSize, pConfig->CreditPwr);
        AmbaSERDES_Enable();

    } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8ND) != 0U) {
        /* todo */

    } else
#endif
    {
        if (AmbaB8_KAL_MutexTake(&B8Comm_Mutex, AMBAB8_KAL_WAIT_FOREVER) == B8_ERR_NONE) {
            /* B6N */
            if ((ChipID & B8_SELECT_CHIP_ID_N_MASK) != 0U) {

                /* B8N */
            } else {
                RetVal |= AmbaB8_PwmEncSetChunkSize((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), pConfig->ChunkSize);
                RetVal |= AmbaB8_PwmDecSetCreditPwr(ChipID, pConfig->CreditPwr);
                RetVal |= AmbaB8_PwmEncSetCreditPwr((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), pConfig->CreditPwr);
                RetVal |= AmbaB8_PwmEncEnable((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), pConfig->ReplayTimes);
            }
            (void) AmbaB8_KAL_MutexGive(&B8Comm_Mutex);
        } else {
            RetVal = B8_ERR_MUTEX;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_CommEnableAck
 *
 *  @Description:: Enable / Disable B8 PWM Encoder ACK mechanism
 *
 *  @Input      ::
 *     ChipID:           B8 chip id
 *     Channel:          B8 PwmEnc channel
 *     Enable:           Enable(1) / Disable(0)
 *
 *  @Output     :: none
 *
 *  @Return     :: B8_ERR_NONE(0)/ERROR CODE
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_CommDisableAck(UINT32 ChipID, UINT32 IsDisable)
{
    UINT32 RetVal = B8_ERR_NONE;

#ifdef BUILT_IN_SERDES
    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
        /* to be removed */
        if(IsDisable != 0U) {
            _pAmbaSerdesPwmEnc_Reg->Ctrl.Bits.DisableAck = 1U;
        } else {
            _pAmbaSerdesPwmEnc_Reg->Ctrl.Bits.DisableAck = 0U;
        }
    } else
#endif
    {
        if (AmbaB8_KAL_MutexTake(&B8Comm_Mutex, AMBAB8_KAL_WAIT_FOREVER) == B8_ERR_NONE) {
            if ((ChipID & B8_SELECT_CHIP_ID_N_MASK) != 0U) {

            } else {
                RetVal = AmbaB8_PwmEncSetDisableAck((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), IsDisable);
            }
            (void) AmbaB8_KAL_MutexGive(&B8Comm_Mutex);
        } else {
            RetVal = B8_ERR_MUTEX;
        }
    }
    return RetVal;
}


UINT32 AmbaB8_RegWrite(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;
#if (B8_DBG_TEMP)
    ULONG RegAddr;
    UINT32 CurRegAddr;
    extern UINT32 gB8DbgShowWCmd;
#endif

    if ((pTxDataBuf == NULL) || (ChipID == 0U)) {
        RetVal = B8_ERR_ARG;
    } else {
        if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
            if (AmbaB8_KAL_MutexTake(&B8Comm_Mutex, AMBAB8_KAL_WAIT_FOREVER) == B8_ERR_NONE) {
                RetVal = AmbaB8_PwmEncRegWrite(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pTxDataBuf);
                (void) AmbaB8_KAL_MutexGive(&B8Comm_Mutex);
            } else {
                RetVal = B8_ERR_MUTEX;
            }

        } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
            RetVal = B8F_RegWrite(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pTxDataBuf);
        } else {
            RetVal = B8_ERR_INVALID_API;
        }
#if (B8_DBG_TEMP)
        AmbaB8_Misra_TypeCast(&RegAddr, &pReg);
        if (gB8DbgShowWCmd != 0U) {
            const UINT32 *pWordData;
            const UINT16 *pHalfWordData;
            UINT32 i = 0U;
            if (DataWidth == B8_DATA_WIDTH_32BIT) {
                AmbaB8_Misra_TypeCast(&pWordData, &pTxDataBuf);
                for (i = 0; i < DataSize; i++) {
                    CurRegAddr = RegAddr + (i * 4U);
                    if ((CurRegAddr & B8_APB_BASE_ADDR) == B8_APB_BASE_ADDR) {
                        AmbaB8_PrintUInt5("sendln 't b8 writew 0x%x apb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pWordData[i], 0U, 0U);
                    } else {
                        AmbaB8_PrintUInt5("sendln 't b8 writew 0x%x ahb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pWordData[i], 0U, 0U);
                    }
                }
            } else if (DataWidth == B8_DATA_WIDTH_16BIT) {
                AmbaB8_Misra_TypeCast(&pHalfWordData, &pTxDataBuf);
                for (i = 0; i < DataSize; i++) {
                    CurRegAddr = RegAddr + (i * 2U);
                    if ((CurRegAddr & B8_APB_BASE_ADDR) == B8_APB_BASE_ADDR) {
                        AmbaB8_PrintUInt5("sendln 't b8 writeh 0x%x apb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pHalfWordData[i], 0U, 0U);
                    } else {
                        AmbaB8_PrintUInt5("sendln 't b8 writeh 0x%x ahb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pHalfWordData[i], 0U, 0U);
                    }
                }
            } else {
                RetVal = B8_ERR_ARG;
            }
        }
#endif
    }
    return RetVal;
}

UINT32 AmbaB8_RegRead(UINT32 ChipID, const volatile void *pReg, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;

    if ((pRxDataBuf == NULL) || (ChipID == 0U)) {
        RetVal = B8_ERR_ARG;
    } else {
        if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
            if (AmbaB8_KAL_MutexTake(&B8Comm_Mutex, AMBAB8_KAL_WAIT_FOREVER) == B8_ERR_NONE) {
                RetVal = AmbaB8_PwmEncRegRead(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pRxDataBuf);
                (void) AmbaB8_KAL_MutexGive(&B8Comm_Mutex);
            } else {
                RetVal = B8_ERR_MUTEX;
            }
        } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
            RetVal = B8F_RegRead(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pRxDataBuf);

        } else {
            RetVal = B8_ERR_INVALID_API;
        }
    }

    return RetVal;
}

UINT32 AmbaB8_RegWriteU32(UINT32 ChipID, ULONG RegAddr, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, const void *pTxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;
    const volatile void* pReg;
#if (B8_DBG_TEMP)
    extern UINT32 gB8DbgShowWCmd;
    ULONG CurRegAddr;
    const UINT32 *pWordData;
    const UINT16 *pHalfWordData;
    UINT32 i = 0U;
#endif

    if ((pTxDataBuf == NULL) || (ChipID == 0U)) {
        RetVal = B8_ERR_ARG;

    } else {
        AmbaB8_Misra_TypeCast(&pReg, &RegAddr);

        if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
            if (AmbaB8_KAL_MutexTake(&B8Comm_Mutex, AMBAB8_KAL_WAIT_FOREVER) == B8_ERR_NONE) {
                RetVal = AmbaB8_PwmEncRegWrite(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pTxDataBuf);
                (void) AmbaB8_KAL_MutexGive(&B8Comm_Mutex);
            } else {
                RetVal = B8_ERR_MUTEX;
            }
        } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
            RetVal = B8F_RegWrite(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pTxDataBuf);
        } else {
            RetVal = B8_ERR_INVALID_API;
        }
    }

#if (B8_DBG_TEMP)
    if (RetVal != B8_ERR_NONE) {
        AmbaB8_PrintUInt5("AmbaB8_RegWriteU32 #err=%d", RetVal, 0U, 0U, 0U, 0U);
    }

    if (gB8DbgShowWCmd != 0U) {
        if (DataWidth == B8_DATA_WIDTH_32BIT) {
            AmbaB8_Misra_TypeCast(&pWordData, &pTxDataBuf);
            for (i = 0U; i < DataSize; i++) {
                CurRegAddr = RegAddr + (i * 4U);
                if ((CurRegAddr & B8_APB_BASE_ADDR) == B8_APB_BASE_ADDR) {
                    AmbaB8_PrintUInt5("sendln 't b8 writew 0x%x apb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pWordData[i], 0U, 0U);
                } else {
                    AmbaB8_PrintUInt5("sendln 't b8 writew 0x%x ahb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pWordData[i], 0U, 0U);
                }
            }
        } else if (DataWidth == B8_DATA_WIDTH_16BIT) {
            AmbaB8_Misra_TypeCast(&pHalfWordData, &pTxDataBuf);
            for (i = 0U; i < DataSize; i++) {
                CurRegAddr = RegAddr + (i * 2U);
                if ((CurRegAddr & B8_APB_BASE_ADDR) == B8_APB_BASE_ADDR) {
                    AmbaB8_PrintUInt5("sendln 't b8 writeh 0x%x apb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pHalfWordData[i], 0U, 0U);
                } else {
                    AmbaB8_PrintUInt5("sendln 't b8 writeh 0x%x ahb 0x%04x 0x%08x'", ChipID, (CurRegAddr&0xffffU), pHalfWordData[i], 0U, 0U);
                }
            }
        } else {
            RetVal = B8_ERR_ARG;
        }
    }
#endif

    return RetVal;
}

UINT32 AmbaB8_RegReadU32(UINT32 ChipID, ULONG RegAddr, UINT8 AddrIncrement, UINT32 DataWidth, UINT32 DataSize, void *pRxDataBuf)
{
    UINT32 RetVal = B8_ERR_NONE;
    const volatile void* pReg;

    if ((pRxDataBuf == NULL) || (ChipID == 0U)) {
        RetVal = B8_ERR_ARG;
    } else {
        AmbaB8_Misra_TypeCast(&pReg, &RegAddr);

        if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
            if (AmbaB8_KAL_MutexTake(&B8Comm_Mutex, AMBAB8_KAL_WAIT_FOREVER) == B8_ERR_NONE) {
                RetVal = AmbaB8_PwmEncRegRead(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pRxDataBuf);
                (void) AmbaB8_KAL_MutexGive(&B8Comm_Mutex);
            } else {
                RetVal = B8_ERR_MUTEX;
            }
        } else if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
            RetVal = B8F_RegRead(ChipID, pReg, AddrIncrement, DataWidth, DataSize, pRxDataBuf);

        } else {
            RetVal = B8_ERR_INVALID_API;
        }
    }

    return RetVal;
}

