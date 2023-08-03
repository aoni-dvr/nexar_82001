#include <thermal.h>

static THERMAL_OBJECT_s *thermal_object = NULL;

#define CHECK_THERMAL_OBJECT {if (thermal_object == NULL) {debug_line("THERMAL No Register");return -1;}}
static void thermal_register(THERMAL_OBJECT_s *thermal_obj)
{
    thermal_object = thermal_obj;
    if (thermal_object != NULL) {
        debug_line("%s: %s", __func__, thermal_object->name);
    } else {
        debug_line("%s: No PMU", __func__);
    }
}

int Thermal_Init(void)
{
#if defined(CONFIG_THERMAL_TMP103)
    extern THERMAL_OBJECT_s thermal_tmp103_object;
    thermal_register(&thermal_tmp103_object);
#endif

    if (thermal_object->init != NULL) {
        thermal_object->init();
    }
    return 0;
}

int Thermal_Read(unsigned char addr, unsigned char *data)
{
    CHECK_THERMAL_OBJECT;
    if (thermal_object->read == NULL) {
        return -1;
    }
    thermal_object->read(addr, data);
    return 0;
}

int Thermal_Write(unsigned char addr, unsigned char data)
{
    CHECK_THERMAL_OBJECT;
    if (thermal_object->write == NULL) {
        return -1;
    }
    thermal_object->write(addr, data);
    return 0;
}

int Thermal_Modify(unsigned char addr, unsigned char mask, unsigned char data)
{
    CHECK_THERMAL_OBJECT;
    if (thermal_object->modify == NULL) {
        return -1;
    }
    thermal_object->modify(addr, mask, data);
    return 0;
}

int Thermal_GetData(THERMAL_DATA_s *data)
{
    CHECK_THERMAL_OBJECT;
    if (thermal_object->get_data == NULL) {
        return -1;
    }
    thermal_object->get_data(data);
    return 0;
}

