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
of diag command locates at rtos/cortex_r/svc/apps/iout/src/threadx/diag/cv2fs/AmbaIOUTDiag_CmdDDR.c 

Notice that on CV2FS, the DTTE has to be disabled before executing the Shmoo process.
We will turn it off when executing the Shmoo command.
You can turn off the DTTE by the following patch if you code base didn't have it yet:
=========================================================================
diff --git a/boot/cv2fs/secure/bootloader/AmbaDramTraining.c b/boot/cv2fs/secure/bootloader/AmbaDramTraining.c
index 20a3da3..d205c21 100755
--- a/boot/cv2fs/secure/bootloader/AmbaDramTraining.c
+++ b/boot/cv2fs/secure/bootloader/AmbaDramTraining.c
@@ -1708,10 +1708,7 @@ __attribute__((section("training_text"))) void ddrc_reinit(const void *pDramPara
     }
     //add training done : boot up every time trigger flag
     ddrc_train_postamble();
-    dtte_start_tracking(0U); //Enable dtte in tracking mode
-    if (readl(0xdffe1780U) == 0x1U) {
-        dtte_start_tracking(1U); //Enable dtte in tracking mode
-    }
+    dtte_stop_tracking(0U); //Enable dtte in tracking mode
     //dtte_start_obs(); //Enable dtte observation mode

     ddrc_training_LdBld();
=========================================================================

We define three categories of Shmoo here. The syntax of the diag command under
 each categories are listed. Ambarella suggests to run the Shmoo on 5 sets during
 bring up stage if possible.

Source code: rtos/cortex_r/svc/apps/iout/src/threadx/diag/cv2fs/AmbaIOUTDiag_CmdDDR.c
Shmoo command: "diag_ddr" (Port from IOUT app if one’s app doesn't support this command)
    Parameters:
        [0:DDRC0|1:DDRCE]: Mandatory parameter specifying the target DDRC controller to perform the Shmoo.
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
        command: diag_ddr lp4vref [0:DDRC0|1:DDRCE] [+|-] [time(ms)]
            Example: Shmoo DQ Vref with increment direction on CV2FS DDRC0
            a:\> diag_ddr lp4vref 0 + 30000
            ...
            [DDRC0][DQ_Byte0 Vref]TrainDqVref = 0x00000020
            [DDRC0][DQ_Byte1 Vref]TrainDqVref = 0x00000020
            [DDRC0][DQ_Byte2 Vref]TrainDqVref = 0x00000020
            [DDRC0][DQ_Byte3 Vref]TrainDqVref = 0x00000020
            [DDRC0][DQ_Byte0 Vref]TrainDqVref = 0x00000021
            [DDRC0][DQ_Byte1 Vref]TrainDqVref = 0x00000021
            [DDRC0][DQ_Byte2 Vref]TrainDqVref = 0x00000021
            [DDRC0][DQ_Byte3 Vref]TrainDqVref = 0x00000021
            [DDRC0][DQ_Byte0 Vref]TrainDqVref = 0x00000022
            [DDRC0][DQ_Byte1 Vref]TrainDqVref = 0x00000022
            [DDRC0][DQ_Byte2 Vref]TrainDqVref = 0x00000022
            [DDRC0][DQ_Byte3 Vref]TrainDqVref = 0x00000022
            ...
            Output message description:
                TrainDqVref holds Vref setting for DQ Vref of byte 0~3. Each byte has it's own TrainDqVref register.
                The value printed is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get center value.
                The registers are mapped to DdrcX_DRAM_lpddr4DqYCaVrefParam in ASM file. (where X could be 0 or E, Y could be 0,1,2 or 3)
    •   Find the range of DqReadDly (Controller side)
        command: diag_ddr lp4rdly [0:DDRC0|1:DDRCE] [+|-] [time(ms)]
            Example: Shmoo Read Delay with increment direction on CV2FS DDRC0
            a:\> diag_ddr lp4rdly 0 + 30000
            ...
            [DDRC0][Read Delay0][Bits0xFF]TrainDqReadDlyA,B = 0x6318C63,0xC63 (value = 0x1)
            [DDRC0][Read Delay1][Bits0xFF]TrainDqReadDlyA,B = 0x6318C63,0xC63 (value = 0x1)
            [DDRC0][Read Delay2][Bits0xFF]TrainDqReadDlyA,B = 0x6318C63,0xC63 (value = 0x1)
            [DDRC0][Read Delay3][Bits0xFF]TrainDqReadDlyA,B = 0x6318C63,0xC63 (value = 0x1)
            [DDRC0][Read Delay0][Bits0xFF]TrainDqReadDlyA,B = 0xA5294A5,0x14A5 (value = 0x2)
            [DDRC0][Read Delay1][Bits0xFF]TrainDqReadDlyA,B = 0xA5294A5,0x14A5 (value = 0x2)
            [DDRC0][Read Delay2][Bits0xFF]TrainDqReadDlyA,B = 0xA5294A5,0x14A5 (value = 0x2)
            [DDRC0][Read Delay3][Bits0xFF]TrainDqReadDlyA,B = 0xA5294A5,0x14A5 (value = 0x2)
            [DDRC0][Read Delay0][Bits0xFF]TrainDqReadDlyA,B = 0xE739CE7,0x1CE7 (value = 0x3)
            [DDRC0][Read Delay1][Bits0xFF]TrainDqReadDlyA,B = 0xE739CE7,0x1CE7 (value = 0x3)
            [DDRC0][Read Delay2][Bits0xFF]TrainDqReadDlyA,B = 0xE739CE7,0x1CE7 (value = 0x3)
            [DDRC0][Read Delay3][Bits0xFF]TrainDqReadDlyA,B = 0xE739CE7,0x1CE7 (value = 0x3)
            ...
            Output message description:
                TrainDqReadDlyA and TrainDqReadDlyB are two registers which hold Read Delay setting for DQ/DM for each byte.
                Each byte has it's own TrainDqReadDlyA/TrainDqReadDlyB registers.
                TrainDqReadDlyA[4:0]   - DQ read delay bit 0
                TrainDqReadDlyA[9:5]   - DQ read delay bit 1
                TrainDqReadDlyA[14:10] - DQ read delay bit 2
                TrainDqReadDlyA[19:15] - DQ read delay bit 3
                TrainDqReadDlyA[24:20] - DQ read delay bit 4
                TrainDqReadDlyA[29:25] - DQ read delay bit 5
                TrainDqReadDlyB[4:0]   - DQ read delay bit 6
                TrainDqReadDlyB[9:5]   - DQ read delay bit 7
                TrainDqReadDlyB[14:10] - DM read delay
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte.
                The registers are mapped to DdrcX_DRAM_lpddr4DqYReadDlyAParam/DdrcX_DRAM_lpddr4DqYReadDlyBParam in ASM file.
                (where X could be 0 or E, Y could be 0, 1, 2 or 3)
-   Output:
    •   Vref register ranges        (DdrcX_DRAM_lpddr4DqYCaVrefParam in ASM file)
    •   DqReadDly register ranges   (DdrcX_DRAM_lpddr4DqYReadDlyAParam/DdrcX_DRAM_lpddr4DqYReadDlyBParam in ASM file)


Write Shmoo:
-   Purpose: find out the workable range of DqWriteDly and Vref (lpddr4-dq-vref, lpddr4-ca-vref, Device side)
-   Procedure:
    •   Find the range of Vref (Device side)
        For LPDDR4:
        command: diag_ddr lp4mr14 [0:DDRC0|1:DDRCE] [+|-] [time(ms)]
            Example: Shmoo DQ Vref of DRAM with increment direction on CV2FS DDRC0
            a:\> diag_ddr lp4mr14 0 + 30000
            ...
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E0020 (value = 0x20)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E0021 (value = 0x21)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E0022 (value = 0x22)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E0023 (value = 0x23)
            [LPDDR4_0][MR14 Vref]Lpddr4DqVref = 0x000E0024 (value = 0x24)
            ...
            Output message description:
                Lpddr4DqVref represents the Vref setting for DQ Vref of DRAM's MR14 (Mode Register 14).
                Lpddr4DqVref[7:0]  - DQ Vref setting
                Lpddr4DqVref[31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to DdrcX_LPDDR4_ModeReg14Param bit[7:0] in ASM file. (where X could be 0 or E)
        command: diag_ddr lp4mr12 [0:DDRC0|1:DDRCE] [+|-] [time(ms)]
            Example: Shmoo CA Vref of DRAM with increment direction on CV2FS DDRC0
            a:\> diag_ddr lp4mr12 0 + 30000
            ...
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C000C (value = 0xC)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C000D (value = 0xD)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C000E (value = 0xE)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C000F (value = 0xF)
            [LPDDR4_0][MR12 Vref]Lpddr4CaVref = 0x000C0010 (value = 0x10)
            ...
            Output message description:
                Lpddr4CaVref represents the Vref setting for CA Vref of DRAM's MR12 (Mode Register 12).
                Lpddr4CaVref[7:0]  - CA Vref setting
                Lpddr4CaVref[31:8] - Access pattern setting for shmoo only
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                Please pick the register setting of bit[7:0] only of the selected value.
                The registers are mapped to DdrcX_LPDDR4_ModeReg12Param bit[7:0] in ASM file. (where X could be 0 or E)
    •   Find the range of DqWriteDly (Controller side)
        For LPDDR4:
        command: diag_ddr lp4wdly [0:DDRC0|1:DDRCE] [+|-] [time(ms)]
            Example: Shmoo Write Delay with increment direction on CV2FS DDRC0
            a:\> diag_ddr lp4wdly 0 + 30000
            ...
            [DDRC0][Write Delay0][Bits0xFF]TrainDqWriteDlyA,B,C = 0x53535353,0x53535353,0x53 (value = 0x29)
            [DDRC0][Write Delay1][Bits0xFF]TrainDqWriteDlyA,B,C = 0x53535353,0x53535353,0x53 (value = 0x29)
            [DDRC0][Write Delay2][Bits0xFF]TrainDqWriteDlyA,B,C = 0x53535353,0x53535353,0x53 (value = 0x29)
            [DDRC0][Write Delay3][Bits0xFF]TrainDqWriteDlyA,B,C = 0x53535353,0x53535353,0x53 (value = 0x29)
            [DDRC0][Write Delay0][Bits0xFF]TrainDqWriteDlyA,B,C = 0x55555555,0x55555555,0x55 (value = 0x2A)
            [DDRC0][Write Delay1][Bits0xFF]TrainDqWriteDlyA,B,C = 0x55555555,0x55555555,0x55 (value = 0x2A)
            [DDRC0][Write Delay2][Bits0xFF]TrainDqWriteDlyA,B,C = 0x55555555,0x55555555,0x55 (value = 0x2A)
            [DDRC0][Write Delay3][Bits0xFF]TrainDqWriteDlyA,B,C = 0x55555555,0x55555555,0x55 (value = 0x2A)
            [DDRC0][Write Delay0][Bits0xFF]TrainDqWriteDlyA,B,C = 0x57575757,0x57575757,0x57 (value = 0x2B)
            [DDRC0][Write Delay1][Bits0xFF]TrainDqWriteDlyA,B,C = 0x57575757,0x57575757,0x57 (value = 0x2B)
            [DDRC0][Write Delay2][Bits0xFF]TrainDqWriteDlyA,B,C = 0x57575757,0x57575757,0x57 (value = 0x2B)
            [DDRC0][Write Delay3][Bits0xFF]TrainDqWriteDlyA,B,C = 0x57575757,0x57575757,0x57 (value = 0x2B)
            ...
            Output message description:
                TrainDqWriteDlyA, TrainDqWriteDlyB and TrainDqWriteDlyC are three registers which hold Write Delay setting for DQ/DM for each byte.
                Each byte has it's own TrainDqWriteDlyA/TrainDqWriteDlyB/TrainDqWriteDlyC registers.
                TrainDqWriteDlyA[7:0]   - DQ write delay bit 0
                TrainDqWriteDlyA[15:8]  - DQ write delay bit 1
                TrainDqWriteDlyA[23:16] - DQ write delay bit 2
                TrainDqWriteDlyA[31:24] - DQ write delay bit 3
                TrainDqWriteDlyB[7:0]   - DQ write delay bit 4
                TrainDqWriteDlyB[15:8]  - DQ write delay bit 5
                TrainDqWriteDlyB[23:16] - DQ write delay bit 6
                TrainDqWriteDlyB[31:24] - DQ write delay bit 7
                TrainDqWriteDlyC[7:0]   - DM write delay
                The value printed in parenthesis is the real value of the corresponding byte. The average of the minimum value and the maximum value will allow the users to get the center value.
                The shmoo process will change the register value byte by byte.
                The registers are mapped to DdrcX_DRAM_lpddr4DqYWriteDlyAParam/DdrcX_DRAM_lpddr4DqYWriteDlyBParam/
                 DdrcX_DRAM_lpddr4DqYWriteDlyCParam in ASM file. (where X could be 0 or E, Y could be 0, 1, 2 or 3)
-   Output:
    •   Vref register (ModeReg14/ModeReg12) ranges  (DdrcX_LPDDR4_ModeReg14Param/DdrcX_LPDDR4_ModeReg12Param in ASM file)
    •   DqWriteDly register ranges                  (DdrcX_DRAM_lpddr4DqYWriteDlyAParam/DdrcX_DRAM_lpddr4DqYWriteDlyBParam/
                                                     DdrcX_DRAM_lpddr4DqYWriteDlyCParam in ASM file)
    •   The DdrcX_LPDDR4_ModeReg??Param in ASM file only takes bit[7:0] as the register setting. 
        Bit[31:8] are access pattern which should NOT be changed.


DLL Shmoo:
-   Purpose: find out the workable range of DLL.
-   Procedure:
    •   shmoo DLL value of sync/read/write one by one to find out their ranges.
        command: diag_ddr shmoo [0:DDRC0|1:DDRCE] [sync|read|write] [+|-] [time(ms)]
            Example: Shmoo Read DLL with increment direction on CV2FS DDRC0
            a:\> diag_ddr shmoo 0 read + 30000
            ...
            [DDRC0][DLL0]Reg = 0x085F23
            [DDRC0][DLL1]Reg = 0x085F23
            [DDRC0][DLL2]Reg = 0x085F23
            [DDRC0][DLL3]Reg = 0x085F23
            [DDRC0][DLL0]Reg = 0x085F22
            [DDRC0][DLL1]Reg = 0x085F22
            [DDRC0][DLL2]Reg = 0x085F22
            [DDRC0][DLL3]Reg = 0x085F22
            [DDRC0][DLL0]Reg = 0x085F21
            [DDRC0][DLL1]Reg = 0x085F21
            [DDRC0][DLL2]Reg = 0x085F21
            [DDRC0][DLL3]Reg = 0x085F21
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
                The registers are mapped to DdrcX_DLL_SettingYParam in ASM file. 
                (where X could be 0 or E, Y could be 0,1,2 or 3)
            Example: Shmoo Write DLL with decrement direction on CV2FS DDRC0
            a:\> diag_ddr shmoo 0 write - 30000
            ...
            [DDRC0][DLL0]Reg = 0x085F23
            [DDRC0][DLL1]Reg = 0x085F23
            [DDRC0][DLL2]Reg = 0x085F23
            [DDRC0][DLL3]Reg = 0x085F23
            [DDRC0][DLL0]Reg = 0x075F23
            [DDRC0][DLL1]Reg = 0x075F23
            [DDRC0][DLL2]Reg = 0x075F23
            [DDRC0][DLL3]Reg = 0x075F23
            [DDRC0][DLL0]Reg = 0x065F23
            [DDRC0][DLL1]Reg = 0x065F23
            [DDRC0][DLL2]Reg = 0x065F23
            [DDRC0][DLL3]Reg = 0x065F23
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
                The registers are mapped to DdrcX_DLL_SettingYParam in ASM file. 
                (where X could be 0 or E, Y could be 0,1,2 or 3)
-   Output:
    •   DLL register ranges (DdrcX_DLL_SettingYParam in ASM file)
    •   The DdrcX_DLL_SettingYParam in ASM file is shared for sync/read and write DLL. 
        Read DLL takes bit[5:0]. Sync DLL takes bit[13:8]. Write DLL occupies bit[21:16].
        If one wants to change sync/read/write DLL at the same time, one has to merge it manually from 
        the DLL register value of Shmoo output of sync/read/write DLL.

