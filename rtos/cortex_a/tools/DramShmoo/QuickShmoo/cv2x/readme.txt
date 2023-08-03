The Shmoo process keep changing the target DRAM parameter values until system becomes 
unstable or even hangs.
Then we can know the workable minimum and maximum of each parameter. Based on the
 range information, we can select the proper setting with enough margin to be
 the base values in the LPDDR4 ASM file. It is suggested that customer could
 add proper loading to generate as much DRAM traffic as possible in the system
 when doing the Shmoo process to simulate the real usage scenario.

The Shmoo process would change the following DRAM related parameters:
pad-vref:       VREF on DDRC side, for READ
DqReadDly:      coarse read timing delay for DQ bus, for READ
DqWriteDly:     coarse write timing delay for DQ bus, for WRITE
lpddr4-dq-vref: VREF for DQ bus on DRAM side, for LPDDR4 only, for WRITE
lpddr4-ca-vref: VREF for CA bus on DRAM side, for LPDDR4 only, for WRITE

The DRAM Shmoo is triggered by diag command supported in IOUT app in Ambarella SDK. 
While using different app, please port the diag command from IOUT app. The source code 
of diag command locates at rtos/cortex_a/svc/apps/iout/src/threadx/diag/common/AmbaIOUTDiag_CmdDDR.c 


We define three categories of Shmoo here. The syntax of the diag command under
 each categories are listed. Ambarella suggests to run the Shmoo on 5 sets during
 bring up stage if possible.

Source code: rtos/cortex_a/svc/apps/iout/src/threadx/diag/common/AmbaIOUTDiag_CmdDDR.c
Shmoo command: "diag ddr" (Port from IOUT app if one’s app doesn't support this command)
    Parameters:
        [0:DDRC0|1:DDRC1]: Mandatory parameter specifying the target DDRC controller to perform the Shmoo.
                           Some chips have only one controller and some have two.
        [+|-]: Manatory parameter specifying the Shmoo direction. "+" means increasing. "-" mean decreasing.
        [time]: Optional parameter specifying the interval before switching to next Shmoo value
                default value = 20000 (wait 20000 milliseconds on each value)
        [step]: Optional parameter specifying the step when switching to next Shmoo value
                default value = 1 (increase 1 or decrease 1 on each step)
        [bitmask]: Optional parameter specifiying which bit to perform the Shmoo.
                   Some chips support per-bit setting and some support per-byte or per-controller.
                   default value = 0xffffffff (Shmoo 32-bits together)

Three categories: Read, Write, DLL

Read Shmoo:
-   Purpose: find out the workable range of DqReadDly and Vref (pad-vref, Controller side) 
-   Procedure:
    •   Find the range of Vref (Controller side)
        For LPDDR4:
        command: diag ddr lp4vref [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo DQ Vref with increment direction on CV2 DDRC0
            a:\> diag ddr lp4vref 0 + 30000
            ...
            [DDRC0][DQ Vref0]TrainDqVref = 0x02C58B16 (value = 0x16)
            [DDRC0][DQ Vref1]TrainDqVref = 0x02C58B16 (value = 0x16)
            [DDRC0][DQ Vref0]TrainDqVref = 0x02C58B97 (value = 0x17)
            [DDRC0][DQ Vref1]TrainDqVref = 0x02E5CB97 (value = 0x17)
            [DDRC0][DQ Vref0]TrainDqVref = 0x02E5CC18 (value = 0x18)
            [DDRC0][DQ Vref1]TrainDqVref = 0x03060C18 (value = 0x18)
            [DDRC0][DQ Vref0]TrainDqVref = 0x03060C99 (value = 0x19)
            [DDRC0][DQ Vref1]TrainDqVref = 0x03264C99 (value = 0x19)
            [DDRC0][DQ Vref0]TrainDqVref = 0x03264D1A (value = 0x1A)
            [DDRC0][DQ Vref1]TrainDqVref = 0x03468D1A (value = 0x1A)
            ...
            Output message description:
                TrainDqVref holds Vref setting for DQ Vref of byte 0~3.
                TrainDqVref[13:0]  - DQ Vref0 (Byte 0/1)
                TrainDqVref[27:14] - DQ Vref1 (Byte 2/3)
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte. So, please pick the register setting from the last line of the selected value.
                That is, pick the register value of [DQ Vref1]TrainDqVref for byte 0 ~ 3.
                The registers are mapped to DdrcX_DRAM_lpddr4DqCaVrefParam(CV2) or DRAM_lpddr4DqCaVrefParam(others) in ASM file. (where X could be 0 or 1)
    •   Find the range of DqReadDly (Controller side)
        For LPDDR4:
        command: diag ddr lp4rdly [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo Read Delay with increment direction on CV2 DDRC0
            a:\> diag ddr lp4rdly 0 + 30000
            ...
            [DDRC0][Read Delay0]TrainDqReadDly = 0x0008D5B5 (value = 0xA)
            [DDRC0][Read Delay1]TrainDqReadDly = 0x0008D6B5 (value = 0xA)
            [DDRC0][Read Delay2]TrainDqReadDly = 0x0008D6B5 (value = 0xA)
            [DDRC0][Read Delay3]TrainDqReadDly = 0x000AD6B5 (value = 0xA)
            [DDRC0][Read Delay0]TrainDqReadDly = 0x000AD6B7 (value = 0xB)
            [DDRC0][Read Delay1]TrainDqReadDly = 0x000AD6F7 (value = 0xB)
            [DDRC0][Read Delay2]TrainDqReadDly = 0x000ADEF7 (value = 0xB)
            [DDRC0][Read Delay3]TrainDqReadDly = 0x000BDEF7 (value = 0xB)
            [DDRC0][Read Delay0]TrainDqReadDly = 0x000BDEF9 (value = 0xC)
            [DDRC0][Read Delay1]TrainDqReadDly = 0x000BDF39 (value = 0xC)
            [DDRC0][Read Delay2]TrainDqReadDly = 0x000BE739 (value = 0xC)
            [DDRC0][Read Delay3]TrainDqReadDly = 0x000CE739 (value = 0xC)
            ...
            Output message description:
                TrainDqReadDly holds Read Delay setting for DQ of byte 0~3.
                TrainDqReadDly[4:0]   - DQ read delay byte 0
                TrainDqReadDly[9:5]   - DQ read delay byte 1
                TrainDqReadDly[14:10] - DQ read delay byte 2
                TrainDqReadDly[19:15] - DQ read delay byte 3
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte. So, please pick the register setting from the last line of the selected value.
                That is, pick the register value of [Read Delay3]TrainDqReadDly for byte 0 ~ 3.
                The registers are mapped to DdrcX_DRAM_lpddr4DqReadDlyParam(CV2) or DRAM_lpddr4DqReadDlyParam(others) in ASM file. (where X could be 0 or 1)
-   Output:
    •   Vref register ranges        (DdrcX_DRAM_lpddr4DqCaVrefParam(CV2) or DRAM_lpddr4DqCaVrefParam(others) in ASM file)
    •   DqReadDly register ranges   (DdrcX_DRAM_lpddr4DqReadDlyParam(CV2) or DRAM_lpddr4DqReadDlyParam(others) in ASM file)


Write Shmoo:
-   Purpose: find out the workable range of DqWriteDly and Vref (lpddr4-dq-vref, lpddr4-ca-vref, Device side)
-   Procedure:
    •   Find the range of Vref (Device side)
        For LPDDR4:
        command: diag ddr lp4mr14 [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo DQ Vref of DRAM with increment direction on CV2 DDRC0
            a:\> diag ddr lp4mr14 0 + 30000
            ...
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E001A (value = 0x1A)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E001B (value = 0x1B)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E001C (value = 0x1C)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E001D (value = 0x1D)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E001E (value = 0x1E)
            ...
            Output message description:
                Lpddr4DqVref represents the Vref setting for DQ Vref of DRAM's MR14 (Mode Register 14).
                Lpddr4DqVref[7:0]  - DQ Vref setting
                Lpddr4DqVref[31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to DdrcX_LPDDR4_ModeReg14ParamFspY(CV2) or LPDDR4_ModeReg14ParamFspX(others) bit[7:0] in ASM file.
                (where X/Y could be 0 or 1)
                Please change both Fsp together in ASM file. (Fsp0/Fsp1)
        command: diag ddr lp4mr12 [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo CA Vref of DRAM with increment direction on CV2 DDRC0
            a:\> diag ddr lp4mr12 0 + 30000
            ...
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C0028 (value = 0x28)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C0029 (value = 0x29)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C002A (value = 0x2A)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C002B (value = 0x2B)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C002C (value = 0x2C)
            ...
            Output message description:
                Lpddr4CaVref represents the Vref setting for CA Vref of DRAM's MR12 (Mode Register 12).
                Lpddr4CaVref[7:0]  - CA Vref setting
                Lpddr4CaVref[31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to DdrcX_LPDDR4_ModeReg12ParamFspY(CV2) or LPDDR4_ModeReg12ParamFspX(others) bit[7:0] in ASM file.
                (where X/Y could be 0 or 1)
                Please change both Fsp together in ASM file. (Fsp0/Fsp1)
    •   Find the range of DqWriteDly (Controller side)
        For LPDDR4:
        command: diag ddr lp4wdly [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo Write Delay with increment direction on CV2 DDRC0
            a:\> diag ddr lp4wdly 0 + 30000
            ...
            [DDRC0][Write Delay0]TrainWriteDly = 0xC4E9D4A9 (value = 0x29)
            [DDRC0][Write Delay1]TrainWriteDly = 0xC4E9D4A9 (value = 0x29)
            [DDRC0][Write Delay2]TrainWriteDly = 0xC4EA54A9 (value = 0x29)
            [DDRC0][Write Delay3]TrainWriteDly = 0xC52A54A9 (value = 0x29)
            [DDRC0][Write Delay0]TrainWriteDly = 0xC52A54AA (value = 0x2A)
            [DDRC0][Write Delay1]TrainWriteDly = 0xC52A552A (value = 0x2A)
            [DDRC0][Write Delay2]TrainWriteDly = 0xC52A952A (value = 0x2A)
            [DDRC0][Write Delay3]TrainWriteDly = 0xC54A952A (value = 0x2A)
            [DDRC0][Write Delay0]TrainWriteDly = 0xC54A952B (value = 0x2B)
            [DDRC0][Write Delay1]TrainWriteDly = 0xC54A95AB (value = 0x2B)
            [DDRC0][Write Delay2]TrainWriteDly = 0xC54AD5AB (value = 0x2B)
            [DDRC0][Write Delay3]TrainWriteDly = 0xC56AD5AB (value = 0x2B)
            ...
            Output message description:
                TrainWriteDly holds Write Delay setting for DQ of byte 0~3.
                TrainWriteDly[6:0]   - DQ write delay byte 0
                TrainWriteDly[13:7]  - DQ write delay byte 1
                TrainWriteDly[20:14] - DQ write delay byte 2
                TrainWriteDly[27:21] - DQ write delay byte 3
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte. So, please pick the register setting from the last line of the selected value.
                That is, pick the register value of [Write Delay3]TrainWriteDly for byte 0 ~ 3.
                The registers are mapped to DdrcX_DRAM_lpddr4DqWriteDlyParam(CV2) or DRAM_lpddr4DqWriteDlyParam(others) in ASM file. (where X could be 0 or 1)
-   Output:
    •   Vref register (ModeReg14/ModeReg12) ranges  (DdrcX_LPDDR4_ModeReg14ParamFspY(CV2) or LPDDR4_ModeReg14ParamFspX(others)/
                                                     DdrcX_LPDDR4_ModeReg12ParamFspY(CV2) or LPDDR4_ModeReg12ParamFspX(others) in ASM file)
    •   DqWriteDly register ranges                  (DdrcX_DRAM_lpddr4DqWriteDlyParam(CV2) or DRAM_lpddr4DqWriteDlyParam(others) in ASM file)
    •   The DdrcX_LPDDR4_ModeReg??ParamFspY(CV2) or LPDDR4_ModeReg??ParamFspX(others) in ASM file only takes bit[7:0] as the register setting. 
        Bit[31:8] are access pattern which should NOT be changed.


DLL Shmoo:
-   Purpose: find out the workable range of DLL.
-   Procedure:
    •   shmoo DLL value of sync/read/write one by one to find out their ranges.
        command: diag ddr shmoo [0:DDRC0|1:DDRC1] [sync|read|write] [+|-] [time(ms)]
            Example: Shmoo Read DLL with increment direction on CV2 DDRC0
            a:\> diag ddr shmoo 0 read + 30000
            ...
            [DDRC0][DLL0]Reg = 0x202003
            [DDRC0][DLL1]Reg = 0x202003
            [DDRC0][DLL2]Reg = 0x202003
            [DDRC0][DLL3]Reg = 0x202003
            [DDRC0][DLL0]Reg = 0x202004
            [DDRC0][DLL1]Reg = 0x202004
            [DDRC0][DLL2]Reg = 0x202004
            [DDRC0][DLL3]Reg = 0x202004
            [DDRC0][DLL0]Reg = 0x202005
            [DDRC0][DLL1]Reg = 0x202005
            [DDRC0][DLL2]Reg = 0x202005
            [DDRC0][DLL3]Reg = 0x202005
            ...
            Output message description:
                [DLLX]Reg holds DLL setting of sync/read/write for byte X. (Where X could be 0,1,2 or 3)
                [DLLX]Reg[5:0]   - DLL setting for read  (Byte X)
                [DLLX]Reg[13:8]  - DLL setting for sync  (Byte X)
                [DLLX]Reg[21:16] - DLL setting for write (Byte X)
                The msb (bit[5]) of each byte is a sign bit. Sign bit 0 means the positive value and 1 represents the negative value.
                So 6 bits register value from the minimum value to the maximum value are:
                0x3f = -31, 0x3e = -30, ..., 0x22 = -2, 0x21 = -1, 0x20 = -0, 0x0 = 0, 0x1 = 1, ..., 0x1e = 30, 0x1f = 31.
                DLL has an extra lock range boundary which only the values inside the lock range are meaningful.
                The lock range will be printed when shmoo starts or can be determined by the following formula:
                    Lock_Range = (0 - DLL_Vfine) ~ (31 - DLL_Vfine) where DLL_Vfine can be retrieved from DLL_STATUS register
                Please adjust the minimum value and the maximum value to be within the lock range boundary.
                The average of the minimum value and the maximum value will allow the users to get the center value.
                The registers are mapped to DdrcX_DLL_SettingYParam(CV2) or DLL_SettingYParam(others) in ASM file. 
                (where X could be 0 or 1, Y could be 0,1,2 or 3)
            Example: Shmoo Write DLL with decrement direction on CV2 DDRC0
            a:\> diag ddr shmoo 0 write - 30000
            ...
            [DDRC0][DLL0]Reg = 0x202003
            [DDRC0][DLL1]Reg = 0x202003
            [DDRC0][DLL2]Reg = 0x202003
            [DDRC0][DLL3]Reg = 0x202003
            [DDRC0][DLL0]Reg = 0x212003
            [DDRC0][DLL1]Reg = 0x212003
            [DDRC0][DLL2]Reg = 0x212003
            [DDRC0][DLL3]Reg = 0x212003
            [DDRC0][DLL0]Reg = 0x222003
            [DDRC0][DLL1]Reg = 0x222003
            [DDRC0][DLL2]Reg = 0x222003
            [DDRC0][DLL3]Reg = 0x222003
            ...
            Output message description:
                [DLLX]Reg holds DLL setting of sync/read/write for byte X. (Where X could be 0,1,2 or 3)
                [DLLX]Reg[5:0]   - DLL setting for read  (Byte X)
                [DLLX]Reg[13:8]  - DLL setting for sync  (Byte X)
                [DLLX]Reg[21:16] - DLL setting for write (Byte X)
                The msb (bit[5]) of DLL setting is a sign bit. Sign bit 0 means the positive value and 1 represents the negative value.
                So 6 bits register value from the minimum value to the maximum value are:
                0x3f = -31, 0x3e = -30, ..., 0x22 = -2, 0x21 = -1, 0x20 = -0, 0x0 = 0, 0x1 = 1, ..., 0x1e = 30, 0x1f = 31.
                DLL has an extra lock range boundary which only the values inside the lock range are meaningful.
                The lock range will be printed when shmoo starts or can be determined by the following formula:
                    Lock_Range = (0 - DLL_Vfine) ~ (31 - DLL_Vfine) where DLL_Vfine can be retrieved from DLL_STATUS register
                Please adjust the minimum value and the maximum value to be within the lock range boundary.
                The average of the minimum value and the maximum value will allow the users to get the center value.
                The registers are mapped to DdrcX_DLL_SettingYParam(CV2) or DLL_SettingYParam(others) in ASM file. 
                (where X could be 0 or 1, Y could be 0,1,2 or 3)
-   Output:
    •   DLL register ranges (DdrcX_DLL_SettingYParam(CV2) or DLL_SettingYParam(others) in ASM file)
    •   The DdrcX_DLL_SettingYParam(CV2) or DLL_SettingYParam(others) in ASM file is shared for sync/read and write DLL. 
        Read DLL takes bit[5:0]. Sync DLL takes bit[13:8]. Write DLL occupies bit[21:16].
        If one wants to change sync/read/write DLL at the same time, one has to merge it manually from 
        the DLL register value of Shmoo output of sync/read/write DLL.

