echo off

set start_time=%time%

rem run 17 items take around 1 second
 dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m pad-term
 dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m pad-dds
 dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m pad-vref
 dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m rddly
 dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m wrdly
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m lpddr5-dq-vref
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m lpddr5-ca-vref
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m pad-dqs-vref
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m dll0
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m dll2
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m cadly
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m dqs-gatedly
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m ckdly_
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2016.ads -m ckedly

rem d:\temp\work\pre_shmoo\cv5x>cv5_dk_lpddr5_2die_8g_2016
rem 
rem d:\temp\work\pre_shmoo\cv5x>echo off
rem failed to show device: Input/Output Error
rem amba_init_usb_device: try again
rem 
rem [===========================================================] [100%] (7/7)
rem 
rem pad-term
rem   1: 1
rem   2: 1
rem   3: 1
rem   4: 1
rem   5: 1
rem   6: 1
rem   7: 1
rem 
rem failed to show device: Input/Output Error
rem amba_init_usb_device: try again
rem 
rem [===========================================================] [100%] (7/7)
rem 
rem pad-dds
rem   1: 1
rem   2: 1
rem   3: 1
rem   4: 1
rem   5: 1
rem   6: 1
rem   7: 1
rem 
rem failed to show device: Input/Output Error
rem amba_init_usb_device: try again
rem 
rem [===========================================================] [100%] (48/48)
rem 
rem pad-vref
rem   0: 1
rem   1: 1
rem   2: 1
rem   3: 1
rem   4: 1
rem   5: 1
rem   6: 1
rem   7: 1
rem   8: 1
rem   9: 1
rem   10: 1
rem   11: 1
rem   12: 1
rem   13: 1
rem   14: 1
rem   15: 1
rem   16: 1
rem   17: 1
rem   18: 1
rem   19: 1
rem   20: 1
rem   21: 1
rem   22: 1
rem   23: 1
rem   24: 1
rem   25: 1
rem   26: 1
rem   27: 1
rem   28: 1
rem   29: 1
rem   30: 1
rem   31: 1
rem   32: 1
rem   33: 1
rem   34: 1
rem   35: 1
rem   36: 1
rem   37: 1
rem   38: 1
rem   39: 1
rem   40: 1
rem   41: 1
rem   42: 1
rem   43: 1
rem   44: 1
rem   45: 1
rem   46: 1
rem   47: 1
rem 
rem failed to show device: Input/Output Error
rem amba_init_usb_device: try again
rem 
rem [===========================================================] [100%] (384/384)
rem 
rem rddly
rem   27: 1
rem   29: 1
rem   30: 1
rem   31: 1
rem   32: 1
rem   33: 1
rem   34: 1
rem   35: 1
rem   36: 1
rem   37: 1
rem   38: 1
rem   39: 1
rem   40: 1
rem   41: 1
rem   42: 1
rem   43: 1
rem   44: 1
rem   45: 1
rem   46: 1
rem   47: 1
rem 
rem failed to show device: Input/Output Error
rem amba_init_usb_device: try again
rem 
rem [===========================================================] [100%] (384/384)
rem 
rem wrdly
rem   120: 1
rem   121: 1
rem   122: 1
rem   123: 1
rem   124: 1
rem   125: 1
rem   126: 1
rem   127: 1
rem   128: 1
rem   129: 1
rem   130: 1
rem   131: 1
rem   132: 1
rem   133: 1
rem   134: 1
rem   135: 1
rem   136: 1
rem   137: 1
rem   138: 1
rem   139: 1
rem   140: 1
rem   141: 1
rem   142: 1
rem   143: 1
rem 
rem start:22:04:51.68
rem end:  22:10:58.16





rem USAGE: dram_preliminary_shmoo [OPTION]
rem Version: 1.3
rem         -h/-hh                   Show help or more help
rem         -v, --verbose            Show verbose message
rem         -a, --ads                Specify ads file, default: SOC_DDR.ads
rem         -c, --count              Specify loop count when testing dram, default: 1
rem         -t, --training           Specify to do training for LPDDR5: all, wck2ck, dca
rem         -n, --chipname           Specify chip name, default: cv22
rem             --result FILE        Specify file to save result, default: result.txt
rem             --eye-diagram        Specify file to generate eye diagram
rem             --parse-shmoo        Specify file to parse
rem             --parse-training     Parse training result in SoC Scratchpad RAM
rem             --parse-training-bin Parse training result in binary file
rem         -m, --mode               Specify shmoo mode:
rem                                     pad-term        pad-dds         pad-pdds
rem                                     pad-ca-dds      pad-ca-pdds     pad-vref
rem                                     pad-dqs-vref    rddly           wrdly
rem                                     wckdly          ckdly_          ckedly
rem                                     cadly           dqs-gatedly     dll0
rem                                     dll1            dll2            ddr4-vref
rem                                     ddr4-odt        ddr4-dds        lpddr4-pdds
rem                                     lpddr4-ca-odt   lpddr4-dq-odt   lpddr4-ca-vref
rem                                     lpddr4-dq-vref  lpddr5-pdds     lpddr5-ca-odt
rem                                     lpddr5-dq-odt   lpddr5-ca-vref  lpddr5-dq-vref

rem dram_preliminary_shmoo.exe -n cv5 -a cv52_dk_lpddr4_1die_4g_2112.ads -m pad-term -m pad-dds -m pad-vref -m rddly -m wrdly --pad-term-start 3 --pad-term-end 5 --pad-dds-start 6 --pad-vref-start 16 --pad-vref-step 4 --pad-vref-end 36 --rddly-end 80 --wrdly-start 160 --wrdly-end 240 --rddly-step 6 --wrdly-step 6
rem [===========================================================] [100%] (7056/7056)
rem 
rem pad-term
rem   3: 645
rem   4: 407
rem   5: 140
rem 
rem pad-dds
rem   6: 597
rem   7: 595
rem 
rem pad-vref
rem   16: 289
rem   20: 272
rem   24: 241
rem   28: 161
rem   32: 144
rem   36: 85
rem 
rem rddly
rem   18: 49
rem   24: 142
rem   30: 185
rem   36: 229
rem   42: 224
rem   48: 192
rem   54: 130
rem   60: 41
rem 
rem wrdly
rem   184: 148
rem   190: 149
rem   196: 149
rem   202: 150
rem   208: 149
rem   214: 151
rem   220: 149
rem   226: 147
rem 
rem start:10:30:21.50
rem end:  10:37:20.00

rem scan Vref(step=4), ReadDelay(step=6), WriteDelay(step=6) takes 1 hour
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_dk_lpddr5_2die_8g_2880.ads -m pad-vref -m rddly -m wrdly --pad-vref-start 0 --pad-vref-step 4 --pad-vref-end 48 --rddly-start 0 --rddly-end 384 --wrdly-start 0 --wrdly-end 384 --rddly-step 6 --wrdly-step 6
rem [===========================================================] [100%] (54925/54925)
rem 
rem pad-vref
rem   0: 7
rem   4: 14
rem   8: 28
rem   12: 28
rem   16: 35
rem   20: 33
rem   24: 21
rem   28: 21
rem   32: 14
rem   36: 7
rem 
rem rddly
rem   36: 27
rem   42: 56
rem   48: 63
rem   54: 48
rem   60: 14
rem 
rem wrdly
rem   108: 30
rem   114: 30
rem   120: 30
rem   126: 30
rem   132: 30
rem   138: 30
rem   144: 28

rem dram_preliminary_shmoo.exe -n cv5 -a cv52_dk_lpddr4_1die_4g_2112.ads -m pad-vref -m rddly -m wrdly -m lpddr4-dq-vref --pad-vref-start 0 --pad-vref-step 4 --pad-vref-end 48 --rddly-start 0 --rddly-end 384 --wrdly-start 0 --wrdly-end 384 --rddly-step 6 --wrdly-step 6 --lpddr4-dq-vref-step 6

rem dram_preliminary_shmoo.exe -n cv5 -a cv52_dk_lpddr4_1die_4g_2112.ads -m pad-term -m pad-dds -m pad-vref -m rddly -m wrdly -m lpddr4-ca-vref -m lpddr4-dq-vref -m dll0 --pad-term-start 3 --pad-term-end 5 --pad-dds-start 6 --pad-vref-start 16 --pad-vref-step 4 --pad-vref-end 36 --rddly-end 60 --wrdly-start 200 --wrdly-end 240 --rddly-step 6 --wrdly-step 6 --lpddr4-ca-vref-start 10 --lpddr4-ca-vref-end 40 --lpddr4-ca-vref-step 6 --lpddr4-dq-vref-start 10 --lpddr4-dq-vref-end 40 --lpddr4-dq-vref-step 6 --dll0-step 5


set end_time=%time%

echo start:%start_time% 
echo end:  %end_time%