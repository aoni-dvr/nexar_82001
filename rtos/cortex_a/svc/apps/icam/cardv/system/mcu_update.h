#ifndef __MCU_UPDATE_H__
#define __MCU_UPDATE_H__

int mcu_update_task_start(void);
int mcu_update_task_stop(void);
int mcu_update_task_set_debug(int debug);
void mcu_update_set_run(void);

#endif//__MCU_UPDATE_H__
