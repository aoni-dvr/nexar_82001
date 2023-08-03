#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "cvtask_api.h"
#include "cvapi_memio_interface.h"
#include "AmbaFlexidagIO.h"

#include "cam_util.h"

#define TRANS_OUTPUT_TYPE (FLEXIDAGIO_RESULT_OUTTYPE_SEG)

static uint32_t g_seq_num = 0;
static uint32_t g_frame_num = 0;

static void RefCV_TransferInit(uint32_t Channel, uint32_t OutType)
{
    if (AmbaIPC_FlexidagIO_Init(Channel) != FLEXIDAGIO_OK) {
        fprintf(stderr, "RefCV_TransferInit AmbaIPC_FlexidagIO_Init fail\n");
    }

    if (AmbaIPC_FlexidagIO_Config(Channel, OutType) != FLEXIDAGIO_OK) {
        fprintf(stderr, "RefCV_TransferInit AmbaIPC_FlexidagIO_Config fail\n");
    }
}

static void RefCV_TransferDeinit(uint32_t Channel)
{
    if (AmbaIPC_FlexidagIO_Deinit(Channel) != FLEXIDAGIO_OK) {
        fprintf(stderr, "RefCV_TransferDeinit AmbaIPC_FlexidagIO_Deinit fail\n");
    }
}

static uint32_t RefCV_TransferSourceData(uint32_t Channel, memio_source_recv_picinfo_t *pInfo)
{
    uint32_t Rval = FLEXIDAGIO_OK;
    unsigned int GetLen;

    Rval = AmbaIPC_FlexidagIO_GetInput(Channel, pInfo, &GetLen);
    if (Rval != FLEXIDAGIO_OK) {
        fprintf(stderr, "RefCV_TransferSourceData AmbaIPC_FlexidagIO_GetProcess fail\n");
    } else {
        g_seq_num++;
        g_frame_num = pInfo->pic_info.frame_num;
#if DEBUG
        fprintf(stderr, "RefCV_TransferSourceData Channel=%d frame_num=%d rpLumaLeft=0x%x\n", Channel, pInfo->pic_info.frame_num, pInfo->pic_info.rpLumaLeft[0]);
#endif
    }

    return Rval;
}

static uint32_t RefCV_TransferSinkData(uint32_t Channel, uint32_t type, uint32_t BufOutAddr, uint32_t BufOutSize)
{
    uint32_t Rval = FLEXIDAGIO_OK;
    memio_sink_send_out_t SinkOut;

    SinkOut.type = type;
    SinkOut.cvtask_frameset_id = g_frame_num;
    SinkOut.num_of_io = g_seq_num;
    SinkOut.io[0].addr = BufOutAddr;
    SinkOut.io[0].size= BufOutSize;
    Rval = AmbaIPC_FlexidagIO_SetResult(Channel, &SinkOut, sizeof(SinkOut));
    if (Rval != FLEXIDAGIO_OK) {
        fprintf(stderr, "RefCV_TransferSinkData AmbaIPC_FlexidagIO_Result fail\n");
    } else {
#if DEBUG
        fprintf(stderr, "RefCV_TransferSinkData Channel=%d frameset_id=%d PA=0x%x size=0x%x\n",
                Channel, SinkOut.cvtask_frameset_id, SinkOut.io[0].addr, SinkOut.io[0].size);
#endif
    }

    return Rval;
}

static uint32_t GetFrame(uint32_t ch, memio_source_recv_picinfo_t *PicInfo)
{
    uint32_t Rval = 0;

    Rval = RefCV_TransferSourceData(ch, PicInfo);

    return Rval;
}

static uint32_t SendResult(uint32_t ch, memio_sink_send_out_t *out)
{
    uint32_t Rval = FLEXIDAGIO_OK;

    Rval = AmbaIPC_FlexidagIO_SetResult(ch, out, sizeof(memio_sink_send_out_t));
    if (Rval != FLEXIDAGIO_OK) {
        fprintf(stderr, "RefCV_TransferSinkData AmbaIPC_FlexidagIO_Result fail\n");
    } else {
#if DEBUG
        fprintf(stderr, "RefCV_TransferSinkData Channel=%d frameset_id=%d PA=0x%x size=0x%x\n",
                ch, out->cvtask_frameset_id, out->io[0].addr, out->io[0].size);
#endif
    }

    return Rval;
}

/**
 * This function is to init CamUtil
 **/
uint32_t CamUtil_Init(uint32_t ch)
{
    uint32_t ret = 0;

    RefCV_TransferInit(ch, TRANS_OUTPUT_TYPE);
    return ret;
}

/**
 * This function is to release CamUtil
 **/
uint32_t CamUtil_Release(uint32_t ch)
{
    uint32_t ret = 0;

    RefCV_TransferDeinit(ch);
    return ret;
}

/**
 * This function is to retrieve a frame
 * Please note, the rpLumaLeft[], rpLumaRight[], rpChromaLeft, and rpChromaRight of pInfo->pic_info are physical address.
 * You have to do ambacv_p2v() if you need to access them in User Space.
 **/
uint32_t CamUtil_GetFrame(uint32_t ch, memio_source_recv_picinfo_t *pInfo)
{
    uint32_t ret = 0;

    ret = GetFrame(ch, pInfo);
    return ret;
}

/**
 * This function is to send a result.
 * Please note, the out->io[].addr has to be physical address.
 **/
uint32_t CamUtil_SendResult(uint32_t ch, memio_sink_send_out_t *out)
{
    uint32_t ret = 0;

    ret = SendResult(ch, out);
    return ret;
}

