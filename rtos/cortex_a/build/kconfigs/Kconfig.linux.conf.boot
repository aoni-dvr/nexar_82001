menu "Linux Cofiguration"

config BOOT_CORE_LINUX
	int
	default 0

config LINUX_MEM_ADDR
	hex "Start address of memory for Linux usage"
	depends on !DEVICE_TREE_SUPPORT
	help
	  For arm64, the text_offset can be assumed to be 0x8_0000.
	  So LINUX_MEM_ADDR - (LINUX_MEM_ADDR) could be used for shared memory,
	  e.g. RPMSG, Spin/Mutex-Lock, DTB data, etc.
	  cf. FWPROG_LNX_LOADADDR

config LINUX_MEM_SIZE
	hex "Total size of memory allocated for Linux."
	depends on !DEVICE_TREE_SUPPORT
	help
	  Total size of memory allocated for Linux.
	  Including kernel/user space and shared memory.
	  This size MUST be 2MB aligned!

config LINUX_CMDLINE
	string "Command line parameters of Linux"
	default "root=/dev/mtdblock5 rootfstype=squashfs console=ttyS1 nr_cpus=4 maxcpus=1" if !ENABLE_EMMC_BOOT
	default "root=/dev/mmcblk0p5 rootfstype=ext2 rootwait ro console=ttyS1 nr_cpus=4 maxcpus=1" if ENABLE_EMMC_BOOT
	help
	  Comand line parameters for Linux.

endmenu # "Linux Cofiguration"


