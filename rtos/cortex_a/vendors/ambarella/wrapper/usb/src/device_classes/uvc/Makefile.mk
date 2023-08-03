UVCD_TARGET=libusb_uvcd
UVCD_PATH=src/device_classes/uvc
obj-y += $(UVCD_TARGET).a
$(UVCD_TARGET)-objs = $(UVCD_PATH)/ux_device_class_uvc_activate.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_commit_control.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_control_request.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_entry.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_initialize.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_payload_send.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_probe_control.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_reset.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_thread.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_status_get.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_status_set.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_deactivate.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_processing_unit.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_input_terminal.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/ux_device_class_uvc_buffer.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/AmbaUSBD_Uvc.o
$(UVCD_TARGET)-objs += $(UVCD_PATH)/AmbaVerUvc.o

$(obj)/AmbaVerUvc.o	:	$(call amba_ver_dep,AmbaVerUvc.o,$(libusb_uvcd-objs))
CFLAGS_AmbaVerUvc.o	=	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
						-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
						-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)		\
						-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)		\
						-DAMBA_CI_COUNT=$(AMBA_CI_COUNT_N2)			\
						-DAMBA_CI_ID=$(AMBA_CI_ID_N2)

install-files += $(UVCD_TARGET).a