obj-y += libambacv.a

ccflags-y += -I$(srctree)/$(CV_COMSVC_DIR)/cv_common/inc/cvapi

libambacv-objs += AmbaCV_Init.o
libambacv-objs += AmbaCV_Utils.o
install-files += libambacv.a

