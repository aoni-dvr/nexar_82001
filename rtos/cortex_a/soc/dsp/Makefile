# THIS FILE WON'T WORK!!
# If you want to make this file work, please add following line in Makefile.Amba
#   ssp_common_dirs += $(call if_exist,ssp)

ifneq ($(wildcard $(srctree)/$(src)/visual),)
obj-$(CONFIG_BUILD_DSP_VISUAL) += visual/build/threadx/
else
endif

ifneq ($(wildcard $(srctree)/$(src)/audio),)
obj-$(CONFIG_BUILD_DSP_AUDIO) += audio/
endif

ifneq ($(wildcard $(srctree)/$(src)/audio/aac/dec),)
obj-$(CONFIG_ENABLE_AAC_DECODER) += audio/aac/dec/
endif

ifneq ($(wildcard $(srctree)/$(src)/audio/aac/enc),)
obj-$(CONFIG_ENABLE_AAC_ENCODER) += audio/aac/enc/
endif

ifdef CONFIG_SOC_CV2
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv2x),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv2x/build/threadx/
endif
endif

ifdef CONFIG_SOC_CV22
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv2x),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv2x/build/threadx/
endif
endif

ifdef CONFIG_SOC_CV25
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv2x),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv2x/build/threadx/
endif
endif

ifdef CONFIG_SOC_H32
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv2x),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv2x/build/threadx/
endif
endif

ifdef CONFIG_SOC_CV28
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv2x),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv2x/build/threadx/
endif
endif

ifdef CONFIG_SOC_CV2FS
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv2xfs),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv2xfs/build/threadx/
endif
endif

ifdef CONFIG_SOC_CV20
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv2xfs),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv2xfs/build/threadx/
endif
endif

ifdef CONFIG_SOC_CV5
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv5x),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv5x/build/threadx/
endif
endif

ifdef CONFIG_SOC_CV52
ifneq ($(wildcard $(srctree)/$(src)/imgkernel/imgkernel/cv5x),)
obj-$(CONFIG_BUILD_IK) += imgkernel/imgkernel/cv5x/build/threadx/
endif
endif
