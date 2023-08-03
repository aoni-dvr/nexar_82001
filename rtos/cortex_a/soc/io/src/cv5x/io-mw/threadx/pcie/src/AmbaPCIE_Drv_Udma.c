/**
 *  @file AmbaPCIE_Drv_Udma.c
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
 *  @details PCIe UDMA driver higher level functions. Providing example usage of lower layer.
 */
#include "AmbaPCIE_Drv.h"


/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
convertToPhys(const PCIE_td_cfg * p_td_cfg,
              UINT64     in,
              UINT64 *   out)
{
    UINT64 virt_offset;

    /* If "in" is 0, don't try converting */
    if (in == 0U) {
        *out = 0;
    } else {

        virt_offset = in - (ConvertTdCfgToUintptr(p_td_cfg));

        *out = p_td_cfg->desc_mem_phys_base_addr + virt_offset;
    }

    return (0);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
convertToVirt(const PCIE_td_cfg * p_td_cfg,
              UINT64     in,
              UINT64 *   out)
{
    UINT64 phys_offset;

    /* If "in" is 0, don't try converting */
    if (in == 0U) {
        *out = 0;
    } else {
        phys_offset = in - p_td_cfg->desc_mem_phys_base_addr;

        *out = (ConvertTdCfgToUintptr(p_td_cfg)) + phys_offset;
    }
    return (0);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
getFreeDar(const PCIE_td_cfg * p_td_cfg, UINT32 * dar_idx)
{
    UINT32 arr;
    UINT32 result;
    for (arr = 0; arr < PCIE_MAX_DARS; arr++) {
        if (p_td_cfg->dars[arr].state == PCIE_DAR_FREE) {
            break;
        }
    }

    if (arr == PCIE_MAX_DARS) {
        /* No free dars left */
        result = 1;
    } else {
        *dar_idx = arr;
        result = 0;
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
freeDar(PCIE_td_dar * p_dar)
{
    UINT32 jj;

    p_dar->state          = PCIE_DAR_FREE;
    p_dar->next           = 0xFFFFFFFFU;
    p_dar->num_slots_used =  0;
    p_dar->next_free_slot =  0;

    for (jj = 0; jj < PCIE_MAX_DESC_SLOTS; jj++) {
        UINT32 ii;
        for (ii = 0; ii < sizeof(PCIE_xd_desc); ii++) {
            ((UINT8 *)(&(p_dar->xds[jj])))[ii] = 0;
        }
    }

    return (0);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
freeUnusedDars(PCIE_td_cfg * p_td_cfg)
{
    UINT32 ii;
    PCIE_td_cfg * local_p_td_cfg = p_td_cfg;
    for (ii = 0; ii < PCIE_MAX_DARS; ii++) {
        PCIE_td_dar *p_dar = &(local_p_td_cfg->dars[ii]);
        if (p_dar->num_slots_used == 0U) {
            /* free the unused dar */
            (void)freeDar(p_dar);
        }
    }

    return (0);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
clearDescMem(PCIE_td_cfg * p_td_cfg)
{
    UINT32 ii;

    for (ii = 0; ii < PCIE_MAX_DARS; ii++) {
        (void)freeDar(&(p_td_cfg->dars[ii]));
    }

    for (ii = 0; ii < PCIE_NUM_UDMA_CHANNELS; ii++) {
        p_td_cfg->channels[ii].headDarIdx = 0xFFFFFFFFU;
        p_td_cfg->channels[ii].state      = PCIE_CHANNEL_FREE;
        p_td_cfg->channels[ii].hwState    = PCIE_CHANNEL_FREE;
    }

    p_td_cfg->altd_head = NULL;

    return (0);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32 validateTdCfg(const PCIE_td_cfg * p_td_cfg)
{
    UINT32 result;
    if (p_td_cfg == NULL) {
        result = 1U;
    } else if (p_td_cfg->altd_head == NULL) {
        result = 1U;
    } else {
        result = 0U;
    }
    return (result);
}
UINT32
PCIE_UDMA_Init(PCIE_PrivateData *pD, PCIE_td_cfg *pDescMem, UINT32 phys_base)
{
    PCIE_td_cfg *pcieAddr;
    UINT32 result;
    if ( (pD == NULL) || (pDescMem == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        pD->p_td_cfg = pDescMem;
        pcieAddr = pD->p_td_cfg;
        /* Checking of pcieAddr against NULL is performed, but it violates MISRA2012-RULE-14_3_zc-2 - always evaluates to false
         * Otherwise it would violate MISRA2012-DIR-4_7_a-2, because return from ConvertUintptrToTdCfg is tested in other places */
        if (validateTdCfg(pcieAddr) != 0U) {
            result = PCIE_ERR_ARG;
        } else {

            /* initialize the phys_base from input parameters so that we
             * can do physical to virtual address conversions and vice versa
             */
            pcieAddr->desc_mem_phys_base_addr = phys_base;

            /* clear descriptor memory
             */
            (void)clearDescMem(pcieAddr);
            result = PCIE_ERR_SUCCESS;
        }

    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
linkXdsInTmpDars(PCIE_td_cfg *    p_td_cfg,
                 UINT32         num_slots,
                 const UINT32 * tmp_dar_list,
                 UINT32         num_ex_dars)
{
    UINT32 kk = num_slots;
    UINT32 ii;
    UINT64 phys_addr;

    for (ii = 0; ii < num_ex_dars; ii++) {
        UINT32 nxt_dar_idx, jj;
        UINT32 dar_idx = tmp_dar_list[ii];

        PCIE_xd_desc *xds = p_td_cfg->dars[dar_idx].xds;

        for (jj = 0; jj < (PCIE_MAX_DESC_SLOTS - 1U); jj++) {
            if ((kk - 1U) == 0U) {
                xds[jj].next = 0;
                break;
            } else {

                (void)convertToPhys(p_td_cfg,
                                    ConvertXdDescToUintptr(&(xds[jj + 1U])),
                                    &phys_addr);

                /* Violation of MISRA2012-RULE-18_1_a-3 if there is no condition */
                if (jj > 0U) {
                    xds[jj].next = (UINT32)phys_addr;
                }
            }
            kk--;
        }

        nxt_dar_idx = tmp_dar_list[ii + 1U];

        if (nxt_dar_idx != (UINT32)(-1) ) {

            (void)convertToPhys(p_td_cfg,
                                ConvertXdDescToUintptr(&(xds[0])),
                                &phys_addr);

            xds[jj].next = (UINT32)phys_addr;

            kk--;
        } else {
            xds[jj].next = 0;
        }

    }

    return (0);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
getDarListIdxForChannel(const PCIE_td_cfg * p_td_cfg,
                        UINT32            channel,
                        UINT32 *          idx)
{
    UINT32 result;

    /* if channel is free it couldn't have a dar assigned */
    if (p_td_cfg->channels[channel].state == PCIE_CHANNEL_FREE) {
        result = 1;
    } else {
        *idx = p_td_cfg->channels[channel].headDarIdx;
        result = 0;
    }

    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
unbindDarHeadFromChannel(PCIE_td_cfg * p_td_cfg, const PCIE_td_dar * p_dar)
{
    UINT32 ii;
    UINT32 cnt = 0;
    UINT32 result;

    for (ii = 0; ii < PCIE_NUM_UDMA_CHANNELS; ii++) {
        UINT32 daridx = p_td_cfg->channels[ii].headDarIdx;

        if (p_dar == &(p_td_cfg->dars[daridx])) {
            p_td_cfg->channels[ii].headDarIdx = 0xFFFFFFFFU;
            p_td_cfg->channels[ii].state = PCIE_CHANNEL_FREE;
            cnt++;
        }
    }

    /* we can find only one or fewer instances of the dar being
     * the channel head */
    if (cnt > 1U) {
        result = 1;
    } else {
        result = 0;
    }

    return (result);
}

static UINT32 updateNextFreeSlot(
    UINT32      num_slots_needed,
    UINT32      next_free,
    PCIE_td_dar * p_start,
    const PCIE_td_cfg * p_td_cfg)
{
    UINT32 result;

    /* Use local for num_slots_needed and next_free.
     * A function parameter should not be modified (MISRA2012-RULE-17_8-4) */
    UINT32 local_num_slots_needed = num_slots_needed;
    UINT32 local_next_free = next_free;

    UINT64 phys_addr;
    PCIE_xd_desc *xds = p_start->xds;
    UINT64 next_address;

    /* update next_free_slot */
    if (local_num_slots_needed <= (PCIE_MAX_DESC_SLOTS - local_next_free)) {
        /* we are done - we have enough room in
         * the current dar */
        p_start->next_free_slot += local_num_slots_needed;

        /* now link the xds up together */
        while ((local_num_slots_needed - 1U) > 0U) {
            (void)convertToPhys(p_td_cfg,
                                ConvertXdDescToUintptr(&(xds[local_next_free + 1U])),
                                &phys_addr);

            next_address = (UINT64)phys_addr;
            xds[local_next_free].next = (UINT32)(next_address & 0xFFFFFFFFU);
            xds[local_next_free].next_hi_addr = (UINT32)((next_address >> 32) & 0xFFFFFFFFU);
            local_next_free++;

            local_num_slots_needed--;
        }

        xds[local_next_free].next = 0;

        result = 0U;
    } else {
        p_start->next_free_slot = PCIE_MAX_DESC_SLOTS;

        /* now link the xds up together */
        while (((PCIE_MAX_DESC_SLOTS - local_next_free) - 1U) > 0U) {
            (void)convertToPhys(p_td_cfg,
                                ConvertXdDescToUintptr(&(xds[local_next_free + 1U])),
                                &phys_addr);

            xds[local_next_free].next = (UINT32)phys_addr;
            local_next_free++;
        }

        xds[local_next_free].next = 0;   /* This will link to the new DAR allocated eventually */
        result = 1U;
    }
    return (result);
}

static UINT32 unmarkFreeDarsBusy(
    PCIE_td_cfg * p_td_cfg,
    UINT32 *    p_tmp_dar_list,
    UINT32      num_ex_dars,
    PCIE_td_dar * p_start,
    UINT32      next_free)
{
    UINT32 result = 0U;
    UINT32 ii;
    if (getFreeDar(p_td_cfg, &(p_tmp_dar_list[num_ex_dars])) != 0U) {
        /* no more available dars */

        /* undo tmp DARS marked busy */
        for (ii = 0; ii < num_ex_dars; ii++) {
            (void)freeDar(&(p_td_cfg->dars[p_tmp_dar_list[ii]]));
        }

        /* Back up the first dar too */
        p_start->next_free_slot = next_free;

        result = 1U;
    }
    return (result);
}

static void markFreeDarsBusy(
    UINT32 *       p_num_slots_needed,
    PCIE_td_cfg *    p_td_cfg,
    const UINT32 * p_tmp_dar_list,
    const UINT32 * p_num_ex_dars)
{

    if (*p_num_slots_needed > PCIE_MAX_DESC_SLOTS) {
        p_td_cfg->dars[p_tmp_dar_list[*p_num_ex_dars]].next_free_slot
            = PCIE_MAX_DESC_SLOTS;
        *p_num_slots_needed -= PCIE_MAX_DESC_SLOTS;
    } else {
        p_td_cfg->dars[p_tmp_dar_list[*p_num_ex_dars]].next_free_slot
            = *p_num_slots_needed;
        *p_num_slots_needed  = 0;
    }
    return;
}
/****************************************************************************/
/* This function tries a get a list of empty xd slots                       */
/* for accomodating the mapped td into xds. This may require                */
/* allocation of a new dar if the td does not fit into the                  */
/* current dar.                                                             */
/* Command will fail and return 1 if it cannot allocate the                 */
/* required amount of space.                                                */
/****************************************************************************/
static UINT32 linkFirstDar(
    UINT32         num_slots_in_tmp_dar_list,
    UINT32         num_ex_dars,
    const UINT32 *tmp_dar_list,
    PCIE_td_dar *const *p_start,
    PCIE_td_cfg *const *p_td_cfg)
{
    UINT32 ii;
    UINT32 result = 0U;

    if (tmp_dar_list[0] == 0xFFFFFFFFU) {
        result = 1U;
    } else {
        /* if we got to here, we have a temp array of free dars to
         * accomodate the xds. the dars have not been linked yet, but
         * we know that we have room for the td, so we link them up
         * and send them back
         */
        PCIE_td_dar *p_tmp = *p_start;

        for (ii = 0; ii < num_ex_dars; ii++) {
            if (tmp_dar_list[ii] != 0xFFFFFFFFU ) {
                UINT32 dar_idx = tmp_dar_list[ii];
                p_tmp->next = dar_idx;
                (*p_td_cfg)->dars[dar_idx].next = 0xFFFFFFFFU;
                p_tmp = &((*p_td_cfg)->dars[dar_idx]);
            }
        }

        /* link the first dar to the tmp dars */

        UINT64 phys_addr;

        (void)convertToPhys(*p_td_cfg,
                            ConvertXdDescToUintptr(&((*p_td_cfg)->dars[tmp_dar_list[0]].xds[0])),
                            &phys_addr);

        (*p_start)->xds[PCIE_MAX_DESC_SLOTS - 1U].next = (UINT32)phys_addr;

        /* must have something here or shouldn't be here */
        if (num_slots_in_tmp_dar_list == 0U) {
            result = 1U;
        } else {
            /* link up the xds in the tmp dars */
            (void)linkXdsInTmpDars(*p_td_cfg, num_slots_in_tmp_dar_list, tmp_dar_list, num_ex_dars);
        }
    }
    return (result);
}
static UINT32
getListOfEmptyLinkedSlots(PCIE_td_cfg *   p_td_cfg,
                          PCIE_td_dar *   p_start,
                          UINT32        num_slots_needed,
                          PCIE_xd_desc ** p_slots,        /* starting slot */
                          PCIE_td_dar **  p_dar)          /* in which dar  */
{
    UINT32 ii;
    UINT32 num_ex_dars;        /* number of extra dars required */
    UINT32 next_free;     /* next free slot in start dar   */
    UINT32 tmp_dar_list[PCIE_MAX_DARS];
    UINT32 num_slots_in_tmp_dar_list;

    /* Use local for num_slots_needed.
     * A function parameter should not be modified (MISRA2012-RULE-17_8-4) */
    UINT32 local_num_slots_needed = num_slots_needed;

    UINT32 result;

    for (ii = 0; ii < PCIE_MAX_DARS; ii++) {
        tmp_dar_list[ii] = 0xFFFFFFFFU;
    }

    /* try to allocate the slots incrementally. we need to
     * undo changes if we figure that the td cannot be accomodated.
     * i.e., any dars allocated with an unfulfilled intent to be
     * filled will be freed. we fail right away if we are out of dars
     */

    /* start counting slots */

    /* current dar */
    next_free = p_start->next_free_slot;

    /* handle first dar */
    /* fill up the current dar */
    *p_slots = &(p_start->xds[next_free]);
    *p_dar   =    p_start;

    /* update next_free_slot */
    result = updateNextFreeSlot(local_num_slots_needed, next_free, p_start, p_td_cfg);

    if (result != 0U) {

        result = 0U;

        /* decrement num_slots_needed */

        num_ex_dars = 0;

        /* We need to accomodate 'num_slots_needed' more slots */

        num_slots_in_tmp_dar_list = local_num_slots_needed;

        do {
            /* If there are no dars marked as BUSY, exit loop */
            if (unmarkFreeDarsBusy(p_td_cfg, tmp_dar_list, num_ex_dars, p_start, next_free) == 1U) {
                result = 1U;
                break;
            }

            /* Mark the free dar found as BUSY. We may reverse this if we
             * figure we actually don't have enough dars to cover the request
             * (need to update state of DAR so it doesn't get counted twice)
             */
            p_td_cfg->dars[tmp_dar_list[num_ex_dars]].state = PCIE_DAR_ATTACHED;

            markFreeDarsBusy(&local_num_slots_needed, p_td_cfg, tmp_dar_list, &num_ex_dars);

            /* update the number of additional or extra dars needed */
            num_ex_dars++;
        } while (local_num_slots_needed > 0U);

        if (result == 0U) {
            result = linkFirstDar(num_slots_in_tmp_dar_list, num_ex_dars, tmp_dar_list,
                                  &p_start, &p_td_cfg);

        }
    }
    return (result);
}

/****************************************************************************/
/* Given the pointer for an xd, return the pointer to the DAR in            */
/* which the xd exists. this is required for freeing a dar when             */
/* all xd slots in the DAR have been freed                                  */
/****************************************************************************/
static UINT32
getDarPtrFromXdPtr(PCIE_td_cfg *        p_td_cfg,
                   const PCIE_xd_desc * p_xd_slot,
                   PCIE_td_dar **       p_dar)
{
    UINT32 ii;
    UINT32 result;
    PCIE_td_cfg * local_p_td_cfg = p_td_cfg;
    UINT64 xd_slot = PCIE_UtilityXdDescP2U64(p_xd_slot);
    for (ii = 0; ii < PCIE_MAX_DARS; ii++) {
        UINT64 first_slot = PCIE_UtilityXdDescP2U64(&(local_p_td_cfg->dars[ii].xds[0]));
        UINT64 last_slot = PCIE_UtilityXdDescP2U64(&(local_p_td_cfg->dars[ii].xds[PCIE_MAX_DESC_SLOTS - 1U]));

        if ((xd_slot >= first_slot) && (xd_slot <= last_slot)) {
            *p_dar = &(local_p_td_cfg->dars[ii]);
            break;
        }
    }

    if (ii == PCIE_MAX_DARS) {
        /* Did not find any dar for th xd pointer, this
         * should not happen and is an error */
        result = 1;
    } else {
        result = 0;
    }
    return (result);
}

/****************************************************************************/
/* The xds are already linked together when this function is entered        */
/****************************************************************************/
static UINT32 configXdSlotsBlkOp(
    PCIE_xd_desc *const *p_xd_slots,
    const PCIE_td_desc * p_this_td)
{
    UINT32 result = 0U;

    switch (p_this_td->direction) {
    case PCIE_OUTB_DIR:
        (*p_xd_slots)->ext_lo_addr = p_this_td->xfer.a;
        (*p_xd_slots)->sys_lo_addr = p_this_td->xfer.xn->x;
        break;
    case PCIE_INB_DIR:
        (*p_xd_slots)->sys_lo_addr = p_this_td->xfer.a;
        (*p_xd_slots)->ext_lo_addr = p_this_td->xfer.xn->x;
        break;
    case PCIE_UNDEFINED_DIR:
    default:
        result = 1;
        break;
    }

    if (result == 0U) {
        (*p_xd_slots)->ext_hi_addr = 0;
        (*p_xd_slots)->sys_hi_addr = 0;
        (*p_xd_slots)->sys_attr    = 0;
        (*p_xd_slots)->ext_attr    = 0;

        (*p_xd_slots)->size_and_ctrl.size = (UINT32)(p_this_td->xfer.xn->n & 0xffffffU);
        (*p_xd_slots)->size_and_ctrl.ctrl_bits.control_bits =
            SetUDMA_ControlBits(&((*p_xd_slots)->size_and_ctrl.ctrl_bits.control_bits), PCIE_READ_WRITE, PCIE_TRUE, PCIE_FALSE);
    }

    return (result);
}

static UINT32 calculateDescSize(const PCIE_td_desc * p_this_td)
{
    UINT32 ii;
    UINT32 size = 0U;
    for (ii = 0; ii < (UINT32)(p_this_td->xfer.num_x_vals); ii++) {
        size += p_this_td->xfer.xn[ii].n;
    }
    return (size);
}

static UINT32 populateIndexedDescriptor(
    const PCIE_td_desc *p_this_td,
    PCIE_xd_desc *const *ptr,
    UINT32             index)
{

    UINT32 result = 0U;
    UINT32 sys_addr = 0U;
    UINT32 ext_addr = 0U;
    UINT32 x_addr = (UINT32)(p_this_td->xfer.xn[index].x & 0xFFFFFFFFU);
    PCIE_td_direction direction = p_this_td->direction;
    switch (p_this_td->op_typ) {

    case PCIE_SCT_OP:
        switch (direction) {
        case PCIE_OUTB_DIR:
            ext_addr = x_addr;
            sys_addr =  0;
            break;
        case PCIE_INB_DIR:
            sys_addr = x_addr;
            ext_addr =  0;
            break;
        case PCIE_UNDEFINED_DIR:
        default:
            result = 1;
            break;
        }
        break;

    case PCIE_GTH_OP:
        switch (direction) {
        case PCIE_OUTB_DIR:
            sys_addr = x_addr;
            ext_addr = 0;
            break;
        case PCIE_INB_DIR:
            ext_addr = x_addr;
            sys_addr = 0;
            break;
        case PCIE_UNDEFINED_DIR:
        default:
            result = 1;
            break;
        }
        break;
    case PCIE_UNDEFINED_OP:
    case PCIE_BLK_OP:
    default:
        result = 1;
        break;
    }
    /* assign data to output parameters */
    (*ptr)->sys_lo_addr = sys_addr;
    (*ptr)->ext_lo_addr = ext_addr;
    return (result);
}
static UINT32 setAddressesSctOp(
    PCIE_td_direction direction,
    UINT32 *        p_sys_addr,
    UINT32          a_addr,
    UINT32 *        p_ext_addr)
{
    UINT32 result = 0U;
    switch (direction) {
    case PCIE_OUTB_DIR:
        *p_sys_addr = a_addr;
        *p_ext_addr = 0;
        break;
    case PCIE_INB_DIR:
        *p_sys_addr = 0;
        *p_ext_addr = a_addr;
        break;
    case PCIE_UNDEFINED_DIR:
    default:
        result = 1;
        break;
    }
    return (result);
}
static UINT32 populateNonIndexedDescriptor(
    const PCIE_td_desc * p_this_td,
    PCIE_xd_desc *const *p_xd_slots,
    PCIE_xd_desc *const *ptr)
{
    UINT32 result = 0U;
    UINT32 sys_addr = 0U;
    UINT32 ext_addr = 0U;
    UINT32 a_addr = p_this_td->xfer.a & 0xFFFFFFFFU;
    PCIE_td_direction direction = p_this_td->direction;

    switch (p_this_td->op_typ) {
    case PCIE_SCT_OP:

        result = setAddressesSctOp(direction, &sys_addr, a_addr, &ext_addr);

        (*p_xd_slots)->sys_lo_addr = sys_addr;
        (*p_xd_slots)->ext_lo_addr = ext_addr;

        break;
    case PCIE_GTH_OP:

        switch (direction) {
        case PCIE_OUTB_DIR:
            ext_addr = a_addr;
            sys_addr = 0;
            break;
        case PCIE_INB_DIR:
            sys_addr = a_addr;
            ext_addr = 0;
            break;
        case PCIE_UNDEFINED_DIR:
        default:
            result = 1;
            break;
        }

        (*ptr)->ext_lo_addr = ext_addr;
        (*ptr)->sys_lo_addr = sys_addr;

        break;
    case PCIE_UNDEFINED_OP:
    case PCIE_BLK_OP:
    default:
        result = 1;
        break;
    }

    return (result);
}
static void ResetStatusBytes(PCIE_xd_desc *const *p_xd_slots)
{
    (*p_xd_slots)->status.sys_status = 0;
    (*p_xd_slots)->status.ext_status = 0;
    (*p_xd_slots)->status.chnl_status = 0;
    (*p_xd_slots)->status.reserved_0 = 0;
    return;
}
static void initXdSlotsSctOp(
    PCIE_xd_desc *const *p_xd_slots,
    UINT32        size)
{
    /* use local variable for input data */
    (*p_xd_slots)->sys_hi_addr = 0;
    (*p_xd_slots)->sys_attr = 0;
    (*p_xd_slots)->ext_hi_addr = 0;
    (*p_xd_slots)->ext_attr = 0;

    (*p_xd_slots)->size_and_ctrl.size = size & 0xffffffU;

    /* prefetch descriptor */
    (*p_xd_slots)->size_and_ctrl.ctrl_bits.control_bits =
        SetUDMA_ControlBits(&((*p_xd_slots)->size_and_ctrl.ctrl_bits.control_bits), PCIE_PREFETCH, PCIE_FALSE, PCIE_TRUE);
    ResetStatusBytes(p_xd_slots);

    return;
}
static void setXdSlotsSctOp(
    PCIE_xd_desc *const *ptr,
    const PCIE_td_desc * p_this_td,
    UINT32             index)
{
    (*ptr)->sys_hi_addr =          0;
    (*ptr)->sys_attr    =          0;
    (*ptr)->ext_hi_addr =          0;
    (*ptr)->ext_attr    =          0;

    (*ptr)->size_and_ctrl.size = p_this_td->xfer.xn[index].n & 0xffffffU;

    (*ptr)->size_and_ctrl.ctrl_bits.control_bits =
        SetUDMA_ControlBits(&((*ptr)->size_and_ctrl.ctrl_bits.control_bits),
                            PCIE_POSTWRITE,
                            ((*ptr)->next > 0U) ? PCIE_FALSE : PCIE_TRUE,
                            ((*ptr)->next > 0U) ? PCIE_TRUE : PCIE_FALSE);
    return;
}
static PCIE_xd_desc * convertXd(
    const PCIE_td_cfg *p_td_cfg,
    const PCIE_xd_desc *pxd,
    UINT64 *          p_vp_xd)
{
    (void)convertToVirt(p_td_cfg, ConvertXdDescToUintptr(pxd), p_vp_xd);
    PCIE_xd_desc *local_pxd = ConvertUintptrToXdDesc(*p_vp_xd);
    return local_pxd;
}
static UINT32 mapTdIntoXdsSctOp(
    PCIE_xd_desc *       ptr,
    const PCIE_td_cfg *p_td_cfg,
    const PCIE_td_desc *p_this_td,
    PCIE_xd_desc *const *p_xd_slots)
{
    UINT64 next_address;
    UINT32 ii = 0;
    UINT32 result;
    UINT64 virt_addr;
    PCIE_xd_desc* local_ptr = ptr;
    /* populate descriptor */

    /* calculate size */
    UINT32 size = calculateDescSize(p_this_td);

    result = populateNonIndexedDescriptor(p_this_td, p_xd_slots, &local_ptr);

    if (result == 0U) {

        initXdSlotsSctOp(p_xd_slots, size);

        /* postwrite descriptors */
        next_address = (*p_xd_slots)->next + ((UINT64)((*p_xd_slots)->next_hi_addr) << 32);
        local_ptr = ConvertUintptrToXdDesc((UINT64)next_address);
        while (local_ptr != NULL) {
            /* convert local_ptr to a virtual address */

            local_ptr = convertXd(p_td_cfg, local_ptr, &virt_addr);

            /* populate the descriptor */
            result = populateIndexedDescriptor(p_this_td, &local_ptr, ii);

            if (result == 1U) {
                break;
            }

            setXdSlotsSctOp(&local_ptr, p_this_td, ii);

            (*p_xd_slots)->status.sys_status = 0;
            (*p_xd_slots)->status.ext_status = 0;
            (*p_xd_slots)->status.chnl_status = 0;
            (*p_xd_slots)->status.reserved_0 = 0;

            ii++;
            next_address = local_ptr->next + ((UINT64)local_ptr->next_hi_addr << 32);
            local_ptr = ConvertUintptrToXdDesc((UINT64)next_address);
        }
    }
    return (result);
}
static void initXdSlotsGthOp(
    PCIE_xd_desc *const *ptr,
    const PCIE_td_desc * p_this_td,
    UINT32             index)
{
    (*ptr)->sys_hi_addr =          0;

    (*ptr)->sys_attr    =          0;
    (*ptr)->ext_hi_addr =          0;
    (*ptr)->ext_attr    =          0;

    (*ptr)->size_and_ctrl.size = p_this_td->xfer.xn[index].n & 0xffffffU;

    (*ptr)->size_and_ctrl.ctrl_bits.control_bits =
        SetUDMA_ControlBits(&((*ptr)->size_and_ctrl.ctrl_bits.control_bits),
                            PCIE_PREFETCH,
                            PCIE_FALSE,
                            PCIE_TRUE);
    (*ptr)->status.sys_status = 0;
    (*ptr)->status.ext_status = 0;
    (*ptr)->status.chnl_status = 0;
    (*ptr)->status.reserved_0 = 0;
    return;
}

static void setXdSlotsGthOp(
    PCIE_xd_desc *const *ptr,
    UINT32        size)
{
    (*ptr)->sys_hi_addr =       0;

    (*ptr)->sys_attr    =       0;
    (*ptr)->ext_hi_addr =       0;
    (*ptr)->ext_attr    =       0;

    (*ptr)->size_and_ctrl.size = size & 0xffffffU;
    (*ptr)->size_and_ctrl.ctrl_bits.control_bits =
        SetUDMA_ControlBits(&((*ptr)->size_and_ctrl.ctrl_bits.control_bits),
                            PCIE_POSTWRITE,
                            PCIE_TRUE,
                            PCIE_FALSE);

    (*ptr)->status.sys_status = 0;
    (*ptr)->status.ext_status = 0;
    (*ptr)->status.chnl_status = 0;
    (*ptr)->status.reserved_0 = 0;

    (*ptr)->next          = 0;
    (*ptr)->next_hi_addr  = 0;
    return;
}
static UINT32 mapTdIntoXdsGthOp(
    const PCIE_td_cfg *p_td_cfg,
    const PCIE_td_desc *p_this_td,
    PCIE_xd_desc *const *p_xd_slots)
{
    UINT64 next_address;

    /* prefetch descriptors */
    UINT32 ii = 0;
    UINT32 result = 0;
    PCIE_xd_desc *ptr = *p_xd_slots;
    UINT64 virt_addr;
    while (ptr->next > 0U) {

        /* populate descriptor */
        result = populateIndexedDescriptor(p_this_td, &ptr, ii);

        if (result == 1U) {
            break;
        }

        initXdSlotsGthOp(&ptr, p_this_td, ii);

        /* copy descriptor into sram */

        ii++;
        next_address = ptr->next + ((UINT64)ptr->next_hi_addr << 32);
        ptr = ConvertUintptrToXdDesc((UINT64)next_address);

        /* first list pointer is always a virtual address, the
         * others need to be converted
         */
        ptr = convertXd(p_td_cfg, ptr, &virt_addr);
    }

    if (result == 0U) {
        /* postwrite descriptor */

        /* calculate size */
        UINT32 size = calculateDescSize(p_this_td);

        /* populate descriptor */
        result = populateNonIndexedDescriptor(p_this_td, p_xd_slots, &ptr);

        if (result == 0U) {
            setXdSlotsGthOp(&ptr, size);
        }
    }
    return (result);
}

static void convertPhiAddrToVirtualPtr(
    UINT32        cnt,
    PCIE_td_cfg *   p_td_cfg,
    PCIE_xd_desc ** ptr,
    PCIE_td_dar **  p_dar)
{
    UINT64 virt_addr;
    /* use local variables to avoid 3rd level pointers */
    PCIE_xd_desc * local_ptr = *ptr;
    PCIE_td_dar * local_p_dar = *p_dar;

    if (cnt == 0U) {
        (void)getDarPtrFromXdPtr(p_td_cfg,
                                 local_ptr,
                                 &local_p_dar);
    } else {
        (void)convertToVirt(p_td_cfg, ConvertXdDescToUintptr(local_ptr), &virt_addr);

        (void)getDarPtrFromXdPtr(p_td_cfg,
                                 ConvertUintptrToXdDesc(virt_addr),
                                 &local_p_dar);

        local_ptr = ConvertUintptrToXdDesc(virt_addr);
    }
    /* assign local vars back to formal parameters */
    *ptr = local_ptr;
    *p_dar = local_p_dar;
    return;
}

static UINT32
mapTdIntoXds(PCIE_td_cfg *  p_td_cfg,
             PCIE_td_desc * p_this_td,
             PCIE_xd_desc * p_xd_slots)
{
    UINT32 cnt = 0;
    PCIE_xd_desc *ptr   = p_xd_slots;
    PCIE_td_dar *p_dar = NULL;
    UINT32 result;
    PCIE_td_op_type transfer_type = p_this_td->op_typ;

    UINT64 next_address;

    result = 0;

    while (ptr != NULL) {

        /* The head pointer is a virtual address, every next
         * pointer will be a physical address that needs to
         * be converted back into a virtual pointer
         */
        convertPhiAddrToVirtualPtr(cnt, p_td_cfg, &ptr, &p_dar);

        cnt++;
        if (p_dar != NULL) {
            p_dar->num_slots_used++;
        }

        next_address = ptr->next + ((UINT64)ptr->next_hi_addr << 32);
        ptr = ConvertUintptrToXdDesc((UINT64)next_address);
    }

    /*
     * The xd-list pointed to by 'p_xd_slots' now has 'cnt' number
     * of nodes reserved for accomodating xds
     */

    if (transfer_type == PCIE_BLK_OP) {
        result = configXdSlotsBlkOp(&p_xd_slots, p_this_td);

    } else if (transfer_type == PCIE_SCT_OP)   {
        result = mapTdIntoXdsSctOp(ptr, p_td_cfg, p_this_td, &p_xd_slots);

    } else if (transfer_type == PCIE_GTH_OP)   {
        result = mapTdIntoXdsGthOp(p_td_cfg, p_this_td, &p_xd_slots);

    } else {
        /* Every else if must end with else - MISRA2012-RULE-15_7-3 */
        result = 1;
    }
    if (result == 0U) {
        /* Update the TD with the first node of the XD list */
        p_this_td->drv_reserved = &(p_xd_slots->sys_lo_addr);
        p_this_td->ref_count    = cnt;
    }

    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
assignNewHeadDarToChannel(PCIE_td_cfg * p_td_cfg,
                          UINT32      darIdx,
                          UINT32      channel)
{
    UINT32 result;
    /* return failure if the dar still has slots used */

    if (p_td_cfg->dars[darIdx].num_slots_used > 0U) {
        result = 1;
    } else {

        p_td_cfg->channels[channel].headDarIdx     = darIdx;
        p_td_cfg->channels[channel].state          = PCIE_CHANNEL_BUSY;
        p_td_cfg->dars[darIdx].state          = PCIE_DAR_ATTACHED;
        p_td_cfg->dars[darIdx].next_free_slot =  0;
        p_td_cfg->dars[darIdx].next           = 0xFFFFFFFFU;
        result = 0;
    }

    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
clearNewHeadDarFromChannel(PCIE_td_cfg * p_td_cfg,
                           UINT32      channel)
{
    UINT32 darIdx = p_td_cfg->channels[channel].headDarIdx;
    UINT32 result;
    /* return failure if not new */

    if ((p_td_cfg->dars[darIdx].next_free_slot > 0U) ||
        (p_td_cfg->dars[darIdx].num_slots_used > 0U) ||
        (p_td_cfg->dars[darIdx].next  !=  0xFFFFFFFFU)) {
        result = 1;
    } else {
        result = 0;
        p_td_cfg->channels[channel].headDarIdx     = 0xFFFFFFFFU;
        p_td_cfg->channels[channel].state          = PCIE_CHANNEL_FREE;
        p_td_cfg->dars[darIdx].state          = PCIE_DAR_FREE;
        p_td_cfg->dars[darIdx].next_free_slot =  0;
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
releaseChannel(PCIE_td_cfg * p_td_cfg,
               UINT32      channel)
{
    p_td_cfg->channels[channel].headDarIdx = 0xFFFFFFFFU;
    p_td_cfg->channels[channel].state      = PCIE_CHANNEL_FREE;

    return (0);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32 allocateAvailableDar(
    PCIE_td_cfg *const *p_td_cfg,
    PCIE_td_dar **   p_dar,
    PCIE_td_dar **   ret_pdar,
    PCIE_td_dar *    ptr,
    const UINT32 * p_idx2)
{
    UINT32 result = 0U;

    if (*p_idx2 != 0U) {

        ptr->next = *p_idx2; /* link in the alloted dar */
        (*p_td_cfg)->dars[*p_idx2].next_free_slot =  0;
        (*p_td_cfg)->dars[*p_idx2].state          = PCIE_DAR_ATTACHED;
        (*p_td_cfg)->dars[*p_idx2].next           = 0xFFFFFFFFU;

        *p_dar = &((*p_td_cfg)->dars[*p_idx2]);

        *ret_pdar = *p_dar;

    } else {
        result = 1U;
    }
    return result;
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32 allocateNewDar(
    const UINT32 * p_idx,
    PCIE_td_cfg *const *p_td_cfg,
    PCIE_td_dar **   p_dar,
    PCIE_td_dar **   ret_pdar)
{
    UINT32 idx2;
    UINT32 result = 0U;

    /* walk the dar list */
    PCIE_td_dar *ptr = &((*p_td_cfg)->dars[*p_idx]);

    while (ptr->next != 0xFFFFFFFFU) {
        ptr = &((*p_td_cfg)->dars[ptr->next]);
    }

    /* check if the last dar in the list is full */
    if (ptr->next_free_slot == PCIE_MAX_DESC_SLOTS) {
        /* we need to allocate a new dar and link it up */
        if (getFreeDar(*p_td_cfg, &idx2) > 0U) {
            /* no more available dars */
            result = 1;
        } else {
            result = allocateAvailableDar(p_td_cfg, p_dar, ret_pdar, ptr, &idx2);
        }
    } else {
        *ret_pdar = ptr;
    }
    return (result);
}
static UINT32
getLastDarPtrForChannel(PCIE_td_cfg *  p_td_cfg,
                        UINT32       channel,
                        PCIE_td_dar ** ret_pdar,
                        UINT32 *     new_dar_assigned)
{
    UINT32 idx;
    PCIE_td_dar *p_dar;
    UINT32 result;
    *new_dar_assigned = 0;

    result = 0;
    if (getDarListIdxForChannel(p_td_cfg, channel, &idx) > 0U) {
        /* no head dar for this channel, but we need to que
         * a command, so lets allocate a new dar for this
         * channel
         */

        /* allocate a free dar to this channel */
        if (getFreeDar(p_td_cfg, &idx) > 0U) {
            /* no more available dars, so cannot queue
             * this command, so return failure, the td will
             * be retried when memory is available eventually
             */
            *ret_pdar = NULL;

            result = 1;
        } else {

            (void)assignNewHeadDarToChannel(p_td_cfg, idx, channel);

            *new_dar_assigned = 1;

            p_dar = &(p_td_cfg->dars[idx]);
            *ret_pdar = p_dar;
        }
    } else {

        /* Use temporary variable to avoid 3-level pointer */
        PCIE_td_dar * tmp_pdar = NULL;

        result = allocateNewDar(&idx, &p_td_cfg, &p_dar, &tmp_pdar);

        if (result == 0U) {
            *ret_pdar = tmp_pdar;
        }

    }
    return (result);
}

/****************************************************************************/
/* perform td to xd mapping & store in dar                                  */
/****************************************************************************/
static UINT32
tryToQueTd(PCIE_td_cfg *  p_td_cfg,
           PCIE_td_desc * p_this_td,
           UINT32       channel)
{
    PCIE_td_dar   *p_dar;
    PCIE_xd_desc *p_xd_slots = NULL;
    PCIE_td_dar   *p_ex_dar;                            /* extended dar list */
    UINT32 num_slots = 0;
    UINT32 new_dar_assigned_as_chnl_head = 0;
    UINT32 result;
    result = 0;
    if (p_this_td->xfer.num_x_vals == 0U) {
        /* we cannot have a Td with no x values */
        result = 1;
    } else if (p_td_cfg->channels[channel].hwState == PCIE_CHANNEL_BUSY) {
        result = 1;
    } else {

        /* calculate number of xds required for this td */
        switch (p_this_td->op_typ) {
        case PCIE_BLK_OP:
            num_slots = p_this_td->xfer.num_x_vals;
            break;
        case PCIE_SCT_OP:
        case PCIE_GTH_OP:
            num_slots = p_this_td->xfer.num_x_vals + 1U;
            break;
        case PCIE_UNDEFINED_OP:
        default:         /* unknown op-type */
            result = 1;
            break;
        }
        if (result == 0U) {
            /* get the last dar pointer for the dar-list associated
             * with this channel
             */
            if (getLastDarPtrForChannel(p_td_cfg,
                                        channel,
                                        &p_dar,
                                        &new_dar_assigned_as_chnl_head) > 0U) {
                /* The channel with index 'channel' has no head dar & there
                 * are no resources to allocate one the td
                 */

                result = 1;
            }
            /* get a list of empty linked slots for this td */
            else if (getListOfEmptyLinkedSlots(p_td_cfg,
                                               p_dar,
                                               num_slots,
                                               &p_xd_slots,
                                               &p_ex_dar) > 0U) {
                /* not enough resources to accomodate this td */

                /* free head dar for the channel if (a new) one was
                 * assigned just for this td
                 */
                if (new_dar_assigned_as_chnl_head > 0U) {
                    (void)clearNewHeadDarFromChannel(p_td_cfg, channel);
                }

                result = 1;
            } else {
                (void)mapTdIntoXds(p_td_cfg,
                                   p_this_td,
                                   p_xd_slots);

                /* we are done mapping the td into xds */
            }
        }
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32 validateTdDesc(const PCIE_td_desc * ptr)
{
    UINT32 result = 0U;
    if (ptr->drv_reserved != NULL) {
        /* td cannot be busy in hardware or incomplete
         * without having a link to its corresponding
         * xd(s)
         */
        if ((ptr->state != PCIE_TD_BUSY_IN_HW) &&
            (ptr->state != PCIE_TD_COMPLETE) &&
            (ptr->state != PCIE_TD_FAILURE)) {
            result = 1;
        }
    }
    return (result);
}

static void freeDarWithUnusedSlots(
    PCIE_td_dar * p_dar,
    PCIE_td_cfg * p_td_cfg)
{

    if ( (p_dar != NULL) && (p_dar->num_slots_used == 0U) ) {
        /* make this dar ready to be freed */

        p_dar->next_free_slot = 0;

        /* if the DAR is the head of a channel, the
         * channel must free the DAR
         */
        (void)unbindDarHeadFromChannel(p_td_cfg, p_dar);
    } else {
        /* we cannot free this dar since we still
         * have slots in the dar that are in use
         */
    }

    return;
}
static UINT32 unhookXdList(
    const PCIE_xd_desc *     pxd,
    PCIE_td_desc *           ptr,
    PCIE_td_cfg *            p_td_cfg,
    UINT32 *               p_td_ptr_to_xd_updated,
    const PCIE_td_desc_state tmp)
{
    UINT32 result = 0U;
    /* we should be at the end of the list
     * for the ref count to go to 0
     */
    if (pxd->next > 0U) {
        /* there is something very wrong if we aren't at the
         * end of the list and the ref count has gone to 0
         */
        result = 1;
    } else {
        /* unhook the xd list from the td, we are getting to
         * being close to done with this td
         */
        ptr->drv_reserved = NULL;

        /* free the dars that were emptied as a result of clearing
         * out this xd-list from the TD.
         */
        (void)freeUnusedDars(p_td_cfg);

        /* release the channel that this td was hooked up to
         */
        (void)releaseChannel(p_td_cfg, ptr->channel);

        /* now mark the TD as complete or failed, because we indeed,
         * are finally done with it
         */
        ptr->state = tmp;

        *p_td_ptr_to_xd_updated = 0;
    }
    return (result);
}

static UINT32 freeXdList(
    const PCIE_xd_desc * pxd,
    PCIE_td_desc *       ptr,
    PCIE_td_cfg *        p_td_cfg,
    UINT32 *           p_td_ptr_to_xd_updated)
{
    UINT32 result = 0U;

    UINT64 vp_next_xd;

    /* ref count has not gone to zero, but we have a node
     * that has been freed. we would lose the head of the list
     * if we do not readjust the pointer to the list of xds
     * as pointed to by the td
     */

    if (pxd->next == 0U) {
        /* There is something drastically wrong if the
         * ref count has not gone to 0 but we are already
         * at the end of the list
         */
        result = 1;
    } else {
        UINT64 vp_xd;
        (void)convertToVirt(p_td_cfg, ConvertXdDescToUintptr(pxd), &vp_xd);

        if ((ptr->drv_reserved == ConvertUintptrToUint32ptr(vp_xd)) ||
            (ptr->drv_reserved == &(pxd->sys_lo_addr))) {
            (void)convertToVirt(p_td_cfg, (UINT64)pxd->next, &vp_next_xd);

            ptr->drv_reserved = ConvertUintptrToUint32ptr(vp_next_xd);

            *p_td_ptr_to_xd_updated = 1;
        }

        /* free the dars that were emptied as a result of clearing
         * out this xd
         */
        (void)freeUnusedDars(p_td_cfg);
    }

    return (result);
}
static void switchAddressToVirtual(
    UINT32 *           cnt,
    UINT32             td_ptr_to_xd_updated,
    PCIE_xd_desc **      pxd,
    const PCIE_td_desc * ptr)
{
    UINT64 next_address;
    if (td_ptr_to_xd_updated > 0U) {
        *pxd = PCIE_UtilityVoidP2XdDescP(ptr->drv_reserved);
        *cnt = 0; /* reset cnt because we are back to a virtual address */
    } else {
        next_address = (*pxd)->next + ((UINT64)(*pxd)->next_hi_addr << 32U);
        *pxd = ConvertUintptrToXdDesc((UINT64)next_address);
    }
    return;
}

static void processDars(
    PCIE_td_cfg *const *p_td_cfg,
    PCIE_xd_desc **pxd,
    PCIE_td_dar **p_dar,
    PCIE_td_desc *const *ptr)
{
    /* use local variables to avoid 3rd level pointers */
    PCIE_xd_desc * local_pxd = *pxd;
    PCIE_td_dar *  local_p_dar = *p_dar;

    (void)getDarPtrFromXdPtr(*p_td_cfg, local_pxd, &local_p_dar);

    /* adjust the reference count */
    (*ptr)->ref_count--;

    /* if no more slots in the dar are being used, we can
     * go ahead and free the dar
     */
    freeDarWithUnusedSlots(local_p_dar, *p_td_cfg);

    /* assign local variables to formal parameters */
    *pxd = local_pxd;
    *p_dar = local_p_dar;
    return;
}
static UINT32 CheckUpdatedStatusBytes(const PCIE_xd_desc * pxd)
{
    UINT32 result = 1U;
    if ((pxd->status.sys_status > 0U)  ||
        (pxd->status.ext_status > 0U)  ||
        (pxd->status.chnl_status > 0U)) {
        result = 0U;
    }
    return (result);
}

static UINT32 CheckSuccessStatusBytes(const PCIE_xd_desc * pxd)
{
    UINT32 result = 0U;
    /* If status is different then 0x00_01_00_00, return error */
    if ((pxd->status.sys_status != 0U)  ||
        (pxd->status.ext_status != 0U)  ||
        (pxd->status.chnl_status != 1U)) {
        result = 1U;
    }
    return (result);
}

static UINT32 mapXdtoTd(
    PCIE_xd_desc **  pxd,
    PCIE_td_desc *   ptr,
    PCIE_td_cfg *    p_td_cfg,
    const UINT32 * p_cnt)
{
    UINT32 cnt = *p_cnt;
    PCIE_xd_desc * local_pxd = *pxd;
    UINT32 result = 0;
    UINT64 vp_xd = 0;
    UINT32 td_ptr_to_xd_updated = 0;

    PCIE_td_dar *p_dar = NULL;

    /* initially, we assume the call succeeded. we will
     * update the state if and when the call fails
     */
    static PCIE_td_desc_state tmp;

    if (cnt == 0U) {
        tmp = PCIE_TD_COMPLETE;
    }

    /* pxd is a physical address, except for the address pointed
     * to by drv_reserved. convert the physical to a virtual
     * address */

    if (cnt > 0U) {
        local_pxd = convertXd(p_td_cfg, local_pxd, &vp_xd);
    }

    cnt++;

    if (local_pxd != NULL) {
        if ((CheckUpdatedStatusBytes(local_pxd) == 0U) || /* status has been updated (success:0x10000)              */
            (tmp  == PCIE_TD_FAILURE)) {    /* an xd in the xd-list has failed -- we lost the command */
            /* if the td is already marked as an error, leave it
             * as is even if all the remaining xds pass fine
             */
            if (CheckSuccessStatusBytes(local_pxd) != 0U) {
                /* our initial assumption failed */
                tmp = PCIE_TD_FAILURE;
            }

            /* Reset the status field to zero, so we don't do
             * this again next time we run the list
             */
            local_pxd->status.sys_status = 0;
            local_pxd->status.ext_status = 0;
            local_pxd->status.chnl_status = 0;
            local_pxd->status.reserved_0 = 0;

            /* walk the list and decrement the used-slot count
             * for the dars that the xd belongs to. Note that we
             * simply need to decrement the count until the used-count
             * in the dar goes to 0 at which point the whole dar is
             * freed. (i.e., we do not fill in "holes" in dars with
             * new commands. dars are filled incrementally and
             * deleted en-block - i.e., no special per slot
             * cleanup is required)
             */
            processDars(&p_td_cfg, &local_pxd, &p_dar, &ptr);

            if (p_dar != NULL) {
                p_dar->num_slots_used--;
            }
            /* if ref-count goes to zero, free the xd_list and unhook
             * the xd-list from the td
             */
            if (ptr->ref_count == 0U) {
                result = unhookXdList(local_pxd, ptr, p_td_cfg, &td_ptr_to_xd_updated, tmp);
            } else {
                result = freeXdList(local_pxd, ptr, p_td_cfg, &td_ptr_to_xd_updated);
            }

        } else {
            /* Every else if must end with else - MISRA2012-RULE-15_7-3 */
        }
        if (result == 0U) {
            switchAddressToVirtual(&cnt, td_ptr_to_xd_updated, &local_pxd, ptr);
        }
    }
    *pxd = local_pxd;

    return (result);
}

static UINT32
mapXdStatusBackIntoTd(PCIE_td_cfg * p_td_cfg, PCIE_td_desc * ptr)
{
    UINT32 cnt = 0U;
    UINT32 result = validateTdDesc(ptr);

    if (result == 0U) {
        /* walk the xd list for this td */
        PCIE_xd_desc *pxd = PCIE_UtilityVoidP2XdDescP(ptr->drv_reserved);
        while (pxd != NULL) {
            result = mapXdtoTd(&pxd, ptr,p_td_cfg, &cnt);
        }
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/***************************************************************************/
static UINT32 updateTdState(
    PCIE_td_desc * ptr,
    PCIE_td_cfg *  p_td_cfg)
{
    UINT32 result = 0U;
    switch (ptr->state) {
    case PCIE_TD_UNDEFINED:
    case PCIE_TD_FREE:
        result = 1;
        break;
    case PCIE_TD_STATUS_PEND:
        break;
    case PCIE_TD_BUSY_WAITING:
        break;

    case PCIE_TD_BUSY_IN_HW:
        /* the td is busy in hw, check for
         * partial completion and update the
         * td appropriately
         */
        (void)mapXdStatusBackIntoTd(p_td_cfg, ptr);
        break;
    case PCIE_TD_COMPLETE:
        /* the td completed successfully, update
         * the td state appropriately
         */
        (void)mapXdStatusBackIntoTd(p_td_cfg, ptr);
        break;
    case PCIE_TD_FAILURE:
        /* the td failed, update the td state
         * appropriately
         */
        (void)mapXdStatusBackIntoTd(p_td_cfg, ptr);
        break;

    default:
        result = 1;
        break;
    }
    return (result);
}

static inline void enableUDMAInterrupts(
    const PCIE_PrivateData * pD,
    const PCIE_td_desc * ptr)
{
    (void)PCIE_UDMA_ControlErrInterrupts(pD, ptr->channel, PCIE_ENABLE_PARAM);
    (void)PCIE_UDMA_ControlDoneInterrupts(pD, ptr->channel, PCIE_ENABLE_PARAM);
    return;
}
static void startUdmaTransfer(
    const PCIE_PrivateData * pD,
    PCIE_td_desc *           ptr,
    const PCIE_ListAttribute *pListAttr,
    PCIE_td_cfg *            p_td_cfg)
{
    UINT64 phys_addr;

    /* we found dar space to get this td in */

    /* Hit channel Go */

    ptr->state = PCIE_TD_BUSY_IN_HW;

    (void)convertToPhys(p_td_cfg,
                        ConvertUint32ptrToUintptr(ptr->drv_reserved),
                        &phys_addr);

    enableUDMAInterrupts(pD, ptr);

    /* we can program the channel to do the transfer from the
     * descriptor list pointed to by 'phys_addr'
     */

    p_td_cfg->channels[ptr->channel].hwState = PCIE_CHANNEL_BUSY;
    (void)PCIE_UDMA_DoTransfer(pD,
                               ptr->channel,
                               (ptr->direction == PCIE_OUTB_DIR) ?  PCIE_TRUE : PCIE_FALSE,
                               ConvertUintptrToUint32ptr(phys_addr),
                               pListAttr);
    return;
}
UINT32 td_cfg_processTdsInAltd(const PCIE_PrivateData * pD)
{
    PCIE_td_desc *ptr;
    UINT32 result;
    PCIE_ListAttribute listAttributes = {.lower = 0U, .upper = 0U };
    const PCIE_ListAttribute* pListAttr = &listAttributes;
    result = 0U;
    PCIE_td_cfg *p_td_cfg = pD->p_td_cfg;

    if (validateTdCfg(p_td_cfg) != 0U) {
        result = 1U;
    } else {
        ptr = p_td_cfg->altd_head;

        while (ptr != NULL) {
            /* ignore if already queued or is completed */

            if ((ptr->state == PCIE_TD_COMPLETE) ||
                (ptr->state == PCIE_TD_FAILURE) ||
                (ptr->state == PCIE_TD_BUSY_IN_HW)) {
            } else {
                /* let us try to queue this td */

                if (tryToQueTd(p_td_cfg, ptr, ptr->channel) == 0U) {
                    startUdmaTransfer(pD, ptr, pListAttr, p_td_cfg);

                } else {
                    /* td is back to waiting to be queued */

                    /* if the tryToQueTd command failed to queue the
                     * TD, it means that it didn't have enough
                     * space. So we ignore this TD for now, but
                     * will get back to it eventually */

                    ptr->state = PCIE_TD_BUSY_WAITING;
                }
            }
            ptr = ptr->next;
        }

        ptr = p_td_cfg->altd_head;

        while (ptr != NULL) {
            result = updateTdState(ptr, p_td_cfg);
            if (result == 1U) {
                break;
            }
            ptr = ptr->next;
        }
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
areAllDarsFree(const PCIE_td_cfg * p_td_cfg)
{
    UINT32 ii, cnt = 0, ret;

    for (ii = 0; ii < PCIE_MAX_DARS; ii++) {
        if (p_td_cfg->dars[ii].state != PCIE_DAR_FREE) {
            cnt++;
        }
    }

    if (cnt > 0U) {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
areAllChnlsFree(const PCIE_td_cfg * p_td_cfg)
{
    UINT32 ii, cnt = 0, ret;

    for (ii = 0; ii < PCIE_NUM_UDMA_CHANNELS; ii++) {
        if (p_td_cfg->channels[ii].state != PCIE_CHANNEL_FREE) {
            cnt++;
        }
    }

    if (cnt > 0U) {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}

static UINT32 checkLtdOperationType(const PCIE_td_desc * p_ltd)
{
    UINT32 transferSize;
    UINT32 result = 0;
    UINT32 ii;
    switch (p_ltd->op_typ) {
    case PCIE_BLK_OP:
        if ( (p_ltd->xfer.num_x_vals != 1U) || (p_ltd->xfer.xn[0].n > 0x1000000U) ) {
            result = 1U;
        }
        break;
    case PCIE_GTH_OP:
    case PCIE_SCT_OP:
        transferSize = 0U;
        for (ii = 0; ii < p_ltd->xfer.num_x_vals; ii++) {
            transferSize += p_ltd->xfer.xn[ii].n;
        }
        if (transferSize > PCIE_UDMA_BUFFER_DEPTH) {
            result = 1U;
        }
        break;
    case PCIE_UNDEFINED_OP:
    default:
        result = 1U;
        break;
    }
    return (result);
}

static UINT32 checkLtdDirection(const PCIE_td_desc * p_ltd)
{
    UINT32 result = 0;
    switch (p_ltd->direction) {
    case PCIE_INB_DIR:
    case PCIE_OUTB_DIR:
        break;
    case PCIE_UNDEFINED_DIR:
    default:
        result = 1;
        break;
    }
    return (result);
}

static UINT32 checkLtdState(const PCIE_td_desc * p_ltd)
{
    UINT32 result = 0;
    switch (p_ltd->state) {
    case PCIE_TD_UNDEFINED:
        break;
    case PCIE_TD_FREE:
    case PCIE_TD_STATUS_PEND:
    case PCIE_TD_BUSY_WAITING:
    case PCIE_TD_BUSY_IN_HW:
    case PCIE_TD_COMPLETE:
    case PCIE_TD_FAILURE:
    default:
        result = 1;
        break;
    }
    return (result);
}

static UINT32 checkLtdAlignment(const PCIE_td_desc * p_ltd)
{
    UINT32 result = 0;
    if (p_ltd->channel >= PCIE_NUM_UDMA_CHANNELS) {
        result = 1;
    }

    else {
        UINT32 ii;
        /* All transfers must be 32bit aligned, length of transfer must be multiple of 4 bytes */
        if ( (p_ltd->xfer.a & 0x3U) > 0U ) {
            result = 1;
        } else {
            for (ii = 0; ii < p_ltd->xfer.num_x_vals; ii++) {
                if ( ((p_ltd->xfer.xn[ii].x & 0x3U) > 0U) ||
                     ((p_ltd->xfer.xn[ii].n & 0x3U) > 0U) ) {
                    result = 1;
                    /* break from for loop */
                    break;
                }
            }
        }
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32 checkLtdValid(const PCIE_td_desc *p_ltd)
{
    UINT32 result;
    /* Use local data for p_ltd to avoid:
     * A function parameter should not be modified (MISRA2012-RULE-17_8-4)
     * while incrementing pointer within loop */
    const PCIE_td_desc * p_ltd_local = p_ltd;
    result = 0U;

    if (p_ltd_local == NULL) {
        result = 1;
    } else {
        while (p_ltd_local != NULL) {
            if (p_ltd_local->xfer.num_x_vals == 0U) {
                result = 1U;
            } else {
                /* Validate Ltd parameters*/
                result |= checkLtdOperationType(p_ltd_local);
                result |= checkLtdDirection(p_ltd_local);
                result |= checkLtdState(p_ltd_local);
                if (result == 0U) {
                    result = checkLtdAlignment(p_ltd_local);
                }
                if (result == 0U) {
                    p_ltd_local = p_ltd_local->next;
                }
            }
            if (result != 0U) {
                break;
            }
        }
    }
    return (result);
}

/****************************************************************************/
/* Request the driver for an attach of an ltd                               */
/****************************************************************************/
UINT32
PCIE_UDMA_RequestLtdAttach(const PCIE_PrivateData * pD,
                           PCIE_td_desc *           p_ltd)
{
    PCIE_td_cfg *p_td_cfg;
    PCIE_td_desc *p_ltd_head;
    UINT32 result;
    result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        p_td_cfg   = pD->p_td_cfg;
        p_ltd_head = p_ltd;

        if ( (p_td_cfg == NULL) || (p_ltd_head == NULL) || (p_td_cfg->altd_head != NULL) ) {
            result = PCIE_ERR_ARG;
        } else {

            /* Check whether any completions to handle */
            (void)PCIE_UDMA_Isr(pD);

            if (areAllDarsFree(p_td_cfg) > 0U) {
                result = 1;
            } else if (areAllChnlsFree(p_td_cfg) > 0U) {
                result = 1;
            }
            /* Check that the LTD has valid content */
            else if (checkLtdValid(p_ltd_head) > 0U) {
                result = 1;
            } else {
                p_td_cfg->altd_head = p_ltd_head; /* link the ltd */
                result = td_cfg_processTdsInAltd(pD);
            }
        }

    }
    /* This should trigger the DMA transfer if the channel is not busy */
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
UINT32
PCIE_UDMA_RequestLtdExtend(const PCIE_PrivateData * pD,
                           PCIE_td_desc *           p_ltd)
{
    UINT32 result;
    const PCIE_td_cfg *p_td_cfg;
    PCIE_td_desc *p_exl_head;
    PCIE_td_desc *ptr;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {

        p_td_cfg   = pD->p_td_cfg;
        p_exl_head = p_ltd;

        if (p_td_cfg == NULL) {
            result = PCIE_ERR_ARG;
        } else if (p_exl_head == NULL) {
            result = PCIE_ERR_ARG;
        } else if (p_td_cfg->altd_head == NULL) {
            result = PCIE_ERR_ARG;
        } else {

            ptr = p_td_cfg->altd_head;

            /* Check that the LTD for valid content */
            if (checkLtdValid(p_exl_head) > 0U) {
                result = PCIE_ERR_ARG;
            } else {

                while (ptr->next != NULL) {
                    ptr = ptr->next;
                }

                ptr->next = p_exl_head;
                result = td_cfg_processTdsInAltd(pD);
            }
        }
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
UINT32
PCIE_UDMA_ClearState(const PCIE_PrivateData * pD)
{
    PCIE_td_cfg *pcieAddr;
    UINT32 result;
    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {

        /* Clear all descriptor memory */

        pcieAddr = pD->p_td_cfg;

        if (pcieAddr == NULL) {
            result = PCIE_ERR_ARG;
        } else {
            (void)clearDescMem(pcieAddr);
            result = PCIE_ERR_SUCCESS;
        }
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
UINT32
PCIE_UDMA_RequestLtdDetach(const PCIE_PrivateData * pD)
{
    PCIE_td_cfg *p_td_cfg;
    UINT32 result;
    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {

        p_td_cfg = pD->p_td_cfg;

        if (p_td_cfg == NULL) {
            result = PCIE_ERR_ARG;
        } else {
            /* If there is no ALTD to detach, then return
             * an error
             */
            if (p_td_cfg->altd_head == NULL) {
                result = PCIE_ERR_ARG;
            } else {

                /* Clear out the ALTD from the driver
                 */
                p_td_cfg->altd_head = NULL;

                /* clear out descriptor memory
                 */
                (void)clearDescMem(p_td_cfg);
                result = PCIE_ERR_SUCCESS;
            }
        }
    }
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32
td_cfg_areAllTDsInAltdProcessed(const PCIE_td_cfg * p_td_cfg)
{
    UINT32 cnt_pending = 0;
    const PCIE_td_desc *ptr;
    UINT32 result;

    ptr = p_td_cfg->altd_head;
    while (ptr != NULL) {
        if ((ptr->state == PCIE_TD_COMPLETE) ||
            (ptr->state == PCIE_TD_FAILURE)) {
        } else {
            cnt_pending++;
        }
        ptr = ptr->next;
    }

    result = (cnt_pending > 0U) ? PCIE_ERR_INPROGRESS : PCIE_ERR_SUCCESS;
    return (result);
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
UINT32
PCIE_UDMA_CheckLtdDetach(const PCIE_PrivateData * pD)
{
    const PCIE_td_cfg *p_td_cfg;
    UINT32 result;
    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {

        p_td_cfg = pD->p_td_cfg;

        if ( (p_td_cfg == NULL) || (p_td_cfg->altd_head == NULL) ) {
            result = PCIE_ERR_ARG;
        } else {
            result = td_cfg_areAllTDsInAltdProcessed(p_td_cfg);
        }
    }
    return (result);
}

/* parasoft-end-suppress METRICS-41-3 */
/* parasoft-end-suppress METRICS-36-3 */




/****************************************************************************/
/* Start UDMA transfer                                                      */
/****************************************************************************/
static void SetUdmaChannelAddrDrn(
    UINT32                    channel,
    struct PCIE_IClientUdma_s * chanregAddr,
    UINT32                    direction)
{
    if (channel == 0U) {
        PCIE_RegPtrWrite32(&chanregAddr->dma_ch0.channel_ctrl, direction);
    }
    if (channel == 1U) {
        PCIE_RegPtrWrite32(&chanregAddr->dma_ch1.channel_ctrl, direction);
    }
    if (channel == 2U) {
        PCIE_RegPtrWrite32(&chanregAddr->dma_ch2.channel_ctrl, direction);
    }
    if (channel == 3U) {
        PCIE_RegPtrWrite32(&chanregAddr->dma_ch3.channel_ctrl, direction);
    }
    return;
}

static UINT32 WriteIsOutboundState(
    UINT32                    channel,
    PCIE_Bool                   isOutbound,
    struct PCIE_IClientUdma_s * chanregAddr)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    if (isOutbound == PCIE_TRUE) {
        SetUdmaChannelAddrDrn(channel, chanregAddr,3);
    } else {
        SetUdmaChannelAddrDrn(channel, chanregAddr,1);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 ValidateInput_UDMA_DoTransfer(
    const PCIE_PrivateData *   pD,
    const UINT32 *           pListAddr,
    const UINT32             channel,
    const PCIE_ListAttribute * pListAttr)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    if ( (pD == NULL) ||
         (pListAddr == NULL) ||
         (pListAttr == NULL) ||
         (channel >= (UINT32)PCIE_NUM_UDMA_CHANNELS) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CheckLtssmState(const PCIE_LtssmState ltssmState)
{
    UINT32 result;

    if ((ltssmState != PCIE_L0)  && (ltssmState != PCIE_RX_L0S_ENTRY) && (ltssmState != PCIE_RX_L0S_IDLE) &&
        (ltssmState != PCIE_RX_L0S_FTS)  && (ltssmState != PCIE_TX_L0S_ENTRY) &&
        (ltssmState != PCIE_TX_L0S_IDLE)  && (ltssmState != PCIE_TX_L0S_FTS) ) {
        result = 1U;
    } else {
        result = 0U;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static void SetUdmaChannel0Addr(
    struct PCIE_IClientUdma_s * chanregAddr,
    UINT64                    address)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch0.channel_sp_l), (UINT32)(address & 0xFFFFFFFFu));
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch0.channel_sp_u), (UINT32)((address >> 32) & 0xFFFFFFFFu));
    return;
}
static void SetUdmaChannel1Addr(
    struct PCIE_IClientUdma_s * chanregAddr,
    UINT64                    address)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch1.channel_sp_l), (UINT32)(address & 0xFFFFFFFFu));
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch1.channel_sp_u), (UINT32)((address >> 32) & 0xFFFFFFFFu));
    return;
}
static void SetUdmaChannel2Addr(
    struct PCIE_IClientUdma_s * chanregAddr,
    UINT64                    address)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch2.channel_sp_l), (UINT32)(address & 0xFFFFFFFFu));
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch2.channel_sp_u), (UINT32)((address >> 32) & 0xFFFFFFFFu));
    return;
}
static void SetUdmaChannel3Addr(
    struct PCIE_IClientUdma_s * chanregAddr,
    UINT64                    address)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch3.channel_sp_l), (UINT32)(address & 0xFFFFFFFFu));
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch3.channel_sp_u), (UINT32)((address >> 32) & 0xFFFFFFFFu));
    return;
}
static void SetUdmaChannelAddr(
    UINT32                    channel,
    struct PCIE_IClientUdma_s * chanregAddr,
    UINT64                    address)

{
    struct PCIE_IClientUdma_s * chAddr = chanregAddr;
    /* static const should not require memory allocation */
    static const SetUdmaChannelAddressFunctions SetUdmaChannelAddrFuncArray[PCIE_NUM_UDMA_CHANNELS] = {
        [0] = SetUdmaChannel0Addr,
        [1] = SetUdmaChannel1Addr,
        [2] = SetUdmaChannel2Addr,
        [3] = SetUdmaChannel3Addr,
    };

    SetUdmaChannelAddrFuncArray[channel](chAddr, address);

    return;
}
static void SetUdmaChannel0Attr(
    struct PCIE_IClientUdma_s * chanregAddr,
    const PCIE_ListAttribute *  pListAttr)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch0.channel_attr_l), pListAttr->lower);
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch0.channel_attr_u), pListAttr->upper);
    return;
}
static void SetUdmaChannel1Attr(
    struct PCIE_IClientUdma_s * chanregAddr,
    const PCIE_ListAttribute *  pListAttr)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch1.channel_attr_l), pListAttr->lower);
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch1.channel_attr_u), pListAttr->upper);
    return;
}
static void SetUdmaChannel2Attr(
    struct PCIE_IClientUdma_s * chanregAddr,
    const PCIE_ListAttribute *  pListAttr)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch2.channel_attr_l), pListAttr->lower);
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch2.channel_attr_u), pListAttr->upper);
    return;
}
static void SetUdmaChannel3Attr(
    struct PCIE_IClientUdma_s * chanregAddr,
    const PCIE_ListAttribute *  pListAttr)
{
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch3.channel_attr_l), pListAttr->lower);
    PCIE_RegPtrWrite32(&(chanregAddr->dma_ch3.channel_attr_u), pListAttr->upper);
    return;
}
static void SetUdmaChannelAttr(
    UINT32                    channel,
    struct PCIE_IClientUdma_s * chanregAddr,
    const PCIE_ListAttribute *  pListAttr)
{
    struct PCIE_IClientUdma_s * chAddr = chanregAddr;
    static const SetUdmaChannelAttributesFunctions SetUdmaChannelAttrFuncArray[PCIE_NUM_UDMA_CHANNELS] = {
        [0] = SetUdmaChannel0Attr,
        [1] = SetUdmaChannel1Attr,
        [2] = SetUdmaChannel2Attr,
        [3] = SetUdmaChannel3Attr,
    };

    SetUdmaChannelAttrFuncArray[channel](chAddr, pListAttr);
    return;
}
static UINT32 SetChannelRegsAddr(
    UINT32                    channel,
    struct PCIE_IClientUdma_s * chanregAddr,
    UINT64                    address,
    const PCIE_ListAttribute *  pListAttr,
    PCIE_Bool                   isOutbound)
{
    UINT32 result;

    SetUdmaChannelAddr(channel, chanregAddr, address);
    SetUdmaChannelAttr(channel, chanregAddr, pListAttr);

    result = WriteIsOutboundState(channel, isOutbound, chanregAddr);

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_UDMA_DoTransfer(
    const PCIE_PrivateData *   pD,
    UINT32                   channel,
    PCIE_Bool                  isOutbound,
    const UINT32 *           pListAddr,
    const PCIE_ListAttribute * pListAttr)
{
    struct PCIE_IClientUdma_s *chanregAddr;
    UINT64 address = (UINT64)(ConvertUint32ptrToUintptr(pListAddr));
    PCIE_LtssmState ltssmState;

    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if (ValidateInput_UDMA_DoTransfer(pD, pListAddr, channel, pListAttr) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        chanregAddr = pD->p_ud_base;
        /* Wait until L0 or L0S state */
        if (PCIE_GetLinkTrainingState(pD, &ltssmState) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            while ( CheckLtssmState(ltssmState) != 0U ) {
                if (PCIE_GetLinkTrainingState(pD, &ltssmState) != 0U) {
                    result = PCIE_ERR_ARG;
                    break;
                }
            }

            if (result == PCIE_ERR_SUCCESS) {
                result = SetChannelRegsAddr(channel, chanregAddr, address, pListAttr, isOutbound);

                if (result == PCIE_ERR_SUCCESS) {
                    /* Check for errors reported from PCIe IP */
                    result = CallErrorCallbackIfError(pD);
                }
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Prepare a UDMA descriptor                                                */
/****************************************************************************/
UINT32
PCIE_UDMA_PrepareDescriptor(
    const PCIE_PrivateData * pD,
    const UINT32 *         pDesc,
    UINT32 *               pDescAddr,
    UINT32                 count)
{
    UINT32 ii;
    const UINT32 *src = pDesc;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ((pD == NULL) ||
        (pDesc == NULL) ||
        (pDescAddr == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Use local data for pDescAddr to avoid
         *  A function parameter should not be modified (MISRA2012-RULE-17_8-4)
         * while incrementing pointer within 'for' loop */
        UINT32 * desc = pDescAddr;
        for (ii = 0; ii < (count * ((sizeof(PCIE_xd_desc)) / sizeof(UINT32))); ii++) {
            PCIE_RegPtrWrite32(desc, *src);

            desc++;
            src++;
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* Extend a bulk UDMA descriptor                                            */
/****************************************************************************/
static UINT8 CheckControlBits(const UINT8 * dst, UINT8 bitNum)
{
    UINT8 bit_status = 0;
    if (bitNum < 8U) {
        bit_status = (*dst >> bitNum) & 1U;
    }
    return (bit_status);
}
static UINT8 CheckControlBitsContinuity(const UINT8 *dst)
{
    /* shift value by one, and clear oll other fields to get first 2 bits */
    return ((*dst >> 1U) & 0x03U);
}
UINT32
PCIE_UDMA_ExtendBulkDescriptor(
    const PCIE_PrivateData * pD,
    const UINT32 *  pDesc,
    const UINT32 *  pDescAddr)
{
    UINT64 next_address;
    UINT32 result = PCIE_ERR_SUCCESS;

    PCIE_xd_desc *last;
    PCIE_xd_desc *pXd = ConvertUint32ptrToXdDesc(pDescAddr);

    if ( (pD == NULL) ||
         (pDesc == NULL) ||
         (pDescAddr == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Check that descriptor is for bulk transfer */
        if (CheckControlBitsContinuity(&(pXd->size_and_ctrl.ctrl_bits.control_bits)) != (UINT8)PCIE_READ_WRITE) {
            result = PCIE_ERR_ARG;
        } else {
            /* Find last descriptor in list */
            do {
                last = pXd;
                next_address = pXd->next + ((UINT64)pXd->next_hi_addr << 32);
                pXd = ConvertUintptrToXdDesc((UINT64)next_address);
            } while ( (pXd != NULL) && (CheckControlBits(&(pXd->size_and_ctrl.ctrl_bits.control_bits), 5U) == 1U) );

            /* Set the next pointer to the new descriptor */
            next_address = ConvertUint32ptrToUintptr(pDesc);
            PCIE_RegPtrWrite32(&last->next, (UINT32)(next_address & 0xFFFFFFFFu));
            PCIE_RegPtrWrite32(&last->next_hi_addr, (UINT32)(next_address >> 32));
            SetControlBit(&(last->size_and_ctrl.ctrl_bits.control_bits), 5U);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
