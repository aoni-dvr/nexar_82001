/**
 *  Read frame from source and send to RTOS through netfifo
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "AmbaNetFifo.h"
#include "libambamem.h"
#include "example_source.h"

/**
 *  debug or workaround
 */
#define HAVE_PRVATE_MEMCPY
#include "local_mem_util.h"


enum amba_streamin_msg_e {
    AMBA_STREAMIN_MSG_START = 1,
    AMBA_STREAMIN_MSG_STOP,
    AMBA_STREAMIN_MSG_ERROR
};

typedef struct amba_media_hndlr_s {
    unsigned long long fifo_hndlr;
    unsigned long long buf_base;          // fifo ring-buffer base addr
    unsigned long long buf_limit;         // fifo ring-buffer limit addr
} amba_media_hndlr_t;

typedef struct example_stream_s {
    amba_media_hndlr_t video;       // video info
    amba_media_hndlr_t audio;       // audio info
    int                netfifo_sd;  // socket for libnetfifo
} example_stream_t;


static int cbFrameReady(void *hdlr, unsigned int event, void *info, void *user_data)
{
    printf("%s: Got event 0x%08x\n", __func__, event);
    return 0;
}

static int cbControl(unsigned int cmd, unsigned int param1, unsigned int param2, void *user_data)
{
    printf("%s: cmd=%u\n", __FUNCTION__, cmd);
    return 0;
}

static void send_streamin_msg(int msg)
{
    AMBA_NETFIFO_STREAMIN_MSG_PARAM_s in = {0, {0}};
    AMBA_NETFIFO_STREAMIN_MSG_PARAM_s out = {0, {0}};
    in.MSG = msg;
    AmbaNetFifo_StreamIn_MSG(&in, &out);
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  create_vfifo
 *
 *  @Description::  create virtual fifo for stream-in playback
 *
 *  @Input      ::
 *
 *  @Output     ::
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int create_vfifo(example_stream_t *streamIn)
{
    int rval = 0, i = 0;
    AMBA_NETFIFO_MOVIE_INFO_CFG_s   mInfo;
    AMBA_NETFIFO_CFG_s              cfg;

    // retrieve hcodec from rtos, for now, use getMediainfo
    rval = AmbaNetFifo_GetMediaInfo(0xaabb, &mInfo);
    if (rval < 0) {
        fprintf(stderr, "%s: Fail to do AmbaNetFifo_GetMediaInfo()\n", __FUNCTION__);
        return -1;
    }

    // create virtual fifo
    rval = AmbaNetFifo_GetDefaultCFG(&cfg);
    if (rval < 0) {
        fprintf(stderr, "%s: Fail to do AmbaNetFifo_GetDefaultCFG()\n", __FUNCTION__);
        return -1;
    }

    for (i = 0; i < mInfo.nTrack; i++) {
        if (mInfo.Track[i].nTrackType == AMBA_NETFIFO_MEDIA_TRACK_TYPE_VIDEO) {
            streamIn->video.buf_base  = (unsigned long long)(uintptr_t) mInfo.Track[i].pBufferBase;
            streamIn->video.buf_limit = (unsigned long long)(uintptr_t) mInfo.Track[i].pBufferLimit;

            if (mInfo.Track[i].hCodec != NULL) {
                cfg.hCodec     = mInfo.Track[i].hCodec;
                cfg.cbEvent    = streamIn->netfifo_sd;
                cfg.NumEntries = 256;
                cfg.IsVirtual  = 1;
                streamIn->video.fifo_hndlr = AmbaNetFifo_Create(&cfg);
                if (streamIn->video.fifo_hndlr == 0ull) {
                    fprintf(stderr, "%s: Fail to do AmbaNetFifo_Create()\n", __FUNCTION__);
                    rval = -1;
                } else {
                    printf("%s:(Video) Create new fifo_handle_id= x%llx for hCodec= %p success!\n",
                           __FUNCTION__, streamIn->video.fifo_hndlr, cfg.hCodec);
                    rval = 0;
                }
            }
        } else if (mInfo.Track[i].nTrackType == AMBA_NETFIFO_MEDIA_TRACK_TYPE_AUDIO) {
            streamIn->audio.buf_base = (unsigned long long)(uintptr_t) mInfo.Track[i].pBufferBase;
            streamIn->audio.buf_limit = (unsigned long long)(uintptr_t) mInfo.Track[i].pBufferLimit;

            if (mInfo.Track[i].hCodec != NULL) {
                cfg.hCodec     = mInfo.Track[i].hCodec;
                cfg.cbEvent    = streamIn->netfifo_sd;
                cfg.NumEntries = 256;
                cfg.IsVirtual  = 1;
                streamIn->audio.fifo_hndlr = AmbaNetFifo_Create(&cfg);
                if (streamIn->audio.fifo_hndlr == 0ull) {
                    fprintf(stderr, "%s: Fail to do AmbaNetFifo_Create()\n", __FUNCTION__);
                    rval = -1;
                } else {
                    printf("%s:(Audio) Create new fifo_handle_id= x%llx for hCodec= %p success!\n",
                           __FUNCTION__, streamIn->audio.fifo_hndlr, cfg.hCodec);
                    rval = 0;
                }
            }
        } else {
            rval = -1;
        }
    }
    return rval;
}

static int write_fifo(unsigned char *src, int data_len, amba_media_hndlr_t *media)
{
    AMBA_NETFIFO_WRITEENTRY_ARG_s   entry = { 0 };
    unsigned long long              fifo_hndlr;
    unsigned char                   *entryBuf = NULL;
    unsigned long long              buf_base, buf_limit;
    int                             rval;

    fifo_hndlr = media->fifo_hndlr;
    buf_base   = media->buf_base;
    buf_limit  = media->buf_limit;
    if (fifo_hndlr) {
        if (AmbaNetFifo_PrepareEntry(fifo_hndlr, &entry.desc) < 0) {
            fprintf(stderr, "[%s]: IPC PrepareEntry fail", __func__);
            return -1;
        }
    } else {
        fprintf(stderr, "fifo handler is empty!(ignore)\n");
        return -1;
    }

    entryBuf = entry.desc.StartAddr;
    if (entry.desc.Size < data_len) {
        fprintf(stderr, "target entry size is not enough!(x%x) <- (x%x)\n", entry.desc.Size, data_len);
        return 1;
    }

    entry.desc.Pts       = 0;
    entry.desc.Size      = data_len;
    entry.desc.Completed = 1;
    entry.fifo           = fifo_hndlr;

    // copy data(ring buffer)
    if ( (unsigned long long) ((uintptr_t)entryBuf + data_len - 1) <= buf_limit ) {
        memcpy(entryBuf, src, data_len);
    } else {
        unsigned long long  freeToEnd = buf_limit - (unsigned long long)(uintptr_t) src + 1;
        unsigned int        Remain = data_len - freeToEnd;
        memcpy(entryBuf, src, freeToEnd);
        memcpy((char *)(uintptr_t)buf_base, src + freeToEnd, Remain);
    }
    rval = AmbaNetFifo_WriteEntry(&entry);
    if (rval < 0) {
        printf("WriteEntry Fail. rval = %d\n", rval);
    }
    return rval;
}

static int release_fifo(example_stream_t *streamIn)
{
    if( streamIn->video.fifo_hndlr) {
        fprintf(stderr, "Delete fifo x%llx\n", streamIn->video.fifo_hndlr);
        AmbaNetFifo_Delete(streamIn->video.fifo_hndlr);
    }
    if( streamIn->audio.fifo_hndlr) {
        fprintf(stderr, "Delete fifo x%llx\n", streamIn->audio.fifo_hndlr);
        AmbaNetFifo_EraseAll(streamIn->audio.fifo_hndlr);//rtos won't release audio fifo, need to clean it
        AmbaNetFifo_Delete(streamIn->audio.fifo_hndlr);
    }
    return AmbaNetFifo_release();
}

static int init_fifo(example_stream_t *streamIn)
{
    int rval;

    rval = AmbaNetFifo_init(&streamIn->netfifo_sd);
    if (rval < 0) {
        fprintf(stderr, "%s: Fail to do AmbaNetFifo_init()\n", __FUNCTION__);
        return -1;
    }

    AmbaNetFifo_Reg_cbFifoEvent(cbFrameReady, NULL);
    AmbaNetFifo_Reg_cbControlEvent(cbControl, NULL);

    rval = create_vfifo(streamIn);
    if (rval < 0) {
        fprintf(stderr, "%s: Fail to do create_vfifo()\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int rval;
    example_stream_t streamIn;
    struct example_source_s *ex_source;
    unsigned char *data;
    unsigned int data_len;
    unsigned int frame_num = 0;

    if (argc < 2) {
        printf("usage: %s input \n", argv[0]);
        return 1;
    }

    memset(&streamIn, 0, sizeof(example_stream_t));

    //open source
    rval = example_source_open(&ex_source, argv[1]);
    if(rval < 0) {
        fprintf(stderr, "%s: Fail to open input: %s\n", __FUNCTION__, argv[1]);
    }

    //do init
    rval = init_fifo(&streamIn);
    if (rval < 0) {
        fprintf(stderr, "%s: Fail to init fifo\n", __FUNCTION__);
        return -1;
    }

    while(1) {
        rval = example_source_read(ex_source, &data, &data_len);
        if (rval < 0) {
            fprintf(stderr, "read_frame error\n");
            break;
        } else if (rval > 0) {
            fprintf(stderr, "read EOS from source, %d frames written\n", frame_num);
            break;
        }
        frame_num++;

        //only handle audio
        rval = write_fifo(data, data_len, &streamIn.audio);
        if (rval < 0) {
            fprintf(stderr, "write_fifo error\n");
            break;
        }
        /*
         *  Control the speed of feeding frame (clock = 48000, ts_diff = 1024)
         *  Slightly faster than frame rate in case usleep does not accurately
         *  return when system loading is high
         */
        usleep(19000);

        //activate after feeding some frames
        if(frame_num == 5)      send_streamin_msg(AMBA_STREAMIN_MSG_START);
    }

    //release flow
    if (rval >= 0) {
        send_streamin_msg(AMBA_STREAMIN_MSG_STOP);
    } else {
        send_streamin_msg(AMBA_STREAMIN_MSG_ERROR);
    }
    example_source_close(ex_source);
    release_fifo(&streamIn);
    return 0;
}
