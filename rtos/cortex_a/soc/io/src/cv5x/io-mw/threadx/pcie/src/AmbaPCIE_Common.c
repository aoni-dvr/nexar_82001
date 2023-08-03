/**
 *  @file AmbaPCIE_Common.c
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
 *  @details PCIe driver for both root and endpoint.
 */
#include "AmbaPCIE_Drv.h"
#include "AmbaPCIE.h"

/*-------------------------------------------------------------------------------*/
/*        Display info about the DMA configuration                               */
/*-------------------------------------------------------------------------------*/
static UINT32 Impl_GetUdmaInfo(const PCIE_PrivateData *pD )
{
    UINT8 dmaMajVer = 0;
    UINT8 dmaMinVer = 0;

    PCIE_UdmaConfiguration dmaConf;
    UINT32 ret_val;

    const PCIE_OBJ * obj = PCIE_GetInstance();

    /*--- get the DMA Core version --*/
    ret_val = obj->UDMA_GetVersion (pD, &dmaMajVer,&dmaMinVer);
    if (ret_val != PCIE_ERR_SUCCESS) {
        PCIE_UtilityPrint(">> get DMA version failed");
    } else {
        PCIE_UtilityPrint("DMA Info");
        PCIE_UtilityPrintUInt5(" DMA Version 0x%x.0x%x", dmaMajVer,dmaMinVer, 0, 0, 0);

        ret_val = obj->UDMA_GetConfiguration (pD,&  dmaConf);
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint(">> get DMA info failed");
        } else {
            PCIE_UtilityPrint("Capabilities:");
            PCIE_UtilityPrintUInt5("    channels %d", dmaConf.numChannels, 0, 0, 0, 0);
            PCIE_UtilityPrintUInt5("    partitions %d", dmaConf.numPartitions, 0, 0, 0, 0);
            PCIE_UtilityPrintUInt5("    partition size %d", dmaConf.partitionSize, 0, 0, 0, 0);
        }
    }

    return ret_val;
}

/*-------------------------------------------------------------------------------*/
/* Display Configuration Info                                                    */
/*-------------------------------------------------------------------------------*/
UINT32  Impl_GetConfigurationInfo(const PCIE_PrivateData *pD)
{

    UINT32 ret_val;
    const PCIE_OBJ * obj = PCIE_GetInstance();
    PCIE_CoreEpOrRp epOrRp;

    ret_val = obj->isCoreStrappedAsEpOrRp(pD, &epOrRp);
    if (ret_val != PCIE_ERR_SUCCESS) {
        PCIE_UtilityPrint(">> isCoreStrappedAsEpOrRp failed");

    } else {
        if (epOrRp==PCIE_CORE_EP_STRAPPED) {
            PCIE_UtilityPrint(" Core is strapped as EP");
        } else {
            PCIE_UtilityPrint(" Core is strapped as RC");
        }

        /* Display DMA configuration info */
        ret_val = Impl_GetUdmaInfo(pD);
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint(">> Get DMA info failed");
        }
    }
    return ret_val;
}

#define DESC_NUM 64U

static PCIE_xd_desc *bulkDescriptor = NULL; // allocate non cache memory for dma descriptor
static UINT32 dma_channel = 0;

UINT32 AmbaPCIE_DmaXfer(const PCIE_DMA_XFER_PARAM_s *params)
{
    UINT32 ret = PCIE_ERR_SUCCESS;
    UINT32 flag = Impl_ApiFlagGet();

    if ((flag & FLAG_PCIE_API_READY_MASK) == 0U) {
        ret = PCIE_ERR_FLOW;
    } else if (params == NULL) {
        ret = PCIE_ERR_ARG;
    } else {
        const PCIE_PrivateData *pd = PCIE_GetPrivateData();
        PCIE_OBJ * obj = PCIE_GetInstance();
        PCIE_xd_desc  *desc;
        PCIE_ListAttribute listAttributes = {.lower = 0U, .upper = 0U };
        const PCIE_ListAttribute* pListAttr = &listAttributes;
        UINT32 desc_num, seg_size;
        UINT32 i;

        if (bulkDescriptor == NULL) {
            UINT64 Addr;
            // allocate descriptor memory
            (void)AmbaPCIE_MemAlloc((UINT32)sizeof (PCIE_xd_desc) * DESC_NUM, &Addr);
            const void *ptr = PCIE_UtilityU64AddrToPtr(Addr);
            // store descriptor address
            bulkDescriptor = PCIE_UtilityVoidP2XdDescP(ptr);

            PCIE_UtilityPrintUInt5("bulk descriptor addr =  0x%x.%x",
                                   (UINT32)(PCIE_UtilityXdDescP2U64(bulkDescriptor) >> 32U),
                                   (UINT32)(PCIE_UtilityXdDescP2U64(bulkDescriptor) & 0xFFFFFFFFU), 0, 0, 0);
        }

        obj->DmaSysCb = params->Func;
        desc = bulkDescriptor;

        // determine the size for each descriptor
        if (params->Rw == PCIE_DMA_READ) {
            seg_size = 0x10; // Due to CV5 HW issue that we need to limit 16 bytes for inbound DMA
        } else {
            seg_size = (UINT32)1 << 24U;
        }

        // how many descriptor chain
        desc_num = params->Size / seg_size;
        if ((params->Size % seg_size) != 0U) {
            desc_num++;
        }

        // register callback

        // prepare descriptor
        if (desc_num > DESC_NUM) {
            ret = 0xFFFFFFFFU;
        } else {
            UINT64 AxiAddr = params->AxiAddr;
            UINT64 PciAddr = params->PciAddr;
            UINT64 Size = params->Size;

            for (i = 0; i < (desc_num - 1U); i++) {
                desc[i].sys_lo_addr = (UINT32)( AxiAddr & 0xFFFFFFFFU);
                desc[i].sys_hi_addr  = (UINT32)(((AxiAddr) >> 32U) & 0xFFFFFFFFU);
                desc[i].sys_attr    = 0;
                desc[i].ext_lo_addr   = (UINT32)( PciAddr & 0xFFFFFFFFU);
                desc[i].ext_hi_addr   = (UINT32)(((PciAddr) >> 32U) & 0xFFFFFFFFU);
                desc[i].ext_attr    = 0;

                desc[i].size_and_ctrl.size = seg_size & 0xffffffU;
                desc[i].size_and_ctrl.ctrl_bits.control_bits = SetUDMA_ControlBits(&(desc[i].size_and_ctrl.ctrl_bits.control_bits), PCIE_READ_WRITE, PCIE_FALSE, PCIE_TRUE);

                desc[i].status.sys_status = 0;
                desc[i].status.ext_status = 0;
                desc[i].status.chnl_status = 0;
                desc[i].status.reserved_0 = 0;
                desc[i].next         = (UINT32)(PCIE_UtilityXdDescP2U64(&desc[i+1U]) & 0xFFFFFFFFU);
                desc[i].next_hi_addr = (UINT32)((PCIE_UtilityXdDescP2U64(&desc[i+1U]) >> 32U) & 0xFFFFFFFFU);

                AxiAddr += seg_size;
                PciAddr += seg_size;
                Size -= seg_size;
            }

            // last descriptor
            desc[i].sys_lo_addr = (UINT32)( AxiAddr & 0xFFFFFFFFU);
            desc[i].sys_hi_addr  = (UINT32)(((AxiAddr) >> 32U) & 0xFFFFFFFFU);
            desc[i].sys_attr    = 0;
            desc[i].ext_lo_addr   = (UINT32)( PciAddr & 0xFFFFFFFFU);
            desc[i].ext_hi_addr   = (UINT32)(((PciAddr) >> 32U) & 0xFFFFFFFFU);
            desc[i].ext_attr    = 0;

            desc[i].size_and_ctrl.size = (UINT32)(Size & 0xffffffU);
            desc[i].size_and_ctrl.ctrl_bits.control_bits = SetUDMA_ControlBits(&(desc[i].size_and_ctrl.ctrl_bits.control_bits), PCIE_READ_WRITE, PCIE_TRUE, PCIE_FALSE);

            desc[i].status.sys_status = 0;
            desc[i].status.ext_status = 0;
            desc[i].status.chnl_status = 0;
            desc[i].status.reserved_0 = 0;
            desc[i].next         = 0;
            desc[i].next_hi_addr = 0;
        }

        if (params->Rw == PCIE_DMA_WRITE) {
            dma_channel = 3;
        } else {
            dma_channel = 0;
        }

        if (ret == PCIE_ERR_SUCCESS) {
            ret = obj->UDMA_ControlErrInterrupts(pd, dma_channel, PCIE_ENABLE_PARAM);
        }

        if (ret == PCIE_ERR_SUCCESS) {
            ret = obj->UDMA_ControlDoneInterrupts(pd, dma_channel, PCIE_ENABLE_PARAM);
        }

        if (ret == PCIE_ERR_SUCCESS) {
            if (params->Rw == PCIE_DMA_WRITE) {
                // DMA outbound transfer to external memory
                ret = obj->UDMA_DoTransfer(pd, dma_channel, PCIE_TRUE, PCIE_UtilityXdDescP2U32p(bulkDescriptor), pListAttr);
            } else {
                ret = obj->UDMA_DoTransfer(pd, dma_channel, PCIE_FALSE, PCIE_UtilityXdDescP2U32p(bulkDescriptor), pListAttr);
            }
        }
    }
    return ret;
}

UINT32  AmbaPCIE_WaitDmaXferDone(void)
{
    UINT32 ret;
    UINT32 flag = Impl_ApiFlagGet();

    if ((flag & FLAG_PCIE_API_READY_MASK) == 0U) {
        ret = PCIE_ERR_FLOW;
    } else {
        const PCIE_PrivateData *pd = PCIE_GetPrivateData();
        const PCIE_OBJ * obj = PCIE_GetInstance();
        UINT32 descNo = 1;
        UINT8 chnl_status;
        const PCIE_xd_desc  *xd_desc = bulkDescriptor;
        PCIE_Bool done = PCIE_FALSE;

        while (done == PCIE_FALSE) {
            (void)AmbaKAL_TaskSleep(100);
            ret = obj->UDMA_GetErrorInterrupt(pd,dma_channel,&done);
            if (ret != PCIE_ERR_SUCCESS) {
                PCIE_UtilityPrint("Failed to get error interrupt");
            }
            if (done == PCIE_TRUE) {
                PCIE_UtilityPrint("DMA transfer Error");
                break;
            }
            ret = obj->UDMA_GetDoneInterrupt(pd,dma_channel,&done);
            if (ret != PCIE_ERR_SUCCESS) {
                PCIE_UtilityPrint("Failed to get done interrupt");
            }
            if (done == PCIE_TRUE) {
                PCIE_UtilityPrint(" DMA transfer Done");
            }
        }

        while (descNo != 0U) {
            PCIE_UtilityPrintUInt5("For descriptor %d :",descNo, 0, 0, 0, 0);
            PCIE_UtilityPrintUInt5("    sys_status: 0x%x", PCIE_RegPtrRead8(&xd_desc->status.sys_status), 0, 0, 0, 0);
            PCIE_UtilityPrintUInt5("    ext_status: 0x%x", PCIE_RegPtrRead8(&xd_desc->status.ext_status), 0, 0, 0, 0);

            chnl_status = PCIE_RegPtrRead8(&xd_desc->status.chnl_status);

            switch (chnl_status) {
            case 0x01:
                PCIE_UtilityPrint("    chnl_status: Complete");
                break;
            case 0x02:
                PCIE_UtilityPrint("    chnl_status: Ext Early Completion");
                break;
            case 0x04:
                PCIE_UtilityPrint("    chnl_status: Sys Early Completion");
                break;
            case 0x08:
                PCIE_UtilityPrint("    chnl_status: Data Integrity Error");
                break;
            case 0x10:
                PCIE_UtilityPrint("    chnl_status: Descriptor Error");
                break;
            case 0x20:
                PCIE_UtilityPrint("    chnl_status: Buffer Overflow");
                break;
            case 0x40:
                PCIE_UtilityPrint("    chnl_status: Buffer Underflow");
                break;
            case 0x80:
                PCIE_UtilityPrint("    chnl_status: Buffer Not Empty");
                break;
            default:
                PCIE_UtilityPrintUInt5("    chnl_status: 0x%x", chnl_status,0,0,0,0);
                break;
            }

            if ((PCIE_RegPtrRead8(&xd_desc->size_and_ctrl.ctrl_bits.control_bits) & 0x20U) != 0U) {
                // Continue to next
                UINT64 next_addr = (UINT64)PCIE_RegPtrRead32(&xd_desc->next);
                UINT64 next_hi_addr = (UINT64)PCIE_RegPtrRead32(&xd_desc->next_hi_addr);
                const void *ptr = PCIE_UtilityU64AddrToPtr((next_hi_addr << 32U) | next_addr);
                xd_desc = PCIE_UtilityVoidP2XdDescP(ptr);
                descNo++;
            } else {
                descNo=0;
            }

        }
    }
    return ret;
}

