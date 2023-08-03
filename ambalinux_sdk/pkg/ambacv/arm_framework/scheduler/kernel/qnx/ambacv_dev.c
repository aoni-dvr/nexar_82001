/**
 *  @file ambacv_dev.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Ambacv dev APIs
 *
 */

#include <stdint.h>
#include "cavalry_ioctl.h"
#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "cache_kernel.h"
#include "schdr_kernel.h"
#include "flexidag_kernel.h"
#include "schdr_api.h"
#include <devctl.h>
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"  // for misra depress
#endif

#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

/*
 * Define THREAD_POOL_PARAM_T such that we can avoid a compiler
 * warning when we use the dispatch_*() functions below
 */
#define THREAD_POOL_PARAM_T dispatch_context_t

#include <sys/iofunc.h>
#include <sys/dispatch.h>

static resmgr_connect_funcs_t    connect_funcs;
static resmgr_io_funcs_t         io_funcs;
static iofunc_attr_t             attr;
static struct cache_ctrl         cinfo;
kernel_memblk_t cma_master;

static uint64_t cv_schdr_size = 0UL;
#if !defined(ENABLE_AMBA_MAL)
static uint64_t cv_att_size = 0xFFFFFFFFUL;
static uint64_t cv_att_pa = 0x0UL;
static uint64_t cv_att_ca = 0x0UL;
#endif

#if defined(ENABLE_AMBA_MAL)
AMBA_MAL_INFO_s     krn_schdr_core_info;
AMBA_MAL_INFO_s     krn_schdr_cma_info;
AMBA_MAL_INFO_s     krn_schdr_rtos_info;
AMBA_MAL_INFO_s     krn_dsp_data_info;
#endif

int ambacv_ioctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int     nbytes, status;
    void *arg;
    int ret;
    /*
     Let common code handle DCMD_ALL_* cases.
     You can do this before or after you intercept devctls, depending
     on your intentions.  Here we aren't using any predefined values,
     so let the system ones be handled first. See note 2.
    */
    if (EOK != (status = iofunc_devctl_verify(ctp, msg, ocb, _IO_DEVCTL_VERIFY_LEN))) {
        return status;
    }
    iofunc_attr_unlock(&attr);
    status = nbytes = 0;

    /*
     Note this assumes that you can fit the entire data portion of
     the devctl into one message.  In reality you should probably
     perform a MsgReadv() once you know the type of message you
     have received to get all of the data, rather than assume
     it all fits in the message.  We have set in our main routine
     that we'll accept a total message size of up to 2 KB, so we
     don't worry about it in this example where we deal with ints.
    */

    /* Get the data from the message. See Note 3. */
    // rx_data = _DEVCTL_DATA(msg->i);
    switch (msg->i.dcmd) {
    case AMBACV_START_VISORC:
        nbytes = 0U;
        break;
    case AMBACV_FIND_CVTASKMSG:
        nbytes = sizeof(ambacv_get_value_t);
        break;
    case AMBACV_GET_SYSCONFIG:
        nbytes = sizeof(visorc_init_params_t);
        break;
    case AMBACV_RECV_ORCMSG:
        nbytes = sizeof(ambacv_get_value_t);
        break;
    case AMBACV_SEND_ARMMSG:
        nbytes = 0U;
        break;
    case AMBACV_GET_MEM_LAYOUT:
        nbytes = sizeof(ambacv_mem_t);
        break;
    case AMBACV_UCODE_LOADED:
        nbytes = 0U;
        break;
    case AMBACV_STOP_VISORC:
        nbytes = 0U;
        break;
    case AMBACV_SEND_VPMSG:
        nbytes = 0U;
        break;
    case AMBACV_CONFIG_IDSP:
        nbytes = 0U;
        break;
    case AMBACV_CONFIG_VIN:
        nbytes = 0U;
        break;
    case AMBACV_SEND_VINMSG:
        nbytes = 0U;
        break;
    case AMBACV_CACHE_CLEAN:
        nbytes = 0U;
        break;
    case AMBACV_CACHE_INVLD:
        nbytes = 0U;
        break;
    case AMBACV_SET_BIN_PATH:
        nbytes = 0U;
        break;
    case AMBACV_GET_LOG_INFO:
        nbytes = sizeof(ambacv_log_t);
        break;
    case AMBACV_SEND_ASYNCMSG:
        nbytes = 0U;
        break;
    case AMBACV_GET_VERSION:
        nbytes = sizeof(uint32_t);
        break;
    case AMBACV_GET_MAL_VERSION:
        nbytes = sizeof(uint32_t);
        break;
    case AMBACV_GET_VALUE:
        nbytes = sizeof(ambacv_get_value_t);
        break;
    case AMBACV_CORE_MUTEX:
        nbytes = 0U;
        break;
    case AMBACV_RESUME_VISORC:
        nbytes = sizeof(visorc_init_params_t);
        break;
    case AMBACV_GET_STATE:
        nbytes = sizeof(uint32_t);
        break;
    case AMBACV_GET_ATT:
        ret = procmgr_ability( ctp->info.pid, PROCMGR_ADN_NONROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_MEM_PHYS,PROCMGR_AID_EOL);
        if(ret != 0) {
            krn_printU5("[ERROR] ambacv_ioctl() : AMBACV_GET_ATT procmgr_ability fail ret %d errno %d ", ret, errno, 0U, 0U, 0U);
        }
        nbytes = sizeof(ambacv_att_region_t);
        break;
    case AMBACV_GET_TIME:
        nbytes = sizeof(uint32_t);
        break;
    case AMBACV_WAIT_ORCMSG:
        nbytes = sizeof(uint32_t);
        break;
    /*-= Flexidag specific IOCTL =--------------------------------------------*/
    case AMBACV_FLEXIDAG_ENABLE:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_CREATE:
        nbytes = sizeof(flexidag_ioctl_create_t);
        break;
    case AMBACV_FLEXIDAG_CREATE_QUERY:
        nbytes = sizeof(flexidag_ioctl_create_t);
        break;
    case AMBACV_FLEXIDAG_SET_PARAMETER:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_ADD_TBAR:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_ADD_SFB:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_OPEN:
        nbytes = sizeof(flexidag_ioctl_open_t);
        break;
    case AMBACV_FLEXIDAG_OPEN_QUERY:
        nbytes = sizeof(flexidag_ioctl_open_t);
        break;
    case AMBACV_FLEXIDAG_SET_STATE_BUFFER:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_SET_TEMP_BUFFER:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_INIT:
        nbytes = sizeof(flexidag_ioctl_handle_t);
        break;
    case AMBACV_FLEXIDAG_INIT_QUERY:
        nbytes = sizeof(flexidag_ioctl_handle_t);
        break;
    case AMBACV_FLEXIDAG_PREP_RUN:
        nbytes = sizeof(flexidag_ioctl_pre_run_t);
        break;
    case AMBACV_FLEXIDAG_SET_INPUT_BUFFER:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_RUN:
        nbytes = sizeof(flexidag_ioctl_run_t);
        break;
    case AMBACV_FLEXIDAG_RUN_QUERY:
        nbytes = sizeof(flexidag_ioctl_run_t);
        break;
    case AMBACV_FLEXIDAG_WAIT_RUN_FINISH:
        nbytes = sizeof(flexidag_ioctl_run_t);
        break;
    case AMBACV_FLEXIDAG_CLOSE:
        nbytes = sizeof(flexidag_ioctl_handle_t);
        break;
    case AMBACV_FLEXIDAG_CLOSE_QUERY:
        nbytes = sizeof(flexidag_ioctl_handle_t);
        break;
    case AMBACV_FLEXIDAG_INFO:
        nbytes = 0U;
        break;
    case AMBACV_FLEXIDAG_HANDLE_MSG:
        nbytes = 0U;
        break;
    /*-= Cavalry CMA specific IOCTL =---------------------------------------------*/
    case CAVALRY_QUERY_BUF:
        nbytes = 0U;
        break;
    case CAVALRY_ALLOC_MEM:
        nbytes = sizeof(struct cavalry_mem);
        break;
    case CAVALRY_FREE_MEM:
        nbytes = sizeof(struct cavalry_mem);
        break;
    case CAVALRY_SYNC_CACHE_MEM:
        nbytes = 0U;
        break;
    case CAVALRY_GET_USAGE_MEM:
        nbytes = 0U;
        break;
    /*-= Superdag/Cavalry specific IOCTL =------------------------------------*/
    case CAVALRY_ENABLE:
        nbytes = 0U;
        break;
    case CAVALRY_ASSOCIATE_BUF:
        nbytes = 0U;
        break;
    case CAVALRY_SET_MEMORY_BLOCK:
        nbytes = 0U;
        break;
    case CAVALRY_HANDLE_VISORC_REPLY:
        nbytes = 0U;
        break;
    default:
        krn_printU5("[ERROR] ambacv_ioctl() : unknown IOCTL 0x%x ", msg->i.dcmd, 0U, 0U, 0U, 0U);
        status = (int32_t)ERR_DRV_SCHDR_IOCTL_UNKNOW;
        break;
    }

    if(status == 0) {
        arg = _DEVCTL_DATA(msg->i);
        status = (int32_t)krn_ambacv_ioctl(ocb, msg->i.dcmd, arg);
    }

    /* Clear the return message. Note that we saved our data past
       this location in the message. */
    memset(&msg->o, 0, sizeof(msg->o));

    /*
     If you wanted to pass something different to the return
     field of the devctl() you could do it through this member.
     See note 5.
    */
    msg->o.ret_val = status;

    /* Indicate the number of bytes and return the message */
    msg->o.nbytes = nbytes;

    iofunc_attr_lock(&attr);
    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
}

static char buffer[512] = {"Hello world\n"};
int ambacv_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    size_t nleft;
    size_t nbytes;
    int nparts;
    int status;

    if ((status = iofunc_read_verify (ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return (ENOSYS);
    /*
     * On all reads (first and subsequent), calculate
     * how many bytes we can return to the client,
     * based upon the number of bytes available (nleft)
     * and the client's buffer size
     */

    // Every read return the buffer defined here.
    nleft = strlen(buffer) + 1 - ocb->offset;
    nbytes = min (_IO_READ_GET_NBYTES(msg), nleft);
    if (nbytes > 0) {
        /* set up the return data IOV */
        SETIOV (ctp->iov, buffer + ocb->offset, nbytes);


        /* set up the number of bytes (returned by client's read()) */
        _IO_SET_READ_NBYTES (ctp, nbytes);
        /*
              * advance the offset by the number of bytes
              * returned to the client.
              */
        ocb->offset += nbytes;
        nparts = 1;
    } else {
        /*
         * they've asked for zero bytes or they've already previously
         * read everything
         */
        _IO_SET_READ_NBYTES (ctp, 0);
        nparts = 0;
    }
    /* mark the access time as invalid (we just accessed it) */
    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_ATIME;
    return (_RESMGR_NPARTS (nparts));
}

int ambacv_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
    int     status;
    size_t  nbytes;

    if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return(ENOSYS);

    /* Extract the length of the client's message. */
    nbytes = _IO_WRITE_GET_NBYTES(msg);

    /* Filter out malicious write requests that attempt to write more
       data than they provide in the message. */
    if(nbytes > (size_t)ctp->info.srcmsglen - (size_t)ctp->offset - sizeof(io_write_t)) {
        return EBADMSG;
    }

    /* set up the number of bytes (returned by client's write()) */
    _IO_SET_WRITE_NBYTES (ctp, nbytes);

    /*
     *  Reread the data from the sender's message buffer.
     *  We're not assuming that all of the data fit into the
     *  resource manager library's receive buffer.
     */

    resmgr_msgread(ctp, buffer, nbytes, sizeof(msg->i));
    buffer [nbytes] = '\0'; /* just in case the text is not NULL terminated */

    if (nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

    return (_RESMGR_NPARTS (0));
}

int ambacv_close(resmgr_context_t *__ctp, void *__reserved, iofunc_ocb_t *__ocb)
{
    int ret;

    krn_flexidag_close_by_owner((uint64_t)__ocb);
    ret = iofunc_close_ocb_default(__ctp, __reserved, __ocb);
    return ret;
}

#if defined(ENABLE_AMBA_MAL)
static int ambacv_of_get_mal_info(void)
{
    int rval = 0;

    if(AmbaMAL_Init() != 0) {
        krn_printU5("[ERROR] ambacv_of_get_mal_info() : AmbaMAL_Init fail ", 0U, 0U, 0U, 0U, 0U);
        rval = -1;
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SCHDR, &krn_schdr_core_info) != 0U) {
            krn_printU5("[ERROR] ambacv_of_get_mal_info() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_SCHDR fail \n", 0U, 0U, 0U, 0U, 0U);
            rval = -1;
        }
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SYS, &krn_schdr_cma_info) != 0U) {
            krn_printU5("[ERROR] ambacv_of_get_mal_info() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_SYS fail \n", 0U, 0U, 0U, 0U, 0U);
            rval = -1;
        }
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_RTOS, &krn_schdr_rtos_info) != 0U) {
            krn_printU5("[ERROR] ambacv_of_get_mal_info() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_CV_RTOS fail \n", 0U, 0U, 0U, 0U, 0U);
            rval = -1;
        }
    }

    if(rval == 0) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_DSP_DATA, &krn_dsp_data_info) != 0U) {
            krn_printU5("[ERROR] ambacv_of_get_mal_info() : SYS: AmbaMAL_GetInfo AMBA_MAL_ID_DSP_DATA fail \n", 0U, 0U, 0U, 0U, 0U);
            rval = -1;
        }
    }
    return rval;
}

static void ambacv_of_init(void)
{
    ambacv_all_mem_t *mem_all;
    void *vaddr = NULL;
    uint32_t ret = 0;
    AMBA_MAL_BUF_s Buf;

    mem_all = krn_ambacv_get_mem();
    krn_printU5("[AMBACV] SCHEDULER ID  :   %d", scheduler_id, 0U, 0U, 0U, 0U);
    mem_all->cv_region.base = 0x0U;
    mem_all->cv_region.size = 0xFFFFFFFFFFU;
    mem_all->cv_att_region.base = 0x0U;
    mem_all->cv_att_region.size = 0xFFFFFFFFFFU;

    if(ambacv_of_get_mal_info() == 0) {
        if(cache_init(0, &cinfo, NULL) != 0 ) {
            krn_printU5("[ERROR] ambacv_of_init() : cache_init fail ret = %d errno = %d ", ret, errno, 0U, 0U, 0U);
        } else {
            /* Get CV mem range */
            if(krn_schdr_core_info.Type == AMBA_MAL_TYPE_INVALID) {
                ret = AmbaMAL_Alloc(AMBA_MAL_ID_CV_SYS, cv_schdr_size, 0x1000, &Buf);
                if(ret != 0U) {
                    krn_printU5("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Alloc fail ret=0x%x", ret, 0U, 0U, 0U, 0U);
                } else {
                    if(enable_arm_cache == 1U) {
                        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, Buf.PhysAddr, Buf.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
                    } else {
                        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, Buf.PhysAddr, Buf.Size, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
                    }
                    if (vaddr == NULL) {
                        krn_printU5("[ERROR] ambacv_of_init() : mmap fail errno = %d", errno, 0U, 0U, 0U, 0U);
                    } else {
                        mem_all->all.base = krn_p2c(Buf.PhysAddr);
                        mem_all->all.size = cv_schdr_size - CAVALRY_MEM_LOG_SIZE;
                        mem_all->cma_region.base = krn_p2c(Buf.PhysAddr + cv_schdr_size);
                        mem_all->cma_region.size = krn_schdr_cma_info.Size - cv_schdr_size;

                        krn_ambacv_set_sysinit((visorc_init_params_t *)(vaddr + SYSINIT_OFFSET));
                        krn_mmap_add_master(vaddr, Buf.PhysAddr, cv_schdr_size, enable_arm_cache);
                        krn_printU5("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] ",
                                    (krn_schdr_cma_info.PhysAddr), (krn_schdr_cma_info.PhysAddr + cv_schdr_size), 0U, 0U, 0U);
                        krn_printU5("[AMBACV] CMA ALLOCABLE :   [0x%llx--0x%llx] ",
                                    (krn_schdr_cma_info.PhysAddr + cv_schdr_size), (krn_schdr_cma_info.PhysAddr + krn_schdr_cma_info.Size), 0U, 0U, 0U);

                        vaddr = NULL;
                        if(AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, mem_all->cma_region.base, mem_all->cma_region.size, AMBA_MAL_ATTRI_CACHE, &vaddr) != 0) {
                            krn_printU5("ambacv_of_init() : AmbaMAL_Map fail ", 0U, 0U, 0U, 0U, 0U);
                        } else {
                            cma_master.pBuffer = vaddr;
                            if (cma_master.pBuffer == MAP_FAILED) {
                                krn_printU5("[ERROR] ambacv_of_init() : mmap fail errno = %d", errno, 0U, 0U, 0U, 0U);
                            } else {
                                cma_master.buffer_cacheable = 1U;
                                cma_master.buffer_daddr = krn_c2p(mem_all->cma_region.base);
                                cma_master.buffer_size = mem_all->cma_region.size;
                            }
                        }
                    }
                }
            } else {
                if(enable_arm_cache == 1U) {
                    ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SCHDR, krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
                } else {
                    ret = AmbaMAL_Map(AMBA_MAL_ID_CV_SCHDR, krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
                }

                if(vaddr == NULL) {
                    krn_printU5("[ERROR] ambacv_of_init() : SYS: AmbaMAL_Map fail ret=0x%x", ret, 0U, 0U, 0U, 0U);
                    ret = 1;
                } else {
                    mem_all->all.base = krn_p2c(krn_schdr_core_info.PhysAddr);
                    mem_all->all.size = krn_schdr_core_info.Size - CAVALRY_MEM_LOG_SIZE;
                    mem_all->cma_region.base = krn_p2c(krn_schdr_cma_info.PhysAddr);
                    mem_all->cma_region.size = krn_schdr_cma_info.Size;

                    krn_ambacv_set_sysinit((visorc_init_params_t *)(vaddr + SYSINIT_OFFSET));
                    krn_mmap_add_master(vaddr, krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, enable_arm_cache);
                    krn_printU5("[AMBACV] CACHE CONFIG  :  ARM(%s), ORC(%s)\n",
                                enable_arm_cache ? "enabled" : "disabled",
                                enable_orc_cache ? "enabled" : "disabled", 0U, 0U, 0U);
                    krn_printU5("[AMBACV] CMA RANGE     :   [0x%llx--0x%llx]\n",
                                krn_schdr_cma_info.PhysAddr, krn_schdr_cma_info.Size, 0U, 0U, 0U);
                    krn_printU5("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] ",
                                krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, 0U, 0U, 0U);

                    vaddr = NULL;
                    if(AmbaMAL_Map(AMBA_MAL_ID_CV_SYS, krn_schdr_core_info.PhysAddr, krn_schdr_core_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr) != 0) {
                        krn_printU5("ambacv_of_init() : AmbaMAL_Map fail ", 0U, 0U, 0U, 0U, 0U);
                    } else {
                        cma_master.pBuffer = vaddr;
                        if (cma_master.pBuffer == MAP_FAILED) {
                            krn_printU5("[ERROR] ambacv_of_init() : mmap fail errno = %d", errno, 0U, 0U, 0U, 0U);
                        } else {
                            cma_master.buffer_cacheable = 1U;
                            cma_master.buffer_daddr = krn_c2p(mem_all->cma_region.base);
                            cma_master.buffer_size = mem_all->cma_region.size;
                        }
                    }
                }
            }

            /* Get CV RTOS mem range */
            if(ret == 0U) {
                /* Get CV RTOS mem range */
                vaddr = NULL;
                if((krn_schdr_rtos_info.Type != AMBA_MAL_TYPE_INVALID) && (krn_schdr_rtos_info.Size != 0UL)) {
                    if(enable_rtos_cache == 1U) {
                        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_RTOS, krn_schdr_rtos_info.PhysAddr, krn_schdr_rtos_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
                    } else {
                        ret = AmbaMAL_Map(AMBA_MAL_ID_CV_RTOS, krn_schdr_rtos_info.PhysAddr, krn_schdr_rtos_info.Size, AMBA_MAL_ATTRI_NON_CACHE, &vaddr);
                    }

                    if(vaddr == NULL) {
                        krn_printU5("[ERROR] ambacv_of_init() : RTOS: AmbaMAL_Map fail ret=0x%x", ret, 0U, 0U, 0U, 0U);
                        ret = 1;
                    } else {
                        mem_all->rtos_region.base = krn_p2c(krn_schdr_rtos_info.PhysAddr);
                        mem_all->rtos_region.size = krn_schdr_rtos_info.Size;
                        krn_mmap_add_rtos(vaddr, krn_schdr_rtos_info.PhysAddr, krn_schdr_rtos_info.Size, enable_rtos_cache);
                        krn_printU5("[AMBACV] RTOS RANGE    :   [0x%llx--0x%llx] @ (%p) (cache is %s)\n",
                                    krn_schdr_rtos_info.PhysAddr, (krn_schdr_rtos_info.PhysAddr + krn_schdr_rtos_info.Size), vaddr, (enable_rtos_cache != 0) ? "enabled" : "disabled",0U);
                    }
                }
            }

            if(ret == 0) {
                /* Get DSP DATA mem range */
                vaddr = NULL;
                if((krn_dsp_data_info.Type != AMBA_MAL_TYPE_INVALID) && (krn_dsp_data_info.Size != 0UL)) {
                    ret = AmbaMAL_Map(AMBA_MAL_ID_DSP_DATA, krn_dsp_data_info.PhysAddr, krn_dsp_data_info.Size, AMBA_MAL_ATTRI_CACHE, &vaddr);
                    if(vaddr == NULL) {
                        krn_printU5("[ERROR] ambacv_of_init() : RTOS: AmbaMAL_Map fail ret=0x%x", ret, 0U, 0U, 0U, 0U);
                        ret = 1;
                    } else {
                        krn_mmap_add_dsp_data(vaddr, krn_dsp_data_info.PhysAddr, krn_dsp_data_info.Size, enable_rtos_cache);
                        krn_printU5("[AMBACV] DSP DATA RANGE    :   [0x%llx--0x%llx] @ (%p) \n",
                                    krn_dsp_data_info.PhysAddr, (krn_dsp_data_info.PhysAddr + krn_dsp_data_info.Size), vaddr, 0U, 0U);
                    }
                }
            }

        }
    }
}
#else
static void ambacv_of_init(void)
{
    ambacv_all_mem_t *mem_all;
    struct posix_typed_mem_info info;
    int32_t alloc_fd,map_fd;
    void *vaddr;
    uint64_t offset = 0UL;
    int32_t ret = 0;

    mem_all = krn_ambacv_get_mem();

    ret = cache_init(0, &cinfo, NULL);
    if(ret != 0) {
        krn_printU5("[ERROR] ambacv_of_init() : cache_init fail ret = %d errno = %d ", ret, errno, 0U, 0U, 0U);
    } else {
        /* Get CV mem range */
        alloc_fd = posix_typed_mem_open( "/ram/flexidag_sys", O_RDWR,POSIX_TYPED_MEM_ALLOCATE_CONTIG);
        if(alloc_fd < 0) {
            krn_printU5("[ERROR] ambacv_of_init() : posix_typed_mem_open fail(/ram/flexidag_sys) fd = %d errno = %d", alloc_fd, errno, 0U, 0U, 0U);
        } else {
            ret = posix_typed_mem_get_info(alloc_fd, &info);
            if(ret != 0) {
                krn_printU5("[ERROR] ambacv_of_init() : posix_typed_mem_get_info fail ret = %d errno = %d", ret, errno, 0U, 0U, 0U);
            } else {
                if(cv_schdr_size > info.posix_tmi_length) {
                    krn_printU5("[ERROR] ambacv_of_init() : cv_schdr_size (0x%x) > info.posix_tmi_length (0x%x)", cv_schdr_size, info.posix_tmi_length, 0U, 0U, 0U);
                } else {
                    vaddr = mmap( NULL, cv_schdr_size, PROT_READ | PROT_WRITE,MAP_SHARED, alloc_fd, 0);
                    if (vaddr == MAP_FAILED) {
                        krn_printU5("[ERROR] ambacv_of_init() : mmap fail errno = %d", errno, 0U, 0U, 0U, 0U);
                    } else {
                        ret = mem_offset(vaddr, NOFD, 1, &offset, 0);
                        if(ret != 0) {
                            krn_printU5("[ERROR] ambacv_of_init() : mem_offset fail ret = %d errno = %d", ret, errno, 0U, 0U, 0U);
                        } else {
                            mem_all->cv_region.base = cv_att_pa;
                            mem_all->cv_region.size = cv_att_size;
                            mem_all->cv_att_region.base = cv_att_ca;
                            mem_all->cv_att_region.size = cv_att_size;
                            krn_printU5("[AMBACV] ATT PA RANGE  :   [0x%llx--0x%llx]",mem_all->cv_region.base,(mem_all->cv_region.base+mem_all->cv_region.size), 0U, 0U, 0U);
                            krn_printU5("[AMBACV] ATT CA RANGE  :   [0x%llx--0x%llx]",mem_all->cv_att_region.base,(mem_all->cv_att_region.base+mem_all->cv_region.size), 0U, 0U, 0U);

                            mem_all->all.base = krn_p2c(offset);
                            mem_all->all.size = cv_schdr_size - CAVALRY_MEM_LOG_SIZE;
                            mem_all->cma_region.base = krn_p2c(offset + cv_schdr_size);
                            mem_all->cma_region.size = info.posix_tmi_length - cv_schdr_size;

                            map_fd = posix_typed_mem_open( "/ram/flexidag_sys", O_RDWR,POSIX_TYPED_MEM_MAP_ALLOCATABLE);
                            cma_master.pBuffer = mmap( NULL, mem_all->cma_region.size, PROT_READ | PROT_WRITE,MAP_SHARED, map_fd, mem_all->cma_region.base);
                            if (cma_master.pBuffer == MAP_FAILED) {
                                krn_printU5("[ERROR] ambacv_of_init() : mmap fail errno = %d", errno, 0U, 0U, 0U, 0U);
                            } else {
                                cma_master.buffer_cacheable = 1U;
                                cma_master.buffer_daddr = krn_c2p(mem_all->cma_region.base);
                                cma_master.buffer_size = mem_all->cma_region.size;
                            }
                            krn_ambacv_set_sysinit((visorc_init_params_t *)(vaddr + SYSINIT_OFFSET));
                            krn_mmap_add_master(vaddr, offset, cv_schdr_size, enable_arm_cache);
                            krn_printU5("[AMBACV] CMA PRIVATE   :   [0x%llx--0x%llx] ",
                                        (offset), (offset + cv_schdr_size), 0U, 0U, 0U);
                            krn_printU5("[AMBACV] CMA ALLOCABLE :   [0x%llx--0x%llx] ",
                                        (offset + cv_schdr_size), (offset + info.posix_tmi_length), 0U, 0U, 0U);
                        }
                    }
                }
            }

        }

        /* Get CV RTOS mem range */
        mem_all->rtos_region.base = 0U;
        mem_all->rtos_region.size = 0U;
        krn_mmap_add_rtos(NULL, 0U, 0U, enable_rtos_cache);
        krn_printU5("[AMBACV] RTOS RANGE    :   [0x%llx--0x%llx] @ (cache is %d)",
                    0U, (0U), enable_rtos_cache, 0U, 0U);

        /* Get DSP DATA mem range */
        krn_mmap_add_dsp_data(NULL, 0U, 0U, 1);
    }

}
#endif

extern kisr_entry_t relay_handler;

static void *interrupt_thread (void * data)
{
    struct sigevent event;
    uint32_t irq_num;
    int32_t iid;
    int32_t rval;
    uint32_t ret;
    pthread_t thread_id;

    (void) data;

    thread_id = pthread_self();
    rval = pthread_setschedprio(thread_id, 128);
    if (rval != 0) {
        printf("[ERR] scheduler interrupt thread set priority fail rval %d", rval);
    }
    /* fill in "event" structure */
    memset(&event, 0, sizeof(event));
    event.sigev_notify = SIGEV_INTR;
    /* Enable the INTERRUPTEVENT ability */

    procmgr_ability(0,
                    PROCMGR_ADN_ROOT|PROCMGR_AOP_ALLOW|PROCMGR_AID_INTERRUPTEVENT,
                    PROCMGR_AID_EOL);
    /* intNum is the desired interrupt level */
    ret = krn_get_sod_irq(&irq_num);
    if (ret != 0) {
        printf("[ERR] scheduler interrupt thread get sod irq fail ret 0x%x", ret);
    }
    iid = InterruptAttachEvent (irq_num, &event, 0);
    InterruptMask(irq_num, -1);
    /*... insert your code here ... */
    while (1) {
        InterruptWait (0, NULL);
        if(relay_handler != NULL) {
            relay_handler(irq_num, NULL);
        }
        InterruptUnmask(irq_num, iid);
    }

    return NULL;
}

#ifdef CONFIG_DEVICE_TREE_SUPPORT
static void fdt_get_size(void)
{
    const void *fdt = NULL;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const uint64_t *ptr64;
    const struct fdt_property *prop = NULL;
    uint32_t cpu_id;

    fdt = mmap( NULL, CONFIG_DTB_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED|MAP_PHYS, NOFD, (off64_t)CONFIG_DTB_LOADADDR);
    if (fdt == MAP_FAILED) {
        krn_printU5("[ERROR] fdt_get_size() : fdt map fail errno = %d", errno, 0U, 0U, 0U, 0U);
    } else {
        ret = AmbaFDT_CheckHeader(fdt);
        if (ret != 0) {
            krn_printU5("[ERROR] fdt_get_size() : fdt_check_header ret = %d", ret, 0U, 0U, 0U, 0U);
        } else {
            offset = AmbaFDT_PathOffset(fdt, "/cpus");
            offset = AmbaFDT_FirstSubnode(fdt, offset);
            prop = AmbaFDT_GetProperty(fdt,offset, "reg", &len);
            if (prop != NULL) {
                chr1 = (const char *)&prop->data[0];
                krn_typecast(&ptr1, &chr1);
                cpu_id = AmbaFDT_Fdt32ToCpu(ptr1[0]);
                cluster_id = ((cpu_id >> 16U) & 0xFFU);
                scheduler_id = cluster_id;
                krn_printU5("[AMBACV] CPU ID        :   0x%x", cpu_id, 0U, 0U, 0U, 0U);
            } else {
                scheduler_id = 0U;
            }
            krn_printU5("[AMBACV] SCHEDULER ID  :   %d ", scheduler_id, 0U, 0U, 0U, 0U);

            offset = AmbaFDT_PathOffset(fdt, "scheduler");
            if (offset < 0) {
                krn_printU5("[ERROR] fdt_get_size() : invalid fdt offset %d", offset, 0U, 0U, 0U, 0U);
            } else {
#if !defined(ENABLE_AMBA_MAL)
                prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_pa", &len);
                if ((prop != NULL) && (len == 4)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr1, &chr1);
                    cv_att_pa = AmbaFDT_Fdt32ToCpu(ptr1[0]);
                } else if ((prop != NULL) && (len == 8)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr64, &chr1);
                    cv_att_pa = AmbaFDT_Fdt64ToCpu(ptr64[0]);
                } else {
                    krn_printU5("[ERROR] fdt_get_size() : invalid fdt property(cv_att_pa)", 0U, 0U, 0U, 0U, 0U);
                }

                prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_ca", &len);
                if ((prop != NULL) && (len == 4)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr1, &chr1);
                    cv_att_ca = AmbaFDT_Fdt32ToCpu(ptr1[0]);
                } else if ((prop != NULL) && (len == 8)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr64, &chr1);
                    cv_att_ca = AmbaFDT_Fdt64ToCpu(ptr64[0]);
                } else {
                    krn_printU5("[ERROR] fdt_get_size() : invalid fdt property(cv_att_ca)", 0U, 0U, 0U, 0U, 0U);
                }

                prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_size", &len);
                if ((prop != NULL) && (len == 4)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr1, &chr1);
                    cv_att_size = AmbaFDT_Fdt32ToCpu(ptr1[0]);
                } else if ((prop != NULL) && (len == 8)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr64, &chr1);
                    cv_att_size = AmbaFDT_Fdt64ToCpu(ptr64[0]);
                } else {
                    krn_printU5("[ERROR] fdt_get_size() : invalid fdt property(cv_att_size)", 0U, 0U, 0U, 0U, 0U);
                }
#endif
                prop = AmbaFDT_GetProperty(fdt, offset, "cv_schdr_size", &len);
                if ((prop != NULL) && (len == 4)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr1, &chr1);
                    cv_schdr_size = AmbaFDT_Fdt32ToCpu(ptr1[0]);
                } else if ((prop != NULL) && (len == 8)) {
                    chr1 = (const char *)&prop->data[0];
                    krn_typecast(&ptr64, &chr1);
                    cv_schdr_size = AmbaFDT_Fdt64ToCpu(ptr64[0]);
                } else {
                    krn_printU5("[ERROR] fdt_get_size() : invalid fdt property(cv_schdr_size)", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
}
#endif

int main(int argc, char **argv)
{
    /* declare variables we'll be using */
    thread_pool_attr_t   pool_attr;
    resmgr_attr_t        resmgr_attr;
    dispatch_t           *dpp;
    thread_pool_t        *tpp;
    int                  id;

    (void) argc;
    (void) argv;
    /* initialize dispatch interface */
    if((dpp = dispatch_create()) == NULL) {
        krn_printS5("%s: Unable to allocate dispatch handle.",argv[0], NULL, NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

    /* initialize resource manager attributes */
    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /* initialize functions for handling messages */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);

    io_funcs.devctl = ambacv_ioctl;
    io_funcs.read = ambacv_read;
    io_funcs.write = ambacv_write;
    io_funcs.close_ocb = ambacv_close;
    /* initialize attribute structure used by the device */
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);

    /* attach our device name */
    id = resmgr_attach(dpp,            /* dispatch handle        */
                       &resmgr_attr,   /* resource manager attrs */
                       "/dev/ambacv",  /* device name            */
                       _FTYPE_ANY,     /* open type              */
                       0,              /* flags                  */
                       &connect_funcs, /* connect routines       */
                       &io_funcs,      /* I/O routines           */
                       &attr);         /* handle                 */
    if(id == -1) {
        krn_printS5("%s: Unable to attach name.", argv[0], NULL, NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

#ifdef CONFIG_DEVICE_TREE_SUPPORT
    fdt_get_size();
#else
    cv_schdr_size = CONFIG_CV_MEM_SCHDR_SIZE;
#endif
    ambacv_of_init();
    (void) krn_ambacv_init();

    /* Start the thread that will handle interrupt events. */
    if(pthread_create (NULL, NULL, interrupt_thread, NULL) != 0) {
        krn_printS5("%s: Unable to create interrupt_thread.", argv[0], NULL, NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

    /* initialize thread pool attributes */
    memset(&pool_attr, 0, sizeof pool_attr);
    pool_attr.handle = dpp;
    pool_attr.context_alloc = dispatch_context_alloc;
    pool_attr.block_func = dispatch_block;
    pool_attr.unblock_func = dispatch_unblock;
    pool_attr.handler_func = dispatch_handler;
    pool_attr.context_free = dispatch_context_free;
    pool_attr.lo_water = 2;
    pool_attr.hi_water = 4;
    pool_attr.increment = 1;
    pool_attr.maximum = 50;

    /* allocate a thread pool handle */
    if((tpp = thread_pool_create(&pool_attr,
                                 POOL_FLAG_EXIT_SELF)) == NULL) {
        krn_printS5("%s: Unable to initialize thread pool.",
                    argv[0], NULL, NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

    /* Start the threads. This function doesn't return. */
    thread_pool_start(tpp);

    krn_printU5("ambacv dev main stop ", 0U, 0U, 0U, 0U, 0U);
    return EXIT_SUCCESS;
}

