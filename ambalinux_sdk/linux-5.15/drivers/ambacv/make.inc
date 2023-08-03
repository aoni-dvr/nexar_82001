##
## kernel/private/drivers/ambacv/make.inc
##
## History:
##    2017/05/23 - [Zhikan Yang] Created file
##
## Copyright (c) 2017 Ambarella, Inc.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(BUILD_AMBARELLA_AMBACV_DRV), y)

LOCAL_PATH	:= $(call my-dir)

###
include $(CLEAR_VARS)

LOCAL_SRCS	+= $(wildcard $(LOCAL_PATH)/*.c) \
		   $(wildcard $(LOCAL_PATH)/*.i) \
		   $(wildcard $(LOCAL_PATH)/*.h)
LOCAL_SRCS	+= $(LOCAL_PATH)/Kbuild

include $(BUILD_DRIVER)

endif
