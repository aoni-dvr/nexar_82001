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

#include "AmbaTypes.h"

#include "AmbaCSL_NAND.h"
#include "AmbaCSL_FIO.h"

#ifdef CONFIG_QNX
AMBA_FIO_REG_s *pAmbaFIO_Reg;
#endif

void AmbaCSL_FioFdmaClearMainStatus(void)
{
    AMBA_FDMA_MAIN_STATUS_REG_s d;

    if (0U != AmbaWrap_memset(&d, 0, sizeof(d))) { /* Misra C */ };

    pAmbaFIO_Reg->FdmaMainStatus = d;
}

/*
 *  @RoutineName:: AmbaCSL_FioClearIrqStatus
 *
 *  @Description:: Clear FIO DMA status
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_FioClearIrqStatus(void)
{
    pAmbaFIO_Reg->IntStatusRaw.FioIntRaw                 = 1U;
    pAmbaFIO_Reg->IntStatusRaw.ErrorDetectedIntRaw       = 1U;
    pAmbaFIO_Reg->IntStatusRaw.ErrorNotCorrectableIntRaw = 1U;
}

/*
 *  @RoutineName:: AmbaCSL_FioInterruptEnable
 *
 *  @Description:: Enable All FIO Interrupt
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_FioInterruptEnable(void)
{
    pAmbaFIO_Reg->IntEnable.IntFioEnable             = 1U;
    pAmbaFIO_Reg->IntEnable.IntErrorDetectedEnable   = 1U;
    pAmbaFIO_Reg->IntEnable.IntErrorNotCorrectEnable = 1U;
}

/*
 *  @RoutineName:: AmbaCSL_FioDmaFifoModeEnable
 *
 *  @Description:: Enable DMA Mode for FIO-DMA FIFO
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 */
void AmbaCSL_FioDmaFifoModeEnable(void)
{
    AmbaCSL_FioFifoRandomReadModeDisable();
}

void AmbaCSL_FioSetEccReportConfig(UINT32 BchBits)
{
    UINT8 Tmp;
    if (0U != AmbaWrap_memcpy(&Tmp, &BchBits, sizeof(Tmp))) { /* Misra C */ };
    pAmbaFIO_Reg->EccReportConfig.ErrorReportBitNumber = Tmp;
}

