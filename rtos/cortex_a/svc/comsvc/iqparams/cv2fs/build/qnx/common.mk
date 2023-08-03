ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

EXCLUDE_OBJS =

define POST_INSTALL
    @$(CURDIR)/Iq_bat_qnx.bat
    #@rm -f *.o
endef

include $(MKFILES_ROOT)/qtargets.mk