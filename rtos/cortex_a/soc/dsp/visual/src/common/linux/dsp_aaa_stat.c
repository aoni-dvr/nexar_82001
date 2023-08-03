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

#include "AmbaDSP_Def.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "dsp_osal.h"

#if defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_H32) || defined (CONFIG_SOC_CV28)
static UINT32 aaa_process_cfa_statistic_header(const UINT8 *pSrcStitchNumX,
                                               const UINT8 *pSrcStitchNumY,
                                               const ULONG *SrcImgCfaAaaStatAddr,
                                               AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;
    UINT32 SrcIdx;
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    const AMBA_IK_3A_HEADER_s *pSrcCfaHeader;
    AMBA_IK_3A_HEADER_s *pDstCfaHeader;

    for (i = 0U; i < *pSrcStitchNumY; i++) {
        for (j = 0U; j < *pSrcStitchNumX; j++) {

            SrcIdx = j + (*pSrcStitchNumX *i);
            dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);
            //dsp_osal_printU5("############## SOURCE CFA 3a pSrcStitchNumX:%d, pSrcStitchNumY:%d addr:%p, size:%d", j, i, SrcImgCfaAaaStatAddr[SrcIdx], sizeof(AMBA_IK_CFA_3A_DATA_s), 0U);
            //===== pointer init =====//
            pSrcCfaHeader = &pSrcImgCfaAaaStat->Header;
            pDstCfaHeader = &pDstImgCfaAaaStat->Header;

            //===== Cfa Header =====//
            if ((i==0U) && (j==0U)) {
                (void)dsp_osal_memcpy(pDstCfaHeader, pSrcCfaHeader, sizeof(AMBA_IK_3A_HEADER_s));
                // Reset reserved bits
                pDstCfaHeader->Reserved0 = 0U;
                (void)dsp_osal_memset(&pDstCfaHeader->Reserved1[0U], 0, sizeof(UINT32)*2U);
                pDstCfaHeader->Reserved2 = 0U;
                pDstCfaHeader->Reserved3 = 0U;
                (void)dsp_osal_memset(&pDstCfaHeader->reserved4[0U], 0, sizeof(UINT32)*13U);
            } else {
                if (i == 0U) {
                    pDstCfaHeader->AwbTileNumCol += pSrcCfaHeader->AwbTileNumCol;
                    pDstCfaHeader->AeTileNumCol  += pSrcCfaHeader->AeTileNumCol;
                    pDstCfaHeader->AfTileNumCol  += pSrcCfaHeader->AfTileNumCol;
                }
                if (j == 0U) {
                    pDstCfaHeader->AwbTileNumRow += pSrcCfaHeader->AwbTileNumRow;
                    pDstCfaHeader->AeTileNumRow  += pSrcCfaHeader->AeTileNumRow;
                    pDstCfaHeader->AfTileNumRow  += pSrcCfaHeader->AfTileNumRow;
                }
            }

            //===== Cfa frame_id =====//
            if ((i==0U) && (j==0U)) {
                pDstImgCfaAaaStat->FrameId = pSrcImgCfaAaaStat->FrameId;
            } else {
                // DO NOTHING
            }
        }
    }

    return Rval;
}

static void aaa_process_cfa_awb_statistic(UINT32 CurIdxX,
                                          UINT32 CurIdxY,
                                          const UINT8 *pSrcStitchNumX,
                                          const ULONG *SrcImgCfaAaaStatAddr,
                                          AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const AMBA_IK_3A_HEADER_s *pSrcCfaHeader;
    const AMBA_IK_CFA_AWB_s *pSrcAwb;
    AMBA_IK_CFA_AWB_s *pDstAwb;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG ULAddr;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);

    //===== pointer init =====//
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcImgCfaAaaStat);

    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[m]);
        offset += pSrcImgCfaAaaStat->Header.AwbTileNumCol;
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgCfaAaaStat->Header.AwbTileNumCol*(UINT32)pSrcImgCfaAaaStat->Header.AwbTileNumRow);
    }

    pDstAwb = &pDstImgCfaAaaStat->Awb[0];
    dsp_osal_typecast(&ULAddr, &pDstAwb);
    ULAddr += offset * sizeof(AMBA_IK_CFA_AWB_s);
    dsp_osal_typecast(&pDstAwb, &ULAddr);

    dsp_osal_typecast(&ULAddr, &pSrcImgCfaAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    dsp_osal_typecast(&pSrcAwb, &ULAddr);

    for (k = 0U; k < pSrcCfaHeader->AwbTileNumRow; k++) {
        (void)dsp_osal_memcpy(pDstAwb, pSrcAwb, sizeof(AMBA_IK_CFA_AWB_s)*pSrcCfaHeader->AwbTileNumCol);

        dsp_osal_typecast(&ULAddr, &pSrcAwb);
        ULAddr = ULAddr + (pSrcCfaHeader->AwbTileNumCol*sizeof(AMBA_IK_CFA_AWB_s));
        dsp_osal_typecast(&pSrcAwb, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAwb);
        ULAddr = ULAddr + (pDstImgCfaAaaStat->Header.AwbTileNumCol*sizeof(AMBA_IK_CFA_AWB_s));
        dsp_osal_typecast(&pDstAwb, &ULAddr);
    }

}

static void aaa_process_cfa_ae_statistic(UINT32 CurIdxX,
                                         UINT32 CurIdxY,
                                         const UINT8 *pSrcStitchNumX,
                                         const ULONG *SrcImgCfaAaaStatAddr,
                                         AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const AMBA_IK_3A_HEADER_s *pSrcCfaHeader;
    const AMBA_IK_CFA_AE_s *pSrcAe;
    AMBA_IK_CFA_AE_s *pDstAe;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG ULAddr;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);

    //===== pointer init =====//
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcImgCfaAaaStat);

    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[m]);
        offset += pSrcImgCfaAaaStat->Header.AeTileNumCol;
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgCfaAaaStat->Header.AeTileNumCol*(UINT32)pSrcImgCfaAaaStat->Header.AeTileNumRow);
    }
    pDstAe = &pDstImgCfaAaaStat->Ae[0];
    dsp_osal_typecast(&ULAddr, &pDstAe);
    ULAddr = ULAddr + (offset * sizeof(AMBA_IK_CFA_AE_s));
    dsp_osal_typecast(&pDstAe, &ULAddr);

    dsp_osal_typecast(&ULAddr, &pSrcImgCfaAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16); //FrmId
    ULAddr += (sizeof(AMBA_IK_CFA_AWB_s)*pSrcCfaHeader->AwbTileNumRow)*pSrcCfaHeader->AwbTileNumCol;
    dsp_osal_typecast(&pSrcAe, &ULAddr);

    for (k = 0U; k < pSrcCfaHeader->AeTileNumRow; k++) {
        (void)dsp_osal_memcpy(pDstAe, pSrcAe, sizeof(AMBA_IK_CFA_AE_s)*pSrcCfaHeader->AeTileNumCol);

        dsp_osal_typecast(&ULAddr, &pSrcAe);
        ULAddr += pSrcCfaHeader->AeTileNumCol * sizeof(AMBA_IK_CFA_AE_s);
        dsp_osal_typecast(&pSrcAe, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAe);
        ULAddr += pDstImgCfaAaaStat->Header.AeTileNumCol*sizeof(AMBA_IK_CFA_AE_s);
        dsp_osal_typecast(&pDstAe, &ULAddr);
    }
}

static void aaa_process_cfa_af_statistic(UINT32 CurIdxX,
                                         UINT32 CurIdxY,
                                         const UINT8 *pSrcStitchNumX,
                                         const ULONG *SrcImgCfaAaaStatAddr,
                                         AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const void *pSrcCfa;
    const AMBA_IK_3A_HEADER_s *pSrcCfaHeader;
    const AMBA_IK_CFA_AF_s *pSrcAf;
    AMBA_IK_CFA_AF_s *pDstAf;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG ULAddr;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);

    //===== pointer init =====//
    dsp_osal_typecast(&pSrcCfa, &pSrcImgCfaAaaStat);
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcCfa);

    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[m]);
        offset += pSrcImgCfaAaaStat->Header.AfTileNumCol;
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgCfaAaaStat->Header.AfTileNumCol*(UINT32)pSrcImgCfaAaaStat->Header.AfTileNumRow);
    }

    pDstAf = &pDstImgCfaAaaStat->Af[0];
    dsp_osal_typecast(&ULAddr, &pDstAf);
    ULAddr += offset * sizeof(AMBA_IK_CFA_AF_s);
    dsp_osal_typecast(&pDstAf, &ULAddr);

    dsp_osal_typecast(&ULAddr, &pSrcCfa);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_CFA_AWB_s)*pSrcCfaHeader->AwbTileNumRow)*pSrcCfaHeader->AwbTileNumCol;
    ULAddr += (sizeof(AMBA_IK_CFA_AE_s)*pSrcCfaHeader->AeTileNumRow)*pSrcCfaHeader->AeTileNumCol;
    dsp_osal_typecast(&pSrcCfa, &ULAddr);
    dsp_osal_typecast(&pSrcAf, &pSrcCfa);

    for (k = 0U; k < pSrcCfaHeader->AfTileNumRow; k++) {
        (void)dsp_osal_memcpy(pDstAf, pSrcAf, sizeof(AMBA_IK_CFA_AF_s)*pSrcCfaHeader->AfTileNumCol);
        dsp_osal_typecast(&ULAddr, &pSrcAf);
        ULAddr += pSrcCfaHeader->AfTileNumCol*sizeof(AMBA_IK_CFA_AF_s);
        dsp_osal_typecast(&pSrcAf, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAf);
        ULAddr += pDstImgCfaAaaStat->Header.AfTileNumCol*sizeof(AMBA_IK_CFA_AF_s);
        dsp_osal_typecast(&pDstAf, &ULAddr);
    }
}

static void aaa_process_cfa_histo_statistic(UINT32 CurIdxX,
                                            UINT32 CurIdxY,
                                            const UINT8 *pSrcStitchNumX,
                                            const ULONG *SrcImgCfaAaaStatAddr,
                                            AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const void *pSrcCfa;
    const AMBA_IK_3A_HEADER_s *pSrcCfaHeader;
    const AMBA_IK_CFA_HISTO_s *pSrcHisto;
    AMBA_IK_CFA_HISTO_s *pDstHisto;
    ULONG ULAddr;
    UINT32 k;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);

    //===== pointer init =====//
    dsp_osal_typecast(&pSrcCfa, &pSrcImgCfaAaaStat);
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcCfa);

    pDstHisto = &pDstImgCfaAaaStat->Histogram;

    dsp_osal_typecast(&ULAddr, &pSrcCfa);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_CFA_AWB_s)*pSrcCfaHeader->AwbTileNumRow)*pSrcCfaHeader->AwbTileNumCol;
    ULAddr += (sizeof(AMBA_IK_CFA_AE_s)*pSrcCfaHeader->AeTileNumRow)*pSrcCfaHeader->AeTileNumCol;
    ULAddr += (sizeof(AMBA_IK_CFA_AF_s)*pSrcCfaHeader->AfTileNumRow)*pSrcCfaHeader->AfTileNumCol;
    dsp_osal_typecast(&pSrcCfa, &ULAddr);
    dsp_osal_typecast(&pSrcHisto, &pSrcCfa);

    if ((CurIdxY == 0U) && (CurIdxX == 0U)) {
        (void)dsp_osal_memcpy(pDstHisto, pSrcHisto, sizeof(AMBA_IK_CFA_HISTO_s));
    } else {
        for (k = 0U; k< 64U; k++) {
            pDstHisto->HisBinR[k] += pSrcHisto->HisBinR[k];
            pDstHisto->HisBinG[k] += pSrcHisto->HisBinG[k];
            pDstHisto->HisBinB[k] += pSrcHisto->HisBinB[k];
            pDstHisto->HisBinY[k] += pSrcHisto->HisBinY[k];
        }
    }
}

static UINT32 aaa_process_cfa_statistic(const UINT8 *pSrcStitchNumX,
                                        const UINT8 *pSrcStitchNumY,
                                        const ULONG *SrcImgCfaAaaStatAddr,
                                        AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;

    Rval = aaa_process_cfa_statistic_header(pSrcStitchNumX, pSrcStitchNumY, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);

    for (i = 0U; i < *pSrcStitchNumY; i++) {
        for (j = 0U; j < *pSrcStitchNumX; j++) {
            aaa_process_cfa_awb_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
            aaa_process_cfa_ae_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
            aaa_process_cfa_af_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
            aaa_process_cfa_histo_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
        }
    }

    return Rval;
}

static UINT32 aaa_process_pg_statistic_header(const UINT8 *pSrcStitchNumX,
                                              const UINT8 *pSrcStitchNumY,
                                              const ULONG *SrcImgPgAaaStatAddr,
                                              AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;
    UINT32 SrcIdx;
    const AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;
    const AMBA_IK_3A_HEADER_s *pSrcPgHeader;
    AMBA_IK_3A_HEADER_s *pDstPgHeader;

    for (i = 0U; i< *pSrcStitchNumY; i++) {
        for (j = 0U; j< *pSrcStitchNumX; j++) {
            SrcIdx = j + (*pSrcStitchNumX *i);
            dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[SrcIdx]);
            //dsp_osal_printU5("############## SOURCE CFA 3a pSrcStitchNumX:%d, pSrcStitchNumY:%d addr:%p, size:%d", j, i, SrcImgCfaAaaStatAddr[SrcIdx], sizeof(AMBA_IK_CFA_3A_DATA_s), 0);

            //===== pointer init =====//
            pSrcPgHeader = &pSrcImgPgAaaStat->Header;
            pDstPgHeader = &pDstImgPgAaaStat->Header;

            //===== Cfa Header =====//
            if ((i==0U) && (j==0U)) {
                (void)dsp_osal_memcpy(pDstPgHeader, pSrcPgHeader, sizeof(AMBA_IK_3A_HEADER_s));
                // Reset reserved bits
                pDstPgHeader->Reserved0 = 0U;
                (void)dsp_osal_memset(&pDstPgHeader->Reserved1[0U], 0, sizeof(UINT32)*2U);
                pDstPgHeader->Reserved2 = 0U;
                pDstPgHeader->Reserved3 = 0U;
                (void)dsp_osal_memset(&pDstPgHeader->reserved4[0U], 0, sizeof(UINT32)*13U);
            } else {
                if (i == 0U) {
                    pDstPgHeader->AwbTileNumCol += pSrcPgHeader->AwbTileNumCol;
                    pDstPgHeader->AeTileNumCol += pSrcPgHeader->AeTileNumCol;
                    pDstPgHeader->AfTileNumCol += pSrcPgHeader->AfTileNumCol;
                }
                if (j == 0U) {
                    pDstPgHeader->AwbTileNumRow += pSrcPgHeader->AwbTileNumRow;
                    pDstPgHeader->AeTileNumRow += pSrcPgHeader->AeTileNumRow;
                    pDstPgHeader->AfTileNumRow += pSrcPgHeader->AfTileNumRow;
                }
            }
            //===== Cfa frame_id =====//
            if ((i==0U) && (j==0U)) {
                pDstImgPgAaaStat ->FrameId = pSrcImgPgAaaStat->FrameId;
            } else {
                // DO NOTHING
            }
        }
    }
    return Rval;
}

static void aaa_process_pg_ae_statistic(UINT32 CurIdxX,
                                        UINT32 CurIdxY,
                                        const UINT8 *pSrcStitchNumX,
                                        const ULONG *SrcImgPgAaaStatAddr,
                                        AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    const AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;
    UINT32 SrcIdx;
    const void *pSrcPg;
    const AMBA_IK_3A_HEADER_s *pSrcPgHeader;
    const AMBA_IK_PG_AE_s *pSrcAe;
    AMBA_IK_PG_AE_s *pDstAe;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG ULAddr;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[SrcIdx]);

    //===== pointer init =====//
    dsp_osal_typecast(&pSrcPg, &pSrcImgPgAaaStat);
    dsp_osal_typecast(&pSrcPgHeader, &pSrcPg);

    for(m = 0; m < CurIdxX; m++){
        dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[m]);
        offset += pSrcImgPgAaaStat->Header.AeTileNumCol;
    }

    for(n = 0; n < CurIdxY; n++){
        dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgPgAaaStat->Header.AeTileNumCol*(UINT32)pSrcImgPgAaaStat->Header.AeTileNumRow);
    }

    pDstAe = &pDstImgPgAaaStat->Ae[0];
    dsp_osal_typecast(&ULAddr, &pDstAe);
    ULAddr += offset * sizeof(AMBA_IK_PG_AE_s);
    dsp_osal_typecast(&pDstAe, &ULAddr);

    dsp_osal_typecast(&ULAddr, &pSrcPg);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_PG_AF_s)*pSrcPgHeader->AfTileNumRow)*pSrcPgHeader->AfTileNumCol;
    dsp_osal_typecast(&pSrcPg, &ULAddr);
    dsp_osal_typecast(&pSrcAe, &pSrcPg);

    for (k = 0U; k < pSrcPgHeader->AeTileNumRow; k++) {
        (void)dsp_osal_memcpy(pDstAe, pSrcAe, sizeof(AMBA_IK_PG_AE_s)*pSrcPgHeader->AeTileNumCol);
        dsp_osal_typecast(&ULAddr, &pSrcAe);
        ULAddr = ULAddr + (pSrcPgHeader->AeTileNumCol*sizeof(AMBA_IK_PG_AE_s));
        dsp_osal_typecast(&pSrcAe, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAe);
        ULAddr = ULAddr + (pDstImgPgAaaStat->Header.AeTileNumCol*sizeof(AMBA_IK_PG_AE_s));
        dsp_osal_typecast(&pDstAe, &ULAddr);
    }
}

static void aaa_process_pg_af_statistic(UINT32 CurIdxX,
                                        UINT32 CurIdxY,
                                        const UINT8 *pSrcStitchNumX,
                                        const ULONG *SrcImgPgAaaStatAddr,
                                        AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    const AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;
    UINT32 SrcIdx;
    const void *pSrcPg;
    const AMBA_IK_3A_HEADER_s *pSrcPgHeader;
    const AMBA_IK_PG_AF_s *pSrcAf;
    AMBA_IK_PG_AF_s *pDstAf;
    UINT32 offset = 0;
    UINT32 m, n, k;
    ULONG ULAddr;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[SrcIdx]);

    //===== pointer init =====//
    dsp_osal_typecast(&pSrcPg, &pSrcImgPgAaaStat);
    dsp_osal_typecast(&pSrcPgHeader, &pSrcPg);

    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[m]);
        offset += pSrcImgPgAaaStat->Header.AfTileNumCol;
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgPgAaaStat->Header.AfTileNumCol*(UINT32)pSrcImgPgAaaStat->Header.AfTileNumRow);
    }

    pDstAf = &pDstImgPgAaaStat->Af[0];
    dsp_osal_typecast(&ULAddr, &pDstAf);
    ULAddr += offset * sizeof(AMBA_IK_PG_AF_s);
    dsp_osal_typecast(&pDstAf, &ULAddr);

    dsp_osal_typecast(&ULAddr, &pSrcPg);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    dsp_osal_typecast(&pSrcPg, &ULAddr);
    dsp_osal_typecast(&pSrcAf, &pSrcPg);


    for (k = 0U; k < pSrcPgHeader->AfTileNumRow; k++) {
        (void)dsp_osal_memcpy( pDstAf, pSrcAf, sizeof(AMBA_IK_PG_AF_s)*pSrcPgHeader->AfTileNumCol);
        dsp_osal_typecast(&ULAddr, &pSrcAf);
        ULAddr += pSrcPgHeader->AfTileNumCol*sizeof(AMBA_IK_PG_AF_s);
        dsp_osal_typecast(&pSrcAf, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAf);
        ULAddr += pDstImgPgAaaStat->Header.AfTileNumCol*sizeof(AMBA_IK_PG_AF_s);
        dsp_osal_typecast(&pDstAf, &ULAddr);
    }
}

static void aaa_process_pg_histo_statistic(UINT32 CurIdxX,
                                           UINT32 CurIdxY,
                                           const UINT8 *pSrcStitchNumX,
                                           const ULONG *SrcImgPgAaaStatAddr,
                                           AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    const AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;
    UINT32 SrcIdx;
    const void *pSrcPg;
    const AMBA_IK_3A_HEADER_s *pSrcPgHeader;
    const AMBA_IK_PG_HISTO_s *pSrcHisto;
    AMBA_IK_PG_HISTO_s *pDstHisto;
    ULONG ULAddr;
    UINT32 k;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[SrcIdx]);

    //===== pointer init =====//
    dsp_osal_typecast(&pSrcPg, &pSrcImgPgAaaStat);
    dsp_osal_typecast(&pSrcPgHeader, &pSrcPg);

    pDstHisto = &pDstImgPgAaaStat->Histogram;

    dsp_osal_typecast(&ULAddr, &pSrcPg);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_PG_AF_s)*pSrcPgHeader->AfTileNumRow)*pSrcPgHeader->AfTileNumCol;
    ULAddr += (sizeof(AMBA_IK_PG_AE_s)*pSrcPgHeader->AeTileNumRow)*pSrcPgHeader->AeTileNumCol;
    dsp_osal_typecast(&pSrcPg, &ULAddr);
    dsp_osal_typecast(&pSrcHisto, &pSrcPg);

    if ((CurIdxY == 0U) && (CurIdxX == 0U)) {
        (void)dsp_osal_memcpy(pDstHisto, pSrcHisto, sizeof(AMBA_IK_PG_HISTO_s));
    } else {
        for (k = 0U; k< 64U; k++) {
            pDstHisto->HisBinR[k] += pSrcHisto->HisBinR[k];
            pDstHisto->HisBinG[k] += pSrcHisto->HisBinG[k];
            pDstHisto->HisBinB[k] += pSrcHisto->HisBinB[k];
            pDstHisto->HisBinY[k] += pSrcHisto->HisBinY[k];
        }
    }
}

static UINT32 aaa_process_pg_statistic(const UINT8 *pSrcStitchNumX,
                                       const UINT8 *pSrcStitchNumY,
                                       const ULONG *SrcImgPgAaaStatAddr,
                                       AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;

    Rval = aaa_process_pg_statistic_header(pSrcStitchNumX, pSrcStitchNumY, SrcImgPgAaaStatAddr, pDstImgPgAaaStat);

    for (i = 0U; i < *pSrcStitchNumY; i++) {
        for (j = 0U; j < *pSrcStitchNumX; j++) {
            aaa_process_pg_ae_statistic(j, i, pSrcStitchNumX, SrcImgPgAaaStatAddr, pDstImgPgAaaStat);
            aaa_process_pg_af_statistic(j, i, pSrcStitchNumX, SrcImgPgAaaStatAddr, pDstImgPgAaaStat);
            aaa_process_pg_histo_statistic(j, i, pSrcStitchNumX, SrcImgPgAaaStatAddr, pDstImgPgAaaStat);
        }
    }
    return Rval;
}

static UINT32 aaa_process_hist_statistic(const UINT8 *p_type,
                                         const UINT8 *pSrcStitchNumX,
                                         const UINT8 *pSrcStitchNumY,
                                         const ULONG *SrcImgHistAaaStatAddr,
                                         AMBA_IK_CFA_HIST_STAT_s *pDstImgHistAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i,j,k;
    const AMBA_IK_CFA_HIST_CFG_INFO_s *pSrcHistHeader;
    AMBA_IK_CFA_HIST_CFG_INFO_s *pDstHistHeader;
    const AMBA_IK_CFA_HIST_STAT_s *pSrcImgHistAaaStat;
    UINT32 SrcIdx;

    for (i = 0U; i < *pSrcStitchNumY; i++) {
        for (j = 0U; j < *pSrcStitchNumX; j++) {
            SrcIdx = j + (*pSrcStitchNumX *i);
            //pSrcImgHistAaaStat = (AMBA_IK_CFA_HIST_STAT_s  *)SrcImgHistAaaStatAddr[SrcIdx];
            dsp_osal_typecast(&pSrcImgHistAaaStat, &SrcImgHistAaaStatAddr[SrcIdx]);
            //dsp_osal_printU5("############## SOURCE CFA hist pSrcStitchNumX:%d, pSrcStitchNumY:%d addr:%p, size:%d", j, i, SrcImgHistAaaStatAddr[SrcIdx], sizeof(AMBA_IK_CFA_HIST_STAT_s), 0);
            //===== pointer init =====//
            pSrcHistHeader = &pSrcImgHistAaaStat->Header;
            pDstHistHeader = &pDstImgHistAaaStat->Header;

            //===== Cfa Header =====//
            if ((i==0U) && (j==0U)) {
                (void)dsp_osal_memcpy(pDstHistHeader, pSrcHistHeader, sizeof(AMBA_IK_CFA_HIST_CFG_INFO_s));
                // Reset reserved bits
                pDstHistHeader->Reserved1 = 0U;
                (void)dsp_osal_memset(&pDstHistHeader->Reserved2[0U], 0, sizeof(UINT32)*28U);
            }
            if ((i==(((UINT32)*pSrcStitchNumY)-1U)) && (j==(((UINT32)*pSrcStitchNumX)-1U))) {
                pDstHistHeader->StatsWidth = pSrcHistHeader->StatsLeft + pSrcHistHeader->StatsWidth;
                pDstHistHeader->StatsHeight = pSrcHistHeader->StatsTop + pSrcHistHeader->StatsHeight;
            }
        }
    }

    for (i = 0U; i < *pSrcStitchNumY; i++) {
        for (j = 0U; j < *pSrcStitchNumX; j++) {
            SrcIdx = j + (*pSrcStitchNumX *i);
            //pSrcImgHistAaaStat = (AMBA_IK_CFA_HIST_STAT_s  *)SrcImgHistAaaStatAddr[SrcIdx];
            dsp_osal_typecast(&pSrcImgHistAaaStat, &SrcImgHistAaaStatAddr[SrcIdx]);

            //===== histogram =====//
            {
                const AMBA_IK_CFA_HISTOGRAM_s *pSrcHistogram;
                AMBA_IK_CFA_HISTOGRAM_s *pDstHistogram;

                pSrcHistogram = &pSrcImgHistAaaStat->Data;
                pDstHistogram = &pDstImgHistAaaStat->Data;
                if ((i == 0U) && (j == 0U)) {
                    (void)dsp_osal_memcpy(pDstHistogram, pSrcHistogram, sizeof(AMBA_IK_CFA_HISTOGRAM_s));
                } else {
                    for (k = 0U; k < AMBA_IK_HDR_HISTO_COUNT; k++) {
                        pDstHistogram->HistoBinR[k] += pSrcHistogram->HistoBinR[k];
                        pDstHistogram->HistoBinG[k] += pSrcHistogram->HistoBinG[k];
                        pDstHistogram->HistoBinB[k] += pSrcHistogram->HistoBinB[k];
                        if (*p_type==DSP_HIST_VIDEO_RGBIR) {
                            pDstHistogram->HistoBinIR[k] += pSrcHistogram->HistoBinIR[k];
                        } else {
                            pDstHistogram->HistoBinIR[k] = 0U;
                        }
                    }
                }
            }
        }
    }

    return Rval;
}
#else // CV2xFS
/* Dsp report compact format header, so we shall cast that one instead of IK_3A_HEADER one */
typedef struct {
    UINT32 AwbEnable:1;
    UINT32 AwbTileNumCol:6;
    UINT32 AwbTileNumRow:6;
    UINT32 Reserved0:19;

    UINT32 AwbTileColStart:13;
    UINT32 AwbTileRowStart:13;
    UINT32 Reserved1:6;

    UINT32 AwbTileWidth:9;
    UINT32 AwbTileHeight:9;
    UINT32 Reserved2:14;

    UINT32 AwbTileActiveWidth:9;
    UINT32 AwbTileActiveHeight:9;
    UINT32 Reserved3:14;
} __attribute__((packed)) dsp_awb_stat_t;

typedef struct {
    UINT32 AeEnable:1;
    UINT32 AeTileNumCol:5;
    UINT32 AeTileNumRow:4;
    UINT32 Reserved0:22;

    UINT32 AeTileColStart:13;
    UINT32 AeTileRowStart:13;
    UINT32 Reserved1:6;

    UINT32 AeTileWidth:9;
    UINT32 AeTileHeight:9;
    UINT32 Reserved2:14;

    UINT32 AeMinThresh:14;
    UINT32 AeMaxThresh:14;
    UINT32 Reserved3:4;
} __attribute__((packed)) dsp_cfa_ae_stat_t;

typedef struct {
    UINT32 AeEnable:1;
    UINT32 AeTileNumCol:5;
    UINT32 AeTileNumRow:4;
    UINT32 Reserved0:22;

    UINT32 AeTileColStart:13;
    UINT32 AeTileRowStart:13;
    UINT32 Reserved1:6;

    UINT32 AeTileWidth:9;
    UINT32 AeTileHeight:9;
    UINT32 Reserved2:14;

    UINT32 AeTileYShift:5;
    UINT32 Reserved3:27;
} __attribute__((packed)) dsp_pg_ae_stat_t;

typedef struct {
    UINT32 AfEnable:1;
    UINT32 AfTileNumCol:5;
    UINT32 AfTileNumRow:4;
    UINT32 Reserved0:22;

    UINT32 AfTileColStart:13;
    UINT32 AfTileRowStart:13;
    UINT32 Reserved1:6;

    UINT32 AfTileWidth:10;
    UINT32 AfTileHeight:10;
    UINT32 Reserved2:12;

    UINT32 AfTileActiveWidth:9;
    UINT32 AfTileActiveHeight:9;
    UINT32 Reserved3:14;
} __attribute__((packed)) dsp_af_stat_t;

typedef struct {
    UINT32 BayerPattern:2;
    UINT32 IrMode:2;
    UINT32 CropEn:1;
    UINT32 Reserved0:27;

    UINT32 CropColStart:13;
    UINT32 CropRowStart:13;
    UINT32 Reserved1:6;

    UINT32 CropWidth:13;
    UINT32 CropHeight:13;
    UINT32 Reserved2:6;
} __attribute__((packed)) dsp_vin_stat_exp_t;

typedef  struct {
    UINT8  StitchTileIdxX;
    UINT8  StitchTileIdxY;
    UINT16 Reserved0;
    dsp_vin_stat_exp_t VinHistogram;
    dsp_awb_stat_t Awb;
    dsp_cfa_ae_stat_t CfaAe;
    dsp_af_stat_t CfaAf;
    dsp_pg_ae_stat_t PgAe;
    dsp_af_stat_t PgAf;
    UINT32 CfaAeTileHistogramMask[16];
    UINT32 PgAeTileHistogramMask[16];

    UINT16 AwbRgbShift;
    UINT16 AwbMinMaxShift;
    UINT16 PgAeYShift;
    UINT16 AeLinearYShift;
    UINT16 AeMinMax_shift;
    UINT16 PgAfYShift;
    UINT16 AfCfaYShift;

    UINT8  ChanIndex;
    UINT8  VinStatsType;
    UINT32 RawPicSeqNum;
    UINT8  AaaCfaMuxSel;
    UINT8  Reserved1;
    UINT16 reserved2[5];
}__attribute__((packed)) dsp_3a_header_t;

static UINT32 aaa_process_statistic_header(const UINT8 *pSrcStitchNumX,
                                           const UINT8 *pSrcStitchNumY,
                                           const ULONG *SrcImgAaaStatAddr,
                                           AMBA_IK_3A_HEADER_s *pDstImgAaaHeader)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcHeader;

    for (i = 0U; i < *pSrcStitchNumY; i++) {
        for (j = 0U; j < *pSrcStitchNumX; j++) {
            SrcIdx = j + (*pSrcStitchNumX *i);
            dsp_osal_typecast(&pSrcHeader, &SrcImgAaaStatAddr[SrcIdx]);

            if ((i==0U) && (j==0U)) {
                (void)dsp_osal_memcpy(pDstImgAaaHeader, pSrcHeader, sizeof(dsp_3a_header_t));
                pDstImgAaaHeader->VinHistogram.BayerPattern = pSrcHeader->VinHistogram.BayerPattern;
                pDstImgAaaHeader->VinHistogram.IrMode = pSrcHeader->VinHistogram.IrMode;
                pDstImgAaaHeader->VinHistogram.CropEn = pSrcHeader->VinHistogram.CropEn;
                pDstImgAaaHeader->VinHistogram.Reserved0 = 0U;
                pDstImgAaaHeader->VinHistogram.CropWidth = pSrcHeader->VinHistogram.CropWidth + 1U;
                pDstImgAaaHeader->VinHistogram.CropHeight = pSrcHeader->VinHistogram.CropHeight + 1U;
                pDstImgAaaHeader->Awb.AwbEnable = (UINT8)pSrcHeader->Awb.AwbEnable;
                pDstImgAaaHeader->Awb.AwbTileNumCol = (UINT8)pSrcHeader->Awb.AwbTileNumCol + 1U;
                pDstImgAaaHeader->Awb.AwbTileNumRow = (UINT8)pSrcHeader->Awb.AwbTileNumRow + 1U;
                pDstImgAaaHeader->Awb.Reserved = 0U;
                pDstImgAaaHeader->Awb.AwbTileColStart = (UINT8)pSrcHeader->Awb.AwbTileColStart;
                pDstImgAaaHeader->Awb.AwbTileRowStart = (UINT8)pSrcHeader->Awb.AwbTileRowStart;
                pDstImgAaaHeader->Awb.AwbTileWidth = (UINT16)pSrcHeader->Awb.AwbTileWidth + 1U;
                pDstImgAaaHeader->Awb.AwbTileHeight = (UINT16)pSrcHeader->Awb.AwbTileHeight + 1U;
                pDstImgAaaHeader->Awb.AwbTileActiveWidth = (UINT16)pSrcHeader->Awb.AwbTileActiveWidth + 1U;
                pDstImgAaaHeader->Awb.AwbTileActiveHeight = (UINT16)pSrcHeader->Awb.AwbTileActiveHeight + 1U;
                pDstImgAaaHeader->CfaAe.AeEnable = (UINT8)pSrcHeader->CfaAe.AeEnable;
                pDstImgAaaHeader->CfaAe.AeTileNumCol = (UINT8)pSrcHeader->CfaAe.AeTileNumCol + 1U;
                pDstImgAaaHeader->CfaAe.AeTileNumRow = (UINT8)pSrcHeader->CfaAe.AeTileNumRow + 1U;
                pDstImgAaaHeader->CfaAe.Reserved = 0U;
                pDstImgAaaHeader->CfaAe.AeTileColStart = (UINT16)pSrcHeader->CfaAe.AeTileColStart;
                pDstImgAaaHeader->CfaAe.AeTileRowStart = (UINT16)pSrcHeader->CfaAe.AeTileRowStart;
                pDstImgAaaHeader->CfaAe.AeTileWidth = (UINT16)pSrcHeader->CfaAe.AeTileWidth + 1U;
                pDstImgAaaHeader->CfaAe.AeTileHeight = (UINT16)pSrcHeader->CfaAe.AeTileHeight + 1U;
                pDstImgAaaHeader->CfaAe.AeMinThresh = (UINT16)pSrcHeader->CfaAe.AeMinThresh;
                pDstImgAaaHeader->CfaAe.AeMaxThresh = (UINT16)pSrcHeader->CfaAe.AeMaxThresh;
                pDstImgAaaHeader->PgAe.AeEnable = (UINT8)pSrcHeader->PgAe.AeEnable;
                pDstImgAaaHeader->PgAe.AeTileNumCol = (UINT8)pSrcHeader->PgAe.AeTileNumCol + 1U;
                pDstImgAaaHeader->PgAe.AeTileNumRow = (UINT8)pSrcHeader->PgAe.AeTileNumRow + 1U;
                pDstImgAaaHeader->PgAe.AeTileYShift = (UINT8)pSrcHeader->PgAe.AeTileYShift;
                pDstImgAaaHeader->PgAe.AeTileColStart = (UINT8)pSrcHeader->PgAe.AeTileColStart;
                pDstImgAaaHeader->PgAe.AeTileRowStart = (UINT8)pSrcHeader->PgAe.AeTileRowStart;
                pDstImgAaaHeader->PgAe.AeTileWidth = (UINT16)pSrcHeader->PgAe.AeTileWidth + 1U;
                pDstImgAaaHeader->PgAe.AeTileHeight = (UINT16)pSrcHeader->PgAe.AeTileHeight + 1U;
                pDstImgAaaHeader->PgAe.Reserved = 0U;
                pDstImgAaaHeader->CfaAf.AfEnable = (UINT8)pSrcHeader->CfaAf.AfEnable;
                pDstImgAaaHeader->CfaAf.AfTileNumCol = (UINT8)pSrcHeader->CfaAf.AfTileNumCol + 1U;
                pDstImgAaaHeader->CfaAf.AfTileNumRow = (UINT8)pSrcHeader->CfaAf.AfTileNumRow + 1U;
                pDstImgAaaHeader->CfaAf.Reserved = 0U;
                pDstImgAaaHeader->CfaAf.AfTileColStart = (UINT8)pSrcHeader->CfaAf.AfTileColStart;
                pDstImgAaaHeader->CfaAf.AfTileRowStart = (UINT8)pSrcHeader->CfaAf.AfTileRowStart;
                pDstImgAaaHeader->CfaAf.AfTileWidth = (UINT16)pSrcHeader->CfaAf.AfTileWidth + 1U;
                pDstImgAaaHeader->CfaAf.AfTileHeight = (UINT16)pSrcHeader->CfaAf.AfTileHeight + 1U;
                pDstImgAaaHeader->CfaAf.AfTileActiveWidth = (UINT16)pSrcHeader->CfaAf.AfTileActiveWidth + 1U;
                pDstImgAaaHeader->CfaAf.AfTileActiveHeight = (UINT16)pSrcHeader->CfaAf.AfTileActiveHeight + 1U;
                pDstImgAaaHeader->PgAf.AfEnable = (UINT8)pSrcHeader->PgAf.AfEnable;
                pDstImgAaaHeader->PgAf.AfTileNumCol = (UINT8)pSrcHeader->PgAf.AfTileNumCol + 1U;
                pDstImgAaaHeader->PgAf.AfTileNumRow = (UINT8)pSrcHeader->PgAf.AfTileNumRow + 1U;
                pDstImgAaaHeader->PgAf.Reserved = 0U;
                pDstImgAaaHeader->PgAf.AfTileColStart = (UINT8)pSrcHeader->PgAf.AfTileColStart;
                pDstImgAaaHeader->PgAf.AfTileRowStart = (UINT8)pSrcHeader->PgAf.AfTileRowStart;
                pDstImgAaaHeader->PgAf.AfTileWidth = (UINT16)pSrcHeader->PgAf.AfTileWidth + 1U;
                pDstImgAaaHeader->PgAf.AfTileHeight = (UINT16)pSrcHeader->PgAf.AfTileHeight + 1U;
                pDstImgAaaHeader->PgAf.AfTileActiveWidth = (UINT16)pSrcHeader->PgAf.AfTileActiveWidth + 1U;
                pDstImgAaaHeader->PgAf.AfTileActiveHeight = (UINT16)pSrcHeader->PgAf.AfTileActiveHeight + 1U;
            } else {
                if (i == 0U) {
                    pDstImgAaaHeader->VinHistogram.CropWidth += pSrcHeader->VinHistogram.CropWidth + 1U;
                    pDstImgAaaHeader->Awb.AwbTileNumCol += pSrcHeader->Awb.AwbTileNumCol + 1U;
                    pDstImgAaaHeader->CfaAe.AeTileNumCol += pSrcHeader->CfaAe.AeTileNumCol + 1U;
                    pDstImgAaaHeader->PgAe.AeTileNumCol += pSrcHeader->PgAe.AeTileNumCol + 1U;
                    pDstImgAaaHeader->CfaAf.AfTileNumCol += pSrcHeader->CfaAf.AfTileNumCol + 1U;
                    pDstImgAaaHeader->PgAf.AfTileNumCol += pSrcHeader->PgAf.AfTileNumCol + 1U;
                }
                if (j == 0U) {
                    pDstImgAaaHeader->VinHistogram.CropHeight += pSrcHeader->VinHistogram.CropHeight + 1U;
                    pDstImgAaaHeader->Awb.AwbTileNumRow += pSrcHeader->Awb.AwbTileNumRow + 1U;
                    pDstImgAaaHeader->CfaAe.AeTileNumRow += pSrcHeader->CfaAe.AeTileNumRow + 1U;
                    pDstImgAaaHeader->PgAe.AeTileNumRow += pSrcHeader->PgAe.AeTileNumRow + 1U;
                    pDstImgAaaHeader->CfaAf.AfTileNumRow += pSrcHeader->CfaAf.AfTileNumRow + 1U;
                    pDstImgAaaHeader->PgAf.AfTileNumRow += pSrcHeader->PgAf.AfTileNumRow + 1U;
                }
            }
        }
    }

    return Rval;
}

static void aaa_process_cfa_awb_statistic(UINT32 CurIdxX,
                                          UINT32 CurIdxY,
                                          const UINT8 *pSrcStitchNumX,
                                          const ULONG *SrcImgCfaAaaStatAddr,
                                          AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcCfaHeader;
    const AMBA_IK_CFA_AWB_s *pSrcAwb;
    AMBA_IK_CFA_AWB_s *pDstAwb;
    UINT32 offset = 0;
    UINT32 m, n;
    UINT8 k;
    ULONG ULAddr;

    //===== pointer init =====//
    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcCfaHeader, &SrcImgCfaAaaStatAddr[m]);
        offset += ((UINT32)pSrcCfaHeader->Awb.AwbTileNumCol + 1U);
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcCfaHeader, &SrcImgCfaAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgCfaAaaStat->Header.Awb.AwbTileNumCol*((UINT32)pSrcCfaHeader->Awb.AwbTileNumRow + 1U));
    }

    pDstAwb = &pDstImgCfaAaaStat->Awb[0];
    dsp_osal_typecast(&ULAddr, &pDstAwb);
    ULAddr += offset * sizeof(AMBA_IK_CFA_AWB_s);
    dsp_osal_typecast(&pDstAwb, &ULAddr);

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcImgCfaAaaStat);

    dsp_osal_typecast(&ULAddr, &pSrcImgCfaAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    dsp_osal_typecast(&pSrcAwb, &ULAddr);

    for (k = 0U; k < (pSrcCfaHeader->Awb.AwbTileNumRow + 1U); k++) {
        (void)dsp_osal_memcpy(pDstAwb, pSrcAwb, sizeof(AMBA_IK_CFA_AWB_s)*(pSrcCfaHeader->Awb.AwbTileNumCol + 1U));

        dsp_osal_typecast(&ULAddr, &pSrcAwb);
        ULAddr = ULAddr + ((pSrcCfaHeader->Awb.AwbTileNumCol + 1U)*sizeof(AMBA_IK_CFA_AWB_s));
        dsp_osal_typecast(&pSrcAwb, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAwb);
        ULAddr = ULAddr + (pDstImgCfaAaaStat->Header.Awb.AwbTileNumCol*sizeof(AMBA_IK_CFA_AWB_s));
        dsp_osal_typecast(&pDstAwb, &ULAddr);
    }

}

static void aaa_process_cfa_ae_statistic(UINT32 CurIdxX,
                                         UINT32 CurIdxY,
                                         const UINT8 *pSrcStitchNumX,
                                         const ULONG *SrcImgCfaAaaStatAddr,
                                         AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcCfaHeader;
    const AMBA_IK_CFA_AE_s *pSrcAe;
    AMBA_IK_CFA_AE_s *pDstAe;
    UINT32 offset = 0;
    UINT32 m, n;
    UINT8 k;
    ULONG ULAddr;

    //===== pointer init =====//
    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcCfaHeader, &SrcImgCfaAaaStatAddr[m]);
        offset += ((UINT32)pSrcCfaHeader->CfaAe.AeTileNumCol + 1U);
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcCfaHeader, &SrcImgCfaAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgCfaAaaStat->Header.CfaAe.AeTileNumCol*((UINT32)pSrcCfaHeader->CfaAe.AeTileNumRow + 1U));
    }
    pDstAe = &pDstImgCfaAaaStat->Ae[0];
    dsp_osal_typecast(&ULAddr, &pDstAe);
    ULAddr = ULAddr + (offset * sizeof(AMBA_IK_CFA_AE_s));
    dsp_osal_typecast(&pDstAe, &ULAddr);

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcImgCfaAaaStat);

    dsp_osal_typecast(&ULAddr, &pSrcImgCfaAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16); //FrmId
    ULAddr += (sizeof(AMBA_IK_CFA_AWB_s)*(pSrcCfaHeader->Awb.AwbTileNumRow + 1U))*(pSrcCfaHeader->Awb.AwbTileNumCol + 1U);
    dsp_osal_typecast(&pSrcAe, &ULAddr);

    for (k = 0U; k < (pSrcCfaHeader->CfaAe.AeTileNumRow + 1U); k++) {
        (void)dsp_osal_memcpy(pDstAe, pSrcAe, sizeof(AMBA_IK_CFA_AE_s)*(pSrcCfaHeader->CfaAe.AeTileNumCol + 1U));

        dsp_osal_typecast(&ULAddr, &pSrcAe);
        ULAddr += (pSrcCfaHeader->CfaAe.AeTileNumCol + 1U) * sizeof(AMBA_IK_CFA_AE_s);
        dsp_osal_typecast(&pSrcAe, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAe);
        ULAddr += pDstImgCfaAaaStat->Header.CfaAe.AeTileNumCol*sizeof(AMBA_IK_CFA_AE_s);
        dsp_osal_typecast(&pDstAe, &ULAddr);
    }
}

static void aaa_process_cfa_af_statistic(UINT32 CurIdxX,
                                         UINT32 CurIdxY,
                                         const UINT8 *pSrcStitchNumX,
                                         const ULONG *SrcImgCfaAaaStatAddr,
                                         AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcCfaHeader;
    const AMBA_IK_CFA_AF_s *pSrcAf;
    AMBA_IK_CFA_AF_s *pDstAf;
    UINT32 offset = 0;
    UINT32 m, n;
    UINT8 k;
    ULONG ULAddr;

    //===== pointer init =====//
    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcCfaHeader, &SrcImgCfaAaaStatAddr[m]);
        offset += ((UINT32)pSrcCfaHeader->CfaAf.AfTileNumCol + 1U);
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcCfaHeader, &SrcImgCfaAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgCfaAaaStat->Header.CfaAf.AfTileNumCol*((UINT32)pSrcCfaHeader->CfaAf.AfTileNumRow + 1U));
    }

    pDstAf = &pDstImgCfaAaaStat->Af[0];
    dsp_osal_typecast(&ULAddr, &pDstAf);
    ULAddr += offset * sizeof(AMBA_IK_CFA_AF_s);
    dsp_osal_typecast(&pDstAf, &ULAddr);

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcImgCfaAaaStat);

    dsp_osal_typecast(&ULAddr, &pSrcImgCfaAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_CFA_AWB_s)*(pSrcCfaHeader->Awb.AwbTileNumRow + 1U))*(pSrcCfaHeader->Awb.AwbTileNumCol + 1U);
    ULAddr += (sizeof(AMBA_IK_CFA_AE_s)*(pSrcCfaHeader->CfaAe.AeTileNumRow + 1U))*(pSrcCfaHeader->CfaAe.AeTileNumCol + 1U);
    dsp_osal_typecast(&pSrcAf, &ULAddr);

    for (k = 0U; k < (pSrcCfaHeader->CfaAf.AfTileNumRow + 1U); k++) {
        (void)dsp_osal_memcpy(pDstAf, pSrcAf, sizeof(AMBA_IK_CFA_AF_s)*(pSrcCfaHeader->CfaAf.AfTileNumCol + 1U));
        dsp_osal_typecast(&ULAddr, &pSrcAf);
        ULAddr += (pSrcCfaHeader->CfaAf.AfTileNumCol + 1U)*sizeof(AMBA_IK_CFA_AF_s);
        dsp_osal_typecast(&pSrcAf, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAf);
        ULAddr += pDstImgCfaAaaStat->Header.CfaAf.AfTileNumCol*sizeof(AMBA_IK_CFA_AF_s);
        dsp_osal_typecast(&pDstAf, &ULAddr);
    }
}

static void aaa_process_cfa_histo_statistic(UINT32 CurIdxX,
                                            UINT32 CurIdxY,
                                            const UINT8 *pSrcStitchNumX,
                                            const ULONG *SrcImgCfaAaaStatAddr,
                                            AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    const AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcCfaHeader;
    const AMBA_IK_CFA_HISTO_s *pSrcHisto;
    AMBA_IK_CFA_HISTO_s *pDstHisto;
    ULONG ULAddr;
    UINT32 k;

    //===== pointer init =====//
    pDstHisto = &pDstImgCfaAaaStat->Histogram;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[SrcIdx]);
    dsp_osal_typecast(&pSrcCfaHeader, &pSrcImgCfaAaaStat);

    dsp_osal_typecast(&ULAddr, &pSrcImgCfaAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_CFA_AWB_s)*(pSrcCfaHeader->Awb.AwbTileNumRow + 1U))*(pSrcCfaHeader->Awb.AwbTileNumCol + 1U);
    ULAddr += (sizeof(AMBA_IK_CFA_AE_s)*(pSrcCfaHeader->CfaAe.AeTileNumRow + 1U))*(pSrcCfaHeader->CfaAe.AeTileNumCol + 1U);
    ULAddr += (sizeof(AMBA_IK_CFA_AF_s)*(pSrcCfaHeader->CfaAf.AfTileNumRow + 1U))*(pSrcCfaHeader->CfaAf.AfTileNumCol + 1U);
    dsp_osal_typecast(&pSrcHisto, &ULAddr);

    if ((CurIdxY == 0U) && (CurIdxX == 0U)) {
        (void)dsp_osal_memcpy(pDstHisto, pSrcHisto, sizeof(AMBA_IK_CFA_HISTO_s));
    } else {
        for (k = 0U; k< 64U; k++) {
            pDstHisto->HisBinR[k] += pSrcHisto->HisBinR[k];
            pDstHisto->HisBinG[k] += pSrcHisto->HisBinG[k];
            pDstHisto->HisBinB[k] += pSrcHisto->HisBinB[k];
            pDstHisto->HisBinY[k] += pSrcHisto->HisBinY[k];
        }
    }
}

static UINT32 aaa_process_cfa_statistic(const UINT8 *pSrcStitchNumX,
                                        const UINT8 *pSrcStitchNumY,
                                        const ULONG *SrcImgCfaAaaStatAddr,
                                        AMBA_IK_CFA_3A_DATA_s *pDstImgCfaAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;
    AMBA_IK_3A_HEADER_s *pDstHistHeader;
    AMBA_IK_CFA_3A_DATA_s *pSrcImgCfaAaaStat;

    dsp_osal_typecast(&pDstHistHeader, &pDstImgCfaAaaStat);
    Rval = aaa_process_statistic_header(pSrcStitchNumX, pSrcStitchNumY, SrcImgCfaAaaStatAddr, pDstHistHeader);
    if (Rval == DSP_ERR_NONE) {
        dsp_osal_typecast(&pSrcImgCfaAaaStat, &SrcImgCfaAaaStatAddr[0U]);
        pDstImgCfaAaaStat->FrameId = pSrcImgCfaAaaStat->FrameId;
        for (i = 0U; i < *pSrcStitchNumY; i++) {
            for (j = 0U; j < *pSrcStitchNumX; j++) {
                aaa_process_cfa_awb_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
                aaa_process_cfa_ae_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
                aaa_process_cfa_af_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
                aaa_process_cfa_histo_statistic(j, i, pSrcStitchNumX, SrcImgCfaAaaStatAddr, pDstImgCfaAaaStat);
            }
        }
    }

    return Rval;
}

static void aaa_process_pg_af_statistic(UINT32 CurIdxX,
                                        UINT32 CurIdxY,
                                        const UINT8 *pSrcStitchNumX,
                                        const ULONG *SrcImgPgAaaStatAddr,
                                        AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    const AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcPgHeader;
    const AMBA_IK_PG_AF_s *pSrcAf;
    AMBA_IK_PG_AF_s *pDstAf;
    UINT32 offset = 0;
    UINT32 m, n;
    UINT8 k;
    ULONG ULAddr;

    //===== pointer init =====//
    for (m = 0U; m < CurIdxX; m++) {
        dsp_osal_typecast(&pSrcPgHeader, &SrcImgPgAaaStatAddr[m]);
        offset += ((UINT32)pSrcPgHeader->PgAf.AfTileNumCol + 1U);
    }

    for (n = 0U; n < CurIdxY; n++) {
        dsp_osal_typecast(&pSrcPgHeader, &SrcImgPgAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgPgAaaStat->Header.PgAf.AfTileNumCol*((UINT32)pSrcPgHeader->PgAf.AfTileNumRow + 1U));
    }

    pDstAf = &pDstImgPgAaaStat->Af[0];
    dsp_osal_typecast(&ULAddr, &pDstAf);
    ULAddr += offset * sizeof(AMBA_IK_PG_AF_s);
    dsp_osal_typecast(&pDstAf, &ULAddr);

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[SrcIdx]);
    dsp_osal_typecast(&pSrcPgHeader, &pSrcImgPgAaaStat);

    dsp_osal_typecast(&ULAddr, &pSrcImgPgAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    dsp_osal_typecast(&pSrcAf, &ULAddr);

    for (k = 0U; k < (pSrcPgHeader->PgAf.AfTileNumRow + 1U); k++) {
        (void)dsp_osal_memcpy( pDstAf, pSrcAf, sizeof(AMBA_IK_PG_AF_s)*(pSrcPgHeader->PgAf.AfTileNumCol + 1U));
        dsp_osal_typecast(&ULAddr, &pSrcAf);
        ULAddr += (pSrcPgHeader->PgAf.AfTileNumCol + 1U)*sizeof(AMBA_IK_PG_AF_s);
        dsp_osal_typecast(&pSrcAf, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAf);
        ULAddr += pDstImgPgAaaStat->Header.PgAf.AfTileNumCol*sizeof(AMBA_IK_PG_AF_s);
        dsp_osal_typecast(&pDstAf, &ULAddr);
    }
}

static void aaa_process_pg_ae_statistic(UINT32 CurIdxX,
                                        UINT32 CurIdxY,
                                        const UINT8 *pSrcStitchNumX,
                                        const ULONG *SrcImgPgAaaStatAddr,
                                        AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    const AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcPgHeader;
    const AMBA_IK_PG_AE_s *pSrcAe;
    AMBA_IK_PG_AE_s *pDstAe;
    UINT32 offset = 0;
    UINT32 m, n;
    UINT8 k;
    ULONG ULAddr;

    //===== pointer init =====//
    for(m = 0; m < CurIdxX; m++){
        dsp_osal_typecast(&pSrcPgHeader, &SrcImgPgAaaStatAddr[m]);
        offset += ((UINT32)pSrcPgHeader->PgAe.AeTileNumCol + 1U);
    }

    for(n = 0; n < CurIdxY; n++){
        dsp_osal_typecast(&pSrcPgHeader, &SrcImgPgAaaStatAddr[*pSrcStitchNumX*n]);
        offset += ((UINT32)pDstImgPgAaaStat->Header.PgAe.AeTileNumCol*((UINT32)pSrcPgHeader->PgAe.AeTileNumRow + 1U));
    }

    pDstAe = &pDstImgPgAaaStat->Ae[0];
    dsp_osal_typecast(&ULAddr, &pDstAe);
    ULAddr += offset * sizeof(AMBA_IK_PG_AE_s);
    dsp_osal_typecast(&pDstAe, &ULAddr);

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[SrcIdx]);
    dsp_osal_typecast(&pSrcPgHeader, &pSrcImgPgAaaStat);

    dsp_osal_typecast(&ULAddr, &pSrcImgPgAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_PG_AF_s)*(pSrcPgHeader->PgAf.AfTileNumRow + 1U))*(pSrcPgHeader->PgAf.AfTileNumCol + 1U);
    dsp_osal_typecast(&pSrcAe, &ULAddr);

    for (k = 0U; k < (pSrcPgHeader->PgAe.AeTileNumRow + 1U); k++) {
        (void)dsp_osal_memcpy(pDstAe, pSrcAe, sizeof(AMBA_IK_PG_AE_s)*(pSrcPgHeader->PgAe.AeTileNumCol + 1U));
        dsp_osal_typecast(&ULAddr, &pSrcAe);
        ULAddr = ULAddr + ((pSrcPgHeader->PgAe.AeTileNumCol + 1U)*sizeof(AMBA_IK_PG_AE_s));
        dsp_osal_typecast(&pSrcAe, &ULAddr);

        dsp_osal_typecast(&ULAddr, &pDstAe);
        ULAddr = ULAddr + (pDstImgPgAaaStat->Header.CfaAe.AeTileNumCol*sizeof(AMBA_IK_PG_AE_s));
        dsp_osal_typecast(&pDstAe, &ULAddr);
    }
}

static void aaa_process_pg_histo_statistic(UINT32 CurIdxX,
                                           UINT32 CurIdxY,
                                           const UINT8 *pSrcStitchNumX,
                                           const ULONG *SrcImgPgAaaStatAddr,
                                           AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    const AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;
    UINT32 SrcIdx;
    const dsp_3a_header_t *pSrcPgHeader;
    const AMBA_IK_PG_HISTO_s *pSrcHisto;
    AMBA_IK_PG_HISTO_s *pDstHisto;
    ULONG ULAddr;
    UINT32 k;

    //===== pointer init =====//
    pDstHisto = &pDstImgPgAaaStat->Histogram;

    SrcIdx = CurIdxX + (*pSrcStitchNumX*CurIdxY);
    dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[SrcIdx]);
    dsp_osal_typecast(&pSrcPgHeader, &pSrcImgPgAaaStat);

    dsp_osal_typecast(&ULAddr, &pSrcImgPgAaaStat);
    ULAddr += sizeof(AMBA_IK_3A_HEADER_s);
    ULAddr += sizeof(UINT16);
    ULAddr += (sizeof(AMBA_IK_PG_AF_s)*(pSrcPgHeader->PgAf.AfTileNumRow + 1U))*(pSrcPgHeader->PgAf.AfTileNumCol + 1U);
    ULAddr += (sizeof(AMBA_IK_PG_AE_s)*(pSrcPgHeader->PgAe.AeTileNumRow + 1U))*(pSrcPgHeader->PgAe.AeTileNumCol + 1U);
    dsp_osal_typecast(&pSrcHisto, &ULAddr);

    if ((CurIdxY == 0U) && (CurIdxX == 0U)) {
        (void)dsp_osal_memcpy(pDstHisto, pSrcHisto, sizeof(AMBA_IK_PG_HISTO_s));
    } else {
        for (k = 0U; k < 64U; k++) {
            pDstHisto->HisBinR[k] += pSrcHisto->HisBinR[k];
            pDstHisto->HisBinG[k] += pSrcHisto->HisBinG[k];
            pDstHisto->HisBinB[k] += pSrcHisto->HisBinB[k];
            pDstHisto->HisBinY[k] += pSrcHisto->HisBinY[k];
        }
    }
}

static UINT32 aaa_process_pg_statistic(const UINT8 *pSrcStitchNumX,
                                       const UINT8 *pSrcStitchNumY,
                                       const ULONG *SrcImgPgAaaStatAddr,
                                       AMBA_IK_PG_3A_DATA_s *pDstImgPgAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j;
    AMBA_IK_3A_HEADER_s *pDstHistHeader;
    AMBA_IK_PG_3A_DATA_s *pSrcImgPgAaaStat;

    dsp_osal_typecast(&pDstHistHeader, &pDstImgPgAaaStat);
    Rval = aaa_process_statistic_header(pSrcStitchNumX, pSrcStitchNumY, SrcImgPgAaaStatAddr, pDstHistHeader);
    if (Rval == DSP_ERR_NONE) {
        dsp_osal_typecast(&pSrcImgPgAaaStat, &SrcImgPgAaaStatAddr[0U]);
        pDstImgPgAaaStat->FrameId = pSrcImgPgAaaStat->FrameId;
        for (i = 0U; i < *pSrcStitchNumY; i++) {
            for (j = 0U; j < *pSrcStitchNumX; j++) {
                aaa_process_pg_af_statistic(j, i, pSrcStitchNumX, SrcImgPgAaaStatAddr, pDstImgPgAaaStat);
                aaa_process_pg_ae_statistic(j, i, pSrcStitchNumX, SrcImgPgAaaStatAddr, pDstImgPgAaaStat);
                aaa_process_pg_histo_statistic(j, i, pSrcStitchNumX, SrcImgPgAaaStatAddr, pDstImgPgAaaStat);
            }
        }
    }

    return Rval;
}

static UINT32 aaa_process_hist_statistic(const UINT8 *p_type,
                                         const UINT8 *pSrcStitchNumX,
                                         const UINT8 *pSrcStitchNumY,
                                         const ULONG *SrcImgHistAaaStatAddr,
                                         AMBA_IK_CFA_HIST_STAT_s *pDstImgHistAaaStat)
{
    UINT32 Rval = DSP_ERR_NONE;
    UINT32 i, j, k;
    AMBA_IK_3A_HEADER_s *pDstHistHeader;
    const AMBA_IK_CFA_HIST_STAT_s *pSrcImgHistAaaStat;
    UINT32 SrcIdx;

    dsp_osal_typecast(&pDstHistHeader, &pDstImgHistAaaStat);
    Rval = aaa_process_statistic_header(pSrcStitchNumX, pSrcStitchNumY, SrcImgHistAaaStatAddr, pDstHistHeader);
    if (Rval == DSP_ERR_NONE) {
        for (i = 0U; i < *pSrcStitchNumY; i++) {
            for (j = 0U; j < *pSrcStitchNumX; j++) {
                SrcIdx = j + (*pSrcStitchNumX *i);
                //pSrcImgHistAaaStat = (AMBA_IK_CFA_HIST_STAT_s  *)SrcImgHistAaaStatAddr[SrcIdx];
                dsp_osal_typecast(&pSrcImgHistAaaStat, &SrcImgHistAaaStatAddr[SrcIdx]);

                //===== histogram =====//
                {
                    const AMBA_IK_CFA_HISTOGRAM_s *pSrcHistogram;
                    AMBA_IK_CFA_HISTOGRAM_s *pDstHistogram;

                    pSrcHistogram = &pSrcImgHistAaaStat->Data;
                    pDstHistogram = &pDstImgHistAaaStat->Data;
                    if ((i == 0U) && (j == 0U)) {
                        (void)dsp_osal_memcpy(pDstHistogram, pSrcHistogram, sizeof(AMBA_IK_CFA_HISTOGRAM_s));
                    } else {
                        for (k = 0U; k < AMBA_IK_HDR_HISTO_COUNT; k++) {
                            pDstHistogram->HistoBinR[k] += pSrcHistogram->HistoBinR[k];
                            pDstHistogram->HistoBinG[k] += pSrcHistogram->HistoBinG[k];
                            pDstHistogram->HistoBinB[k] += pSrcHistogram->HistoBinB[k];
                            if (*p_type==DSP_HIST_VIDEO_RGBIR) {
                                pDstHistogram->HistoBinIR[k] += pSrcHistogram->HistoBinIR[k];
                            } else {
                                pDstHistogram->HistoBinIR[k] = 0U;
                            }
                        }
                    }
                }
            }
        }
    }

    return Rval;
}
#endif

UINT32 TransferCFAAaaStatData(const void *pData)
{
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_DSP_EVENT_3A_TRANSFER_s *pAaaStatData;
    AMBA_IK_CFA_3A_DATA_s *pCfaAaaData = NULL;
    UINT32 SourceCheck = 0U;
    UINT32 i, j;

    (void)dsp_osal_typecast(&pAaaStatData, &pData);

    for (i = 0U; i < pAaaStatData->SrcSliceY; i++) {
        for (j = 0U; j < pAaaStatData->SrcSliceX; j++) {
            if (pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] == 0U) {
                dsp_osal_printU5("[AAATrans] SourceCheck, =====pAaaStatData->SrcAddr[%d] ==== 0x%X%X"
                    , j + (i * pAaaStatData->SrcSliceX)
                    , dsp_osal_get_u64msb((UINT64)pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)])
                    , dsp_osal_get_u64lsb((UINT64)pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]), 0U, 0U);
                SourceCheck |= 1U;
            }
            if (pAaaStatData->DstAddr == 0U) {
                dsp_osal_printU5("[AAATrans] SourceCheck, =====pAaaStatData->DstAddr ==== 0x%X%X"
                    , dsp_osal_get_u64msb((UINT64)pAaaStatData->DstAddr)
                    , dsp_osal_get_u64lsb((UINT64)pAaaStatData->DstAddr), 0U, 0U, 0U);
                SourceCheck |= 1U;
            }
        }
    }

    if (SourceCheck == 0U) {
        (void)dsp_osal_typecast(&pCfaAaaData, &pAaaStatData->DstAddr);
        Rval = aaa_process_cfa_statistic(&pAaaStatData->SrcSliceX, &pAaaStatData->SrcSliceY, pAaaStatData->SrcAddr, pCfaAaaData);
        //dsp_osal_printU5("############## DEST CFA 3a addr:%p, size:%d", pAaaStatData->DstAddr, sizeof(AMBA_IK_CFA_3A_DATA_s), 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 TransferPGAaaStatData(const void *pData)
{
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_DSP_EVENT_3A_TRANSFER_s *pAaaStatData;
    AMBA_IK_PG_3A_DATA_s *pPgAaaData;
    UINT32 i, j;
    UINT32 SourceCheck = 0U;

    (void)dsp_osal_typecast(&pAaaStatData, &pData);

    for (i = 0U; i < pAaaStatData->SrcSliceY; i++) {
        for (j = 0U; j < pAaaStatData->SrcSliceX; j++) {
            if (pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)] == 0U){
                dsp_osal_printU5("[PgTrans] SourceCheck, =====pAaaStatData->SrcAddr[%d] ==== 0x%X%X\n"
                    , j + (i * pAaaStatData->SrcSliceX)
                    , dsp_osal_get_u64msb((UINT64)pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)])
                    , dsp_osal_get_u64lsb((UINT64)pAaaStatData->SrcAddr[j + (i * pAaaStatData->SrcSliceX)]), 0U, 0U);
                SourceCheck |= 1U;
            }
            if (pAaaStatData->DstAddr == 0U) {
                dsp_osal_printU5("[PgTrans] SourceCheck, =====pAaaStatData->DstAddr ==== 0x%X%X\n"
                    , dsp_osal_get_u64msb((UINT64)pAaaStatData->DstAddr)
                    , dsp_osal_get_u64lsb((UINT64)pAaaStatData->DstAddr), 0U, 0U, 0U);
                SourceCheck |= 1U;
            }
        }
    }

    if (SourceCheck == 0U) {
        (void)dsp_osal_typecast(&pPgAaaData, &pAaaStatData->DstAddr);
        Rval = aaa_process_pg_statistic(&pAaaStatData->SrcSliceX, &pAaaStatData->SrcSliceY, pAaaStatData->SrcAddr, pPgAaaData);
        //dsp_osal_printU5("############## DEST PG 3a addr:%p, size:%d", pAaaStatData->DstAddr, sizeof(AMBA_IK_PG_3A_DATA_s), 0U, 0U, 0U);
    }

    return Rval;

}

UINT32 TransCFAHistStatData(const void *pData)
{
    UINT32 Rval = DSP_ERR_NONE;
    const AMBA_DSP_EVENT_3A_TRANSFER_s *pHistoStatData;
    AMBA_IK_CFA_HIST_STAT_s *pCfaHistogramStat;

    dsp_osal_typecast(&pHistoStatData, &pData);

    (void)dsp_osal_typecast(&pCfaHistogramStat, &pHistoStatData->DstAddr);
    Rval = aaa_process_hist_statistic(&pHistoStatData->Type, &pHistoStatData->SrcSliceX, &pHistoStatData->SrcSliceY, pHistoStatData->SrcAddr, pCfaHistogramStat);
    //dsp_osal_printU5("############## DEST CFA hist addr:%p, size:%d", pHistoStatData->DstAddr, sizeof(AMBA_IK_CFA_HIST_STAT_s), 0U, 0U, 0U);

    return Rval;
}

