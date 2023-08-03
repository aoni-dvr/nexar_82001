/**
 *  @file AmbaPCIE_System.c
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
 *  @details PCIe system functions.
 */

#include "AmbaPCIE_Drv.h"
#include "AmbaPCIE.h"
#include "AmbaINT_Def.h"
#include "arm/AmbaRTSL_GIC.h"

UINT  _txe_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, UINT32 pool_size, UINT pool_control_block_size);
UINT  _txe_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr,
                         UINT32 memory_size,  UINT32 wait_option);
static TX_BYTE_POOL malloc_pool_ = {0};

static UINT32 pcie_mem_init(const PCIE_MEM_CONFIG_s *config)
{
    static CHAR PcieMemPoolStr[] = "PCIE Pool";
    UINT ret = _txe_byte_pool_create(&malloc_pool_,
                                     PcieMemPoolStr,
                                     config->StackNonCacheMemory,
                                     config->StackNonCacheMemorySize,
                                     (UINT32)sizeof(TX_BYTE_POOL));

    PCIE_UtilityPrintUInt5("AmbaPCIE_SysInit(): %d", ret, 0, 0, 0, 0);

    return ret;
}

static void *pcie_malloc(size_t size)
{
    void * ptr = NULL;

    if(size > 0U) {
        // We simply wrap the threadX call into a standard form
        UINT r = _txe_byte_allocate(&malloc_pool_, &ptr, (UINT32)size, 100);

        PCIE_UtilityPrintUInt5("threadx_malloc(): size = %d return = %d", (UINT32)size, r, 0, 0, 0);

        if(r != TX_SUCCESS) {
            ptr = NULL;
        }
    }
    //else NULL if there was no size

    return ptr;
}

UINT32 AmbaPCIE_MemAlloc(UINT32 size, UINT64 *addr)
{
    UINT32 Ret = PCIE_ERR_SUCCESS;
    UINT32 flag = Impl_ApiFlagGet();

    if (flag == FLAG_PCIE_API_UNINIT) {
        Ret = PCIE_ERR_FLOW;
    } else if (addr == NULL) {
        Ret = PCIE_ERR_ARG;
    } else {
        // keep the address is size aligned.
        // Address start from XXXX3000 means the memory width is 12 bits
        UINT32 align = size;
        UINT64 mask = ~((UINT64)align - 1U);
        const void *p_mem = pcie_malloc((size_t)size+align-1U);
        UINT64 mem = PCIE_UtilityVp2U64(p_mem);
        const void *ptr = PCIE_UtilityU64AddrToPtr((mem+align-1U) & mask);
        *addr = PCIE_UtilityVp2U64(ptr);
    }
    return Ret;
}

#define MAX_ISR_OPERATIONS             30U
#define PCIE_ISR_QUEUE_MEM_SIZE        (sizeof(PCIE_ISR_REQUEST_s) * MAX_ISR_OPERATIONS)

static AMBA_KAL_MSG_QUEUE_t    IsrRequestQueue; //!< Request Queue for ISR task

static void PCIE_ISR(UINT32 IntID, UINT32 UserArg)
{
    (void) UserArg;
    PCIE_ISR_REQUEST_s request;
    UINT32 uret;

    uret = AmbaRTSL_GicIntDisable(IntID);
    if (uret == INT_ERR_NONE) {
        request.IntID = IntID;
        if (0U != PCIE_UtilityQueueIsrSend(&IsrRequestQueue, &request, TX_NO_WAIT)) {
            // action TBD
        }
    }
}


static UINT32 isr_queue_create(void)
{
    static UINT8 isr_queue_memory[PCIE_ISR_QUEUE_MEM_SIZE] __attribute__((section(".bss.noinit")));
    UINT32 uret;

    PCIE_UtilityMemorySet(isr_queue_memory, 0, sizeof(isr_queue_memory));
    uret = PCIE_UtilityQueueCreate(&IsrRequestQueue,
                                   isr_queue_memory,
                                   (UINT32)sizeof(PCIE_ISR_REQUEST_s),
                                   MAX_ISR_OPERATIONS);

    return uret;
}

static void isr_handler_dma(void)
{
    const PCIE_PrivateData *pd = PCIE_GetPrivateData();
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT32 ret, ch, status = PCIE_ERR_FAIL, get_src = 0;
    PCIE_UdmaConfiguration dmaConf;
    PCIE_Bool done = PCIE_FALSE;

    ret = obj->UDMA_GetConfiguration (pd,&  dmaConf);

    if (ret == PCIE_ERR_SUCCESS) {

        for (ch = 0; ch < dmaConf.numChannels; ch++) {

            // check and clear error interrupt
            ret = obj->UDMA_GetErrorInterrupt(pd,ch,&done);
            if ((ret == PCIE_ERR_SUCCESS) && (done == PCIE_TRUE)) {
                status = PCIE_ERR_FAIL;
                get_src = 1;
            }
            // check and clear done interrupt
            if (get_src == 0U) {
                ret = obj->UDMA_GetDoneInterrupt(pd, ch, &done);
                if ((ret == PCIE_ERR_SUCCESS) && (done == PCIE_TRUE)) {
                    status = PCIE_ERR_SUCCESS;
                    get_src = 1;
                }
            }
            if (get_src == 1U) {
                break;
            }
        }

    } else {
        status = PCIE_ERR_FAIL;
    }

    if (obj->DmaSysCb != NULL) {
        obj->DmaSysCb(status);
    }

}

static void *isr_task_handler(void *EntryArg)
{

    PCIE_ISR_REQUEST_s request = {0};
    // pass misra check
    AmbaMisra_TouchUnused(EntryArg);

    for (;;) {
        if (0U == PCIE_UtilityQueueIsrRecv(&IsrRequestQueue, &request, AMBA_KAL_WAIT_FOREVER)) {

            UINT32 IntID = request.IntID;
            switch(IntID) {
            case AMBA_INT_SPI_ID259_PCIEC_DMA:
                isr_handler_dma();
                break;
            default:
                /* Should not be here */
                break;
            }

            (void)AmbaRTSL_GicIntEnable(IntID);

        } else {
            break;
        }
    }
    return NULL;
}

static UINT32 isr_task_create(void)
{
    static char IsrTaskStr[]   = "PCIE ISR Task";
    static AMBA_KAL_TASK_t  IsrTask;
    static UINT8            IsrTaskStack[2048];

    UINT32 uret;

    /* Create Tx Complete Task */
    uret = AmbaKAL_TaskCreate(&IsrTask,               /* pTask */
                              IsrTaskStr,              /* pTaskName */
                              12,                             /* Priority */
                              isr_task_handler,               /* EntryFunction */
                              NULL,                           /* EntryArg */
                              IsrTaskStack,            /* pStackBase */
                              (UINT32)sizeof(IsrTaskStack),    /* StackByteSize */
                              1);

    return uret;
}



static PCIE_INIT_PARAM_s save_config;
static UINT32 flag_pcie_api = FLAG_PCIE_API_UNINIT;

UINT32 Impl_ApiFlagGet(void)
{
    return flag_pcie_api;
}

void Impl_ApiFlagSet(UINT32 flag)
{
    flag_pcie_api |= flag;
}


UINT32 AmbaPCIE_Init(const PCIE_INIT_PARAM_s *params)
{
    UINT32 Ret = 0;
    UINT32 flag = Impl_ApiFlagGet();

    if (flag != FLAG_PCIE_API_UNINIT) {
        Ret = PCIE_ERR_FLOW;
    } else if (params == NULL) {
        Ret = PCIE_ERR_ARG;
    } else {
        AMBA_INT_CONFIG_s IntConfig;
        UINT32 IntID = AMBA_INT_SPI_ID259_PCIEC_DMA;

        // record app config
        PCIE_UtilityMemoryCopy(&save_config, params, sizeof(PCIE_INIT_PARAM_s));

        // create isr queue
        (void)isr_queue_create();
        // creat isr task
        (void)isr_task_create();

        if (params->IsrEnable == PCIE_ISR_ENABLE) {

            IntConfig.TriggerType   = AMBA_INT_HIGH_LEVEL_TRIGGER;
            IntConfig.IrqType    = AMBA_INT_FIQ;
            IntConfig.CpuTargets = 0x01;

            Ret = AmbaRTSL_GicIntConfig(IntID, &IntConfig, PCIE_ISR, 0);
            if (Ret == INT_ERR_NONE) {
                Ret = AmbaRTSL_GicIntEnable(IntID);
                if (Ret == INT_ERR_NONE) {

                } else {
                    /* Should not be here */
                }
            } else {
                /* Should not be here */
            }
        }

        // allocate memory pool
        if (save_config.MemConfig != NULL) {
            (void)pcie_mem_init(save_config.MemConfig);
        }

        // init controller, and then we can program related registers
        Impl_PcieInit(save_config.Mode, save_config.Gen, save_config.Lane, save_config.ClockSrc);

        if (save_config.Mode == PCIE_RC_MODE) {
            (void)Impl_RcInit(PCIE_CFG_BASE, PCIE_RP_OB_REGION0_AXI_ADDR);
            Impl_ApiFlagSet(FLAG_PCIE_API_RC_INIT);
        } else if (save_config.Mode == PCIE_EP_MODE) {
            (void)Impl_EpInit(PCIE_CFG_BASE);
            Impl_ApiFlagSet(FLAG_PCIE_API_EP_INIT);
        } else {
            /* should not be here */
        }
    }

    return Ret;
}

UINT32 AmbaPCIE_Start(void)
{
    UINT32 ret = PCIE_ERR_SUCCESS;
    UINT32 flag = Impl_ApiFlagGet();

    if ((flag & FLAG_PCIE_API_INIT_MASK) == 0U) {
        ret = PCIE_ERR_FLOW;
    } else {
        Impl_LinkStart(save_config.Mode, save_config.Gen);

        if (save_config.Mode == PCIE_RC_MODE) {
            // Show endpoint supported BARs
            (void)Impl_RcGetEpBarInfo();
            Impl_ApiFlagSet(FLAG_PCIE_API_RC_START);
        } else if (save_config.Mode == PCIE_EP_MODE) {
            Impl_ApiFlagSet(FLAG_PCIE_API_EP_START);
        } else {
            /* should not be here */
        }
    }

    return ret;
}


