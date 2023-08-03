#source /opt/fsl-imx-wayland/5.15-honister/environment-setup-cortexa53-crypto-poky-linux
#source  /opt/poky/imx8/environment-setup-aarch64-poky-linux
#export MY_KERNEL=/home/chuanquan/work/imx/imx8-evk-yocto-linux/linux-imx_lf-5.15.5-1.0.0/
#export MY_KERNEL=/home/chuanquan/work/imx/imx8-evk-yocto-linux/linux-imx_lf-5.10.y
#source /opt/fsl-imx-xwayland/5.4-zeus/environment-setup-aarch64-poky-linux
#export MY_KERNEL=/home/chuanquan/work/imx/imx8-evk-yocto-linux/linux-imx-5.4.24-2.1.0
#export MY_KERNEL=/home/chuanquan/work/imx/ea-imx8mq/imx8_linux_ea_5.10.35
#export MY_KERNEL=/home/chuanquan/work/imx/ea-imx8mq/imx8_linux_ea_5.15.y
export CROSS_COMPILE=/opt/amba/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-
export ARCH=arm64
export MY_KERNEL=$HOME/work/amba/cv2x_linux_pure_sdk_3.0.8/ambarella/out/cv25_hazelnut/kernel/linux-5.4_cv25



export KDIR=${MY_KERNEL}

make clean
#make -j8 CONFIG_CYW89570=y CONFIG_BCMDHD=m CONFIG_BCMDHD_SDIO=y CONFIG_BCMDHD_SDMMC=y CONFIG_HAVE_IMX8_SOC=y CONFIG_BCMDHD_VENDOR_EXT=y
make -j8 CONFIG_BCMDHD=m CONFIG_BCMDHD_SDIO=y CONFIG_BCMDHD_SDMMC=y CONFIG_BCMDHD_VENDOR_EXT=y
 
