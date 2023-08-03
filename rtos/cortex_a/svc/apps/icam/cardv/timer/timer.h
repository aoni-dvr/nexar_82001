#ifndef __TIMER_H__
#define __TIMER_H__

#include <AmbaKAL.h>

#define MAX_TIMER_HANDLER   (30)

typedef enum _app_timer_id_e_ {
    TIMER_1HZ = 0,
    TIMER_2HZ,
    TIMER_30S,
    TIMER_NUM
} app_timer_id_e;

typedef struct _app_timer_info_s_ {
    app_timer_id_e timer_id;
    int period;
    int msg_id;
} app_timer_info_s;

typedef void (*app_timer_handler)(int eid);
typedef struct _app_timer_s_ {
    unsigned int Valid;  /* 1:Valid timer, 0:free entry */
    int HandlerNum;
    AMBA_KAL_TIMER_t AppTimer;
    app_timer_handler Handler[MAX_TIMER_HANDLER];
} app_timer_s;

#define TIMER_TICK              (1)  /**<TIMER_TICK              (1) */
#define TIMER_UNREGISTER        (2)  /**<TIMER_UNREGISTER        (2) */

extern int app_timer_init(void);
extern int app_timer_unregister_all(void);
extern int app_timer_register(int tid, app_timer_handler handler);
extern int app_timer_unregister(int tid, app_timer_handler handler);
extern int app_timer_callback_handler(int tid);

#endif//__TIMER_H__

