
# Need to disable flags to use headers of toolchain.
NOSTDINC_FLAGS :=

obj-y += libcomsvc_container.a

libcomsvc_container-objs := \
	../../BitOp.o	\
	../../ByteOp.o	\
	../../FormatAPI.o	\
	../../H264.o	\
	../../H265.o\
	../../MemByteOp.o

libcomsvc_container-objs += \
	../../index/IndexCache.o

libcomsvc_container-objs += \
	../../Tiff.o

ifdef CONFIG_BUILD_COMSVC_MUXER
libcomsvc_container-objs += \
	../../Muxer.o
libcomsvc_container-objs += \
	../../iso/IsoMux.o
ifdef CONFIG_BUILD_COMSVC_MX_FMP4
libcomsvc_container-objs += \
	../../iso/Fmp4MuxImpl.o
libcomsvc_container-objs += \
	../../Fmp4Mux.o
endif
ifdef CONFIG_BUILD_COMSVC_MX_EXIF
libcomsvc_container-objs += \
	../../ExifMux.o
endif
endif

ifdef CONFIG_BUILD_COMSVC_DEMUXER
libcomsvc_container-objs += \
	../../demuxer/Demuxer.o	\
	../../demuxer/DemuxerImpl.o	\
	../../demuxer/DemuxerImage.o	\
	../../demuxer/DemuxerMovie.o	\
	../../demuxer/FrameFeeder.o	\
	../../demuxer/FeedingRuleDefault.o	\
	../../demuxer/FeedingRuleH264.o	\
	../../demuxer/FeedingRuleH265.o
libcomsvc_container-objs += \
	../../iso/IsoDmx.o
ifdef CONFIG_BUILD_COMSVC_DX_FMP4
libcomsvc_container-objs += \
	../../iso/Fmp4DmxImpl.o	\
	../../iso/Fmp4Idx.o
libcomsvc_container-objs += \
	../../Fmp4Dmx.o
endif
ifdef CONFIG_BUILD_COMSVC_DX_MP4
libcomsvc_container-objs += \
	../../iso/Mp4DmxImpl.o	\
	../../iso/Mp4Idx.o
libcomsvc_container-objs +=	\
	../../Mp4Dmx.o
endif
ifdef CONFIG_BUILD_COMSVC_DX_EXIF
libcomsvc_container-objs += \
	../../ExifDmx.o
endif

endif

ifdef CONFIG_BUILD_COMSVC_FILE
libcomsvc_container-objs += \
	../../stream/File.o
endif

# Dependency of AmbaVer.o
$(obj)/AmbaVer.o: $(call amba_ver_dep,AmbaVer.o,$(libcomsvc_container-objs))

CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)	\
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)	\
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)		\
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -Ivendors/ambarella/inc	\
			-Ivendors/ambarella/inc/io	\
			-Isvc/comsvc/misc	\
			-Isvc/comsvc/print	\
			-Isvc/comsvc/cfs/inc	\
			-Isvc/comsvc/imgproc/imgaaa/inc/aaa	\
			-Isvc/comsvc/codec/inc	\
			-I$(srctree)/$(src)/../../../fifo/inc	\
			-I$(srctree)/$(src)/../../inc	\
			-I$(srctree)/$(src)/../..
			

install-files := libcomsvc_container.a


