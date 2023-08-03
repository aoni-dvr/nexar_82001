AMBARELLA_OVERLAY	?= $(shell pwd)
AMBARELLA_PKG_DIR	?= $(AMBARELLA_OVERLAY)/../pkg
AMBARELLA_OUTPUT_DIR    ?= $(AMBARELLA_OVERLAY)/../output.oem
AMBA_OUT_TARGET 	?= h2_ambalink
TARGET			?= $(AMBA_OUT_TARGET)
AMBARELLA_BOOTLOADER    ?= bootloader/boards/a12evk
MANIFEST2		?= manifest2.txt
MANIFEST3		?= sdk8/manifest_dailybuild.txt
MANIFEST4		?= sdk9/manifest_dailybuild.txt

export AMBARELLA_OVERLAY
export AMBARELLA_PKG_DIR
export AMBARELLA_OUTPUT_DIR

BUILDROOT_TOP		= ../buildroot

all:
	ln -fs ../ambarella $(BUILDROOT_TOP)
	make -C $(BUILDROOT_TOP)

%_defconfig:
	ln -fs ../ambarella $(BUILDROOT_TOP)
	make -C $(BUILDROOT_TOP) $(MAKECMDGOALS)

.PHONY: distribute distribute2 prepare_oem
prepare_oem:
	@( \
		if [ ! -d ../output.oem/$(TARGET)/build/libipc.* ] && \
		   [ -d ../output/$(AMBA_OUT_TARGET)/build ] ; then  \
			mkdir -p ../output.oem/$(TARGET);	\
			cp -a ../output/$(AMBA_OUT_TARGET)/build ../output.oem/$(TARGET); \
		fi; \
	)

#MANIFEST2=manifest2.txt
MANIFEST2_FILE=$(shell pwd)/support/manifests/$(MANIFEST2)
distribute2: $(MANIFEST2_FILE)
	@( \
		 cd .. ; \
		 ./ambarella/support/scripts/distribute $(MANIFEST2_FILE) $(AMBA_OUT_TARGET) \
	)

MANIFEST3_FILE=$(shell pwd)/support/manifests/$(MANIFEST3)
distribute3: $(MANIFEST3_FILE)
	@( \
		 cd .. ; \
		 ./ambarella/support/scripts/distribute_sdk8 $(MANIFEST3_FILE) $(AMBA_OUT_TARGET) \
	)

MANIFEST4_FILE=$(shell pwd)/support/manifests/$(MANIFEST4)
distribute4: $(MANIFEST4_FILE)
	@( \
		 cd .. ; \
		 ./ambarella/support/scripts/distribute_sdk9 $(MANIFEST4_FILE) $(AMBA_OUT_TARGET) \
	)

distribute_output:
	@( \
		 cd .. ; \
		 ./ambarella/support/scripts/distribute_output $(AMBA_OUT_TARGET) \
	)

amboot:
	@( \
		if [ ! -e $(AMBARELLA_BOOTLOADER)/.config ]; then \
		        make -C $(AMBARELLA_BOOTLOADER) hawthorn_amboot_config; \
		fi; \
		make -C $(AMBARELLA_BOOTLOADER); \
	)

amboot-clean:
	make -C $(AMBARELLA_BOOTLOADER) clean

amboot-distclean:
	make -C $(AMBARELLA_BOOTLOADER) distclean


