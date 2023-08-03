#ifndef  OSAL_IOCTL_H
#define  OSAL_IOCTL_H

#define OSAL_DEV "/dev/osal"

typedef struct {
    uint16_t    module;
    uint8_t     enable;
} osal_get_value_t;

/* set ModulePrint ioctl */
#define OSAL_SET_ALLOW_LIST _IOW  ('o', 0x0U, osal_get_value_t)

#endif  //OSAL_IOCTL_H
