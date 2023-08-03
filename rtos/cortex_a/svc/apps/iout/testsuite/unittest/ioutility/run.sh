#!/bin/bash

ROOT_DIR=../../../../../../..
REPORT_DIR=report
JSON_FILES=""

make clean
make -j


cd ${REPORT_DIR}

# run main_001.exe
../main_001.exe

# generate json report for main_001.exe
gcovr ../obj -r ${ROOT_DIR}/soc/io/src/common --exclude-unreachable-branches --json -o run_001.json
#gcovr ./obj -r $(ROOT_DIR)/vendors/ambarella/wrapper/usb --html --html-details -o example-html-details.html
JSON_FILES='${JSON_FILES} --add-tracefile run_001.json'

# run main_002.exe
../main_002.exe

# generate json report for main_002.exe
gcovr ../obj -r ${ROOT_DIR}/soc/io/src/common --exclude-unreachable-branches --json -o run_002.json
JSON_FILES='${JSON_FILES} --add-tracefile run_002.json'

# run main_003.exe
../main_003.exe

## generate json report for main_003.exe
gcovr ../obj -r ${ROOT_DIR}/soc/io/src/common --exclude-unreachable-branches --json -o run_003.json
JSON_FILES='${JSON_FILES} --add-tracefile run_003.json'

# run main_004.exe
../main_004.exe

## generate json report for main_004.exe
gcovr ../obj -r ${ROOT_DIR}/soc/io/src/common --exclude-unreachable-branches --json -o run_004.json
JSON_FILES='${JSON_FILES} --add-tracefile run_004.json'

# run main_005.exe
../main_005.exe

## generate json report for main_005.exe
gcovr ../obj -r ${ROOT_DIR}/soc/io/src/common --exclude-unreachable-branches --json -o run_005.json
JSON_FILES='${JSON_FILES} --add-tracefile run_005.json'

# Combine the JSON reports
gcovr -r ${ROOT_DIR}/soc/io/src/common --exclude-unreachable-branches ${JSON_FILES} --html-details coverage.html