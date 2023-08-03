/*
 * $QNXLicenseC:
 * Copyright 2009, 2018, QNX Software Systems.
 * Copyright 2013, Adeneo Embedded.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <sys/siginfo.h>
#include "Amba_gdma.h"

#include "AmbaKAL.h"
#include "AmbaINT.h"

static AMBA_KAL_EVENT_FLAG_t AmbaGdmaEventFlags;

static void GDMA_TransferDoneISR(UINT32 IntID, UINT32 UserArg)
{
    (void) IntID;
    (void) UserArg;

    AmbaKAL_EventFlagSet(&AmbaGdmaEventFlags, 0x1U);
}

static void GdmaWaitComplete(UINT32 TimeOut)
{
    UINT32 ActualFlags = 0;

    do {
        AmbaKAL_EventFlagGet(&AmbaGdmaEventFlags,
                             0x1U,
                             KAL_FLAGS_WAIT_ANY,
                             KAL_FLAGS_CLEAR_AUTO,
                             &ActualFlags,
                             TimeOut);
    } while (AmbaRTSL_GdmaGetNumAvails() != GDMA_NUM_INSTANCE);
}

int main(int argc, char *argv[])
{
    int id;
    resmgr_connect_funcs_t connect_funcs;
    resmgr_io_funcs_t io_funcs;
    dispatch_t *dpp;
    resmgr_attr_t rattr;
    dispatch_context_t *ctp;
    iofunc_attr_t ioattr;
    UINT32 IrqId;
    AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType = INT_TRIG_RISING_EDGE,
        .IrqType = INT_TYPE_IRQ,
        .CpuTargets = 1U,
    };

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    IrqId = AMBA_INT_SPI_ID106_GDMA;
#else
    IrqId = AMBA_INT_SPI_ID102_GDMA;
#endif
    (void)AmbaINT_Disable(IrqId);
    (void)AmbaINT_Config(IrqId, &IntConfig, GDMA_TransferDoneISR, 0U);
    (void)AmbaINT_Enable(IrqId);

    (void) argc;
    /* Initialize the dispatch interface */
    dpp = dispatch_create();
    if (!dpp) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "Clock error: Failed to create dispatch interface\n");
        goto fail;
    }

    /* Initialize the resource manager attributes */
    memset(&rattr, 0, sizeof(rattr));

    /* Attach the device name */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.devctl = gdma_io_devctl;
    iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

    id = resmgr_attach(dpp, &rattr, "/dev/gdma", _FTYPE_ANY, 0,
                       &connect_funcs, &io_funcs, &ioattr);
    if (id == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
              "Clock error: Failed to attach pathname\n");
        goto fail;
    }

    /* Allocate a context structure */
    ctp = dispatch_context_alloc(dpp);

    if (0x0 != Amba_Gdma_Init()) {
        printf( "gdma dev init fail %s(%d)\n", __func__, __LINE__);
    }

    AmbaKAL_EventFlagCreate(&AmbaGdmaEventFlags, NULL);

    /* Run in the background */
    if (procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "%s:  procmgr_daemon",
              argv[0]);
        goto fail;
    }

    while (1) {
        if ((ctp = dispatch_block(ctp)) == NULL) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,
                  "Clock error: Block error\n");
            goto fail;
        }
        dispatch_handler(ctp);
    }

fail:

    return EXIT_SUCCESS;
}

int gdma_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int status, nbytes;
    amba_gdma_linear_t *pGdmaLinear;
    amba_gdma_block_t *pGdmaBlock;
    amba_gdma_wait_t *pGdmaWait;
    int err = EOK;
    UINT32 ActualFlags = 0;
    AMBA_GDMA_LINEAR_s GdmaLiear = {0};
    AMBA_GDMA_BLOCK_s GdmaBlock = {0};

    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    if (0x0 == (AmbaRTSL_GdmaGetNumAvails())) {
        AmbaKAL_EventFlagGet(&AmbaGdmaEventFlags,
                             0x1U,
                             KAL_FLAGS_WAIT_ANY,
                             AMBA_KAL_FLAGS_CLEAR_NONE,
                             &ActualFlags,
                             AMBA_KAL_WAIT_FOREVER);
    }

    nbytes = 0;
    switch (msg->i.dcmd) {
    case DCMD_GDMA_LINEAR:
        pGdmaLinear = _DEVCTL_DATA(msg->i);
        GdmaLiear.NumPixels = pGdmaLinear->NumPixels;
        GdmaLiear.pSrcImg = pGdmaLinear->pSrcImg;
        GdmaLiear.pDstImg = pGdmaLinear->pDstImg;
        GdmaLiear.PixelFormat = pGdmaLinear->PixelFormat;
        (void)AmbaRTSL_GdmaLinearCopy(&GdmaLiear);
        break;

    case DCMD_GDMA_BLOCK:
        pGdmaBlock = _DEVCTL_DATA(msg->i);
        GdmaBlock.pSrcImg = pGdmaBlock->pSrcImg;
        GdmaBlock.pDstImg = pGdmaBlock->pDstImg;
        GdmaBlock.SrcRowStride = pGdmaBlock->SrcRowStride;
        GdmaBlock.DstRowStride = pGdmaBlock->DstRowStride;
        GdmaBlock.BltWidth = pGdmaBlock->BltWidth;
        GdmaBlock.BltHeight = pGdmaBlock->BltHeight;
        GdmaBlock.PixelFormat = pGdmaBlock->PixelFormat;
        (void)AmbaRTSL_GdmaBlockCopy(&GdmaBlock);
        break;

    case DCMD_GDMA_COLOTKEY:
        pGdmaBlock = _DEVCTL_DATA(msg->i);
        GdmaBlock.pSrcImg = pGdmaBlock->pSrcImg;
        GdmaBlock.pDstImg = pGdmaBlock->pDstImg;
        GdmaBlock.SrcRowStride = pGdmaBlock->SrcRowStride;
        GdmaBlock.DstRowStride = pGdmaBlock->DstRowStride;
        GdmaBlock.BltWidth = pGdmaBlock->BltWidth;
        GdmaBlock.BltHeight = pGdmaBlock->BltHeight;
        GdmaBlock.PixelFormat = pGdmaBlock->PixelFormat;
        (void)AmbaRTSL_GdmaColorKeying(&GdmaBlock, pGdmaBlock->TransparentColor);
        break;

    case DCMD_GDMA_ALPHABLEND:
        pGdmaBlock = _DEVCTL_DATA(msg->i);
        GdmaBlock.pSrcImg = pGdmaBlock->pSrcImg;
        GdmaBlock.pDstImg = pGdmaBlock->pDstImg;
        GdmaBlock.SrcRowStride = pGdmaBlock->SrcRowStride;
        GdmaBlock.DstRowStride = pGdmaBlock->DstRowStride;
        GdmaBlock.BltWidth = pGdmaBlock->BltWidth;
        GdmaBlock.BltHeight = pGdmaBlock->BltHeight;
        GdmaBlock.PixelFormat = pGdmaBlock->PixelFormat;
        (void)AmbaRTSL_GdmaAlphaBlending(&GdmaBlock, pGdmaBlock->AlphaVal, pGdmaBlock->BlendMode);
        break;

    case DCMD_GDMA_WAIT_COMPLETE:
        pGdmaWait = _DEVCTL_DATA(msg->i);

        GdmaWaitComplete(pGdmaWait->TimeOut);
        break;
    }

    if (nbytes == 0) {
        return (err);
    } else {
        msg->o.ret_val = 0;
        msg->o.nbytes = nbytes;
        return (_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
    }
}
