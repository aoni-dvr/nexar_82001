The Shmoo process keep changing the target DRAM parameter values until system becomes 
unstable or even hangs.
Then we can know the workable minimum and maximum of each parameter. Based on the
 range information, we can select the proper setting with enough margin to be
 the base values in the LPDDR4/LPDDR5 ASM file. It is suggested that customer could
 add proper loading to generate as much DRAM traffic as possible in the system
 when doing the Shmoo process to simulate the real usage scenario.

The Shmoo process would change the following DRAM related parameters:
pad-vref:       VREF on DDRC side, for READ
DqReadDly:      coarse read timing delay for DQ bus, for READ
DqWriteDly:     coarse write timing delay for DQ bus, for WRITE
lpddr4-dq-vref: VREF for DQ bus on DRAM side, for LPDDR4 only, for WRITE
lpddr4-ca-vref: VREF for CA bus on DRAM side, for LPDDR4 only, for WRITE
lpddr5-dq-vref: VREF for DQ bus on DRAM side, for LPDDR5 only, for WRITE
lpddr5-ca-vref: VREF for CA bus on DRAM side, for LPDDR5 only, for WRITE

The DRAM Shmoo is triggered by diag command supported in IOUT app in Ambarella SDK. 
While using different app, please enable the DRAM Shmoo function in common service 
(CONFIG_BUILD_COMSVC_DRAMSHMOO=y). The source code of DRAM Shmoo function locates 
at rtos/cortex_a/svc/comsvc/dramshmoo/cv5x/AmbaDiag_CmdDDR.c.


We define three categories of Shmoo here. The syntax of the diag command under
 each categories are listed. Ambarella suggests to run the Shmoo on 5 sets during
 bring up stage if possible.

Source code: rtos/cortex_a/svc/comsvc/dramshmoo/cv5x/AmbaDiag_CmdDDR.c
Shmoo command: "diag ddr" (or "diag_ddr" in iCAM app, Port from IOUT app if one’s app doesn't support this command)
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
        For LPDDR4/LPDDR5:
        command: diag ddr lp4vref [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo DQ Vref with increment direction on CV5 DDRC0
            a:\> diag ddr lp4vref 0 + 30000
            ...
            [DDRC0][DQ Vref0]TrainDqVref0 = 0x03180318 (value = 0x18)
            [DDRC0][DQ Vref1]TrainDqVref0 = 0x03180318 (value = 0x18)
            [DDRC0][DQ Vref2]TrainDqVref1 = 0x03180318 (value = 0x18)
            [DDRC0][DQ Vref3]TrainDqVref1 = 0x03180318 (value = 0x18)
            [DDRC0][DQ Vref0]TrainDqVref0 = 0x03180319 (value = 0x19)
            [DDRC0][DQ Vref1]TrainDqVref0 = 0x03190319 (value = 0x19)
            [DDRC0][DQ Vref2]TrainDqVref1 = 0x03180319 (value = 0x19)
            [DDRC0][DQ Vref3]TrainDqVref1 = 0x03190319 (value = 0x19)
            [DDRC0][DQ Vref0]TrainDqVref0 = 0x0319031A (value = 0x1A)
            [DDRC0][DQ Vref1]TrainDqVref0 = 0x031A031A (value = 0x1A)
            [DDRC0][DQ Vref2]TrainDqVref1 = 0x0319031A (value = 0x1A)
            [DDRC0][DQ Vref3]TrainDqVref1 = 0x031A031A (value = 0x1A)
            ...
            Output message description:
                TrainDqVref0 and TrainDqVref1 are two registers which hold Vref setting for DQ Vref of byte 0~3.
                TrainDqVref0[5:0]   - DQ Vref0 (Byte 0)
                TrainDqVref0[21:16] - DQ Vref1 (Byte 1)
                TrainDqVref1[5:0]   - DQ Vref2 (Byte 2)
                TrainDqVref1[21:16] - DQ Vref3 (Byte 3)
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte. So, please pick the register setting from the last line of the selected value.
                That is, pick the register value of [DQ Vref1]TrainDqVref0 for byte 0 and 1, pick [DQ Vref3]TrainDqVref1 for byte 2 and 3.
                The registers are mapped to DRAM_VrefXParam_HostY in ASM file (where X/Y could be 0 or 1)
        command: diag ddr dqsvref [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo DQS Vref with increment direction on CV5 DDRC0
            a:\> diag ddr dqsvref 0 + 30000
            ...
            [DDRC0][DQS Vref0]TrainDqVref0 = 0x03180318 (value = 0xC)
            [DDRC0][DQS Vref1]TrainDqVref0 = 0x03180318 (value = 0xC)
            [DDRC0][DQS Vref2]TrainDqVref1 = 0x03180318 (value = 0xC)
            [DDRC0][DQS Vref3]TrainDqVref1 = 0x03180318 (value = 0xC)
            [DDRC0][DQS Vref0]TrainDqVref0 = 0x03180358 (value = 0xD)
            [DDRC0][DQS Vref1]TrainDqVref0 = 0x03580358 (value = 0xD)
            [DDRC0][DQS Vref2]TrainDqVref1 = 0x03180358 (value = 0xD)
            [DDRC0][DQS Vref3]TrainDqVref1 = 0x03580358 (value = 0xD)
            [DDRC0][DQS Vref0]TrainDqVref0 = 0x03580398 (value = 0xE)
            [DDRC0][DQS Vref1]TrainDqVref0 = 0x03980398 (value = 0xE)
            [DDRC0][DQS Vref2]TrainDqVref1 = 0x03580398 (value = 0xE)
            [DDRC0][DQS Vref3]TrainDqVref1 = 0x03980398 (value = 0xE)
            ...
            Output message description:
                TrainDqVref0 and TrainDqVref1 are two registers which hold Vref setting for DQS Vref of byte 0~3.
                TrainDqVref0[11:6]  - DQS Vref0 (Byte 0)
                TrainDqVref0[27:22] - DQS Vref1 (Byte 1)
                TrainDqVref1[11:6]  - DQS Vref2 (Byte 2)
                TrainDqVref1[27:22] - DQS Vref3 (Byte 3)
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte. So, please pick the register setting from the last line of the selected value.
                That is, pick the register value of [DQS Vref1]TrainDqVref0 for byte 0 and 1, pick [DQS Vref3]TrainDqVref1 for byte 2 and 3.
                The registers are mapped to DRAM_VrefXParam_HostY in ASM file. (where X/Y could be 0 or 1)
    •   Find the range of DqReadDly (Controller side)
        For LPDDR4/LPDDR5:
        command: diag ddr lp4rdly [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo Read Delay with increment direction on CV5 DDRC0
            a:\> diag ddr lp4rdly 0 + 30000
            ...
            [DDRC0][DqRead Delay0]Delay1,4,5 = 0x10512C00, 0x08080808, 0x08080808 (value = 0x20)
            [DDRC0][DqRead Delay1]Delay1,4,5 = 0x10512C00, 0x08080808, 0x08080808 (value = 0x20)
            [DDRC0][DqRead Delay2]Delay1,4,5 = 0x10512C00, 0x08080808, 0x08080808 (value = 0x20)
            [DDRC0][DqRead Delay3]Delay1,4,5 = 0x10512C00, 0x08080808, 0x08080808 (value = 0x20)
            [DDRC0][DqRead Delay0]Delay1,4,5 = 0x12512C00, 0x09090909, 0x09090909 (value = 0x21)
            [DDRC0][DqRead Delay1]Delay1,4,5 = 0x12512C00, 0x09090909, 0x09090909 (value = 0x21)
            [DDRC0][DqRead Delay2]Delay1,4,5 = 0x12512C00, 0x09090909, 0x09090909 (value = 0x21)
            [DDRC0][DqRead Delay3]Delay1,4,5 = 0x12512C00, 0x09090909, 0x09090909 (value = 0x21)
            [DDRC0][DqRead Delay0]Delay1,4,5 = 0x14512C00, 0x0A0A0A0A, 0x0A0A0A0A (value = 0x22)
            [DDRC0][DqRead Delay1]Delay1,4,5 = 0x14512C00, 0x0A0A0A0A, 0x0A0A0A0A (value = 0x22)
            [DDRC0][DqRead Delay2]Delay1,4,5 = 0x14512C00, 0x0A0A0A0A, 0x0A0A0A0A (value = 0x22)
            [DDRC0][DqRead Delay3]Delay1,4,5 = 0x14512C00, 0x0A0A0A0A, 0x0A0A0A0A (value = 0x22)
            ...
            Output message description:
                Delay1, Delay4 and Delay5 are three registers which hold Read Delay setting for DQ/DM for each byte.
                Each byte has it's own Delay1/Delay4/Delay5 registers.
                Delay1[19:15] - DQ read delay coarse   (Coarse delay is shared by DM and DQ bit 0~7)
                Delay1[29:25] - DM read delay fine
                Delay4[31:0]  - DQ read delay fine bit 0~3
                Delay5[31:0]  - DQ read delay fine bit 4~7
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte. The shmoo process will change the register value of both dies.
                The registers are mapped to DRAM_DieXByteYDly1Param_HostZ/DRAM_DieXByteYDly4Param_HostZ/DRAM_DieXByteYDly5Param_HostZ in ASM file.
                (where X could be 0 or 1, Y could be 0, 1, 2 or 3, Z could be 0 or 1)
                Please change both dies together in ASM file. (Die0/Die1)
-   Output:
    •   Vref register ranges        (DRAM_VrefXParam_HostY in ASM file)
    •   DqReadDly register ranges   (DRAM_DieXByteYDly1Param_HostZ/DRAM_DieXByteYDly4Param_HostZ/DRAM_DieXByteYDly5Param_HostZ in ASM file)
    •   The DRAM_VrefXParam_HostY in ASM file is shared for both DQ Vref and DQS Vref 
        DQ Vref takes bit[21:16] and bit[5:0]. DQS Vref occupies bit[27:22] and bit[11:6].
        If one wants to change DQ/DQS Vref in the same time, one has to merge it manually from the TrainDqVref0/TrainDqVref1 register value of Shmoo output.
    •   The DRAM_DieXByteYDly1Param_HostZ in ASM file is shared for both read delay and write delay. 
        Read delay takes bit[29:25] and bit[19:15]. Write delay occupies bit[24:20] and bit[14:10].
        If one wants to change read/write delay in the same time, one has to merge it manually from the Dly1 register value of Shmoo output of read/write delay.


Write Shmoo:
-   Purpose: find out the workable range of DqWriteDly and Vref (lpddr4-dq-vref, lpddr4-ca-vref, lpddr5-dq-vref, lpddr5-ca-vref, Device side)
-   Procedure:
    •   Find the range of Vref (Device side)
        For LPDDR4:
        command: diag ddr lp4mr14 [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo DQ Vref of DRAM with increment direction on CV5 DDRC0
            a:\> diag ddr lp4mr14 0 + 30000
            ...
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x1A0E3232 (value = 0x32)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x1A0E5555 (value = 0x33)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x1A0E5656 (value = 0x34)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x1A0E5757 (value = 0x35)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x1A0E5858 (value = 0x36)
            ...
            Output message description:
                Lpddr4DqVref represents the Vref setting for DQ Vref of DRAM's MR14 (Mode Register 14).
                Lpddr4DqVref[7:0]  - DQ Vref setting
                Lpddr4DqVref[31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value of both dies. Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to LPDDR4_ModeReg14ParamCXDY_HostZ bit[7:0] in ASM file. (where X/Y/Z could be 0 or 1)
                Please change both channels/dies together in ASM file. (C0D0/C0D1/C1D0/C1D1)
        command: diag ddr lp4mr12 [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo CA Vref of DRAM with increment direction on CV5 DDRC0
            a:\> diag ddr lp4mr12 0 + 30000
            ...
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x1A0C3232 (value = 0x32)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x1A0C5555 (value = 0x33)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x1A0C5656 (value = 0x34)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x1A0C5757 (value = 0x35)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x1A0C5858 (value = 0x36)
            ...
            Output message description:
                Lpddr4CaVref represents the Vref setting for CA Vref of DRAM's MR12 (Mode Register 12).
                Lpddr4CaVref[7:0]  - CA Vref setting
                Lpddr4CaVref[31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value of both dies. Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to LPDDR4_ModeReg12ParamCXDY_HostZ bit[7:0] in ASM file. (where X/Y/Z could be 0 or 1)
                Please change both channels/dies together in ASM file. (C0D0/C0D1/C1D0/C1D1)
        For LPDDR5:
        command: diag ddr lp5mr14 [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo DQ Vref of DRAM with increment direction on CV5 DDRC1
            a:\> diag ddr lp5mr14 1 + 30000
            ...
            [LPDDR5_1][MR14 Vref]Dq[7:0] = 0x1A0E3232 (value = 0x32)
            [LPDDR5_1][MR14 Vref]Dq[7:0] = 0x1A0E3333 (value = 0x33)
            [LPDDR5_1][MR14 Vref]Dq[7:0] = 0x1A0E3434 (value = 0x34)
            [LPDDR5_1][MR14 Vref]Dq[7:0] = 0x1A0E3535 (value = 0x35)
            [LPDDR5_1][MR14 Vref]Dq[7:0] = 0x1A0E3636 (value = 0x36)
            ...
            Output message description:
                Dq[7:0] represents the Vref setting for DQ Vref bit7~0 of DRAM's MR14 (Mode Register 14).
                Dq[7:0][7:0]  - DQ Vref setting
                Dq[7:0][31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value of both dies. Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to LPDDR5_ModeReg14ParamCXDY_HostZ bit[7:0] in ASM file. (where X/Y/Z could be 0 or 1)
                Please change both channels/dies together in ASM file. (C0D0/C0D1/C1D0/C1D1)
        command: diag ddr lp5mr15 [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo DQ Vref of DRAM with increment direction on CV5 DDRC1
            a:\> diag ddr lp5mr15 1 + 30000
            ...
            [LPDDR5_1][MR15 Vref]Dq[15:8] = 0x1A0F3232 (value = 0x32)
            [LPDDR5_1][MR15 Vref]Dq[15:8] = 0x1A0F3333 (value = 0x33)
            [LPDDR5_1][MR15 Vref]Dq[15:8] = 0x1A0F3434 (value = 0x34)
            [LPDDR5_1][MR15 Vref]Dq[15:8] = 0x1A0F3535 (value = 0x35)
            [LPDDR5_1][MR15 Vref]Dq[15:8] = 0x1A0F3636 (value = 0x36)
            ...
            Output message description:
                Dq[15:8] represents the Vref setting for DQ Vref bit15~8 of DRAM's MR15 (Mode Register 15).
                Dq[15:8][7:0]  - DQ Vref setting
                Dq[15:8][31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value of both dies. Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to LPDDR5_ModeReg15ParamCXDY_HostZ bit[7:0] in ASM file. (where X/Y/Z could be 0 or 1)
                Please change both channels/dies together in ASM file. (C0D0/C0D1/C1D0/C1D1)
        command: diag ddr lp5mr12 [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo CA Vref of DRAM with increment direction on CV5 DDRC0
            a:\> diag ddr lp5mr12 0 + 30000
            ...
            [LPDDR5_0][MR12 Vref]Ca = 0x1A0C3232 (value = 0x32)
            [LPDDR5_0][MR12 Vref]Ca = 0x1A0C3333 (value = 0x33)
            [LPDDR5_0][MR12 Vref]Ca = 0x1A0C3434 (value = 0x34)
            [LPDDR5_0][MR12 Vref]Ca = 0x1A0C3535 (value = 0x35)
            [LPDDR5_0][MR12 Vref]Ca = 0x1A0C3636 (value = 0x36)
            ...
            Output message description:
                Ca represents the Vref setting for CA Vref of DRAM's MR12 (Mode Register 12).
                Ca[7:0]  - CA Vref setting
                Ca[31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value of both dies. Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to LPDDR5_ModeReg12ParamCXDY_HostZ bit[7:0] in ASM file. (where X/Y/Z could be 0 or 1)
                Please change both channels/dies together in ASM file. (C0D0/C0D1/C1D0/C1D1)
    •   Find the range of DqWriteDly (Controller side)
        For LPDDR4/LPDDR5:
        command: diag ddr lp4wdly [0:DDRC0|1:DDRC1] [+|-] [time(ms)]
            Example: Shmoo Write Delay with increment direction on CV5 DDRC0
            a:\> diag ddr lp4wdly 0 + 30000
            ...
            [DDRC0][DqWrite Delay0]Delay1,2,3 = 0x10512C00, 0x05050505, 0x05050505 (value = 0x89)
            [DDRC0][DqWrite Delay1]Delay1,2,3 = 0x10512C00, 0x05050505, 0x05050505 (value = 0x89)
            [DDRC0][DqWrite Delay2]Delay1,2,3 = 0x10512C00, 0x05050505, 0x05050505 (value = 0x89)
            [DDRC0][DqWrite Delay3]Delay1,2,3 = 0x10512C00, 0x05050505, 0x05050505 (value = 0x89)
            [DDRC0][DqWrite Delay0]Delay1,2,3 = 0x10612C00, 0x06060606, 0x06060606 (value = 0x8A)
            [DDRC0][DqWrite Delay1]Delay1,2,3 = 0x10612C00, 0x06060606, 0x06060606 (value = 0x8A)
            [DDRC0][DqWrite Delay2]Delay1,2,3 = 0x10612C00, 0x06060606, 0x06060606 (value = 0x8A)
            [DDRC0][DqWrite Delay3]Delay1,2,3 = 0x10612C00, 0x06060606, 0x06060606 (value = 0x8A)
            [DDRC0][DqWrite Delay0]Delay1,2,3 = 0x10712C00, 0x07070707, 0x07070707 (value = 0x8B)
            [DDRC0][DqWrite Delay1]Delay1,2,3 = 0x10712C00, 0x07070707, 0x07070707 (value = 0x8B)
            [DDRC0][DqWrite Delay2]Delay1,2,3 = 0x10712C00, 0x07070707, 0x07070707 (value = 0x8B)
            [DDRC0][DqWrite Delay3]Delay1,2,3 = 0x10712C00, 0x07070707, 0x07070707 (value = 0x8B)
            ...
            Output message description:
                Delay1, Delay2 and Delay3 are three registers which hold Write Delay setting for DQ/DM for each byte.
                Each byte has it's own Delay1/Delay4/Delay5 registers.
                Delay1[14:10] - DQ write delay coarse   (Coarse delay is shared by DM and DQ bit 0~7)
                Delay1[24:20] - DM write delay fine
                Delay2[31:0]  - DQ write delay fine bit 0~3
                Delay3[31:0]  - DQ write delay fine bit 4~7
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte. The shmoo process will change the register value of both dies.
                The registers are mapped to DRAM_DieXByteYDly1Param_HostZ/DRAM_DieXByteYDly2Param_HostZ/DRAM_DieXByteYDly3Param_HostZ in ASM file.
                (where X could be 0 or 1, Y could be 0, 1, 2 or 3, Z could be 0 or 1)
                Please change both dies together in ASM file. (Die0/Die1)
-   Output:
    •   Vref register (ModeReg14/ModeReg12/ModeReg15) ranges    (LPDDRW_ModeReg12ParamCXDY_HostZ/LPDDRW_ModeReg14ParamCXDY_HostZ/
                                                                 LPDDR5_ModeReg15ParamCXDY_HostZ in ASM file)
    •   DqWriteDly register ranges                              (DRAM_DieXByteYDly1Param_HostZ/DRAM_DieXByteYDly2Param_HostZ/
                                                                 DRAM_DieXByteYDly3Param_HostZ in ASM file)
    •   The LPDDRW_ModeReg??ParamCXDY_HostZ in ASM file only takes bit[7:0] as the register setting. Bit[31:8] are access pattern which should NOT be changed.
    •   The DRAM_DieXByteYDly1Param_HostZ in ASM file is shared for both read delay and write delay. 
        Read delay takes bit[29:25] and bit[19:15]. Write delay occupies bit[24:20] and bit[14:10].
        If one wants to change read/write delay in the same time, one has to merge it manually from the Dly1 register value of Shmoo output of read/write delay.


DLL Shmoo: (DLL Shmoo is less important or no longer required in CV5x and later chips)
-   Purpose: find out the workable range of DLL.
-   Procedure:
    •   shmoo DLL value of sync/read/write one by one to find out their ranges.
        command: diag ddr shmoo [0:DDRC0|1:DDRC1] [sync|read|write] [+|-] [time(ms)]
            Example: Shmoo Read DLL with increment direction on CV5 DDRC0
            a:\> diag ddr shmoo 0 read + 30000
            ...
            [DDRC0][DLL0]Reg = 0x22222222
            [DDRC0][DLL0]Reg = 0x21212121
            [DDRC0][DLL0]Reg = 0x20202020
            [DDRC0][DLL0]Reg = 0x00000000
            [DDRC0][DLL0]Reg = 0x01010101
            ...
            Output message description:
                [DLL0]Reg holds Read DLL setting for byte 0~3.
                [DLL0]Reg[5:0]   - Byte 0
                [DLL0]Reg[13:8]  - Byte 1
                [DLL0]Reg[21:16] - Byte 2
                [DLL0]Reg[29:24] - Byte 3
                The msb (bit[5]) of each DLL setting is a sign bit. Sign bit 0 means the positive value and 1 represents the negative value.
                So 6 bits register value from the minimum value to the maximum value are:
                0x3f = -31, 0x3e = -30, ..., 0x22 = -2, 0x21 = -1, 0x20 = -0, 0x0 = 0, 0x1 = 1, ..., 0x1e = 30, 0x1f = 31.
                DLL has an extra lock range boundary which only the values inside the lock range are meaningful.
                The lock range will be printed when shmoo starts or can be determined by the following formula:
                    Lock_Range = (0 - DLL_Vfine) ~ (31 - DLL_Vfine) where DLL_Vfine can be retrieved from DLL_STATUS register
                Please adjust the minimum value and the maximum value to be within the lock range boundary.
                The average of the minimum value and the maximum value will allow the users to get the center value. The shmoo process will change the register value of both dies.
                The registers are mapped to DLL_Setting0DXParam_HostY in ASM file. (where X/Y could be 0 or 1)
                Please change both dies together in ASM file. (D0/D1)
            Example: Shmoo Write DLL with decrement direction on CV5 DDRC0
            a:\> diag ddr shmoo 0 write - 30000
            ...
            [DDRC0][DLL2]Reg = 0x20202020
            [DDRC0][DLL2]Reg = 0x21212121
            [DDRC0][DLL2]Reg = 0x22222222
            [DDRC0][DLL2]Reg = 0x23232323
            [DDRC0][DLL2]Reg = 0x24242424
            ...
            Output message description:
                [DLL2]Reg holds Write DLL setting for byte 0~3.
                [DLL2]Reg[5:0]   - Byte 0
                [DLL2]Reg[13:8]  - Byte 1
                [DLL2]Reg[21:16] - Byte 2
                [DLL2]Reg[29:24] - Byte 3
                The msb (bit[5]) of each DLL setting is a sign bit. Sign bit 0 means the positive value and 1 represents the negative value.
                So 6 bits register value from the minimum value to the maximum value are:
                0x3f = -31, 0x3e = -30, ..., 0x22 = -2, 0x21 = -1, 0x20 = -0, 0x0 = 0, 0x1 = 1, ..., 0x1e = 30, 0x1f = 31.
                DLL has an extra lock range boundary which only the values inside the lock range are meaningful.
                The lock range will be printed when shmoo starts or can be determined by the following formula:
                    Lock_Range = (0 - DLL_Vfine) ~ (31 - DLL_Vfine) where DLL_Vfine can be retrieved from DLL_STATUS register
                Please adjust the minimum value and the maximum value to be within the lock range boundary.
                The average of the minimum value and the maximum value will allow the users to get the center value. The shmoo process will change the register value of both dies.
                The registers are mapped to DLL_Setting2DXParam_HostY in ASM file. (where X/Y could be 0 or 1)
                Please change both dies together in ASM file. (D0/D1)
-   Output:
    •   DLL register ranges (DLL_SettingXDYParam_HostZ in ASM file)

