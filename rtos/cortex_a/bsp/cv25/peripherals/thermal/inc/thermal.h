#ifndef __THERMAL_H__
#define __THERMAL_H__

#include "platform.h"

typedef struct _THERMAL_DATA_s_ {
    int value;
} THERMAL_DATA_s;

typedef struct _THERMAL_OBJECT_s_ {
    char *name;
    int (*init)(void);
    int (*read)(unsigned char, unsigned char *);
    int (*write)(unsigned char, unsigned char);
    int (*modify)(unsigned char, unsigned char, unsigned char);
    int (*get_data)(THERMAL_DATA_s *);    
} THERMAL_OBJECT_s;

int Thermal_Init(void);
int Thermal_Read(unsigned char addr, unsigned char *data);
int Thermal_Write(unsigned char addr, unsigned char data);
int Thermal_Modify(unsigned char addr, unsigned char mask, unsigned char data);
int Thermal_GetData(THERMAL_DATA_s *data);
#endif

