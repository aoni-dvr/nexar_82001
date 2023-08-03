echo off

set start_time=%time%

rem parameters
rem -m pad-term     --pad-term-start 3 --pad-term-end 5
rem -m pad-dds      --pad-dds-start 1 --pad-dds-end 7
rem -m pad-vref     --pad-vref-start 16 --pad-vref-end 24 --pad-vref-step 2
rem -m rddly        --rddly-start 3 --rddly-end 9 --rddly-step 2
rem -m wrdly        --wrdly-start 37 --wrdly-end 52  --wrdly-step 3

rem -m pad-pdds
rem -m pad-ca-dds
rem -m pad-ca-pdds
rem -m lpddr4-pdds
rem -m lpddr4-dq-vref   --lpddr4-dq-vref-end 0x30
rem -m lpddr4-dq-odt
rem -m lpddr4-ca-vref
rem -m lpddr4-ca-odt
rem -m dll0
rem -m dll2


rem Step1. Scan import value to make sure we have some success combination
dram_preliminary_shmoo -a h32_bub_lpddr4_1die_1g_1596.ads -n h32 -m pad-term -m pad-dds -m pad-vref -m rddly -m wrdly --pad-term-start 3 --pad-term-end 5 --pad-dds-start 6 --pad-vref-start 16 --pad-vref-step 4 --pad-vref-end 36 --rddly-end 12 --wrdly-start 16 --rddly-step 3 --wrdly-step 3
rem Result:
rem [===========================================================] [100%] (2880/2880)
rem 
rem pad-term
rem   3: 140
rem   4: 156
rem   5: 144
rem 
rem pad-dds
rem   6: 204
rem   7: 236
rem 
rem pad-vref
rem   16: 52
rem   20: 72
rem   24: 110
rem   28: 96
rem   32: 66
rem   36: 44
rem 
rem rddly
rem   3: 127
rem   6: 134
rem   9: 124
rem   12: 55
rem 
rem wrdly
rem   37: 77
rem   40: 79
rem   43: 80
rem   46: 79
rem   49: 79
rem   52: 46


rem Step2. Scan write direction based on previous read parameter (Padterm, read delay, Vref)
rem dram_preliminary_shmoo -a h32_bub_lpddr4_1die_1g_1596.ads -n h32 -m pad-dds -m pad-pdds -m wrdly -m lpddr4-dq-vref -m lpddr4-dq-odt --pad-dds-start 5 --pad-pdds-end 3 --wrdly-start 35 --wrdly-step 3 --wrdly-end 50 --lpddr4-dq-vref-end 20 --lpddr4-dq-vref-step 3 -m pad-term -m pad-vref -m rddly --pad-term-start 4 --pad-term-end 4 --pad-vref-start 20 --pad-vref-end 20 --rddly-start 6 --rddly-end 6
rem Result:
rem [===========================================================] [100%] (3528/3528)
rem 
rem pad-term
rem   4: 1982
rem 
rem pad-dds
rem   5: 644
rem   6: 666
rem   7: 672
rem 
rem pad-pdds
rem   0: 213
rem   1: 582
rem   2: 614
rem   3: 573
rem 
rem pad-vref
rem   20: 1982
rem 
rem rddly
rem   6: 1982
rem 
rem wrdly
rem   35: 85
rem   38: 456
rem   41: 459
rem   44: 413
rem   47: 335
rem   50: 234
rem 
rem lpddr4-dq-odt
rem   0: 161
rem   1: 283
rem   2: 329
rem   3: 330
rem   4: 321
rem   5: 295
rem   6: 263
rem 
rem lpddr4-dq-vref
rem   0: 267
rem   3: 275
rem   6: 290
rem   9: 289
rem   12: 292
rem   15: 289
rem   18: 280
  

rem Step3. Scan read direction based on previous write parameter (Padterm, read delay, Vref)
rem dram_preliminary_shmoo -a h32_bub_lpddr4_1die_1g_1596.ads -n h32 -m pad-term -m pad-vref -m rddly -m lpddr4-pdds -m dll0 --pad-term-start 3 --pad-term-end 5 --pad-vref-start 16 --pad-vref-step 3 --pad-vref-end 28 --rddly-start 3 --rddly-end 9 --rddly-step 2 --dll0-step 3 -m pad-dds -m pad-pdds -m wrdly -m lpddr4-dq-vref -m lpddr4-dq-odt --pad-dds-start 6 --pad-dds-end 6 --pad-pdds-start 2 --pad-pdds-end 2 --wrdly-start 41 --wrdly-end 41 --lpddr4-dq-odt-start 2 --lpddr4-dq-odt-end 2 --lpddr4-dq-vref-start 6 --lpddr4-dq-vref-end 6
rem Result:
rem [===========================================================] [100%] (2880/2880)
rem 
rem pad-term
rem   3: 775
rem   4: 808
rem   5: 572
rem 
rem pad-dds
rem   6: 2155
rem 
rem pad-pdds
rem   2: 2155
rem 
rem pad-vref
rem   16: 311
rem   19: 403
rem   22: 468
rem   25: 496
rem   28: 477
rem 
rem rddly
rem   3: 434
rem   5: 548
rem   7: 600
rem   9: 573
rem 
rem wrdly
rem   41: 2155
rem 
rem dll0
rem   0: 276
rem   3: 283
rem   6: 283
rem   9: 274
rem   12: 269
rem   15: 263
rem   18: 252
rem   21: 255
rem 
rem lpddr4-pdds
rem   1: 316
rem   2: 363
rem   3: 375
rem   4: 374
rem   5: 368
rem   6: 359
rem 
rem lpddr4-dq-odt
rem   2: 2155
rem 
rem lpddr4-dq-vref
rem   6: 2155


set end_time=%time%

echo start:%start_time% 
echo end:  %end_time%
