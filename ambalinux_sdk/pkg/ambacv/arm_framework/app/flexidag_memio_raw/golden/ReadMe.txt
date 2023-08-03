build :
	rm -rf flexidag_bisenet_mnv2_raw && make -j flexidag_bisenet_mnv2_raw-init && make flexidag_bisenet_mnv2_raw-build

copy to SD card:
	copy output\cv22_ambalink\diags\flexidag_bisenet_mnv2_raw/bin to SD
	card

run :	
cd /tmp/SD0/flexidag_bisenet_mnv2_raw/
modprobe ambacv.ko
visload -l visorc
cd flexidag_memio_raw
./flexidag_memio_raw -f ../flexibin/ -d /tmp/SD0/ -n 0xffffffff -t 1


-f : flexbin folder path
-d : file path
-m : 0 for send raw struct to orc, 1 for send raw data to orc  
-n : loop number
-t : thread number (max number = 4)
