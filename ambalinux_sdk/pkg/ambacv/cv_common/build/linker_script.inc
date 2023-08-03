################################################################################
#
#                    generates linker script file
#
################################################################################
LDS_GEN   = $(CMNDIR)/build/lds_gen.sh
APP_LSR   = $(OBJDIR)/linker.lds

$(APP_LSR): $(LDS_GEN)
	@mkdir -p $(dir $@)
	@$(LDS_GEN) $(LD) $@
