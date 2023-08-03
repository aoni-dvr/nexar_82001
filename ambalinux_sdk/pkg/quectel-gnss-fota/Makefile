export AMBALINK_CV25_DIR=${PWD}/../../output.oem/ambalink

CROSS_COMPILE="${AMBALINK_CV25_DIR}/host/bin/aarch64-none-linux-gnu-"
export CC=${CROSS_COMPILE}gcc
#export CC=gcc

all:
	$(CC) -o gnss-fota *.c ${LDFLAGS} -lrtos_api
