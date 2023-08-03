echo off

set start_time=%time%

rem run 17 items take around 1 second
dram_preliminary_shmoo.exe -n cv22 -a cv22_bub_lpddr4_1die_2g_1392.ads -m pad-term
dram_preliminary_shmoo.exe -n cv22 -a cv22_bub_lpddr4_1die_2g_1392.ads -m pad-dds
dram_preliminary_shmoo.exe -n cv22 -a cv22_bub_lpddr4_1die_2g_1392.ads -m pad-vref
dram_preliminary_shmoo.exe -n cv22 -a cv22_bub_lpddr4_1die_2g_1392.ads -m rddly
dram_preliminary_shmoo.exe -n cv22 -a cv22_bub_lpddr4_1die_2g_1392.ads -m wrdly
rem d:\temp\work\pre_shmoo\cv2x>cv22_bub_lpddr4_1die_2g_1392.bat
rem 
rem d:\temp\work\pre_shmoo\cv2x>echo off
rem 
rem [===========================================================] [100%] (7/7)
rem 
rem pad-term
rem   3: 1
rem   4: 1
rem   5: 1
rem   6: 1
rem   7: 1
rem 
rem 
rem [===========================================================] [100%] (8/8)
rem 
rem pad-dds
rem   3: 1
rem   4: 1
rem   5: 1
rem   6: 1
rem   7: 1
rem 
rem 
rem [===========================================================] [100%] (48/48)
rem 
rem pad-vref
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
rem 
rem 
rem [===========================================================] [100%] (16/16)
rem 
rem rddly
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
rem 
rem 
rem [===========================================================] [100%] (64/64)
rem 
rem wrdly
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
rem   48: 1
rem   49: 1
rem   50: 1
rem   51: 1
rem   52: 1
rem   53: 1
rem   54: 1
rem   55: 1
rem   56: 1
rem   57: 1
rem 
rem start:21:07:59.26
rem end:  21:08:13.52


rem dram_preliminary_shmoo_old.exe -n cv22 -a cv22_bub_lpddr4_1die_2g_1392.ads -m pad-term -m pad-dds -m pad-vref -m rddly -m wrdly --pad-term-start 3 --pad-term-end 5 --pad-dds-start 6 --pad-vref-start 16 --pad-vref-step 4 --pad-vref-end 36 --rddly-step 2 --wrdly-step 4

rem [1800===========================================================] [100%] (7056/7056)
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

rem [1920===========================================================] [100%] (7056/7056)
rem 
rem pad-term
rem   3: 348
rem   4: 198
rem   5: 50
rem 
rem pad-dds
rem   6: 296
rem   7: 300
rem 
rem pad-vref
rem   16: 163
rem   20: 132
rem   24: 112
rem   28: 81
rem   32: 70
rem   36: 38
rem 
rem rddly
rem   12: 33
rem   18: 82
rem   24: 121
rem   30: 137
rem   36: 111
rem   42: 73
rem   48: 39
rem 
rem wrdly
rem   214: 118
rem   220: 117
rem   226: 121
rem   232: 120
rem   238: 120

rem scan Vref(step=4), ReadDelay(step=6), WriteDelay(step=6) takes 1 hour
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_2112.ads -m pad-vref -m rddly -m wrdly --pad-vref-start 0 --pad-vref-step 4 --pad-vref-end 48 --rddly-start 0 --rddly-end 384 --wrdly-start 0 --wrdly-end 384 --rddly-step 6 --wrdly-step 6

rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_2112.ads -m pad-vref -m rddly -m wrdly -m lpddr4-dq-vref --pad-vref-start 0 --pad-vref-step 4 --pad-vref-end 48 --rddly-start 0 --rddly-end 384 --wrdly-start 0 --wrdly-end 384 --rddly-step 6 --wrdly-step 6 --lpddr4-dq-vref-step 6

rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_2112.ads -m pad-term -m pad-dds -m pad-vref -m rddly -m wrdly -m lpddr4-ca-vref -m lpddr4-dq-vref --pad-term-start 3 --pad-term-end 5 --pad-dds-start 6 --pad-vref-start 16 --pad-vref-step 4 --pad-vref-end 36 --rddly-end 80 --wrdly-start 160 --wrdly-end 240 --rddly-step 6 --wrdly-step 6 --lpddr4-ca-vref-start 15 --lpddr4-ca-vref-end 35 --lpddr4-ca-vref-step 6 --lpddr4-dq-vref-start 15 --lpddr4-dq-vref-end 35 --lpddr4-dq-vref-step 6


set end_time=%time%

echo start:%start_time% 
echo end:  %end_time%