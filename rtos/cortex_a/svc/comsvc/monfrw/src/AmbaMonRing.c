/**
 *  @file AmbaMonRing.c
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
 *  @details Amba Monitor Ring
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaMonVar.h"

#include "AmbaMonRing.h"
#include "AmbaMonRing_Internal.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static AMBA_MON_RING_s AmbaMonRing;

/**
 *  Amba monitor ring init
 *  @return error code
 */
UINT32 AmbaMonRing_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* mutex */
    FuncRetCode = AmbaKAL_MutexCreate(&(AmbaMonRing.Mutex), NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    /* list header */
    AmbaMonRing.pIdList = NULL;

    return RetCode;
}

/**
 *  @private
 *  Amba monitor ring create
 *  @param[in] pRingId pointer to the ring obj id
 *  @param[in] pName pointer to the ring obj string
 *  @param[in] pChunkInfo pointer to the ring chunk data information
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonRing_Create(AMBA_MON_RING_ID_s *pRingId, const char *pName, const AMBA_MON_RING_CHUNK_INFO_s *pChunkInfo)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_RING_ID_s *pIdList;

    if ((pRingId != NULL) &&
        (pRingId->Magic != 0xCafeU) &&
        (pName != NULL) &&
        (pChunkInfo != NULL) &&
        (pChunkInfo->pMemBase != NULL)) {
        /* create mutex */
        FuncRetCode = AmbaKAL_MutexCreate(&(pRingId->Mutex), NULL);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* buf index reset */
            pRingId->Index.Rd = 0U;
            pRingId->Index.Wr = 0U;

            /* chunk info */
            FuncRetCode = AmbaWrap_memcpy(&(pRingId->Buf.ChunkInfo), pChunkInfo, sizeof(AMBA_MON_RING_CHUNK_INFO_s));
            if (FuncRetCode == OK) {
                /* magic (valid), name */
                pRingId->Magic = 0xCafeU;
                pRingId->pName = pName;

                /* port list init */
                pRingId->pPortList = NULL;

                /* take ring list */
                FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonRing.Mutex), AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode == KAL_ERR_NONE) {
                    /* hook to ring list */
                    if (AmbaMonRing.pIdList == NULL) {
                        pRingId->Link.Up = NULL;
                        pRingId->Link.Down = NULL;
                        AmbaMonRing.pIdList = pRingId;
                    } else {
                        pIdList = AmbaMonRing.pIdList;
                        while (pIdList->Link.Down != NULL) {
                            pIdList = pIdList->Link.Down;
                        }
                        pRingId->Link.Up = pIdList;
                        pRingId->Link.Down = NULL;
                        pIdList->Link.Down = pRingId;
                    }

                    /* give ring list */
                    FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonRing.Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            } else {
                /* memcpy fail */
                RetCode = NG_UL;
            }
        }
    } else {
        /* ring/name/chunk null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor ring reset
 *  @param[in] pRingId pointer to the ring obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonRing_Reset(AMBA_MON_RING_ID_s *pRingId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_RING_PORT_s *pPortList;

    if ((pRingId != NULL) &&
        (pRingId->Magic == 0xCafeU)) {
        /* take ring id */
        FuncRetCode = AmbaKAL_MutexTake(&(pRingId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* rd & we clear */
            pRingId->Index.Rd = 0U;
            pRingId->Index.Wr = 0U;

            pPortList = pRingId->pPortList;
            /* notification */
            while (pPortList != NULL) {
                if (pPortList->Magic == 0xCafeU) {
                    FuncRetCode = AmbaKAL_MutexTake(&(pPortList->Mutex), AMBA_KAL_NO_WAIT);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        pPortList->Op.Bits.Reset = 1U;
                        FuncRetCode = AmbaKAL_MutexGive(&(pPortList->Mutex));
                        if (FuncRetCode != KAL_ERR_NONE) {
                            /* */
                        }
                    } else {
                        pPortList->Op.Bits.Reset = 1U;
                    }
                }
                pPortList = pPortList->Link.Down;
            }

            /* give ring id */
            FuncRetCode = AmbaKAL_MutexGive(&(pRingId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* ring null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor ring delete
 *  @param[in] pRingId pointer to the ring obj id
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonRing_Delete(AMBA_MON_RING_ID_s *pRingId)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pRingId != NULL) &&
        (pRingId->Magic == 0xCafeU) &&
        (pRingId->pPortList == NULL)) {
        /* take ring list */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonRing.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take ring id */
            FuncRetCode = AmbaKAL_MutexTake(&(pRingId->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* remove from ring list */
                if (AmbaMonRing.pIdList != NULL) {
                    /* up link */
                    if (pRingId->Link.Up == NULL) {
                        /* root */
                        AmbaMonRing.pIdList= pRingId->Link.Down;
                    } else {
                        /* linker */
                        pRingId->Link.Up->Link.Down = pRingId->Link.Down;
                    }
                    /* down link */
                    if (pRingId->Link.Down != NULL) {
                        /* root/linker */
                        pRingId->Link.Down->Link.Up = pRingId->Link.Up;
                    }
                    /* magic (invaild), name */
                    pRingId->Magic = 0U;
                    pRingId->pName = NULL;

                    /* give ring id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pRingId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* mutex delete */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pRingId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    RetCode = NG_UL;
                    /* give ring id */
                    FuncRetCode = AmbaKAL_MutexGive(&(pRingId->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }

            /* give ring list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonRing.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* ring null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor ring open
 *  @param[in] pPort pointer to the ring port
 *  @param[in] pName pointer to the ring obj string
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonRing_Open(AMBA_MON_RING_PORT_s *pPort, const char *pName)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AMBA_MON_RING_ID_s *pIdList;
    AMBA_MON_RING_PORT_s *pPortList;

    if ((pPort != NULL) &&
        (pPort->Magic != 0xCafeU) &&
        (pName != NULL)) {
        /* take ring lsit */
        FuncRetCode = AmbaKAL_MutexTake(&(AmbaMonRing.Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* ring id find */
            if (AmbaMonRing.pIdList != NULL) {
                /* search by name */
                pIdList = AmbaMonRing.pIdList;
                do {
                    FuncRetCode = svar_strcmp(pName, pIdList->pName);
                    if (FuncRetCode == OK_UL) {
                        /* found */
                        break;
                    }
                    /* next */
                    pIdList = pIdList->Link.Down;
                } while (pIdList != NULL);

                if (pIdList != NULL) {
                    /* port mutex */
                    FuncRetCode = AmbaKAL_MutexCreate(&(pPort->Mutex), NULL);
                    if (FuncRetCode == KAL_ERR_NONE) {
                        /* port settings */
                        pPort->Magic = 0xCafeU;
                        pPort->pRingId = pIdList;
                        pPort->Link.Up = NULL;
                        pPort->Link.Down = NULL;

                        /* port index */
                        pPort->Index.Rd = pPort->pRingId->Index.Rd;
                        pPort->Index.Wr = pPort->pRingId->Index.Wr;

                        /* port op status clear */
                        pPort->Op.Bits.Reset = 0;
                        pPort->Op.Bits.Overflow = 0;

                        /* take ring id */
                        FuncRetCode = AmbaKAL_MutexTake(&(pIdList->Mutex), AMBA_KAL_WAIT_FOREVER);
                        if (FuncRetCode == KAL_ERR_NONE) {
                            /* hook port to ring */
                            pPortList = pPort->pRingId->pPortList;
                            if (pPortList == NULL) {
                                /* root */
                                pPort->pRingId->pPortList = pPort;
                            } else {
                                /* last */
                                while (pPortList->Link.Down != NULL) {
                                    pPortList = pPortList->Link.Down;
                                }
                                pPort->Link.Up = pPortList;
                                pPortList->Link.Down = pPort;
                            }

                            /* give ring id */
                            FuncRetCode = AmbaKAL_MutexGive(&(pIdList->Mutex));
                            if (FuncRetCode != KAL_ERR_NONE) {
                                /* */
                            }
                        }
                    }
                } else {
                    RetCode = NG_UL;
                }
            } else {
                RetCode = NG_UL;
            }

            /* give ring list */
            FuncRetCode = AmbaKAL_MutexGive(&(AmbaMonRing.Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port/name null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor ring read seek
 *  @param[in] pPort pointer to the ring port
 *  @param[in] Offset position offset
 *  @param[in] Flag position flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonRing_RSeek(AMBA_MON_RING_PORT_s *pPort, UINT32 Offset, UINT32 Flag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Rd, Wr, Num;
    UINT32 Length;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU)) {
        /* take ring id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pRingId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take ring port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* rd, wr & num */
                Rd  = pPort->pRingId->Index.Rd;
                Wr  = pPort->pRingId->Index.Wr;
                Num = pPort->pRingId->Buf.ChunkInfo.Num;

                if (Wr >= Rd) {
                    Length = Wr - Rd;
                } else {
                    Length = Num + Wr - Rd;
                }

                switch (Flag) {
                    case (UINT32) R_SEEK_PASS_BEGIN:
                        if (Wr == Rd) {
                            /* none */
                            pPort->Index.Rd = Rd;
                        } else {
                            /* pass shift */
                            if (Offset < Length) {
                                /* offset */
                                pPort->Index.Rd = (Wr + Num - (Offset + 1U)) % Num;
                            } else {
                                /* last */
                                pPort->Index.Rd = Rd;
                                RetCode = NG_UL;
                            }
                        }
                        break;
                    case (UINT32) R_SEEK_PASS_LAST:
                        pPort->Index.Rd = Rd;
                        break;
                    default:
                        RetCode = NG_UL;
                        break;
                }

                if (RetCode == OK_UL) {
                    /* overflow/reset clear */
                    pPort->Op.Bits.Reset = 0;
                    pPort->Op.Bits.Overflow = 0;
                }

                /* give ring port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give ring id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pRingId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor ring write
 *  @param[in] pPort pointer to the ring port
 *  @param[in] pData pointer to the data
 *  @param[in] Size data size
 *  @return ring address
 *  @note this function is intended for internal use only
 */
void *AmbaMonRing_Write(AMBA_MON_RING_PORT_s *pPort, const void *pData, UINT32 Size)
{
    UINT32 FuncRetCode;

    UINT32 Rd, Wr, Num;
    UINT32 RemInNum;

    AMBA_MON_RING_MEM_s MemInfo;

    AMBA_MON_RING_PORT_s *pPortList;

    MemInfo.Ctx.pVoid = NULL;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pRingId != NULL) &&
        (Size <= pPort->pRingId->Buf.ChunkInfo.Size) &&
        (pData != NULL)) {
        /* take ring id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pRingId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take ring port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* rd, wr & num */
                Rd  = pPort->pRingId->Index.Rd;
                Wr  = pPort->pRingId->Index.Wr;
                Num = pPort->pRingId->Buf.ChunkInfo.Num;

                /* save data */
                MemInfo.Ctx.pVoid = pPort->pRingId->Buf.ChunkInfo.pMemBase;
                MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[Wr * pPort->pRingId->Buf.ChunkInfo.Size]);

                FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pData, Size);
                if (FuncRetCode == OK) {
                    /* update wr */
                    pPort->pRingId->Index.Wr = (Wr + 1U) % Num;
                    Wr = pPort->pRingId->Index.Wr;

                    /* remainder in num */
                    if (Wr > Rd) {
                        RemInNum = Num + Rd - Wr;
                    } else {
                        RemInNum = Rd - Wr;
                    }

                    /* check buf in mum, keep aux reserved buf for processing latency */
                    if (RemInNum < pPort->pRingId->Buf.ChunkInfo.AuxInNum) {
                        pPort->pRingId->Index.Rd = (Rd + 1U) % Num;
                    }

                    /* check port list in ring */
                    pPortList = pPort->pRingId->pPortList;

                    while (pPortList != NULL) {
                        /* take ring port */
                        if (pPortList != pPort) {
                            FuncRetCode = AmbaKAL_MutexTake(&(pPortList->Mutex), AMBA_KAL_WAIT_FOREVER);
                        } else {
                            FuncRetCode = KAL_ERR_NONE;
                        }
                        if (FuncRetCode == KAL_ERR_NONE) {
                            pPortList->Index.Wr = Wr;
                            if (pPortList->Index.Rd == pPortList->Index.Wr) {
                                /* overflow */
                                pPortList->Op.Bits.Overflow = 1;
                            }
                            /* give ring port */
                            if (pPortList != pPort) {
                                FuncRetCode = AmbaKAL_MutexGive(&(pPortList->Mutex));
                                if (FuncRetCode != KAL_ERR_NONE) {
                                    /* */
                                }
                            }
                        }
                        /* next port */
                        pPortList = pPortList->Link.Down;
                    }
                }

                /* give ring port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give ring id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pRingId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    }

    return MemInfo.Ctx.pVoid;
}

/**
 *  @private
 *  Amba monitor ring write 2
 *  @param[in] pPort pointer to the ring port
 *  @param[in] pHeader pointer to the header
 *  @param[in] HSize header size
 *  @param[in] pData pointer to the data
 *  @param[in] DSize data size
 *  @return ring address
 *  @note this function is intended for internal use only
 */
void *AmbaMonRing_Write2(AMBA_MON_RING_PORT_s *pPort, const void *pHeader, UINT32 HSize, const void *pData, UINT32 DSize)
{
    UINT32 FuncRetCode;
    UINT32 Size;

    UINT32 Rd, Wr, Num;
    UINT32 RemInNum;

    AMBA_MON_RING_MEM_s MemInfo;
    AMBA_MON_RING_MEM_s MemInfo2;

    AMBA_MON_RING_PORT_s *pPortList;

    MemInfo.Ctx.pVoid = NULL;

    /* data size */
    Size = HSize + DSize;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pRingId != NULL) &&
        (Size <= pPort->pRingId->Buf.ChunkInfo.Size) &&
        (pHeader != NULL) &&
        (pData != NULL)) {
        /* take ring id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pRingId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take ring port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* rd, wr & num */
                Rd  = pPort->pRingId->Index.Rd;
                Wr  = pPort->pRingId->Index.Wr;
                Num = pPort->pRingId->Buf.ChunkInfo.Num;

                /* save data */
                MemInfo.Ctx.pVoid = pPort->pRingId->Buf.ChunkInfo.pMemBase;
                MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[Wr * pPort->pRingId->Buf.ChunkInfo.Size]);
                /* header */
                FuncRetCode = AmbaWrap_memcpy(MemInfo.Ctx.pVoid, pHeader, HSize);
                if (FuncRetCode == OK) {
                    /* message */
                    MemInfo2.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HSize]);
                    FuncRetCode = AmbaWrap_memcpy(MemInfo2.Ctx.pVoid, pData, DSize);
                    if (FuncRetCode == OK) {
                        /* update wr */
                        pPort->pRingId->Index.Wr = (Wr + 1U) % Num;
                        Wr = pPort->pRingId->Index.Wr;

                        /* remainder in num */
                        if (Wr > Rd) {
                            RemInNum = Num + Rd - Wr;
                        } else {
                            RemInNum = Rd - Wr;
                        }

                        /* check buf in mum, keep aux reserved buf for processing latency */
                        if (RemInNum < pPort->pRingId->Buf.ChunkInfo.AuxInNum) {
                            pPort->pRingId->Index.Rd = (Rd + 1U) % Num;
                        }

                        /* check port list in ring */
                        pPortList = pPort->pRingId->pPortList;

                        while (pPortList != NULL) {
                            /* take ring port */
                            if (pPortList != pPort) {
                                FuncRetCode = AmbaKAL_MutexTake(&(pPortList->Mutex), AMBA_KAL_WAIT_FOREVER);
                            } else {
                                FuncRetCode = KAL_ERR_NONE;
                            }
                            if (FuncRetCode == KAL_ERR_NONE) {
                                pPortList->Index.Wr = Wr;
                                if (pPortList->Index.Rd == pPortList->Index.Wr) {
                                    /* overflow */
                                    pPortList->Op.Bits.Overflow = 1;
                                }
                                /* give ring port */
                                if (pPortList != pPort) {
                                    FuncRetCode = AmbaKAL_MutexGive(&(pPortList->Mutex));
                                    if (FuncRetCode != KAL_ERR_NONE) {
                                        /* */
                                    }
                                }
                            }
                            /* next port */
                            pPortList = pPortList->Link.Down;
                        }
                    }
                }

                /* give ring port */
                FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* */
                }
            }

            /* give ring id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pRingId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    }

    return MemInfo.Ctx.pVoid;
}

/**
 *  @private
 *  Amba monitor ring read
 *  @param[in] pPort pointer to the ring port
 *  @param[in] pData pointer to the data pointer
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonRing_Read(AMBA_MON_RING_PORT_s *pPort, void **pData)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Rd, Wr, Num;

    AMBA_MON_RING_MEM_s MemInfo;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pRingId != NULL)) {
        /* take ring port */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            if ((((UINT32) pPort->Op.Bits.Overflow) == 0U) && (((UINT32) pPort->Op.Bits.Reset) == 0U)) {
                /* read data */
                Rd  = pPort->Index.Rd;
                Wr  = pPort->Index.Wr;
                Num = pPort->pRingId->Buf.ChunkInfo.Num;
                if (Rd != Wr) {
                    MemInfo.Ctx.pVoid = pPort->pRingId->Buf.ChunkInfo.pMemBase;
                    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[Rd * pPort->pRingId->Buf.ChunkInfo.Size]);
                    *pData = MemInfo.Ctx.pVoid;
                    pPort->Index.Rd = (Rd + 1U) % Num;
                } else {
                    RetCode = NG_UL;
                }
            } else {
                /* overflow/reset */
                if (((UINT32) pPort->Op.Bits.Reset) == 1U) {
                    RetCode = MON_RING_RESET;
                } else {
                    RetCode = MON_RING_OVERFLOW;
                }
            }
            /* give ring port */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor ring close
 *  @param[in] pPort pointer to the ring port
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonRing_Close(AMBA_MON_RING_PORT_s *pPort)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if ((pPort != NULL) &&
        (pPort->Magic == 0xCafeU) &&
        (pPort->pRingId != NULL)) {
        /* take ring id */
        FuncRetCode = AmbaKAL_MutexTake(&(pPort->pRingId->Mutex), AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* take ring port */
            FuncRetCode = AmbaKAL_MutexTake(&(pPort->Mutex), AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* remove port list  */
                if (pPort->pRingId->pPortList != NULL) {
                    /* up link check */
                    if (pPort->Link.Up == NULL) {
                        /* root */
                        pPort->pRingId->pPortList = pPort->Link.Down;
                    } else {
                        /* linker */
                        pPort->Link.Up->Link.Down = pPort->Link.Down;
                    }
                    /* down link check */
                    if (pPort->Link.Down != NULL) {
                        /* root and linker */
                        pPort->Link.Down->Link.Up = pPort->Link.Up;
                    }
                    /* clear magic */
                    pPort->Magic = 0U;

                    /* give ring port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                    /* delete ring port mutex */
                    FuncRetCode = AmbaKAL_MutexDelete(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                } else {
                    RetCode = NG_UL;
                    /* give ring port */
                    FuncRetCode = AmbaKAL_MutexGive(&(pPort->Mutex));
                    if (FuncRetCode != KAL_ERR_NONE) {
                        /* */
                    }
                }
            }
            /* give ring id */
            FuncRetCode = AmbaKAL_MutexGive(&(pPort->pRingId->Mutex));
            if (FuncRetCode != KAL_ERR_NONE) {
                /* */
            }
        }
    } else {
        /* port null or invalid */
        RetCode = NG_UL;
    }

    return RetCode;
}

