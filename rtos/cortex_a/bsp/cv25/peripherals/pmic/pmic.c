#include <pmic.h>
#include "AmbaGPIO_Def.h"
#include "AmbaGPIO.h"
#include "bsp.h"

static PMIC_OBJECT_s *pmic_object = NULL;

#define CHECK_PMIC_OBJECT {if (pmic_object == NULL) {debug_line("PMU No Register");return -1;}}
static inline void pmic_register(PMIC_OBJECT_s *pmic_obj)
{
    pmic_object = pmic_obj;
    if (pmic_object != NULL) {
        debug_line("%s: %s", __func__, pmic_object->name);
    } else {
        debug_line("%s: No PMU", __func__);
    }
}

int Pmic_Init(void)
{
#if defined(CONFIG_PMIC_AMBA_PWC)
    extern PMIC_OBJECT_s pmic_amba_pwc_object;
    pmic_register(&pmic_amba_pwc_object);
#endif
#if defined(CONFIG_PMIC_CHIPON)
    extern PMIC_OBJECT_s pmic_chipon_object;
    pmic_register(&pmic_chipon_object);
#endif

    if (pmic_object->init != NULL) {
        pmic_object->init();
    }
    return 0;
}

int Pmic_DoPowerOff(int param)
{
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
    Pmic_SetBootFailTimes(0);
    CHECK_PMIC_OBJECT;
    if (pmic_object->power_off != NULL) {
        pmic_object->power_off(param);
    }

    return 0;
}

int Pmic_DumpRegs(void)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->dump_regs == NULL) {
        return -1;
    }
    pmic_object->dump_regs();
    return 0;
}

int Pmic_CheckUsbConnected(void)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->check_usb_connected == NULL) {
        return -1;
    }
    return pmic_object->check_usb_connected();
}

int Pmic_SetBootFailTimes(int times)
{
    int val = Pmic_GetSramRegister();

    val &= ~0x0C;
    val |= ((times & 0x03) << 2);
    Pmic_SetSramRegister(val);

    return 0;
}

//bit 2-3: boot fail times
//bit 1: factory reset
//bit 0: hard_reset
//xxxxxxxx
int Pmic_SetSramRegister(int val)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->set_sram_reg == NULL) {
        return -1;
    }
    pmic_object->set_sram_reg(val);
    return 0;
}

int Pmic_GetSramRegister(void)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->get_sram_reg == NULL) {
        return -1;
    }
    return pmic_object->get_sram_reg();
}

int Pmic_NormalSoftReset(void)
{
    Pmic_SetBootFailTimes(0);
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
    if (pmic_object != NULL && pmic_object->set_softreset_flag != NULL) {
        pmic_object->set_softreset_flag();
    }
    if (pmic_object != NULL && pmic_object->reset != NULL) {
        pmic_object->reset();
    }

    return 0;
}

int Pmic_SoftReset(void)
{
    i2c_write_reg(LED_I2C_CHANNEL, LED_I2C_SLAVE_ADDR, 0x00, 0x55);//reset led
    if (pmic_object != NULL && pmic_object->reset != NULL) {
        pmic_object->reset();
    }

    return 0;
}

int Pmic_Read(unsigned char addr, unsigned char *data)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->read == NULL) {
        return -1;
    }
    pmic_object->read(addr, data);
    return 0;
}

int Pmic_Write(unsigned char addr, unsigned char data)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->write == NULL) {
        return -1;
    }
    pmic_object->write(addr, data);
    return 0;
}

int Pmic_Modify(unsigned char addr, unsigned char mask, unsigned char data)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->modify == NULL) {
        return -1;
    }
    pmic_object->modify(addr, mask, data);
    return 0;
}

boot_reason_e Pmic_GetBootReason(void)
{
    static int reason = -1;

    if (reason != -1) {
        return reason;
    }
    if (pmic_object != NULL) {
        reason = pmic_object->get_boot_reason();
    }

    return reason;
}

int Pmic_GetVersion(char *version)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->get_version == NULL) {
        return -1;
    }
    return pmic_object->get_version(version);
}

int Pmic_GetAccState(void)
{
    CHECK_PMIC_OBJECT;
    if (pmic_object->get_acc_state == NULL) {
        return -1;
    }
    return pmic_object->get_acc_state();
}

