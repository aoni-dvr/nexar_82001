# IK gcovr makefile
# 2021.05.25 First creation.

IK_FOLDER := ../../../imgkernel/cv2xfs
IKC_FOLDER := ../../../imgkernelcore/cv2xfs
IKS_FOLDER := ../../../imgkernel_similar/cv2xfs
EARLY_TEST_FOLDER := ../../../imgkernelcore/cv2xfs/early_test
SRCDIR := ../../../../imgkernel

#-------------------- include ------------------
include $(IK_FOLDER)/src/efiles.mk
include $(IK_FOLDER)/src/arch/efiles.mk
include $(IK_FOLDER)/src/config/efiles.mk
include $(IK_FOLDER)/src/context/efiles.mk
include $(IK_FOLDER)/src/executer/efiles.mk
include $(IKC_FOLDER)/src/efiles.mk
include $(IKS_FOLDER)/src/efiles.mk
include $(IKS_FOLDER)/src/arch/efiles.mk
include $(IKS_FOLDER)/src/config/efiles.mk
include $(IKS_FOLDER)/src/context/efiles.mk
include $(IKS_FOLDER)/src/executer/efiles.mk
include $(IKS_FOLDER)/src/idspdrv_50/efiles.mk

#-------------------- IK ------------------
CMD_MSG_FOLDER := $(EARLY_TEST_FOLDER)/early_test_inc/cmd_msg
INC += -I./
INC += -I$(IKC_FOLDER)/inc
INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc
INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp
INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp/cv2fs
INC += -I$(IK_FOLDER)/inc
INC += -I$(IK_FOLDER)/inc/common
INC += -I$(IK_FOLDER)/inc/arch
INC += -I$(IK_FOLDER)/inc/context
INC += -I$(IK_FOLDER)/inc/config
INC += -I$(IK_FOLDER)/inc/ikc
INC += -I$(IK_FOLDER)/inc/iks
INC += -I$(IKS_FOLDER)/inc
INC += -I$(IKS_FOLDER)/inc/arch
INC += -I$(IKS_FOLDER)/inc/context
INC += -I$(IKS_FOLDER)/inc/config
INC += -I$(IKS_FOLDER)/inc/idspdrv_50
INC += -I$(IKS_FOLDER)/inc/idspdrv_50/hw

INC_IKS += -I$(IK_FOLDER)/../../../../../vendors/ambarella/inc
INC_IKS += -I$(IK_FOLDER)/../../../../../vendors/ambarella/inc/dsp/cv2fs
INC_IKS += -I$(IK_FOLDER)/inc
INC_IKS += -I$(IK_FOLDER)/src
INC_IKS += -I$(IK_FOLDER)/inc/common
INC_IKS += -I$(IK_FOLDER)/inc/iks
INC_IKS += -I$(IKS_FOLDER)/inc
INC_IKS += -I$(IKS_FOLDER)/inc/arch
INC_IKS += -I$(IKS_FOLDER)/inc/context
INC_IKS += -I$(IKS_FOLDER)/inc/config
INC_IKS += -I$(IKS_FOLDER)/inc/idspdrv_50
INC_IKS += -I$(IKS_FOLDER)/inc/idspdrv_50/hw
INC_IKS += -I$(IKC_FOLDER)/inc
INC_IKS += -I$(IK_FOLDER)/inc/ikc

IK_EI_INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc
IK_EI_INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp
IK_EI_INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp/cv2fs
IK_EI_INC += -I$(IKC_FOLDER)/../../../../../svc/comsvc/print
IK_EI_INC += -I$(CMD_MSG_FOLDER)
IK_EI_INC += -I$(EARLY_TEST_FOLDER)/early_test_inc/vendors
IK_CI_ARCH_INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp
IK_CI_ARCH_INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp/cv2fs
IK_CI_CFG_INC += -I$(CMD_MSG_FOLDER)
IK_CI_EXE_INC += -I$(CMD_MSG_FOLDER)
IK_CI_EXE_INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp
IK_CI_EXE_INC += -I$(IKC_FOLDER)/../../../../../vendors/ambarella/inc/dsp/cv2fs
IK_OBJ := $(IK_EI_OBJ) $(IK_CI_ARCH_OBJ) $(IK_CI_CTX_OBJ) $(IK_CI_CFG_OBJ) $(IK_CI_EXE_OBJ)
IKS_OBJ := $(IDSPDRV_SRC_OBJ) $(IKS_EI_OBJ) $(IKS_CI_ARCH_OBJ) $(IKS_CI_CTX_OBJ) $(IKS_CI_CFG_OBJ) $(IKS_CI_EXE_OBJ)

#-------------------- gcovr APP ------------------
SRC := main.c
SRC += test_group_ik.c test_group_ik_arch.c test_group_ik_config.c test_group_ik_context.c test_group_ik_executer.c test_group_ikc.c ./stub/safety_stub.c
SRC += test_group_hdr20.c test_group_y2y422.c test_group_y2y420_cc.c test_group_calib16.c test_group_ir_only20.c test_group_vid15_sbp.c test_group_vid15_ca.c test_group_vid15_warp.c test_group_vid8_ldly_tile2x4.c test_group_aaa.c test_group_remap.c
SRC_IKS := test_group_iks.c test_group_iks_arch.c test_group_iks_config.c test_group_iks_context.c test_group_iks_executer.c test_group_iks_idspdrv.c
MAIN_OBJ := $(SRC:.c=.o)
MAIN_IKS_OBJ := $(SRC_IKS:.c=.o)
DST_FOLDER := out
DST_BIN := $(DST_FOLDER)/cv2a_ik_gcovr_test

#-------------------- Complier options ------------------
AMBA_BUILD_DATE = \""$(shell date)"\"
AMBA_BUILD_MACHINE = \""$(shell echo `whoami`@`hostname`)"\"

LDFLAGS += -m32 -lm -Wall -g --coverage
LDFLAGS += -DAPP_BUILD_DATE=$(AMBA_BUILD_DATE)
LDFLAGS += -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)
LDFLAGS += -DCONFIG_ENABLE_DSP_MONITOR
#LDFLAGS += -DEARLYTEST_ENV
LDFLAGS += -DSUPPORT_IK_SAFETY
LDFLAGS += -DIK_GCOVR_ENV

#-------------------- Make options ------------------
.PHONY: clean
all: $(DST_BIN) $(MAIN_OBJ) $(MAIN_IKS_OBJ) $(IK_OBJ) $(IKC_OBJ) $(IKS_OBJ) run coverage

$(IKC_OBJ): %.o:%.c
	@echo "build ikc src, $@..."
	@$(CC) $(IKC_INC) -c $< -o $@ $(LDFLAGS)

$(IK_EI_OBJ): %.o:%.c
	@echo "build IK_EI_OBJ, $@..."
	@$(CC) $(IK_EI_INC) -c $< -o $@ $(LDFLAGS)

$(IK_CI_ARCH_OBJ): %.o:%.c
	@echo "build IK_CI_ARCH_OBJ, $@..."
	@$(CC) $(IK_CI_ARCH_INC) -c $< -o $@ $(LDFLAGS)

$(IK_CI_CTX_OBJ): %.o:%.c
	@echo "build IK_CI_CTX_OBJ, $@..."
	@$(CC) $(IK_CI_CTX_INC) -c $< -o $@ $(LDFLAGS)

$(IK_CI_CFG_OBJ): %.o:%.c
	@echo "build IK_CI_CFG_OBJ, $@..."
	@$(CC) $(IK_CI_CFG_INC) -c $< -o $@ $(LDFLAGS)

$(IK_CI_EXE_OBJ): %.o:%.c
	@echo "build IK_CI_EXE_OBJ, $@..."
	@$(CC) $(IK_CI_EXE_INC) -c $< -o $@ $(LDFLAGS)

$(IDSPDRV_SRC_OBJ): %.o:%.c
	@echo "build idspdrv_50 src, $@..."
	@$(CC) $(IDSPDRV_INC) -c $< -o $@ $(LDFLAGS)

$(IKS_EI_OBJ): %.o:%.c
	@echo "build IKS_EI_OBJ, $@..."
	@$(CC) $(IKS_EI_INC) -c $< -o $@ $(LDFLAGS)

$(IKS_CI_ARCH_OBJ): %.o:%.c
	@echo "build IKS_CI_ARCH_OBJ, $@..."
	@$(CC) $(IKS_CI_ARCH_INC) -c $< -o $@ $(LDFLAGS)

$(IKS_CI_CTX_OBJ): %.o:%.c
	@echo "build IKS_CI_CTX_OBJ, $@..."
	@$(CC) $(IKS_CI_CTX_INC) -c $< -o $@ $(LDFLAGS)

$(IKS_CI_CFG_OBJ): %.o:%.c
	@echo "build IKS_CI_CFG_OBJ, $@..."
	@$(CC) $(IKS_CI_CFG_INC) -c $< -o $@ $(LDFLAGS)

$(IKS_CI_EXE_OBJ): %.o:%.c
	@echo "build IKS_CI_EXE_OBJ, $@..."
	@$(CC) $(IKS_CI_EXE_INC) -c $< -o $@ $(LDFLAGS)

$(MAIN_OBJ): %.o:%.c
	@echo "build MAIN_OBJ, $@..."
	@$(CC) $(INC) $(IK_EI_INC) $(IK_CI_EXE_INC) -c -liks_dump_amalgam -L. $< -o $@ $(LDFLAGS)

$(MAIN_IKS_OBJ): %.o:%.c
	@echo "build MAIN_IKS_OBJ, $@..."
	@$(CC) $(INC_IKS) -c -liks_dump_amalgam -L. $< -o $@ $(LDFLAGS)
	
$(DST_BIN): $(MAIN_OBJ) $(MAIN_IKS_OBJ) $(IK_OBJ) $(IKC_OBJ) $(IKS_OBJ)
	@mkdir -p $(DST_FOLDER)
	@mkdir -p $(DST_FOLDER)/data
	@$(CC) $(INC) $(IK_OBJ) $(IKC_OBJ) $(MAIN_OBJ) $(MAIN_IKS_OBJ) $(IKS_OBJ) -o $(DST_BIN) $(LDFLAGS);
	@echo "build gcovr test app done..."

run:
	echo "Running IK..."
	cd out; ./cv2a_ik_gcovr_test

coverage:
	@echo "Running gcovr..."
	#gcovr $(SRCDIR) -r $(SRCDIR) -f ../src/AmbaDSP_Image3aStatistic.c -f ../src/AmbaDSP_ImageFilters.c -f ../src/AmbaDSP_ImageUtility.c --html --html-details -o out/coverage.html;
	gcovr $(SRCDIR) -r $(SRCDIR) --html --html-details -o out/coverage.html;
	echo "See coverage.html in out/coverage.html"

clean:
	@rm -rf $(DST_BIN) $(IK_OBJ) $(MAIN_OBJ) $(MAIN_IKS_OBJ) $(IKC_OBJ) $(IKS_OBJ)
	@rm -rf out/*.*
	@rm -rf out/data/*.*
	@rm -rf *.gcno *.gcda
	@rm -rf ./stub/*.gcno ./stub/*.gcda
	@rm -rf $(IKC_FOLDER)/src/*.gcno $(IKC_FOLDER)/src/*.gcda
	@rm -rf $(IK_FOLDER)/src/*.gcda $(IK_FOLDER)/src/*.gcno
	@rm -rf $(IK_FOLDER)/src/arch/*.gcda $(IK_FOLDER)/src/arch/*.gcno
	@rm -rf $(IK_FOLDER)/src/config/*.gcda $(IK_FOLDER)/src/config/*.gcno
	@rm -rf $(IK_FOLDER)/src/context/*.gcda $(IK_FOLDER)/src/context/*.gcno
	@rm -rf $(IK_FOLDER)/src/executer/*.gcda $(IK_FOLDER)/src/executer/*.gcno
	@rm -rf $(IKS_FOLDER)/src/*.gcda $(IKS_FOLDER)/src/*.gcno
	@rm -rf $(IKS_FOLDER)/src/arch/*.gcda $(IKS_FOLDER)/src/arch/*.gcno
	@rm -rf $(IKS_FOLDER)/src/config/*.gcda $(IKS_FOLDER)/src/config/*.gcno
	@rm -rf $(IKS_FOLDER)/src/context/*.gcda $(IKS_FOLDER)/src/context/*.gcno
	@rm -rf $(IKS_FOLDER)/src/executer/*.gcda $(IKS_FOLDER)/src/executer/*.gcno
	@rm -rf $(IKS_FOLDER)/src/idspdrv_50/*.gcda $(IKS_FOLDER)/src/idspdrv_50/*.gcno

