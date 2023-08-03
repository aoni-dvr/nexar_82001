/**
 *  @file AmbaPCIE_Drv_LmTiming.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details PCIe implementation of driver API functions (local management for timing).
 */
#include "AmbaPCIE_Drv.h"

#define PCIE_TIM_DEL_FUNC_ELEMENTS 14U
#define PCIE_TIMEOUT_PARAMS_ELEMENTS 12U

/****************************************************************************/
/****************************************************************************/
/* TIMING PARAMS INCLUDE TIMEOUTS, DELAYS, LATENCY SETTINGS AND SCALES      */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************
 * Access_Timing_Params
 *****************************************************************************/
static UINT32 AccessTimingParamsTimeout(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    if ((pD == NULL) || (pRdWrVal == NULL)) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_L0S_timeout_limit_reg));

        if (rdOrWr == PCIE_DO_READ) {
            *pRdWrVal = PCIE_RegFldRead(LM_L0S_TO_LT_MASK, LM_L0S_TO_LT_SHIFT, regVal);
        } else {
            /* PCIE_DO_WRITE
             * Check if the value to be written is in valid range
             */
            if ( ((*pRdWrVal) >> LM_L0S_TO_LT_WIDTH) != 0U) {
                result = PCIE_ERR_ARG;
            } else {
                regVal = PCIE_RegFldWrite(LM_L0S_TO_LT_MASK, LM_L0S_TO_LT_SHIFT, regVal, *pRdWrVal);
                PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_L0S_timeout_limit_reg), regVal); /* Write back to register */
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimingParamsTimeoutLimit0(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_compln_tmout_lim_0_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_COMPL_TO0_CTL_MASK, LM_COMPL_TO0_CTL_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE
         * Check if the value to be written is in valid range
         */
        if ( ((*pRdWrVal) >> LM_COMPL_TO0_CTL_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_COMPL_TO0_CTL_MASK, LM_COMPL_TO0_CTL_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_compln_tmout_lim_0_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimingParamsTimeoutLimit1(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_compln_tmout_lim_1_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_COMPL_TO1_CTL_MASK, LM_COMPL_TO1_CTL_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE
         * Check if the value to be written is in valid range
         */
        if ( ((*pRdWrVal) >> LM_COMPL_TO1_CTL_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_COMPL_TO1_CTL_MASK, LM_COMPL_TO1_CTL_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_compln_tmout_lim_1_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimingParamsRetryDelay(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_L1_st_reentry_delay_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_L1_REENTRY_DELAY_L1RD_MASK, LM_L1_REENTRY_DELAY_L1RD_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_L1_REENTRY_DELAY_L1RD_MASK, LM_L1_REENTRY_DELAY_L1RD_SHIFT, regVal, *pRdWrVal);
        PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_L1_st_reentry_delay_reg), regVal); /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimingParamsTimeoutDelay(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_aspm_L1_entry_tmout_delay_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_ASPM_L1_ENTRY_TO_L1T_MASK, LM_ASPM_L1_ENTRY_TO_L1T_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE
         * Check if the value to be written is in valid range
         */
        if ( ((*pRdWrVal) >> LM_ASPM_L1_ENTRY_TO_L1T_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_ASPM_L1_ENTRY_TO_L1T_MASK, LM_ASPM_L1_ENTRY_TO_L1T_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_aspm_L1_entry_tmout_delay_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimParamsTurnoffAckDelay(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pme_turnoff_ack_delay_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_PME_TURNOFF_ACK_PTOAD_MASK, LM_PME_TURNOFF_ACK_PTOAD_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE
         * Check if the value to be written is in valid range
         */
        if ( ((*pRdWrVal) >> LM_PME_TURNOFF_ACK_PTOAD_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_PME_TURNOFF_ACK_PTOAD_MASK, LM_PME_TURNOFF_ACK_PTOAD_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pme_turnoff_ack_delay_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimParamSideReplayTimeout(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_dll_tmr_config_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_DLL_TMR_CONFIG_TSRT_MASK, LM_DLL_TMR_CONFIG_TSRT_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE
         * Check if the value to be written is in valid range
         */
        if ( ((*pRdWrVal) >> LM_DLL_TMR_CONFIG_TSRT_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_DLL_TMR_CONFIG_TSRT_MASK, LM_DLL_TMR_CONFIG_TSRT_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_dll_tmr_config_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimParamNackReplayTimeout(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Read register from PCIe IP
     */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_dll_tmr_config_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_DLL_TMR_CONFIG_RSART_MASK, LM_DLL_TMR_CONFIG_RSART_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE
         * Check if the value to be written is in valid range
         */
        if ( ((*pRdWrVal) >> LM_DLL_TMR_CONFIG_RSART_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_DLL_TMR_CONFIG_RSART_MASK, LM_DLL_TMR_CONFIG_RSART_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_dll_tmr_config_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimParamsServiceToutDelay(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_RP_STRAPPED;

    result = PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp);
    if (epOrRp == PCIE_CORE_RP_STRAPPED) {
        /* Only valid if IP strapped as EP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pme_service_timeout_delay_reg));

        if (rdOrWr == PCIE_DO_READ) {
            *pRdWrVal = PCIE_RegFldRead(LM_PME_SVC_TO_PSTD_MASK, LM_PME_SVC_TO_PSTD_SHIFT, regVal);
        } else {
            /* PCIE_DO_WRITE
             * Check if the value to be written is in valid range
             */
            if ( ((*pRdWrVal) >> LM_PME_SVC_TO_PSTD_WIDTH) != 0U) {
                result = PCIE_ERR_ARG;
            }

            if (result == PCIE_ERR_SUCCESS) {
                regVal = PCIE_RegFldWrite(LM_PME_SVC_TO_PSTD_MASK, LM_PME_SVC_TO_PSTD_SHIFT, regVal, *pRdWrVal);
                PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pme_service_timeout_delay_reg), regVal); /* Write back to register */
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimingParamsLatencyValue(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_RP_STRAPPED;

    result = PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp);
    if (epOrRp == PCIE_CORE_RP_STRAPPED) {
        /* Only valid if IP strapped as EP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg));

        if (rdOrWr == PCIE_DO_READ) {
            *pRdWrVal = PCIE_RegFldRead(LM_LTR_SNOOP_LAT_NSLV_MASK, LM_LTR_SNOOP_LAT_NSLV_SHIFT, regVal);
        } else {
            /* PCIE_DO_WRITE
             * Check if the value to be written is in valid range
             */
            if ( ((*pRdWrVal) >> LM_LTR_SNOOP_LAT_NSLV_WIDTH) != 0U) {
                result = PCIE_ERR_ARG;
            } else {
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_NSLV_MASK, LM_LTR_SNOOP_LAT_NSLV_SHIFT, regVal, *pRdWrVal);
                PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg), regVal); /* Write back to register */
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimingParamsLatencyScale(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_RP_STRAPPED;

    result = PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp);
    if (epOrRp == PCIE_CORE_RP_STRAPPED) {
        /* Only valid if IP strapped as EP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg));

        if (rdOrWr == PCIE_DO_READ) {
            *pRdWrVal = PCIE_RegFldRead(LM_LTR_SNOOP_LAT_NSLS_MASK, LM_LTR_SNOOP_LAT_NSLS_SHIFT, regVal);
        } else {
            /* PCIE_DO_WRITE
             * Check if the value to be written is in valid range
             */
            if ( ((*pRdWrVal) >> LM_LTR_SNOOP_LAT_NSLS_WIDTH) != 0U) {
                result = PCIE_ERR_ARG;
            } else {
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_NSLS_MASK, LM_LTR_SNOOP_LAT_NSLS_SHIFT, regVal, *pRdWrVal);
                PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg), regVal); /* Write back to register */
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimParamsSnoopLatencyVal(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_RP_STRAPPED;

    result = PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp);
    if (epOrRp == PCIE_CORE_RP_STRAPPED) {
        /* Only valid if IP strapped as EP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg));

        if (rdOrWr == PCIE_DO_READ) {
            *pRdWrVal = PCIE_RegFldRead(LM_LTR_SNOOP_LAT_SLV_MASK, LM_LTR_SNOOP_LAT_SLV_SHIFT, regVal);
        } else {
            /* PCIE_DO_WRITE
             * Check if the value to be written is in valid range
             */
            if ( ((*pRdWrVal) >> LM_LTR_SNOOP_LAT_SLV_WIDTH) != 0U) {
                result = PCIE_ERR_ARG;
            } else {
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_SLV_MASK, LM_LTR_SNOOP_LAT_SLV_SHIFT, regVal, *pRdWrVal);
                PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg), regVal); /* Write back to register */
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessTimParamSnoopLatencyScale(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_RP_STRAPPED;

    result = PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp);
    if (epOrRp == PCIE_CORE_RP_STRAPPED) {
        /* Only valid if IP strapped as EP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg));

        if (rdOrWr == PCIE_DO_READ) {
            *pRdWrVal = PCIE_RegFldRead(LM_LTR_SNOOP_LAT_SLS_MASK, LM_LTR_SNOOP_LAT_SLS_SHIFT, regVal);
        } else {
            /* PCIE_DO_WRITE
             * Check if the value to be written is in valid range
             */
            if ( ((*pRdWrVal) >> LM_LTR_SNOOP_LAT_SLS_WIDTH) != 0U) {
                result = PCIE_ERR_ARG;
            } else {
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_SLS_MASK, LM_LTR_SNOOP_LAT_SLS_SHIFT, regVal, *pRdWrVal);
                PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg), regVal); /* Write back to register */
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static UINT32 AccessLinkDownTimer(
    const PCIE_PrivateData * pD,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal;
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Read register from PCIe IP */
    pcieAddr = pD->p_lm_base;
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ld_ctrl));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_LD_CTRL_LDTIMER_MASK, LM_LD_CTRL_LDTIMER_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE
         * Check if the value to be written is in valid range
         */
        if ( ((*pRdWrVal) >> LM_LD_CTRL_LDTIMER_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_LD_CTRL_LDTIMER_MASK, LM_LD_CTRL_LDTIMER_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ld_ctrl), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************
 * Access_Timing_Params
 *****************************************************************************/
static UINT32 AccessTimingParams(
    const PCIE_PrivateData *   pD,
    PCIE_TimeoutAndDelayValues timingVal,
    PCIE_ReadOrWrite           rdOrWr,
    UINT32 *                 pRdWrVal)
{
    UINT32 status = PCIE_ERR_SUCCESS;
    UINT32 result = PCIE_ERR_SUCCESS;

    static const TimeoutAndDelayFunctions timingParamsFunctions[PCIE_TIM_DEL_FUNC_ELEMENTS] = {
        [PCIE_L0S_TIMEOUT]                          = AccessTimingParamsTimeout,
        [PCIE_COMPLETION_TIMEOUT_LIMIT_0]           = AccessTimingParamsTimeoutLimit0,
        [PCIE_COMPLETION_TIMEOUT_LIMIT_1]           = AccessTimingParamsTimeoutLimit1,
        [PCIE_L1_STATE_RETRY_DELAY]                 = AccessTimingParamsRetryDelay,
        [PCIE_ASPM_L1_ENTRY_TIMEOUT_DELAY]          = AccessTimingParamsTimeoutDelay,
        [PCIE_PME_TURNOFF_ACK_DELAY]                = AccessTimParamsTurnoffAckDelay,
        [PCIE_TRANSMIT_SIDE_REPLAY_TIMEOUT]         = AccessTimParamSideReplayTimeout,
        [PCIE_RECEIVE_SIDE_ACK_NACK_REPLAY_TIMEOUT] = AccessTimParamNackReplayTimeout,
        [PCIE_PME_SERVICE_TIMEOUT_DELAY]            = AccessTimParamsServiceToutDelay,

        [PCIE_NO_SNOOP_LATENCY_VALUE]               = AccessTimingParamsLatencyValue,
        [PCIE_NO_SNOOP_LATENCY_SCALE]               = AccessTimingParamsLatencyScale,
        [PCIE_SNOOP_LATENCY_VALUE]                  = AccessTimParamsSnoopLatencyVal,
        [PCIE_SNOOP_LATENCY_SCALE]                  = AccessTimParamSnoopLatencyScale,

        [PCIE_LDTIMER_TIMEOUT       ]               = AccessLinkDownTimer,
    };

    UINT32 timingParamsIndex;
    timingParamsIndex = (UINT32)timingVal;

    if (timingParamsIndex >= (UINT32)PCIE_TIM_DEL_FUNC_ELEMENTS) {
        result = PCIE_ERR_ARG;
    } else {
        UINT32 * tmp = pRdWrVal;
        status = timingParamsFunctions[timingParamsIndex](pD, rdOrWr, tmp);
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }

    if (result != 0U) {
        status = result;
    }

    return (status);
}

UINT32 PCIE_SetTimingParams(
    const PCIE_PrivateData *   pD,
    PCIE_TimeoutAndDelayValues timingVal,
    UINT32                   timeDelay)
{
    UINT32 result;

    /*
     * Check input parameters are valid
     */
    if ( (pD == NULL) || ((UINT32)timingVal >= (UINT32)PCIE_TIM_DEL_FUNC_ELEMENTS) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = AccessTimingParams(pD, timingVal, PCIE_DO_WRITE, &timeDelay);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_GetTimingParams(
    const PCIE_PrivateData *   pD,
    PCIE_TimeoutAndDelayValues timingVal,
    UINT32 *                 timeDelay)
{
    UINT32 result;

    /*
     * Check input parameters are valid
     */
    if ( (pD == NULL) || (timeDelay == NULL) || ((UINT32)timingVal >= (UINT32)PCIE_TIM_DEL_FUNC_ELEMENTS) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = AccessTimingParams(pD, timingVal, PCIE_DO_READ, timeDelay);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Set_L0sTimeout                                          */
/****************************************************************************/
UINT32 PCIE_SetL0sTimeout(
    const PCIE_PrivateData *pD,
    UINT32           *timeout)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    }

    if (result == PCIE_ERR_SUCCESS) {
        result = AccessTimingParamsTimeout(pD, PCIE_DO_WRITE, timeout);
    }

    if (result == PCIE_ERR_SUCCESS) {
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* PCIE_Get_L0sTimeout                                          */
/****************************************************************************/
UINT32 PCIE_GetL0sTimeout(
    const PCIE_PrivateData * pD,
    UINT32 *               timeout)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (timeout == NULL) ) {
        result = PCIE_ERR_ARG;
    }

    if (result == PCIE_ERR_SUCCESS) {
        result = AccessTimingParamsTimeout(pD, PCIE_DO_READ, timeout);
    }

    if (result == PCIE_ERR_SUCCESS) {
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/****************************************************************************/
/* TRANSITION INTO L2 / L0S                                                 */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* PCIE_DisableRpTransitToL0s                                               */
/****************************************************************************/
UINT32 PCIE_DisableRpTransitToL0s(
    const PCIE_PrivateData * pD)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal =  PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_L0S_timeout_limit_reg));

        /* Write a 0 here to disable the transition into L0S permanently */
        regVal = PCIE_RegFldWrite(LM_L0S_TO_LT_MASK, LM_L0S_TO_LT_SHIFT, regVal, 0);

        PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_L0S_timeout_limit_reg), regVal); /* Write back to register */

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillTimingParamsChangeDefArray(
    PCIE_InitTimingRelatedParams TimingParams,
    UINT32                     TimingParamsArray[PCIE_TIMEOUT_PARAMS_ELEMENTS])
{
    /*
     * Fill array with timing parameter - changeDefault
     */
    TimingParamsArray[0] = TimingParams.txReplayTimeoutAdjustment.changeDefault;
    TimingParamsArray[1] = TimingParams.rxReplayTimeoutAdjustment.changeDefault;
    TimingParamsArray[2] = TimingParams.L0sTimeout.changeDefault;
    TimingParamsArray[3] = TimingParams.completionTimeoutLimit0.changeDefault;
    TimingParamsArray[4] = TimingParams.completionTimeoutLimit1.changeDefault;
    TimingParamsArray[5] = TimingParams.l1RetryDelay.changeDefault;
    TimingParamsArray[6] = TimingParams.l1Timeout.changeDefault;
    TimingParamsArray[7] = TimingParams.pmeTurnoffAckDelay.changeDefault;
    TimingParamsArray[8] = TimingParams.pmeServiceTimeoutDelay.changeDefault;

    TimingParamsArray[9] = TimingParams.linkDownResetTim.changeDefault;

    TimingParamsArray[10] = 0U;
    TimingParamsArray[11] = 0U;
    /* Returning after array duly filled */
    return;
}
/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillTimingParamsNewValueArray(
    PCIE_InitTimingRelatedParams TimingParams,
    UINT32                     TimingParamsArray[PCIE_TIMEOUT_PARAMS_ELEMENTS])
{
    /*
     * Fill array with timing parameter - newValue
     */
    TimingParamsArray[0] = TimingParams.txReplayTimeoutAdjustment.newValue;
    TimingParamsArray[1] = TimingParams.rxReplayTimeoutAdjustment.newValue;
    TimingParamsArray[2] = TimingParams.L0sTimeout.newValue;
    TimingParamsArray[3] = TimingParams.completionTimeoutLimit0.newValue;
    TimingParamsArray[4] = TimingParams.completionTimeoutLimit1.newValue;
    TimingParamsArray[5] = TimingParams.l1RetryDelay.newValue;
    TimingParamsArray[6] = TimingParams.l1Timeout.newValue;
    TimingParamsArray[7] = TimingParams.pmeTurnoffAckDelay.newValue;
    TimingParamsArray[8] = TimingParams.pmeServiceTimeoutDelay.newValue;

    TimingParamsArray[9] = TimingParams.linkDownResetTim.newValue;
    /* Returning after array duly filled */
    return;
}

UINT32 Init_TimingParams(const PCIE_PrivateData * pD,
                         const PCIE_InitParam *   pInitParam)
{

    UINT32 TimingParamsChangeDefaultArray[PCIE_TIMEOUT_PARAMS_ELEMENTS];
    UINT32 TimingParamsNewValueArray[PCIE_TIMEOUT_PARAMS_ELEMENTS];
    /* static const should not require memory allocation */
    static const PCIE_TimeoutAndDelayValues TimeoutAndDelayArray[PCIE_TIMEOUT_PARAMS_ELEMENTS] = {
        [0] = PCIE_TRANSMIT_SIDE_REPLAY_TIMEOUT,
        [1] = PCIE_RECEIVE_SIDE_ACK_NACK_REPLAY_TIMEOUT,

        [2] = PCIE_L0S_TIMEOUT,
        [3] = PCIE_COMPLETION_TIMEOUT_LIMIT_0,
        [4] = PCIE_COMPLETION_TIMEOUT_LIMIT_1,
        [5] = PCIE_L1_STATE_RETRY_DELAY,
        [6] = PCIE_ASPM_L1_ENTRY_TIMEOUT_DELAY,
        [7] = PCIE_PME_TURNOFF_ACK_DELAY,
        [8] = PCIE_PME_SERVICE_TIMEOUT_DELAY,

        [9] = PCIE_LDTIMER_TIMEOUT,
    };
    UINT32 retVal = 0U;
    UINT32 idx = 0U;

    /* Fill arrays with data*/
    FillTimingParamsChangeDefArray(pInitParam->initTimingParams, TimingParamsChangeDefaultArray);
    FillTimingParamsNewValueArray(pInitParam->initTimingParams, TimingParamsNewValueArray);

    while ( idx < PCIE_TIMEOUT_PARAMS_ELEMENTS ) {
        if ( TimingParamsChangeDefaultArray[idx] != 0U) {
            UINT32 *pval = &(TimingParamsNewValueArray[idx]);
            retVal = AccessTimingParams(
                         pD,
                         TimeoutAndDelayArray[idx],
                         PCIE_DO_WRITE,
                         pval);
        }

        if (retVal != 0U) {
            break;
        }
        idx = idx + 1U;
    }
    return (retVal);
}
