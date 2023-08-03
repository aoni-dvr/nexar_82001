#ifndef __BEEP_H__
#define __BEEP_H__

typedef enum _beep_id_e_ {
    BEEP_ID_POWER_ON = 0,
    BEEP_ID_POWER_OFF,
    BEEP_ID_RESET,
    BEEP_ID_PIV,
    BEEP_ID_EVENT_RECORD,
    BEEP_ID_BUTTON_PRESS,
    BEEP_ID_1K,
    BEEP_ID_SAMPLE_AAC,
    BEEP_ID_DUMP_LOG,
    BEEP_ID_NUM
} beep_id_e;

typedef struct _beep_item_s_ {
    beep_id_e beep_id;
    char *filename;
} beep_item_s;

int beep_play(beep_id_e beep_id);
int beep_file(const char *path);
int beep_stop(void);
int beep_is_playing(void);
unsigned int beep_remain_ms(void);
int beep_wait_finish(int timeout_ms);

#endif

