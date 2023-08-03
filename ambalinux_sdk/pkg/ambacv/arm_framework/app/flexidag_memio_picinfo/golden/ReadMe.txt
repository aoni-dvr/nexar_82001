build :
	rm -rf flexidag_mnetssd_sensor && make -j flexidag_mnetssd_sensor-init && make flexidag_mnetssd_sensor-build

Copy to SD card:
	copy output\cv22_ambalink\diags\flexidag_mnetssd_sensor\bin to your SD
	card

run :	
cd /tmp/SD0/flexidag_mnetssd_sensor
modprobe ambacv.ko
visload -l visorc
cd flexidag_memio_picinfo
./flexidag_memio_picinfo -f ../flexibin/ -d ../golden -n 1 -t 1

-f : flexbin folder path
-d : file path
-n : loop number
-t : thread number (max number = 4)
