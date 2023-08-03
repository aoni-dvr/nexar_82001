# RNG gcovr makefile
SRCPATH := ../../../../../../
SRCDIR += $(SRCPATH)/soc/io/src/common/
CC = gcc

AMBA_CHIP_ID = cv2fs
OUTPUT	= main_rng

LDFLAGS += -m32 -lm
LDFLAGS += -DCONFIG_SOC_CV2FS

CFLAGS  = -fprofile-arcs -ftest-coverage -fPIC 
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/ 
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/io/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/threadx/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/rtsl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/csl/

SRC 	+= main.c
SRC 	+= rng_ctest_init.c
SRC 	+= rng_ctest_dataget.c
SRC 	+= rng_ctest_deinit.c
SRC 	+= rng_ctest_stub.c
SRC 	+= $(SRCPATH)/soc/io/src/common/io-mw/threadx/AmbaRNG.c
SRC 	+= $(SRCPATH)/soc/io/src/common/rtsl/AmbaRTSL_RNG.c
SRC 	+= $(SRCPATH)/soc/io/src/common/csl/AmbaCSL_RNG.c

#default: all
.PHONY: clean

all: $(OUTPUT) run coverage

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) $(LDFLAGS)

run:
	echo "Running RNG..."
	./main_rng

coverage:
	@echo "Running gcovr..."	
	gcovr $(SRCPATH) -r $(SRCDIR) --html --html-details -o rng-html-details.html;	
	echo "See coverage.html in rng-html-details.html"

clean:
	$(RM) $(OUTPUT)
	$(RM) *.gcda *.gcno *.html *.o *.txt *.css *.log bbb
	

