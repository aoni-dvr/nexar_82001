.PHONY: app

APPS     = $(subst /,,$(wildcard */))

all: app

app:
	@for i in $(APPS); do	\
		echo "Build APP=$$i"; \
		$(MAKE) -C ./$$i app CVCHIP=$(CVCHIP);	\
		if [ $$? -ne 0 ]; then \
		    echo "Build $$i fail"; exit 1; \
		fi ;\
	done
