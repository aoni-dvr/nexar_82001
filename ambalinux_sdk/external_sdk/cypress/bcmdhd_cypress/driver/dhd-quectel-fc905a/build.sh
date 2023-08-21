source /opt/fsl-imx-wayland/5.15-honister/environment-setup-cortexa53-crypto-poky-linux
#export MY_KERNEL=/home/chuanquan/work/imx/imx8-evk-yocto-linux/linux-imx_lf-5.15.5-1.0.0/
#export MY_KERNEL=/home/chuanquan/work/imx/imx8-evk-yocto-linux/linux-imx_lf-5.10.y
#source /opt/fsl-imx-xwayland/5.4-zeus/environment-setup-aarch64-poky-linux
#export MY_KERNEL=/home/chuanquan/work/imx/imx8-evk-yocto-linux/linux-imx-5.4.24-2.1.0
#export MY_KERNEL=/home/chuanquan/work/imx/ea-imx8mq/imx8_linux_ea_5.10.35
#export MY_KERNEL=/home/chuanquan/work/imx/ea-imx8mq/imx8_linux_ea_5.15.y
#export MY_KERNEL=/home/chuanquan/work/imx/ea-imx8mq/imx8_linux_ea_4.14.78
#export MY_KERNEL=/home/chuanquan/work/imx/ea-imx8mq/imx8_linux_ea_5.4.47
export MY_KERNEL=/home/david/work/imx8/linux-imx_lf-5.15.5-1.0.0
export KDIR=/home/david/work/imx8/linux-imx_lf-5.15.5-1.0.0


export KERNEL_SRC=${MY_KERNEL}

make clean
#make -j8 CONFIG_CYW89570=y CONFIG_BCMDHD=m CONFIG_BCMDHD_SDIO=y CONFIG_BCMDHD_SDMMC=y CONFIG_HAVE_IMX8_SOC=y CONFIG_BCMDHD_VENDOR_EXT=y
#make -j8 CONFIG_BCM4373=y CONFIG_BCMDHD=m CONFIG_BCMDHD_SDIO=y CONFIG_BCMDHD_SDMMC=y CONFIG_BCMDHD_VENDOR_EXT=y
make -j8 CONFIG_BCMDHD=m CONFIG_BCMDHD_SDIO=y CONFIG_BCMDHD_SDMMC=y CONFIG_BCMDHD_VENDOR_EXT=y CONFIG_BCMDHD_WPA3=y CONFIG_ANDROID=y 
#make -j8 CONFIG_BCMDHD=m CONFIG_BCMDHD_PCIE=y CONFIG_ANDROID=y 
 
