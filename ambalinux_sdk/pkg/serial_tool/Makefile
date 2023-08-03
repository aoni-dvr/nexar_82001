export AMBALINK_CV25_DIR=${PWD}/../../output.oem/ambalink

CROSS_COMPILE="${AMBALINK_CV25_DIR}/host/bin/aarch64-none-linux-gnu-"
export CC=${CROSS_COMPILE}gcc

all:
	$(CC) -o serial_tool *.c
	cp -f serial_tool ../../pkg/tools/
