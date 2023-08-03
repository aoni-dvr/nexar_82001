ifeq ($(CONFIG_ENABLE_AMBALINK),y)

obj-y += app.o

#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))


#RPMSG_PROFILE := y

app-objs := \
	    AmbaIPC_Test.o

ifneq ($(CONFIG_AMBALINK_BOOT_OS),)
app-objs += \
	    AmbaIPC_Client.o	\
	    AmbaIPC_Clock.o	\
	    AmbaIPC_LinkCtrl.o	\
	    AmbaIPC_Rpc.o	\
	    AmbaIPC_SD.o	\
	    AmbaIPC_Svc.o	\
	    AmbaIPC_Vfs.o

ifeq ($(CONFIG_BUILD_CV),y)
app-objs += \
	    AmbaIPC_FlexidagIO.o	\
	    AmbaIPC_CVProtection.o
endif

ifneq ($(RPMSG_PROFILE),)
app-objs += \
	    AmbaIPC_Profile.o

#ccflags-y += -DRPMSG_PROFILE
endif

ccflags-y += -I$(srctree)/vendors/foss/libwrapc

endif # CONFIG_AMBALINK_BOOT_OS

endif # $(CONFIG_ENABLE_AMBALINK)

