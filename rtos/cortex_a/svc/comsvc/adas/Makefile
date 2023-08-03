# THIS FILE WON'T WORK!!		1
# If you want to make this file work, please add following line in Makefile.Amba		2
#   refcode_dirs»       »       += $(call if_exist,adas)


ifdef CONFIG_BUILD_AMBA_ADAS
ifneq ($(wildcard $(srctree)/$(src)/src),)
obj-y += src/
endif
obj-y += refflow/
endif

ifdef CONFIG_BUILD_AMBA_ADAS_UTILS
obj-y += utils/
endif

ifdef CONFIG_BUILD_AMBA_ADAS_UNITTEST
obj-y += unittest/
endif

