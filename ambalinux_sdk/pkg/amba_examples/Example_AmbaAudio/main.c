/**
 *  @file mian.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise preparederivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include "AmbaTypes.h"

#include <mqueue.h>
#include "ambarella_audio.h"
#include <termios.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <locale.h>
#include <alsa/asoundlib.h>
#include <assert.h>
#include <sys/poll.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <asm/byteorder.h>
#include <libintl.h>
#include <endian.h>
#include <byteswap.h>


#define EOK    0

static amba_audio_ctrl_t g_audio_ctrl;
static snd_pcm_t *g_handle_c[2];
static snd_pcm_t *g_handle_p[2];

//local function for audio process
static int audio_shm_init(int buf_size, int timeout)
{
    int rval = EOK;
    int p_rfd;

    /* Create shared memory object and set its size */
    p_rfd = shm_open(SHM_AUDIO_BUF, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (p_rfd == -1) {
    /* Handle error */;
        printf("error");
        rval = -1;
    }

    if (ftruncate(p_rfd, buf_size) == -1) {
        /* Handle error */;
        printf("error");
        rval = -1;
    }

    /* Map shared memory object */
    g_audio_ctrl.p_buf = mmap(NULL, buf_size,
               PROT_READ | PROT_WRITE, MAP_SHARED, p_rfd, 0);
    if (g_audio_ctrl.p_buf == MAP_FAILED) {
        /* Handle error */;
        printf("error");
        rval = -1;
    }

    memset(g_audio_ctrl.p_buf, 0, buf_size);

    return rval;
}

static mqd_t audio_msq_init(const char *name, int rq_num)
{
    amba_audio_msg_t msg;
    mqd_t msg_queue;
    struct mq_attr attrs;

    /* Create messsage queue for audio request */
    memset(&attrs, 0, sizeof attrs);
    attrs.mq_maxmsg = rq_num;
    attrs.mq_msgsize = sizeof(msg);

    msg_queue = mq_open(name, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, &attrs );
    if (msg_queue == -1) {
      printf ("mq_open(): %s\n", name);
    }

    return msg_queue;
}

static int pcm_capture_create(INT8 i2s_idx, int set_frames, int ch, int freq, int dmic_en)
{
  int rc = -1;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  //char *buffer;
  //int size;
  //long loops;

  /* Open PCM device for recording (capture). */
  if (i2s_idx == 0 && dmic_en == 0) {
      rc = snd_pcm_open(&handle, "default",
                        SND_PCM_STREAM_CAPTURE, 0);
  } else if (i2s_idx == 1) {
      if (dmic_en == 0) {
          rc = snd_pcm_open(&handle, "plughw:0,1",
                            SND_PCM_STREAM_CAPTURE, 0);
      } else {
          rc = snd_pcm_open(&handle, "plughw:0,2",
                            SND_PCM_STREAM_CAPTURE, 0);
      }
  } else if (i2s_idx == 0 && dmic_en == 1) {
      rc = snd_pcm_open(&handle, "plughw:0,1",
                        SND_PCM_STREAM_CAPTURE, 0);
  } else {
      fprintf(stderr,
            "incorrect i2s index: %d, dmic_en: %d\n", i2s_idx, dmic_en);
  }

  if (rc < 0) {
      fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
      exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                        SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 32-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                                SND_PCM_FORMAT_S32_LE);

  /* Two channels (stereo) */
  if (ch > AUDIO_CHANNELS) {
      fprintf(stderr,
              "unable to set ch number: %d, max: %d\n",
              ch, AUDIO_CHANNELS);
      exit(1);
  }
  snd_pcm_hw_params_set_channels(handle, params, ch);

  /* 48000 bits/second sampling rate (CD quality) */
  val = freq;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                    &val, &dir);

  /* Set period size to 1024 frames. */
  if (set_frames > AUDIO_FRAME_SIZE) {
      fprintf(stderr,
              "unable to set frame size: %d, max: %d\n",
              set_frames, AUDIO_FRAME_SIZE);
      exit(1);
  }
  frames = (snd_pcm_uframes_t)set_frames;
  snd_pcm_hw_params_set_period_size_near(handle,
                                params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
      fprintf(stderr,
              "unable to set hw parameters: %s\n",
              snd_strerror(rc));
      exit(1);
  }

  g_handle_c[i2s_idx] = handle;
  g_audio_ctrl.param[i2s_idx].capture_frames = set_frames;
  g_audio_ctrl.param[i2s_idx].channels = ch;
  g_audio_ctrl.param[i2s_idx].freq = freq;

  printf("pcm_capture_create: idx: %d, ch: %d, frame: %d, freq: %d, dmic_en: %d\n", i2s_idx, ch, set_frames, freq, dmic_en);

#if 0
  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params,
                                        &frames, &dir);
  size = frames * 8; /* 4 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  loops = 5000000 / val;

  printf("%s: loops: %ld, size: %d, frames: %ld, period_time: %d\n", __func__, loops, size, frames, val);

  while (loops > 0) {
      loops--;
      rc = snd_pcm_readi(handle, buffer, frames);
      if (rc == -EPIPE) {
        /* EPIPE means overrun */
        fprintf(stderr, "overrun occurred\n");
        snd_pcm_prepare(handle);
      } else if (rc < 0) {
        fprintf(stderr,
                "error from read: %s\n",
                snd_strerror(rc));
      } else if (rc != (int)frames) {
        fprintf(stderr, "short read, read %d frames\n", rc);
      }
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);

  printf("%s: loops: %ld\n", __func__, loops);
#endif

  return 0;
}

static int pcm_capture_read(INT8 i2s_idx)
{
  UINT8 *p_buf;
  snd_pcm_t *handle;
  int rc;
  snd_pcm_uframes_t frames;

  handle = g_handle_c[i2s_idx];
  p_buf =  g_audio_ctrl.p_buf + (i2s_idx*AUDIO_MAX_BUFFER_LENGTH/2);
  frames = (snd_pcm_uframes_t)g_audio_ctrl.param[i2s_idx].capture_frames;

  rc = snd_pcm_readi(handle, p_buf, frames);
  if (rc == -EPIPE) {
    /* EPIPE means overrun */
    fprintf(stderr, "overrun occurred\n");
    snd_pcm_prepare(handle);
  } else if (rc < 0) {
    fprintf(stderr,
            "error from read: %s\n",
            snd_strerror(rc));
  } else if (rc != (int)frames) {
    fprintf(stderr, "short read, read %d frames\n", rc);
  }

  return 0;
}

static int pcm_capture_stop(INT8 i2s_idx)
{
  snd_pcm_t *handle;

  handle = g_handle_c[i2s_idx];

  snd_pcm_drain(handle);
  snd_pcm_close(handle);

  printf("pcm_capture_stop: %d\n", i2s_idx);

  return 0;
}

static int pcm_playback_create(INT8 i2s_idx, int set_frames, int ch, int freq, int dmic_en)
{
  int rc = -1;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;

  /* Open PCM device for playback. */
  if (i2s_idx == 0) {
      rc = snd_pcm_open(&handle, "default",
                        SND_PCM_STREAM_PLAYBACK, 0);
  } else if (i2s_idx == 1) {
      if (dmic_en == 0) {
          rc = snd_pcm_open(&handle, "plughw:0,1",
                            SND_PCM_STREAM_PLAYBACK, 0);
      } else {
          rc = snd_pcm_open(&handle, "plughw:0,2",
                            SND_PCM_STREAM_PLAYBACK, 0);
      }
  } else {
      fprintf(stderr,
            "incorrect i2s index: %d\n", i2s_idx);
  }
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 32-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                              SND_PCM_FORMAT_S32_LE);

  /* Two channels (stereo) */
  if (ch > AUDIO_CHANNELS) {
      fprintf(stderr,
              "unable to set ch number: %d, max: %d\n",
              ch, AUDIO_CHANNELS);
      exit(1);
  }
  snd_pcm_hw_params_set_channels(handle, params, ch);

  /* 48000 bits/second sampling rate (CD quality) */
  val = freq;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

  /* Set period size to 1024 frames. */
  if (set_frames > AUDIO_FRAME_SIZE) {
      fprintf(stderr,
              "unable to set frame size: %d, max: %d\n",
              set_frames, AUDIO_FRAME_SIZE);
      exit(1);
  }
  frames = set_frames;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  g_handle_p[i2s_idx] = handle;
  g_audio_ctrl.param[i2s_idx].playback_frames = set_frames;
  g_audio_ctrl.param[i2s_idx].channels = ch;
  g_audio_ctrl.param[i2s_idx].freq = freq;

  printf("pcm_playback_creat:: idx: %d, ch: %d, frame: %d, freq: %d\n", i2s_idx, ch, set_frames, freq);

  return 0;
}

static int pcm_playback_write(INT8 i2s_idx)
{
  UINT8 *p_buf;
  snd_pcm_t *handle;
  int rc;
  snd_pcm_uframes_t frames;

  handle = g_handle_p[i2s_idx];
  p_buf =  g_audio_ctrl.p_buf + (AUDIO_FRAME_SIZE*4*AUDIO_CHANNELS) + (i2s_idx*AUDIO_MAX_BUFFER_LENGTH/2);
  frames = (snd_pcm_uframes_t)g_audio_ctrl.param[i2s_idx].playback_frames;

  rc = snd_pcm_writei(handle, p_buf, frames);
  if (rc == -EPIPE) {
    /* EPIPE means underrun */
    fprintf(stderr, "underrun occurred\n");
    snd_pcm_prepare(handle);
  } else if (rc < 0) {
    fprintf(stderr,
            "error from writei: %s\n",
            snd_strerror(rc));
  }  else if (rc != (int)frames) {
    fprintf(stderr,
            "short write, write %d frames\n", rc);
  }

  return 0;
}

static int pcm_playback_stop(INT8 i2s_idx)
{
  snd_pcm_t *handle;

  handle = g_handle_p[i2s_idx];

  snd_pcm_drain(handle);
  snd_pcm_close(handle);

  printf("pcm_playback_stop: %d\n", i2s_idx);

  return 0;
}

static int msg_parser(void)
{
    int ret = 0;
    amba_audio_msg_t msg;
    ssize_t received_len;
    unsigned int prio;

    received_len = mq_receive (g_audio_ctrl.mq_snd, (char*)&msg, sizeof(msg), &prio);
    if (received_len == -1) {
       printf("msg_parser mq_receive() error\n");
    }

    if (msg.dir == AUDIO_CAPTURE) {
        switch (msg.cmd) {
        case AUDIO_CMD_CREATE:
            ret = pcm_capture_create(msg.i2s_idx, msg.frames, msg.channels, msg.freq, msg.dmic_en);
            if (ret == 0) {
                msg.cmd = AUDIO_CMD_CREATE_RDY;
                mq_send(g_audio_ctrl.mq_rcv, (char*)&msg, sizeof(msg), prio);
            }
            break;
        case AUDIO_CMD_DATA:
            ret = pcm_capture_read(msg.i2s_idx);
            if (ret == 0) {
                msg.cmd = AUDIO_CMD_DATA_RDY;
                mq_send(g_audio_ctrl.mq_rcv, (char*)&msg, sizeof(msg), prio);
            }
            break;
        case AUDIO_CMD_STOP:
            ret = pcm_capture_stop(msg.i2s_idx);
            if (ret == 0) {
                msg.cmd = AUDIO_CMD_STOP_RDY;
                mq_send(g_audio_ctrl.mq_rcv, (char*)&msg, sizeof(msg), prio);
            }
            break;
        default:
            ret = -1;
            printf("wrong cmd parameter: 0x%x\n",msg.cmd);
            break;
        }
    } else if (msg.dir == AUDIO_PLAYBACK) {
        switch (msg.cmd) {
        case AUDIO_CMD_CREATE:
            ret = pcm_playback_create(msg.i2s_idx, msg.frames, msg.channels, msg.freq, msg.dmic_en);
            if (ret == 0) {
                msg.cmd = AUDIO_CMD_CREATE_RDY;
                mq_send(g_audio_ctrl.mq_rcv, (char*)&msg, sizeof(msg), prio);
            }
            break;
        case AUDIO_CMD_DATA:
            ret = pcm_playback_write(msg.i2s_idx);
            if (ret == 0) {
                msg.cmd = AUDIO_CMD_DATA_RDY;
                mq_send(g_audio_ctrl.mq_rcv, (char*)&msg, sizeof(msg), prio);
            }
            break;
        case AUDIO_CMD_STOP:
            ret = pcm_playback_stop(msg.i2s_idx);
            if (ret == 0) {
                msg.cmd = AUDIO_CMD_STOP_RDY;
                mq_send(g_audio_ctrl.mq_rcv, (char*)&msg, sizeof(msg), prio);
            }
            break;
        default:
            ret = -1;
            printf("wrong cmd parameter: 0x%x\n",msg.cmd);
            break;
        }
    } else {
        ret = -1;
        printf("wrong dir parameter: %d\n", msg.dir);
    }

  return ret;
}

#if 0
int PB_test(void)
{
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 32-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                              SND_PCM_FORMAT_S32_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 48000;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

  /* Set period size to 1024 frames. */
  frames = 1024;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
  size = frames * 8; /* 4 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                    &val, &dir);
  /* 5 seconds in microseconds divided by
   * period time */
  loops = 5000000 / val;

  printf("%s: loops: %ld, size: %d, frames: %ld, period_time: %d\n", __func__, loops, size, frames, val);

  while (loops > 0) {
    loops--;
    #if 0
    rc = read(0, buffer, size);
    if (rc == 0) {
      fprintf(stderr, "end of file on input\n");
      break;
    } else if (rc != size) {
      fprintf(stderr,
              "short read: read %d bytes\n", rc);
    }
    #endif
    rc = snd_pcm_writei(handle, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr,
              "error from writei: %s\n",
              snd_strerror(rc));
    }  else if (rc != (int)frames) {
      fprintf(stderr,
              "short write, write %d frames\n", rc);
    }
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);

  printf("%s: loops: %ld\n", __func__, loops);

  return 0;
}
#endif

int main(int argc, char *argv[])
{
    int ret = 0;

    //PB_test();

    audio_shm_init(AUDIO_MAX_BUFFER_LENGTH, AUDIO_REQUEST_TIMEOUT);
    g_audio_ctrl.mq_snd = audio_msq_init(MQ_AUDIO_SND, AUDIO_REQUEST_QUEUE_NUM);
    g_audio_ctrl.mq_rcv = audio_msq_init(MQ_AUDIO_RCV, AUDIO_REQUEST_QUEUE_NUM);

    while (1) {
        ret = msg_parser();
    }

    return ret;
}
