# Init DRAM controller...


# cortex pll setting
write 0x20ed080264,0x42100000
write 0x20ed080264,0x42100001
write 0x20ed080264,0x42100000

# dsu pll setting
write 0x20ed080814,0x31100000
write 0x20ed080814,0x31100001
write 0x20ed080814,0x31100000

# core pll setting
write 0x20ed080000,0x27110000
write 0x20ed080000,0x27110001
write 0x20ed080000,0x27110000

# idsp pll setting
write 0x20ed0800e4,0x37110000
write 0x20ed0800e4,0x37110001
write 0x20ed0800e4,0x37110000

# vision pll setting
write 0x20ed0806e4,0x29110000
write 0x20ed0806e4,0x29110001
write 0x20ed0806e4,0x29110000

# dram pll setting
write 0x20ed180000,0x18002000
write 0x20ed180008,0x20521300
write 0x20ed18000c,0x000c8004
write 0x20ed180000,0x18002001
write 0x20ed180000,0x18002000

# dram pll setting
write 0x20ed190000,0x18002000
write 0x20ed190008,0x20521300
write 0x20ed19000c,0x000c8004
write 0x20ed190000,0x18002001
write 0x20ed190000,0x18002000

# Program Number of DDRHOST in DRAM
write 0x1000000000,0x00000016

# setup DDRIO dll Control settings
write 0x1000005210,0x20202020
write 0x1000005218,0x0f0f0f0f
write 0x1000005220,0x20202020
write 0x1000005214,0x20202020
write 0x100000521c,0x0f0f0f0f
write 0x1000005224,0x20202020
write 0x1000005200,0x00e1b881
write 0x1000005204,0x00e1b881
write 0x1000005208,0x00e1b881
write 0x100000520c,0x00e1b881
write 0x1000005228,0x00090841
write 0x100000522c,0x12214800
write 0x1000005230,0x02020202
write 0x1000005234,0x02020202
write 0x1000005238,0x09090909
write 0x100000523c,0x09090909
write 0x1000005288,0x00090841
write 0x100000528c,0x12214800
write 0x1000005290,0x02020202
write 0x1000005294,0x02020202
write 0x1000005298,0x09090909
write 0x100000529c,0x09090909
write 0x1000005240,0x00090841
write 0x1000005244,0x12214800
write 0x1000005248,0x02020202
write 0x100000524c,0x02020202
write 0x1000005250,0x09090909
write 0x1000005254,0x09090909
write 0x10000052a0,0x00090841
write 0x10000052a4,0x12214800
write 0x10000052a8,0x02020202
write 0x10000052ac,0x02020202
write 0x10000052b0,0x09090909
write 0x10000052b4,0x09090909
write 0x1000005258,0x00090841
write 0x100000525c,0x12214800
write 0x1000005260,0x02020202
write 0x1000005264,0x02020202
write 0x1000005268,0x09090909
write 0x100000526c,0x09090909
write 0x10000052b8,0x00090841
write 0x10000052bc,0x12214800
write 0x10000052c0,0x02020202
write 0x10000052c4,0x02020202
write 0x10000052c8,0x09090909
write 0x10000052cc,0x09090909
write 0x1000005270,0x00090841
write 0x1000005274,0x12214800
write 0x1000005278,0x02020202
write 0x100000527c,0x02020202
write 0x1000005280,0x09090909
write 0x1000005284,0x09090909
write 0x10000052d0,0x00090841
write 0x10000052d4,0x12214800
write 0x10000052d8,0x02020202
write 0x10000052dc,0x02020202
write 0x10000052e0,0x09090909
write 0x10000052e4,0x09090909
write 0x10000052e8,0x00000000
write 0x10000052ec,0x00000000
write 0x1000005314,0x00000000
write 0x1000005378,0x24132413
write 0x100000537c,0x24132413

# initial delay of 300us
usleep 1000


# ----------------------------------------------------------------
# Starting Initialization sequence for DDRC0
# ----------------------------------------------------------------

# Disable DRAM initially
write 0x1000005000,0x00000000

# wait tINIT1 = 200us
usleep 1000


# Programming DDRC configurations and timing registers
# program DRAM_CONFIG1
write 0x1000005004,0x010f2890

# program DRAM_CONFIG2
write 0x1000005008,0x00001000

# program DRAM_TIMING1
write 0x100000500c,0x4b202012

# program DRAM_TIMING2
write 0x1000005010,0x0e200d6c

# program DRAM_TIMING3
write 0x1000005014,0x480f1fde

# program DRAM_TIMING4
write 0x1000005018,0x00251212

# program DRAM_TIMING5
write 0x100000501c,0x1643a288

# program DUAL_DIE_TIMING
write 0x1000005020,0x1f1f1f1f

# program REFRESH_TIMING
write 0x1000005024,0x0dbbf0fc

# program LP5_TIMING
write 0x1000005028,0x000039a1

# program DRAM_RSVD_SPACE
write 0x1000005038,0x7ff00000

# program DRAM_BYTE_MAP
write 0x100000503c,0x000000e4

# program DRAM_WDQS_TIMING
write 0x10000050c8,0x00002105

# program DTTE_TIMING
write 0x100000516c,0x00640040

# program DDRIO_PAD_CTRL
write 0x1000005360,0x00000010

# program DDRIO_DQS_PUPD
write 0x1000005364,0x00000006

# program DDRIO_CA_PADCTRL
write 0x1000005370,0x02c702c7

# program DDRIO_DQ_PADCTRL
write 0x1000005374,0x02970297

# program DDRIO_IBIAS_CTRL
write 0x1000005380,0x04000000

# wait 10ns to allow RESET/CKE PAD to start driving after DDS is set to non-0 value
usleep 1000


# Disable DRAM Reset
write 0x10000050d0,0x00000010

# wait tINIT3 = 2ms
usleep 2000


# issue DLL RST
write 0x100000502c,0x00000008

# Wait for completion
poll 0x100000502c,0x00000008,0x00000000

# wait 20 ns to make sure DLL reset done
usleep 1000


# Set CKE for Power down exit
write 0x10000050d0,0x0000001f

# wait for tINIT5 = 2us
usleep 1000


#  **** SET MODE REGISTERS FOR FSP0 ***

# Program MR13 -- set FS_WR to 0

# Program MR13
write 0x1000005030,0x9f0d0008

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Program MR3
write 0x1000005030,0x9f030031

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Program MR2
write 0x1000005030,0x9f020036

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Program MR1
write 0x1000005030,0x9f010064

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Program MR11
write 0x1000005030,0x9f0b0022

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Program MR14
write 0x1000005030,0x9f0e0032

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Program MR12
write 0x1000005030,0x9f0c0032

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Program MR22
write 0x1000005030,0x9f160004

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# Perform ZQ calibration for die 0
write 0x10000050ac,0x0000000d

# Program  DRAM_UINST1
write 0x10000050b0,0x00000f20

# Program  DRAM_UINST2
write 0x10000050b8,0x00000301

# Program  DRAM_UINST4
write 0x10000050bc,0x00000001

# Program  DRAM_UINST5
poll 0x10000050bc,0x00000001,0x00000000

# wait 1us (tZQCAL)
usleep 1000

write 0x10000050ac,0x0000000d

# Program  DRAM_UINST1
write 0x10000050b0,0x00001120

# Program  DRAM_UINST2
write 0x10000050b8,0x00000301

# Program  DRAM_UINST4
write 0x10000050bc,0x00000001

# Program  DRAM_UINST5
poll 0x10000050bc,0x00000001,0x00000000

# wait 30ns (TZQLAT)
usleep 1000


# trigger manual long PAD calibration
write 0x100000536c,0x00000280

# Wait for acknowledgment
poll 0x1000005384,0x00002000,0x00002000

# drop manual long PAD calibration
write 0x100000536c,0x00000200

# setting DDR to Read preamble mode

# Program MR13
write 0x1000005030,0x9b0d000a

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# set DQS gate to 1
write 0x1000005008,0x00000800
usleep 1000


# issue GET_RTT_EN command for die 0
write 0x100000502c,0x00000004

# Wait for completion
poll 0x100000502c,0x00000004,0x00000000
write 0x1000005008,0x00001000
usleep 1000


# unset DDR Read preamble mode

# Program MR13
write 0x1000005030,0x9b0d0008

# Wait for completion
poll 0x1000005030,0x80000000,0x00000000

# setup DDRIO dll Control settings
write 0x1000006210,0x20202020
write 0x1000006218,0x0f0f0f0f
write 0x1000006220,0x20202020
write 0x1000006214,0x20202020
write 0x100000621c,0x0f0f0f0f
write 0x1000006224,0x20202020
write 0x1000006200,0x00e1b881
write 0x1000006204,0x00e1b881
write 0x1000006208,0x00e1b881
write 0x100000620c,0x00e1b881
write 0x1000006228,0x00090841
write 0x100000622c,0x12214800
write 0x1000006230,0x02020202
write 0x1000006234,0x02020202
write 0x1000006238,0x09090909
write 0x100000623c,0x09090909
write 0x1000006288,0x00090841
write 0x100000628c,0x12214800
write 0x1000006290,0x02020202
write 0x1000006294,0x02020202
write 0x1000006298,0x09090909
write 0x100000629c,0x09090909
write 0x1000006240,0x00090841
write 0x1000006244,0x12214800
write 0x1000006248,0x02020202
write 0x100000624c,0x02020202
write 0x1000006250,0x09090909
write 0x1000006254,0x09090909
write 0x10000062a0,0x00090841
write 0x10000062a4,0x12214800
write 0x10000062a8,0x02020202
write 0x10000062ac,0x02020202
write 0x10000062b0,0x09090909
write 0x10000062b4,0x09090909
write 0x1000006258,0x00090841
write 0x100000625c,0x12214800
write 0x1000006260,0x02020202
write 0x1000006264,0x02020202
write 0x1000006268,0x09090909
write 0x100000626c,0x09090909
write 0x10000062b8,0x00090841
write 0x10000062bc,0x12214800
write 0x10000062c0,0x02020202
write 0x10000062c4,0x02020202
write 0x10000062c8,0x09090909
write 0x10000062cc,0x09090909
write 0x1000006270,0x00090841
write 0x1000006274,0x12214800
write 0x1000006278,0x02020202
write 0x100000627c,0x02020202
write 0x1000006280,0x09090909
write 0x1000006284,0x09090909
write 0x10000062d0,0x00090841
write 0x10000062d4,0x12214800
write 0x10000062d8,0x02020202
write 0x10000062dc,0x02020202
write 0x10000062e0,0x09090909
write 0x10000062e4,0x09090909
write 0x10000062e8,0x00000000
write 0x10000062ec,0x00000000
write 0x1000006314,0x00000000
write 0x1000006378,0x24132413
write 0x100000637c,0x24132413

# initial delay of 300us
usleep 1000


# ----------------------------------------------------------------
# Starting Initialization sequence for DDRC1
# ----------------------------------------------------------------

# Disable DRAM initially
write 0x1000006000,0x00000000

# wait tINIT1 = 200us
usleep 1000


# Programming DDRC configurations and timing registers
# program DRAM_CONFIG1
write 0x1000006004,0x010f2890

# program DRAM_CONFIG2
write 0x1000006008,0x00001000

# program DRAM_TIMING1
write 0x100000600c,0x4b202012

# program DRAM_TIMING2
write 0x1000006010,0x0e200d6c

# program DRAM_TIMING3
write 0x1000006014,0x480f1fde

# program DRAM_TIMING4
write 0x1000006018,0x00251212

# program DRAM_TIMING5
write 0x100000601c,0x1643a288

# program DUAL_DIE_TIMING
write 0x1000006020,0x1f1f1f1f

# program REFRESH_TIMING
write 0x1000006024,0x0dbbf0fc

# program LP5_TIMING
write 0x1000006028,0x000039a1

# program DRAM_RSVD_SPACE
write 0x1000006038,0x7ff00000

# program DRAM_BYTE_MAP
write 0x100000603c,0x000000e4

# program DRAM_WDQS_TIMING
write 0x10000060c8,0x00002105

# program DTTE_TIMING
write 0x100000616c,0x00640040

# program DDRIO_PAD_CTRL
write 0x1000006360,0x00000010

# program DDRIO_DQS_PUPD
write 0x1000006364,0x00000006

# program DDRIO_CA_PADCTRL
write 0x1000006370,0x02c702c7

# program DDRIO_DQ_PADCTRL
write 0x1000006374,0x02970297

# program DDRIO_IBIAS_CTRL
write 0x1000006380,0x04000000

# wait 10ns to allow RESET/CKE PAD to start driving after DDS is set to non-0 value
usleep 1000


# Disable DRAM Reset
write 0x10000060d0,0x00000010

# wait tINIT3 = 2ms
usleep 2000


# issue DLL RST
write 0x100000602c,0x00000008

# Wait for completion
poll 0x100000602c,0x00000008,0x00000000

# wait 20 ns to make sure DLL reset done
usleep 1000


# Set CKE for Power down exit
write 0x10000060d0,0x0000001f

# wait for tINIT5 = 2us
usleep 1000


#  **** SET MODE REGISTERS FOR FSP0 ***

# Program MR13 -- set FS_WR to 0

# Program MR13
write 0x1000006030,0x9f0d0008

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Program MR3
write 0x1000006030,0x9f030031

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Program MR2
write 0x1000006030,0x9f020036

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Program MR1
write 0x1000006030,0x9f010064

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Program MR11
write 0x1000006030,0x9f0b0022

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Program MR14
write 0x1000006030,0x9f0e0032

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Program MR12
write 0x1000006030,0x9f0c0032

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Program MR22
write 0x1000006030,0x9f160004

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Perform ZQ calibration for die 0
write 0x10000060ac,0x0000000d

# Program  DRAM_UINST1
write 0x10000060b0,0x00000f20

# Program  DRAM_UINST2
write 0x10000060b8,0x00000301

# Program  DRAM_UINST4
write 0x10000060bc,0x00000001

# Program  DRAM_UINST5
poll 0x10000060bc,0x00000001,0x00000000

# wait 1us (tZQCAL)
usleep 1000

write 0x10000060ac,0x0000000d

# Program  DRAM_UINST1
write 0x10000060b0,0x00001120

# Program  DRAM_UINST2
write 0x10000060b8,0x00000301

# Program  DRAM_UINST4
write 0x10000060bc,0x00000001

# Program  DRAM_UINST5
poll 0x10000060bc,0x00000001,0x00000000

# wait 30ns (TZQLAT)
usleep 1000


# trigger manual long PAD calibration
write 0x100000636c,0x00000280

# Wait for acknowledgment
poll 0x1000006384,0x00002000,0x00002000

# drop manual long PAD calibration
write 0x100000636c,0x00000200

# setting DDR to Read preamble mode

# Program MR13
write 0x1000006030,0x9b0d000a

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# set DQS gate to 1
write 0x1000006008,0x00000800
usleep 1000


# issue GET_RTT_EN command for die 0
write 0x100000602c,0x00000004

# Wait for completion
poll 0x100000602c,0x00000004,0x00000000
write 0x1000006008,0x00001000
usleep 1000


# unset DDR Read preamble mode

# Program MR13
write 0x1000006030,0x9b0d0008

# Wait for completion
poll 0x1000006030,0x80000000,0x00000000

# Enable AUTO Refresh - Enable DRAM - (for all DDRHOSTs)
write 0x1000004000,0x00000003

# DRAM controller is initialized...