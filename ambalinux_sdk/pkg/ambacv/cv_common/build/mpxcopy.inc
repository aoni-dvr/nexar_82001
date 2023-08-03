################################################################################
#
#                    copy message-pump-xml files
#
################################################################################
MPXSRC = $(wildcard $(LMKDIR)/*.mpx)
MPXDST = $(subst $(LMKDIR),$(BINDIR),$(MPXSRC))

mpxcopy: $(MPXDST)

$(BINDIR)/%.mpx: $(LMKDIR)/%.mpx
	mkdir -p $(BINDIR)
	cp $< $@
