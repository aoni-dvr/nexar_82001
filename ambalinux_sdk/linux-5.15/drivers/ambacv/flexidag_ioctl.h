#ifndef FLEXIDAG_IOCTL_H_FILE
#define FLEXIDAG_IOCTL_H_FILE

#include <ambacv_kal.h>
#include <cvapi_flexidag.h>

void  krn_flexidag_system_init(void);
void  krn_flexidag_system_reset(int is_init);
int   krn_flexidag_ioctl_process(struct file *f, unsigned int cmd, unsigned long arg);
int   krn_flexidag_ioctl_handle_message(armvis_msg_t *pUserMsg);
void  krn_flexidag_system_shutdown(void);
void  krn_flexidag_system_set_trace_daddr(uint64_t trace_daddr);
uint64_t krn_flexidag_system_get_trace_daddr(void);
void krn_flexidag_close_by_filp(struct file *f);
void krn_flexidag_shutdown(void);

#endif /* ?FLEXIDAG_IOCTL_H_FILE */

