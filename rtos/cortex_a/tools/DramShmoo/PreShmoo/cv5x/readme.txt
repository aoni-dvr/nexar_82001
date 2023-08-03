bstiniparser:
    Linux tool to translate ini file to other format. (description in bstiniparser.txt)
    For DRAM side:
        - DS/ODT: 
            Setting the value to "RZQ/X" specifics the real value to be 240ohm/X.
            Ex: if X=3, the real value is 80ohm.
    For DDRC side:
        - DDRC_TERM (ODT): 
            Setting the value to "RZQ/X" specifics the real value to be 240ohm/X.
            Ex: if X=3, the real value is 80ohm.
            When set it to "Disable", it means Hi-Z
        - DDRC_DDS/DDRC_PDDS (DS):
            Setting the value to "RZQ/Y" specifics the real value to be 240ohm/(Y+1).
            Ex: if Y=3, the real value is 60ohm.
            When set it to "Disable", the value will be 240ohm/(0+1) = 240ohm.

bstiniparser.txt
    Readme for bstiniparser tool.

bstiniparser_gen_all.sh:
    Linux shell script to help do ini translation in one time.
        Usage: ./bstiniparser_gen_all.sh [filename.ini]
        Ex: ./bstiniparser_gen_all.sh cv52_dk_lpddr4_1die_2g_540.ini 
            Three files will be generated:
            cv52_dk_lpddr4_1die_2g_540.csf  => JTAG script for DRAM init
            cv52_dk_lpddr4_1die_2g_540.ads  => Pre-Shmoo input script file and AmbaUSB DRAM init script (cv2x only)
            cv52_dk_lpddr4_1die_2g_540.h    => Header file to show register value. (can be referenced to copy value to .asm)

dram_preliminary_shmoo.exe:
    Windows Pre-Shmoo tool to shmoo DDR parameters. (description in dram_preliminary_shmoo.txt)
    It needs an ads file (can be translated from ini file by bstiniparser) to do the DRAM initialization.

    Notice that the Pre-Shmoo result of DLL doesn't shows the real DLL setting to be used in .ini
    To map the Pre-Shmoo result of DLL to the DLL setting, you will need to read DLL lock value in DLL_STATUS register via JTAG first.
    The DLL lock value can be calculated as below:
        CV2x:      dll_lock_value = (DDRC_reg_0x130[20:16] + DDRC_reg_0x134[20:16]) / 2
        CV5x/CV3x: dll_lock_value = (DDRC_reg_0x388[5:1]   + DDRC_reg_0x388[21:17]) / 2
    Then use the following logic to map to DLL setting:
        if (pre_shmoo_val < dll_lock_value) {
            dll_setting_val = dll_lock_value - pre_shmoo_val;
            dll_setting_val |= 0x20;
        } else {
            dll_setting_val = pre_shmoo_val  - dll_lock_value;
        }

dram_preliminary_shmoo:
    Linux Pre-Shmoo tool to shmoo DDR parameters. (description in dram_preliminary_shmoo.txt)
    It needs an ads file (can be translated from ini file by bstiniparser) to do the DRAM initialization.

dram_preliminary_shmoo.txt:
    Readme for dram_preliminary_shmoo tool.

dram_preliminary_shmoo_sample.bat
    Windows batch script to run the Pre-Shmoo tool with sample parameters.

*.ini:
    Sample ini files, the ini file is the DDR parameter source file.