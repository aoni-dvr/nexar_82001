#ifndef __PMIC_H__
#define __PMIC_H__

#include "platform.h"

typedef enum _boot_reason_e_ {
    BOOT_REASON_VBUS_TRIGGER = 0x01,
    BOOT_REASON_ACC_TRIGGER = 0x02,
    BOOT_REASON_BUTTON_TIGGER = 0x04,
    BOOT_REASON_HARD_RESET_TRIGGER = 0x08,
    BOOT_REASON_IMU_TRIGGER = 0x10,
    BOOT_REASON_LTE_TRIGGER = 0x20,
    BOOT_REASON_TIMER_TRIGGER = 0x40,
    BOOT_REASON_SOFT_RESET_TRIGGER = 0x80,
    BOOT_REASON_WATCHDOG_TRIGGER = 0x100,
    BOOT_REASON_FACTORY_RESET_TRIGGER = 0x200,
} boot_reason_e;

typedef struct _PMIC_OBJECT_s_ {
    char *name;
    int (*init)(void);
    int (*set_lcd_backlight)(int);
    int (*get_lcd_backlight)(void);
    int (*check_usb_connected)(void);
    int (*set_sram_reg)(int);
    int (*get_sram_reg)(void);
    int (*set_softreset_flag)(void);
    int (*reset)(void);
    int (*set_except_softreset_flag)(void);
    int (*power_off)(int wakeup_minutes);
    int (*set_led)(int);
    int (*dump_regs)(void);
    int (*read)(unsigned char, unsigned char *);
    int (*write)(unsigned char, unsigned char);
    int (*modify)(unsigned char, unsigned char, unsigned char);
    int (*switch_sd_power)(int);
    int (*check_battery_temp)(void);
    int (*get_battery_temp)(void);
    int (*check_power_key_pressed)(void);
    int (*check_acc_connected)(void);
    boot_reason_e (*get_boot_reason)(void);
    int (*get_version)(char *version);
    int (*get_acc_state)(void);
} PMIC_OBJECT_s;

int Pmic_Init(void);
int Pmic_DoPowerOff(int param);
int Pmic_CheckUsbConnected(void);
int Pmic_SetSramRegister(int enable);
int Pmic_GetSramRegister(void);
int Pmic_SoftReset(void);
int Pmic_NormalSoftReset(void);
int Pmic_DumpRegs(void);
int Pmic_Read(unsigned char addr, unsigned char *data);
int Pmic_Write(unsigned char addr, unsigned char data);
int Pmic_Modify(unsigned char addr, unsigned char mask, unsigned char data);
boot_reason_e Pmic_GetBootReason(void);
int Pmic_GetVersion(char *version);
int Pmic_SetBootFailTimes(int times);
int Pmic_GetAccState(void);

extern UINT32 AmbaNAND_SwitchBoot(UINT8 bootA);

#endif

