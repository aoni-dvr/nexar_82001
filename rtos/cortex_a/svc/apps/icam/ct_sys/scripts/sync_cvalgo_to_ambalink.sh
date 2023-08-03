#!/bin/bash

### SRC : SDK8 
SRC_PATH=${AMBA_SDK8}
SRC_CCF_PATH=svc/comsvc/cv/arm_framework/app/SvcCvAlgoUtil/src
SRC_CCF_INC_PATH=svc/comsvc/cv/cv_common/inc/svccvalgo
SRC_CVALGO_PATH=svc/comsvc/cv/arm_framework/app
SRC_CVALGO_INC_PATH=svc/comsvc/cv/arm_framework/app
SRC_DATAFLOW_PATH=svc/apps/icam/ct_sys/icam_cnntestbed/src/dataflow
SRC_DATAFLOW_INC_PATH=svc/apps/icam/ct_sys/icam_cnntestbed/inc/dataflow

### DST : AMBALINK
DST_PATH=${AMBALINK_SDK8}/arm_framework/app/cvflow_comm
DST_CCF_PATH=svccvalgo
DST_CCF_INC_PATH=svccvalgo/inc
DST_CVALGO_PATH=algo
DST_CVALGO_INC_PATH=algo/inc
DST_DATAFLOW_PATH=dataflow
DST_DATAFLOW_INC_PATH=dataflow/inc

### DST2 : CVFLOW_COMM in SDK8
DST2_PATH=${AMBA_SDK8}
DST2_DATAFLOW_PATH=svc/apps/icam/ct_sys/cvflow_comm/src
DST2_DATAFLOW_INC_PATH=svc/apps/icam/ct_sys/cvflow_comm/inc


### Sync file list
CCF_C="
CvCommFlexi.c
SvcCvAlgo.c
"
CCF_H="
CvCommFlexi.h
SvcCvAlgo.h
"
CVALGO_C="
CtSingleFD/src/SingleFD.c
CtSSD/src/SSD.c
CtDetCls/src/DetCls.c
CtFrcnnProc/src/FrcnnProc.c
CtSingleFDHeader/src/SingleFDHeader.c
CtUtils/src/ArmSsdFusion.c
CtUtils/src/ArmHdrParser.c
"
CVALGO_H="
CtSingleFD/inc/SingleFD.h
CtSingleFDHeader/inc/SingleFDHeader.h
CtSSD/inc/SSD.h
CtDetCls/inc/DetCls.h
CtFrcnnProc/inc/FrcnnProc.h
CtUtils/inc/ArmSsdFusion.h
CtUtils/inc/ArmHdrParser.h
"
DATAFLOW_C="
CtCvAlgoWrapper.c
FileFeeder.c
FileSink.c
"
DATAFLOW_H="
CtCvAlgoWrapper.h
FileFeeder.h
FileSink.h
"

### 1. Sync files for CCF ###
echo "Sync from ${SRC_PATH} to ${DST_PATH}"
echo "Sync CCF .... Start"

for f in ${CCF_C}; do
	echo "  Copy [${f}]"
	dst_f="$(basename -- $f)"
	cp -rf ${SRC_PATH}/${SRC_CCF_PATH}/${f} ${DST_PATH}/${DST_CCF_PATH}/${dst_f}
done

for f in ${CCF_H}; do
	echo "  Copy [${f}]"
	dst_f="$(basename -- $f)"
	cp -rf ${SRC_PATH}/${SRC_CCF_INC_PATH}/${f} ${DST_PATH}/${DST_CCF_INC_PATH}/${dst_f}
done


### 2. Sync files for CvAlgo ###
echo "Sync cvalgo .... Start"

for f in ${CVALGO_C}; do
	echo "  Copy [${f}]"
	dst_f="$(basename -- $f)"
	cp -rf ${SRC_PATH}/${SRC_CVALGO_PATH}/${f} ${DST_PATH}/${DST_CVALGO_PATH}/${dst_f}
done

for f in ${CVALGO_H}; do
	echo "  Copy [${f}]"
	dst_f="$(basename -- $f)"
	cp -rf ${SRC_PATH}/${SRC_CVALGO_INC_PATH}/${f} ${DST_PATH}/${DST_CVALGO_INC_PATH}/${dst_f}
done

### 3. Sync files for Dataflow ###
echo "Sync dataflow .... Start"

for f in ${DATAFLOW_C}; do
	echo "  Copy [${f}]"
	dst_f="$(basename -- $f)"
	cp -rf ${SRC_PATH}/${SRC_DATAFLOW_PATH}/${f} ${DST_PATH}/${DST_DATAFLOW_PATH}/${dst_f}
	cp -rf ${SRC_PATH}/${SRC_DATAFLOW_PATH}/${f} ${DST2_PATH}/${DST2_DATAFLOW_PATH}/${dst_f}
done

for f in ${DATAFLOW_H}; do
	echo "  Copy [${f}]"
	dst_f="$(basename -- $f)"
	cp -rf ${SRC_PATH}/${SRC_DATAFLOW_INC_PATH}/${f} ${DST_PATH}/${DST_DATAFLOW_INC_PATH}/${dst_f}
	cp -rf ${SRC_PATH}/${SRC_DATAFLOW_INC_PATH}/${f} ${DST2_PATH}/${DST2_DATAFLOW_INC_PATH}/${dst_f}
done

echo "Done ...."

