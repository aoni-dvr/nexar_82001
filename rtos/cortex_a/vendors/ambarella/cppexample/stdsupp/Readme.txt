This is the example package for C++ support.
It shows how to add C++ support to RTOS and user will have to add more implementation to fulfill the freature he needs.

There are several known limitations:
1. it does not support "exception".
2. it does not support "RTTI". (https://en.wikibooks.org/wiki/C%2B%2B_Programming/RTTI)
3. it does not support fully libstd/libstdc++. User may need to do workaround or add low level implementation if there is any un-supported feature used in his flow.

There is no MMU implementation in this example.
Therefore, it will have problem after some malloc()/free() operations.
User has to implement better MMU flow for this requirement.

-[Please note]-
  The command starts with prompt '$ ' means it is for PC/Server;
  The command starts with prompt 'a:\> '  means it is for RTOS console;


[How to run the example]
Please follow these steps to build with C++ support.

1. apply patch to rtos and refapp/svc.
Since code base might be changed, if the patch doesn't work, please refer to them and modify by yourself.
$ cd rtos/cortex_a
$ git apply vendors/ambarella/cppexample/stdsupp/patch/rtos_cxx_test.patch
$ cd svc
$ git apply ../vendors/ambarella/cppexample/stdsupp/patch/svc_cxx_test.patch

2. source build environment. (in docker container)
$ source /opt/amba/env/env_set.sh [chip_id]

3. under rtos/cortex_a/, apply defconfig and then enable C++ support as well as use customized syscall library.
$ make cv22/icam/tx32/cv22_tx32_bub_icam_cv_nand_lt6911_defconfig
$ make menuconfig
GCC Options  --->
	-*- Support C++ files (code size will be bigger)
	-*- Use standard libraries from toolchain

CPP Support Example  --->
	[*] Build libsyscall.a

4. build rtos firmware.
$ make diags && make -j

5. use AmbaUSB to update firmware.

6. run cxx test after system boot.
a:\> svc_app test_cxx

If it runs successfully, the result will look like:
cpp_test ....
string var using test 1
string var using test 2
testType: 0x11
string var using test 3
b6=0.995000
b7=0.995000
b8=0.995000
test here1
d: 0.000000
Push 17
Size: 11
Sort
11 12 17 21 30 31 41 55 66 80 98
Find 31...
Got it!
Pop last
Reverse sort
80 66 55 41 31 30 21 17 12 11


[Customization]
To enable C++ support in your own RTOS application flow, you will have to:
1. enable required part in lds.
	You can refer to "#ifdef CONFIG_CC_CXX_SUPPORT" and "#ifdef CONFIG_CC_USESTD" parts in rtos/cortex_a/svc/apps/icam/main/build/threadx/amba_svc.lds.S.
	If you would like to use malloc() in C++ implementation, you might have to change the size of sbrk to meet your requirement:
	#ifdef CONFIG_CC_USESTD
	. = __sbrk_start + 0x500000;
	#else

	Please note, you should not do malloc()/free() rapidly; otherwise it will meet error.
	It is better to do malloc() once and reuse the buffer in your flow. (or do a better implementation for _sbrk())

2. link required libraries in link-amba_data.sh.
	You can refer to rtos/cortex_a/svc/apps/icam/main/build/threadx/link-amba_data.sh.

3. to use stdlib, you will have to implement and link with libsyscall.a.
	You can refer to rtos/cortex_a/vendors/ambarella/cppexample/syscall for sample implementation.
	Some of the features are left as empty in this sample implementation.
	You will have to implement them if you need to use these features.

4. call AmbaCRT0() in your APP flow before using C++ functions.
	You can refer to rtos/cortex_a/svc/apps/icam/main/src/cmd/SvcCmdApp.c.

5. then, you could add your C++ implementation and use it on RTOS.
	You can refer to rtos/cortex_a/vendors/ambarella/cppexample/stdsupp/test/cpp_case.cpp.


[partial opencv porting test]
Beside basic C++ test, we also verified c++ environment with opencv's canny feature.
Please note, this is just a porting for test and only partial of opencv features are ported.
If you have to use opencv in your flow, you may need to port it by your own.

To enable the test, please do as follow:
1. download opencv 3.0.0-rc1 source from official website.
(https://github.com/opencv/opencv/archive/refs/tags/3.0.0-rc1.zip)

2. unzip the opencv 3.0.0-rc1 source package and put it into rtos/cortex_a/vendors/ambarella/cppexample/ as "opencv" folder.

3. apply patch for c++ environment.
$ cd rtos/cortex_a/vendors/ambarella/cppexample/opencv
For the users, who uses SDK release, please use this command line to apply the patch:
$ git apply --whitespace=nowarn ../stdsupp/patch/opencv_3.0.0-rc1.patch
For Ambarella enginner, who checkout code from internal git repository, you will need to use this command instead:
$ git apply --whitespace=nowarn --directory=vendors/ambarella/cppexample/opencv ../stdsupp/patch/opencv_3.0.0-rc1.patch

4. under rtos/cortex_a/, enable CONFIG_BUILD_FOSS_OPENCV in menuconfig.
$ make menuconfig
CPP Support Example  --->
	[*]   Build OpenCV libraries

5. build firmware.
$ make -j

6. use AmbaUSB to update firmware.

7. copy test image file to SD card.
	The test file is placed at rtos/cortex_a/vendors/ambarella/cppexample/opencv/modules/test/data/fruits.y.
	Please copy it to SD card (under root folder) and then insert this SD card to BUB.

8. run cxx test after system boot.
a:\> svc_app test_cxx

If it runs successfully, the result will look like:
cpp_test ....
string var using test 1
string var using test 2
string var using test 3
b6=0.995000
b7=0.995000
b8=0.995000
test here1
Push 17
Size: 11
Sort
11 12 17 21 30 31 41 55 66 80 98
Find 31...
Got it!
Pop last
Reverse sort
80 66 55 41 31 30 21 17 12 11
in_file(c:/fruits.y) size=245760
MatType=0, MatSize = 245760 (512 x 480 x 1)
Opencv_CannyTest() returns 0

9. check the output file "fruits_res.y" on PC.
