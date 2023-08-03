ifneq ($(CONFIG_ARM64),)
bit = 64b
ifneq ($(CONFIG_CC_USESTD),)
install_bin = N
else
install_bin = Y
endif # CC_USESTD
else
bit = 32b
ifneq ($(CONFIG_CC_USESTD),)
install_bin = N
else
install_bin = Y
endif # CC_USESTD
endif

bin_path = $(srctree)/$(src)/$(bit)
lib_a_bin := $(foreach libbin,$(wildcard $(bin_path)/*.bin),$(notdir $(libbin)))

ifeq ($(install_bin),Y)
lib_a := $(patsubst %.bin,%,$(lib_a_bin))

obj-y += $(lib_a)

$(obj)/%.a: $(bin_path)/%.a.bin $(objtree)/include/generated/autoconf.h
	$(Q)if [ ! -e $(objtree)/$@ ] || \
	[ "`diff $< $(objtree)/$@ 2> /dev/null`" != "" ]; then \
		cp -fv $< $@; \
	fi

install-files := $(lib_a)
clean-files += $(lib_a)

else # install_bin = N
obj-y += .stamp_clean

$(obj)/.stamp_clean:
	@rm -f ${AMBA_O_LIB}/libc.a
	@rm -f ${AMBA_O_LIB}/libm.a
	@rm -f ${AMBA_O_LIB}/libgccc.a
	@touch $@

clean-files += $(obj)/.stamp_clean
endif # install_bin

