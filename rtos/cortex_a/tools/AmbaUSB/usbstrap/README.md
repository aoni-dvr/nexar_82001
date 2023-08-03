Ambarella USB Bootstrap
=======================

This document describes the AmbUST on CV5 or later platform.

AmbUST is a usb bootstrap running in force USB mode. It is responsible for DRAM initialization
and booting an executable firmware with Android fastboot protocol.

The size AmbUST binary is **12KB**, and it's **mandatory**.

pack_image_usb
--------------

**pack_image_usb** is a host tool, responsible for transitioning AmbUST binary to 16KB SRAM in CV5.
After the transitioning is completed, ARM will jump and execute AmbUST.

How to generate an AmbUST binary
--------------------------------
AmbUST needs to initialize DRAM, so we using **bstiniparse** to generate DRAM parameters, and take **.ini**  file as input.

```
	$ cd ambarella/cv5_timn
	$ source ../../build/env/aarch64-linux5.4-gcc.env
	$ make sync_build_mkcfg
	$ make cv5_ipcam_config
	$ make menuconfig

		Ambarella Boot Configuration --->
			USBstrap Configuration --->
				USBstrap DRAM type (LPDDR5)  --->
				($(AMB_BOARD_DIR)/bsp/cv5_timn_540_micron_lpddr5.ini) .ini file in USBstrap used for DRAM initialization

```
NOTE:
	[1] DRAM traning is currently unimplemented in AmbUST, so please use a low enough DRAM clock to guarantee the DRAM stability.
	[2] Please use a correct **.ini** file, and check the following parameters in **.ini** file. Refer to *amboot/doc/bstiniparse.txt* to know more details

	DRAM_TYPE			# LP4 or LP5
	DRAM_BUS_WIDTH		# 32 bits or 64 bits

```
	$ make ambust

	$ ls ../../out/cv5_timn/images/
		cv5_timn_ambust_lpddr5_signed.bin

	$ du -sb ../../out/cv5_timn/images/cv5_timn_ambust_lpddr5_signed.bin
		12288	../../out/cv5_timn/images/cv5_timn_ambust_lpddr5_signed.bin
```

Separately build AmbUST binary
-------------------------------
To separately build the AmbUST binary, change to the root directory of the AmbUST source tree and follow these steps:

```
	$ export CROSS_COMPILE=aarch64-linux-gnu-
	$ mkdir out
	$ make LOCAL_OUTPUT=out DRAM_CFG=config/cv5_64bit_540_micron_lpddr5.ini DRAM_TYPE=lpddr5 parameter	# DRAM_TYPE=lpddr5 or lpddr4
	$ make LOCAL_OUTPUT=out DRAM_CFG=config/cv5_64bit_540_micron_lpddr5.ini DRAM_TYPE=lpddr5 all
	$ ls out/*_signed.bin
	out/ambust_lpddr5_signed.bin
```
Please set a correct `.ini` file and DRAM type.
To understand the setting in `.ini` file, please refer to `amboot/doc/bstiniparser.txt`.

CV5 Recovery
------------
1. Enter force USB mode by switching on POC6.

2. Transition AmbUST binary using pack_image_usb

   ```
   $ pack_image_usb -f ../../out/cv5_timn/images/cv5_timn_ambust_lpddr5_signed.bin
   ...
   Write: 12192/12288
   Write: 12208/12288
   Write: 12224/12288
   Write: 12240/12288
   Write: 12256/12288
   Write: 12272/12288
   Write: 12288/12288
   Transfer ../../out/cv5_timn/images/cv5_timn_ambust_lpddr5_signed.bin Done.

   # UART output
   *** Ambarella Recovery ***
   Enumerated HIGH speed
   Enumerated HIGH speed
   ```

3.  Boot an executable binary using fastboot

    ```
	$ fastboot devices
	123456789ABC	fastboot

	$ fastboot boot ../../out/cv5_timn/images/bst_bld_release.bin -b 0x4000000		# 0x4000000 is MEMFWPROG_RAM_START
	creating boot image...
	creating boot image - 542720 bytes
	downloading 'boot.img'...
	OKAY [  0.271s]
	booting...
	OKAY [  0.125s]
	finished. total time: 0.397s

    ```

    Note: You may meet following log output from UART, if so, please check you DRAM parameters.

    ```
    Data corruption, please check the DRAM parameter
    ```
