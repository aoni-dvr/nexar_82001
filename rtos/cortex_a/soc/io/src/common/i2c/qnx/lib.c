/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include "proto.h"

int i2c_master_getfuncs(i2c_master_funcs_t *funcs, int tabsize)
{
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 init, Amba_I2c_init, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 fini, Amba_I2c_fini, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 send, Amba_I2c_send, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 recv, Amba_I2c_recv, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 abort, Amba_I2c_Abort, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 set_slave_addr, Amba_I2c_SetSlaveAddr, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 set_bus_speed, Amba_I2c_SetSpeed, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 version_info, Amba_I2C_VerInfo, tabsize);
    I2C_ADD_FUNC(i2c_master_funcs_t, funcs,
                 driver_info, Amba_I2C_DriverInfo, tabsize);
    return 0;
}
