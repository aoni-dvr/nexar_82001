echo off

set start_time=%time%

rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_1800.ads -m pad-term
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_1800.ads -m pad-dds
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_1800.ads -m pad-vref
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_1800.ads -m rddly
rem dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_1800.ads -m wrdly

dram_preliminary_shmoo.exe -n cv5 -a cv5_bub_lpddr4_1die_4g_1800.ads -m pad-term -m pad-dds -m pad-vref -m rddly -m wrdly --pad-term-start 3 --pad-term-end 5 --pad-dds-start 6 --pad-vref-start 16 --pad-vref-step 4 --pad-vref-end 36 --rddly-end 80 --wrdly-start 160 --wrdly-end 240 --rddly-step 6 --wrdly-step 6
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

set end_time=%time%

echo start:%start_time% 
echo end:  %end_time%