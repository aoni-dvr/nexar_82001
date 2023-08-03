#include <app_base.h>
#include "SvcPlat.h"

static unsigned int beep_buf_len = 0, beep_buf_size = 0;
static unsigned char beep_busy = 0;
static beep_item_s beep_items[BEEP_ID_NUM] = {
    {BEEP_ID_POWER_ON,      "power_on.pcm"},
    {BEEP_ID_POWER_OFF,     "power_off.pcm"},
    {BEEP_ID_RESET,         "reset.pcm"},
    {BEEP_ID_PIV,           "piv.pcm"},
    {BEEP_ID_EVENT_RECORD,  "event_record.pcm"},
    {BEEP_ID_BUTTON_PRESS,  "button_press.pcm"},
    {BEEP_ID_1K,            "1k.pcm"},
    {BEEP_ID_SAMPLE_AAC,    "sample.aac"},
    {BEEP_ID_DUMP_LOG,      "dump_log.pcm"},
};

static int get_file_info(const char *path, unsigned int *size, int *is_romfs, int *is_aac)
{
    unsigned char header[2] = {0};

    if (path == NULL || size == NULL) {
        debug_line("%s invalid param", __func__);
        return -1;
    }

    if (path[1] == ':' && path[2] == '\\') {
        struct stat fstat;
        *is_romfs = 0;
        if (stat(path, &fstat) != 0) {
            return -1;
        }
        *size = fstat.size;
        if (*size >= 2) {
            FILE *pFile = fopen(path, "rb");
            if (pFile) {
                fread(header, 1, 2, (void *)pFile);
                fclose(pFile);
            }
        }
    } else {
        *is_romfs = 1;
        AmbaNVM_GetRomFileSize(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, path, size);
        AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, path, 0, 2, header, 5000);
    }

    if (header[0] == 0xFF
        && (header[1] == 0xF1 || header[1] == 0xF9)) {
        *is_aac = 1;
    }

    return 0;
}

static int read_file(const char *path, const void *pFile, int offset, int size, unsigned char *buf)
{
    int rval = 0;
    int read_size = 0;

    if (path == NULL || buf == NULL || size <= 0) {
        debug_line("%s invalid param", __func__);
        return -1;
    }

    if (offset >= (int)beep_buf_size) {
        memset(buf, 0xFF, size);
        return 0;
    } else if ((offset + size) > (int)beep_buf_size) {
        read_size = beep_buf_size - offset;
        memset(buf + read_size, 0xFF, size - read_size);
    } else {
        read_size = size;
    }

    if (pFile != NULL) {
        rval = fread(buf, 1, read_size, (void *)pFile);
    } else {
        rval = AmbaNVM_ReadRomFile(NVM_TYPE, AMBA_NVM_ROM_REGION_SYS_DATA, path, offset, read_size, buf, 5000);
    }

    return rval;
}

static AMBA_AUDIO_DEMUX_CTRL_s *pDmuxHdlr = NULL;
#define AUDIO_FEED_DATA        0x00000001UL

static void *beep_pFile = NULL;
static char beep_path[128] = {0};
static unsigned int beep_buf_offset = 0;
static int force_end = 0;
static ULONG DecId = 0;
extern int BeepPlayEos;

static SVC_TASK_CTRL_s         AudioFeedTask;
static UINT32  AudioFeedTaskStack[0x2000] GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t SvcAudioFeedFlag;
static void *audio_feed_task_entry(void *arg);

static void AoutCtrl(UINT32 Chan, UINT32 Op)
{
    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbAoutCtrl != NULL)) {
        g_pPlatCbEntry->pCbAoutCtrl(Chan, Op);
    }
}

static int beep_init(void)
{
    AmbaKAL_EventFlagCreate(&SvcAudioFeedFlag, "DecFeedFlag");
    /* Create a feeding audio task */
    AudioFeedTask.Priority   = SVC_BEEP_FEED_TASK_PRI;
    AudioFeedTask.EntryFunc  = audio_feed_task_entry;
    AudioFeedTask.EntryArg   = 0;
    AudioFeedTask.pStackBase = AudioFeedTaskStack;
    AudioFeedTask.StackSize  = sizeof(AudioFeedTaskStack);
    AudioFeedTask.CpuBits    = SVC_BEEP_FEED_TASK_CPU_BITS;
    if (SvcTask_Create("audio_feed_task", &AudioFeedTask) != KAL_ERR_NONE) {
        debug_line("%s create audio feed task fail", __func__);
        return -1;
    }

    return 0;
}

int beep_ready(void)
{
    UINT32 rval = 0;

    rval = AmbaKAL_EventFlagSet(&SvcAudioFeedFlag, AUDIO_FEED_DATA);
    if (ADEC2AOUT_OK != rval) {
        debug_line("SvcAudioFeedFlag FlagSet error, RtVal: %d", rval);
        return -1;
    }

    return 0;
}

static int beep_feed_audio(AMBA_AUDIO_DEMUX_CTRL_s *pHdlr, const char *path, const void *pFile)
{
    AMBA_ADEC_AUDIO_DESC_s *pDesc = NULL;
    UINT8 *pAddr = NULL;
    int tmp_len = 0;

    if ((beep_buf_offset + AMBA_AUDIO_DEMUX_BUFFER_SIZE) >= beep_buf_len) {
        tmp_len = beep_buf_len - beep_buf_offset;
    } else {
        tmp_len = AMBA_AUDIO_DEMUX_BUFFER_SIZE;
    }
    if (tmp_len <= 0 || force_end) {
        if (force_end) {
            debug_line("%s user stop play", __func__);
        }
        return -1;
    }
    pDesc = AmbaAudio_BsBufDemuxGetWrPtr(pHdlr);
    pAddr = pDesc->pBufAddr;
    read_file(path, pFile, beep_buf_offset, tmp_len, pAddr);
    beep_buf_offset += tmp_len;
    pHdlr->BsCurSize += tmp_len;
    (void)AmbaAudio_BsBufDemuxUptWrPtr(pHdlr);
    (void)AmbaADEC_UpdateBsBufWp(pHdlr->pDecHdlr, pAddr, tmp_len);
    if (beep_buf_offset >= beep_buf_len) {
        return -1;
    }

    return 0;
}

static void *audio_feed_task_entry(void *arg)
{
    UINT32 ActualFlags = 0U;
    AMBA_ADEC_AUDIO_DESC_s *pDesc = NULL;

    (void)arg;
    while (1) {
        (void)AmbaKAL_EventFlagGet(&SvcAudioFeedFlag, AUDIO_FEED_DATA,
                                 AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO,
                                 &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (beep_busy && beep_feed_audio(pDmuxHdlr, beep_path, beep_pFile) < 0) {
            pDesc = AmbaAudio_BsBufDemuxGetWrPtr(pDmuxHdlr);
            pDesc->Eos = 1U;
            if (beep_pFile) {
                fclose(beep_pFile);
            }
            beep_pFile = NULL;
            while (BeepPlayEos == 0 && force_end == 0) {
                msleep(30);
            }
            AmbaAudio_CodecEnableSpeaker(0, 0);
            AmbaADecFlow_Stop(DecId, 0U, AoutCtrl);
            AmbaADecFlow_Delete(DecId);
            beep_busy = 0;
            debug_line("beep play finished");
        }
    }
    return NULL;
}

static unsigned int total_ms = 0, beep_start_tick = 0;
static int beep_sound_impl(const char *path)
{
    unsigned int i = 0, size = 0;
    int rval = 0, is_romfs = 0, is_aac = 0;
    static int inited = 0;

    if (inited == 0) {
        inited = 1;
        beep_init();
    }
    rval = get_file_info(path, &size, &is_romfs, &is_aac);
    if (rval < 0 || size == 0) {
        debug_line("%s get_file_info [%s] fail. size=%d", __func__, path, size);
        return -1;
    }
    size = size / 4 * 4;
    total_ms = size / 2 / 2 / 48;
    debug_line("%s load [%s] ok. size=%dB, duration=%dms, is_aac: %d", __func__, path, size, total_ms, is_aac);

    beep_start_tick = tick();
    memset(beep_path, 0, sizeof(beep_path));
    snprintf(beep_path, sizeof(beep_path) - 1, "%s", path);
    if (is_romfs) {
        beep_pFile = NULL;
    } else {
        beep_pFile = fopen(path, "rb");
        if (beep_pFile == NULL) {
            debug_line("%s open fail", __func__);
            return -1;
        }
    }
    BeepPlayEos = 0;
    beep_buf_offset = 0;
    beep_buf_size = size;
    AmbaKAL_EventFlagClear(&SvcAudioFeedFlag, 0xffffffffUL);
    if (is_aac) {
        beep_buf_len = size;
        AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_AAC, is_romfs ? AMBA_AUDIO_SOURCE_ROM_AUD : AMBA_AUDIO_SOURCE_AUDIO, 0, NULL, &DecId);
    } else {
        if (beep_buf_size < (50 * 1024)) {
            beep_buf_len = 50 * 1024;
        } else {
            beep_buf_len = size;
        }
        AmbaADecFlow_Setup(AMBA_AUDIO_TYPE_PCM, is_romfs ? AMBA_AUDIO_SOURCE_ROM_AUD : AMBA_AUDIO_SOURCE_AUDIO, 0, NULL, &DecId);
    }
    pDmuxHdlr = (AMBA_AUDIO_DEMUX_CTRL_s *)AmbaADecFlow_GetDemuxCtrlHdlr();
    for (i = 0; i < AU_DEMUX_DESC_NUM; i++) {
        if (beep_feed_audio(pDmuxHdlr, beep_path, beep_pFile) < 0) {
            break;
        }
    }
    AmbaAudio_CodecEnableSpeaker(0, 1);
    AmbaADecFlow_DecStart(DecId);
    AmbaADecFlow_OupStart(DecId, AoutCtrl);

    return 0;
}

int beep_play(beep_id_e beep_id)
{
    int i = 0;
    int rval = 0;

    if (beep_busy) {
        debug_line("beep busy, ignore new beep");
        return -1;
    }

    for (i = 0; i < BEEP_ID_NUM; i++) {
        if (beep_id == beep_items[i].beep_id) {
            break;
        }
    }

    if (i >= BEEP_ID_NUM) {
        debug_line("%s not find beep", __func__);
        return -1;
    }

    force_end = 0;
    beep_busy = 1;
    rval = beep_sound_impl(beep_items[i].filename);
    if (rval < 0) {
        beep_busy = 0;
    }
    return rval;
}

int beep_file(const char *path)
{
    int rval = 0;

    if (beep_busy) {
        debug_line("beep busy, ignore new beep");
        return -1;
    }
    force_end = 0;
    beep_busy = 1;
    rval = beep_sound_impl(path);
    if (rval < 0) {
        beep_busy = 0;
    }
    return rval;
}

int beep_stop(void)
{
    if (beep_busy) {
        debug_line("%s user stop play", __func__);
        force_end = 1;
    } else {
        debug_line("%s not playing now", __func__);
    }
    return 0;
}

int beep_wait_finish(int timeout_ms)
{
    int cnt = 0;

    while (beep_busy) {
        cnt++;
        if ((cnt * 10) >= timeout_ms) {
            break;
        }
        msleep(10);
    }

    return 0;
}

int beep_is_playing(void)
{
    return beep_busy;
}

unsigned int beep_remain_ms(void)
{
    unsigned int used_ms = 0;

    if (beep_busy == 0) {
        return 0;
    }
    used_ms = tick() - beep_start_tick;
    if (total_ms > used_ms) {
        return (total_ms - used_ms);
    }

    return 0;
}

