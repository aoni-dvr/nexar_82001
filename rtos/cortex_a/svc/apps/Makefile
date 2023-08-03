# libsvc_app.a

APP_NAME = $(subst ",,$(CONFIG_SVC_APP_NAME))
ifneq ($(wildcard $(srctree)/$(src)/$(APP_NAME)),)
ifeq ($(CONFIG_SVC_APPS_ICAM),y)
ifeq ($(CONFIG_LINUX),y)
obj-y += $(APP_NAME)/main/build/linux/
else
obj-y += $(APP_NAME)/main/build/threadx/
endif
else
obj-y += $(APP_NAME)/
endif
endif
