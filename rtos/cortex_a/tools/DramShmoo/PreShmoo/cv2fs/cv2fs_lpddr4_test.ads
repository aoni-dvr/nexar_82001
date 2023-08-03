# Init DRAM controller...
write 0xed0800dc,0x21101000
write 0xed080110,0x30520000
write 0xed080114,0x00088002
write 0xed0800dc,0x21101001
write 0xed0800dc,0x21101000

# idsp pll setting
write 0xed0800e4,0x1e100000
write 0xed0800e4,0x1e100001
write 0xed0800e4,0x1e100000

# vision pll setting
write 0xed0806e4,0x3e110000
write 0xed0806e4,0x3e110001
write 0xed0806e4,0x3e110000

#program DDRIO_DLL_CTRL_MISC 
write 0xdffe10ec, 0x0188000

write 0xdffe1230,0x0001b9b1
write 0xdffe12b0,0x0001b9b1
write 0xdffe1330,0x0001b9b1
write 0xdffe13b0,0x0001b9b1

write 0xdffe122c,0x00000000
write 0xdffe12ac,0x00000000
write 0xdffe132c,0x00000000
write 0xdffe13ac,0x00000000

write 0xdffe1234,0x00000000
write 0xdffe12b4,0x00000000
write 0xdffe1334,0x00000000
write 0xdffe13b4,0x00000000

usleep 2000

write 0xdffe1238,0x00000014
write 0xdffe12b8,0x00000014
write 0xdffe1338,0x00000014
write 0xdffe13b8,0x00000014

write 0xdffe0000,0x00000010

usleep 1000

write 0xdffe10f0,0x00000000

write 0xdffe1000,0x6cc03a80

usleep 1000

write 0xdffe1004,0x702800c8
write 0xdffe1008,0x445e69a7
write 0xdffe100c,0x14a1c4d6
write 0xdffe1010,0x000000c4
write 0xdffe1014,0x11ce731f
write 0xdffe1024,0x03002a00
write 0xdffe102c,0x00001000
write 0xdffe1044,0x90580000
write 0xdffe1048,0x09039003
write 0xdffe104c,0x100aa600
write 0xdffe10a4,0x0000a4b8
write 0xdffe10dc,0x00000001
write 0xdffe10e8,0x00000038
write 0xdffe10f0,0x00000000
write 0xdffe10f4,0x00500000
write 0xdffe1140,0x00032032

# BYTE0 WRITE DELAY
write 0xdffe1200,0x57575757
write 0xdffe1204,0x57575757
write 0xdffe1208,0x00000057
write 0xdffe120c,0x00000000
write 0xdffe1210,0x00000000
write 0xdffe1214,0x00000000

write 0xdffe1228,0x00000000

# BYTE1 WRITE DELAY
write 0xdffe1280,0x57575757
write 0xdffe1284,0x57575757
write 0xdffe1288,0x00000057
write 0xdffe128c,0x00000000
write 0xdffe1290,0x00000000
write 0xdffe1294,0x00000000

# program DQS1_DLY
write 0xdffe12a8,0x00000000

# BYTE2 WRITE DELAY
write 0xdffe1300,0x57575757
write 0xdffe1304,0x57575757
write 0xdffe1308,0x00000057
write 0xdffe130c,0x00000000
write 0xdffe1310,0x00000000
write 0xdffe1314,0x00000000

# program DQS2_DLY
write 0xdffe1328,0x00000000

# BYTE3 WRITE DELAY
write 0xdffe1380,0x57575757
write 0xdffe1384,0x57575757
write 0xdffe1388,0x00000057
write 0xdffe138c,0x00000000
write 0xdffe1390,0x00000000
write 0xdffe1394,0x00000000

# program DQS3_DLY
write 0xdffe13a8,0x00000000

# BYTE0 READ DELAY
write 0xdffe1218,0x0e739ce7
write 0xdffe121c,0x00001ce7

# BYTE1 READ DELAY
write 0xdffe1298,0x0e739ce7
write 0xdffe129c,0x00001ce7

# BYTE2 READ DELAY
write 0xdffe1318,0x0e739ce7
write 0xdffe131c,0x00001ce7

# BYTE3 READ DELAY
write 0xdffe1398,0x0e739ce7
write 0xdffe139c,0x00001ce7

usleep 1000

# Disable DRAM Reset
write 0xdffe1000,0x6cc03a88

usleep 1000

# issue DLL RST
write 0xdffe1018,0x00000008

# Wait for completion
poll 0xdffe1018,0x00000008,0x00000000

usleep 1000

# Set CKE 
write 0xdffe1000,0x6e401a8c

usleep 1000

# program DRAM_RSVD_SPACE
write 0xdffe1030,0x7ff00000

# write to DRAM_BYTE_MAP
write 0xdffe1034,0x004400e4

write 0xdffe101c,0x010d0000

usleep 1000

poll 0xdffe101c,0x80000000,0x00000000

write 0xdffe101c,0x01030031

usleep 1000
poll 0xdffe101c,0x80000000,0x00000000
write 0xdffe101c,0x0102001b
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000
write 0xdffe101c,0x01010034
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000

write 0xdffe101c,0x010b0024
# Wait for completion
poll 0xdffe101c,0x80000000,0x00000000

# Program MR14
write 0xdffe101c,0x010e0008
# Wait for completion
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000

write 0xdffe101c,0x010e0008
# Wait for completion
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000

write 0xdffe101c,0x010c000c
# Wait for completion
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000

usleep 1000
poll 0xdffe101c,0x80000000,0x00000000
write 0xdffe101c,0x010d0040
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000
write 0xdffe101c,0x01030031
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000
write 0xdffe101c,0x0102001b
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000
write 0xdffe101c,0x01010034
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000
write 0xdffe101c,0x010b0024
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000

write 0xdffe101c,0x010e0008
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000

write 0xdffe101c,0x010c000c
usleep 1000
poll 0xdffe101c,0x80000000,0x00000000

# Set CS_CNTRL to Select die 0 only
write 0xdffe1000,0x2cc03a8c


# issue Manual DDR ZQ CALIB command
write 0xdffe1018,0x00000010

usleep 1000

# Wait for completion
poll 0xdffe1018,0x00000010,0x00000000

# Set CS_CNTRL to Select die 1 only
write 0xdffe1000,0x4cc03a8c

# issue Manual DDR ZQ CALIB command
write 0xdffe1018,0x00000010

# Wait for completion
poll 0xdffe1018,0x00000010,0x00000000


# Set CS_MODE back to default
write 0xdffe1000,0x6cc03a8c

# wait 30 ns (tZQLAT)
usleep 1000

# issue LONG PAD CALIB command
write 0xdffe1018,0x00000040

usleep 1000

# Wait for completion
poll 0xdffe1018,0x00000020,0x00000000


# issue GET_RTT_EN command
write 0xdffe1018,0x00000004

usleep 1000

# Wait for completion
poll 0xdffe1018,0x00000004,0x00000000

# Enable AUTO Refresh - Enable DRAM - (for all DDRHOSTs)
write 0xdffe10f0, 0x00000003

usleep 1000

#write  0x0,0x14000000
#write  0xf2000028,0x000e0000

#write  0xf200000c,0x1
#write  0xf2000008,0x1
#write  0xf200000c,0x1

#write  0xf2000090,0x0
#write  0xf2000094,0x0
#write  0xf2000098,0x0
#write  0xf20000a0,0x0
#write  0xf20000a4,0x0
#write  0xf20000a8,0x0

#write  0xf2000100,0xf0000000
#write  0xf2000104,0xf00fffff
