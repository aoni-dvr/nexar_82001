# Init DRAM controller...


# cortex pll setting
write 0xed080264,0x29100000
write 0xed080264,0x29100001
write 0xed080264,0x29100000

# core pll setting
write 0xed080000,0x25110000
write 0xed080000,0x25110001
write 0xed080000,0x25110000

# idsp pll setting
write 0xed0800e4,0x25110000
write 0xed0800e4,0x25110001
write 0xed0800e4,0x25110000

# vision pll setting
write 0xed0806e4,0x29110000
write 0xed0806e4,0x29110001
write 0xed0806e4,0x29110000

# dram pll setting
write 0xed0800dc,0x63111000
write 0xed080110,0x30520000
write 0xed080114,0x00088004
write 0xed0800dc,0x63111001
write 0xed0800dc,0x63111000

# Program Number of DDRHOST in DRAM
write 0xdffe0000,0x00000010

# setup DDRIO dll Control settings
write 0xdffe0900,0x000c000c
write 0xdffe0904,0x000c000c
write 0xdffe0908,0x000c000c
write 0xdffe090c,0x000c000c
write 0xdffe0920,0x0062b9b1
write 0xdffe0924,0x0062b9b1
write 0xdffe0928,0x0062b9b1
write 0xdffe092c,0x0062b9b1
write 0xdffe0910,0x00188000
write 0xdffe0940,0x0000b081
write 0xdffe0944,0x0000b081
write 0xdffe0948,0x0000b081
write 0xdffe094c,0x0000b081

# wait for clean clock 200 us
usleep 1000


# ----------------------------------------------------------------
# Starting Initialization sequence for DDRC0
# ----------------------------------------------------------------

# Reset Dram -- disable AUTO_REFRESH initially
write 0xdffe0800,0x6e402680

# issue DLL RST command in order to lock on the slow clock
write 0xdffe0818,0x00000008

# Wait for completion
poll 0xdffe0818,0x00000008,0x00000000

# wait 20 ns to make sure DLL reset done
usleep 1000


# program DRAM_RSVD_SPACE
write 0xdffe0830,0x7ff00000

# disable DQS SYNC initially
write 0xdffe0824,0x0301ea00

# write to DRAM_CFG reg
write 0xdffe0804,0x702862d8

# write to DRAM_TIMING reg
write 0xdffe0808,0x6694c28b

# write to DRAM_TIMING2 reg
write 0xdffe080c,0x18c2a148

# write to DRAM_TIMING3 reg
write 0xdffe0810,0x20000124

# write to DRAM_TIMING4 reg
write 0xdffe0814,0x1296ac2f

# write to DRAM_TIMING5_LP4TRAIN reg
write 0xdffe0844,0x6c588000

# write to PAD_TERM
write 0xdffe0828,0x00020074

# write to DRAM_PAD_TERM2
write 0xdffe0848,0x090d990d

# write to DRAM_PAD_TERM3
write 0xdffe084c,0x00965000

# write to DRAM_BYTE_MAP
write 0xdffe0834,0x000000e4

# write to LPDDR4_DQ_CA_VREF
write 0xdffe08b4,0x03468d1a

# write to LPDDR4_DQ_WRITE_DLY
write 0xdffe08a4,0xc56ad5ab

# write to LPDDR4_DQ_READ_DLY
write 0xdffe08a8,0x0005294a

# write to LPDDR4_DQS_WRITE_DLY
write 0xdffe08ac,0x00000000

# write to LPDDR4_CA_CK_DLY
write 0xdffe08b0,0x00000000

# DRAM_ZQ_CALIB config to set T_ZQC_SEL_POR_MDDR4 for DDR ZQ Calibration
write 0xdffe082c,0x00000460

# write to LPDDR4_TRAIN_MPC_RDLY
write 0xdffe08cc,0x00000030

# wait 10 ns to allow RESET/CKE PAD to start driving after DDS is set to non-0 value
usleep 1000


# Disable DRAM Reset
write 0xdffe0800,0x6e402688

# wait 2 ms (tINIT3)
usleep 2000


# Set CKE
write 0xdffe0800,0x6e40268c

# wait 2 us (tINIT5)
usleep 1000


#  **** SET MODE REGISTERS FOR FSP0 ***

# Program MR13 -- set FS_WR to 0

# Program MR13
write 0xdffe081c,0x010d0008

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR3
write 0xdffe081c,0x01030033

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR2
write 0xdffe081c,0x01020024

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR1
write 0xdffe081c,0x01010044

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR11
write 0xdffe081c,0x010b0011

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR14
write 0xdffe081c,0x010e001c

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR12
write 0xdffe081c,0x010c0026

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR22
write 0xdffe081c,0x01160000

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

#  **** SET MODE REGISTERS FOR FSP1 ***

# Program MR13 -- set FS_WR to 1

# Program MR13
write 0xdffe081c,0x010d0048

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR3
write 0xdffe081c,0x01030033

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR2
write 0xdffe081c,0x01020024

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR1
write 0xdffe081c,0x01010044

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR11
write 0xdffe081c,0x010b0011

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR14
write 0xdffe081c,0x010e001c

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR12
write 0xdffe081c,0x010c0026

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Program MR22
write 0xdffe081c,0x01160000

# Wait for completion
poll 0xdffe081c,0x80000000,0x00000000

# Set CS_CNTRL to Select die 0 only
write 0xdffe0800,0x2e40268c

# issue Manual DDR ZQ CALIB command
write 0xdffe0818,0x00000010

# Wait for completion
poll 0xdffe0818,0x00000010,0x00000000

# Set CS_CNTRL to Select die 1 only
write 0xdffe0800,0x4e40268c

# issue Manual DDR ZQ CALIB command
write 0xdffe0818,0x00000010

# Wait for completion
poll 0xdffe0818,0x00000010,0x00000000

# Set CS_MODE back to default
write 0xdffe0800,0x6e40268c

# wait 30 ns (tZQLAT)
usleep 1000


# issue LONG PAD CALIB command
write 0xdffe0818,0x00000020

# Wait for completion
poll 0xdffe0818,0x00000020,0x00000000

# issue GET_RTT_EN command
write 0xdffe0818,0x00000004

# Wait for completion
poll 0xdffe0818,0x00000004,0x00000000

# write DRAM_DQS_SYNC config
write 0xdffe0824,0x0301ea00

# write DRAM_ZQ_CALIB config
write 0xdffe082c,0x00000460

# write DRAM_DLL_CALIB config
write 0xdffe083c,0x001e3204

# write to selfRefresh register
write 0xdffe0820,0x00000001

# write to AUTO power down register
write 0xdffe0838,0x00000030

# Enable AUTO Refresh - Enable DRAM - (for all DDRHOSTs)
write 0xdffe0800,0x6e40268f

# DRAM controller is initialized...
