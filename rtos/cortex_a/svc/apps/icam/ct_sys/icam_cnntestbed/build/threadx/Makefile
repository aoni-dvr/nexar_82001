
CT_SYS        = ../../../ct_sys/icam_cnntestbed
CT_ENTRY      = $(CT_SYS)/src/entry
CT_DATAFLOW   = $(CT_SYS)/src/dataflow
CT_SYS_PREFIX = $(src)/$(CT_SYS)

COMSVC_PREFIX = $(srctree)/svc/comsvc
CT_CVALGO_PREFIX = $(COMSVC_PREFIX)/cv/arm_framework/app

# include path
ccflags-y += -I$(CT_SYS_PREFIX)/inc/entry                          \
             -I$(CT_SYS_PREFIX)/inc/dataflow                       \
             -I$(COMSVC_PREFIX)/cv/cv_common/inc/cvapi             \
             -I$(COMSVC_PREFIX)/cv/cv_common/inc/svccvalgo         \
             -I$(COMSVC_PREFIX)/cv/cv_common/inc/svccvalgo/threadx \
             -I$(COMSVC_PREFIX)/ambalink/link_user                 \
             -I$(COMSVC_PREFIX)/ambalink/core/inc                  \
             -I$(CT_CVALGO_PREFIX)/CtUtils/inc                     \
             -I$(CT_CVALGO_PREFIX)/CtSingleFD/inc                  \
             -I$(CT_CVALGO_PREFIX)/CtSSD/inc                       \
             -I$(CT_CVALGO_PREFIX)/CtDetCls/inc                    \
             -I$(CT_CVALGO_PREFIX)/CtFrcnnProc/inc                 \
             -I$(CT_CVALGO_PREFIX)/CtSingleFDHeader/inc

# include entry objects
libsvc_icam-objs += $(CT_ENTRY)/CtShell_CommandCv.o        \
                    $(CT_ENTRY)/CtShell_CommandCvFlexi.o   \
                    $(CT_ENTRY)/CtShell_CommandCvSdg.o     \
                    $(CT_ENTRY)/CtShell_CommandLv.o        \
                    $(CT_ENTRY)/CtSysInit.o

# include dataflow objects
libsvc_icam-objs += $(CT_DATAFLOW)/CtCvAlgoWrapper.o        \
                    $(CT_DATAFLOW)/FileFeeder.o             \
                    $(CT_DATAFLOW)/FileSink.o               \
                    $(CT_DATAFLOW)/FileSinkHeader.o         \
                    $(CT_DATAFLOW)/IdspFeeder.o             \
                    $(CT_DATAFLOW)/SegDisplay.o             \
                    $(CT_DATAFLOW)/SsdDisplay.o             \
                    $(CT_DATAFLOW)/FrcnnProcDisplay.o       \
                    $(CT_DATAFLOW)/ArmOsdSender.o
