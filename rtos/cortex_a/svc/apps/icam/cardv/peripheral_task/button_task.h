#ifndef __BUTTON_TASK_H__
#define __BUTTON_TASK_H__

#include <app_base.h>

int button_task_start(void);
int button_task_stop(void);
int button_task_set_debug(int enable);
int button_task_set_time(unsigned int x, unsigned int y, unsigned int z);

#endif//__BUTTON_TASK_H__

