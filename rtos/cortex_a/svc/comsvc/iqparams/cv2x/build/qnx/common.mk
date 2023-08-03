ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

EXCLUDE_OBJS =

define POST_INSTALL
    @$(CURDIR)/Iq_bat_qnx_$(AMBA_CHIP_ID).bat
    @rm -f *.o
    @rm -f *.a
    @rm -f *.so
endef

include $(MKFILES_ROOT)/qtargets.mk